
//char * jokernum[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K","E","R"};
#define JOKER_GETNAME_FULL 	1
#define JOKER_GETNAME_COLORONLY 2
#define JOKER_GETNAME_BRIDGE 	3
#define JOKER_GETNAME_POINTONLY	4
#define JOKER_VALUE_TOP_3 3
#define JOKER_VALUE_TOP_2 2
#define JOKER_VALUE_TOP_A 1
#define JOKER_VALUE_TOP_1 1
#define JOKER_VALUE_TOP_LEVEL 4
#define JOKER_SHOWDECK_COLORONLY 	1
#define JOKER_SHOWDECK_BRIDGE 		2
#define JOKER_SHOWDECK_FULL		0
#define JOKER_VALID_POINTONLY 			1
#define JOKER_VALID_FULL			2
#define JOKER_VALID_NOJOKER_POINTONLY		3
#define JOKER_VALID_NOJOKER_FULL		4
#define JOKER_SORT_BY_VALUE 	 1
#define JOKER_SORT_BY_SUIT	 2
#define JOKER_NEXT_PLAYER_MAYPASS 	1
#define JOKER_NEXT_PLAYER_MAYNOTPASS 	2
#define JOKER_NEXT_PLAYER_BRIDGE_BID   	3
#define JOKER_NEXT_PLAYER_ALLOWFIRST 	1
#define JOKER_NEXT_PLAYER_NOTALLOWFIRST 0
#define JOKER_PASS_NOCARD 	1
#define JOKER_PASS_GIVEUP 	2
#define JOKER_PASS_BRIDGE  	3
#define JOKER_ONTABLE_LAST	1
#define JOKER_ONTABLE_ROUND	2
#define JOKER_ONTABLE_BID	3
#define JOKER_TYPE_BY_NONE	0
#define JOKER_TYPE_BY_COUNT	1
#define JOKER_TYPE_BY_SUIT	2
#define JOKER_TYPE_BY_SUITCOUNT	3
#define JOKER_TYPE_BY_CUSTOM	4
#define JOKER_TYPE_COUNT_MAX	11
int joker_getpos (int tablepos)
{
	return POS_LIST[tablepos];
}
int joker_gettablepos (int pos)
{
	return TABLE_POS_LIST[pos];
}

void joker_getname (char num, char * name, int style)
{
	char * jokername[15]= { "\33[32m黑桃", "\33[31m红桃", "\33[35m方块", "\33[36m梅花", "\33[33m小王", "\33[33m大王" };
	char * jokername2[15]= { "\33[32m", "\33[31m", "\33[35m", "\33[36m", "\33[33m小王", "\33[33m大王" };
	char * jokername3[15]= { "\33[32mS", "\33[31mH", "\33[35mD", "\33[36mC", "\33[33m", "\33[33m" };
	char * jokername4[15]= { "", "", "", "", "", "" };
	char * jokernum[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K","E","R"};

	char * * jname;
	switch (style)
	{
	case JOKER_GETNAME_FULL:
		jname=jokername;
		break;
	case JOKER_GETNAME_COLORONLY:
		jname=jokername2;
		break;
	case JOKER_GETNAME_BRIDGE:
		jname=jokername3;
		break;
	case JOKER_GETNAME_POINTONLY:
		jname=jokername4;
		break;
	default :
		jname=jokername;
	}
	if (num==52)
	{
		strcpy (name,jname[4]);
		strcat (name,jokernum[13]);
	}
	else if (num==53)
	{
		strcpy (name, jname[5]);
		strcat (name, jokernum[14]);
	}
	else 
	{
		strcpy (name, jname[(int)(num/13)]);
		strcat (name, jokernum[num-(int)(num/13)*13]);
	}
	//if (num!=10)
	//	strcat(name," ");
	//strcat(name,"\33[m");
}

int joker_single_num2value ( char num) //if not 4 level is ignored
{
	char top;
	int i;
	int style=JOKER_VALUE_STYLE;
	char level=JOKER_VALUE_LEVEL;
	switch (style)
	{
		case JOKER_VALUE_TOP_3:
			top=3;
			break;
		case JOKER_VALUE_TOP_2:
			top=2;
			break;
		case JOKER_VALUE_TOP_A:
			top=1;
			break;
		case JOKER_VALUE_TOP_LEVEL:
			top=1;
			break;
	}
	if (num==52)
		return 13;
	else if (num==53) 
		return 14;
	else if (style==JOKER_VALUE_TOP_3 || style==JOKER_VALUE_TOP_2 || style==JOKER_VALUE_TOP_A)
	{
		i=num-(int)(num/13)*13;
		if (i<=top-1)
			i+=13-top;
		else 
			i-=top;
		return i;
	}
	else if (style==JOKER_VALUE_TOP_LEVEL)
	{
		i=num-(int)(num/13)*13;
		if (i==level)
			return 12;
		else if (i==0 )
		{
			if (level>=13 || level==0)
				return 12;
			else 
				return 11;
		}
		else if (i<level || level==0)
			return i-1;
		else 
			return i-2;
	}
	return -1;
}
#define JOKER_V2N_FULL 1
#define JOKER_V2N_POINTONLY 2

int joker_single_value2num ( char value, char suit,  int style) //if not 4 level is ignored
{
	char top;
	int i;
	
	switch (JOKER_VALUE_STYLE)
	{
		case JOKER_VALUE_TOP_3:
			top=3;
			break;
		case JOKER_VALUE_TOP_2:
			top=2;
			break;
		case JOKER_VALUE_TOP_A:
			top=1;
			break;
		case JOKER_VALUE_TOP_LEVEL:
			top=1;
			break;
	}
	if (value==13)
	{
		if (style==JOKER_V2N_FULL)
			return 52;
		else if (style==JOKER_V2N_POINTONLY)
			return 13;
	}
	else if (value==14) 
	{
		if (style==JOKER_V2N_FULL)
			return 53;
		else if (style==JOKER_V2N_POINTONLY)
			return 14;
	}
	else if (JOKER_VALUE_STYLE==JOKER_VALUE_TOP_3 || JOKER_VALUE_STYLE==JOKER_VALUE_TOP_2 || JOKER_VALUE_STYLE==JOKER_VALUE_TOP_A)
	{
		i=value;
		if (i>=13-top)
			i=13-top-i;
		else 
			i+=top;
	}
	else if (JOKER_VALUE_STYLE==JOKER_VALUE_TOP_LEVEL)
	{
		i=value;
		if (i==12 && JOKER_VALUE_LEVEL <=12 )
			i=JOKER_VALUE_LEVEL;
		else if (i==12 && JOKER_VALUE_LEVEL>=13)
			i=0;
		else if (i==11 && JOKER_VALUE_LEVEL<13 && JOKER_VALUE_LEVEL>0)
			i=0;
		else if (i<JOKER_VALUE_LEVEL-1 || JOKER_VALUE_LEVEL==0)
			i++;
		else 
			i+=2;
	}
	if (style==JOKER_V2N_FULL)
		i+=suit*13;
	return i;
}
		
int joker_single_name2value ( char card)
{
	char i;
	switch (card)
	{
		case 'E':
		case 'e':
			i=52;
			break;
		case 'R':
		case 'r':
			i=53;
			break;
		case 'A':
		case 'a':
		case '1':
			i=0;
			break;
		case 'K':
		case 'k':
			i=12;
			break;
		case 'Q':
		case 'q':
			i=11;
			break;
		case 'J':
		case 'j':
			i=10;
			break;
		case '0':
		case 'T':
		case 't':
			i=9;
			break;
		default:
			i=card-'1';
			break;
	}
	return joker_single_num2value (i);
}

int joker_num2suit(char num)
{
	if (num==52)
		return 4;
	else if (num==53)
		return 5;
	else 
		return 	(int)(num/13);
}
int joker_card2suit (char card)
{
	switch (card)
	{
		case 'S':
		case 's':
			return 0;
		case 'H':
		case 'h':
			return 1;
		case 'D':
		case 'd':
			return 2;
		case 'C':
		case 'c':
			return 3;
		case 'E':
		case 'e':
			return 4;
		case 'R':
		case 'r':
			return 5;
	}
	return -1;
}


//

void joker_showdeck (int hand, int pos, int max_per_hand, int style)
{
	char buf[40];
	int hispos=joker_getpos(hand);
	if (hispos==pos)
		strcpy (buf, "\33[32;1m你的牌\33[m: ");
	else
		sprintf (buf, "\33[32;1m%d %s的牌\33[m: ", hispos+1, 	PINFO(hispos).nick);
	
	send_msg(pos,buf);
	joker_showdeck_common(RDECKS[hand],max_per_hand, pos, style);
}
void joker_showontable (int pos, int style)
{
	send_msg(pos,"桌上的牌:");
	joker_showdeck_common(ONTABLE_LIST,ONTABLE_NUM, pos, style);
}

void joker_showdeck_common(char * deck,int count, int pos, int style )
{
	char buf[300], buf2[20];//, buf3[20];
	int j,v,s,sv,c;
	char * jokername3[15]= { "\33[32m黑桃S", "\33[31m红桃H", "\33[35m方块D", "\33[36m草花C", "\33[33m", "\33[33m" };
	s=0;
	c=0;
	strcpy (buf, "\33[m\33[1m");
	
	
	for (j=0; j<count;j++)
	{
		v=deck[j];
		if (style == JOKER_SHOWDECK_COLORONLY)
		{
			if (v>=0) //not empty
			{
				if (j>0)
					strcat(buf,",");
			
				if (j%10==9)
				{
					send_msg(pos,buf);
					kill_msg(pos);
					strcpy(buf,"\33[m\33[1m");
			        }
				joker_getname(v,buf2, JOKER_GETNAME_COLORONLY);
				strcat (buf, buf2);
			}
		}
		else if (style ==JOKER_SHOWDECK_FULL)
		{
			if (v>=0)
			{
				if (j>0)
					strcat(buf,",");
			
				if (j%4==3)
				{
					send_msg(pos,buf);
					strcpy(buf,"\33[m\33[1m");
			        }
				joker_getname(v,buf2, JOKER_GETNAME_FULL);
				if (v!=9) //只有10是双字符，其它的加空格
					strcat (buf2, " ");
				strcat (buf, buf2);
			}
		}
		else if (style == JOKER_SHOWDECK_BRIDGE)
		{//必须预先按黑、红、方、梅排列好了
			if (j==0)
			{
				strcpy(buf,jokername3[0]);
				strcat(buf,": ");
			}
			else 
				strcat(buf,",");
			sv=joker_num2suit(v);
			while (sv>s)
			{
				c=0;
				strcat (buf, "\33[m");
				send_msg(pos,buf);
				kill_msg(pos);
				s++;
				strcpy(buf,jokername3[s]);
				strcat(buf,": ");
			}
			if (c%10==9)
			{
				strcat (buf,"\33[m");
				send_msg(pos,buf);
				kill_msg(pos);
				strcpy(buf,jokername3[s]);
				strcat(buf,": ");
			}
			c++;
			joker_getname(v,buf2,JOKER_GETNAME_POINTONLY);
			strcat (buf, buf2);
		}
	}
	strcat(buf,"\33[m");
	send_msg(pos, buf);
	kill_msg(pos);
	if (style==JOKER_SHOWDECK_BRIDGE)
	{
		while (s<3)
		{
			s++;
			strcpy (buf,jokername3[s]);
			strcat (buf, ": ");
			send_msg(pos,buf);
			kill_msg(pos);
		}
	}
}
int joker_isvalid (char c, char s)
{
	int style=JOKER_VALID_STYLE;
	if ( (c >='0' && c<='9') || c=='A' || c=='J' || c== 'Q' || c=='K' || c=='T' ||c=='a'  
		|| c=='j' || c== 'q' || c=='k' || c=='t'|| ((c=='E' || c=='R' || c=='e' || c=='r')
		&&(style!=JOKER_VALID_NOJOKER_POINTONLY) && style!=JOKER_VALID_NOJOKER_FULL))
	{
		if (style==JOKER_VALID_POINTONLY || style==JOKER_VALID_NOJOKER_POINTONLY ||
				s=='S' || s=='s' || s=='H' || s=='h' || s=='D' || s=='d' || s=='C' ||s=='c'
		 ||((s=='E' || s=='e' || s=='R' || s=='r') && style != JOKER_VALID_NOJOKER_FULL))
				return 1;
	}
	return 0;
}

void joker_sort (char * list, int count, int sort_style)
{
	int i,j;
	int t;
	int v1,v2;
	int s1,s2;
	if (count <=1)
		return ;
	for (i=0;i<count-1;i++)
		for (j=i+1; j<count; j++)
			if (list[i]==-1 && list[j]!=-1)
			{
				list[i]=list[j];
				list[j]=-1;
			}
			else if (list[j]!=-1)
			{
				v1=joker_single_num2value(list[i]);
				v2=joker_single_num2value(list[j]);
				s1=joker_num2suit(list[i]);
				s2=joker_num2suit(list[j]);
				if ( ( (sort_style ==JOKER_SORT_BY_VALUE) && ( v1<v2 || (v1==v2 && s1>s2) ) )
					||(sort_style==JOKER_SORT_BY_SUIT && (s1>s2 || (s1==s2 && v1<v2) ))) 
				{
					t=list[i];
					list[i]=list[j];
					list[j]=t;
				}
			}
}
void joker_pass_msg (int pos, int style)
{
	char buf[200];
	if (style== JOKER_PASS_GIVEUP)
		sprintf(buf,"\33[34;1m%d %s不出牌。\33[m", pos+1, PINFO(pos).nick);
	if (style== JOKER_PASS_NOCARD)
		sprintf(buf,"\33[34;1m%d %s无法出牌。\33[m", pos+1, PINFO(pos).nick);
	if (style== JOKER_PASS_BRIDGE)
		sprintf(buf,"\33[34;1m%d %s: \33[31;1m Pass. \33[m", pos+1, PINFO(pos).nick);
	send_msg(-1,buf);
	kill_msg(-1);
}

int joker_next_player(int mode, int style) //mode=0 if first is not allowed, mode=1 if first is allowed
{
	//下一个人是谁呢？
	int i,k;
	i=CURRENT_TABLEPOS; //牌桌上的位置
	k=0;
	set_turn_time();
	while ((!(PINFO(joker_getpos(i)).vnum > 0) ||
		(PINFO(joker_getpos(i)).flag & PEOPLE_VOTED) ||
		(mode==JOKER_NEXT_PLAYER_NOTALLOWFIRST) )&&!k)
	{
		mode=JOKER_NEXT_PLAYER_ALLOWFIRST;
		i++;
		if (i>=MAX_JOKER_PLAYER)
			i=0;
		//假如有人掉线，马上结束
		if (PINFO(joker_getpos(i)).vnum >0 && 
			(PINFO(joker_getpos(i)).style ==-1 || 
			!(PINFO(joker_getpos(i)).flag & PEOPLE_ALIVE)))
		{
			k=check_win();
			return -1;
		}
		//假如已经回到上一次出牌的人那里
		if (joker_getpos(i)==CURRENT_LEADER)
		{	
			if (CURRENT_JOKER_TYPE==-1)
				return -1;
			k=joker_next_round(style);
		}
		if (PINFO(joker_getpos(i)).flag & PEOPLE_VOTED && style == JOKER_NEXT_PLAYER_MAYPASS)
			joker_pass_msg(joker_getpos(i),JOKER_PASS_GIVEUP);
		if (PINFO(joker_getpos(i)).flag & PEOPLE_VOTED && style == JOKER_NEXT_PLAYER_BRIDGE_BID)
			joker_pass_msg(joker_getpos(i),JOKER_PASS_BRIDGE);
		
	}	
	if (style == JOKER_NEXT_PLAYER_MAYPASS || CURRENT_JOKER_TYPE>=0)
	{
		CURRENT_POS=joker_getpos(i);
		CURRENT_TABLEPOS=i;
		send_msg3(-1, "轮到\33[33;1m", joker_getpos(i),"\33[m出牌");
		if (style==JOKER_NEXT_PLAYER_MAYPASS)
			send_msg(joker_getpos(i), 
					"\33[34mCtrl-T跳过，Ctrl-S在跳过和询问之间切换,Ctrl-U查看自己的牌。\33[m");
		else if (style==JOKER_NEXT_PLAYER_MAYNOTPASS)
			send_msg(joker_getpos(i),"\33[34mCtrl-U查看自己的牌。\33[m");
		else if (style==JOKER_NEXT_PLAYER_BRIDGE_BID)
			send_msg(joker_getpos(i),
					"\33[34mCtrl-T跳过，Ctrl-S在跳过和询问之间切换,Ctrl-U查看自己的牌。\33[m");
        }
	else if (style ==JOKER_NEXT_PLAYER_MAYNOTPASS && CURRENT_JOKER_TYPE<0)
	{
		i=CURRENT_TABLEPOS;
		send_msg3(-1, "轮到\33[33;1m", joker_getpos(i),"\33[32;1m领先\33[m出牌");
		send_msg(joker_getpos(i),"\33[34mCtrl-U查看自己的牌。\33[m");
	}

	kill_msg(-1);
	return i;
}	
int joker_next_round(int style)
{
	int j;
	for (j=0;j<MAX_JOKER_PLAYER;j++)
		if (PINFO(joker_getpos(j)).style!=-1 && PINFO(joker_getpos(j)).flag & PEOPLE_ALIVE)
			PINFO(joker_getpos(j)).flag &=~ PEOPLE_VOTED;
	if (style==JOKER_NEXT_PLAYER_MAYPASS)
	{
		CURRENT_JOKER_TYPE=-1;
		CURRENT_JOKER_VALUE=-1;
		ONTABLE_NUM=0;
		return check_win();
	}
	else if (style==JOKER_NEXT_PLAYER_MAYNOTPASS)
	{
		vote_result();
		ONTABLE_NUM=0;
		for (j=0;j<MAX_JOKER_PLAYER;j++)
			ONTABLE_LIST[j]=-1;
		CURRENT_JOKER_TYPE=-1;
		CURRENT_JOKER_VALUE=-1;
		ONTABLE_NUM=0;
		return check_win();
		
	}
		//joker_ontable_show
//		return vote_result(); //结算一下本轮的胜利者和得分
	else if (style==JOKER_NEXT_PLAYER_BRIDGE_BID);
//		return vote_result(); //3pass叫牌结束
	return 0;
}

int joker_havethissuit(char cardname, char suitname, char* deck, int * mark, int count )
{
	int i;
	int n;
	int v;
	int allowlist[count];

	int validstyle=JOKER_VALID_STYLE;
	for (i=0;i<count;i++)
		allowlist[i]=-1;
	n=0;
	v=joker_single_name2value(cardname);
	for (i=0;i<count;i++)
	{
		if (deck[i]<0)
			break;
		if (joker_single_num2value(deck[i])==v && mark[i]<0)
		{
			if ( ((validstyle==JOKER_VALID_FULL||validstyle==JOKER_VALID_NOJOKER_FULL)
					&& (joker_card2suit(suitname)== joker_num2suit(deck[i])))
				||(validstyle==JOKER_VALID_POINTONLY ||validstyle==JOKER_VALID_NOJOKER_POINTONLY))
			{
					allowlist[n++]=i;
			}
		}	
	}
	if (n==0)
		return 0;
	else 
	{
		i=rand()%n;
		mark[allowlist[i]]=1;
		return 1;
	}
}
void joker_ontable_showcurrent(int pos, int style)
{
	char buf[200],buf2[100];
	if (CURRENT_JOKER_TYPE>0 )
	{		
		if (style==JOKER_ONTABLE_LAST)
		{
			strcpy (buf, "\33[33;1m目前桌上的牌\33[m: ");
			sprintf (buf2, "\33[31;1m%d %s\33[m的", CURRENT_LEADER+1, PINFO(CURRENT_LEADER).nick);
			strcat (buf,buf2);
			joker_multiname(CURRENT_JOKER_TYPE, CURRENT_JOKER_VALUE,buf2,JOKER_TYPE_STYLE);
			strcat (buf,buf2);
			send_msg(pos,buf);
			kill_msg(pos);
		}
		else if (style==JOKER_ONTABLE_ROUND)
		{
			int j,k,m;
			j=joker_gettablepos(CURRENT_LEADER);
			k=0;
			strcpy (buf, "\33[33;1m目前桌上的牌\33[m: ");
			send_msg(pos,buf);

			while (k!=ONTABLE_NUM)
			{
				sprintf (buf, "\33[31;1m%d %s\33[m的", joker_getpos(j)+1, PINFO(joker_getpos(j)).nick);
				m=joker_single_num2value(ONTABLE_LIST[k]);
				joker_multiname (ONTABLE_TYPE_LIST[k],m, buf2, JOKER_TYPE_STYLE);
				strcat (buf,buf2);

				send_msg(pos,buf);
				j++;
				k++;
				if (j>=MAX_JOKER_PLAYER)
					j=0;
			}
			kill_msg(pos);
		}
		else if (style==JOKER_ONTABLE_BID)
		{//28
			int j,k;
			j=joker_gettablepos(CURRENT_LEADER);
			k=0;
			strcpy (buf, "\33[33;1m目前叫牌情况\33[m: ");
			send_msg(pos,buf);
			//慢慢写，不着急，内存分配是主要问题
			kill_msg(pos);
		}	
			
	}
	else
	{
		if (CURRENT_POS==pos)
			strcpy (buf, "\33[33;1m轮到你先出牌\33[m");
		else
			sprintf (buf, "\33[33;1m轮到%d %s先出牌\33[m", CURRENT_POS+1, PINFO(CURRENT_POS).nick);
	}
}
#define JOKER_MULTITYPE_SINGLE 		1
#define JOKER_MULTITYPE_PAIR		2
#define JOKER_MULTITYPE_THREE		3
#define JOKER_MULTITYPE_FOUR		4
#define JOKER_MULTITYPE_STRAIGHT	5
#define JOKER_MULTITYPE_SAMESUIT	6
#define JOKER_MULTITYPE_FULLHOUSE	7
#define JOKER_MULTITYPE_FOURONE		8
#define JOKER_MULTITYPE_SSS		9
#define JOKER_MULTITYPE_TRUCK		10
#define JOKER_MULTITYPE_MANY		11

void joker_multiname(int type, int value, char * buf, int style)
{
	char * jokerq[JOKER_TYPE_COUNT_MAX]= {"一张", "一对", "三张", "四张" , "顺子", "同花", "三拖二", "四拖一", "同花顺", "拖拉机",
	"一组牌"};
	char * jokernum[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K","E","R"};
	int num;
	if (style==JOKER_TYPE_BY_COUNT)
	{
		num=joker_single_value2num(value, 0, JOKER_V2N_POINTONLY);
		sprintf (buf, "%s%s", jokerq[type], jokernum[num]);
	}
	else if (style==JOKER_TYPE_BY_SUIT)
	{

		num=joker_single_value2num(value,type,JOKER_V2N_FULL);

		joker_getname(num, buf, JOKER_GETNAME_FULL);

	}
	else if (style==JOKER_TYPE_BY_SUITCOUNT)
	{
		int typecount,typesuit;
		char buf2[20];
		typecount=type % JOKER_TYPE_COUNT_MAX;
		typesuit=(int)(type/JOKER_TYPE_COUNT_MAX);
		num=joker_single_value2num(value,type,JOKER_V2N_FULL);
		joker_getname(num, buf2, JOKER_GETNAME_FULL);
		sprintf (buf, "%s%s", jokerq[typecount],buf2);
	}
	else if (style==JOKER_TYPE_BY_NONE)
	{
		num=joker_single_value2num(value,type,JOKER_V2N_FULL);
		joker_getname(num, buf, JOKER_GETNAME_FULL);
	}
			
	
}

int joker_count_suit (char * deck, int count, int suit)
{
	int num,s,c;
	int i;
	c=0;
	for (i=0;i<count;i++)
	{
		num=deck[i];
		if (num<0) 
			break;
		s=joker_num2suit(num);
		if (suit==s)
			c++;
	}
	return c;
}
		
