
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

/* XXX: what are these bird words above? should it be cut after the rewrite of deny? - etnlegend */

#include "bbs.h"
#include "read.h"

/*Add by SmallPig*/

extern int ingetdata;

int listdeny(int page)
{                               /* Haohmaru.12.18.98.Ϊ��Щ��̬�÷��˳���һ���İ�����д */
    FILE *fp;
    int x = 0, y = 3, cnt = 0, max = 0, len;
    int i;
    char u_buf[STRLEN * 2], line[STRLEN * 2], *nick;

    clear();
    prints("�趨�޷� Post ������\n");
    move(y, x);
    CreateNameList();
    setbfile(genbuf, currboard->filename, "deny_users");
    if ((fp = fopen(genbuf, "r")) == NULL) {
        prints("(none)\n");
        return 0;
    }
    for (i = 1; i <= page * 20; i++) {
        if (fgets(genbuf, 2 * STRLEN, fp) == NULL)
            break;
    }
    while (fgets(genbuf, 2 * STRLEN, fp) != NULL) {
        strtok(genbuf, " \n\r\t");
        strcpy(u_buf, genbuf);
        AddNameList(u_buf);
        nick = (char *) strtok(NULL, "\n\r\t");
        if (nick != NULL) {
            while (*nick == ' ')
                nick++;
            if (*nick == '\0')
                nick = NULL;
        }
        if (nick == NULL) {
            strcpy(line, u_buf);
        } else {
            if (cnt < 20)
                sprintf(line, "%-12s%s", u_buf, nick);
        }
        if ((len = strlen(line)) > max)
            max = len;
/*        if( x + len > 90 )
            line[ 90 - x ] = '\0';*-P-*/
        if (x + len > 79)
            line[79] = '\0';
        if (cnt < 20)           /*haohmaru.12.19.98 */
            prints("%s", line);
        cnt++;
        if ((++y) >= t_lines - 1) {
            y = 3;
            x += max + 2;
            max = 0;
            /*
             * if( x > 90 )  break; 
             */
        }
        move(y, x);
    }
    fclose(fp);
    if (cnt == 0)
        prints("(none)\n");
    return cnt;
}

int addtodeny(char *uident)
{                               /* ��� ��ֹPOST�û� */
    char buf2[50], strtosave[256], date[STRLEN] = "0";
    int maxdeny;

    /*
     * Haohmaru.99.4.1.auto notify 
     */
    time_t now;
    char buffer[STRLEN];
    FILE *fn;
    char filename[STRLEN];
    int autofree = 0;
    char filebuf[STRLEN];
    char denymsg[STRLEN];
    int denyday;
    int reasonfile;
	int gdataret;

    now = time(0);
    strncpy(date, ctime(&now) + 4, 7);
    setbfile(genbuf, currboard->filename, "deny_users");
    if (seek_in_file(genbuf, uident) || !strcmp(currboard->filename, "denypost"))
        return -1;
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS))
        maxdeny = 70;
    else
        maxdeny = 14;

    *denymsg = 0;
    if ((reasonfile = open("etc/deny_reason", O_RDONLY)) != -1) {
        int reason = -1;
        int maxreason;
        char *file_buf;
        char *denymsglist[50];
        struct stat st;

        move(3, 0);
        clrtobot();
        if (fstat(reasonfile, &st) == 0) {
            int i;

            file_buf = (char *) malloc(st.st_size);
            read(reasonfile, file_buf, st.st_size);
            maxreason = 1;
            denymsglist[0] = file_buf;
            prints("%s", "��������б�.\n");
            for (i = 1; i < st.st_size; i++) {
                if (file_buf[i] == '\n') {
                    file_buf[i] = 0;
                    prints("%d.%s\n", maxreason, denymsglist[maxreason - 1]);
                    if (i == st.st_size - 1)
                        break;
                    if (file_buf[i + 1] == '\r') {
                        if (i + 1 == st.st_size - 1)
                            break;
                        denymsglist[maxreason] = &file_buf[i + 2];
                        maxreason++;
                        i += 2;
                    } else {
                        denymsglist[maxreason] = &file_buf[i + 1];
                        maxreason++;
                        i++;
                    }
                }
            }
            prints("%s", "0.�ֶ�����������");
            while (1) {
                gdataret = getdata(2, 0, "����б�ѡ��������(0Ϊ�ֹ�����,*�˳�):", denymsg, 2, DOECHO, NULL, true);
                if (gdataret == -1 || denymsg[0] == '*') {
                    free(file_buf);
                    close(reasonfile);
                    return 0;
                }
                if (isdigit(denymsg[0])) {
                    reason = atoi(denymsg);
                    if (reason == 0) {
                        denymsg[0] = 0;
                        move(2, 0);
                        clrtobot();
                        break;
                    }
                    if (reason <= maxreason) {
                        strncpy(denymsg, denymsglist[reason - 1], STRLEN - 1);
                        denymsg[STRLEN - 1] = 0;
                        move(2, 0);
                        clrtobot();
                        prints("�������: %s\n", denymsg);
                        break;
                    }
                }
                move(3, 0);
				clrtoeol();
                prints("%s", "�������!");
            }
            free(file_buf);
        }
        close(reasonfile);
    }

	gdataret = 0;
    while (gdataret != -1 && 0 == strlen(denymsg)) {
        gdataret = getdata(2, 0, "����˵��(��*ȡ��): ", denymsg, 30, DOECHO, NULL, true);
    }
    if (gdataret == -1 || denymsg[0] == '*')
        return 0;
#ifdef MANUAL_DENY
    autofree = askyn("�÷���Ƿ��Զ���⣿(ѡ \033[1;31mY\033[m ��ʾ�����Զ����)", true);
#else
    autofree = true;
#endif
    sprintf(filebuf, "��������(�%d��)(��*ȡ�����)", maxdeny);
    denyday = 0;
    while (!denyday) {
        gdataret = getdata(3, 0, filebuf, buf2, 4, DOECHO, NULL, true);
	if (gdataret == -1 || buf2[0] == '*')return 0; 
        if ((buf2[0] < '0') || (buf2[0] > '9'))
            continue;           /*goto MUST1; */
        denyday = atoi(buf2);
        if ((denyday < 0) || (denyday > maxdeny))
            denyday = 0;        /*goto MUST1; */
        else if (!(HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) && !denyday)
            denyday = 0;        /*goto MUST1; */
        else if ((HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) && !denyday && !autofree)
            break;
    }
#ifdef ZIXIA
	int ndenypic,dpcount;
	clear();
	dpcount=CountDenyPic(DENYPIC);
	sprintf(filebuf, "ѡ������ͼ(1-%d)(0Ϊ���ѡ��,VΪ��ͼƬ)[0]:",dpcount);
	ndenypic=-1;
	while(ndenypic>dpcount || ndenypic<0)
	{
		getdata(0,0,filebuf,buf2,4, DOECHO, NULL, true);
		if(buf2[0]=='v' ||buf2[0]=='V' )
		{
			ansimore(DENYPIC, 0);
			continue;
		}
		if(buf2[0]=='\0')
		{
			ndenypic=0;
			break;
		}
		if ((buf2[0] < '0') || (buf2[0] > '9'))
		       continue; 
		ndenypic = atoi(buf2);
	}
#endif
    if (denyday && autofree) {
        struct tm *tmtime;
        time_t undenytime = now + denyday * 24 * 60 * 60;

        tmtime = gmtime(&undenytime);

        sprintf(strtosave, "%-12.12s %-30.30s%-12.12s %2d��%2d�ս�\x1b[%lum", uident, denymsg, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime);   /*Haohmaru 98,09,25,��ʾ��˭ʲôʱ���� */
    } else {
        struct tm *tmtime;
        time_t undenytime = now + denyday * 24 * 60 * 60;

        tmtime = gmtime(&undenytime);
        sprintf(strtosave, "%-12.12s %-30.30s%-12.12s %2d��%2d�պ�\x1b[%lum", uident, denymsg, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime);
    }

    if (addtofile(genbuf, strtosave) == 1) {
        struct userec *lookupuser, *saveptr;
        int my_flag = 0;        /* Bigman. 2001.2.19 */
        struct userec saveuser;

        /*
         * Haohmaru.4.1.�Զ�����֪ͨ���������ڰ��� 
         */
		gettmpfilename( filename, "deny" );
        //sprintf(filename, "tmp/%s.deny", getCurrentUser()->userid);
        fn = fopen(filename, "w+");
        memcpy(&saveuser, getCurrentUser(), sizeof(struct userec));
        saveptr = getCurrentUser();
        getCurrentUser() = &saveuser;
        sprintf(buffer, "%s��ȡ����%s��ķ���Ȩ��", uident, currboard->filename);

        if ((HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) && !chk_BM_instr(currBM, getCurrentUser()->userid)) {
            my_flag = 0;
            fprintf(fn, "������: SYSOP (System Operator) \n");
            fprintf(fn, "��  ��: %s\n", buffer);
            fprintf(fn, "����վ: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "��  Դ: %s\n", NAME_BBS_ENGLISH);
            fprintf(fn, "\n");
            fprintf(fn, "�������� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m���Һ��ź���֪ͨ���� \n", currboard->filename, denymsg);
            if (denyday)
                fprintf(fn, DENY_DESC_AUTOFREE " \x1b[4m%d\x1b[m ��", denyday);
            else
                fprintf(fn, DENY_DESC_NOAUTOFREE);
            if (!autofree)
                fprintf(fn, "�����ں���ظ�\n��������ָ�Ȩ�ޡ�\n");
            fprintf(fn, "\n");
#ifdef ZIXIA
            ndenypic=GetDenyPic(fn, DENYPIC, ndenypic, dpcount);
#endif
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "��\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, getCurrentUser()->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
            strcpy(getCurrentUser()->userid, "SYSOP");
            strcpy(getCurrentUser()->username, NAME_SYSOP);
            /*strcpy(getCurrentUser()->realname, NAME_SYSOP);*/
        } else {
            my_flag = 1;
            fprintf(fn, "������: %s \n", getCurrentUser()->userid);
            fprintf(fn, "��  ��: %s\n", buffer);
            fprintf(fn, "����վ: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "��  Դ: %s \n", SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
            fprintf(fn, "\n");
            fprintf(fn, "�������� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m���Һ��ź���֪ͨ���� \n", currboard->filename, denymsg);
            if (denyday)
                fprintf(fn, DENY_DESC_AUTOFREE " \x1b[4m%d\x1b[m ��", denyday);
            else
                fprintf(fn, DENY_DESC_NOAUTOFREE);
            if (!autofree)
                fprintf(fn, "�����ں���ظ�\n��������ָ�Ȩ�ޡ�\n");
            fprintf(fn, "\n");
#ifdef ZIXIA
            ndenypic=GetDenyPic(fn, DENYPIC, ndenypic, dpcount);
#endif
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", getCurrentUser()->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
        }
        fclose(fn);
        mail_file(getCurrentUser()->userid, filename, uident, buffer, 0, NULL);
        fn = fopen(filename, "w+");
        fprintf(fn, "���� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m ��� \x1b[4m%s\x1b[m ��Ϊ��\n", uident, currboard->filename, denymsg);
        if (denyday)
            fprintf(fn, DENY_BOARD_AUTOFREE " \x1b[4m%d\x1b[m �졣\n", denyday);
        else
            fprintf(fn, DENY_BOARD_NOAUTOFREE "\n");
#ifdef ZIXIA
	GetDenyPic(fn, DENYPIC, ndenypic, dpcount);
#endif
        if (my_flag == 0) {
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "��\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, saveptr->userid);
        } else {
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", getCurrentUser()->userid);
        }
        fprintf(fn, "                              %s\n", ctime(&now));
        fclose(fn);
        post_file(getCurrentUser(), "", filename, currboard->filename, buffer, 0, 2, getSession());
        /*
         * unlink(filename); 
         */
        getCurrentUser() = saveptr;

        sprintf(buffer, "%s �� %s �������POSTȨ", uident, getCurrentUser()->userid);
        getuser(uident, &lookupuser);

        if (PERM_BOARDS & lookupuser->userlevel)
            sprintf(buffer, "%s ��ĳ��" NAME_BM " %s �� %s", getCurrentUser()->userid, uident, currboard->filename);
        else
            sprintf(buffer, "%s �� %s �� %s", getCurrentUser()->userid, uident, currboard->filename);
        post_file(getCurrentUser(), "", filename, "denypost", buffer, 0, -1, getSession());
        unlink(filename);
        bmlog(getCurrentUser()->userid, currboard->filename, 10, 1);
    }
    return 0;
}


int deny_user(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{                               /* ��ֹPOST�û����� ά�������� */
    char uident[STRLEN];
    int page = 0;
    char ans[10];
    int count;

    /*
     * Haohmaru.99.4.1.auto notify 
     */
    time_t now;
    int id;
    FILE *fp;
    int find;                   /*Haohmaru.99.12.09 */

/*   static page=0; *//*
 * * Haohmaru.12.18 
 */
    now = time(0);
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currBM, getCurrentUser())) {
            return DONOTHING;
        }

    while (1) {
        char querybuf[0xff];
        char LtNing[24];
        if (fileinfo == NULL) LtNing[0] = '\0';
        else sprintf(LtNing, "(O)����%s ", fileinfo->owner);

      Here:
        clear();
        count = listdeny(0);
        if (count > 0 && count < 20)    /*Haohmaru.12.18,����һ�� */
            snprintf(querybuf, 0xff, "%s(A)���� (D)ɾ�� or (E)�뿪 [E]: ", LtNing);
        else if (count >= 20)
            snprintf(querybuf, 0xff, "%s(A)���� (D)ɾ�� (N)�����N�� or (E)�뿪 [E]: ", LtNing);
        else
            snprintf(querybuf, 0xff, "%s(A)���� or (E)�뿪 [E]: ", LtNing);

        getdata(1, 0, querybuf, ans, 7, DOECHO, NULL, true);
        *ans = (char) toupper((int) *ans);

        if (*ans == 'A' || (*ans == 'O' && fileinfo != NULL)) {
            struct userec *denyuser;

            move(1, 0);
            if (*ans == 'A')
                usercomplete("�����޷� POST ��ʹ����: ", uident);
            else
                strncpy(uident, fileinfo->owner, STRLEN - 4);
            /*
             * Haohmaru.99.4.1,���ӱ���ID��ȷ�Լ�� 
             */
            if (!(id = getuser(uident, &denyuser))) {   /* change getuser -> searchuser , by dong, 1999.10.26 */
                move(3, 0);
                prints("�Ƿ� ID");
                clrtoeol();
                pressreturn();
                goto Here;
            }
            strncpy(uident, denyuser->userid, IDLEN);
            uident[IDLEN] = 0;
            
            /*
             * windinsn.04.5.17,��׼��� guest �� SYSOP
             */
            if (!strcasecmp(uident,"guest") 
                || !strcasecmp(uident,"SYSOP")
#ifdef ZIXIA
                || !strcasecmp(uident,"SuperMM")
#endif
            ) {
                move(3, 0);
                prints("���ܷ�� %s", uident);
                clrtoeol();
                pressreturn();
                goto Here;
            }
            
            if (*uident != '\0') {
                addtodeny(uident);
            }
        } else if ((*ans == 'D') && count) {
            int len;

            move(1, 0);
            sprintf(genbuf, "ɾ���޷� POST ��ʹ����: ");
            getdata(1, 0, genbuf, uident, 13, DOECHO, NULL, true);
            find = 0;           /*Haohmaru.99.12.09.ԭ���Ĵ����������������ɱ��ɾ������ */
            setbfile(genbuf, currboard->filename, "deny_users");
            if ((fp = fopen(genbuf, "r")) == NULL) {
                prints("(none)\n");
                return 0;
            }
            len = strlen(uident);
			if(len){
            while (fgets(genbuf, 256 /*STRLEN*/, fp) != NULL) {
                if (!strncasecmp(genbuf, uident, len)) {
                    if (genbuf[len] == 32) {
                        strncpy(uident, genbuf, len);
                        uident[len] = 0;
                        find = 1;
                        break;
                    }
                }
            }
			}
            fclose(fp);
            if (!find) {
                move(3, 0);
                prints("��ID���ڷ��������!");
                clrtoeol();
                pressreturn();
                goto Here;
            }
            /*---	add to check if undeny time reached.	by period 2000-09-11	---*/
            {
                char *lptr;
                time_t ldenytime;

                /*
                 * now the corresponding line in genbuf 
                 */
                if (NULL != (lptr = strrchr(genbuf, '[')))
                    sscanf(lptr + 1, "%lu", &ldenytime);
                else
                    ldenytime = now + 1;
                if (ldenytime > now) {
                    move(3, 0);
                    prints(genbuf);
                    if (false == askyn("���û����ʱ��δ����ȷ��Ҫ��⣿", false /*, false */ ))
                        goto Here;      /* I hate Goto!!! */
                }
            }
            /*---		---*/
            move(1, 0);
            clrtoeol();
            if (uident[0] != '\0') {
                if (deldeny(getCurrentUser(), currboard->filename, uident, 0, getSession())) {
                }
            }
        } else if (count > 20 && isdigit(ans[0])) {
            if (ans[1] == 0)
                page = *ans - '0';
            else
                page = atoi(ans);
            if (page < 0)
                break;          /*������˳���10����?�ǿ���200�˰�!  ��ģ� */
            listdeny(page);
            pressanykey();
        } else
            break;
    }                           /*end of while */
    clear();
    return FULLUPDATE;
}

/* etnlegend, 2005.12.26, ���ֲ���Ȩ����ӿڽṹ���� */
static int func_query_club_users(struct userec *user,void *varg){
    if(user->userid[0]&&get_user_club_perm(user,currboard,*(int*)varg))
        AddNameList(user->userid);
    return 0;
}
static int func_remove_club_users(struct userec *user,void *varg){
    if(user->userid[0]&&get_user_club_perm(user,currboard,*(int*)varg)&&!del_user_club_perm(user,currboard,*(int*)varg)){
        if(!(*(int*)varg)&&user==getCurrentUser()&&!check_read_perm(user,currboard))
            set_user_club_perm(user,currboard,*(int*)varg);
        else
            AddNameList(user->userid);
    }
    return 0;
}
static int func_dump_users(char *userid,void *varg){
    ((char**)(((void**)varg)[0]))[(*(int*)(((void**)varg)[1]))]=userid;
    (*(int*)(((void**)varg)[1]))++;
    return 0;
}
static int func_clear_send_mail(char *userid,void *varg){
    return club_maintain_send_mail(userid,(char*)(((void**)varg)[0]),1,(*(int*)(((void**)varg)[1])),currboard,getSession());
}
typedef int (*APPLY_USERS_FUNC) (int(*)(struct userec*,void*),void*);
int clubmember(struct _select_def *conf,struct fileheader *fh,void *varg){
    static const char *title="\033[1;32m[�趨���ֲ���Ȩ�û�]\033[m";
    static const char *echo="\033[1;37m��ҳ[\033[1;36m<SP>\033[1;37m]/����[\033[1;36mA\033[1;37m]"
        "/ɾ��[\033[1;36mD\033[1;37m]/����[\033[1;36mI\033[1;37m]/����[\033[1;36mC\033[1;37m]"
        "/Ⱥ��[\033[1;36mM\033[1;37m]/�˳�[\033[1;36m<ESC>\033[1;37m/\033[1;36m<CR>\033[1;37m]: \033[m";
    static const char *choice="\033[1;37m������ȡ[R]/����[\033[1;36mP\033[1;37m]Ȩ���б� [R]: \033[m";
    static char comment[128]="";
    APPLY_USERS_FUNC func=(APPLY_USERS_FUNC)apply_users;
    NLNode *head,*start,*curr;
    FILE *fp;
    struct userec *user;
    char genbuf[1024],buf[256],line[256],fn[256],userid[16],ans[4],**p_users;
    int i,j,k,write_perm,need_refresh,count,page;
    void *arg[2];
    if(!chk_currBM(currBM,getCurrentUser()))
        return DONOTHING;
    if(!(currboard->flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))||!(currboard->clubnum>0)||(currboard->clubnum>MAXCLUB))
        return DONOTHING;
    clear();
    if((currboard->flag&BOARD_CLUB_READ)&&(currboard->flag&BOARD_CLUB_WRITE)){
        move(0,0);
        prints("%s",title);
        getdata(1,0,(char*)choice,ans,2,DOECHO,NULL,true);
        ans[0]=toupper(ans[0]);
        write_perm=(ans[0]=='P');
    }
    else
        write_perm=(currboard->flag&BOARD_CLUB_WRITE);
    move(0,0);
    clrtoeol();
    prints("%s \033[1;36m<%s>\033[m",title,(!write_perm?"��ȡ":"����"));
    need_refresh=1;
    count=0;
    page=0;
    head=NULL;
    start=NULL;
    while(1){
        if(need_refresh){
            CreateNameList();
            func(func_query_club_users,&write_perm);
            SortNameList(0);
            count=GetNameListCount();
            head=GetNameListHead();
            start=head;
            curr=head;
            page=0;
            need_refresh=0;
        }
        else
            curr=start;
        move(1,0);
        clrtobot();
        move(3,0);
        if(!count)
            prints("\033[1;33m%s\033[m","������Ȩ�û�...");
        else{
            j=0;
            do{
                k=MaxLen(curr,t_lines-4);
                if(!((j+k)<t_columns))
                    break;
                for(i=3;i<t_lines-1;i++){
                    move(i,j);
                    prints("%s",curr->word);
                    if(!(curr=curr->next))
                        break;
                }
                j+=(k+2);
            }
            while(curr);
            move(t_lines-1,0);
            if(!curr){
                if(!page)
                    sprintf(genbuf,"��ǰ�� %d ҳ, �� 1 ҳ, ���Ľ��� ...",page+1);
                else
                    sprintf(genbuf,"��ǰ�� %d ҳ, Ϊ�б�����һҳ, �� <SPACE> ���ص��� 1 ҳ ...",page+1);
            }
            else
                sprintf(genbuf,"��ǰ�� %d ҳ, �� <SPACE> ��������һҳ ...",page+1);
            for(i=strlen(genbuf);i<t_columns;i++)
                genbuf[i]=32;
            genbuf[i]=0;
            prints("\033[1;32;42m%s\033[m",genbuf);
        }
        move(1,0);
        prints("%s",echo);
        ingetdata=1;
        do{
            ans[1]=0;
            switch(ans[0]=igetkey()){
                case 10:
                case 13:
                case 27:
                case 32:
                case 'a':
                case 'A':
                case 'd':
                case 'D':
                case 'i':
                case 'I':
                case 'c':
                case 'C':
                case 'm':
                case 'M':
                    ans[1]=1;
                    break;
                default:
                    continue;
            }
        }
        while(!ans[1]);
        ingetdata=0;
        ans[0]=toupper(ans[0]);
        if(ans[0]==32){
            if(!curr){
                start=head;
                page=0;
            }
            else{
                start=curr;
                page++;
            }
        }
        else if(ans[0]=='A'){
            move(1,0);
            clrtobot();
            usercomplete("���Ӿ��ֲ���Ȩ�û�: ",buf);
            move(1,0);
            clrtobot();
            snprintf(userid,16,"%s",buf);
            if(!userid[0]||!getuser(userid,&user)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","������û���...");
                WAIT_RETURN;
                continue;
            }
            if(!strcmp(user->userid,"guest")){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","������� guest ������Ȩ...");
                WAIT_RETURN;
                continue;
            }
            if(get_user_club_perm(user,currboard,write_perm)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���û��Ѿ�����Ȩ...");
                WAIT_RETURN;
                continue;
            }
            prints("\033[1;37m���Ӿ��ֲ���Ȩ�û�: \033[1;32m%s\033[m",user->userid);
            sprintf(genbuf,"\033[1;37m����˵�� [\033[1;36m%s\033[1;37m]: \033[m",comment);
            getdata(2,0,genbuf,buf,64,DOECHO,NULL,true);
            if(buf[0]){
                trimstr(buf);
                snprintf(comment,128,"%s",buf);
            }
            sprintf(genbuf,"\033[1;33mȷ������ \033[1;32m%s\033[1;33m �����ֲ�%sȨ�� [y/N]: \033[m",
                user->userid,(!write_perm?"��ȡ":"����"));
            getdata(3,0,genbuf,ans,2,DOECHO,NULL,true);
            ans[0]=toupper(ans[0]);
            if(ans[0]!='Y')
                continue;
            need_refresh=1;
            move(4,0);
            if(set_user_club_perm(user,currboard,write_perm)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���������з�������...");
                WAIT_RETURN;
                continue;
            }
            club_maintain_send_mail(user->userid,comment,0,write_perm,currboard,getSession());
            prints("\033[1;32m%s\033[0;33m<Enter>\033[m","���ӳɹ�!");
            WAIT_RETURN;
        }
        else if(ans[0]=='D'){
            move(1,0);
            clrtoeol();
            namecomplete("ɾ�����ֲ���Ȩ�û�: ",buf);
            move(1,0);
            clrtobot();
            snprintf(userid,16,"%s",buf);
            if(!userid[0]||!getuser(userid,&user)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","������û���...");
                WAIT_RETURN;
                continue;
            }
            if(!get_user_club_perm(user,currboard,write_perm)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���û���δ����Ȩ...");
                WAIT_RETURN;
                continue;
            }
            prints("\033[1;37mɾ�����ֲ���Ȩ�û�: \033[1;32m%s\033[m",user->userid);
            sprintf(genbuf,"\033[1;37m����˵�� [\033[1;36m%s\033[1;37m]: \033[m",comment);
            getdata(2,0,genbuf,buf,64,DOECHO,NULL,true);
            if(buf[0]){
                trimstr(buf);
                snprintf(comment,128,"%s",buf);
            }
            sprintf(genbuf,"\033[1;33mȷ��ȡ�� \033[1;32m%s\033[1;33m �����ֲ�%sȨ�� [y/N]: \033[m",
                user->userid,(!write_perm?"��ȡ":"����"));
            getdata(3,0,genbuf,ans,2,DOECHO,NULL,true);
            ans[0]=toupper(ans[0]);
            if(ans[0]!='Y')
                continue;
            need_refresh=1;
            move(4,0);
            if(del_user_club_perm(user,currboard,write_perm)){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���������з�������...");
                WAIT_RETURN;
                continue;
            }
            club_maintain_send_mail(user->userid,comment,1,write_perm,currboard,getSession());
            prints("\033[1;32m%s\033[0;33m<Enter>\033[m","ɾ���ɹ�!");
            WAIT_RETURN;
        }
        else if(ans[0]=='I'){
            move(1,0);
            clrtobot();
            sprintf(genbuf,"\033[1;37m����˵�� [\033[1;36m%s\033[1;37m]: \033[m",comment);
            getdata(1,0,genbuf,buf,64,DOECHO,NULL,true);
            if(buf[0]){
                trimstr(buf);
                snprintf(comment,128,"%s",buf);
            }
            move(3,0);
            prints("%s",
                "    \033[1;33m[�����������ֲ���Ȩ�б���Ϣ�ļ���ʽ]\033[m\n\n"
                "    \033[1;37m����Ϊ��λ, ÿ�в���һλ�û�, ����ǰ׺����:\033[m\n"
                "    \033[1;33m#\033[1;37m ��ʼ����Ϊע����, ������;\033[m\n"
                "    \033[1;31m-\033[1;37m ��ʼ���б�ʾɾ�������û�;\033[m\n"
                "    \033[1;32m+\033[1;37m ��ʼ���б�ʾ���������û�;\033[m\n"
                "    \033[1;37m��ǰ׺ʱĬ�ϲ���Ϊ����...\033[m\n\n"
                "\033[1;37m�� \033[1;32m<Enter>\033[1;37m ����ʼ�༭�����޸��б�: \033[m");
            WAIT_RETURN;
            saveline(0,0,NULL);
            j=uinfo.mode;
            modify_user_mode(EDITANN);
            sprintf(fn,"tmp/club_perm_%ld_%d",time(NULL),getpid());
            k=vedit(fn,0,NULL,NULL,0);
            modify_user_mode(j);
            clear();
            saveline(0,1,NULL);
            move(1,0);
            if(k==-1){
                unlink(fn);
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","ȡ��...");
                WAIT_RETURN;
                continue;
            }
            else{
                sprintf(genbuf,"\033[1;33mȷ�������������ֲ�%s��Ȩ�б� [y/N]: \033[m",(!write_perm?"��ȡ":"����"));
                getdata(1,0,genbuf,ans,2,DOECHO,NULL,true);
                ans[0]=toupper(ans[0]);
                if(ans[0]!='Y')
                    continue;
            }
            if(!(fp=fopen(fn,"r")))
                continue;
            need_refresh=1;
            i=0;
            j=0;
            while(fgets(line,256,fp)){
                k=strlen(line);
                if(line[k-1]==10||line[k-1]==13)
                    line[k-1]=0;
                trimstr(line);
                switch(line[0]){
                    case 0:
                    case 10:
                    case 13:
                    case '#':
                        continue;
                    case '-':
                        trimstr(&line[1]);
                        if(!getuser(&line[1],&user)||!get_user_club_perm(user,currboard,write_perm))
                            continue;
                        if(!del_user_club_perm(user,currboard,write_perm)){
                            club_maintain_send_mail(user->userid,comment,1,write_perm,currboard,getSession());
                            j++;
                        }
                        break;
                    case '+':
                        line[0]=32;
                        trimstr(line);
                    default:
                        if(!getuser(line,&user)||!strcmp(user->userid,"guest")||get_user_club_perm(user,currboard,write_perm))
                            continue;
                        if(!set_user_club_perm(user,currboard,write_perm)){
                            club_maintain_send_mail(user->userid,comment,0,write_perm,currboard,getSession());
                            i++;
                        }
                        break;
                }
            }
            fclose(fp);
            unlink(fn);
            move(2,0);
            prints("\033[1;37m���� \033[1;33m%d\033[1;37m λ�û�, ɾ�� \033[1;33m%d\033[1;37m λ�û�..."
                "\033[1;32m%s\033[0;33m<Enter>\033[m",i,j,"���������!");
            WAIT_RETURN;
        }
        else if(ans[0]=='M'){
            /* ע: ���ֲ�Ⱥ�Ų���ԭ����Ϊ asing@zixia */
            if(HAS_PERM(getCurrentUser(),PERM_DENYMAIL)||!HAS_PERM(getCurrentUser(),PERM_LOGINOK))
                continue;
            move(1,0);
            clrtobot();
            if(!(i=GetNameListCount())){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","������Ȩ�û�...");
                WAIT_RETURN;
                continue;
            }
            if(!(p_users=(char**)malloc(i*sizeof(char*))))
                continue;
            i=0;
            arg[0]=p_users;
            arg[1]=&i;
            ApplyToNameList(func_dump_users,arg);
            getdata(1,0,"\033[1;37m�趨Ⱥ�ű���: \033[m",buf,40,DOECHO,NULL,true);
            sprintf(genbuf,"[���ֲ� %s Ⱥ��] %s",currboard->filename,buf);
            snprintf(buf,ARTICLE_TITLE_LEN,"%s",genbuf);
            saveline(0,0,NULL);
            j=do_gsend(p_users,buf,i);
            free(p_users);
            clear();
            saveline(0,1,NULL);
            move(1,0);
            if(j){
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","����ȡ������Ⱥ�Ź����з�������...");
                WAIT_RETURN;
                continue;
            }
            prints("\033[1;32m%s\033[0;33m<Enter>\033[m","Ⱥ���ѷ���!");
            WAIT_RETURN;
        }
        else if(ans[0]=='C'){
            move(1,0);
            clrtobot();
            if(!HAS_PERM(getCurrentUser(),(PERM_OBOARDS|PERM_SYSOP))){
                prints("\033[1;33m\033[0;33m<Enter>\033[m","��ǰ�û�������������ֲ���Ȩ�б��Ȩ��...");
                WAIT_RETURN;
                continue;
            }
            sprintf(genbuf,"\033[1;37m����˵�� [\033[1;36m%s\033[1;37m]: \033[m",comment);
            getdata(1,0,genbuf,buf,64,DOECHO,NULL,true);
            if(buf[0]){
                trimstr(buf);
                snprintf(comment,128,"%s",buf);
            }
            sprintf(genbuf,"\033[1;31mȷ���������ֲ�%s��Ȩ�б� [y/N]: \033[m",(!write_perm?"��ȡ":"����"));
            getdata(2,0,genbuf,ans,2,DOECHO,NULL,true);
            ans[0]=toupper(ans[0]);
            if(ans[0]!='Y')
                continue;
            need_refresh=1;
            CreateNameList();
            func(func_remove_club_users,&write_perm);
            arg[0]=comment;
            arg[1]=&write_perm;
            ApplyToNameList(func_clear_send_mail,&arg);
            move(3,0);
            prints("\033[1;32m%s\033[0;33m<Enter>\033[m","���������!");
            WAIT_RETURN;
        }
        else{
            CreateNameList();
            break;
        }
    }
    clear();
    return FULLUPDATE;
}

/* etnlegend, 2006.04.21, ����ɾ��������� */

struct delete_range_arg{
    struct _select_item *items;
    enum delete_range_type{menu_main,menu_sub,menu_sub_safe} type;
    int fw;
    int id_from;
    int id_to;
};

static int delete_range_read(char *buf,int len,const char *valid){
#define DELETE_RANGE_READ_FORMAT    "\033[1;32;42m%c\033[m"
#define DELETE_RANGE_READ_BORDER_L  '['
#define DELETE_RANGE_READ_BORDER_R  ']'
#define KEY_SP 32
#define KEY_CR 13
#define KEY_LF 10
#define KEY_BS 8
    int i,row,col;
    getyx(&row,&col);
    prints(DELETE_RANGE_READ_FORMAT,DELETE_RANGE_READ_BORDER_L);
    for(i=0;i<len;i++)
        prints(DELETE_RANGE_READ_FORMAT,KEY_SP);
    prints(DELETE_RANGE_READ_FORMAT,DELETE_RANGE_READ_BORDER_R);
    move(row,col+1);
    i=0;
    ingetdata=1;
    while(!(i>len)){
        buf[i]=igetkey();
        switch(buf[i]){
            case KEY_CR:
            case KEY_LF:
                buf[i]=0;
                break;
            case KEY_BS:
                if(i>0){
                    move(row,col+i);
                    prints(DELETE_RANGE_READ_FORMAT,KEY_SP);
                    move(row,col+i);
                    i--;
                }
                continue;
            case KEY_ESC:
                if(!i){
                    buf[i]=0;
                    break;
                }
                move(row,col+1);
                for(i=0;i<len;i++)
                    prints(DELETE_RANGE_READ_FORMAT,KEY_SP);
                move(row,col+1);
                i=0;
                continue;
            default:
                break;
        }
        if(!buf[i])
            break;
        if(i==len||(valid&&!strchr(valid,buf[i])))
            continue;
        prints(DELETE_RANGE_READ_FORMAT,buf[i]);
        i++;
    }
    ingetdata=0;
    return i;
#undef DELETE_RANGE_READ_FORMAT
#undef DELETE_RANGE_READ_BORDER_L
#undef DELETE_RANGE_READ_BORDER_R
#undef KEY_SP
#undef KEY_CR
#undef KEY_LF
#undef KEY_BS
}

static int delete_range_select(struct _select_def *conf){
    struct delete_range_arg *arg=(struct delete_range_arg*)conf->arg;
    char buf[16];
    if(((arg->type==menu_sub)&&(conf->pos==4))||((arg->type==menu_sub_safe)&&(conf->pos==1))){
        move(arg->items[conf->pos-1].y,arg->items[conf->pos-1].x);
        clrtoeol();
        delete_range_read(buf,arg->fw,"0123456789");
        if(!buf[0])
            return SHOW_REFRESH;
        arg->id_from=atoi(buf);
        move(arg->items[conf->pos-1].y,arg->items[conf->pos-1].x+(arg->fw+3));
        prints("\033[1;37m%s \033[m","��");
        delete_range_read(buf,arg->fw,"0123456789");
        if(!buf[0])
            return SHOW_REFRESH;
        arg->id_to=atoi(buf);
    }
    return SHOW_SELECT;
}

static int delete_range_show(struct _select_def *conf,int index){
    struct _select_item *item=&(((struct delete_range_arg*)conf->arg)->items[index-1]);
    move(item->y,item->x);
    prints("\033[1;37m[\033[1;36m%c\033[1;37m] %s\033[m",item->hotkey,item->data);
    return SHOW_CONTINUE;
}

static int delete_range_key(struct _select_def *conf,int key){
    struct delete_range_arg *arg=(struct delete_range_arg*)conf->arg;
    int index;
    if(key==KEY_ESC)
        return SHOW_QUIT;
    for(index=0;index<conf->item_count;index++)
        if(toupper(key)==toupper(arg->items[index].hotkey)){
            conf->new_pos=(index+1);
            return SHOW_SELCHANGE;
        }
    return SHOW_CONTINUE;
}

static int delete_range_interface_sub_menu(int mode,int current,int total,struct delete_range_arg *arg){
    struct _select_item sel[5];
    struct _select_def conf;
    POINT pts[4];
    char menustr[4][128],buf[16];
    int safe,fw[2];
    safe=!(mode&(DELETE_RANGE_BASE_MODE_TOKEN|DELETE_RANGE_BASE_MODE_CLEAR));
    sel[0].x=32;sel[0].y=2;sel[0].hotkey='0';sel[0].type=SIT_SELECT;sel[0].data=menustr[(safe?0:3)];
    sel[1].x=32;sel[1].y=3;sel[1].hotkey='1';sel[1].type=SIT_SELECT;sel[1].data=menustr[1];
    sel[2].x=32;sel[2].y=4;sel[2].hotkey='2';sel[2].type=SIT_SELECT;sel[2].data=menustr[2];
    sel[3].x=32;sel[3].y=5;sel[3].hotkey='3';sel[3].type=SIT_SELECT;sel[3].data=menustr[(safe?3:0)];
    sel[4].x=-1;sel[4].y=-1;sel[4].hotkey=-1;sel[4].type=0;sel[4].data=NULL;
    pts[0].x=sel[0].x;pts[0].y=sel[0].y;
    pts[1].x=sel[1].x;pts[1].y=sel[1].y;
    pts[2].x=sel[2].x;pts[2].y=sel[2].y;
    pts[3].x=sel[3].x;pts[3].y=sel[3].y;
    sprintf(buf,"%d",current);
    fw[0]=strlen(buf);
    sprintf(buf,"%d",total);
    fw[1]=strlen(buf);
    sprintf(menustr[0],"%s","ָ��ɾ������");
    sprintf(menustr[1],"\033[1;37m��ǰλ����ǰ [ \033[1;31m%*d \033[1;37m- \033[1;31m%*d \033[1;37m]\033[m",
        fw[0],1,fw[1],current);
    sprintf(menustr[2],"\033[1;37m��ǰλ����� [ \033[1;31m%*d \033[1;37m- \033[1;31m%*d \033[1;37m]\033[m",
        fw[0],current,fw[1],total);
    sprintf(menustr[3],"\033[1;37mȫ��         [ \033[1;31m%*d \033[1;37m- \033[1;31m%*d \033[1;37m]\033[m",
        fw[0],1,fw[1],total);
    memset(arg,0,sizeof(struct delete_range_arg));
    arg->items=sel;
    arg->type=(safe?menu_sub_safe:menu_sub);
    arg->fw=fw[1];
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=4;
    conf.item_per_page=conf.item_count;
    conf.flag=LF_LOOP;
    conf.prompt="��";
    conf.pos=1;
    conf.item_pos=pts;
    conf.arg=arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.on_select=delete_range_select;
    conf.show_data=delete_range_show;
    conf.key_command=delete_range_key;
    if(list_select_loop(&conf)!=SHOW_SELECT)
        return -1;
    switch(conf.pos){
        case 1:
            if(!safe){
                arg->id_from=1;
                arg->id_to=total;
            }
            break;
        case 2:
            arg->id_from=1;
            arg->id_to=current;
            break;
        case 3:
            arg->id_from=current;
            arg->id_to=total;
            break;
        case 4:
            if(safe){
                arg->id_from=1;
                arg->id_to=total;
            }
            break;
        default:
            return -1;
    }
    return 0;
}

static int delete_range_interface_main_menu(void){
    struct _select_item sel[6];
    struct _select_def conf;
    struct delete_range_arg arg;
    POINT pts[5];
    int ret;
    sel[0].x=4;sel[0].y=2;sel[0].hotkey='0';sel[0].type=SIT_SELECT;sel[0].data="ɾ����ɾ����";
    sel[1].x=4;sel[1].y=3;sel[1].hotkey='1';sel[1].type=SIT_SELECT;sel[1].data="��������ɾ��";
    sel[2].x=4;sel[2].y=4;sel[2].hotkey='2';sel[2].type=SIT_SELECT;sel[2].data="ǿ������ɾ��";
    sel[3].x=4;sel[3].y=5;sel[3].hotkey='3';sel[3].type=SIT_SELECT;sel[3].data="������ɾ���";
    sel[4].x=4;sel[4].y=6;sel[4].hotkey='4';sel[4].type=SIT_SELECT;sel[4].data="�����ɾ���";
    sel[5].x=-1;sel[5].y=-1;sel[5].hotkey=-1;sel[5].type=0;sel[5].data=NULL;
    pts[0].x=sel[0].x;pts[0].y=sel[0].y;
    pts[1].x=sel[1].x;pts[1].y=sel[1].y;
    pts[2].x=sel[2].x;pts[2].y=sel[2].y;
    pts[3].x=sel[3].x;pts[3].y=sel[3].y;
    pts[4].x=sel[4].x;pts[4].y=sel[4].y;
    memset(&arg,0,sizeof(struct delete_range_arg));
    arg.items=sel;
    arg.type=menu_main;
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=5;
    conf.item_per_page=conf.item_count;
    conf.flag=LF_LOOP;
    conf.prompt="��";
    conf.pos=1;
    conf.item_pos=pts;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.on_select=delete_range_select;
    conf.show_data=delete_range_show;
    conf.key_command=delete_range_key;
    if((ret=list_select_loop(&conf))!=SHOW_SELECT)
        return -1;
    switch(conf.pos){
        case 1:
            return DELETE_RANGE_BASE_MODE_TOKEN;
        case 2:
            return DELETE_RANGE_BASE_MODE_RANGE;
        case 3:
            return DELETE_RANGE_BASE_MODE_FORCE;
        case 4:
            return DELETE_RANGE_BASE_MODE_MPDEL;
        case 5:
            return DELETE_RANGE_BASE_MODE_CLEAR;
        default:
            return -1;
    }
}

int delete_range(struct _select_def *conf,struct fileheader *file,void *varg){
#define DELETE_RANGE_ALLOWED_INTERVAL   20
#define DELETE_RANGE_QUIT(n,s)          do{\
                                            move((n),4);\
                                            prints("\033[1;33m%s\033[0;33m<Enter>\033[m",(s));\
                                            if(uinfo.mode==MAIL)\
                                                modify_user_mode(RMAIL);\
                                            WAIT_RETURN;\
                                            return FULLUPDATE;\
                                        }while(0)
    sigset_t mask_set,old_mask_set;
    struct stat st_src;
    struct read_arg *rarg;
    struct delete_range_arg arg;
    char buf[256],ans[4];
    const char *ident,*src,*dst,*type;
    int mail,current,total,mode,ret,line;
    time_t timestamp;
    rarg=(struct read_arg*)conf->arg;
    total=rarg->filecount;
    current=((conf->pos>total)?total:(conf->pos));
    line=8;
    switch(rarg->mode){
        case DIR_MODE_NORMAL:
            mail=0;
            ident=currboard->filename;
            src=".DIR";
            dst=".DELETED";
            mode=DELETE_RANGE_BASE_MODE_CHECK;
            break;
        case DIR_MODE_DIGEST:
            mail=0;
            ident=currboard->filename;
            src=".DIGEST";
            dst=NULL;
            mode=DELETE_RANGE_BASE_MODE_CHECK;
            break;
        case DIR_MODE_MAIL:
            mail=1;
            ident=getCurrentUser()->userid;
            if(!(src=strrchr(rarg->direct,'.')))
                return DONOTHING;
            dst=(!strcmp(src,".DELETED")?NULL:".DELETED");
            mode=DELETE_RANGE_BASE_MODE_MAIL;
            break;
        default:
            return DONOTHING;
    }
    if(!mail&&deny_del_article(currboard,NULL,getSession()))
        return DONOTHING;
    timestamp=time(NULL);
    !mail?setbfile(buf,ident,src):setmailfile(buf,ident,src);
    if(stat(buf,&st_src)==-1||!S_ISREG(st_src.st_mode))
        return DONOTHING;
    clear();
    move(0,0);
    prints("\033[1;32m%s \033[1;33m%s\033[m","[����ɾ��ѡ��]","<Enter>��ѡ��/<Esc>���˳�");
    if(uinfo.mode==RMAIL)
        modify_user_mode(MAIL);
    if((ret=delete_range_interface_main_menu())==-1)
        DELETE_RANGE_QUIT(line,"����ȡ��...");
    mode|=ret;
    switch(mode&DELETE_RANGE_BASE_MODE_OPMASK){
        case DELETE_RANGE_BASE_MODE_TOKEN:
            type="ɾ����ɾ����";
            break;
        case DELETE_RANGE_BASE_MODE_RANGE:
            type="��������ɾ��";
            break;
        case DELETE_RANGE_BASE_MODE_FORCE:
            type="ǿ������ɾ��";
            break;
        case DELETE_RANGE_BASE_MODE_MPDEL:
            type="������ɾ���";
            break;
        case DELETE_RANGE_BASE_MODE_CLEAR:
            type="�����ɾ���";
            break;
        default:
            DELETE_RANGE_QUIT(line,"����δ֪����, ����ȡ��...");
    }
    if(delete_range_interface_sub_menu(mode,current,total,&arg)==-1)
        DELETE_RANGE_QUIT(line,"����ȡ��...");
    if(uinfo.mode==MAIL)
        modify_user_mode(RMAIL);
    move(line++,4);
    sprintf(buf,"\033[1;33m%s \033[1;32m%d \033[1;37m- \033[1;32m%d \033[1;37mȷ�ϲ���? [y/N] \033[m",
        type,arg.id_from,arg.id_to);
    prints("%s",buf);
    delete_range_read(ans,1,"ynYN");
    switch(ans[0]){
        case 'Y':
            mode|=DELETE_RANGE_BASE_MODE_OVERM;
        case 'y':
            break;
        default:
            DELETE_RANGE_QUIT(++line,"����ȡ��...");
    }
    move(++line,4);
    prints("\033[1;33m%s\033[m","���β���������Ҫ�ϳ�ʱ��, �����ĵȺ�...");
    refresh();
    sigemptyset(&mask_set);
    sigaddset(&mask_set,SIGHUP);
    sigaddset(&mask_set,SIGBUS);
    sigaddset(&mask_set,SIGPIPE);
    sigaddset(&mask_set,SIGTERM);
    sigprocmask(SIG_SETMASK,NULL,&old_mask_set);
    sigprocmask(SIG_BLOCK,&mask_set,NULL);
    ret=delete_range_base(ident,src,dst,arg.id_from,arg.id_to,mode,NULL,&st_src);
    if(!mail)
        newbbslog(BBSLOG_USER,"delete_range %s %d - %d <%d,%#04x>",ident,arg.id_from,arg.id_to,mode,ret);
    sigprocmask(SIG_SETMASK,&old_mask_set,NULL);
    if(ret==0x21){
        move(line++,4);
        clrtoeol();
        prints("%s","\033[1;37mϵͳ��⵽���������ڼ���������б��Ѿ������仯, \033[m");
        move(line++,4);
        sprintf(buf,"%s","\033[1;33mǿ�Ʋ���\033[1;37m[\033[1;31m���ز�����\033[1;37m]? [y/N] \033[m");
        prints("%s",buf);
        delete_range_read(ans,1,"ynYN");
        if(toupper(ans[0])!='Y')
            DELETE_RANGE_QUIT(++line,"����ȡ��...");
        if((time(NULL)-timestamp)<DELETE_RANGE_ALLOWED_INTERVAL){
            mode&=~DELETE_RANGE_BASE_MODE_CHECK;
            sigprocmask(SIG_BLOCK,&mask_set,NULL);
            ret=delete_range_base(ident,src,dst,arg.id_from,arg.id_to,mode,NULL,NULL);
            if(!mail)
                newbbslog(BBSLOG_USER,"delete_range %s %d - %d <%d,%#04x>",ident,arg.id_from,arg.id_to,mode,ret);
            sigprocmask(SIG_SETMASK,&old_mask_set,NULL);
        }
        else{
            move(++line,4);
            sprintf(buf,"\033[1;33mǿ�Ʋ���ʱ��Ϊ \033[1;31m%d \033[1;33m��, "
                "���˴β����Ѿ���ʱ, ����ȡ��...\033[0;33m<Enter>\033[m",DELETE_RANGE_ALLOWED_INTERVAL);
            prints("%s",buf);
            WAIT_RETURN;
            return FULLUPDATE;
        }
    }
    if(!mail)
        bmlog(getCurrentUser()->userid,ident,5,1);
    move(line++,4);
    clrtoeol();
    if(!ret){
        if(!mail){
            if((mode&DELETE_RANGE_BASE_MODE_FORCE)
                ||((mode&DELETE_RANGE_BASE_MODE_OVERM)&&(mode&DELETE_RANGE_BASE_MODE_TOKEN)))
                setboardmark(ident,1);
            setboardorigin(ident,1);
            updatelastpost(ident);
        }
        prints("\033[1;32m%s\033[0;33m<Enter>\033[m","�������!");
        WAIT_RETURN;
        return DIRCHANGED;
    }
    sprintf(buf,"\033[1;33m%s\033[1;31m<%#04x>\033[0;33m<Enter>\033[m","�����з�������!",ret);
    prints("%s",buf);
    WAIT_RETURN;
    return FULLUPDATE;
#undef DELETE_RANGE_ALLOWED_INTERVAL
#undef DELETE_RANGE_QUIT
}

