void vote_result()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			vote_result_original();
			break;
		case GAME_CROW :
			vote_result_crow();
			break;
		case GAME_BUTTERFLY :
			vote_result_butterfly();
			break;
		case GAME_TSINGHUA :
			vote_result_tsinghua();
			break;
		case GAME_JOKERPIG :
			vote_result_jokerpig();
	}
}
void vote_result_original()
{
	int i;
	int k;
	int mr[2];
	start_change_inroom();
	//ͶƱ�׶�
	if (!(RINFO.status==INROOM_VOTE))
	{
		end_change_inroom();
		return;
	}
	//������ûͶƱ
	for (i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 && PINFO(i).vote==-1)
		{
			end_change_inroom();
			return;
		}
				  
	int m1,m2,tt;
	tt=0;
	//��ʾͶƱ���,����Ʊ��
	tt=show_vote_result_list();
	//ͳ�����Ʊ�ʹθ�Ʊ
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1*2>tt)
	{
		// ֱ��Ͷ��
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
					&& (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //������˵ķ���?����m1�������أ�
		goto_dark();
	}
	else
	{
		// δ����1/2
		tt=0;
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
					&& (PINFO(i).flag & PEOPLE_VOTED))
			{
				if (PINFO(i).vnum!=m1 && PINFO(i).vnum!=m2)
					PINFO(i).flag &=~PEOPLE_VOTED;
				else
					tt++;
			}
		if (RINFO.round>3)
		{
			//����3��,�������
			send_msg(-1, "\33[31;1mͶƱ������δ�н���������������\33[m");
			k=rand()%tt;
			for(i=0;i<MAX_PLAYER;i++)
				if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).style !=-1 
						&& (PINFO(i).flag & PEOPLE_VOTED))
				{
					if (k==0)
						break;
					k--;			
				}
			RINFO.victim=i;
			goto_dark();
		}
		else
		{
			send_msg (-1, "\33[31;1m���Ʊ��δ��������,�������Ʊ�����˱绤\33[m");
			while (m1>0)
			{
				tt=0;
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) 
							&& (PINFO(i).flag & PEOPLE_VOTED))
					{
						if (PINFO(i).vnum==m1)
							tt++;
					}
				if (tt>0)
					break;
				m1--;
			}
			if (m1>0 && tt>0)
			{
				k=rand()%tt;
				for(i=0;i<MAX_PLAYER;i++)
					if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE)
							&& (PINFO(i).flag & PEOPLE_VOTED)&& (PINFO(i).vnum==m1))
					{
						if (k==0)
							break;
						k--;			
					}
				RINFO.victim=i;
				goto_defence();
			}
			else
			{
				RINFO.victim=-1;
				goto_defence();
			}
		}
	}
	kill_msg(-1);
	end_change_inroom();
}

void vote_result_crow ()
{
	int i;
	int v,m1,m2;
	int mr[2];
	start_change_inroom();
	//ͶƱ�׶�:�����һ��ָ֤�ַ��Ե��˵�ctrl-t����vote_result_crow
	int tt;
	tt=show_vote_result_list();
	//���밵Ʊ
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (RINFO.secretvotelist[i]==-1)
			break;
		if (RINFO.secretvotelist[i]>=0) //-2 ��ʾ����Ϊ������Ч
		{
			v=RINFO.secretvotelist[i];
			if (PINFO(v).flag & PEOPLE_ALIVE)
				PINFO(v).vnum++;
		}
	}
	//�����ѻ���ϵ�Ʊ
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).flag & PEOPLE_ALIVE && PINFO(i).flag & PEOPLE_CROW)
			PINFO(i).vnum=0;
	}
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1==m2) 
	{
		send_msg(-1, "ƽƱ�������ˡ�");
		kill_msg(-1);
		goto_night();
	}
	else
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //������˵ķ���?����m1�������أ�
		goto_dark();
	}
	end_change_inroom();
}
void vote_result_butterfly()
{
	int i;
	int m1,m2;
	int mr[2];
	start_change_inroom();
	//ͶƱ�׶�:�����һ��ָ֤�ַ��Ե��˵�ctrl-t����vote_result_crow
	int tt;
	tt=show_vote_result_list();
	vote_result_highest2(mr);
	m1=mr[0];
	m2=mr[1];
	if (m1==m2) 
	{
		send_msg(-1, "ƽƱ�������ˡ�");
		kill_msg(-1);
		goto_night();
	}
	else
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
#ifdef KILLER_DEBUG
		sprintf (buf, "ͶƱ���, ���Ʊ��%d��%dƱ",i+1, m1);
		send_msg(-1,buf);
		kill_msg(-1);
#endif
		RINFO.victim=i; //������˵ķ���?����m1�������أ�
		goto_dark();
	}
	end_change_inroom();
}
void vote_result_tsinghua()
{
	int i;
	int m1=0,m2=0,mo=0;
//	int mr[2];
	char buf[200];
	start_change_inroom();
	int tt;
//	vote_result_highest2(mr);
	
	for(i=0;i<MAX_PLAYER;i++)
	{
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
		{
			if (PINFO(i).vnum>m1)
			{
				m2=m1;
				m1=PINFO(i).vnum;
			}
			else if (PINFO(i).vnum>m2)
				m2=PINFO(i).vnum;
		}
		if (PINFO(i).style!=-1 && (PINFO(i).flag & PEOPLE_ALIVE) && PINFO(i).vote==-1)
		{
			mo++;
		}
	}
//	m1=mr[0];
//	m2=mr[1];
	if (m1==m2 && mo==0) 
	{
		tt=show_vote_result_list();
//			send_msg(-1, "\33[31;1mͶƱ������δ�н���������������\33[m");
		sprintf(buf, "\33[1;31m���Ʊ%d, �θ�Ʊ%d, δͶƱ%d\33[m", m1, m2, mo);
		send_msg(-1, buf);
		send_msg(-1, "\33[1;31mƽƱ������ͶƱ��\33[m");

		for(i=0;i<MAX_PLAYER;i++)
		{
			PINFO(i).vote=-1;
			PINFO(i).flag &=~PEOPLE_VOTED;
			RINFO.voted[i]=-1;
			PINFO(i).vnum=0;
		}
		kill_msg(-1);
		goto_check();
		end_change_inroom();
	}
	else if ( m1 > m2+mo )
	{
		sprintf(buf, "\33[1;31m���Ʊ%d, �θ�Ʊ%d, δͶƱ%d\33[m", m1, m2, mo);
		send_msg(-1, buf);
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
				if (PINFO(i).vnum==m1)
					break;
		RINFO.victim=i; //������˵ķ���?����m1�������أ�
		tt=show_vote_result_list();
		//PINFO(i).flag &=~PEOPLE_ALIVE;
//		sprintf (buf, "%d %s��Ͷ���ˡ� ",i+1, PINFO(i).nick);
//		send_msg(-1, buf);
		kill_msg(-1);
		goto_dark();
		end_change_inroom();
	}
}
		

		

int show_vote_result_list()
{
	int i;
	char buf[200];
	int tt;
	tt=0;
	send_msg (-1, "\33[33;1mͶƱ�������:\33[m");
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE))// && PINFO(i).vote!=-1)
		{
			if ( PINFO(i).vote != -1 )
			{
				sprintf(buf,"\33[32;1m%d %s Ͷ�� %d %s һƱ\33[m",i+1,PINFO(i).nick,
					PINFO(i).vote+1,PINFO(PINFO(i).vote).nick);
			}
			else
			{
				sprintf(buf,"\33[32;1m%d %s ����ͶƱ\33[m",i+1,PINFO(i).nick);
			}
			send_msg(-1,buf);
			PINFO(i).vnum=0;
			tt++;
		}
		
	if ( RINFO.gametype != GAME_TSINGHUA )
	{
		for(i=0;i<MAX_PLAYER;i++)
			if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE))
				PINFO(PINFO(i).vote).vnum++;
	}
	return tt;
}
void vote_result_highest2 ( int * result)
{
	int i, m1, m2;
	m1=-1;
	m2=-1;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & PEOPLE_VOTED))
		{
			if (PINFO(i).vnum>m1)
			{
				m2=m1;
				m1=PINFO(i).vnum;
			}
			else if (PINFO(i).vnum>m2)
				m2=PINFO(i).vnum;
		}
	result[0]=m1;
	result[1]=m2;
}

