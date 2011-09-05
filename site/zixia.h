#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define ZIXIA		1
/*
 * Ŀǰ ZIXIA �����������Ĺ��ܱ仯
 * 1. ��ע�ᵥ��ʱ����ʾ��ϸ��Ϣ
 * 2. ���������б� ~ ��ʾ���� URL
 * 3. �������ַ����������
 * 4. web��ֻҪ��Ȩ�޽����ڲ������������Ķ���Ӧ�ľ���������ʹ.Names����û���������
 * 5. ��վ����ʾ�Ǹ���һ����ף����/У���ȵ�
 * 6. �ż��б� s ��ʾÿ���ż��Ĵ�С
 * 7. ��վѡ�񲻼Ļ�ѶϢ�Ļ���Ҫɾ��ѶϢ
 * 8. web �������ӹ���
 */

#define ACBOARD_BNAME "notepad"

#define NEW_COMERS 		1	/* ע����� newcomers ���Զ����� */
#define OWNSENDMAIL "/usr/sbin/sendmail"
#define HAPPY_BBS		0
#define HAVE_COLOR_DATE		1
#define HAVE_FRIENDS_NUM	0
#define HAVE_REVERSE_DNS	0
#define CHINESE_CHARACTER	1
#define CNBBS_TOPIC		0	/* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#define MAIL2BOARD		0	/* �Ƿ�����ֱ�� mail to any board */
#define MAILOUT			0	/* �Ƿ�������վ���������� */
#define MANUAL_DENY         0   /*�Ƿ������ֶ����*/
#define BBS_SERVICE_DICT    1

#define BUILD_PHP_EXTENSION 1   /*��php lib���php extension */

#define HAVE_WFORUM 1

#define SMTH			0		/* SMTHר�д��� */
#define FILTER			1		/* ʹ���������ݹ��� */

#define MYUNLINK_BACKUPDIR "0Announce/backup"

#define ALLOW_PUBLIC_USERONBOARD 1

#undef SITE_HIGHCOLOR

#define IDLE_TIMEOUT    (60*20) 

#define BBSUID 			80//9999
#define BBSGID 			80//99

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 10
#define BBS_PAGE_SIZE 20

#define DEFAULTBOARD    	"zixia.net"//test
#define FILTER_BOARD        "Filter"
#define SYSMAIL_BOARD       "sysmail"
#define MAXUSERS  		150000 //150,000
#define MAXBOARD  		1024//400
#define MAXCLUB                 128
#define MAXACTIVE 		10000  //3000
/* remeber: if MAXACTIVE>46656 need change get_telnet_sessionid,
    make the number of session char from 3 to 4
    */
#define MAX_GUEST_NUM		1000
#define WWW_MAX_LOGIN		5000 /* ���www�û����� */

#define POP3PORT		3110	//110
#define POP3SPORT		995

/* ASCIIArt, by czz, 2002.7.5 */
#define	LENGTH_SCREEN_LINE	256	//220
#define	LENGTH_FILE_BUFFER 	260	//160
#define	LENGTH_ACBOARD_BUFFER	300	//150
#define	LENGTH_ACBOARD_LINE 	300	//80

#define LIFE_DAY_USER		365	//120
#define LIFE_DAY_LONG		666	//666
#define LIFE_DAY_SYSOP		500	//120
#define LIFE_DAY_NODIE		999	//120
#define LIFE_DAY_NEW		30	//15
#define LIFE_DAY_SUICIDE	15	//15

#define DAY_DELETED_CLEAN	97	//20
#define SEC_DELETED_OLDHOME	0 	/*  3600*24*30��ע�����û������������û���Ŀ¼������ʱ��*/

#define	REGISTER_WAIT_TIME	(1) // (72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"1����" //72 Сʱ

#define MAIL_BBSDOMAIN      "bbs.zixia.net"
#define MAIL_MAILSERVER     "127.0.0.1:25"

#define NAME_BBS_ENGLISH	"wforum.zixia.net" //smth.org
#define	NAME_BBS_CHINESE	"������" //ˮľ�廪
#define NAME_BBS_NICK		"��ͷ��" // BBS վ
#define BBS_FULL_NAME		"BBS ������վ"

#define FOOTER_MOVIE		"  ��  ͷ  ��  " // ��  ӭ  Ͷ  ��
#define ISSUE_LOGIN		"��վʹ��һ̨X86������" //��վʹ����⹫˾������ݷ�����
#define ISSUE_LOGOUT		"�������_�ܣ�" //����������

#define NAME_USER_SHORT		"��ͷ����" //�û�
#define NAME_USER_LONG		"��ͷ����" //"ˮľ�û�"
#define NAME_SYSOP		"ǿ��ͷ" //"System Operator"
#define NAME_BM			"�����ҵ�" //����
#define NAME_POLICE		"����" //"����"
#define	NAME_SYSOP_GROUP	"��ͷ��߲�" //"վ����"
#define NAME_ANONYMOUS		"Do you zixia!?" //"ˮĸ���� Today!"
#define NAME_ANONYMOUS_FROM	"ˮ����" //������ʹ�ļ�
#define ANONYMOUS_DEFAULT 0

#define NAME_MATTER		"����" //վ��
#define NAME_SYS_MANAGE		"���ڹ���" //"ϵͳά��"
#define NAME_SEND_MSG		"����ͷ" // "��ѶϢ"
#define NAME_VIEW_MSG		"�鸫ͷ" // "��ѶϢ"

#define CHAT_MAIN_ROOM		"zixia" //main
#define	CHAT_TOPIC		"��˿����������ˮ�������" //"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** ��˿����Ҫ�Ҵ��� ***" //"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"����"//"������"
#define	CHAT_SERVER		"��˿��" //"����㳡"
#define CHAT_MSG_QUIT		"�ص������ǰ" //"����ϵͳ"
#define CHAT_OP			"����" //"������ op"
#define CHAT_SYSTEM		"����" //"ϵͳ"
#define	CHAT_PARTY		"����" // "���"

#define DEFAULT_NICK		"����"
//#define LOCAL_ARTICLE_DEFAULT 	0	//ȱʡת��

#define MSG_ERR_USERID		"�ţ������ͷ��˭��..."
#define LOGIN_PROMPT		"\033[s����\033[4D\033[u��������"
#define PASSWD_PROMPT		"\033[s����\033[4D\033[u��ͷ����"

#define PERM_NEWBOARD   PERM_OBOARDS


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
    int unused[2];
    int altar;
    int unused2[4];
};



/**
 * ������ز��֡�
 */
#define STRLEN          80
#define BM_LEN 60
#define FILENAME_LEN 20
#define OWNER_LEN 30
#define ARTICLE_TITLE_LEN 60
typedef struct fileheader {     /* This structure is used to hold data in */
    char filename[FILENAME_LEN];        /* the DIR files */
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
    int posttime;
    unsigned int attachment;
    char title[ARTICLE_TITLE_LEN];
    unsigned char accessed[4];
} fileheader;

typedef struct fileheader fileheader_t;

#define GET_POSTFILENAME(x,y) get_postfilename(x,y,0)
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#define VALID_FILENAME(x) valid_filename(x,0)


#define HAVE_OWN_USERIP
#define SHOW_USERIP(user,x) showuserip(user,x)

//#define SMS_SUPPORT

/**
attach define
*/
#define ATTACHTMPPATH "boards/_attach"

/* zixia addon */
/*asing add*/

#define PERCENT_SIGN_SUPPORT

int NoSpaceBdT(char *title);

#define DENYPIC "0Announce/groups/AxFaction/heaven/denypic"
#define DENY_DESC_AUTOFREE      "������������¯����"
#define DENY_DESC_NOAUTOFREE  	"����ѹ������ɽ��"
#define DENY_BOARD_AUTOFREE     "��������������Ȩ������������¯����"
#define DENY_BOARD_NOAUTOFREE   "��ѹ������ɽ��"
#define DENY_NAME_SYSOP         "��������"

int CountDenyPic(char *fn);
int GetDenyPic(FILE* denyfile,char * fn,unsigned int i,int count);

int m_altar(void);
struct userec;
int uinfo_altar(struct userec *u);
struct boardheader;
int board_change_report(char *log, struct boardheader *old, struct boardheader *new);

#endif
