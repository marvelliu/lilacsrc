#define VERSIONFILE "etc/version"

/*
MODULE_VERSION(name,description,allow lowest version,major,minjor)
����ģ��汾
allow lowest �������ʲô�汾�����Ϊ�գ�
        ��ʾ���뾫ȷƥ�䣬���Ϊ"0.0"���Ͳ���Ҫƥ��

MODULE_OPTION(description,major)
����ģ��ѡ��
*/

BEGIN_DEFINE
MODULE_VERSION("versioncontrol","version control module",NULL,1,2)

MODULE_VERSION("bbsd","BBS DAEMON","0.0",1,2)
MODULE_VERSION("sshbbsd","BBS SSH DAEMON","0.0",1,2)
MODULE_VERSION("www","BBS web Module","0.0",1,2)

MODULE_VERSION("public","public data in share memory",null,1,0)
MODULE_OPTION("banner in the bottom",FLOWBANNER)

MODULE_VERSION("password",".PASSWDS file",null,1,0)
MODULE_OPTION("convert password to md5 format",CONV_PASS)

MODULE_VERSION("userdata","user data file in user home",null,1,0)
MODULE_OPTION("have birthday field",HAVE_BIRTHDAY)

MODULE_VERSION("userinfo","user session data in share memory",null,1,0)
MODULE_OPTION("wforum data",HAVE_WFORUM)
MODULE_OPTION("fiend multi-group field",FRIEND_MULTI_GROUP)

MODULE_VERSION("friends","friend list store in friends file",null,1,0)
MODULE_OPTION("fiend multi-group field",FRIEND_MULTI_GROUP)

MODULE_VERSION("boardheader","board information file",null,1,0)
MODULE_OPTION("banner in the bottom",FLOWBANNER)

MODULE_VERSION("boardstatus","board information store in share memory",null,1,0)
MODULE_OPTION("wforum data",,HAVE_WFORUM)
END_DEFINE
