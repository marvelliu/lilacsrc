
void refreshit()
{
	int i, j, me, msgst,col;
	char disp[500];
	char sign[5];
	int color;
	static int inrefresh=0;
	
	if (inrefresh)
		return;
		
	inrefresh=1;
 
	for (i = 0; i < t_lines - 1; i++)
	{
		move(i, 0);
		clrtoeol();
	}
	refreshit_headlines();
	msgst = get_msgt();
	me = mypos;
	for (i = 2; i <= t_lines - 3; i++)
	{
		strcpy(disp,"\x1b[m\x1b[33;1m│");
		col=2;
		if (ipage + i - 2 >= 0 && ipage + i - 2 < rooms[myroom].numplayer+rooms[myroom].numspectator)
		{
			j = getpeople(ipage + i - 2);
			if (j == -1)
				refreshit_cols_fillblank(16,&col,disp);
			else
			{
				refreshit_col2(j,me,sign,&color);
				k_resetcolor();
				k_setfcolor(color,0);
				strcat(disp,sign);
				col++;
				
				refreshit_cols_fillblank(3,&col,disp);
				refreshit_col3(j,me,sign,&color);
				k_setfcolor(color, 1);
				strcat(disp,sign);
				col++;

				refreshit_cols_fillblank(4,&col,disp);
				refreshit_cols_sn(i,&col,j,selected, disp);

				refreshit_col7(j,me,sign,&color);
				k_setfcolor(color,1);
				strcat(disp,sign);
				col++;
				
				refreshit_cols_fillblank(7,&col,disp);
				refreshit_cols_nick(i,&col,j,selected, disp);
			}
		}
		refreshit_cols_fillblank(16,&col,disp);
		refreshit_cols_right(i,&col,msgst,disp);
		move(i, 0);
		prints("%s",disp);
	}
	inrefresh=0;
}
void refreshit_headlines()
{
	char *ss;
	switch(RINFO.status)
	{
	case INROOM_STOP:
		ss="未";
		break;
	case INROOM_NIGHT:
		ss="夜";
		break;
	case INROOM_DAY:
		ss="晨";
		break;
	case INROOM_CHECK:
		ss="指";
		break;
	case INROOM_DEFENCE:
		ss="辨";
		break;
	case INROOM_VOTE:
		ss="投";
		break;
	case INROOM_DARK:
		ss="昏";
		break;
	}
	move(0, 0);
	prints("\x1b[44;33;1m 房间:\x1b[36m%-12s\x1b[33m话题:\x1b[36m%-40s\x1b[33m状态:\x1b[36m%2s \x1b[33mOP:\x1b[36m%2d",
		   rooms[myroom].name,
		   rooms[myroom].title,
		   ss,
		   rooms[myroom].op+1);
	clrtoeol();
	move(1, 0);
	prints("\x1b[m\x1b[33;1m╭\x1b[32m玩家\x1b[33m—————╮╭\x1b[32m讯息\x1b[33m———————————————————————————╮");
	move(t_lines - 2, 0);
	prints("\x1b[33;1m╰———————╯╰—————————————————————————————╯");
}
void refreshit_cols_fillblank (int col_to, int * col, char * disp)
{
	while (*col<col_to)
	{
		strcat(disp," ");
		(*col)++;
	}
}
	
void refreshit_col2 (const int who, const int me, char * sign, int * color)
{
	strcpy(sign," ");
	*color=WHITE;
	if (who>=MAX_PLAYER)
	{
		strcpy(sign,"O");
		*color=GREEN;		
	}
	else if (RINFO.status != INROOM_STOP)
	{
		if ((PINFO(who).flag & PEOPLE_KILLER) && 
			(me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_KILLER) //同伙
			|| ( me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND)) //旁观
			|| (RINFO.gametype ==5) )//打牌
			|| (!(PINFO(who).flag & PEOPLE_ALIVE)&& (PINFO(who).flag & PEOPLE_SEENBAD) ) //死了的坏人
		)
		{
			if (!(PINFO(who).flag & PEOPLE_HIDEKILLER) 		//投死的
				||( (PINFO(who).flag & PEOPLE_KILLER) && me<MAX_PLAYER 
					&& (PINFO(me).flag & PEOPLE_KILLER) ) ) //杀手同伙之间
			{
				strcpy(sign,SIGN_KILLER);
				*color=RED;
			}
		}
		if ((PINFO(who).flag & PEOPLE_MAGIC)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_MAGIC)  					
				|| me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))) 
		{
			strcpy(sign, SIGN_MAGIC);
			*color=RED;
		}
		else if ((PINFO(who).flag & PEOPLE_FOREST)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_FOREST) 
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign,SIGN_FOREST);
			*color=RED;
		}
		else if ((PINFO(who).flag & PEOPLE_BADMAN)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_BADMAN)  
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign, SIGN_BADMAN);
			*color=PINK;
		}
		else if ((PINFO(who).flag & PEOPLE_BUTTERFLY)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_BUTTERFLY)  
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign, SIGN_BUTTERFLY);
			*color=CYAN;
		}
		else if ((PINFO(who).flag & PEOPLE_SHOOTER)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_SHOOTER)  
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign,SIGN_SHOOTER);
			*color=CYAN;
		}
		else if ((PINFO(who).flag & PEOPLE_POLICE) 
			&&((me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_POLICE) )//&& RINFO.gametype !=4)
			|| (RINFO.gametype==5)
			 || (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign,SIGN_POLICE);
			*color=CYAN;
		}
		else if ((PINFO(who).flag & PEOPLE_DOCTOR)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_DOCTOR)
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign, SIGN_DOCTOR);
			*color=CYAN;
		}
		else if ((PINFO(who).flag & PEOPLE_GOODMAN)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_GOODMAN) 
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign, SIGN_GOODMAN);
			*color=CYAN;
		}
		else if ((PINFO(who).flag & PEOPLE_CROW)
			&& (me<MAX_PLAYER && (PINFO(me).flag & PEOPLE_CROW) 
			|| (me>=MAX_PLAYER && !(rooms[myroom].flag & ROOM_SPECBLIND))))
		{
			strcpy(sign, SIGN_CROW);
			*color=GREEN;
		}
	}
}
void refreshit_col3(const int who, const int me, char * sign, int * color)
{
	*color=WHITE;
	strcpy(sign, " ");
	if ((RINFO.status!=INROOM_STOP)&&!(PINFO(who).flag & PEOPLE_ALIVE))
	{
		*color=BLUE;
		strcpy(sign,"X");
	}
	else if ( RINFO.status!=INROOM_STOP
		&& (RINFO.status==INROOM_VOTE || RINFO.gametype==GAME_TSINGHUA)
		&& (PINFO(who).flag & PEOPLE_ALIVE)
		&& (PINFO(who).vote != -1))
	{
		*color=YELLOW;
		strcpy(sign,"v");
	}
}

void refreshit_col7 (const int who, const int me, char * sign, int * color)
{
	*color=WHITE;
	strcpy(sign," ");

	if (RINFO.gametype ==GAME_ORIGINAL || RINFO.gametype==GAME_JOKER1 || RINFO.gametype==GAME_JOKERPIG)
	{
		if ((RINFO.status == INROOM_CHECK || RINFO.status == INROOM_DEFENCE 
			|| RINFO.status == INROOM_VOTE)	&& who<MAX_PLAYER
			&& (PINFO(who).flag & PEOPLE_ALIVE)&& (PINFO(who).flag & PEOPLE_VOTED))
		{
			*color=CYAN;
			strcpy(sign,"$");
		}
		else if (RINFO.gametype==GAME_JOKERPIG && RINFO.status ==INROOM_NIGHT && (PINFO(who).flag & PEOPLE_VOTED))
		{
			*color=CYAN;
			strcpy(sign,"$");
		}
	
	}
	else if (RINFO.gametype ==GAME_CROW || RINFO.gametype ==GAME_BUTTERFLY || RINFO.gametype ==GAME_TSINGHUA)
	{
		if ((RINFO.status == INROOM_CHECK || RINFO.status == INROOM_DEFENCE 
			|| RINFO.status == INROOM_VOTE)	&& who<MAX_PLAYER
			&& (PINFO(who).flag & PEOPLE_ALIVE)&& (PINFO(who).flag & PEOPLE_VOTED)
			&& PINFO(who).vnum>=1)
		{
			sign[0]='a';
			sign[1]='\0';
			sign[0]+=PINFO(who).vnum-1;
			*color=CYAN;
		}
	}
}
void refreshit_cols_sn(int i, int * col, int who, int selected, char* disp)
{
	char buf[10];
	if (ipage + i - 2 == selected)
		k_setfcolor(RED, 1);
	else
		k_resetcolor();
	sprintf(buf,"%2d",(who+1)%100);
	strcat(disp,buf);
	*col+=2;
}

void refreshit_cols_nick(int i, int * col, int who, int selected, char* disp)
{
	char buf[30],buf3[30];
	if (ipage + i - 2 == selected)
		k_setfcolor(RED, 1);
	else
		k_resetcolor();
	safe_substr(buf3,PINFO(who).nick,9);
	sprintf(buf, "%-9.9s",buf3);
	strcat(disp,buf);
	*col+=9;
}

void refreshit_cols_right(int i, int * col, int msgst, char* disp)
{
	strcat(disp, "\x1b[1;33m││\x1b[m");
	*col+=4;
	if (msgst - 1 - (t_lines - 3 - i) - jpage >= 0)
	{
		char *ss = get_msgs(msgst - 1 - (t_lines - 3 - i) - jpage);
		if (ss)
		{
			if (!strcmp(ss, "你被踢了"))
				kicked = 1;
			strcat(disp, ss);
			*col+=strlen2(ss);
		}
	}
	refreshit_cols_fillblank(78, col, disp);
	strcat(disp, "\x1b[m\x1b[1;33m│\x1b[m");
	
}
		
