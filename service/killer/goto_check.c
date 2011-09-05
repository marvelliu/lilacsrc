void goto_check_clear_vote()
{
	int i;
	for (i=0;i<MAX_PLAYER;i++)
	{
		if(PINFO(i).style !=-1 && PINFO(i).flag & PEOPLE_ALIVE)
		{
			PINFO(i).vnum=0;
		}
	}
}
void goto_check()
{
	int gametype;
	gametype=RINFO.gametype;
	RINFO.surrenderflag=-1;
	goto_check_clear_vote();

	if (gametype==1)
		goto_check_original();
	else if (gametype==2)
		goto_check_crow();
	else if (gametype==3)
		goto_check_butterfly();
	else if (gametype==4)
		goto_check_tsinghua();
}
	
void goto_check_original()
{
	if (!check_win())
	{
		if (RINFO.nokill)
		{
			RINFO.nokill=0;
			RINFO.turn=RINFO.action_target[S_KILLER];
		}
		else
			RINFO.turn=RINFO.action_target[S_KILLER]+1;
		if (!look_for_next_speaker(RINFO.turn, RINFO.turn,1))
		{
			send_msg (-1, "\33[31;1m���ڿ�ʼָ֤\33[m");
			goto_check_speak ();
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
void goto_check_crow ()
{
	if (!check_win())
	{
		if (RINFO.deadnum>0)
				RINFO.turn=RINFO.deadlist[RINFO.deadnum-1]+1;
		else if (RINFO.mulenum>0)
				RINFO.turn=RINFO.mulelist[RINFO.mulenum-1]+1;
		else
			RINFO.turn=0;
		RINFO.victim=RINFO.turn;
		if (!look_for_next_speaker(RINFO.turn, RINFO.turn,1))
		{
			send_msg (-1, "\33[31;1m���ڿ�ʼ���Բ�ͬʱͶƱ\33[m");
			goto_check_speak ();
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
void goto_check_butterfly ()
{
	goto_check_crow();
}
	
void goto_check_tsinghua()
{
	int i;
	if (!check_win())
	{
		RINFO.round=0;
		RINFO.status=INROOM_CHECK;
		i=RINFO.action_target[S_KILLER];
		PINFO(i).flag &=~PEOPLE_ALIVE;
		if (PINFO(i).flag & PEOPLE_KILLER)
			 PINFO(i).flag |= PEOPLE_HIDEKILLER;
	}
}

int look_for_next_speaker (int start, int finish,int mode)
{
	//mode: 0: from start+1 to finish, check mode
	//	1: from start to finish, check mode
	//	2: from start+1 to finish, defence mode
	//	3: from start to finish, defence mode

	int k;
	k=0;
	RINFO.turn=start;
	while (!(PINFO(RINFO.turn).flag & PEOPLE_ALIVE) || PINFO(RINFO.turn).style==-1 || !(mode%2) ||
			(!(PINFO(RINFO.turn).flag & PEOPLE_VOTED) && mode>=2) )
	{
		if (mode == 0 || mode == 2)
				mode++;
		if (mode<2)
			RINFO.turn++;
		else if (RINFO.round % 2)
			RINFO.turn--;
		else 
			RINFO.turn++;
		
		if (RINFO.turn>=MAX_PLAYER)
				RINFO.turn=0;
		if (RINFO.turn<0)
			RINFO.turn=MAX_PLAYER-1;
		
		if (RINFO.turn==finish)
		{
				k=1;
				break;
		}
	}
	return k;
}
void goto_check_speak ()
{
	int gametype;
	char buf[200];
	gametype=RINFO.gametype;
	sprintf (buf,"������\33[32;1m %d %s \33[m����", RINFO.turn + 1, PINFO(RINFO.turn).nick);
	send_msg (-1, buf);
	kill_msg (-1);
	if (gametype==1)
	{
//		send_msg (RINFO.turn, "��\x1b[31;1m�����ƶ����\x1b[m���������ɵĶ�����,��\33[31;1mCtrl+S\33[mָ֤");
//		kill_msg (RINFO.turn);
//		send_msg (RINFO.turn, "ָ֤�����\33[31;1mCtrl+T\33[m��������");
		send_msg (RINFO.turn, "��\33[31;1mCtrl+T\33[m��������");
		kill_msg (RINFO.turn);
		if (PINFO(RINFO.turn).flag & PEOPLE_KILLER)
		{
				send_msg (RINFO.turn, "\33[31;1m��Ctrl+U�ɱ���\33[m");
				kill_msg (RINFO.turn);
		}
	}
	else
	{
		send_msg (RINFO.turn, "��\x1b[31;1m�����ƶ����\x1b[m���������ɵĶ�����,��\33[31;1mCtrl+S\33[mͶƱ");
		kill_msg (RINFO.turn);
		send_msg (RINFO.turn, "ͶƱ�����\33[31;1mCtrl+T\33[m��������");
		kill_msg (RINFO.turn);
		if (PINFO(RINFO.turn).flag & PEOPLE_BAD)
		{
				send_msg (RINFO.turn, "\33[31;1m��Ctrl+U�ɱ���\33[m");
				kill_msg (RINFO.turn);
		}
	}
}
