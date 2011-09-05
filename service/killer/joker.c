#define JOKER1_NUM 54
#define JOKER1_PER_PERSON 9
#define JOKER1_PERSON 6
#define JOKER1_WIN_BY_ERROR 1
#define JOKER1_WIN_BY_MARGIN 2
#define JOKER1_WIN_BY_DECISIVE 3
#include "joker_common.c"
#include "jokerpig.c"
//char * jokername[15]= { "\33[32m黑桃", "\33[31m红桃", "\33[35m方块", "\33[36m梅花", "\33[33m小王", "\33[33m大王" };
//char * jokername2[15]= { "\33[32m", "\33[31m", "\33[35m", "\33[36m", "\33[33m", "\33[33m" };
//char * jokernum[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K","E","R"};
char * joker1val[15]= { "4", "5", "6", "7", "8", "9", "10(T)", "J", "Q", "K", "A", "2", "3", "小王","大王"};
char * joker1q[9]= {"一张", "一对", "三张", "四张" , "五张", "六张", "七张", "八张", "九张"};
void start_game_joker1()
{
	int total;
	int i,j;
	int joker1[JOKER1_NUM];
	total=start_game_preparation();
	if (total<6 || total >6)
	{
		send_msg (-1,"砸六家只能6个人玩");
		kill_msg (-1);
		return;
	}
	MAX_JOKER_PLAYER=JOKER1_PERSON;
	JOKER_VALUE_STYLE=JOKER_VALUE_TOP_3;
	JOKER_VALID_STYLE=JOKER_VALID_POINTONLY;
	JOKER_TYPE_STYLE=JOKER_TYPE_BY_COUNT;
	//分拨
	j=0;
	FINISH_NUM=0;//标记出完人数
	FINISH_WINNER=-1; //标记头供是哪一方的
	for (i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).style !=-1)
		{
			PINFO(i).flag=PEOPLE_ALIVE;
			if (j%2)
			{
				PINFO(i).flag |=PEOPLE_POLICE;
				RINFO.polices[(int)(j/2)]=i;
			}
			else
			{
				PINFO(i).flag |=PEOPLE_KILLER;
				RINFO.killers[(int)(j/2)]=i;
			}
			TABLE_POS_LIST[i]=j;//mulelist标记人的顺序号，对应牌堆 mulelist 编号->座位
			POS_LIST[j]=i; //secretvotelist, 座位->编号
			j++;
			FINISH_LIST[i]=-1; //deadlist记录出完顺序
		}
	}
	//发牌
	for (i=0;i<JOKER1_NUM;i++)
		joker1[i]=i;
	random_sort (joker1, JOKER1_NUM);
	for (j=0;j<JOKER1_PER_PERSON;j++)
	{
		for (i=0;i<JOKER1_PERSON;i++)
		{
			RDECKS[i][j]=(char)joker1[i*JOKER1_PER_PERSON+j];
//红桃4在谁手里？
			if (joker1[i*JOKER1_PER_PERSON+j] == 16 )
					CURRENT_TABLEPOS=i;
		}
		
	}
	send_msg(mypos, "注意，设定杀手数为1可以禁止聊天， 设置警察数可以控制超时时间为10(n+1)秒");
	kill_msg(mypos);
	send_msg(-1,"\33[31;1m游戏开始了，人群中出现了54张扑克牌。\33[m");
	send_msg(-1, "\33[31;1m请双方看牌。\33[m");
	send_msg(-1, "   出牌方法是直接输入字符串并回车。例如，单个的\"3\"代");
	send_msg(-1, "表一张3，\"qq\"代表一对Q，\"239\"则代表用百搭2和3与9组");
	send_msg(-1, "成的三张9。特殊的，用e表示小王，r代表大王, t或者0则代");
	send_msg(-1, "表10。大小写可以任意。");
	send_msg(-1, "\33[31;1m下面请红桃4先出牌。\33[m");
	kill_msg(-1);
	for(i=0;i<JOKER1_PERSON;i++)
	{
		PINFO(i).vnum=JOKER1_PER_PERSON;
		joker1_sort_by_value(RDECKS[i], PINFO(POS_LIST[i]).vnum);
		joker1_showdeck(i, POS_LIST[i]);
	}
	set_turn_time();
	goto_defence();
}
		

void goto_defence_joker1 ()
{
	start_change_inroom();
	RINFO.status=INROOM_DEFENCE;
	end_change_inroom();
	kill_msg(-1);
	CURRENT_JOKER_TYPE=-1;
	joker1_next_player(1);
}
	
int normalmsg_defence_joker1 (char * card)
{
	int n,i,j;
	int mark[JOKER1_PER_PERSON];
	int count, value;
	char buf[200],buf2[20],buf3[20];
	n=strlen(card);

	//看看是不是轮到你
	if (mypos>=MAX_PLAYER || PINFO(mypos).style==-1||!(PINFO(mypos).flag&PEOPLE_ALIVE)
			||mypos !=CURRENT_POS)
		return 0;
	if (strcmp(card,"skip")==0||strcmp(card,"SKIP")==0)
	{
		ctrl_t_defence_joker1();
		return 1;
	}
	//看看是不是出牌
	for (i=0;i<n;i++)
		if (!isvalidjoker1(card[i]))
			return 0;
	//看看牌配对不配对
	if (joker1_multivalue(card, &count,&value))
		return 0;

	//turn 表示出牌人，deadlist[] 记录出完牌的次序
	//round表示目前的牌型，victim表示目前的牌大小
	//看看出的牌够不够大，单双三张对不对
	if (CURRENT_JOKER_TYPE !=-1  && (CURRENT_JOKER_TYPE !=count || CURRENT_JOKER_VALUE >=value))
		return 0;
	//看看怎么出牌，有没有牌
	for (i=0;i<JOKER1_PER_PERSON;i++)
		mark[i]=-1;
	

	for (i=0;i<n;i++)
		if (!joker1_havethissuit(card[i],RDECKS[TABLE_POS_LIST[mypos]],mark))
			return 0;
	//mark了的牌就出
	sprintf (buf, "\33[36;1m%d %s出了%s%s\33[m: ", mypos+1, PINFO(mypos).nick, joker1q[count-1], joker1val[value]);
	j=0;
	for (i=0;i<JOKER1_PER_PERSON;i++)
	{
		if (mark[i]!=-1)
		{
			if (j>0) 
				strcat (buf, "，");
			j++;
			joker1_getname(RDECKS[TABLE_POS_LIST[mypos]][i],buf3);
			sprintf (buf2, "\33[32;1m%s\33[m",buf3 );
			strcat (buf,buf2);
			//这张牌已经出了
			RDECKS[TABLE_POS_LIST[mypos]][i]=-1;
		}
	}
	send_msg(-1, buf);
	kill_msg(-1);
	CURRENT_JOKER_TYPE=count;
	CURRENT_JOKER_VALUE=value;
	CURRENT_LEADER=mypos; //记录最后一个出牌人
	joker1_sort_by_value(RDECKS[TABLE_POS_LIST[mypos]], PINFO(mypos).vnum);
	PINFO(mypos).vnum-=count;
	if (PINFO(mypos).vnum<=0)//出完啦！
	{
		sprintf (buf, "\33[31;1m%d %s手里的牌已经出完了！\33[m", mypos+1, PINFO(mypos).nick);
		send_msg(-1, buf);
		//看看是不是头供！
		if (FINISH_LIST[0]==-1)
		{
			sprintf (buf, "\33[31;1m%d %s是第一个出完牌的！\33[m", mypos+1, PINFO(mypos).nick);
			send_msg(-1,buf);
			if (PINFO(mypos).flag & PEOPLE_POLICE) 
				FINISH_WINNER=1;
			else if (PINFO(mypos).flag & PEOPLE_KILLER)
				FINISH_WINNER=2;
			//别指望游戏结束的时候查询mypos的旗帜，mypos可能早就掉线了。	
		}
		kill_msg(-1);
		//记录一下
		FINISH_LIST[FINISH_NUM++]=mypos;
		PINFO(mypos).flag &=~ PEOPLE_ALIVE;
		//看看是否有一方胜利！
	}
	if (!check_win())
			joker1_next_player(0);
	return 1;
}

void ctrl_s_defence_joker1()
{//在跳过模式和选择模式之间切换
	if (PINFO(mypos).flag & PEOPLE_VOTED )
	{
		PINFO(mypos).flag &=~PEOPLE_VOTED;
		send_msg (mypos, "你决定要出牌。");
		kill_msg (mypos);
	}
	else
	{
		PINFO(mypos).flag |=PEOPLE_VOTED;
		ctrl_t_defence_joker1();
		send_msg (mypos, "一直不要，直到再按Ctrl-S或者下一组牌。");
		kill_msg (mypos);
	}
}
void ctrl_g_defence_joker1()
{
	joker1_showdeck( TABLE_POS_LIST[mypos],mypos);
}

void ctrl_t_defence_joker1()
{//轮到自己出牌，而且不是第一个出
	if (CURRENT_POS==mypos)
	{
		if (CURRENT_JOKER_TYPE==-1)
		{
			send_msg (mypos, "该你先出牌，你不能不出。");
			kill_msg (mypos);
		}
		else
		{
			joker1_pass_msg(mypos);
			joker1_next_player(0);
		}
	}
	
}
void joker1_timeout_skip()
{
	int who;
	who=POS_LIST[CURRENT_POS];
	if (CURRENT_JOKER_TYPE==-1)
	{
		send_msg(CURRENT_POS, "该你出牌了，快一点！");
		kill_msg(CURRENT_POS);
	}
	else
	{
		send_msg(CURRENT_POS, "超时了，放弃出牌。");
		kill_msg(CURRENT_POS);
		joker1_pass_msg(CURRENT_POS);
		PINFO(CURRENT_POS).flag |=PEOPLE_VOTED;
		joker1_next_player(0);
	}
}
int check_win_joker1()
{
	int i,v,totalk, totalp;
	int tr[3];
	//首先，如果有人没出完牌就掉线，这一方失败
	for (i=0; i<JOKER1_PERSON; i++)
		if (PINFO(POS_LIST[i]).vnum>0 && 
				(PINFO(POS_LIST[i]).style==-1
				 ||!(PINFO(POS_LIST[i]).flag&PEOPLE_ALIVE)))
		{
			send_msg(-1,"\33[31;1m有人掉线了。\33[m");
			kill_msg(-1);
			if (PINFO(POS_LIST[i]).flag & PEOPLE_POLICE) 
				
			{
				check_win_badwin_joker1(JOKER1_WIN_BY_ERROR);
				return 2;
			}
			else
			{
				check_win_goodwin_joker1(JOKER1_WIN_BY_ERROR);
				return 1;
			}
		}
	//其次，如果没有出头供，自然不会结束
	if(FINISH_LIST[0]<0)
		return 0;
	v=FINISH_WINNER; //police=1, killer=2
	//看看有没有一方三个人都出完了。
	check_win_census(tr);
	totalk=tr[0];
	totalp=tr[1];
	if (totalk>0 && totalp>0)
		return 0;
	if (totalk==0 && v==2)
	{
		if (totalp<3)
			check_win_badwin_joker1(JOKER1_WIN_BY_MARGIN);
		else 
			check_win_badwin_joker1(JOKER1_WIN_BY_DECISIVE);
		return 2;
	}
	else if (totalp==0 && v==1)
	{
		if (totalk<3)
			check_win_goodwin_joker1(JOKER1_WIN_BY_MARGIN);
		else 
			check_win_goodwin_joker1(JOKER1_WIN_BY_DECISIVE);
		return 1;
	}
	else
	{
		check_win_draw_joker1();
		return 3;
	}
}
void check_win_goodwin_joker1(int mode)
{
	RINFO.status=INROOM_STOP;
	if (mode == JOKER1_WIN_BY_DECISIVE)
		send_msg(-1, "警察一方取得大胜！");
	else if (mode == JOKER1_WIN_BY_MARGIN)
		send_msg(-1, "警察一方取得小胜！");
	else if (mode == JOKER1_WIN_BY_ERROR)
		send_msg(-1, "杀手掉线，警察胜利。");
	kill_msg(-1);
	save_result(0);
}
void check_win_badwin_joker1(int mode)
{
	RINFO.status=INROOM_STOP;
	if (mode == JOKER1_WIN_BY_DECISIVE)
		send_msg(-1, "杀手一方取得大胜！");
	else if (mode == JOKER1_WIN_BY_MARGIN)
		send_msg(-1, "杀手一方取得小胜！");
	else if (mode == JOKER1_WIN_BY_ERROR)
		send_msg(-1, "警察掉线，杀手胜利。");
	kill_msg(-1);
	save_result(0);
}
void check_win_draw_joker1()
{
	RINFO.status=INROOM_STOP;
	send_msg(-1, "双方战平，再接再厉。");
	kill_msg(-1);
	save_result(0);
}
		
		
		
	
		
		
				
void joker1_pass_msg (int pos)
{
	joker_pass_msg(pos, JOKER_PASS_GIVEUP);
}

void joker1_next_player(int mode) //mode=0 if first is not allowed, mode=1 if first is allowed
{
	int i;
	i=joker_next_player(mode, JOKER_NEXT_PLAYER_MAYPASS);
	joker1_showdeck (i, POS_LIST[i]);
}	
	

int joker1_havethissuit(char card, char* deck, int * mark)
{
	return joker_havethissuit(card,0,deck,mark,JOKER1_PER_PERSON);
}

int isvalidjoker1 (char c)
{
	return joker_isvalid (c, 0);
}
int iswildcard (char c)
{
	if ( c=='E' || c=='R' || c=='2' || c== '3' || c=='e' || c=='r')
		return 1;
	else 
		return 0;
}
	
void joker1_sort_by_value (char * list, int count)
{
	joker_sort(list, count, JOKER_SORT_BY_VALUE);
}
void joker1_showcurrent (int pos)
{
	char buf[200],buf2[100];
	if (CURRENT_JOKER_TYPE>0)
	{		
		strcpy (buf, "\33[33;1m目前桌上的牌\33[m: ");
		if ((PINFO(pos).flag & PEOPLE_POLICE && PINFO(CURRENT_LEADER).flag &PEOPLE_POLICE) ||
			 (PINFO(pos).flag & PEOPLE_KILLER && PINFO(CURRENT_LEADER).flag &PEOPLE_KILLER ))
		{
			strcpy (buf2, "\33[33;1m同伴");
		}
		else
		{
			strcpy (buf2, "\33[33;1m对手");
		}
		strcat (buf,buf2);
				
		sprintf (buf2, "\33[31;1m%d %s\33[m的", CURRENT_LEADER+1, PINFO(CURRENT_LEADER).nick);
		strcat (buf,buf2);
		sprintf (buf2, "\33[33;1m%s%s\33[m: ", joker1q[CURRENT_JOKER_TYPE-1], joker1val[CURRENT_JOKER_VALUE]);
		strcat (buf,buf2);
	}
	else
	{
		if (CURRENT_POS==pos)
			strcpy (buf, "\33[33;1m轮到你先出牌\33[m");
		else
			sprintf (buf, "\33[33;1m轮到%d %s先出牌\33[m", CURRENT_POS+1, PINFO(CURRENT_POS).nick);
	}
	send_msg(pos, buf);
	kill_msg(pos);
}
	
	
	
		
void joker1_showdeck(int i,int pos)
{
	joker1_showcurrent(pos);
	joker_showdeck(i,pos,JOKER1_PER_PERSON,JOKER_SHOWDECK_COLORONLY);
}			
	

void joker1_getname (char num, char * name)
{
	joker_getname(num, name, JOKER_GETNAME_FULL );
}

void joker1_getname2 (char num, char * name)
{
	joker_getname(num, name, JOKER_GETNAME_COLORONLY);
}

int joker1_singlevalue (char num)
{
	return joker_single_num2value(num);
}
int joker1_singlevalue2 (char card)
{
	return joker_single_name2value(card);
}
	
	
int joker1_multivalue (char * card, int * count, int * value)
{
	int i,n;
	int v2;
	char v;
	n=strlen(card);
	i=0;
	*count=n;
	if (n<1)
		return 1;
	if (n==1)
	{
		*value=joker1_singlevalue2(card[0]);
		return 0;
	}
//n>1
	do 
		v=card[i++];
	while (iswildcard(v) && i<n);
	if (i<n)		
	{
		for (i=0;i<n;i++)
			if (!iswildcard(card[i]) && joker1_singlevalue2(card[i])!=joker1_singlevalue2(v))
				return 1;
		*value= joker1_singlevalue2(v);
		return 0;
	}
	else //all wild card 2<3<Q<W
	{
		*value=joker1_singlevalue2(card[0]);
		for (i=1;i<n;i++)
		{
			v2=joker1_singlevalue2(card[i]);
			if (v2 < *value)
				*value=v2;
		}
		return 0;
	}
}
		


