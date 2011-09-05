
void ctrl_g_callback ()
{
	char buf[200];
	switch(RINFO.status)
	{
		case INROOM_STOP:
				break;
		case INROOM_NIGHT:
				ctrl_g_night();
				break;
		case INROOM_DAY:
				break;
		case INROOM_CHECK:
				if (RINFO.turn==mypos&& (PINFO(mypos).flag & PEOPLE_ALIVE))
				{
					if (PINFO(mypos).flag & PEOPLE_BAD)
					{
						if (RINFO.voted[mypos]!=-2)
						{
							sprintf (buf, "\33[31m你想崩溃了\33[m");
							RINFO.voted[mypos]=-2;
						}
						else
						{
							sprintf(buf,"\33[31m你不想崩溃了\33[m");
							RINFO.voted[mypos]=-1;
						}
					}
					else
							sprintf(buf,"不能崩溃");
					send_msg(mypos,buf);
					kill_msg(mypos);
				}
				break;
		case INROOM_DEFENCE:
				ctrl_g_defence();
				break;
		case INROOM_VOTE:
					break;
		case INROOM_DARK:
					break;
	}
}

void ctrl_g_night()
{
	int gametype;
	gametype=RINFO.gametype;
	if (gametype != GAME_CROW)
		return ;
	ctrl_g_night_crow();
}
void ctrl_g_night_crow ()
{
	if (ctrl_g_night_common(PEOPLE_KILLER, S_KILLER, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_MAGIC, S_MAGIC, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_FOREST, S_FOREST, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_BADMAN, S_BADMAN, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_BUTTERFLY, S_BUTTERFLY,0))
	{}
	else if (ctrl_g_night_common( PEOPLE_SHOOTER, S_SHOOTER, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_DOCTOR, S_DOCTOR, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_POLICE, S_POLICE, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_GOODMAN, S_GOODMAN, 0))
	{}
	else if (ctrl_g_night_common( PEOPLE_CROW, S_CROW, 0))
        {}
}
	
int ctrl_g_night_common(long people_type, int s_type, int forbidden)
{
	char buf[200];
	if ((PINFO(mypos).flag & people_type)
			&& RINFO.action_target[s_type]==-1 
			&& !forbidden)
	{
		RINFO.action_target_tmp[s_type]=-2;
		sprintf (buf, "想放弃行动吗?");
		send_msg2(people_type, buf);
		return 1;
	}
	return 0;
}
void ctrl_g_defence()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			ctrl_g_defence_original();
			break;
		case GAME_JOKER1 :
			ctrl_g_defence_joker1();
			break;
		case GAME_JOKERPIG:
			ctrl_g_defence_jokerpig();
			break;
	}
}
void ctrl_g_defence_original()
{
	char buf[200];
	if (RINFO.turn==mypos&& (PINFO(mypos).flag & PEOPLE_ALIVE))
	{
		if (PINFO(mypos).flag & PEOPLE_BAD)
		{
			if (RINFO.voted[mypos]!=-2)
			{
				sprintf (buf, "\33[31m你想崩溃了\33[m");
				RINFO.voted[mypos]=-2;
			}
			else
			{
				sprintf(buf,"\33[31m你不想崩溃了\33[m");
				RINFO.voted[mypos]=-1;
			}
		}
		else
			sprintf(buf,"不能崩溃");
		send_msg(mypos,buf);
		kill_msg(mypos);
	}
}

