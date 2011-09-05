static int room_list_select(struct _select_def *conf)
{
	struct room_struct *r, *r2;
	int i = room_get(conf->pos - 1), j;
	char ans[4];
	int spec;

	if (i == -1)
		return SHOW_CONTINUE;
	r = rooms + i;
	j = find_room(r->name);
	if (j == -1)
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间已被锁定!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	k_getdata(0, 0, "是否以旁观者身份进入? [y/N]", ans, 3, 1, NULL, 1);
	spec=(toupper(ans[0]) == 'Y');
	j = find_room(r->name);
	if (j == -1)
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间已被锁定!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	r2 = rooms + j;
	if (spec && r2->flag & ROOM_DENYSPEC && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间拒绝旁观者");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	if ( ( !spec && r2->numplayer >= r2->maxplayer && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP) )
	    ||( !spec && r2->numplayer>=MAX_PLAYER )
		||( spec && r2->numspectator >= MAX_PEOPLE-MAX_PLAYER)
	)
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间人数已满");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	join_room(find_room(r2->name), spec);
	return SHOW_DIRCHANGE;
}


