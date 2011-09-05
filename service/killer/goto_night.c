
void goto_night()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			goto_night_original();
			break;
		case GAME_CROW :
			goto_night_crow();
			break;
		case GAME_BUTTERFLY :
			goto_night_butterfly();
			break;
		case GAME_TSINGHUA :
			goto_night_tsinghua();
			break;
		case GAME_JOKERPIG :
			goto_night_jokerpig();
			break;
	}
}
void goto_night_original()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1mҹĻ������...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
				{
					if (RINFO.nokill)
						goto_night_msg(i, "�⵶","�Ż�");
					else
						goto_night_msg(i, "�⵶","�к�");
				}
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "����","�鿴");
			}
		goto_night_common();
 
	}
}

void goto_night_crow()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1mҹĻ������...\x1b[m");
		kill_msg(-1);
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
					goto_night_msg(i, "�⵶","�к�");
				else if (PINFO(i).flag & PEOPLE_MAGIC)
					goto_night_msg(i, "����","ʩ��");
				else if (PINFO(i).flag & PEOPLE_FOREST)
					goto_night_msg(i, "��֦","����");
				else if (PINFO(i).flag & PEOPLE_BADMAN)
					goto_night_msg(i, "֤��","�ݺ�");
				else if (PINFO(i).flag & PEOPLE_BUTTERFLY)
					goto_night_msg(i, "���","ӵ��");
				else if (PINFO(i).flag & PEOPLE_SHOOTER)
					goto_night_msg(i, "����","����");
				else if (PINFO(i).flag & PEOPLE_DOCTOR)
					goto_night_msg(i, "��Ͳ","ע��");
				else if (PINFO(i).flag & PEOPLE_POLICE)
					goto_night_msg(i, "����","�鿴");
				else if (PINFO(i).flag & PEOPLE_GOODMAN)
					goto_night_msg(i, "ͼֽ","���");
				else if (PINFO(i).flag & PEOPLE_CROW)
					goto_night_msg(i, "�۾�","�۲�");
			}
		for (i=0;i<MAX_PLAYER;i++)
		{
			if (PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).style != -1)
			{
				send_msg(i,"\x1b[31;1m�����ѡ���ҹ���ж�,�Ȱ�,\x1b[31;1mctrl-U\x1b[m�ٰ�,\x1b[31;1mctrl-Tȷ��...\x1b[m");
				kill_msg(i);
			}
		}
		goto_night_common();
 
	}
}
void goto_night_butterfly()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1mҹĻ������...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
						goto_night_msg(i, "�⵶","�к�");
				else if (PINFO(i).flag & PEOPLE_FOREST)
						goto_night_msg(i, "��֦","����");
				else if (PINFO(i).flag & PEOPLE_BUTTERFLY)
						goto_night_msg(i, "���","ӵ��");
				else if (PINFO(i).flag & PEOPLE_SHOOTER)
						goto_night_msg(i, "����","����");
				else if (PINFO(i).flag & PEOPLE_DOCTOR)
						goto_night_msg(i, "��Ͳ","ע��");
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "����","�鿴");
				
			}
		goto_night_common();
 
	}
}
void goto_night_tsinghua()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1mҹĻ������...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
						goto_night_msg(i, "�⵶","�к�");
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "����","�鿴");
			}
		goto_night_common();
	}
}

void goto_night_msg(int me, char * tool, char * act)
{
	char  buf [200];
//	if (RINFO.gametype !=4)
		sprintf (buf, "�����%s(\x1b[31;1mCtrl+S\x1b[m)ѡ��Ҫ%s����,\x1b[31;1mCtrl+T\x1b[mȷ��", tool, act);
//	else 
//		sprintf (buf, "�����%s(\x1b[31;1mCtrl+S\x1b[m)ѡ��Ҫ%s����", tool, act);
		
	send_msg(me, buf);
}
	
void goto_night_common()
{
	int i;
	RINFO.day++;
	RINFO.victim=-1;
	RINFO.turn=-1;
	RINFO.round=-1;
//	RINFO.informant=-1;
	for (i=0;i<MAX_ROLE;i++)
	{
		RINFO.action_target_tmp[i]=-1;
	}
	for(i=0;i<MAX_PLAYER;i++)
	{
		RINFO.action_target[i]=-1;//���ڼ����廪��Ϸ�о�����ж�

		PINFO(i).vote=-1;
		PINFO(i).flag &=~PEOPLE_VOTED;
		PINFO(i).flag &=~PEOPLE_MULE;
		PINFO(i).flag &=~PEOPLE_LASTWORDS;
		RINFO.voted[i]=-1;
		PINFO(i).vnum=0;
	}
	RINFO.status=INROOM_NIGHT; 
}

