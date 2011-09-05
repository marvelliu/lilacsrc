void ctrl_t_check(int selected)
{
	int gametype;
	int sel;
	gametype=RINFO.gametype;
	sel=getpeople(selected);
	if (gametype==1)
		ctrl_t_check_original();
	else if (gametype==2)
		ctrl_t_check_crow();
	else if (gametype==3)
		ctrl_t_check_butterfly();
//	else if (gametype==4)
//		ctrl_t_check_tsinghua(sel);
}
void ctrl_t_check_tsinghua(int sel)
{
	int i,j;
	char buf[200];
	j=RINFO.action_target[mypos];
	if (PINFO(mypos).flag&PEOPLE_POLICE && PINFO(mypos).flag & PEOPLE_ALIVE && PINFO(mypos).style!=-1)
	{
		if (PINFO(sel).flag & PEOPLE_ALIVE && PINFO(sel).style != -1 && sel!=mypos)
		{
			if (j>0 )
			{
				sprintf(buf, "今天已经验证过%d了。", j+1);
				send_msg(mypos, buf);
			}
			else if (PINFO(sel).flag & PEOPLE_KILLER)
			{
				sprintf(buf, "啊，他身上有一把西瓜刀，%d %s就是一个杀手！",sel+1,PINFO(sel).nick);
				send_msg(mypos, buf);
			}
			else if (PINFO(sel).flag & PEOPLE_POLICE)
			{
				sprintf(buf, "啊，%d %s似乎是同行！",sel+1,PINFO(sel).nick);
				send_msg(mypos, buf);
			}
			else
			{
				sprintf(buf, "啊，%d %s似乎是好人啊！",sel+1,PINFO(sel).nick);
				send_msg(mypos, buf);
			}

			kill_msg(mypos);
			RINFO.action_target[mypos]=sel;
			for (i=0;i<RINFO.policenum; i++)
			{
				if (i==mypos)
					break;
			}
			if (i<RINFO.policenum)
				RINFO.seq_action[RINFO.day][i]=sel;
		}
	}
}
			
void ctrl_t_check_crow()
{
	int k;
	if (RINFO.turn==mypos && (RINFO.voted[mypos]!=-1 ||RINFO.surrenderflag>0))
	{
				
		ctrl_t_check_common();
		if (!check_win())
		{
			PINFO(mypos).vote=RINFO.voted[mypos];
			PINFO(RINFO.voted[mypos]).flag|=PEOPLE_VOTED;
			PINFO(RINFO.voted[mypos]).vnum++;
			if (!look_for_next_speaker (RINFO.turn,RINFO.victim,0))
							goto_check_speak();
			else
			{
				
				k=kill_surrender();
				if (k>0)
				{
					send_msg(-1,"\33[31;1m杀手崩溃了，取消辩护和投票\33[m");
					kill_msg(-1);
					goto_night();
				}
				else
					vote_result();
			}
		}
	}
	kill_msg(-1);
}
void ctrl_t_check_butterfly()
{
	ctrl_t_check_crow();
}

void ctrl_t_check_original()
{
	int i;
	if ( RINFO.voted[mypos] != -2)
		RINFO.voted[mypos]=mypos;
	if (RINFO.turn==mypos && (RINFO.voted[mypos]!=-1 ||RINFO.surrenderflag>0))
	{
				
		ctrl_t_check_common();
		if (!check_win())
		{
			PINFO(RINFO.voted[mypos]).flag|=PEOPLE_VOTED;
			for(i=0;i<MAX_PLAYER;i++)
				RINFO.voted[i]=-1;
			if (!look_for_next_speaker (RINFO.turn,RINFO.action_target[S_KILLER],0))
							goto_check_speak();
			else
			{
				RINFO.round=1;
				goto_defence();
			}
		}
	}
	kill_msg(-1);
}
void ctrl_t_check_common()
{
	char buf[200];
	int gametype;
	gametype=RINFO.gametype;
	if (announce_surrender())	{}
	else if (RINFO.voted[mypos]<0)
	{
		sprintf(buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
		send_msg(-1, buf);
	}
	
	else if (gametype==1)
	{
//		sprintf (buf, "\x1b[32;1m%d %s\x1b[m 指证 %d %s\33[m",	mypos + 1, PINFO(mypos).nick,RINFO.voted[mypos]+1,PINFO(RINFO.voted[mypos]).nick);
//		send_msg(-1, buf);
		sprintf(buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
		send_msg(-1, buf);
	}
	else if (gametype==2 || gametype ==3 )
	{
		sprintf (buf, "\x1b[32;1m%d %s\x1b[m 投 %d %s一票\33[m",	mypos + 1, PINFO(mypos).nick,RINFO.voted[mypos]+1,PINFO(RINFO.voted[mypos]).nick);
		send_msg(-1, buf);
		sprintf(buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
		send_msg(-1, buf);
	}
	
	kill_msg(-1);
}

int announce_surrender ()
{
	int i;
	char buf[200];
	if ((PINFO(mypos).flag & PEOPLE_BAD) && -2==RINFO.voted[mypos])
	{
			PINFO(mypos).flag|=PEOPLE_SURRENDER;
			i=rand()%MAX_POEM;
			sprintf(buf, "\x1b[32;1m%d %s\x1b[m: %s", mypos + 1, PINFO(mypos).nick, poem[i]);
			send_msg(-1,buf);
			sprintf (buf, "\x1b[32;1m%d %s\x1b[m 崩溃了...\33[m", mypos + 1, PINFO(mypos).nick);
			send_msg(-1,buf);
			RINFO.surrenderflag=1;
			kill_msg(-1);
			return 1;
	}
	return 0;
}
	
