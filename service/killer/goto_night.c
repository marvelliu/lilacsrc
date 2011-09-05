
void goto_night()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			goto_night_original();
			break;
		case GAME_CROW :
			goto_night_crow();
			break;
		case GAME_BUTTERFLY :
			goto_night_butterfly();
			break;
		case GAME_TSINGHUA :
			goto_night_tsinghua();
			break;
		case GAME_JOKERPIG :
			goto_night_jokerpig();
			break;
	}
}
void goto_night_original()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1m夜幕降临了...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
				{
					if (RINFO.nokill)
						goto_night_msg(i, "尖刀","吓唬");
					else
						goto_night_msg(i, "尖刀","残害");
				}
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "警棒","查看");
			}
		goto_night_common();
 
	}
}

void goto_night_crow()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1m夜幕降临了...\x1b[m");
		kill_msg(-1);
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
					goto_night_msg(i, "尖刀","残害");
				else if (PINFO(i).flag & PEOPLE_MAGIC)
					goto_night_msg(i, "法杖","施法");
				else if (PINFO(i).flag & PEOPLE_FOREST)
					goto_night_msg(i, "树枝","禁言");
				else if (PINFO(i).flag & PEOPLE_BADMAN)
					goto_night_msg(i, "证书","陷害");
				else if (PINFO(i).flag & PEOPLE_BUTTERFLY)
					goto_night_msg(i, "翅膀","拥抱");
				else if (PINFO(i).flag & PEOPLE_SHOOTER)
					goto_night_msg(i, "弓箭","消灭");
				else if (PINFO(i).flag & PEOPLE_DOCTOR)
					goto_night_msg(i, "针筒","注射");
				else if (PINFO(i).flag & PEOPLE_POLICE)
					goto_night_msg(i, "警棒","查看");
				else if (PINFO(i).flag & PEOPLE_GOODMAN)
					goto_night_msg(i, "图纸","算计");
				else if (PINFO(i).flag & PEOPLE_CROW)
					goto_night_msg(i, "眼睛","观察");
			}
		for (i=0;i<MAX_PLAYER;i++)
		{
			if (PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).style != -1)
			{
				send_msg(i,"\x1b[31;1m如果你选择今夜不行动,先按,\x1b[31;1mctrl-U\x1b[m再按,\x1b[31;1mctrl-T确认...\x1b[m");
				kill_msg(i);
			}
		}
		goto_night_common();
 
	}
}
void goto_night_butterfly()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1m夜幕降临了...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
						goto_night_msg(i, "尖刀","残害");
				else if (PINFO(i).flag & PEOPLE_FOREST)
						goto_night_msg(i, "树枝","禁言");
				else if (PINFO(i).flag & PEOPLE_BUTTERFLY)
						goto_night_msg(i, "翅膀","拥抱");
				else if (PINFO(i).flag & PEOPLE_SHOOTER)
						goto_night_msg(i, "弓箭","消灭");
				else if (PINFO(i).flag & PEOPLE_DOCTOR)
						goto_night_msg(i, "针筒","注射");
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "警棒","查看");
				
			}
		goto_night_common();
 
	}
}
void goto_night_tsinghua()
{
	int i;
	if (!check_win())
	{
		send_msg(-1,"\x1b[31;1m夜幕降临了...\x1b[m");
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE))
			{
				if (PINFO(i).flag & PEOPLE_KILLER)
						goto_night_msg(i, "尖刀","残害");
				else if (PINFO(i).flag & PEOPLE_POLICE)
						goto_night_msg(i, "警棒","查看");
			}
		goto_night_common();
	}
}

void goto_night_msg(int me, char * tool, char * act)
{
	char  buf [200];
//	if (RINFO.gametype !=4)
		sprintf (buf, "用你的%s(\x1b[31;1mCtrl+S\x1b[m)选择要%s的人,\x1b[31;1mCtrl+T\x1b[m确认", tool, act);
//	else 
//		sprintf (buf, "用你的%s(\x1b[31;1mCtrl+S\x1b[m)选择要%s的人", tool, act);
		
	send_msg(me, buf);
}
	
void goto_night_common()
{
	int i;
	RINFO.day++;
	RINFO.victim=-1;
	RINFO.turn=-1;
	RINFO.round=-1;
//	RINFO.informant=-1;
	for (i=0;i<MAX_ROLE;i++)
	{
		RINFO.action_target_tmp[i]=-1;
	}
	for(i=0;i<MAX_PLAYER;i++)
	{
		RINFO.action_target[i]=-1;//用于记载清华游戏中警察的行动

		PINFO(i).vote=-1;
		PINFO(i).flag &=~PEOPLE_VOTED;
		PINFO(i).flag &=~PEOPLE_MULE;
		PINFO(i).flag &=~PEOPLE_LASTWORDS;
		RINFO.voted[i]=-1;
		PINFO(i).vnum=0;
	}
	RINFO.status=INROOM_NIGHT; 
}

