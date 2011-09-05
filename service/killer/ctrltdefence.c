void ctrl_t_defence()
{
	switch (RINFO.gametype)
	{
		case GAME_ORIGINAL :
			ctrl_t_defence_original();
			break;
		case GAME_JOKER1 :
			ctrl_t_defence_joker1();
			break;
		case GAME_JOKERPIG :
			ctrl_t_defence_jokerpig();
			break;
	}
}
void ctrl_t_defence_original()
{
	if (RINFO.turn==mypos && (PINFO(mypos).flag & PEOPLE_VOTED))
	{//只有被指的人才能说话，只有轮到的人才能说话。
		ctrl_t_defence_common();
		if (!check_win())
		{
			if (! look_for_next_speaker(RINFO.turn, RINFO.victim, 2))
				goto_defence_speak();
			else
				goto_vote();
		}
	}
}
void ctrl_t_defence_common ()
{
	char buf[200];
	if (!announce_surrender())
	{
		sprintf (buf, "\x1b[32;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
		send_msg (-1, buf);
		kill_msg (-1);
	}
}

