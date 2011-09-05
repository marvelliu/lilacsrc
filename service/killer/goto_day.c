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
	//清华规则不需要留遗言
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
	//在每个遗言之后也调用
	char buf[200];
	if (turn>=MAX_PLAYER ||turn<0)
	{
		sprintf(buf,"\33[31;1m没有死者了。\33[m");
		send_msg(-1,buf);
		goto_check();
		return 1;

	}
	else
	{
		if (!check_win()){
			sprintf (buf, "现在，被害者\33[32;1m %d %s \33[m在天堂有什么话要说？", turn + 1, PINFO(turn).nick);
			send_msg (-1, buf);
			RINFO.status=INROOM_DAY;
			send_msg (turn, "请按\33[31;1mCtrl+T\33[m结束遗言");
			RINFO.turn=turn;
			return 0;
		}
		else
			return 2;
	}
}


