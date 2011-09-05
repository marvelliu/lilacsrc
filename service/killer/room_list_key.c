static int room_list_key(struct _select_def *conf, int key)
{
	struct room_struct r, *r2;
	int i, j;
	char name[40], ans[4];
	int spec;

	switch (key)
	{
	case 'a':
		r.op=-1;
		k_getdata(0, 0, "房间名:", name, 13, 1, NULL, 1);
		if (!name[0])
			return SHOW_REFRESH;
		if (name[0] == ' ' || name[strlen(name) - 1] == ' ')
		{
			move(0, 0);
			clrtoeol();
			prints(" 房间名开头结尾不能为空格");
			refresh();
			sleep(1);
			return SHOW_CONTINUE;
		}
		strcpy(r.name, name);
		r.style = 1;
		r.flag = ROOM_SPECBLIND;
		r.numplayer = 0;
		r.numspectator=0;
		r.maxplayer = MAX_PLAYER;
		r.op=-1;
		strcpy(r.title, "我杀我杀我杀杀杀");
		if (add_room(&r) == -1)
		{
			move(0, 0);
			clrtoeol();
			prints(" 有一样名字的房间啦!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(r.name), 0);
		return SHOW_DIRCHANGE;
	case 'J':
		k_getdata(0, 0, "房间名:", name, 12, 1, NULL, 1);
		if (!name[0])
			return SHOW_REFRESH;
		if ((i = find_room(name)) == -1)
		{
			move(0, 0);
			clrtoeol();
			prints(" 没有找到该房间!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		k_getdata(0, 0, "是否以旁观者身份进入? [y/N]", ans, 3, 1, NULL, 1);
		spec=(toupper(ans[0])=='Y');
		if ((i = find_room(name)) == -1) 
		{
			move(0, 0);
			clrtoeol();
			prints(" 没有找到该房间!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		if (spec && r2->flag & ROOM_DENYSPEC && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP)) 
		{
			move(0, 0);
			clrtoeol();
			prints(" 该房间拒绝旁观者");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		r2 = rooms + i;
		if ( ( !spec && r2->numplayer >= r2->maxplayer && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP) )
			||( !spec && r2->numplayer>=MAX_PLAYER )
			||( spec && r2->numspectator >= MAX_PEOPLE-MAX_PLAYER )
		)
		{
			move(0, 0);
			clrtoeol();
			prints(" 该房间人数已满");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(name), toupper(ans[0]) == 'Y');
		return SHOW_DIRCHANGE;
	case 'K':
		if (!K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
			return SHOW_CONTINUE;
		i = room_get(conf->pos - 1);
		if (i != -1)
		{
			r2 = rooms + i;
			r2->style = -1;
			for (j = 0; j < MAX_PEOPLE; j++)
				if (inrooms[i].peoples[j].style != -1)
					kill(inrooms[i].peoples[j].pid, SIGUSR1);
		}
		return SHOW_DIRCHANGE;
	}
	return SHOW_CONTINUE;
}

