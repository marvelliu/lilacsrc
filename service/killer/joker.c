#define JOKER1_NUM 54
#define JOKER1_PER_PERSON 9
#define JOKER1_PERSON 6
#define JOKER1_WIN_BY_ERROR 1
#define JOKER1_WIN_BY_MARGIN 2
#define JOKER1_WIN_BY_DECISIVE 3
#include "joker_common.c"
#include "jokerpig.c"
//char * jokername[15]= { "\33[32m����", "\33[31m����", "\33[35m����", "\33[36m÷��", "\33[33mС��", "\33[33m����" };
//char * jokername2[15]= { "\33[32m", "\33[31m", "\33[35m", "\33[36m", "\33[33m", "\33[33m" };
//char * jokernum[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K","E","R"};
char * joker1val[15]= { "4", "5", "6", "7", "8", "9", "10(T)", "J", "Q", "K", "A", "2", "3", "С��","����"};
char * joker1q[9]= {"һ��", "һ��", "����", "����" , "����", "����", "����", "����", "����"};
void start_game_joker1()
{
	int total;
	int i,j;
	int joker1[JOKER1_NUM];
	total=start_game_preparation();
	if (total<6 || total >6)
	{
		send_msg (-1,"������ֻ��6������");
		kill_msg (-1);
		return;
	}
	MAX_JOKER_PLAYER=JOKER1_PERSON;
	JOKER_VALUE_STYLE=JOKER_VALUE_TOP_3;
	JOKER_VALID_STYLE=JOKER_VALID_POINTONLY;
	JOKER_TYPE_STYLE=JOKER_TYPE_BY_COUNT;
	//�ֲ�
	j=0;
	FINISH_NUM=0;//��ǳ�������
	FINISH_WINNER=-1; //���ͷ������һ����
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
			TABLE_POS_LIST[i]=j;//mulelist����˵�˳��ţ���Ӧ�ƶ� mulelist ���->��λ
			POS_LIST[j]=i; //secretvotelist, ��λ->���
			j++;
			FINISH_LIST[i]=-1; //deadlist��¼����˳��
		}
	}
	//����
	for (i=0;i<JOKER1_NUM;i++)
		joker1[i]=i;
	random_sort (joker1, JOKER1_NUM);
	for (j=0;j<JOKER1_PER_PERSON;j++)
	{
		for (i=0;i<JOKER1_PERSON;i++)
		{
			RDECKS[i][j]=(char)joker1[i*JOKER1_PER_PERSON+j];
//����4��˭���
			if (joker1[i*JOKER1_PER_PERSON+j] == 16 )
					CURRENT_TABLEPOS=i;
		}
		
	}
	send_msg(mypos, "ע�⣬�趨ɱ����Ϊ1���Խ�ֹ���죬 ���þ��������Կ��Ƴ�ʱʱ��Ϊ10(n+1)��");
	kill_msg(mypos);
	send_msg(-1,"\33[31;1m��Ϸ��ʼ�ˣ���Ⱥ�г�����54���˿��ơ�\33[m");
	send_msg(-1, "\33[31;1m��˫�����ơ�\33[m");
	send_msg(-1, "   ���Ʒ�����ֱ�������ַ������س������磬������\"3\"��");
	send_msg(-1, "��һ��3��\"qq\"����һ��Q��\"239\"������ðٴ�2��3��9��");
	send_msg(-1, "�ɵ�����9������ģ���e��ʾС����r�������, t����0���");
	send_msg(-1, "��10����Сд�������⡣");
	send_msg(-1, "\33[31;1m���������4�ȳ��ơ�\33[m");
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

	//�����ǲ����ֵ���
	if (mypos>=MAX_PLAYER || PINFO(mypos).style==-1||!(PINFO(mypos).flag&PEOPLE_ALIVE)
			||mypos !=CURRENT_POS)
		return 0;
	if (strcmp(card,"skip")==0||strcmp(card,"SKIP")==0)
	{
		ctrl_t_defence_joker1();
		return 1;
	}
	//�����ǲ��ǳ���
	for (i=0;i<n;i++)
		if (!isvalidjoker1(card[i]))
			return 0;
	//��������Բ����
	if (joker1_multivalue(card, &count,&value))
		return 0;

	//turn ��ʾ�����ˣ�deadlist[] ��¼�����ƵĴ���
	//round��ʾĿǰ�����ͣ�victim��ʾĿǰ���ƴ�С
	//���������ƹ������󣬵�˫���ŶԲ���
	if (CURRENT_JOKER_TYPE !=-1  && (CURRENT_JOKER_TYPE !=count || CURRENT_JOKER_VALUE >=value))
		return 0;
	//������ô���ƣ���û����
	for (i=0;i<JOKER1_PER_PERSON;i++)
		mark[i]=-1;
	

	for (i=0;i<n;i++)
		if (!joker1_havethissuit(card[i],RDECKS[TABLE_POS_LIST[mypos]],mark))
			return 0;
	//mark�˵��ƾͳ�
	sprintf (buf, "\33[36;1m%d %s����%s%s\33[m: ", mypos+1, PINFO(mypos).nick, joker1q[count-1], joker1val[value]);
	j=0;
	for (i=0;i<JOKER1_PER_PERSON;i++)
	{
		if (mark[i]!=-1)
		{
			if (j>0) 
				strcat (buf, "��");
			j++;
			joker1_getname(RDECKS[TABLE_POS_LIST[mypos]][i],buf3);
			sprintf (buf2, "\33[32;1m%s\33[m",buf3 );
			strcat (buf,buf2);
			//�������Ѿ�����
			RDECKS[TABLE_POS_LIST[mypos]][i]=-1;
		}
	}
	send_msg(-1, buf);
	kill_msg(-1);
	CURRENT_JOKER_TYPE=count;
	CURRENT_JOKER_VALUE=value;
	CURRENT_LEADER=mypos; //��¼���һ��������
	joker1_sort_by_value(RDECKS[TABLE_POS_LIST[mypos]], PINFO(mypos).vnum);
	PINFO(mypos).vnum-=count;
	if (PINFO(mypos).vnum<=0)//��������
	{
		sprintf (buf, "\33[31;1m%d %s��������Ѿ������ˣ�\33[m", mypos+1, PINFO(mypos).nick);
		send_msg(-1, buf);
		//�����ǲ���ͷ����
		if (FINISH_LIST[0]==-1)
		{
			sprintf (buf, "\33[31;1m%d %s�ǵ�һ�������Ƶģ�\33[m", mypos+1, PINFO(mypos).nick);
			send_msg(-1,buf);
			if (PINFO(mypos).flag & PEOPLE_POLICE) 
				FINISH_WINNER=1;
			else if (PINFO(mypos).flag & PEOPLE_KILLER)
				FINISH_WINNER=2;
			//��ָ����Ϸ������ʱ���ѯmypos�����ģ�mypos������͵����ˡ�	
		}
		kill_msg(-1);
		//��¼һ��
		FINISH_LIST[FINISH_NUM++]=mypos;
		PINFO(mypos).flag &=~ PEOPLE_ALIVE;
		//�����Ƿ���һ��ʤ����
	}
	if (!check_win())
			joker1_next_player(0);
	return 1;
}

void ctrl_s_defence_joker1()
{//������ģʽ��ѡ��ģʽ֮���л�
	if (PINFO(mypos).flag & PEOPLE_VOTED )
	{
		PINFO(mypos).flag &=~PEOPLE_VOTED;
		send_msg (mypos, "�����Ҫ���ơ�");
		kill_msg (mypos);
	}
	else
	{
		PINFO(mypos).flag |=PEOPLE_VOTED;
		ctrl_t_defence_joker1();
		send_msg (mypos, "һֱ��Ҫ��ֱ���ٰ�Ctrl-S������һ���ơ�");
		kill_msg (mypos);
	}
}
void ctrl_g_defence_joker1()
{
	joker1_showdeck( TABLE_POS_LIST[mypos],mypos);
}

void ctrl_t_defence_joker1()
{//�ֵ��Լ����ƣ����Ҳ��ǵ�һ����
	if (CURRENT_POS==mypos)
	{
		if (CURRENT_JOKER_TYPE==-1)
		{
			send_msg (mypos, "�����ȳ��ƣ��㲻�ܲ�����");
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
		send_msg(CURRENT_POS, "��������ˣ���һ�㣡");
		kill_msg(CURRENT_POS);
	}
	else
	{
		send_msg(CURRENT_POS, "��ʱ�ˣ��������ơ�");
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
	//���ȣ��������û�����ƾ͵��ߣ���һ��ʧ��
	for (i=0; i<JOKER1_PERSON; i++)
		if (PINFO(POS_LIST[i]).vnum>0 && 
				(PINFO(POS_LIST[i]).style==-1
				 ||!(PINFO(POS_LIST[i]).flag&PEOPLE_ALIVE)))
		{
			send_msg(-1,"\33[31;1m���˵����ˡ�\33[m");
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
	//��Σ����û�г�ͷ������Ȼ�������
	if(FINISH_LIST[0]<0)
		return 0;
	v=FINISH_WINNER; //police=1, killer=2
	//������û��һ�������˶������ˡ�
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
		send_msg(-1, "����һ��ȡ�ô�ʤ��");
	else if (mode == JOKER1_WIN_BY_MARGIN)
		send_msg(-1, "����һ��ȡ��Сʤ��");
	else if (mode == JOKER1_WIN_BY_ERROR)
		send_msg(-1, "ɱ�ֵ��ߣ�����ʤ����");
	kill_msg(-1);
	save_result(0);
}
void check_win_badwin_joker1(int mode)
{
	RINFO.status=INROOM_STOP;
	if (mode == JOKER1_WIN_BY_DECISIVE)
		send_msg(-1, "ɱ��һ��ȡ�ô�ʤ��");
	else if (mode == JOKER1_WIN_BY_MARGIN)
		send_msg(-1, "ɱ��һ��ȡ��Сʤ��");
	else if (mode == JOKER1_WIN_BY_ERROR)
		send_msg(-1, "������ߣ�ɱ��ʤ����");
	kill_msg(-1);
	save_result(0);
}
void check_win_draw_joker1()
{
	RINFO.status=INROOM_STOP;
	send_msg(-1, "˫��սƽ���ٽ�������");
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
		strcpy (buf, "\33[33;1mĿǰ���ϵ���\33[m: ");
		if ((PINFO(pos).flag & PEOPLE_POLICE && PINFO(CURRENT_LEADER).flag &PEOPLE_POLICE) ||
			 (PINFO(pos).flag & PEOPLE_KILLER && PINFO(CURRENT_LEADER).flag &PEOPLE_KILLER ))
		{
			strcpy (buf2, "\33[33;1mͬ��");
		}
		else
		{
			strcpy (buf2, "\33[33;1m����");
		}
		strcat (buf,buf2);
				
		sprintf (buf2, "\33[31;1m%d %s\33[m��", CURRENT_LEADER+1, PINFO(CURRENT_LEADER).nick);
		strcat (buf,buf2);
		sprintf (buf2, "\33[33;1m%s%s\33[m: ", joker1q[CURRENT_JOKER_TYPE-1], joker1val[CURRENT_JOKER_VALUE]);
		strcat (buf,buf2);
	}
	else
	{
		if (CURRENT_POS==pos)
			strcpy (buf, "\33[33;1m�ֵ����ȳ���\33[m");
		else
			sprintf (buf, "\33[33;1m�ֵ�%d %s�ȳ���\33[m", CURRENT_POS+1, PINFO(CURRENT_POS).nick);
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
		


