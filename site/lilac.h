#ifndef __SYSNAME_H_
#define __SYSNAME_H_

//#define COLOR_ONLINE 1

#ifndef HAVE_CUSTOM_USER_TITLE
#define HAVE_CUSTOM_USER_TITLE
#endif


#define IP_CTRL_SHM_KEY  0x19791117
#define MAX_IPCTRL       512
#define IPCTRL_READ       0
#define IPCTRL_WRITE      1
#define IPCTRL_REG        2


#define LILAC               1
//����������������Ļ��������������Ҫ����
//#define LILAC_PROXY           1
#define LILAC_REG           1
#define LILAC_REG_MARVEL           1
#define USE_IPCTRL              1
#define RECMD_INFO		1
#define RECMD_INFO_BOARD	"RecommendInfo"
#define RECMD_INFO_NUM	4
//By Kaede
#define BBSUID 500
#define BBSGID 500
//By Marvel
//By lanslot 
#define SAVELIVE
#define PHP_BBS_LILAC_FUNCTIONS
#define HOROSCOPE

#define USE_TMPFS           0  /*ʹ���ڴ��ļ�ϵͳ����*/
#define TMPFSROOT   "cache" /*tmpfs�ĸ���bbshome/cache */
#define CONV_PASS		1
#define NINE_BUILD		1
#define NEW_COMERS		1	/* ע����� newcomers ���Զ����� */
#define HAVE_BIRTHDAY	1
#define HAPPY_BBS		0
#define HAVE_COLOR_DATE		1
#define HAVE_TEMPORARY_NICK	1
#define HAVE_FRIENDS_NUM	1
#define HAVE_REVERSE_DNS	0
#define CHINESE_CHARACTER	1
#define CNBBS_TOPIC		0	/* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#define MAIL2BOARD		0	/* �Ƿ�����ֱ�� mail to any board */
#define MAILOUT			1	/* �Ƿ�������վ���������� */
#define MANUAL_DENY		0	/*�Ƿ������ֶ����*/
#define BBS_SERVICE_DICT	1
#define HAVE_TSINGHUA_INFO_REGISTER 0

#define BUILD_PHP_EXTENSION 1   /*��php lib���php extension */
/*#define USE_SEM_LOCK 1*/

//By Marvel
#define FB2KENDLINE     1
#define BIRTHFILEPATH "etc/birthfile"
#define BIRTHFILENUM 12

#define PERSONAL_CORP
#define HAVE_WFORUM	1
#define RAW_ARTICLE 0
#define FB2000			1
#define SMTH			1
#define FILTER			1
/*
 *    Define DOTIMEOUT to set a timer to bbslog out users who sit idle on the system.
 *       Then decide how long to let them stay: MONITOR_TIMEOUT is the time in
 *          seconds a user can sit idle in Monitor mode; IDLE_TIMEOUT applies to all
 *             other modes.
 *             */
#define DOTIMEOUT 1

/*
 *    These are moot if DOTIMEOUT is commented; leave them defined anyway.
 *    */
#define IDLE_TIMEOUT    (60*20)
#define MONITOR_TIMEOUT (60*20)

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 12
#define BBS_PAGE_SIZE 20

#define SQUID_ACCL

#define DEFAULTBOARD        "sysop"
#define FILTER_BOARD        "Filter"
#define SYSMAIL_BOARD       "sysmail"
#define BLESS_BOARD         "Bless"

#define MAXUSERS  		40000
#define MAXCLUB			1024   //ǧ��Ҫ�޸ģ��޸Ĺ�����Ҫת��.PASSWDS
#define MAXBOARD  		1024
#define MAXACTIVE 		3000
/* remeber: if MAXACTIVE>46656 need change get_telnet_sessionid,
    make the number of session char from 3 to 4
    */
#define MAX_GUEST_NUM		1024

#define POP3PORT		110
#define POP3SPORT		995
/* ASCIIArt, by czz, 2002.7.5 */
#define       LENGTH_SCREEN_LINE      255
#define       LENGTH_FILE_BUFFER      255
#define       LENGTH_ACBOARD_BUFFER   300
#define       LENGTH_ACBOARD_LINE     300

#define LIFE_DAY_USER		120
#define LIFE_DAY_YEAR          365
#define LIFE_DAY_LONG		666
#define LIFE_DAY_SYSOP		120
#define LIFE_DAY_NODIE		999
#define LIFE_DAY_NEW		15
#define LIFE_DAY_SUICIDE	15

#define DAY_DELETED_CLEAN	30
#define SEC_DELETED_OLDHOME	2592000 /* 3600*24*30��ע�����û������������û���Ŀ¼������ʱ�� */

#define	REGISTER_WAIT_TIME	(86400)
//#define	REGISTER_WAIT_TIME	(0)
#define	REGISTER_WAIT_TIME_NAME	"24Сʱ"

#ifdef SMTH
#define REGISTER_TSINGHUA_WAIT_TIME (24*60*60)
#endif
#define MAIL_BBSDOMAIN      "lilacbbs.com"
#define MAIL_MAILSERVER     "127.0.0.1:25"

#define NAME_BBS_ENGLISH	"lilacbbs.com"
#define	NAME_BBS_CHINESE	"�϶�������"
#define NAME_BBS_NICK		"BBS վ"

#define BBS_FULL_NAME           "�϶�������"

#define FOOTER_MOVIE		"��  ӭ  ��  ��"
/*#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"*/
#define ISSUE_LOGIN		"�϶�������"
#define ISSUE_LOGOUT		"�ٱ��϶���"

#define NAME_USER_SHORT		"�û�"
#define NAME_USER_LONG		"�϶����û�"
#define NAME_SYSOP		"System Operator"
#define NAME_BM			"����"
#define NAME_POLICE		"����"
#define	NAME_SYSOP_GROUP	"վ����"
#define NAME_ANONYMOUS		"�Ұ��϶���!"
#define NAME_ANONYMOUS_FROM	"������ʹ�ļ�"
#define ANONYMOUS_DEFAULT 0

#define NAME_MATTER		"վ��"
#define NAME_SYS_MANAGE		"ϵͳά��"
#define NAME_SEND_MSG		"��ѶϢ"
#define NAME_VIEW_MSG		"��ѶϢ"

#define CHAT_MAIN_ROOM		"main"
#define	CHAT_TOPIC		"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"������"
#define	CHAT_SERVER		"����㳡"
#define CHAT_MSG_QUIT		"����ϵͳ"
#define CHAT_OP			"������ op"
#define CHAT_SYSTEM		"ϵͳ"
#define	CHAT_PARTY		"���"

#define DEFAULT_NICK		"ÿ�찮���һЩ"

#define MSG_ERR_USERID		"�����ʹ�����˺�..."
#define LOGIN_PROMPT		"�������ʺ�"
#define PASSWD_PROMPT		"����������"

/* Ȩ��λ���� */
/*
   These are the 16 basic permission bits.
   All but the last one are used in comm_lists.c and help.c to control user
   access to priviliged functions. The symbolic names are given to roughly
   correspond to their actual usage; feel free to use them for different
   purposes though. The PERM_SPECIAL1 and PERM_SPECIAL2 are not used by
   default and you can use them to set up restricted boards or chat rooms.
*/

#define NUMPERMS (30)

#define PERM_BASIC          000001
#define PERM_CHAT           000002
#define PERM_PAGE           000004
#define PERM_POST           000010
#define PERM_LOGINOK        000020
#define PERM_BMAMANGER 	    000040
#define PERM_CLOAK          000100
#define PERM_SEECLOAK       000200
#define PERM_XEMPT          000400
#define PERM_WELCOME        001000
#define PERM_BOARDS         002000
#define PERM_ACCOUNTS       004000
#define PERM_CHATCLOAK      010000
#define PERM_DENYRELAX      020000
#define PERM_SYSOP          040000
#define PERM_POSTMASK      0100000
#define PERM_ANNOUNCE      0200000
#define PERM_OBOARDS       0400000
#define PERM_ACBOARD       01000000
#define PERM_NOZAP         02000000
#define PERM_CHATOP        04000000
#define PERM_ADMIN        010000000
#define PERM_HORNOR       020000000
#define PERM_JURY         040000000
#define PERM_LARGEMAIL    100000000
#define PERM_NOIPCTRL    0200000000
#define PERM_CHECKCD     0200000000
#define PERM_SUICIDE     0400000000
#define PERM_COLLECTIVE 01000000000
#define PERM_DENYMAIL  	02000000000
#define PERM_DISS       04000000000


#define PERM_SECANC    0200000000
#define XPERMSTR "bTCPRp#@XWBA$VS!DEM1234567890%"
/* means the rest is a post mask */

/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | PERM_POST | PERM_LOGINOK)

/* These permissions are bitwise ORs of the basic bits. They work that way
   too. For example, anyone with PERM_SYSOP or PERM_BOARDS or both has
   PERM_SEEBLEVELS. */

#define PERM_ADMINMENU  (PERM_ACBOARD| PERM_ACCOUNTS | PERM_SYSOP|PERM_OBOARDS|PERM_WELCOME)
#define PERM_MULTILOG   (PERM_SYSOP | PERM_ANNOUNCE | PERM_OBOARDS)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS | PERM_BOARDS | PERM_OBOARDS | PERM_WELCOME)
#define PERM_SEEULEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_MARKPOST   (PERM_SYSOP | PERM_BOARDS)
#define PERM_UCLEAN     (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_NOTIMEOUT  PERM_SYSOP
/* PERM_MANAGER will be used to allow 2 windows and
bigger mailbox. --stephen 2001.10.31*/
#define PERM_MANAGER    (PERM_CHATOP | PERM_JURY | PERM_BOARDS)
#define PERM_SENDMAIL   0
#define PERM_READMAIL   PERM_BASIC
#define PERM_VOTE       PERM_BASIC

/* These are used only in Internet Mail Forwarding */
/* You may want to be more restrictive than the default, especially for an
   open access BBS. */

#define PERM_SETADDR    PERM_BASIC      /* to set address for forwarding */
#define PERM_FORWARD    PERM_BASIC      /* to do the forwarding */

/* Don't mess with this. */
#define HAS_PERM(user,x) ((x)?((user)->userlevel)&(x):1)
#define DEFINE(user,x)     ((x)?((user)->userdefine[def_list(x)])&(x):1)

#define TDEFINE(x) ((x)?(tmpuser)&(x):1)

/* �û��Զ��������� */


#define DEF_FRIENDCALL   0x00000001
#define DEF_ALLMSG       0x00000002
#define DEF_FRIENDMSG    0x00000004
#define DEF_SOUNDMSG     0x00000008
#define DEF_COLOR        0x00000010
#define DEF_ACBOARD      0x00000020
#define DEF_ENDLINE      0x00000040
#define DEF_EDITMSG      0x00000080
#define DEF_NOTMSGFRIEND 0x00000100
#define DEF_NORMALSCR    0x00000200
#define DEF_NEWPOST      0x00000400
#define DEF_CIRCLE       0x00000800
#define DEF_FIRSTNEW     0x00001000
#define DEF_LOGFRIEND    0x00002000
#define DEF_LOGOUT         0x00004000 // 000200
#define DEF_NOTEPAD        0x00008000
#define DEF_INNOTE         0x00010000 //000400
#define DEF_OUTNOTE        0x00020000 //001000
#define DEF_IGNOREMSG      0x00040000
#define DEF_SHOWSTATISTIC  0x00080000
#define DEF_SHOWHOT        0x00100000
#define DEF_TITLECOLOR     0x00200000 //040000
#define DEF_MAILMSG        0x00400000 //01000000
#define DEF_LOGININFORM    0x00800000 //02000000
#define DEF_SHOWSCREEN          0x01000000 //04000000
#define DEF_HIGHCOLOR	        0x02000000   /*Leeward 98.01.12 */
#define DEF_UNREADMARK          0x04000000       /* Luzi 99.01.12 */
#define DEF_USEGB               0x08000000       /* KCN,99.09.05 */
#define DEF_CHCHAR              0x10000000
#define DEF_SHOWDETAILUSERDATA	0x20000000
#define DEF_SHOWREALUSERDATA	0x40000000
#define DEF_NATIONTOP10         0x80000000
#define DEF_HIDEIP			040000000001LL
/*#define DEF_HIDEIP    02000000000  Haohmaru,99.12.18*/

#define DEF_SHOWSIZE	040000000002LL

/*#define PERM_POSTMASK  0100000  *//*
 * means the rest is a post mask
 */

#define NUMDEFINES 34
//#define NUMDEFINES 33

#define SET_DEFINE(user,x) ((user)->userdefine[def_list(x)] |= x)
#define SET_UNDEFINE(user,x) ((user)->userdefine[def_list(x)] &= ~x)
#define SET_CHANGEDEFINE(user,x) ((user)->userdefine[def_list(x)] ^= x)


#define TDEF_SPLITSCREEN 000001

extern const char * const permstrings[];
extern const char    * const groups[];
extern const char    * const explain[];
extern const char * const user_definestr[];
extern const char * const mailbox_prop_str[];

/**
 * �������û�ʱ�İ��������ַ���
 */
#define UL_CHANGE_NICK_UPPER   'C'
#define UL_CHANGE_NICK_LOWER   'c'
#define UL_SWITCH_FRIEND_UPPER 'F'
#define UL_SWITCH_FRIEND_LOWER 'f'

/**
 * ������ز��֡�
 */
#define STRLEN          80
#define BM_LEN 60
#define FILENAME_LEN 20
#define OWNER_LEN 14
#define ARTICLE_TITLE_LEN 60
typedef struct fileheader {     /* This structure is used to hold data in */
    char filename[FILENAME_LEN];      /* the DIR files */
    unsigned int id, groupid, reid;
#if defined(FILTER) || defined(COMMEND_ARTICLE)
	int o_bid;
    unsigned int o_id;
    unsigned int o_groupid;
    unsigned int o_reid;
#else
    char unused1[16];
#endif
    char innflag[2];
    char owner[OWNER_LEN];
    unsigned int eff_size;
    time_t posttime;
    long attachment;
    char title[ARTICLE_TITLE_LEN];
    unsigned char accessed[4];
} fileheader;

typedef struct fileheader fileheader_t;

#define GET_POSTFILENAME(x,y) get_postfilename(x,y,0)
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#define VALID_FILENAME(x) valid_filename(x,0)

// WWW����

//�޸�����֮���ҳ��ʹ��https
#define SECURE_HTTPS

/**
 * Mailbox properties.
 *
 * @author flyriver
 */

#define MBP_SAVESENTMAIL      0x00000001
#define MBP_FORCEDELETEMAIL   0x00000002
#define MBP_MAILBOXSHORTCUT   0x00000004

#define MBP_NUMS 3

#define HAS_MAILBOX_PROP(u, x) ((u)->mailbox_prop & x)

/**
 * Mailgroup macros.
 *
 * @author flyriver
 */
#define MAX_MAILGROUP_NUM 30
#define MAX_MAILGROUP_USERS 300

/**
attach define
*/
#define ATTACHTMPPATH "boards/_attach"

#ifndef LILAC_PROXY
#define CHECK_IP_LINK 1
#endif

#define BOARD_SHOW_ONLINE 0


#define COMMEND_ARTICLE "Recommend"

#define NOT_USE_DEFAULT_SMS_FUNCTIONS

#define HAVE_OWN_USERIP
#define SHOW_USERIP(user,x) showuserip(user,x)

#define QUOTED_LINES 3
#define QUOTELEV 0

#define LILAC_SCORE

#endif