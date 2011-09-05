#include "bbs.h"

const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const char seccode[SECNUM][5] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8",
};


const char *const groups[] = {
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
    {"��ܰС��", "[��վ][ϵͳ]"},
    {"У԰����", "[У԰][Э��]"},
    {"BBS ����", "[�汾][����][ת��]"},
    {"���Լ���", "[���][����][ϵͳ]"},
    {"ѧ����ѧ", "[ѧϰ][����]"},
    {"�Ļ�����", "[��ѧ]"},
    {"��������", "[����][����][����]"},
    {"֪�Ը���", "[����][����]"},
    {"��������", "[����][����][��Ϣ]"},
};

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


struct count_arg {
    int www_count;
    int telnet_count;
};

int countuser(struct user_info *uinfo, struct count_arg *arg, int pos)
{
    if (uinfo->pid == 1)
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
        if (apply_utmp((APPLY_UTMP_FUNC) checkguestip, 0, "guest", getSession()->fromhost) > MAX_GUEST_PER_IP)
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

int old_compute_user_value(const struct userec *urec)
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

int compute_user_value(const struct userec *urec)
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
        return old_compute_user_value(urec) + 15;
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
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24) + 15;
    /**********************/
    if (urec->numlogins <= 3)
        return (LIFE_DAY_SUICIDE * 24 * 60 - value) / (60 * 24) + 15;
    if (!(urec->userlevel & PERM_LOGINOK))
        return (LIFE_DAY_NEW * 24 * 60 - value) / (60 * 24) + 15;
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
    return (basiclife * 24 * 60 - value) / (60 * 24) + 15;
}

/**
 * ��������غ�����
 */
int ann_get_postfilename(char *filename, struct fileheader *fileinfo, MENU * pm)
{
    char fname[PATHLEN];
    char *ip;

    if (fileinfo->filename[1] == '/')
        strcpy(filename, fileinfo->filename + 2);
    else
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
    if (fileinfo->filename[1] == '/')
        return fileinfo->posttime;
    else
        return atoi(fileinfo->filename + 2);
}

void set_posttime(struct fileheader *fileinfo)
{
    if (fileinfo->filename[1] == '/')
        fileinfo->posttime = atoi(fileinfo->filename + 4);
}

void set_posttime2(struct fileheader *dest, struct fileheader *src)
{
    dest->posttime = src->posttime;
}

/**
 * ������ء�
 */
void build_board_structure(const char *board)
{
    int i;
    int len;
    char buf[STRLEN];

    len = strlen(alphabet);
    for (i = 0; i < len; i++) {
        snprintf(buf, sizeof(buf), "boards/%s/%c", board, alphabet[i]);
        mkdir(buf, 0755);
    }
    return;
}


void get_mail_limit(struct userec *user, int *sumlimit, int *numlimit)
{
    if ((!(user->userlevel & PERM_SYSOP)) && strcmp(user->userid, "Arbitrator")) {
        if (user->userlevel & PERM_JURY) {
            *sumlimit = 10000;
            *numlimit = 10000;
        } else if (user->userlevel & PERM_BMAMANGER) {
            *sumlimit = 8000;
            *numlimit = 8000;
        } else if (user->userlevel & PERM_CHATCLOAK) {
            *sumlimit = 8000;
            *numlimit = 8000;
        } else
            /*
             * if (lookupuser->userlevel & PERM_BOARDS)
             * set BM, chatop, and jury have bigger mailbox, stephen 2001.10.31 
             */
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
    } else {
        *sumlimit = 9999;
        *numlimit = 9999;
        return;
    }
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

    if (board->level & PERM_POSTMASK || HAS_PERM(user, board->level) || (board->level & PERM_NOZAP)) {
        if (board->flag & BOARD_CLUB_READ) {    /*���ֲ� */
            if (HAS_PERM(user, PERM_OBOARDS) && HAS_PERM(user, PERM_SYSOP))
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

int check_see_perm(const struct userec *user, const struct boardheader *board)
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


