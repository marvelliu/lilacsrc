/**
  ȱʡ�������ļ�
*/

#ifndef __SYS_DEFAULT_H_
#define __SYS_DEFAULT_H_

#ifndef TMPFSROOT
#define TMPFSROOT "cache"
#endif

#ifndef SMTH /*ˮľ�廪ר�д���,ȱʡʹ��*/
#define SMTH 1
#else
#if SMTH == 0
#undef SMTH
#endif
#endif

#ifdef HAPPY_BBS
#if HAPPY_BBS == 0 /* HAPPY BBSר�д��� */
#undef HAPPY_BBS
#endif
#endif

#ifdef FREE
#if FREE == 0 /* FREE BBSר�д��� */
#undef FREE
#endif
#endif

#ifdef FLOWBANNER
#ifndef MAXBANNER
#define MAXBANNER 8
#endif
#ifndef BANNERSIZE
#define BANNERSIZE 120
#endif
#endif

#if NEW_COMERS == 0
#undef NEW_COMERS		/* ע����� newcomers ���Զ����� */
#endif

#if HAVE_COLOR_DATE == 0
#undef HAVE_COLOR_DATE
#endif

#if HAVE_FRIENDS_NUM == 0
#undef HAVE_FRIENDS_NUM
#endif

#if HAVE_REVERSE_DNS == 0 /*����dns���� */
#undef HAVE_REVERSE_DNS
#endif

#if HAVE_WFORUM == 0
#undef HAVE_WFORUM
#endif


/* �����������ʷԭ�����������ģ���Ҫ����ΪһЩת�������Ծ���Ҫ������� - atppp 20051004 */
#ifdef HAVE_WFORUM
#define HAVE_USERMONEY 1
#endif



/* Ĭ������¼��� HAVE_BIRTHDAY ֧�� - atppp 20040819 */
#ifdef HAVE_BIRTHDAY
#if HAVE_BIRTHDAY == 0
#undef HAVE_BIRTHDAY
#endif
#else
#define HAVE_BIRTHDAY 1
#endif

#ifndef CHINESE_CHARACTER /*�������ִ���,ȱʡʹ��*/
#define CHINESE_CHARACTER
#else
#if CHINESE_CHARACTER == 0
#undef CHINESE_CHARACTER
#endif
#endif

#if CNBBS_TOPIC	== 0		/* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#undef CNBBS_TOPIC		/* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#endif

#if MAIL2BOARD == 0		/* �Ƿ�����ֱ�� mail to any board */
#undef MAIL2BOARD		/* �Ƿ�����ֱ�� mail to any board */
#endif

#if MAILOUT == 0		/* �Ƿ�������վ���������� */
#undef MAILOUT			/* �Ƿ�������վ���������� */
#endif

#ifndef MANUAL_DENY		/* �ֶ���⣬ȱʡʹ��*/
#define MANUAL_DENY
#else
#if MANUAL_DENY == 0
#undef MANUAL_DENY
#endif
#endif

#ifndef BBS_SERVICE_DICT	/* �ʵ书�� */
#define BBS_SERVICE_DICT
#else
#if BBS_SERVICE_DICT == 0
#undef BBS_SERVICE_DICT
#endif
#endif

#ifndef BBS_SERVICE_QUIZ	/* �ʵ书�� */
#define BBS_SERVICE_QUIZ
#else
#if BBS_SERVICE_QUIZ == 0
#undef BBS_SERVICE_QUIZ
#endif
#endif

#if HAVE_PERSONAL_DNS == 0	/*������������ȱʡ��ʹ��*/
#undef HAVE_PERSONAL_DNS
#endif

#ifndef HAVE_BRC_CONTROL
#define HAVE_BRC_CONTROL
#else
#if HAVE_BRC_CONTROL == 0
#undef HAVE_BRC_CONTROL
#endif
#endif

#ifndef FILTER /*������*/
#define FILTER
#else
#if FILTER == 0
#undef FILTER
#endif
#endif



#ifndef IDLE_TIMEOUT
#define IDLE_TIMEOUT    (60*20) 
#endif

#ifndef MAX_WWW_GUEST_IDLE_TIME
#define MAX_WWW_GUEST_IDLE_TIME (3600)
#endif

#ifndef BBSUID
#error You must define BBSUID in site.h for your site!
#endif

#ifndef BBSGID
#error You must define BBSGID in site.h for your site!
#endif


#ifndef SECNUM /* ��������*/
#error must define SECNUM in your site.h
#endif

#ifndef DEFAULTBOARD
#define DEFAULTBOARD    	"test"
#endif

#ifndef FILTER_BOARD
#define FILTER_BOARD        "Filter"
#endif

#ifndef SYSMAIL_BOARD
#define SYSMAIL_BOARD       "sysmail"
#endif

#ifndef BLESS_BOARD
#define BLESS_BOARD "Blessing"
#endif

#ifndef MAXUSERS
#define MAXUSERS  		20000
#endif

#ifndef MAXCLUB
#define MAXCLUB			128
#endif

#ifndef MAXBOARD
#define MAXBOARD  		400
#endif

#ifndef MAXACTIVE
#define MAXACTIVE 		8000
#endif
/* remeber: if MAXACTIVE>46656 need change get_telnet_sessionid,
    make the number of session char from 3 to 4
    */

#ifndef MAX_GUEST_NUM
#define MAX_GUEST_NUM		800
#endif

#ifndef WWW_MAX_LOGIN
#define WWW_MAX_LOGIN 1000
#endif

#ifndef MAX_WWW_GUEST
#define MAX_WWW_GUEST 30016
#endif

#ifndef POP3PORT
#define POP3PORT		110
#endif

#ifndef POP3SPORT
#define POP3SPORT		995
#endif

#ifndef DELIVER
#define DELIVER "deliver"
#endif

/* ASCIIArt, by czz, 2002.7.5 */
#ifndef       LENGTH_SCREEN_LINE
#define       LENGTH_SCREEN_LINE      255
#endif

#ifndef       LENGTH_FILE_BUFFER
#define       LENGTH_FILE_BUFFER      255
#endif

#ifndef       LENGTH_ACBOARD_BUFFER
#define       LENGTH_ACBOARD_BUFFER   200
#endif

#ifndef       LENGTH_ACBOARD_LINE
#define       LENGTH_ACBOARD_LINE     300
#endif

#ifndef DAY_DELETED_CLEAN /*����������ͻ���վ���������*/
#define DAY_DELETED_CLEAN	97
#endif

#ifndef LEN_FRIEND_EXP		/*�����FB2k������smth,����������Ϊ40 */
#define LEN_FRIEND_EXP		40
#endif

#ifndef SEC_DELETED_OLDHOME
#define SEC_DELETED_OLDHOME	2592000 /* 3600*24*30��ע�����û������������û���Ŀ¼������ʱ�� */
#endif

#ifndef	REGISTER_WAIT_TIME /*��ע�ᵥ��Ҫ�ȴ���ʱ��*/
#define	REGISTER_WAIT_TIME	0
#endif

#ifndef REGISTER_WAIT_TIME_NAME	
#define	REGISTER_WAIT_TIME_NAME	"����"
#endif

#ifndef MAIL_BBSDOMAIN
#define MAIL_BBSDOMAIN      "smth.org"
#endif

#ifndef NAME_BBS_ENGLISH
#define NAME_BBS_ENGLISH	"smth.org"
#endif

#ifndef	NAME_BBS_CHINESE
#define	NAME_BBS_CHINESE	"ˮľ����"
#endif

#ifndef BBS_FULL_NAME
#define BBS_FULL_NAME "BBS ˮľ����վ"
#endif

#ifndef FOOTER_MOVIE
#define FOOTER_MOVIE		"��  ӭ  Ͷ  ��"
#endif

/*#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"*/
#ifndef ISSUE_LOGIN
#define ISSUE_LOGIN		"���PC  ��21����˻�ָ��������"
#endif

#ifndef ISSUE_LOGOUT
#define ISSUE_LOGOUT		"����������"
#endif

#ifndef NAME_USER_SHORT
#define NAME_USER_SHORT		"�û�"
#endif

#ifndef NAME_SYSOP
#define NAME_SYSOP		"System Operator"
#endif

#ifndef NAME_BM
#define NAME_BM			"����"
#endif

#ifndef NAME_POLICE
#define NAME_POLICE		"����"
#endif

#ifndef	NAME_SYSOP_GROUP
#define	NAME_SYSOP_GROUP	"վ����"
#endif

#ifndef NAME_ANONYMOUS /*�������ĵ��ǳ�*/
#define NAME_ANONYMOUS		"ˮĸ���� Today!"
#endif

#ifndef NAME_ANONYMOUS_FROM /*ȱʡ��������Դ*/
#define NAME_ANONYMOUS_FROM	"������ʹ�ļ�"
#endif

#ifndef ANONYMOUS_DEFAULT /*ȱʡ������*/
#define ANONYMOUS_DEFAULT 0
#endif

#ifndef NAME_MATTER
#define NAME_MATTER		"վ��"
#endif

#ifndef NAME_SYS_MANAGE
#define NAME_SYS_MANAGE		"ϵͳά��"
#endif

#ifndef NAME_SEND_MSG
#define NAME_SEND_MSG		"��ѶϢ"
#endif

#ifndef NAME_VIEW_MSG
#define NAME_VIEW_MSG		"��ѶϢ"
#endif

#ifndef CHAT_MAIN_ROOM
#define CHAT_MAIN_ROOM		"main"
#endif

#ifndef CHAT_TOPIC
#define	CHAT_TOPIC		"�����������İ�"
#endif

#ifndef CHAT_MSG_NOT_OP
#define CHAT_MSG_NOT_OP		"*** �����Ǳ������ҵ�op ***"
#endif

#ifndef CHAT_ROOM_NAME
#define	CHAT_ROOM_NAME		"������"
#endif

#ifndef CHAT_SERVER
#define	CHAT_SERVER		"����㳡"
#endif

#ifndef CHAT_MSG_QUIT
#define CHAT_MSG_QUIT		"����ϵͳ"
#endif

#ifndef CHAT_OP
#define CHAT_OP			"������ op"
#endif

#ifndef CHAT_SYSTEM
#define CHAT_SYSTEM		"ϵͳ"
#endif

#ifndef CHAT_PARTY
#define	CHAT_PARTY		"���"
#endif

#ifndef DEFAULT_NICK
#define DEFAULT_NICK		"ÿ�찮���һЩ"
#endif

#ifndef MSG_ERR_USERID
#define MSG_ERR_USERID		"�����ʹ���ߴ���..."
#endif

#ifndef LOGIN_PROMPT
#define LOGIN_PROMPT		"���������"
#endif

#ifndef PASSWD_PROMPT
#define PASSWD_PROMPT		"����������"
#endif

#ifndef DENY_NAME_SYSOP
#define DENY_DESC_AUTOFREE	"������ʱȡ���ڸð�ķ���Ȩ��"
#define DENY_DESC_NOAUTOFREE	DENY_DESC_AUTOFREE
#define DENY_BOARD_AUTOFREE	"����ʱȡ���ڱ���ķ���Ȩ��"
#define DENY_BOARD_NOAUTOFREE	"����ʱȡ���ڸð�ķ���Ȩ�������ں���ظ�"
#define DENY_NAME_SYSOP		"ֵ��վ��"
#endif

#ifndef NUMPERMS
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

#define PERM_BASIC      000001
#define PERM_CHAT       000002
#define PERM_PAGE       000004
#define PERM_POST       000010
#define PERM_LOGINOK    000020
#define PERM_BMAMANGER	000040
#define PERM_CLOAK      000100
#define PERM_SEECLOAK   000200
#define PERM_XEMPT      000400
#define PERM_WELCOME    001000
#define PERM_BOARDS     002000
#define PERM_ACCOUNTS   004000
#define PERM_CHATCLOAK  010000
#define PERM_DENYRELAX      020000
#define PERM_SYSOP      040000
#define PERM_POSTMASK  0100000
#define PERM_ANNOUNCE  0200000
#define PERM_OBOARDS   0400000
#define PERM_ACBOARD   01000000
#define PERM_NOZAP     02000000
#define PERM_CHATOP    04000000
#define PERM_ADMIN     010000000
#define PERM_HORNOR   	020000000
#define PERM_SECANC    040000000
#define PERM_JURY      0100000000
#define PERM_CHECKCD      0200000000
#define PERM_SUICIDE   0400000000
#define PERM_COLLECTIVE 	01000000000
#define PERM_DISS        02000000000
#define PERM_DENYMAIL   	04000000000

//#define PERM_NOIPCTRL    0200000000


#define XPERMSTR "bTCPRp#@XWBA$VS!DEM1234567890%"
/* means the rest is a post mask */

/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | PERM_POST | PERM_LOGINOK)
#define PERM_MULTILOG   (PERM_SYSOP | PERM_ANNOUNCE | PERM_OBOARDS)
#define PERM_MANAGER    (PERM_CHATOP | PERM_JURY | PERM_BOARDS)

#endif //permission define NUMPERMS





#ifndef PERM_AUTOSET
#define PERM_AUTOSET	PERM_BASIC
#endif


#ifndef NUMDEFINES

/* �û��Զ��������� */
#define DEF_ACBOARD      000001
#define DEF_COLOR        000002
#define DEF_EDITMSG      000004
#define DEF_NEWPOST      000010
#define DEF_ENDLINE      000020
#define DEF_LOGFRIEND    000040
#define DEF_FRIENDCALL   000100
#define DEF_LOGOUT       000200
#define DEF_INNOTE       000400
#define DEF_OUTNOTE      001000
#define DEF_NOTMSGFRIEND 002000
#define DEF_NORMALSCR    004000
#define DEF_CIRCLE       010000
#define DEF_FIRSTNEW     020000
#define DEF_TITLECOLOR   040000
#define DEF_ALLMSG       0100000
#define DEF_FRIENDMSG    0200000
#define DEF_SOUNDMSG     0400000
#define DEF_MAILMSG      01000000
#define DEF_LOGININFORM  02000000
#define DEF_SHOWSCREEN   04000000
#define DEF_SHOWHOT      010000000
#define DEF_NOTEPAD      020000000
#define DEF_IGNOREMSG    040000000      /* Added by Marco */
#define DEF_HIGHCOLOR	0100000000   /*Leeward 98.01.12 */
#define DEF_SHOWSTATISTIC 0200000000    /* Haohmaru */
#define DEF_UNREADMARK 0400000000       /* Luzi 99.01.12 */
#define DEF_USEGB     01000000000       /* KCN,99.09.05 */
#define DEF_CHCHAR				02000000000
#define DEF_SHOWDETAILUSERDATA	04000000000
#define DEF_SHOWREALUSERDATA	010000000000
#define DEF_HIDEIP			040000000001LL
#define DEF_SHOWSIZE            040000000002LL

//#define NUMDEFINES 34
#endif

/* Don't mess with this. */
#define HAS_PERM(user,x) ((x)?((user)->userlevel)&(x):1)
#define DEFINE(user,x)     ((x)?((user)->userdefine[def_list(x)])&(x):1)
#define TDEFINE(x) ((x)?(tmpuser)&(x):1)
#define SET_DEFINE(user,x) ((user)->userdefine[def_list(x)] |= x)
#define SET_UNDEFINE(user,x) ((user)->userdefine[def_list(x)] &= ~x)
#define SET_CHANGEDEFINE(user,x) ((user)->userdefine[def_list(x)] ^= x)


#define TDEF_SPLITSCREEN 000001



extern const char * const permstrings[];
extern const char    * const groups[];
extern const char * const user_definestr[];
extern const char * const user_definestr1[];
#ifdef NEW_HELP
extern const char * const helpmodestr[];
#endif

#ifdef SMS_SUPPORT
#define NUMSMSDEF 1
#define SMSDEF_AUTOTOMOBILE      000001
extern const char * const user_smsdefstr[];
#endif
extern const char * const mailbox_prop_str[];

extern const char seccode[SECNUM][5];
extern const char secname[SECNUM][2][20];

/**
 * �������û�ʱ�İ��������ַ���
 */
#ifndef UL_CHANGE_NICK_UPPER
#define UL_CHANGE_NICK_UPPER   'C'
#endif

#ifndef UL_CHANGE_NICK_LOWER
#define UL_CHANGE_NICK_LOWER   'c'
#endif

#ifndef UNREAD_SIGN
#define UNREAD_SIGN '*'
#endif

#ifndef UL_SWITCH_FRIEND_UPPER
#define UL_SWITCH_FRIEND_UPPER 'F'
#endif

#ifndef UL_SWITCH_FRIEND_LOWER
#define UL_SWITCH_FRIEND_LOWER 'f'
#endif


/**
 * ������ز��֡�
 */
#ifndef STRLEN
#define STRLEN          80
#endif

#ifndef BM_LEN
#define BM_LEN 60
#endif

#ifndef FILENAME_LEN
#define FILENAME_LEN 20
#endif

#ifndef OWNER_LEN
#define OWNER_LEN 14
#endif

#ifndef ARTICLE_TITLE_LEN
#define ARTICLE_TITLE_LEN 60
#endif

#ifdef HAVE_FILEHEADER_DEFINE

/**
 * ������ز��֡�
 */
typedef struct fileheader {     /* This structure is used to hold data in */
    char filename[FILENAME_LEN];        /* the DIR files */
    unsigned int id, groupid, reid;

    /* please kill these four fuckers */
    int o_bid;
    unsigned int o_id;
    unsigned int o_groupid;
    unsigned int o_reid;

    char innflag[2];
    char owner[OWNER_LEN];
    unsigned int eff_size; /* ��������: ��ʾ��Ч�ֽ������ż�: ��ʾ�ļ���С - atppp */
    int posttime;
    unsigned int attachment;
    char title[ARTICLE_TITLE_LEN];
    //unsigned short replycount;  /* will be used, pig2532 */
    //char last_owner[OWNER_LEN];
    //int last_posttime;
    //char unused[96];
    unsigned char accessed[4];
} fileheader;

typedef struct fileheader fileheader_t;
#endif

#ifndef GET_POSTFILENAME
#define GET_POSTFILENAME(x,y) get_postfilename(x,y,0)
#endif

#ifndef GET_MAILFILENAME
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#endif

#ifndef VALID_FILENAME
#define VALID_FILENAME(x) valid_filename(x,0)
#endif

#ifndef POSTFILE_BASENAME
#define POSTFILE_BASENAME(x) (x)
#endif

#ifndef MAILFILE_BASENAME
#define MAILFILE_BASENAME(x) (x)
#endif

#ifndef SHOW_USERIP
#define SHOW_USERIP(y,x) x
#endif



/**
 * Mailbox properties.
 * 
 * @author flyriver
 */
#define MBP_SAVESENTMAIL      0x00000001
#define MBP_FORCEDELETEMAIL   0x00000002
#define MBP_MAILBOXSHORTCUT   0x00000004

#define MBP_NUMS 3

#ifndef MBP_DEFAULT
#define MBP_DEFAULT           0x00000001  //Ĭ�ϣ���ע���û�������������
#endif

#define HAS_MAILBOX_PROP(u, x) ((u)->mailbox_prop & x)
/**
 * Mailgroup macros.
 *
 * @author flyriver
 */
#define MAX_MAILGROUP_NUM 30
#define MAX_MAILGROUP_USERS 300


/***
  new help
*/
#ifdef NEW_HELP

#ifndef NUMHELPMODE

#define NUMHELPMODE 14
#define HELP_BOARD 1
#define HELP_GOODBOARD 2
#define HELP_ARTICLE 3
#define HELP_ANNOUNCE 4
#define HELP_EDIT 5
#define HELP_CHAT 6
#define HELP_VOTE 7
#define HELP_FRIEND 8
#define HELP_TIME 9
#define HELP_TMPL 10
#define HELP_FRIENDTEST 11
#define HELP_CHICKEN 12
#define HELP_KILLER 13
#define HELP_MAIL 14

#endif //NUMHELPMODE
#endif //NEW_HELP

/**
attach define
*/
#ifndef ATTACHTMPPATH
#define ATTACHTMPPATH "boards/_attach"
#endif

/*
 * max number of attachments in one article
 */
#ifndef MAXATTACHMENTCOUNT
#define MAXATTACHMENTCOUNT	20
#endif

#ifndef MAXATTACHMENTSIZE
#define MAXATTACHMENTSIZE 5*1024*1024                     //���¸����ļ���������
#endif


#if CHECK_IP_LINK==0
#undef CHECK_IP_LINK
#endif

#ifndef ID_CONNECT_CON_THRESHOLD //��ֹ��վ��
#define ID_CONNECT_CON_THRESHOLD 20.0/60/60
#define ID_CONNECT_CON_THRESHOLD2 30.0
#endif

#ifndef CHECK_CONNECT /*�Ƿ�������Ƶ�ʲ�ban*/
#define CHECK_CONNECT
#else
#if CHECK_CONNECT == 0
#undef CHECK_CONNECT
#endif
#endif

#ifndef ZMODEM_RATE
#define ZMODEM_RATE 5000
#endif
#if ZMODEM_RATE == 0
#undef ZMODEM_RATE
#endif

#ifndef QUOTED_LINES
#define QUOTED_LINES 10
#endif

#ifndef MAXnettyLN
#define MAXnettyLN      5       /* lines of  activity board  */
#endif

#ifndef QUOTELEV
#define QUOTELEV 0
#endif

#ifndef FIRSTARTICLE_SIGN
#define FIRSTARTICLE_SIGN "��"
#endif

#ifndef BONLINE_LOGDIR /* ���� log �� BBSHOME �µĴ��Ŀ¼ */
#define BONLINE_LOGDIR "bonlinelog"
#endif


#ifndef USERD_PORT
#define USERD_PORT  60001
#endif

#define ISV4ADDR(addr) ((addr.s6_addr[0]==0)&&(addr.s6_addr[1]==0)&& \
	(addr.s6_addr[2]==0)&&(addr.s6_addr[3]==0)&&(addr.s6_addr[4]==0)&& \
	(addr.s6_addr[5]==0)&&(addr.s6_addr[6]==0)&&(addr.s6_addr[7]==0)&& \
	(addr.s6_addr[8]==0)&&(addr.s6_addr[9]==0)&& \
	(addr.s6_addr[10]==0xff)&&(addr.s6_addr[11]==0xff))



/* old bbsconfig.h */

/* 
   Turn this on to allow users to create their own accounts by typing 'new'
   at the "Enter userid:" prompt. Comment out to restrict access to accounts
   created by the Sysop (see important note in README.install). 
*/
#define LOGINASNEW 1 

#define MAXSIGLINES    6 /* max. # of lines appended for post signature */

/*Define this for Use Notepad.*/
#define USE_NOTEPAD 1

/* end old bbsconfig.h */

#if !defined(DELETE_RANGE_RESERVE_DIGEST) && defined(FREE)
#define DELETE_RANGE_RESERVE_DIGEST 1
#endif /* !DELETE_RANGE_RESERVE_DIGEST && FREE */

#if defined(DELETE_RANGE_RESERVE_DIGEST) && (DELETE_RANGE_RESERVE_DIGEST == 0)
#undef DELETE_RANGE_RESERVE_DIGEST
#endif /* DELETE_RANGE_RESERVE_DIGEST && DELETE_RANGE_RESERVE_DIGEST == 0 */

#endif /* __SYS_DEFAULT_H_ */

