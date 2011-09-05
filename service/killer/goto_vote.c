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
		send_msg(-1,"\33[31;1m杀手崩溃了，投票取消\33[m");
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
		send_msg (-1, "\33[31;1m现在请大家投票\33[m");
		kill_msg (-1);
		send_msg2 (PEOPLE_ALIVE, "按\33[31;1mCtrl+S\33[m投票，\33[31;1mCtrl+T\33[m确认");
	}
}

	
