int check_win()
{
	int gametype = RINFO.gametype;
	if (gametype == 1)
		return check_win_original();
	else if (gametype == 2)
		return check_win_crow();
	else if (gametype == 3)
		return check_win_butterfly();
	else if (gametype == 4)
		return check_win_tsinghua();
	else if (gametype == 5)
		return check_win_joker1();
}
int check_win_crow()
{
	int tc,tk,tp;
	int tr[3];
	check_win_census(tr);
	tc=tr[2];//乌鸦
	tk=tr[0];//坏人
	tp=tr[1];//好人
	if (tk==0)
	{
		// 好人胜利
		check_win_goodwin();
		return 1;
	}
	else if (tp==0 )
	{
		// 坏人胜利
		check_win_badwin1();
		return 2;
	}
	else if (RINFO.day>=MAX_DAY-1 || 
			(tk==0 && tp==0) )
	{
		if (tc>0)
			check_win_crowwin();
		else
			check_win_draw();
		return 3;
	}
	else if (tk+tp<=2)
	{
		if (RINFO.draw_counter >=1) 
		{
			if (tc>0)
				check_win_crowwin();
			else
				check_win_draw();
			return 3;
		}
		else
			RINFO.draw_counter++;
	}

	return 0;
}
int check_win_butterfly()
{
	int tc,tk,tp;
	int tr[3];
	check_win_census(tr);
	tc=tr[2];//平民
	tk=tr[0];//坏人
	tp=tr[1];//好人特殊身份
	if (tk==0 && tp>0)
	{
		// 好人胜利
		check_win_goodwin();
		return 1;
	}
	else if (tp==0 && tk>0)
	{
		// 坏人胜利
		check_win_badwin1();
		return 2;
	}
	else if (tc==0 && RINFO.nokill==0)
	{
		check_win_badwin2();
		return 2;
	}	
	else if (RINFO.day>=MAX_DAY-1 || 
			(tk==0 && tp==0)  )
	{
		check_win_draw();
		return 3;
	}
	else if (tc==0 && tk+tp<=2)
	{
		if (RINFO.draw_counter >=1) 
		{
			check_win_draw();
			return 3;
		}
		else if (RINFO.day != RINFO.draw_check_day)
		{
			RINFO.draw_counter++;
			RINFO.draw_check_day=RINFO.day;
		}
	}
			
	return 0;
}

int check_win_original()
{
	int tc,tk,tp;
	int tr[3];
	check_win_census(tr);
	tc=tr[2];
	tk=tr[0];
	tp=tr[1];
	if (tk==0)
	{
		// 好人胜利
		check_win_goodwin();
		return 1;
	}
	else if (tp==0 || tc==0 || tk>=tp+tc)
	{
		// 坏人胜利
		if (tp==0)
			check_win_badwin1();
		else if (tc==0)
			check_win_badwin2();
		else if (tk>=tp+tc)
			check_win_badwin3();
		return 2;
	}
	else if (RINFO.day >=MAX_DAY-1)
	{
		check_win_draw();
		return 3;
	}
	return 0;
}
int check_win_tsinghua()
{
	int tc,tk,tp;
	int tr[3];
	check_win_census(tr);
	tc=tr[2];
	tk=tr[0];
	tp=tr[1];
	if (tk==0)
	{
		// 好人胜利
		check_win_goodwin();
		return 1;
	}
	else if ( RINFO.policenum > 1 && tp == 0 )
	//如果警察超过2个，则警察死光，坏人胜利 
	{
		check_win_badwin1();
		return 2;
	}
	else if ( tc == 0 )
	//如果平民死光，坏人胜利
	{
		check_win_badwin2();
		return 2;
	}
	else if (tk>=tp+tc)
	{
		// 坏人胜利
		check_win_badwin3();
		return 2;
	}
	else if (RINFO.day >=MAX_DAY-1)
	{
		check_win_draw();
		return 3;

	}
	return 0;
}

void check_win_showkiller ()
{
	int i;
	char buf[200];
	for (i = 0; i < MAX_PLAYER; ++i)
		if (PINFO(i).style != -1
			&& (PINFO(i).flag & PEOPLE_BAD)
			&& (PINFO(i).flag & PEOPLE_ALIVE))
		{
			sprintf (buf, "原来\33[32;1m %d %s \33[m是坏人!", i + 1, PINFO(i).nick);
			send_msg (-1, buf);
		}
}
void check_win_census(int * result)
{
	int i, tk, tp, tc;
	tk=tp=tc=0;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && !(PINFO(i).flag & PEOPLE_LASTWORDS))
		{
			if (PINFO(i).flag & PEOPLE_BAD)
			{
				if (!(PINFO(i).flag & PEOPLE_SURRENDER))
					tk++;
			}
			else
				if (PINFO(i).flag & PEOPLE_GOOD)
					tp++;
				else
					tc++;
		}
	result[0]=tk;
	result[1]=tp;
	result[2]=tc;
#ifdef KILLER_DEBUG
	char buf[200];
	sprintf (buf, "平民%d,杀手%d,警察%d", tc,tk,tp);
	send_msg(-1,buf);
	kill_msg(-1);
#endif
}

void check_win_goodwin()
{
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m所有坏人都被处决了，好人获得了胜利...\33[m");
		show_killerpolice();
		save_result (0);
}
void check_win_badwin1()
{
	int gametype;
	RINFO.status=INROOM_STOP;
	gametype=RINFO.gametype;
	if ( gametype==1 || gametype==4 )
		send_msg (-1, "\33[31;1m警察全部牺牲，坏人获得了胜利...\33[m");
	else if (gametype ==2)
		send_msg (-1, "\33[31;1m好人全部牺牲，坏人获得了胜利...\33[m");
	else if (gametype ==3)
		send_msg (-1, "\33[31;1m好人特殊身份全部牺牲，坏人获得了胜利...\33[m");
	check_win_showkiller();
	show_killerpolice();
	save_result (1);
}
void check_win_badwin2()
{
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m无辜的平民全部死亡，坏人获得了胜利...\33[m");
		check_win_showkiller();
		show_killerpolice();
		save_result (1);
}
void check_win_badwin3()
{
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m好人没有坏人多，坏人获得了胜利...\33[m");
		check_win_showkiller();
		show_killerpolice();
		save_result (1);
}
void check_win_draw()
{
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m游戏以平局告终...\33[m");
		show_killerpolice();
		save_result (1);
}
void check_win_crowwin()
{
		RINFO.status=INROOM_STOP;
		send_msg (-1, "\33[31;1m游戏以平局告终，乌鸦胜利了...\33[m");
		show_killerpolice();
		save_result (1);
}



