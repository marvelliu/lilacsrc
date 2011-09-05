

void show_killerpolice ()
{
	int gametype;
	gametype=RINFO.gametype;
	if (gametype == 1)
		show_kp_original();
	else if (gametype == 2)
		show_kp_crow();
	else if (gametype == 3)
		show_kp_butterfly();
	else if (gametype == 4)
		show_kp_tsinghua();
	kill_msg(-1);
}
void show_kp_tsinghua()
{
/*	int i;
	show_kp_showrolelist(RINFO.killers, "ɱ��");
	show_kp_showseq_actionlist(RINFO.seq_action[RINFO.policenum], "��ɱ");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist(RINFO.polices, "����");
		for (i=0; i<RINFO.policenum; i++)
				show_kp_showseq_actionlist(RINFO.seq_action[i], "̽��");
	}*/
	show_kp_showrolelist(RINFO.killers, "ɱ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "��ɱ");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist (RINFO.polices, "����");
		show_kp_showseq_actionlist (RINFO.seq_detect, "����");
	}
}
		
void show_kp_crow()
{
	show_kp_showspecial(S_KILLER, "ɱ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "��ɱ");
	show_kp_showspecial(S_MAGIC, "ħ��ʦ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_MAGIC], "ħ��");
	show_kp_showspecial(S_FOREST, "ɭ������");
	show_kp_showseq_actionlist (RINFO.seq_action[S_FOREST], "����");
	show_kp_showrolelist(RINFO.killers, "����");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BADMAN], "��Ʊ");
	show_kp_showspecial(S_BUTTERFLY, "������");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BUTTERFLY], "ӵ��");
	show_kp_showspecial(S_SHOOTER, "�ѻ���");
	show_kp_showseq_actionlist (RINFO.seq_action[S_SHOOTER], "�ѻ�");
	show_kp_showspecial(S_DOCTOR, "ҽ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_DOCTOR], "����");
	show_kp_showspecial(S_POLICE, "����");
	show_kp_showseq_actionlist (RINFO.seq_action[S_POLICE], "̽��");
	show_kp_showrolelist(RINFO.polices, "����");
	show_kp_showseq_actionlist (RINFO.seq_action[S_GOODMAN], "��Ʊ");
	show_kp_showrolelist(RINFO.crows, "��ѻ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_CROW], "�۲�");
}	
void show_kp_butterfly()
{
	show_kp_showrolelist(RINFO.killers, "ɱ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "��ɱ");
	show_kp_showspecial(S_FOREST, "ɭ������");
	show_kp_showseq_actionlist (RINFO.seq_action[S_FOREST], "����");
	show_kp_showspecial(S_BUTTERFLY, "������");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BUTTERFLY], "ӵ��");
	show_kp_showspecial(S_SHOOTER, "�ѻ���");
	show_kp_showseq_actionlist (RINFO.seq_action[S_SHOOTER], "�ѻ�");
	show_kp_showspecial(S_DOCTOR, "ҽ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_DOCTOR], "����");
	if (RINFO.policenum>=1)
	{
		show_kp_showspecial(S_POLICE, "����");
		show_kp_showseq_actionlist (RINFO.seq_action[S_POLICE], "̽��");
	}
}		
void show_kp_original()
{
	show_kp_showrolelist(RINFO.killers, "ɱ��");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "��ɱ");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist (RINFO.polices, "����");
		show_kp_showseq_actionlist (RINFO.seq_detect, "����");
	}
}
void show_kp_showspecial(int seq, char * role)
{
	char buf[200];
	if (RINFO.special[seq] == -1)
		return;
	sprintf(buf,"\x1b[34;1m%s��\x1b[m %d",role, RINFO.special[seq]+1);
	send_msg(-1,buf);

}

void show_kp_showrolelist(char * list, char * role)
{
	char buf[200],buf2[20];
	int i;
	sprintf(buf,"\x1b[34;1m%s�� \x1b[m",role);
	for (i=0; i<MAX_PLAYER;i++)
	{
		if (list[i]==-1)
			break;
		if (i!=0)
			strcat(buf,", ");
		sprintf(buf2,"%d",list[i]+1);
		strcat(buf,buf2);
	}
	send_msg(-1,buf);
}
void show_kp_showseq_actionlist(char * list, char * role)
{
	char buf[200],buf2[20];
	int i;
	sprintf(buf,"\x1b[34;1m%s˳�� \x1b[m",role);
	for (i=0; i<MAX_DAY;i++)
	{
		if (list[i]==-1)
			break;
		if (i!=0)
			strcat(buf,", ");
		if (list[i]!=-2)
			sprintf(buf2,"%d",list[i]+1);
		else
			sprintf(buf2,"%s","���ж�");
		strcat(buf,buf2);
	}
	send_msg(-1,buf);
}
