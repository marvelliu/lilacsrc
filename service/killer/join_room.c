#include "ctrlt.c"
#include "ctrls.c"
#include "ctrlg.c"
#include "timeout.c"
#include "normalmsg.c"
#include "emote.c"
void join_room(int w, int spec)
{
	char buf[200], buf2[220], buf3[200], roomname[80];
	int i, j;
	clear();

	selected = 0;
	ipage = 0;
	jpage = 0;
	
	myroom = w;
	start_change_inroom();
	if (rooms[myroom].style != 1)
	{
		end_change_inroom();
		return;
	}
	strcpy(roomname, rooms[myroom].name);
	signal(SIGUSR1, room_refresh);
	if (spec)
	  i=MAX_PLAYER;
	else
	  i = 0;
	while (PINFO(i).style != -1)
		i++;
	if ( (i>=MAX_PLAYER && !spec) || (i>=MAX_PEOPLE && spec) )
	{
		end_change_inroom();
		return;
	}
	mypos = i;
	PINFO(i).style = 0;
	PINFO(i).flag = 0;
	strcpy(PINFO(i).id, CURRENTUSER->userid);
	strcpy(PINFO(i).nick, CURRENTUSER->userid);
	PINFO(i).pid = uinfo.pid;

	if (!spec)
		rooms[myroom].numplayer++;
	else
		rooms[myroom].numspectator++;

	if (!spec)
	{
		if (rooms[myroom].op==-1 || PINFO(rooms[myroom].op).style==-1)
		{
			rooms[myroom].op=mypos;
		}
	}

	end_change_inroom();

	kill_msg(-1);

	room_refresh(0);
	while (1)
	{
		do
		{
			int ch;
			sprintf(buf2,"%2d输入:",(mypos+1)%100);
			ch = -k_getdata(t_lines - 1, 0, buf2, buf, 70, 1, NULL, 1);
			if (rooms[myroom].style != 1)
				kicked = 1;
			if (kicked)
				goto quitgame;
//时间控制		
			if (RINFO.status != INROOM_STOP)
			{
				if (check_timeout_turn())
					timeout_callback(TIMEOUT_TURN);
				if (check_timeout_action())
					timeout_callback(TIMEOUT_ACTION);
				if (check_timeout_auto())
					timeout_callback(TIMEOUT_AUTO);
			}
			
			if (ch == KEY_UP) 
			{
				selected--;
				if (selected < 0)
					selected = rooms[myroom].numplayer - 1;
				if (ipage > selected)
					ipage = selected;
				if (selected > ipage + t_lines - 5)
					ipage = selected - (t_lines - 5);
				kill_msg(mypos);
			}
			else if (ch == KEY_DOWN) 
			{
				selected++;
				if (selected >= rooms[myroom].numplayer)
					selected = 0;
				if (ipage > selected)
					ipage = selected;
				if (selected > ipage + t_lines - 5)
					ipage = selected - (t_lines - 5);
				kill_msg(mypos);
			} 
			else if (ch == KEY_PGUP)
			{
				jpage += t_lines / 2;
				kill_msg(mypos);
			} 
			else if (ch == KEY_PGDN)
			{
				jpage -= t_lines / 2;
				if (jpage <= 0)
					jpage = 0;
				kill_msg(mypos);
			} 
			else if (mypos<MAX_PLAYER
				&& (PINFO(mypos).flag & PEOPLE_ALIVE)
			   && ch == Ctrl('U'))
			{
				ctrl_g_callback();
			} 

			else if (mypos<MAX_PLAYER
				&& (PINFO(mypos).flag & PEOPLE_ALIVE)
			   && ch == Ctrl('T'))
			{
				ctrl_t_callback(selected);
			} 
			else if (mypos<MAX_PLAYER
				&& (PINFO(mypos).flag & PEOPLE_ALIVE)
				&& ch == Ctrl('S'))
			{
				ctrl_s_callback(selected);
			}
			else if (ch <= 0 && !buf[0]) 
			{
				j = do_com_menu();
				if (kicked)
					goto quitgame;
				if (j == 1)
					goto quitgame;
			}
			else if (ch <= 0)
			{
				if (!normalmsg_callback(buf))
					break;
			}
		} while (1);
		join_room_talk(buf);
	}

	quitgame:
	start_change_inroom();
	//me = mypos;
	if (rooms[myroom].op==mypos)
	{
		int k;
		for (k = 0; k < MAX_PEOPLE; k++)
			if (PINFO(k).style != -1)
				if (k != mypos)
				{
					rooms[myroom].op=k;
					sprintf(buf, "\x1b[34;1m%d %s成为新房主\x1b[m", k+1,PINFO(k).nick);
					send_msg(-1, buf);
					kill_msg(-1);
					break;
				}
		if (k>=MAX_PEOPLE)
		{
			rooms[myroom].op=-1;
			send_msg(-1, "房间解散了");
			kill_msg(-1);
			rooms[myroom].style = -1;
			end_change_inroom();
			goto quitgame2;
		}
	}
	if (PINFO(mypos).style != -1)
	{
		PINFO(mypos).style = -1;
		if (mypos>=MAX_PLAYER)
			rooms[myroom].numspectator--;
		else
			rooms[myroom].numplayer--;
		kill_msg(-1);
	}
	end_change_inroom();

/*
	if(killer)
		sprintf(buf, "杀手%s潜逃了", buf2);
	else
		sprintf(buf, "%s离开房间", buf2);
	for(i=0;i<myroom->people;i++)
	{
		send_msg(inrooms.peoples+i, buf);
		kill(inrooms.peoples[i].pid, SIGUSR1);
	}
*/
quitgame2:
	kicked = 0;
	sprintf(buf, "tmp/%d.msg", rand());
	save_msgs(buf);
	sprintf(buf2, "\"%s\"的杀人记录", roomname);
	k_getdata(t_lines - 1, 0, "寄回本次全部信息吗?[y/N]", buf3, 3, 1, 0, 1);
	if (toupper(buf3[0]) == 'Y')
	{
		k_mail_file(CURRENTUSER->userid, buf, CURRENTUSER->userid, buf2, BBSPOST_MOVE, NULL);
	}
	else
		unlink(buf);
	//kill_msg(-1);
	signal(SIGUSR1, DEFAULT_SIGNAL);
}





void join_room_talk(char * buf)
{
	int i;
	time_t ct;
	char msg[200],buf2[220];
	
	if (mypos<MAX_PLAYER  && (PINFO(mypos).flag & PEOPLE_ALIVE)&& //RINFO.gametype !=GAME_TSINGHUA && 
		( ( (RINFO.gametype != GAME_JOKER1 && RINFO.gametype != GAME_JOKERPIG) || RINFO.killernum==1) )
	)
	{
		//活人说话
		if (RINFO.status ==INROOM_VOTE && RINFO.gametype != GAME_TSINGHUA)
			return;
		if ((RINFO.status ==INROOM_DARK)&& RINFO.turn!=mypos)//留遗言时，清华规则不能乱发言
			return;
		if ((RINFO.status ==INROOM_DAY)	&& RINFO.turn!=mypos)//留遗言时，清华规则不能乱发言
			return;
		if ((RINFO.status ==INROOM_CHECK) && RINFO.turn!=mypos && RINFO.gametype != GAME_TSINGHUA) 
			return;		  
		if ((RINFO.status ==INROOM_DEFENCE) && RINFO.turn!=mypos && RINFO.gametype != GAME_TSINGHUA)
			return;
	}
	// 说话控制
	ct=time(NULL);
	if (!denytalk && ct==lasttalktime)
	{
//			send_msg(mypos,"你说话太快，禁止发言5秒钟");
//			denytalk=1;
//			kill_msg(mypos);
//			continue;
	}
	if (denytalk && ct-lasttalktime>5)
	{
			denytalk=0;
	}
	if (denytalk)
		return;
	start_change_inroom();
	  
	lasttalktime=ct;
	//emote
	//me = mypos;
	strcpy(msg, buf);
	if (msg[0] == '/' && msg[1] == '/')
			emote_callback(mypos, msg, buf);
	else 
	{
		strcpy(buf2, buf);
		sprintf(buf, "%d %s: %s", mypos + 1, PINFO(mypos).nick, buf2);
	}


	//发言权控制
	if (PINFO(mypos).flag & PEOPLE_DENYSPEAK)
	{
			send_msg(i, "你现在没有发言权");
			end_change_inroom();
			kill_msg(i);
			return;
	}
	//不同时段
	if (RINFO.status!=INROOM_STOP 	&& mypos<MAX_PLAYER  && (PINFO(mypos).flag & PEOPLE_ALIVE 
			//	|| RINFO.gametype ==4 
				||RINFO.gametype==5))
	{
		//晚上只有同伙之间说话
		if (mypos<MAX_PLAYER && RINFO.status == INROOM_NIGHT)
		{
			if (RINFO.gametype == 1) 
			{
				if ( PINFO(mypos).flag & (PEOPLE_KILLER))
						send_msg2((PEOPLE_KILLER), buf);
				else if (PINFO(mypos).flag & (PEOPLE_POLICE))
						send_msg2((PEOPLE_POLICE), buf);
			}
			else if (RINFO.gametype == 2)
			{
				if ( PINFO(mypos).flag & (PEOPLE_BADMAN))
						send_msg2((PEOPLE_BADMAN), buf);
				else if (PINFO(mypos).flag & (PEOPLE_GOODMAN))
						send_msg2((PEOPLE_GOODMAN), buf);
				else if (PINFO(mypos).flag & (PEOPLE_CROW))
						send_msg2((PEOPLE_CROW), buf);
				
			}
			else if (RINFO.gametype == 3 || RINFO.gametype == 4)
				if ( PINFO(mypos).flag & (PEOPLE_KILLER))
						send_msg2((PEOPLE_KILLER), buf);
			
		
		}//白天轮到你了，就可以说话
		else if (mypos<MAX_PLAYER && !(PINFO(mypos).flag & PEOPLE_MULE))
		{
			send_msg(-1,buf);
			kill_msg(-1);
		}//但是如果被禁言了，就会胡说八道
		else if (mypos<MAX_PLAYER && PINFO(mypos).flag & PEOPLE_MULE)
		{
			sprintf (buf,"%d %s: %s", mypos + 1, PINFO(mypos).nick,fudge[rand()%MAX_FUDGE]);
			send_msg(-1,buf);
			kill_msg(-1);
		}
	}
	else if (RINFO.status!=INROOM_STOP)
	{
		//死人的天堂
		strcpy(buf2,"\33[35;1m*");
		strcat(buf2,buf);
		strcat(buf2,"\33[m");
		for (i = 0; i < MAX_PEOPLE; i++)
			if (PINFO(i).style != -1)
			{
				if (i>=MAX_PLAYER || !(PINFO(i).flag & PEOPLE_ALIVE))
				{
					send_msg(i, buf2);
					kill_msg(i);
				}
			}
	}
	else
	{
			send_msg(-1,buf);
			kill_msg(-1);
	}
	end_change_inroom();
	//kill_msg(-1);
}

