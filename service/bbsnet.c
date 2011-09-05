// NJU tinybbsnet, Preview Version, zhch@dii.nju.edu.cn, 2000.3.23 //

#include "../rzsz/zglobal.h"

#include "service.h"
#include "bbs.h"
#include "select.h"
#include "tcplib.h"
#include <netdb.h>

SMTH_API struct user_info uinfo;
SMTH_API int msg_count;

#define TIME_OUT	15
#define MAX_PROCESS_BAR_LEN 30
#define BBSNET_LOG_BOARD "bbsnet"
#define DATAFILE "etc/bbsnet.ini"

#define MAXSTATION  26*2
#define MAXSECTION 14

char host1[MAXSTATION][19], host2[MAXSTATION][40], ip[MAXSTATION][40];
char sectiontitle[MAXSECTION][9];

int port[MAXSTATION]; 
int sectionindex;
int sectioncount;
char str[]= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char section[]="1234567890!@#%&*()";

char user[21];
int sockfd;
jmp_buf jb;

// added by flyriver, 2001.3.2
// �����ռ�
// mode == 0, ��ʼ����
//         1, ��������
int bbsnet_report(char *station, char *addr, long id, int mode)
{
	struct fileheader fh;
	char buf[1024];
	char fname[256];
	time_t now;
#ifndef LILAC_PROXY
	FILE *fp;
#endif

	bzero(&fh, sizeof(fh));
	setbpath(buf, BBSNET_LOG_BOARD);
	GET_POSTFILENAME(fname, buf);
	strncpy(fh.filename, fname, sizeof(fh.filename)-1);
	fh.filename[sizeof(fh.filename)-1] = '\0';
	setbfile(fname, BBSNET_LOG_BOARD, fh.filename);
	now = time(NULL);
	strcpy(fh.owner, "deliver");
	fh.innflag[0] = 'L';
	fh.innflag[1] = 'L';
#ifndef LILAC_PROXY
	if (mode == 0)
		snprintf(fh.title, ARTICLE_TITLE_LEN, "[%ld]%s����%s", id, user, station);
	else
		snprintf(fh.title, ARTICLE_TITLE_LEN, "[%ld]%s������%s�Ĵ���", id, user, station);
	
	if ((fp = fopen(fname, "w")) == NULL)
		return -1;
	fprintf(fp, "������: deliver (�Զ�����ϵͳ), ����: %s\n", BBSNET_LOG_BOARD);
	fprintf(fp, "��  ��: %s\n", fh.title);
	fprintf(fp, "����վ: %s (%24.24s)\n\n", BBS_FULL_NAME, ctime(&now));
	fprintf(fp, "    \033[1;33m%s\033[m �� \033[1;37m%24.24s\033[m ���ñ�վbbsnet����,\n",
			user, ctime(&now));
	if (mode == 0)
	{
		fprintf(fp, "    ���� \033[1;32m%s\033[m վ, ��ַΪ\033[1;31m%s\033[m.\n",
				station, addr);
	}
	else
	{
		int t;
		int h;
		fprintf(fp, "    ������ \033[1;32m%s\033[m վ�Ĵ���, ��ַΪ\033[1;31m%s\033[m.\n",
				station, addr);
		t = now - id;
		if (t < 2400)
			sprintf(buf, "\033[1;32m%d\033[m����", t/60);
		else
		{
			h = t / 2400;
			t -= h * 2400;
			sprintf(buf, "\033[1;32m%d\033[mСʱ\033[1;32m%d\033[m����", h, t/60);
		}
		fprintf(fp, "    ���δ���һ������ %s.\n", buf);
	}
	fprintf(fp, "    ���û��� \033[1;31m%s\033[m ��¼��վ.\n", getSession()->fromhost);
	fclose(fp);
#endif
	
	return after_post(NULL, &fh, BBSNET_LOG_BOARD, NULL, 0, getSession());
}

void init_data()
{
    FILE *fp;
    char t[256], *t1, *t2, *t3, *t4;

    sectioncount=0;
    fp = fopen(DATAFILE, "r");
    if(fp== NULL) return;
    while(fgets(t, 255, fp))
	{
        t1= strtok(t, " \t");
        t2= strtok(NULL, " \t\n");
        t3= strtok(NULL, " \t\n"); 
        t4= strtok(NULL, " \t\n");
        if ((*t1=='*')&&!strcmp(t2,"[section]")) {
        	sectioncount++;
        	if (sectioncount>=MAXSECTION)
        		break;
        }
        if ((sectioncount>0)&&(*t1=='*')&&!strcmp(t2,"[title]"))
        	strncpy(sectiontitle[sectioncount-1],t3,8);
        	sectiontitle[sectioncount-1][8]=0;
    } 
    fclose(fp);
}

static bool bbsnet_redraw=true;
int load_section(struct _select_def* conf,int pos,int len)
{
    FILE *fp;
    char t[256], *t1, *t2, *t3, *t4;
    int section;

    fp = fopen(DATAFILE, "r");
    conf->item_count=0;
    if(fp== NULL) return SHOW_REFRESH;
    section=0;
    while(fgets(t, 255, fp)&& conf->item_count < MAXSTATION)
	{
        t1= strtok(t, " \t");
        t2= strtok(NULL, " \t\n");
        t3= strtok(NULL, " \t\n"); 
        t4= strtok(NULL, " \t\n");
        if ((*t1=='*')&&!strcmp(t2,"[section]")) {
        	section++;
        }
        if (section>sectionindex) break;
        if (section==sectionindex) {
            if(t1[0]== '#'|| t1[0] == '*' || t1== NULL|| t2== NULL|| t3== NULL)
    			continue;
            strncpy(host1[conf->item_count], t2, 18);
	    host1[conf->item_count][18]=0;
            strncpy(host2[conf->item_count], t1, 36);
	    host2[conf->item_count][36]=0;
            strncpy(ip[conf->item_count], t3, 36);
            port[conf->item_count]= t4? atoi(t4): 23;
            conf->item_count++;
        }
    } 
    fclose(fp);
  conf->item_per_page = conf->item_count;
  bbsnet_redraw=true;
  return SHOW_REFRESH;
}

// from Maple-hightman
// added by flyriver, 2001.3.3
int telnetopt(int fd, char* buf, int max)
{
	unsigned char c,d,e;
	int pp=0;
	unsigned char tmp[30];
	while(pp<max)
	{
		c=buf[pp++];
		if(c==255)
		{
			d=buf[pp++];
			e=buf[pp++];
			oflush();
			if((d==253)&&(e==3||e==24))
			{
				tmp[0]=255;
				tmp[1]=251;
				tmp[2]=e;
				write(fd,tmp,3);
				continue;
			}
			if((d==251||d==252)&&(e==1||e==3||e==24))
			{
				tmp[0]=255;
				tmp[1]=253;
				tmp[2]=e;
				write(fd,tmp,3);
				continue;
			}
			if(d==251||d==252)
			{
				tmp[0]=255;
				tmp[1]=254;
				tmp[2]=e;
				write(fd,tmp,3);
				continue;
			}
			if(d==253||d==254)
			{
				tmp[0]=255;
				tmp[1]=252;
				tmp[2]=e;
				write(fd,tmp,3);
				continue;
			}
			if(d==250)
			{
				while(e!=240&&pp<max)
					e=buf[pp++];
				tmp[0]=255;
				tmp[1]=250;
				tmp[2]=24;
				tmp[3]=0;
				tmp[4]=65;
				tmp[5]=78;
				tmp[6]=83;
				tmp[7]=73;
				tmp[8]=255;
				tmp[9]=240;
				write(fd,tmp,10);
			}
		}
		else
			  ochar(c);
	}
       oflush();
	return 0;
}

// �����ö��̲߳���ʵ�ֽ������ˣ�sigh
// UPDATE: ����Ҫ���̣߳�by flyriver, 2002.8.10
void bbsnet_timeout(int signo)
{
	longjmp(jb, signo);
}

static void process_bar(int n, int len)
{
	char buf[256];
	char buf2[256];
	char *ptr;
	char *ptr2;
	char *ptr3;

	move(4, 0);
	prints("����������������������������������\n");
	sprintf(buf2, "            %3d%%              ", n * 100 / len);
	ptr = buf;
	ptr2 = buf2;
	ptr3 = buf + n;
	while (ptr != ptr3)
		*ptr++ = *ptr2++;
	*ptr++ = '\x1b';
	*ptr++ = '[';
	*ptr++ = '4';
	*ptr++ = '4';
	*ptr++ = 'm';
	while (*ptr2 != '\0')
		*ptr++ = *ptr2++;
	*ptr++ = '\0';
	prints("��\033[46m%s\033[m��\n", buf);
	prints("����������������������������������\n");
	redoscr();
}

static time_t last_refresh;

static int 
bbsnet_read(int fd, char *buf, int len)
{
	int rc;
	time_t now;

	rc = raw_read(fd, buf, len);
	if (rc > 0)
	{
		now = time(NULL);
		if (now - last_refresh > 60)
		{
			uinfo.freshtime = now;
			UPDATE_UTMP(freshtime, uinfo);
			last_refresh = now;
		}
	}
	return rc;
}

int bbsnet(int n)
{
	time_t now;
	struct hostent *pHost = NULL;
	struct sockaddr_in remote;
	unsigned char buf[BUFSIZ];
	int rc;
	int rv;
	int maxfdp1;
	fd_set readset;
	struct timeval tv;
	int tos = 020; /* Low delay bit */
	int i;
	sig_t oldsig;
	int ret;

	now = time(NULL);
	clear();
	prints("\033[1;32m���ڲ����� %s (%s) �����ӣ����Ժ�... \033[m\n", 
			host1[n], ip[n]);
	prints("\033[1;32m����� %d �����޷����ϣ�������򽫷������ӡ�\033[m\n",
			TIME_OUT);
	if (setjmp(jb) == 0)
	{
		oldsig = signal(SIGALRM, bbsnet_timeout);
		alarm(TIME_OUT);
		pHost = gethostbyname(ip[n]);
		alarm(0);
        signal(SIGALRM, oldsig);
	}
	if (pHost == NULL)
	{
		prints("\033[1;31m����������ʧ�ܣ�\033[m\n");
		pressreturn();
		return -1;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		prints("\033[1;31m�޷�����socket��\033[m\n");
		pressreturn();
		return -1;
	}
	bzero(&remote, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port[n]);
	remote.sin_addr = *(struct in_addr *)pHost->h_addr_list[0];

	prints("\033[1;32m�����������ʾ����ǰ��ʹ�õ�ʱ�䡣\033[m\n");
	process_bar(0, MAX_PROCESS_BAR_LEN);
	for (i = 0; i < MAX_PROCESS_BAR_LEN; i++)
	{
		if (i == 0)
			rv = NonBlockConnectEx(sockfd, (struct sockaddr *)&remote, 
				sizeof(remote), 500, 1);
		else
			rv = NonBlockConnectEx(sockfd, (struct sockaddr *)&remote, 
				sizeof(remote), 500, 0);
		if (rv == ERR_TCPLIB_TIMEOUT)
		{
			process_bar(i+1, MAX_PROCESS_BAR_LEN);
			continue;
		}
		else if (rv == 0)
			break;
		else
		{
			prints("\033[1;31m����ʧ�ܣ�\033[m\n");
			pressreturn();
			ret = -1;
			goto on_error;
		}
	}
	if (i == MAX_PROCESS_BAR_LEN)
	{
		prints("\033[1;31m���ӳ�ʱ��\033[m\n");
		pressreturn();
		ret =  -1;
		goto on_error;
	}
	setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(int));	
	prints("\033[1;31m���ӳɹ���\033[m\n");
#ifndef LILAC_PROXY
	bbsnet_report(host1[n], ip[n], now, 0);
#endif
	clear();
	refresh();
	for (;;)
	{
		FD_ZERO(&readset);
		FD_SET(0, &readset);
		FD_SET(sockfd, &readset);
		maxfdp1 = sockfd + 1;
		tv.tv_sec = 1200;
		tv.tv_usec = 0;

		/*if ((rv = SignalSafeSelect(maxfdp1, &readset, NULL, NULL, &tv)) == -1)*/
		if ((rv = select(maxfdp1, &readset, NULL, NULL, &tv)) == -1)
		{
			if (errno == EINTR)
			{
				while (msg_count)
				{
					msg_count--;
					r_msg();
				}
				continue;
			}
			ret = -1;
			goto on_error;
		}
		if (rv == 0)
		{
			ret = -1;
			goto on_error;
		}

		if (FD_ISSET(sockfd, &readset))
		{
			if ((rc = read(sockfd, buf, BUFSIZ)) < 0)
			{
				ret = -1;
				goto on_error;
			}
			else if (rc == 0)
				break;
			else if (strchr((void*)buf, 255))	/* �����Ƿ���TELNET����IAC */
				telnetopt(sockfd, (void*)buf, rc);
			else
			{
				output((void*)buf, rc);
				oflush();
			}
		}
		if (FD_ISSET(0, &readset))
		{
			if ((rc = bbsnet_read(0, (void*)buf, BUFSIZ)) < 0)
			{
				ret = -1;
				goto on_error;
			}
			if (rc == 0)
				break;
			write(sockfd, buf, rc);
		}
	}
	bbsnet_report(host1[n], ip[n], now, 1);
	ret = 0;
on_error:
	close(sockfd);
	clear();
	redoscr();
	return ret;
}

static int bbsnet_onselect(struct _select_def *conf)
{
	bbsnet(conf->pos-1);
	bbsnet_redraw=true;
	return SHOW_REFRESH;
}

static int bbsnet_show(struct _select_def *conf, int pos)
{
    if (conf->pos==pos)
    	prints("\x1b[1;32m>%c\x1b[36m%-18s",str[pos-1],host1[pos-1]);
    else
    	prints("\x1b[1;32m %c\x1b[00m%-18s",str[pos-1],host1[pos-1]);
    if (pos==conf->item_count&&bbsnet_redraw) {
    int i;
    move(4,64);
    outs("\x1b[m�q�����������r");
    for (i=0;i<sectioncount;i++)
    {
        move(i+5,64);
        if (i+1==sectionindex) 
            prints("\x1b[m��\x1b[1;44;32m%c %-8s\x1b[m��",section[i],sectiontitle[i]);
        else
            prints("\x1b[m��\x1b[1;32m%c %-8s\x1b[m��",section[i],sectiontitle[i]);
    }
    move(i+5,64);
    outs("\x1b[m�t�����������s");
    for (i=2;i<19;i++) {
        move(i,0);
        outs("�U");
        move(i,78);
        outs("\x1b[m�U");
    }
    bbsnet_redraw=false;
    } else
    if (pos==conf->item_count) {
        move(2,78);
        outs("\x1b[m�U");
    }
    return SHOW_CONTINUE;
}

static int bbsnet_key(struct _select_def *conf, int command)
{
    char* ptr;
    if ((ptr=strchr(str,command))!=NULL) {
	  conf->new_pos = (ptr-str) + 1;
	  return SHOW_SELCHANGE;
    }
    if ((ptr=strchr(section,command))!=NULL) {
    	  if (ptr-section<sectioncount) {
    	  	sectionindex=ptr-section+1;
    	       return SHOW_DIRCHANGE;
    	  }
    }
    switch (command) {
    case ' ':
    	  sectionindex++;
    	  sectionindex=sectionindex%sectioncount;
	  if (sectionindex==0) sectionindex=sectioncount;
    	   return SHOW_DIRCHANGE;
    case Ctrl('C'):
    case Ctrl('A'):
    	   return SHOW_QUIT;
    case '?':
        show_help("help/bbsnethelp");
	bbsnet_redraw=true;
        return SHOW_REFRESH;
    case '$':
    	  conf->new_pos=conf->item_count;
    	  return SHOW_SELCHANGE;
    case '^':
    	  conf->new_pos=1;
    	  return SHOW_SELCHANGE;
    }
    return SHOW_CONTINUE;
}

int bbsnet_selchange(struct _select_def* conf,int new_pos)
{
    move(20,0);
    clrtoeol();
    if(new_pos<1)
        new_pos=conf->item_count;
    if(new_pos>conf->item_count)
        new_pos=1;
    prints("�U\x1b[1m��λ:\x1b[1;33m%-18s\x1b[m  վ��:\x1b[1;33m%s\x1b[m",
        host2[new_pos-1],
        host1[new_pos-1]);
    move(20,78);
    outs("�U");
    move(21,0);
    clrtoeol();
    prints("�U\x1b[1m����:\x1b[1;33m%-20s",ip[new_pos-1]);
    if (port[new_pos-1]!=23)
        prints("  %d",port[new_pos-1]);
    outs("\x1b[m");
    move(21,78);
    outs("�U");
    return SHOW_CONTINUE;
}

static int bbsnet_refresh(struct _select_def *conf)
{
    clear();
    move(0,0);
    prints("  �� %s ��",sectiontitle[sectionindex-1]);
    move(1,0);
    outs("�q�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�r");
    /*
    for (i=2;i<19;i++) {
    	move(i,0);
    	outs("�U");
    	move(i,78);
    	outs("�U");
    }
    */
    move(19,0);
    outs("�U�����������������������������������������������������������������������������U");
    bbsnet_selchange(conf,conf->pos);
    move(22,0);
    outs("�t�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�s");
    move(23,0);
    outs("\033[1;36m[\x1b[1;32m?\x1b[m]���� [\x1b[1;32mCtrl+C\x1b[m]�˳� [\x1b[1;32mCtrl+L\x1b[m]�ػ�өĻ [\x1b[1;32m�ո�\x1b[m]�л�Ŀ¼ [\x1b[1;32m^\x1b[m]��һ�� [\x1b[1;32m$\x1b[m]���һ��\033[0;37m\033[m");
    return 0;
}

void main_loop()
{
	int i;
	POINT pts[MAXSTATION];
       struct _select_def bbsnet_conf;

       for (i = 0; i < MAXSTATION; i++) {
           pts[i].x = 2 + ((i>=17)?20:0) + ((i>=34)?20:0) + ((i>=51)?20:0);
           pts[i].y = i + 2 -((i>=17)?17:0) - ((i>=34)?17:0) - ((i>=51)?17:0);
       };

       sectionindex=1;
        bzero(&bbsnet_conf,sizeof(bbsnet_conf));
        load_section(&bbsnet_conf,1,MAXSTATION);
        bbsnet_conf.item_per_page = bbsnet_conf.item_count;
        bbsnet_conf.flag = LF_FORCEREFRESHSEL | LF_BELL | LF_LOOP;     //|LF_HILIGHTSEL;
        bbsnet_conf.prompt = NULL;
        bbsnet_conf.item_pos = pts;
        bbsnet_conf.arg = NULL;
        bbsnet_conf.title_pos.x = 0;
        bbsnet_conf.title_pos.y = 0;
        bbsnet_conf.pos = 1;
        bbsnet_conf.page_pos = 1;

        bbsnet_conf.on_select = bbsnet_onselect;
        bbsnet_conf.show_data = bbsnet_show;
        bbsnet_conf.key_command = bbsnet_key;
        bbsnet_conf.show_title = bbsnet_refresh;
        bbsnet_conf.get_data = load_section;
        bbsnet_conf.on_selchange = bbsnet_selchange;
#ifdef LILAC_PROXY
		bbsnet_conf.pos = 1;
		bbsnet_onselect(&bbsnet_conf);
#else
	list_select_loop(&bbsnet_conf);
#endif
}

int bbsnet_main()
{
#ifndef LILAC_PROXY
	strncpy(user, getCurrentUser()->userid, 20);
	modify_user_mode(BBSNET);
#endif

    init_data();
    main_loop();
	return 0;
}
