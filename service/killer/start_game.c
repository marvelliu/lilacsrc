
void start_game()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			start_game_original();
			break;
		case GAME_CROW :
			start_game_crow();
			break;
		case GAME_BUTTERFLY :
			start_game_butterfly();
			break;
		case GAME_TSINGHUA :
			start_game_tsinghua();
			break;
		case GAME_JOKER1 :
			start_game_joker1();
			break;
		case GAME_JOKERPIG :
			start_game_jokerpig();
			break;
	}
}
	
void start_game_original()
{
	int i, totalk = 0, total = 0, totalc = 0;
	char buf[80];
	RINFO.votetype=1;
	total=start_game_preparation();
	if (RINFO.policenum<1)
	 	RINFO.policenum=1;
	totalk = RINFO.killernum;
	totalc = RINFO.policenum;
	if(start_game_checktotal(total, totalk, totalc))
		return;
	if (totalc == 0)
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人\x1b[m", totalk);
	else
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人, %d个警察\x1b[m", totalk, totalc);
	send_msg(-1, buf);

	start_game_autopost(totalk, totalc, total);
	
	for (i = 0; i < totalk; i++)
		RINFO.killers[i] = start_game_give_card(PEOPLE_KILLER, "你做了一个无耻的坏人");

	if (totalc>1 && total-totalk-totalc>1)
		RINFO.nokill = 0;
	else
		RINFO.nokill = 1;
	for (i = 0; i < totalc; i++)
		RINFO.polices[i]=start_game_give_card(PEOPLE_POLICE, "你做了一位光荣的人民警察");
	goto_night();
	kill_msg(-1);
}

void start_game_tsinghua()
{
	int i,me, totalk, totalc, total;
	char buf[80];
	me=mypos;
	RINFO.votetype=3;
	RINFO.nokill=0;
	total=start_game_preparation();
	totalk=RINFO.killernum;
	totalc=RINFO.policenum;
	if (start_game_checktotal(total,totalk,totalc))
		return;
	if (totalc == 0)
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人\x1b[m", totalk);
	else
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人, %d个警察\x1b[m", totalk, totalc);
	send_msg(-1, buf);

	start_game_autopost(totalk, totalc, total);
	
	for (i = 0; i < totalk; i++)
		RINFO.killers[i] = start_game_give_card(PEOPLE_KILLER, "你做了一个无耻的坏人");
	for (i = 0; i < totalc; i++)
		RINFO.polices[i] = start_game_give_card(PEOPLE_POLICE, "你做了一位光荣的人民警察");
	goto_night();
	kill_msg(-1);
}
	

void start_game_crow()
{
	int i,totalk, totalc, total;
	char buf[80];
	RINFO.votetype=2;	
	RINFO.nokill=0;
	total=start_game_preparation();
	
	//if (RINFO.policenum<1)
	// 	RINFO.policenum=1;
	totalk = RINFO.killernum;
	totalc = RINFO.policenum;
	if(start_game_checktotal(total,totalk,totalc))
		return;
	sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人和%d个好人,还有%d个乌鸦\x1b[m", totalk+3, totalc+4, total-totalk-totalc-7);
	send_msg(-1, buf);
	kill_msg(-1);
	start_game_autopost(totalk, totalc, total);
	for (i = 0; i < totalk; i++)
		RINFO.killers[i]=start_game_give_card(PEOPLE_BADMAN, "你做了一个恶民, 用你的智慧和暗票帮助坏人吧。");
	for (i = 0; i < totalc; i++)
		RINFO.polices[i]=start_game_give_card(PEOPLE_GOODMAN, "你做了一个善民, 用你的智慧和暗票帮助好人吧。");
	for (i = 0; i < total-totalk-totalc-7; i++)
		RINFO.crows[i]=start_game_give_card(PEOPLE_CROW, "你做了一个最聪明、最伟大的乌鸦, 用你的智慧让他们平局吧。");
	RINFO.special[S_KILLER]=start_game_give_card(PEOPLE_KILLER, "你做了一个杀手, 用你的刀法把好人杀光。");
	RINFO.special[S_MAGIC]=start_game_give_card(PEOPLE_MAGIC,  "你做了一个魔法师, 你要让好人的行动全都作废。");
	RINFO.special[S_FOREST]=start_game_give_card(PEOPLE_FOREST, "你做了一个森林老人, 你要让警察有口难言。");
	RINFO.special[S_BUTTERFLY]=start_game_give_card(PEOPLE_BUTTERFLY, "你就是主角：花蝴蝶。");
	RINFO.special[S_SHOOTER]=start_game_give_card(PEOPLE_SHOOTER, "你做了一个狙击手, 你要杀的是坏人。");
	RINFO.special[S_DOCTOR]=start_game_give_card(PEOPLE_DOCTOR, "你做了一个医生, 一针救好人, 两空针杀坏人。");
	RINFO.special[S_POLICE]=start_game_give_card(PEOPLE_POLICE, "你做了一个人民警察, 你是好人的眼睛。");
	goto_night();
	kill_msg(-1);
}
void start_game_butterfly()
{
	int i,totalk, totalc,total;
	char buf[80];
	RINFO.votetype=2;
	total=start_game_preparation();
	if (RINFO.policenum>1)
		RINFO.policenum=1;
	totalk = RINFO.killernum;
	totalc=total-totalk-5;
	if(start_game_checktotal(total, totalk, RINFO.policenum))
		return;
	if (RINFO.policenum>=1 || totalk+1>=4)
	{
		RINFO.policenum=1;
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人和4个好人特殊身份\x1b[m", totalk+1);
		send_msg(-1, buf);
		send_msg(-1, "\x1b[31;1m本局有一名警察。");
		RINFO.special[S_POLICE]=start_game_give_card(PEOPLE_POLICE, "你做了一个人民警察, 你是好人的眼睛。");
	}
	else
	{
		RINFO.policenum=0;
		sprintf(buf, "\x1b[31;1m游戏开始啦! 人群中出现了%d个坏人和3个好人特殊身份\x1b[m", totalk+1);
		send_msg(-1, buf);
		send_msg(-1, "\x1b[31;1m本局没有警察。");
	}
	kill_msg(-1);
	if (totalc<=totalk) 
	{
		send_msg(-1, "\x1b[31;1m平民不比杀手多, 所以平民全部死亡不算杀手胜利。\x1b[m");
		RINFO.nokill=1;
	}
	else
	{
		send_msg(-1, "\x1b[31;1m平民比杀手多, 所以平民全部死亡算杀手胜利。\x1b[m");
		RINFO.nokill=0;
	}
	start_game_autopost(totalk, totalc, total);
	kill_msg(-1);
	//分发身份
	for (i = 0; i < totalk; i++)
			RINFO.killers[i]=start_game_give_card(PEOPLE_KILLER, "你做了一个杀手, 用好你的刀。");
	RINFO.special[S_FOREST]=start_game_give_card(PEOPLE_FOREST, "你做了一个森林老人, 你要让警察有口难言。");
	RINFO.special[S_BUTTERFLY]=start_game_give_card(PEOPLE_BUTTERFLY, "你就是主角：花蝴蝶。");
	RINFO.special[S_SHOOTER]=start_game_give_card(PEOPLE_SHOOTER, "你做了一个狙击手, 你要杀的是坏人。");
	RINFO.special[S_DOCTOR]=start_game_give_card(PEOPLE_DOCTOR, "你做了一个医生, 一针救好人, 两空针杀坏人。");
	goto_night();
	kill_msg(-1);
}




//发牌用的子程序
int start_game_give_card ( long people_type, char * msgstr)
{
	int j;
	do 
	{
		j = rand() % MAX_PLAYER;
	} while (PINFO(j).style == -1
		|| PINFO(j).flag & PEOPLE_CARD);
	PINFO(j).flag |= people_type;
	send_msg(j, msgstr);
	kill_msg(j);
	return j;
}
//开始前的准备
int start_game_preparation ()
{
	int i, j, total;
	total=0;
	for (i = 0; i < MAX_PLAYER; i++)
		if (PINFO(i).style !=-1)
		{
			PINFO(i).flag=PEOPLE_ALIVE;
			PINFO(i).vote = -1;
		}
	RINFO.day=-1;
	RINFO.draw_counter=0;
	RINFO.draw_check_day=-1;
	for(i=0;i<MAX_PLAYER;i++)
	{
		RINFO.killers[i]=-1;
		RINFO.polices[i]=-1;
		RINFO.crows[i]=-1;
		RINFO.pinhole[i]=-1;
	}
	for(j=0;j<MAX_ROLE;j++)
			RINFO.special[j]=-1;
	for(i=0;i<MAX_DAY;i++)
	{
		RINFO.seq_detect[i]=-1;
		for(j=0;j<MAX_ROLE;j++)
				RINFO.seq_action[j][i]=-1;
		
	}
	if( RINFO.gametype != GAME_CROW )
	{
		if (RINFO.killernum<1)
			RINFO.killernum=1;
	}
//	if (RINFO.policenum<1)
//	 	RINFO.policenum=1;
	for (i = 0; i < MAX_PLAYER; i++)
		if (PINFO(i).flag & PEOPLE_ALIVE 
				&& PINFO(i).style !=-1)
			total++;
	start_change_inroom();
	return total;
}
int start_game_checktotal(int total, int totalk, int totalc)
{
	char buf[200];
	int n1[4]= {3,7,6,6};
	int n2[4]= {-1,3,4,-1};
	char * gamename[4]={"南开", "恶民和乌鸦", "传统花蝴蝶", "哈工大"};
	int gametype=RINFO.gametype;

	if (total<n1[gametype-1])
	{
		sprintf (buf, "\x1b[31;1m至少%d人参加，才能进行%s杀人游戏。\x1b[m", n1[gametype-1], gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	if (n2[gametype-1]<0 && total-totalk-totalc<totalk)
	{
		sprintf (buf, "\x1b[31;1m平民必须比杀手多，才能进行%s杀人游戏。\x1b[m",gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	else if (n2[gametype-1]>total-totalk-totalc)
	{
		sprintf (buf, "\x1b[31;1m除去设定的杀手和警察(或者善民和恶民)数量以外，至少还要有%d人参加，才能进行%s杀人游戏。\x1b[m", n2[gametype-1], gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	sprintf(buf,"\x1b[31;1m可以进行%s杀人游戏。\x1b[m", gamename[gametype-1]);
	send_msg(mypos, buf);
	kill_msg(mypos);
	return 0;
}
//autopost的部分
void start_game_autopost (int totalk, int totalc, int total) 
{
	time_t t;
	struct tm * tt;
		time(&t);
	tt=localtime(&t);
	sprintf(RINFO.gamename,"%s: %d-%d-%d, %04d/%02d/%02d %02d:%02d:%02d",rooms[myroom].name,totalk,totalc,
			total-totalk-totalc,tt->tm_year+1900,tt->tm_mon+1,tt->tm_mday,tt->tm_hour,tt->tm_min, tt->tm_sec);

#ifdef AUTOPOST
	char buf[80];
	int i;
	struct flock ldata;
	int fd;
	unlink(ap_tmpname());
	fd = open(ap_tmpname(), O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd>=0)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			lseek(fd, 0, SEEK_END);
			for(i=0;i<MAX_PLAYER;i++)
			{
				if (PINFO(i).style!=-1)
				{
					sprintf(buf, "\x1b[34;1m%2d-%s(%s)\x1b[m", (i+1)%100, PINFO(i).nick, PINFO(i).id);
					write(fd,buf,strlen(buf));
					write(fd,"\n",1);
				}
			}
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);			
	}
#endif // AUTOPOST
}
	
	
		
	
