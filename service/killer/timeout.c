void set_turn_time()
{
	RINFO.lastturntime=bbstime(NULL);
}
void set_action_time()
{
	RINFO.lastactiontime=bbstime(NULL);
}
void set_auto_time()
{
	RINFO.lastautotime=bbstime(NULL);
}
int check_timeout_turn ()
{
	time_t ct;
	ct=bbstime(NULL);
	if (ct-RINFO.lastturntime >RINFO.time_turnlimit && RINFO.time_turnlimit)
		return 1;
	else
		return 0;
}
int check_timeout_action ()
{
	time_t ct;
	ct=bbstime(NULL);
	if (ct-RINFO.lastactiontime >RINFO.time_actionlimit && RINFO.time_actionlimit)
		return 1;
	else
		return 0;
}
int check_timeout_auto ()
{
	time_t ct;
	ct=bbstime(NULL);
	if (ct-RINFO.lastautotime >RINFO.time_autolimit && RINFO.time_autolimit)
		return 1;
	else
		return 0;
}

void timeout_callback (int type)
{
	switch(RINFO.status)
	{
		case INROOM_STOP:
				break;
		case INROOM_NIGHT:
				timeout_night(type);
				break;
		case INROOM_DAY:
				break;
		case INROOM_CHECK:
				break;
		case INROOM_DEFENCE:
				timeout_defence(type);
				break;
		case INROOM_VOTE:
				break;
		case INROOM_DARK:
				break;
	}
}

void timeout_defence(int type)
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL:
		case GAME_CROW:
		case GAME_BUTTERFLY:
		case GAME_TSINGHUA:
			break;
		case GAME_JOKER1:
			joker1_timeout_skip();
			break;
		case GAME_JOKERPIG:
			jokerpig_timeout_skip();
			break;
	}
	
}
void timeout_night(int type)
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL:
		case GAME_CROW:
		case GAME_BUTTERFLY:
		case GAME_TSINGHUA:
			break;
		case GAME_JOKER1:
			break;
		case GAME_JOKERPIG:
			timeout_night_jokerpig(type);
			break;
	}
	
}

