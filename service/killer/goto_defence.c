void goto_defence()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			goto_defence_original();
			break;
		case GAME_JOKER1 :
			goto_defence_joker1();	
			break;
		case GAME_JOKERPIG :
			goto_defence_jokerpig();
			break;
	}
}
void goto_defence_setvictim()
{
	
	if (RINFO.victim<0 || RINFO.victim>=MAX_PLAYER)
	{
		if (RINFO.action_target[S_KILLER]>=0 && RINFO.action_target[S_KILLER]<MAX_PLAYER)
			RINFO.victim=RINFO.action_target[S_KILLER]; 
		else
			RINFO.victim=0;
	}
}
int kill_surrender()
{
	int i,k;
	k=0;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1
			&& (PINFO(i).flag & PEOPLE_ALIVE)
			&& (PINFO(i).flag & PEOPLE_BAD)
			&& (PINFO(i).flag & PEOPLE_SURRENDER))
		{
			PINFO(i).flag &=~PEOPLE_ALIVE;
			k++;
		}
	if (RINFO.surrenderflag>0 && k==0)
		k=1;
	return k;
}

void goto_defence_original()
{
	int k;
	int voted[MAX_PLAYER];
	RINFO.round++;
	goto_defence_setvictim();
	k=kill_surrender();
	if (k>0)
	{
		send_msg(-1,"\33[31;1mɱ�ֱ����ˣ�ȡ���绤��ͶƱ\33[m");
		kill_msg(-1);
		goto_night();
		return;
	}
	k=living_role_list(PEOPLE_VOTED,voted);
	if (k==0)
	{
		send_msg (-1, "\33[31;1mû�б�ָ֤���ˣ�ȡ���绤��ͶƱ\33[m");
		goto_night();
		return;
	}
	else if (k==1)
	{
		send_msg(-1,"\33[31;1mֻ��һ����ָ֤���ˣ�ȡ���绤��ͶƱ\33[m");
		RINFO.victim=voted[0];
		goto_dark();
		return;
	}
	send_msg (-1, "\33[31;1m���ڿ�ʼ�绤\33[m");
	kill_msg (-1);
	RINFO.status=INROOM_DEFENCE;
	RINFO.turn=RINFO.victim;
	if (!look_for_next_speaker(RINFO.turn, RINFO.victim, 3))
		goto_defence_speak();
	else
	{
		send_msg(-1,"\33[31;1mû�б绤���ˣ�ȡ���绤��ͶƱ\33[m");
		goto_night();
	}
}
void goto_defence_speak ()
{
	int gametype;
	char buf[200];
	gametype=RINFO.gametype;
	sprintf (buf,"������\33[32;1m %d %s \33[mΪ�Լ��绤", RINFO.turn + 1, PINFO(RINFO.turn).nick);
	send_msg (-1, buf);
	kill_msg (-1);
	send_msg (RINFO.turn, "�绤�����\33[31;1mCtrl+T\33[m��������");
	kill_msg (RINFO.turn);
	if (PINFO(RINFO.turn).flag & PEOPLE_BAD)
	{
		send_msg (RINFO.turn, "\33[31;1m��Ctrl+U�ɱ���\33[m");
		kill_msg (RINFO.turn);
	}
}


