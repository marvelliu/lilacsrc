


void ctrl_s_callback (int selected)
{
	int sel;
	sel = getpeople(selected);
	if (sel!=-1 && sel<MAX_PLAYER && PINFO(sel).style!=-1)
	{
		switch(RINFO.status)
		{
			case INROOM_STOP:
					break;
			case INROOM_NIGHT:
					ctrl_s_night(sel);
					break;
			case INROOM_DAY:
					break;
			case INROOM_CHECK:
					ctrl_s_check(sel);
					break;
			case INROOM_DEFENCE:
					ctrl_s_defence(sel);
					break;
			case INROOM_VOTE:
					ctrl_s_vote(sel);
					break;
			case INROOM_DARK:
					break;
		}
	}
}
void ctrl_s_vote (int sel)
{
	int gametype;
	char buf[200];
	gametype=RINFO.gametype;
	if ((PINFO(sel).flag & PEOPLE_VOTED)&& PINFO(mypos).vote==-1 && gametype==1 )
	{
		RINFO.voted[mypos]=sel;
		sprintf (buf, "想投票给\33[32;1m%d %s \33[m吗?", sel + 1, PINFO(sel).nick);
		send_msg(mypos,buf);
		kill_msg(mypos);
	}
}

void ctrl_s_defence (int sel)
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			ctrl_s_defence_original(sel);
				break;
		case GAME_JOKER1 :
			ctrl_s_defence_joker1();
				break;
		case GAME_JOKERPIG :
			ctrl_s_defence_jokerpig();
				break;
	}
}

void ctrl_s_defence_original (int sel)
{
	char buf[200];
	if (RINFO.turn==mypos&& (PINFO(sel).flag & PEOPLE_ALIVE) && sel==mypos
			&& PINFO(sel).flag & PEOPLE_KILLER )
			sprintf (buf, "\33[31m崩溃请用ctrl-U\33[m");
}

	
void ctrl_s_check (int sel)
{
	int gametype;
	gametype=RINFO.gametype;

	if ((RINFO.turn==mypos||gametype==4) && PINFO(sel).flag & PEOPLE_ALIVE && PINFO(sel).style != -1 )
	{
		switch (gametype)
		{
			case GAME_ORIGINAL :
//				ctrl_s_check_original(sel);
				break;
			case GAME_CROW :
				ctrl_s_check_crow(sel);
				break;
			case GAME_BUTTERFLY :
				ctrl_s_check_butterfly(sel);
				break;
			case GAME_TSINGHUA :
				ctrl_s_check_tsinghua(sel);
				break;
		}
	}
}

void ctrl_s_check_original(int sel)
{		
	char buf[200];
	if (mypos==sel)
	{
		if (PINFO(mypos).flag & PEOPLE_KILLER)
			sprintf (buf, "\33[31m崩溃请用ctrl-U\33[m");
		else
			sprintf(buf,"不能指证自己");
	}
	else
	{
		sprintf (buf, "想指证\33[32;1m%d %s \33[m吗?", sel + 1, PINFO(sel).nick);
		RINFO.voted[mypos]=mypos;
	}
	send_msg(mypos,buf);
	kill_msg(mypos);
}

void ctrl_s_check_crow(int sel)
{		
	char buf[200];
	if (PINFO(mypos).flag & PEOPLE_CROW)
		sprintf(buf,"装做投谁的票呢?");
	else if (mypos==sel)
		sprintf(buf,"投自己一票？");
	else
		sprintf (buf, "想投\33[32;1m%d %s \33[m一票吗?", sel + 1, PINFO(sel).nick);
	RINFO.voted[mypos]=sel;
	send_msg(mypos,buf);
	kill_msg(mypos);
}
void ctrl_s_check_butterfly(int sel)
{		
	char buf[200];
	if (mypos==sel)
		sprintf(buf,"投自己一票？");
	else
		sprintf (buf, "想投\33[32;1m%d %s \33[m一票吗?", sel + 1, PINFO(sel).nick);
	RINFO.voted[mypos]=sel;
	send_msg(mypos,buf);
	kill_msg(mypos);
}
void ctrl_s_check_tsinghua(int sel)
{
	char buf[200];
	int i;
	if (mypos==sel)
	{
		send_msg(mypos, "你不能投自己。");
		kill_msg(mypos);
		return;
	}
	if (!check_win() && PINFO(mypos).vote<0)
	{
		sprintf (buf, "\33[32;1m你投了%d %s 一票。\33[m", sel+1, PINFO(sel).nick);
		send_msg(mypos, buf);
		sprintf (buf, "\33[32;1m%d %s 投了%d %s 一票。\33[m", mypos+1, PINFO(mypos).nick, sel+1, PINFO(sel).nick);
		send_msg(-1, buf);
		RINFO.voted[mypos]=sel;
		PINFO(mypos).vote=RINFO.voted[mypos];
		PINFO(PINFO(mypos).vote).flag|=PEOPLE_VOTED; 
		PINFO(RINFO.voted[mypos]).vnum++;
	}

	kill_msg(-1);
//	for (i=0;i<MAX_PLAYER;i++)
//		if (PINFO(i).style != -1 && PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).vote == -1)
//				break;
//	if (i>=MAX_PLAYER) 
			vote_result();
					
}
			
	
void ctrl_s_night (int sel)
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			ctrl_s_night_original(sel);
			break;
		case GAME_CROW:
			ctrl_s_night_crow(sel);
			break;
		case GAME_BUTTERFLY:
			ctrl_s_night_butterfly(sel);
			break;
		case GAME_TSINGHUA:
			ctrl_s_night_tsinghua(sel);
			break;
		case GAME_JOKERPIG:
			ctrl_s_night_jokerpig(sel);
			break;
	}
}
void ctrl_s_night_original(int sel)
{
	if (ctrl_s_night_common(sel, PEOPLE_KILLER, S_KILLER, 0, "砍"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_POLICE, S_POLICE, PEOPLE_POLICE|PEOPLE_TESTED, "查"))
	{}
}

void ctrl_s_night_crow(int sel)
{
	if (ctrl_s_night_common(sel, PEOPLE_KILLER, S_KILLER, 0, "砍"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_MAGIC, S_MAGIC, PEOPLE_MAGIC, "魔"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_FOREST, S_FOREST, 0, "哑"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_BADMAN, S_BADMAN, 0, "投"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_BUTTERFLY, S_BUTTERFLY, PEOPLE_BUTTERFLY, "抱"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_SHOOTER, S_SHOOTER, 0, "射"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_DOCTOR, S_DOCTOR, 0, "扎"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_POLICE, S_POLICE, PEOPLE_POLICE|PEOPLE_TESTED, "查"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_GOODMAN, S_GOODMAN, 0, "投"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_CROW, S_CROW, S_CROW, "看"))
	{}
}
void ctrl_s_night_butterfly(int sel)
{
	if (ctrl_s_night_common(sel, PEOPLE_KILLER, S_KILLER, 0, "砍"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_FOREST, S_FOREST, 0, "哑"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_BUTTERFLY, S_BUTTERFLY, PEOPLE_BUTTERFLY, "抱"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_SHOOTER, S_SHOOTER, 0, "射"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_DOCTOR, S_DOCTOR, 0, "扎"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_POLICE, S_POLICE, PEOPLE_POLICE, "查"))
		//警察可以查查过的人，因为可能都查过了
	{}
}
void ctrl_s_night_tsinghua(int sel)
{
	if (ctrl_s_night_common(sel, PEOPLE_KILLER, S_KILLER, 0, "砍"))
	{}
	else if (ctrl_s_night_common(sel, PEOPLE_POLICE, S_POLICE, PEOPLE_POLICE, "查"))
	{}
/*	char buf[200];
	int i;
	if (PINFO(mypos).flag & PEOPLE_KILLER  && PINFO(sel).flag & PEOPLE_ALIVE && PINFO(sel).style !=-1)
	{
		sprintf (buf, "好，就砍\33[32;1m%d %s \33[m!", sel+1, PINFO(sel).nick);
		send_msg2(PEOPLE_KILLER, buf);
		RINFO.action_target[PEOPLE_KILLER]=sel;
		start_change_inroom();
		sprintf (buf, "\33[1;33m阴森森的月光下，横躺着\33[32;1m %d %s \33[33m的尸体...\33[m",RINFO.action_target[S_KILLER]+ 1, PINFO(RINFO.action_target[S_KILLER]).nick);
		send_msg(-1,buf);
		send_msg(-1, "天亮了。");
		kill_msg(-1);
		i=RINFO.action_target[S_KILLER];
		PINFO(i).flag &=~PEOPLE_ALIVE;
		if (PINFO(i).flag & PEOPLE_KILLER)
			PINFO(i).flag |= PEOPLE_HIDEKILLER;
		for(i=0;i<MAX_PLAYER;i++)
			RINFO.voted[i]=-1;
		
		goto_check();
		end_change_inroom();
		kill_msg(-1);
		RINFO.seq_action[RINFO.policenum][RINFO.day]=RINFO.action_target[PEOPLE_KILLER];
		RINFO.action_target[PEOPLE_KILLER]=-1;
	}
*/
}

int ctrl_s_night_common(int sel, long people_type, int s_type, int forbid_type, char * verb)
{
	char buf[200];
	if ((PINFO(mypos).flag & people_type)
			&& RINFO.action_target[s_type]==-1 
			&& (PINFO(sel).flag & PEOPLE_ALIVE)
			&& !(PINFO(sel).flag & forbid_type))
	{
		RINFO.action_target_tmp[s_type]=sel;
		sprintf (buf, "想%s\33[32;1m%d %s \33[m吗?", verb, sel + 1, PINFO(sel).nick);
		send_msg2(people_type, buf);
		return 1;
	}
	return 0;
}

	

