#include "bbs.h"
#include <utime.h>

int outgo_post(struct fileheader *fh, const char *board, const char *title, session_t* session)
{
    FILE *foo;

    if ((foo = fopen("innd/out.bntp", "a")) != NULL) {
        fprintf(foo, "%s\t%s\t%s\t%s\t%s\n", board, fh->filename, session->currentuser->userid, session->currentuser->username, title);
        fclose(foo);
        return 0;
    }
    return -1;
}

extern char alphabet[];

int get_postfilename(char *filename, char *direct, int use_subdir)
{
    static const char post_sufix[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int fp;
    time_t now;
    int i;
    char fname[255];
    int pid = getpid();
    int rn;
    int len;

    /*
     * �Զ����� POST �ļ��� 
     */
    now = time(NULL);
    len = strlen(alphabet);
    for (i = 0; i < 10; i++) {
        if (use_subdir) {
            rn = 0 + (int) (len * 1.0 * rand() / (RAND_MAX + 1.0));
            sprintf(filename, "%c/M.%lu.%c%c", alphabet[rn], now, post_sufix[(pid + i) % 62], post_sufix[(pid * i) % 62]);
        } else
//            sprintf(filename, "M.%lu.%c%c", now, post_sufix[(pid + i) % 62], post_sufix[(pid * i) % 62]);
            sprintf(filename,"M.%lu.%c%c",now,post_sufix[rand()%62],post_sufix[rand()%62]);
        sprintf(fname, "%s/%s", direct, filename);
        if ((fp = open(fname, O_CREAT | O_EXCL | O_WRONLY, 0644)) != -1) {
            break;
        };
    }
    if (fp == -1)
        return -1;
    close(fp);
    return 0;
}

int isowner(const struct userec *user, const struct fileheader *fileinfo)
{
    time_t posttime;

    if (strcmp(fileinfo->owner, user->userid))
        return 0;
    posttime = get_posttime(fileinfo);
    if (posttime < user->firstlogin)
        return 0;
    return 1;
}

int cmpname(fhdr, name)         /* Haohmaru.99.3.30.�Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */
struct fileheader *fhdr;
char name[STRLEN];
{
    if (!strncmp(fhdr->filename, name, FILENAME_LEN))
        return 1;
    return 0;
}

/*
  * �жϵ�ǰģʽ�Ƿ����ʹ��id����
  */
bool is_sorted_mode(int mode)
{
    switch (mode) {
    case DIR_MODE_NORMAL:
    case DIR_MODE_THREAD:
    case DIR_MODE_MARK:
    case DIR_MODE_ORIGIN:
    case DIR_MODE_AUTHOR:
    case DIR_MODE_TITLE:
    case DIR_MODE_SUPERFITER:
    case DIR_MODE_WEB_THREAD:
        return true;
    }
    return false;
}

/** ��ʼ��filearg�ṹ
  */
void init_write_dir_arg(struct write_dir_arg *filearg)
{
    filearg->filename = NULL;
    filearg->fileptr = MAP_FAILED;
    filearg->ent = -1;
    filearg->fd = -1;
    filearg->size = -1;
    filearg->needclosefd = false;
    filearg->needlock = true;
}

/** ��ʼ��filearg�ṹ,�Ѹ�������mmap��
  */
int malloc_write_dir_arg(struct write_dir_arg *filearg)
{
    int ret;
    char *ret_ptr;
    if (filearg->fileptr == MAP_FAILED) {
        if (filearg->fd == -1) {
            ret = safe_mmapfile(filearg->filename, O_RDWR, PROT_READ | PROT_WRITE, MAP_SHARED, &ret_ptr, &filearg->size, &filearg->fd);
            filearg->fileptr = (struct fileheader*)ret_ptr;
            if (ret == 0)
                return -1;
            filearg->needclosefd = true;
        } else {                //��fd����
            ret = safe_mmapfile_handle(filearg->fd, PROT_READ | PROT_WRITE, MAP_SHARED, &ret_ptr, &filearg->size);
            filearg->fileptr = (struct fileheader*)ret_ptr;
            if (ret == 0)
                return -1;
        }
    }
    return 0;
}

/*
 *  �ͷ�filearg���������Դ��
 */
void free_write_dir_arg(struct write_dir_arg *filearg)
{
    if (filearg->needclosefd && (filearg->fd != -1)) {
        close(filearg->fd);
    }
    if (filearg->fileptr != MAP_FAILED) {
        end_mmapfile((void *) filearg->fileptr, filearg->size, -1);
        filearg->fileptr = MAP_FAILED;
    }
}

/*
 * дdir�ļ�֮ǰ���ļ���λ����ȷ��λ�ã�lockס����������Ӧ������
 * ����߲�������free_write_dir_arg���ͷ���Դ����Ҫ�ϲ��Լ���
 * ���ô˺�������ɹ����ǵü�ʱflock(filearg->fd,LOCK_UN)
 * @param filearg ����Ľṹ�����filearg->fd��Ϊ-1��˵����Ҫ��dirarg->direct�ļ�
 *                        ����ʹ��filearg->fd��Ϊ�ļ������
 *                        filearg->entΪԤ�Ƶ�λ�ã�ent>=1
 *                        filearg->fileptr���������MAP_FAILED,��ô���������mmap�ļ���
 *                        ��дfilearg->fileptr��filearg->size���ڷ�������
 * @param fileinfo ���ڶ�λ�Ķ�������Ϊ�յ�ʱ����Ҫ��λ
 * @param mode ��ǰģʽ��ֻ��sorted�Ŀ���ʹ�ö���
 * @return 0�ɹ�,��ʱmmap��ϡ�
 */
int prepare_write_dir(struct write_dir_arg *filearg, struct fileheader *fileinfo, int mode)
{
    int ret = 0;
    bool needrelocation = false;

    BBS_TRY {
        int count;
        struct fileheader *nowFh;

        if (malloc_write_dir_arg(filearg) != 0)
            BBS_RETURN(-1);
        count = filearg->size / sizeof(struct fileheader);
        if (filearg->needlock)
            flock(filearg->fd, LOCK_EX);
        if (fileinfo) {         //��λһ��
            if ((filearg->ent > count) || (filearg->ent <= 0))
                needrelocation = true;
            else {
                nowFh = filearg->fileptr + (filearg->ent - 1);
                needrelocation = strcmp(fileinfo->filename, nowFh->filename);
            }

        }
        if (needrelocation) {   //�ض�λ���λ��
            int i;

            if (is_sorted_mode(mode)) {
                filearg->ent = Search_Bin(filearg->fileptr, fileinfo->id, 0, count - 1) + 1;
            } else {            //ƥ���ļ���
                int oldent = filearg->ent;

                nowFh = filearg->fileptr;
                filearg->ent = -1;
                /*
                 * �ȴӵ�ǰλ����ǰ�ң���Ϊһ�㶼�Ǳ�ɾ��������ǰ��
                 */
                nowFh = filearg->fileptr + (oldent - 1);
                for (i = oldent - 1; i >= 0; i--, nowFh--) {
                    if (!strcmp(fileinfo->filename, nowFh->filename)) {
                        filearg->ent = i + 1;
                        break;
                    }
                }
                /*
                 * �ٴӵ�ǰλ��������
                 */
                nowFh = filearg->fileptr + oldent;
                for (i = oldent; i < count; i++, nowFh++) {
                    if (!strcmp(fileinfo->filename, nowFh->filename)) {
                        filearg->ent = i + 1;
                        break;
                    }
                }
            }
            if (filearg->ent <= 0)
                ret = -1;
        }
    }
    BBS_CATCH {
        ret = -1;
    }
    BBS_END;
    if (ret != 0)
        flock(filearg->fd, LOCK_UN);
    return ret;
}

int del_origin(const char *board, struct fileheader *fileinfo)
{
    struct write_dir_arg dirarg;
    char olddirect[PATHLEN];
//    struct fileheader fh;

    if (setboardorigin(board, -1)) {
        board_regenspecial(board, DIR_MODE_ORIGIN, NULL);
        return 0;
    }

    init_write_dir_arg(&dirarg);

    setbdir(DIR_MODE_ORIGIN, olddirect, board);
    dirarg.filename = olddirect;

    if (prepare_write_dir(&dirarg, fileinfo, DIR_MODE_ORIGIN) != 0) {
        free_write_dir_arg(&dirarg);
        return -1;
    }

    BBS_TRY {
//        fh = *(dirarg.fileptr + (dirarg.ent - 1));
        memmove(dirarg.fileptr + (dirarg.ent - 1), dirarg.fileptr + dirarg.ent, dirarg.size - sizeof(struct fileheader) * dirarg.ent);
    }
    BBS_CATCH {
    }
    BBS_END;

    dirarg.needclosefd = false;
    free_write_dir_arg(&dirarg);
    dirarg.size -= sizeof(struct fileheader);
    ftruncate(dirarg.fd, dirarg.size);
    if (dirarg.needlock)
        flock(dirarg.fd, LOCK_UN);
    close(dirarg.fd);

    return 0;
}

int deny_modify_article(const struct boardheader *bh, const struct fileheader *fileinfo, int mode, session_t* session)
{
    if (fileinfo==NULL || session->currentuser==NULL) {
        return -1;
    }

    if (deny_me(session->currentuser->userid, bh->filename) && (!HAS_PERM(session->currentuser, PERM_SYSOP))) {
        return -2;
    }

    if (!strcmp(bh->filename, "syssecurity")) {
        return -3;
    }

    if ((mode>= DIR_MODE_THREAD) && (mode<= DIR_MODE_WEB_THREAD)) /*��Դdirect�����޸�*/
        return -4;

    if(mode==DIR_MODE_SELF)
        return -4;

    if (checkreadonly(bh->filename))      /* Leeward 98.03.28 */
        return -5;

    if (!HAS_PERM(session->currentuser, PERM_SYSOP)
        && !chk_currBM(bh->BM, session->currentuser)
        && !isowner(session->currentuser, fileinfo)) {
        return -6;
    }
    return 0;
}

int deny_del_article(const struct boardheader *bh,const struct fileheader *fileinfo,session_t* session){
    if(!session||!(session->currentuser)||!bh||!(bh->filename[0]))
        return -1;
    if(!strcmp(bh->filename,"syssecurity"))
        return -3;
    if(HAS_PERM(session->currentuser,PERM_SYSOP)||chk_currBM(bh->BM,session->currentuser))
        return 0;
    if(fileinfo){
        if(!isowner(session->currentuser,fileinfo))
            return -6;
#ifdef HAPPY_BBS
        if(!strcmp(bh->filename,"newcomers"))
            return -6;
#endif
#ifdef COMMEND_ARTICLE
        if(!strcmp(bh->filename,COMMEND_ARTICLE))
            return -6;
#endif
        return 0;
    }
    return -6;
}

int do_del_post(struct userec *user,struct write_dir_arg *dirarg,struct fileheader *fileinfo,
    const char *board,int currmode,int flag,session_t* session){
    int owned;
    struct fileheader fh;

#ifdef CYGWIN
    bool old_needclosefd;
#endif

    if (prepare_write_dir(dirarg, fileinfo, currmode) != 0)
        return -1;
    BBS_TRY {
        fh = *(dirarg->fileptr + (dirarg->ent - 1));
        memmove(dirarg->fileptr + (dirarg->ent - 1), dirarg->fileptr + dirarg->ent, dirarg->size - sizeof(struct fileheader) * dirarg->ent);
#if 0 //atppp 20060422
        newbbslog(BBSLOG_DEBUG, "%s ftruncate %d", dirarg->filename ? dirarg->filename : currboard->filename, dirarg->size);
#endif
#ifdef CYGWIN
        old_needclosefd = dirarg->needclosefd;
        dirarg->needclosefd = false;
        free_write_dir_arg(dirarg);
        dirarg->size -= sizeof(struct fileheader);
        ftruncate(dirarg->fd, dirarg->size);
        dirarg->needclosefd = old_needclosefd;
        malloc_write_dir_arg(dirarg);
#else
        dirarg->size -= sizeof(struct fileheader);
        ftruncate(dirarg->fd, dirarg->size);
#endif
    }
    BBS_CATCH {
    }
    BBS_END;
    if (dirarg->needlock)
        flock(dirarg->fd, LOCK_UN);     /*�������Ҫ�Ͻ����� */

    if (fh.id == fh.groupid) {
        del_origin(board, &fh);
    }
    setboardtitle(board, 1);

    /* reduce reply count, pig2532 */
#ifdef HAVE_REPLY_COUNT
    if(!(flag & ARG_BMFUNC_FLAG))
        if(fh.id != fh.groupid)
            modify_reply_count(board, fh.groupid, -1, 0, NULL);
#endif /* HAVE_REPLY_COUNT */

    owned=(!(flag&ARG_BMFUNC_FLAG)&&isowner(user,&fh));
    cancelpost(board, user->userid, &fh, owned, 1, session);
    updatelastpost(board);
    if (fh.accessed[0] & FILE_MARKED)
        setboardmark(board, 1);
    if (DIR_MODE_NORMAL == currmode) {    /* Leeward 98.06.17 ����ժ��ɾ�Ĳ���������Ŀ */
        if (owned) {
            if ((int) user->numposts > 0 && !junkboard(board)) {
                user->numposts--;       /*�Լ�ɾ�������£�����post�� */
            }
        } else if ((flag&ARG_DELDECPOST_FLAG) /*����ɾ��,����POST�� */ && !strstr(fh.owner, ".")) {
            struct userec *lookupuser;
            int id = getuser(fh.owner, &lookupuser);

            if (id && (int) lookupuser->numposts > 0 && !junkboard(board) && strcmp(board, SYSMAIL_BOARD)) {    /* SYSOP MAIL��ɾ�Ĳ������� Bigman: 2000.8.12 *//* Leeward 98.06.21 adds above later 2 conditions */
                lookupuser->numposts--;
            }
        }
    }
    if (user != NULL)
        bmlog(user->userid, board, 8, 1);
    newbbslog(BBSLOG_USER, "Del '%s' on '%s'", fh.title, board);        /* bbslog */
    return 0;
}

/* del top article, by pig2532 on 2005.12.22
this function is made from the original "del_ding"
parameters:
    boardname
    ent: top article's number
    fh: fileheader of which top article to be deleted
    session: usually transfer the current session
retuen:
    -2: fileheader is NULL
    -1: "delete_record" function return failed
    0: success
*/
/* add by stiger,delete �ö����� */
int do_del_ding(char *boardname, int bid, int ent, struct fileheader *fh, session_t* session)
{
    int failed;
    char dingdirect[PATHLEN];

    if (fh==NULL)
        return -2;  /* nothing in fileheader */

    setbdir(DIR_MODE_ZHIDING, dingdirect, boardname);
    failed = delete_record(dingdirect, sizeof(struct fileheader), ent, (RECORD_FUNC_ARG) cmpname, fh->filename);

    if(failed){
        return -1;  /* failed to delete */
    }
    else
    {
		char buf[PATHLEN],fn_old[PATHLEN],fn_new[PATHLEN];
		struct fileheader postfile;

        memcpy(&postfile, fh, sizeof(postfile));
    	snprintf(postfile.title, ARTICLE_TITLE_LEN, "%-32.32s - %s", fh->title, session->currentuser->userid);
    	postfile.accessed[sizeof(postfile.accessed) - 1] = time(0) / (3600 * 24) % 100;

        setbfile(fn_old,boardname,postfile.filename);
        postfile.filename[(postfile.filename[1]=='/')?2:0]='Y';
        setbfile(fn_new,boardname,postfile.filename);

        setbdir(DIR_MODE_DELETED, buf, boardname);
        append_record(buf, &postfile, sizeof(postfile));

        f_mv(fn_old,fn_new);
        board_update_toptitle(bid, true);

    }
    return 0;   /* success */
}

static int insert_func(int fd, struct fileheader *start, int ent, int total, struct fileheader *data, bool match)
{
    int i;
    struct fileheader UFile;

    if (match||!total)
        return 0;
    UFile = start[total - 1];
    for (i = total - 1; i >= ent; i--)
        start[i] = start[i - 1];
    lseek(fd, 0, SEEK_END);
    if (safewrite(fd, &UFile, sizeof(UFile)) == -1)
        bbslog("user", "%s", "apprec write err!");
    start[ent - 1] = *data;
    return ent;
}

/* do undel an article to board
	modified from original UndeleteArticle by pig2532 on 2005.12.18
	parameters:
		boardname: the board's name to undelete at
		dirfname: index file name, usually boards/(boardname)/.DELETED
		fileheader: the file header of which article to be undeleted
		title: to RETURN the original article title
	return:
		-1: file not exists
		0: unable to open file
		1: success
*/
/* undelete һƪ���� Leeward 98.05.18 */
/* modified by ylsdd */
int do_undel_post(char* boardname, char *dirfname, int num, struct fileheader *fileinfo, char *title, session_t* session)
{
    char *p, buf[1024], genbuf[1024];
    char UTitle[128];
    struct fileheader UFile;
    int i;
    FILE *fp;
    int fd;

    sprintf(buf, "boards/%s/%s", boardname, fileinfo->filename);
    if (!dashf(buf)) {
        return -1;
    }
    fp = fopen(buf, "r");
    if (!fp)
        return 0;

    strcpy(UTitle, fileinfo->title);
    if ((p = strrchr(UTitle, '-')) != NULL) {   /* create default article title */
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i--) {
            if (UTitle[i] != ' ')
                break;
            else
                UTitle[i] = 0;
        }
    }

    i = 0;
    while (!feof(fp) && i < 2) {
        skip_attach_fgets(buf, 1024, fp);
        if (feof(fp))
            break;
        if (strstr(buf, "������: ") && strstr(buf, "), ����: ")) {
            i++;
        } else if (strstr(buf, "��  ��: ")) {
            i++;
            strncpy(UTitle, buf + 8, sizeof(UTitle));
            UTitle[sizeof(UTitle)-1] = '\0';
            if ((p = strchr(UTitle, '\n')) != NULL)
                *p = 0;
        }
    }
    fclose(fp);

    memcpy(&UFile, fileinfo, sizeof(UFile));
    strnzhcpy(UFile.title, UTitle, ARTICLE_TITLE_LEN);
    UFile.accessed[1] &= ~FILE_DEL;
    if (UFile.filename[1] == '/')
        UFile.filename[2] = 'M';
    else
        UFile.filename[0] = 'M';

    setbfile(genbuf, boardname, fileinfo->filename);
    setbfile(buf, boardname, UFile.filename);
    if (dashf(buf)) {
        return -1;
    }
    f_mv(genbuf, buf);

    sprintf(buf, "boards/%s/.DIR", boardname);
    if ((fd = open(buf, O_RDWR | O_CREAT, 0644)) != -1) {
        if ((UFile.id == 0) || mmap_search_apply(fd, &UFile, insert_func) == 0) {
            flock(fd, LOCK_EX);
            UFile.id = get_nextid(boardname);
            UFile.groupid = UFile.id;
            UFile.reid = UFile.id;
            lseek(fd, 0, SEEK_END);
            if (safewrite(fd, &UFile, sizeof(UFile)) == -1)
                bbslog("user", "%s", "apprec write err!");
            flock(fd, LOCK_UN);
        }
        close(fd);
    }

    /* restore origin, pig2532, 2007.6 */
    if(UFile.id == UFile.groupid) {
        if(setboardorigin(boardname, -1))
            board_regenspecial(boardname, DIR_MODE_ORIGIN, NULL);
        else {
            sprintf(buf, "boards/%s/.ORIGIN", boardname);
            if ((fd = open(buf, O_RDWR | O_CREAT, 0644)) != -1) {
                if ((UFile.id == 0) || mmap_search_apply(fd, &UFile, insert_func) == 0) {
                    flock(fd, LOCK_EX);
                    UFile.id = get_nextid(boardname);
                    UFile.groupid = UFile.id;
                    UFile.reid = UFile.id;
                    lseek(fd, 0, SEEK_END);
                    if (safewrite(fd, &UFile, sizeof(UFile)) == -1)
                        bbslog("user", "%s", "apprec origin write err!");
                    flock(fd, LOCK_UN);
                }
                close(fd);
            }
        }
    }
    
    updatelastpost(boardname);
    fileinfo->filename[0] = '\0';
    substitute_record(dirfname, fileinfo, sizeof(*fileinfo), num);
    sprintf(buf, "undeleted %s's ��%s�� on %s", UFile.owner, UFile.title, boardname);
    bbslog("user", "%s", buf);

#ifdef HAVE_REPLY_COUNT
    if(fileinfo->id == fileinfo->groupid)
        refresh_reply_count(boardname, fileinfo->groupid);
    else
        modify_reply_count(boardname, fileinfo->groupid, 1, 0, NULL);
#endif /* HAVE_REPLY_COUNT */

	if(title != NULL)
	{
		sprintf(title, "%s", UFile.title);
	}

    bmlog(session->currentuser->userid, boardname, 9, 1);

    return 1;
}

/* by ylsdd 
   this item is added to the file '.DELETED' under
   the board's directory,
   Unlike the fb code which moves the file to the deleted
   board.
*/
void cancelpost(const char *board, const char *userid, struct fileheader *fh, int owned, int autoappend, session_t* session)
{
    char oldpath[PATHLEN];
    char newpath[PATHLEN];
    time_t now = time(NULL);

/*
    sprintf(oldpath, "/board/%s/%s.html", board, fh->filename);
    ca_expire_file(oldpath);*/

    if ((fh->innflag[1] == 'S') && (fh->innflag[0] == 'S')
        && (get_posttime(fh) > now - 14 * 86400)) {
        cancel_inn(board, fh);
    }

    setbfile(oldpath, board, fh->filename);
    if (fh->filename[1] == '/')
        fh->filename[2] = (owned) ? 'J' : 'D';
    else
        fh->filename[0] = (owned) ? 'J' : 'D';
    setbfile(newpath, board, fh->filename);
    f_mv(oldpath, newpath);

    sprintf(oldpath, "%-32.32s - %s", fh->title, userid);
    strncpy(fh->title, oldpath, ARTICLE_TITLE_LEN - 1);
    fh->title[ARTICLE_TITLE_LEN - 1] = 0;
    fh->accessed[sizeof(fh->accessed) - 1] = now / (3600 * 24) % 100;
    if (autoappend) {
        setbdir((owned) ? DIR_MODE_JUNK : DIR_MODE_DELETED, oldpath, board);
        append_record(oldpath, fh, sizeof(struct fileheader));
    }
}


void add_loginfo(char *filepath, struct userec *user, char *currboard, int Anony, session_t* session)
{                               /* POST ���һ�� ��� */
    FILE *fp;
    int color, noidboard;
    char fname[STRLEN];

    noidboard = (anonymousboard(currboard) && Anony);   /* etc/anonymous�ļ��� ����������� */
    color = (user->numlogins % 7) + 31; /* ��ɫ����仯 */
    sethomefile(fname, user->userid, "signatures");
    fp = fopen(filepath, "ab");
    if (!dashf(fname) ||        /* �ж��Ƿ��Ѿ� ���� ǩ���� */
        user->signature == 0 || noidboard) {
        fputs("\n--\n", fp);
    } else {                    /*Bigman 2000.8.10�޸�,���ٴ��� */
        fprintf(fp, "\n");
    }
    /*
     * ��Bigman����:2000.8.10 Announce�������������� 
     */
    if (!strcmp(currboard, "Announce"))
        fprintf(fp, "\033[m\033[1;%2dm�� ��Դ:��%s %s��[FROM: %s]\033[m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, BBS_FULL_NAME);
    else
        fprintf(fp, "\n\033[m\033[1;%2dm�� ��Դ:��%s %s��[FROM: %s]\033[m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, (noidboard) ? NAME_ANONYMOUS_FROM : SHOW_USERIP(session->currentuser, session->fromhost));
    fclose(fp);
    return;
}

void addsignature(FILE * fp, struct userec *user, int sig)
{
    FILE *sigfile;
    int i, valid_ln = 0;
    char tmpsig[MAXSIGLINES][256];
    char inbuf[256];
    char fname[STRLEN];

    if (sig == 0)
        return;
    sethomefile(fname, user->userid, "signatures");
    if ((sigfile = fopen(fname, "r")) == NULL) {
        return;
    }
    fputs("\n--\n", fp);
    for (i = 1; i <= (sig - 1) * MAXSIGLINES && sig != 1; i++) {
        if (!fgets(inbuf, sizeof(inbuf), sigfile)) {
            fclose(sigfile);
            return;
        }
    }
    for (i = 1; i <= MAXSIGLINES; i++) {
        if (fgets(inbuf, sizeof(inbuf), sigfile)) {
            if (inbuf[0] != '\n')
                valid_ln = i;
            strcpy(tmpsig[i - 1], inbuf);
        } else
            break;
    }
    fclose(sigfile);
    for (i = 1; i <= valid_ln; i++)
        fputs(tmpsig[i - 1], fp);
}

int write_posts(char *id, const char *board, unsigned int groupid)
{
    time_t now;
    struct posttop postlog, pl;
    char xpostfile[PATHLEN];

#ifdef BLESS_BOARD
    if (strcasecmp(board, BLESS_BOARD) && (!poststatboard(board) || normal_board(board) != 1))
#else
    if (!poststatboard(board) || normal_board(board) != 1)
#endif
        return 0;
    now = time(0);
//    strcpy(postlog.author, id);
    strncpy(postlog.board, board, sizeof(postlog.board));
    postlog.board[sizeof(postlog.board)-1]='\0';
    postlog.groupid = groupid;
    postlog.date = now;
    postlog.number = 1;

    sprintf(xpostfile, "tmp/Xpost/%s", id);

    {                           /* added by Leeward 98.04.25 
                                 * TODO: ����ط��е㲻��,ÿ�η���Ҫ����һ��,���浽.Xpost��,
                                 * �������ʮ����ͳ�����ID����������.����
                                 * KCN */
        int log = 1;
        FILE *fp = fopen(xpostfile, "r");

        if (fp) {
            while (!feof(fp)) {
                fread(&pl, sizeof(pl), 1, fp);
                if (feof(fp))
                    break;

                if (pl.groupid == groupid && !strcmp(pl.board, board)) {
                    log = 0;
                    break;
                }
            }
            fclose(fp);
        }

        if (log) {
            append_record(xpostfile, &postlog, sizeof(postlog));
            append_record(".newpost", &postlog, sizeof(postlog));
        }
    }

/*    append_record(".post.X", &postlog, sizeof(postlog));
*/
    return 0;
}

void write_header(FILE * fp, struct userec *user, int in_mail, const char *board, const char *title, int Anony, int mode, session_t* session)
{
    int noname;
    char uid[20];
    char uname[40];
    time_t now;

    now = time(0);
    strncpy(uid, user->userid, 20);
    uid[19] = '\0';
    if (in_mail)
#if defined(MAIL_REALNAMES)
        strncpy(uname, user->realname, NAMELEN);
#else
        strncpy(uname, user->username, NAMELEN);
#endif
    else
#if defined(POSTS_REALNAMES)
        strncpy(uname, user->realname, NAMELEN);
#else
        strncpy(uname, user->username, NAMELEN);
#endif
    /*
     * uid[39] = '\0' ; SO FUNNY:-) ����� 20 ����ȴ�� 39 !
     * Leeward: 1997.12.11 
     */
    uname[39] = 0;              /* ��ʵ��д���������! �ٺ� */
    if (in_mail)
        fprintf(fp, "������: %s (%s)\n", uid, uname);
    else {
        noname = anonymousboard(board);
        /*
         * if (((mode == 0) || (mode == 2)) && !(noname && Anony)) {
         * *
         * * mode=0�������ķ��Ĳ���local save 
         * * * mode=1�ǲ���Ҫ��¼��
         * * * mode=2�Ƿ�local save��
         * *
         * write_posts(user->userid, board, title);
         * }
         */
#ifdef SMTH
        if (!strcmp(board, "Announce") && Anony)
            /*
             * added By Bigman 
             */
            fprintf(fp, "������: %s (%s), ����: %s\n", "SYSOP", NAME_SYSOP, board);
        else
#endif
#ifdef ANONYPOST 
        {
            char anonynick[40];
            sprintf(anonynick, "%s-%d", NAME_ANONYMOUS, session->anonyindex);
            fprintf(fp, "������: %s (%s), ����: %s\n", (noname && Anony) ? "guest" : uid, (noname && Anony) ? anonynick : uname, board);
        }
#else /* ANONYPOST*/
            fprintf(fp, "������: %s (%s), ����: %s\n", (noname && Anony) ? board : uid, (noname && Anony) ? NAME_ANONYMOUS : uname, board);
#endif /* ANONYPOST*/
    }

    fprintf(fp, "��  ��: %s\n", title);
    /*
     * ����ת�ű�� czz 020819 
     */
    if (in_mail)
        fprintf(fp, "����վ: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
    else if (mode != 2)
        fprintf(fp, "����վ: %s (%24.24s), վ��\n", BBS_FULL_NAME, ctime(&now));
    else
        fprintf(fp, "����վ: %s (%24.24s), ת��\n", BBS_FULL_NAME, ctime(&now));
    if (in_mail)
        fprintf(fp, "��  Դ: %s \n", session->fromhost);
    fprintf(fp, "\n");

}

static int getcross(const char *filepath,const char *quote_file,struct userec *user,int in_mail,const char *sourceboard,
    const char *title,int Anony,int mode,int local_article,const struct boardheader *toboard,session_t* session){
    FILE *fin,*fout;
    char buf[256];
    int size;
    if(!(fin=fopen(quote_file,"r")))
        return -1;
    if(!(fout=fopen(filepath,"w"))){
        fclose(fin);
        return -1;
    }
    if(mode==0||(mode==3&&in_mail)){
        write_header(fout,user,in_mail,toboard->filename,title,Anony,(local_article?1:2),session);
        if(in_mail)
            fprintf(fout,"\033[1;37m�� ��������ת���� \033[1;32m%s \033[1;37m������ ��\033[m\n",user->userid);
        else
            fprintf(fout,"�� ��������ת���� %s ������ ��\n",sourceboard);
    }
    else if(mode==1){
        time_t current=time(NULL);
        fprintf(fout,"������: "DELIVER" (�Զ�����ϵͳ), ����: %s\n",sourceboard);
        fprintf(fout,"��  ��: %s\n",title);
        fprintf(fout,"����վ: %s�Զ�����ϵͳ (%24.24s)\n\n",BBS_FULL_NAME,ctime(&current));
        fprintf(fout,"����ƪ���������Զ�����ϵͳ��������\n\n");
    }
    /* fancyrabbit May 28 2007 mode == 5 ˵���Ǿ���ת���������ϼ�������ĳЩ���������ľͱ𲻿� re �� ...*/
    else if(mode==2 || mode == 5){
        write_header(fout,user,in_mail,toboard->filename,title,Anony,(local_article?1:2),session);
    }
    else if(mode==3){
        write_header(fout,user,in_mail,toboard->filename,title,Anony,(local_article?1:2),session);
        if((!skip_attach_fgets(buf,256,fin)||strncmp(buf,"������: ",8))
            ||(!skip_attach_fgets(buf,256,fin)||strncmp(buf,"��  ��: ",8))
            ||(!skip_attach_fgets(buf,256,fin)||strncmp(buf,"����վ: ",8))){
            fseek(fin,0,SEEK_SET);
        }
        else{
            while(skip_attach_fgets(buf,256,fin)&&buf[0]!='\n')
                continue;
            fprintf(fout,"\033[0;33m[ �û� %s ��ת��ʱѡ���������ڲ�ת����Դ ]\033[m\n\n",user->userid);
        }
    }
    else if(mode==4){
        write_header(fout,user,in_mail,toboard->filename,title,Anony,(local_article?1:2),session);
        fprintf(fout,"\033[0;33m[ �û� %s ��ת��ʱ��%s���ݽ����˱༭ ]\033[m\n\n",user->userid,(!in_mail?"����":"�ż�"));
    }
    if((toboard->flag&BOARD_ATTACH)||(!user||HAS_PERM(user,PERM_SYSOP))){
        while((size=-attach_fgets(buf,256,fin))){
            if(!strncmp(buf,"�� ��������ת���� ",18)&&strstr(&buf[18]," ������ ��"))
                continue;
            if(size<0)
                fprintf(fout,"%s",buf);
            else
                put_attach(fin,fout,size);
        }
    }
    else{
        while((size=skip_attach_fgets(buf,256,fin))){
            if(!strncmp(buf,"�� ��������ת���� ",18)&&strstr(&buf[18]," ������ ��"))
                continue;
            if(size>0)
                fprintf(fout,"%s",buf);
        }
    }
    fclose(fin);
    fclose(fout);
    return 0;
}

#ifdef COMMEND_ARTICLE
int post_commend(struct userec *user, const char *fromboard, struct fileheader *fileinfo)
{                               /* �Ƽ� */
    struct fileheader postfile;
    char filepath[STRLEN];
    char oldfilepath[STRLEN];
    char buf[256];
    int aborted;
    int fd, err = 0, nowid = 0;

    memset(&postfile, 0, sizeof(postfile));

    setbfile(filepath, COMMEND_ARTICLE, "");

    if ((aborted = GET_POSTFILENAME(postfile.filename, filepath)) != 0) {
        return -1;
    }

    setbfile(filepath, COMMEND_ARTICLE, postfile.filename);
    setbfile(oldfilepath, fromboard, fileinfo->filename);

    if (f_cp(oldfilepath, filepath, 0) < 0)
        return -1;

    strcpy(postfile.title, fileinfo->title);
    strncpy(postfile.owner, user->userid, OWNER_LEN);
    postfile.owner[OWNER_LEN - 1] = 1;
    postfile.eff_size = get_effsize(oldfilepath);
    postfile.o_id = fileinfo->id;
    postfile.o_groupid = fileinfo->groupid;
    postfile.o_reid = fileinfo->reid;
    postfile.o_bid = getbid(fromboard, NULL);
    //strncpy(postfile.o_board, fromboard, STRLEN- BM_LEN);
    //postfile.o_board[STRLEN-BM_LEN-1]=0;

#ifdef HAVE_REPLY_COUNT
    postfile.replycount = 1;
#endif /* HAVE_REPLY_COUNT */

    setbfile(buf, COMMEND_ARTICLE, DOT_DIR);

    if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) == -1) {
        err = 1;
    }

    if (!err) {
        flock(fd, LOCK_EX);
        nowid = get_nextid(COMMEND_ARTICLE);
        postfile.id = nowid;
        postfile.groupid = postfile.id;
        postfile.reid = postfile.id;
        set_posttime(&postfile);
        lseek(fd, 0, SEEK_END);
        if (safewrite(fd, &postfile, sizeof(fileheader)) == -1) {
            bbslog("user", "%s", "apprec write err!");
            err = 1;
        }
        flock(fd, LOCK_UN);
        close(fd);
    }
    if (err) {
        bbslog("3error", "Posting '%s' on '%s': append_record failed!", postfile.title, COMMEND_ARTICLE);
        my_unlink(filepath);
        return -1;
    }
    updatelastpost(COMMEND_ARTICLE);

    setboardorigin(COMMEND_ARTICLE, 1);
    setboardtitle(COMMEND_ARTICLE, 1);

    return 0;
}
#endif

/* Add by SmallPig */
/* ע��˺��������Ȩ�ޣ�caller �뱣֤�����ĺϷ��� */
int post_cross(struct userec *user, const struct boardheader *toboard, const char *fromboard, const char *title, const char *filename, int Anony, int in_mail, char islocal, int mode, session_t* session)
{                               /* (�Զ������ļ���) ת�����Զ����� */
    struct fileheader postfile;
    char filepath[STRLEN];
    char buf4[STRLEN], whopost[IDLEN], save_title[STRLEN];
    int aborted, local_article;

    memset(&postfile, 0, sizeof(postfile));

    strcpy(buf4,title);
    if((mode==0||mode==3||mode==4)&&!strstr(buf4," (ת��)"))
        strcat(buf4," (ת��)");
    strncpy(save_title,buf4,STRLEN);

    setbfile(filepath, toboard->filename, "");

    if ((aborted = GET_POSTFILENAME(postfile.filename, filepath)) != 0) {
        return -1;
    }

    if (mode == 1)
        strcpy(whopost, DELIVER);     /* mode==1Ϊ�Զ����� */
    else
        strcpy(whopost, user->userid);

    strncpy(postfile.owner, whopost, OWNER_LEN);
    postfile.owner[OWNER_LEN - 1] = '\0';
    setbfile(filepath, toboard->filename, postfile.filename);

    local_article=((toupper(islocal)!='L'&&(toboard->flag&BOARD_OUTFLAG))?0:1);

    if (-1 == getcross(filepath, filename, user, in_mail, fromboard, title, Anony, mode, local_article, toboard, session)) {
        return -1;
    }

    postfile.eff_size = get_effsize_attach(filepath, (unsigned int *)&postfile.attachment);     /* FreeWizard: get effsize & attachment */

    strnzhcpy(postfile.title, save_title, ARTICLE_TITLE_LEN);

    if (local_article == 1) {   /* local save */
        postfile.innflag[1] = 'L';
        postfile.innflag[0] = 'L';
    } else {
        postfile.innflag[1] = 'S';
        postfile.innflag[0] = 'S';
        outgo_post(&postfile, toboard->filename, save_title, session);
    }
    if (!strcmp(toboard->filename, "syssecurity")
        && strstr(title, "�޸� ")
        && strstr(title, " ��Ȩ��"))
        postfile.accessed[0] |= FILE_MARKED;    /* Leeward 98.03.29 */
    if (strstr(title, "����Ȩ��") && mode == 2) {
        postfile.accessed[1] |= FILE_READ;
    }
    /* fancyrabbit May 28 2007 ��������� U ���õװ� ...*/
#ifdef SMTH
    if (!strcmp(title, "����澡�����һ�����������") && mode == 2)
    {
	    postfile.accessed[0] |= FILE_MARKED;
	    postfile.accessed[1] |= FILE_READ;
	    add_top(&postfile, toboard -> filename, 0);
    }
#endif /* SMTH */
    after_post(user, &postfile, toboard->filename, NULL, !(Anony), session);
    return 1;
}


/* ע��˺��������Ȩ�ޣ�caller �뱣֤�����ĺϷ��� */
int post_file(struct userec *user, const char *fromboard, const char *filename, const char *nboard, const char *posttitle, int Anony, int mode, session_t* session)
/* ��ĳ�ļ� POST ��ĳ�� */
{
    const struct boardheader *toboard;
    if (mode == 0) {
        return -1; // �Դ���������� atppp ���� 20060425
    }
    if (getbid(nboard, &toboard) <= 0) {       /* ����ҪPOST�İ� ,�ж��Ƿ���ڸð� */
        return -1;
    }
    post_cross(user, toboard, fromboard, posttitle, filename, Anony, false, 'l', mode, session);  /* post �ļ� */
    return 0;
}


/*
 * ע�⣺fh->attachment = 0           ���û�и��� (���ʱ�������ļ���Ҫ����)
 *                      = ���ĳ���    ����и���   (���ʱ������Ĳ��ֹ���)
 * ����һ����֤ attachment ����ֶε���ȷ��������ܻ�©���ˣ�����
 * atppp 20051127
 *
 * ����ֵ��
 * -1: ϵͳ����
 * -2: ��������
 * >0: �·���ȥ���µ� ID 
 *
 */
int after_post(struct userec *user, struct fileheader *fh, const char *boardname, struct fileheader *re, int poststat, session_t* session)
{
    char buf[256];
    int fd, err = 0, nowid = 0;
    char cmd[512];
    
#ifdef FILTER
    char oldpath[50], newpath[50];
    int filtered;
#endif
    const struct boardheader *bh = NULL;
    int bid;

    if ((re == NULL) && (!strncmp(fh->title, "Re: ", 4))) {
        strncpy(fh->title, fh->title + 4, ARTICLE_TITLE_LEN);
    }
    bid = getbid(boardname, &bh);
#ifdef FILTER
    setbfile(oldpath, boardname, fh->filename);
    filtered = 0;
    if (strcmp(fh->owner, DELIVER)) {
        if (((bh && bh->level & PERM_POSTMASK) || normal_board(boardname)) && strcmp(boardname, FILTER_BOARD)) {
            if (check_badword_str(fh->title, strlen(fh->title), session) || check_badword(oldpath, fh->attachment, session))
            {
                /*
                 * FIXME: There is a potential bug here. 
                 */
                setbfile(newpath, FILTER_BOARD, fh->filename);
                f_mv(oldpath, newpath);
                fh->o_bid = bid;
                //strncpy(fh->o_board, boardname, STRLEN - BM_LEN);
                nowid = get_nextid_bid(bid);
                fh->o_id = nowid;
                if (re == NULL) {
                    fh->o_groupid = fh->o_id;
                    fh->o_reid = fh->o_id;
                } else {
                    fh->o_groupid = re->groupid;
                    fh->o_reid = re->id;
                }
#ifdef ZIXIA
                {
                    char newtitle[STRLEN];
                    snprintf(newtitle, ARTICLE_TITLE_LEN, "[��Ⱥ����]%s", fh->title);
                    mail_file(session->currentuser->userid, newpath, session->currentuser->userid, newtitle, 0, fh);
                }
#endif
                    boardname = FILTER_BOARD;
                filtered = 1;
            };
        }
    }
#endif
    setbfile(buf, boardname, DOT_DIR);

    if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) == -1) {
        err = 1;
    }
    process_control_chars(fh->title,NULL);
    if (!err) {
        flock(fd, LOCK_EX);
        nowid = get_nextid(boardname);
        fh->id = nowid;
        if (re == NULL) {
            fh->groupid = fh->id;
            fh->reid = fh->id;
        } else {
            fh->groupid = re->groupid;
            fh->reid = re->id;
        }
        set_posttime(fh);
        lseek(fd, 0, SEEK_END);
        if (safewrite(fd, fh, sizeof(fileheader)) == -1) {
            bbslog("user", "%s", "apprec write err!");
            err = 1;
        }
        flock(fd, LOCK_UN);
        close(fd);
    }
    if (err) {
        bbslog("3error", "Posting '%s' on '%s': append_record failed!", fh->title, boardname);
        setbfile(buf, boardname, fh->filename);
        my_unlink(buf);
        return -1;
    }
    updatelastpost(boardname);
        
#ifdef FILTER
    if (filtered)
        sprintf(buf, "posted '%s' on '%s' filtered", fh->title, getboard(fh->o_bid)->filename);
    else {
#endif
#ifdef HAVE_BRC_CONTROL
        brc_add_read(fh->id, bid, session);
#endif

        /*
         * ���ļĵ�ԭ��������, stiger 
         */
        if (re) {
            if (re->accessed[1] & FILE_MAILBACK) {

                struct userec *lookupuser;
                char newtitle[STRLEN];

                if (getuser(re->owner, &lookupuser) != 0) {
                    if ((false != canIsend2(session->currentuser, re->owner)) && (check_mail_perm(NULL, lookupuser) == 0)) {
                        setbfile(buf, boardname, fh->filename);
                        snprintf(newtitle, ARTICLE_TITLE_LEN, "[����ת��]%s", fh->title);
                        mail_file(session->currentuser->userid, buf, re->owner, newtitle, 0, fh);
                    }
                }
            }
        }


        sprintf(buf, "posted '%s' on '%s'", fh->title, boardname);

    sprintf(cmd, "wget http://localhost/lilacPost.php?b=%s@%d@%d",boardname,fh->id,fh->reid);
    system(&cmd);
#ifndef NEWPOSTSTAT
        if (poststat && user)
            write_posts(user->userid, boardname, fh->groupid);
#endif

#ifdef DENYANONY
		if (user && !poststat){
			char anonybuf[256];
			struct fileheader tmpf;

			setbfile(anonybuf, boardname, ".ANONYDIR");
			memcpy(&tmpf, fh, sizeof(tmpf));

			strcpy(tmpf.owner, session->currentuser->userid);

		    if ((fd = open(anonybuf, O_WRONLY | O_CREAT, 0664)) != -1) {
		        flock(fd, LOCK_EX);
		        lseek(fd, 0, SEEK_END);
		        safewrite(fd, &tmpf, sizeof(fileheader));
		        flock(fd, LOCK_UN);
		        close(fd);
		    }
		}
#endif

#ifdef FILTER
    }
#endif
#ifdef NEWPOSTLOG
	if(user)
		newpostlog(user->userid, boardname, fh->title, fh->groupid);
#else
    newbbslog(BBSLOG_USER, "%s", buf);
#endif

    if (fh->id == fh->groupid) {
        if (setboardorigin(boardname, -1)) {
            board_regenspecial(boardname, DIR_MODE_ORIGIN, NULL);
        } else {
            setbdir(DIR_MODE_ORIGIN, buf, boardname);
            if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) >= 0) {
                flock(fd, LOCK_EX);
                lseek(fd, 0, SEEK_END);
                if (safewrite(fd, fh, sizeof(fileheader)) == -1) {
                    bbslog("user", "%s", "apprec origin write err!");
                }
                flock(fd, LOCK_UN);
                close(fd);
            }
        }
    }
    
    /* add to reply count in .ORIGIN, pig2532 */
#ifdef HAVE_REPLY_COUNT
    if(fh->groupid == fh->id) 
        modify_reply_count(boardname, fh->id, 1, 1, fh);
    else
        modify_reply_count(boardname, fh->groupid, 1, 0, fh);
#endif /* HAVE_REPLY_COUNT */

    setboardtitle(boardname, 1);
    if (fh->accessed[0] & FILE_MARKED)
        setboardmark(boardname, 1);
    if (user != NULL)
        bmlog(user->userid, boardname, 2, 1);
#ifdef FILTER
    if (filtered)
        return -2;
    else
#endif
        return nowid;
}
/*
 * ע�⣺fh->attachment = 0           ���û�и��� (���ʱ�������ļ���Ҫ����)
 *                      = ���ĳ���    ����и���   (���ʱ������Ĳ��ֹ���)
 * ����һ����֤ attachment ����ֶε���ȷ��������ܻ�©���ˣ�����
 * atppp 20051127
 * ͬ���������������ᴥ��term��discuz��ͬ��Ч����
 * kaede 20101013
 * ����ֵ��
 * -1: ϵͳ����
 * -2: ��������
 * >0: �·���ȥ���µ� ID 
 *
 */
int after_post_nosyn(struct userec *user, struct fileheader *fh, const char *boardname, struct fileheader *re, int poststat, session_t* session)
{
    char buf[256];
    int fd, err = 0, nowid = 0;
   
    
#ifdef FILTER
    char oldpath[50], newpath[50];
    int filtered;
#endif
    const struct boardheader *bh = NULL;
    int bid;

    if ((re == NULL) && (!strncmp(fh->title, "Re: ", 4))) {
        strncpy(fh->title, fh->title + 4, ARTICLE_TITLE_LEN);
    }
    bid = getbid(boardname, &bh);
#ifdef FILTER
    setbfile(oldpath, boardname, fh->filename);
    filtered = 0;
    if (strcmp(fh->owner, DELIVER)) {
        if (((bh && bh->level & PERM_POSTMASK) || normal_board(boardname)) && strcmp(boardname, FILTER_BOARD)) {
            if (check_badword_str(fh->title, strlen(fh->title), session) || check_badword(oldpath, fh->attachment, session))
            {
                /*
                 * FIXME: There is a potential bug here. 
                 */
                setbfile(newpath, FILTER_BOARD, fh->filename);
                f_mv(oldpath, newpath);
                fh->o_bid = bid;
                //strncpy(fh->o_board, boardname, STRLEN - BM_LEN);
                nowid = get_nextid_bid(bid);
                fh->o_id = nowid;
                if (re == NULL) {
                    fh->o_groupid = fh->o_id;
                    fh->o_reid = fh->o_id;
                } else {
                    fh->o_groupid = re->groupid;
                    fh->o_reid = re->id;
                }
#ifdef ZIXIA
                {
                    char newtitle[STRLEN];
                    snprintf(newtitle, ARTICLE_TITLE_LEN, "[��Ⱥ����]%s", fh->title);
                    mail_file(session->currentuser->userid, newpath, session->currentuser->userid, newtitle, 0, fh);
                }
#endif
                    boardname = FILTER_BOARD;
                filtered = 1;
            };
        }
    }
#endif
    setbfile(buf, boardname, DOT_DIR);

    if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) == -1) {
        err = 1;
    }
    process_control_chars(fh->title,NULL);
    if (!err) {
        flock(fd, LOCK_EX);
        nowid = get_nextid(boardname);
        fh->id = nowid;
        if (re == NULL) {
            fh->groupid = fh->id;
            fh->reid = fh->id;
        } else {
            fh->groupid = re->groupid;
            fh->reid = re->id;
        }
        set_posttime(fh);
        lseek(fd, 0, SEEK_END);
        if (safewrite(fd, fh, sizeof(fileheader)) == -1) {
            bbslog("user", "%s", "apprec write err!");
            err = 1;
        }
        flock(fd, LOCK_UN);
        close(fd);
    }
    if (err) {
        bbslog("3error", "Posting '%s' on '%s': append_record failed!", fh->title, boardname);
        setbfile(buf, boardname, fh->filename);
        my_unlink(buf);
        return -1;
    }
    updatelastpost(boardname);
        
#ifdef FILTER
    if (filtered)
        sprintf(buf, "posted '%s' on '%s' filtered", fh->title, getboard(fh->o_bid)->filename);
    else {
#endif
#ifdef HAVE_BRC_CONTROL
        brc_add_read(fh->id, bid, session);
#endif

        /*
         * ���ļĵ�ԭ��������, stiger 
         */
        if (re) {
            if (re->accessed[1] & FILE_MAILBACK) {

                struct userec *lookupuser;
                char newtitle[STRLEN];

                if (getuser(re->owner, &lookupuser) != 0) {
                    if ((false != canIsend2(session->currentuser, re->owner)) && (check_mail_perm(NULL, lookupuser) == 0)) {
                        setbfile(buf, boardname, fh->filename);
                        snprintf(newtitle, ARTICLE_TITLE_LEN, "[����ת��]%s", fh->title);
                        mail_file(session->currentuser->userid, buf, re->owner, newtitle, 0, fh);
                    }
                }
            }
        }


        sprintf(buf, "posted '%s' on '%s'", fh->title, boardname);

    
#ifndef NEWPOSTSTAT
        if (poststat && user)
            write_posts(user->userid, boardname, fh->groupid);
#endif

#ifdef DENYANONY
		if (user && !poststat){
			char anonybuf[256];
			struct fileheader tmpf;

			setbfile(anonybuf, boardname, ".ANONYDIR");
			memcpy(&tmpf, fh, sizeof(tmpf));

			strcpy(tmpf.owner, session->currentuser->userid);

		    if ((fd = open(anonybuf, O_WRONLY | O_CREAT, 0664)) != -1) {
		        flock(fd, LOCK_EX);
		        lseek(fd, 0, SEEK_END);
		        safewrite(fd, &tmpf, sizeof(fileheader));
		        flock(fd, LOCK_UN);
		        close(fd);
		    }
		}
#endif

#ifdef FILTER
    }
#endif
#ifdef NEWPOSTLOG
	if(user)
		newpostlog(user->userid, boardname, fh->title, fh->groupid);
#else
    newbbslog(BBSLOG_USER, "%s", buf);
#endif

    if (fh->id == fh->groupid) {
        if (setboardorigin(boardname, -1)) {
            board_regenspecial(boardname, DIR_MODE_ORIGIN, NULL);
        } else {
            setbdir(DIR_MODE_ORIGIN, buf, boardname);
            if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) >= 0) {
                flock(fd, LOCK_EX);
                lseek(fd, 0, SEEK_END);
                if (safewrite(fd, fh, sizeof(fileheader)) == -1) {
                    bbslog("user", "%s", "apprec origin write err!");
                }
                flock(fd, LOCK_UN);
                close(fd);
            }
        }
    }
    
    /* add to reply count in .ORIGIN, pig2532 */
#ifdef HAVE_REPLY_COUNT
    if(fh->groupid == fh->id) 
        modify_reply_count(boardname, fh->id, 1, 1, fh);
    else
        modify_reply_count(boardname, fh->groupid, 1, 0, fh);
#endif /* HAVE_REPLY_COUNT */

    setboardtitle(boardname, 1);
    if (fh->accessed[0] & FILE_MARKED)
        setboardmark(boardname, 1);
    if (user != NULL)
        bmlog(user->userid, boardname, 2, 1);
#ifdef FILTER
    if (filtered)
        return -2;
    else
#endif
        return nowid;
}
int mmap_search_apply(int fd, struct fileheader *buf, DIR_APPLY_FUNC func)
{
    char *datac;
    struct fileheader *dataf;
    off_t filesize;
    int total;
    int low, high;
    int ret = 0;

    if (flock(fd, LOCK_EX) == -1)
        return 0;
    BBS_TRY {
        if (safe_mmapfile_handle(fd, PROT_READ | PROT_WRITE, MAP_SHARED, &datac, &filesize) == 0) {
            flock(fd, LOCK_UN);
            BBS_RETURN(0);
        }
        dataf = (struct fileheader*)datac;
        total = filesize / sizeof(struct fileheader);
        low = 0;
        high = total - 1;
        while (low <= high) {
            int mid, comp;

            mid = (high + low) / 2;
            comp = (buf->id) - ((dataf + mid)->id);
            if (comp == 0) {
                ret = (*func) (fd, dataf, mid + 1, total, buf, true);
                end_mmapfile(datac, filesize, -1);
                flock(fd, LOCK_UN);
                BBS_RETURN(ret);
            } else if (comp < 0)
                high = mid - 1;
            else
                low = mid + 1;
        }
        ret = (*func) (fd, dataf, low + 1, total, buf, false);
    }
    BBS_CATCH {
    }
    BBS_END;
    end_mmapfile((void *) datac, filesize, -1);

    flock(fd, LOCK_UN);
    return ret;
}

int mmap_dir_search(int fd, const fileheader_t * key, search_handler_t func, void *arg)
{
    char *datac;
    struct fileheader *dataf;
    off_t filesize;
    int total;
    int low, high;
    int mid, comp;
    int ret = 0;

    if (flock(fd, LOCK_EX) == -1)
        return 0;
    BBS_TRY {
        if (safe_mmapfile_handle(fd, PROT_READ | PROT_WRITE, MAP_SHARED, &datac, &filesize) == 0) {
            flock(fd, LOCK_UN);
            BBS_RETURN(0);
        }
        dataf = (struct fileheader*)datac;
        total = filesize / sizeof(fileheader_t);
        low = 0;
        high = total - 1;
        while (low <= high) {
            mid = (high + low) / 2;
            comp = (key->id) - ((dataf + mid)->id);
            if (comp == 0) {
                ret = (*func) (fd, dataf, mid + 1, total, true, arg);
                end_mmapfile(datac, filesize, -1);
                flock(fd, LOCK_UN);
                BBS_RETURN(ret);
            } else if (comp < 0)
                high = mid - 1;
            else
                low = mid + 1;
        }
        ret = (*func) (fd, dataf, low + 1, total, false, arg);
    }
    BBS_CATCH {
    }
    BBS_END;
    end_mmapfile((void *) datac, filesize, -1);

    flock(fd, LOCK_UN);

    return ret;
}

struct dir_record_set {
    fileheader_t *records;
    int num;
    int rec_no;                 /* ��¼�����м��¼�������ļ��еļ�¼�ţ��� 1 �ģ�
                                 * ������¼�ļ�¼�ſ���ͨ�� num �� rec_no ��� */
};

static int get_dir_records(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
    if (match) {
        struct dir_record_set *rs = (struct dir_record_set *) arg;
        int i;
        int off;
        int count = 0;

        off = ent - rs->num / 2;
        rs->rec_no = ent;       /* �����ﱣ���¼�� */
        for (i = 0; i < rs->num; i++) {
            if (off < 1 || off > total)
                bzero(rs->records + i, sizeof(fileheader_t));
            else {
                memcpy(rs->records + i, base + off - 1, sizeof(fileheader_t));
                count++;
            }
            off++;
        }
        return count;
    }

    return 0;
}

int get_records_from_id(int fd, int id, fileheader_t * buf, int num, int *index)
{
    struct dir_record_set rs;
    fileheader_t key;
    int ret;

    rs.records = buf;
    rs.num = num;
    rs.rec_no = 0;
    bzero(&key, sizeof(key));
    key.id = id;
    ret = mmap_dir_search(fd, &key, get_dir_records, &rs);
    if (index != NULL)
        *index = rs.rec_no;

    return ret;
}

int get_ent_from_id_ext(int mode, int id, const char *bname, fileheader_t *fh)
{
    char dir[PATHLEN];
    int fd, index;
	setbdir(mode, dir, bname);
    
    fd = open(dir, O_RDWR, 0644);
    if(fd < 0)
    {
        return(-1);
    }
    if(!get_records_from_id(fd, id, fh, 1, &index))
    {
        close(fd);
	    return(-2);
    }
    close(fd);
    return(index);
}

static int get_ent_id(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
	if(match){
		return ent;
	}else
		return 0;
}

int get_ent_from_id(int mode, int id, const char *bname)
{
	char direct[PATHLEN];
	int fd;
	int ret=0;
	fileheader_t key;

    setbdir(mode, direct, bname);
	if ((fd = open(direct, O_RDWR, 0)) == -1){
		return 0;
	}

	key.id = id;
    ret = mmap_dir_search(fd, &key, get_ent_id, NULL);

	close(fd);
    return ret;
}

struct dir_thread_set {
    fileheader_t *records;
    int num;
};

static int get_dir_threads(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
    if (match) {
        struct dir_thread_set *ts = (struct dir_thread_set *) arg;
        int i;
        int off = 1;
        int count = 0;
        int start = ent + 1;
        int end = total;

        if (ts->num < 0) {
            off = -1;
            start = ent - 1;
            end = 1;
            ts->num = -ts->num;
            for (i = start; i >= end; i--) {
                if (count == ts->num)
                    break;
                if (base[i - 1].groupid == base[ent - 1].groupid) {
                    memcpy(ts->records + count, base + i - 1, sizeof(fileheader_t));
                    count++;
                }
            }
        } else {
            for (i = start; i <= end; i++) {
                if (count == ts->num)
                    break;
                if (base[i - 1].groupid == base[ent - 1].groupid) {
                    memcpy(ts->records + count, base + i - 1, sizeof(fileheader_t));
                    count++;
                }
            }
        }
        return count;
    }

    return 0;
}

/* ���� num ��ʾȡ id ��ͬ����� num ƪ���£�
   ���� num ��ʾȡ id ��ͬ����ǰ |num| ƪ���� */
int get_threads_from_id(const char *filename, int id, fileheader_t * buf, int num)
{
    struct dir_thread_set ts;
    fileheader_t key;
    int fd;
    int ret;

    if (num == 0)
        return 0;
    ts.records = buf;
    ts.num = num;
    bzero(&key, sizeof(key));
    key.id = id;
    if ((fd = open(filename, O_RDWR, 0644)) < 0)
        return -1;
    ret = mmap_dir_search(fd, &key, get_dir_threads, &ts);
    close(fd);

    return ret;
}


/* add article to announce clipboard. pig2532 2006-03-04
create new clipboard file when bname is not empty
return 0 when success, -1 when fail.
*/
int ann_article_import(const char *bname, const char *title, const char *fname, const char *userid)
{
    char clipfile[PATHLEN];
    FILE *fp;
    const struct boardheader *bp;

    sprintf(clipfile, "tmp/clip/%s.announce", userid);
    if(bname)
    {
        if((bp = getbcache(bname)) == NULL)
        {
            return -1;
        }
        if((fp = fopen(clipfile, "w")) == NULL)
            return -1;
        fprintf(fp, "DelSource=OnBoard\n");
        fprintf(fp, "Board=%s\n", bp->filename);
    }
    else
    {
        if((fp = fopen(clipfile, "a")) == NULL)
            return -1;
    }
    if((title != NULL) && (fname != NULL))
    {
        fprintf(fp, "Title=%s\n", title);
        fprintf(fp, "Filename=%s\n", fname);
    }
    fclose(fp);
    return 0;
}


struct dir_gthread_set {
    fileheader_t *records;
    int num;
    int groupid;
    int start;
    int haveprev;
    int operate;
    char *userid;
};

static int get_dir_gthreads(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
    struct dir_gthread_set *ts = (struct dir_gthread_set *) arg;
    int i;
    int count = 0;
    int start = ent;
    int end = total;
    int passprev = 0;
    struct fileheader *fh;

    for (i = start; i <= end; i++) {
        if (count == ts->num)
            break;
        if (base[i - 1].groupid == ts->groupid) {
            if (base[i - 1].id < ts->start) {
                passprev = i;
                continue;
            }
            if(ts->records)
                memcpy(ts->records + count, base + i - 1, sizeof(fileheader_t));
            count++;
            
            fh = base + i - 1;
            switch(ts->operate)
            {
            case 2:    /* mark */
                fh->accessed[0] |= FILE_MARKED;
                break;
            case 3:    /* unmark */
                fh->accessed[0] &= ~FILE_MARKED;
                break;
            case 5:
                ann_article_import(NULL, fh->title, fh->filename, ts->userid);
                fh->accessed[0] |= FILE_IMPORTED;
                break;
            case 6:    /* X */
                fh->accessed[1] |= FILE_DEL;
                break;
            case 7:    /* unX */
                fh->accessed[1] &= ~FILE_DEL;
                break;
            case 8:    /* no reply */
                fh->accessed[1] |= FILE_READ;
                break;
            case 9:    /* cancel no reply */
                fh->accessed[1] &= ~FILE_READ;
                break;
            case 10:
            case 11:
            case 12:
            case 13:
                fh->accessed[0] |= FILE_IMPORTED;
                break;
            }
            
        }
    }

    if (passprev) {
        int i = 1;

        for (; passprev >= start; passprev--) {
            if (i >= ts->num)
                break;
            if (base[passprev - 1].groupid == ts->groupid) {
                ts->haveprev = base[passprev - 1].id;
                i++;
            }
        }
    }
    return count;
}

int get_threads_from_gid(const char *filename, int gid, fileheader_t * buf, int num, int startid, int *haveprev, int operate, struct userec *user)
{
    struct dir_gthread_set ts;
    fileheader_t key;
    int fd;
    int ret;

    if (num == 0)
        return 0;
    ts.records = buf;
    ts.num = num;
    ts.groupid = gid;
    ts.start = startid;
    ts.haveprev = 0;
    ts.operate = operate;
    ts.userid = user->userid;
    bzero(&key, sizeof(key));
    key.id = gid;
    if ((fd = open(filename, O_RDWR, 0644)) < 0)
        return -1;
    ret = mmap_dir_search(fd, &key, get_dir_gthreads, &ts);
    close(fd);

    *haveprev = ts.haveprev;
    return ret;
}

//�������ַ���    by yuhuan
//��flyriverͬѧ����������������
int Search_Bin(struct fileheader *ptr, int key, int start, int end)
{
    // ����������۰������ؼ��ֵ���key������Ԫ�ء�
    // �����ҵ�����������
    // ����Ϊ����key����С����Ԫ������m������(-m-1)
    // -1 ���Ź�������ʹ��
    int low, high, mid;
    struct fileheader *totest;

    low = start;
    high = end;
    while (low <= high) {
        mid = (low + high) / 2;
        totest = (struct fileheader *) (ptr + mid);
        if (key == totest->id)
            return mid;
        else if (key < totest->id)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return -(low + 1);
}

/* *common_flag ������ͨ�û��ɼ��ı��(����δ�����) */
char get_article_flag(struct fileheader *ent, struct userec *user, const char *boardname, int is_bm, char *common_flag, session_t* session)
{
    char unread_mark = (DEFINE(user, DEF_UNREADMARK) ? UNREAD_SIGN : 'N');
    char type, common_type = ' ';

#ifdef HAVE_BRC_CONTROL
    if (strcmp(user->userid, "guest"))
        type = brc_unread(ent->id, session) ? unread_mark : ' ';
    else
#endif
        type = ' ';
    /*
     * add by stiger 
     */
    if (POSTFILE_BASENAME(ent->filename)[0] == 'Z') {
        if (type == ' ')
            type = 'd';
        else
            type = 'D';
        if (common_flag) *common_flag = 'd';
        return type;
    }

    if(POSTFILE_BASENAME(ent->filename)[0]=='Y'){
        type=(type==' '?'y':'Y');
        if(common_flag)
            *common_flag='y';
        return type;
    }

    /*
     * add end 
     */
    if ((ent->accessed[0] & FILE_DIGEST)) {
        common_type = 'g';
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if (ent->accessed[0] & FILE_MARKED) {
        switch (type) {
        case ' ':
            common_type = type = 'm';
            break;
        case UNREAD_SIGN:
        case 'N':
            common_type = 'm';
            type = 'M';
            break;
        case 'g':
            common_type = type = 'b';
            break;
        case 'G':
            common_type = 'b';
            type = 'B';
            break;
        }
    }
#ifdef FREE
	if (0)
#elif defined(OPEN_NOREPLY)
    if (ent->accessed[1] & FILE_READ)
#else
    if (is_bm && (ent->accessed[1] & FILE_READ))
#endif
    {
        switch (type) {
        case 'g':
        case 'G':
            type = 'O';
            break;
        case 'm':
        case 'M':
            type = 'U';
            break;
        case 'b':
        case 'B':
            type = '8';
            break;
        case ' ':
        case '*':
        case 'N':
        default:
            type = ';';
            break;
        }
    } else if ((is_bm || HAS_PERM(user, PERM_OBOARDS)) && (ent->accessed[0] & FILE_SIGN)) {
        type = '#';
    } else if ((is_bm || HAS_PERM(user, PERM_OBOARDS)) && (ent->accessed[0] & FILE_PERCENT)) {
        type = '%';
#ifdef FILTER
#ifdef NEWSMTH
    } else if ((ent->accessed[1] & FILE_CENSOR)
               && ((!strcmp(boardname, FILTER_BOARD) && is_bm)
                   || (!strcmp(boardname, "NewsClub") && (haspostperm(user, "NewsClub") || HAS_PERM(user, PERM_OBOARDS))))) {
        type = '@';
#else
    } else if (is_bm && (ent->accessed[1] & FILE_CENSOR) && !strcmp(boardname, FILTER_BOARD)) {
        type = '@';
#endif
#endif
    }

    if (is_bm && (ent->accessed[1] & FILE_DEL)) {
        type = 'X';
    }
    if (common_flag) *common_flag = common_type;
    return type;
}

int Origin2(text)
char text[256];
{
    char tmp[STRLEN];

    sprintf(tmp, "�� ��Դ:��%s ", BBS_FULL_NAME);

    return (strstr(text, tmp) != NULL);
}

int add_edit_mark(char *fname, int mode, char *title, session_t* session)
{
    FILE *fp, *out;
    char buf[256];
    time_t now;
    char outname[STRLEN];
    int step = 0;
    int added = 0;
    int asize;

    if ((fp = fopen(fname, "rb")) == NULL)
        return 0;
    sprintf(outname, "tmp/%d.editpost", (int)getpid());
    if ((out = fopen(outname, "w")) == NULL) {
        fclose(fp);
        return 0;
    }

    while ((asize = -attach_fgets(buf, 256, fp)) != 0) {
        if (asize <= 0) {
            if (mode & 2) {
                if (step != 3 && !strncmp(buf, "��  ��: ", 8)) {
                    step = 3;
                    fprintf(out, "��  ��: %s\n", title);
                    continue;
                }
            }

            if (!strncmp(buf, "\033[36m�� �޸�:��", 15))
                continue;
            if (Origin2(buf) && (!added)) {
                now = time(0);
                if (mode & 1)
                    fprintf(out, "\033[36m�� �޸�:��%s �� %15.15s �޸ı��š�[FROM: %s]\033[m\n", session->currentuser->userid, ctime(&now) + 4, SHOW_USERIP(session->currentuser, session->fromhost));
                else
                    fprintf(out, "\033[36m�� �޸�:��%s �� %15.15s �޸ı��ġ�[FROM: %s]\033[m\n", session->currentuser->userid, ctime(&now) + 4, SHOW_USERIP(session->currentuser, session->fromhost));
                step = 3;
                added = 1;
            }
            fputs(buf, out);
        } else
            put_attach(fp, out, asize);
    }
    if (!added) {
        now = time(0);
        if (mode & 1)
            fprintf(out, "\033[36m�� �޸�:��%s �� %15.15s �޸ı��š�[FROM: %s]\033[m\n", session->currentuser->userid, ctime(&now) + 4, SHOW_USERIP(session->currentuser, session->fromhost));
        else
            fprintf(out, "\033[36m�� �޸�:��%s �� %15.15s �޸ı��ġ�[FROM: %s]\033[m\n", session->currentuser->userid, ctime(&now) + 4, SHOW_USERIP(session->currentuser, session->fromhost));
    }
    fclose(fp);
    fclose(out);

    f_cp(outname, fname, O_TRUNC);
    unlink(outname);

    return 1;
}

const char *get_mime_type_from_ext(const char *ext) {
	if (ext == NULL)
		return "text/plain";
	if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0)
		return "image/jpeg";
	if (strcasecmp(ext, ".gif") == 0)
		return "image/gif";
	if (strcasecmp(ext, ".png") == 0)
		return "image/png";
	if (strcasecmp(ext, ".pcx") == 0)
		return "image/pcx";
	if (strcasecmp(ext, ".au") == 0)
		return "audio/basic";
	if (strcasecmp(ext, ".wav") == 0)
		return "audio/wav";
	if (strcasecmp(ext, ".avi") == 0)
		return "video/x-msvideo";
	if (strcasecmp(ext, ".mov") == 0 || strcasecmp(ext, ".qt") == 0)
		return "video/quicktime";
	if (strcasecmp(ext, ".mpeg") == 0 || strcasecmp(ext, ".mpe") == 0)
		return "video/mpeg";
	if (strcasecmp(ext, ".vrml") == 0 || strcasecmp(ext, ".wrl") == 0)
		return "model/vrml";
	if (strcasecmp(ext, ".midi") == 0 || strcasecmp(ext, ".mid") == 0)
		return "audio/midi";
	if (strcasecmp(ext, ".mp3") == 0)
		return "audio/mpeg";
	if (strcasecmp(ext, ".txt") == 0)
		return "text/plain";
	if (strcasecmp(ext, ".xht") == 0 || strcasecmp(ext, ".xhtml") == 0)
		return "application/xhtml+xml";
	if (strcasecmp(ext, ".xml") == 0)
		return "text/xml";
	if (strcasecmp(ext, ".swf") == 0)
		return "application/x-shockwave-flash";
	if (strcasecmp(ext, ".pdf") == 0)
		return "application/pdf";
	if (strcasecmp(ext, ".html") == 0 || strcasecmp(ext, ".htm") == 0)
		return "text/html";
	if (strcasecmp(ext, ".css") == 0)
		return "text/css";
	return "application/octet-stream";
}
const char *get_mime_type(const char *filename) {
    char *dot = strrchr(filename, '.');
    return (get_mime_type_from_ext(dot));
}
int get_attachment_type(const char *filename) {
    const char * mime_type = get_mime_type(filename);
    if (strncmp(mime_type, "image/", strlen("image/")) == 0) {
        return ATTACH_IMG;
    }
    if (strcmp(mime_type, "application/x-shockwave-flash") == 0) {
        return ATTACH_FLASH;
    }
    return ATTACH_OTHERS;
}
int get_attachment_type_from_ext(const char *ext) {
    const char * mime_type = get_mime_type_from_ext(ext);
    if (strncmp(mime_type, "image/", strlen("image/")) == 0) {
        return ATTACH_IMG;
    }
    if (strcmp(mime_type, "application/x-shockwave-flash") == 0) {
        return ATTACH_FLASH;
    }
    return ATTACH_OTHERS;
}


char *checkattach(char *buf, long size, long *len, char **attachptr)
{
    char *ptr;
    int att_size;
    if (size < ATTACHMENT_SIZE + sizeof(int) + 2)
        return NULL;
    if (memcmp(buf, ATTACHMENT_PAD, ATTACHMENT_SIZE))
        return NULL;
    buf += ATTACHMENT_SIZE;
    size -= ATTACHMENT_SIZE;

    ptr = buf;
    for (; size > 0 && *buf != 0; buf++, size--);
    if (size == 0)
        return NULL;
    buf++;
    size--;
    if (size < sizeof(int))
        return NULL;
    memcpy(&att_size, buf, sizeof(int));
    *len = ntohl(att_size);
    if (*len > size)
        *len = size;
    *attachptr = buf + sizeof(int);
    return ptr;
}

/**
 * һ���ܼ��attach��fgets
 * �ļ�β����0,���򷵻�1
 */
int skip_attach_fgets(char *s, int size, FILE * stream)
{
    int matchpos = 0;
    int ch;
    char *ptr;

    ptr = s;
    while ((ch = fgetc(stream)) != EOF) {
        if (ch == ATTACHMENT_PAD[matchpos]) {
            matchpos++;
            if (matchpos == ATTACHMENT_SIZE) {
                int size, d;

                while ((ch = fgetc(stream)) != 0);
                fread(&d, 1, 4, stream);
                size = htonl(d);
                fseek(stream, size, SEEK_CUR);
                ptr -= (ATTACHMENT_SIZE - 1);
                matchpos = 0;
                continue;
            }
        }
        *ptr = ch;
        ptr++;
        *ptr = 0;
        if ((ptr - s) == size - 1) {
//           *(ptr-1)=0;
            return 1;
        }
        if (ch == '\n') {
//        *ptr=0;
            return 1;
        }
    }
    /*
     * if(ptr!=s) return 1;
     * else return 0;
     */
    return (ptr != s);
}

int attach_fgets(char *s, int size, FILE * stream)
{
    int matchpos = 0;
    int ch;
    char *ptr;

    ptr = s;
    while ((ch = fgetc(stream)) != EOF) {
        if (ch == ATTACHMENT_PAD[matchpos]) {
            matchpos++;
            if (matchpos == ATTACHMENT_SIZE) {
                int size, d = 0, count = ATTACHMENT_SIZE + 4 + 1;

                ptr = s;
                while ((ch = fgetc(stream)) != 0) {
                    *ptr = ch;
                    ptr++;
                    count++;
                }
                fread(&d, 1, 4, stream);
                size = htonl(d);
                fseek(stream, -count, SEEK_CUR);
                *ptr = 0;
                return -(count + size);
            }
        }
        *ptr = ch;
        ptr++;
        if ((ptr - s) >= size - 2) {
            *ptr = 0;
            return 1;
        }
        if (ch == '\n') {
            *ptr = 0;
            return 1;
        }
    }
    /*
     * if(ptr!=s) return 1;
     * else return 0;
     */
    return (ptr != s);
}

int put_attach(FILE * in, FILE * out, int size)
{
    char buf[1024 * 16];
    int o;

    if (size <= 0)
        return -1;
    while ((o = fread(buf, 1, size > 1024 * 16 ? 1024 * 16 : size, in)) != 0) {
        size -= o;
        fwrite(buf, 1, o, out);
    }
    return 0;
}

int get_effsize_attach(char *ffn, unsigned int *att)
{
    char *p, *op, *attach;
    long attach_len;
    int j;
    off_t fsize;
    int k, abssize = 0, entercount = 0, ignoreline = 0;

    j = safe_mmapfile(ffn, O_RDONLY, PROT_READ, MAP_SHARED, &p, &fsize, NULL);
    op = p;
    if (att) *att = 0;
    if (j) {
        k = fsize;
        while (k) {
            if (NULL != (checkattach(p, k, &attach_len, &attach))) {
                if (att && !*att)
                    *att = p - op;
                k -= (attach - p) + attach_len;
                p = attach + attach_len;
                continue;
            }
            if (j) { //Ϊ�˼�������ǰһ��,������j���ж�bool��,��������eff_done_flag��...
                if (*p == '\n') {
                    if (k >= 3 && *(p + 1) == '-' && *(p + 2) == '-' && *(p + 3) == '\n') {
                        if (att) j = 0;
                        else break; //no need to determine attachment, so we are done.
                    } else if (k >= 5 && *(p + 1) == '\xa1' && *(p + 2) == '\xbe' && *(p + 3) == ' ' && *(p + 4) == '\xd4' && *(p + 5) == '\xda') {
                        ignoreline = 1;
                    } else if (k >= 2 && *(p + 1) == ':' && *(p + 2) == ' ') {
                        ignoreline = 2;
                    } else {
                        entercount++;
                        ignoreline = 0;
                    }
                } else if (k >= 2 && *p == KEY_ESC && *(p + 1) == '[' && *(p + 2) == 'm') {
                    ignoreline = 3;
                } else {
                    if (entercount >= 4 && !ignoreline)
                        abssize++;
                }
            }
            k--;
            p++;
        }
    }
    end_mmapfile((void *) op, fsize, -1);
    return abssize;
}

int get_effsize(char *ffn)
{
    return get_effsize_attach(ffn, NULL);
}

/* �����ö�����*/
int add_top(struct fileheader *fileinfo, const char *boardname, int flag)
{
    struct fileheader top;
    char path[MAXPATH], newpath[MAXPATH], dirpath[MAXPATH];

    if (POSTFILE_BASENAME(fileinfo->filename)[0] == 'Z')
        return 3;
    memcpy(&top, fileinfo, sizeof(top));
    POSTFILE_BASENAME(top.filename)[0] = 'Z';
    setbfile(path, boardname, top.filename);
    top.accessed[0] = flag;
    setbfile(newpath, boardname, fileinfo->filename);
    setbdir(DIR_MODE_ZHIDING, dirpath, boardname);
    if (get_num_records(dirpath, sizeof(top)) > MAX_DING) {
        return 4;
    }
    link(newpath, path);
    append_record(dirpath, &top, sizeof(top));
    board_update_toptitle(getbid(boardname, NULL), true);
    return 0;
}

/*������ժ*/
static int add_digest(struct fileheader *fileinfo, const char *boardname)
{
    struct fileheader digest;
    char path[MAXPATH], newpath[MAXPATH], dirpath[MAXPATH];

    memcpy(&digest, fileinfo, sizeof(digest));
    digest.accessed[0] &= ~FILE_DIGEST;
    POSTFILE_BASENAME(digest.filename)[0] = 'G';
    setbfile(path, boardname, digest.filename);
    digest.accessed[0] = 0;
    setbfile(newpath, boardname, fileinfo->filename);
    setbdir(DIR_MODE_DIGEST, dirpath, boardname);
    if (get_num_records(dirpath, sizeof(digest)) > MAX_DIGEST) {
        return 4;
    }
    link(newpath, path);
    append_record(dirpath, &digest, sizeof(digest));    /* ��ժĿ¼����� .DIR */
    fileinfo->accessed[0] = fileinfo->accessed[0] | FILE_DIGEST;
    return 0;
}

/*ɾ����ժ*/
static int dele_digest(char *dname, const char *boardname)
{                               /* ɾ����ժ��һƪPOST, dname=post�ļ���,boardname�������� */
    char digest_name[STRLEN];
    char new_dir[STRLEN];
    char buf[STRLEN];
    struct fileheader fh;
    int pos;

    strcpy(digest_name, dname);
    setbdir(DIR_MODE_DIGEST, new_dir, boardname);

    POSTFILE_BASENAME(digest_name)[0] = 'G';
    //TODO: ����Ҳ�и�ͬ������
    pos = search_record(new_dir, &fh, sizeof(fh), (RECORD_FUNC_ARG) cmpname, digest_name);      /* ��ժĿ¼�� .DIR�� ���� ��POST */
    if (pos <= 0) {
        return 2;
    }
    delete_record(new_dir, sizeof(struct fileheader), pos, (RECORD_FUNC_ARG) cmpname, digest_name);
    setbfile(buf, boardname, digest_name);
    my_unlink(buf);
    return 0;
}

#ifdef FILTER
static int pass_filter(struct fileheader *fileinfo, const struct boardheader *board, session_t* session)
{
#ifdef SMTH
    if ((!strcmp(board->filename, FILTER_BOARD)) || (!strcmp(board->filename, "NewsClub")))
#else
    if (!strcmp(board->filename, FILTER_BOARD))
#endif
    {
        if (fileinfo->accessed[1] & FILE_CENSOR || fileinfo->o_bid == 0) {
            return 3;
        } else {
            char oldpath[MAXPATH], newpath[MAXPATH], dirpath[MAXPATH];
            struct fileheader newfh;
            int nowid;
            int filedes;

            fileinfo->accessed[1] |= FILE_CENSOR;
            setbfile(oldpath, board->filename, fileinfo->filename);
            setbfile(newpath, getboard(fileinfo->o_bid)->filename, fileinfo->filename);
            f_cp(oldpath, newpath, 0);

            setbfile(dirpath, getboard(fileinfo->o_bid)->filename, DOT_DIR);
            if ((filedes = open(dirpath, O_WRONLY | O_CREAT, 0664)) == -1) {
                return -1;
            }
            newfh = *fileinfo;
            flock(filedes, LOCK_EX);
            nowid = get_nextid_bid(fileinfo->o_bid);
            newfh.id = nowid;
            if (fileinfo->o_id == fileinfo->o_groupid)
                newfh.groupid = newfh.reid = newfh.id;
            else {
                newfh.groupid = fileinfo->o_groupid;
                newfh.reid = fileinfo->o_reid;
            }
            lseek(filedes, 0, SEEK_END);
            if (safewrite(filedes, &newfh, sizeof(fileheader)) == -1) {
                bbslog("user", "apprec write err! %s", newfh.filename);
            }
            flock(filedes, LOCK_UN);
            close(filedes);

            updatelastpost(getboard(fileinfo->o_bid)->filename);
#if 0 /* ����������Ǵ���� - atppp 20051228 */
#ifdef HAVE_BRC_CONTROL
            brc_add_read(newfh.id, session);
#endif
#endif
            if (newfh.id == newfh.groupid)
                setboardorigin(getboard(fileinfo->o_bid)->filename, 1);
            setboardtitle(getboard(fileinfo->o_bid)->filename, 1);
            if (newfh.accessed[0] & FILE_MARKED)
                setboardmark(getboard(fileinfo->o_bid)->filename, 1);
        }
    }
    return 0;
}
#endif                          /* FILTER */

/**
  ����fileheader������
  @param dirarg ��Ҫ������.DIR
  @param currmode ��ǰdir��ģʽ
  @param board ��ǰ����
  @param fileinfo ���½ṹ
  @param flag Ҫ�����ı�־
  @param data ���еĲ������ݡ�
  @param bmlog �Ƿ�������������¼
  TODO: ��������������ĵط����붼������Ȩ��
  @return 0 �ɹ�
              1 ����������
              2 �Ҳ���ԭ��
              3 ���������
              4 ��ժ��(�ö���)��
              -1 �ļ��򿪴���
  */
int change_post_flag(struct write_dir_arg *dirarg, int currmode, const struct boardheader *board, struct fileheader *fileinfo, int flag, struct fileheader *data, bool dobmlog, session_t* session)
{
    char buf[MAXPATH];
    struct fileheader *originFh;
    int ret = 0;

    if (fileinfo && POSTFILE_BASENAME(fileinfo->filename)[0] == 'Z')
        /*
         * �ö������²���������
         */
        return 1;

    /*
     * ����ժ����������ժ����
     */
    if ((flag == FILE_DIGEST_FLAG) && (currmode == DIR_MODE_DIGEST))
        return 1;
    /*
     * if ((flag == FILE_DIGEST_FLAG) && (currmode != DIR_MODE_NORMAL))
     * return 1;
     */

    if (currmode == DIR_MODE_DELETED || currmode == DIR_MODE_JUNK || currmode == DIR_MODE_SELF)
        /*
         * ��ɾ��������ɾ������������
         */
        return 1;

    if ((flag == FILE_MARK_FLAG || flag == FILE_DELETE_FLAG) && (!strcmp(board->filename, "syssecurity")
                                                                 || !strcmp(board->filename, FILTER_BOARD)))
        /*
         * �ڹ��˰壬ϵͳ��¼���治����mark,���ɾ������
         */
        return 1;               /* Leeward 98.03.29 */

    if (flag == FILE_TITLE_FLAG && currmode != DIR_MODE_NORMAL)
        /*
         * ����ͨģʽ�²����޸�����
         */
        return 1;

#ifdef COMMEND_ARTICLE
    if (flag == FILE_COMMEND_FLAG && currmode != DIR_MODE_NORMAL)
        /*
         * ����ͨģʽ�²����Ƽ�
         */
        return 1;
#endif
    if (prepare_write_dir(dirarg, fileinfo, currmode) != 0)
        return 2;

    originFh = dirarg->fileptr + (dirarg->ent - 1);     /*�µ�fileheader */
    setbfile(buf, board->filename, originFh->filename);

    /*
     * mark ����
     */
    if (flag & FILE_MARK_FLAG) {
        if (data->accessed[0] & FILE_MARKED) {
            originFh->accessed[0] |= FILE_MARKED;
            if (dobmlog)
                bmlog(session->currentuser->userid, board->filename, 7, 1);
        } else {
            originFh->accessed[0] &= ~FILE_MARKED;
            if (dobmlog)
                bmlog(session->currentuser->userid, board->filename, 6, 1);
        }
        setboardmark(board->filename, 1);
    }

    /*
     * ���ɻظ� ����
     */
    if (flag & FILE_NOREPLY_FLAG) {
        if (!strcmp(board->filename, SYSMAIL_BOARD)) {
            char ans[STRLEN];

            snprintf(ans, STRLEN, "��%s�� ����: %s", session->currentuser->userid, fileinfo->title);
            strnzhcpy(originFh->title, ans, ARTICLE_TITLE_LEN);
        }
        if (data->accessed[1] & FILE_READ) {
            originFh->accessed[1] |= FILE_READ;
        } else {
            originFh->accessed[1] &= ~FILE_READ;
        }
    }
#ifdef COMMEND_ARTICLE
    if (flag & FILE_COMMEND_FLAG) {
        if (data->accessed[1] & FILE_COMMEND)
            originFh->accessed[1] |= FILE_COMMEND;
        else
            originFh->accessed[1] &= ~FILE_COMMEND;
    }
#endif

    /*
     * ��� ����
     */
    if (flag & FILE_SIGN_FLAG) {
        if (data->accessed[0] & FILE_SIGN)
            originFh->accessed[0] |= FILE_SIGN;
        else
            originFh->accessed[0] &= ~FILE_SIGN;
    }
    if (flag & FILE_PERCENT_FLAG) {
        if (data->accessed[0] & FILE_PERCENT)
            originFh->accessed[0] |= FILE_PERCENT;
        else
            originFh->accessed[0] &= ~FILE_PERCENT;
    }
    /*
     * ���ɾ�� ����
     */
    if (flag & FILE_DELETE_FLAG) {
        if (data->accessed[1] & FILE_DEL)
            originFh->accessed[1] |= FILE_DEL;
        else
            originFh->accessed[1] &= ~FILE_DEL;
    }

    /*
     * ������ժ����
     */
    if (flag & FILE_DIGEST_FLAG) {
        if (data->accessed[0] & FILE_DIGEST) {  /*����DIGEST */
            if (dobmlog) {
                bmlog(session->currentuser->userid, board->filename, 3, 1);
                ret = add_digest(originFh, board->filename);
            } else {            /*��ʵ��ʱ��ֻ��Ҫ��һ�±�־�͹��� */
                originFh->accessed[0] |= FILE_DIGEST;
            }
        } else {                /* ����Ѿ�����ժ�Ļ��������ժ��ɾ����post */
            originFh->accessed[0] = (originFh->accessed[0] & ~FILE_DIGEST);
            if (dobmlog) {
                bmlog(session->currentuser->userid, board->filename, 4, 1);
                ret = dele_digest(originFh->filename, board->filename);
            }
        }
    }
    if (ret == 0) {
        if (flag & FILE_TITLE_FLAG) {
            originFh->groupid = originFh->id;
            originFh->reid = originFh->id;
            if (!strncmp(originFh->title, "Re: ", 4)) {
                strcpy(buf, originFh->title + 4);
                if (*buf != 0)
                    strcpy(originFh->title, buf);
            }
        }
        if (flag & FILE_IMPORT_FLAG) {
            if (data->accessed[0] & FILE_IMPORTED)
                originFh->accessed[0] |= FILE_IMPORTED;
            else
                originFh->accessed[0] &= ~FILE_IMPORTED;
        }
#ifdef FILTER
        if (flag & FILE_CENSOR_FLAG) {
            ret = pass_filter(originFh, board, session);

            if (!ret && !strcmp(board->filename, FILTER_BOARD)){
                char ans[STRLEN];
                snprintf(ans, STRLEN, "��%s������: %s", session->currentuser->userid, fileinfo->title);
                strnzhcpy(originFh->title, ans, ARTICLE_TITLE_LEN);
            }
        }
#endif
        if (flag & FILE_ATTACHPOS_FLAG) {
            originFh->attachment = data->attachment;
        }
        if (flag & FILE_DING_FLAG) {
            ret = add_top(originFh, board->filename, data->accessed[0]);
        }
        if (flag & FILE_EFFSIZE_FLAG) {
            originFh->eff_size = data->eff_size;
        }
    }
    if ((currmode != DIR_MODE_NORMAL) && (currmode != DIR_MODE_DIGEST)) {
        /*
         * ��Ҫ����.DIR�ļ�
         */
        char dirpath[MAXPATH];
        struct write_dir_arg dotdirarg;

        init_write_dir_arg(&dotdirarg);
        setbdir(DIR_MODE_NORMAL, dirpath, board->filename);
        dotdirarg.filename = dirpath;
        change_post_flag(&dotdirarg, DIR_MODE_NORMAL, board, originFh, flag, data, false, session);
        free_write_dir_arg(&dotdirarg);
    }
    if (dirarg->needlock)
        flock(dirarg->fd, LOCK_UN);
    return ret;
}

/* etnlegend - �޸ĸ������� */
long ea_dump(int fd_src,int fd_dst,long offset){
    char buf[2048];
    long length,ret,len;
    void *p;
    if(fd_src==fd_dst||offset<0)
        return -1;
    if(ftruncate(fd_dst,offset)==-1)
        return -1;
    lseek(fd_src,offset,SEEK_SET);
    lseek(fd_dst,offset,SEEK_SET);
    length=0;
    do{                                             //�����ļ�
        if((ret=read(fd_src,buf,2048*sizeof(char)))>0)
            for(p=buf,len=ret;len>0&&ret!=-1;vpm(p,ret),len-=ret)
                length+=(ret=write(fd_dst,p,len));
    }while(ret>0);
    if(ret==-1)
        return -1;
    return length;
}
static long ea_parse(int fd,struct ea_attach_info *ai){
    char buf[8],c;
    int count,n;
    long ret,length,end;
    unsigned int size;
    if(!ai)
        return -2;
    bzero(ai,MAXATTACHMENTCOUNT*sizeof(struct ea_attach_info));
    count=0;length=lseek(fd,0,SEEK_CUR);
    end=lseek(fd,0,SEEK_END);lseek(fd,length,SEEK_SET);
    do{
        if((ret=read(fd,buf,ATTACHMENT_SIZE*sizeof(char)))>0){
            if(ret==ATTACHMENT_SIZE*sizeof(char)){
                if(!memcmp(buf,ATTACHMENT_PAD,ATTACHMENT_SIZE*sizeof(char))){
                    ai[count].offset=length;        //��ǰ��������ʼλ��
                    n=0;
                    do{                             //��ǰ�����ļ�����
                        if((ret=read(fd,&c,sizeof(char)))>0){
                            if(ret==sizeof(char)){
                                ai[count].name[n++]=c;
                                if(!c||n>60)        //������ﵽ�����ļ����Ƴ������� /* TODO: filenamename length - atppp*/
                                    break;
                            }
                            else{
                                if(lseek(fd,0,SEEK_CUR)==end)
                                    break;
                                else
                                    lseek(fd,-ret,SEEK_CUR);
                            }
                        }
                    }while(ret>0);
                    if(ret==-1||!ret)
                        break;
                    do{                             //��ǰ�����ļ�����
                        if((ret=read(fd,&size,sizeof(unsigned int)))>0){
                            if(ret==sizeof(unsigned int)){
                                ai[count].size=ntohl(size);
                                ai[count].length=((ATTACHMENT_SIZE+n)*sizeof(char)+
                                    sizeof(unsigned int)+ai[count].size);
                                break;
                            }
                            else{
                                if(lseek(fd,0,SEEK_CUR)==end)
                                    break;
                                else
                                    lseek(fd,-ret,SEEK_CUR);
                            }
                        }
                    }while(ret>0);
                    if(ret==-1||(length+ai[count].length)>end)
                        break;
                    length=lseek(fd,ai[count].size,SEEK_CUR);count++;
                    if(!(count<MAXATTACHMENTCOUNT))
                        break;
                }
                else
                    break;
            }
            else{
                if(lseek(fd,0,SEEK_CUR)==end)
                    break;
                else
                    lseek(fd,-ret,SEEK_CUR);
            }
        }
    }while(ret>0);
    if(ret==-1)
        return -1;
    return length;
}
long ea_locate(int fd,struct ea_attach_info *ai){
    char c;
    int n;
    long ret,offset,end;
    end=lseek(fd,0,SEEK_END);
    n=0;lseek(fd,0,SEEK_SET);
    do{                                             //��λ������ʶ ATTACHMENT_PAD
        if((ret=read(fd,&c,sizeof(char)))>0){
            if(ret==sizeof(char)){
                if(c==ATTACHMENT_PAD[n])
                    n++;
                else
                    n=0;
            }
            else{
                if(lseek(fd,0,SEEK_CUR)==end)
                    break;
                else
                    lseek(fd,-ret,SEEK_CUR);
            }
        }
    }while(ret>0&&n<ATTACHMENT_SIZE*sizeof(char));
    if(ret==-1)
        return -1;
    offset=lseek(fd,-(n*sizeof(char)),SEEK_CUR);
    ret=ea_parse(fd,ai);
    if(ret==-1)
        return -1;
    if(!(ret<0)&&ret!=end&&ftruncate(fd,ret)==-1)
        return -1;
    return offset;
}
static long ea_append_helper(int fd,struct ea_attach_info *ai,const char *fn,const char *original_filename){
    char buf[2048];
    const char *pos1, *pos2;
    int fd_recv,count;
    unsigned int size;
    long ret,len,end;
    void *p;
    if(!ai)
        return -1;
    for(count=0;count<MAXATTACHMENTCOUNT&&ai[count].name[0];count++)
        continue;
    if(count==MAXATTACHMENTCOUNT)                         //�Ѵﵽ��������
        return -1;
    if((fd_recv=open(fn,O_RDONLY,0644))==-1)
        return -1;
    flock(fd_recv,LOCK_SH);


    pos1 = strrchr(original_filename, '\\');
    pos2 = strrchr(original_filename, '/');
    if (pos1 && pos2) {
        if (pos1 < pos2) pos1 = pos2;
        original_filename = pos1 + 1;
    } else {
        pos1 = pos1 ? pos1 : pos2;
        if (pos1) original_filename = pos1 + 1;
    }
    len = strlen(original_filename);
    if (!len)
        return -1;
    if (len > 60)
        original_filename += (len-60);
    strcpy(ai[count].name,original_filename);
    filter_upload_filename(ai[count].name);

    end=lseek(fd_recv,0,SEEK_END);ai[count].size=(unsigned int)end;
    ai[count].length=((ATTACHMENT_SIZE+strlen(ai[count].name)+1)*sizeof(char)
        +sizeof(unsigned int)+ai[count].size);
    lseek(fd_recv,0,SEEK_SET);ai[count].offset=lseek(fd,0,SEEK_END);
    ret=0;
    for(p=ATTACHMENT_PAD,len=ATTACHMENT_SIZE*sizeof(char);len>0&&ret!=-1;vpm(p,ret),len-=ret)
        ret=write(fd,p,len);                        //д�븽����ʶ ATTACHMENT_PAD
    for(p=ai[count].name,len=(strlen(ai[count].name)+1)*sizeof(char);len>0&&ret!=-1;vpm(p,ret),len-=ret)
        ret=write(fd,p,len);                        //д�븽���ļ�����
    for(size=htonl(end),p=&size,len=sizeof(unsigned int);len>0&&ret!=-1;vpm(p,ret),len-=ret)
        ret=write(fd,p,len);                        //д�븽���ļ���С(�����ֽ���)
    while(ret>0){                                   //д�븽���ļ�����
        if((ret=read(fd_recv,buf,2048*sizeof(char)))>0)
            for(p=buf,len=ret;len>0&&ret!=-1;vpm(p,ret),len-=ret)
                ret=write(fd,p,len);
    }
    len=lseek(fd_recv,0,SEEK_CUR);
    close(fd_recv);
    if(ret==-1||len!=end){
        bzero(&ai[count],sizeof(struct ea_attach_info));
        if(ftruncate(fd,ai[count].offset)==-1)
            return -2;
        return -1;
    }
    return end;
}
long ea_append(int fd,struct ea_attach_info *ai,const char *fn,const char *original_filename){
    long ret = ea_append_helper(fd,ai,fn,original_filename);
    unlink(fn);
    return(ret);
}
long ea_delete(int fd,struct ea_attach_info *ai,int pos){
    int count,n;
    long ret,end;
    void *p;
    if(!ai)
        return -1;
    for(count=0;count<MAXATTACHMENTCOUNT&&ai[count].name[0];count++)
        continue;
    if(!count)                                      //�޸���
        return -1;
    if(!(pos>0)||pos>count)                         //��������
        return -1;
    ret=ai[pos-1].size;
    if(pos==count){                                 //���λ�õĸ���
        if(ftruncate(fd,ai[pos-1].offset)==-1)
            return -1;
        bzero(&ai[pos-1],sizeof(struct ea_attach_info));
    }
    else{                                           //����λ�õĸ���
        end=lseek(fd,0,SEEK_END);
        p=mmap(NULL,end,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        if(p==(void*)-1)
            return -1;
        memmove(vpo(p,ai[pos-1].offset),vpo(p,ai[pos].offset),(end-ai[pos].offset));
        munmap(p,end);
        if(ftruncate(fd,end-ai[pos-1].length)==-1)
            return -2;
        for(n=pos;n<count;n++)
            ai[n].offset-=ai[pos-1].length;
        memmove(&ai[pos-1],&ai[pos],(count-pos)*sizeof(struct ea_attach_info));
        bzero(&ai[count-1],sizeof(struct ea_attach_info));
    }
    return ret;
}
/* �޸ĸ������Ľ��� */




int getattachtmppath(char *buf, size_t buf_len, session_t *session)
{
#if ! defined(FREE)
    /* setcachehomefile() ������ buf_len ��������ֱ����ôд�� */
    snprintf(buf,buf_len,"%s/home/%c/%s/%d/upload",TMPFSROOT,toupper(session->currentuser->userid[0]),
			session->currentuser->userid, session->utmpent);
#else
    snprintf(buf,buf_len,"%s/%s_%d",ATTACHTMPPATH,session->currentuser->userid,  session->utmpent);
#endif
    return 0;
}

int upload_post_append(FILE *fp, struct fileheader *post_file, session_t *session)
{
    char buf[256];
    char attachdir[MAXPATH], attachfile[MAXPATH];
    FILE *fp2;
    int fd, n=0;

    getattachtmppath(attachdir, MAXPATH, session);
    snprintf(attachfile, MAXPATH, "%s/.index", attachdir);
    if ((fp2 = fopen(attachfile, "r")) != NULL) {
        fputs("\n", fp);
        while (fgets(buf, 256, fp2)) {
            char *name;
            long begin = 0;
            unsigned int save_size;
            char *ptr;
            off_t size;

            name = strchr(buf, ' ');
            if (name == NULL)
                continue;
            *name = 0;
            name++;
            ptr = strchr(name, '\n');
            if (ptr)
                *ptr = 0;

            if (-1 == (fd = open(buf, O_RDONLY)))
                continue;
            if (post_file->attachment == 0) {
                post_file->attachment = ftell(fp) + 1;
            }
            fwrite(ATTACHMENT_PAD, ATTACHMENT_SIZE, 1, fp);
            fwrite(name, strlen(name) + 1, 1, fp);
            BBS_TRY {
                if (safe_mmapfile_handle(fd,  PROT_READ, MAP_SHARED, &ptr, & size) == 0) {
                    size = 0;
                    save_size = htonl(size);
                    fwrite(&save_size, sizeof(save_size), 1, fp);
                } else {
                    save_size = htonl(size);
                    fwrite(&save_size, sizeof(save_size), 1, fp);
                    begin = ftell(fp);
                    fwrite(ptr, size, 1, fp);
                    n++;
                }
            }
            BBS_CATCH {
                ftruncate(fileno(fp), begin + size);
                fseek(fp, begin + size, SEEK_SET);
            }
            BBS_END;
            end_mmapfile((void *) ptr, size, -1);

            close(fd);
        }
		fclose(fp2);
    }
    f_rm(attachdir);
    return n;
}

int upload_read_fileinfo(struct ea_attach_info *ai, session_t *session) {
    char buf[256];
    char attachdir[MAXPATH], attachfile[MAXPATH];
    FILE *fp2;
    int n=0;
    struct stat stat_buf;

    bzero(ai,MAXATTACHMENTCOUNT*sizeof(struct ea_attach_info));
    getattachtmppath(attachdir, MAXPATH, session);
    snprintf(attachfile, MAXPATH, "%s/.index", attachdir);
    if ((fp2 = fopen(attachfile, "r")) != NULL) {
        while (fgets(buf, 256, fp2)) {
            char *name;
            char *ptr;

            name = strchr(buf, ' ');
            if (name == NULL)
                continue;
            *name = 0;
            name++;
            ptr = strchr(name, '\n');
            if (ptr)
                *ptr = 0;

            strncpy(ai[n].name, name, 60);
            ai[n].name[60] = '\0';

            if (stat(buf, &stat_buf) != -1 && S_ISREG(stat_buf.st_mode)) {
                ai[n].size = stat_buf.st_size;
            } else {
                ai[n].size = 0;
            }

            n++;
            if (n >= MAXATTACHMENTCOUNT) break;
        }
		fclose(fp2);
    }
    return n;

}

int upload_del_file(const char *original_file, session_t *session) {
    char buf[256];
    char attachdir[MAXPATH], attachfile[MAXPATH], attachfile2[MAXPATH];
    FILE *fp, *fp2;
    int ret = -2;

    getattachtmppath(attachdir, MAXPATH, session);
    snprintf(attachfile, MAXPATH, "%s/.index", attachdir);
    snprintf(attachfile2, MAXPATH, "%s/.index2", attachdir);
    if ((fp = fopen(attachfile2, "w")) == NULL) {
        return -1;
    }
    if ((fp2 = fopen(attachfile, "r")) == NULL) {
        fclose(fp);
        return -1;
    }
    while (fgets(buf, 256, fp2)) {
        char *name;
        char *ptr;

        name = strchr(buf, ' ');
        if (name == NULL)
            continue;
        *name = 0;
        name++;
        ptr = strchr(name, '\n');
        if (ptr)
            *ptr = 0;

        if (strcmp(original_file, name)) {
            fprintf(fp, "%s %s\n", buf, name);
            continue;
        }

        ret = 0;
        unlink(buf);
    }
	fclose(fp2);
    fclose(fp);
    f_mv(attachfile2, attachfile);
    return ret;
}

static int upload_add_file_helper(const char *filename, char *original_filename, session_t *session) {
    struct ea_attach_info ai[MAXATTACHMENTCOUNT];
    char attachdir[MAXPATH], attachfile[MAXPATH];
    FILE *fp;
    char buf[256];
    int i, n, len;
    int totalsize=0;
    char *pos1, *pos2;
    struct stat stat_buf;

    n = upload_read_fileinfo(ai, session);
    if (n >= MAXATTACHMENTCOUNT)
        return -2;

    pos1 = strrchr(original_filename, '\\');
    pos2 = strrchr(original_filename, '/');
    if (pos1 && pos2) {
        if (pos1 < pos2) pos1 = pos2;
        original_filename = pos1 + 1;
    } else {
        pos1 = pos1 ? pos1 : pos2;
        if (pos1) original_filename = pos1 + 1;
    }
    len = strlen(original_filename);
    if (!len)
        return -3;
    if (len > 60)
        original_filename += (len-60);
    filter_upload_filename(original_filename);

    for (i=0;i<n;i++) {
        if (strcmp(ai[i].name, original_filename) == 0) return -4;
        totalsize+=ai[i].size;
    }
    if (stat(filename, &stat_buf) != -1 && S_ISREG(stat_buf.st_mode)) {
        totalsize += stat_buf.st_size;
    } else {
        return -5;
    }
    if (!HAS_PERM(session->currentuser, PERM_SYSOP) && totalsize > MAXATTACHMENTSIZE) return -6;
    
    getattachtmppath(attachdir, MAXPATH, session);
    mkdir(attachdir, 0700);

    snprintf(attachfile, MAXPATH, "%s/%d_%d", attachdir, (int)(time(NULL)%10000), rand()%10000);
    f_mv(filename, attachfile);

    snprintf(buf, sizeof(buf), "%s %s\n", attachfile, original_filename);

    snprintf(attachfile, MAXPATH, "%s/.index", attachdir);
    if ((fp = fopen(attachfile, "a")) == NULL)
        return -1;
    fprintf(fp, "%s", buf);
    fclose(fp);
    return(0);
}

int upload_add_file(const char *filename, char *original_filename, session_t *session) {
    int ret = upload_add_file_helper(filename, original_filename, session);
    if (ret) unlink(filename);
    return(ret);
}

/* etnlegend, 2006.04.16, ����ɾ������ */

#define DRBP_MODE       0644
#define DRBP_LEN        _POSIX_PATH_MAX

int cancel_inn(const char *board,const struct fileheader *file){
    FILE *fp,*fp_bntp;
    char buf[DRBP_LEN],nick[DRBP_LEN],*p;
    int len;
    setbfile(buf,board,file->filename);
    if(!(fp=fopen(buf,"r")))
        return 1;
    if(!(fp_bntp=fopen("innd/cancel.bntp","a"))){
        fclose(fp);
        return 2;
    }
    nick[0]=0;
    while(skip_attach_fgets(buf,DRBP_LEN,fp)){
        buf[len=(strlen(buf)-1)]=0;
        if(len<8)
            break;
        if(!strncmp(buf,"������: ",8)){
            if(!(p=strrchr(buf,')')))
                break;
            *p=0;
            if(!(p=strchr(buf,'(')))
                break;
            snprintf(nick,DRBP_LEN,"%s",&p[1]);
            break;
        }
    }
    snprintf(buf,DRBP_LEN,"%s\t%s\t%s\t%s\t%s\n",board,file->filename,file->owner,nick,file->title);
    fputs(buf,fp_bntp);
    fclose(fp);
    fclose(fp_bntp);
    return 0;
}

static inline int delete_range_cancel_post_mv(const char *board,struct fileheader *file){
    char buf[DRBP_LEN],obuf[DRBP_LEN],nbuf[DRBP_LEN];
    time_t currtime;
    setbfile(obuf,board,file->filename);
    !(file->filename[1]=='/')?(file->filename[0]='D'):(file->filename[2]='D');
    setbfile(nbuf,board,file->filename);
    currtime=time(NULL);
    if(file->innflag[0]=='S'&&file->innflag[1]=='S'&&(get_posttime(file)>(currtime-(14*86400))))
        cancel_inn(board,file);
    file->accessed[sizeof(((const struct fileheader*)NULL)->accessed)-1]=((currtime/86400)%100);
    if(getCurrentUser()){
        snprintf(buf,ARTICLE_TITLE_LEN,"%-32.32s - %s",file->title,getCurrentUser()->userid);
        snprintf(file->title,ARTICLE_TITLE_LEN,"%s",buf);
    }
    return f_mv(obuf,nbuf);
}

static inline int delete_range_cancel_post_del(const char *board,struct fileheader *file){
    char buf[DRBP_LEN];
    setbfile(buf,board,file->filename);
    if(file->innflag[0]=='S'&&file->innflag[1]=='S'&&(get_posttime(file)>(time(NULL)-(14*86400))))
        cancel_inn(board,file);
    return unlink(buf);
}

static inline int delete_range_cancel_mail_mv(const char *user,struct fileheader *file){
    return 0;
}

static inline int delete_range_cancel_mail_del(const char *user,struct fileheader *file){
    struct stat st;
    char buf[DRBP_LEN];
    int ret;
    setmailfile(buf,user,file->filename);
    if(lstat(buf,&st)==-1||!S_ISREG(st.st_mode)||st.st_nlink>1)
        ret=0;
    else
        ret=(int)st.st_size;
    unlink(buf);
    return ret;
}

int delete_range_base(
    const char *videntity,              /* ���������ʶ, ���������ƻ��û����� */
    const char *vdir_src,               /* Դ DIR ����, Ϊ NULL ʱĬ��Ϊ DOT_DIR �����õ�ֵ */
    const char *vdir_dst,               /* Ŀ�� DIR ����, Ϊ NULL ʱֱ��ɾ���ļ� */
    int vid_from,                       /* ���������»��ż�λ�� */
    int vid_to,                         /* ����β���»��ż�λ�� */
    int vmode,                          /* ����ģʽ, DELETE_RANGE_BASE_MODE_ ϵ�к��λ�� */
    int (*func)(
        const char*,
        struct fileheader*
    ),                                  /* ����Ҫ����������ִ�еĸ��Ӳ���, Ϊ NULL ʱִ��Ĭ�ϲ��� */
    const struct stat *vst_src          /* ����У���ļ��޸ĵ�Դ DIR ���ļ�״̬����, Ϊ NULL ʱ������У�� */
){
#define DRBP_CSRC       ((vmode&DELETE_RANGE_BASE_MODE_CHECK)&&vst_src)
#define DRBP_MAIL       (vmode&DELETE_RANGE_BASE_MODE_MAIL)
#define DRBP_DST        (vdir_dst&&(*vdir_dst)&&!(vmode&(DELETE_RANGE_BASE_MODE_MPDEL|DELETE_RANGE_BASE_MODE_CLEAR)))
#define DRBP_RSRC       do{munmap(pm,st_src.st_size);fcntl(fd_src,F_SETLKW,&lck_clr);close(fd_src);}while(0)
#define DRBP_RDST       do{fcntl(fd_dst,F_SETLKW,&lck_clr);close(fd_dst);}while(0)
#ifndef DELETE_RANGE_RESERVE_DIGEST
#define DRBP_MARKS(f)   ((f)->accessed[0]&(FILE_MARKED|FILE_PERCENT))
#else /* DELETE_RANGE_RESERVE_DIGEST */
#define DRBP_MARKS(f)   ((f)->accessed[0]&(FILE_MARKED|FILE_PERCENT|FILE_DIGEST))
#endif /* DELETE_RANGE_RESERVE_DIGEST */
#define DRBP_TOKEN(f)   ((f)->accessed[1]&FILE_DEL)
#define DRBP_CHK_T      (vmode&DELETE_RANGE_BASE_MODE_TOKEN)
#define DRBP_CHK_F      (DELETE_RANGE_BASE_MODE_TOKEN|DELETE_RANGE_BASE_MODE_MPDEL)
#define DRBP_CHK_M      (!((vmode&DELETE_RANGE_BASE_MODE_OVERM)&&(vmode&DRBP_CHK_F)))
#define DRBP_UNDEL(f)   ((DRBP_CHK_T&&!DRBP_TOKEN(f))||(DRBP_CHK_M&&DRBP_MARKS(f)))
#define DRBP_TSET(n)    do{tab[((n)>>3)]|=(1<<((n)&0x07));}while(0)
#define DRBP_TGET(n)    (tab[((n)>>3)]&(1<<((n)&0x07)))
    static const struct flock lck_set={.l_type=F_WRLCK,.l_whence=SEEK_SET,.l_start=0,.l_len=0,.l_pid=0};
    static const struct flock lck_clr={.l_type=F_UNLCK,.l_whence=SEEK_SET,.l_start=0,.l_len=0,.l_pid=0};
    struct userec *user;
    struct fileheader *src,*dst;
    struct stat st_src,st_dst;
    char path_src[DRBP_LEN],path_dst[DRBP_LEN];
    unsigned char *tab;
    int fd_src,fd_dst,count,total,reserved,id_from,id_to,i,j,n_src,n_dst,ret,len;
    void *pm,*p;
    /* �Ϸ��Լ�� */
    if(!videntity||!*videntity)
        return 0x10;
    if(!vdir_src||!*vdir_src)
        vdir_src=DOT_DIR;
    if(!(vid_from>0))
        return 0x11;
    /* ����Դ DIR �ļ� */
    !DRBP_MAIL?setbfile(path_src,videntity,vdir_src):setmailfile(path_src,videntity,vdir_src);
    if(stat(path_src,&st_src)==-1||!S_ISREG(st_src.st_mode))
        return 0x20;
    if(DRBP_CSRC&&(st_src.st_mtime>vst_src->st_mtime))      /* ���Դ DIR �ļ����ݸ��� */
        return 0x21;
    if((fd_src=open(path_src,O_RDWR,DRBP_MODE))==-1)
        return 0x22;
    if(fcntl(fd_src,F_SETLKW,&lck_set)==-1){
        close(fd_src);
        return 0x23;
    }
    if((pm=mmap(NULL,st_src.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_src,0))==MAP_FAILED){
        fcntl(fd_src,F_SETLKW,&lck_clr);
        close(fd_src);
        return 0x24;
    }
    /* ����Դ���� */
    total=st_src.st_size/sizeof(struct fileheader);
    id_from=(vid_from-1);
    id_to=((!(vid_to>total)?vid_to:total)-1);
    if(!((count=((id_to-id_from)+1))>0)){
        DRBP_RSRC;
        return 0x30;
    }
    reserved=((total-count)-id_from);
    src=(struct fileheader*)pm;
    src+=id_from;
    /* ����Ŀ�� DIR �ļ� */
    if(DRBP_DST){
        !DRBP_MAIL?setbfile(path_dst,videntity,vdir_dst):setmailfile(path_dst,videntity,vdir_dst);
        if(stat(path_dst,&st_dst)==-1){
            if(errno==ENOENT)
                st_dst.st_size=0;
            else{
                DRBP_RSRC;
                return 0x40;
            }
        }
        else{
            if(!S_ISREG(st_dst.st_mode)){
                DRBP_RSRC;
                return 0x41;
            }
        }
        if((fd_dst=open(path_dst,O_WRONLY|O_CREAT|O_APPEND,DRBP_MODE))==-1){
            DRBP_RSRC;
            return 0x42;
        }
        if(fcntl(fd_dst,F_SETLKW,&lck_set)==-1){
            DRBP_RSRC;
            close(fd_dst);
            return 0x43;
        }
    }
    else
        fd_dst=-1;
    /* �������β��� */
    switch(vmode&DELETE_RANGE_BASE_MODE_OPMASK){
        case DELETE_RANGE_BASE_MODE_TOKEN:                  /* ɾ����ɾ���� */
        case DELETE_RANGE_BASE_MODE_RANGE:                  /* ��������ɾ�� */
            /* ���������� */
            if(!(tab=(unsigned char*)malloc(((count>>3)+1)*sizeof(unsigned char)))){
                DRBP_RDST;
                DRBP_RSRC;
                return 0x80;
            }
            memset(tab,0,((count>>3)+1)*sizeof(unsigned char));
            /* ����Ŀ�� DIR ���� */
            if(DRBP_DST){
                if(!(dst=(struct fileheader*)malloc(count*sizeof(struct fileheader)))){
                    DRBP_RDST;
                    DRBP_RSRC;
                    free(tab);
                    return 0x81;
                }
            }
            else
                dst=NULL;
            /* �����������б�� */
            if(!func){
                if(!DRBP_MAIL){
                    if(DRBP_DST)
                        for(i=0;i<count;i++){
                            if(DRBP_UNDEL(&src[i]))
                                continue;
                            delete_range_cancel_post_mv(videntity,&src[i]);
                            DRBP_TSET(i);
                        }
                    else
                        for(i=0;i<count;i++){
                            if(DRBP_UNDEL(&src[i]))
                                continue;
                            delete_range_cancel_post_del(videntity,&src[i]);
                            DRBP_TSET(i);
                        }
                }
                else{
                    if(DRBP_DST)
                        for(i=0;i<count;i++){
                            if(DRBP_UNDEL(&src[i]))
                                continue;
                            delete_range_cancel_mail_mv(videntity,&src[i]);
                            DRBP_TSET(i);
                        }
                    else{
                        for(ret=0,i=0;i<count;i++){
                            if(DRBP_UNDEL(&src[i]))
                                continue;
                            ret+=delete_range_cancel_mail_del(videntity,&src[i]);
                            DRBP_TSET(i);
                        }
                        if(getuser(videntity,&user)){
                            if(!(ret>user->usedspace))
                                user->usedspace-=ret;
                            else
                                get_mailusedspace(user,1);
                        }
                    }
                }
            }
            else
                for(i=0;i<count;i++){
                    if(!func(videntity,&src[i]))
                        continue;
                    DRBP_TSET(i);
                }
            /* ����Ŀ�� DIR �ļ�д�� */
            if(DRBP_DST){
                /* �Ż����ƴ��������ݸ��� */
                for(n_dst=0,i=0,j=0;i<count;i++){
                    if(DRBP_TGET(i)){
                        j++;
                        continue;
                    }
                    if(j){
                        memcpy(&dst[n_dst],&src[i-j],j*sizeof(struct fileheader));
                        n_dst+=j;
                        j=0;
                    }
                }
                if(j){                                      /* ���һ����Ҫ���Ƶ����� */
                    memcpy(&dst[n_dst],&src[count-j],j*sizeof(struct fileheader));
                    n_dst+=j;
                }
                /* д������ */
                for(p=dst,len=n_dst*sizeof(struct fileheader),ret=0;len>0&&ret!=-1;vpm(p,ret),len-=ret)
                    ret=write(fd_dst,p,len);
                if(ret==-1){
                    ftruncate(fd_dst,st_dst.st_size);
                    DRBP_RDST;
                    DRBP_RSRC;
                    free(tab);
                    free(dst);
                    return 0x60;
                }
            }
            DRBP_RDST;
            /* ����Դ DIR �ļ�д�� */
            for(n_src=0,i=0,j=0;i<count;i++){
                if(!DRBP_TGET(i)){
                    j++;
                    src[i].accessed[1]&=~FILE_DEL;
                    continue;
                }
                if(j){
                    memmove(&src[n_src],&src[i-j],j*sizeof(struct fileheader));
                    n_src+=j;
                    j=0;
                }
            }
            if(j){
                memmove(&src[n_src],&src[count-j],j*sizeof(struct fileheader));
                n_src+=j;
            }
            memmove(&src[n_src],&src[count],reserved*sizeof(struct fileheader));
            /* ȷ���ļ����Ȳ�ͬ��ӳ������ */
            if(ftruncate(fd_src,(st_src.st_size-(count-n_src)*sizeof(struct fileheader)))==-1){
                DRBP_RSRC;
                free(tab);
                free(dst);
                return 0x61;
            }
            if(msync(pm,(st_src.st_size-(count-n_src)*sizeof(struct fileheader)),MS_SYNC)==-1){
                DRBP_RSRC;
                free(tab);
                free(dst);
                return 0x62;
            }
            DRBP_RSRC;
            free(tab);
            free(dst);
            return 0x00;
        case DELETE_RANGE_BASE_MODE_FORCE:                  /* ǿ������ɾ�� */
            if(!func){
                if(!DRBP_MAIL){
                    if(DRBP_DST)
                        for(i=0;i<count;i++)
                            delete_range_cancel_post_mv(videntity,&src[i]);
                    else
                        for(i=0;i<count;i++)
                            delete_range_cancel_post_del(videntity,&src[i]);
                }
                else{
                    if(DRBP_DST)
                        for(i=0;i<count;i++)
                            delete_range_cancel_mail_mv(videntity,&src[i]);
                    else{
                        for(ret=0,i=0;i<count;i++)
                            ret+=delete_range_cancel_mail_del(videntity,&src[i]);
                        if(getuser(videntity,&user)){
                            if(!(ret>user->usedspace))
                                user->usedspace-=ret;
                            else
                                get_mailusedspace(user,1);
                        }
                    }
                }
            }
            else
                for(i=0;i<count;i++)
                    func(videntity,&src[i]);
            if(DRBP_DST){
                for(p=src,len=count*sizeof(struct fileheader),ret=0;len>0&&ret!=-1;vpm(p,ret),len-=ret)
                    ret=write(fd_dst,p,len);
                if(ret==-1){
                    ftruncate(fd_dst,st_dst.st_size);
                    DRBP_RDST;
                    DRBP_RSRC;
                    return 0x70;
                }
            }
            DRBP_RDST;
            memmove(src,&src[count],reserved*sizeof(struct fileheader));
            if(ftruncate(fd_src,(st_src.st_size-count*sizeof(struct fileheader)))==-1){
                DRBP_RSRC;
                return 0x71;
            }
            if(msync(pm,(st_src.st_size-count*sizeof(struct fileheader)),MS_SYNC)==-1){
                DRBP_RSRC;
                return 0x72;
            }
            DRBP_RSRC;
            return 0x00;
        case DELETE_RANGE_BASE_MODE_MPDEL:                  /* ���α����ɾ */
            for(i=0;i<count;i++)
                !DRBP_UNDEL(&src[i])?(src[i].accessed[1]|=FILE_DEL):(src[i].accessed[1]&=~FILE_DEL);
            DRBP_RDST;
            DRBP_RSRC;
            return 0x00;
        case DELETE_RANGE_BASE_MODE_CLEAR:                  /* ���������ɾ */
            for(i=0;i<count;i++)
                src[i].accessed[1]&=~FILE_DEL;
            DRBP_RDST;
            DRBP_RSRC;
            return 0x00;
        default:
            DRBP_RDST;
            DRBP_RSRC;
            return 0x90;
    }
#undef DRBP_CSRC
#undef DRBP_MAIL
#undef DRBP_DST
#undef DRBP_RSRC
#undef DRBP_RDST
#undef DRBP_MARKS
#undef DRBP_TOKEN
#undef DRBP_CHK_T
#undef DRBP_CHK_F
#undef DRBP_CHK_M
#undef DRBP_UNDEL
#undef DRBP_TSET
#undef DRBP_TGET
}

#undef DRBP_MODE
#undef DRBP_LEN

/* --END--, etnlegend, 2006.04.19, ����ɾ������ */


/* ����ظ���ͳ�ƣ�pig2532 */

#ifdef HAVE_REPLY_COUNT

struct modify_reply_arg {
    int value;
    int mode;
    struct fileheader *lastpost;
};

static int update_reply_count(int fd, fileheader_t* base, int ent, int total, bool match, void* arg) {
    if(match) {
        struct modify_reply_arg *marg;
        struct fileheader *fh;
        marg = (struct modify_reply_arg *)arg;
        fh = &base[ent - 1];
        if(marg->mode == 0)
            fh->replycount += marg->value;
        else
            fh->replycount = marg->value;
        if(marg->lastpost) {
            strcpy(fh->last_owner, marg->lastpost->owner);
            fh->last_posttime = get_posttime(marg->lastpost);
        }
    }
    return 0;
}

int modify_reply_count(const char* bname, int gid, int value, int mode, struct fileheader* lastpost) {
    char dirpath[PATHLEN];
    int fd;
    struct fileheader fh;
    struct modify_reply_arg arg;
	
    setbdir(DIR_MODE_NORMAL, dirpath, bname);
    fd = open(dirpath, O_RDWR, 0644);
    if(fd < 0)
        return 0;
	
    fh.id = gid;
    arg.value = value;
    arg.mode = mode;
    arg.lastpost = lastpost;
    mmap_dir_search(fd, &fh, update_reply_count, &arg);
    close(fd);
    
    setbdir(DIR_MODE_ORIGIN, dirpath, bname);
    fd = open(dirpath, O_RDWR, 0644);
    if(fd < 0)
        return 0;
	
    mmap_dir_search(fd, &fh, update_reply_count, &arg);
    close(fd);
    	
    return 0;
}

int refresh_reply_count(const char* bname, int gid) {
    int fd, count, i, replycount;
    struct stat buf;
    struct fileheader *ptr, *lastpost, lastfh;
    char *head, dirpath[PATHLEN];
    
    setbdir(DIR_MODE_NORMAL, dirpath, bname);
    fd = open(dirpath, O_RDWR, 0644);
    if(fd < 0)
        return 0;
    
    replycount = -1;
    lastpost = NULL;
    BBS_TRY {
        if(!safe_mmapfile_handle(fd, PROT_READ | PROT_WRITE, MAP_SHARED, &head, &buf.st_size)) {
            close(fd);
            return 0;
        }
        count = buf.st_size / sizeof(struct fileheader);
        ptr = (struct fileheader *)head;
        replycount = 0;
        for(i=0; i<count; i++) {
            if(ptr->groupid == gid) {
                replycount++;
                lastpost = ptr;
            }
            ptr++;
        }
	}
    BBS_CATCH {
    }
    BBS_END;
    
    if(lastpost)
        memcpy(&lastfh, lastpost, sizeof(struct fileheader));
    
    end_mmapfile((void *)head, buf.st_size, -1);
    close(fd);
    
    if(replycount >= 0)
        modify_reply_count(bname, gid, replycount, 1, &lastfh);
    
    return 0;
}

#endif /* HAVE_REPLY_COUNT */

