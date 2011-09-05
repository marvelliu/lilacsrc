#define JOKERPIG_NUM 52
#define JOKERPIG_PER_PERSON 13
#define JOKERPIG_PERSON 4
#define JOKER_WIN_BY_ERROR 1
#define JOKER_WIN_BY_MARGIN 2
#define JOKER_WIN_BY_DECISIVE 3
#define JOKERPIG_DOUBLE_FLAG_PIG 0
#define JOKERPIG_DOUBLE_FLAG_HEARTA 1
#define JOKERPIG_DOUBLE_FLAG_HEART5 4
#define JOKERPIG_DOUBLE_FLAG_SHEEP 2
#define JOKERPIG_DOUBLE_FLAG_DOUBLE 3
#define JOKERPIG_FULLRED	13
#define JOKERPIG_PIG 0
#define JOKERPIG_HEART 1
#define JOKERPIG_SHEEP 2
#define JOKERPIG_DOUBLE 3

void start_game_jokerpig()
{
	int total;
	int i,j;
	int joker1[JOKERPIG_NUM];
	total=start_game_preparation();
	if (total<4 || total >4)
	{
		send_msg (-1,"����Ŀǰֻ��4������");
		kill_msg (-1);
		return;
	}
	MAX_JOKER_PLAYER=JOKERPIG_PERSON;
	JOKER_VALUE_STYLE=JOKER_VALUE_TOP_A;
	JOKER_VALUE_LEVEL=0;
	JOKER_VALID_STYLE=JOKER_VALID_NOJOKER_FULL;
	JOKER_TYPE_STYLE=JOKER_TYPE_BY_SUIT;
	//�ֲ�
	j=0;
	ONTABLE_NUM=0;//��ǳ�������
	for (i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).style !=-1)
		{
			PINFO(i).flag=PEOPLE_ALIVE;
			TABLE_POS_LIST[i]=j;//mulelist����˵�˳��ţ���Ӧ�ƶ� mulelist ���->��λ
			POS_LIST[j]=i; //secretvotelist, ��λ->���
			j++;
			JOKER_POINT_RECORD_TMP[i]=0;
		}
	}
	//����
	for (i=0;i<JOKERPIG_NUM;i++)
		joker1[i]=i;
	random_sort (joker1, JOKERPIG_NUM);
	for (j=0;j<JOKERPIG_PER_PERSON;j++)
	{
		for (i=0;i<MAX_JOKER_PLAYER;i++)
		{
			RDECKS[i][j]=(char)joker1[i*JOKERPIG_PER_PERSON+j];
//����2��˭���
			if (joker1[i*JOKERPIG_PER_PERSON+j] == 14 )
					CURRENT_TABLEPOS=i;
		}
		
	}
	//send_msg(mypos, "ע�⣬�趨ɱ����Ϊ1���Խ�ֹ���죬 ���þ��������Կ��Ƴ�ʱʱ��Ϊ10(n+1)��");
	//kill_msg(mypos);
	send_msg(-1,"\33[31;1m��Ϸ��ʼ�ˣ���Ⱥ�г�����52���˿��ơ�\33[m");
	send_msg(-1, "\33[31;1m��˫�����ơ�\33[m");
	send_msg(-1, "   ���Ʒ�����ֱ�������ַ������س������磬\"H3\"��");
	send_msg(-1, "�����3��\"DA\"�����飬\"cq\"����ݻ�Q��\"ST\"���Ǻ���10");
	send_msg(-1, "t����0����10����Сд�������⡣");
	send_msg(-1, "��Ctrl-T���Բ鿴�ۼƵ÷֡�");
	kill_msg(-1);
	for(i=0;i<MAX_JOKER_PLAYER;i++)
	{
		RDECKS_COUNT(POS_LIST[i])=JOKERPIG_PER_PERSON;
	}
	set_turn_time();
	goto_night();
}
void goto_night_jokerpig ()
{
	int i;
	RINFO.status=INROOM_NIGHT;
	send_msg(-1,"����ˣ���ʼ���ƣ�");
	send_msg(-1, "���Ұ���ͷ�����ơ�");

	kill_msg(-1);
	ONTABLE_NUM=0;
	RINFO.time_autolimit=1;
	for (i=0;i<MAX_PLAYER;i++)
		JOKERPIG_DOUBLE_FLAG[i]=-1;
}
		
void ctrl_s_night_jokerpig(int select)
{
	if (PINFO(mypos).flag & PEOPLE_ALIVE && PINFO(mypos).style !=-1)
	{
		if (PINFO(mypos).flag & PEOPLE_VOTED)
		{
			PINFO(mypos).flag &=~PEOPLE_VOTED;
			send_msg(mypos, "�㲻ͷ���ˡ�");
		}
		else
		{
			PINFO(mypos).flag |=PEOPLE_VOTED;
			send_msg(mypos, "��Ҫͷ����");
		}

	}
	kill_msg(mypos);
}
void ctrl_t_night_jokerpig()
{
	jokerpig_showscore(mypos);	
}

int normalmsg_night_jokerpig(char* card)
{
	int value, suit;
	int dflg,pcsn;
	char buf[200], buf2[20];
	int n,mark;
	n=strlen(card);
	if (mypos>=MAX_PLAYER || PINFO(mypos).style==-1||!(PINFO(mypos).flag&PEOPLE_ALIVE))
			return 0;
	//�����ǲ��ǳ���
	if (n<2 || n>2) 
		return 0;
	if (!isvalidjokerpig(card[0],card[1]))
	{
		char t;
		t=card[0];
		card[0]=card[1];
		card[1]=t;
		if (!isvalidjokerpig(card[0],card[1]))
			return 0;
	}
	//��������Բ����:�����û�г������Ƶ���
	value=joker_single_name2value(card[1]);
	suit=joker_card2suit(card[0]);

	//�����ǲ��ǿ���������
	dflg=jokerpig_isshowcard2(card[1],card[0]);
	pcsn=jokerpig_ispointcard2(card[1],card[0]);
	if (dflg<0 || pcsn<0)
		return 0;

	
	//������ô���ƣ���û����
	mark=-1;
	
	if (!jokerpig_havethissuit(card[1],card[0],RDECKS[TABLE_POS_LIST[mypos]],&mark))
			return 0;

	//��û���������ƣ�
	if (mark>=ONTABLE_NUM)
			return 0;

	//�ǲ����Ѿ������ˣ�
	if (JOKERPIG_DOUBLE_FLAG[dflg]>0)
			return 0;

	//mark�˵��ƾͳ�
	sprintf (buf, "\33[36;1m%d %s����: ", mypos+1, PINFO(mypos).nick);
	jokerpig_getname(RDECKS[TABLE_POS_LIST[mypos]][mark],buf2);
	strcat (buf,buf2);

	
	//�������Ѿ�����
	JOKERPIG_DOUBLE_FLAG[dflg]=1;
	//ע�����Ƶı�Ƿ�ʽ
	JOKERPIG_POINTCARD_LIST[pcsn-1]=-TABLE_POS_LIST[mypos]-10;
	send_msg(-1, buf);
	kill_msg(-1);
	return 1;

}
//����
void timeout_night_jokerpig (int type)
{
	int i;
	int dflg, pcsn;
	char buf[200],buf2[20];
	if (type !=TIMEOUT_AUTO)
		return;
	set_auto_time();
	if (ONTABLE_NUM < JOKERPIG_PER_PERSON)
	{
		ONTABLE_NUM++;
		for (i=0;i<MAX_JOKER_PLAYER; i++)
		{
			if (PINFO(POS_LIST[i]).flag & PEOPLE_VOTED )
			{
				//ͷ�������һ����
				dflg=jokerpig_isshowcard(RDECKS[i][ONTABLE_NUM-1]);
				pcsn=jokerpig_ispointcard(RDECKS[i][ONTABLE_NUM-1]);
				if (dflg>=0 && pcsn>=0 && JOKERPIG_DOUBLE_FLAG[dflg]<0)
				{
					JOKERPIG_DOUBLE_FLAG[dflg]=2;
					JOKERPIG_POINTCARD_LIST[pcsn-1]=-TABLE_POS_LIST[mypos]-10;
					//��������У��Ǹ���������ƣ�-1����û�г�ʾ��-10�͸�С������
					send_msg(-1, "\33[31;1m����ͷ����һ���ƣ�\33[m");
					sprintf (buf, "\33[36;1m%d %s����: ", POS_LIST[i]+1, PINFO(POS_LIST[i]).nick);	
					jokerpig_getname(RDECKS[i][ONTABLE_NUM-1],buf2);
					strcat (buf,buf2);
					send_msg(-1, buf);
					kill_msg(-1);
				}
			}
			joker_sort (RDECKS[i], ONTABLE_NUM,JOKER_SORT_BY_SUIT);
			joker_showdeck (i, POS_LIST[i], ONTABLE_NUM, JOKER_SHOWDECK_BRIDGE);
			send_msg(i, "�����У������������Ƶ����֣�ͷ������Ctrl-S��");
			kill_msg(i);
		}
		kill_msg(-1);
	}
	else if (ONTABLE_NUM >= JOKERPIG_PER_PERSON && ONTABLE_NUM-3 < JOKERPIG_PER_PERSON)
	{
		send_msg(-1,"������ˣ������������Ƶ����֣�");
		kill_msg(-1);
		ONTABLE_NUM++;
	}
	else
	{
		RINFO.time_autolimit=0;
		send_msg(-1,"�����ˣ���ʼ���ƣ�");
		kill_msg(-1);
		goto_defence();
	}
}

void goto_defence_jokerpig ()
{
	int i;
	start_change_inroom();
	RINFO.status=INROOM_DEFENCE;
	end_change_inroom();
	CURRENT_JOKER_TYPE=-1;
	for (i=0;i<MAX_PLAYER;i++)
	{
		PINFO(i).flag &=~ PEOPLE_VOTED;
		ONTABLE_LIST[i]=-1;
		JOKERPIG_DOUBLE_FIRSTROUND[i]=-1;
	}
	//��start_game_jokerpig�У��趨��CURRENT_TABLEPOSΪ����2�ĳ�����
	CURRENT_POS=POS_LIST[CURRENT_TABLEPOS];
	CURRENT_LEADER=CURRENT_POS;
	ONTABLE_NUM=0;
	
	send_msg(-1, "\33[31;1m���������2�ȳ��ơ�\33[m");
	kill_msg(-1);
	
	jokerpig_next_player(JOKER_NEXT_PLAYER_ALLOWFIRST);
}
void ctrl_s_defence_jokerpig()
{
	jokerpig_showcurrent(mypos);
}
void ctrl_g_defence_jokerpig()
{
	jokerpig_showdeck( TABLE_POS_LIST[mypos],mypos);
}
void ctrl_t_defence_jokerpig()
{
	jokerpig_showscore(mypos);	
}
	
int normalmsg_defence_jokerpig (char * card)
{
	int n,i,j,pcsn;
//	int mark[JOKERPIG_PER_PERSON];
	int mark;
	int count, value,suit;
	char buf[200],buf2[20],buf3[20];
	n=strlen(card);
	//�����ǲ����ֵ���
	if (mypos>=MAX_PLAYER || PINFO(mypos).style==-1||!(PINFO(mypos).flag&PEOPLE_ALIVE)
			||mypos !=CURRENT_POS)
		return 0;
	//�����ǲ��ǳ���
	if (n<2 || n>2) 
		return 0;
	if (!isvalidjokerpig(card[0],card[1]))
	{
		char t;
		t=card[0];
		card[0]=card[1];
		card[1]=t;
		if (!isvalidjokerpig(card[0],card[1]))
			return 0;
	}
	//��������Բ����:�����û�г������Ƶ���
	value=joker_single_name2value(card[1]);
	suit=joker_card2suit(card[0]);
	//turn ��ʾ�����ˣ�deadlist[] ��¼ÿ�ֵ���
	//round��ʾĿǰ�Ƿ���ƣ�victim��ʾĿǰ�Ļ�ɫ
	//���������ƹ������󣬵�˫���ŶԲ���
	if (CURRENT_JOKER_TYPE!=-1)
		count=joker_count_suit(RDECKS[TABLE_POS_LIST[mypos]],RDECKS_COUNT(mypos), CURRENT_JOKER_TYPE);
	else 
		count=joker_count_suit(RDECKS[TABLE_POS_LIST[mypos]],RDECKS_COUNT(mypos), suit);
		
	if (CURRENT_JOKER_TYPE !=-1  && (CURRENT_JOKER_TYPE !=suit && count>0))
		return 0;
	//������ô���ƣ���û����
	//for (i=0;i<JOKERPIG_PER_PERSON;i++)
	//	mark[i]=-1;
	mark=-1;

	if (!jokerpig_havethissuit(card[1],card[0],RDECKS[TABLE_POS_LIST[mypos]],&mark))
			return 0;
	//����Ļ���������˵��Ʋ��ǵ��ţ���ô��һ���ǲ��ܳ��ġ�
	j=jokerpig_isshowcard2(card[1],card[0]);
	pcsn=jokerpig_ispointcard2(card[1],card[0]);
	if (j>=0 && JOKERPIG_DOUBLE_FIRSTROUND[j]<0 && JOKERPIG_POINTCARD_LIST[pcsn-1]<=-10)
	{
		if (count>1 || CURRENT_JOKER_TYPE==-1)
		{
			send_msg(mypos, "���˵��ƣ���һ�ֲ��ܳ���");
			kill_msg(mypos);
			return 0;
		}
	}
	//mark�˵��ƾͳ�
	//joker_multiname (suit, value, buf2, JOKER_TYPE_BY_SUIT);
	sprintf (buf, "\33[36;1m%d %s����: ", mypos+1, PINFO(mypos).nick);
	//strcat(buf,buf2);
	jokerpig_getname(RDECKS[TABLE_POS_LIST[mypos]][mark],buf3);
	sprintf (buf2, "\33[32;1m%s\33[m",buf3 );
	strcat (buf,buf2);
	send_msg(-1, buf);
	kill_msg(-1);

	//�������Ѿ�����
	i=joker_single_value2num(value, suit,  JOKER_V2N_FULL);
	j=jokerpig_isimagecard(i);
	if (j>=0)
	{
		jokerpig_showimage(buf,j);
		send_msg(-1,buf);
		kill_msg(-1);
	}
	
	RDECKS[TABLE_POS_LIST[mypos]][mark]=-1;
	ONTABLE_LIST[ONTABLE_NUM]=i;
	ONTABLE_TYPE_LIST[ONTABLE_NUM]=joker_num2suit(i);
	ONTABLE_NUM+=1;
	if (CURRENT_JOKER_TYPE==-1)
	{
		CURRENT_LEADER=mypos; //��¼��˭��ʼ��
		CURRENT_JOKER_TYPE=suit;
	}
//	RINFO.victim=value;
//	CURRENT_LEADER=mypos; //��¼���һ��������
	jokerpig_sort_by_suit(RDECKS[TABLE_POS_LIST[mypos]], RDECKS_COUNT(mypos));
	RDECKS_COUNT(mypos)-=1;
	jokerpig_next_player(JOKER_NEXT_PLAYER_NOTALLOWFIRST);
	return 1;
}
void jokerpig_timeout_skip()
{
	send_msg(CURRENT_POS, "��������ˣ���һ�㣡");
	kill_msg(CURRENT_POS);
}

void vote_result_jokerpig()
{
	//˭Ӯ�ˣ�
	char ontable[MAX_JOKER_PLAYER];
	int i,j,k;
	//���ȱ��Ƴ����б�
	j=CURRENT_LEADER;
	for (i=0;i<MAX_JOKER_PLAYER;i++)
	{
		ontable[j++]=ONTABLE_LIST[i];
		if (j>=MAX_JOKER_PLAYER)
			j=0;
	}
	//˭Ӯ�ˣ�
	k=jokerpig_largest(ontable, MAX_JOKER_PLAYER);
	//�ѼƷ��ƽ���ʤ����
	jokerpig_winpoints(ontable, k);
	//��ʾһ��ս��
	jokerpig_showcurrent(-1);
	send_msg3(-1, "\33[33;1m",POS_LIST[k], "\33[31;1mӮ�������ơ�\33[m");
	//�ɻ�ñ���ʤ�������ȳ�
	CURRENT_TABLEPOS=k;
	CURRENT_POS=POS_LIST[CURRENT_TABLEPOS];
	CURRENT_LEADER=CURRENT_POS;
	//������ر��趨�� ���Ʋ������ֳ��Ĵ���
	switch (CURRENT_JOKER_TYPE)
	{
		case 0:
			JOKERPIG_DOUBLE_FIRSTROUND[JOKERPIG_DOUBLE_FLAG_PIG]=1;
			break;
		case 1:
			JOKERPIG_DOUBLE_FIRSTROUND[JOKERPIG_DOUBLE_FLAG_HEARTA]=1;
			JOKERPIG_DOUBLE_FIRSTROUND[JOKERPIG_DOUBLE_FLAG_HEART5]=1;
			break;
		case 2:
			JOKERPIG_DOUBLE_FIRSTROUND[JOKERPIG_DOUBLE_FLAG_SHEEP]=1;
			break;
		case 3:
			JOKERPIG_DOUBLE_FIRSTROUND[JOKERPIG_DOUBLE_FLAG_DOUBLE]=1;
			break;
	}	
}
	
			



int check_win_jokerpig()
{
	int i;
	//���ȣ��������û�����ƾ͵��ߣ���һ��ʧ��
	for (i=0; i<MAX_JOKER_PLAYER; i++)
		if (RDECKS_COUNT(joker_getpos(i))>0 && 
				(PINFO(joker_getpos(i)).style==-1
				 ||!(PINFO(joker_getpos(i)).flag&PEOPLE_ALIVE)))
		{
			send_msg(-1,"\33[31;1m���˵����ˡ�\33[m");
			kill_msg(-1);
			//check_win_draw_joker(JOKER_WIN_BY_ERROR);
			RINFO.status=INROOM_STOP;
			return 3;
		}
	//��Σ����û�г����ƣ���Ȼ�������
	if (RDECKS_COUNT(joker_getpos(0))>0)
		return 0;
//	if(ONTABLE_LIST[0]<0)
//		return 0;
	//������ͳ�Ƶ÷�
	jokerpig_showcurrent(-1);
	for (i=0;i<MAX_JOKER_PLAYER;i++)
		JOKER_POINT_RECORD[i]+=JOKER_POINT_RECORD_TMP[i];
	jokerpig_showscore(-1);
	start_change_inroom();
	RINFO.status=INROOM_STOP;
	end_change_inroom();
	return 1;
}

		
		
	
		
		
				

void jokerpig_next_player(int mode) //mode=0 if first is not allowed, mode=1 if first is allowed
{
	int i;
	i=joker_next_player(mode, JOKER_NEXT_PLAYER_MAYNOTPASS);
	
	//CURRENT_TABLEPOS=i;
	//CURRENT_POS=POS_LIST[CURRENT_TABLEPOS];
	//if (CURRENT_JOKER_TYPE<0)
	//	CURRENT_LEADER=CURRENT_POS;

	jokerpig_showdeck (i, joker_getpos(i));
}	
	

int jokerpig_havethissuit(char card, char suit, char* deck, int * mark)
{
	int marklist[JOKERPIG_PER_PERSON];
	int i;
	for (i=0;i<JOKERPIG_PER_PERSON;i++)
		marklist[i]=-1;
	i=joker_havethissuit(card,suit,deck,marklist,JOKERPIG_PER_PERSON);
	if (i)
	{
		for (i=0;i<JOKERPIG_PER_PERSON;i++)
			if (marklist[i]>0)
			{
				*mark=i;
				break;
			}
		return 1;
	}
	return 0;
}

int isvalidjokerpig (char s, char c)
{
	return joker_isvalid (c, s);
}
	
void jokerpig_sort_by_suit (char * list, int count)
{
	joker_sort(list, count, JOKER_SORT_BY_SUIT);
}
void jokerpig_showcurrent_doubleflags(int pos)
{
	char buf[200];
	jokerpig_getdc(JOKERPIG_DOUBLE_FLAG_HEARTA,"����A",13, buf);
	send_msg(pos, buf);
	jokerpig_getdc(JOKERPIG_DOUBLE_FLAG_HEART5,"����5",4, buf);
	send_msg(pos, buf);
	jokerpig_getdc(JOKERPIG_DOUBLE_FLAG_DOUBLE,"�ݻ�10",14, buf);
	send_msg(pos, buf);
	jokerpig_getdc(JOKERPIG_DOUBLE_FLAG_SHEEP,"����J",15, buf);
	send_msg(pos, buf);
	jokerpig_getdc(JOKERPIG_DOUBLE_FLAG_PIG,"����Q",16, buf);
	send_msg(pos, buf);
	kill_msg(pos);
}

void jokerpig_showcurrent (int pos)
{
	char scards[20];
	char buf[200];
	int i,j,k;
	long p;
	//��ʾ��ǰһ��δ�������Ƶ����
	//���Ƶļ�¼�͵��Ƽ�¼����JOKERPIG_POINTCARD_LIST��
	jokerpig_showcurrent_doubleflags(pos);
	//��ʾ��ǰһ�ֵĵ��ƺ͵÷�
	for (i=0;i<MAX_JOKER_PLAYER; i++)
	{
		for (j=0; j<20;j++)
			scards[j]=-1;
		k=0;
		for (j=0;j<16;j++)
			if (JOKERPIG_POINTCARD_LIST[j]==i)
				scards[k++]=jokerpig_pcsn2num(j+1);
		if (k>0)
		{
			send_msg3(pos, "\33[33;1m", POS_LIST[i], "\33[31;1m�ĵ��������");
			joker_showdeck_common(scards,k,pos, JOKER_SHOWDECK_FULL);
			p=jokerpig_countpoint (scards, k, JOKERPIG_DOUBLE_FLAG);
			sprintf(buf,"�ܼƵ÷֣�%ld", p);
			JOKER_POINT_RECORD_TMP[i]=p;
			send_msg(pos,buf);
			kill_msg(pos);
		}
	}
	//��ʾ���˵���
	joker_ontable_showcurrent(pos, JOKER_ONTABLE_ROUND);
	send_msg3(pos, "\33[31;1mĿǰ��\33[33;1m", CURRENT_POS, "\33[31;1m���ơ�");
}

void jokerpig_getdc (int doubleflag, char * cardname, int pcsn, char * buf)
{
	char buf2[50];
	if (JOKERPIG_DOUBLE_FLAG[doubleflag]>0)
	{
		if (JOKERPIG_DOUBLE_FLAG[doubleflag]==1)
			sprintf(buf, "%s�Ѿ�����, ", cardname);
		else
			sprintf(buf, "%s�Ѿ�ͷ��, ", cardname);
			
		if (JOKERPIG_POINTCARD_LIST[pcsn-1]<-1)
		{
			sprintf(buf2, "%d %s��ʾ%s��", -(JOKERPIG_POINTCARD_LIST[pcsn-1]+10)+1, PINFO(-(JOKERPIG_POINTCARD_LIST[pcsn-1]+10)).nick, cardname);
			strcat(buf,buf2);
		}
		else
		{
			sprintf(buf2, "%d %s���%s��", JOKERPIG_POINTCARD_LIST[pcsn-1]+1, PINFO(JOKERPIG_POINTCARD_LIST[pcsn-1]).nick, cardname);
			strcat(buf,buf2);
		}
	}
	else
	{
		sprintf (buf, "%sû�����ơ�", cardname);
	}
}
	
	
		
void jokerpig_showdeck(int i,int pos)
{
	joker_ontable_showcurrent(pos, JOKER_ONTABLE_ROUND);
	send_msg3(pos, "\33[31;1mĿǰ��\33[33;1m", CURRENT_POS, "\33[31;1m���ơ�");
//	jokerpig_showcurrent(pos);
	joker_showdeck(i,pos,RDECKS_COUNT(POS_LIST[i]),JOKER_SHOWDECK_BRIDGE);
}			
	

void jokerpig_getname (char num, char * name)
{
	joker_getname(num, name, JOKER_GETNAME_FULL );
}

void jokerpig_getname2 (char num, char * name)
{
	joker_getname(num, name, JOKER_GETNAME_COLORONLY);
}
//�йؼǷ��Ƶ��趨
int jokerpig_pcsn2num (int pcsn)
{
	int cn,sn,i;
	if(pcsn<=13)
	{
		sn=1;
		cn=pcsn-1;
	}
	else if (pcsn==14)
	{
		sn=3;
		cn=8;
	}
	else if (pcsn==15)
	{
		sn=2;
		cn=9;
	}
	else if (pcsn==16)
	{
		sn=0;
		cn=10;
	}
	i=joker_single_value2num(cn, sn, JOKER_V2N_FULL);
	return i;
}
			
int jokerpig_ispointcard (char num)
{
	int cn, sn;
	sn=joker_num2suit(num);
	cn=joker_single_num2value(num);
	if (sn==1) //����
		return cn+1;//2-A= 1-13
	if (sn==3 && cn==8)//�ݻ�10
		return 14;
	if (sn==2 && cn==9)//����J
		return 15;
	if (sn==0 && cn==10)//����Q
		return 16;
	return 0;
}
	
int jokerpig_ispointcard2 (char card, char suit)
{
	int cn, sn;
	sn=joker_card2suit(suit);
	cn=joker_single_name2value(card);
	if (sn==1) //����
		return cn+1;//2-A= 1-13
	if (sn==3 && cn==8)//�ݻ�10
		return 14;
	if (sn==2 && cn==9)//����J
		return 15;
	if (sn==0 && cn==10)//����Q
		return 16;
	return 0;
}
int jokerpig_isshowcard(char num)
{
	int cn, sn;
	sn=joker_num2suit(num);
	cn=joker_single_num2value(num);
	if (sn==1 && cn==3 )
		return JOKERPIG_DOUBLE_FLAG_HEART5;
	if (sn==1 &&cn==12) //����5,A
		return JOKERPIG_DOUBLE_FLAG_HEARTA;
	if (sn==3 && cn==8)//�ݻ�10
		return JOKERPIG_DOUBLE_FLAG_DOUBLE;
	if (sn==2 && cn==9)//����J
		return JOKERPIG_DOUBLE_FLAG_SHEEP;
	if (sn==0 && cn==10)//����Q
		return JOKERPIG_DOUBLE_FLAG_PIG;
	return -1;
}

int jokerpig_isshowcard2(char card, char suit)
{
	int cn, sn;
	sn=joker_card2suit(suit);
	cn=joker_single_name2value(card);
	if (sn==1 && cn==3 )
		return JOKERPIG_DOUBLE_FLAG_HEART5;
	if (sn==1 &&cn==12) //����5,A
		return JOKERPIG_DOUBLE_FLAG_HEARTA;
	if (sn==3 && cn==8)//�ݻ�10
		return JOKERPIG_DOUBLE_FLAG_DOUBLE;
	if (sn==2 && cn==9)//����J
		return JOKERPIG_DOUBLE_FLAG_SHEEP;
	if (sn==0 && cn==10)//����Q
		return JOKERPIG_DOUBLE_FLAG_PIG;
	return -1;
}
int jokerpig_isimagecard (int num)
{
	int cn,sn;
	sn=joker_num2suit(num);
	cn=joker_single_num2value(num);
	if (sn==3 && cn==8)//�ݻ�10
		return JOKERPIG_DOUBLE;
	if (sn==2 && cn==9)//����J
		return JOKERPIG_SHEEP;
	if (sn==0 && cn==10)//����Q
		return JOKERPIG_PIG;
	return -1;
}
	

//doubleflag����policenum
long jokerpig_countpoint (char * list, int count, char * doubleflag)
{
	int i,cn, sn;
	int db;
	long p;
	int countred,v;
	db=1;
	p=0;
	countred=0;
	for (i=0;i<count;i++)
	{
		sn=joker_num2suit(list[i]);
		cn=joker_single_num2value(list[i]);
		if (sn==3 && cn==8)
		{
			if (doubleflag[JOKERPIG_DOUBLE_FLAG_DOUBLE]>0)
				db*=doubleflag[JOKERPIG_DOUBLE_FLAG_DOUBLE]*4;
			else
				db*=2;
		}
		else if (sn==2 && cn==9)
		{
			if (doubleflag[JOKERPIG_DOUBLE_FLAG_SHEEP]>0)
			{
				if (doubleflag[JOKERPIG_DOUBLE_FLAG_SHEEP]==2)
					p+=400;
				else if (doubleflag[JOKERPIG_DOUBLE_FLAG_SHEEP]==1)
					p-=200;
			}
			else
				p-=100;
		}
		else if (sn==0 && cn==10)
		{
			if (doubleflag[JOKERPIG_DOUBLE_FLAG_PIG]>0)
				p+=200* doubleflag[JOKERPIG_DOUBLE_FLAG_PIG];
			else 
				p+=100;
		}
		else if (sn==1)
		{
			countred++;
			
			if (cn>=0 && cn<=2)//2,3,4
			{
				if (doubleflag[JOKERPIG_DOUBLE_FLAG_HEART5]>0)
					v=10;
				else
					v=0;
			}
			else if (cn>=3 && cn<=8) //5-10
				v=10;
			else if (cn>=9) //JQKA
				v=(cn-7)*10;
			if (doubleflag[JOKERPIG_DOUBLE_FLAG_HEARTA]>0)
				p+=v*doubleflag[JOKERPIG_DOUBLE_FLAG_HEARTA]*2;
			else
				p+=v;
		}
	}
//ȫ��
	if (countred==JOKERPIG_FULLRED)
	{
		v=200;
		if (doubleflag[JOKERPIG_DOUBLE_FLAG_HEART5]>0)
			v=230;
		if (doubleflag[JOKERPIG_DOUBLE_FLAG_HEARTA]>0)
			v*=doubleflag[JOKERPIG_DOUBLE_FLAG_HEARTA]*2;
		p-=v*2;
	}
	if (p==0 && count==1 && db>=2)
	{
		p-=25*db;
	}
	else
	{
		p*=db;
	}
	return p;
}
				
//4���ƺ�Ľ���				
int jokerpig_largest (char * list,int count)
{
	int suit;
	int i,j;
	int s,v,vm;
	suit=CURRENT_JOKER_TYPE;
	j=0;
	vm=joker_single_num2value(list[0]);
	//�޽�
	for (i=1;i<count;i++)
	{
		s=joker_num2suit(list[i]);
		v=joker_single_num2value(list[i]);	
		if (s==suit && v>vm)
		{
			vm=v;
			j=i;
		}
	}
	return j;
}
void jokerpig_winpoints (char *list,  int who)
{
	int i,j;
	j=0;
	for (i=0;i<MAX_JOKER_PLAYER;i++)
	{
		j=jokerpig_ispointcard(list[i]);
		if (j>0)
			JOKERPIG_POINTCARD_LIST[j-1]=who;
	}
}
//�ۼƵ÷ֵ���ʾ
void jokerpig_showscore(int pos)
{
	char buf[200];
	int i;
	for (i=0;i<MAX_JOKER_PLAYER;i++)
	{
		sprintf (buf,"\33[31;1m%d %s���ۼƵ÷�: \33[32;1m%d\33[m", POS_LIST[i]+1, PINFO(POS_LIST[i]).nick,
				JOKER_POINT_RECORD[i]);
		send_msg(pos,buf);
	}
	kill_msg(pos);
}

void jokerpig_showimage (char * buf, int which)
{
	switch (which)
	{
		case JOKERPIG_PIG :
			strcpy (buf, " (\\(\\\n (.. )\n((oo) )");
			break;
		case JOKERPIG_SHEEP :
			strcpy (buf, "    S__S\n   < . .>\n @(  \\-/ \n      :");
			break;
		case JOKERPIG_DOUBLE :
			strcpy (buf, " /\\ /\\\n \\ X /\n / 2 \\\n \\/ \\/"); 
			break;
		case JOKERPIG_HEART :
			strcpy (buf, "    /--\\\n   (^T ^)\n   (   )/");

	}
}




