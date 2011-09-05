void vote_result()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			vote_result_original();
			break;
		case GAME_CROW :
			vote_result_crow();
			break;
		case GAME_BUTTERFLY :
			vote_result_butterfly();
			break;
		case GAME_TSINGHUA :
			vote_result_tsinghua();
			break;
		case GAME_JOKERPIG :
			vote_result_jokerpig();
	}
}
void vote_result_original()
{
	int i;
	int k;
	int mr[2];
	start_change_inroom();
	//投票阶段
	if (!(RINFO.status==INROOM_VOTE))
	{
		end_change_inroom();
		return;
	}
	//还有人没投票
	for (i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 && PINFO(i).vote==-1)
		{
			end_change_inroom();
			return;
		}
				  
	int m1,m2,tt;
	tt=0;
	//显示投票结果,计算票数
	tt=show_vote_result_list();
	//统计最高票和次高票
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1*2>tt)
	{
		// 直接投死
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
					&& (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //标记死人的方法?假如m1掉线了呢？
		goto_dark();
	}
	else
	{
		// 未超过1/2
		tt=0;
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
					&& (PINFO(i).flag & PEOPLE_VOTED))
			{
				if (PINFO(i).vnum!=m1 && PINFO(i).vnum!=m2)
					PINFO(i).flag &=~PEOPLE_VOTED;
				else
					tt++;
			}
		if (RINFO.round>3)
		{
			//超过3轮,随机处决
			send_msg(-1, "\33[31;1m投票三次仍未有结果，法官随机处决\33[m");
			k=rand()%tt;
			for(i=0;i<MAX_PLAYER;i++)
				if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
						&& (PINFO(i).flag & PEOPLE_VOTED))
				{
					if (k==0)
						break;
					k--;			
				}
			RINFO.victim=i;
			goto_dark();
		}
		else
		{
			send_msg (-1, "\33[31;1m最高票数未超过半数,请获得最多票数的人辩护\33[m");
			while (m1>0)
			{
				tt=0;
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) 
							&& (PINFO(i).flag & PEOPLE_VOTED))
					{
						if (PINFO(i).vnum==m1)
							tt++;
					}
				if (tt>0)
					break;
				m1--;
			}
			if (m1>0 && tt>0)
			{
				k=rand()%tt;
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE)
							&& (PINFO(i).flag & PEOPLE_VOTED)&& (PINFO(i).vnum==m1))
					{
						if (k==0)
							break;
						k--;			
					}
				RINFO.victim=i;
				goto_defence();
			}
			else
			{
				RINFO.victim=-1;
				goto_defence();
			}
		}
	}
	kill_msg(-1);
	end_change_inroom();
}

void vote_result_crow ()
{
	int i;
	int v,m1,m2;
	int mr[2];
	start_change_inroom();
	//投票阶段:由最后一个指证轮发言的人的ctrl-t引发vote_result_crow
	int tt;
	tt=show_vote_result_list();
	//加入暗票
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (RINFO.secretvotelist[i]==-1)
			break;
		if (RINFO.secretvotelist[i]>=0) //-2 表示不作为或者无效
		{
			v=RINFO.secretvotelist[i];
			if (PINFO(v).flag & PEOPLE_ALIVE)
				PINFO(v).vnum++;
		}
	}
	//清除乌鸦身上的票
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).flag & PEOPLE_CROW)
			PINFO(i).vnum=0;
	}
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1==m2) 
	{
		send_msg(-1, "平票，不死人。");
		kill_msg(-1);
		goto_night();
	}
	else
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //标记死人的方法?假如m1掉线了呢？
		goto_dark();
	}
	end_change_inroom();
}
void vote_result_butterfly()
{
	int i;
	int m1,m2;
	int mr[2];
	start_change_inroom();
	//投票阶段:由最后一个指证轮发言的人的ctrl-t引发vote_result_crow
	int tt;
	tt=show_vote_result_list();
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1==m2) 
	{
		send_msg(-1, "平票，不死人。");
		kill_msg(-1);
		goto_night();
	}
	else
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
#ifdef KILLER_DEBUG
		sprintf (buf, "投票情况, 最高票是%d的%d票",i+1, m1);
		send_msg(-1,buf);
		kill_msg(-1);
#endif
		RINFO.victim=i; //标记死人的方法?假如m1掉线了呢？
		goto_dark();
	}
	end_change_inroom();
}
void vote_result_tsinghua()
{
	int i;
	int m1=0,m2=0,mo=0;
//	int mr[2];
	char buf[200];
	start_change_inroom();
	int tt;
//	vote_result_highest2(mr);
	
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
		{
			if (PINFO(i).vnum>m1)
			{
				m2=m1;
				m1=PINFO(i).vnum;
			}
			else if (PINFO(i).vnum>m2)
				m2=PINFO(i).vnum;
		}
		if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).vote==-1)
		{
			mo++;
		}
	}
//	m1=mr[0];
//	m2=mr[1];
	if (m1==m2 && mo==0) 
	{
		tt=show_vote_result_list();
//			send_msg(-1, "\33[31;1m投票三次仍未有结果，法官随机处决\33[m");
		sprintf(buf, "\33[1;31m最高票%d, 次高票%d, 未投票%d\33[m", m1, m2, mo);
		send_msg(-1, buf);
		send_msg(-1, "\33[1;31m平票，重新投票。\33[m");

		for(i=0;i<MAX_PLAYER;i++)
		{
			PINFO(i).vote=-1;
			PINFO(i).flag &=~PEOPLE_VOTED;
			RINFO.voted[i]=-1;
			PINFO(i).vnum=0;
		}
		kill_msg(-1);
		goto_check();
		end_change_inroom();
	}
	else if ( m1 > m2+mo )
	{
		sprintf(buf, "\33[1;31m最高票%d, 次高票%d, 未投票%d\33[m", m1, m2, mo);
		send_msg(-1, buf);
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //标记死人的方法?假如m1掉线了呢？
		tt=show_vote_result_list();
		//PINFO(i).flag &=~PEOPLE_ALIVE;
//		sprintf (buf, "%d %s被投死了。 ",i+1, PINFO(i).nick);
//		send_msg(-1, buf);
		kill_msg(-1);
		goto_dark();
		end_change_inroom();
	}
}
		

		

int show_vote_result_list()
{
	int i;
	char buf[200];
	int tt;
	tt=0;
	send_msg (-1, "\33[33;1m投票结果如下:\33[m");
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE))// && PINFO(i).vote!=-1)
		{
			if ( PINFO(i).vote != -1 )
			{
				sprintf(buf,"\33[32;1m%d %s 投了 %d %s 一票\33[m",i+1,PINFO(i).nick,
					PINFO(i).vote+1,PINFO(PINFO(i).vote).nick);
			}
			else
			{
				sprintf(buf,"\33[32;1m%d %s 无需投票\33[m",i+1,PINFO(i).nick);
			}
			send_msg(-1,buf);
			PINFO(i).vnum=0;
			tt++;
		}
		
	if ( RINFO.gametype != GAME_TSINGHUA )
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE))
				PINFO(PINFO(i).vote).vnum++;
	}
	return tt;
}
void vote_result_highest2 ( int * result)
{
	int i, m1, m2;
	m1=-1;
	m2=-1;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
		{
			if (PINFO(i).vnum>m1)
			{
				m2=m1;
				m1=PINFO(i).vnum;
			}
			else if (PINFO(i).vnum>m2)
				m2=PINFO(i).vnum;
		}
	result[0]=m1;
	result[1]=m2;
}

