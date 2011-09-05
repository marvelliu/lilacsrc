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


/* Leeward 99.03.06 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "bbs.h"

#define INPUT_ACTIVE 0
/*Haohmaru.98.11.3*/
#define INPUT_IDLE 1
#define WAITTIME  150

/* Marvel 07.07.08*/
#ifdef LILAC
#include "lilacfuncs.h"
#include <math.h>
#endif
/* KCN add 1999.11.07 
#undef LOGINASNEW 
*/


extern struct screenline *big_picture;
extern struct userec *user_data;

/* extern char* pnt; */

int temp_numposts;              /*Haohmaru.99.4.02.�ð���ˮ���˿�ȥ��//grin */
int nettyNN = 0;
int count_friends, count_users; /*Add by SmallPig for count users and Friends */
char *getenv();
char *Ctime();
void r_msg_sig(int signo);
int friend_login_wall();
int listmode;
jmp_buf byebye;

int convcode = 0;               /* KCN,99.09.05 */

FILE *ufp;
int RUNSH = false;
int ERROR_READ_SYSTEM_FILE = false;
int talkrequest = false;
time_t lastnote;

struct user_info uinfo;

time_t login_start_time;
int showansi = 1;

static int i_domode = INPUT_ACTIVE;
extern time_t calltime;
extern char calltimememo[];

int switch_code(void){
    convcode = !convcode;
    redoscr();
    return convcode;
}

int canbemsged(uin)             /*Haohmaru.99.5.29 */
    struct user_info *uin;
{
    if (uinfo.pager & ALLMSG_PAGER)
        return true;
    if (uinfo.pager & FRIENDMSG_PAGER) {
        if (hisfriend(getSession()->currentuid, uin))
            return true;
    }
    return false;
}

void wait_alarm_clock(int signo)
{                               /*Haohmaru.98.11.3 */
    if (i_domode == INPUT_IDLE) {
        clear();
        exit(0);
    }
    i_domode = INPUT_IDLE;
    alarm(WAITTIME);
}

void initalarm()
{                               /*Haohmaru.98.11.3 */
    signal(SIGALRM, wait_alarm_clock);
    alarm(WAITTIME);
}

void Net_Sleep(int times)
{                               /* KCN 1999.9.15 */
    struct timeval tv;
    int sr;
    fd_set efd;
    int old;

    int csock = 0;

    tv.tv_sec = times;
    tv.tv_usec = 0;
//    FD_ZERO(&fd);
    FD_ZERO(&efd);
//    FD_SET(csock, &fd);
    FD_SET(csock, &efd);
    old = time(0);

    while ((sr = select(csock + 1, NULL, NULL, &efd, &tv)) > 0) {
        if (FD_ISSET(csock, &efd))
            abort_bbs(0);
        tv.tv_sec = times - (time(0) - old);
        tv.tv_usec = 0;
//        FD_ZERO(&fd);
        FD_ZERO(&efd);
//        FD_SET(csock, &fd);
        FD_SET(csock, &efd);
    };

}


void u_enter()
{
    memset(&uinfo, 0, sizeof(uinfo));
    uinfo.active = true;
    uinfo.pid = getpid();

    /* Bigman 2000.8.29 �������ܹ����� */
    if ((HAS_PERM(getCurrentUser(), PERM_CHATCLOAK) || HAS_PERM(getCurrentUser(), PERM_CLOAK)) && (getCurrentUser()->flags & CLOAK_FLAG))
        uinfo.invisible = true;
    uinfo.mode = LOGIN;
    uinfo.pager = 0;
/*    uinfo.pager = !(getCurrentUser()->flags[0] & PAGER_FLAG);*/
    if (DEFINE(getCurrentUser(), DEF_FRIENDCALL)) {
        uinfo.pager |= FRIEND_PAGER;
    }
    if (getCurrentUser()->flags & PAGER_FLAG) {
        uinfo.pager |= ALL_PAGER;
        uinfo.pager |= FRIEND_PAGER;
    }
    if (DEFINE(getCurrentUser(), DEF_FRIENDMSG)) {
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    if (DEFINE(getCurrentUser(), DEF_ALLMSG)) {
        uinfo.pager |= ALLMSG_PAGER;
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    uinfo.uid = getSession()->currentuid;
    strncpy(uinfo.from, getSession()->fromhost, IPLEN);
    uinfo.freshtime = time(0);
	uinfo.logintime = time(0);
    strncpy(uinfo.userid, getCurrentUser()->userid, 20);

//    strncpy(uinfo.realname, curruserdata.realname, 20);
    strncpy(uinfo.realname, getSession()->currentmemo->ud.realname, 20);
    strncpy(uinfo.username, getCurrentUser()->username, 40);
    getSession()->utmpent = getnewutmpent(&uinfo, 0);
    if (getSession()->utmpent == -1) {
        prints("��������,�޷������û���Ŀ!\n");
        oflush();
        Net_Sleep(20);
        exit(-1);
    }

    getfriendstr(getCurrentUser(),get_utmpent(getSession()->utmpent), getSession());
    listmode = 0;
}

void setflags(mask, value)
    int mask, value;
{
    if (((getCurrentUser()->flags & mask) && 1) != value) {
        if (value)
            getCurrentUser()->flags |= mask;
        else
            getCurrentUser()->flags &= ~mask;
    }
}

/*---	moved to here from below	period	2000-11-19	---*/
int started = 0;
static void sync_stay(void){
    time_t now,stay;
    if(((stay=(now=time(NULL))-uinfo.logintime)<300)&&(started>1)&&(getCurrentUser()->numlogins>5))
        getCurrentUser()->numlogins--;
    else{
        if(!(now-uinfo.freshtime<IDLE_TIMEOUT))
            stay-=IDLE_TIMEOUT;
        getCurrentUser()->stay+=stay;
    }
    return;
}
void u_exit()
{
/*---	According to ylsdd's article, deal with SUPER_CLOAK problem	---*
 *---   Added by period		2000-09-19				---*/
/* ��Щ�źŵĴ���Ҫ�ص�, ����������ʱ�Ⱥ�س�ʱ�����źŻᵼ����д����,
 * ������µ��������ұ�kick user���� */
    signal(SIGHUP, SIG_DFL);
    signal(SIGALRM, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
/*---	Added by period		2000-11-19	sure of this	---*/
    if (!started || !uinfo.active)
        return;
/*---		---*/
    setflags(PAGER_FLAG, (uinfo.pager & ALL_PAGER));

    /* Bigman 2000.8.29 �������ܹ����� */
    if ((HAS_PERM(getCurrentUser(), PERM_CHATCLOAK) || HAS_PERM(getCurrentUser(), PERM_CLOAK)))
        setflags(CLOAK_FLAG, uinfo.invisible);

#ifdef HAVE_BRC_CONTROL
    brc_update(getCurrentUser()->userid, getSession());
#endif

    /* etnlegend, 2006.04.04, ��������ʱ�� */
    sync_stay();

    if (getSession()->utmpent > 0)
        clear_utmp(getSession()->utmpent, getSession()->currentuid, getpid());
}

int cmpuids(uid, up)
    char *uid;
    struct userec *up;
{
    return !strncasecmp(uid, up->userid, sizeof(up->userid));
}

int dosearchuser(userid)
    char *userid;
{
    int id;

    if ((id = getuser(userid, &getCurrentUser())) != 0)
        return getSession()->currentuid = id;
    return getSession()->currentuid = 0;
}

void talk_request(int signo)
{
    signal(SIGUSR1, talk_request);
    talkrequest = true;
    bell();
    sleep(1);
    bell();
    return;
}

extern int icurrchar, ibufsize;

void abort_bbs(int signo){
    static bool in_abort_bbs=false;
    time_t stay;
    if(in_abort_bbs)
        return;
    in_abort_bbs=true;
#ifndef SSHBBS
    output("\x1b[m",3);oflush();
#endif
    switch(uinfo.mode){
        case POSTING:
        case SMAIL:
        case EDIT:
        case EDITUFILE:
        case EDITSFILE:
        case EDITANN:
            keep_fail_post();
    }
    if(started){
        record_exit_time();
        stay=time(NULL)-uinfo.logintime;
/*---	period	2000-10-20	4 debug	---*/
        newbbslog(BBSLOG_USIES,"AXXED Stay: %3ld (%s)[%d %d]",stay/60,getCurrentUser()->username,
            getSession()->utmpent,getSession()->currentuid);
        u_exit();
    }
    shutdown(0,2);close(0);exit(0);
}

struct aol {
    int count;
    int ent[10];
	int mode[10];
    char idlemin[10][10];
    char ip[10][IPLEN+4];
    time_t login[10];
};

static int attach_online(struct user_info *uentp, int *arg, int pos){
    struct aol *a = (struct aol *)arg;
    int min;
    a->ent[a->count] = pos;
    strcpy(a->ip[a->count], uentp->from);
    a->login[a->count] = uentp->logintime;
	a->mode[a->count] = uentp->mode;
    min = (time(0) - uentp->freshtime) / 60;
    if (min) {
        snprintf(a->idlemin[a->count], 10, "[%d]", min);
    } else {
        strcpy(a->idlemin[a->count], "");
    }
    a->count++;
    if(a->count >= 10) return QUIT;
    return COUNT;
}

/* to be Continue to fix kick problem */
void multi_user_check()
{
	struct user_info *tmpinfo;
    char buffer[40];
    int ret = 1;
	int i;
	int num;
	struct aol a;

    while (ret != 0) {
        ret = multilogin_user(getCurrentUser(), getSession()->currentuid,0);
        if (ret == 3) {
            prints("\x1b[33m��Ǹ, ��IP��̫�� \x1b[36mguest ����, ���Ժ����ԡ�\x1b[m\n");
            pressreturn();
            oflush();
            sleep(5);
            exit(1);
        }
        if (ret == 2) {
            prints("\x1b[33m��Ǹ, Ŀǰ����̫�� \x1b[36mguest, ���Ժ����ԡ�\x1b[m\n");
            pressreturn();
            oflush();
            sleep(5);
            exit(1);
        }
        if (ret == 1) {
			memset(&a, 0, sizeof(a));
		    apply_utmp((APPLY_UTMP_FUNC) attach_online, 10, getCurrentUser()->userid, &a);

		    if(a.count==0) break;

			clear();
		    prints("��ͬʱ���ߵĴ��������࣬�޷��ٵ�¼����ѡ��ϣ���߳��Ĵ��ڣ��س��Ͽ���������\n");
			for(i=0;i<a.count;i++){
				move(i+2,0);
				prints("  %d: ip:%-15s ��¼ʱ��:%-24s,  %-12s %s\n",i+1, a.ip[i], a.login[i]?ctime(&a.login[i]):"δ֪",
                    modestring(buffer,a.mode[i], NULL, 0, NULL) , a.idlemin[i]);
			}
			buffer[0]='\0';
			move(15,0);
			getdata(0, 0, "�������ţ��س�����:", buffer, 3, DOECHO, NULL, true);
			num = atoi(buffer);
			if(num <1 || num >a.count){
            	oflush();
            	exit(1);
			}
            newbbslog(BBSLOG_USER,"%s","kicked (multi-login)");
			tmpinfo = get_utmpent(a.ent[num-1]);
            kick_user_utmp(getSession()->currentuid, tmpinfo, 0);
            sleep(1);
        }
    }
}

void system_init()
{
    login_start_time = time(0);
    gethostname(genbuf, 256);
#ifdef SINGLE
    if (strcmp(genbuf, SINGLE)) {
        prints("Not on a valid machine!\n");
        oflush();
        exit(-1);
    }
#endif

    signal(SIGHUP, abort_bbs);
    signal(SIGPIPE, abort_bbs);
    signal(SIGTERM, abort_bbs);
    signal(SIGQUIT, abort_bbs);
    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGURG, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGUSR1, talk_request);
    msg_count=0;
    signal(SIGUSR2, r_msg_sig);
}

#ifdef FREE
static int strallalpha(char *uid)
{
	char *c;

	for(c=uid; *c; c++){
		if(!isalpha(*c))
			return 0;
	}

	return 1;

}
#endif

void login_query()
{
	const char *ptr;
    int curr_login_num, i;
    int curr_http_num;          /* Leeward 99.03.06 */
    char fname[STRLEN], tmpstr[30], genbuf[PATHLEN];
    FILE *fn;
#ifndef SSHBBS
    char uid[STRLEN], passbuf[40];
    int attempts;
    char buf[256];
#endif /* !defined(SSHBBS) */

#ifndef SSHBBS
#ifdef LILAC_PROXY
	strcpy(uid,"guest");
	exec_mbem("@mod:service/libbbsnet.so#bbsnet_main");
	strcpy(uid,"guest");
	exit(0);
#endif
#endif

    curr_login_num = get_utmp_number();;
    if (curr_login_num >= MAXACTIVE) {
        ansimore("etc/loginfull", false);
        oflush();
        Net_Sleep(20);
        exit(1);
    }
    curr_http_num = 0;
/*disable by KCN     curr_http_num = num_active_http_users(); *//* Leeward 99.03.06 */

/* Ŀǰû��ʹ���������,sigh*/
    ptr = sysconf_str("BBSNAME");
    if (ptr == NULL)
        ptr = "��δ��������վ";
/* add by KCN for input bbs */

    initalarm();

    ansimore("etc/issue", false);

#if 0 /* added by atppp for ZIXIA */
// ws code to reset max_user. do not enable unless you know what you are doing.
	setpublicshmreadonly(0);
	get_publicshm()->max_user = 0;
	setpublicshmreadonly(1);
#endif
	{
		int nowon,nowmax;
		nowon = curr_login_num + getwwwguestcount();
		nowmax = get_publicshm()->max_user;
    	prints("\033[1m��ӭ���� ��\033[31m%s\033[37m�� \033[36m�������� \033[1m%d[���: %d](%d WWW GUEST)\033[m", BBS_FULL_NAME, nowon, nowon>nowmax?nowon:nowmax,getwwwguestcount());
	}
#ifndef SSHBBS
    attempts = 0;
#ifdef LOGINASNEW
    prints("\n\033[1m\033[37m���������� `\033[36mguest\033[37m', ע��������`\033[36mnew\033[37m',add `\033[36m.\33[37m' after your ID for BIG5\033[m");
#else
    prints("\n\033[1m\033[37m���������(���������� `\033[36mguest\033[37m', ��ͣע�����ʺ�,add '\033[36m.\33[37m' after your ID for BIG5\033[m");
#endif //LOGINASNEW
    while (1) {
        if (attempts++ >= 3) {
            ansimore("etc/goodbye", false);
            oflush();
            sleep(1);
            exit(1);
        }
/*Haohmaru.98.11.3*/
        initalarm();

        getdata(0, 0, "\n" LOGIN_PROMPT ": ", uid, IDLEN + 2, DOECHO, NULL, true);
        uid[IDLEN + 2] = 0;
        if (uid[strlen(uid) - 1] == '.') {
            convcode = 1;
            uid[strlen(uid) - 1] = 0;
        }

		/*
        // 2007����Marvel��ӣ���ֹguest����
		//2007��12��5��ȥ��
        if (!strcmp(uid,"guest")) {
	        prints("����ϵͳ��ֹguest���룬��ע����ߵ�½");
	        oflush();
	        sleep(1);
	        exit(1);
		}
		*/
        if (strcasecmp(uid, "new") == 0) {
        	clear();
			move(1,0);
			prints("���ã�����http://www.lilacbbs.com/bbs/member.php?mod=register.php����ע��");
        	pressanykey();
            exit(1);
#ifdef LOGINASNEW
            if (check_ban_IP(getSession()->fromhost, buf) <= 0) {
                new_register();
                sethomepath(tmpstr, getCurrentUser()->userid);
                sprintf(buf, "/bin/mv -f %s " BBSHOME "/homeback/%s", tmpstr, getCurrentUser()->userid);
                system(buf);
                setmailpath(tmpstr, getCurrentUser()->userid);       /*Haohmaru.00.04.23,����ܿ�ǰ�˵��� */
                sprintf(buf, "/bin/mv -f %s " BBSHOME "/mailback/%s", tmpstr, getCurrentUser()->userid);
                system(buf);
                /*����ע����û�һ���� added by binxun .2003-6-24*/
                #if defined(SMTH) || defined(ZIXIA)
                mail_file(DELIVER,"etc/tonewuser",getCurrentUser()->userid,"����ע���û�����",0,NULL);
                #endif
                break;
            }
            prints("��ϵͳ��Ϊ %s ��ԭ���ֹ����������ע�����û�\n", buf);
#else
            prints("\033[37m��ϵͳĿǰ�޷��� new ע��, ���� guest ����.\033[m\n");
#endif
        } else if (*uid == '\0' || !dosearchuser(uid)
#ifdef FREE
		|| ! strallalpha(uid)
#endif
						) {
            prints("\033[32m" MSG_ERR_USERID "\033[m\n");
        } else
/* Add by KCN for let sysop can use extra 10 UTMP */
        if (!HAS_PERM(getCurrentUser(), PERM_SYSOP) && (curr_login_num >= MAXACTIVE + 10)) {
            ansimore("etc/loginfull", false);
            oflush();
            sleep(1);
            exit(1);
        } else if ( /*strcmp */ strcasecmp(uid, "guest") == 0) {
            getCurrentUser()->userlevel = PERM_DENYMAIL|PERM_DENYRELAX;
            getCurrentUser()->flags = PAGER_FLAG;
            break;
        } else {
            if (!convcode)
                convcode = !(DEFINE(getCurrentUser(), DEF_USEGB));      /* KCN,99.09.05 */

            if(check_ip_acl(getCurrentUser()->userid, getSession()->fromhost)) {
                prints("�� ID ����ӭ���� %s ���û���byebye!", getSession()->fromhost);
                oflush();
                sleep(1);
                exit(1);
            }
            getdata(0, 0, "\033[1m\033[37m"PASSWD_PROMPT": \033[m", passbuf, 39, NOECHO, NULL, true);

            if (!checkpasswd2(passbuf, getCurrentUser())) {
                if(passbuf[0])
                    logattempt(getCurrentUser()->userid, getSession()->fromhost, "telnet");
                prints("\033[32m�����������...\033[m\n");
            } else {
                if (id_invalid(uid)) {
                    prints("\033[31m��Ǹ!!\033[m\n");
                    prints("\033[32m���ʺ�ʹ������Ϊ���ţ����ʺ��Ѿ�ʧЧ...\033[m\n");
                    prints("\033[32m�뱣���κ�ǩ�������վ������ ����(��)��Ϊ�����\033[m\n");
                    getdata(0, 0, "�� [RETURN] ����", genbuf, 10, NOECHO, NULL, true);
                    oflush();
                    sleep(1);
                    exit(1);
                }
                if (simplepasswd(passbuf)) {
                    prints("\033[33m* ������ڼ�, ��ѡ��һ�����ϵ�������Ԫ.\033[m\n");
                    getdata(0, 0, "�� [RETURN] ����", genbuf, 10, NOECHO, NULL, true);
                }
                /* passwd ok, covert to md5 --wwj 2001/5/7 */
#ifdef CONV_PASS
                if (getCurrentUser()->passwd[0]) {
                    bbslog("covert", "for md5passwd");
                    setpasswd(passbuf, getCurrentUser());
                }
#endif
                break;
            }
        }
    }
#else
    prints("\n%s ��ӭ��ʹ��ssh��ʽ���ʡ�", getCurrentUser()->userid);
    getdata(0, 0, "�� [RETURN] ����", genbuf, 10, NOECHO, NULL, true);
#endif //SSHBBS

#ifdef CHECK_CONNECT
    if(check_ID_lists(getCurrentUser()->userid)) {
        prints("�������Ƶ�ʹ��ߣ�byebye!");
        oflush();
        sleep(1);
        exit(1);
    }
#endif

	/* We must create home directory before initializing current userdata */
    sethomepath(genbuf, getCurrentUser()->userid);
    mkdir(genbuf, 0755);
/* init user data */
//    read_userdata(getCurrentUser()->userid, &curruserdata);

    clear();
    oflush();


    //2007-06-06 by Marvel ������û�С�IP�����ơ�Ȩ�޵����û�����ֹ����
    if (!HAS_PERM(getCurrentUser(), PERM_NOIPCTRL) && HAS_PERM(getCurrentUser(), PERM_LOGINOK)) {        
        //prints("\033[32m���ʺŵ�IP�ܵ����ƣ��뼤����� \033[36mSYSOP\033[32m ��ѯԭ��\033[m\n");
        
		if (read_user_memo(getCurrentUser()->userid, &getSession()->currentmemo) <= 0) {
			prints("���ڳ�����£������˳����ʺ��������������µ�½\n");
			oflush();
			sleep(1);
			igetkey();
			exit(1);
		}
    
        //prints("realname: %s\naddress: %s\nmobilenumber: %s\nrealemail:%s\n",currentmemo->ud.realname, currentmemo->ud.address, currentmemo->ud.telephone,currentmemo->ud.realemail);
        //pressanykey();
        char filename[256];
        if(generate_active_url(getSession()->currentmemo->ud.realname, getSession()->currentmemo->ud.address, getSession()->currentmemo->ud.telephone, getSession()->currentmemo->ud.realemail,filename, getSession()) ==1 )
        		ansimore(filename, true);
        else
        		prints("�������������,����ϵ����վ��\n");
        remove(filename);        
        pressanykey();
        oflush();
        clear();
        exit(1);
    }
    

    if (strcasecmp(getCurrentUser()->userid, "guest") && !HAS_PERM(getCurrentUser(), PERM_BASIC)) {
        int s[GIVEUPINFO_PERM_COUNT];
        get_giveupinfo(getCurrentUser(),s);
        if(!s[0])
            prints("\033[1;33mϵͳ��������Ѿ��������¼Ȩ��, ���跨��ϵ \033[1;32mSYSOP\033[1;33m ��֪ԭ��...\033[m\n");
        else{
            i=(int)(((s[0]<0)?(-s[0]):s[0])-(time(NULL)/86400));
            if(s[0]>0)
                prints("\033[1;33m���Ѿ����ڽ���(��¼)״̬, Ŀǰ����������� %d ��...\033[m\n",i);
            else
                prints("\033[1;33m���Ѿ��������¼Ȩ��, Ŀǰ���������� %d ��...\033[m\n",i);
        }
        oflush();
        sleep(1);
        exit(1);
    }
#ifdef DEBUG
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        prints("���˿ڽ��������ã������ӱ�վ���������Ŷ˿ڡ�\n");
        oflush();
        Net_Sleep(3);
        abort_bbs(0);
    }
#endif
    multi_user_check();

	i = read_user_memo( getCurrentUser()->userid, & getSession()->currentmemo ) ;

	if(i==0){
		char ans[3];
		move(1,0);
		prints("���ڳ�����£������˳����ʺ��������������µ�¼\n");
		getdata(3, 0, "ȷ��Ҫ�߳�������¼��(Y/n)[n]: ", ans, 2, DOECHO, NULL, true);
			if(ans[0]=='y' || ans[0]=='Y'){
				int uid;
    			struct userec *u;
				if( (uid = getuser( getCurrentUser()->userid, &u) ) != 0){
					kick_user_utmp(uid, NULL, 0);
				}
			}
	 	oflush();
		sleep(1);
		igetkey();
		exit(1);
	}else if(i<0){
		prints("��ʼ���û����ݴ���,�����:%d\n", i);
	 	oflush();
		sleep(1);
		igetkey();
		exit(1);
	}

#ifdef BIRTHFILEPATH
	mail_birth();
#endif

#ifdef ANONYPOST 
    srand(getpid()+time(NULL));
    getSession()->anonyindex = rand();
    if(getSession()->anonyindex < 65536)
        getSession()->anonyindex += 65536;
#endif
    
    alarm(0);
    signal(SIGALRM, SIG_IGN);   /*Haohmaru.98.11.12 */
    term_init();
    scrint = 1;
    sethomepath(tmpstr, getCurrentUser()->userid);
    sprintf(fname, "%s/%s.deadve", tmpstr, getCurrentUser()->userid);
    if ((fn = fopen(fname, "r")) != NULL) {
	    if(strcasecmp(getCurrentUser()->userid,"guest"))
            mail_file(getCurrentUser()->userid, fname, getCurrentUser()->userid, "�����������������Ĳ���...", BBSPOST_MOVE, NULL);
        else {
            fclose(fn);
			unlink(fname);
        }
    }
    temp_numposts = 0;          /*Haohmaru.99.4.02.�ð���ˮ���˿�ȥ��//grin */
}

void write_defnotepad()
{
    getCurrentUser()->notedate = time(NULL);
    return;
}

void notepad_init()
{
    FILE *check;
    char notetitle[STRLEN];
    char tmp[STRLEN * 2];
    char *fname, *bname, *ntitle;
    long int maxsec;
    time_t now;

    maxsec = 24 * 60 * 60;
    lastnote = 0;
    if ((check = fopen("etc/checknotepad", "r")) != NULL) {
        fgets(tmp, sizeof(tmp), check);
        lastnote = atol(tmp);
        fclose(check);
    } else
        lastnote = 0;
    if (lastnote == 0) {
        lastnote = time(NULL) - (time(NULL) % maxsec);
        check = fopen("etc/checknotepad", "w");
        if (check) {
            fprintf(check, "%lu", lastnote);
            fclose(check);
        }
        sprintf(tmp, "���԰��� %s Login �������ڶ�����ʱ��ʱ��Ϊ %s", getCurrentUser()->userid, Ctime(lastnote));
        bbslog("user","%s",tmp);
    }
    if ((time(NULL) - lastnote) >= maxsec) {
        move(t_lines - 1, 0);
        prints("�Բ���ϵͳ�Զ����ţ����Ժ�.....");
        now = time(0);
        if (check) {
        check = fopen("etc/checknotepad", "w");
        lastnote = time(NULL) - (time(NULL) % maxsec);
        fprintf(check, "%lu", lastnote);
        fclose(check);
        } else lastnote=0;
        if ((check = fopen("etc/autopost", "r")) != NULL) {
            while (fgets(tmp, STRLEN, check) != NULL) {
                fname = strtok(tmp, " \n\t:@");
                bname = strtok(NULL, " \n\t:@");
                ntitle = strtok(NULL, " \n\t:@");
                if (fname == NULL || bname == NULL || ntitle == NULL)
                    continue;
                else {
                    sprintf(notetitle, "[%.10s] %s", ctime(&now), ntitle);
                    if (dashf(fname)) {
                        post_file(getCurrentUser(), "", fname, bname, notetitle, 0, 1, getSession());
                        sprintf(tmp, "%s �Զ�����", ntitle);
                        bbslog("user","%s",tmp);
                    }
                }
            }
            fclose(check);
        }
        sprintf(notetitle, "[%.10s] ���԰��¼", ctime(&now));
        if (dashf("etc/notepad")) {
            post_file(getCurrentUser(), "", "etc/notepad", "notepad", notetitle, 0, 1, getSession());
            unlink("etc/notepad");
        }
        bbslog("user","%s","�Զ�����ʱ�����");
    }
    return;
}

void showsysinfo(char * fn)
{
    FILE* fp;
    char buf[500];
    int count=1,i;
    fp=fopen(fn, "r");
    if(!fp) return;
    while(!feof(fp)) {
        if(!fgets(buf, 500, fp)) break;
        if(strstr(buf, "@systeminfo@")) count++;
    }
    fclose(fp);
    i=rand()%count;
    count=0;
    clear();
    fp=fopen(fn, "r");
    while(!feof(fp)) {
        if(!fgets(buf, 500, fp)) break;
        if(strstr(buf, "@systeminfo@")) count++;
        else {
            if(count==i) prints("%s", buf);
        }
        if(count>i) break;
    }
    fclose(fp);
}

void user_login()
{
    char fname[STRLEN];
    char ans[5];
    unsigned unLevel = PERM_SUICIDE;

    /* ?????���滹��check_register_info */
    newbbslog(BBSLOG_USIES,"ENTER @%s", getSession()->fromhost);
    u_enter();
    sprintf(genbuf, "Enter from %s", getSession()->fromhost);      /* Leeward: 97.12.02 */

    bbslog("user","%s",genbuf);
/*---	period	2000-10-19	4 debug	---*/
    newbbslog(BBSLOG_USIES,"ALLOC: [%d %d]", getSession()->utmpent, getSession()->currentuid);
/*---	---*/
    started = 1;
    if (USE_NOTEPAD == 1)
        notepad_init();
    if (strcmp(getCurrentUser()->userid, "guest") != 0 && USE_NOTEPAD == 1) {
        if (DEFINE(getCurrentUser(), DEF_NOTEPAD)) {
            int noteln;

            if (lastnote > getCurrentUser()->notedate)
                getCurrentUser()->noteline = 0;
            noteln = countln("etc/notepad");
            if ((noteln > 0) && (lastnote > getCurrentUser()->notedate || getCurrentUser()->noteline == 0)) {
                shownotepad();
                getCurrentUser()->noteline = noteln;
                write_defnotepad();
            } else if ((noteln - getCurrentUser()->noteline) > 0) {
                clear();
                ansimore2("etc/notepad", false, 0, noteln - getCurrentUser()->noteline + 1);
                prints("\033[31m�ѩء����������������������������������������������������������������������ء�\033[m\n");
                igetkey();
                getCurrentUser()->noteline = noteln;
                write_defnotepad();
                clear();
            }
        }
    }
    /* Leeward 98.09.24 Use SHARE MEM to diaplay statistic data below */
    if (DEFINE(getCurrentUser(), DEF_SHOWSTATISTIC)) {
        /*ansimore("0Announce/bbslists/countlogins", true); ȥ����ʾ��վ���� */
#ifdef BLESS_BOARD
        if (dashf("etc/posts/bless"))
            ansimore("etc/posts/bless", true);
#endif
    }
    if (vote_flag(NULL, '\0', 2 /*�������µ�Welcome û */ ) == 0) {
        if (dashf("Welcome")) {
            clear();
            ansimore("Welcome", true);
            vote_flag(NULL, 'R', 2 /*д������µ�Welcome */ );
        }
    }
    clear();
    if (DEFINE(getCurrentUser(), DEF_SHOWHOT)) {     /* Leeward 98.09.24 Use SHARE MEM and disable old code
                                                   if (DEFINE(getCurrentUser(),DEF_SHOWSTATISTIC)) {
                                                   ansimore("etc/posts/day", false);
                                                   }
                                                 */
        ansimore("etc/posts/day", false);       /* Leeward: disable old code */
    }

    move(t_lines - 2 /*1 */ , 0);       /* Leeward: 98.09.24 Alter below message */
    clrtoeol();
    prints("\033[1;36m�� �������� \033[33m%d\033[36m ����վ���ϴ����Ǵ� \033[33m%s\033[36m ������վ��\n", getCurrentUser()->numlogins + 1, getCurrentUser()->lasthost);
    prints("�� �ϴ�����ʱ��Ϊ \033[33m%s\033[m ", Ctime(getCurrentUser()->lastlogin));
    igetkey();
    /* ȫ��ʮ�����Ż��� added by Czz 020128 */
#ifdef CNBBS_TOPIC
    show_help("0Announce/bbslists/newsday");
#endif
    /* added end */
    if (dashf("0Announce/systeminfo")) {
        showsysinfo("0Announce/systeminfo");
        move(t_lines - 1 /*1 */ , 0);       /* Leeward: 98.09.24 Alter below message */
        clrtoeol();
        prints("\033[1;36m�� �����������...\033[33m\033[m ");
        igetkey();
    }
    if (dashf("0Announce/hotinfo")) {
        ansimore("0Announce/hotinfo", false);
        move(t_lines - 1 /*1 */ , 0);       /* Leeward: 98.09.24 Alter below message */
        clrtoeol();
        prints("\033[1;36m�� �����������...\033[33m\033[m ");
        igetkey();
    }
	/* Load getCurrentUser()'s mailbox properties, added by flyriver, 2003.1.5 */
	uinfo.mailbox_prop = load_mailbox_prop(getCurrentUser()->userid);
    move(t_lines - 1, 0);
    sethomefile(fname, getCurrentUser()->userid, BADLOGINFILE);
    if (ansimore(fname, false) != -1) {
        getdata(t_lines - 1, 0, "��δ�������������������¼  (m)�ʻ�����  (y)���  (n)����  [n]: ", ans, 4, DOECHO, NULL, true);
        ans[0] = toupper(ans[0]);
        if (ans[0] == 'M') {
            mail_file(getCurrentUser()->userid, fname, getCurrentUser()->userid, "������������¼", BBSPOST_MOVE, NULL);
        } else if (ans[0] == 'Y') {
            my_unlink(fname);
        }
    }

    strncpy(getCurrentUser()->lasthost, getSession()->fromhost, IPLEN);
    getCurrentUser()->lasthost[IPLEN-1] = '\0';   /* dumb mistake on my part */
   if(uinfo.invisible == true && HAS_PERM(getCurrentUser(), PERM_SYSOP)){
		clear();
		move(3,0);
		prints("����:�������n�س�,��ô��ε�¼�������ӵ�¼����,��������ϴ���վʱ��,���������\n     ���ֱ�ӻس�,��ô����ǰ����һ����\n");
        getdata(t_lines - 1, 0, "����������,��ε�¼��Ҫ���ӵ�¼������ (Y/N)? [Y] ", ans, 4, DOECHO, NULL, true);
        if (*ans != 'N' && *ans != 'n'){
		    getCurrentUser()->lastlogin = time(NULL);
		    getCurrentUser()->numlogins++;
		}
   }else{
    getCurrentUser()->lastlogin = time(NULL);
    getCurrentUser()->numlogins++;
   }
    /* etnlegend, 2006.10.22, �ҿ����ǻ���ô����վ��! */
    started=2;

#ifndef SSHBBS
   if( (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_DISS) ) && strcmp(getSession()->fromhost, "127.0.0.1")){
	   char pip[64];
	   if(check_proxy_IP(getSession()->fromhost, pip) > 0){
		   clear();
		   move(3,0);
		   prints("\033[1;31m����:��������Ȩ��,�����������ǴӴ����ַ���ʱ�վ\n     ��ע������ȵİ�ȫ\033[m\n");
		   while(1){
	           getdata(t_lines - 1, 0, "������`yes`��ʾ����֪��(����������):", ans, 4, DOECHO, NULL, true);
			   if(!strncasecmp(ans,"yes", 3)){
				   break;
			   }
		   }
	   }else{
		   clear();
		   move(3,0);
		   prints("\033[1;31m����:��������Ȩ��,������������telnet���ʱ�վ\n     �뾡��ʹ��ssh��ʽ,����ע������ȵİ�ȫ\033[m\n");
		   while(1){
	           getdata(t_lines - 1, 0, "������`y`��ʾ����֪��:", ans, 4, DOECHO, NULL, true);
			   if(ans[0]=='y' || ans[0]=='Y'){
				   break;
			   }
		   }
	   }
   }
#endif

    /* Leeward 98.06.20 adds below 3 lines */
    if ((int) getCurrentUser()->numlogins < 1)
        getCurrentUser()->numlogins = 1;
    if ((int) getCurrentUser()->numposts < 0)
        getCurrentUser()->numposts = 0;
    if ((int) getCurrentUser()->stay < 0)
        getCurrentUser()->stay = 1;
    getCurrentUser()->userlevel &= (~unLevel);       /* �ָ���ɱ��־ Luzi 98.10.10 */

    if (getCurrentUser()->firstlogin == 0) {
        getCurrentUser()->firstlogin = login_start_time - 7 * 86400;
    }
    check_register_info();
    load_mail_list(getCurrentUser(),&user_mail_list);
}

int chk_friend_book()
{
    FILE *fp;
    int idnum, n = 0;
    char buf[STRLEN], *ptr;

    move(3, 0);
    if ((fp = fopen("friendbook", "r")) == NULL)
        return n;
    prints("\033[1mϵͳѰ�������б�:\033[m\n\n");
    /*if((fp=fopen("friendbook","r"))==NULL)
       return n; Moved before "prints", Leeward 98.12.03 */
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        char uid[14];
        char msg[STRLEN];
        struct user_info *uin;

        ptr = strstr(buf, "@");
        if (ptr == NULL)
            continue;
        ptr++;
        strcpy(uid, ptr);
        ptr = strstr(uid, "\n");
        *ptr = '\0';
        idnum = atoi(buf);
        if (idnum != getSession()->currentuid || idnum <= 0)
            continue;
        uin = t_search(uid, false);
        sprintf(msg, "%s �Ѿ���վ��", getCurrentUser()->userid);
        /* ��������msg��Ŀ��uid 1998.7.5 by dong */
        strcpy(getSession()->MsgDesUid, uin ? uin->userid : "");
        idnum = 0;              /*Haohmaru.99.5.29.����һ��bug,����������������ɧ�ű��� */
        if (uin != NULL && canbemsged(uin))
            idnum = do_sendmsg(uin, msg, 2);
        if (idnum)
            prints("%s ���㣬ϵͳ�Ѿ�������(��)����վ����Ϣ��\n", uid);
        else
            prints("%s ���㣬ϵͳ�޷����絽��(��)���������(��)���硣\n", uid);
        del_from_file("friendbook", buf);
        n++;
        if (n > 15) {           /* Leeward 98.12.03 */
            pressanykey();
            move(5, 0);
            clrtobot();
        }
    }
    fclose(fp);
    return n;
}

#ifdef FB2KENDLINE
void fill_date()
{
	time_t now,next;
	struct public_data *publicshm = get_publicshm();
	char   buf[82], buf2[30], index[5], index_buf[5], *t;
	struct tm tm;
	FILE   *fp;

	now = time(0);

	if (now < publicshm->nextfreshdatetime && publicshm->date[0]!='\0')
		return;

	localtime_r(&now,&tm);
	next = now - (tm.tm_hour * 3600) - (tm.tm_min * 60) - tm.tm_sec 
		 + 86400;	/* ������� 0:0:00 ��ʱ��, Ȼ���������һ�� */
	setpublicshmreadonly(0);
	publicshm->nextfreshdatetime = next;
	setpublicshmreadonly(1);

	fp = fopen("etc/whatdate", "r");

	if (fp == NULL)
		return;

	strftime(index_buf, 5, "%m%d", &tm);

	while (fgets(buf, 80, fp)) {
		buf[80]='\0';
        t = strchr(buf,'\n');  if(t) *t='\0';
        t = strchr(buf,'\r');  if(t) *t='\0';

		if (buf[0] == ';' || buf[0] == '#' || buf[0] == ' ' || strlen(buf)<6)
			continue;

		buf[35] = '\0';
		strncpy(index,buf,4);
		index[4] = '\0';
		strcpy(buf2,buf+5);	

		if (!strcmp(index, "0000") || !strcmp(index_buf, index) ){
			buf2[29]='\0';
			if(strlen(buf2)<29){
				int i;
				for(i=strlen(buf2);i<29;i++)
					buf2[i]=' ';
				buf2[29]='\0';
			}
			setpublicshmreadonly(0);
			strcpy(publicshm->date, buf2);
			setpublicshmreadonly(1);
		}
	}

	fclose(fp);

	return;
}

#endif

void main_bbs(int convit, char *argv)
{
    char notename[STRLEN];
    int currmail;
	int summail;
	int nummail;

/* Add by KCN for avoid free_mem core dump */
    getSession()->topfriend = NULL;
    big_picture = NULL;
    user_data = NULL;
    load_sysconf();
    resolve_ucache();
    resolve_utmp();
    resolve_boards();
    init_bbslog();
    srand(time(0) + getpid());

    /* commented by period for it changed to local variable 2000.11.12
       pnt = NULL; */

    dup2(0, 1);
#ifdef BBS_INFOD
    if (strstr(argv[0], "bbsinfo") != NULL) {
        load_sysconf();
        bbsinfod_main(argc, argv);
        exit(0);
    }
#endif
    initscr();

    convcode = convit;
    conv_init(getSession());                /* KCN,99.09.05 */

    system_init();
    if (setjmp(byebye)) {
        abort_bbs(0);
    }
    login_query();
    user_login();
//    m_init();
    clear();
    load_key(NULL);

#ifdef HAVE_PERSONAL_DNS
  //��̬��������
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        struct dns_msgbuf msg;
        int msqid;
        msqid = msgget(sysconf_eval("BBSDNS_MSG", 0x999), IPC_CREAT | 0664);
        if (msqid >= 0) {
	    struct msqid_ds buf;

            msg.mtype=1;
            strncpy(msg.userid,getCurrentUser()->userid,IDLEN);
            msg.userid[IDLEN]=0;
            //ˮľ�ǿ�����fromhost�ģ�����������dns����þ�Ҫ����һ����
            strncpy(msg.ip,getSession()->fromhost,IPLEN);
            msg.ip[IPLEN]=0;
            msgctl(msqid, IPC_STAT, &buf);
	    buf.msg_qbytes = (sizeof(msg)-sizeof(msg.mtype))*20;
	    msgctl(msqid, IPC_SET, &buf);

            msgsnd(msqid, &msg, sizeof(msg)-sizeof(msg.mtype), IPC_NOWAIT | MSG_NOERROR);
        }
    }
#endif
#ifndef DEBUG
#ifdef SSHBBS
    sprintf(genbuf, "sshbbsd:%s", getCurrentUser()->userid);
#else
    sprintf(genbuf, "bbsd:%s", getCurrentUser()->userid);
#endif
	set_proc_title(argv, genbuf);
#endif

#ifdef TALK_LOG
    tBBSlog_recover();             /* 2000.9.15 Bigman ����ж�talk�Ļָ� */
#endif

    setmailfile(genbuf, getCurrentUser()->userid, DOT_DIR);
    currmail = get_num_records(genbuf, sizeof(struct fileheader));
	get_mail_limit(getCurrentUser(), &summail, &nummail);
	if (currmail > nummail)
	{
		clear();
        prints("����ż��ߴ� %d ��, ��ɾ�������ż�, ά���� %d �����£����򽫲��ܷ���\n", currmail, nummail);
		pressanykey();
	}

#ifdef FB2KENDLINE
	fill_date();
#endif

	calc_calltime(1);
	while(calltime != 0 && calltime < time(0)){
		clear();
		move(1,0);
		prints("����һ�������� %s",ctime(&calltime));
		move(3,0);
		prints("��ʾ��ϢΪ:");
		move(4,10);
		calltimememo[39]='\0';
		prints("\033[1;31m %s \033[m",calltimememo);
		prints("\n%s",ctime(&(getCurrentUser()->lastlogin)));
		move(t_lines-1,0);
		prints("                          press any key to continue...");
		refresh();
		calc_calltime(0);
		igetch();
	}

    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) && dashf("new_register"))
        prints("����ʹ�������ڵ���ͨ��ע�����ϡ�\n");

#ifdef SMS_SUPPORT
	chk_smsmsg(1, getSession());
#endif

    /*chk_friend_book(); */
    /* Leeward 98.12.03 */
    if (!uinfo.invisible && chk_friend_book()) {
        pressreturn();
    }
    clear();
    nettyNN = NNread_init();
    if (DEFINE(getCurrentUser(), DEF_INNOTE)) {
        sethomefile(notename, getCurrentUser()->userid, "notes");
        if (dashf(notename))
            ansimore(notename, true);
    }
    b_closepolls();
    num_alcounter();
    if (count_friends > 0 && DEFINE(getCurrentUser(), DEF_LOGFRIEND))
        t_friends();
    while (1) {
        if (DEFINE(getCurrentUser(), DEF_NORMALSCR))
            domenu("TOPMENU");
        else
            domenu("TOPMENU2");
        Goodbye();
    }
}

/*Add by SmallPig*/
void update_endline()
{
#ifndef FB2KENDLINE
	char buf[STRLEN];
#endif
#ifdef RECMD_INFO
	int recmd = -1;
#endif
    char stitle[256];
    time_t now;
    int allstay;
    int colour;

    if (DEFINE(getCurrentUser(), DEF_TITLECOLOR)) {
        colour = 4;
    } else {
        colour = getCurrentUser()->numlogins % 4 + 3;
        if (colour == 3)
            colour = 1;
    }
    if (!DEFINE(getCurrentUser(), DEF_ENDLINE)) {
        move(t_lines - 1, 0);
        clrtoeol();
        return;
    }
    now = time(0);
#ifdef FLOWBANNER
	allstay = (DEFINE(getCurrentUser(), DEF_SHOWBANNER)) ? (time(0) % 3) : 0;
	if (allstay) {
		if (allstay & 1) {	//��ʾϵͳ������Ϣ
			struct public_data *publicshm = get_publicshm();
			if (publicshm->bannercount) 
				snprintf(stitle, 256, "\033[%s4%dm\033[33m%s", ((DEFINE(getCurrentUser(),DEF_HIGHCOLOR)) ? "1;" : ""), colour, publicshm->banners[(time(0)>>1)%publicshm->bannercount]);
			else allstay=0;
		} else {	//��ʾ���渡����Ϣ
			if ((currboard)&&(currboard->bannercount))
				snprintf(stitle, 256, "\033[%s4%dm\033[33m%s", ((DEFINE(getCurrentUser(),DEF_HIGHCOLOR)) ? "1;" : ""), colour, currboard->banners[(time(0)>>1)%currboard->bannercount]);
			else allstay=0;
		}
	}
	if (!allstay) {
#endif
    allstay = (now - login_start_time) / 60;
#ifdef FB2KENDLINE
	{
		struct public_data *publicshm = get_publicshm();
		struct tm *tm;
		char mydatestring[12];
		char weeknum[7][3]={"��","һ","��","��","��","��","��"};

		tm = localtime(&now);
		sprintf(mydatestring,"%02d:%02d:%02d %s", tm->tm_hour,tm->tm_min,tm->tm_sec,weeknum[tm->tm_wday]);
		num_alcounter();


#ifdef LILAC
	time_t now; 
	struct tm t;
	char tmpbuf[1024];
	int tmp;
	int copied = 0;
	now = time(0);
	localtime_r(&now,&t);
	if (t.tm_mon+1==getSession()->currentmemo->ud.birthmonth && t.tm_mday==getSession()->currentmemo->ud.birthday)
	{ 
		srand(time(NULL));
		tmp = rand() % 4;
		if(tmp!=0){
			strcpy(tmpbuf, "���տ��֣�����ǵ�Ҫ���Ŷ��");
			copied = 1;
		}
	}
	if(copied ==0)
		strcpy(tmpbuf, publicshm->date);
	sprintf(stitle, "\033[1;44;33m[\033[36m%s\033[33m][\033[36m%11s\033[33m][\033[36m%4d\033[33m��/\033[1;36m%3d\033[33m��][\033[36m%.12s\033[33m]", 
		tmpbuf ,mydatestring,get_utmp_number() + getwwwguestcount(),count_friends,getCurrentUser()->userid);

#ifdef RECMD_INFO
	char info[ARTICLE_TITLE_LEN];
	srand(time(NULL));
	recmd = rand() % 6;
	if(recmd==0){
		tmp = rand() % RECMD_INFO_NUM; 
		if(get_recommend_info(tmp, info)) /* tmp is 0 ~ (RECMD_INFO_NUM-1) */
			sprintf(stitle, "\033[1;44;33m��ܰ��ʾ��[\033[36m%.60s\033[33m]\t[\033[36m%4d\033[33m��/\033[1;36m%3d\033[33m��][\033[36m%.12s\033[33m]", 
					info,get_utmp_number() + getwwwguestcount(),count_friends,getCurrentUser()->userid ); 
	}	

#endif

#else
		sprintf(stitle, "\033[1;44;33m[\033[36m%s\033[33m][\033[36m%11s\033[33m][\033[36m%4d\033[33m��/\033[1;36m%3d\033[33m��][\033[36m%.12s\033[33m]", 
			publicshm->date,mydatestring,count_users,count_friends,getCurrentUser()->userid);
#endif




	}
#else
    sprintf(buf, "[\033[36m%.12s\033[33m]", getCurrentUser()->userid);
    if (DEFINE(getCurrentUser(), DEF_NOTMSGFRIEND)) {
		if (DEFINE(getCurrentUser(),DEF_HIGHCOLOR))
        	sprintf(stitle, "\033[1;4%dm\033[33mʱ��[\033[36m%12.12s\033[33m] ������[����:%3s��һ��:%3s] ʹ����%s", colour, ctime(&now) + 4,
                (!(uinfo.pager & FRIEND_PAGER)) ? "NO " : "YES", (uinfo.pager & ALL_PAGER) ? "YES" : "NO ", buf);
		else
        	sprintf(stitle, "\033[4%dm\033[33mʱ��[\033[36m%12.12s\033[33m] ������[����:%3s��һ��:%3s] ʹ����%s", colour, ctime(&now) + 4,
                (!(uinfo.pager & FRIEND_PAGER)) ? "NO " : "YES", (uinfo.pager & ALL_PAGER) ? "YES" : "NO ", buf);
    } else {
#ifdef HAVE_FRIENDS_NUM
            num_alcounter();
            sprintf(stitle,"\033[1;4%dm\033[33mʱ��[\033[36m%12.12s\033[33m] ������/����[%3d/%3d][%c��%c] ʹ����%s",colour,
                    ctime(&now)+4,count_users,count_friends,(uinfo.pager&ALL_PAGER)?'Y':'N',(!(uinfo.pager&FRIEND_PAGER))?'N':'Y',buf);
#else
	if (DEFINE(getCurrentUser(),DEF_HIGHCOLOR))
        sprintf(stitle, "\x1b[1;4%dm\x1b[33mʱ��[\x1b[36m%12.12s\x1b[33m] ������ [ %3d ] [%c��%c] ʹ����%s", colour,
                ctime(&now) + 4, get_utmp_number() + getwwwguestcount(), (uinfo.pager & ALL_PAGER) ? 'Y' : 'N', (!(uinfo.pager & FRIEND_PAGER)) ? 'N' : 'Y', buf);
	else
        sprintf(stitle, "\x1b[4%dm\x1b[33mʱ��[\x1b[36m%12.12s\x1b[33m] ������ [ %3d ] [%c��%c] ʹ����%s", colour,
                ctime(&now) + 4, get_utmp_number() + getwwwguestcount(), (uinfo.pager & ALL_PAGER) ? 'Y' : 'N', (!(uinfo.pager & FRIEND_PAGER)) ? 'N' : 'Y', buf);
#endif //HAVE_FRIENDS_NUM
    }
#endif //FB2KENDLINE
    move(t_lines - 1, 0);
    prints("%s", stitle);
    clrtoeol();
#ifdef FB2KENDLINE
    sprintf(stitle, "[\033[36m%3d\033[33m:\033[36m%d\033[33m]\033[m", (allstay / 60) % 1000, allstay % 60);
    move(t_lines - 1, -8);
#else
    sprintf(stitle, "ͣ��[%3d:%d]", (allstay / 60) % 1000, allstay % 60);
    move(t_lines - 1, -strlen(stitle)-1);
#endif //FB2KENDLINE

#ifdef RECMD_INFO
	if(recmd!=0)
    	prints("%s", stitle);
#else
    prints("%s", stitle);
#endif

    resetcolor();
#ifdef FLOWBANNER
	} else {
    move(t_lines - 1, 0);
    prints("%s", stitle);
    clrtoeol();
    resetcolor();
	}
#endif

    /* Leeward 98.09.30 show hint for rookies */
    /* PERMs should coincide with ~bbsroot/etc/sysconf.ini: PERM_ADMENU */
    if (!DEFINE(getCurrentUser(), DEF_NORMALSCR) && MMENU == uinfo.mode && !HAS_PERM(getCurrentUser(), PERM_ACCOUNTS) 
        && !HAS_PERM(getCurrentUser(), PERM_SYSOP) && !HAS_PERM(getCurrentUser(), PERM_OBOARDS)
        && !HAS_PERM(getCurrentUser(), PERM_WELCOME) && !HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)) {
        move(t_lines - 2, 0);
        clrtoeol();
        prints("\033[1m\033[32m���Ǿ���ģʽ��ѡ����Ҫʹ��һ��ģʽ�����趨���˲����ڣ���Ϊ�ϣβ�������վ�ٽ�վ��\033[m");
    }
}


/*ReWrite by SmallPig*/
void showtitle(const char *title, const char *mid){
    int spc1;
    int colour;
    char note[STRLEN];

    if (DEFINE(getCurrentUser(), DEF_TITLECOLOR)) {
        colour = BLUE;
    } else {
        colour = getCurrentUser()->numlogins % 4 + 3;
        if (colour == YELLOW)
            colour = RED;
    }

    if(currboard)
	    sprintf(note,"������ [%s]",currboard->filename);
    else{
        currboardent=getbnum_safe(DEFAULTBOARD,getSession());
        currboard=(struct boardheader*)getboard(currboardent);
        if(!currboard){
            currboardent=0;
            strcpy(note,"Ŀǰ��δѡ��������");
        }
        else{
#ifdef HAVE_BRC_CONTROL
            brc_initial(getCurrentUser()->userid,currboard->filename,getSession());
#endif /* HAVE_BRC_CONTROL */
            sprintf(note,"������ [%s]",currboard->filename);
        }
    }

    spc1 = scr_cols/2 - 1 - strlen(mid) / 2;
    if ((strstr(title,"����")!=NULL)&&(spc1-num_noans_chr(title)<4))
	/*Ϊ�˶�����޸� Bigman:2002.9.7 */
    {
        strcpy(note , note + 7 );
        spc1 = scr_cols - 3 - strlen(mid) - strlen(note);
    }
/*    if (spc2 < 2)
        spc2 = 2;
    if (spc1 < 2) {
        spc2 -= 2 - spc1;
        spc1 = 2;
        if (spc2 < 2)
            spc2 = 2;
    }*/
/* Modified by Leeward 97/11/23 -- modification stops */
/* rewrite by bad */
    move(0, 0);
    resetcolor();
#ifdef FREE
    setfcolor(YELLOW,1);
#else
    setfcolor(YELLOW,DEFINE(getCurrentUser(),DEF_HIGHCOLOR));
#endif
    setbcolor(colour);
    clrtoeol();
    prints("%s", title);

    move(0, spc1);
    resetcolor();
    if(strcmp(mid, BBS_FULL_NAME)&&mid[0]!='[')
#ifdef FREE
	    setfcolor(CYAN,1);
#else
        setfcolor(CYAN, DEFINE(getCurrentUser(),DEF_HIGHCOLOR));
#endif
    else
#ifdef FREE
	    setfcolor(WHITE,1);
#else
        setfcolor(WHITE, DEFINE(getCurrentUser(),DEF_HIGHCOLOR));
#endif
    setbcolor(colour);
    if(mid[0]=='[') prints("\033[5m");
    prints("%s", mid);

    move(0, -strlen(note));
    resetcolor();
#ifdef FREE
    setfcolor(YELLOW,1);
#else
    setfcolor(YELLOW,DEFINE(getCurrentUser(),DEF_HIGHCOLOR));
#endif
    setbcolor(colour);
    prints("%s", note);
    resetcolor();
    move(1, 0);
}


void docmdtitle(const char *title,const char *prompt){
    char middoc[30];
    int chkmailflag = 0;
	int chksmsmsg = 0;

    chkmailflag = chkmail();
#ifdef SMS_SUPPORT
	chksmsmsg = chk_smsmsg(0, getSession());
#endif

    if (chkmailflag == 2)       /*Haohmaru.99.4.4.������Ҳ������ */
        strcpy(middoc, "[���䳬��]");
    else if (chkmailflag)
        strcpy(middoc, "[�����ż�]");
/*    else if ( vote_flag( DEFAULTBOARD, '\0' ,0) == 0&&(bp->flag&BOARD_VOTEFLAG))
        strcpy(middoc,"[ϵͳͶƱ��]");*/
    else
        strcpy(middoc, BBS_FULL_NAME);

	if( chksmsmsg ){
		if( chkmailflag ){
			strcat(middoc, "[���ж���]");
		}else{
			strcpy(middoc, "[���ж���]");
		}
	}

    showtitle(title, middoc);
	if(prompt){
    move(1, 0);
    clrtoeol();
    prints("%s", prompt);
    clrtoeol();
	}
}

/* 2000.9.15 Bigman �ָ������¼ */
#ifdef TALK_LOG

int tBBSlog_recover()
{
    char buf[256];

    sprintf(buf, "home/%c/%s/talklog", toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid);

    if (strcasecmp(getCurrentUser()->userid, "guest") == 0 || !dashf(buf))
        return;

    clear();
    strcpy(genbuf, "");

    getdata(0, 0, "\033[1;32m����һ���������������������������¼, ��Ҫ .. (M) �Ļ����� (Q) ���ˣ�[Q]��\033[m", genbuf, 4, DOECHO, NULL, true);

    if (genbuf[0] == 'M' || genbuf[0] == 'm')
        mail_file(getCurrentUser()->userid, buf, getCurrentUser()->userid, "�����¼", BBSPOST_MOVE, NULL);
    else
        my_unlink(buf);
    return;

}
#endif
