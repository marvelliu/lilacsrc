void ctrl_t_night()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			ctrl_t_night_original();
			break;
		case GAME_CROW :
			ctrl_t_night_crow();
			break;
		case GAME_BUTTERFLY :
			ctrl_t_night_butterfly();
			break;
		case GAME_TSINGHUA:
			ctrl_t_night_tsinghua();
			break;
		case GAME_JOKERPIG:
			ctrl_t_night_jokerpig();
	}
}
void ctrl_t_night_tsinghua()
{
	char buf[200];
	int i;
	if (ctrl_t_night_common(PEOPLE_KILLER, S_KILLER, "砍"))
	{
		RINFO.seq_action[S_KILLER][RINFO.day]=RINFO.action_target[S_KILLER];
	}
	else if (ctrl_t_night_common(PEOPLE_POLICE, S_POLICE, "查"))
			answer_result_police();
	else
		return;
	
	start_change_inroom();
	if (RINFO.status==INROOM_NIGHT && night_over(PEOPLE_POLICE, S_POLICE) && night_over(PEOPLE_KILLER, S_KILLER))
	{
		
		// turn to check
	//	if (RINFO.nokill)
	//		sprintf (buf, "\33[1;33m注意，\33[32;1m %d %s \33[33m被盯上了...\33[m", RINFO.action_target[S_KILLER] + 1, PINFO(RINFO.action_target[S_KILLER]).nick);
	//	else
		{
			sprintf (buf, "\33[1;33m阴森森的月光下，横躺着\33[32;1m %d %s \33[33m的尸体...\33[m",RINFO.action_target[S_KILLER]+ 1, PINFO(RINFO.action_target[S_KILLER]).nick);
			PINFO(RINFO.action_target[S_KILLER]).flag |=PEOPLE_LASTWORDS;
		}
		send_msg(-1,buf);
		for(i=0;i<MAX_PLAYER;i++)
			RINFO.voted[i]=-1;
		goto_day();
			 
	}
	end_change_inroom();
	kill_msg(-1);
}
void ctrl_t_night_crow()
{
	//int i;
	char buf[200];//, buf2[200];
	if (ctrl_t_night_common(PEOPLE_KILLER, S_KILLER, "砍")){}
	else if (ctrl_t_night_common(PEOPLE_MAGIC, S_MAGIC, "魔")){}
	else if (ctrl_t_night_common(PEOPLE_FOREST, S_FOREST, "禁言")){}
	else if (ctrl_t_night_common(PEOPLE_BADMAN, S_BADMAN, "投")){}
	else if (ctrl_t_night_common(PEOPLE_BUTTERFLY, S_BUTTERFLY, "抱")){}
	else if (ctrl_t_night_common(PEOPLE_SHOOTER, S_SHOOTER, "射")){}
	else if (ctrl_t_night_common(PEOPLE_POLICE, S_POLICE, "查"))
	{
		sprintf (buf, "好，那么天亮前就会知道结果");
		PINFO(RINFO.action_target[S_POLICE]).flag |= PEOPLE_TESTED;
		send_msg2(PEOPLE_POLICE,buf);
	}
	else if (ctrl_t_night_common(PEOPLE_DOCTOR, S_DOCTOR, "扎")){}
	else if (ctrl_t_night_common(PEOPLE_GOODMAN, S_GOODMAN, "投")){}
	else if (ctrl_t_night_common(PEOPLE_CROW, S_CROW, "查"))
	{
		sprintf (buf, "好，那么天亮前就会知道结果");
		send_msg2(PEOPLE_CROW,buf);
	}
	else
		return;

	start_change_inroom();
	if (RINFO.status==INROOM_NIGHT && night_over(PEOPLE_KILLER, S_KILLER) 
			&& night_over(PEOPLE_MAGIC, S_MAGIC)
			&& night_over(PEOPLE_FOREST, S_FOREST)
			&& night_over(PEOPLE_BADMAN, S_BADMAN)
			&& night_over(PEOPLE_BUTTERFLY, S_BUTTERFLY)
			&& night_over(PEOPLE_SHOOTER, S_SHOOTER)
			&& night_over(PEOPLE_POLICE, S_POLICE)
			&& night_over(PEOPLE_DOCTOR, S_DOCTOR)
			&& night_over(PEOPLE_GOODMAN, S_GOODMAN)
			&& night_over(PEOPLE_CROW, S_CROW))
	{
		//首先回答乌鸦的问题
		answer_result_crow();
		//然后处理各种情况
		night_result_crow();
		//然后回答警察的问题
		answer_result_police();
		announce_death_mule();
		if (RINFO.deadnum==0)
				goto_check();
		else
				goto_day();
	}
	end_change_inroom();
	kill_msg(-1);

}
void ctrl_t_night_butterfly()
{
	//int i;
	char buf[200];//, buf2[200];
	if (ctrl_t_night_common(PEOPLE_KILLER, S_KILLER, "砍")){}
	else if (ctrl_t_night_common(PEOPLE_FOREST, S_FOREST, "禁言")){}
	else if (ctrl_t_night_common(PEOPLE_BUTTERFLY, S_BUTTERFLY, "抱")){}
	else if (ctrl_t_night_common(PEOPLE_SHOOTER, S_SHOOTER, "射")){}
	else if (ctrl_t_night_common(PEOPLE_POLICE, S_POLICE, "查"))
	{
		sprintf (buf, "好，那么天亮前就会知道结果");
		PINFO(RINFO.action_target[S_POLICE]).flag |= PEOPLE_TESTED;
		send_msg2(PEOPLE_POLICE,buf);
	}
	else if (ctrl_t_night_common(PEOPLE_DOCTOR, S_DOCTOR, "扎")){}
	else
		return;

	start_change_inroom();
	if (RINFO.status==INROOM_NIGHT && night_over(PEOPLE_KILLER, S_KILLER) 
			&& night_over(PEOPLE_FOREST, S_FOREST)
			&& night_over(PEOPLE_BUTTERFLY, S_BUTTERFLY)
			&& night_over(PEOPLE_SHOOTER, S_SHOOTER)
			&& night_over(PEOPLE_POLICE, S_POLICE)
			&& night_over(PEOPLE_DOCTOR, S_DOCTOR))
	{
		//然后处理各种情况
		night_result_butterfly();
		//然后回答警察的问题
		answer_result_police();
		//宣布禁言和死人
		announce_death_mule();
		if (RINFO.deadnum==0)
				goto_check();
		else
				goto_day();
	}
	end_change_inroom();
	kill_msg(-1);

}



void ctrl_t_night_original()
{
	char buf[200];
	int i;
	if (ctrl_t_night_common(PEOPLE_KILLER, S_KILLER, "砍")){}
	else if (ctrl_t_night_common(PEOPLE_POLICE, S_POLICE, "查"))
			answer_result_police();
	else
		return;
	
	start_change_inroom();
	if (RINFO.status==INROOM_NIGHT && night_over(PEOPLE_POLICE, S_POLICE) && night_over(PEOPLE_KILLER, S_KILLER))
	{
		
		// turn to check
		if (RINFO.nokill)
			sprintf (buf, "\33[1;33m注意，\33[32;1m %d %s \33[33m被盯上了...\33[m", RINFO.action_target[S_KILLER] + 1, PINFO(RINFO.action_target[S_KILLER]).nick);
		else
		{
			sprintf (buf, "\33[1;33m阴森森的月光下，横躺着\33[32;1m %d %s \33[33m的尸体...\33[m",RINFO.action_target[S_KILLER]+ 1, PINFO(RINFO.action_target[S_KILLER]).nick);
			PINFO(RINFO.action_target[S_KILLER]).flag |=PEOPLE_LASTWORDS;
		}
		send_msg(-1,buf);
		for(i=0;i<MAX_PLAYER;i++)
			RINFO.voted[i]=-1;
		if (RINFO.nokill)
		{
		// bluster
			goto_check();
		}
		else
		{
			goto_day();
		}
			 
	}
	end_change_inroom();
	kill_msg(-1);
}
int ctrl_t_night_common(long people_type, int s_type, char * verb)
{
	//int i;
	char buf[200];
	if ((PINFO(mypos).flag & people_type) && RINFO.action_target_tmp[s_type]!=-1 && RINFO.action_target[s_type]==-1)
	{
		RINFO.action_target[s_type]=RINFO.action_target_tmp[s_type];
		if (RINFO.action_target[s_type]>=0) 
			sprintf (buf, "决定了，就%s\33[32;1m%d %s \33[m!", verb, RINFO.action_target[s_type] + 1, PINFO(RINFO.action_target[s_type]).nick);
		else
			sprintf (buf, "决定了，今夜不行动!");

		send_msg2(people_type, buf);
		return 1;
	}
	return 0;
}
int night_over (long people_type, int s_type)
{
#ifdef KILLER_DEBUG
	char buf[200];
#endif
	if (living_role(people_type) && RINFO.action_target[s_type]==-1)
	{
#ifdef KILLER_DEBUG
		sprintf (buf, "类型%d,存在%d,投票%d", s_type, living_role(people_type), RINFO.action_target[s_type]);
		send_msg(-1,buf);
		kill_msg(-1);
#endif
		return 0;
	}
	return 1;
}

void night_result_crow()
{
	int i; 
	int hug;
	int defunct=RINFO.action_target[S_MAGIC];
	for (i=0;i<MAX_ROLE;i++)
		RINFO.seq_action[i][RINFO.day]=RINFO.action_target[i];
	if (living_role(PEOPLE_KILLER))
		RINFO.upgrade=0;
	else if (living_role (PEOPLE_MAGIC))
		RINFO.upgrade=1;
	else
		RINFO.upgrade=2;
//魔法师的无效化
	if (RINFO.upgrade == 0)
	{
		if (defunct==RINFO.action_target[S_BUTTERFLY] || PINFO(defunct).flag & PEOPLE_CROW)
		{
			defunct=-2;
			RINFO.action_target[S_MAGIC]=-2;
		}
		if (defunct>=0)
		{
			night_result_defunct(defunct,PEOPLE_KILLER, S_KILLER);
			night_result_defunct(defunct,PEOPLE_FOREST, S_FOREST);
			night_result_defunct(defunct,PEOPLE_BADMAN, S_BADMAN);
			night_result_defunct(defunct,PEOPLE_BUTTERFLY, S_BUTTERFLY);
			night_result_defunct(defunct,PEOPLE_SHOOTER, S_SHOOTER);
			night_result_defunct(defunct,PEOPLE_DOCTOR, S_DOCTOR);
			night_result_defunct(defunct,PEOPLE_POLICE, S_POLICE);
			night_result_defunct(defunct,PEOPLE_GOODMAN, S_GOODMAN);
		}
	}
	else if (RINFO.upgrade == 1)
	{
		RINFO.action_target[S_MAGIC]=-2;
		RINFO.action_target[S_KILLER]=defunct;
		defunct=-2;
	}
	else
	{
		RINFO.action_target[S_KILLER]=RINFO.action_target[S_FOREST];
		RINFO.action_target[S_FOREST]=-2;
	}
//乌鸦的无效化
	for (i=0; i<MAX_ROLE; i++)
	{
		if (i != S_CROW && PINFO(RINFO.action_target[i]).flag & PEOPLE_CROW)
		{
			RINFO.action_target[i]=-2;
		}
	}
//花蝴蝶的无效化
	hug=RINFO.action_target[S_BUTTERFLY];
	if (hug>=0)
		for (i=0; i<MAX_ROLE; i++)
			if (i != S_CROW && RINFO.action_target[i]==hug && i != S_BUTTERFLY)
				RINFO.action_target[i]=-2;
//禁言
	i=night_result_hug(RINFO.mulelist, S_FOREST,-1);
	RINFO.mulenum=i;
//暗票
	i=night_result_hug(RINFO.secretvotelist, S_BADMAN,-1);
	night_result_hug(RINFO.secretvotelist, S_GOODMAN,i);
//生死
	night_result_pinned();
	random_sort(RINFO.deadlist, RINFO.deadnum);
	random_sort(RINFO.mulelist, RINFO.mulenum);
//回答：按照调整的结果回答
	
}
void night_result_butterfly()
{
	int i; 
	int hug;
#ifdef KILLER_DEBUG
	char buf[200];
#endif
	for (i=0;i<MAX_ROLE;i++)
	{
		RINFO.seq_action[i][RINFO.day]=RINFO.action_target[i];
#ifdef KILLER_DEBUG
		sprintf(buf, "%d=>%d", i, RINFO.action_target[i]);
		send_msg(-1,buf);
		kill_msg(-1);
#endif
		
	}
//花蝴蝶的无效化
	hug=RINFO.action_target[S_BUTTERFLY];
	if (hug>=0)
		for (i=0; i<MAX_ROLE; i++)
			if (RINFO.action_target[i]==hug && i != S_BUTTERFLY)
				RINFO.action_target[i]=-2;
#ifdef KILLER_DEBUG

	for (i=0;i<MAX_ROLE;i++)
	{
		sprintf(buf, "%d=>%d", i, RINFO.action_target[i]);
		send_msg(-1,buf);
		kill_msg(-1);
		
	}
#endif

	
//禁言

	i=night_result_hug(RINFO.mulelist, S_FOREST,-1);
	RINFO.mulenum=i;

//生死
	night_result_pinned();
	random_sort(RINFO.deadlist, RINFO.deadnum);
	random_sort(RINFO.mulelist, RINFO.mulenum);
//回答：按照调整的结果回答
	
}

void night_result_pinned ()
{
	int i;
	int victim, shot, pinned,hug;
	for (i=0; i<MAX_PLAYER; i++)
		RINFO.deadlist[i]=-1;
	victim=RINFO.action_target[S_KILLER];
	shot=RINFO.action_target[S_SHOOTER];
	pinned=RINFO.action_target[S_DOCTOR];
	hug=RINFO.action_target[S_BUTTERFLY];
	if (victim==pinned)
	{
		RINFO.action_target[S_KILLER]=-2;
		pinned=-2;
	}
	else if (shot==pinned)
	{
		RINFO.action_target[S_SHOOTER]=-2;
		pinned=-2;
	}
	else if (shot==victim)
	{
		RINFO.action_target[S_SHOOTER]=-2;
	}
	i=0;
	if (pinned>=0)
	{
		if (RINFO.pinhole[pinned] >0 && PINFO(pinned).flag & PEOPLE_ALIVE && PINFO(pinned).style !=-1)
			RINFO.deadlist[i++]=pinned;
		else 
			RINFO.pinhole[pinned]=1;
		if (pinned==RINFO.special[S_BUTTERFLY])
		{
			if (RINFO.pinhole[hug] >0 && PINFO(hug).flag & PEOPLE_ALIVE && PINFO(hug).style !=-1)
				RINFO.deadlist[i++]=hug;
			else 
				RINFO.pinhole[hug]=1;
		}			
	}
	i=night_result_hug(RINFO.deadlist, S_KILLER, i);
	i=night_result_hug(RINFO.deadlist, S_SHOOTER, i);
	RINFO.deadnum=i;
}	
		

int night_result_hug (int * rlist, int s_type, int pointer)	 
{
	int i,obj;
	if (pointer<0)
	{
		for (i=0; i<MAX_PLAYER; i++)
			rlist[i]=-1;
		pointer=0;
	}
	i=pointer;
	obj=RINFO.action_target[s_type];
	if (obj>=0)
	{
		i=pointer;
		if (PINFO(obj).flag & PEOPLE_ALIVE && PINFO(obj).style!=-1)
			rlist[i++]=obj;

		if (RINFO.special[S_BUTTERFLY]==obj && RINFO.action_target[S_BUTTERFLY]>=0)
		{
			if (PINFO(RINFO.action_target[S_BUTTERFLY]).flag & PEOPLE_ALIVE 
				&&PINFO(RINFO.action_target[S_BUTTERFLY]).style != -1)
						rlist[i++]=RINFO.action_target[S_BUTTERFLY];
			
		}
	}
	return i;
}
	


int night_result_defunct(int target, long people_type, int s_type)
{
	if (PINFO(target).flag & people_type)
	{
		RINFO.action_target[s_type]=-2;
		return 1;
	}
	return 0;
}
int living_role (long people_type)
{
	int i;
	for (i=0;i<MAX_PLAYER; i++)
	{
		if (PINFO(i).style !=-1 && PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).flag & people_type)
			return 1;
	}
	return 0;
}
int living_role_count (long people_type)
{
	int i,k;
	k=0;
	for (i=0;i<MAX_PLAYER; i++)
	{
		if (PINFO(i).style !=-1 && PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).flag & people_type)
			k++;
	}
	return k;
}
int living_role_list (long people_type, int * list)
{
	int i,k;
	k=0;
	for (i=0;i<MAX_PLAYER; i++)
	{
		if (PINFO(i).style !=-1 && PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).flag & people_type)
			list[k++]=i;
	}
	return k;
}

int random_sort (int * list, int  count)
{
	int i,j;
	int a[count];
	int t;
	long k;
	k=MAX_PLAYER;
	if (k<count)
		k=count;
	k*=100;
	if (count <=1)
		return -1;
	for (i=0;i<count;i++) 
		a[i]=(int)(rand()%k);
	for (i=0;i<count-1;i++)
		for (j=i+1; j<count; j++)
			if (a[i]<a[j])
			{
				t=a[i];
				a[i]=a[j];
				a[j]=t;
				t=list[i];
				list[i]=list[j];
				list[j]=t;
			}
	return 0;
}
				
	
void answer_result_police ()
{
	int inf;
	char  buf[200];
	if (RINFO.action_target[S_POLICE]>=0)
	{
		inf=RINFO.action_target[S_POLICE];
		if (PINFO(inf).flag & PEOPLE_ALIVE && PINFO(inf).style !=-1 && PINFO(inf).flag & PEOPLE_SEENBAD)
			sprintf (buf, "没错，\33[32;1m%d %s \33[m就是一个坏人!", RINFO.action_target[S_POLICE]+ 1, PINFO(RINFO.action_target[S_POLICE]).nick);
		else
			sprintf (buf, "哦，\33[32;1m%d %s \33[m看起来是一个好人!", RINFO.action_target[S_POLICE]+ 1, PINFO(RINFO.action_target[S_POLICE]).nick);
		PINFO(RINFO.action_target[S_POLICE]).flag |= PEOPLE_TESTED;
	}
	else
		sprintf (buf, "你昨夜没有得到什么调查结果");
	send_msg2(PEOPLE_POLICE,buf);
	RINFO.seq_detect[RINFO.day]=RINFO.action_target[S_POLICE];
}
void answer_result_crow()
{
	char  buf [200];
	if (RINFO.action_target[S_CROW]>=0)
		{
			int s_type=get_s_type(RINFO.action_target[S_CROW]);
			int result;
			if (s_type>=0)
			{
				result=RINFO.action_target[s_type];
				if (result>0)
					sprintf (buf, "你查看了%d \33[33m%s\33[m昨夜的行动，他的目标是 %d \33[33m%s\33[m",
						RINFO.action_target[S_CROW]+1, PINFO(RINFO.action_target[S_CROW]).nick,
						result+1, PINFO(result).nick);
				else	
					sprintf (buf, "你查看了%d \33[33m%s\33[m昨夜的行动，他好像呼呼大睡",
						RINFO.action_target[S_CROW]+1, PINFO(RINFO.action_target[S_CROW]).nick);			     }
			else
				sprintf (buf, "你查看的目标不在了");
			send_msg2(PEOPLE_CROW, buf);
	}

}
				
void announce_death_mule()
{
	char buf[200], buf2[200];
	int i;
	sprintf (buf, "天亮了。");
	send_msg(-1, buf);
	kill_msg(-1);

	if (RINFO.deadnum>0)
	{
		sprintf (buf, "昨晚有%d个人死了:", RINFO.deadnum);
		for (i=0;i<RINFO.deadnum; i++)
		{
			PINFO(RINFO.deadlist[i]).flag|= PEOPLE_LASTWORDS;
			sprintf (buf2, "%d %s", RINFO.deadlist[i]+1,PINFO(RINFO.deadlist[i]).nick);
			if (i>0)
				strcat (buf, ",");
			strcat (buf, buf2);
		}		
	}
	else
		sprintf (buf, "昨晚没有人死掉。");
	send_msg(-1, buf);
	kill_msg(-1);
	
	if (RINFO.mulenum>0)
	{
		sprintf (buf, "今天有%d个人被禁言:", RINFO.mulenum);
		for (i=0;i<RINFO.mulenum; i++)
		{
			PINFO(RINFO.mulelist[i]).flag|= PEOPLE_MULE;
			sprintf (buf2, "%d %s", RINFO.mulelist[i]+1,PINFO(RINFO.mulelist[i]).nick);
			if (i>0)
				strcat (buf, ",");
			strcat (buf, buf2);
		}		
	}
	else
			sprintf (buf, "今天没有人被禁言。");
	send_msg(-1, buf);
	kill_msg(-1);
}
int get_s_type (int num)
{
	if (num<MAX_PLAYER && PINFO(num).flag & PEOPLE_ALIVE )
	{
		if (PINFO(num).flag & PEOPLE_KILLER)
			return S_KILLER;
		if (PINFO(num).flag & PEOPLE_MAGIC)
			return S_MAGIC;
		if (PINFO(num).flag & PEOPLE_FOREST)
			return S_FOREST;
		if (PINFO(num).flag & PEOPLE_BADMAN)
			return S_BADMAN;
		if (PINFO(num).flag & PEOPLE_BUTTERFLY)
			return S_BUTTERFLY;
		if (PINFO(num).flag & PEOPLE_SHOOTER)
			return S_SHOOTER;
		if (PINFO(num).flag & PEOPLE_POLICE)
			return S_POLICE;
		if (PINFO(num).flag & PEOPLE_DOCTOR)
			return S_DOCTOR;
		if (PINFO(num).flag & PEOPLE_GOODMAN)
			return S_GOODMAN;
		if (PINFO(num).flag & PEOPLE_CROW)
			return S_CROW;
	}
	return -1;
}

	
