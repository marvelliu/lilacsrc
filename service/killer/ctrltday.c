
void ctrl_t_day()
{
	int gametype;
	gametype=RINFO.gametype;
	if (RINFO.turn==mypos)
	{
		if (gametype==1)
			ctrl_t_day_original();
		else if (gametype==2)
			ctrl_t_day_crow();
		else if (gametype==3)
			ctrl_t_day_butterfly();
		else if (gametype==4)
			ctrl_t_day_tsinghua();
	}
}
void ctrl_t_day_crow ()
{
		ctrl_t_day_common();
		RINFO.deadpointer++;
		goto_day_common(RINFO.deadlist[RINFO.deadpointer]);
}
void ctrl_t_day_butterfly ()
{
		ctrl_t_day_common();
		RINFO.deadpointer++;
		goto_day_common(RINFO.deadlist[RINFO.deadpointer]);
}
	
void ctrl_t_day_original ()
{
		ctrl_t_day_common();
		goto_check(); //ԭ������Ϸ��Ȼ�ǵ���������
}
void ctrl_t_day_tsinghua ()
{
		ctrl_t_day_common();
		goto_check(); //ԭ������Ϸ��Ȼ�ǵ���������
}
void ctrl_t_day_common()
{
	char buf[200];
	sprintf (buf, "\x1b[35;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
	send_msg (-1, buf);
	sprintf (buf, "��ô����\33[35;1m %d %s \33[m�������Թ��ߵ���ݣ�������Ϸ...", mypos + 1, PINFO(mypos).nick);
	send_msg (-1, buf);
	if (PINFO(mypos).flag & PEOPLE_SEENBAD)
					PINFO(mypos).flag |=PEOPLE_HIDEKILLER;
	PINFO(mypos).flag&=~PEOPLE_ALIVE;
	kill_msg(-1);
}

