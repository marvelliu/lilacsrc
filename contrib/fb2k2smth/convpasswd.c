/*
 * This program converts .PASSWD from Firebird 2000 to smth_bbs
 */

#include "bbs.h"

#define OLD_PASSWDS_FILE "PASSWDS.BAK_FOR_TAR"
#define NEW_PASSWDS_FILE ".PASSWDS.NEW"
#undef printf
#undef perror

#define HAVE_BIRTHDAY 1
//#define CONV_PASS     1
typedef struct olduserec {      /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    time_t firstlogin;
    char lasthost[16];
    unsigned int numlogins;
    unsigned int numposts;
//    unsigned int medals;        /* ������ */
//    unsigned int money;         /* ��Ǯ */
//    unsigned int inbank;        /* ��� */
//    time_t banktime;            /* ����ʱ�� */
    char flags[2];
    char passwd[OLDPASSLEN];
    char username[NAMELEN];
    char ident[NAMELEN];
    char termtype[16];
    char reginfo[STRLEN - 16];
    unsigned userlevel;
    time_t lastlogin;
    time_t lastlogout;          /* �������ʱ�� */
    time_t stay;
    char realname[NAMELEN];
    char address[STRLEN];
    char email[STRLEN - 12];
    unsigned int nummails;
    time_t lastjustify;
    char gender;
    unsigned char birthyear;
    unsigned char birthmonth;
    unsigned char birthday;
    int signature;
    unsigned int userdefine;
    time_t notedate;
    int noteline;
} olduserec;

static void create_userdata(olduserec * olduser)
{
    struct userdata ud;
    char datafile[256];
    int fd;
    struct stat st;

    bzero(&ud, sizeof(ud));
    memcpy(ud.userid, olduser->userid, sizeof(ud.userid));
    memcpy(ud.realemail, olduser->reginfo, sizeof(ud.realemail));
    memcpy(ud.realname, olduser->realname, sizeof(ud.realname));
    memcpy(ud.address, olduser->address, sizeof(ud.address));
    memcpy(ud.email, olduser->email, sizeof(ud.email));
#ifdef HAVE_BIRTHDAY
    ud.gender = olduser->gender;
    ud.birthyear = olduser->birthyear;
    ud.birthmonth = olduser->birthmonth;
    ud.birthday = olduser->birthday;
#endif

    sethomepath(datafile, olduser->userid);
    if (stat(datafile, &st) == -1) {
        fprintf(stderr, "Warning: %s's home directory not found.\n", olduser->userid);
        fprintf(stderr, "         Creating it.\n");
        if (mkdir(datafile, 0755) == -1)
            return;
    } else {
        if (!(st.st_mode & S_IFDIR))
            return;
    }
    sethomefile(datafile, olduser->userid, USERDATA);
    if ((fd = open(datafile, O_WRONLY | O_CREAT, 0644)) < 0) {
        fprintf(stderr, "open %s failed.\n", datafile);
        return;
    }
    if (write(fd, &ud, sizeof(ud)) < 0)
        fprintf(stderr, "write %s failed.\n", datafile);
    close(fd);
}

static void convert_userec(struct olduserec *olduser, struct userec *user)
{
    memcpy(user->userid, olduser->userid, IDLEN + 2);
    printf("Tranfering %s ....\n", user->userid);
    user->flags = olduser->flags[0];


    if (user->userlevel & 0x0400000000)
	user->flags |= PCORP_FLAG;
    user->flags &= ~CLOAK_FLAG;
    user->flags &= ~GIVEUP_FLAG;

    
    user->title = 0;
    user->firstlogin = olduser->firstlogin;
    memcpy(user->lasthost, olduser->lasthost, 16);
    user->numlogins = olduser->numlogins;
    user->numposts = olduser->numposts;
#ifdef CONV_PASS
    memcpy(user->passwd, olduser->passwd, OLDPASSLEN);
    bzero(user->unused_padding, 2);
#endif
    memcpy(user->username, olduser->username, NAMELEN);
    bzero(user->club_read_rights, sizeof(user->club_read_rights));
    bzero(user->club_write_rights, sizeof(user->club_write_rights));
    bzero(user->md5passwd, sizeof(user->md5passwd));
    user->userlevel = olduser->userlevel;
   
    user->userlevel &= ~PERM_ADMIN;
    user->userlevel &= ~PERM_HORNOR;
    user->userlevel &= ~PERM_JURY;
    user->userlevel &= ~PERM_CHECKCD;
    user->userlevel &= ~PERM_SUICIDE;
    user->userlevel &= ~PERM_COLLECTIVE;
    user->userlevel &= ~PERM_DISS;
    user->userlevel &= ~PERM_DENYMAIL;
    
    user->lastlogin = olduser->lastlogin;
    user->stay = olduser->stay;
    user->signature = olduser->signature;
    user->userdefine[0] = olduser->userdefine;
 
    user->userdefine[0] |= DEF_LOGININFORM;
    user->userdefine[0] |= DEF_SHOWSCREEN;
    user->userdefine[0] |= DEF_TITLECOLOR;
    user->userdefine[0] |= DEF_UNREADMARK;
	user->userdefine[0] &= ~DEF_NOTMSGFRIEND;
    user->userdefine[0] |= DEF_USEGB;
    user->userdefine[0] |= DEF_CHCHAR;
    user->userdefine[0] |= DEF_IGNOREMSG;
    user->userdefine[0] &= ~DEF_SHOWDETAILUSERDATA;
    user->userdefine[0] &= ~DEF_SHOWREALUSERDATA;
    
    user->notedate = olduser->notedate;
    user->noteline = olduser->noteline;
    user->exittime = olduser->lastlogout;
    user->usedspace = 0;
    if (strcasecmp(user->userid, "new") == 0)
        return;
    create_userdata(olduser);
}

int main()
{
    struct olduserec *olduser = NULL;
    struct olduserec *ptr = NULL;
    struct userec user;
    int fd, fd2;
    struct stat fs;
    int records;
    int i;
    int allocid;

    chdir(BBSHOME);

    if ((fd = open(OLD_PASSWDS_FILE, O_RDONLY, 0644)) < 0) {
        perror("open");
        return -1;
    }
    if ((fd2 = open(NEW_PASSWDS_FILE, O_WRONLY | O_CREAT, 0644)) < 0) {
        perror("open");
        return -1;
    }
    fstat(fd, &fs);
    olduser = mmap(NULL, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (olduser == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    records = fs.st_size / sizeof(struct olduserec);
    for (i = 0; i < records; i++) {
        ptr = olduser + i;
        if (ptr->userid[0] == '\0')
            continue;
        bzero(&user, sizeof(user));
        convert_userec(ptr, &user);
        write(fd2, &user, sizeof(user));
    }
    close(fd2);
    munmap(olduser, fs.st_size);

    return 0;
}
