#define MENU_ITEM_Q 10
#define MENU_ITEM_LENGTH 15
void menu_init (char * menus);
int menu_displaymenu(char *menus, int sel);
int menu_ask_yn( char * prompt);
void menu_ask_string( char * prompt, int length, char * buf);
int menu_ask_num( char * prompt);

#include "menu6.c"


int do_com_menu()
{
	int  i, sel = 0, ch,  me;//, offset = 0;
	int max=0;
	char menus[MENU_ITEM_Q][MENU_ITEM_LENGTH];
	menu_init(menus[0]);
	do
	{
		max=menu_displaymenu(menus[0],sel);	
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
				return menu_1_callback();
			case '2':
				return menu_2_callback();
			case '3':
				return menu_3_callback();
			case '4':
				return menu_4_callback();
			case '5':
				return menu_5_callback();
			case '6':
				return menu_6_callback();
			case '7':
				return menu_7_callback();
			case '8':
				return menu_8_callback();
			case '9':
				return menu_9_callback();
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


void menu_init (char * menus)
{
	int i;
	char menukey[MENU_ITEM_Q]="0123456789";
	char menutitle[MENU_ITEM_Q][MENU_ITEM_LENGTH-2]= { "����", "�˳�", "����", "�б�", "����",	"����", "����", "�����", "������", "��ʼ"};
	for (i=0;i<MENU_ITEM_Q;i++)
		sprintf(menus+i*MENU_ITEM_LENGTH, "%c-%s",menukey[i],menutitle[i]);
  	if (mypos<MAX_PLAYER)
    		strcpy(menus+4*MENU_ITEM_LENGTH,"4-�Թ�");
  
	if (RINFO.status != INROOM_STOP)
		strcpy(menus+(MENU_ITEM_Q-1)*MENU_ITEM_LENGTH, "9-����");
}
int menu_displaymenu(char * menus, int sel)
{
	int menupos[MENU_ITEM_Q],i,offset;
	int max = 0;
	char disp[256];
	menupos[0] = 0;
	for (i = 1; i < MENU_ITEM_Q; i++)
		menupos[i] = menupos[i - 1] + strlen(menus+(i - 1)*MENU_ITEM_LENGTH) + 1;
	move(t_lines - 1, 0);
	clrtoeol();
	offset = 0;
	strcpy(disp,"\x1b[m");
	while (menupos[sel] - menupos[offset] + strlen(menus+sel*MENU_ITEM_LENGTH) >= scr_cols)
		offset++;
	max=MENU_ITEM_Q;
	if (rooms[myroom].op!=mypos)
		max=4;
	for (i = 0; i < MENU_ITEM_Q; i++)
		if (rooms[myroom].op==mypos || i <= 4)
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
				strcat(disp, menus+i*MENU_ITEM_LENGTH);
			}
	strcat(disp,"\x1b[m");
	move(t_lines - 1, 0);
	prints("%s",disp);
	return max;
}
int menu_ask_yn(char * prompt)
{
	char buf[200];
	move(t_lines - 1, 0);
	clrtoeol();
	k_getdata(t_lines - 1, 0, prompt, buf, 3, 1, 0, 1);
	if (toupper(buf[0]) == 'Y')
		return 1;
	else if (toupper(buf[0])=='N')
		return 0;
	return -1;
}
void menu_ask_string(char * prompt, int length, char * buf)
{
	move(t_lines - 1, 0);
	clrtoeol();
	k_getdata(t_lines - 1, 0, prompt, buf, length, 1, 0, 1);
}
int menu_ask_num( char * prompt)
{
	char buf[200];
	move(t_lines - 1, 0);
	clrtoeol();
	k_getdata(t_lines - 1, 0, prompt, buf, 30, 1, 0, 1);
	if (buf[0])
		return atoi(buf);
	else 
		return -1;
}
void menu_announce( char * prompt)
{
	move(t_lines - 1, 0);
	clrtoeol();
	prints(prompt);
}
	

int menu_1_callback()
{
	int k;
	if (mypos<MAX_PLAYER && (PINFO(mypos).flag & PEOPLE_ALIVE) && RINFO.status != INROOM_STOP)
	{
		send_msg(mypos, "�㻹����Ϸ,�����˳�");
		kill_msg(mypos);
		return 0;
	}
	k=menu_ask_yn( "ȷ���˳��� [Y/N] ");
	if ( ( mypos<MAX_PLAYER && (PINFO(mypos).flag & PEOPLE_ALIVE) && RINFO.status != INROOM_STOP )
			|| kicked || k<=0 )
		return 0;
	return 1;
}
int menu_2_callback()
{
	int i,j,k;
	char buf[200];
	if (RINFO.status != INROOM_STOP)
	{
		send_msg(mypos, "��Ϸ�У����ܸ�ID");
		kill_msg(mypos);
		return 0;
	}
	menu_ask_string("����������:",13, buf);
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
			menu_announce(" ���ֲ����Ϲ淶");
			refresh();
			sleep(1);
			return 0;
		}
		j = 0;
		for (i = 0; i < MAX_PEOPLE; i++)
			if (PINFO(i).style != -1)
				if (i != mypos)
					if (!strcmp(buf, PINFO(i).id) || !strcmp(buf, PINFO(i).nick))
						j = 1;
		if (j)
		{
			menu_announce(" �����������������");
			refresh();
			sleep(1);
			return 0;
		}
		start_change_inroom();
		strcpy(PINFO(mypos).nick, buf);
		end_change_inroom();
		kill_msg(-1);
	}
	return 0;
}
int menu_3_callback()
{
	int i;
	char buf[200];
	for (i = 0; i < MAX_PEOPLE; i++)
		if (PINFO(i).style != -1)
		{
			sprintf(buf, "%2d %-12s %-12s", (i+1)%100,PINFO(i).id, PINFO(i).nick);
			send_msg(mypos, buf);
		}
		kill_msg(mypos);
	return 0;
}

int menu_4_callback()
{
	int me;
	me=mypos;
	if (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_ALIVE) && RINFO.status != INROOM_STOP)
	{
		send_msg(me, "��Ϸ�У������뿪");
		kill_msg(me);
		return 0;
	}
	else if ( (mypos>=MAX_PLAYER &&  (rooms[myroom].numplayer>=rooms[myroom].maxplayer)
		||rooms[myroom].numplayer>=MAX_PLAYER)	
		|| ( mypos<MAX_PLAYER &&   rooms[myroom].numspectator>=MAX_PEOPLE-MAX_PLAYER ) )
	{
		send_msg(me, "��Ա����");
		kill_msg(me);
		return 0;
	}
	else if (me>=MAX_PLAYER)
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
}
int menu_5_callback()
{
	char buf[200];
	if (RINFO.status != INROOM_STOP)
	{
		send_msg (mypos, "��Ϸ�У����ܸĻ���");
		kill_msg(mypos);
		return 0;
	}
	menu_ask_string("�����뻰��:", 31, buf);
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
}

int menu_7_callback()
{
	int i;
	char buf[200];
	menu_ask_string("������Ҫ�ߵ����(����):", 4, buf);
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
}
int menu_7_callback2()
{
	int i;
	char buf[200];
	menu_ask_string("������Ҫ��ֹ/�ָ�����Ȩ�����(����):", 4,buf);
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
}
int menu_8_callback()
{
	char buf[200];
	int i;
	menu_ask_string("������ת�����������(����):", 4, buf);
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
}
int menu_9_callback()
{
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
