#define JOKER2_NUM 52
#define JOKER2_PER_PERSON 13
#define JOKER2_PERSON 4
#define JOKER2_WIN_BY_ERROR 1
#define JOKER2_WIN_BY_MARGIN 2
#define JOKER2_WIN_BY_DECISIVE 3

char * joker2name[4]= { "����", "����", "����", "÷��"};
char * joker2num[15]= { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
char * joker2val[15]= { "2", "3","4", "5", "6", "7", "8", "9", "10(T)", "J", "Q", "K", "A"};
//char * joker2q[9]= {"һ��", "һ��", "����", "����" , "����", "����", "����", "����", "����"};
void start_game_joker2()
{
	int total;
	int i,j;
	int joker2[JOKER2_NUM];
	char buf[80];
	total=start_game_preparation();
	if (total<4 || total >4)
	{
		send_msg (-1,"����ֻ��4������");
		kill_msg (-1);
		return;
	}
	//�ֲ�
	j=0;
//	RINFO.deadnum=0;//��ǳ�������
//	RINFO.deadpointer=0;
//	RINFO.mulenum=-1; //���ͷ������һ����
	for (i=0;i<MAX_PLAYER;i++)
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
		RINFO.mulelist[i]=j;//mulelist����˵�˳��ţ���Ӧ�ƶ� mulelist ���->��λ
		RINFO.secretvotelist[j]=i; //secretvotelist, ��λ->���
		j++;
		RINFO.deadlist[i]=-1; //deadlist��¼����˳��
	}
	//����
	for (i=0;i<JOKER2_NUM;i++)
		joker2[i]=i;
	random_sort (joker2, JOKER2_NUM);
	for (j=0;j<JOKER2_PER_PERSON;j++)
	{
		for (i=0;i<JOKER2_PERSON;i++)
		{
			RINFO.seq_action[i][j]=(char)joker2[i*JOKER2_PER_PERSON+j];
//����4��˭���
			if (joker2[i*JOKER2_PER_PERSON+j] == 16 )
					RINFO.numvoted=i;
		}
		
	}
	send_msg(mypos, "ע�⣬�趨ɱ����Ϊ1���Խ�ֹ���죬 ���þ��������Կ��Ƴ�ʱʱ��Ϊ10(n+1)��");
	kill_msg(mypos);
	send_msg(-1,"\33[31;1m��Ϸ��ʼ�ˣ���Ⱥ�г�����52���˿��ơ�\33[m");
	send_msg(-1, "\33[31;1m��˫�����ơ�\33[m");
//	send_msg(-1, "   ���Ʒ�����ֱ�������ַ������س������磬������\"3\"��");
//	send_msg(-1, "��һ��3��\"qq\"����һ��Q��\"239\"������ðٴ�2��3��9��");
//	send_msg(-1, "�ɵ�����9������ģ���e��ʾС����r�������, t����0���");
//	send_msg(-1, "��10����Сд�������⡣");
//	send_msg(-1, "\33[31;1m���������4�ȳ��ơ�\33[m");
	kill_msg(-1);
	for(i=0;i<JOKER2_PERSON;i++)
	{
//		PINFO(i).vnum=JOKER2_PER_PERSON;
		joker2_sort_by_suit(RINFO.seq_action[i], PINFO(RINFO.secretvotelist[i]).vnum);
		joker2_showdeck(i, RINFO.secretvotelist[i]);
	}
//	RINFO.lastturntime=bbstime(NULL);
	goto_check();
}
		

void goto_defence_joker2 ()
{
	start_change_inroom();
	RINFO.status=INROOM_DEFENCE;
	end_change_inroom();
	kill_msg(-1);
	RINFO.round=-1;
	joker2_next_player(1);
}
	
int normalmsg_defence_joker2 (char * card)
{
	int n,i,j;
	//int deck[JOKER2_PER_PERSON];
	//int mark[JOKER2_PER_PERSON];
	char v;
	int count, value;
	char buf[200],buf2[20],buf3[20];
	n=strlen(card);

	//�����ǲ����ֵ���
	if (mypos>=MAX_PLAYER || PINFO(mypos).style==-1||!(PINFO(mypos).flag&PEOPLE_ALIVE)
			||mypos !=RINFO.turn)
		return 0;
	//�����ǲ��ǳ���
	for (i=0;i<n;i++)
		if (!isvalidjoker2(card[i]))
			return 0;

	if (RINFO.round !=-1  && (RINFO.round !=count || RINFO.victim >=value))
		return 0;
	//������ô���ƣ���û����
	for (i=0;i<JOKER2_PER_PERSON;i++)
		mark[i]=-1;
	

	for (i=0;i<n;i++)
		if (!joker2_havethissuit(card[i],RINFO.seq_action[RINFO.mulelist[mypos]],mark))
			return 0;
	//mark�˵��ƾͳ�
	sprintf (buf, "\33[36;1m%d %s����%s%s\33[m: ", mypos+1, PINFO(mypos).nick, joker2q[count-1], joker2val[value]);
	j=0;
	for (i=0;i<JOKER2_PER_PERSON;i++)
	{
		if (mark[i]!=-1)
		{
			if (j>0) 
				strcat (buf, "��");
			j++;
			joker2_getname(RINFO.seq_action[RINFO.mulelist[mypos]][i],buf3);
			sprintf (buf2, "\33[32;1m%s\33[m",buf3 );
			strcat (buf,buf2);
			//�������Ѿ�����
			RINFO.seq_action[RINFO.mulelist[mypos]][i]=-1;
		}
	}
	send_msg(-1, buf);
	kill_msg(-1);
	RINFO.round=count;
	RINFO.victim=value;
	RINFO.nokill=mypos; //��¼���һ��������
	joker2_sort_by_value(RINFO.seq_action[RINFO.mulelist[mypos]], PINFO(mypos).vnum);
	PINFO(mypos).vnum-=count;
	if (PINFO(mypos).vnum<=0)//��������
	{
		sprintf (buf, "\33[31;1m%d %s��������Ѿ������ˣ�\33[m", mypos+1, PINFO(mypos).nick);
		send_msg(-1, buf);
		//�����ǲ���ͷ����
		if (RINFO.deadlist[0]==-1)
		{
			sprintf (buf, "\33[31;1m%d %s�ǵ�һ�������Ƶģ�\33[m", mypos+1, PINFO(mypos).nick);
			send_msg(-1,buf);
			if (PINFO(mypos).flag & PEOPLE_POLICE) 
				RINFO.mulenum=1;
			else if (PINFO(mypos).flag & PEOPLE_KILLER)
				RINFO.mulenum=2;
			//��ָ����Ϸ������ʱ���ѯmypos�����ģ�mypos������͵����ˡ�	
		}
		kill_msg(-1);
		//��¼һ��
		RINFO.deadlist[RINFO.deadnum++]=mypos;
		PINFO(mypos).flag &=~ PEOPLE_ALIVE;
		//�����Ƿ���һ��ʤ����
	}
	if (!check_win())
			joker2_next_player(0);
	return 1;
}

void ctrl_s_defence_joker2()
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
		ctrl_t_defence_joker2();
		send_msg (mypos, "һֱ��Ҫ��ֱ���ٰ�Ctrl-S������һ���ơ�");
		kill_msg (mypos);
	}
}
void ctrl_g_defence_joker2()
{
	joker2_showdeck( RINFO.mulelist[mypos],mypos);
}

void ctrl_t_defence_joker2()
{//�ֵ��Լ����ƣ����Ҳ��ǵ�һ����
	if (RINFO.turn==mypos)
	{
		if (RINFO.round==-1)
		{
			send_msg (mypos, "�����ȳ��ƣ��㲻�ܲ�����");
			kill_msg (mypos);
		}
		else
		{
			joker2_pass_msg(mypos);
			joker2_next_player(0);
		}
	}
	
}
void joker2_timeout_skip()
{
	int who;
	who=RINFO.secretvotelist[RINFO.turn];
	if (RINFO.round==-1)
	{
		send_msg(RINFO.turn, "��������ˣ���һ�㣡");
		kill_msg(RINFO.turn);
	}
	else
	{
		send_msg(RINFO.turn, "��ʱ�ˣ��������ơ�");
		kill_msg(RINFO.turn);
		joker2_pass_msg(RINFO.turn);
		PINFO(RINFO.turn).flag |=PEOPLE_VOTED;
		joker2_next_player(0);
	}
}
int check_win_joker2()
{
	int i,v,totalk, totalp;
	int tr[3];
	//���ȣ��������û�����ƾ͵��ߣ���һ��ʧ��
	for (i=0; i<JOKER2_PERSON; i++)
		if (PINFO(RINFO.secretvotelist[i]).vnum>0 && 
				(PINFO(RINFO.secretvotelist[i]).style==-1
				 ||!(PINFO(RINFO.secretvotelist[i]).flag&PEOPLE_ALIVE)))
		{
			send_msg(-1,"\33[31;1m���˵����ˡ�\33[m");
			kill_msg(-1);
			if (PINFO(RINFO.secretvotelist[i]).flag & PEOPLE_POLICE) 
				
			{
				check_win_badwin_joker(JOKER2_WIN_BY_ERROR);
				return 2;
			}
			else
			{
				check_win_goodwin_joker(JOKER2_WIN_BY_ERROR);
				return 1;
			}
		}
	//��Σ����û�г�ͷ������Ȼ�������
	if(RINFO.deadlist[0]<0)
		return 0;
	v=RINFO.mulenum; //police=1, killer=2
	//������û��һ�������˶������ˡ�
	check_win_census(tr);
	totalk=tr[0];
	totalp=tr[1];
	if (totalk>0 && totalp>0)
		return 0;
	if (totalk==0 && v==2)
	{
		if (totalp<3)
			check_win_badwin_joker2(JOKER2_WIN_BY_MARGIN);
		else 
			check_win_badwin_joker2(JOKER2_WIN_BY_DECISIVE);
		return 2;
	}
	else if (totalp==0 && v==1)
	{
		if (totalk<3)
			check_win_goodwin_joker2(JOKER2_WIN_BY_MARGIN);
		else 
			check_win_goodwin_joker2(JOKER2_WIN_BY_DECISIVE);
		return 1;
	}
	else
	{
		check_win_draw_joker2();
		return 3;
	}
}
void check_win_goodwin_joker2(int mode)
{
	RINFO.status=INROOM_STOP;
	if (mode == JOKER2_WIN_BY_DECISIVE)
		send_msg(-1, "����һ��ȡ�ô�ʤ��");
	else if (mode == JOKER2_WIN_BY_MARGIN)
		send_msg(-1, "����һ��ȡ��Сʤ��");
	else if (mode == JOKER2_WIN_BY_ERROR)
		send_msg(-1, "ɱ�ֵ��ߣ�����ʤ����");
	kill_msg(-1);
	save_result(0);
}
void check_win_badwin_joker2(int mode)
{
	RINFO.status=INROOM_STOP;
	if (mode == JOKER2_WIN_BY_DECISIVE)
		send_msg(-1, "ɱ��һ��ȡ�ô�ʤ��");
	else if (mode == JOKER2_WIN_BY_MARGIN)
		send_msg(-1, "ɱ��һ��ȡ��Сʤ��");
	else if (mode == JOKER2_WIN_BY_ERROR)
		send_msg(-1, "������ߣ�ɱ��ʤ����");
	kill_msg(-1);
	save_result(0);
}
void check_win_draw_joker2()
{
	RINFO.status=INROOM_STOP;
	send_msg(-1, "˫��սƽ���ٽ�������");
	kill_msg(-1);
	save_result(0);
}
		
		
		
	
		
		
				
void joker2_pass_msg (int pos)
{
	char buf[200];
	sprintf(buf,"\33[34;1m%d %s�����ơ�\33[m", pos+1, PINFO(pos).nick);
	send_msg(-1,buf);
	kill_msg(-1);
}

void joker2_next_player(int mode) //mode=0 if first is not allowed, mode=1 if first is allowed
{
	char buf[200];
	//��һ������˭�أ�
	int i,j;
	i=RINFO.numvoted;
	RINFO.lastturntime=bbstime(NULL);

	while (!(PINFO(RINFO.secretvotelist[i]).vnum > 0) || (PINFO(RINFO.secretvotelist[i]).flag & PEOPLE_VOTED) || (!mode) )
	{
		mode=1;
		i++;
		if (i>=JOKER2_PERSON)
			i=0;
		//�������˵��ߣ����Ͻ���
		if (PINFO(RINFO.secretvotelist[i]).vnum >0 && (PINFO(RINFO.secretvotelist[i]).style ==-1 || !(PINFO(RINFO.secretvotelist[i]).flag & PEOPLE_ALIVE)))
		{
			check_win();
			return;
		}
		//�����Ѿ��ص���һ�γ��Ƶ�������
		if (RINFO.secretvotelist[i]==RINFO.nokill)
		{	
			if (RINFO.round==-1)
				return;
			joker2_next_round();
		}
		if (PINFO(RINFO.secretvotelist[i]).flag & PEOPLE_VOTED)
			joker2_pass_msg(RINFO.secretvotelist[i]);
	}			
	RINFO.turn=RINFO.secretvotelist[i];
	RINFO.numvoted=i;
	sprintf (buf, "\33[35;0m�ֵ�%d %s����\33[m", RINFO.secretvotelist[i]+1, PINFO(RINFO.secretvotelist[i]).nick);
	send_msg(-1, buf);
	send_msg(RINFO.secretvotelist[i], "\33[35;0mCtrl-T������Ctrl-S��������ѯ��֮���л�,Ctrl-U�鿴�Լ����ơ�\33[m");
	kill_msg(-1);
	joker2_showdeck (i, RINFO.secretvotelist[i]);
	}	
	
void joker2_next_round()
{
	int j;
	RINFO.round=-1;
	RINFO.victim=-1;
	for (j=0;j<JOKER2_PERSON;j++)
		if (PINFO(RINFO.secretvotelist[j]).style!=-1 && PINFO(RINFO.secretvotelist[j]).flag & PEOPLE_ALIVE)
			PINFO(RINFO.secretvotelist[j]).flag &=~ PEOPLE_VOTED;
	check_win();
}

int joker2_havethissuit(char card, char* deck, int * mark)
{
	int i,j;
	int n;
	int v;
	int allowlist[JOKER2_PER_PERSON];
	for (i=0;i<JOKER2_PER_PERSON;i++)
		allowlist[i]=-1;
	n=0;
	v=joker2_singlevalue2(card);
	for (i=0;i<JOKER2_PER_PERSON;i++)
	{
		if (deck[i]<0)
			break;
		if (joker2_singlevalue(deck[i])==v && mark[i]<0)
			allowlist[n++]=i;
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

int isvalidjoker2 (char c)
{
	if (c >='0' && c<='9' )
		return 1;
	if (c=='A' || c=='E' || c=='R' || c=='J' || c== 'Q' || c=='K' || c=='T')
		return 1;
	if (c=='a' || c=='e' || c=='r' || c=='j' || c== 'q' || c=='k' || c=='t')
		return 1;
	return 0;
}
int iswildcard (char c)
{
	if ( c=='E' || c=='R' || c=='2' || c== '3' || c=='e' || c=='r')
		return 1;
	else 
		return 0;
}
	
void joker2_sort_by_suit (char * list, int count)
{
	int i,j;
	int t;
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
				if (joker2_singlevalue(list[i])<joker2_singlevalue(list[j])
					|| (joker2_singlevalue(list[i])==joker2_singlevalue(list[j])
						&& list[i]>list[j]))
				{
					t=list[i];
					list[i]=list[j];
					list[j]=t;
				}
			}
}
void joker2_showcurrent (int pos)
{
	char buf[200],buf2[100];
	if (RINFO.round>0)
	{		
		strcpy (buf, "\33[33;1mĿǰ���ϵ���\33[m: ");
		sprintf (buf2, "\33[31;1m%d %s\33[m��", RINFO.nokill+1, PINFO(RINFO.nokill).nick);
		strcat (buf,buf2);
		sprintf (buf2, "\33[33;1m%s%s\33[m: ", joker2q[RINFO.round-1], joker2val[RINFO.victim]);
		strcat (buf,buf2);
	}
	else
	{
		if (RINFO.turn==pos)
			strcpy (buf, "\33[33;1m�ֵ����ȳ���\33[m");
		else
			sprintf (buf, "\33[33;1m�ֵ�%d %s�ȳ���\33[m", RINFO.turn+1, PINFO(RINFO.turn).nick);
	}
	send_msg(pos, buf);
	kill_msg(pos);
}
	
	
	
		
void joker2_showdeck(int i,int pos)
{
	char buf[300],buf2[20],buf3[20];
	int j,v;
	joker2_showcurrent(pos);
	strcpy (buf, "\33[32;1m�����\33[m: ");
	for (j=0; j< JOKER2_PER_PERSON;j++)
	{
		v=RINFO.seq_action[i][j];
		if (v>=0)
		{
			if (j>0)
				strcat(buf, ",");
			joker2_getname(v,buf3);
			sprintf (buf2, "\33[32;1m%s\33[m",buf3);
			strcat (buf, buf2);
		}	

	}
	send_msg(pos, buf);
	kill_msg(pos);
}			
	

void joker2_getname (char num, char * name)
{
	int i,j;
	if (num==52)
	{
		strcpy (name,joker2name[4]);
		strcat (name,joker2num[13]);
	}
	else if (num==53)
	{
		strcpy (name, joker2name[5]);
		strcat (name, joker2num[14]);
	}
	else 
	{
		strcpy (name, joker2name[(int)(num/13)]);
		strcat (name, joker2num[num-(int)(num/13)*13]);
	}
}
int joker2_singlevalue (char num)
{
	int i;
	if (num==52)
		return 13;
	else if (num==53) 
		return 14;
	else
	{
		i=num-(int)(num/13)*13;
		if (i<=2)
			i+=10;
		else 
			i-=3;
		return i;
	}
}
int joker2_singlevalue2 (char card)
{
	if (card=='A' || card=='a'|| card =='1')
		return 10;
	else if (card=='K' || card=='k')
		return 9;
	else if (card=='Q' || card=='q')
		return 8;
	else if (card=='J' || card=='j')
		return 7;
	else if (card=='0' ||card=='T' ||card=='t')
		return 6;
	else if (card<='9' && card>='4') 
		return (int)(card-'4');
	else if (card=='3')
		return 12;
	else if (card=='2')
		return 11;
	else if (card=='E'||card=='e')
		return 13;
	else if (card=='R'||card=='r')
		return 14;
}
	
	
int joker2_multivalue (char * card, int * count, int * value)
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
		*value=joker2_singlevalue2(card[0]);
		return 0;
	}
//n>1
	do 
		v=card[i++];
	while (iswildcard(v) && i<n);
	if (i<n)		
	{
		for (i=0;i<n;i++)
			if (!iswildcard(card[i]) && joker2_singlevalue2(card[i])!=joker2_singlevalue2(v))
				return 1;
		*value= joker2_singlevalue2(v);
		return 0;
	}
	else //all wild card 2<3<Q<W
	{
		*value=joker2_singlevalue2(card[0]);
		for (i=1;i<n;i++)
		{
			v2=joker2_singlevalue2(card[i]);
			if (v2 < *value)
				*value=v2;
		}
		return 0;
	}
}
		


