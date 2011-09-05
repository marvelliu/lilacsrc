

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
	show_kp_showrolelist(RINFO.killers, "É±ÊÖ");
	show_kp_showseq_actionlist(RINFO.seq_action[RINFO.policenum], "´ÌÉ±");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist(RINFO.polices, "¾¯²ì");
		for (i=0; i<RINFO.policenum; i++)
				show_kp_showseq_actionlist(RINFO.seq_action[i], "Ì½²â");
	}*/
	show_kp_showrolelist(RINFO.killers, "É±ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "´ÌÉ±");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist (RINFO.polices, "¾¯²ì");
		show_kp_showseq_actionlist (RINFO.seq_detect, "µ÷²é");
	}
}
		
void show_kp_crow()
{
	show_kp_showspecial(S_KILLER, "É±ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "´ÌÉ±");
	show_kp_showspecial(S_MAGIC, "Ä§·¨Ê¦");
	show_kp_showseq_actionlist (RINFO.seq_action[S_MAGIC], "Ä§·¨");
	show_kp_showspecial(S_FOREST, "É­ÁÖÀÏÈË");
	show_kp_showseq_actionlist (RINFO.seq_action[S_FOREST], "½ûÑÔ");
	show_kp_showrolelist(RINFO.killers, "¶ñÃñ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BADMAN], "°µÆ±");
	show_kp_showspecial(S_BUTTERFLY, "»¨ºûµû");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BUTTERFLY], "Óµ±§");
	show_kp_showspecial(S_SHOOTER, "¾Ñ»÷ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_SHOOTER], "¾Ñ»÷");
	show_kp_showspecial(S_DOCTOR, "Ò½Éú");
	show_kp_showseq_actionlist (RINFO.seq_action[S_DOCTOR], "ÔúÕë");
	show_kp_showspecial(S_POLICE, "¾¯²ì");
	show_kp_showseq_actionlist (RINFO.seq_action[S_POLICE], "Ì½²â");
	show_kp_showrolelist(RINFO.polices, "ÉÆÃñ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_GOODMAN], "°µÆ±");
	show_kp_showrolelist(RINFO.crows, "ÎÚÑ»");
	show_kp_showseq_actionlist (RINFO.seq_action[S_CROW], "¹Û²ì");
}	
void show_kp_butterfly()
{
	show_kp_showrolelist(RINFO.killers, "É±ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "´ÌÉ±");
	show_kp_showspecial(S_FOREST, "É­ÁÖÀÏÈË");
	show_kp_showseq_actionlist (RINFO.seq_action[S_FOREST], "½ûÑÔ");
	show_kp_showspecial(S_BUTTERFLY, "»¨ºûµû");
	show_kp_showseq_actionlist (RINFO.seq_action[S_BUTTERFLY], "Óµ±§");
	show_kp_showspecial(S_SHOOTER, "¾Ñ»÷ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_SHOOTER], "¾Ñ»÷");
	show_kp_showspecial(S_DOCTOR, "Ò½Éú");
	show_kp_showseq_actionlist (RINFO.seq_action[S_DOCTOR], "ÔúÕë");
	if (RINFO.policenum>=1)
	{
		show_kp_showspecial(S_POLICE, "¾¯²ì");
		show_kp_showseq_actionlist (RINFO.seq_action[S_POLICE], "Ì½²â");
	}
}		
void show_kp_original()
{
	show_kp_showrolelist(RINFO.killers, "É±ÊÖ");
	show_kp_showseq_actionlist (RINFO.seq_action[S_KILLER], "´ÌÉ±");
	if (RINFO.policenum>0)
	{
		show_kp_showrolelist (RINFO.polices, "¾¯²ì");
		show_kp_showseq_actionlist (RINFO.seq_detect, "µ÷²é");
	}
}
void show_kp_showspecial(int seq, char * role)
{
	char buf[200];
	if (RINFO.special[seq] == -1)
		return;
	sprintf(buf,"\x1b[34;1m%s£º\x1b[m %d",role, RINFO.special[seq]+1);
	send_msg(-1,buf);

}

void show_kp_showrolelist(char * list, char * role)
{
	char buf[200],buf2[20];
	int i;
	sprintf(buf,"\x1b[34;1m%s£º \x1b[m",role);
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
	sprintf(buf,"\x1b[34;1m%sË³Ðò£º \x1b[m",role);
	for (i=0; i<MAX_DAY;i++)
	{
		if (list[i]==-1)
			break;
		if (i!=0)
			strcat(buf,", ");
		if (list[i]!=-2)
			sprintf(buf2,"%d",list[i]+1);
		else
			sprintf(buf2,"%s","²»ÐÐ¶¯");
		strcat(buf,buf2);
	}
	send_msg(-1,buf);
}
