#include "bbs.h"

#ifndef REGISTER_TSINGHUA_WAIT_TIME
#define REGISTER_TSINGHUA_WAIT_TIME (24*60*60)
#endif

const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const char seccode[SECNUM][5] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8",
};                              /*by wisi */

const char *permstrings[] = {
    "����Ȩ��",                 /* PERM_BASIC */
    "����������",               /* PERM_CHAT */
    "������������",             /* PERM_PAGE */
    "��������",                 /* PERM_POST */
    "ʹ����������ȷ",           /* PERM_LOGINOK */
    "ʵϰվ��",                 /* PERM_BMANAGER */
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
    "����Ȩ�� 5",               /* PERM_SECANC */
    "�ٲ�ίԱ",                 /* PERM_JURY */
    "����Ȩ�� 7",               /* PERM_CHECKCD */
    "��ɱ������",               /*PERM_SUICIDE */
    "����ר���ʺ�",             /* PERM_COLLECTIVE */
    "��ϵͳ���۰�",             /* PERM_DISS */
    "���Mail",                 /* PERM_DENYMAIL */

};

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
    "�Ժ��ֽ������ִ���",       /* DEF_SPLITSCREEN 2002.9.1 */
    "��ʾ��ϸ�û�����",         /*DEF_SHOWDETAILUSERDATA 2003.7.31 */
    "��ʾ��ʵ�û�����",         /*DEF_REALDETAILUSERDATA 2003.7.31 */
    "",
    "����ip"                    /* DEF_SHOWALLIP */
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

const char *mailbox_prop_str[] = {
    "����ʱ�����ż���������",
    "ɾ���ż�ʱ�����浽������",
    "���水 'v' ʱ����: �ռ���(OFF) / ����������(ON)",
};

struct _shmkey {
    char key[20];
    int value;
};

static const struct _shmkey shmkeys[] = {
    {"BCACHE_SHMKEY", 3693},
    {"UCACHE_SHMKEY", 3696},
    {"UTMP_SHMKEY", 3699},
    {"ACBOARD_SHMKEY", 9013},
    {"ISSUE_SHMKEY", 5010},
    {"GOODBYE_SHMKEY", 5020},
    {"PASSWDCACHE_SHMKEY", 3697},
    {"STAT_SHMKEY", 5100},
    {"CONVTABLE_SHMKEY", 5101},
    {"MSG_SHMKEY", 5200},
    {"", 0}
};

int get_shmkey(char *s)
{
    int n = 0;

    while (shmkeys[n].key != 0) {
        if (!strcasecmp(shmkeys[n].key, s))
            return shmkeys[n].value;
        n++;
    }
    return 0;
}

int uleveltochar(char *buf, struct userec *lookupuser)
{                               /* ȡ�û�Ȩ������˵�� Bigman 2001.6.24 */
    unsigned lvl;
    char userid[IDLEN + 2];

    if (lookupuser->title != 0) {
        strcpy(buf, get_user_title(lookupuser->title));
        if (buf[0] != 0)
            return 0;
    }
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

/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* rrr - This is separated so I can suck it into the IRC source for use
   there too */

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
        return "\033[1;32m�ĺ��ӱ�";
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
    case MONITOR:
        return "�࿴��";
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
        return "\033[1;33m��������";    //ft
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
        return "\033[1;35mWeb���";
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
        return "\033[1;31mɱ����Ϸ";
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
    default:
        return "ȥ������!?";
    }
}

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

int checkguestip(struct user_info *uentp, char *arg, int count)
{
    if (!strcmp(uentp->from, arg))
        return COUNT;
    return 0;
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

#ifdef FILTER
    if (!strcmp(user->userid, "menss") && logincount < 2)
        return 0;
#endif
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

    if (!strcmp("guest", user->userid)) {
        if (logincount > MAX_GUEST_NUM)
            return 2;
#define MAX_GUEST_PER_IP 20
        if (apply_utmp((APPLY_UTMP_FUNC) checkguestip, 0, "guest", fromhost) > MAX_GUEST_PER_IP)
            return 3;
        return 0;
    }

    /*
     * δͨ��ע����û�����˫�� added by bixnun 2003.5.30 
     */
    if ((!HAS_PERM(user, PERM_LOGINOK)) && logincount > 0)
        return 1;

    if (((curr_login_num < 700) && (logincount >= 3))   /*С��700�������� */
        ||((curr_login_num >= 700) && (logincount >= 2) /*700������ */
           &&!(((arg.telnet_count == 0) && (mode == 0)) /* telnet����Ϊ������ٵ�һ��telnet */
               ||(((arg.www_count == 0) && (mode == 1))))))     /*user login limit */
        return 1;
    return 0;
}

int old_compute_user_value(struct userec *urec)
{
    int value;

    /*
     * if (urec) has CHATCLOAK permission, don't kick it 
     */
    /*
     * Ԫ�Ϻ������ʺ� �ڲ���ɱ������£� ������999 Bigman 2001.6.23 
     */
    /*
     * * zixia 2001-11-20 ���е���������ʹ�ú��滻��
     * * �� smth.h/zixia.h �ж��� 
     * * 
     */

    if (((urec->userlevel & PERM_HORNOR) || (urec->userlevel & PERM_CHATCLOAK)) && (!(urec->userlevel & PERM_SUICIDE)))
        return LIFE_DAY_NODIE;

    if (urec->userlevel & PERM_SYSOP)
        return LIFE_DAY_SYSOP;
    /*
     * վ����Ա���������� Bigman 2001.6.23 
     */


    value = (time(0) - urec->lastlogin) / 60;   /* min */
    if (0 == value)
        value = 1;              /* Leeward 98.03.30 */

    /*
     * �޸�: �������ʺ�תΪ�����ʺ�, Bigman 2000.8.11 
     */
    if ((urec->userlevel & PERM_XEMPT) && (!(urec->userlevel & PERM_SUICIDE))) {
        if (urec->lastlogin < 988610030)
            return LIFE_DAY_LONG;       /* ���û�е�¼���� */
        else
            return (LIFE_DAY_LONG * 24 * 60 - value) / (60 * 24);
    }
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
    return (LIFE_DAY_USER * 24 * 60 - value) / (60 * 24);
}

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
     * * zixia 2001-11-20 ���е���������ʹ�ú��滻��
     * * �� smth.h/zixia.h �ж��� 
     * * 
     */
    /*
     * ���⴦�����ƶ���cvs ���� 
     */

    if (urec->lastlogin < 1022036050)
        return old_compute_user_value(urec);
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
    return 0;
}

/**
 * ������غ�����
 */
time_t get_posttime(const struct fileheader * fileinfo)
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


void get_mail_limit(struct userec *user, int *sumlimit, int *numlimit)
{
    if ((!(user->userlevel & PERM_SYSOP)) && strcmp(user->userid, "Arbitrator")) {
        if (user->userlevel & PERM_COLLECTIVE) {
            *sumlimit = 2000;
            *numlimit = 2000;
        } else if (user->userlevel & PERM_JURY) {
            *sumlimit = 2000;
            *numlimit = 2000;
        } else if (user->userlevel & PERM_BMAMANGER) {
            *sumlimit = 2000;
            *numlimit = 2000;
        } else if (user->userlevel & PERM_CHATCLOAK) {
            *sumlimit = 2000;
            *numlimit = 2000;
        } else
            /*
             * if (lookupuser->userlevel & PERM_BOARDS)
             * set BM, chatop, and jury have bigger mailbox, stephen 2001.10.31 
             */
        if (user->userlevel & PERM_MANAGER) {
            *sumlimit = 1500;
            *numlimit = 1500;
        } else if (user->userlevel & PERM_LOGINOK) {
            *sumlimit = 1000;
            *numlimit = 1000;
        } else {
            *sumlimit = 15;
            *numlimit = 15;
        }
    } else {
        *sumlimit = 2000;
        *numlimit = 2000;
        return;
    }
}

/* board permissions control */
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

int check_see_perm(struct userec *user, const struct boardheader *board)
{
    if (board == NULL)
        return 0;
    if (user == NULL) {
        if (board->title_level != 0)
            return 0;
    } else if (!HAS_PERM(user, PERM_OBOARDS) && board->title_level && (board->title_level != user->title))
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

//�Զ�ͨ��ע��ĺ���  binxun
int auto_register(char *userid, char *email, int msize)
{
    struct userdata ud;
    struct userec *uc;
    char *item;
    char fdata[7][STRLEN];
    char genbuf[STRLEN];
    char buf[STRLEN];
    char fname[STRLEN];
    int unum;
    FILE *fout;
    int n;
    struct userec deliveruser;
    static const char *finfo[] = { "�ʺ�λ��", "�������", "��ʵ����", "����λ",
        "Ŀǰסַ", "����绰", "��    ��", NULL
    };
    static const char *field[] = { "usernum", "userid", "realname", "career",
        "addr", "phone", "birth", NULL
    };

    bzero(&deliveruser, sizeof(struct userec));
    strcpy(deliveruser.userid, "deliver");
    deliveruser.userlevel = -1;
    strcpy(deliveruser.username, "�Զ�����ϵͳ");



    bzero(fdata, 7 * STRLEN);

    if ((unum = getuser(userid, &uc)) == 0)
        return -1;              //faild
    if (read_userdata(userid, &ud) < 0)
        return -1;

    strncpy(genbuf, email, STRLEN - 16);
    item = strtok(genbuf, "#");
    if (item)
        strncpy(ud.realname, item, NAMELEN);
    item = strtok(NULL, "#");   //ѧ��
    item = strtok(NULL, "#");
    if (item)
        strncpy(ud.address, item, STRLEN);

    email[strlen(email) - 3] = '@';
    strncpy(ud.realemail, email, STRLEN - 16);  //email length must be less STRLEN-16


    sprintf(fdata[0], "%d", unum);
    strncpy(fdata[2], ud.realname, NAMELEN);
    strncpy(fdata[4], ud.address, STRLEN);
    strncpy(fdata[5], ud.email, STRLEN);
    strncpy(fdata[1], userid, IDLEN);

    sprintf(buf, "tmp/email/%s", userid);
    if ((fout = fopen(buf, "w")) != NULL) {
        fprintf(fout, "%s\n", email);
        fclose(fout);
    }

    if (write_userdata(userid, &ud) < 0)
        return -1;
    mail_file("deliver", "etc/s_fill", userid, "��ϲ��,���Ѿ����ע��.", 0, 0);
    //sprintf(genbuf,"deliver �� %s �Զ�ͨ�����ȷ��.",uinfo.userid);

    sprintf(fname, "tmp/security.%d", getpid());
    if ((fout = fopen(fname, "w")) != NULL) {
        fprintf(fout, "ϵͳ��ȫ��¼ϵͳ\n\033[32mԭ��%s�Զ�ͨ��ע��\033[m\n", userid);
        fprintf(fout, "������ͨ���߸�������");
        fprintf(fout, "\n\n���Ĵ���     : %s\n", ud.userid);
        fprintf(fout, "�����ǳ�     : %s\n", uc->username);
        fprintf(fout, "��ʵ����     : %s\n", ud.realname);
        fprintf(fout, "�����ʼ����� : %s\n", ud.email);
        fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
        fprintf(fout, "����λ     : %s\n", "");
        fprintf(fout, "Ŀǰסַ     : %s\n", ud.address);
        fprintf(fout, "����绰     : %s\n", "");
        fprintf(fout, "ע������     : %s", ctime(&uc->firstlogin));
        fprintf(fout, "����������� : %s", ctime(&uc->lastlogin));
        fprintf(fout, "������ٻ��� : %s\n", uc->lasthost);
        fprintf(fout, "��վ����     : %d ��\n", uc->numlogins);
        fprintf(fout, "������Ŀ     : %d(Board)\n", uc->numposts);
        fprintf(fout, "��    ��     : %s\n", "");

        fclose(fout);
        //post_file(currentuser, "", fname, "Registry", str, 0, 2);

        sprintf(genbuf, "%s �Զ�ͨ��ע��", ud.userid);
        post_file(&deliveruser, "", fname, "Registry", genbuf, 0, 1);
        /*
         * if (( fout = fopen(logfile,"a")) != NULL)
         * {
         * fclose(fout);
         * }
         */
    }

    sethomefile(buf, userid, "/register");
    if ((fout = fopen(buf, "w")) != NULL) {
        for (n = 0; field[n] != NULL; n++)
            fprintf(fout, "%s     : %s\n", finfo[n], fdata[n]);
        fprintf(fout, "�����ǳ�     : %s\n", uc->username);
        fprintf(fout, "�����ʼ����� : %s\n", ud.email);
        fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
        fprintf(fout, "ע������     : %s\n", ctime(&uc->firstlogin));
        fprintf(fout, "ע��ʱ�Ļ��� : %s\n", uc->lasthost);
        fprintf(fout, "Approved: %s\n", userid);
        fclose(fout);
    }

    return 0;
}

char *showuserip(struct userec *user, char *ip)
{
    static char sip[25];
    char *c;

    if ((currentuser != NULL) && (currentuser->title == 10))
        return ip;
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
