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

