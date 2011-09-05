#include "bbs.h"

#ifdef USE_DEFAULT_ALPHABET
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
#endif

#ifdef USE_DEFAULT_SECODE
const char seccode[SECNUM][5] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8",
};                              /* by wisi@Tongji */

/* ignore smth by wisi@Tongji 
const char *groups[] = {
    "system.faq",
    "thu.faq",
    "univ.faq",
    "sci.faq",
    "rec.faq",
    "literal.faq",
    "social.faq",
    "game.faq",
    "sport.faq",
    "talk.faq",
    "info.faq",
    "develop.faq",
    "os.faq",
    "comp.faq",
    NULL
};
const char secname[SECNUM][2][20] = {
    {"BBS ϵͳ", "[վ��]"},
    {"�廪��ѧ", "[��У]"},
    {"ѧ����ѧ", "[ѧ��/����]"},
    {"��������", "[����/����]"},
    {"�Ļ�����", "[�Ļ�/����]"},
    {"�����Ϣ", "[���/��Ϣ]"},
    {"��Ϸ���", "[��Ϸ/��ս]"},
    {"��������", "[�˶�/����]"},
    {"֪�Ը���", "[̸��/����]"},
    {"������Ϣ", "[����/��Ϣ]"},
    {"�������", "[����/����]"},
    {"����ϵͳ", "[ϵͳ/�ں�]"},
    {"���Լ���", "[ר���]"},
};

const char *explain[] = {
    "��վϵͳ",
    "�廪��ѧ",
    "У԰��Ϣ",
    "ѧ����ѧ",
    "��������",
    "�Ļ�����",
    "�����Ϣ",
    "��Ϸ���",
    "��������",
    "֪�Ը���",
    "������Ϣ",
    "�������",
    "����ϵͳ",
    "���Լ���",
    NULL
};
  ignore end by wisi@Tongji */
/* by wisi@Tongji */
const char *groups[] = {
    "GROUP_0",
    "GROUP_1",
    "GROUP_2",
    "GROUP_3",
    "GROUP_4",
    "GROUP_5",
    "GROUP_6",
    "GROUP_7",
    "GROUP_8",
    NULL
};
const char secname[SECNUM][2][20] = {
    {"BBS ϵͳ", "[��վ]"},
    {"ͬ�ô�ѧ", "[Ժϵ/У԰/����]"},
    {"���Լ���", "[����/ϵͳ/����]"},
    {"ѧ����ѧ", "[ѧ��/����]"},
    {"�Ļ�����", "[�Ļ�/����]"},
    {"��������", "[����/����/����]"},
    {"��������", "[����/����]"},
    {"֪�Ը���", "[����/����]"},
    {"������Ϣ", "[����/��Ϣ]"},
};

const char *explain[] = {
    "��վϵͳ",
    "ͬ�ô�ѧ",
    "���Լ���",
    "ѧ����ѧ",
    "�Ļ�����",
    "��������",
    "��������",
    "֪�Ը���",
    "������Ϣ",
    NULL
};

#endif

#ifdef USE_DEFAULT_PERMSTRINGS
const char *permstrings[] = {
    "����Ȩ��",                 /* PERM_BASIC */
    "����������",               /* PERM_CHAT */
    "������������",             /* PERM_PAGE */
    "��������",                 /* PERM_POST */
    "ʹ����������ȷ",           /* PERM_LOGINOK */
    "ʵϰվ��",                 /* PERM_BMMANAGER */
    "������",                   /* PERM_CLOAK */
    "�ɼ�����",                 /* PERM_SEECLOAK */
    "�����ʺ�",                 /* PERM_XEMPT */
    "�༭ϵͳ����",             /* PERM_WELCOME */
    "����",                     /* PERM_BOARDS */
    "�ʺŹ���Ա",               /* PERM_ACCOUNTS */
    "ˮľ�廪������",           /* PERM_CHATCLOAK */
    "�������Ȩ��",             /* PERM_DENYRELAX */
    "ϵͳά������Ա",           /* PERM_SYSOP */
    "Read/Post ����",           /* PERM_POSTMASK */
    "�������ܹ�",               /* PERM_ANNOUNCE */
    "�������ܹ�",               /* PERM_OBOARDS */
    "������ܹ�",             /* PERM_ACBOARD */
    "���� ZAP(������ר��)",     /* PERM_NOZAP */
    "������OP(Ԫ��Ժר��)",     /* PERM_CHATOP */
    "ϵͳ�ܹ���Ա",             /* PERM_ADMIN */
    "�����ʺ�",                 /* PERM_HONOR */
    "����Ȩ�� 5",               /* PERM_UNUSE? */
    "�ٲ�ίԱ",                 /* PERM_JURY */
    "����Ȩ�� 7",               /* PERM_UNUSE? */
    "��ɱ������",               /*PERM_SUICIDE */
    "����ר���ʺ�",             /* PERM_COLLECTIVE */
    "��ϵͳ���۰�",             /* PERM_UNUSE? */
    "���Mail",                 /* PERM_DENYMAIL */

};
#endif

#ifdef USE_DEFAULT_DEFINESTR
/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
const char *user_definestr[] = {
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
    "ʹ�ø�������",             /* DEF_HIGHCOLOR */
    "�ۿ�����ͳ�ƺ�ף����",     /* DEF_SHOWSTATISTIC Haohmaru 98.09.24 */
    "δ�����ʹ�� *",           /* DEF_UNREADMARK Luzi 99.01.12 */
    "ʹ��GB���Ķ�",             /* DEF_USEGB KCN 99.09.03 */
    "�Ժ��ֽ������ִ���"        /* DEF_SPLITSCREEN 2002.9.1 */
};
#endif

#ifdef USE_DEFAULT_MAILBOX_PROP_STR
const char *mailbox_prop_str[] = {
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

    /*
     * Bigman: �������Ĳ�ѯ��ʾ 2000.8.10 
     */
    /*
     * if( lvl & PERM_ZHANWU ) strcpy(buf,"վ��"); 
     */
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
    /*
     * Bigman: �޸���ʾ 2001.6.24 
     */
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
        return "�ĺ��ӱ�";
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
        return "��������";      /* by wisi@Tongji */
        /*
         * return "��������";
         */
//    case EDITWELC:
//        return "�༭ Welc";
    case EDITUFILE:
        return "�༭����";
    case EDITSFILE:
        return "ϵͳ����";
        /*
         * case  EDITSIG:  return "��ӡ";
         * case  EDITPLAN: return "��ƻ�"; 
         */
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
        return "ɱ����Ϸ";      /* by wisi@Tongji */
    default:
        return "ȥ������!?";
    }
}

#endif

#ifdef USE_DEFAULT_MULTILOGIN_CHECK
struct count_arg {
    int www_count;
    int telnet_count;
};
int countuser(struct user_info *uinfo, struct count_arg *arg, int pos)
{
    if (uinfo->mode == WEBEXPLORE)
        arg->www_count++;
    else
        arg->telnet_count++;
    return COUNT;
}
int multilogin_user(struct userec *user, int usernum, int mode)
{
    int logincount;
    int curr_login_num;
    struct count_arg arg;

    bzero(&arg, sizeof(arg));
    logincount = apply_utmpuid((APPLY_UTMP_FUNC) countuser, usernum, &arg);

    if (logincount < 1)
        RemoveMsgCountFile(user->userid);

    if (HAS_PERM(user, PERM_MULTILOG))
        return 0;               /* don't check sysops */
    curr_login_num = get_utmp_number();
    /*
     * binxun 2003.5 �ٲã�������Chatop���ȶ���������
     */
    if ((HAS_PERM(user, PERM_BOARDS) || HAS_PERM(user, PERM_CHATOP)
         || HAS_PERM(user, PERM_JURY) || HAS_PERM(user, PERM_CHATCLOAK)
         || HAS_PERM(user, PERM_BMAMANGER))
        && logincount < 3)
        return 0;
    /*
     * allow multiple guest user 
     */
    if (!strcmp("guest", user->userid)) {
        if (logincount > MAX_GUEST_NUM) {
            return 2;
        }
        return 0;
    } else if (((curr_login_num < 700) && (logincount >= 3))
               || ((curr_login_num >= 700) && (logincount >= 2)
                   && !(((arg.telnet_count == 0) && (mode == 0))
                        || (((arg.www_count == 0) && (mode == 1))))))
        return 1;
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
int compute_user_value(struct userec *urec)
{
    int value;
    int registeryear;
    int basiclife;

    /*
     * if (urec) has CHATCLOAK permission, don't kick it 
     */
    /*
     * Ԫ�Ϻ������ʺ� �ڲ���ɱ������£� ������999 Bigman 2001.6.23 
     */
    /*
     * ��������˵�id,sigh 
     */
    if ((urec->userlevel & PERM_HORNOR) && !(urec->userlevel & PERM_LOGINOK))
        return LIFE_DAY_LONG;


    if (((urec->userlevel & PERM_HORNOR) || (urec->userlevel & PERM_CHATCLOAK)) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_NODIE;

    if ((urec->userlevel & PERM_ANNOUNCE) && (urec->userlevel & PERM_OBOARDS))
        return LIFE_DAY_SYSOP;
    /*
     * վ����Ա���������� Bigman 2001.6.23 
     */


    value = (time(0) - urec->lastlogin) / 60;   /* min */
    if (0 == value)
        value = 1;              /* Leeward 98.03.30 */

    /*
     * new user should register in 30 mins 
     */
    if (strcmp(urec->userid, "new") == 0) {
        return (LIFE_DAY_NEW - value) / 60;     /* *->/ modified by dong, 1998.12.3 */
    }

    /*
     * ��ɱ����,Luzi 1998.10.10 
     */
    if (urec->userlevel & PERM_SUICIDE)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24);
    /**********************/
    if (urec->numlogins <= 3)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24);
    if (!(urec->userlevel & PERM_LOGINOK))
        return (LIFE_DAY_NEW * 24 * 60 - value) / (60 * 24);
    /*
     * if (urec->userlevel & PERM_LONGID)
     * return (667 * 24 * 60 - value)/(60*24); 
     */
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
int ann_get_postfilename(char *filename, struct fileheader *fileinfo, MENU * pm)
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


#ifdef USE_DEFAULT_MAIL_LIMIT   /*�ʼ��������� */
void get_mail_limit(struct userec *user, int *sumlimit, int *numlimit)
{
    /*
     * unlimit by wisi@Tongji
     */
    *sumlimit = 150;
    *numlimit = 300;
    return;
}
#endif

/* board permissions control */
#ifdef USE_DEFAULT_READ_PERM
int check_read_perm(struct userec *user, const struct boardheader *board)
{
    if (board == NULL)
        return 0;
    if (board->level & PERM_POSTMASK || HAS_PERM(user, board->level) || (board->level & PERM_NOZAP)) {
        if (board->flag & BOARD_CLUB_READ) {    /*���ֲ� */
            if (HAS_PERM(user, PERM_OBOARDS) && HAS_PERM(user, PERM_SYSOP))
                return 1;
            if (board->clubnum <= 0 || board->clubnum >= MAXCLUB)
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
#endif                          /* USE_DEFAULT_READ_PERM */

#ifdef USE_DEFAULT_SEE_PERM
int check_see_perm(struct userec *user, const struct boardheader *board)
{
    if (board == NULL)
        return 0;
    if (board->level & PERM_POSTMASK || ((user == NULL) && (board->level == 0))
        || ((user != NULL) && HAS_PERM(user, board->level))
        || (board->level & PERM_NOZAP)) {
        if (board->flag & BOARD_CLUB_HIDE) {    /*���ؾ��ֲ� */
            if (user == NULL)
                return 0;
            if (HAS_PERM(user, PERM_OBOARDS))
                return 1;
            return check_read_perm(user, board);
        }
        return 1;
    }
    return 0;
}


int smsnumber2uid(byte number[4])
{
    int uid;
    char buf[10];

    uid = byte2long(number);
    return uid;
}

void uid2smsnumber(struct user_info *uin, char *number)
{
    sprintf(number, "%d", uin->uid);
}

#endif                          /* USE_DEFAULT_SEE_PERM */
