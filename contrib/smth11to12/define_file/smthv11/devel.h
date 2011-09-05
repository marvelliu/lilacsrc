#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define BBSUID 			501
#define BBSGID 			501

/*#define USE_TMPFS 1*/
#define TMPFSROOT "cache"
#define SMTH    1               /*��ΪSMTH�Ĳ���վ�� */
#define HAVE_TSINGHUA_INFO_REGISTER 0   /* by wisi@Tongji */

/** ʹ��ȱʡ��FILEHeader�ṹ*/
#define HAVE_FILEHEADER_DEFINE
/** ����ʹ��ȱʡ�ĺ��� */
#define USE_DEFAULT_SECODE
#define USE_DEFAULT_ALPHABET
#define USE_DEFAULT_PERMSTRINGS
#define USE_DEFAULT_DEFINESTR
#define USE_DEFAULT_MAILBOX_PROP_STR
#define USE_DEFAULT_LEVELCHAR
#define USE_DEFAULT_MODE
#define USE_DEFAULT_MULTILOGIN_CHECK
#define USE_DEFAULT_USER_LIFE
#define USE_DEFAULT_ANNOUNCE_FILENAME
#define USE_DEFAULT_GET_POSTTIME
#define USE_DEFAULT_SET_POSTTIME
#define USE_DEFAULT_SET_POSTTIME2
#define USE_DEFAULT_BUILD_BOARD
#define USE_DEFAULT_MAIL_LIMIT  /*�ʼ��������� */
#define USE_DEFAULT_READ_PERM
#define USE_DEFAULT_SEE_PERM
#define REGISTER_TSINGHUA_WAIT_TIME (240*60)
#define WWW_MAX_LOGIN 128

#define NEW_COMERS 1            /* by wisi@Tongji */
#define HAVE_BIRTHDAY 1         /* by wisi@Tongji */
#define HAVE_TEMPORARY_NICK 1   /* by wisi@Tongji */
#define HAVE_FRIENDS_NUM 1      /* by wisi@Tongji */
#define CNBBS_TOPIC 1           /* by wisi@Tongji */
#define ANONYMOUS_DEFAULT 1     /* by wisi@Tongji */
#define CONV_PASS 1

#endif
