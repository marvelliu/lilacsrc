void goto_dark()
{
	char buf[200];
	if (RINFO.victim>=MAX_PLAYER || RINFO.victim<0)
	{
		sprintf(buf,"\33[31;1mû���о�,û������\33[m");
		send_msg(-1,buf);
		kill_msg(-1);
		goto_night();
	}
	else if (PINFO(RINFO.victim).flag & PEOPLE_SEENBAD)
	{
		sprintf(buf,"\33[31;1m%d %s �����ٴ����ˣ�û������\33[m",RINFO.victim+1,PINFO(RINFO.victim).nick);
		PINFO(RINFO.victim).flag&=~PEOPLE_ALIVE;
		send_msg(-1,buf);
		kill_msg(-1);
		goto_night();
	}
	else
	{
		PINFO(RINFO.victim).flag |=PEOPLE_LASTWORDS;
		RINFO.turn=RINFO.victim;
		sprintf(buf,"\33[31;1m%d %s �����ٴ�����\33[m",RINFO.victim+1,PINFO(RINFO.victim).nick);
		send_msg(-1,buf);
		if (check_win())
		{
			kill_msg(-1);
//			PINFO(RINFO.victim).flag&=~PEOPLE_ALIVE;
//			goto_night();
		}
		else
		{
			sprintf(buf,"\33[31;1m��������\33[m");
			send_msg(-1,buf);
			kill_msg(-1);
			RINFO.status=INROOM_DARK;
			send_msg(RINFO.victim,"��\33[31;1mCtrl+T\33[m��������");
			kill_msg(RINFO.victim);
		}
	}
}

