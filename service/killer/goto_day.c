void goto_day()
{
	int gametype;
	gametype=RINFO.gametype;
	if (gametype==1)
		goto_day_original();
	else if (gametype==2)
		goto_day_crow();
	else if (gametype==3)
		goto_day_butterfly();
	else if (gametype==4)
		goto_day_tsinghua();
	//�廪������Ҫ������
}
void goto_day_original()
{
	goto_day_common(RINFO.action_target[S_KILLER]);
}
void goto_day_tsinghua()
{
	goto_day_common(RINFO.action_target[S_KILLER]);
}
void goto_day_butterfly()
{
	RINFO.deadpointer=0;
	goto_day_common(RINFO.deadlist[RINFO.deadpointer]);
}
	
void goto_day_crow()
{
	RINFO.deadpointer=0;
	goto_day_common(RINFO.deadlist[RINFO.deadpointer]);
}

int goto_day_common ( int turn) 
{
	//��ÿ������֮��Ҳ����
	char buf[200];
	if (turn>=MAX_PLAYER ||turn<0)
	{
		sprintf(buf,"\33[31;1mû�������ˡ�\33[m");
		send_msg(-1,buf);
		goto_check();
		return 1;

	}
	else
	{
		if (!check_win()){
			sprintf (buf, "���ڣ�������\33[32;1m %d %s \33[m��������ʲô��Ҫ˵��", turn + 1, PINFO(turn).nick);
			send_msg (-1, buf);
			RINFO.status=INROOM_DAY;
			send_msg (turn, "�밴\33[31;1mCtrl+T\33[m��������");
			RINFO.turn=turn;
			return 0;
		}
		else
			return 2;
	}
}


