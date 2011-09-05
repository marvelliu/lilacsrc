/* user cache ����
   ��hask table ��������ID��id��,�ӿ�����ٶ�
   2001.5.4 KCN
*/


#include "bbs.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <signal.h>

#include "uhashgen.h"

struct UCACHE {
    ucache_hashtable hashtable;
    ucache_hashtable hashusage;
    int hashhead[UCACHE_HASHSIZE + 1];
    int next[MAXUSERS];
    time_t uptime;
    int number;
    char user_title[255][USER_TITLE_LEN]; //�����û��ĳƺ��ַ�����
    struct userec passwd[MAXUSERS];
};

#ifndef USE_SEM_LOCK
static int ucache_lock()
{
    int lockfd;

    lockfd = open(ULIST, O_RDWR | O_CREAT, 0600);
    if (lockfd < 0) {
        bbslog("3system", "CACHE:lock ucache:%s", strerror(errno));
        return -1;
    }
    flock(lockfd, LOCK_EX);
    return lockfd;
}

static void ucache_unlock(int fd)
{
    flock(fd, LOCK_UN);
    close(fd);
}
#else
static int ucache_lock()
{
	lock_sem(UCACHE_SEMLOCK);
	return 0;
}

static void ucache_unlock(int fd)
{
	unlock_sem_check(UCACHE_SEMLOCK);
}

#endif 
/* do init hashtable , read from uhashgen.dat -- wwj*/
static void ucache_hashinit()
{
    FILE *fp;
    char line[256];
    int i, j, ptr, data;


    fp = fopen("uhashgen.dat", "rt");
    if (!fp) {
        bbslog("3system", "UCACHE:load uhashgen.dat fail");
        exit(0);
    }
    i = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#')
            continue;
        j = 0;
        ptr = 0;
        while ((line[ptr] >= '0' && line[ptr] <= '9') || line[ptr] == '-') {
            data = ptr;
            while ((line[ptr] >= '0' && line[ptr] <= '9') || line[ptr] == '-')
                ptr++;
            line[ptr++] = 0;
            if (i == 0) {
                if (j >= 26) {
                    bbslog("3system", "UCACHE:hash0>26");
                    exit(0);
                }
                uidshm->hashtable.hash0[j++] = atoi(&line[data]);
            } else {
                if (j >= 36) {
                    bbslog("3system", "UCACHE:hash0>26");
                    exit(0);
                }
                uidshm->hashtable.hash[i - 1][j++] = atoi(&line[data]);
            }
        }
        i++;
        if (i > (int) sizeof(uidshm->hashtable.hash) / 36) {
            bbslog("3system", "hashline %d exceed", i);
            exit(0);
        }
    }
    fclose(fp);

    newbbslog(BBSLOG_USIES,"HASH load");
}

/*
   CaseInsensitive ucache_hash, assume
   isalpha(userid[0])
   isahpha(userid[n]) || isnumber(userid[n]) n>0
   01/5/5 wwj
 */
unsigned int ucache_hash_deep(const char *userid)
{
    int n1, n2, n;
    ucache_hashtable *hash;
    ucache_hashtable *usage;

    if (!*userid)
        return 0;
    hash = &uidshm->hashtable;
    usage = &uidshm->hashusage;

    n1 = *userid++;
    if (n1 >= 'a' && n1 <= 'z')
        n1 &= 0xdf;
    n1 -= 'A';
    if (n1 < 0 || n1 >= 26)
        return 0;

    n1 = hash->hash0[n1];

    n = 1;
    while (n1 < 0) {
        n1 = -n1 - 1;
        if (!*userid) {
/* disable it            usage->hash[n1][0]++; */
            n1 = hash->hash[n1][0];
        } else {
            n2 = *userid++;
            if (n2 >= 'a' && n2 <= 'z') {
                n2 -= 'a' - 10;
            } else if (n2 >= 'A' && n2 <= 'Z') {
                n2 -= 'A' - 10;
            } else {
                n2 -= '0';
            }
            if (n2 < 0 || n2 >= 36)
                return 0;
            n1 = hash->hash[n1][n2];
        }
        n++;
    }
    return n;
}


/*
   CaseInsensitive ucache_hash, assume 
   isalpha(userid[0]) 
   isahpha(userid[n]) || isnumber(userid[n]) n>0
   01/5/5 wwj
 */
unsigned int ucache_hash(const char *userid)
{
    int n1, n2, n, len;
    ucache_hashtable *hash;
    ucache_hashtable *usage;

    if (!*userid)
        return 0;
    hash = &uidshm->hashtable;
    usage = &uidshm->hashusage;

    n1 = *userid++;
    if (n1 >= 'a' && n1 <= 'z')
        n1 &= 0xdf;
    n1 -= 'A';
    if (n1 < 0 || n1 >= 26)
        return 0;

/* disable it    usage->hash0[n1]++;*/
    n1 = hash->hash0[n1];

    while (n1 < 0) {
        n1 = -n1 - 1;
        if (!*userid) {
/* disable it            usage->hash[n1][0]++; */
            n1 = hash->hash[n1][0];
        } else {
            n2 = *userid++;
            if (n2 >= 'a' && n2 <= 'z') {
                n2 -= 'a' - 10;
            } else if (n2 >= 'A' && n2 <= 'Z') {
                n2 -= 'A' - 10;
            } else {
                n2 -= '0';
            }
            if (n2 < 0 || n2 >= 36)
                return 0;
/* disable it            usage->hash[n1][n2]++; */
            n1 = hash->hash[n1][n2];
        }
    }
    n1 = (n1 * UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1;
    if (!*userid)
        return n1;

    n2 = 0;
    len = strlen(userid);
    while (*userid) {
        n = *userid++;
        if (n >= 'a' && n <= 'z') {
            n -= 32;
        }
        n2 += (n - 47) * len;
        len--;
    }
    n1 = (n1 + n2 % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1;
    return n1;
}

static int fillucache(struct userec *uentp, int *number, int *prev)
{
    if (*number < MAXUSERS) {
        int hashkey;

        if (id_invalid(uentp->userid))
            hashkey = 0;
        else
            hashkey = ucache_hash(uentp->userid);
        if (hashkey < 0 || hashkey > UCACHE_HASHSIZE) {
            bbslog("3system", "UCACHE:hash(%s) %d error", uentp->userid, hashkey);
            exit(0);
        }
      addempty:
        if (hashkey == 0) {     /* empty user add in recurise sort 
                                   int i=uidshm->hashhead[0];
                                   uidshm->next[*number] = 0;
                                   if (i==0) uidshm->hashhead[0]=++(*number);
                                   else {
                                   int prev;
                                   while (i) {
                                   prev=i;
                                   i=uidshm->next[i-1];
                                   };
                                   uidshm->next[prev-1]=++(*number);
                                   } */
            uidshm->next[*number] = 0;
            (*number)++;
            if (!(*prev)) {
                uidshm->hashhead[0] = *number;
            } else {
                uidshm->next[(*prev) - 1] = *number;
            }
            *prev = *number;
        } else {
/* check multi-entry of user */
            int i, prev;

            i = uidshm->hashhead[hashkey];
            prev = -1;
            while (i != 0) {
                struct userec *uentp;

                uentp = &uidshm->passwd[i - 1];
                if (!strcasecmp(uidshm->passwd[*number].userid, uentp->userid)) {
                    if (uidshm->passwd[*number].numlogins > uentp->numlogins) {
                        bbslog("3passwd", "deleted %s in %d", uentp->userid, i - 1);
                        if (prev == -1)
                            uidshm->hashhead[hashkey] = uidshm->next[i - 1];
                        else
                            uidshm->next[prev - 1] = uidshm->next[i - 1];
                        uentp->userid[0] = 0;
                        uidshm->next[i - 1] = 0;
                        uidshm->hashhead[0] = i;
                    } else {
                        bbslog("3passwd", "deleted %s in %d", uidshm->passwd[*number].userid, *number);
                        uidshm->passwd[*number].userid[0] = 0;
                        hashkey = 0;
                        goto addempty;
                    }
                }
                prev = i;
                i = uidshm->next[i - 1];
            }
            uidshm->next[*number] = uidshm->hashhead[hashkey];
            uidshm->hashhead[hashkey] = ++(*number);
        }
    }
    return 0;
}

static void flush_user_title();

int flush_ucache()
{
    int ret;
    ret= substitute_record(PASSFILE, uidshm->passwd, MAXUSERS * sizeof(struct userec), 1);
    flush_user_title();
    return ret;
}

int load_ucache()
{
    int iscreate;
    int usernumber, i;
    int passwdfd;
    int prev;

    int fd;

    fd=ucache_lock();
    uidshm = (struct UCACHE *) attach_shm("UCACHE_SHMKEY", 3696, sizeof(*uidshm), &iscreate);   /*attach to user shm */

    if (!iscreate) {
        bbslog("4system", "load a exitist ucache shm!");
    } else {

        load_user_title();
        if ((passwdfd = open(PASSFILE, O_RDWR | O_CREAT, 0644)) == -1) {
            bbslog("3system", "Can't open " PASSFILE "file %s", strerror(errno));
            ucache_unlock(fd);
            exit(-1);
        }
        ftruncate(passwdfd, MAXUSERS * sizeof(struct userec));
    	close(passwdfd);
        if (get_records(PASSFILE, uidshm->passwd, sizeof(struct userec), 1, MAXUSERS) != MAXUSERS) {
            bbslog("4system", "error PASS file!");
            ucache_unlock(fd);
            exit(-1);
        }
        bzero(uidshm->hashhead, UCACHE_HASHSIZE * sizeof(int));
        usernumber = 0;

        ucache_hashinit();

        prev = 0;
        for (i = 0; i < MAXUSERS; i++)
            fillucache(&uidshm->passwd[i], &usernumber, &prev);

        newbbslog(BBSLOG_USIES, "CACHE:reload ucache for %d users", usernumber);
        uidshm->number = usernumber;
    }
    ucache_unlock(fd);
    return 0;
}

int resolve_ucache()
{
    int iscreate;

    iscreate = 0;
    if (uidshm == NULL) {
        uidshm = (struct UCACHE *) attach_shm("UCACHE_SHMKEY", 3696, sizeof(*uidshm), &iscreate);
        if (iscreate) {         /* shouldn't load passwd file in this place */
            bbslog("4system", "passwd daemon havn't startup");
            remove_shm("UCACHE_SHMKEY",3696,sizeof(*uidshm));
            return -1;
        }
        
    }
    return 0;
}

void detach_ucache()
{
    shmdt((void *)uidshm);
    uidshm=NULL;
}

/*---	period	2000-10-20	---*/
int getuserid(char *userid, int uid)
{
    if (uid > uidshm->number || uid <= 0)
        return 0;
    strncpy(userid, (char *) uidshm->passwd[uid - 1].userid, IDLEN + 1);
    userid[IDLEN] = 0;
    return uid;
}

static int setuserid_internal(int num, const char *userid)
{                               /* ����user num��idΪuser id */
    if (num > 0 && num <= MAXUSERS) {
        int oldkey, newkey, find;

        oldkey = ucache_hash((char *) uidshm->passwd[num - 1].userid);
        newkey = ucache_hash(userid);
        find = uidshm->hashhead[newkey];
	if (newkey!=oldkey) { //���������� faint
        while ((newkey!=0)&&find) { //check duplicate
            if (!strcasecmp(uidshm->passwd[find-1].userid,userid))
                return -1;
            find = uidshm->next[find-1];
        }
        }
        if (num > uidshm->number)
            uidshm->number = num;
/*        if (oldkey!=newkey) { disable,Ϊ�˼�ǿ������*/
        find = uidshm->hashhead[oldkey];

        if (find == num)
            uidshm->hashhead[oldkey] = uidshm->next[find - 1];
        else {                  /* find and remove the hash node */
            int i = 0;

            while (uidshm->next[find - 1] && uidshm->next[find - 1] != num) {
                find = uidshm->next[find - 1];
                i++;
                if (i > MAXUSERS) {
                    bbslog("3system", "UCACHE:uhash loop! find %d,%s", num, userid);
                    exit(0);
                }
            }
            if (!uidshm->next[find - 1]) {
                if (oldkey != 0) {
                    bbslog("3system", "UCACHE:can't find %s in hash table", uidshm->passwd[num - 1].userid);
/*		          	exit(0);*/
                }
                return -1;
            } else
                uidshm->next[find - 1] = uidshm->next[num - 1];
        }

        uidshm->next[num - 1] = uidshm->hashhead[newkey];
        uidshm->hashhead[newkey] = num;
/*        }	        */
        strncpy(uidshm->passwd[num - 1].userid, userid, IDLEN + 1);
    }
    return 0;
}

int setuserid2(int num, const char *userid)
{
    int lockfd, ret;

    lockfd = ucache_lock();
    ret = setuserid_internal(num, userid);
    ucache_unlock(lockfd);
    return ret;
}

void setuserid(int num, const char *userid)
{

    int m_socket;
    char cmdbuf[255];
#ifdef HAVE_IPV6_SMTH
    struct sockaddr_in6 sin;
#else
    struct sockaddr_in sin;
#endif
    fd_set rfds;
    int result;
    struct timeval tv;

#ifdef HAVE_IPV6_SMTH
    m_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
#else
    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    if (m_socket < 0)
        return;
#ifdef HAVE_IPV6_SMTH
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons(USERD_PORT);
    inet_pton(AF_INET6, "::1", &sin.sin6_addr);
#else /* IPV6 */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(USERD_PORT);
#ifdef HAVE_INET_ATON
    inet_aton("127.0.0.1", &sin.sin_addr);
#elif defined HAVE_INET_PTON
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
#else
	/* Is it OK? */
    my_inet_aton("127.0.0.1", &sin.sin_addr);
#endif
#endif /* IPV6 */
    if (connect(m_socket, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
        close(m_socket);
        return;
    }
    if (userid[0] == 0)
        sprintf(cmdbuf, "DEL %s %d", userid, num);
    else
        sprintf(cmdbuf, "SET %s %d", userid, num);
    write(m_socket, cmdbuf, strlen(cmdbuf));
    FD_ZERO(&rfds);
    FD_SET(m_socket, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    result = select(m_socket + 1, &rfds, NULL, NULL, &tv);
    if (result) {
        int len = read(m_socket, &result, sizeof(result));

        close(m_socket);
        if (len != sizeof(result))
            return;
        return;
    }
    close(m_socket);
    return;
}

static int searchnewuser()
{                               /* ��cache�� ���е� user num */
    if (uidshm->hashhead[0])
        return uidshm->hashhead[0];
    if (uidshm->number < MAXUSERS)
        return uidshm->number + 1;
    return 0;
}
int searchuser(const char *userid)
{
    register int i;

    if ((i = ucache_hash(userid)) == 0) return 0;
    i = uidshm->hashhead[i];
    while (i)
        if (!strcasecmp(userid, uidshm->passwd[i - 1].userid))
            return i;
        else
            i = uidshm->next[i - 1];
    return 0;
}

int getuser(const char *userid,struct userec **user){
    int uid=searchuser(userid);
    if(user)
        *user=(!uid?NULL:&uidshm->passwd[uid-1]);
    return uid;
}

int modify_user_score( const char *userid, int score, char *reason )
{
	struct userec *lookupuser;
	char fname[STRLEN];
	FILE* update_fd = fopen("etc/posts/update_score","w+");
	int tuid = getuser( userid, &lookupuser );
	if ( score == 0 )return 1;
	
	lookupuser->score += score;
	if ( score > 0 )
	{
		sprintf( fname, "[����] ���� %s ���� %d ��", userid, score );
	}
	else
	{
		sprintf( fname, "[����] ���� %s ���� %d ��", userid, score );
	}
	fprintf( update_fd, "˵��:\n" );
	if ( reason != NULL )
		fprintf( update_fd, "        %s\n", reason);
	fclose( update_fd );
	post_file(NULL, "", "etc/posts/update_score", "ScoreRecords", fname, 0, 1, getSession());
	return 1;
}

char *getuserid2(int uid)
{
    if (uid > uidshm->number || uid <= 0)
        return NULL;
    return uidshm->passwd[uid - 1].userid;
}

char *u_namearray(char buf[][IDLEN + 1], int *pnum, char *tag)
/* ����tag ,���� ƥ���user id �б� (�������ע���û�)*/
{
    register struct UCACHE *reg_ushm = uidshm;
    register int n, num, i;
    int hash, len, ksz;
    char tagv[IDLEN + 1];

    *pnum = 0;

    len = strlen(tag);
    if (len > IDLEN)
        return NULL;
    if (!len) {
        return NULL;
    }
    ksz = ucache_hash_deep(tag);

    strcpy(tagv, tag);


    if (len >= ksz) {
        tagv[ksz] = 0;
        hash = ucache_hash(tagv) - 1;
        for (n = 0; n < UCACHE_HASHBSIZE; n++) {
            num = reg_ushm->hashhead[(hash + n % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1];
            while (num) {
                if (!strncasecmp(uidshm->passwd[num - 1].userid, tag, len)) {
                    strcpy(buf[(*pnum)++], uidshm->passwd[num - 1].userid);     /*���ƥ��, add into buf */
                }
                num = reg_ushm->next[num - 1];
            }
        }
    } else {
        for (i = len; i < ksz; i++)
            tagv[i] = '0';
        tagv[ksz] = 0;

        while (1) {
            hash = ucache_hash(tagv) - 1;

            for (n = 0; n < UCACHE_HASHBSIZE; n++) {
                num = reg_ushm->hashhead[(hash + n % UCACHE_HASHBSIZE) % UCACHE_HASHSIZE + 1];  /* see hash() */
                while (num) {
                    if (!strncasecmp(uidshm->passwd[num - 1].userid, tagv, ksz)) {
                        strcpy(buf[(*pnum)++], uidshm->passwd[num - 1].userid); /*���ƥ��, add into buf */
                    }
                    num = reg_ushm->next[num - 1];
                }
            }

            i = ksz - 1;
            while (i >= len) {
                if (tagv[i] == 'Z') {
                    tagv[i] = '0';
                    i--;
                } else if (tagv[i] == '9') {
                    tagv[i] = 'A';
                    break;
                } else {
                    tagv[i]++;
                    break;
                }
            }
            if (i < len)
                break;
        }

    }
    return buf[0];
}

/* disable by KCN 
int
set_safe_record()
{
    struct userec tmp;
    extern int ERROR_READ_SYSTEM_FILE;

    if(get_record(PASSFILE,&tmp,sizeof(struct userec),usernum)==-1)
    {
        char buf[STRLEN];

        sprintf(buf,"Error:Read Passfile %4d %12.12s",usernum,session->getCurrentUser()->userid);
        bbslog("user","%s",buf);
        ERROR_READ_SYSTEM_FILE=true;
        abort_bbs(0);
        return -1;
    }
    session->getCurrentUser()->numposts=tmp.numposts;
    session->getCurrentUser()->numlogins=tmp.numlogins;
    session->getCurrentUser()->stay=tmp.stay;
    session->getCurrentUser()->userlevel=tmp.userlevel;
}
*/

int getnewuserid3(char *userid)
{

    int m_socket;
    char cmdbuf[255];
#ifdef HAVE_IPV6_SMTH
    struct sockaddr_in6 sin;
#else
    struct sockaddr_in sin;
#endif
    fd_set rfds;
    int result;
    struct timeval tv;

#ifdef HAVE_IPV6_SMTH
    m_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
#else
    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    if (m_socket < 0)
        return -1;
#ifdef HAVE_IPV6_SMTH
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons(USERD_PORT);
    inet_pton(AF_INET6, "::1", &sin.sin6_addr);
#else /* IPV6 */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(USERD_PORT);
#ifdef HAVE_INET_ATON
    inet_aton("127.0.0.1", &sin.sin_addr);
#elif defined HAVE_INET_PTON
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
#else
	/* Is it OK? */
    my_inet_aton("127.0.0.1", &sin.sin_addr);
#endif
#endif /* IPV6 */
    if (connect(m_socket, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
        close(m_socket);
        return -1;
    }
    sprintf(cmdbuf, "NEW %s", userid);
    write(m_socket, cmdbuf, strlen(cmdbuf));
    FD_ZERO(&rfds);
    FD_SET(m_socket, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    result = select(m_socket + 1, &rfds, NULL, NULL, &tv);
    if (result) {
        int len = read(m_socket, &result, sizeof(result));

        close(m_socket);
        if (len != sizeof(result))
            return -1;
        return result;
    }
    close(m_socket);
    return -1;
}

int getnewuserid2(char *userid)
{
    int result = getnewuserid3(userid);

    if (result >= 0)
        return result;
    return -1;
}

struct userec *getuserbynum(int num)
{
    if (num <= 0 || num >= MAXUSERS)
        return NULL;
    return &uidshm->passwd[num - 1];
}

int getnewuserid(char *userid)
{
    struct userec utmp;
    int fd, i;
    time_t system_time;
    int ret;

    system_time = time(NULL);
/*
    if( (fd = open( PASSFILE, O_RDWR|O_CREAT, 0600 )) == -1 )
        return -1;
    flock( fd, LOCK_EX );
*/
    fd = ucache_lock();

    while (1) {

        i = searchnewuser();
        if (i <= 0 || i > MAXUSERS) {
            ret=-1;
            break;
        }
        memset(&utmp, 0, sizeof(utmp));
        strcpy(utmp.userid, userid);
        utmp.lastlogin = time(NULL);
        ret = setuserid_internal(i, userid);    /* added by dong, 1998.12.2 */
        if (ret <= 0) {
            break;
        }
    }
    if (ret==0)
        update_user(&utmp, i, 0);
    else
        i=ret;
    ucache_unlock(fd);
    return i;
}

int update_user(struct userec *user, int num, int all)
{
    struct userec tmpuser;

    if (!all) {
        if (strncasecmp(user->userid, uidshm->passwd[num - 1].userid, IDLEN))
            return -1;
        tmpuser = *user;
#ifdef CONV_PASS
        memcpy(tmpuser.passwd, uidshm->passwd[num - 1].passwd, sizeof(tmpuser.passwd));
#endif
        memcpy(tmpuser.md5passwd, uidshm->passwd[num - 1].md5passwd, sizeof(tmpuser.md5passwd));
    } else {
        tmpuser = *user;
        memcpy(tmpuser.userid, uidshm->passwd[num - 1].userid, IDLEN + 2);
    }
    memcpy(&uidshm->passwd[num - 1], &tmpuser, sizeof(struct userec));
    return 0;
}

int apply_users(int(*func)(struct userec*,void*),void *arg){
    register int i,count;
    for(count=0,i=0;i<uidshm->number;i++){
        if(func){
            switch((*func)(&uidshm->passwd[i],arg)){
                case QUIT:
                    return count;
                case COUNT:
                    count++;
                default:
                    break;
            }
        }
        else{
            count++;
        }
    }
    return count;
}

int apply_uids(int(*func)(struct userec*,int,void*),void *arg){
    register int i,count;
    for(count=0,i=0;i<uidshm->number;i++){
        if(func){
            switch((*func)(&uidshm->passwd[i],(i+1),arg)){
                case QUIT:
                    return count;
                case COUNT:
                    count++;
                default:
                    break;
            }
        }
        else{
            count++;
        }
    }
    return count;
}

/* etnlegend, 2005.11.26, ������ʱ�޷��֧�ֲ�����������һЩ���� */
int get_giveupinfo(struct userec *user,int s[GIVEUPINFO_PERM_COUNT]){
    static const unsigned int GIVEUP_PERM[GIVEUPINFO_PERM_COUNT]={
        PERM_BASIC,PERM_POST,PERM_CHAT,PERM_PAGE,PERM_DENYMAIL,PERM_DENYRELAX};
    static const unsigned int INV_MASK=(PERM_DENYMAIL|PERM_DENYRELAX);
    FILE *fp;
    char buf[256],deny[GIVEUPINFO_PERM_COUNT];
    int i,j,k,perm;
    bzero(s,GIVEUPINFO_PERM_COUNT*sizeof(int));
    bzero(deny,GIVEUPINFO_PERM_COUNT*sizeof(char));
    perm=0;
    sethomefile(buf,user->userid,"giveup");
    if(!(fp=fopen(buf,"r")))
        return 0;
    while(fgets(buf,256,fp)){
        switch(sscanf(buf,"%d %d %d",&i,&j,&k)){
            case 2:
                k=0;
            case 3:
                if(i>0&&i<GIVEUPINFO_PERM_COUNT+1)
                    break;
            default:
                continue;
        }
        deny[i-1]=k;
        s[i-1]=(!s[i-1]?j:((j<s[i-1])?j:s[i-1]));
        if((user->userlevel^INV_MASK)&GIVEUP_PERM[i-1])
            s[i-1]=0;
        else
            perm|=GIVEUP_PERM[i-1];
    }
    fclose(fp);
    for(i=0;i<GIVEUPINFO_PERM_COUNT;i++)
        if(deny[i])
            s[i]=(-s[i]);
    return perm;
}
int save_giveupinfo(struct userec *user,int s[GIVEUPINFO_PERM_COUNT]){
    static const unsigned int GIVEUP_PERM[GIVEUPINFO_PERM_COUNT]={
        PERM_BASIC,PERM_POST,PERM_CHAT,PERM_PAGE,PERM_DENYMAIL,PERM_DENYRELAX};
    static const unsigned int GIVEUP_MASK=(PERM_BASIC|PERM_POST|PERM_CHAT|PERM_PAGE|PERM_DENYMAIL|PERM_DENYRELAX);
    static const unsigned int INV_MASK=(PERM_DENYMAIL|PERM_DENYRELAX);
    FILE *fp;
    char buf[256],deny[GIVEUPINFO_PERM_COUNT];
    int i,g_perm,u_perm;
    bzero(deny,GIVEUPINFO_PERM_COUNT*sizeof(char));
    g_perm=0;
    sethomefile(buf,user->userid,"giveup");
    for(i=0;i<GIVEUPINFO_PERM_COUNT;i++){
        if(!s[i])
            continue;
        if(s[i]<0){
            deny[i]=1;
            s[i]=(-s[i]);
        }
        if((user->userlevel^INV_MASK)&GIVEUP_PERM[i])
            s[i]=0;
        else if(!deny[i])
            g_perm|=GIVEUP_PERM[i];
    }
    u_perm=((user->userlevel^INV_MASK)&GIVEUP_MASK);
    if(g_perm&&((u_perm|g_perm)==GIVEUP_MASK))
        user->flags|=GIVEUP_FLAG;
    else
        user->flags&=~GIVEUP_FLAG;
    if(!(fp=fopen(buf,"w")))
        return -1;
    for(i=0;i<GIVEUPINFO_PERM_COUNT;i++)
        if(s[i])
            fprintf(fp,"%d %d %d\n",i+1,s[i],deny[i]);
    fclose(fp);
    return 0;
}

#ifdef DENYANONY
/* stiger,���ӷ��ĳ�˵ķ���Ȩ��1�� */
int giveup_addpost(char *userid){
    struct userec *user;
    int s[GIVEUPINFO_PERM_COUNT];
    if(!(getuser(userid,&user)))
        return 0;
    get_giveupinfo(user,s);
    s[1]=-(1+(!s[1]?(time(NULL)/86400):((s[1]<0)?(-s[1]):s[1])));
    user->userlevel&=~PERM_POST;
    return (save_giveupinfo(user,s)+1);
}
#endif

void setcachehomefile(char* path,const char* user,int unum,char* file)
{
    if (unum==-1) {
    if (file==NULL)
      sprintf(path, "%s/home/%c/%s",TMPFSROOT,toupper(user[0]),user);
    else
      sprintf(path, "%s/home/%c/%s/%s",TMPFSROOT, toupper(user[0]), user,file);
    } else {
    if (file==NULL)
      sprintf(path, "%s/home/%c/%s/%d",TMPFSROOT,toupper(user[0]),user,unum);
    else
      sprintf(path, "%s/home/%c/%s/%d/%s",TMPFSROOT, toupper(user[0]), user,unum,file);
    }
}

void init_cachedata(const char* userid,int unum)
{
    char path1[MAXPATH],path2[MAXPATH];
    int fd,logincount;
    int count;
    struct flock ldata;
    setcachehomefile(path1, userid, -1, NULL);
    mkdir(path1,0700);
    setcachehomefile(path1, userid, unum, NULL);
    if (mkdir(path1,0700)==-1)
        bbslog("3error","mkdir %s errorno %d",path1,errno);
    
    setcachehomefile(path1, userid, -1, "logincount");
    if ((fd = open(path1, O_RDWR, 0664)) != -1) {
        ldata.l_type = F_RDLCK;
        ldata.l_whence = 0;
        ldata.l_len = 0;
        ldata.l_start = 0;
        if (fcntl(fd, F_SETLKW, &ldata) == -1) {
            bbslog("3error", "%s", "logincount err");
            close(fd);
            return;              /* lock error*/
        }
        count=read(fd,path2,MAXPATH);
        path2[count]=0;
        logincount=atoi(path2);
    } else {
        if ((fd = open(path1, O_WRONLY|O_CREAT, 0664)) != -1) {
            logincount=0;
        } else {
            bbslog("3error", "%s", "write logincount err");
            return;              /* lock error*/
        }
    }
    logincount++;
    lseek(fd,0,SEEK_SET);
    sprintf(path2,"%d",logincount);
    write(fd,path2,strlen(path2));
    close(fd);
}
void flush_cachedata(const char* userid)
{
}

int clean_cachedata(const char* userid,int unum)
{
    char path1[MAXPATH],path2[MAXPATH];
    int fd,logincount;
    int count;
    struct flock ldata;

    if ((userid[0]==0)||(userid==NULL)) {
        bbslog("3error","error in clean cache");
        return 0;
    }
    setcachehomefile(path1, userid, unum, NULL);
    f_rm(path1);
    //todo: check the dir
    setcachehomefile(path1, userid, -1, "logincount");
    if ((fd = open(path1, O_RDWR, 0664)) != -1) {
    ldata.l_type = F_RDLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        bbslog("3error", "%s", "logincount err");
        close(fd);
        return -1;              /* lock error*/
    }
    count=read(fd,path2,MAXPATH);
    path2[count]=0;
    logincount=atoi(path2);
    logincount--;
    lseek(fd,0,SEEK_SET);
    sprintf(path2,"%d",logincount);
    write(fd,path2,strlen(path2));
    close(fd);
    } else logincount=0;
    if (logincount==0) {
        setcachehomefile(path1, userid, -1, "entry");
        unlink(path1);
        setcachehomefile(path1, userid, -1, NULL);
        f_rm(path1);
    }
    return 0;
}


int do_after_login(struct userec* user,int unum,int mode)
{
  if (mode==0)
    init_cachedata(user->userid,unum);
  else //www guest,ʹ�ø�������telnet guest����
    init_cachedata(user->userid,-unum);
  return 0;
}

int do_after_logout(struct userec* user,struct user_info* userinfo,int unum,int mode,int locked)
{
    char buf[MAXPATH];
    if (locked) { //utmp locked
        if (userinfo&&userinfo->currentboard)
            board_setcurrentuser(userinfo->currentboard,-1);
        return 0;
    }else { //do something without utmp lock
        int lockfd;
        lockfd=lock_user(userinfo->userid);
        if(userinfo->pid > 1 && strcmp(userinfo->userid,"guest")){
              snprintf(buf,MAXPATH,"tmp/%d/%s/", userinfo->pid, userinfo->userid);
              f_rm(buf);
        }
        if (userinfo&&(mode==0)) {
        }
        if (user) {
            if (mode==0)
                clean_cachedata(user->userid,unum);
            else //www guest,ʹ�ø�������telnet guest����
                clean_cachedata(user->userid,-unum);
        }
        unlock_user(lockfd);
    }
	return 0;
}

/**
 * user_title������1 base,����idx��Ҫ��һ
 * ��title==0��ʱ��Ӧ����ԭ������ʾ��ϵ�ṹ
 */

/**
  * �����ļ��б����user title
  */
void load_user_title()
{
    FILE* titlefile;
    bzero(uidshm->user_title,sizeof(uidshm->user_title));
    if ((titlefile = fopen(USER_TITLE_FILE, "r")) == NULL) {
        bbslog("3system", "Can't open " USER_TITLE_FILE "file %s", strerror(errno));
    } else {
        int i;
        for (i=0;i<255;i++) {
            fgets(uidshm->user_title[i],USER_TITLE_LEN,titlefile);
            
            if ((uidshm->user_title[i][0]!=0)&&(uidshm->user_title[i][strlen(uidshm->user_title[i])-1]=='\n'))
                uidshm->user_title[i][strlen(uidshm->user_title[i])-1]=0;
        }
        fclose(titlefile);
    }
}

/**
  *  ��user_title����д�����
  */
static void flush_user_title()
{
    FILE* titlefile;
    if ((titlefile = fopen(USER_TITLE_FILE, "w")) == NULL) {
        bbslog("3system", "Can't open " USER_TITLE_FILE "file %s", strerror(errno));
    } else {
        int i;
        for (i=0;i<255;i++) {
                fprintf(titlefile,"%s\n",uidshm->user_title[i]);
        }
        fclose(titlefile);
    }
}

/**
 * ���title��Ӧ���ַ���
 * @param titleidx 1base��title
 * @return ������ʾ��title
 */
char* get_user_title(unsigned char titleidx)
{
    if (titleidx==0) return "";
    return uidshm->user_title[titleidx-1];
}

/**
 * ����title��Ӧ���ַ���
 * @param titleidx 1base��title
 * @param newtitle ��Ҫ���õ�title
 */
void set_user_title(unsigned char titleidx,char* newtitle)
{
    int fd;
    fd=ucache_lock();
    if (titleidx!=0) {
        uidshm->user_title[titleidx-1][USER_TITLE_LEN-1]=0;
        strncpy(uidshm->user_title[titleidx-1],newtitle,USER_TITLE_LEN-1);
        flush_user_title();
    }
    ucache_unlock(fd);
}

/* WWW GUEST�������и�ͬ�����⣬���ǵ������һ��
 * GUEST��ʱ������������guestˢ���ˣ���ô����д���ݽṹ
 * ���ԣ�Ҫע�����key֮����������Ҫ�䶯�����뱣֤
 * ����������²����߼����ҡ�freshtime��Ϊ��ʹ�������
 * client�����ˣ�������뼶������Ҳ��������û����⣬����
 * ����������������Ҫ����key����ô���п��ܵ�����һ��
 * guest�û���key������ĸ��ǡ�����Ǹ�����
 * ��: www guest��ʹ�õ�idx��Ȼ��0 base�ġ�
 */
struct WWW_GUEST_TABLE *wwwguest_shm = NULL;

static void longlock(int signo)
{
    bbslog("5system", "www_guest lock for so long time!!!.");
    exit(-1);
}

int www_guest_lock()
{
    int fd = 0;

    fd = open("www_guest.lock", O_RDWR | O_CREAT, 0600);
    if (fd < 0) {
        return -1;
    }
    signal(SIGALRM, longlock);
    alarm(10);
    if (flock(fd, LOCK_EX) == -1) {
        return -1;
    }
    signal(SIGALRM, SIG_IGN);
    return fd;
}

void www_guest_unlock(int fd)
{
    flock(fd, LOCK_UN);
    close(fd);
}


int resolve_guest_table()
{
    int iscreate = 0;

    if (wwwguest_shm == NULL) {
        wwwguest_shm = (struct WWW_GUEST_TABLE *)
            attach_shm("WWWGUEST_SHMKEY", 4500, sizeof(*wwwguest_shm), &iscreate);      /*attach user tmp cache */
		if (wwwguest_shm==NULL)
			bbslog("3error","can't load guest shm:%d",errno);
        if (iscreate) {
            struct public_data *pub;
            int fd = www_guest_lock();
            setpublicshmreadonly(0);
	    pub=get_publicshm();
            if (fd == -1)
                return -1;
            bzero(wwwguest_shm, sizeof(*wwwguest_shm));
            wwwguest_shm->uptime = time(0);
            www_guest_unlock(fd);
            pub->www_guest_count = 0;
            setpublicshmreadonly(1);
        }
    }
    return 0;
}



int lock_user(char* userid)
{
	/*
    int fd = 0;
    char buf[MAXPATH];

    sethomefile(buf,userid,"lock");
    fd = open(buf, O_RDWR | O_CREAT, 0600);
    if (fd < 0) {
        return -1;
    }
    if (flock(fd, LOCK_EX) == -1) {
        return -1;
    }
    return fd;
	*/
	return 0;
}

void unlock_user(int fd)
{
	/*
    flock(fd, LOCK_UN);
    close(fd);
	*/
	return ;
}
