
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
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������\x1b[m", totalk);
	else
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������, %d������\x1b[m", totalk, totalc);
	send_msg(-1, buf);

	start_game_autopost(totalk, totalc, total);
	
	for (i = 0; i < totalk; i++)
		RINFO.killers[i] = start_game_give_card(PEOPLE_KILLER, "������һ���޳ܵĻ���");

	if (totalc>1 && total-totalk-totalc>1)
		RINFO.nokill = 0;
	else
		RINFO.nokill = 1;
	for (i = 0; i < totalc; i++)
		RINFO.polices[i]=start_game_give_card(PEOPLE_POLICE, "������һλ���ٵ����񾯲�");
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
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������\x1b[m", totalk);
	else
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������, %d������\x1b[m", totalk, totalc);
	send_msg(-1, buf);

	start_game_autopost(totalk, totalc, total);
	
	for (i = 0; i < totalk; i++)
		RINFO.killers[i] = start_game_give_card(PEOPLE_KILLER, "������һ���޳ܵĻ���");
	for (i = 0; i < totalc; i++)
		RINFO.polices[i] = start_game_give_card(PEOPLE_POLICE, "������һλ���ٵ����񾯲�");
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
	sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d�����˺�%d������,����%d����ѻ\x1b[m", totalk+3, totalc+4, total-totalk-totalc-7);
	send_msg(-1, buf);
	kill_msg(-1);
	start_game_autopost(totalk, totalc, total);
	for (i = 0; i < totalk; i++)
		RINFO.killers[i]=start_game_give_card(PEOPLE_BADMAN, "������һ������, ������ǻۺͰ�Ʊ�������˰ɡ�");
	for (i = 0; i < totalc; i++)
		RINFO.polices[i]=start_game_give_card(PEOPLE_GOODMAN, "������һ������, ������ǻۺͰ�Ʊ�������˰ɡ�");
	for (i = 0; i < total-totalk-totalc-7; i++)
		RINFO.crows[i]=start_game_give_card(PEOPLE_CROW, "������һ�����������ΰ�����ѻ, ������ǻ�������ƽ�ְɡ�");
	RINFO.special[S_KILLER]=start_game_give_card(PEOPLE_KILLER, "������һ��ɱ��, ����ĵ����Ѻ���ɱ�⡣");
	RINFO.special[S_MAGIC]=start_game_give_card(PEOPLE_MAGIC,  "������һ��ħ��ʦ, ��Ҫ�ú��˵��ж�ȫ�����ϡ�");
	RINFO.special[S_FOREST]=start_game_give_card(PEOPLE_FOREST, "������һ��ɭ������, ��Ҫ�þ����п����ԡ�");
	RINFO.special[S_BUTTERFLY]=start_game_give_card(PEOPLE_BUTTERFLY, "��������ǣ���������");
	RINFO.special[S_SHOOTER]=start_game_give_card(PEOPLE_SHOOTER, "������һ���ѻ���, ��Ҫɱ���ǻ��ˡ�");
	RINFO.special[S_DOCTOR]=start_game_give_card(PEOPLE_DOCTOR, "������һ��ҽ��, һ��Ⱥ���, ������ɱ���ˡ�");
	RINFO.special[S_POLICE]=start_game_give_card(PEOPLE_POLICE, "������һ�����񾯲�, ���Ǻ��˵��۾���");
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
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d�����˺�4�������������\x1b[m", totalk+1);
		send_msg(-1, buf);
		send_msg(-1, "\x1b[31;1m������һ�����졣");
		RINFO.special[S_POLICE]=start_game_give_card(PEOPLE_POLICE, "������һ�����񾯲�, ���Ǻ��˵��۾���");
	}
	else
	{
		RINFO.policenum=0;
		sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d�����˺�3�������������\x1b[m", totalk+1);
		send_msg(-1, buf);
		send_msg(-1, "\x1b[31;1m����û�о��졣");
	}
	kill_msg(-1);
	if (totalc<=totalk) 
	{
		send_msg(-1, "\x1b[31;1mƽ�񲻱�ɱ�ֶ�, ����ƽ��ȫ����������ɱ��ʤ����\x1b[m");
		RINFO.nokill=1;
	}
	else
	{
		send_msg(-1, "\x1b[31;1mƽ���ɱ�ֶ�, ����ƽ��ȫ��������ɱ��ʤ����\x1b[m");
		RINFO.nokill=0;
	}
	start_game_autopost(totalk, totalc, total);
	kill_msg(-1);
	//�ַ����
	for (i = 0; i < totalk; i++)
			RINFO.killers[i]=start_game_give_card(PEOPLE_KILLER, "������һ��ɱ��, �ú���ĵ���");
	RINFO.special[S_FOREST]=start_game_give_card(PEOPLE_FOREST, "������һ��ɭ������, ��Ҫ�þ����п����ԡ�");
	RINFO.special[S_BUTTERFLY]=start_game_give_card(PEOPLE_BUTTERFLY, "��������ǣ���������");
	RINFO.special[S_SHOOTER]=start_game_give_card(PEOPLE_SHOOTER, "������һ���ѻ���, ��Ҫɱ���ǻ��ˡ�");
	RINFO.special[S_DOCTOR]=start_game_give_card(PEOPLE_DOCTOR, "������һ��ҽ��, һ��Ⱥ���, ������ɱ���ˡ�");
	goto_night();
	kill_msg(-1);
}




//�����õ��ӳ���
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
//��ʼǰ��׼��
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
	char * gamename[4]={"�Ͽ�", "�������ѻ", "��ͳ������", "������"};
	int gametype=RINFO.gametype;

	if (total<n1[gametype-1])
	{
		sprintf (buf, "\x1b[31;1m����%d�˲μӣ����ܽ���%sɱ����Ϸ��\x1b[m", n1[gametype-1], gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	if (n2[gametype-1]<0 && total-totalk-totalc<totalk)
	{
		sprintf (buf, "\x1b[31;1mƽ������ɱ�ֶ࣬���ܽ���%sɱ����Ϸ��\x1b[m",gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	else if (n2[gametype-1]>total-totalk-totalc)
	{
		sprintf (buf, "\x1b[31;1m��ȥ�趨��ɱ�ֺ;���(��������Ͷ���)�������⣬���ٻ�Ҫ��%d�˲μӣ����ܽ���%sɱ����Ϸ��\x1b[m", n2[gametype-1], gamename[gametype-1]);
		send_msg(mypos, buf);
		kill_msg(mypos);
		end_change_inroom();
		return 1;
	}
	sprintf(buf,"\x1b[31;1m���Խ���%sɱ����Ϸ��\x1b[m", gamename[gametype-1]);
	send_msg(mypos, buf);
	kill_msg(mypos);
	return 0;
}
//autopost�Ĳ���
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
	
	
		
	
