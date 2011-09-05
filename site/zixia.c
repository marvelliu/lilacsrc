#include "bbs.h"

const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const char * const permstrings[] = {
        "����Ȩ��",             /* PERM_BASIC */
        "����������",           /* PERM_CHAT */
        "������������",         /* PERM_PAGE */
        "��������",             /* PERM_POST */
        "ʹ����������ȷ",       /* PERM_LOGINOK */
        //"��ֹ��������",         /* PERM_DENYPOST */
        "վ���",         /* PERM_BMMANAGER */
        "������",               /* PERM_CLOAK */
        "�ɼ�����",             /* PERM_SEECLOAK */
        "�����ʺ�",         /* PERM_XEMPT */
        "�༭ϵͳ����",         /* PERM_WELCOME */
        "����",                 /* PERM_BOARDS */
        "�ʺŹ���Ա",           /* PERM_ACCOUNTS */
        NAME_BBS_CHINESE "������",       /* PERM_CHATCLOAK */
        //"ͶƱ����Ա",           /* PERM_OVOTE */
        "�������Ȩ��",           /* PERM_DENYRELAX */
        "ϵͳά������Ա",       /* PERM_SYSOP */
        "Read/Post ����",       /* PERM_POSTMASK */
        "�������ܹ�",           /* PERM_ANNOUNCE*/
        "�������ܹ�",           /* PERM_OBOARDS*/
        "������ܹ�",         /* PERM_ACBOARD*/
        "���� ZAP(������ר��)", /* PERM_NOZAP*/
        "������OP(Ԫ��Ժר��)", /* PERM_CHATOP */
        "ϵͳ�ܹ���Ա",         /* PERM_ADMIN */
        "��73��",          	/* PERM_HORNOR*/
        "�����ܾ�����",         /* PERM_SECANC*/
        "��AKA��",           /* PERM_JURY*/
        "��Sexy��",           /* PERM_SEXY*/
        "��ɱ������",           /* PERM_SUICIDE?*/
        "�������",           /* PERM_MM*/
        "��ϵͳ���۰�",           /* PERM_DISS*/
        "���Mail",           /* PERM_DENYMAIL*/
};

const char * const groups[] = {
	"AxFaction",
	"zixia",
	"Factions",
	"Entertain",
	"Watering",
	"Poem",
	"GoWest",
	"DouFuGirl",
  	"PandoraBox",
  	"Reserve",
         NULL
};

const char seccode[SECNUM][5] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

const char secname[SECNUM][2][20]={
	{"�� ͷ ��", "[�ڰ�/ϵͳ]"},
	{"������", "[��ѧ/����]"},
	{"��С�ֶ�", "[����/У��]"},
	{"�Ժ�����", "[����/����]"},
	{"��Ϸ����", "[��Ϸ/����]"},
	{"����Ū��", "[����/�Ļ�]"},
	{"����֮·", "[����/ȡ��]"},
	{"������ʩ", "[��Ϣ/��ҵ]"},
	{"��������", "[����/����]"},
	{"��ʥȡ��", "[רҵ/����]"},
};


int uleveltochar( char* buf, struct userec *lookupuser )
{
	unsigned lvl;
	char userid[IDLEN+2];
	
	lvl = lookupuser->userlevel;
	strncpy( userid, lookupuser->userid, IDLEN+2 );

	// buf[10], buf ��� 4 ������ + 1 byte ��\0��β��
	//���� level
	//
        if ( ! (lvl & PERM_BASIC) ) strcpy( buf, "Ϲ��" ); 
        else if( lvl == PERM_BASIC ) strcpy( buf, "����" );
        else if( !( lvl & PERM_POST ) ) strcpy( buf, "�ư�" ); 
        else if( lvl < PERM_DEFAULT ) strcpy( buf, "����" );
    	//else if( lvl & PERM_SYSOP ) strcpy(buf,"����");
      	else if( lvl & PERM_MM && lvl & PERM_CHATCLOAK ) strcpy(buf,"��������");
      	else if( lvl & PERM_MM ) strcpy(buf,"������ü");
      	else  if( lvl & PERM_CHATCLOAK ) strcpy(buf,"�޵�ţʭ");
      	else if  ( lvl & PERM_BOARDS ) strcpy(buf,"�����ҵ�");
      	else strcpy(buf, NAME_USER_SHORT);

    if (lookupuser->title != 0) {
        strcpy(buf, get_user_title(lookupuser->title));
    }
#if 0
	//����˵�������� level
    	if( !strcmp(lookupuser->userid,"SYSOP"))
	    strcpy( buf, "ǿ��ͷ" );
    	else if( !strcmp(lookupuser->userid,"netterm") )
	    strcpy( buf, "�ϰ���" );
    	else if( !strcmp(lookupuser->userid,"zixia") )
	    strcpy( buf, "����" );
    	else if( !strcmp(lookupuser->userid,"click") )
	    	strcpy( buf, "С������" );
    	else if( !strcmp(lookupuser->userid,"wuhu") )
	    	strcpy( buf, "�˽�" );
    	else if( !strcmp(lookupuser->userid,"halen") )
	    strcpy( buf, "СƤ����" );
    	else if (!strcmp(lookupuser->userid,"cclu") ||
		!strcmp(lookupuser->userid,"SuperMM") ||
		!strcmp(lookupuser->userid,"SilverSnow") ||
		!strcmp(lookupuser->userid, "busygirl") )
	    strcpy( buf, "�������" );
    	else if( !strcmp(lookupuser->userid,"Bison") )
	    strcpy( buf, "����" );
    	else if( !strcmp(lookupuser->userid,"Roy") )
	    strcpy( buf, "���" );
    	else if( !strcmp(lookupuser->userid,"dwd") )
	    strcpy( buf, "��������" );
    	else if( !strcmp(lookupuser->userid,"birby") )
	    strcpy( buf, "�ֹ�����" );
    	else if( !strcmp(lookupuser->userid,"KCN") )
	    strcpy( buf, "�ϵ�" );
    	else if( !strcmp(lookupuser->userid,"cityhunter") 
		    || !strcmp(lookupuser->userid,"soso")
		    || !strcmp(lookupuser->userid,"Czz")
		    || !strcmp(lookupuser->userid,"flyriver") )
	    strcpy( buf, "ţħ��" );
    	else if( !strcmp(lookupuser->userid,"guest") )
	    strcpy( buf, "����" );
#endif
    	return 1;
}

#include "modes.h"

char *
ModeType(mode)
int     mode;
{
    switch(mode) {
    case IDLE:      return "^@^zz..ZZ" ;
    case NEW:       return "�°���ע��" ;
    case LOGIN:     return "����" NAME_BBS_NICK;
    case CSIE_ANNOUNCE:     return "��ȡ����";
    case CSIE_TIN:          return "ʹ��TIN";
    case CSIE_GOPHER:       return "ʹ��Gopher";
    case MMENU:     return "���˵�";
    case ADMIN:     return NAME_SYS_MANAGE;
    case SELECT:    return "ѡ��������";
    case READBRD:   return "���������";
    case READNEW:   return "�쿴������";
    case  READING:  return "�������";
    case  POSTING:  return "��������" ;
    case MAIL:      return "�ż�ѡ��" ;
    case  SMAIL:    return "������";
    case  RMAIL:    return "������";
    case TMENU:     return "̸��˵����";
    case  LUSERS:   return "��˭������";
    case  FRIEND:   return "�����Ϻ���";
    case  MONITOR:  return "�࿴��";
    case  QUERY:    return "��ѯ����";
    case  TALK:     return "����" ;
    case  PAGE:     return "���а���";
    case  CHAT2:    return "�λù���";
    case  CHAT1:    return CHAT_SERVER "��";
    case  CHAT3:    return "����ͤ";
    case  CHAT4:    return "�ϴ�������";
    case  IRCCHAT:  return "��̸IRC";
    case LAUSERS:   return "̽�Ӱ���";
    case XMENU:     return "ϵͳ��Ѷ";
    case  VOTING:   return "ͶƱ";
    case  BBSNET:   return "����������";
    case  EDITWELC: return "�༭ Welc";
    case EDITUFILE: return "�༭����";
    case EDITSFILE: return NAME_SYS_MANAGE;
    case  EDITSIG:  return "��ӡ";
    case  EDITPLAN: return "��ƻ�";
    case ZAP:       return "����������";
    case EXCE_MJ:   return "Χ������";
    case EXCE_BIG2: return "�ȴ�Ӫ";
    case EXCE_CHESS:return "���Ӻ���";
    case NOTEPAD:   return "���԰�";
    case GMENU:     return "������";
    case FOURM:     return "4m Chat";
    case ULDL:      return "UL/DL" ;
    case MSGING:       return NAME_SEND_MSG;
    case USERDEF:   return "�Զ�����";
    case EDIT:      return "�޸�����";
    case OFFLINE:   return "��ɱ��..";
    case EDITANN:   return "���޾���";
    case CCUGOPHER: return "��վ����";
    case LOOKMSGS:  return NAME_VIEW_MSG;
    case WFRIEND:   return "Ѱ������";
    case LOCKSCREEN:return "��Ļ����";
    case GIVEUPNET:
        return "������..";
    case WEBEXPLORE:return "Web���";
    case SERVICES:    return "��������..";
	case FRIENDTEST:  return "������Ϭ";
    case CHICKEN:
	return "�ǿ�ս����";
    case KILLER:        return "ɱ����Ϸ";
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
    case POSTCROSS:
        return "ת������";
    default: return "ȥ������!?" ;
    }
}

struct count_arg {
    int www_count;
    int telnet_count;
};

int countuser(struct user_info* uinfo,struct count_arg* arg,int pos)
{
    if (uinfo->pid==1)
        arg->www_count++;
    else
        arg->telnet_count++;
    return COUNT;
}

int multilogin_user(struct userec *user, int usernum,int mode)
{
    int logincount;
    int curr_login_num;
    struct count_arg arg;

    bzero(&arg,sizeof(arg));
    logincount = apply_utmpuid((APPLY_UTMP_FUNC)countuser, usernum, &arg);

    if (logincount < 1)
        RemoveMsgCountFile(user->userid);

    if (HAS_PERM(user, PERM_MULTILOG))
        return 0;               /* don't check sysops */
    curr_login_num = get_utmp_number();
    /* Leeward: 97.12.22 BMs may open 2 windows at any time */
    /* Bigman: 2000.8.17 �������ܹ���2������ */
    /* stephen: 2001.10.30 �ٲÿ��Կ��������� */
	/* roy: ����&AKA��ͬѧ���Կ��������� */
    /* atppp: 3 ! 20050323 */
    if ((HAS_PERM(user,PERM_BOARDS) || HAS_PERM(user,PERM_CHATOP)||
        HAS_PERM(user,PERM_JURY) || HAS_PERM(user,PERM_CHATCLOAK)
		|| HAS_PERM(user, PERM_BMAMANGER)  )
        && logincount< 3)
        return 0;

    /* allow multiple guest user */
    if (!strcmp("guest", user->userid)) {
        if (logincount > MAX_GUEST_NUM) {
            return 2;
        }
        return 0;
    } else if (((curr_login_num < 500) && (logincount >= 3)) /*С��500����3��*/
               || ((curr_login_num >= 500) && (logincount >= 2)  /*500������*/
                     && !(((arg.telnet_count==0)&&(mode==0))  /* telnet����Ϊ������ٵ�һ��telnet */
                            || (((arg.www_count==0)&&(mode==1)) ))))       /*user login limit */
        return 1;
    return 0;
}

int old_compute_user_value(const struct userec *urec)
{
    int         value;

    /* if (urec) has CHATCLOAK permission, don't kick it */
	/* Ԫ�Ϻ������ʺ� �ڲ���ɱ������£� ������999 Bigman 2001.6.23 */
    /* 
    * zixia 2001-11-20 ���е���������ʹ�ú��滻��
    * �� smth.h/zixia.h �ж��� 
    * */

    if( urec->userlevel & PERM_MM )
	return LIFE_DAY_SYSOP;
    
    if( ((urec->userlevel & PERM_HORNOR)||(urec->userlevel & PERM_CHATCLOAK )) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_NODIE;

    if ( urec->userlevel & PERM_SYSOP) 
	return LIFE_DAY_SYSOP;
	/* վ����Ա���������� Bigman 2001.6.23 */
	

    value = (time(0) - urec->lastlogin) / 60;    /* min */
    if (0 == value) value = 1; /* Leeward 98.03.30 */

    /* �޸�: �������ʺ�תΪ�����ʺ�, Bigman 2000.8.11 */
    if ((urec->userlevel & PERM_XEMPT) && (!(urec->userlevel & PERM_SUICIDE)) )
    {	if (urec->lastlogin < 988610030)
        return LIFE_DAY_LONG; /* ���û�е�¼���� */
        else
            return (LIFE_DAY_LONG * 24 * 60 - value)/(60*24);
    }
    /* new user should register in 30 mins */
    if( strcmp( urec->userid, "new" ) == 0 ) {
        return (LIFE_DAY_NEW - value) / 60; /* *->/ modified by dong, 1998.12.3 */
    }

    /* ��ɱ����,Luzi 1998.10.10 */
    if (urec->userlevel & PERM_SUICIDE)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value)/(60*24);
    /**********************/
    if(urec->numlogins <= 3)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value)/(60*24);
    if( !(urec->userlevel & PERM_LOGINOK) )
        return (LIFE_DAY_NEW * 24 * 60 - value)/(60*24);
    /* if (urec->userlevel & PERM_LONGID)
         return (667 * 24 * 60 - value)/(60*24); */
    return (LIFE_DAY_USER * 24 * 60 - value)/(60*24);
}

int compute_user_value(const struct userec *urec)
{
    int value;
    int registeryear;
    int basiclife;

    /* if (urec) has CHATCLOAK permission, don't kick it */
    /* Ԫ�Ϻ������ʺ� �ڲ���ɱ������£� ������999 Bigman 2001.6.23 */
    /* 
       * zixia 2001-11-20 ���е���������ʹ�ú��滻��
       * �� smth.h/zixia.h �ж��� 
       * */
    /* ���⴦�����ƶ���cvs ���� */

    if (urec->lastlogin < 1022036050)
        return old_compute_user_value(urec);
    /* ��������˵�id,sigh */
    if ((urec->userlevel & PERM_HORNOR) && !(urec->userlevel & PERM_LOGINOK))
        return LIFE_DAY_LONG;


    if ((urec->userlevel & PERM_ANNOUNCE) && (urec->userlevel & PERM_OBOARDS))
        return LIFE_DAY_SYSOP;
    /* վ����Ա���������� Bigman 2001.6.23 */


    if (((urec->userlevel & PERM_HORNOR) || (urec->userlevel & PERM_CHATCLOAK)) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_NODIE;


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

	/* roy: ���������û�������ΪLIFE_DAY_LONG */
    if (registeryear < 2)
        basiclife = LIFE_DAY_USER + 1;
    else 
        basiclife = LIFE_DAY_LONG + 1;
    return (basiclife * 24 * 60 - value) / (60 * 24);
}

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

/**
 * ������غ�����
 */
time_t get_posttime(const struct fileheader *fileinfo)
{
	return atoi(fileinfo->filename + 2);
}

void set_posttime(struct fileheader *fileinfo)
{
	return;
}

void set_posttime2(struct fileheader *dest, struct fileheader *src)
{
	return;
}

/**
 * ������ء�
 */
void build_board_structure(const char *board)
{
	return;
}

void get_mail_limit(struct userec* user,int *sumlimit,int * numlimit)
{
    if ((!(user->userlevel & PERM_SYSOP)) && strcmp(user->userid, "Arbitrator")) {
        if (user->userlevel & PERM_CHATCLOAK) {
            *sumlimit = 8000;
            *numlimit = 8000;
        } else
            /*
             *              * if (lookupuser->userlevel & PERM_BOARDS)
             *                           * set BM, chatop, and jury have bigger mailbox, stephen 2001.10.31 
             *                                        */
        if (user->userlevel & PERM_MANAGER) {
            *sumlimit = 4000;
            *numlimit = 4000;
        } else if (user->userlevel & PERM_LOGINOK) {
            *sumlimit = 1000;
            *numlimit = 1000;
        } else {
            *sumlimit = 15;
            *numlimit = 15;
        }
    }
    else {
        *sumlimit = 9999;
        *numlimit = 9999;
        return;
    }
}

char *showuserip(struct userec *user, char *ip)
{
    static char sip[25];
    char *c;

    if (user != NULL && (!DEFINE(user, DEF_HIDEIP)))
        return ip;
    strncpy(sip, ip, 24);
    sip[24] = 0;
    if ((c = strrchr(sip, '.')) != NULL) {
        *(++c) = '*';
        *(++c) = '\0';
    }
    return sip;
}

/* board permissions control */
int check_read_perm(const struct userec *user, const struct boardheader *board)
{
    if (board == NULL)
        return 0;

    if (user == NULL) {
        if (board->title_level != 0)
            return 0;
    } else if (!HAS_PERM(user, PERM_OBOARDS) && board->title_level && (board->title_level != user->title))
        return 0;
//asing add 4.20
if(!strcmp(board->filename,"Hate") ){
	if(user==NULL)
		return 0;
	if(user->numposts<500)
		return 0;
}

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

int check_see_perm(const struct userec* user,const struct boardheader* board)
{
    if (board == NULL)
        return 0;

    if (user == NULL) {
        if (board->title_level != 0)
            return 0;
    } else if (!HAS_PERM(user, PERM_OBOARDS) && board->title_level && (board->title_level != user->title))
        return 0;
//asing add 4.20
if(!strcmp(board->filename,"Hate") ){
	if(user==NULL)
		return 0;
	if(user->numposts<500)
		return 0;
}

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




