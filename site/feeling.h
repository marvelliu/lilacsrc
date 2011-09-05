#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define CONV_PASS		1
#define NEW_COMERS		1       /* ע����� newcomers ���Զ����� */
#define HAPPY_BBS		0
#define HAVE_COLOR_DATE		1
#define HAVE_FRIENDS_NUM	1
#define HAVE_REVERSE_DNS	0
#define CHINESE_CHARACTER	1
#define CNBBS_TOPIC		1       /* �Ƿ��ڽ�վ��������ʾ cn.bbs.* ʮ�����Ż��� */
#define USE_NCM_PATCH		1	/* innbbsd nocem support ת tw.bbs.* �߱ر� */
#define MAIL2BOARD		1       /* �Ƿ�����ֱ�� mail to any board */
#define MAILOUT			1       /* �Ƿ�������վ���������� */
#define MANUAL_DENY		0       /*�Ƿ������ֶ���� */
#define BBS_SERVICE_DICT	1
#define HAVE_PERSONAL_DNS	0       /*������������ */

#define BUILD_PHP_EXTENSION	0       /*��php lib���php extension */

#define HAVE_WFORUM		1
#define SMTH			1
#define FILTER			1

#define IDLE_TIMEOUT    (60*60*3)

#define BBSUID 			9999
#define BBSGID 			999

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 9
#define BBS_PAGE_SIZE 20

#define DEFAULTBOARD    	"sysop"
#define FILTER_BOARD        "Filter"
#define SYSMAIL_BOARD       "sysmail"
#define BLESS_BOARD "Blessing"

#define MAXUSERS  		20236
#define MAXCLUB			128
#define MAXBOARD  		600
#define MAXACTIVE 		256
/* remeber: if MAXACTIVE>46656 need change get_telnet_sessionid,
    make the number of session char from 3 to 4
    */
#define MAX_GUEST_NUM		256
#define WWW_MAX_LOGIN		256

#define POP3PORT		1100
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

#define	REGISTER_WAIT_TIME	(0)
#define	REGISTER_WAIT_TIME_NAME	"1 ����"

#define MAIL_BBSDOMAIN      "feeling.cn-bbs.org"
#define MAIL_MAILSERVER     "127.0.0.1:25"

#define NAME_BBS_ENGLISH	"feeling.cn-bbs.org"
#define	NAME_BBS_CHINESE	"��ܰС��"
#define NAME_BBS_NICK		"BBS վ"

#define BBS_FULL_NAME "��ܰС��"

#define FOOTER_MOVIE		"��  ӭ  Ͷ  ��"
/*#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"*/
#define ISSUE_LOGIN		"���PC  ��21����˻�ָ��������"
#define ISSUE_LOGOUT		"����������"

#define NAME_USER_SHORT		"�û�"
#define NAME_USER_LONG		"��ܰС���û�"
#define NAME_SYSOP		"System Operator"
#define NAME_BM			"����"
#define NAME_POLICE		"����"
#define	NAME_SYSOP_GROUP	"վ����"
#define NAME_ANONYMOUS		"�Ұ���ܰС��!"
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

/** ʹ��ȱʡ��FILEHeader�ṹ*/
#define HAVE_FILEHEADER_DEFINE

#define GET_POSTFILENAME(x,y) get_postfilename(x,y,1)
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#define VALID_FILENAME(x) valid_filename(x,1)
#define POSTFILE_BASENAME(x) (((char *)(x))+2)
#define MAILFILE_BASENAME(x) (x)

#define USE_DEFAULT_MODE 1

/**
attach define
*/
#define ATTACHTMPPATH "boards/_attach"

#define CHECK_IP_LINK 1

#define PERSONAL_CORP

#define NEW_HELP
#define HAVE_DEFAULT_HELPMODE

#define COMMEND_ARTICLE "Recommend"

#define HAVE_OWN_USERIP
#define SHOW_USERIP(user,x) showuserip(user,x)

#define AUTO_CHECK_REGISTER_FORM        //�Զ�ͨ��ע�ᵥ

#define QUOTED_LINES 3
#define QUOTELEV 0

#define BIRTHFILENUM 12
#define BIRTHFILEPATH "0Announce/groups/GROUP_0/sysop/birthfile"
 
#define MYUNLINK_BACKUPDIR "0Announce/backup"
 
#define BMSLOG
#define NEWPOSTLOG
#define NEWBMLOG

#define NEWPOSTSTAT

#define	SAVELIVE
#endif
