void goto_vote()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			goto_vote_original();
			break;
	}
}
void goto_vote_original()
{
	int i,k;
	k=kill_surrender();
	if (k>0)
	{
		send_msg(-1,"\33[31;1mɱ�ֱ����ˣ�ͶƱȡ��\33[m");
		kill_msg (-1);
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
		kill_msg (-1);
		send_msg2 (PEOPLE_ALIVE, "��\33[31;1mCtrl+S\33[mͶƱ��\33[31;1mCtrl+T\33[mȷ��");
	}
}

	
