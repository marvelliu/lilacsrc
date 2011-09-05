/******************************************************
ɱ����Ϸ2003, ����: bad@smth.org  Qian Wenjie
�޸���: znight@smth.org
��ˮľ�廪bbsϵͳ������

����Ϸ����������������⸴����ֲ
�����޸ĺ���ļ�ͷ��������Ȩ��Ϣ
******************************************************/
/*****************************************************

Modified by znight@smth, 01/14/2004
�Ķ���
(1) �ֿ�����Һ��Թ��ߣ�������1~30,�Թ���31~100��ͬʱȡ���Թ��߱�־��
(2) �����˹��ڹ���Ա�ļ������⣬����Ա�˳����������Ϸ������ת������ԱȨ�ޡ�ͬʱȡ������Ա��־��
(3) ��ֹ����˵���������С��1�룩��
(4) ˵����ɫ����#1~#6��
(5) ֻ��ѡ����ң�ѡ����ƶ����Թ��ߣ���
(6) ��Ϸ������Զ�ת����
(7) ��Ϸ�������Զ����������ɱ��������
(8) ��Ϸ�������Զ������������˳��

�Ͽ������棺
(1) ͶƱ����ȷ��(ctrl+sѡ��ctrl+tȷ��)��
(2) �绤��ָ֤ʱ��ɱ�ֿ���ctrl+s�������ٰ�һ��ȡ����
(3) ָ֤ʱ�ɸ���ָ֤���󣬽����ctrl+tʱ�Ż���ʾ��
(4) ��ʾ�Ƿ�Ͷ��Ʊ��
(5) �κ�ʱ�̣�����ɱ�ֱ�������������Ϸ��
******************************************************/

#include "service.h"
#include "bbs.h"

SMTH_API struct user_info uinfo;
SMTH_API int t_lines;

// �Ƿ��Ͽ�BBS
#undef NANKAI
#undef QNGY

#ifdef QNGY
// �Ƿ��Զ�ת��
#define AUTOPOST
#endif

#ifdef AUTOPOST
#define KILLERBOARD "Killer"
#endif // AUTOPOST

#ifndef NANKAI

#define K_HAS_PERM HAS_PERM
#define CURRENTUSER getCurrentUser()
SMTH_API int kicked;

#define DEFAULT_SIGNAL talk_request

#define k_getdata getdata

#define k_attach_shm attach_shm

#define k_mail_file mail_file

int strlen2(char *s)
{
	int p;
	int inansi;
	p=0;
	inansi=0;
	while (*s)
	{
		if (*s=='\x1b')
		{
			inansi=1;
		}
		else
		if (inansi && *s=='m')
		{
			inansi=0;
		}
		else
		if (!inansi)
		{
			p++;
		}
		s++;
	}
	return p;
}

#else // NANKAI

#ifndef BBSPOST_MOVE
#define BBSPOST_MOVE -1
#endif // POSTFILE_MOVE

#define DEFAULT_SIGNAL SIG_DFL

#define k_attach_shm attach_shm2

#define K_HAS_PERM(x,y) HAS_PERM(y)
#define CURRENTUSER (&currentuser)
extern unsigned char scr_cols;

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

int kicked;

int k_getdata(int line,int col,char * prompt,char * buf,int len,int echo,void *nouse,int clearlabel)
{
	buf[0] = '\0';
	getdata(line, col, prompt, buf, len, echo, clearlabel);
}

int k_mail_file(char *fromid, char *tmpfile, char *userid, char *title, int unlinkmode, struct fileheader *fh)
{
	mail_file(tmpfile, userid, title);
	unlink(tmpfile);
}

#endif // NANKAI

#define k_setfcolor(x,y) kf_setfcolor(disp,x,y)

void kf_setfcolor(char * disp, int i,int j)
{
	char str[20];
	if (j)
		sprintf(str,"\x1b[%d;%dm",i+30,j);
	else
		sprintf(str,"\x1b[%dm",i+30);
	strcat(disp,str);
}

#define k_resetcolor() strcat(disp,"\x1b[m")

#define MAX_ROOM 100
#define MAX_PEOPLE 100
#define MAX_PLAYER 30
#define MAX_MSG 2000

#define MAX_KILLER 10
#define MAX_POLICE 10

#define ROOM_LOCKED 01
#define ROOM_SECRET 02
#define ROOM_DENYSPEC 04
#define ROOM_SPECBLIND 010

struct room_struct {
	int w;
	int style;				  /* 0 - chat room 1 - killer room */
	char name[14];
	char title[NAMELEN];
	int op;
	unsigned int level;
	int flag;
	int numplayer;
	int maxplayer;
	int numspectator;
};

#define PEOPLE_KILLER 0x1
#define PEOPLE_POLICE 0x2

#define PEOPLE_ALIVE 0x100
#define PEOPLE_DENYSPEAK 0x200
#define PEOPLE_VOTED  0x400
#define PEOPLE_HIDEKILLER 0x800
#define PEOPLE_SURRENDER 0x1000

#define PEOPLE_TESTED 0x8000

#define SIGN_KILLER "*"
#define SIGN_POLICE "@"

struct people_struct {
	int style;
	char id[IDLEN + 2];
	char nick[NAMELEN];
	int flag;
	int pid;
	int vote;
	int vnum;
};

// 2����->3��������->4ָ֤->5�绤->6ͶƱ->7��������(��������ɱ��������)
#define INROOM_STOP 1
//����
#define INROOM_NIGHT 2
//����
#define INROOM_DAY 3
//ָ֤�׶�
#define INROOM_CHECK 4
//�绤�׶�
#define INROOM_DEFENCE 5
//ͶƱ�׶�
#define INROOM_VOTE 6
//����������
#define INROOM_DARK 7

struct inroom_struct {
	int w;
	int status;
	
	int killernum;
	int policenum;
	
	// add for nk
	int turn; // �ֵ�ĳ�˷���
	int round; // round of vote
	int victim; // be killed
	int informant; // be inspected
	
	int nokill;
	char voted[MAX_PLAYER];
	int numvoted;
	// end for nk
	struct people_struct peoples[MAX_PEOPLE];
	char msgs[MAX_MSG][100];
	int msgpid[MAX_MSG];
	int msgi;
	
	char killers[MAX_PLAYER];
	char polices[MAX_PLAYER];
	
	char seq_detect[MAX_PLAYER];
	
	char gamename[80];
};

struct room_struct *rooms;
struct inroom_struct *inrooms;

struct killer_record {	
	int w;					  //0 - ƽ��ʤ�� 1 - ɱ��ʤ��
	time_t t;
	int peoplet;
	char id[MAX_PLAYER][IDLEN + 2];
	int st[MAX_PLAYER];		 // 0 - ����ƽ�� 1 - ����ƽ�� 2 - ����ɱ�� 3 - ����ɱ�� 4 - �������
};

int myroom, mypos;

#define RINFO inrooms[myroom]
#define PINFO(x) RINFO.peoples[x]

time_t lasttalktime;
int denytalk;

void player_drop(int d);
void kill_msg(int u);

#ifdef AUTOPOST

char * ap_tmpname()
{
	static char fn[80];
	sprintf(fn,"tmp/killer%d.tmp",myroom);
	return fn;
}

#endif // AUTOPOST

void safe_substr(char * d, const char * s ,int n)
{
	int i;
	for(i=0;*s && i<n ;i++,d++,s++)
	{
		*d=*s;
		if (*s&0x80)
		{
			d++;
			s++;
			i++;
			*d=*s;
		}
	}
	if (i>n)
		*(d-2)=0;
	else
		*d=0;
}

void save_result(int w)
{
	int fd;
	struct flock ldata;
	struct killer_record r;
	int i, j;
	char filename[80] = "service/.KILLERRESULT";

#ifdef AUTOPOST
	post_file(CURRENTUSER,"",ap_tmpname(),KILLERBOARD,RINFO.gamename,0,1,getSession());
	unlink(ap_tmpname());
#endif // AUTOPOST

	memset(&r,0,sizeof(r));
	r.t = time(0);
	r.w = w;
	r.peoplet = 0;
	j = 0;
	for (i = 0; i < MAX_PLAYER; i++)
		if (PINFO(i).style != -1)
		{
			strcpy(r.id[j], PINFO(i).id);
			r.st[j] = 4;
			if (!(PINFO(i).flag & PEOPLE_KILLER))
			{
				if (PINFO(i).flag & PEOPLE_ALIVE)
					r.st[j] = 0;
				else
					r.st[j] = 1;
			}
			else
			{
				if (PINFO(i).flag & PEOPLE_ALIVE)
					r.st[j] = 2;
				else
					r.st[j] = 3;
			}

			j++;
			r.peoplet++;
		}
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			lseek(fd, 0, SEEK_END);
			write(fd, &r, sizeof(struct killer_record));
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

/*
void load_msgs()
{
	FILE* fp;
	int i;
	char filename[80], buf[80];
	msgst=0;
	sprintf(filename, "home/%c/%s/.INROOMMSG%d", toupper(CURRENTUSER->userid[0]), CURRENTUSER->userid, uinfo.pid);
	fp = fopen(filename, "r");
	if(fp) {
		while(!feof(fp))
		{
			if(fgets(buf, 79, fp)==NULL)
				break;
			if(buf[0])
			{
				if(!strncmp(buf, "�㱻����", 8)) kicked=1;
				if(msgst==200)
				{
					msgst--;
					for(i=0;i<msgst;i++)
						strcpy(msgs[i],msgs[i+1]);
				}
				strcpy(msgs[msgst],buf);
				msgst++;
			}
		}
		fclose(fp);
	}
}
*/

void start_change_inroom()
{
	if (RINFO.w)
		sleep(0);
	RINFO.w = 1;
}

void end_change_inroom()
{
	RINFO.w = 0;
}

struct action {
	char *verb;				 /* ���� */
	char *part1_msg;			/* ��� */
	char *part2_msg;			/* ���� */
};

struct action party_data[] = {
	{"?", "���ɻ�Ŀ���", ""},
	{"admire", "��", "�ľ���֮���������Ͻ�ˮ���಻��"},
	{"agree", "��ȫͬ��", "�Ŀ���"},
	{"bearhug", "�����ӵ��", ""},
	{"bless", "ף��", "�����³�"},
	{"bow", "�Ϲ��Ͼ�����", "�Ϲ�"},
	{"bye", "����", "�ı�Ӱ����Ȼ���¡����������������������ĸ���:\n\"\x1b[31m���վ�����.....\x1b[m\""},
	{"caress", "����ĸ���", ""},
	{"cat", "��ֻСè���������", "�Ļ���������"},
	{"cringe", "��", "������ϥ��ҡβ����"},
	{"cry", "��", "�������"},
	{"comfort", "���԰�ο", ""},
	{"clap", "��", "���ҹ���"},
	{"dance", "����", "������������"},
	{"dogleg", "��", "����"},
	{"drivel", "����", "����ˮ"},
	{"dunno", "�ɴ��۾���������ʣ�", "����˵ʲ���Ҳ���Ү... :("},
	{"faint", "�ε���", "�Ļ���"},
	{"fear", "��", "¶�����µı���"},
	{"fool", "����ע��", "�����׳�....\n������������....�˼����Ļ....\n����̫��ϧ�ˣ�"},
	{"forgive", "��ȵĶ�", "˵�����ˣ�ԭ������"},
	{"giggle", "����", "ɵɵ�Ĵ�Ц"},
	{"grin", "��", "¶��а���Ц��"},
	{"growl", "��", "��������"},
	{"hand", "��", "����"},
	{"hammer", "����ô�ô�����������ۣ���",
	 "ͷ������һ�ã�\n***************\n*  5000000 Pt *\n***************\n	  | |	  �������\n	  | |		 �ö������Ӵ\n	  |_|"},
	{"heng", "��������", "һ�ۣ� ����һ�����߸ߵİ�ͷ��������,��мһ�˵�����..."},
	{"hug", "�����ӵ��", ""},
	{"idiot", "����س�Ц", "�ĳմ���"},
	{"kick", "��", "�ߵ���ȥ����"},
	{"kiss", "����", "������"},
	{"laugh", "������Ц", ""},
	{"lovelook", "����", "���֣������ĬĬ���ӡ�Ŀ�����к���ǧ�����飬�������"},
	{"nod", "��", "��ͷ����"},
	{"nudge", "�����ⶥ", "�ķʶ���"},
	{"oh", "��", "˵����Ŷ�����Ӱ�����"},
	{"pad", "����", "�ļ��"},
	{"papaya", "������", "��ľ���Դ�"},
	{"pat", "��������", "��ͷ"},
	{"pinch", "�����İ�", "š�ĺ���"},
	{"puke", "����", "�°��°�����˵�¶༸�ξ�ϰ����"},
	{"punch", "�ݺ�����", "һ��"},
	{"pure", "��", "¶�������Ц��"},
	{"qmarry", "��", "�¸ҵĹ�������:\n\"��Ը��޸�����\"\n---���������ɼΰ�"},
	{"report", "͵͵�ض�", "˵���������Һ��𣿡�"},
	{"shrug", "���ε���", "�����ʼ��"},
	{"sigh", "��", "̾��һ����"},
	{"slap", "žž�İ���", "һ�ٶ���"},
	{"smooch", "ӵ����", ""},
	{"snicker", "�ٺٺ�..�Ķ�", "��Ц"},
	{"sniff", "��", "��֮�Ա�"},
	{"sorry", "ʹ�����������", "ԭ��"},
	{"spank", "�ð��ƴ�", "���β�"},
	{"squeeze", "������ӵ����", ""},
	{"thank", "��", "��л"},
	{"tickle", "��ߴ!��ߴ!ɦ", "����"},
	{"waiting", "����ض�", "˵��ÿ��ÿ�µ�ÿһ�죬ÿ��ÿ���Ҷ������������"},
	{"wake", "Ŭ����ҡҡ", "��������ߴ�У��������ѣ��������ģ���"},
	{"wave", "����", "ƴ����ҡ��"},
	{"welcome", "���һ�ӭ", "�ĵ���"},
	{"wink", "��", "���ص�գգ�۾�"},
	{"xixi", "�����ض�", "Ц�˼���"},
	{"zap", "��", "���Ĺ���"},
	{"inn", "˫�۱�������ˮ���޹�������", ""},
	{"mm", "ɫ���еĶ�", "�ʺã�����ü�á�������������ɫ�ǰ�������"},
	{"disapp", "����û��ͷ����Ϊʲô", "����������������ȫû��Ӧ��û�취��"},
	{"miss", "��ϵ�����", "�����������������������̫--��������!���಻����?"},
	{"buypig", "ָ��", "���������ͷ������һ�룬лл����"},
	{"rascal", "��", "��У������������å����������������������������������"},
	{"qifu", "С��һ�⣬��", "�޵��������۸��ң����۸��ң�������"},
	{"wa", "��", "���һ�����������������ۿ����Ү������������������������������"},
	{"feibang", "ร���������죬", "�������ҽ�������һ�����Ը���ٰ�������"},
	{"badman", "ָ��", "���Դ�, �����㵸�ĺ�:�� ɱ~~��~~��~~��~~��~~~��"},
	{NULL, NULL, NULL}
};

struct action speak_data[] = {
	{"ask", "ѯ��", NULL},
	{"chant", "����", NULL},
	{"cheer", "�Ȳ�", NULL},
	{"chuckle", "��Ц", NULL},
	{"curse", "����", NULL},
	{"demand", "Ҫ��", NULL},
	{"frown", "��ü", NULL},
	{"groan", "����", NULL},
	{"grumble", "����ɧ", NULL},
	{"hum", "������", NULL},
	{"moan", "��̾", NULL},
	{"notice", "ע��", NULL},
	{"order", "����", NULL},
	{"ponder", "��˼", NULL},
	{"pout", "������˵", NULL},
	{"pray", "��", NULL},
	{"request", "����", NULL},
	{"shout", "���", NULL},
	{"sing", "����", NULL},
	{"smile", "΢Ц", NULL},
	{"smirk", "��Ц", NULL},
	{"swear", "����", NULL},
	{"tease", "��Ц", NULL},
	{"whimper", "���ʵ�˵", NULL},
	{"yawn", "��Ƿ����", NULL},
	{"yell", "��", NULL},
	{NULL, NULL, NULL}
};

struct action condition_data[] = {
	{":D", "�ֵĺϲ�£��", NULL},
	{":)", "�ֵĺϲ�£��", NULL},
	{":P", "�ֵĺϲ�£��", NULL},
	{":(", "�ֵĺϲ�£��", NULL},
	{"applaud", "žžžžžžž....", NULL},
	{"blush", "��������", NULL},
	{"cough", "���˼���", NULL},
	{"faint", "�۵�һ�����ε��ڵ�", NULL},
	{"happy", "������¶�����Ҹ��ı��飬��ѧ�Ա��˵���ߺ�������", NULL},
	{"lonely", "һ�������ڷ��������������ϣ��˭�����㡣������", NULL},
	{"luck", "�ۣ���������", NULL},
	{"puke", "����ģ������˶�����", NULL},
	{"shake", "ҡ��ҡͷ", NULL},
	{"sleep", "Zzzzzzzzzz�������ģ�����˯����", NULL},
	{"so", "�ͽ���!!", NULL},
	{"strut", "��ҡ��ڵ���", NULL},
	{"tongue", "��������ͷ", NULL},
	{"think", "����ͷ����һ��", NULL},
	{"wawl", "���춯�صĿ�", NULL},
	{"goodman", "�ü����޹��ı��鿴�Ŵ��: ��������Ǻ���Ү~~��", NULL},
	{NULL, NULL, NULL}
};

void send_msg(int u, char *msg)
{
	int i, j, k, f;
	int maxi;
	char buf[200], buf2[200], buf3[80];

	if (strlen(msg)>199)
		msg[199]=0;

	strcpy(buf, msg);

	for (i = 0; i <= 6; i++)
	{
		buf3[0] = '%';
		buf3[1] = i + 48;
		buf3[2] = 0;
		while (strstr(buf, buf3) != NULL)
		{
			strcpy(buf2, buf);
			k = strstr(buf, buf3) - buf;
			buf2[k] = 0;
			k += 2;
			sprintf(buf, "%s\x1b[3%dm%s", buf2, (i > 0) ? i : 7, buf2 + k);
		}
	}
	for (i = 0; i <= 6; i++)
	{
		buf3[0] = '#';
		buf3[1] = i + 48;
		buf3[2] = 0;
		while (strstr(buf, buf3) != NULL)
		{
			strcpy(buf2, buf);
			k = strstr(buf, buf3) - buf;
			buf2[k] = 0;
			k += 2;
			sprintf(buf, "%s\x1b[3%d;1m%s", buf2, (i > 0) ? i : 7, buf2 + k);
		}
	}

	while (strchr(buf, '\n') != NULL)
	{
		i = strchr(buf, '\n') - buf;
		buf[i] = 0;
		send_msg(u, buf);
		strcpy(buf2, buf + i + 1);
		strcpy(buf, buf2);
	}
 
	j = 0;
	f = 0;
	for (i = 0; i < strlen(buf); i++)
	{
		if (buf[i] == '\x1b')
			f = 1;
		if (f)
		{
			if (isalpha(buf[i]))
				f = 0;
			continue;
		}
		if (j >= 54 || i>=96)
			break;
		j++;
		if (buf[i]&0x80)
		{
			i++;
			j++;
		}
	}
	maxi=i;
	if (maxi < strlen(buf) && maxi != 0)
	{
		buf2[0]=' ';
		strcpy(buf2+1,buf+maxi);
		buf[maxi] = 0;
	}
	else
		buf2[0]=0;
		   
	j = MAX_MSG;
	if (RINFO.msgs[(MAX_MSG - 1 + RINFO.msgi) % MAX_MSG][0] == 0)
		for (i = 0; i < MAX_MSG; i++)
			if (RINFO.msgs[(i + RINFO.msgi) % MAX_MSG][0] == 0)
			{
				j = (i + RINFO.msgi) % MAX_MSG;
				break;
			}
	if (j == MAX_MSG)
	{
		strcpy(RINFO.msgs[RINFO.msgi], buf);
		if (u == -1)
			RINFO.msgpid[RINFO.msgi] = -1;
		else
			RINFO.msgpid[RINFO.msgi] = PINFO(u).pid;
		RINFO.msgi = (RINFO.msgi + 1) % MAX_MSG;
	}
	else
	{
		strcpy(RINFO.msgs[j], buf);
		if (u == -1)
			RINFO.msgpid[j] = u;
		else
			RINFO.msgpid[j] = PINFO(u).pid;
	}
	
#ifdef AUTOPOST
	if (u==-1)
	{
		struct flock ldata;
		int fd;
		fd = open(ap_tmpname(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd>=0)
		{
			ldata.l_type = F_WRLCK;
			ldata.l_whence = 0;
			ldata.l_len = 0;
			ldata.l_start = 0;
			if (fcntl(fd, F_SETLKW, &ldata) != -1)
			{
				lseek(fd, 0, SEEK_END);
				write(fd, buf, strlen(buf));
				write(fd,"\n",1);
				ldata.l_type = F_UNLCK;
				fcntl(fd, F_SETLKW, &ldata);
			}
			close(fd);			
		}
	}
#endif	// AUTOPOST

	if (buf2[0])
		send_msg(u,buf2);
}

void check_users(int u)
{
	int i,j,k;
	char buf[80];
	start_change_inroom();
	for (i = 0; i < MAX_PEOPLE; i++)
		if (PINFO(i).style != -1)
		{
			if (u==-1)
				j=kill(PINFO(i).pid,SIGUSR1);
			else
				j=kill(PINFO(i).pid,0);
			if (j == -1)
			{
				sprintf(buf, "\x1b[34;1m%d %s������\x1b[m",i+1,PINFO(i).nick);
				send_msg(-1,buf);
				player_drop (i); 
		 
				k = -1;
			}
		}
	if (rooms[myroom].op>=0 && PINFO(rooms[myroom].op).style==-1)
	{
		for (k = 0; k < MAX_PEOPLE; k++)
			if (PINFO(k).style != -1)
			{
				rooms[myroom].op=k;
				sprintf(buf, "\x1b[34;1m%d %s��Ϊ�·���\x1b[m", k+1,PINFO(k).nick);
				send_msg(-1, buf);
				kill_msg(-1);
				break;
			}
		if (k>=MAX_PEOPLE)
		{
			rooms[myroom].op=-1;
			rooms[myroom].style = -1;
			send_msg(-1, "�����ɢ��");
			kill_msg(-1);
		}
	}

	end_change_inroom();
}

void kill_msg(int u)
{
	check_users(u);
	if (u!=-1)
	{
		if (PINFO(u).style!=-1)
			kill(PINFO(u).pid,SIGUSR1);
	}
}

int add_room(struct room_struct *r)
{
	int i, j;

	for (i = 0; i < MAX_ROOM; i++)
		if (rooms[i].style == 1)
		{
			if (!strcmp(rooms[i].name, r->name))
				return -1;
			if (rooms[i].op!=-1 
				&& !strcmp(inrooms[i].peoples[rooms[i].op].id, CURRENTUSER->userid))
				return -1;
		}
	for (i = 0; i < MAX_ROOM; i++)
		if (rooms[i].style == -1)
		{
			memcpy(rooms + i, r, sizeof(struct room_struct));
			inrooms[i].status = INROOM_STOP;
			inrooms[i].killernum = 0;
			inrooms[i].msgi = 0;
			inrooms[i].policenum = 0;
			inrooms[i].w = 0;
			for (j = 0; j < MAX_MSG; j++)
				inrooms[i].msgs[j][0] = 0;
			for (j = 0; j < MAX_PEOPLE; j++)
				inrooms[i].peoples[j].style = -1;
			return 0;
		}
	return -1;
}

/*
int del_room(struct room_struct * r)
{
	int i, j;
	for(i=0;i<*roomst;i++)
	if(!strcmp(rooms[i].name, r->name))
	{
		rooms[i].name[0]=0;
		break;
	}
	return 0;
}
*/

void clear_room()
{
	int i;

	for (i = 0; i < MAX_ROOM; i++)
		if ((rooms[i].style != -1) && (rooms[i].numplayer + rooms[i].numspectator == 0))
			rooms[i].style = -1;
}

int can_see(struct room_struct *r)
{
	if (r->style == -1)
		return 0;
	if ((r->level & CURRENTUSER->userlevel) != r->level)
		return 0;
	if (r->style != 1)
		return 0;
	if (r->flag & ROOM_SECRET && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
		return 0;
	return 1;
}

int can_enter(struct room_struct *r)
{
	if (r->style == -1)
		return 0;
	if ((r->level & CURRENTUSER->userlevel) != r->level)
		return 0;
	if (r->style != 1)
		return 0;
	if ((r->flag & ROOM_LOCKED) && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
		return 0;
	return 1;
}

int room_count()
{
	int i, j = 0;

	for (i = 0; i < MAX_ROOM; i++)
		if (can_see(rooms + i))
			j++;
	return j;
}

int room_get(int w)
{
	int i, j = 0;

	for (i = 0; i < MAX_ROOM; i++) {
		if (can_see(rooms + i))
		{
			if (w == j)
				return i;
			j++;
		}
	}
	return -1;
}

int find_room(char *s)
{
	int i;
	struct room_struct *r2;

	for (i = 0; i < MAX_ROOM; i++)
	{
		r2 = rooms + i;
		if (!can_enter(r2))
			continue;
		if (!strcmp(r2->name, s))
			return i;
	}
	return -1;
}

int selected = 0, ipage = 0, jpage = 0;

int getpeople(int i)
{
	int j, k = 0;

	for (j = 0; j < MAX_PEOPLE; j++)
	{
		if (PINFO(j).style == -1)
			continue;
		if (i == k)
			return j;
		k++;
	}
	return -1;
}

int get_msgt()
{
	int i, j = 0, k;

	for (i = 0; i < MAX_MSG; i++)
	{
		if (RINFO.msgs[(i + RINFO.msgi) % MAX_MSG][0] == 0)
			break;
		k = RINFO.msgpid[(i + RINFO.msgi) % MAX_MSG];
		if (k == -1 || k == uinfo.pid)
			j++;
	}
	return j;
}

char *get_msgs(int s)
{
	int i, j = 0, k;
	char *ss;

	for (i = 0; i < MAX_MSG; i++) 
	{
		if (RINFO.msgs[(i + RINFO.msgi) % MAX_MSG][0] == 0)
			break;
		k = RINFO.msgpid[(i + RINFO.msgi) % MAX_MSG];
		if (k == -1 || k == uinfo.pid)
		{
			if (j == s)
			{
				ss = RINFO.msgs[(i + RINFO.msgi) % MAX_MSG];
				return ss;
			}
			j++;
		}
	}
	return NULL;
}

void save_msgs(char *s)
{
	FILE *fp;
	int i;

	fp = fopen(s, "w");
	if (fp == NULL)
		return;
	for (i = 0; i < get_msgt(); i++)
		fprintf(fp, "%s\n", get_msgs(i));
	fclose(fp);
}

void refreshit()
{
	int i, j, me, msgst,col;
	char buf[30], buf3[30], *ss, disp[500];
	static int inrefresh=0;
	
	if (inrefresh)
		return;
		
	inrefresh=1;
 
	for (i = 0; i < t_lines - 1; i++)
	{
		move(i, 0);
		clrtoeol();
	}
	switch(RINFO.status)
	{
	case INROOM_STOP:
		ss="δ";
		break;
	case INROOM_NIGHT:
		ss="ҹ";
		break;
	case INROOM_DAY:
		ss="��";
		break;
	case INROOM_CHECK:
		ss="ָ";
		break;
	case INROOM_DEFENCE:
		ss="��";
		break;
	case INROOM_VOTE:
		ss="Ͷ";
		break;
	case INROOM_DARK:
		ss="��";
		break;
    default:
        ss="";
        break;
	}
	move(0, 0);
	prints("\x1b[44;33;1m ����:\x1b[36m%-12s\x1b[33m����:\x1b[36m%-40s\x1b[33m״̬:\x1b[36m%2s \x1b[33mOP:\x1b[36m%2d",
		   rooms[myroom].name,
		   rooms[myroom].title,
		   ss,
		   rooms[myroom].op+1);
	clrtoeol();
	move(1, 0);
	prints("\x1b[m\x1b[33;1m�q\x1b[32m���\x1b[33m�����������r�q\x1b[32mѶϢ\x1b[33m�������������������������������������������������������r");
	move(t_lines - 2, 0);
	prints("\x1b[33;1m�t���������������s�t�����������������������������������������������������������s");
/*	for (i = 2; i <= t_lines - 3; i++)
	{
		move(i, 0);
		prints("��");
		move(i, 16);
		prints("��");
		move(i, 18);
		prints("��");
		move(i, 78);
		prints("��");
	}
*/
	msgst = get_msgt();
	me = mypos;
	for (i = 2; i <= t_lines - 3; i++)
	{
		col=2;
		strcpy(disp,"\x1b[m\x1b[33;1m��");
		if (ipage + i - 2 >= 0 && ipage + i - 2 < rooms[myroom].numplayer+rooms[myroom].numspectator)
		{
			j = getpeople(ipage + i - 2);
			if (j == -1)
			{
				while (col<16)
				{
		  			strcat(disp," ");
		  			col++;
		  		}
			}
			else
			if (j>=MAX_PLAYER)
			{
				k_resetcolor();
				move(i, 2);
				k_setfcolor(GREEN, 0);
				strcat(disp,"O");
				col++;
			}
			else
			if (RINFO.status != INROOM_STOP)
			{
				if ((PINFO(j).flag & PEOPLE_KILLER)
					&& ((me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_KILLER)) // ������ң�����Ҳ��ɱ��
						|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND)) // �����Թ��ߣ�����ֹ�Թۿ���ɱ�־���
						|| !(PINFO(j).flag & PEOPLE_ALIVE))) // ���˵�ɱ��
				{
					if (!(PINFO(j).flag & PEOPLE_HIDEKILLER)
						|| (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_KILLER)))
					{
						k_setfcolor(RED, 1);
						strcat(disp,SIGN_KILLER);
						col++;
					}
				}
				if ((PINFO(j).flag & PEOPLE_POLICE)
					&& ((me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_POLICE))  // ������ң�����Ҳ�Ǿ���
					|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND)))) // �����Թ��ߣ�����ֹ�Թۿ���ɱ�־���
				{
					k_setfcolor(CYAN, 1);
					strcat(disp,SIGN_POLICE);
					col++;
				}
				while (col<3)
				{
					strcat(disp," ");
					col++;
				}
				if (!(PINFO(j).flag & PEOPLE_ALIVE))
				{
					k_setfcolor(BLUE, 1);
					strcat(disp,"X");
					col++;
				}
				if ((RINFO.status==INROOM_VOTE)
					&& (PINFO(j).flag & PEOPLE_ALIVE)
					&& (PINFO(j).vote != -1))
				{
					k_setfcolor(YELLOW, 0);
					strcat(disp,"v");
					col++;
				}
			}
			while (col<4)
			{
				strcat(disp," ");
				col++;
			}
			if (ipage + i - 2 == selected)
				k_setfcolor(RED, 1);
			else
				k_resetcolor();

			sprintf(buf,"%2d",(j+1)%100);
			strcat(disp,buf);
			col+=2;

			if ((RINFO.status == INROOM_CHECK || RINFO.status == INROOM_DEFENCE || RINFO.status == INROOM_VOTE)
				&& j<MAX_PLAYER
				&& (PINFO(j).flag & PEOPLE_ALIVE)
				&& (PINFO(j).flag & PEOPLE_VOTED))
			{
				k_setfcolor(CYAN, 1);
				strcat(disp,"$");
				col++;
			}
			else
			{
				strcat(disp," ");
				col++;
			}
			
			if (ipage + i - 2 == selected)
				k_setfcolor(RED, 1);
			else
				k_resetcolor();
			safe_substr(buf3,PINFO(j).nick,9);
			sprintf(buf, "%-9.9s",buf3);
			strcat(disp,buf);
			col+=9;
		}
		while (col<16)
		{
			strcat(disp," ");
			col++;
		}
		strcat(disp, "\x1b[1;33m����\x1b[m");
		col+=4;
		if (msgst - 1 - (t_lines - 3 - i) - jpage >= 0)
		{
			char *ss = get_msgs(msgst - 1 - (t_lines - 3 - i) - jpage);

			if (ss)
			{
				if (!strcmp(ss, "�㱻����"))
					kicked = 1;
				strcat(disp, ss);
				col+=strlen2(ss);
			}
		}
		while (col<78)
		{
			strcat(disp," ");
			col++;
		}
		strcat(disp, "\x1b[m\x1b[1;33m��\x1b[m");
		move(i, 0);
		prints("%s",disp);
	}
	inrefresh=0;
}

SMTH_API int RMSG;

void room_refresh(int signo)
{
	int y, x;

	signal(SIGUSR1, room_refresh);

	if (RMSG)
		return;
	if (rooms[myroom].style != 1)
		kicked = 1;

	getyx(&y, &x);
	refreshit();
	move(y, x);
	refresh();
}

void show_killerpolice()
{
	char buf[200],buf2[20];
	int i;
	strcpy(buf,"\x1b[34;1mɱ�֣�\x1b[m");
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (RINFO.killers[i]==-1)
			break;
		if (i!=0)
			strcat(buf,", ");
		sprintf(buf2,"%d",RINFO.killers[i]+1);
		strcat(buf,buf2);
	}
	send_msg(-1,buf);
	
	if (RINFO.policenum>0)
	{
		strcpy(buf,"\x1b[34;1m���죺\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
		{
			if (RINFO.polices[i]==-1)
				break;
			if (i!=0)
				strcat(buf,", ");
			sprintf(buf2,"%d",RINFO.polices[i]+1);
			strcat(buf,buf2);
		}
		send_msg(-1,buf);
		
		strcpy(buf,"\x1b[34;1m����˳��\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
		{
			if (RINFO.seq_detect[i]==-1)
				break;
			if (i!=0)
				strcat(buf,", ");
			sprintf(buf2,"%d",RINFO.seq_detect[i]+1);
			strcat(buf,buf2);
		}
		send_msg(-1,buf);
	}
}

int check_win()
{
	int tc,tk,tp;
	char buf[200];
	int i;
	tc=0;
	tk=0;
	tp=0;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE))
		{
			if (PINFO(i).flag & PEOPLE_KILLER)
			{
				if (!(PINFO(i).flag & PEOPLE_SURRENDER))
					tk++;
			}
			else
				if (PINFO(i).flag & PEOPLE_POLICE)
					tp++;
				else
					tc++;
	}
	if (tk==0)
	{
		// ����ʤ��
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m����ɱ�ֶ��������ˣ����˻����ʤ��...\33[m");
		show_killerpolice();
		save_result (0);
		return 1;
	}
	else if (tp==0 || tc==0 || tk>=tp+tc)
	{
		// ����ʤ��
		RINFO.status=INROOM_STOP;
		if (tp==0)
			send_msg (-1, "\33[31;1mӢ�µľ���ȫ�����������˻����ʤ��...\33[m");
		else
		if (tc==0)
			send_msg (-1, "\33[31;1m�޹���ƽ��ȫ�����������˻����ʤ��...\33[m");
		else
			send_msg (-1, "\33[31;1m����û�л��˶࣬���˻����ʤ��...\33[m");
		for (i = 0; i < MAX_PLAYER; ++i)
			if (PINFO(i).style != -1
				&& (PINFO(i).flag & PEOPLE_KILLER)
				&& (PINFO(i).flag & PEOPLE_ALIVE))
			{
				sprintf (buf, "ԭ��\33[32;1m %d %s \33[m��ɱ��!", i + 1, PINFO(i).nick);
				send_msg (-1, buf);
		}
		show_killerpolice();
		save_result (1);
		return 2;
	}
	return 0;
}

void goto_night()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1mҹĻ������...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1
				&& (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
					if (RINFO.nokill)
						send_msg(i,"����ļ⵶(\x1b[31;1mCtrl+S\x1b[m)ѡ��Ҫ�Ż�����,\x1b[31;1mCtrl+T\x1b[mȷ��");
					else
						send_msg(i,"����ļ⵶(\x1b[31;1mCtrl+S\x1b[m)ѡ��Ҫ�к�����,\x1b[31;1mCtrl+T\x1b[mȷ��");
				else
					if (PINFO(i).flag & PEOPLE_POLICE)
						send_msg(i,"����ľ���(\x1b[31;1mCtrl+S\x1b[m)�鿴�㻳�ɵ���,\x1b[31;1mCtrl+T\x1b[mȷ��");
			}
 
		RINFO.victim=-1;
		RINFO.turn=-1;
		RINFO.round=-1;
		RINFO.informant=-1;
		for(i=0;i<MAX_PLAYER;i++)
		{
			PINFO(i).vote=-1;
			PINFO(i).flag &=~PEOPLE_VOTED;
			RINFO.voted[i]=-1;
		}
		RINFO.status=INROOM_NIGHT; 
	}
}

void goto_check()
{
	char buf[200];
	int k;
	k=0;
	if (!check_win())
	{
		if (RINFO.nokill)
		{
			RINFO.nokill=0;
			RINFO.turn=RINFO.victim;
		}
		else
		{
			RINFO.turn=RINFO.victim+1;
		}
		while (PINFO(RINFO.turn).style==-1 
			|| !(PINFO(RINFO.turn).flag & PEOPLE_ALIVE) )
		{
			RINFO.turn++;
			if (RINFO.turn>=MAX_PLAYER)
				RINFO.turn=0;
			if (RINFO.turn==RINFO.victim)
			{
				k=1;
				break;
			}
		}
		if (!k)
		{
			send_msg (-1, "\33[31;1m���ڿ�ʼָ֤\33[m");
			sprintf (buf,"����\33[32;1m %d %s \33[m����", RINFO.turn + 1, PINFO(RINFO.turn).nick);
			send_msg (-1, buf);
			send_msg (RINFO.turn, "��\x1b[31;1m�����ƶ����\x1b[m���������ɵĶ�����");
			send_msg (RINFO.turn, "��\33[31;1mCtrl+S\33[mָ֤");
			send_msg (RINFO.turn, "ָ֤�����\33[31;1mCtrl+T\33[m��������");
			if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
				send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
			RINFO.round=0;
			RINFO.status=INROOM_CHECK;
		}
		else
		{
			send_msg(-1,"\33[31;1mû��ָ֤���ˣ�ȡ��ָ֤���绤��ͶƱ\33[m");
			goto_night();
		}
	}
}
	
void goto_dark()
{
	char buf[200];
	if (RINFO.victim>=MAX_PLAYER || RINFO.victim<0)
	{
		sprintf(buf,"\33[31;1mû���о�,û������\33[m");
		send_msg(-1,buf);
		goto_night();
	}
	else if (PINFO(RINFO.victim).flag & PEOPLE_KILLER)
	{
		sprintf(buf,"\33[31;1m%d %s �����ٴ����ˣ�û������\33[m",RINFO.victim+1,PINFO(RINFO.victim).nick);
		PINFO(RINFO.victim).flag&=~PEOPLE_ALIVE;
		send_msg(-1,buf);
		goto_night();
	}
	else
	{
		int i,tk,tp,tc;
		tk=tp=tc=0;
		for(i=0;i<MAX_PLAYER;i++)
			if (i!=RINFO.victim && PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
					tk++;
				else
				if (PINFO(i).flag & PEOPLE_POLICE)
					tp++;
				else
					tc++;
			}
		if (tk==0 || tp==0 || tc==0 || tp+tc<=tk)
		{
			sprintf(buf,"\33[31;1m%d %s �����ٴ�����\33[m",RINFO.victim+1,PINFO(RINFO.victim).nick);
			PINFO(RINFO.victim).flag&=~PEOPLE_ALIVE;
			goto_night();
		}
		else
		{
			sprintf(buf,"\33[31;1m%d %s �����ٴ����ˣ���������\33[m",RINFO.victim+1,PINFO(RINFO.victim).nick);
			send_msg(-1,buf);
			RINFO.status=INROOM_DARK;
			send_msg(RINFO.victim,"��\33[31;1mCtrl+T\33[m��������");
		}
	}
}

void goto_defence()
{
	char buf[200];
	int k;
	int i,j=0;
	
	RINFO.round++;

	if (RINFO.victim<0 || RINFO.victim>=MAX_PLAYER)
		RINFO.victim=0;
	
	k=0;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE)
			&& (PINFO(i).flag & PEOPLE_KILLER)
			&& (PINFO(i).flag & PEOPLE_SURRENDER))
		{
			PINFO(i).flag &=~PEOPLE_ALIVE;
			k++;
		}
	if (k>0)
	{
		send_msg(-1,"\33[31;1mɱ�ֱ����ˣ�ȡ���绤��ͶƱ\33[m");
		goto_night();
		return;
	}

	k=0;	
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE)
			&& (PINFO(i).flag & PEOPLE_VOTED))
			{
				k++;
				j=i;
			}
	if (k==0)
	{
		send_msg (-1, "\33[31;1mû�б�ָ֤���ˣ�ȡ���绤��ͶƱ\33[m");
		goto_night();
		return;
	}
	else if (k==1)
	{
		send_msg(-1,"\33[31;1mֻ��һ����ָ֤���ˣ�ȡ���绤��ͶƱ\33[m");
		RINFO.victim=j;
		goto_dark();
		return;
	}
	
	RINFO.turn=RINFO.victim;
	k=0;
	while (PINFO(RINFO.turn).style==-1 
		|| !(PINFO(RINFO.turn).flag & PEOPLE_ALIVE)
		|| !(PINFO(RINFO.turn).flag & PEOPLE_VOTED) )
	{
		if (RINFO.round%2)
			RINFO.turn--;
		else
			RINFO.turn++;
		if (RINFO.turn>=MAX_PLAYER)
			RINFO.turn=0;
		if (RINFO.turn<0)
			RINFO.turn=MAX_PLAYER-1;
		if (RINFO.turn==RINFO.victim)
		{
			k=1;
			break;
		}
	}
	if (!k)
	{
		send_msg (-1, "\33[31;1m���ڿ�ʼ�绤\33[m");
		sprintf (buf,"����\33[32;1m %d %s \33[m�绤", RINFO.turn + 1, PINFO(RINFO.turn).nick);
		send_msg (-1, buf);

		send_msg (RINFO.turn, "�绤�����\33[31;1mCtrl+T\33[m����");
		if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
			send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
		RINFO.status=INROOM_DEFENCE;
	}
	else
	{
		send_msg(-1,"\33[31;1mû�б绤���ˣ�ȡ���绤��ͶƱ\33[m");
		goto_night();
	}
}

void goto_day()
{
	char buf[200];
	if (RINFO.victim>=MAX_PLAYER || RINFO.victim<0)
	{
		sprintf(buf,"\33[31;1mû������,û������\33[m");
		send_msg(-1,buf);
		goto_check();

	}
	else
	{
		int i,tk,tp,tc;
		tk=tp=tc=0;
		for(i=0;i<MAX_PLAYER;i++)
			if (i!=RINFO.victim && PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
					tk++;
				else
				if (PINFO(i).flag & PEOPLE_POLICE)
					tp++;
				else
					tc++;
			}
		if (tk==0 || tp==0 || tc==0)
		{
			PINFO(RINFO.victim).flag&=~PEOPLE_ALIVE;
			goto_check();
		}
		else
		{
			sprintf (buf, "���ڣ�������\33[32;1m %d %s \33[m��������ʲô��Ҫ˵��", RINFO.victim + 1, PINFO(RINFO.victim).nick);
			send_msg (-1, buf);
			RINFO.status=INROOM_DAY;
			send_msg (RINFO.victim, "�밴\33[31;1mCtrl+T\33[m��������");
		}
	}
}

void goto_vote()
{
	int i,k;
	k=0;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE)
			&&(PINFO(i).flag & PEOPLE_KILLER)
			&& (PINFO(i).flag & PEOPLE_SURRENDER))
		{
			PINFO(i).flag &=~PEOPLE_ALIVE;
			k++;
		}
	if (k>0)
	{
		send_msg(-1,"\33[31;1mɱ�ֱ����ˣ�ͶƱȡ��\33[m");
		goto_night();
	}
	else
	{
		for(i=0;i<MAX_PLAYER;i++)
		{
			RINFO.voted[i]=-1;
			PINFO(i).vote=-1;
		}
		RINFO.status=INROOM_VOTE;
				  
		send_msg (-1, "\33[31;1m��������ͶƱ\33[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1
				&& (PINFO(i).flag & PEOPLE_ALIVE))
				send_msg (i, "��\33[31;1mCtrl+S\33[mͶƱ��\33[31;1mCtrl+T\33[mȷ��");
	}
}

void start_game()
{
	int i, j, totalk = 0, total = 0, totalc = 0, me;
	char buf[80];
	time_t t;
	struct tm * tt;
	me = mypos;
	for (i = 0; i < MAX_PLAYER; i++)
		if (PINFO(i).style != -1)
		{
			PINFO(i).flag=PEOPLE_ALIVE;
			PINFO(i).vote = -1;
		}
		
	if (RINFO.killernum<1)
		RINFO.killernum=1;
	if (RINFO.policenum<1)
	  RINFO.policenum=1;
		
	totalk = RINFO.killernum;
	totalc = RINFO.policenum;
	for (i = 0; i < MAX_PLAYER; i++)
		if (PINFO(i).style != -1)
			total++;
	if (total < 3)
	{
		send_msg(me, "\x1b[31;1m����3�˲μӲ��ܿ�ʼ��Ϸ\x1b[m");
		end_change_inroom();
		kill_msg(mypos);
		return;
	}

	if (totalk > total-totalk-totalc)
	{
		send_msg(me, "\x1b[31;1mƽ�������ڻ�������,�޷���ʼ��Ϸ\x1b[m");
		end_change_inroom();
		kill_msg(mypos);
		return;
	}
	if (totalc == 0)
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������\x1b[m", totalk);
	else
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������, %d������\x1b[m", totalk, totalc);
	send_msg(-1, buf);
	
	time(&t);
	tt=localtime(&t);
	sprintf(RINFO.gamename,"%s: %d-%d-%d, %04d/%02d/%02d %02d:%02d:%02d",rooms[myroom].name,totalk,totalc,total-totalk-totalc,
		tt->tm_year+1900,tt->tm_mon+1,tt->tm_mday,tt->tm_hour,tt->tm_min,tt->tm_sec);

#ifdef AUTOPOST
	unlink(ap_tmpname());
	fd = open(ap_tmpname(), O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd>=0)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			lseek(fd, 0, SEEK_END);
			for(i=0;i<MAX_PLAYER;i++)
			{
				if (PINFO(i).style!=-1)
				{
					sprintf(buf, "\x1b[34;1m%2d-%s(%s)\x1b[m", (i+1)%100, PINFO(i).nick, PINFO(i).id);
					write(fd,buf,strlen(buf));
					write(fd,"\n",1);
				}
			}
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);			
	}
#endif // AUTOPOST
	
	for(i=0;i<MAX_PLAYER;i++)
	{
		RINFO.killers[i]=-1;
		RINFO.polices[i]=-1;
		RINFO.seq_detect[i]=-1;
	}
	for (i = 0; i < totalk; i++)
	{
		do
		{
			j = rand() % MAX_PLAYER;
		} while (PINFO(j).style == -1 || PINFO(j).flag & PEOPLE_KILLER);
		PINFO(j).flag |= PEOPLE_KILLER;
		RINFO.killers[i]=j;
		send_msg(j, "������һ���޳ܵĻ���");
	}
	if (totalc>1 && total-totalk-totalc>1)
		RINFO.nokill=0;
	else
		RINFO.nokill=1;
	for (i = 0; i < totalc; i++)
	{
		do
		{
			j = rand() % MAX_PLAYER;
		} while (PINFO(j).style == -1 
			|| PINFO(j).flag & PEOPLE_KILLER
			|| PINFO(j).flag & PEOPLE_POLICE);
		PINFO(j).flag |= PEOPLE_POLICE;
		RINFO.polices[i]=j;
		send_msg(j, "������һλ���ٵ����񾯲�");
	}
	goto_night();

	kill_msg(-1);
}

#define menust 10

int do_com_menu()
{
	char menus[menust][15] = { 
		"0-����", 
		"1-�˳�", 
		"2-����", 
		"3-�б�", 
		"4-����",
		"5-����", 
		"6-����", 
		"7-�����", 
		"8-������", 
		"9-��ʼ"
	};
	int menupos[menust], i, j, k, sel = 0, ch, max = 0, me, offset = 0;
	char buf[80],disp[256];
 
  me=mypos;
  if (me<MAX_PLAYER)
    strcpy(menus[4],"4-�Թ�");
  
	if (RINFO.status != INROOM_STOP)
		strcpy(menus[menust-1], "9-����");
	menupos[0] = 0;
	for (i = 1; i < menust; i++)
		menupos[i] = menupos[i - 1] + strlen(menus[i - 1]) + 1;
	do
	{
		move(t_lines - 1, 0);
		clrtoeol();
		offset = 0;
		strcpy(disp,"\x1b[m");
		while (menupos[sel] - menupos[offset] + strlen(menus[sel]) >= scr_cols)
			offset++;
		j = mypos;
		for (i = 0; i < menust; i++)
			if (rooms[myroom].op==j || i <= 4)
				if (menupos[i] - menupos[offset] >= 0)
				{
					if (i == sel)
						k_setfcolor(RED, 1);
					else
						k_resetcolor();
					if (i >= max - 1)
						max = i + 1;
					if (i>0)
						strcat(disp," ");
					strcat(disp, menus[i]);
				}
		strcat(disp,"\x1b[m");
		move(t_lines - 1, 0);
		prints("%s",disp);
		
		ch = igetkey();
		if (kicked)
			return 0;
		switch (ch)
		{
		case KEY_LEFT:
		case KEY_UP:
			sel--;
			if (sel < 0)
				sel = max - 1;
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
			sel++;
			if (sel >= max)
				sel = 0;
			break;
		case '\n':
		case '\r':
			me = mypos;
			switch (menus[sel][0]) 
			{
			case '0':
				return 0;
			case '1':
				if (me<MAX_PLAYER
					&& (PINFO(me).flag & PEOPLE_ALIVE)
					&& RINFO.status != INROOM_STOP)
					{
						send_msg(me, "�㻹����Ϸ,�����˳�");
						kill_msg(me);
						return 0;
					}
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "ȷ���˳��� [y/N] ", buf, 3, 1, 0, 1);
				if (me<MAX_PLAYER
					&& (PINFO(me).flag & PEOPLE_ALIVE)
					&& RINFO.status != INROOM_STOP)
				{
					return 0;
				}
				if (kicked)
					return 0;
				if (toupper(buf[0]) != 'Y')
					return 0;
				return 1;
			case '2':
				if (RINFO.status != INROOM_STOP)
				{
					send_msg (me, "��Ϸ�У����ܸ�ID");
					kill_msg(me);
					return 0;
				}
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "����������:", buf, 13, 1, 0, 1);
				if (kicked || RINFO.status != INROOM_STOP)
					return 0;
				if (buf[0])
				{
					k = 1;
					for (j = 0; j < strlen(buf); j++)
						k = k && (isprint2(buf[j]));
					k = k && (buf[0] != ' ');
					k = k && (buf[strlen(buf) - 1] != ' ');
					if (!k)
					{
						move(t_lines - 1, 0);
						k_resetcolor();
						clrtoeol();
						prints(" ���ֲ����Ϲ淶");
						refresh();
						sleep(1);
						return 0;
					}
					j = 0;
					for (i = 0; i < MAX_PEOPLE; i++)
						if (PINFO(i).style != -1)
							if (i != me)
								if (!strcmp(buf, PINFO(i).id) || !strcmp(buf, PINFO(i).nick))
									j = 1;
					if (j)
					{
						move(t_lines - 1, 0);
						k_resetcolor();
						clrtoeol();
						prints(" �����������������");
						refresh();
						sleep(1);
						return 0;
					}
					start_change_inroom();
					strcpy(PINFO(me).nick, buf);
					end_change_inroom();
					kill_msg(-1);
				}
				return 0;
			case '3':
				for (i = 0; i < MAX_PEOPLE; i++)
					if (PINFO(i).style != -1)
					{
						sprintf(buf, "%2d %-12s %-12s", (i+1)%100,PINFO(i).id, PINFO(i).nick);
						send_msg(me, buf);
					}
				kill_msg(me);
				return 0;
			case '4':
				if (me<MAX_PLAYER
					&& (PINFO(me).flag & PEOPLE_ALIVE)
					&& RINFO.status != INROOM_STOP)
				{
					send_msg (me, "��Ϸ�У������뿪");
					kill_msg(me);
					return 0;
				}
				else
				if ((me>=MAX_PLAYER && 
					(rooms[myroom].numplayer>=rooms[myroom].maxplayer
					||rooms[myroom].numplayer>=MAX_PLAYER))
					|| (me<MAX_PLAYER
					&& rooms[myroom].numspectator>=MAX_PEOPLE-MAX_PLAYER))
				{
					send_msg(me, "��Ա����");
					kill_msg(me);
					return 0;
				}
				else
				if (me>=MAX_PLAYER)
				{
					// �����Ϸ��
					int i;
					for(i=0;i<MAX_PLAYER;i++)
					{
						if (PINFO(i).style==-1)
							break;
					}
					if (i<MAX_PLAYER)
					{
						start_change_inroom();
						PINFO(me).style=-1;
						mypos = i;
						PINFO(i).style = 0;
						PINFO(i).flag = 0;
						strcpy(PINFO(i).id, CURRENTUSER->userid);
						strcpy(PINFO(i).nick, CURRENTUSER->userid);
						PINFO(i).pid = uinfo.pid;
					
						rooms[myroom].numplayer++;
						rooms[myroom].numspectator--;
					
						if (rooms[myroom].op==me || rooms[myroom].op==-1 || PINFO(rooms[myroom].op).style==-1)
						{
							rooms[myroom].op=mypos;
						}
					
						end_change_inroom();
					
						kill_msg(-1);
					
						room_refresh(0);
						
					}
				}
				else
				{
					// ����Թ���
					int i;
					for(i=MAX_PLAYER;i<MAX_PEOPLE;i++)
					{
						if (PINFO(i).style==-1)
							break;
					}
					if (i<MAX_PEOPLE)
					{
						start_change_inroom();
						PINFO(me).style=-1;
						mypos = i;
						PINFO(i).style = 0;
						PINFO(i).flag = 0;
						strcpy(PINFO(i).id, CURRENTUSER->userid);
						strcpy(PINFO(i).nick, CURRENTUSER->userid);
						PINFO(i).pid = uinfo.pid;
					
						rooms[myroom].numspectator++;
						rooms[myroom].numplayer--;
					
						if (rooms[myroom].op==me || rooms[myroom].op==-1 || PINFO(rooms[myroom].op).style==-1)
						{
							rooms[myroom].op=mypos;
						}
					
						end_change_inroom();
					
						kill_msg(-1);
					
						room_refresh(0);
						
					}
				}
				return 0;
			case '5':
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "�����뻰��:", buf, 31, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0]) 
				{
					start_change_inroom();
					strcpy(rooms[myroom].title, buf);
					end_change_inroom();
					kill_msg(-1);
				}
				return 0;
			case '6':
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "�����뷿���������:", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf);
					if (i > 0 && i <= MAX_PLAYER)
					{
						rooms[myroom].maxplayer = i;
						sprintf(buf, "�������÷����������Ϊ%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "����Ϊ���ط���? [Y/N]", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				buf[0] = toupper(buf[0]);
				if (buf[0] == 'Y' || buf[0] == 'N') 
				{
					if (buf[0] == 'Y')
						rooms[myroom].flag |= ROOM_SECRET;
					else
						rooms[myroom].flag &= ~ROOM_SECRET;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0] == 'Y') ? "����" : "������");
					send_msg(-1, buf);
				}
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "����Ϊ��������? [Y/N]", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				buf[0] = toupper(buf[0]);
				if (buf[0] == 'Y' || buf[0] == 'N')
				{
					if (buf[0] == 'Y')
						rooms[myroom].flag |= ROOM_LOCKED;
					else
						rooms[myroom].flag &= ~ROOM_LOCKED;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0] == 'Y') ? "����" : "������");
					send_msg(-1, buf);
				}
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "����Ϊ�ܾ��Թ��ߵķ���? [Y/N]", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				buf[0] = toupper(buf[0]);
				if (buf[0] == 'Y' || buf[0] == 'N')
				{
					if (buf[0] == 'Y')
						rooms[myroom].flag |= ROOM_DENYSPEC;
					else
						rooms[myroom].flag &= ~ROOM_DENYSPEC;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0] == 'Y') ? "�ܾ��Թ�" : "���ܾ��Թ�");
					send_msg(-1, buf);
				}
				/*
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "����Ϊ�Թ����޷�����ɱ�־���ķ���? [Y/N]", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				buf[0] = toupper(buf[0]);
				if (buf[0] == 'Y' || buf[0] == 'N')
				{
					if (buf[0] == 'Y')
						rooms[myroom].flag |= ROOM_SPECBLIND;
					else
						rooms[myroom].flag &= ~ROOM_SPECBLIND;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0] == 'Y') ? "�Թ��޷�����ɱ�־���" : "�Թۿ��Կ���ɱ�־���");
					send_msg(-1, buf);
				}
				*/
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "���û��˵���Ŀ:", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf);
					if (i >= 0 && i <= MAX_KILLER)
					{
						RINFO.killernum = i;
						sprintf(buf, "�������÷��仵����Ϊ%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines - 1, 0);
				clrtoeol();
				k_getdata(t_lines - 1, 0, "���þ������Ŀ:", buf, 30, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf);
					if (i >= 0 && i <= MAX_POLICE)
					{
						RINFO.policenum = i;
						sprintf(buf, "�������÷��侯����Ϊ%d", i);
						send_msg(-1, buf);
					}
				}
				kill_msg(-1);
				return 0;
			case '7':
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "������Ҫ�ߵ����(����):", buf, 4, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf) - 1;
					if (i >= 0 && i < MAX_PEOPLE && PINFO(i).style != -1 && PINFO(i).pid != uinfo.pid)
					{
						sprintf(buf,"\x1b[34;1m%d %s������\x1b[m",i+1,PINFO(i).nick);
						send_msg(-1,buf);
						send_msg(i, "�㱻����");
						kill_msg(-1);
						start_change_inroom();
						player_drop(i);
						end_change_inroom();
				 
						return 2;
					}
				}
				return 0;
/*			case '7':

				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "������Ҫ��ֹ/�ָ�����Ȩ�����(����):", buf, 4, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf) - 1;
					if (i >= 0 && i < MAX_PEOPLE && PINFO(i).style != -1 && PINFO(i).pid != uinfo.pid)
					{
						if (PINFO(i).flag & PEOPLE_DENYSPEAK)
							PINFO(i).flag &= ~PEOPLE_DENYSPEAK;
						else
							PINFO(i).flag |= PEOPLE_DENYSPEAK;
						sprintf(buf, "%d %s��%s�˷���Ȩ", i + 1, PINFO(i).nick, (PINFO(i).flag & PEOPLE_DENYSPEAK) ? "��ֹ" : "�ָ�");
						send_msg(-1, buf);
						kill_msg(-1);
						return 0;
					}
				}
				return 0;
*/				
			case '8':
				move(t_lines - 1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines - 1, 0, "������ת�����������(����):", buf, 4, 1, 0, 1);
				if (kicked)
					return 0;
				if (buf[0])
				{
					i = atoi(buf) - 1;
					if (i >= 0 && i < MAX_PLAYER && PINFO(i).style != -1 && PINFO(i).pid != uinfo.pid)
					{
						rooms[myroom].op=i;
						sprintf(buf, "\x1b[34;1m%d %s��Ϊ�·���\x1b[m", i+1,PINFO(i).nick);
						send_msg(-1, buf);
						kill_msg(-1);
						return 0;
					}
				}
				return 0;
			case '9':
				start_change_inroom();
				if (RINFO.status == INROOM_STOP)
					start_game();
				else
				{
					RINFO.status = INROOM_STOP;
					send_msg(-1, "��Ϸ������ǿ�ƽ���");
					show_killerpolice();
					kill_msg(-1);
				}
				end_change_inroom();
				return 0;
			}
			break;
		default:
			for (i = 0; i < max; i++)
				if (ch == menus[i][0])
					sel = i;
			break;
		}
	} while (1);
}

void vote_result()
{
	int i;
	int k;
	char buf[200];
	start_change_inroom();
	if (RINFO.status==INROOM_VOTE)
	{
		for (i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE)
			&& PINFO(i).vote==-1)
			break;
				  
		if (i>=MAX_PLAYER)
		{
			int m1,m2,tt;
			tt=0;
			for(i=0;i<MAX_PLAYER;i++)
				if (PINFO(i).style!=-1
					&& (PINFO(i).flag & PEOPLE_ALIVE))
				{
					sprintf(buf,"\33[32;1m%d %s Ͷ�� %d %s һƱ\33[m",
						i+1,PINFO(i).nick,
						PINFO(i).vote+1,PINFO(PINFO(i).vote).nick);
					send_msg(-1,buf);
					PINFO(i).vnum=0;
					tt++;
				}
			for(i=0;i<MAX_PLAYER;i++)
				if (PINFO(i).style!=-1
					&& (PINFO(i).flag & PEOPLE_ALIVE))
				{
					PINFO(PINFO(i).vote).vnum++;
				}
			m1=-1;
			m2=-1;
					 
			for(i=0;i<MAX_PLAYER;i++)
				if (PINFO(i).style!=-1
					&& (PINFO(i).flag & PEOPLE_ALIVE)
					&& (PINFO(i).flag & PEOPLE_VOTED))
				{
					if (PINFO(i).vnum>m1)
					{
						m2=m1;
						m1=PINFO(i).vnum;
					}
					else if (PINFO(i).vnum>m2)
						m2=PINFO(i).vnum;

				}
			 
			if (m1*2>tt)
			{
				// ֱ��Ͷ��
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1
						&& (PINFO(i).flag & PEOPLE_ALIVE)
						&& (PINFO(i).flag & PEOPLE_VOTED))
					{
						if (PINFO(i).vnum==m1)
							break;
					}
				RINFO.victim=i;
				goto_dark();
			}
			else
			{
				// δ����1/2

				tt=0;
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1
						&& (PINFO(i).flag & PEOPLE_ALIVE)
						&& (PINFO(i).flag & PEOPLE_VOTED))
					{
						if (PINFO(i).vnum!=m1
							&& PINFO(i).vnum!=m2)
							PINFO(i).flag &=~PEOPLE_VOTED;
						else
							tt++;
					}
				if (RINFO.round>3)
				{
					//����3��,�������
					send_msg(-1, "\33[31;1mͶƱ������δ�н���������������\33[m");
					k=rand()%tt;
					for(i=0;i<MAX_PLAYER;i++)
						if (PINFO(i).style!=-1
							&& (PINFO(i).flag & PEOPLE_ALIVE)
							&& (PINFO(i).flag & PEOPLE_VOTED))
						{
							if (k==0)
								break;
							k--;			
						}
					RINFO.victim=i;
					goto_dark();
				}
				else
				{
					send_msg (-1, "\33[31;1m���Ʊ��δ��������,�������Ʊ�����˱绤\33[m");
					while (m1>0)
					{
						tt=0;
						for(i=0;i<MAX_PLAYER;i++)
							if (PINFO(i).style!=-1
								&& (PINFO(i).flag & PEOPLE_ALIVE)
								&& (PINFO(i).flag & PEOPLE_VOTED))
							{
								if (PINFO(i).vnum==m1)
									tt++;
							}
						if (tt>0)
							break;
						m1--;
					}
					if (m1>0 && tt>0)
					{
						k=rand()%tt;
						for(i=0;i<MAX_PLAYER;i++)
							if (PINFO(i).style!=-1
								&& (PINFO(i).flag & PEOPLE_ALIVE)
								&& (PINFO(i).flag & PEOPLE_VOTED)
								&& (PINFO(i).vnum==m1))
							{
								if (k==0)
									break;
								k--;			
							}
						RINFO.victim=i;
						goto_defence();
					}
					else
					{
						RINFO.victim=-1;
						goto_defence();
					}
				}
			}
			kill_msg(-1);
		}
	}
	end_change_inroom();
}

void player_drop(int d)
{
	char buf[200];
	if (PINFO(d).style!=-1)
	{
		PINFO(d).style=-1;
		if (d<MAX_PLAYER)
			rooms[myroom].numplayer--;
		else
			rooms[myroom].numspectator--;
		kill_msg(d);
	}
	switch(RINFO.status)
	{
	case INROOM_STOP:
		break;
	case INROOM_NIGHT:
		check_win();
		break;
	case INROOM_DAY:
		if (d==RINFO.victim)
		{
			sprintf (buf, "\x1b[1;32m%d %s\x1b[m: \x1b[1;31mOver.\x1b[m", d + 1, PINFO(d).nick);
			send_msg (-1, buf);
			goto_check();
		}
		break;
	case INROOM_CHECK:
		if (d==RINFO.turn)
		{
			sprintf (buf, "\x1b[1;32m%d %s\x1b[m: \x1b[1;31mOver.\x1b[m", d + 1, PINFO(d).nick);
			send_msg (-1, buf);
			while (1)
			{
				RINFO.turn++;
				if (RINFO.turn>=MAX_PLAYER)
					RINFO.turn=0;
				if (RINFO.turn==RINFO.victim)
					break;
				if (PINFO(RINFO.turn).style!=-1 
					&& (PINFO(RINFO.turn).flag & PEOPLE_ALIVE) )
					break;
			}
			if (RINFO.turn!=RINFO.victim)
			{
				sprintf (buf,"��\33[32;1m %d %s \33[m����", RINFO.turn + 1, PINFO(RINFO.turn).nick);
				send_msg (-1, buf);
				send_msg (RINFO.turn, "��\x1b[31;1m�����ƶ����\x1b[m���������ɵĶ�����");
				send_msg (RINFO.turn, "��\33[31;1mCtrl+S\33[mָ֤");
				send_msg (RINFO.turn, "ָ֤�����\33[31;1mCtrl+T\33[m��������");
				if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
					send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
			}
			else
			{
				RINFO.round=1;
				goto_defence();
						 
			}
		}
		break;
	case INROOM_DEFENCE:
		if (d==RINFO.turn)
		{
			sprintf (buf, "\x1b[1;32m%d %s\x1b[m: \x1b[1;31mOver.\x1b[m", d + 1, PINFO(d).nick);
			send_msg (-1, buf);
			while (1)
			{
				if (RINFO.round%2)
					RINFO.turn--;
				else
					RINFO.turn++;
				if (RINFO.turn>=MAX_PLAYER)
					RINFO.turn=0;
				if (RINFO.turn<0)
					RINFO.turn=MAX_PLAYER-1;
				if (RINFO.turn==RINFO.victim)
					break;
				if (PINFO(RINFO.turn).style!=-1 
					&& (PINFO(RINFO.turn).flag & PEOPLE_ALIVE)
					&& (PINFO(RINFO.turn).flag & PEOPLE_VOTED))
					break;
			}
			if (RINFO.turn!=RINFO.victim)
			{
				sprintf (buf,"��\33[32;1m %d %s \33[m�绤", RINFO.turn + 1, PINFO(RINFO.turn).nick);
				send_msg (-1, buf);
				send_msg (RINFO.turn, "�绤�����\33[31;1mCtrl+T\33[m����");
				if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
					send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
			}
			else
			{
				goto_vote();
	
			}
		}
		break;
	case INROOM_VOTE:
		vote_result();
		break;
	case INROOM_DARK:
		if (d==RINFO.victim)
		{
			sprintf (buf, "\x1b[1;32m%d %s\x1b[m: \x1b[1;31mOver.\x1b[m", d + 1, PINFO(d).nick);
			send_msg (-1, buf);
			goto_night();
		}
		break;
	}
}

void join_room(int w, int spec)
{
	char buf[200], buf2[220], buf3[200], msg[200], roomname[80];
	int i, j, k, me;
	time_t ct;

	clear();

	selected = 0;
	ipage = 0;
	jpage = 0;
	
	myroom = w;
	start_change_inroom();
	if (rooms[myroom].style != 1)
	{
		end_change_inroom();
		return;
	}
	strcpy(roomname, rooms[myroom].name);
	signal(SIGUSR1, room_refresh);
	if (spec)
	  i=MAX_PLAYER;
	else
	  i = 0;
	while (PINFO(i).style != -1)
		i++;
	if ((i>=MAX_PLAYER && !spec) || (i>=MAX_PEOPLE && spec))
	{
		end_change_inroom();
		return;
	}
	mypos = i;
	PINFO(i).style = 0;
	PINFO(i).flag = 0;
	strcpy(PINFO(i).id, CURRENTUSER->userid);
	strcpy(PINFO(i).nick, CURRENTUSER->userid);
	PINFO(i).pid = uinfo.pid;

	if (!spec)
		rooms[myroom].numplayer++;
	else
		rooms[myroom].numspectator++;

	if (!spec)
	{
		if (rooms[myroom].op==-1 || PINFO(rooms[myroom].op).style==-1)
		{
			rooms[myroom].op=mypos;
		}
	}

	end_change_inroom();

	kill_msg(-1);

	room_refresh(0);
	while (1)
	{
		do
		{
			int ch;
			sprintf(buf2,"%2d����:",(mypos+1)%100);
			ch = -k_getdata(t_lines - 1, 0, buf2, buf, 70, 1, NULL, 1);
			if (rooms[myroom].style != 1)
				kicked = 1;
			if (kicked)
				goto quitgame;
			if (ch == KEY_UP) 
			{
				selected--;
				if (selected < 0)
					selected = rooms[myroom].numplayer - 1;
				if (ipage > selected)
					ipage = selected;
				if (selected > ipage + t_lines - 5)
					ipage = selected - (t_lines - 5);
				kill_msg(mypos);
			}
			else if (ch == KEY_DOWN) 
			{
				selected++;
				if (selected >= rooms[myroom].numplayer)
					selected = 0;
				if (ipage > selected)
					ipage = selected;
				if (selected > ipage + t_lines - 5)
					ipage = selected - (t_lines - 5);
				kill_msg(mypos);
			} 
			else if (ch == KEY_PGUP)
			{
				jpage += t_lines / 2;
				kill_msg(mypos);
			} 
			else if (ch == KEY_PGDN)
			{
				jpage -= t_lines / 2;
				if (jpage <= 0)
					jpage = 0;
				kill_msg(mypos);
			} 
			else if (mypos<MAX_PLAYER
				&& (PINFO(mypos).flag & PEOPLE_ALIVE)
			   && ch == Ctrl('T'))
			{
				switch(RINFO.status)
				{
				case INROOM_STOP:
					break;
				case INROOM_NIGHT:
					if ((PINFO(mypos).flag & PEOPLE_KILLER) && RINFO.turn!=-1 && RINFO.victim==-1)
					{
						RINFO.victim=RINFO.turn;
						if (RINFO.nokill)
							sprintf (buf, "�����ˣ����Ż�\33[32;1m%d %s \33[m!", RINFO.victim + 1, PINFO(RINFO.victim).nick);
						else
							sprintf (buf, "�����ˣ���ɱ\33[32;1m%d %s \33[m!", RINFO.victim + 1, PINFO(RINFO.victim).nick);
				
						for(i=0;i<MAX_PLAYER;i++)
							if (PINFO(i).style!=-1
								&& (PINFO(i).flag & PEOPLE_ALIVE)
								&& (PINFO(i).flag & PEOPLE_KILLER))
							{
								send_msg(i,buf);
								kill_msg(i); 
							}
					}
					else if ((PINFO(mypos).flag & PEOPLE_POLICE) 
						&& RINFO.round!=-1 
						&& RINFO.informant==-1)
					{
						RINFO.informant=RINFO.round;
						for(i=0;i<MAX_PLAYER;i++)
							if (RINFO.seq_detect[i]==-1)
							{
								RINFO.seq_detect[i]=RINFO.informant;
								break;
							}
						if (PINFO(RINFO.informant).flag & PEOPLE_KILLER)
							sprintf (buf, "û��\33[32;1m%d %s \33[m����һλ��ɫ��ɱ��!", RINFO.informant + 1, PINFO(RINFO.informant).nick);
						else
							sprintf (buf, "��������´��ˣ�\33[32;1m%d %s \33[m��һ������!", RINFO.informant + 1, PINFO(RINFO.informant).nick);
						for(i=0;i<MAX_PLAYER;i++)
							if (PINFO(i).style!=-1
								&& (PINFO(i).flag & PEOPLE_ALIVE)
							  && (PINFO(i).flag & PEOPLE_POLICE))
						  {
								send_msg(i,buf);
								kill_msg(i);
							}
					}
					start_change_inroom();
					if (RINFO.status==INROOM_NIGHT && RINFO.informant!=-1 && RINFO.victim!=-1)
					{
						// turn to check
						if (RINFO.nokill)
							sprintf (buf, "\33[1;33mע�⣬\33[32;1m %d %s \33[33m��������...\33[m", RINFO.victim + 1, PINFO(RINFO.victim).nick);
						else
							sprintf (buf, "\33[1;33m��ɭɭ���¹��£�������\33[32;1m %d %s \33[33m��ʬ��...\33[m", RINFO.victim + 1, PINFO(RINFO.victim).nick);
						send_msg(-1,buf);
					
						for(i=0;i<MAX_PLAYER;i++)
							RINFO.voted[i]=-1;
					
						if (RINFO.nokill)
						{
							// bluster
							goto_check();
						}
						else
						{
							goto_day();
						}
			 
					}
					end_change_inroom();
					kill_msg(-1);
					break;
				case INROOM_DAY:
					if (RINFO.victim==mypos)
					{
						  sprintf (buf, "\x1b[35;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
						send_msg (-1, buf);
						sprintf (buf, "��ô����\33[35;1m %d %s \33[m�������Թ��ߵ���ݣ�������Ϸ...", mypos + 1, PINFO(mypos).nick);
						send_msg (-1, buf);
					
						if (PINFO(mypos).flag & PEOPLE_KILLER)
							PINFO(mypos).flag |=PEOPLE_HIDEKILLER;
						PINFO(mypos).flag&=~PEOPLE_ALIVE;
											
						goto_check();	
						kill_msg(-1);
					}
					break;
				case INROOM_CHECK:
					if (RINFO.turn==mypos && RINFO.voted[mypos]!=-1)
					{
						if ((PINFO(mypos).flag & PEOPLE_KILLER) && mypos==RINFO.voted[mypos])
						{
							PINFO(mypos).flag|=PEOPLE_SURRENDER;
							sprintf (buf, "\x1b[32;1m%d %s\x1b[m ������...\33[m", 
								mypos + 1, PINFO(mypos).nick);
						}
						else
							  sprintf (buf, "\x1b[32;1m%d %s\x1b[m ָ֤ %d %s\33[m",
								mypos + 1, PINFO(mypos).nick,
								  RINFO.voted[mypos]+1,PINFO((int)RINFO.voted[mypos]).nick);
						send_msg(-1, buf);
						sprintf(buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
						send_msg(-1, buf);
						
						if (!check_win())
						{
							PINFO((int)RINFO.voted[mypos]).flag|=PEOPLE_VOTED;
							for(i=0;i<MAX_PLAYER;i++)
								RINFO.voted[i]=-1;
						
							while (1)
							{
								RINFO.turn++;
								if (RINFO.turn>=MAX_PLAYER)
									RINFO.turn=0;
								if (RINFO.turn==RINFO.victim)
									break;
								if (PINFO(RINFO.turn).style!=-1 
									&& (PINFO(RINFO.turn).flag & PEOPLE_ALIVE) )
									break;
							}
							if (RINFO.turn!=RINFO.victim)
							{
								sprintf (buf,"��\33[32;1m %d %s \33[m����", RINFO.turn + 1, PINFO(RINFO.turn).nick);
								send_msg (-1, buf);
								send_msg (RINFO.turn, "��\x1b[31;1m�����ƶ����\x1b[m���������ɵĶ�����");
								send_msg (RINFO.turn, "��\33[31;1mCtrl+S\33[mָ֤");
								send_msg (RINFO.turn, "ָ֤�����\33[31;1mCtrl+T\33[m��������");
								if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
									send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
							}
							else
							{
								RINFO.round=1;
								goto_defence();
							 
							}
						}
					}
					kill_msg(-1);
					break;
				case INROOM_DEFENCE:
					if (RINFO.turn==mypos && (PINFO(mypos).flag & PEOPLE_VOTED))
					{
						if ((PINFO(mypos).flag & PEOPLE_KILLER) && mypos==RINFO.voted[mypos])
						{
							PINFO(mypos).flag|=PEOPLE_SURRENDER;
							sprintf (buf, "\x1b[32;1m%d %s\x1b[m ������...\33[m", mypos + 1, PINFO(mypos).nick);
						}
						else
							  sprintf (buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
						send_msg (-1, buf);

						if (!check_win())
						{
							while (1)
							{
								if (RINFO.round%2)
									RINFO.turn--;
								else
									RINFO.turn++;
								if (RINFO.turn>=MAX_PLAYER)
									RINFO.turn=0;
								if (RINFO.turn<0)
									RINFO.turn=MAX_PLAYER-1;
								if (RINFO.turn==RINFO.victim)
									break;
								if (PINFO(RINFO.turn).style!=-1 
									&& (PINFO(RINFO.turn).flag & PEOPLE_ALIVE)
									&& (PINFO(RINFO.turn).flag & PEOPLE_VOTED))
									break;
							}
							if (RINFO.turn!=RINFO.victim)
							{
								sprintf (buf,"��\33[32;1m %d %s \33[m�绤", RINFO.turn + 1, PINFO(RINFO.turn).nick);
								send_msg (-1, buf);
								send_msg (RINFO.turn, "�绤�����\33[31;1mCtrl+T\33[m����");
								if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
									send_msg (RINFO.turn, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
	
							}
							else
							{
								goto_vote();
		
							}
						}
					}
					kill_msg(-1);
					break;
				case INROOM_VOTE:
					if ( RINFO.voted[mypos]!=-1
						&& PINFO(mypos).vote==-1 )
					{
						sprintf (buf, "\x1b[32;1m��ͶƱ�� %d %s\33[m", 
							RINFO.voted[mypos]+1,PINFO((int)RINFO.voted[mypos]).nick);
						send_msg (mypos, buf);
						PINFO(mypos).vote=RINFO.voted[mypos];
						vote_result();
						kill_msg(-1);
					}
					break;
				case INROOM_DARK:
					if (RINFO.victim==mypos)
					{
						sprintf (buf, "\x1b[35;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
						send_msg (-1, buf);
						sprintf (buf, "��ô����\33[35;1m %d %s \33[m�������Թ��ߵ���ݣ�������Ϸ...", mypos + 1, PINFO(mypos).nick);
						send_msg (-1, buf);
						PINFO(mypos).flag&=~PEOPLE_ALIVE;
						goto_night();
						kill_msg(-1);
					}
					break;
				}
			} 
			else if (mypos<MAX_PLAYER
				&& (PINFO(mypos).flag & PEOPLE_ALIVE)
				&& ch == Ctrl('S'))
			{
				int sel;
				sel = getpeople(selected);
				if (sel!=-1 && sel<MAX_PLAYER
					&& PINFO(sel).style!=-1)
				{
					switch(RINFO.status)
					{
					case INROOM_STOP:
						break;
					case INROOM_NIGHT:
						if ((PINFO(mypos).flag & PEOPLE_KILLER) 
							&& RINFO.victim==-1
							&& (PINFO(sel).flag & PEOPLE_ALIVE))
						{
							RINFO.turn=sel;
							if (RINFO.nokill)
								sprintf (buf, "���Ż�\33[32;1m%d %s \33[m��?",
									RINFO.turn + 1,
									PINFO(RINFO.turn).nick);
							else
								sprintf (buf, "��ɱ\33[32;1m%d %s \33[m��?",
									RINFO.turn + 1,
									PINFO(RINFO.turn).nick);
							for(i=0;i<MAX_PLAYER;i++)
								if (PINFO(i).style!=-1
									&& (PINFO(i).flag & PEOPLE_ALIVE)
									&& (PINFO(i).flag & PEOPLE_KILLER))
								{
									send_msg(i,buf);
									kill_msg(i);
								}
						}
						else if ((PINFO(mypos).flag & PEOPLE_POLICE)
							&& RINFO.informant==-1 
							&& (PINFO(sel).flag & PEOPLE_ALIVE)
							&& !(PINFO(sel).flag & PEOPLE_POLICE))
						{
							RINFO.round=sel;
							sprintf (buf, "���\33[32;1m%d %s \33[m��?", RINFO.round + 1, PINFO(RINFO.round).nick);
							for(i=0;i<MAX_PLAYER;i++)
								if (PINFO(i).style!=-1
									&& (PINFO(i).flag & PEOPLE_ALIVE)
									&& (PINFO(i).flag & PEOPLE_POLICE))
								{
									send_msg(i,buf);
									kill_msg(i);
								}
						}
						break;
					case INROOM_DAY:
						break;
					case INROOM_CHECK:
						if (RINFO.turn==mypos
							&& (PINFO(sel).flag & PEOPLE_ALIVE))
						{
							if (mypos==sel)
							{
								if (PINFO(mypos).flag & PEOPLE_KILLER)
								{
									if (RINFO.voted[mypos]!=mypos)
									{
										sprintf (buf, "\33[31m���������\33[m");
										RINFO.voted[mypos]=sel;
									}
									else
									{
										sprintf(buf,"\33[31m�㲻�������\33[m");
										RINFO.voted[mypos]=-1;
									}
								}
								else
								{
									sprintf(buf,"����ָ֤�Լ�");
								}
							}
							else
							{

								sprintf (buf, "��ָ֤\33[32;1m%d %s \33[m��?", sel + 1, PINFO(sel).nick);
								RINFO.voted[mypos]=sel;
								 
							}
							send_msg(mypos,buf);
							kill_msg(mypos);
		
						}
						break;
					case INROOM_DEFENCE:
						if (RINFO.turn==mypos
							&& (PINFO(sel).flag & PEOPLE_ALIVE)
							&& (PINFO(sel).flag & PEOPLE_KILLER)
							&& mypos==sel)
						{	
							if (RINFO.voted[mypos]!=mypos)
							{
								  sprintf (buf, "\33[31m���������\33[m");
								RINFO.voted[mypos]=sel;
							  }
							else
							{
								sprintf(buf,"\33[31m�㲻�������\33[m");
								RINFO.voted[mypos]=-1;
							}
 
							send_msg(mypos,buf);
							kill_msg(mypos);
		
						}
						break;
					case INROOM_VOTE:
						if ((PINFO(sel).flag & PEOPLE_VOTED)
							&& PINFO(mypos).vote==-1)
						{
							RINFO.voted[mypos]=sel;
							sprintf (buf, "��ͶƱ��\33[32;1m%d %s \33[m��?", 
								sel + 1, PINFO(sel).nick);
							send_msg(mypos,buf);
							kill_msg(mypos);
						}
						break;
					case INROOM_DARK:
						break;
					}
				}
			}
			else if (ch <= 0 && !buf[0]) 
			{
				j = do_com_menu();
				if (kicked)
					goto quitgame;
				if (j == 1)
					goto quitgame;
			}
			else if (ch <= 0)
			{
				break;
			}
		} while (1);
		
		if (mypos<MAX_PLAYER  && (PINFO(mypos).flag & PEOPLE_ALIVE))
		{
			if (RINFO.status ==INROOM_VOTE)
				continue;
			if ((RINFO.status == INROOM_NIGHT)
				&& !(PINFO(mypos).flag & PEOPLE_POLICE)
				&& !(PINFO(mypos).flag & PEOPLE_KILLER))
				continue;
			if ((RINFO.status ==INROOM_DARK)
				&& RINFO.victim!=mypos)
				continue;
			if ((RINFO.status ==INROOM_DAY)
				&& RINFO.victim!=mypos)
				continue;
			if ((RINFO.status ==INROOM_CHECK)
				&& RINFO.turn!=mypos)
				continue;		  
			if ((RINFO.status ==INROOM_DEFENCE)
				&& RINFO.turn!=mypos)
				continue;
		}
		ct=time(NULL);
		if (!denytalk && ct==lasttalktime)
		{
//			send_msg(mypos,"��˵��̫�죬��ֹ����5����");
//			denytalk=1;
//			kill_msg(mypos);
//			continue;
		}
		if (denytalk && ct-lasttalktime>5)
		{
			denytalk=0;
		}
		if (denytalk)
			continue;
		start_change_inroom();
	  
		lasttalktime=ct;
		me = mypos;
		strcpy(msg, buf);
		if (msg[0] == '/' && msg[1] == '/')
		{
			i = 2;
			while (msg[i] != ' ' && i < strlen(msg))
				i++;
			strcpy(buf, msg + 2);
			buf[i - 2] = 0;
			while (msg[i] == ' ' && i < strlen(msg))
				i++;
			buf2[0] = 0;
			buf3[0] = 0;
			if (msg[i - 1] == ' ' && i < strlen(msg))
			{
				k = i;
				while (msg[k] != ' ' && k < strlen(msg))
					k++;
				strcpy(buf2, msg + i);
				buf2[k - i] = 0;
				i = k;
				while (msg[i] == ' ' && i < strlen(msg))
					i++;
				if (msg[i - 1] == ' ' && i < strlen(msg))
				{
					k = i;
					while (msg[k] != ' ' && k < strlen(msg))
						k++;
					strcpy(buf3, msg + i);
					buf3[k - i] = 0;
				}
			}
			k = 1;
			for (i = 0;; i++)
			{
				if (!party_data[i].verb)
					break;
				if (!strcasecmp(party_data[i].verb, buf))
				{
					k = 0;
					sprintf(buf, "%s \x1b[1m%s\x1b[m %s", party_data[i].part1_msg, buf2[0] ? buf2 : "���", party_data[i].part2_msg);
					break;
				}
			}
			if (k)
				for (i = 0;; i++)
				{
					if (!speak_data[i].verb)
						break;
					if (!strcasecmp(speak_data[i].verb, buf))
					{
						k = 0;
						sprintf(buf, "%s: %s", speak_data[i].part1_msg, buf2);
						break;
					}
				}
			if (k)
				for (i = 0;; i++)
				{
					if (!condition_data[i].verb)
						break;
					if (!strcasecmp(condition_data[i].verb, buf)) 
					{
						k = 0;
						sprintf(buf, "%s", condition_data[i].part1_msg);
						break;
					}
				}

			if (k)
				continue;
			strcpy(buf2, buf);
			sprintf(buf, "\x1b[1m%d %s\x1b[m %s", me + 1, PINFO(me).nick, buf2);
		} 
		else
		{
			strcpy(buf2, buf);
			sprintf(buf, "%d %s: %s", me + 1, PINFO(me).nick, buf2);
		}
		if (PINFO(me).flag & PEOPLE_DENYSPEAK)
		{
			send_msg(i, "������û�з���Ȩ");
		} 
		else
		{
			if (RINFO.status!=INROOM_STOP 
				&& mypos<MAX_PLAYER 
				&& (PINFO(mypos).flag & PEOPLE_ALIVE))
			{
				if (mypos<MAX_PLAYER
					&& (RINFO.status == INROOM_NIGHT) 
					&& PINFO(me).flag & PEOPLE_KILLER)
				{
					for (i = 0; i < MAX_PLAYER; i++)
						if (PINFO(i).style != -1
							&& (PINFO(i).flag & PEOPLE_ALIVE)
							&& (PINFO(i).flag & PEOPLE_KILLER)) 
						{
							send_msg(i, buf);
							kill_msg(i);
						}
			   
				}
				else
				if (mypos<MAX_PLAYER
					&& (RINFO.status == INROOM_NIGHT) 
					&& PINFO(me).flag & PEOPLE_POLICE)
				{
					for (i = 0; i < MAX_PLAYER; i++)
						if (PINFO(i).style != -1
							&& (PINFO(i).flag & PEOPLE_ALIVE)
							&& (PINFO(i).flag & PEOPLE_POLICE))
						{
							send_msg(i, buf);
							kill_msg(i);
						}
					
				}
				else
				{
					send_msg(-1,buf);
					kill_msg(-1);
				}
		  
			}
			else
			if (RINFO.status!=INROOM_STOP)
			{
				strcpy(buf2,"\33[35;1m*");
				strcat(buf2,buf);
				strcat(buf2,"\33[m");
				for (i = 0; i < MAX_PEOPLE; i++)
				if (PINFO(i).style != -1)
				{
					if (i>=MAX_PLAYER || !(PINFO(i).flag & PEOPLE_ALIVE))
					{
						send_msg(i, buf2);
						kill_msg(i);
					}
				}
			}
			else
			{
				send_msg(-1,buf);
				kill_msg(-1);
			}
		}
		end_change_inroom();
		//kill_msg(-1);
	}

	quitgame:
	start_change_inroom();
	me = mypos;
	if (rooms[myroom].op==me)
	{
		int k;
		for (k = 0; k < MAX_PEOPLE; k++)
			if (PINFO(k).style != -1)
				if (k != me)
				{
					rooms[myroom].op=k;
					sprintf(buf, "\x1b[34;1m%d %s��Ϊ�·���\x1b[m", k+1,PINFO(k).nick);
					send_msg(-1, buf);
					kill_msg(-1);
					break;
				}
		if (k>=MAX_PEOPLE)
		{
			rooms[myroom].op=-1;
			send_msg(-1, "�����ɢ��");
			kill_msg(-1);
			rooms[myroom].style = -1;
			end_change_inroom();
			goto quitgame2;
		}
	}
	if (PINFO(me).style != -1)
	{
		PINFO(me).style = -1;
		if (me>=MAX_PLAYER)
			rooms[myroom].numspectator--;
		else
			rooms[myroom].numplayer--;
		kill_msg(-1);
	}
	end_change_inroom();

/*
	if(killer)
		sprintf(buf, "ɱ��%sǱ����", buf2);
	else
		sprintf(buf, "%s�뿪����", buf2);
	for(i=0;i<myroom->people;i++)
	{
		send_msg(inrooms.peoples+i, buf);
		kill(inrooms.peoples[i].pid, SIGUSR1);
	}
*/
quitgame2:
	kicked = 0;
	sprintf(buf, "tmp/%d.msg", rand());
	save_msgs(buf);
	sprintf(buf2, "\"%s\"��ɱ�˼�¼", roomname);
	k_getdata(t_lines - 1, 0, "�Ļر���ȫ����Ϣ��?[y/N]", buf3, 3, 1, 0, 1);
	if (toupper(buf3[0]) == 'Y')
	{
		k_mail_file(CURRENTUSER->userid, buf, CURRENTUSER->userid, buf2, BBSPOST_MOVE, NULL);
	}
	else
		unlink(buf);
	//kill_msg(-1);
	signal(SIGUSR1, DEFAULT_SIGNAL);
}

static int room_list_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[��Ϸ��ѡ��]",
			   "  �˳�[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ����[\x1b[1;32mEnter\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] ���[\x1b[1;32ma\x1b[0;37m] ����[\x1b[1;32mJ\x1b[0;37m] \x1b[m	  ����: \x1b[31;1mbad@smth.org\x1b[m");
	move(2, 0);
	prints("\x1b[37;44;1m    %4s %-14s %-12s %4s %4s %6s %-20s", "���", "��Ϸ������", "����Ա", "����", "���", "������", "����");
	clrtoeol();
//	k_resetcolor();
	update_endline();
	return SHOW_CONTINUE;
}

static int room_list_show(struct _select_def *conf, int i)
{
	struct room_struct *r;
	int j = room_get(i - 1);

	if (j != -1)
	{
		r = rooms + j;
		prints("  %3d  %-14s %-12s %3d  %3d  %2s%2s%2s %-36s", 
			i, r->name, inrooms[j].peoples[r->op].id,
			r->numplayer, r->maxplayer, (r->flag & ROOM_LOCKED) ? "��" : "",
			(r->flag & ROOM_SECRET) ? "��" : "",
			(!(r->flag & ROOM_DENYSPEC)) ? "��" : "", r->title);
	}
	return SHOW_CONTINUE;
}

static int room_list_select(struct _select_def *conf)
{
	struct room_struct *r, *r2;
	int i = room_get(conf->pos - 1), j;
	char ans[4];
	int spec;

	if (i == -1)
		return SHOW_CONTINUE;
	r = rooms + i;
	j = find_room(r->name);
	if (j == -1)
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷����ѱ�����!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	k_getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
	spec=(toupper(ans[0]) == 'Y');
	j = find_room(r->name);
	if (j == -1)
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷����ѱ�����!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	r2 = rooms + j;
	if (spec && r2->flag & ROOM_DENYSPEC && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷���ܾ��Թ���");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	if ((!spec && r2->numplayer >= r2->maxplayer && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
	    ||(!spec && r2->numplayer>=MAX_PLAYER)
		||(spec && r2->numspectator >= MAX_PEOPLE-MAX_PLAYER))
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷�����������");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	join_room(find_room(r2->name), spec);
	return SHOW_DIRCHANGE;
}

static int room_list_k_getdata(struct _select_def *conf, int pos, int len)
{
	clear_room();
	conf->item_count = room_count();
	return SHOW_CONTINUE;
}

static int room_list_prekey(struct _select_def *conf, int *key)
{
	switch (*key)
	{
	case 'e':
	case 'q':
		*key = KEY_LEFT;
		break;
	case 'p':
	case 'k':
		*key = KEY_UP;
		break;
	case ' ':
	case 'N':
		*key = KEY_PGDN;
		break;
	case 'n':
	case 'j':
		*key = KEY_DOWN;
		break;
	}
	return SHOW_CONTINUE;
}

static int room_list_key(struct _select_def *conf, int key)
{
	struct room_struct r, *r2;
	int i, j;
	char name[40], ans[4];
	int spec;

	switch (key)
	{
	case 'a':
		r.op=-1;
		k_getdata(0, 0, "������:", name, 13, 1, NULL, 1);
		if (!name[0])
			return SHOW_REFRESH;
		if (name[0] == ' ' || name[strlen(name) - 1] == ' ')
		{
			move(0, 0);
			clrtoeol();
			prints(" ��������ͷ��β����Ϊ�ո�");
			refresh();
			sleep(1);
			return SHOW_CONTINUE;
		}
		strcpy(r.name, name);
		r.style = 1;
		r.flag = ROOM_SPECBLIND;
		r.numplayer = 0;
		r.numspectator=0;
		r.maxplayer = MAX_PLAYER;
		r.op=-1;
		strcpy(r.title, "��ɱ��ɱ��ɱɱɱ");
		if (add_room(&r) == -1)
		{
			move(0, 0);
			clrtoeol();
			prints(" ��һ�����ֵķ�����!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(r.name), 0);
		return SHOW_DIRCHANGE;
	case 'J':
		k_getdata(0, 0, "������:", name, 12, 1, NULL, 1);
		if (!name[0])
			return SHOW_REFRESH;
		if ((i = find_room(name)) == -1)
		{
			move(0, 0);
			clrtoeol();
			prints(" û���ҵ��÷���!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		k_getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
		spec=(toupper(ans[0])=='Y');
		if ((i = find_room(name)) == -1) 
		{
			move(0, 0);
			clrtoeol();
			prints(" û���ҵ��÷���!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
        r2 = rooms + i;
		if (spec && r2->flag & ROOM_DENYSPEC && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP)) 
		{
			move(0, 0);
			clrtoeol();
			prints(" �÷���ܾ��Թ���");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		if ((!spec && r2->numplayer >= r2->maxplayer && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
			||(!spec && r2->numplayer>=MAX_PLAYER)
			||(spec && r2->numspectator >= MAX_PEOPLE-MAX_PLAYER))
		{
			move(0, 0);
			clrtoeol();
			prints(" �÷�����������");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(name), toupper(ans[0]) == 'Y');
		return SHOW_DIRCHANGE;
	case 'K':
		if (!K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
			return SHOW_CONTINUE;
		i = room_get(conf->pos - 1);
		if (i != -1)
		{
			r2 = rooms + i;
			r2->style = -1;
			for (j = 0; j < MAX_PEOPLE; j++)
				if (inrooms[i].peoples[j].style != -1)
					kill(inrooms[i].peoples[j].pid, SIGUSR1);
		}
		return SHOW_DIRCHANGE;
	}
	return SHOW_CONTINUE;
}
/*
void show_top_board()
{
	FILE *fp;
	char buf[80];
	int i, j, x, y;

	clear();
	for (i = 1; i <= 6; i++) 
	{
		sprintf(buf, "service/killer.%d", i);
		fp = fopen(buf, "r");
		if (!fp)
			return;
		for (j = 0; j < 7; j++)
		{
			if (feof(fp))
				break;
			y = (i - 1) % 3 * 8 + j;
			x = (i - 1) / 3 * 40;
			if (fgets(buf, 80, fp) == NULL)
				break;
			move(y, x);
			k_resetcolor();
			if (j == 2)
				k_setfcolor(RED, 1);
			prints("%s", buf);
		}
		fclose(fp);
	}
	pressanykey();
}
*/
int choose_room()
{
	struct _select_def grouplist_conf;
	int i;
	POINT *pts;

	clear_room();
	bzero(&grouplist_conf, sizeof(struct _select_def));
	grouplist_conf.item_count = room_count();
	if (grouplist_conf.item_count == 0)
	{
		grouplist_conf.item_count = 1;
	}
	pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
	for (i = 0; i < BBS_PAGESIZE; i++) 
	{
		pts[i].x = 2;
		pts[i].y = i + 3;
	}
	grouplist_conf.item_per_page = BBS_PAGESIZE;
	grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
	grouplist_conf.prompt = "��";
	grouplist_conf.item_pos = pts;
	grouplist_conf.arg = NULL;
	grouplist_conf.title_pos.x = 0;
	grouplist_conf.title_pos.y = 0;
	grouplist_conf.pos = 1;	 /* initialize cursor on the first mailgroup */
	grouplist_conf.page_pos = 1;		/* initialize page to the first one */

	grouplist_conf.on_select = room_list_select;
	grouplist_conf.show_data = room_list_show;
	grouplist_conf.pre_key_command = room_list_prekey;
	grouplist_conf.key_command = room_list_key;
	grouplist_conf.show_title = room_list_refresh;
	grouplist_conf.get_data = room_list_k_getdata;
//	show_top_board();
	list_select_loop(&grouplist_conf);
	free(pts);
    return 0;
}

int killer_main()
{
	int i, oldmode;
	void *shm, *shm2;
	shm = k_attach_shm("GAMEROOM2_SHMKEY", 3452, sizeof(struct room_struct) * MAX_ROOM, &i);
	rooms = shm;
	if (i) 
	{
		for (i = 0; i < MAX_ROOM; i++)
		{
			rooms[i].style = -1;
			rooms[i].w = 0;
			rooms[i].level = 0;
		}
	}
	shm2 = k_attach_shm("KILLER2_SHMKEY", 9579, sizeof(struct inroom_struct) * MAX_ROOM, &i);
	inrooms = shm2;
	if (i) 
	{
		for (i = 0; i < MAX_ROOM; i++)
			inrooms[i].w = 0;
	}
	oldmode = uinfo.mode;
	lasttalktime=-1;
	denytalk=0;
	modify_user_mode(KILLER);
	choose_room();
	modify_user_mode(oldmode);
	shmdt(shm);
	shmdt(shm2);
    return 0;
}

