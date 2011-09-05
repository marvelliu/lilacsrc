#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define NEW_COMERS 		0       /* ע����� newcomers ���Զ����� */
#define HAVE_USERMONEY 1
#define CONV_PASS		1
#define HAPPY_BBS		0
#define FREE 1
#define HAVE_COLOR_DATE		1
#define HAVE_FRIENDS_NUM	1
#define HAVE_REVERSE_DNS	0
#define CHINESE_CHARACTER	1
#define CNBBS_TOPIC		0       /* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#define MAIL2BOARD		0       /* �Ƿ�����ֱ�� mail to any board */
#define MAILOUT			0       /* �Ƿ�������վ���������� */
#define MANUAL_DENY		0       /*�Ƿ������ֶ���� */
#define BBS_SERVICE_DICT	0
#define HAVE_PERSONAL_DNS	0       /*������������ */

#define BUILD_PHP_EXTENSION 1   /*��php lib���php extension */

#define HAVE_WFORUM		0
#undef RAW_ARTICLE
#define SMTH			0
#ifndef FILTER
#define FILTER			0
#endif
#define IDLE_TIMEOUT    (60*20)

#define BBSUID 			510
#define BBSGID 			510

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 8
#define BBS_PAGE_SIZE 20

#define SQUID_ACCL

#define DEFAULTBOARD    	"test"
#define FILTER_BOARD        "Filter"
#define SYSMAIL_BOARD       "sysmail"
#define BLESS_BOARD "Blessing"

#define MAXUSERS  		20000
#define MAXCLUB			128
#define MAXBOARD  		1024
#define MAXACTIVE 		2000
/* remeber: if MAXACTIVE>46656 need change get_telnet_sessionid,
    make the number of session char from 3 to 4
    */
#define MAX_GUEST_NUM		1000
#define WWW_MAX_LOGIN 5000

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

#define DAY_DELETED_CLEAN	97
#define SEC_DELETED_OLDHOME	2592000 /* 3600*24*30��ע�����û������������û���Ŀ¼������ʱ�� */

#define	REGISTER_WAIT_TIME	(48*60*60)
#define	REGISTER_WAIT_TIME_NAME	"48 Сʱ"

#define MAIL_BBSDOMAIN      "ppjj.org"
#define MAIL_MAILSERVER     "127.0.0.1:25"

#define NAME_BBS_ENGLISH	"bbs.ee.tsinghua.edu.cn"
#define	NAME_BBS_CHINESE	"���ɿռ�"
#define NAME_BBS_NICK		"Free"

#define BBS_FULL_NAME "���ɿռ�"

#define FOOTER_MOVIE		"��  ӭ  Ͷ  ��"
/*#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"*/
#define ISSUE_LOGIN		"welcome"
#define ISSUE_LOGOUT		"����������"

#define NAME_USER_SHORT		"�û�"
#define NAME_USER_LONG		"�û�"
#define NAME_SYSOP		"System Operator"
#define NAME_BM			"����"
#define NAME_POLICE		"����"
#define	NAME_SYSOP_GROUP	"վ����"
#define NAME_ANONYMOUS		"ˮĸ���� Today!"
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

#define MSG_ERR_USERID		"�����ʹ���ߴ���..."
#define LOGIN_PROMPT		"���������"
#define PASSWD_PROMPT		"����������"

#define USE_DEFAULT_PERMSTRINGS
#define USE_DEFAULT_DEFINESTR
#define USE_DEFAULT_MAILBOX_PROP_STR

/**
 * �������û�ʱ�İ��������ַ���
 */
#define UL_CHANGE_NICK_UPPER   'C'
#define UL_CHANGE_NICK_LOWER   'c'
#define UL_SWITCH_FRIEND_UPPER 'F'
#define UL_SWITCH_FRIEND_LOWER 'f'

#define OWN_USEREC 1
#define IPLEN 16
struct userec {                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    char flags; /*һЩ��־����������������֮���*/
    unsigned char title; /*�û�����*/
    time_t firstlogin;
    char lasthost[IPLEN];
    unsigned int numlogins;
    unsigned int numposts;
#ifdef CONV_PASS
    char passwd[35];
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
    int notemode;
    time_t exittime;
	/* ��������ת�Ƶ� userdata �ṹ�� */
    unsigned int usedspace;     /* used space of user's mailbox, in bytes */
    int money;
    int score;
    int unused[5];
};

/** ʹ��ȱʡ��FILEHeader�ṹ*/
#define HAVE_FILEHEADER_DEFINE

#define GET_POSTFILENAME(x,y) get_postfilename(x,y,0)
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#define VALID_FILENAME(x) valid_filename(x,1)
#define POSTFILE_BASENAME(x) (((char *)(x))+2)
#define MAILFILE_BASENAME(x) (x)

/**
attach define
*/
#define ATTACHTMPPATH "boards/_attach"

//#define CHECK_IP_LINK 1

//#define SMS_SUPPORT

//#define PERSONAL_CORP

#define NEW_HELP
#define HAVE_DEFAULT_HELPMODE

#define COMMEND_ARTICLE "Recommend"

//#define NOT_USE_DEFAULT_SMS_FUNCTIONS

#define HAVE_OWN_USERIP
#define SHOW_USERIP(user,x) showuserip(user,x)

//#define AUTO_CHECK_REGISTER_FORM        //�Զ�ͨ��ע�ᵥ

#define QUOTED_LINES 10
#define QUOTELEV 0

//#define BIRTHFILENUM 12
//#define BIRTHFILEPATH "0Announce/groups/system.faq/SYSOP/birthfile"

#define MYUNLINK_BACKUPDIR "backup"

#define BMSLOG
#define NEWPOSTLOG
#define NEWBMLOG

#define NEWPOSTSTAT

#define FB2KENDLINE
#define LOWCOLOR_ONLINE
#define OPEN_NOREPLY

#define FIRSTARTICLE_SIGN "��"

#define POST_QUIT

#define ANN_COUNT

#define ANN_SHOW_WELCOME

#define ANN_CTRLK
#define ANN_GUESTBOOK
#define ANN_AUTONAME
#define FB2KPC "pc"

#define ACBOARD_BNAME "notepad"

#define MAXnettyLN      7       /* lines of  activity board  */
#define DENYANONY

#define AUTOREMAIL
#define OWNSENDMAIL "/usr/lib/sendmail"

#define UNREAD_SIGN '+'
#define OPEN_BMONLINE
//#define USE_SEM_LOCK
#endif
