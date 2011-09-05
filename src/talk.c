/*    Pirate Bulletin Board System
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

#include "bbs.h"
#include "read.h"
#ifdef lint
#include <sys/uio.h>
#endif                          /* 
                                 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define M_INT 8                 /* monitor mode update interval */
#define P_INT 20                /* interval to check for page req. in talk/chat */
extern int numf, friendmode;
int talkidletime = 0;
int ulistpage;
int badlist(void);
static int do_talk(int fd);

/* Bigman 2000.9.15 Talk�ļ�¼ */
#ifdef TALK_LOG
void do_log();
int talkrec = -1;
char partner[IDLEN + 1];
#endif
                                 

#include "lilacfuncs.h"

#ifdef LILAC
char st[50][MAXBOARD];
static int numboards = 0;
char userid[IDLEN+2];

int findboard(const struct boardheader *fptr,int idx,void* arg)
{
	char *p1, *p2;
    if (numboards >= MAXBOARD)
        return 0;
    if(!check_see_perm(NULL,fptr)||!*(fptr->filename))
        return 0;
    if (fptr->flag & BOARD_GROUP)
        return 0;
	if((p1 = strstr(fptr->BM,userid))==NULL)
		return 0;
	//��Ҫ��ȷƥ��
	p2 = p1 + strlen(userid);
	if(*p2!=' ' && *p2!='\0')
		return 0;
	//userid�ڿ�ͷ
	if(p1!=fptr->BM && *(p1-1)!=' ')
		return 0;

	strcpy(st[numboards], fptr->filename);
//    strcpy(st[numboards], fptr->title + 13);
    numboards++;
    return 0;
}

int fillboard(void){
    return apply_record(BOARDS, (APPLY_FUNC_ARG)findboard, sizeof(struct boardheader), NULL, 0,false);
}

#endif

struct talk_win {

    int curcol, curln;

    int sline, eline;
};
extern int t_columns;
char *talk_uent_buf;

/* begin - jjyang */
char save_page_requestor[STRLEN];

/* end - jjyang */

/*---	changed to isidhidden by period	2000-10-20	---*
int
ishidden(user)
char *user;
{
    int tuid;
    struct user_info uin;
    if (!(tuid = getuser(user))) return 0;
    search_ulist( &uin, t_cmpuids, tuid );
    return( uin.invisible );
}
---*/
char canpage(friend, pager)
int friend, pager;
{
    if ((pager & ALL_PAGER) || HAS_PERM(getCurrentUser(), PERM_SYSOP))
        return true;
    if ((pager & FRIEND_PAGER)) {
        if (friend)
            return true;
    }
    return false;
}
int listcuent(struct user_info *uentp, char *arg, int pos)
{
    if (uentp == NULL) {
        CreateNameList();
        return 0;
    }
    /*
     * if(uentp->uid == usernum)             rem by Haohmaru,00.5.26,�������ܸ��Լ���msg
     * return 0;
     */
    if (!uentp->active || !uentp->pid)
        return 0;
    if (uentp->mode == ULDL)
        return 0;
    if (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible)
        return 0;
    AddNameList(uentp->userid);
    return 0;
}

void creat_list()
{
    listcuent(NULL, 0, 0);
    apply_ulist_addr((APPLY_UTMP_FUNC) listcuent, 0);
}

int t_pager(void){
    if (uinfo.pager & ALL_PAGER) {
        uinfo.pager &= ~ALL_PAGER;
        if (DEFINE(getCurrentUser(), DEF_FRIENDCALL))
            uinfo.pager |= FRIEND_PAGER;
        else
            uinfo.pager &= ~FRIEND_PAGER;
    } else {
        uinfo.pager |= ALL_PAGER;
        uinfo.pager |= FRIEND_PAGER;
    }
    if (!uinfo.in_chat && uinfo.mode != TALK) {
        move(1, 0);
        prints("���ĺ����� (pager) �Ѿ�\033[1m%s\033[m��!", (uinfo.pager & ALL_PAGER) ? "��" : "�ر�");
        pressreturn();
    }
    UPDATE_UTMP(pager, uinfo);
    return 0;
}

/*Add by SmallPig*/
/*�˺���ֻ������ӡ˵�����������������λ�����⡣*/
extern int minln; //added by bad

int show_user_plan(userid)
char userid[IDLEN];
{
    int i;
    char pfile[STRLEN], pbuf[256];
    FILE *pf;

    sethomefile(pfile, userid, "plans");
    if ((pf = fopen(pfile, "r")) == NULL) {
#ifndef LILAC
        prints("\033[36mû�и���˵����\033[m\n");
#endif
/*fclose(pf); *//*
 * * * Leeward 98.04.20 
 */
        return false;
    } else {
#ifndef LILAC
        prints("\033[36m����˵�������£�\033[m\n");//added by bad
#endif
        getyx(&minln, &i);
        for (i = 1; i <= scr_lns-8; i++) {
            if (fgets(pbuf, sizeof(pbuf), pf))
                prints("%s", pbuf);
            else
                break;
        }
        fclose(pf);
        minln=0;
        return true;
    }
}
int t_printstatus(struct user_info *uentp, int *arg, int pos)
{
    char buf[80],buf2[20],buf3[80];
    char* p;
    if (uentp->invisible == 1) {
        if (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK))
            return COUNT;
    }
    (*arg)++;
    if (*arg == 1)
#ifndef LILAC
        strcpy(genbuf, "Ŀǰ��վ�ϣ�״̬���£�\n");
#else
        strcpy(genbuf, "Ŀǰ��վ�ϣ�״̬���£�");
#endif

    p = idle_str(buf3,uentp);
    if(p[0]==' '&&p[1]==' ') buf2[0]=0;
    else sprintf(buf2, "[%s]", p);
    sprintf(buf, "%s\033[1m%s\033[m%s ", 
			uentp->invisible?(uentp->pid==1?"\033[33m":"\033[32m"):(uentp->pid==1?"\033[36m":""), 
			modestring(buf3,uentp->mode, uentp->destuid, 0,   /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                          (uentp->in_chat ? uentp->chatid : NULL)), buf2);
    strcat(genbuf, buf);

    if ((*arg) % 8 == 0)
        strcat(genbuf, "\n");
    UNUSED_ARG(pos);
    return COUNT;
}

/* Modified By Excellent*/
struct _tag_talk_showstatus {
    int count;
    int pos[20];
};
int talk_showstatus(struct user_info *uentp, struct _tag_talk_showstatus *arg, int pos)
{
    char buf[80],buf2[80];
	struct userec *lookupuser;

    if (uentp->invisible && !HAS_PERM(getCurrentUser(), PERM_SEECLOAK))
        return 0;
    if( getuser(uentp->userid, &lookupuser) == 0 ){
		return 0;
    }                                  
    arg->pos[arg->count++] = pos;

    sprintf(buf, "(%d) Ŀǰ״̬: %s, ����: %s \n", arg->count, modestring(buf2,uentp->mode, uentp->destuid, 0,       /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                                                          uentp->in_chat ? uentp->chatid : NULL), 
			SHOW_USERIP(lookupuser, uentp->from));
    strcat(genbuf, buf);
    return COUNT;
}

int t_cmpuids(int uid, struct user_info *up)
{
    return (up->active && uid == up->uid);
}

#ifdef FREE
static char   * horoscope(month, day)
unsigned char    month, day;
{
	char   *name[12] = {
		"Ħ��", "ˮƿ", "˫��", "ĵ��", "��ţ", "˫��",
		"��з", "ʨ��", "��Ů", "���", "��Ы", "����"
	};
	switch (month) {
	case 1:
		if (day < 21)
			return (name[0]);
		else
			return (name[1]);
	case 2:
		if (day < 19)
			return (name[1]);
		else
			return (name[2]);
	case 3:
		if (day < 21)
			return (name[2]);
		else
			return (name[3]);
	case 4:
		if (day < 21)
			return (name[3]);
		else
			return (name[4]);
	case 5:
		if (day < 21)
			return (name[4]);
		else
			return (name[5]);
	case 6:
		if (day < 22)
			return (name[5]);
		else
			return (name[6]);
	case 7:
		if (day < 23)
			return (name[6]);
		else
			return (name[7]);
	case 8:
		if (day < 23)
			return (name[7]);
		else
			return (name[8]);
	case 9:
		if (day < 23)
			return (name[8]);
		else
			return (name[9]);
	case 10:
		if (day < 24)
			return (name[9]);
		else
			return (name[10]);
	case 11:
		if (day < 23)
			return (name[10]);
		else
			return (name[11]);
	case 12:
		if (day < 22)
			return (name[11]);
		else
			return (name[0]);
	}
	return ("����");
}
#endif

int t_query(char* q_id)
{
    char uident[STRLEN], *newline;
    int tuid = 0;
    int exp, perf;              /*Add by SmallPig */
    char qry_mail_dir[STRLEN];
    char planid[IDLEN + 2];
    char permstr[USER_TITLE_LEN];
    char exittime[40];
    time_t exit_time, temp /*Haohmaru.98.12.04 */ ;
    int logincount, seecount, oldmode;
    struct userec *lookupuser;

    if (q_id==NULL) {
        /*
         * count = shortulist(NULL); 
         */
        oldmode = uinfo.mode;
        modify_user_mode(QUERY);
        move(2, 0);
        clrtobot();
        prints("<����ʹ���ߴ���, ���հ׼���Tab�����г������ִ�>\n");
        move(1, 0);
        clrtoeol();
        prints("��ѯ˭: ");
        usercomplete(NULL, uident);
        if (uident[0] == '\0') {
            clear();
            modify_user_mode(oldmode);
            return 0;
        }
        modify_user_mode(oldmode);
    } else {
        char *p;

        strcpy(uident, q_id);
        for (p = uident; *p; p++)
            if (*p == ' ') {
                *p = 0;
                break;
            };
    }
    oldmode = uinfo.mode;
    modify_user_mode(QUERY);
    if (!(tuid = getuser(uident, &lookupuser))) {
        move(2, 0);
        clrtoeol();
        prints("\033[1m����ȷ��ʹ���ߴ���\033[m\n");
        pressanykey();
        move(2, 0);
        clrtoeol();
        modify_user_mode(oldmode);
        return -1;
    }
    uinfo.destuid = tuid;
/*    UPDATE_UTMP(destuid,uinfo);  I think it is not very importance.KCN*/
/*    search_ulist( &uin, t_cmpuids, tuid );*/
#if defined(FREE) || defined(ZIXIA)
	move(0, 0);
#else
    move(1, 0);
#endif
    clrtobot();
    setmailfile(qry_mail_dir, lookupuser->userid, DOT_DIR);
    exp = countexp(lookupuser);
    perf = countperf(lookupuser);

#ifdef FREE
	{
		char horobuf[50];
		char buf1[256];
		int clr;
		struct userdata udata;

		if( ! DEFINE(lookupuser, DEF_SHOWDETAILUSERDATA) || read_userdata(uident,&udata) ){
			clr=2;
			strcpy(horobuf," ");
		}else{
			clr = (udata.gender == 'F') ? 5 : 6 ;
			sprintf(horobuf, "[\033[1;3%dm%s\033[m]", clr, horoscope(udata.birthmonth, udata.birthday) );
		}

		sprintf(buf1,"\033[1;37m%s \033[m(\033[1;33m%s\033[m) ����վ \033[1;32m%d\033[m ��  %s", lookupuser->userid, lookupuser->username, lookupuser->numlogins, horobuf);

		prints(buf1);

	}
#else
        prints("%s (%s) ����վ %d �Σ������ %d ƪ����", lookupuser->userid, lookupuser->username, lookupuser->numlogins, lookupuser->numposts);
#endif
    strcpy(planid, lookupuser->userid);
    if ((newline = strchr(genbuf, '\n')) != NULL)
        *newline = '\0';
    seecount = 0;
    logincount = apply_utmp((APPLY_UTMP_FUNC) t_printstatus, 10, lookupuser->userid, &seecount);
    /*
     * �������ʱ�� Luzi 1998/10/23 
     */
    exit_time = get_exit_time(lookupuser, exittime);
    if ((newline = strchr(exittime, '\n')) != NULL)
        *newline = '\0';
    if (exit_time <= lookupuser->lastlogin) {
        if (logincount != seecount) {
            temp = lookupuser->lastlogin + ((lookupuser->numlogins + lookupuser->numposts) % 100) + 60;
            strcpy(exittime, ctime(&temp));     /*Haohmaru.98.12.04.�������û�����ȥ����ʱ�������ʱ����60��160���� */
            if ((newline = strchr(exittime, '\n')) != NULL)
                *newline = '\0';
        } else
#ifdef FREE
            strcpy(exittime, "       ��������       ");
#else
            strcpy(exittime, "�����߻���߲���");
#endif
    }
#if defined(FREE)
	prints("\n�� �� ��: [\033[1;32m%s\033[m] �� [\033[1;32m%s\033[m] ����վһ�Ρ�\n", Ctime(lookupuser->lastlogin), ((lookupuser->lasthost[0] == '\0') /*|| DEFINE(getCurrentUser(),DEF_HIDEIP) */ ? "(����)" : ( (!strcmp(lookupuser->userid , getCurrentUser()->userid) || HAS_PERM(getCurrentUser(), PERM_SYSOP) ) ? lookupuser->lasthost: SHOW_USERIP(lookupuser, lookupuser->lasthost)) ) );
	prints("��վʱ��: [\033[1;32m%s\033[m] ", exittime);
#else
    prints("\n�ϴ���  [%s] �� [%s] ����վһ�Ρ�\n����ʱ��[%s] ", Ctime(lookupuser->lastlogin), ((lookupuser->lasthost[0] == '\0') /*|| DEFINE(getCurrentUser(),DEF_HIDEIP) */ ? "(����)" : ( (!strcmp(lookupuser->userid , getCurrentUser()->userid) || HAS_PERM(getCurrentUser(), PERM_SYSOP) ) ? lookupuser->lasthost: SHOW_USERIP(lookupuser, lookupuser->lasthost)) ),    /*Haohmaru.99.12.18. hide ip */
           exittime);
#endif

#if defined(FREE)
	prints("����ֵ��[\033[1;32m%d\033[m](\033[1;33m%s\033[m) ���䣺[\033[1;5;32m%2s\033[m]\n"
	      , exp,c_exp(exp), (check_query_mail(qry_mail_dir, NULL)) ? "��" : "  ");
	prints("������: [\033[1;32m%d\033[m] ���д��: [\033[1;32m%dԪ\033[m] ������: [\033[1;32m%d\033[m] ������: [\033[1;32m%d\033[m]\n",
	      lookupuser->numposts,lookupuser->money, lookupuser->score,
		  compute_user_value(lookupuser) );
#elif defined(ZIXIA)
    uleveltochar(permstr, lookupuser);
    prints(" ���䣺[\033[5m%2s\033[m]  ��������[%d] \n",
           (check_query_mail(qry_mail_dir, NULL)) ? "��" : "  ", compute_user_value(lookupuser));
    prints("��������[\033[1;32m%d��\033[m]    ���: [\033[1;32m%s\033[m]%s\n",
		//(lookupuser->userlevel & (PERM_OBOARDS | PERM_SYSOP | PERM_ADMIN))? 9999 : lookupuser->altar, 
		lookupuser->altar,permstr, (lookupuser->userlevel & PERM_SUICIDE) ? " (��ɱ��)" : "��");
#else
    uleveltochar(permstr, lookupuser);
    prints("���䣺[\033[5m%2s\033[m] ��������[%d] ���: [\033[1;32m%s\033[m]%s\n",
           (check_query_mail(qry_mail_dir, NULL)) ? "��" : "  ", compute_user_value(lookupuser), permstr, (lookupuser->userlevel & PERM_SUICIDE) ? " (��ɱ��)" : "��");
#endif

#ifdef HOROSCOPE
	char horobuf[50];
	int clr;
	struct userdata udata;
	if( ! DEFINE(lookupuser, DEF_SHOWDETAILUSERDATA) || read_userdata(uident,&udata) ){
		strcpy(horobuf," \n");
	}else{
		clr = (udata.gender == 'F') ? 5 : 6 ;
		sprintf(horobuf, "������[\033[1;3%dm%s��\033[m]", clr, horoscope(udata.birthmonth, udata.birthday) );
		//sprintf(horobuf, "�Ա�[\033[1;3%dm%s\033[m]", clr, (udata.gender == 'F')?:"Ů":"��" );
		prints("%s\t",horobuf);
	}
#endif

#ifdef LILAC
	prints("����ֵ: [%d](%s)\t",exp,c_exp(exp));
	if(strcmp(lookupuser->userid, getCurrentUser()->userid)==0 
			|| HAS_PERM(getCurrentUser(), (PERM_OBOARDS | PERM_SYSOP | PERM_ADMIN))) 
	{
		char genbuf[20];
		sprintf(genbuf,"%d  %.3lf%%", lookupuser->score, (float)lookupuser->score_rank/1000);
		prints("�û�����: [\033[1;33m%s\033[m]", 
			 genbuf);
	}
	prints("\n");
	int i;
	numboards = 0;
	if(HAS_PERM(lookupuser, PERM_BOARDS)){
		strcpy(userid,lookupuser->userid);
    	resolve_boards();
    	fillboard();
		if(numboards>0){
			prints("[");
			for(i=0;i<numboards;i++)
				prints("\033[1;32m%s\033[m ",st[i]);
			prints(" \033[1;33m�����\033[m]\n");
		}
	}

#endif

    if ((genbuf[0]) && seecount) {
        prints(genbuf);
        prints("\n");
    }
    show_user_plan(planid);
    if (oldmode != LUSERS && oldmode != LAUSERS && oldmode != FRIEND && oldmode != GMENU) {
        int ch;
        char buf[STRLEN];
        char *t1, *t2, *t3;
        struct user_info *uin;
        move(t_lines - 1, 0);
        if ((genbuf[0]) && seecount) {
            t1 = "����[\x1b[1;32mt\x1b[m\x1b[0;44m]";
            t2 = "��ѶϢ[\x1b[1;32ms\x1b[m\x1b[0;44m]";
        }
        else {
            t1 = "       ";
            t2 = "         ";
        }
#ifdef PERSONAL_CORP
        if (lookupuser->flags & PCORP_FLAG) {
            t3 = "BLOG[\x1b[1;32mx\x1b[m\x1b[0;44m]";
        }
        else
#endif
        {
            t3 = "       ";
        }
        prints("\x1b[m\x1b[44m%s ����[\x1b[1;32mm\x1b[m\x1b[0;44m] %s ��,������[\x1b[1;32mo,d\x1b[m\x1b[0;44m] �鿴˵����[\x1b[1;32ml\x1b[m\x1b[0;44m] %s ����������", t1, t2, t3);

        clrtoeol();
        resetcolor();
        ch = igetkey();
        switch(toupper(ch)) {
#ifdef PERSONAL_CORP
            case 'X':
                if (lookupuser->flags & PCORP_FLAG) {
                    modify_user_mode(PC);
                    pc_read(lookupuser->userid);
                }
                break;
#endif
            case 'T':
                if (!((genbuf[0]) && seecount)) break;
                if (strcmp(uident, "guest") && !HAS_PERM(getCurrentUser(), PERM_PAGE))
                    break;
                uin = t_search(uident, false);
                if(uin==NULL) break;
                ttt_talk(uin);
                break;
            case 'S':
                if (!((genbuf[0]) && seecount)) break;
                if (strcmp(uident, "guest") && !HAS_PERM(getCurrentUser(), PERM_PAGE))
	                break;
                uin = t_search(uident, false);
                if (!uin || !canmsg(getCurrentUser(), uin))
                    break;
                strcpy(getSession()->MsgDesUid, uident);
                do_sendmsg(uin, NULL, 0);
                break;
            case 'M':
                if (HAS_PERM(getCurrentUser(), PERM_DENYMAIL)
                	||!HAS_PERM(getCurrentUser(), PERM_LOGINOK))
                    break;
                m_send(uident);
                break;
            case 'O':
                if (!strcmp("guest", getCurrentUser()->userid))
                    break;
                clear();
                addtooverride(uident);
                break;
            case 'L':
                do{
                    struct stat st;
                    sethomefile(buf,planid,"plans");
                    if(!stat(buf,&st)&&S_ISREG(st.st_mode)&&st.st_size){
                        ansimore(buf,true);
                        move(0,0);
                    }
                }
                while(0);
                break;
            case 'D':
                if (!strcmp("guest", getCurrentUser()->userid))
                    break;
                sprintf(buf, "��Ҫ�� %s �����������Ƴ��� (Y/N) [N]: ", uident);
                move(BBS_PAGESIZE + 3, 0);
                clrtoeol();
                getdata(BBS_PAGESIZE + 3, 0, buf, genbuf, 4, DOECHO, NULL, true);
                move(BBS_PAGESIZE + 3, 0);
                clrtoeol();
                if (genbuf[0] != 'Y' && genbuf[0] != 'y') break;
                if (deleteoverride(uident) == -1)
                    sprintf(buf, "%s �����Ͳ�������������", uident);
                else
                    sprintf(buf, "%s �Ѵ����������Ƴ�", uident);
                move(BBS_PAGESIZE + 3, 0);
                clrtoeol();
                prints("%s", buf);
                refresh();
                sleep(1);
                break;
        }
    }
    uinfo.destuid = 0;
    modify_user_mode(oldmode);
    return 0;
}
int count_visible_active(struct user_info *uentp, int *count, int pos)
{
    if (!uentp->active || !uentp->pid)
        return 0;
    if (!(!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible))
        (*count)++;
    return 1;
}
int alcounter(struct user_info *uentp, char *arg, int pos)
{
    int canseecloak;

    if (!uentp->active || !uentp->pid)
        return 0;
    canseecloak = (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible) ? 0 : 1;
    if (myfriend(uentp->uid, NULL,getSession())) {
        count_friends++;
        if (!canseecloak)
            count_friends--;
    }
    count_users++;
    if (!canseecloak)
        count_users--;
    return 1;
}

int num_alcounter()
{
    count_friends = 0;
    count_users = 0;
    apply_ulist_addr((APPLY_UTMP_FUNC) alcounter, 0);
    return count_users;
}
int num_user_logins(char *uid)
{
    strcpy(save_page_requestor, uid);
    return apply_utmp(NULL, 0, uid, 0);
}

int num_visible_users()
{
    int count;

    count = 0;
    apply_ulist_addr((APPLY_UTMP_FUNC) count_visible_active, (char *) &count);
    return count;
}
int ttt_talk(struct user_info *userinfo)
{
    char uident[STRLEN];
    char test[STRLEN];
    int tuid, ucount, unum, tmp;
    struct user_info uin;
    struct _tag_talk_showstatus ts;

    move(1, 0);
    clrtobot();
    if (!HAS_PERM(getCurrentUser(),PERM_PAGE))
        return 0;
    if (userinfo == NULL) {
        move(2, 0);
        prints("<����ʹ���ߴ���>\n");
        move(1, 0);
        clrtoeol();
        prints("��˭����: ");
        creat_list();
        namecomplete(NULL, uident);
        if (uident[0] == '\0') {
            clear();
            return 0;
        }
        if (!(tuid = searchuser(uident)) || tuid == getSession()->currentuid) {
            move(2, 0);
            prints("�������\n");
            pressreturn();
            move(2, 0);
            clrtoeol();
            return -1;
        }
        genbuf[0] = 0;
        ts.count = 0;
        ucount = apply_utmp((APPLY_UTMP_FUNC) talk_showstatus, 20, uident, &ts);
        move(3, 0);
        prints("Ŀǰ %s �� %d logins ����: \n", uident, ucount);
        clrtobot();
        if (ucount > 1) {
            char buf[6];

          list:move(5, 0);
            prints("(0) �������ˣ������ˡ�\n");
            prints(genbuf);
            clrtobot();
            tmp = ucount + 8;
            getdata(tmp, 0, "��ѡһ���㿴�ıȽ�˳�۵� [0]: ", buf, 4, DOECHO, NULL, true);
            unum = atoi(buf);
            if (unum == 0) {
                clear();
                return 0;
            }
            if (unum > ucount || unum < 0) {
                move(tmp, 0);
                prints("��������ѡ��������\n");
                clrtobot();
                pressreturn();
                goto list;
            }
            uin = utmpshm->uinfo[ts.pos[unum - 1]];
        } else
            search_ulist(&uin, t_cmpuids, tuid);
    } else {
        /*
         * memcpy(&uin,userinfo,sizeof(uin)); 
         */
        uin = *userinfo;
        tuid = uin.uid;
        strcpy(uident, uin.userid);
        move(1, 0);
        clrtoeol();
        prints("��˭����: %s", uin.userid);
    }
    /*
     * check if pager on/off       --gtv 
     */
    if (!canpage(hisfriend(getSession()->currentuid,&uin), uin.pager)) {
        move(2, 0);
        prints("�Է��������ѹر�.\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    /*
     * modified by Excellent 
     */
    if (uin.mode == ULDL || uin.mode == IRCCHAT || uin.mode == BBSNET || uin.mode == FOURM || uin.mode == EXCE_BIG2 || uin.mode == EXCE_MJ || uin.mode == EXCE_CHESS || uin.mode == TETRIS || uin.mode==WINMINE) {
        move(2, 0);
        prints("Ŀǰ�޷�����.\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    if (LOCKSCREEN == uin.mode) {       /* Leeward 98.02.28 */
        move(2, 0);
        prints("�Է��Ѿ�������Ļ�����Ժ��ٺ�����(��)����...\n");
        clrtobot();
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    if (!uin.active || (kill(uin.pid, 0) == -1)) {
        move(2, 0);
        prints("�Է����뿪\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    if (false == canIsend2(getCurrentUser(),uin.userid)) {       /*Haohmaru.99.6.6.����Ƿ�ignore */
        move(2, 0);
        prints("�Է��ܾ���������\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    } else {
        int sock, msgsock;
        int length;
        struct sockaddr_in server;
        char c;
        char buf[512];

        move(3, 0);
        clrtobot();
        show_user_plan(uident);
        getdata(2, 0, "ȷ��Ҫ����/��������? (Y/N) [N]: ", genbuf, 4, DOECHO, NULL, true);
        if (*genbuf != 'y' && *genbuf != 'Y') {
            clear();
            return 0;
        }
        sprintf(buf, "Talk to '%s'", uident);
        bbslog("user","%s",buf);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket err\n");
            return -1;
        }
		bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
/*        server.sin_addr.s_addr = INADDR_ANY ;
����Ӧ����INADDR_LOOPBACK�ȽϺ� KCN
*/
        server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        server.sin_port = 0;
        if (bind(sock, (struct sockaddr *) &server, sizeof server) < 0) {
            perror("bind err");
            return -1;
        }
        length = sizeof server;
        if (getsockname(sock, (struct sockaddr *) &server, (socklen_t *) & length) < 0) {
            perror("socket name err");
            return -1;
        }
        uinfo.sockactive = true;
        uinfo.sockaddr = server.sin_port;
        uinfo.destuid = tuid;
        UPDATE_UTMP(sockactive, uinfo);
        UPDATE_UTMP(sockaddr, uinfo);
        UPDATE_UTMP(destuid, uinfo);
        modify_user_mode(PAGE);
        kill(uin.pid, SIGUSR1);
        clear();
        prints("���� %s ��...\n\n���� Ctrl-D ����\n", uident);  /* modified by dong , 1999.1.27 */
        listen(sock, 1);
        add_io(sock, 20);
        while (true) {
            int ch;

            ch = igetkey();
            if (ch == I_TIMEOUT) {
                move(0, 0);
                prints("�ٴκ���.\n");
                bell();
                if (kill(uin.pid, SIGUSR1) == -1) {
                    move(0, 0);
                    prints("�Է�������\n");
                    pressreturn();
                    /*
                     * Add by SmallPig 2 lines 
                     */
                    uinfo.sockactive = false;
                    uinfo.destuid = 0;
                    return -1;
                }
                continue;
            }
            if (ch == I_OTHERDATA)
                break;
            if (ch == '\004') {
                add_io(0, 0);
                close(sock);
                uinfo.sockactive = false;
                uinfo.destuid = 0;
                UPDATE_UTMP(sockactive, uinfo);
                UPDATE_UTMP(destuid, uinfo);
                clear();
                return 0;
            }
        }
        msgsock = accept(sock, (struct sockaddr *) 0, (socklen_t *) 0);
        if (msgsock == -1) {
            perror("accept");
            return -1;
        }
        add_io(0, 0);
        close(sock);
        uinfo.sockactive = false;
        uinfo.destuid = 0;
        UPDATE_UTMP(sockactive, uinfo);
        UPDATE_UTMP(destuid, uinfo);
        /*
         * uinfo.destuid = 0 ; 
         */
        read(msgsock, &c, sizeof c);
        clear();
        if (c == 'y' || c == 'Y') {
            sprintf(save_page_requestor, "%s (%s)", uin.userid, uin.username);
            /*
             * Bigman 2000.9.15 ����Talk��¼ 
             */
#ifdef TALK_LOG
            strcpy(partner, uin.userid);
#endif                          /* 
                                 */
            do_talk(msgsock);
            /*
             * Add by SmallPig 
             */
        } else if (c == 'n' || c == 'N') {
            prints("%s (%s)˵����Ǹ�������ں�æ�����ܸ����ġ�\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'b' || c == 'B') {
            prints("%s (%s)˵�������ںܷ���������������졣\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'c' || c == 'C') {
            prints("%s (%s)˵�����м��£��ҵ�һ���� Call �㡣\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'd' || c == 'D') {
            prints("%s (%s)˵�����㲻Ҫ�� Page���Ҳ�������ġ�\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'e' || c == 'E') {
            prints("%s (%s)˵����Ҫ�뿪�ˣ��´����İɡ�\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'F' || c == 'f') {
            prints("%s (%s)˵�������һ���Ÿ��ң�������û�ա�\n", uin.userid, uin.username);
            pressreturn();
        } else if (c == 'M' || c == 'm') {
            read(msgsock, test, sizeof test);
            prints("%s (%s)˵��%s\n", uin.userid, uin.username, test);
            pressreturn();
        } else {
            sprintf(save_page_requestor, "%s (%s)", uin.userid, uin.username);
            /*
             * Bigman 2000.9.15 ����Talk��¼ 
             */
#ifdef TALK_LOG
            strcpy(partner, uin.userid);
#endif                          /* 
                                 */
            do_talk(msgsock);
        }
	if (talkrequest)
	    talkreply();
        close(msgsock);
        clear();
    }
    return 0;
}

int t_talk(void){
    int netty_talk;
    netty_talk = ttt_talk(NULL);
    clear();
    return (netty_talk);
}

struct user_info ui;
char page_requestor[STRLEN];
char page_requestorid[STRLEN];
int cmpunums(unum, up)
int unum;
struct user_info *up;
{
    if (!up->active)
        return 0;
    return (unum == up->destuid);
}

int cmpmsgnum(unum, up)
int unum;
struct user_info *up;
{
    if (!up->active)
        return 0;
    return (unum == up->destuid && up->sockactive == 2);
}

int setpagerequest(mode)
int mode;
{
    int tuid;

    if (mode == 0)
        tuid = search_ulist(&ui, cmpunums, getSession()->currentuid);
    else
        tuid = search_ulist(&ui, cmpmsgnum, getSession()->currentuid);
    if (tuid == 0)
        return 1;
    if (!ui.sockactive)
        return 1;
    uinfo.destuid = ui.uid;
    sprintf(page_requestor, "%s (%s)", ui.userid, ui.username);
    strcpy(page_requestorid, ui.userid);
    return 0;
}
int servicepage(int line, char *mesg)
{
    static time_t last_check;
    time_t now;
    char buf[STRLEN];
    int tuid = search_ulist(&ui, cmpunums, getSession()->currentuid);

    if (tuid == 0 || !ui.sockactive)
        talkrequest = false;
    if (!talkrequest) {
        if (page_requestor[0]) {
            switch (uinfo.mode) {
            case TALK:
                move(line, 0);
                printdash(mesg);
                break;
            default:           /* a chat mode */
                sprintf(mesg, "** %s ��ֹͣ����.", page_requestor);
            }
            memset(page_requestor, 0, STRLEN);
            last_check = 0;
        }
        return false;
    } else {
        now = time(0);
        if (now - last_check > P_INT) {
            last_check = now;
            if (!page_requestor[0] && setpagerequest(0 /*For Talk */ ))
                return false;
            else
                switch (uinfo.mode) {
                case TALK:
                    move(line, 0);
                    sprintf(buf, "** %s ���ں�����", page_requestor);
                    printdash(buf);
                    break;
                default:       /* chat */
                    sprintf(mesg, "** %s ���ں�����", page_requestor);
                }
        }
    }
    return true;
}

int talkreply()
{
    int a;
    char buf[512];
    char reason[51];
    struct sockaddr_in sin;
    char inbuf[STRLEN * 2];

    talkrequest = false;
    if (setpagerequest(0 /*For Talk */ ))
        return 0;
    /*
     * added by netty  
     */
    set_alarm(0, 0, NULL, NULL);
    clear();
    /*
     * to show plan -cuteyu 
     */
    move(5, 0);
    clrtobot();
    show_user_plan(page_requestorid);
    /*
     * Add by SmallPig 
     */
    move(1, 0);
    prints("(N)����Ǹ�������ں�æ�����ܸ����ġ���(B)�������ںܷ���������������졣 ��\n");
    prints("(C)�����м��£��ҵ�һ���� Call �㡣��(D)���벻Ҫ�� Page���Ҳ�������ġ���\n");
    prints("(E)����Ҫ�뿪�ˣ��´����İɡ�      ��(F)�����һ���Ÿ��ң�������û�ա� ��\n");
    prints("(M)�����Ը� %-12s           ��\n", page_requestorid);
    sprintf(inbuf, "����� %s ��������? (Y N B C D E F)[Y]: ", page_requestor);
    strcpy(save_page_requestor, page_requestor);
    /*
     * 2000.9.15 Bigman ���Talk��¼ 
     */
#ifdef TALK_LOG
    strcpy(partner, page_requestorid);
#endif                          /* 
                                 */
    memset(page_requestor, 0, sizeof(page_requestor));
    memset(page_requestorid, 0, sizeof(page_requestorid));
    getdata(0, 0, inbuf, buf, STRLEN, DOECHO, NULL, true);
    /*
     * gethostname(hostname,STRLEN) ;
     * if(!(h = gethostbyname(hostname))) {
     * perror("gethostbyname") ;
     * return -1 ;
     * }
     * memcpy( &sin.sin_addr, h->h_addr, h->h_length) ;
     * ����Ӧ����INADDR_LOOPBACK�ȽϺ� KCN
     */
    memset(&sin, 0, sizeof sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sin.sin_port = ui.sockaddr;
    a = socket(sin.sin_family, SOCK_STREAM, 0);
    if ((connect(a, (struct sockaddr *) &sin, sizeof sin))) {
        prints("connect err");
        return 0;
    }
    if (buf[0] != 'n' && buf[0] != 'N' && buf[0] != 'B' && buf[0] != 'b'
        && buf[0] != 'C' && buf[0] != 'c' && buf[0] != 'D' && buf[0] != 'd' && buf[0] != 'e' && buf[0] != 'E' && buf[0] != 'f' && buf[0] != 'F' && buf[0] != 'm' && buf[0] != 'M')
        buf[0] = 'y';
    if (buf[0] == 'M' || buf[0] == 'm') {
        move(1, 0);
        clrtobot();
        getdata(1, 0, "������", reason, 50, DOECHO, NULL, true);
    }
    write(a, buf, 1);
    if (buf[0] == 'M' || buf[0] == 'm')
        write(a, reason, sizeof reason);
    if (buf[0] != 'y') {
        close(a);
        bbslog("user","%s","page refused");
        clear();
        return 0;
    }
    bbslog("user","%s","page accepted");
    clear();
    do_talk(a);
    close(a);
    clear();
    return 0;
}

int dotalkent(uentp, buf)
struct user_info *uentp;
char *buf;
{
    char mch;

    if (!uentp->active || !uentp->pid)
        return -1;
    if (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible)
        return -1;
    switch (uentp->mode) {
    case ULDL:
        mch = 'U';
        break;
    case TALK:
        mch = 'T';
        break;
    case CHAT1:
    case CHAT2:
    case CHAT3:
    case CHAT4:
        mch = 'C';
        break;
    case IRCCHAT:
        mch = 'I';
        break;
    case FOURM:
        mch = '4';
        break;
    case BBSNET:
        mch = 'B';
        break;
    case READNEW:
    case READING:
        mch = 'R';
        break;
    case POSTING:
        mch = 'P';
        break;
    case SMAIL:
    case RMAIL:
    case MAIL:
        mch = 'M';
        break;
    default:
        mch = '-';
    }
    sprintf(buf, "%s%s(%c), ", uentp->invisible ? "*" : "", uentp->userid, mch);
    return 0;
}
int dotalkuent(struct user_info *uentp, char *arg, int pos)
{
    char buf[STRLEN];

    if (dotalkent(uentp, buf) != -1) {
        strcpy(talk_uent_buf, buf);
        talk_uent_buf += strlen(buf);
    }
    return 0;
}

void do_talk_nextline(twin)
struct talk_win *twin;
{
    twin->curln = twin->curln + 1;
    if (twin->curln > twin->eline)
        twin->curln = twin->sline;
    /*
     * if( curln != twin->eline ) {
     * move( curln + 1, 0 );
     * clrtoeol();
     * } 
     */
    move(twin->curln, 0);
    clrtoeol();
    twin->curcol = 0;
}

void do_talk_char(twin, ch)
struct talk_win *twin;
int ch;
{
    if (isprint2(ch)) {
        if (twin->curcol < 79) {
            move(twin->curln, (twin->curcol)++);
            prints("%c", ch);
            return;
        }
        do_talk_nextline(twin);
        twin->curcol++;
        prints("%c", ch);
        return;
    }
    switch (ch) {
    case Ctrl('H'):
    case '\177':
        if (twin->curcol == 0) {
            return;
        }
        (twin->curcol)--;
        move(twin->curln, twin->curcol);
        prints(" ");
        move(twin->curln, twin->curcol);
        return;
    case Ctrl('M'):
    case Ctrl('J'):
        do_talk_nextline(twin);
        return;
    case Ctrl('G'):
        bell();
        return;
    default:
        break;
    }
    return;
}

void do_talk_string(twin, str)
struct talk_win *twin;
char *str;
{
    while (*str) {
        do_talk_char(twin, *str++);
    }
}
void dotalkuserlist(twin)
struct talk_win *twin;
{
    char bigbuf[USHM_SIZE * 20];        /* change MAXACTIVE->USHM_SIZE, dong, 1999.9.15 */
    int savecolumns;

    do_talk_string(twin, "\n*** �������� ***\n");
    savecolumns = (t_columns > STRLEN ? t_columns : 0);
    talk_uent_buf = bigbuf;
    if (apply_ulist_addr((APPLY_UTMP_FUNC) dotalkuent, 0) == -1) {
        strcpy(bigbuf, "û���κ�ʹ��������\n");
    }
    strcpy(talk_uent_buf, "\n");
    do_talk_string(twin, bigbuf);
    if (savecolumns)
        t_columns = savecolumns;
}

char talkobuf[80];
int talkobuflen;
int talkflushfd;
void talkflush()
{
    if (talkobuflen)
        write(talkflushfd, talkobuf, talkobuflen);
    talkobuflen = 0;
}
static void moveto(int mode, struct talk_win *twin)
{
    if (mode == 1)
        twin->curln--;
    if (mode == 2)
        twin->curln++;
    if (mode == 3)
        twin->curcol++;
    if (mode == 4)
        twin->curcol--;
    if (twin->curcol < 0) {
        twin->curln--;
        twin->curcol = 0;
    } else if (twin->curcol > 79) {
        twin->curln++;
        twin->curcol = 0;
    }
    if (twin->curln < twin->sline) {
        twin->curln = twin->eline;
    }
    if (twin->curln > twin->eline) {
        twin->curln = twin->sline;
    }
    move(twin->curln, twin->curcol);
}
void endmsg(void *data)
{
    int x, y;
    int tmpansi;

    tmpansi = showansi;
    showansi = 1;
    talkidletime += 60;
    if (talkidletime >= IDLE_TIMEOUT)
        kill(getpid(), SIGHUP);
    if (uinfo.in_chat == true)
        return;
    getyx(&x, &y);
    update_endline();
    move(x, y);
    refresh();
    set_alarm(60, 0, endmsg, NULL);
    showansi = tmpansi;
    return;
}
static int do_talk(int fd)
{
    struct talk_win mywin, itswin;
    char mid_line[256];
    int page_pending = false;
    int i, i2;
    int previous_mode;

#ifdef TALK_LOG
    char mywords[80], itswords[80], buf[80];
    int mlen = 0, ilen = 0;
    time_t now;

    mywords[0] = itswords[0] = '\0';
#endif                          /* 
                                 */
    endmsg(NULL);
    previous_mode = uinfo.mode;
    modify_user_mode(TALK);
    sprintf(mid_line, " %s (%s) �� %s ���ڳ�̸��", getCurrentUser()->userid, getCurrentUser()->username, save_page_requestor);
    memset(&mywin, 0, sizeof(mywin));
    memset(&itswin, 0, sizeof(itswin));
    i = (t_lines - 1) / 2;
    mywin.eline = i - 1;
    itswin.curln = itswin.sline = i + 1;
    itswin.eline = t_lines - 2;
    move(i, 0);
    printdash(mid_line);
    move(0, 0);
    talkobuflen = 0;
    talkflushfd = fd;
    add_io(fd, 0);
    add_flush(talkflush);
    while (true) {
        int ch;

        if (talkrequest)
            page_pending = servicepage((t_lines - 1) / 2, mid_line);
        ch = igetkey();
        if (ch>255&&ch<0)
        	continue;
        talkidletime = 0;
        if (ch == I_OTHERDATA) {
            char data[80];
            int datac;
            register int i;

            datac = read(fd, data, 80);
            if (datac <= 0)
                break;
            for (i = 0; i < datac; i++) {
                if (data[i] >= 1 && data[i] <= 4) {
                    moveto(data[i] - '\0', &itswin);
                    continue;
                }
#ifdef TALK_LOG
                /*
                 * Bigman 2000.9.15 ���TALK��¼
                 */
                /*
                 * existing do_log() overflow problem       
                 */
                else if (isprint2(data[i])) {
                    if (ilen >= 80) {
                        itswords[80] = '\0';
                        (void) do_log(itswords, 2);
                        ilen = 0;
                    } else {
                        itswords[ilen] = data[i];
                        ilen++;
                    }
                } else if ((data[i] == Ctrl('H') || data[i] == '\177') && !ilen) {
                    itswords[ilen--] = '\0';
                } else if (data[i] == Ctrl('M') || data[i] == '\r' || data[i] == '\n') {
                    itswords[ilen] = '\0';
                    (void) do_log(itswords, 2);
                    ilen = 0;
                }
#endif
                do_talk_char(&itswin, data[i]);
            }
        } else {
            if (ch == Ctrl('D') || ch == Ctrl('C'))
                break;
            if (isprint2(ch) || ch == Ctrl('H') || ch == '\177' || ch == Ctrl('G') /* || ch == Ctrl('M') */ ) {
                talkobuf[talkobuflen++] = ch;
                if (talkobuflen == 80)
                    talkflush();
#ifdef TALK_LOG
                if (mlen < 80) {
                    if ((ch == Ctrl('H') || ch == '\177') && mlen != 0) {
                        mywords[mlen--] = '\0';
                    } else {
                        mywords[mlen] = ch;
                        mlen++;
                    }
                } else if (mlen >= 80) {
                    mywords[80] = '\0';
                    (void) do_log(mywords, 1);
                    mlen = 0;
                }
#endif                          /* 
                                 */
                do_talk_char(&mywin, ch);
                /*
                 * } else if (ch == '\n') {   Bigman 2000.9.15 
                 */
            } else if (ch == '\n' || ch == Ctrl('M') || ch == '\r') {
#ifdef TALK_LOG
                if (mywords[0] != '\0') {
                    mywords[mlen++] = '\0';
                    (void) do_log(mywords, 1);
                    mlen = 0;
                }
#endif                          /* 
                                 */
                talkobuf[talkobuflen++] = '\r';
                talkflush();
                do_talk_char(&mywin, '\r');
                /*
                 * } else if (ch == Ctrl('T')) {
                 * now=time(0);
                 * strcpy(ct,ctime(&now));
                 * do_talk_string( &mywin, ct);
                 * } else if (ch == Ctrl('U') || ch == Ctrl('W')) {
                 * dotalkuserlist( &mywin ); 
                 */
            } else if (ch >= KEY_UP && ch <= KEY_LEFT) {
                moveto(ch - KEY_UP + 1, &mywin);
                talkobuf[talkobuflen++] = ch - KEY_UP + 1;
                if (talkobuflen == 80)
                    talkflush();
            } else if (ch == Ctrl('E')) {
                for (i2 = 0; i2 <= 10; i2++) {
                    talkobuf[talkobuflen++] = '\r';
                    talkflush();
                    do_talk_char(&mywin, '\r');
                }
            } else if (ch == Ctrl('P') && HAS_PERM(getCurrentUser(), PERM_BASIC)) {
                t_pager();
                update_endline();
            } else if (Ctrl('Z') == ch)
                r_lastmsg();    /* Leeward 98.07.30 support msgX */
        }
    }
    add_io(0, 0);
    talkflush();
    set_alarm(0, 0, NULL, NULL);
    add_flush(NULL);
    modify_user_mode(previous_mode);
#ifdef TALK_LOG
    /*
     * 2000.9.15 Bigman ���Talk��¼ 
     */
    mywords[mlen] = '\0';
    itswords[ilen] = '\0';
    if (mywords[0] != '\0')
        do_log(mywords, 1);
    if (itswords[0] != '\0')
        do_log(itswords, 2);
    now = time(0);
    sprintf(buf, "\n\033[1;34mͨ������, ʱ��: %s \033[m\n", Cdate(now));
    write(talkrec, buf, strlen(buf));
    close(talkrec);
    /*---	���������?	commented by period	---*/
    /*
     * sethomefile(genbuf, getCurrentUser()->userid, "talklog");  
     */
    /*---	changed by period	2000-09-18	---*/
    *genbuf = 0;
    move(t_lines - 1, 0);
    if (askyn("�Ƿ�Ļ������¼ ", false) == true) {
        /*---						---*
            getdata(23, 0, "�Ƿ�Ļ������¼ [Y/n]: ", genbuf, 2, DOECHO, NULL, true); 
            if (genbuf[0] != 'N' || genbuf[0] != 'n')  {
         *---	also '||' used above is wrong...	---*/
        sethomefile(buf, getCurrentUser()->userid, "talklog");
        sprintf(mywords, "�� %s �������¼ [%12.12s]", partner, Ctime(now) + 6);
        mail_file(getCurrentUser()->userid, buf, getCurrentUser()->userid, mywords, 0, NULL);
    }
    sethomefile(buf, getCurrentUser()->userid, "talklog");
    unlink(buf);
#endif                          /* 
                                 */
    return 0;
}

int shortulist(uentp)
struct user_info *uentp;
{
    int i;
    int pageusers = 60;
    extern struct user_info *user_record[];
    extern int range;
	char buf[80];
    if (ulistpage > ((range - 1) / pageusers))
        ulistpage = 0;
    if (ulistpage < 0)
        ulistpage = (range - 1) / pageusers;
    move(1, 0);
    clrtoeol();
    prints("ÿ�� %d �����һ�Σ�Ctrl-C �� Ctrl-D �뿪��[F]����ģʽ[����]�ϡ���һҳ  ��%1dҳ", M_INT, ulistpage + 1);
    clrtoeol();
    move(3, 0);
    clrtobot();
    for (i = ulistpage * pageusers; i < (ulistpage + 1) * pageusers && i < range; i++) {
        char ubuf[STRLEN];
        int ovv;

        if (i < numf || friendmode)
            ovv = true;
        else
            ovv = false;
        sprintf(ubuf, "%s%-12.12s %s%-10.10s\033[m", (ovv) ? "\033[32m��" : "  ", user_record[i]->userid, 
                (user_record[i]->invisible == true)? (user_record[i]->pid==1?"\033[33m":"\033[34m") : (user_record[i]->pid==1?"\033[1;36m":""), 
                modestring(buf,user_record[i]->mode, user_record[i]->destuid, 0, NULL));
        prints("%s", ubuf);
        if ((i + 1) % 3 == 0)
            prints("\n");
        else
            prints(" |");
    }
    return range;
}

int do_list(modestr)
char *modestr;
{
    char buf[STRLEN];
    int count;
    extern int RMSG;
    int chkmailflag = 0;

    if (RMSG != true) {         /*����յ� Msg ��һ�в���ʾ�� */
        move(0, 0);
        clrtoeol();
        chkmailflag = chkmail();
        if (chkmailflag == 2)
            /*
             * Haohmaru.99.4.4.������Ҳ������ 
             */
            showtitle(modestr, "[�������䳬������,����������!]");
        else if (chkmailflag)
            showtitle(modestr, "[�����ż�]");
        else
            showtitle(modestr, BBS_FULL_NAME);
	update_endline();
    }
    move(2, 0);
    clrtoeol();
    sprintf(buf, "  %-12s %-10s", "ʹ���ߴ���", "Ŀǰ��̬");
    prints("\033[33m\033[44m%s |%s |%s\033[m", buf, buf, buf);
    /*
     * if(apply_ulist( shortulist ) == -1) {
     * prints("No Users Exist\n") ;
     * return 0;
     * }
     * count = shortulist(NULL); 
     */
    count = shortulist(NULL);
    if (uinfo.mode == MONITOR) {
        time_t thetime = time(0);

        move(t_lines - 1, 0);
        prints("\033[44m\033[33mĿǰ�� %3d %6s����, ʱ��: %s , Ŀǰ״̬��%10s   \033[m", count, friendmode ? "������" : "ʹ����", Ctime(thetime), friendmode ? "��ĺ�����" : "����ʹ����");
    }
    return 0;
}

int t_list(void){
    modify_user_mode(LUSERS);
    bbslog("user","%s","t_list");
    do_list("ʹ����״̬");
    pressreturn();
    clear();
    return 0;
}
void sig_catcher(void *data)
{
    ulistpage++;
    if (uinfo.mode != MONITOR) {
        set_alarm(0, 0, NULL, NULL);
        return;
    }
    do_list("̽������");
    idle_count++;
    set_alarm(M_INT * idle_count, 0, sig_catcher, NULL);
    UNUSED_ARG(data);
}

int t_monitor(void){
    int i;
    set_alarm(0, 0, NULL, NULL);
    /*
     * idle_monitor_time = 0; 
     */
    bbslog("user","%s","monitor");
    modify_user_mode(MONITOR);
    ulistpage = 0;
    fill_userlist();
    do_list("̽������");
    set_alarm(M_INT * idle_count, 0, sig_catcher, NULL);
    while (true) {
        i = igetkey();
        if (i==KEY_REFRESH)
            do_list("̽������");
        else
        if (Ctrl('Z') == i)
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
        else
        if (i == 'f' || i == 'F' ) {
            if (friendmode == true)
                friendmode = false;
            else
                friendmode = true;
            fill_userlist();
            do_list("̽������");
        }
        else
        if (i == KEY_DOWN) {
            ulistpage++;
            do_list("̽������");
        }
        else
        if (i == KEY_UP) {
            ulistpage--;
            do_list("̽������");
        }
        else
        if (i == Ctrl('D') || i == Ctrl('C') || i == KEY_LEFT)
            break;
        /*
         * else if (i == -1) {
         * if (errno != EINTR) { perror("read"); exit(1); }
         * } else idle_monitor_time = 0; 
         */
    }
    move(2, 0);
    clrtoeol();
    clear();
    return 0;
}


int listfilecontent(fname)
char *fname;
{
    FILE *fp;
    int x = 0, y = 3, cnt = 0, max = 0, showline=1, len;
    char u_buf[STRLEN+1], line[STRLEN+1], *nick;
    char genbuf[STRLEN+1];

    move(y, x);
    CreateNameList();
    strcpy(genbuf, fname);
    if ((fp = fopen(genbuf, "r")) == NULL) {
        prints("(none)\n");
        return 0;
    }
    while (fgets(genbuf, STRLEN, fp) != NULL) {
	genbuf[STRLEN]=0;
        strtok(genbuf, " \n\r\t");
        strcpy(u_buf, genbuf);
        AddNameList(u_buf);
	if (showline==0) continue;
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
            snprintf(line, STRLEN, "%-12s%s", u_buf, nick);
        }
        if ((len = strlen(line)) > max)
            max = len;
        if (x + len > 78)
            line[78 - x] = '\0';
        if( showline ) prints("%s", line);
        cnt++;
        if ((++y) >= t_lines - 1) {
            y = 3;
            x += max + 2;
            max = 0;
            if (x > 68)
                showline = 0;
   //           break;
        }
        move(y, x);
    }
    fclose(fp);
    if (cnt == 0)
        prints("(none)\n");
    return cnt;
}

int addtooverride(uident)
char *uident;
{
    struct friends tmp;
    int n, uid;
    char buf[STRLEN];

    memset(&tmp, 0, sizeof(tmp));
    sethomefile(buf, getCurrentUser()->userid, "friends");
    if ((!HAS_PERM(getCurrentUser(), PERM_SYSOP)) && (get_num_records(buf, sizeof(struct friends)) >= MAXFRIENDS)) {
        move(t_lines - 2, 0);
        clrtoeol();
        prints("��Ǹ����վĿǰ�������趨 %d ������, �밴�κμ�����...", MAXFRIENDS);
        igetkey();
        /* TODO: add KEY_REFRESH support */
        move(t_lines - 2, 0);
        clrtoeol();
        return -1;
    }
    uid = searchuser(uident);
    if (uid == 0)
        return -1;

    strcpy(tmp.id, uident);

    if (myfriend(uid, NULL,getSession())){
        move(2, 0);
        clrtoeol();

				prints("�û���%s���Ѿ������ĺ���������!\n", tmp.id);
				pressanykey();
        return -1;
      }
    if (uinfo.mode != LUSERS && uinfo.mode != LAUSERS && uinfo.mode != FRIEND) {
        move(2, 0);
        clrtoeol();

        char ans[3];
        sprintf(buf, "ȷʵҪ�����û���%s��Ϊ������?(Y/N) [N]: ", tmp.id);
        getdata(0, 0, buf, ans, sizeof(ans), DOECHO, NULL, true);
				if (ans[0] == 'Y' || ans[0] == 'y'){
	        sprintf(genbuf, "����������ѡ�%s����˵��: ", tmp.id);
	        getdata(2, 0, genbuf, tmp.exp, 15, DOECHO, NULL, true);
				}
				else{
				  return -1;
				}
    } else {
        move(t_lines - 2, 0);
        clrtoeol();
        sprintf(genbuf, "����������ѡ�%s����˵��: ", tmp.id);
        getdata(t_lines - 2, 0, genbuf, tmp.exp, 15, DOECHO, NULL, true);
    }
    sethomefile(genbuf, getCurrentUser()->userid, "friends");
    n = append_record(genbuf, &tmp, sizeof(struct friends));
    if (n != -1)
        getfriendstr(getCurrentUser(),get_utmpent(getSession()->utmpent),getSession());
    else
        bbslog("user","%s","append friendfile error");
    return n;
}
int deleteoverride(char *uident)
{
    int deleted;
    struct friends fh;

    sethomefile(genbuf, getCurrentUser()->userid, "friends");
    deleted = search_record(genbuf, &fh, sizeof(fh), (RECORD_FUNC_ARG) cmpfnames, uident);
    if (deleted > 0) {
        if (delete_record(genbuf, sizeof(fh), deleted, NULL, NULL) == 0)
            getfriendstr(getCurrentUser(),get_utmpent(getSession()->utmpent),getSession());
        else {
            deleted = -1;
            bbslog("user","%s","delete friend error");
        }
    }
    return (deleted > 0) ? 1 : -1;
}

void friend_title(struct _select_def* conf)
{
    int chkmailflag = 0;

    chkmailflag = chkmail();
    if (chkmailflag == 2)
        /*
         * Haohmaru.99.4.4.������Ҳ������ 
         */
        strcpy(genbuf, "[�������䳬������,����������!]");
    else if (chkmailflag)
        strcpy(genbuf, "[�����ż�]");
    else
        strcpy(genbuf, BBS_FULL_NAME);
    showtitle("[�༭��������]", genbuf);
    prints(" [��,e] �뿪 [h] ���� [��,r] ����˵���� [��,��] ѡ�� [a] ���Ӻ��� [d] ɾ������\n");
    prints("\033[44m ���  �����б�      ����˵��                                                   \033[m\n");
}

char *friend_doentry(char *buf, int ent, struct friends *fh,void* readdata,struct _select_def * conf)
{
    sprintf(buf, " %4d  %-12.12s  %s", ent, fh->id, fh->exp);
    return buf;
}

int friend_edit(struct _select_def* conf,struct friends *fh,void* extraarg)
{
    struct friends nh;
    char buf[STRLEN / 2];
    int pos;
    struct read_arg* read_arg=(struct read_arg*)conf->arg;

    pos = search_record(read_arg->direct, &nh, sizeof(nh), (RECORD_FUNC_ARG) cmpfnames, fh->id);
    move(t_lines - 2, 0);
    clrtoeol();
    if (pos > 0) {
        sprintf(buf, "������ %s ���º���˵��: ", fh->id);
        getdata(t_lines - 2, 0, buf, nh.exp, 15, DOECHO, NULL, false);
    }
    if (substitute_record(read_arg->direct, &nh, sizeof(nh), pos) < 0)
        bbslog("user","%s","Friend files subs err");
    move(t_lines - 2, 0);
    clrtoeol();
    return DIRCHANGED;
}

int friend_help(struct _select_def* conf,struct friends *fh,void* extraarg)
{
    show_help("help/friendshelp");
    return FULLUPDATE;
}

int friend_add()
{
    char uident[STRLEN];

    clear();
    move(1, 0);
    usercomplete("������Ҫ���ӵĴ���: ", uident);
    if (uident[0] != '\0') {
        if (searchuser(uident) <= 0) {
            move(2, 0);
            prints(MSG_ERR_USERID);
            pressanykey();
        } else
            addtooverride(uident);
    }
    return DIRCHANGED;
}

int friend_dele(struct _select_def* conf,struct friends *fh,void* extraarg)
{
    char buf[STRLEN];
    int deleted = false;

    saveline(t_lines - 2, 0, NULL);
    move(t_lines - 2, 0);
    clrtoeol();
    sprintf(buf, "�Ƿ�ѡ�%s���Ӻ���������ȥ��", fh->id);
    if (askyn(buf, false) == true) {
        move(t_lines - 2, 0);
        clrtoeol();
        if (deleteoverride(fh->id) == 1) {
            prints("�ѴӺ����������Ƴ���%s��,���κμ�����...", fh->id);
            deleted = true;
        } else
            prints("�Ҳ�����%s��,���κμ�����...", fh->id);
    } else {
        move(t_lines - 2, 0);
        clrtoeol();
        prints("ȡ��ɾ������...");
    }
    igetkey();
    /* TODO: add KEY_REFRESH support */
    move(t_lines - 2, 0);
    clrtoeol();
    saveline(t_lines - 2, 1, NULL);
    return (deleted) ? FULLUPDATE : DONOTHING;
}

int friend_mail(struct _select_def* conf,struct friends *fh,void* extraarg)
{
    if (!HAS_PERM(getCurrentUser(), PERM_POST))
        return DONOTHING;
    m_send(fh->id);
    return FULLUPDATE;
}

int friend_query(struct _select_def* conf,struct friends *fh,void* extraarg)
{
    int ch;

    if (t_query(fh->id) == -1)
        return FULLUPDATE;
    move(t_lines - 1, 0);
    clrtoeol();
    prints("\033[44m\033[31m[��ȡ����˵����]\033[33m ���Ÿ����� m �� ���� Q,�� ����һλ ������һλ <Space>,��      \033[m");
    ch = igetkey();
    switch (ch) {
    case Ctrl('Z'):
        r_lastmsg();            /* Leeward 98.07.30 support msgX */
        break;
    case KEY_REFRESH:
    case 'N':
    case 'Q':
    case 'n':
    case 'q':
    case KEY_LEFT:
        break;
    case 'm':
    case 'M':
        m_send(fh->id);
        break;
    case ' ':
    case 'j':
    case KEY_RIGHT:
    case KEY_DOWN:
    case KEY_PGDN:
        return READ_NEXT;
    case KEY_UP:
    case KEY_PGUP:
        return READ_PREV;
    default:
        break;
    }
    return FULLUPDATE;
}

struct key_command friend_list[] = {
    {'r', (READ_KEY_FUNC)friend_query,NULL},
    {'m', (READ_KEY_FUNC)friend_mail,NULL},
    {'M', (READ_KEY_FUNC)friend_mail,NULL},
    {'a', (READ_KEY_FUNC)read_callfunc0, (void *)friend_add},
    {'A', (READ_KEY_FUNC)read_callfunc0, (void *)friend_add},
    {'d', (READ_KEY_FUNC)friend_dele,NULL},
    {'D', (READ_KEY_FUNC)friend_dele,NULL},
    {'E', (READ_KEY_FUNC)friend_edit,NULL},
    {'h', (READ_KEY_FUNC)friend_help,NULL},
    {'H', (READ_KEY_FUNC)friend_help,NULL},
    {'\0', NULL},
};

int t_override(void){
    char genbuf[PATHLEN];
    sethomefile(genbuf, getCurrentUser()->userid, "friends");
    new_i_read(DIR_MODE_FRIEND, genbuf, friend_title, (READ_ENT_FUNC) friend_doentry, friend_list, sizeof(struct friends));
    clear();
    return 0;
}

int cmpfuid(a, b)
struct friends *a, *b;
{
    return strncasecmp(a->id, b->id, IDLEN);
}

int wait_friend(void){
    FILE *fp;
    int tuid, oldmode;
    char buf[STRLEN];
    char uid[13];

    oldmode = uinfo.mode;
    modify_user_mode(WFRIEND);
    clear();
    move(1, 0);
    usercomplete("������ʹ���ߴ����Լ���ϵͳ��Ѱ������: ", uid);
    if (uid[0] == '\0') {
        clear();
        modify_user_mode(oldmode);
        return 0;
    }
    if (!(tuid = getuser(uid, NULL))) {
        move(2, 0);
        prints("\033[1m����ȷ��ʹ���ߴ���\033[m\n");
        pressanykey();
        clear();
        modify_user_mode(oldmode);
        return -1;
    }
    sprintf(buf, "��ȷ��Ҫ�� %s ����ϵͳѰ��������", uid);
    move(2, 0);
    if (askyn(buf, true) == false) {
        clear();
        modify_user_mode(oldmode);
        return -1;
    }
    if ((fp = fopen("friendbook", "a")) == NULL) {
        prints("ϵͳ��Ѱ�������޷���������֪ͨվ��...\n");
        pressanykey();
        modify_user_mode(oldmode);
        return -1;
    }
    sprintf(buf, "%d@%s", tuid, getCurrentUser()->userid);
    if (!seek_in_file("friendbook", buf))
        fprintf(fp, "%s\n", buf);
    fclose(fp);
    move(3, 0);
    prints("�Ѿ��������Ѱ�������У�%s ��վϵͳһ����֪ͨ��...\n", uid);
    pressanykey();
    clear();
    modify_user_mode(oldmode);
    return 0;
}

/* ��������:Bigman 2000.12.26 */
int list_ignore(fname)
char *fname;
{
    FILE *fp;
    int x = 0, y = 4, nIdx = 0;
    char buf[IDLEN + 1], buf2[80];

    clear();
    move(y, x);
    /*
     * clrtoeol(); 
     */
    if ((fp = fopen(fname, "r")) == NULL) {
        prints("\033[1;33m*** ��δ�趨������ ***\033[m");
        return (0);
    } else {
        strcpy(buf2, "\033[1;32m���������ϵ��û�ID�б�\033[m");
        while (fread(buf, IDLEN + 1, 1, fp) > 0) {
            if (nIdx % 4 == 0) {
                prints(buf2);
                memset(buf2, 0, IDLEN + 1);
                y++;
                move(y, x);
            }
            nIdx++;
            sprintf(buf2 + strlen(buf2), "  %-13s", buf);
        }
        if (nIdx > 0) {
            prints(buf2);
        } else {
            prints("\033[1;32m*** ��δ�趨������ ***\033[m");
        }
        y++;
        move(y, x);
        clrtoeol();
        fclose(fp);
        return (nIdx);
    }
}
void clear_press()
{                               /* 2000.12.28 Bigman �ظ�����Ļس������ */
    pressreturn();
    move(1, 0);
    clrtoeol();
    move(2, 0);
    clrtoeol();
}

int badlist(void){
    char userid[IDLEN + 1], tmp[3];
    int cnt, nIdx;
    char ignoreuser[IDLEN + 1], path[40];
    int search_record(), append_record();
    int usercomplete(), namecomplete();

    modify_user_mode(EDITUFILE);
    clear();
    sethomefile(path, getCurrentUser()->userid, "ignores");
    while (1) {
        cnt = list_ignore(path);
        if (cnt >= MAX_IGNORE) {
            move(1, 0);
            prints("�Ѿ���������������������");
            getdata(0, 0, "(D)ɾ�� (C)��� (Q)����? [Q]�� ", tmp, 2, DOECHO, NULL, true);
        } else if (cnt <= 0)
            getdata(0, 0, "(A)���� (Q)����? [Q]�� ", tmp, 2, DOECHO, NULL, true);
        else
            getdata(0, 0, "(A)���� (D)ɾ�� (C)��� (Q)����? [Q]�� ", tmp, 2, DOECHO, NULL, true);
        if (tmp[0] == 'Q' || tmp[0] == 'q' || tmp[0] == '\0') {
            break;
        }
        if (((tmp[0] == 'a' || tmp[0] == 'A') && (cnt < MAX_IGNORE)) || ((tmp[0] == 'd' || tmp[0] == 'D') && (cnt > 0))) {
            usercomplete("������ʹ���ߴ���(ֻ�� ENTER ��������): ", userid);
            if (userid[0] == '\0') {
                move(1, 0);
                clrtoeol();
                continue;
            }
            if (!searchuser(userid)) {
                prints("���ʹ���ߴ����Ǵ����.\n");
                clear_press();
            } else if (!strcasecmp(userid, getCurrentUser()->userid)) {
                prints("�������Լ��Ĵ���\n");
                clear_press();
            } else {
                nIdx = search_record(path, ignoreuser, IDLEN + 1, (RECORD_FUNC_ARG) cmpinames, userid);
                if (tmp[0] == 'a' || tmp[0] == 'A') {
                    if (nIdx > 0) {
                        prints("��ID�Ѿ��ں������ϣ�");
                        clear_press();
                    } else {
                        if (append_record(path, userid, IDLEN + 1) == 0) {
                            /*
                             * prints("�Ѿ��ɹ���ӵ���������"); 
                             */
                            /*
                             * cnt=list_ignore(path); 
                             */
                        } else {
                            prints("*** ϵͳ���� , ����SYSOP��ϵ***");
                            clear_press();
                        }
                    }
                } else {
                    if (nIdx <= 0) {
                        prints("��IDû���ں������ϣ�");
                        clear_press();
                    } else {
                        if (delete_record(path, IDLEN + 1, nIdx, NULL, NULL) == 0) {
                            ;
                            /*
                             * prints("�Ѿ��ɹ��Ӻ�������ɾ��"); 
                             */
                            /*
                             * cnt=list_ignore(path); 
                             */
                        } else {
                            prints("*** ϵͳ���� , ����SYSOP��ϵ***");
                            clear_press();
                        }
                    }
                }
            }
        } else if ((tmp[0] == 'c' || tmp[0] == 'C') && (cnt > 0)) {
            getdata(1, 0, "ȷ��ɾ��������? (Y/N) [N]:", tmp, 2, DOECHO, NULL, true);
            if (tmp[0] == 'y' || tmp[0] == 'Y') {
                unlink(path);
                /*
                 * cnt=list_ignore(path); 
                 */
            } else {
                move(1, 0);
                clrtoeol();
            }
        }
    }
    pressreturn();
    return (cnt);
}

#ifdef TALK_LOG
/* Bigman 2000.9.15 �ֱ�Ϊ��λ�����������¼ */
/* -=> �Լ�˵�Ļ� */
/* --> �Է�˵�Ļ� */
void do_log(char *msg, int who)
{
    time_t now;
    char buf[100];

    now = time(0);
    if (msg[strlen(msg)] == '\n')
        msg[strlen(msg)] = '\0';
    if (strlen(msg) < 1 || msg[0] == '\r' || msg[0] == '\n')
        return;
    /*
     * ֻ���Լ��� 
     */
    sethomefile(buf, getCurrentUser()->userid, "talklog");
    if (!dashf(buf) || talkrec == -1) {
        talkrec = open(buf, O_RDWR | O_CREAT | O_TRUNC, 0644);
        sprintf(buf, "\033[1;32m�� %s �������¼, ����: %s \033[m\n", save_page_requestor, Cdate(now));
        write(talkrec, buf, strlen(buf));
        sprintf(buf, "\t��ɫ�ֱ����: \033[1;33m%s\033[m \033[1;36m%s\033[m \n\n", getCurrentUser()->userid, partner);
        write(talkrec, buf, strlen(buf));
    }
    if (who == 1) {             /* �Լ�˵�Ļ� */
        sprintf(buf, "\033[1;33m%s \033[m\n", msg);
        write(talkrec, buf, strlen(buf));
    } else if (who == 2) {      /* ����˵�Ļ� */
        sprintf(buf, "\033[1;36m%s \033[m\n", msg);
        write(talkrec, buf, strlen(buf));
    }
}
#endif
