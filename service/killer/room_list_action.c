static int room_list_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[游戏室选单]",
			   "  退出[\x1b[1;32m←\x1b[0;37m,\x1b[1;32me\x1b[0;37m] 进入[\x1b[1;32mEnter\x1b[0;37m] 选择[\x1b[1;32m↑\x1b[0;37m,\x1b[1;32m↓\x1b[0;37m] 添加[\x1b[1;32ma\x1b[0;37m] 加入[\x1b[1;32mJ\x1b[0;37m] \x1b[m	  作者: \x1b[31;1mbad@smth.org\x1b[m");
	move(2, 0);
	prints("\x1b[37;44;1m    %4s %-14s %-12s %4s %4s %6s %-20s", "编号", "游戏室名称", "管理员", "人数", "最多", "锁隐旁", "话题");
	clrtoeol();
//	k_resetcolor();
	update_endline();
	return SHOW_CONTINUE;
}

static int room_list_show(struct _select_def *conf, int i)
{
	struct room_struct *r;
	int j = room_get(i - 1);

	if (j != -1)
	{
		r = rooms + j;
		prints("  %3d  %-14s %-12s %3d  %3d  %2s%2s%2s %-36s", 
			i, r->name, inrooms[j].peoples[r->op].id,
			r->numplayer, r->maxplayer, (r->flag & ROOM_LOCKED) ? "√" : "",
			(r->flag & ROOM_SECRET) ? "√" : "",
			(!(r->flag & ROOM_DENYSPEC)) ? "√" : "", r->title);
	}
	return SHOW_CONTINUE;
}
static int room_list_k_getdata(struct _select_def *conf, int pos, int len)
{
	clear_room();
	conf->item_count = room_count();
	return SHOW_CONTINUE;
}

static int room_list_prekey(struct _select_def *conf, int *key)
{
	switch (*key)
	{
	case 'e':
	case 'q':
		*key = KEY_LEFT;
		break;
	case 'p':
	case 'k':
		*key = KEY_UP;
		break;
	case ' ':
	case 'N':
		*key = KEY_PGDN;
		break;
	case 'n':
	case 'j':
		*key = KEY_DOWN;
		break;
	}
	return SHOW_CONTINUE;
}
#include "killer/room_list_key.c"
/*
void show_top_board()
{
	FILE *fp;
	char buf[80];
	int i, j, x, y;

	clear();
	for (i = 1; i <= 6; i++) 
	{
		sprintf(buf, "service/killer.%d", i);
		fp = fopen(buf, "r");
		if (!fp)
			return;
		for (j = 0; j < 7; j++)
		{
			if (feof(fp))
				break;
			y = (i - 1) % 3 * 8 + j;
			x = (i - 1) / 3 * 40;
			if (fgets(buf, 80, fp) == NULL)
				break;
			move(y, x);
			k_resetcolor();
			if (j == 2)
				k_setfcolor(RED, 1);
			prints("%s", buf);
		}
		fclose(fp);
	}
	pressanykey();
}
*/
int choose_room()
{
	struct _select_def grouplist_conf;
	int i;
	POINT *pts;

	clear_room();
	bzero(&grouplist_conf, sizeof(struct _select_def));
	grouplist_conf.item_count = room_count();
	if (grouplist_conf.item_count == 0)
	{
		grouplist_conf.item_count = 1;
	}
	pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
	for (i = 0; i < BBS_PAGESIZE; i++) 
	{
		pts[i].x = 2;
		pts[i].y = i + 3;
	}
	grouplist_conf.item_per_page = BBS_PAGESIZE;
	grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
	grouplist_conf.prompt = "◆";
	grouplist_conf.item_pos = pts;
	grouplist_conf.arg = NULL;
	grouplist_conf.title_pos.x = 0;
	grouplist_conf.title_pos.y = 0;
	grouplist_conf.pos = 1;	 /* initialize cursor on the first mailgroup */
	grouplist_conf.page_pos = 1;		/* initialize page to the first one */

	grouplist_conf.on_select = room_list_select;
	grouplist_conf.show_data = room_list_show;
	grouplist_conf.pre_key_command = room_list_prekey;
	grouplist_conf.key_command = room_list_key;
	grouplist_conf.show_title = room_list_refresh;
	grouplist_conf.get_data = room_list_k_getdata;
//	show_top_board();
	list_select_loop(&grouplist_conf);
	free(pts);
	return 0;
}

