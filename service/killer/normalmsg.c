int normalmsg_callback(char * msg)
{
	int v; //是否显示这句话：0显示，1不显示
	v=0;
	switch(RINFO.status)
	{
		case INROOM_STOP:
				break;
		case INROOM_NIGHT:
				v=normalmsg_night(msg);
				break;
		case INROOM_DAY:
				break;
		case INROOM_CHECK:
				break;
		case INROOM_DEFENCE:
				v=normalmsg_defence(msg);
				break;
		case INROOM_VOTE:
				break;
		case INROOM_DARK:
				break;
	}
	return v;
}
int normalmsg_night(char *msg)
{
	switch (RINFO.gametype)
	{
		case GAME_JOKERPIG:
			return normalmsg_night_jokerpig(msg);
		default:
			return 0;
	}
	return 0;
}
int normalmsg_defence(char *msg)
{
	switch (RINFO.gametype)
	{
		case GAME_JOKER1:
			return normalmsg_defence_joker1(msg);
		case GAME_JOKERPIG:
			return normalmsg_defence_jokerpig(msg);
		default:
			return 0;
	}
}

