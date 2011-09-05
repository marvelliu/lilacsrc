#include "bbs.h"

#ifdef USE_DEFAULT_ALPHABET
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
#endif

#ifdef USE_DEFAULT_PERMSTRINGS
const char * const permstrings[] = {
        "����Ȩ��",             /* PERM_BASIC */
        "����������",           /* PERM_CHAT */
        "������������",         /* PERM_PAGE */
        "��������",             /* PERM_POST */
        "ʹ����������ȷ",       /* PERM_LOGINOK */
        "ʵϰվ��",         /* PERM_BMMANAGER */
        "������",               /* PERM_CLOAK */
        "�ɼ�����",             /* PERM_SEECLOAK */
        "�����ʺ�",         /* PERM_XEMPT */
        "�༭ϵͳ����",         /* PERM_WELCOME */
        "����",                 /* PERM_BOARDS */
        "�ʺŹ���Ա",           /* PERM_ACCOUNTS */
        "ˮľ������",       /* PERM_CHATCLOAK */
        "�������Ȩ��",           /* PERM_DENYRELAX */
        "ϵͳά������Ա",       /* PERM_SYSOP */
        "Read/Post ����",       /* PERM_POSTMASK */
        "�������ܹ�",           /* PERM_ANNOUNCE*/
        "�������ܹ�",           /* PERM_OBOARDS*/
        "������ܹ�",         /* PERM_ACBOARD*/
        "���� ZAP(������ר��)", /* PERM_NOZAP*/
        "������OP(Ԫ��Ժר��)", /* PERM_CHATOP */
        "ϵͳ�ܹ���Ա",         /* PERM_ADMIN */
        "�����ʺ�",           /* PERM_HONOR*/
        "����Ȩ�� 5",           /* PERM_UNUSE?*/
        "�ٲ�ίԱ",           /* PERM_JURY*/
        "����Ȩ�� 7",           /* PERM_UNUSE?*/
        "��ɱ������",        /*PERM_SUICIDE*/
        "����ר���ʺ�",           /* PERM_COLLECTIVE*/
        "��ϵͳ���۰�",           /* PERM_UNUSE?*/
        "���Mail",           /* PERM_DENYMAIL*/

};
#endif

#ifdef NEW_HELP
#ifdef HAVE_DEFAULT_HELPMODE
const char * const helpmodestr[] = {
	"�����б�",
	"���˶�����",
	"�����б�",
	"������",
	"�༭",
	"����",
	"ͶƱ",
	"�鿴���ߺ���/�鿴�����û�",
	"����",
	"ģ��",
	"�������",
	"�ǿ�ս����",
	"ɱ��",
	"�ż�",
	NULL
};
#endif
#endif

#ifdef SMS_SUPPORT

const char * const user_smsdefstr[] = {
	"������ʱתsms���Լ��ֻ�(�Լ���Ǯ)",
	NULL
};

#endif

#ifdef USE_DEFAULT_DEFINESTR
/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
const char * const user_definestr[] = {
    "�����",                 /* DEF_ACBOARD */
    "ʹ�ò�ɫ",                 /* DEF_COLOR */
    "�༭ʱ��ʾ״̬��",         /* DEF_EDITMSG */
    "������������ New ��ʾ",    /* DEF_NEWPOST */
    "ѡ����ѶϢ��",             /* DEF_ENDLINE */
    "��վʱ��ʾ��������",       /* DEF_LOGFRIEND */
    "�ú��Ѻ���",               /* DEF_FRIENDCALL */
    "ʹ���Լ�����վ����",       /* DEF_LOGOUT */
    "��վʱ��ʾ����¼",         /* DEF_INNOTE */
    "��վʱ��ʾ����¼",         /* DEF_OUTNOTE */
    "ѶϢ��ģʽ��������/����",  /* DEF_NOTMSGFRIEND */
    "�˵�ģʽѡ��һ��/����",  /* DEF_NORMALSCR */
    "�Ķ������Ƿ�ʹ���ƾ�ѡ��", /* DEF_CIRCLE */
    "�Ķ������α�ͣ춵�һƪδ��",       /* DEF_FIRSTNEW */
    "��Ļ����ɫ�ʣ�һ��/�任",  /* DEF_TITLECOLOR */
    "���������˵�ѶϢ",         /* DEF_ALLMSG */
    "���ܺ��ѵ�ѶϢ",           /* DEF_FRIENDMSG */
    "�յ�ѶϢ��������",         /* DEF_SOUNDMSG */
    "��վ��Ļ�����ѶϢ",       /* DEF_MAILMSG */
    "������ʱʵʱ��ʾѶϢ",     /*"���к�����վ��֪ͨ",    DEF_LOGININFORM */
    "�˵�����ʾ������Ϣ",       /* DEF_SHOWSCREEN */
    "��վʱ��ʾʮ������",       /* DEF_SHOWHOT */
    "��վʱ�ۿ����԰�",         /* DEF_NOTEPAD */
    "����ѶϢ���ܼ�: Enter/Esc",        /* DEF_IGNOREMSG */
    "ʹ�ø�������",                   /* DEF_HIGHCOLOR */
    "�ۿ�����ͳ�ƺ�ף����", /* DEF_SHOWSTATISTIC Haohmaru 98.09.24 */
    "δ�����ʹ�� *",           /* DEF_UNREADMARK Luzi 99.01.12 */
    "ʹ��GB���Ķ�",             /* DEF_USEGB KCN 99.09.03 */
    "�Ժ��ֽ������ִ���",  /* DEF_SPLITSCREEN 2002.9.1 */
    "��ʾ��ϸ�û�����(wForum)",  /*DEF_SHOWDETAILUSERDATA 2003.7.31 */
    "��ʾ��ʵ�û�����(wForum)", /*DEF_SHOWREALUSERDATA 2003.7.31 */
	"",
    "����ip",                 /* DEF_HIDEIP */
    "����ʾ�����ֽ���"        /* DEF_SHOWSIZE */
};
#endif

#ifdef USE_DEFAULT_MAILBOX_PROP_STR
const char * const mailbox_prop_str[] =
{
	"����ʱ�����ż���������",
	"ɾ���ż�ʱ�����浽������",
	"���水 'v' ʱ����: �ռ���(OFF) / ����������(ON)",
};
#endif

#ifdef USE_DEFAULT_LEVELCHAR
int uleveltochar(char *buf, struct userec *lookupuser)
{                               /* ȡ�û�Ȩ������˵�� Bigman 2001.6.24 */
    unsigned lvl;
    char userid[IDLEN + 2];

    lvl = lookupuser->userlevel;
    strncpy(userid, lookupuser->userid, IDLEN + 2);

    if (lookupuser->title!=0) {
        strcpy(buf,get_user_title(lookupuser->title));
        if (buf[0]!=0) return 0;
    }
    if (!(lvl & PERM_BASIC) && !(lookupuser->flags & GIVEUP_FLAG)) {
        strcpy(buf, "����");
        return 0;
    }
/*    if( lvl < PERM_DEFAULT )
    {
        strcpy( buf, "- --" );
        return 1;
    }
*/

    /* Bigman: �������Ĳ�ѯ��ʾ 2000.8.10 */
    /*if( lvl & PERM_ZHANWU ) strcpy(buf,"վ��"); */
    if ((lvl & PERM_ANNOUNCE) && (lvl & PERM_OBOARDS))
        strcpy(buf, "վ��");
    else if (lvl & PERM_JURY)
        strcpy(buf, "�ٲ�");    /* stephen :�������Ĳ�ѯ"�ٲ�" 2001.10.31 */
    else if (lvl & PERM_BMAMANGER)
        strcpy(buf, "ʵϰվ��");
    else if (lvl & PERM_COLLECTIVE)
        strcpy(buf, "ר��");
    else if (lvl & PERM_CHATCLOAK)
        strcpy(buf, "Ԫ��");
    else if (lvl & PERM_CHATOP)
        strcpy(buf, "ChatOP");
    else if (lvl & PERM_BOARDS)
        strcpy(buf, "����");
    else if (lvl & PERM_HORNOR)
        strcpy(buf, "����");
    /* Bigman: �޸���ʾ 2001.6.24 */
    else if (lvl & (PERM_LOGINOK)) {
        if (lookupuser->flags & GIVEUP_FLAG)
            strcpy(buf, "����");
        else if (!(lvl & (PERM_CHAT)) || !(lvl & (PERM_PAGE)) || !(lvl & (PERM_POST)) 
        		|| (lvl & (PERM_DENYMAIL)) || (lvl & (PERM_DENYRELAX)))
            strcpy(buf, "����");
        else
            strcpy(buf, "�û�");
    } else if (lookupuser->flags & GIVEUP_FLAG)
        strcpy(buf, "����");
    else if (!(lvl & (PERM_CHAT)) && !(lvl & (PERM_PAGE)) && !(lvl & (PERM_POST)))
        strcpy(buf, "����");
    else
        strcpy(buf, "����");

/*    else {
        buf[0] = (lvl & (PERM_SYSOP)) ? 'C' : ' ';
        buf[1] = (lvl & (PERM_XEMPT)) ? 'L' : ' ';
        buf[2] = (lvl & (PERM_BOARDS)) ? 'B' : ' ';
        buf[3] = !(lvl & (PERM_POST)) ? 'p' : ' ';
        if( lvl & PERM_ACCOUNTS ) buf[3] = 'A';
        if( lvl & PERM_SYSOP ) buf[3] = 'S'; 
        buf[4] = '\0';
    }
*/

    return 1;
}
#endif

#ifdef USE_DEFAULT_MODE

#include "modes.h"

char *ModeType(mode)
    int mode;
{
    switch (mode) {
    case IDLE:
        return "";
    case NEW:
        return "��վ��ע��";
    case LOGIN:
        return "���뱾վ";
    case CSIE_ANNOUNCE:
        return "��ȡ����";
//    case CSIE_TIN:
//        return "ʹ��TIN";
//    case CSIE_GOPHER:
//        return "ʹ��Gopher";
    case MMENU:
        return "���˵�";
    case ADMIN:
        return "ϵͳά��";
    case SELECT:
        return "ѡ��������";
    case READBRD:
        return "���������";
    case READNEW:
        return "�Ķ�������";
    case READING:
        return "�Ķ�����";
    case POSTING:
        return "��������";
    case MAIL:
        return "�ż�ѡ��";
    case SMAIL:
        return "������";
    case RMAIL:
        return "������";
    case TMENU:
        return "̸��˵����";
    case LUSERS:
        return "��˭������";
    case FRIEND:
        return "�����Ϻ���";
//    case MONITOR:
//        return "�࿴��";
    case QUERY:
        return "��ѯ����";
    case TALK:
        return "����";
    case PAGE:
        return "��������";
//    case CHAT2:
//        return "�λù���";
    case CHAT1:
        return "��������";
//    case CHAT3:
//        return "����ͤ";
//    case CHAT4:
//        return "�ϴ�������";
//    case IRCCHAT:
//        return "��̸IRC";
    case LAUSERS:
        return "̽������";
    case XMENU:
        return "ϵͳ��Ѷ";
    case VOTING:
        return "ͶƱ";
    case BBSNET:
        return "ϵͳά��"; //ft
	/*
        return "��������";
	*/
//    case EDITWELC:
//        return "�༭ Welc";
    case EDITUFILE:
        return "�༭����";
    case EDITSFILE:
        return "ϵͳ����";
        /*        case  EDITSIG:  return "��ӡ";
           case  EDITPLAN: return "��ƻ�"; */
//    case ZAP:
//        return "����������";
//    case EXCE_MJ:
//        return "Χ������";
//    case EXCE_BIG2:
//        return "�ȴ�Ӫ";
//    case EXCE_CHESS:
//        return "���Ӻ���";
    case NOTEPAD:
        return "���԰�";
    case GMENU:
        return "������";
//    case FOURM:
//        return "4m Chat";
//    case ULDL:
//        return "UL/DL";
    case MSGING:
        return "ѶϢ��";
    case USERDEF:
        return "�Զ�����";
    case EDIT:
        return "�޸�����";
    case OFFLINE:
        return "��ɱ��..";
    case EDITANN:
        return "���޾���";
    case WEBEXPLORE:
        return "Web���";
//    case CCUGOPHER:
//        return "��վ����";
    case LOOKMSGS:
        return "�쿴ѶϢ";
    case WFRIEND:
        return "Ѱ������";
    case LOCKSCREEN:
        return "��Ļ����";
    case IMAIL:
	return "��վ������";
    case GIVEUPNET:
        return "������..";
    case SERVICES:
        return "��������..";
	case FRIENDTEST:
        return "������Ϭ";
    case CHICKEN:
        return "�ǿ�ս����";
    case KILLER:
        return "ɱ����Ϸ";
    case CALENDAR:
        return "������";
    case CALENEDIT:
        return "�ռǱ�";
    case DICT:
        return "���ֵ�";
    case CALC:
        return "������";
    case SETACL:
        return "��¼����";
    case EDITOR:
        return "�༭��";
	case HELP:
		return "����";
	case POSTTMPL:
		return "ģ�巢��";
	case TETRIS:
        return "����˹����";
	case WINMINE:
        return "ɨ��";
    case PC:
        return "�����ļ�";
    case QUERYBOARD:
        return "��ѯ����";
    case BLOG:
        return "BLOG���";
    case POSTCROSS:
        return "ת������";
    default:
        return "ȥ������!?";
    }
}

#endif

#ifdef USE_DEFAULT_MULTILOGIN_CHECK

int multilogin_user(struct userec *user, int usernum,int mode)
{
    return 0;
}

#endif

#ifdef USE_DEFAULT_USER_LIFE
#define LIFE_DAY_USER		120
#define LIFE_DAY_YEAR          365
#define LIFE_DAY_LONG		666
#define LIFE_DAY_SYSOP		120
#define LIFE_DAY_NODIE		999
#define LIFE_DAY_NEW		15
#define LIFE_DAY_SUICIDE	15
int compute_user_value(const struct userec *urec)
{
    int value;
    int registeryear;
    int basiclife;

    /* if (urec) has CHATCLOAK permission, don't kick it */
    /* Ԫ�Ϻ������ʺ� �ڲ���ɱ������£� ������999 Bigman 2001.6.23 */
    /* ��������˵�id,sigh */
    if ((urec->userlevel & PERM_HORNOR) && !(urec->userlevel & PERM_LOGINOK))
        return LIFE_DAY_LONG;


    if (((urec->userlevel & PERM_HORNOR) || (urec->userlevel & PERM_CHATCLOAK)) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_NODIE;

    if ((urec->userlevel & PERM_ANNOUNCE) && (urec->userlevel & PERM_OBOARDS))
        return LIFE_DAY_SYSOP;
    /* վ����Ա���������� Bigman 2001.6.23 */

    if ((urec->userlevel & PERM_XEMPT) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_USER; /* �����ʺ� - atppp 20041023 */

    value = (time(0) - urec->lastlogin) / 60;   /* min */
    if (0 == value)
        value = 1;              /* Leeward 98.03.30 */

    /* new user should register in 30 mins */
    if (strcmp(urec->userid, "new") == 0) {
        return (LIFE_DAY_NEW - value) / 60;     /* *->/ modified by dong, 1998.12.3 */
    }

    /* ��ɱ����,Luzi 1998.10.10 */
    if (urec->userlevel & PERM_SUICIDE)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24);
    /**********************/
    if (urec->numlogins <= 3)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24);
    if (!(urec->userlevel & PERM_LOGINOK))
        return (LIFE_DAY_NEW * 24 * 60 - value) / (60 * 24);
    /* if (urec->userlevel & PERM_LONGID)
       return (667 * 24 * 60 - value)/(60*24); */
    registeryear = (time(0) - urec->firstlogin) / 31536000;
    if (registeryear < 2)
        basiclife = LIFE_DAY_USER + 1;
    else if (registeryear >= 5)
        basiclife = LIFE_DAY_LONG + 1;
    else
        basiclife = LIFE_DAY_YEAR + 1;
    return (basiclife * 24 * 60 - value) / (60 * 24);
}
#endif

#ifdef USE_DEFAULT_ANNOUNCE_FILENAME
/**
 * ��������غ�����
 */
int ann_get_postfilename(char *filename, struct fileheader *fileinfo,
						MENU *pm)
{
	char fname[PATHLEN];
	char *ip;

	strcpy(filename, fileinfo->filename);
	sprintf(fname, "%s/%s", pm->path, filename);
	ip = &filename[strlen(filename) - 1];
	while (dashf(fname)) {
		if (*ip == 'Z')
			ip++, *ip = 'A', *(ip + 1) = '\0';
		else
			(*ip)++;
		sprintf(fname, "%s/%s", pm->path, filename);
	}
    return 0;
}
#endif

#ifdef USE_DEFAULT_GET_POSTTIME
/**
 * ������غ�����
 */
time_t get_posttime(const struct fileheader *fileinfo)
{
	return atoi(fileinfo->filename + 2);
}
#endif

#ifdef USE_DEFAULT_SET_POSTTIME
void set_posttime(struct fileheader *fileinfo)
{
	return;
}
#endif

#ifdef USE_DEFAULT_SET_POSTTIME2
void set_posttime2(struct fileheader *dest, struct fileheader *src)
{
	return;
}
#endif

#ifdef USE_DEFAULT_BUILD_BOARD
/**
 * ������ء�
 */
void build_board_structure(const char *board)
{
	return;
}
#endif


#ifdef USE_DEFAULT_MAIL_LIMIT /*�ʼ���������*/
void get_mail_limit(struct userec* user,int *sumlimit,int * numlimit)
{
	/*unlimit*/
	*sumlimit = 9999;
	*numlimit = 9999;
	return;
}
#endif

/* board permissions control */
#ifdef USE_DEFAULT_READ_PERM
int check_read_perm(const struct userec *user, const struct boardheader *board)
{
    if (board == NULL)
        return 0;
    if (user==NULL) {
        if (board->title_level!=0) return 0;
    } else 
    if (!HAS_PERM(user, PERM_OBOARDS)&&board->title_level
        &&(board->title_level!=user->title))
        return 0;


    if (board->level & PERM_POSTMASK || HAS_PERM(user, board->level) || (board->level & PERM_NOZAP)) {
        if (board->flag & BOARD_CLUB_READ) {    /*���ֲ�*/
            if (HAS_PERM(user,PERM_OBOARDS)&&HAS_PERM(user, PERM_SYSOP))
                return 1;
            if (board->clubnum <= 0 || board->clubnum > MAXCLUB)
                return 0;
            if (user->club_read_rights[(board->clubnum - 1) >> 5] & (1 << ((board->clubnum - 1) & 0x1f)))
                return 1;
            else
                return 0;
        }
        return 1;
    }
    return 0;
}
#endif /* USE_DEFAULT_READ_PERM */

#ifdef USE_DEFAULT_SEE_PERM
int check_see_perm(const struct userec* user,const struct boardheader* board)
{
    if (board == NULL)
        return 0;
    if (user==NULL) {
        if (board->title_level!=0) return 0;
    } else
    if (!HAS_PERM(user, PERM_OBOARDS)&&board->title_level
        &&(board->title_level!=user->title))
        return 0;
    if (board->level & PERM_POSTMASK
    	|| ((user==NULL)&&(board->level==0))
    	|| ((user!=NULL)&& HAS_PERM(user, board->level) )
    	|| (board->level & PERM_NOZAP))
	{
        if (board->flag & BOARD_CLUB_HIDE)     /*���ؾ��ֲ�*/
		{
			if (user==NULL) return 0;
			   if (HAS_PERM(user, PERM_OBOARDS))
					return 1;
			   return check_read_perm(user,board);
		}
        return 1;
    }
    return 0;
}

#endif /* USE_DEFAULT_SEE_PERM */

#ifdef SMS_SUPPORT
#ifndef NOT_USE_DEFAULT_SMS_FUNCTIONS
int smsid2uid(char* smsid) {
	return getuser(smsid,NULL);
}

void uid2smsid(struct user_info* uin,char* smsid)
{
	sprintf(smsid,"%s",uin->userid);
}

int uid2smsnumber(struct user_info* uin){
	return uin->uid;
}
int smsnumber2uid(byte number[4]){
	return byte2long(number);
}
#endif
	
#endif

#ifndef HAVE_OWN_USERIP
char *showuserip(struct userec *user, char *ip)
{
		return ip;
}
#endif

int def_list(long long XX){
	if (XX < DEF_HIDEIP)
		return 0;
	return 1;
}
