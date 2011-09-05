// LILAC PERSONAL CORPUS   luohao@gmail.com lilac bbs 2004.10 //
#include "service.h"
#include "bbs.h"
#include "select.h"
#include "lilacsearch.h"

char save_scr[LINEHEIGHT][LINELEN * 3];
int save_y, save_x, wh = 0, res_total = 0;
int bid = 0, pos = 0;
int noforward = 0, nobackward = 1;
MY_QUERY query;
MY_RESULT res[100];
POS_CACHE pcache[MAX_CACHE];

int search_menu();

int lilacsearch_main()
{
	search_menu();
	return 0;
}

static int choose_file_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[紫丁香搜索]",
               "  退出[\x1b[1;32m←\x1b[0;37m,\x1b[1;32me\x1b[0;37m] 察看[\x1b[1;32mEnter\x1b[0;37m] 选择[\x1b[1;32m↑\x1b[0;37m,\x1b[1;32m↓\x1b[0;37m] 翻页查找[\x1b[1;32m[\x1b[0;37m,\x1b[1;32m]\x1b[0;37m]          金秋纪念版                         ");
    move(2, 0);
    prints("\033[0;1;37;44m  %4s %-12s %-14s %-10s   %s    %d-%d ", 
				"编号", "发表人", "版面", "日期", "标题", 
				(wh - 1) * MAX_KEEP + 1, (wh - 1) * MAX_KEEP + res_total);
    clrtoeol();
    resetcolor();
    update_endline();
    return SHOW_CONTINUE;
}

static int choose_file_show(struct _select_def *conf, int ii)
{
    prints("%4d %-12s %-14s %-10s %s", 
				ii + (wh - 1) * MAX_KEEP, res[ii - 1].id, res[ii - 1].board, 
				res[ii-1].date, res[ii-1].title);
    clrtoeol();

    return SHOW_CONTINUE;
}

static int choose_file_select(struct _select_def *conf)
{
    char ss[256];
    int ch;

  again:
    snprintf(ss, 256, "boards/%s/%s", res[conf->pos-1].board,
							res[conf->pos - 1].filename);
    ch = ansimore_withzmodem(ss, 0, res[conf->pos - 1].title);
    move(t_lines - 1, 0);
    prints("\033[0;1;31;44m[第%d篇]  \033[33m 版面:%s", 
					conf->pos + (wh - 1) * MAX_KEEP, res[conf->pos - 1].board);
    clrtoeol();
    if (ch == 0)
        ch = igetkey();
    switch (ch) {
    case KEY_UP:
    case 'l':
    case 'k':
        conf->pos--;
        if (conf->pos <= 0)
            conf->pos = res_total;
        goto again;
    case KEY_DOWN:
    case ' ':
    case 'n':
    case 'j':
        conf->pos++;
        if (conf->pos > res_total)
            conf->pos = 1;
        goto again;
    case Ctrl('Y'):
        zsend_file(ss, res[conf->pos - 1].title);
        break;
    }
    return SHOW_DIRCHANGE;
}

static int choose_file_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = res_total;
    return SHOW_CONTINUE;
}

static int choose_file_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
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

static int choose_file_key(struct _select_def *conf, int key)
{
    struct fileheader fh;
    char buf[240];
    int i, old_result;
    POINT *pts;

    switch (key) {
    case ']':
		old_result = res_total;
        move(t_lines - 1, 0);
        prints("查找数据....");
        clrtoeol();
        refresh();
        do_search(1);
		if (!res_total)
			res_total = old_result;
		else
			wh++;
        return SHOW_DIRCHANGE;
        break;
    case '[':
        if (wh > 1) {
			old_result = res_total;
            move(t_lines - 1, 0);
            prints("查找数据....");
            clrtoeol();
            refresh();
        	do_search(0);
			if (!res_total)
				res_total = old_result;
			else
				wh--;
            return SHOW_DIRCHANGE;
        }
        break;
    case 'f':
        t_friends();
        return SHOW_REFRESH;
    case 'M':
        i_read_mail();
        return SHOW_REFRESH;
    case 'F':
        bzero(&fh, sizeof(struct fileheader));
        snprintf(buf, 240, "boards/%s", res[conf->pos-1].board);
        strncpy(fh.title, res[conf->pos - 1].title, ARTICLE_TITLE_LEN - 1);
        fh.title[ARTICLE_TITLE_LEN - 1] = '\0';
        strncpy(fh.filename, res[conf->pos-1].filename, FILENAME_LEN);
//        if (!doforward(buf, &fh, 0))
        if (!doforward(buf, &fh))
            prints("文章转寄成功");
        else
            prints("文章转寄失败");
        pressanykey();
        return SHOW_REFRESH;
    case '+':
        free(conf->item_pos);
        pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
        for (i = 0; i < BBS_PAGESIZE; i++) {
            pts[i].x = 2;
            pts[i].y = i * 4 + 3;
        }
        conf->item_per_page = BBS_PAGESIZE / 4;
        conf->item_pos = pts;
        return SHOW_DIRCHANGE;
    case '-':
        free(conf->item_pos);
        pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
        for (i = 0; i < BBS_PAGESIZE; i++) {
            pts[i].x = 2;
            pts[i].y = i + 3;
        }
        conf->item_per_page = BBS_PAGESIZE;
        conf->item_pos = pts;
        return SHOW_DIRCHANGE;
    }
    return SHOW_CONTINUE;
}

int choose_file()
{
    struct _select_def grouplist_conf;
    int i;
    POINT *pts;

    bzero(&grouplist_conf, sizeof(struct _select_def));
    grouplist_conf.item_count = res_total;
    //TODO: 窗口大小动态改变的情况？这里有bug
    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    grouplist_conf.item_pos = pts;
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "◆";
    grouplist_conf.arg = NULL;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.on_select = choose_file_select;
    grouplist_conf.show_data = choose_file_show;
    grouplist_conf.pre_key_command = choose_file_prekey;
    grouplist_conf.key_command = choose_file_key;
    grouplist_conf.show_title = choose_file_refresh;
    grouplist_conf.get_data = choose_file_getdata;
    list_select_loop(&grouplist_conf);
    free(pts);
	return 0;
}


int do_search(int forward)
{
	char *board;
	char dirpath[256];
	int total, i;
    int fd;
    struct stat buf;
    struct flock ldata;
	char *ptr;
	struct fileheader *ptr1;
	time_t now, tval, posttime;

	if (forward)
	{
		if (wh > 0)
		{
			bid = pcache[wh - 1].ebid;
			pos = pcache[wh - 1].epos + 1;
		}
		else
		{
			bid = 0;
			pos = 0;
		}
		pcache[wh].sbid = bid;
		pcache[wh].spos = pos;
	}
	else
	{
		if (wh > MAX_CACHE - 1)
		{
			bid = pcache[wh + 1].sbid;
			pos = pcache[wh + 1].spos - 1;
		}
		if (wh > 0)
		{
			pcache[wh].ebid = bid;
			pcache[wh].epos = pos;
		}
	}
	
	res_total = 0;
	if (forward && bid < 0)
		bid = 0;
	else if (!forward && bid >= brdshm->numboards )
		bid = brdshm->numboards - 1;
		
	for (bid >= 0; (bid < brdshm->numboards) && (bid >= 0); ) 
	{
    	board = bcache[bid].filename;
    	if (!check_read_perm(getCurrentUser(), &bcache[bid]))
		{
			if (forward)
    			bid++;
			else 
				bid--;

			continue;
		}
		setbdir(DIR_MODE_NORMAL, dirpath, board);

		if ((fd = open(dirpath, O_RDONLY, 0)) == -1)
		{
			if (forward)
				bid++;
			else
				bid--;

			continue;
		}
		ldata.l_type = F_RDLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
	    if (fcntl(fd, F_SETLKW, &ldata)== -1) 
		{
        	close(fd);
			return -1;
	    }
		if (fstat(fd, &buf) == -1) 
		{
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
			close(fd);
			return -1;
    	}

		total = buf.st_size / sizeof(struct fileheader);

		if (forward && (pos > total) )
		{
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);        /* 退出互斥区域*/
			close(fd);
			pos = 0;
			bid++;
			continue;
		}
		else if (forward && pos < 0)
			pos = 0;
		else if (!forward )
		{
			if (pos >= total || pos == -1)
				pos = total - 1;
		}
		if ((i = safe_mmapfile_handle(fd, PROT_READ, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) 
		{
			if (i == 2)
				end_mmapfile((void *) ptr, buf.st_size, -1);
			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
			close(fd);
    	}
		
		now = time(0);
		tval = query.day * 86400;
		ptr1 = (struct fileheader *)ptr;
	
		for ( ;pos >= 0 && pos < total ;)
		{
			if (abs(now - get_posttime(&ptr1[pos])) > tval)
				goto GOON;
			if (query.keyword[0] && !strcasestr(ptr1[pos].title, query.keyword))
				goto GOON;
			if (query.id[0] && strcasecmp(ptr1[pos].owner, query.id))
				goto GOON;
			
			strcpy(res[res_total].filename, ptr1[pos].filename);
			strcpy(res[res_total].board, board);
			strcpy(res[res_total].id, ptr1[pos].owner);
			strcpy(res[res_total].title, ptr1[pos].title);
			
			posttime = get_posttime(&ptr1[pos]);
			strftime(res[res_total].date, 12, "%F", localtime(&posttime));
			
			res_total++;
			if (res_total > MAX_KEEP - 1)
				break;

GOON:
			if (forward)
				pos++;
			else
				pos--;
		}  // end of match article
		end_mmapfile((void *) ptr, buf.st_size, -1);
		ldata.l_type = F_UNLCK;
		fcntl(fd, F_SETLKW, &ldata);
		close(fd);
		if (res_total < MAX_KEEP - 1)
		{
			if (forward)
				bid++;
			else
				bid--;
		}
		else
			break;
	}

	if (forward)
	{
		pcache[wh].ebid = bid;
		pcache[wh].epos = pos;
	}
	else
	{
		pcache[wh].sbid = bid;
		pcache[wh].spos = pos;
	}
	return 1;
}

int search_menu()
{
	int i, v;
	//char ch;
    //struct _select_def pc_conf;
	char buf[40], genbuf[80];

	getyx(&save_y, &save_x);
    for (i = 0; i < t_lines; i++)
		saveline(i, 0, save_scr[i]);

	
	while (1)
	{
    	clear();
		memset(&query, 0, sizeof(MY_QUERY));
		memset(&pcache, 0, sizeof(POS_CACHE) * MAX_CACHE);
		wh = 0;
		query.day = 365;
		i = 10;
		v = 0;

		move(5, 20);
		prints("紫丁香全站文章查询 -- 金秋纪念版");
		move(10, 0);
		buf[0] = 0;
		if (query.keyword[0])
			snprintf(genbuf, 80, "你要检索的关键字(%s)：", query.keyword);
		else
			snprintf(genbuf, 80, "你要检索的关键字：");
		getdata(i, 0, genbuf, buf, 38, DOECHO, NULL, true);
		if (buf[0])
		{
			v = 1;
			memcpy(query.keyword, buf, strlen(buf));
		}
		i++;

        if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
		//if (!strcasecmp(getCurrentUser()->userid, "SYSOP"))
		{
			buf[0] = 0;
			if (query.id[0])
				snprintf(genbuf, 80, "你要检索的用户ID(%s)：", query.id);
			else
				snprintf(genbuf, 80, "你要检索的用户ID：");
			getdata(i, 0, genbuf, buf, 12, DOECHO, NULL, true);
			if (buf[0])
			{
				v = 1;
				memcpy(query.id, buf, strlen(buf));
			}
			i++;
		}
		else
		{
			buf[0] = 0;
			snprintf(genbuf, 80, "检索自己的文章(y/n)：");
			getdata(i, 0, genbuf, buf, 2, DOECHO, NULL, true);
			if (buf[0]=='Y'||buf[0]=='y')
			{
				v = 1;
				memcpy(query.id, getCurrentUser()->userid, strlen(getCurrentUser()->userid));
			}
			i++;
		}
		
		do
		{
			buf[0] = 0;
			snprintf(genbuf, 80, "你要检索最近(%d)天的文章：", query.day);
			getdata(i, 0, genbuf, buf, 4, DOECHO, NULL, true);
			if (!buf[0])
				break;
			query.day = atoi(buf);
		} while (query.day < 1 || query.day > 10000);

		if (v)
		{
			bid = 0;
			pos = 0;
            move(t_lines - 1, 0);
            prints("查找数据....");
            clrtoeol();
			do_search(1);
			if (res_total > 0)
			{
				wh ++;
				choose_file();
			}
			else
			{
				prints("本次搜索没有命中任何目标!");
			}
		}
		else
			break;
	}
	return 0;
}
