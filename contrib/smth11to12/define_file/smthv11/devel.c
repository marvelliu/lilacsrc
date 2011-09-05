#include "bbs.h"

//�Զ�ͨ��ע��ĺ���  binxun
int auto_register(char *userid, char *email, int msize)
{
    struct userdata ud;
    struct userec *uc;
    char *item, *temp;
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
        fprintf(fout, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s�Զ�ͨ��ע��[m\n", userid);
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
