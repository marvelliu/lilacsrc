int menu_6_callback()
{
	int me,i,k;
	char buf[200];
	me=mypos;
	if (RINFO.status != INROOM_STOP)
	{
		send_msg (me, "��Ϸ�У����ܸ�����");
		kill_msg(me);
		return 0;
	}
	menu_ask_string( "�����뷿���������:",30, buf);
	if (kicked)
		return 0;
	if (buf[0])
	{
		i = atoi(buf);
		if (i > 0 && i <= MAX_PLAYER)
		{
			rooms[myroom].maxplayer = i;
			sprintf(buf, "�������÷����������Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	k=menu_ask_yn("����Ϊ���ط���? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0) 
	{
		if (k)
			rooms[myroom].flag |= ROOM_SECRET;
		else
			rooms[myroom].flag &= ~ROOM_SECRET;
		sprintf(buf, "�������÷���Ϊ%s", k ? "����" : "������");
			send_msg(-1, buf);
	}
	k=menu_ask_yn("����Ϊ��������? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_LOCKED;
		else
			rooms[myroom].flag &= ~ROOM_LOCKED;
		sprintf(buf, "�������÷���Ϊ%s", k ? "����" : "������");
		send_msg(-1, buf);
	}
	k=menu_ask_yn("����Ϊ�ܾ��Թ��ߵķ���? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_DENYSPEC;
		else
			rooms[myroom].flag &= ~ROOM_DENYSPEC;
		sprintf(buf, "�������÷���Ϊ%s", k ? "�ܾ��Թ�" : "���ܾ��Թ�");
		send_msg(-1, buf);
	}
	/*
	move(t_lines - 1, 0);
	k=menu_ask_yn("����Ϊ�Թ����޷�����ɱ�־���ķ���? [Y/N]");
	if (kicked)
		return 0;
	if (k>=0)
	{
		if (k)
			rooms[myroom].flag |= ROOM_SPECBLIND;
		else
			rooms[myroom].flag &= ~ROOM_SPECBLIND;
		sprintf(buf, "�������÷���Ϊ%s", k ? "�Թ��޷�����ɱ�־���" : "�Թۿ��Կ���ɱ�־���");
		send_msg(-1, buf);
	}
	*/
	send_msg(mypos, "��Ϸ���ͣ�1--�Ͽ����� 2--�������ѻ��3--��������");
	send_msg(mypos, "          4--��������� 5--�����ң�    6--����");
	kill_msg(mypos);
	menu_ask_string("������Ϸ������: ",30, buf);
	if (kicked)
		return 0;
	if (buf[0])
	{
		i = atoi(buf);
		if (i >= 1 && i <= 6)
		{
			RINFO.gametype = i;
			if (i==GAME_ORIGINAL)
				sprintf(buf, "�������÷�����Ϸ����Ϊ�Ͽ�����");
			else if  (i==GAME_CROW)
				sprintf(buf, "�������÷�����Ϸ����Ϊ�������ѻ");
			else if  (i==GAME_BUTTERFLY)
				sprintf(buf, "�������÷�����Ϸ����Ϊ������");			
			else if  (i==GAME_TSINGHUA)
				sprintf(buf, "�������÷�����Ϸ����Ϊ���������");
			else if  (i==GAME_JOKER1)
				sprintf(buf, "�������÷�����Ϸ����Ϊ������");
			else if  (i==GAME_JOKERPIG)
				sprintf(buf, "�������÷�����Ϸ����Ϊ����");	
			send_msg(-1, buf);
		}
	}
	
	if (RINFO.gametype==GAME_ORIGINAL || RINFO.gametype==GAME_BUTTERFLY ||	RINFO.gametype==GAME_TSINGHUA)
	{
		i=menu_ask_num("����ɱ�ֵ���Ŀ: ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			RINFO.killernum = i;
			sprintf(buf, "�������÷���ɱ����Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_CROW)
	{
		i=menu_ask_num("���ö������Ŀ(�˲���ʱѡ0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			RINFO.killernum = i;
			sprintf(buf, "�������÷��������Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_JOKER1)
	{
		i=menu_ask_num("���÷��Կ���(1=ֻ���Լ����Ƶ�ʱ��˵��): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_KILLER)
		{
			if (i==0)
				i=1;
			RINFO.killernum = i;
			sprintf(buf, "�������÷��Կ���Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	if (RINFO.gametype==GAME_ORIGINAL ||RINFO.gametype==GAME_TSINGHUA)
	{
		i=menu_ask_num("���þ������Ŀ: ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
			RINFO.policenum = i;
			sprintf(buf, "�������÷��侯����Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_BUTTERFLY)
	{
		i=menu_ask_num("���þ������Ŀ(1����0,ֻ�е���������ʱѡ0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
			if (i>0)
				i=1;
			RINFO.policenum = i;
			sprintf(buf, "�������÷��侯����Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_CROW)
	{
		i=menu_ask_num("�����������Ŀ(�˲���ʱѡ0): ");
		if (kicked)
			return 0;
		if (i >= 0 && i <= MAX_POLICE)
		{
//			if (i==0)
//				i=1;
			RINFO.policenum = i;
			sprintf(buf, "�������÷���������Ϊ%d", i);
			send_msg(-1, buf);
		}
	}
	else if (RINFO.gametype==GAME_JOKER1)
	{
	}
	else if (RINFO.gametype==GAME_JOKERPIG)
	{
		k=menu_ask_yn("����÷ּ�¼(����ѡ��Y)? [Y/N]");
		if (kicked)
			return 0;
		if (k>=0)
		{
			if (k)
				for (i=0;i<MAX_PLAYER;i++)
					RINFO.action_target[i]=0;
		}
		sprintf(buf, "�������õ÷ּ�¼Ϊ%s", k ? "���" : "����");
		send_msg(-1, buf);
	}
	i=menu_ask_num ("����ÿ�η��Ի��߳��Ƶ����ʱ��n��10��): ");
	if (kicked) 
		return 0;
	if (i>=0 && i<=100)
	{
		if (i==0)
			i=1;
		RINFO.time_actionlimit= i*10;
		sprintf(buf, "�������÷��䷢�Ի��߳���ʱ�����Ϊ%d��", i*10);
		send_msg(-1, buf);
	}

	i=menu_ask_num ("���÷��������ʱ��n��10��): ");
	if (kicked) 
		return 0;
	if (i>=0 && i<=30)
	{
		if (i==0)
			i=1;
		RINFO.time_actionlimit= i*10;
		sprintf(buf, "�������÷��䷢��ʱ�����Ϊ%d��", i*10);
		send_msg(-1, buf);
	}

	
	kill_msg(-1);
	return 0;
}
