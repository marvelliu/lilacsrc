#ifndef __STRUCT_H__
#define __STRUCT_H__
/* Note the protocol field is not inside an #ifdef FILES...
   this is a waste but allows you to add/remove UL/DL support without
   rebuilding the PASSWDS file (and it's only a lil ole int anyway).
*/
#ifndef OWN_USEREC
struct userec {                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    char flags; /*һЩ��־����������������֮���*/
    unsigned char title; /*�û�����*/
    time_t firstlogin;
    char lasthost[IPLEN];
    unsigned int numlogins;
    unsigned int numposts;
#ifdef CONV_PASS
    char passwd[OLDPASSLEN];
    char unused_padding[2];
#endif
    char username[NAMELEN];
    unsigned int club_read_rights[MAXCLUB>>5];
    unsigned int club_write_rights[MAXCLUB>>5];
    unsigned char md5passwd[MD5PASSLEN];
#ifndef OS_64BIT
    unsigned int userlevel;
#endif
    time_t lastlogin;
    time_t stay;
#ifdef OS_64BIT /*  align 8 bytes... */
    unsigned int userlevel;
#endif
    int signature;
    unsigned int userdefine[2];
    time_t notedate;
    int noteline;
    int unused_atppp;
    time_t exittime;
    /* ��������ת�Ƶ� userdata �ṹ�� */
    unsigned int usedspace;     /* used space of user's mailbox, in bytes */

    int unused[5];
    int score_rank;
    int score;
};
#endif //OWN_USEREC

struct userdata
{
    char userid[IDLEN + 2];
    char __reserved[2];
    /*char username[NAMELEN];*/
    char realemail[STRLEN - 16];
    char realname[NAMELEN];
    char address[STRLEN];
    char email[STRLEN];
#ifdef HAVE_BIRTHDAY
    char            gender;
    unsigned char   birthyear;
    unsigned char   birthmonth;
    unsigned char   birthday;
#endif
    char reg_email[STRLEN]; /* registry email . added by binxun . 2003.6.6 */
/*#ifdef SMS_SUPPORT*/
    int mobileregistered;
    char mobilenumber[MOBILE_NUMBER_LEN];
/*#endif*/
/* add by roy 2003.07.23 for wbbs*/
    char OICQ[STRLEN];
    char ICQ[STRLEN];
    char MSN[STRLEN];
    char homepage[STRLEN];
    int userface_img;
    char userface_url[STRLEN];
    unsigned char userface_width;
    unsigned char userface_height;
    unsigned int group;
    char country[STRLEN];
    char province[STRLEN];
    char city[STRLEN];
    unsigned char shengxiao;
    unsigned char bloodtype;
    unsigned char religion;
    unsigned char profession;
    unsigned char married;
    unsigned char education;
    char graduateschool[STRLEN];
    unsigned char character;
    char photo_url[STRLEN];
    char telephone[STRLEN];
    char smsprefix[41];
    char smsend[41];
    unsigned int smsdef;
    int signum;
    int this_field_is_reserved_by_atppp;
    time_t lastinvite;
};

struct user_info {              /* Structure used in UTMP file */
    int active;                 /* When allocated this field is true */
    int uid;                    /* Used to find user name in passwd file */
    int pid;                    /* kill() to notify user of talk request */
    int invisible;              /* Used by cloaking function in Xyz menu */
    int sockactive;             /* Used to coordinate talk requests */
    int sockaddr;               /* ... */
    int destuid;                /* talk uses this to identify who called */
    int mode;                   /* UL/DL, Talk Mode, Chat Mode, ... */
    int pager;                  /* pager toggle, true, or false */
    int in_chat;                /* for in_chat commands   */
    char chatid[16];            /* chat id, if in chat mode */
    char from[IPLEN + 4];       /* machine name the user called in from */
    time_t logintime;
    int lastpost;
    char unused[32];

    time_t freshtime;
    int utmpkey;
    unsigned int mailbox_prop;  /* properties of getCurrentUser()'s mailbox */
    char userid[20];
    char realname[20];
    char username[40];
    int friendsnum;
    int friends_uid[MAXFRIENDS];
#ifdef FRIEND_MULTI_GROUP
    unsigned int friends_p[MAXFRIENDS];
#endif
    int currentboard;
    unsigned int mailcheck;             /* if have new mail or new msg, stiger */
};

struct usermemo {
    struct userdata ud;
};

struct friends {
    char id[13];
    char exp[LEN_FRIEND_EXP];
#ifdef FRIEND_MULTI_GROUP
    int groupid;
#endif
};
struct friends_info {
    char exp[LEN_FRIEND_EXP];
};
struct friends_group {
    char name[15];
    unsigned int p;
    char f;
    char b;
};

struct boardheader {            /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char BM[BM_LEN];
    char title[STRLEN];
    unsigned level;
    unsigned int idseq;
    unsigned int clubnum; /*����Ǿ��ֲ�������Ǿ��ֲ����*/
    unsigned int flag;
    union {
    unsigned int adv_club; /* ����ָ��club��Ӧ��ϵ,0��ʾû��,1��ʾ���*/
    unsigned int group_total; /*����Ƕ���Ŀ¼��Ӧ����Ŀ¼�İ�������*/
    } board_data;
    time_t createtime;
/*    int toptitle; move to board status */
    unsigned int score_level;
    char ann_path[128];
    int group; /*����Ŀ¼*/
    char title_level; /* �趨�û���Ҫʲôtitle�ɼ��������*/
    char des[195]; /*��������,����www�İ���˵����search */
#ifdef FLOWBANNER
    int bannercount;
    char banners[MAXBANNER][BANNERSIZE];
#endif
};

/* XXX: struct fileheader moved to site.h */

struct one_key {                /* Used to pass commands to the readmenu */
    int key;
    int (*fptr) ();
};

#define USHM_SIZE       (MAXACTIVE + 10)        /*modified by dong, 10->20, 1999.9.15 */
/* modified back by KCN,20->10, because not reboot */
#define UTMP_HASHSIZE  (USHM_SIZE*4)
struct UTMPFILE {
    struct user_info uinfo[USHM_SIZE];
};
struct BoardStatus {            /* use this to speed up board list */
    int total;
    int lastpost;
    int updatemark;
    int updatetitle;
    int updateorigin;
    int currentusers;
    int nowid;
    int toptitle;
    struct fileheader topfh[MAX_DING];
#ifdef HAVE_WFORUM
    int todaynum;
#endif
};
struct favbrd_struct {
    int bnum;
    int bid[MAXBOARDPERDIR];
    /* bid >= 0: ����
       bid < 0: Ŀ¼�� ��ʾ��Ŀ¼�� favbrd_list[-bid]
       */
    char title[61];
    char ename[20];
    int father;
    int level;
};

struct BCACHE {
    int numboards;
    struct BoardStatus bstatus[MAXBOARD];
};

struct BDIRCACHE {
    struct favbrd_struct allbrd_list[FAVBOARDNUM];
    int allbrd_list_t;
    struct favbrd_struct wwwbrd_list[FAVBOARDNUM];
    int wwwbrd_list_t;
};

struct posttop {
/*  char author[IDLEN + 1];      author name */
    char board[IDLEN + 6];  /*     board name */
/*  int bnum; */
/*  char title[66];              title name */
    unsigned int groupid;   /* article group id */
    time_t date;                /* last post's date */
    int number;                 /* post number */
};

struct public_data {
    time_t nowtime;
    int sysconfimg_version;
    int www_guest_count;
    unsigned int max_user;
    unsigned int max_wwwguest;

    /* etnlegend, 2006.03.06, userscore twice sampling for high score users ... */
    unsigned int us_sample_high[8];

    unsigned int logincount;
    unsigned int logoutcount;
    uint64_t staytime;
    unsigned int wwwlogincount;
    unsigned int wwwlogoutcount;
    unsigned int wwwguestlogincount;
    unsigned int wwwguestlogoutcount;
    uint64_t wwwstaytime;
    uint64_t wwwgueststaytime;

    /* etnlegend, 2006.03.06, userscore sampling ... */
    unsigned int us_sample[32];

    /* etnlegend, 2006.05.28, �Ķ�ʮ�� ... */
    unsigned int top_version;
    struct top_header{
        int bid;
        unsigned int gid;
    } top[10];

    char unused[712];

#ifdef FLOWBANNER
    int bannercount;
    char banners[MAXBANNER][BANNERSIZE];
#endif
    
#ifdef FB2KENDLINE
    time_t nextfreshdatetime;
    char date[60];
#endif

};


struct smenuitem {
    int line, col, level;
    char *name, *desc, *arg;
    char *func_name;
    /*
     * �ڴ���Ӧ����func����,��Ϊ��Ҫ���ǵ��޸�func�б�
     * ����ִ�еĳ���һ������� 
    */
};

struct bbs_msgbuf {
    long int mtype;
    time_t msgtime;
    pid_t pid;
    char userid[IDLEN];
    char mtext[1];
};

#ifdef NEWBMLOG
struct _new_bmlog
{
    char boardname[BOARDNAMELEN];
    int type;
    int value;
};
#endif

#ifdef NEWPOSTLOG
struct _new_postlog
{
    char boardname[BOARDNAMELEN];
    char title[81];
    int threadid;
};
#endif

struct _mail_list{
    char mail_list[MAILBOARDNUM][40];
    int mail_list_t;
} ;

typedef struct
{
    char group_name[8]; /** "groupXX\0", XX stands for 00 to 99 */
    unsigned int users_num;
    char group_desc[40];
    char __reserved[12];
}mailgroup_list_item;

typedef struct
{
    unsigned int groups_num;
    mailgroup_list_item groups[MAX_MAILGROUP_NUM];
}mailgroup_list_t;

typedef struct friends mailgroup_t;

struct msghead {
    int pos, len;
    char sent;
    char mode;
    char id[IDLEN+2];
    time_t time;
    int frompid, topid;
};

struct smsmsg {
    int id;
    char userid[13];
    char dest[13];
    char time[15];
    int type;
    int level;
    char *context;
    int readed;
};

struct addresslist{
    int id;
    char userid[13];
    char name[15];
    char bbsid[15];
    char school[100];
    char zipcode[7];
    char homeaddr[100];
    char companyaddr[100];
    char tel_o[20];
    char tel_h[20];
    char mobile[15];
    char email[30];
    char qq[10];
    int birth_year;
    int birth_month;
    int birth_day;
    char *memo;
    char group[10];
};

#define AL_ORDER_NAME 1
#define AL_ORDER_BBSID 2
#define AL_ORDER_GROUPNAME 3
#define AL_ORDER_COUNT 3

struct key_struct { // �Զ���� by bad
    int status[10];
    int key;
    int mapped[10];
};

#ifdef NEW_HELP
struct helps
{
    unsigned int id;
    int modeid;
    char index[11];
    char desc[41];
    char *content;
};
#endif

//�����ļ� by stiger
#ifdef PERSONAL_CORP

struct _pc_selusr
{
    char userid[IDLEN+2];
    char corpusname[41];
    time_t createtime;
};

struct pc_users {
    int uid;    //not uid in PASSWD, �Ǹ����ļ���uid
    char username[IDLEN+2];
    char corpusname[41];
    char description[201];
    char theme[11];
    int nodelimit;
    int dirlimit;
    time_t createtime;
};

struct pc_nodes {
    unsigned long nid;      //node id, auto increment

    //pid��Ϊ��:
    //   type: 0: �ղؼ�����, pid��ʾ�ղؼ�Ŀ¼nodes nid
    //   type: 1: �ղؼ�Ŀ¼, pid��ʾ���ղؼ�Ŀ¼nodes nid
    //pid == 0:
    //   type: 0: ��ͨ����
    //   type: 1: �ղؼи�Ŀ¼
    unsigned long pid;

    //type: 0: ����
    //      1: �ղؼ�Ŀ¼
    int type;

    char source[11];
    char hostname[21];
    time_t created;
    time_t changed;
    int uid;

    //�Ƿ���������
    // 0: ������
    // 1: �����¼�û�
    // 2: ����guest
    int comment;
    long commentcount;
    char subject[201];
    char *body;

    //Ȩ��
    // 0: ����
    // 1: ����
    // 2: ˽��
    // 3: �ղ�
    // 4: ����
    int access;
    int visitcount;
        int htmltag;
};

struct pc_comments {
    unsigned long cid;
    unsigned long nid;
    int uid;
    char hostname[21];
    char username[21];
    char subject[201];
    time_t created;
    time_t changed;
    char *body;
};

//add by windinsn, add blog logs into table LOGS
struct pc_logs {
    unsigned long lid; //blog��log ID auto_increment
    char username[IDLEN+2]; 
    char hostname[21]; 
    char action[100]; //����
    char pri_id[IDLEN+2]; //��һ��������
    char sec_id[IDLEN+2]; //�ڶ���������
    char comment[200]; //��ע
    time_t logtime;
};

#define PC_DEFAULT_NODELIMIT 300
#define PC_DEFAULT_DIRLIMIT 300

#endif


#define MAX_WWW_MAP_ITEM (MAX_WWW_GUEST>>5)

struct WWW_GUEST_S {
    int key;
    time_t freshtime;
    time_t logintime;
    int currentboard;
    struct in_addr fromip;
};

struct WWW_GUEST_TABLE {
    int hashtab[16][256][256];
    int use_map[MAX_WWW_MAP_ITEM + 1];
    time_t uptime;
    struct WWW_GUEST_S guest_entry[MAX_WWW_GUEST];
};


#ifdef HAVE_PERSONAL_DNS
struct dns_msgbuf {
    long int mtype;
    char userid[IDLEN+1];
    char ip[IPLEN+1];
};
#endif
#endif

/* protect id . added by binxun */
struct protect_id_passwd {
    char name[NAMELEN];    //��ʵ����
    unsigned short   birthyear;
    unsigned char    birthmonth;
    unsigned char    birthday;
    char email[STRLEN];     
    char question[STRLEN];   //������ʾ����
    char answer[STRLEN];    //�����
};

struct fileheader_num {
    struct fileheader fh;
    int num;
};

struct super_filter_query_arg {
    int inmail; /*if search in mail, 1, and boardname is username */
    char *query;
    struct fileheader* curfh; /* ��ǰ���� WARNING: curfh MUST NOT BE NULL */
    char * boardname;
    int isbm;
    struct fileheader_num* array; /* ���������� fh д������飬����Ϊ NULL */
    size_t array_size;        /* array �Ĵ�С */
    int detectmore;
    char *write_file;          /* ���������� fh д����ļ�������Ϊ NULL */
    char *direct;
};

/* �޸ĸ����ӿ� etnlegend */
struct ea_attach_info{
    char name[64];                                  //�����ļ�����(Ӧ������ 60 �ַ�)
    long offset;                                    //��������ʼλ��(����� SEEK_SET)
    long length;                                    //�����γ���
    unsigned int size;                              //�����ļ�����
};

/* NameList ����ڵ� */
typedef struct word{
    char *word;
    struct word *next;
}NLNode;

