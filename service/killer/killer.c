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
/*****************************************************
Modified by yanzg@smth, 07/08/2005
�Ķ���
(1) �����˴�ͳ�������Ͷ������ѻ��˵����
(2) ��������ctrl-u,��Ȼ�ٰ�һ�λ�����ctrl-sָ֤����ȡ��
(3) ҹ���ж�Ҳ��ctrl-u��
(4) �������жϸ�Ϊ����һ����ǣ�ɱ�ֵ��߲���Ӱ������
(5) Ϊ���޸ķ��㣬��ʱ�������𿪵������ļ��У�����killerĿ¼�£�
******************************************************/
#include "killer/base.c"
#include "killer/setting.c"
#include "killer/killer.inc"

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
#include "killer/action.c"

#include "killer/send_msg.c"
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
#include "killer/room_action.c"

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
void send_msg3 (int pos, char * msg_head, int who, char * msg_tail)
{
	char buf[200];
	sprintf (buf, "%s%d %s%s", msg_head, who+1, PINFO(who).nick, msg_tail);
	send_msg (pos, buf);
	kill_msg (pos);
}

#include "killer/refreshit.c"
extern int RMSG;

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

#include "killer/show_killerpolice.c"
#include "killer/check_win.c"
#include "killer/goto_night.c"
#include "killer/goto_check.c"
#include "killer/goto_dark.c"	
#include "killer/goto_defence.c"
#include "killer/goto_day.c"
#include "killer/goto_vote.c"
#include "killer/start_game.c"
#include "killer/do_com_menu.c"
#include "killer/vote_result.c"
#include "killer/join_room.c"
#include "killer/joker.c"
#include "killer/player_drop.c"
#include "killer/room_list_select.c"
//room_list_action include more than one functions
#include "killer/room_list_action.c"


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
//	shm3 = k_attach_shm("GAMEJK2_SHMKEY", 9580, sizeof(struct game_struct) * MAX_GAMETYPE, &i);
//	gamesettinglist = shm3;
//	if (i) 
//	{
//		for (i = 0; i < MAX_ROOM; i++)
//			thisgame[i].w = 0;
//	}

	oldmode = uinfo.mode;
	lasttalktime=-1;
	denytalk=0;
	modify_user_mode(KILLER);
	choose_room();
	modify_user_mode(oldmode);
	shmdt(shm);
	shmdt(shm2);
//	shmdt(shm3);
	return 0;
}

