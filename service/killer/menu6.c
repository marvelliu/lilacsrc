int menu_6_callback()
{
	int me,i,k;
	char buf[200];
	me=mypos;
	if (RINFO.status != INROOM_STOP)
	{
		send_msg (me, "游戏中，不能改设置");
		kill_msg(me);
		return 0;
	}
	menu_ask_string( "请输入房间最大人数:",30, buf);
	if (kicked)
		return 0;
	if (buf[0])
	{
		i = atoi(buf);
		if (i > 0 && i <= MAX_PLAYER)
		{
			rooms[myroom].maxplayer = i;
			sprintf(buf, "屋主设置房间最大人数为%d", i);
			send_msg(-1, buf);
		}
	}
	k=menu_ask_yn("设置为隐藏房间? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0) 
	{
		if (k)
			rooms[myroom].flag |= ROOM_SECRET;
		else
			rooms[myroom].flag &= ~ROOM_SECRET;
		sprintf(buf, "屋主设置房间为%s", k ? "隐藏" : "不隐藏");
			send_msg(-1, buf);
	}
	k=menu_ask_yn("设置为锁定房间? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_LOCKED;
		else
			rooms[myroom].flag &= ~ROOM_LOCKED;
		sprintf(buf, "屋主设置房间为%s", k ? "锁定" : "不锁定");
		send_msg(-1, buf);
	}
	k=menu_ask_yn("设置为拒绝旁观者的房间? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_DENYSPEC;
		else
			rooms[myroom].flag &= ~ROOM_DENYSPEC;
		sprintf(buf, "屋主设置房间为%s", k ? "拒绝旁观" : "不拒绝旁观");
		send_msg(-1, buf);
	}
	/*
	move(t_lines - 1, 0);
	k=menu_ask_yn("设置为旁观者无法看见杀手警察的房间? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_SPECBLIND;
		else
			rooms[myroom].flag &= ~ROOM_SPECBLIND;
		sprintf(buf, "屋主设置房间为%s", k ? "旁观无法看见杀手警察" : "旁观可以看见杀手警察");
		send_msg(-1, buf);
	}
	*/
	send_msg(mypos, "游戏类型：1--南开规则； 2--恶民和乌鸦；3--花蝴蝶；");
	send_msg(mypos, "          4--哈工大规则； 5--砸六家；    6--拱猪；");
	kill_msg(mypos);
	menu_ask_string("设置游戏的类型: ",30, buf);
	if (kicked)
		return 0;
	if (buf[0])
	{
		i = atoi(buf);
		if (i >= 1 && i <= 6)
		{
			RINFO.gametype = i;
			if (i==GAME_ORIGINAL)
				sprintf(buf, "屋主设置房间游戏类型为南开规则");
			else if  (i==GAME_CROW)
				sprintf(buf, "屋主设置房间游戏类型为恶民和乌鸦");
			else if  (i==GAME_BUTTERFLY)
				sprintf(buf, "屋主设置房间游戏类型为花蝴蝶");			
			else if  (i==GAME_TSINGHUA)
				sprintf(buf, "屋主设置房间游戏类型为哈工大规则");
			else if  (i==GAME_JOKER1)
				sprintf(buf, "屋主设置房间游戏类型为砸六家");
			else if  (i==GAME_JOKERPIG)
				sprintf(buf, "屋主设置房间游戏类型为拱猪");	
			send_msg(-1, buf);
		}
	}
	
	if (RINFO.gametype==GAME_ORIGINAL || RINFO.gametype==GAME_BUTTERFLY ||	RINFO.gametype==GAME_TSINGHUA)
	{
		i=menu_ask_num("设置杀手的数目: ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			RINFO.killernum = i;
			sprintf(buf, "屋主设置房间杀手数为%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_CROW)
	{
		i=menu_ask_num("设置恶民的数目(人不够时选0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			RINFO.killernum = i;
			sprintf(buf, "屋主设置房间恶民数为%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_JOKER1)
	{
		i=menu_ask_num("设置发言开关(1=只许自己出牌的时候说话): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			if (i==0)
				i=1;
			RINFO.killernum = i;
			sprintf(buf, "屋主设置发言开关为%d", i);
			send_msg(-1, buf);
		}
	}
	if (RINFO.gametype==GAME_ORIGINAL ||RINFO.gametype==GAME_TSINGHUA)
	{
		i=menu_ask_num("设置警察的数目: ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
			RINFO.policenum = i;
			sprintf(buf, "屋主设置房间警察数为%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_BUTTERFLY)
	{
		i=menu_ask_num("设置警察的数目(1或者0,只有当人数不够时选0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
			if (i>0)
				i=1;
			RINFO.policenum = i;
			sprintf(buf, "屋主设置房间警察数为%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_CROW)
	{
		i=menu_ask_num("设置善民的数目(人不够时选0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
//			if (i==0)
//				i=1;
			RINFO.policenum = i;
			sprintf(buf, "屋主设置房间善民数为%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_JOKER1)
	{
	}
	else if (RINFO.gametype==GAME_JOKERPIG)
	{
		k=menu_ask_yn("清除得分记录(初次选择Y)? [Y/N]");
		if (kicked)
			return 0;
		if (k>=0)
		{
			if (k)
				for (i=0;i<MAX_PLAYER;i++)
					RINFO.action_target[i]=0;
		}
		sprintf(buf, "屋主设置得分记录为%s", k ? "清除" : "不变");
		send_msg(-1, buf);
	}
	i=menu_ask_num ("设置每次发言或者出牌的最大时间n（10秒): ");
	if (kicked) 
		return 0;
	if (i>=0 && i<=100)
	{
		if (i==0)
			i=1;
		RINFO.time_actionlimit= i*10;
		sprintf(buf, "屋主设置房间发言或者出牌时间最大为%d秒", i*10);
		send_msg(-1, buf);
	}

	i=menu_ask_num ("设置发呆的最大时间n（10秒): ");
	if (kicked) 
		return 0;
	if (i>=0 && i<=30)
	{
		if (i==0)
			i=1;
		RINFO.time_actionlimit= i*10;
		sprintf(buf, "屋主设置房间发呆时间最大为%d秒", i*10);
		send_msg(-1, buf);
	}

	
	kill_msg(-1);
	return 0;
}
