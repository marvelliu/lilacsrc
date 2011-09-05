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

    checked global variable
*/

#include "bbs.h"

int d_board(void){
    char bname[STRLEN+1];
    char buf[STRLEN];
    const struct boardheader *bh;
    int bid;
    struct boardheader oldbh;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("ɾ��������");
    make_blist(0);
    move(1, 0);
    namecomplete("������������: ", bname);
    if (bname[0] == '\0')
        return 0;

    if ((bid = getbid(bname, &bh)) == 0) {
        move(2, 0);
        prints("����ȷ��������");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    
    memcpy(&oldbh, bh, sizeof(struct boardheader));
    move(1, 0);
    prints("ɾ�������� '%s'.", bh->filename);
    clrtoeol();
    getdata(2, 0, "(Yes, or No) [N]: ", genbuf, 4, DOECHO, NULL, true);
    if (genbuf[0] != 'Y' && genbuf[0] != 'y') {
        move(2, 0);
        prints("ȡ��ɾ��....\n");
        pressreturn();
        clear();
        return 0;
    }

    getdata(3, 0, "�Ƴ������� [y/N]: ", genbuf, 4, DOECHO, NULL, true);
    if (genbuf[0] == 'Y' || genbuf[0] == 'y')
    {
        edit_group(&oldbh, NULL);
    }

    if (delete_board(bid, getSession()) != 0)
        return 0;
#if 0
        sprintf(genbuf, "boards/%s", bname);
        f_rm(genbuf);
        sprintf(genbuf, "vote/%s", bname);
        f_rm(genbuf);
#endif

    /* this should be in delete_board function ? */
    sprintf(buf, "ɾ����������%s", oldbh.filename);
    securityreport(buf, NULL, NULL, getSession());

    move(4, 0);
    prints("���������Ѿ�ɾ��...\n");
    pressreturn();
    clear();
    return 0;
}

int suicide(void){
    char buf[STRLEN];
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    char XPERM[48];
    int oldXPERM;
    int num;

    modify_user_mode(OFFLINE);
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS) || HAS_PERM(getCurrentUser(), PERM_OBOARDS) || HAS_PERM(getCurrentUser(), PERM_ACCOUNTS)
        || HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)
        || HAS_PERM(getCurrentUser(), PERM_JURY) || HAS_PERM(getCurrentUser(), PERM_SUICIDE) || HAS_PERM(getCurrentUser(), PERM_CHATOP) || (!HAS_PERM(getCurrentUser(), PERM_POST))
        || HAS_PERM(getCurrentUser(), PERM_DENYMAIL)
        || HAS_PERM(getCurrentUser(), PERM_DENYRELAX)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m������������������ɱ��\033[m");
        pressanykey();
        return -1;
    }

    clear();
    move(1, 0);
    prints("ѡ����ɱ��ʹ�������������ٵ�14�죬14��������ʺ��Զ���ʧ��");
    move(3, 0);
    prints("����14�������ı�����Ļ��������ͨ����¼��վһ�λָ�ԭ������");
    move(5, 0);
    prints("��ɱ�û�����ʧ����\033[33m����Ȩ��\033[m������");
    move(7, 0);
    /*
       clear();
       move(1,0);
       prints("ѡ����ɱ��ʹ�����ʺ��Զ�������ʧ!");
       move(3,0);
       prints("�����ʺŽ����ϴ�ϵͳ��ɾ��");
     */

    if (askyn("��ȷ��Ҫ��ɱ��", 0) == 1) {
        clear();
        getdata(0, 0, "������ԭ����(������ȷ�Ļ������̶���): ", buf, 39, NOECHO, NULL, true);   /*Haohmaru,98.10.12,check the passwds */
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return -1;
        }

        oldXPERM = getCurrentUser()->userlevel;
        strcpy(XPERM, XPERMSTR);
        for (num = 0; num < (int) strlen(XPERM); num++)
            if (!(oldXPERM & (1 << num)))
                XPERM[num] = ' ';
        XPERM[num] = '\0';
        getCurrentUser()->userlevel &= 0x3F; /*Haohmaru,99.3.20.��ɱ��ֻ��������Ȩ�� */
        getCurrentUser()->userlevel ^= PERM_SUICIDE;

        /*Haohmaru.99.3.20.��ɱ֪ͨ */
        now = time(0);
        sprintf(filename, "etc/%s.tmp", getCurrentUser()->userid);
        fn = fopen(filename, "w");
        fprintf(fn, "\033[1m%s\033[m �Ѿ��� \033[1m%24.24s\033[m ��ɱ�ˣ��������������ϣ��뱣��...", getCurrentUser()->userid, ctime(&now));
        fprintf(fn, "\n\n��������ɱ��ԭ����Ȩ��\n\033[1m\033[33m%s\n\033[m", XPERM);
        getuinfo(fn, getCurrentUser());
        fprintf(fn, "\n                      \033[1m ϵͳ�Զ�����ϵͳ��\033[m\n");
        fclose(fn);
        sprintf(buf, "%s ����ɱ֪ͨ", getCurrentUser()->userid);
        post_file(getCurrentUser(), "", filename, "Goodbye", buf, 0, 1,getSession());
        unlink(filename);

        /*kick_user(&uinfo);
           exit(0); */
        abort_bbs(0);
    }
    return 0;
}

int giveupnet(void){
/*
    PERM_BASIC      ��վ
    PERM_POST       ����
    PERM_CHAT       ����
    PERM_PAGE       ����
    PERM_DENYMAIL   ����
    PERM_DENYRELAX  ����
*/
    static const char *desc[GIVEUPINFO_PERM_COUNT]={
        "��¼��վȨ��","��������Ȩ��","����㳡Ȩ��","վ����ϢȨ��", "վ���ż�Ȩ��","��������Ȩ��"};
    FILE *fn;
    char buf[STRLEN], genbuf[PATHLEN];
    char ans[3], day[10];
    int i, j, k, iDays;
    int s[GIVEUPINFO_PERM_COUNT];

    modify_user_mode(GIVEUPNET);
    if (!HAS_PERM(getCurrentUser(), PERM_LOGINOK)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m���л�û��ע��ͨ�������ܽ�����\033[m");
        pressanykey();
        return -1;
    }

    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS) || HAS_PERM(getCurrentUser(), PERM_OBOARDS) || HAS_PERM(getCurrentUser(), PERM_ACCOUNTS)
        || HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)
        || HAS_PERM(getCurrentUser(), PERM_JURY) || HAS_PERM(getCurrentUser(), PERM_SUICIDE) || HAS_PERM(getCurrentUser(), PERM_CHATOP)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m���������������ܽ�����\033[m");
        pressanykey();
        return -1;
    }

    if(get_giveupinfo(getCurrentUser(),s)){
        for(i=0;i<GIVEUPINFO_PERM_COUNT;i++)
            if(s[i])
                break;
        if(i<GIVEUPINFO_PERM_COUNT){
            clear();
            move(0,0);
            prints("\033[1;33m[��ǰ���������ڷ��״̬]\033[m\n\n");
            for(i=0;i<GIVEUPINFO_PERM_COUNT;i++)
                if(!s[i])
                    continue;
                else{
                    sprintf(genbuf,"%-16.16s%-8.8sĿǰʣ�� %4d ��\n",desc[i],(s[i]>0?"����":"���"),
                        (int)((s[i]>0?s[i]:(-s[i]))-(time(NULL)/86400)));
                    prints("%s",genbuf);
                }
            pressanykey();
        }
    }

    clear();
    move(1, 0);
    prints("��ѡ���������:");
    move(3, 0);
    prints("(0) - ����");
    for(i=0; i<6; i++) {
        move(i + 4, 0);
        prints("(%d) - %s", i + 1, desc[i]);
    }

    getdata(12, 0, "��ѡ�� [0]", ans, 2, DOECHO, NULL, true);
    if (ans[0] < '1' || ans[0] > '6') {
        return -1;
    }
    k = 1;
    switch (ans[0]) {
    case '1':
        k = k && (getCurrentUser()->userlevel & PERM_BASIC);
        break;
    case '2':
        k = k && (getCurrentUser()->userlevel & PERM_POST);
        break;
    case '3':
        k = k && (getCurrentUser()->userlevel & PERM_CHAT);
        break;
    case '4':
        k = k && (getCurrentUser()->userlevel & PERM_PAGE);
        break;
    case '5':
        k = k && !(getCurrentUser()->userlevel & PERM_DENYMAIL);
        break;
    case '6':
        k = k && !(getCurrentUser()->userlevel & PERM_DENYRELAX);
        break;
    }

    if (!k) {
        prints("\n\n���Ѿ�û���˸�Ȩ��");
        pressanykey();
        return -1;
    }

    getdata(11, 0, "������������� [0]", day, 4, DOECHO, NULL, true);
    i = 0;
    while (day[i]) {
        if (!isdigit(day[i]))
            return -1;
        i++;
    }
    j = atoi(day);
    if (j <= 0)
        return -1;

    if (compute_user_value(getCurrentUser()) <= j) {
        prints("\n\n�Բ������������Դ���������...");
        pressanykey();
        return -1;
    }
    iDays = j;
    j = time(0) / 3600 / 24 + j;

    move(13, 0);

    if (askyn("��ȷ��Ҫ������\x1b[1;31m(ע��:����֮������ǰ�⿪!\x1b[m)��", 0) == 1) {
        getdata(15, 0, "����������: ", buf, 39, NOECHO, NULL, true);
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return -1;
        }
        switch (ans[0]) {
        case '1':
            getCurrentUser()->userlevel &= ~PERM_BASIC;
            break;
        case '2':
            getCurrentUser()->userlevel &= ~PERM_POST;
            break;
        case '3':
            getCurrentUser()->userlevel &= ~PERM_CHAT;
            break;
        case '4':
            getCurrentUser()->userlevel &= ~PERM_PAGE;
            break;
        case '5':
            getCurrentUser()->userlevel |= PERM_DENYMAIL;
            break;
        case '6':
            getCurrentUser()->userlevel |= PERM_DENYRELAX;
            break;
        }

        s[ans[0]-49]=j;
        save_giveupinfo(getCurrentUser(),s);

        gettmpfilename( genbuf, "giveup" );
        if ((fn = fopen(genbuf, "w")) != NULL) {
            fprintf(fn, "\033[1m%s\033[m �� %s %d ��", getCurrentUser()->userid, desc[ans[0] - '1'], iDays);
            getuinfo(fn, getCurrentUser());
            fclose(fn);
            sprintf(buf, "%s �Ľ���֪ͨ", getCurrentUser()->userid);
            post_file(getCurrentUser(), "", genbuf, "GiveupNotice", buf, 0,  2, getSession());
            unlink(genbuf);
        }

        prints("\n\n���Ѿ���ʼ������");
        pressanykey();
        if (ans[0] == '1')
            abort_bbs(0);
    }
    return 0;
}

#if 0
static void mail_info()
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now = time(0);
    sprintf(filename, "etc/%s.tmp", getCurrentUser()->userid);
    fn = fopen(filename, "w");
    fprintf(fn, "\033[1m%s\033[m �Ѿ��� \033[1m%24.24s\033[m ��ɱ�ˣ���������(��)�����ϣ��뱣��...", getCurrentUser()->userid, ctime(&now));
    getuinfo(fn, getCurrentUser());
    fprintf(fn, "\n                      \033[1m ϵͳ�Զ�����ϵͳ��\033[m\n");
    fclose(fn);
    mail_file(getCurrentUser()->userid, filename, "acmanager", "��ɱ֪ͨ....", BBSPOST_MOVE, NULL);
}
void offline()
{
    modify_user_mode(OFFLINE);

    if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
        return;
    clear();
    move(1, 0);
    prints("\033[32m���ѹ��.....\033[m");
    move(3, 0);
    if (askyn("��ȷ��Ҫ�뿪������ͥ", 0) == 1) {
        clear();
        if (d_user(getCurrentUser()->userid) == 1) {
            mail_info();
            kick_user(&uinfo);
            exit(0);
        }
    }
}
#endif

int d_user(cid)
    char cid[IDLEN];
{
    int uid, fd;
    char tmpbuf[30];
    char userid[IDLEN + 2];
    struct userec *lookupuser;
       /* ������ʾ�û���Ϣ Bigman:2003.5.11*/
        struct userec uinfo1;
	/*struct userdata ud;*/

    if (uinfo.mode != OFFLINE) {
#ifdef SOURCE_PERM_CHECK
        if (!HAS_PERM(getCurrentUser(), PERM_ADMIN)) {
            move(3, 0);
            clrtobot();
            prints("��Ǹ, ��û�� ADMIN Ȩ��!");
            pressreturn();
            return 0;
        }
#endif
        modify_user_mode(ADMIN);
        if (!check_systempasswd()) {
            return 0;
        }
        clear();
        stand_title("ɾ��ʹ�����ʺ�");
        move(1, 0);
        usercomplete("��������ɾ����ʹ���ߴ���: ", userid);
        if (userid[0] == '\0') {
            clear();
            return 0;
        }
    } else
        strcpy(userid, cid);
    if (!(uid = getuser(userid, &lookupuser))) {
        move(3, 0);
        prints(MSG_ERR_USERID);
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
     /* ������ʾ�û���Ϣ Bigman:2003.5.11*/
    uinfo1 = *lookupuser;
    clrtobot();

    disply_userinfo(&uinfo1, 1);

    /*    if (!isalpha(lookupuser->userid[0])) return 0; */
    /* rrr - don't know how... */
    move(22, 0);
    if (uinfo.mode != OFFLINE)
        prints("ɾ��ʹ���� '%s'.", userid);
    else
        prints(" %s ���뿪����", cid);
/*    clrtoeol(); */
    
    getdata(24, 0, "(Yes, or No) [No](ע��Ҫ����ȫ���ʺʹ�Сд): ", genbuf, 5, DOECHO, NULL, true);
    if (strcmp(genbuf,"Yes")) { /* if not yes quit */
        move(24, 0);
        if (uinfo.mode != OFFLINE)
            prints("ȡ��ɾ��ʹ����...\n");
        else
            prints("�����ڻ���ת���ˣ��ø����...");
        pressreturn();
        clear();
        return 0;
    }
    if (uinfo.mode != OFFLINE) {
        char secu[STRLEN];

        sprintf(secu, "ɾ��ʹ���ߣ�%s", lookupuser->userid);
        securityreport(secu, lookupuser, NULL, getSession());
    }
    newbbslog(BBSLOG_USER,"%s deleted user %s", getCurrentUser()->userid, lookupuser->userid);
    /*Haohmaru.99.12.23.��ɾIDһ�����ڲ���ע�� */
    if ((fd = open(".badname", O_WRONLY | O_CREAT, 0644)) != -1) {
        char buf[STRLEN];
        char thtime[40];
        time_t dtime;

        dtime = time(0);
        sprintf(thtime, "%lu", dtime);
        flock(fd, LOCK_EX);
        lseek(fd, 0, SEEK_END);
        sprintf(buf, "%-12.12s %-66.66s\n", lookupuser->userid, thtime);
        write(fd, buf, strlen(buf));
        flock(fd, LOCK_UN);
        close(fd);
    } else {
        printf("�����뱨��SYSOP");
        pressanykey();
    }
    setmailpath(tmpbuf, lookupuser->userid);
    /*
       sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
     */
    f_rm(tmpbuf);
    sethomepath(tmpbuf, lookupuser->userid);
    /*
       sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
       sprintf(genbuf,"/bin/rm -fr tmp/email/%s", lookupuser->userid) ;
     */
    f_rm(tmpbuf);
    sprintf(genbuf, "tmp/email/%s", lookupuser->userid);
    f_rm(genbuf);
    kick_user_utmp(uid, NULL, SIGKILL);
    setuserid(uid, "");
    lookupuser->userlevel = 0;
    /*strcpy(lookupuser->address, "");*/
    strcpy(lookupuser->username, "");
    /*strcpy(lookupuser->realname, "");*/
	/*read_userdata(lookupuser->userid, &ud);
	strcpy(ud.address, "");
	strcpy(ud.realname, "");
	write_userdata(lookupuser->userid, &ud);*/
/*    lookupuser->userid[0] = '\0' ; */
    move(24, 0);
    prints("%s �Ѿ��Ѿ��ͱ���ͥʧȥ����....\n", userid);
    pressreturn();

    clear();
    return 1;
}

int kick_user(int uid, char *userid, struct user_info *userinfo) {
    newbbslog(BBSLOG_USER, "kicked %s", userid);
    if (strcmp(getCurrentUser()->userid, userid)) {
        char buf[STRLEN];
        sprintf(buf, "%s �߳�ʹ���� %s", getCurrentUser()->userid, userid);
        securityreport(buf, NULL, NULL, getSession());
    }
    return kick_user_utmp(uid, userinfo, 0);
}

int kick_user_menu(void){
    char userid[STRLEN], ans[10];
    int uid;
    struct userec *u;

    modify_user_mode(ADMIN);
    stand_title("���û�");
    move(1, 0);
    usercomplete("����Ҫ�߳����û���: ", userid);
    if (*userid == '\0') {
        clear();
        return 0;
    }
    if (!(uid = getuser(userid, &u))) {       /* change getuser -> searchuser, by dong, 1999.10.26 */
        move(3, 0);
        prints("�û�������");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    strcpy(userid, u->userid);
    move(1, 0);
    prints("�߳��û� '%s'.", userid);
    clrtoeol();
    getdata(2, 0, "(Yes, or No) [N]: ", ans, 4, DOECHO, NULL, true);
    if (ans[0] != 'Y' && ans[0] != 'y') {     /* if not yes quit */
        move(2, 0);
        prints("ȡ��\n");
        pressreturn();
        clear();
        return 0;
    }
    kick_user(uid, userid, NULL);
    move(4, 0);
    prints("�û��ѱ���\n");
    pressreturn();
    clear();
    return 1;
}

