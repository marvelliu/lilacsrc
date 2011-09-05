/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	only too int global: bmonly a_fmode
*/

/* ������ ��� ���� */

#include "bbs.h"

int ann_quick_view = 0;
#define A_PAGESIZE      ((ann_quick_view)?((t_lines - 5)/2):(t_lines - 5))

#define ADDITEM         0
#define ADDGROUP        1
#define ADDMAIL         2
#define ADDGOPHER       3

int bmonly = 0;
int a_fmode = 1;
int a_fmode_show = 1;
void a_menu();
void a_report();                /*Haohmaru.99.12.06.����������������¼����Ϊ���鹤�������� */

extern int draw_content(char *fn, struct fileheader *fh);

int t_search_down();
int t_search_up();

static char *import_path[ANNPATH_NUM];  /*��˿· */
static char *import_title[ANNPATH_NUM];
static int import_path_select = 0;
static time_t import_path_time = 0;

void a_prompt(bot, pmt, buf)    /* ������״̬�� ���� */
int bot;
char *pmt, *buf;
{
    move(t_lines + bot, 0);
    clrtoeol();
    getdata(t_lines + bot, 0, pmt, buf, 39, DOECHO, NULL, true);
}

void a_prompt2(int bot, char *pmt, char *buf)
{                               /* ������״̬�� ���� ,����ԭ�������� */
    move(t_lines + bot, 0);
    clrtoeol();
    getdata(t_lines + bot, 0, pmt, buf, 39, DOECHO, NULL, false);
}

void a_report(s)                /* Haohmaru.99.12.06 */
char *s;
{
    /*
     * disable it because of none using it , KCN,2002.07.31 
     */
    return;
/*	
    if((fd = open("a_trace",O_WRONLY|O_CREAT,0644)) != -1 ) {
        char buf[512] ;
        char timestr[24], *thetime;
        time_t dtime;
        time(&dtime);
        thetime = ctime(&dtime);
        strncpy(timestr, thetime, 20);
        timestr[20] = '\0';
        flock(fd,LOCK_EX) ;
        lseek(fd,0,SEEK_END) ;
        sprintf(buf,"%s %s %s\n",getCurrentUser()->userid, timestr, s) ;
        write(fd,buf,strlen(buf)) ;
        flock(fd,LOCK_UN) ;
        close(fd) ;
        return ;
    }
*/
}

typedef struct {
    bool save_mode;             /* in save mode,path need valid */
    bool show_path;
} a_select_path_arg;

static int a_select_path_onselect(struct _select_def *conf)
{
    a_select_path_arg *arg = (a_select_path_arg *) conf->arg;

    if (arg->save_mode) {       /* check valid path */
        if (import_title[conf->pos - 1][0] == 0 || import_path[conf->pos - 1][0] == 0) {
            bell();
            return SHOW_CONTINUE;
        }
    } else {                    /* confirm replace */
        if (import_title[conf->pos - 1][0] != 0 && import_path[conf->pos - 1][0] != 0) {
            char ans[STRLEN];

            a_prompt(-2, "Ҫ�������е�˿·ô��(Y/N) [N]", ans);
            if (toupper(ans[0]) != 'Y')
                return SHOW_REFRESH;
        }
    }
    return SHOW_SELECT;
}

static int a_select_path_show(struct _select_def *conf, int i)
{
    a_select_path_arg *arg = (a_select_path_arg *) conf->arg;

    if (import_title[i - 1][0] != 0)
        if (arg->show_path)
            prints(" %2d   %s", i, import_path[i - 1]);
        else
            prints(" %2d   %s", i, import_title[i - 1]);
    else
        prints(" %2d   \x1b[32m<��δ�趨>\x1b[m", i);
    return SHOW_CONTINUE;
}

static int a_select_path_prekey(struct _select_def *conf, int *key)
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

static int a_select_path_key(struct _select_def *conf, int key)
{
    a_select_path_arg *arg = (a_select_path_arg *) conf->arg;
    int oldmode;

    switch (key) {
    case 'h':
    case 'H':
        show_help("help/import_announcehelp");
        return SHOW_REFRESH;
    case 'R':
    case 'r':
        free_import_path(import_path,import_title,&import_path_time);
        load_import_path(import_path,import_title,&import_path_time,&import_path_select, getSession());
        return SHOW_DIRCHANGE;
    case 'a':
    case 'A':
        arg->show_path = !arg->show_path;
        return SHOW_DIRCHANGE;
    case 'T':
    case 't':
        if (import_path[conf->pos - 1][0] != 0) {
            char new_title[STRLEN];

            strncpy(new_title, import_title[conf->pos - 1], STRLEN);
            new_title[STRLEN - 1]=0;
	    getdata(t_lines - 2, 0, "�����ƣ�", new_title, STRLEN - 1, DOECHO, NULL, false);
            if (new_title[0] != 0) {
                free(import_title[conf->pos - 1]);
                new_title[STRLEN - 1] = 0;
                import_title[conf->pos - 1] = (char *) malloc(strlen(new_title) + 1);
                strcpy(import_title[conf->pos - 1], new_title);
        		save_import_path(import_path,import_title,&import_path_time, getSession());
                return SHOW_DIRCHANGE;
            }
            return SHOW_REFRESH;
        }
        break;
    case 'D':
    case 'd':
        if (import_title[conf->pos - 1][0] != 0) {
            char ans[STRLEN];

            a_prompt(-2, "Ҫɾ�����˿·��(Y/N)[N]", ans);
            if (toupper(ans[0]) == 'Y') {
                free(import_title[conf->pos - 1]);
                import_title[conf->pos - 1] = (char *) malloc(1);
                import_title[conf->pos - 1][0] = 0;
        		save_import_path(import_path,import_title,&import_path_time, getSession());
                return SHOW_DIRCHANGE;
            }
            return SHOW_REFRESH;
        }
        break;
    case 'M':
    case 'm':
        {
            char ans[STRLEN];

            a_prompt(-2, "����Ҫ�ƶ�����λ�ã�", ans);
            if ((ans[0] != 0) && isdigit(ans[0])) {
                int new_pos;

                new_pos = atoi(ans);
                if ((new_pos >= 1) && (new_pos <= ANNPATH_NUM) && (new_pos != conf->pos)) {
                    char *tmp;

                    tmp = import_title[conf->pos - 1];
                    import_title[conf->pos - 1] = import_title[new_pos - 1];
                    import_title[new_pos - 1] = tmp;
                    tmp = import_path[conf->pos - 1];
                    import_path[conf->pos - 1] = import_path[new_pos - 1];
                    import_path[new_pos - 1] = tmp;
        			save_import_path(import_path,import_title,&import_path_time, getSession());
                    conf->pos = new_pos;
                    return SHOW_DIRCHANGE;
                }
            }
            return SHOW_REFRESH;
        }
        break;
    case Ctrl('Z'):
        oldmode = uinfo.mode;
        r_lastmsg();
        modify_user_mode(oldmode);
        return SHOW_REFRESH;
    case 'l':		/* ԭ���Ǵ�L �ĳ�С���� ͳһ��� by pig2532 on 2005-12-1 */
        oldmode = uinfo.mode;
        show_allmsgs();
        modify_user_mode(oldmode);
        return SHOW_REFRESH;
    case 'w':		/* ԭ���Ǵ�W �ĳ�С���� ͳһ��� by pig2532 on 2005-12-1 */
        oldmode = uinfo.mode;
        if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
            break;
        s_msg();
        modify_user_mode(oldmode);
        return SHOW_REFRESH;
    case 'u':
        oldmode = uinfo.mode;
        clear();
        modify_user_mode(QUERY);
        t_query(NULL);
        modify_user_mode(oldmode);
        clear();
        return SHOW_REFRESH;
	case 'U':		/* pig2532 2005.12.10 */
		board_query();
        return SHOW_REFRESH;
    }
    return SHOW_CONTINUE;
}

static int a_select_path_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[˿·ѡ��˵�]",
               "�˳�[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ����[\x1b[1;32mEnter\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] �л�[\x1b[1;32ma\x1b[0;37m] ����[\x1b[1;32mT\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]\x1b[m �ƶ�[\x1b[1;32mm\x1b[0;37m]����[\x1b[1;32mh\x1b[0;37m]\x1b[m");
    move(2, 0);
    prints("\033[0;1;37;44m %4s   %-64s", "���", "˿·��");
    clrtoeol();
    update_endline();
    return SHOW_CONTINUE;
}

/* select a announce path
     return the index in import_path
     		1 base,0=error
     author: KCN
     */
static int a_select_path(bool save_mode)
{
    int i;
    struct _select_def pathlist_conf;
    POINT *pts;
    a_select_path_arg arg;

    clear();
    load_import_path(import_path,import_title,&import_path_time,&import_path_select, getSession());
    arg.save_mode = save_mode;
    arg.show_path = false;
    pts = (POINT *) malloc(sizeof(POINT) * ANNPATH_NUM);
    for (i = 0; i < 20; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&pathlist_conf, sizeof(struct _select_def));
    pathlist_conf.item_count = ANNPATH_NUM;
    pathlist_conf.item_per_page = 20;
    /*
     * ���� LF_VSCROLL ������ LEFT ���˳� 
     */
    pathlist_conf.flag = LF_NUMSEL | LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    pathlist_conf.prompt = "��";
    pathlist_conf.item_pos = pts;
    pathlist_conf.arg = &arg;
    pathlist_conf.title_pos.x = 0;
    pathlist_conf.title_pos.y = 0;
    pathlist_conf.pos = import_path_select;
    pathlist_conf.page_pos = 1; /* initialize page to the first one */

    pathlist_conf.on_select = a_select_path_onselect;
    pathlist_conf.show_data = a_select_path_show;
    pathlist_conf.pre_key_command = a_select_path_prekey;
    pathlist_conf.key_command = a_select_path_key;
    pathlist_conf.show_title = a_select_path_refresh;
    pathlist_conf.get_data = NULL;

    i = list_select_loop(&pathlist_conf);
    free(pts);
    if (i == SHOW_SELECT)
        return pathlist_conf.pos;
    else
        return 0;
}

void a_showmenu(pm)             /* ������ �˵� ״̬ */
MENU *pm;
{
    struct stat st;
    struct tm *pt;
    char title[MAXPATH], kind[32];
    char fname[STRLEN];
    char ch;
    char buf[MAXPATH], genbuf[MAXPATH];
    time_t mtime;
    int n;
    int chkmailflag = 0;

    clear();
    chkmailflag = chkmail();

    if (chkmailflag == 2) {     /*Haohmaru.99.4.4.������Ҳ������ */
        prints("\033[5m");
        sprintf(genbuf, "[�������䳬������,����������!]");
    } else if (chkmailflag) {
        prints("\033[5m");
        sprintf(genbuf, "[�����ż�]");
    } else
        strncpy(genbuf, pm->mtitle, MAXPATH);
    if (strlen(genbuf) <= 80)
        sprintf(buf, "%*s", (int)( (80 - strlen(genbuf)) / 2 ), "");
    else
        strcpy(buf, "");
    prints("\033[44m%s%s%s\033[m\n", buf, genbuf, buf);
    prints("            F �Ļ��Լ������䩧���� �ƶ����� <Enter> ��ȡ����,q �뿪\033[m\n");
#ifdef ANN_COUNT
    prints("\033[44m\033[37m ���  %-20s\033[32m��Ŀ¼�ѱ����\033[33m%9d\033[32m��\033[37m ��  ��           %8s \033[m", "[���] ��    ��", pm->count, a_fmode_show == 2 ? "��������" : "�༭����");
#else
    prints("\033[44m\033[37m ���  %-45s ��  ��           %8s \033[m", "[���] ��    ��", a_fmode_show == 2 ? "��������" : "�༭����");
#endif
    prints("\n");
    if (pm->num == 0)
        prints("      << Ŀǰû������ >>\n");
    for (n = pm->page; n < pm->page + BBS_PAGESIZE && n < pm->num; n++) {

        /* etnlegend, 2006.04.29, ������ԭ������ʲôд��... */

        snprintf(title,2*STRLEN,"%s",M_ITEM(pm,n)->title);
        snprintf(fname,STRLEN,"%s",M_ITEM(pm,n)->fname);
        snprintf(genbuf,MAXPATH,"%s/%s",pm->path,fname);

        if(lstat(genbuf,&st)==-1||!(S_ISDIR(st.st_mode)||S_ISREG(st.st_mode)||S_ISLNK(st.st_mode))){
            st.st_mode=0;
            st.st_mtime=time(NULL);
        }

        if(a_fmode_show==2){
            ch=(S_ISDIR(st.st_mode)?'/':' ');
            fname[10]=0;
        }
        else{
            mtime=st.st_mtime;
            pt=localtime(&mtime);
            sprintf(fname,"\033[1;37m%04d.%02d.%02d\033[m",(pt->tm_year+1900),(pt->tm_mon+1),pt->tm_mday);
            ch=' ';
        }

        if(!(M_ITEM(pm,n)->host)){
            switch(st.st_mode&S_IFMT){
                case S_IFDIR:
                    snprintf(kind,32,"%s","[\033[0;37mĿ¼\033[m]");
                    break;
                case S_IFREG:
                    snprintf(kind,32,"%s","[\033[0;36m�ļ�\033[m]");
                    break;
                case S_IFLNK:
                    if(stat(genbuf,&st)==-1||!(S_ISDIR(st.st_mode)||S_ISREG(st.st_mode)))
                        snprintf(kind,32,"%s","[\033[0;32m����\033[m]");
                    else if(S_ISDIR(st.st_mode))
                        snprintf(kind,32,"%s","[\033[0;33mĿ¼\033[m]");
                    else
                        snprintf(kind,32,"%s","[\033[0;33m�ļ�\033[m]");
                    break;
                default:
                    snprintf(kind,32,"%s","[\033[0;32m����\033[m]");
                    break;
            }
        }
        else
            snprintf(kind,32,"%s","[\033[0;33m����\033[m]");

        if (!strncmp(title,"[Ŀ¼] ",7)||!strncmp(title,"[�ļ�] ",7)||!strncmp(title,"[����] ",7))
            sprintf(genbuf,"%-s %-55.55s%-s%c",kind,&title[7],fname,ch);
        else
            sprintf(genbuf,"%-s %-55.55s%-s%c",kind,title,fname,ch);

        snprintf(title,2*STRLEN,"%s",genbuf);
        sprintf(genbuf,"  %3d %c%s\n",(n+1),(!(M_ITEM(pm,n)->attachpos)?' ':'@'),title);
        prints("%s",genbuf);

        /* --END--, etnlegend, 2006.04.29, �����ⶼ��ʲôд��... */

    }
    clrtobot();
    move(t_lines - 1, 0);
    prints("%s", (pm->level & PERM_BOARDS) ?
           "\033[31m\033[44m[��  ��]  \033[33m˵�� h �� �뿪 q,�� �� �������� a �� ����Ŀ¼ g �� �޸ĵ��� e        \033[m" :
           "\033[31m\033[44m[���ܼ�] \033[33m ˵�� h �� �뿪 q,�� �� �ƶ��α� k,��,j,�� �� ��ȡ���� Rtn,��         \033[m");
}

int a_chkbmfrmpath(path)
char *path;
{
	int objectbid , pathnum;
	char *pathslice , *pathdelim;
    const struct boardheader* objectboard;
	char *savept;
	
#ifdef FB2KPC
	if(!strncmp(FB2KPC,path,strlen(FB2KPC))){
		if(fb2kpc_is_owner(path))
			return 1;
		else
			return 0;
	}
#endif
	pathnum = 0;
	pathdelim = "/";
	strtok_r( path , pathdelim , &savept);
	while( pathnum < 3 )
	{
	    pathslice = strtok_r( NULL , pathdelim , &savept);
	    pathnum ++ ;
	}
	objectbid = getbnum_safe(pathslice, getSession());
	if (!objectbid) return 0; //���ɼ����� ? - atppp 20051118
	objectboard = getboard(objectbid);
	if (chk_currBM(objectboard->BM, getCurrentUser()))
        	return 1;
        else
        	return 0;
}

/* added by netty to handle post saving into (0)Announce */
int a_Import(path, key, fileinfo, nomsg, direct, ent)
char *path, *key;
struct fileheader *fileinfo;
int nomsg;
char *direct;                   /* Leeward 98.04.15 */
int ent;
{

    char fname[STRLEN], bname[PATHLEN];
    char buf[PATHLEN];
    MENU pm;
    char ans[STRLEN];
    char importpath[MAXPATH];
    int ret;
    char newpath[MAXPATH];

    ret = 0;
    modify_user_mode(CSIE_ANNOUNCE);
    if (ann_get_path(key, buf, sizeof(buf)) == 0) {
        int i;

        bzero(&pm, sizeof(pm));
        if ((path == NULL) || (path[0] == 0)) {
            i = a_select_path(true);
            if (i == 0)
                return 1;
            import_path_select = i;
            i--;
            if (import_path[i][0] != '\0') {
                pm.path = import_path[i];
                if (path) {
                    strncpy(path, import_path[i], MAXPATH);
                }
            } else {
                strncpy(importpath, buf, MAXPATH);
                importpath[MAXPATH - 1]=0;
                pm.path = importpath;
            }
        } else
            pm.path = path;
        
        strncpy(newpath,pm.path, MAXPATH);
        newpath[MAXPATH - 1] = '\0';
        if (!HAS_PERM(getCurrentUser(), PERM_SYSOP) ) 
        {
                if(!a_chkbmfrmpath(newpath))
                {
                	sprintf(buf, " a.o... ���Ѿ���������¼��������... ... ");
                	a_prompt(-1, buf, ans);
                	return 3;
				}
		}
         /*
         * if (!nomsg) {
         * sprintf(buf, "�������·Ž� %s,ȷ����?(Y/N) [N]: ", pm.path);
         * a_prompt(-1, buf, ans);
         * if (ans[0] != 'Y' && ans[0] != 'y')
         * return 2;
         * }
         */

#ifdef FB2KPC
		if(!strncmp(FB2KPC,newpath,strlen(FB2KPC)))
			ret = 2;
#endif
        a_loadnames(&pm, getSession());
        ann_get_postfilename(fname, fileinfo, &pm);
        sprintf(bname, "%s/%s", pm.path, fname);
        sprintf(buf, "%-38.38s %s ", fileinfo->title, getCurrentUser()->userid);
        a_additem(&pm, buf, fname, NULL, 0, fileinfo->attachment);
        if (a_savenames(&pm) == 0) {
            sprintf(buf, "boards/%s/%s", key, fileinfo->filename);
            f_cp(buf, bname, 0);

            /*
             * Leeward 98.04.15 add below FILE_IMPORTED 
             */
			if(ret==0)
            	bmlog(getCurrentUser()->userid, currboard->filename, 12, 1);
        } else {
            if (!nomsg) {
                sprintf(buf, " ���뾫����ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
                a_prompt(-1, buf, ans);
            }
            ret = 3;
        }
        a_freenames(&pm);
        return ret;
    }
    return 1;
}

int a_menusearch(path, key, level)
char *path, *key;
int level;
{
    char bname[STRLEN], bpath[STRLEN];
    struct stat st;
    const struct boardheader *fhdr;
    int num;

    if (key == NULL) {
        key = bname;
        a_prompt(-1, "��������Ѱ֮����������: ", key);
    }

    setbpath(bpath, key);
    if ((*key == '\0') || (stat(bpath, &st) == -1))     /* �ж�board�Ƿ���� */
        return 0;
    if (!S_ISDIR(st.st_mode))
        return 0;
    if ((num = getbid(key, &fhdr)) == 0)
        return 0;


    if (check_read_perm(getCurrentUser(), fhdr) == 0)
        return 0;

    sprintf(bname,"0Announce/groups/%s",fhdr->ann_path);
    a_menu("", bname, (HAS_PERM(getCurrentUser(), PERM_ANNOUNCE) || HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) ? PERM_BOARDS : 0, 0, NULL);
    return 1;
}

void a_forward(char *path, ITEM *pitem)
{
    struct fileheader fhdr;
    char fname[PATHLEN], *mesg=NULL;

    bzero(&fhdr,sizeof(struct fileheader));/* clear,or have attachment. binxun */
    sprintf(fname, "%s/%s", path, pitem->fname);
    if (dashf(fname)) {
        strnzhcpy(fhdr.title, pitem->title, ARTICLE_TITLE_LEN);
        strncpy(fhdr.filename, pitem->fname, FILENAME_LEN - 1);
		fhdr.filename[FILENAME_LEN - 1] = '\0';
        switch (doforward(path, &fhdr)) {
        case 0:
            mesg = "����ת�����!\n";
            break;
        case -1:
            mesg = "system error!!.\n";
            break;
        case -2:
            mesg = "invalid address.\n";
            break;
        case -552:
            prints
                ("\n\033[1m\033[33m�ż���������վ�޶��ż���������Ϊ %d �ֽڣ���ȡ��ת�Ĳ���\033[m\033[m\n\n���֪�����ˣ�Ҳ��������Լ���:PP����\n\n*1* ʹ�� \033[1m\033[33mWWW\033[m\033[m ��ʽ���ʱ�վ����ʱ���Ա������ⳤ�ȵ����µ��Լ��ļ������\n*2* ʹ�� \033[1m\033[33mpop3\033[m\033[m ��ʽ�ӱ�վ�û�������ȡ�ţ�û���κγ������ơ�\n*3* �������Ϥ��վ�� WWW �� pop3 �������Ķ� \033[1m\033[33mAnnounce\033[m ���йع��档\n",
                 MAXMAILSIZE);
            break;
        default:
            mesg = "ȡ��ת�Ķ���.\n";
        }
	if (mesg)
            prints(mesg);
    } else {
        move(t_lines - 1, 0);
        prints("�޷�ת�Ĵ���Ŀ.\n");
    }
    pressanykey();
}

void a_newitem(pm, mode)        /* �û������µ� ITEM */
MENU *pm;
int mode;
{
    char uident[STRLEN];
    char board[STRLEN], title[STRLEN];
    char fname[STRLEN], fpath[PATHLEN], fpath2[PATHLEN];
    char *mesg=NULL;
    FILE *pn;
    char ans[STRLEN];
    char buf[255];
    long attachpos=0;
#ifdef ANN_AUTONAME
	char head;
#endif

    pm->page = 9999;
#ifdef ANN_AUTONAME
	head='X';
#endif
    switch (mode) {
    case ADDITEM:
#ifdef ANN_AUTONAME
		head='A';
#else
        mesg = "���������ļ�֮Ӣ������(�ɺ�����)��";
#endif
        break;
    case ADDGROUP:
#ifdef ANN_AUTONAME
		head='D';
#else
        mesg = "��������Ŀ¼֮Ӣ������(�ɺ�����)��";
#endif
        break;
    case ADDMAIL:
        sprintf(board, "tmp/bm.%s", getCurrentUser()->userid);
        if (!dashf(board)) {
            sprintf(buf, "��ѽ!! �������ð�(������)�����´����ݴ浵! << ");
            a_prompt(-1, buf, ans);
            return;
        }
        mesg = "�������ļ�֮Ӣ������(�ɺ�����)��";
        break;
    default:
	return;
    }
#ifdef ANN_AUTONAME
	sprintf(fname,"%c%X",head,time(0)+rand());
#else
    a_prompt(-2, mesg, fname);
#endif
    if (*fname == '\0')
        return;
    sprintf(fpath, "%s/%s", pm->path, fname);
#ifdef ANN_AUTONAME
	if (0){
#else
    if (!valid_fname(fname)) {
#endif
        sprintf(buf, "��ѽ!! ����ֻ�ܰ���Ӣ�ļ�����! << ");
        a_prompt(-1, buf, ans);
    } else if (dashf(fpath) || dashd(fpath)) {
        sprintf(buf, "��ѽ!! ϵͳ���Ѿ��� %s ����ļ�������! << ", fname);
        a_prompt(-1, buf, ans);
    } else {
        mesg = "�������ļ���Ŀ¼֮�������� <<  ";
        a_prompt(-1, mesg, title);
        if (*title == '\0')
            return;
        sprintf(buf, "�������ļ���Ŀ¼ %s (����: %s)", fpath + 17, title);
        a_report(buf);
        switch (mode) {
        case ADDITEM:
            modify_user_mode(EDITANN);
            if (-1 == vedit(fpath, 0, NULL,NULL, 0)){
				modify_user_mode(CSIE_ANNOUNCE);
                return;         /* Leeward 98.06.12 fixes bug */
			}
            modify_user_mode(CSIE_ANNOUNCE);
            chmod(fpath, 0644);
            break;
        case ADDGROUP:
            mkdir(fpath, 0755);
            chmod(fpath, 0755);
            break;
        case ADDMAIL:
            /*
             * sprintf( genbuf, "mv -f %s %s",board, fpath );
             */
            {
                f_mv(board, fpath);
                sprintf(fpath2, "tmp/bm.%s.attach", getCurrentUser()->userid);
                attachpos = a_append_attachment(fpath, fpath2);
                my_unlink(fpath2);
            }
            break;
        }
        if (mode != ADDGROUP)
            sprintf(buf, "%-38.38s %s ", title, getCurrentUser()->userid);
        else {
            move(1, 0);
            clrtoeol();
            getdata(1, 0, "����: ", uident, STRLEN - 1, DOECHO, NULL, true);
            if (uident[0] != '\0')
                sprintf(buf, "%-38.38s(BM: %s)", title, uident);
            else
                sprintf(buf, "%-38.38s", title);
        }
        a_additem(pm, buf, fname, NULL, 0, attachpos);
        if(a_savenames(pm)){
            a_loadnames(pm,getSession());
            a_additem(pm,buf,fname,NULL,0,attachpos);
            if(a_savenames(pm)){
                sprintf(buf," ��������ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
                a_prompt(-1,buf,ans);
                a_loadnames(pm,getSession());
                return;
            }
        }
        if(mode==ADDGROUP){
            sprintf(fpath2,"%s/%s/.Names",pm->path,fname);
            if((pn=fopen(fpath2,"w"))){
                fprintf(pn,"#\n#Title=%s\n#\n",buf);
                fclose(pn);
            }
        }
        bmlog(getCurrentUser()->userid,currboard->filename,(mode==ADDMAIL?12:13),1);
    }
}

void a_moveitem(pm)             /*�ı� ITEM ���� */
MENU *pm;
{
    ITEM *tmp;
    char newnum[STRLEN];
    int num, n, temp;

    sprintf(genbuf, "������� %d ����´���: ", pm->now + 1);
    temp = pm->now + 1;
    a_prompt(-2, genbuf, newnum);
    num = (newnum[0] == '$') ? 9999 : atoi(newnum) - 1;

    if(num<0||num==pm->now)
        return;
    if (num >= pm->num)
        num = pm->num - 1;

    tmp=M_ITEM(pm,pm->now);
    if(num>pm->now){
        for(n=pm->now;n<num;n++)
            M_ITEM(pm,n)=M_ITEM(pm,n+1);
    }
    else{
        for(n=pm->now;n>num;n--)
            M_ITEM(pm,n)=M_ITEM(pm,n-1);
    }
    M_ITEM(pm,num)=tmp;
    pm->now = num;

    if (a_savenames(pm) == 0) {
        sprintf(genbuf, "�ı� %s �µ� %d ��Ĵ��򵽵� %d ��", pm->path + 17, temp, pm->now + 1);
        bmlog(getCurrentUser()->userid, currboard->filename, 13, 1);
        a_report(genbuf);
    } else {
        char buf[80], ans[40];

        sprintf(buf, " ��������ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
        a_prompt(-1, buf, ans);
        a_loadnames(pm, getSession());
    }
}

/* etnlegend, 2006.04.29, [������] ����/����/ճ������ */
void a_copypaste(MENU *pm,int mode){
    MENU menu;
    ITEM *item;
    FILE *fp;
    struct stat st;
    char title[STRLEN],filename[STRLEN],path[PATHLEN],newpath[PATHLEN],ans[4],*p;
    int copy,ret,i;
    long ap;
    size_t len;
    enum {PASTE_ERROR,PASTE_CUT,PASTE_COPY} type;
#define ACP_ANY_RETURN(msg) do{prints("\033[1;37m%s\033[0;33m<Any>\033[m",(msg));igetkey();pm->page=9999;return;}while(0)
    move(t_lines-1,0);clrtoeol();
    sethomefile(genbuf,getCurrentUser()->userid,".CP");
    if(!(fp=fopen(genbuf,(!(mode==0||mode==1)?"r":"w"))))
        ACP_ANY_RETURN("ʹ��ճ������ǰӦ��ʹ�ü��л�������...");
    if(mode==0||mode==1){
        item=M_ITEM(pm,pm->now);
        snprintf(title,STRLEN,"%s",item->title);
        snprintf(filename,STRLEN,"%s",item->fname);
        snprintf(path,PATHLEN,"%s/%s",pm->path,filename);
        fprintf(fp,"%d\n%s\n%s\n%s\n%ld\n",mode,title,filename,path,item->attachpos);
        fclose(fp);
        ACP_ANY_RETURN((!mode?"���Ʊ�ʶ���, ����ճ���󷽿�ɾ��Դ�ļ���Ŀ¼!":"���б�ʶ���, ����ճ���󷽿�ɾ��Դ�ļ���Ŀ¼!"));
    }
    do{
#define ACP_FGETS(len) if(!fgets(genbuf,((len)+1),fp)||!genbuf[0]||genbuf[0]=='\r'||genbuf[0]=='\n'){type=PASTE_ERROR;continue;}
#define ACP_DUMPS(dst,src,len) do{snprintf((dst),(len),"%s",(src));if((p=strpbrk((dst),"\r\n"))){*p=0;}}while(0)
        ap=0;
        ACP_FGETS(2);
        if(!isdigit(genbuf[0])){
            type=PASTE_ERROR;
            continue;
        }
        switch(atoi(genbuf)){
            case 0:
                type=PASTE_COPY;
                break;
            case 1:
                type=PASTE_CUT;
                break;
            default:
                type=PASTE_ERROR;
                continue;
        }
        ACP_FGETS(STRLEN);
        ACP_DUMPS(title,genbuf,STRLEN);
        ACP_FGETS(STRLEN);
        ACP_DUMPS(filename,genbuf,STRLEN);
        ACP_FGETS(PATHLEN);
        ACP_DUMPS(path,genbuf,PATHLEN);
        ACP_FGETS(21);
        if(!isdigit(genbuf[0])){
            type=PASTE_ERROR;
            continue;
        }
        ap=atol(genbuf);
#undef ACP_FGETS
#undef ACP_DUMPS
    }
    while(0);
    fclose(fp);
    if(type==PASTE_ERROR)
        ACP_ANY_RETURN("ʹ��ճ������ǰӦ��ʹ�ü��л�������...");
    snprintf(newpath,PATHLEN,"%s/%s",pm->path,filename);
    if(!((access(newpath,F_OK)==-1)&&(errno==ENOENT)))
        ACP_ANY_RETURN("��ǰ·�����Ѵ���ͬ���ļ���Ŀ¼...");
    if(!strncmp(path,newpath,(len=strlen(path)))&&newpath[len]=='/')
        ACP_ANY_RETURN("��ǰ�������л���Ŀ¼������Ŀ¼��, ��������ѭ��...");
    if(stat(path,&st)==-1||!(S_ISDIR(st.st_mode)||S_ISREG(st.st_mode)))
        ACP_ANY_RETURN("Դ�ļ���Ŀ¼������...");
    if(type==PASTE_COPY)
        snprintf(genbuf,STRLEN,"���Ʒ�ʽճ��%s %.30s, ȷ��? (C-����/L-����/N) [N]: ",S_ISDIR(st.st_mode)?"Ŀ¼":"�ļ�",filename);
    else
        snprintf(genbuf,STRLEN,"���з�ʽճ��%s %.38s, ȷ��? (Y/N) [N]: ",S_ISDIR(st.st_mode)?"Ŀ¼":"�ļ�",filename);
    getdata(t_lines-1,0,genbuf,ans,2,DOECHO,NULL,true);
    move(t_lines-1,0);clrtoeol();
    ans[0]=toupper(ans[0]);copy=0;
    if(ans[0]=='Y'||(type==PASTE_COPY&&ans[0]=='C')){
        a_additem(pm,title,filename,NULL,0,ap);
        if(a_savenames(pm)){
            a_loadnames(pm,getSession());
            a_additem(pm,title,filename,NULL,0,ap);
            if(a_savenames(pm)){
                a_loadnames(pm,getSession());
                ACP_ANY_RETURN("��������ʧ��...");
            }
        }
        if(!(type==PASTE_CUT&&!rename(path,newpath))){
            if(S_ISDIR(st.st_mode)){
                sprintf(genbuf,"/bin/cp -pr %s %s",path,newpath);
                if(system(genbuf))
                    ACP_ANY_RETURN("����Ŀ¼�����з�������...");
            }
            else{
                if(f_cp(path,newpath,0))
                    ACP_ANY_RETURN("�����ļ������з�������...");
            }
            copy=1;
        }
    }
    else if(type==PASTE_COPY&&ans[0]=='L'){
        a_additem(pm,title,filename,NULL,0,ap);
        if(a_savenames(pm)){
            a_loadnames(pm,getSession());
            a_additem(pm,title,filename,NULL,0,ap);
            if(a_savenames(pm)){
                a_loadnames(pm,getSession());
                ACP_ANY_RETURN("��������ʧ��...");
            }
        }
        if(path[0]!='/'){
            if(!getcwd(genbuf,PATHLEN))
                ACP_ANY_RETURN("������������...");
            p=&genbuf[strlen(genbuf)];
            snprintf(p,PATHLEN,"/%s",path);
        }
        else
            snprintf(genbuf,PATHLEN,"%s",path);
        if(S_ISDIR(st.st_mode)){
            if(symlink(genbuf,newpath)==-1)
                ACP_ANY_RETURN("����Ŀ¼�����з�������...");
        }
        else{
            if(link(path,newpath)==-1&&symlink(genbuf,newpath)==-1)
                ACP_ANY_RETURN("�����ļ������з�������...");
        }
    }
    else{
        pm->page=9999;
        return;
    }
    bmlog(getCurrentUser()->userid,currboard->filename,13,1);
    if(type==PASTE_CUT){
        sethomefile(genbuf,getCurrentUser()->userid,".CP");
        unlink(genbuf);
        if(copy)
            S_ISDIR(st.st_mode)?f_rm(path):unlink(path);
        if((p=strrchr(path,'/')))
            *p=0;
        memset(&menu,0,sizeof(MENU));
        menu.path=path;
        a_loadnames(&menu,getSession());
        for(i=0;i<menu.num;i++)
            if(!strcmp(M_ITEM(&menu,i)->fname,filename))
                a_delitem(&menu,i--);
        ret=a_savenames(&menu);
        a_freenames(&menu);
        if(ret)
            ACP_ANY_RETURN("�����ļ������з�������...");
    }
    pm->page=9999;
#undef ACP_ANY_RETURN
    return;
}

/* etnlegend, 2006.04.29, [������] ɾ������ */
void a_delete(MENU *pm){
    struct stat st;
    char path[PATHLEN],ans[4];
    sprintf(genbuf,"%5d  %s",(pm->now+1),M_ITEM(pm,pm->now)->title);
    move(t_lines-2,0);clrtobot();
    prints("\033[1;37m%s\033[m",genbuf);
    snprintf(path,PATHLEN,"%s/%s",pm->path,M_ITEM(pm,pm->now)->fname);
    if(!lstat(path,&st)&&(S_ISDIR(st.st_mode)||S_ISREG(st.st_mode)||S_ISLNK(st.st_mode))){
        sprintf(genbuf,"\033[1;37mȷ��ɾ����%s? (Y/N) [N]: \033[m",
            (S_ISLNK(st.st_mode)?"����":(S_ISDIR(st.st_mode)?"Ŀ¼":"�ļ�")));
        getdata(t_lines-1,0,genbuf,ans,2,DOECHO,NULL,true);
        if(toupper(ans[0])!='Y'){
            pm->page=9999;
            return;
        }
        S_ISLNK(st.st_mode)?unlink(path):(S_ISDIR(st.st_mode)?my_f_rm(path):my_unlink(path));
    }
    a_delitem(pm,pm->now);
    if(a_savenames(pm)){
        a_loadnames(pm,getSession());
        move(t_lines-1,0);
        prints("\033[1;37m%s\033[0;33m<Any>\033[m","��������ʧ��...");
        igetkey();
    }
    else
        bmlog(getCurrentUser()->userid,currboard->filename,13,1);
    pm->page=9999;
    return;
}

void a_newname(pm)
MENU *pm;
{
    char fname[STRLEN];
    char fpath[PATHLEN];
    char *mesg;

    a_prompt(-2, "���ļ���: ", fname);
    if (*fname == '\0')
        return;
    sprintf(fpath, "%s/%s", pm->path, fname);
    if (!valid_fname(fname)) {
        mesg = "���Ϸ��ļ�����.";
    } else if (dashf(fpath) || dashd(fpath)) {
        mesg = "ϵͳ�����д��ļ�������.";
    } else {
        sprintf(genbuf, "%s/%s", pm->path, M_ITEM(pm,pm->now)->fname);
        strcpy(M_ITEM(pm,pm->now)->fname, fname);
        if (a_savenames(pm) == 0) {
            if (f_mv(genbuf, fpath) == 0) {
                char r_buf[256];

                sprintf(r_buf, "�����ļ���: %s -> %s", genbuf + 17, fpath + 17);
                a_report(r_buf);
                return;
            }
        } else {
            char buf[80], ans[40];

            sprintf(buf, "��������ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
            a_prompt(-1, buf, ans);
            a_loadnames(pm, getSession());
        }
        mesg = "�ļ�������ʧ�� !!";
    }
    prints(mesg);
    pressanykey();
}

/* add by stiger */
/* Ѱ�Ҷ�ʧ��Ŀ */
int a_repair(MENU *pm)
{
	DIR *dirp;
	struct dirent *direntp;
	int i,changed;
	char title[84];

	changed=0;
	
	dirp=opendir(pm->path);
	if(dirp==NULL) return -1;

	while( (direntp=readdir(dirp)) != NULL){
		if(direntp->d_name[0]=='.') continue;
#ifdef ANN_COUNT
		if(!strcmp(direntp->d_name,"counter.person")) continue;
#endif
		for( i=0; i < pm->num; i++ ){
			if(strcmp(M_ITEM(pm,i)->fname, direntp->d_name)==0){
				i=-1;
				break;
			}
		}
		if(i!=-1){
			sprintf(title, "%-38.38s(BM: SYSOPS)", direntp->d_name);
			a_additem(pm, title, direntp->d_name, NULL, 0, 0);
			changed++;
		}
	}
	closedir(dirp);

	if(changed>0){
		if(a_savenames(pm) != 0)
			changed = 0 - changed;
	}
	return changed;
}
/* add end */

#ifdef ANN_CTRLK
static int a_control_user(char *fpath)
{
    char buf[PATHLEN+20];
    int count;
    char ans[20];
    char uident[STRLEN];

    sprintf(buf, "%s/.allow", fpath);

    while (1) {
        clear();
        prints("�趨Ŀ¼��������\n");
        count = listfilecontent(buf);
        if (count)
            getdata(1, 0, "(A)���� (D)ɾ��or (E)�뿪[E]", ans, 7, DOECHO, NULL, true);
        else
            getdata(1, 0, "(A)���� or (E)�뿪 [E]: ", ans, 7, DOECHO, NULL, true);
        if (*ans == 'A' || *ans == 'a') {
            move(1, 0);
            usercomplete("���ӷ��������Ա: ", uident);
            if (*uident != '\0') {
                if(seek_in_file(buf,uident)){
					move(2,0);
					prints("%s �Ѿ���������\n",uident);
					continue;
				}
				addtofile(buf,uident);
				continue;
            }
        } else if ((*ans == 'D' || *ans == 'd') && count) {
            move(1, 0);
            namecomplete("ɾ�����������Ա: ", uident);
            if (uident[0] != '\0') {
                if (seek_in_file(buf, uident)) {
                    del_from_file(buf, uident);
                }
                continue;
            }
        } else
            break;
    }
	if(count<=0){
		clear();
		move(1,0);
		if(askyn("���������������,��Ҫȡ����������,�������û����Է�����?",0) == 1){
			unlink(buf);
		}
	}

    return 1;
}
#endif

static int admin_utils_announce(MENU *menu,ITEM *item,void *varg){
#define AU_LIBRARY  "admin/libadmin_utils.so"
#define AU_FUNCTION "process_key_announce"
    typedef int (*FUNC_ADMIN)(MENU*,ITEM*,void*);
    FUNC_ADMIN function;
    void *handle;
    if(!HAS_PERM(getCurrentUser(),PERM_SYSOP))
        return -1;
    if(!(function=(FUNC_ADMIN)dl_function(AU_LIBRARY,AU_FUNCTION,&handle)))
        return -1;
    (*function)(menu,item,varg);
    dlclose(handle);
    return 0;
#undef AU_LIBRARY
#undef AU_FUNCTION
}

void a_manager(MENU *pm,int ch)
{
    char uident[STRLEN];
    ITEM *item=NULL;
    char fpath[PATHLEN], changed_T[STRLEN], ans[STRLEN];

    if (pm->num > 0) {
        item = M_ITEM(pm,pm->now);
        sprintf(fpath, "%s/%s", pm->path, item->fname);
    }
    switch (ch) {
    case 'a':
        a_newitem(pm, ADDITEM);
        break;
    case 'g':
        a_newitem(pm, ADDGROUP);
        break;
    case 'i':
        a_newitem(pm, ADDMAIL);
        break;
        /*
         * case 'G':  a_newitem( pm, ADDGOPHER );    break; 
         */
    case 'p':
        a_copypaste(pm, 2);
        break;
    case 'f':{
            int i;

            pm->page = 9999;
            i = a_select_path(false);
            if (i == 0)
                break;

            import_path_select = i;
            i--;
            free(import_path[i]);
            import_path[i] = malloc(strlen(pm->path) + 1);
            strcpy(import_path[i], pm->path);
            free(import_title[i]);
            strncpy(ans, pm->mtitle, STRLEN);
            ans[STRLEN-1]=0;
            move(t_lines - 2, 0);
            clrtoeol();
            getdata(t_lines - 2, 0, "�趨˿·��:", ans, STRLEN - 1, DOECHO, NULL, false);
            import_title[i] = malloc(strlen(ans) + 1);
            strcpy(import_title[i], ans);
        	save_import_path(import_path,import_title,&import_path_time, getSession());
        }
        break;
#ifdef ANN_CTRLK
	case Ctrl('K'):
		a_control_user(pm->path);
        pm->page = 9999;
		break;
#endif
	/* add by stiger,20030502 */
	/* Ѱ�Ҷ�ʧ��Ŀ */
	case 'z':
		if(HAS_PERM(getCurrentUser(), PERM_SYSOP)){
			int i;

			i=a_repair(pm);

			if(i>=0){
				sprintf(genbuf,"���� %d ����ʧ��Ŀ,�밴Enter����...",i);
			}else{
				sprintf(genbuf,"���� %d ����ʧ��Ŀ,��������ʧ��,�밴Enter����...",0-i);
			}
			a_prompt(-1,genbuf,ans);
			pm->page = 9999;
		}
		break;
	/* add end */
    }
    if (pm->num > 0)
        switch (ch) {
        case 's':
            if (++a_fmode >= 3)
                a_fmode = 1;
			a_fmode_show = a_fmode;
            pm->page = 9999;
            break;
        case 'm':
            a_moveitem(pm);
            pm->page = 9999;
            break;
        case 'd':
            a_delete(pm);
            pm->page = 9999;
            break;
        case 'V':
            if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
            {
                sprintf(fpath, "%s/.Names", pm->path);

                if (dashf(fpath)) {
                    modify_user_mode(EDITANN);
                    vedit(fpath, 0, NULL,NULL, 0);
                    modify_user_mode(CSIE_ANNOUNCE);
                }
                pm->page = 9999;
            }
            break;
        case 't':
	    if (item) {
            strncpy(changed_T, item->title, 39);
            changed_T[38] = 0;
            uident[0] = '\0';
            if(strlen(item->title) > 44)
            {
                char *ptr, *tempuid;
                if ((ptr = strchr(item->title + 38, '(')) != NULL) {
                    *ptr = '\0';
                    tempuid = ptr + 1;
                    if (strncmp(tempuid, "BM: ", 4) == 0)
                        tempuid += 4;
                    snprintf(uident, STRLEN, "%s", tempuid);
                    if ((ptr = strchr(uident, ')')) != NULL)
                        *ptr = '\0';
                }
            }
            {
                char *p;

                p = changed_T + strlen(changed_T) - 1;
                for (; p >= changed_T; p--) {
                    if (*p == ' ')
                        *p = 0;
                    else
                        break;
                };
            }
            a_prompt2(-2, "�±���: ", changed_T);
            /*
             * modified by netty to properly handle title change,add bm by SmallPig 
             */
            if (*changed_T) {
                if (dashf(fpath)) {
                    sprintf(genbuf, "%-38.38s %s ", changed_T, getCurrentUser()->userid);
                    strcpy(item->title, genbuf);
                    sprintf(genbuf, "�ı��ļ� %s �ı���", fpath + 17);
                } else if (dashd(fpath)) {
                    move(1, 0);
                    clrtoeol();
                    getdata(1, 0, "����: ", uident, STRLEN - 1, DOECHO, NULL, false);
                    if (uident[0] != '\0')
                        sprintf(genbuf, "%-38.38s(BM: %s)", changed_T, uident);
                    else
                        sprintf(genbuf, "%-38.38s", changed_T);
                    strcpy(item->title, genbuf);
                    sprintf(genbuf, "�ı�Ŀ¼ %s �ı���", fpath + 17);
                }
                if (a_savenames(pm) != 0) {
                    sprintf(genbuf, "��������ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
                    a_prompt(-1, genbuf, ans);
                    a_loadnames(pm, getSession());
                }
                else
                    a_report(genbuf);
            }
            pm->page = 9999;
	    }
            break;

        case '`':
            if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)||!HAS_PERM(getCurrentUser(),PERM_ANNOUNCE)||!item)
                break;
            strnzhcpy(genbuf,item->title,39);
            do{
                char *p,*q;
                for(q=NULL,p=&genbuf[0];*p;p++)
                    if(*p!=' ')
                        q=p;
                if(q)
                    *++q=0;
                else
                    strcpy(genbuf,"<�ޱ���>");
            }
            while(0);
            strcpy(item->title,genbuf);
            move(t_lines-1,0);
            clrtoeol();
            if(a_savenames(pm)){
                prints("\033[1;31;47m\t%s\033[K\033[m","���������з�������, ���س�������...");
                WAIT_RETURN;
                a_loadnames(pm,getSession());
            }
            else{
                prints("\033[1;34;47m\t%s\033[K\033[m","����[���������]�ɹ�, ���س�������...");
                WAIT_RETURN;
            }
            pm->page=9999;
            break;
        case '~':
            if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)||!HAS_PERM(getCurrentUser(),PERM_ANNOUNCE)||!item)
                break;
            strnzhcpy(genbuf,item->title,39);
            sprintf(item->title,"%-38.38s(BM: SYSOPS)",genbuf);
            move(t_lines-1,0);
            clrtoeol();
            if(a_savenames(pm)){
                prints("\033[1;31;47m\t%s\033[K\033[m","���������з�������, ���س�������...");
                WAIT_RETURN;
                a_loadnames(pm,getSession());
            }
            else{
                prints("\033[1;34;47m\t%s\033[K\033[m","����[����������Ϊ (BM: SYSOPS)]�ɹ�, ���س�������...");
                WAIT_RETURN;
            }
            pm->page=9999;
            break;

        case 'S':
            if(!item)
                break;
            do{
                char ans[4];
                enum ANN_SORT_MODE mode;
                getdata(t_lines-1,0,"\033[1;33m[����] \033[1;37m���ļ�����/����\033[1;32m{n/N} "
                    "\033[1;37m��������/����\033[1;32m{t/T} \033[1;37m�������ֶ���/����\033[1;32m{b/B} "
                    "\033[1;37m[]: \033[m",ans,2,DOECHO,NULL,true);
                move(t_lines-1,0);
                clrtoeol();
                switch(ans[0]){
                    case 'n':
                        mode=ANN_SORT_BY_FILENAME;
                        break;
                    case 'N':
                        mode=ANN_SORT_BY_FILENAME_R;
                        break;
                    case 't':
                        mode=ANN_SORT_BY_TITLE;
                        break;
                    case 'T':
                        mode=ANN_SORT_BY_TITLE_R;
                        break;
                    case 'b':
                        mode=ANN_SORT_BY_BM;
                        break;
                    case 'B':
                        mode=ANN_SORT_BY_BM_R;
                        break;
                    default:
                        prints("\033[1;34;47m\t%s\033[K\033[m","ȡ���������, ���س�������...");
                        WAIT_RETURN;
                        continue;
                }
                switch(a_sort_items(pm,mode,getSession())){
                    case 0:
                        prints("\033[1;34;47m\t%s\033[K\033[m","��������ɹ�, ���س�������...");
                        WAIT_RETURN;
                        break;
                    case -1:
                    case -3:
                        prints("\033[1;31;47m\t%s\033[K\033[m","��������з�������[��������ʧ��], ���س�������...");
                        WAIT_RETURN;
                        break;
                    case -2:
                        prints("\033[1;31;47m\t%s\033[K\033[m","��������з�������[��������ʧ��], ���س�������...");
                        WAIT_RETURN;
                        break;
                    case -4:
                    default:
                        prints("\033[1;31;47m\t%s\033[K\033[m","��������з�������[δ֪����], ���س�������...");
                        WAIT_RETURN;
                        break;
                }
            }
            while(0);
            pm->page=9999;
            break;

        case 'e':
	    if (item) {
            if (dashf(fpath)) {
                long attachpos=item->attachpos;
                modify_user_mode(EDITANN);
                vedit(fpath, 0, NULL,&attachpos, 0);
                if (item->attachpos!=attachpos) {
                    item->attachpos=attachpos;
                    if (a_savenames(pm) != 0) {
                        int i;
                        ITEM saveitem;
                        saveitem=*item;
                        /* retry */
                        a_loadnames(pm, getSession());
                        for (i=0;i<pm->num;i++) {
                            if (!strcmp(M_ITEM(pm,i)->fname,saveitem.fname))
                                M_ITEM(pm,i)->attachpos=attachpos;
                        }
                        if(a_savenames(pm)){
                            a_loadnames(pm,getSession());
                            move(t_lines-1,0);
                            clrtoeol();
                            prints("\033[1;31;47m%s\033[K\033[m","���¸�����С�����仯, д�������ļ�ʧ��, ���س�������...");
                            WAIT_RETURN;
                            pm->page=9999;
                            break;
                        }
                    }
                }
                modify_user_mode(CSIE_ANNOUNCE);
                sprintf(genbuf, "�޸����� %s ������", pm->path + 17);
                a_report(genbuf);
            }
            pm->page = 9999;
            }
            break;
        case 'n':
            a_newname(pm);
            pm->page = 9999;
            break;
        case 'c':
            a_copypaste(pm, 0);
            break;
        case 'x':              //added by bad 03-2-10
            a_copypaste(pm, 1);
            break;

        case Ctrl('S'):
            if(!admin_utils_announce(pm,item,NULL))
                pm->page=9999;
            break;

/*  do not support thread read in announce: COMMAN 2002.7
        case '=':  t_search_down();     break;
        case '+':  t_search_up();       break;
*/
        }
}

void ann_get_current_url(char* buf,int buf_len,char *ext, int len,void* arg)
{
	char board[STRLEN], path[MAXPATH], phpname[20];
    MENU *m=(MENU *)arg;
	MENU *tmp;
	int bid;
	char bap[PATHLEN];
	const struct boardheader *fh;
	int sz;

 	/* "bbs0an.php" or "bbsanc.php", by pig2532 */
 	snprintf(path, MAXPATH, "%s/%s", m->path, M_ITEM(m,m->now)->fname);
 	if(dashd(path))
 	{
 	    strcpy(phpname, "bbs0an.php");
#if 0 // orz pig2532 started on 20061121 
 	    /* ���bbs0an.php֧�����ִ���ʽ�ľ�����·����������һ�ο�ɾ�� */
 	    snprintf(buf, buf_len, "http://%s/bbs0an.php?path=%s",
 	        get_my_webdomain(0), path+10);
 	    return;
 	    /* ���� */
#endif
 	}
 	else
 	    strcpy(phpname, "bbsanc.php");
 
	board[0]='\0';

    ann_get_board(m->path, board, sizeof(board));
	if(board[0] =='\0' || (bid=getbnum_safe(board,getSession()))==0){
        snprintf(buf,buf_len-9,"http://%s/%s?path=%s/%s",
          get_my_webdomain(0), phpname, m->path+10, M_ITEM(m,m->now)->fname);
	  return;
	}

	strcpy(buf, "error\n");

    if ((fh = getboard(bid)) == NULL)
        return;

    sprintf(bap,"0Announce/groups/%s",fh->ann_path);
	sz = strlen(bap);

	for(tmp=m; tmp; tmp = (MENU *)(tmp->father)){
		if( !strncmp(bap, tmp->path, sz) && (tmp->path[sz]=='\0' || tmp->path[sz+1]=='\0') )
			break;
	}

	if(tmp==NULL) return;

    snprintf(buf,buf_len-9,"http://%s/%s?p=%d",
        get_my_webdomain(0), phpname, bid);

	for(; tmp; tmp=(MENU *)(tmp->nowmenu)){
		if(strlen(buf) < buf_len-9){
			sprintf(bap, "-%d", tmp->now+1);
			strcat(buf, bap);
		}
	}

	return;

}

/* Show file info in announce, pig2532 */
void ann_showinfo(MENU *m)
{
    char url[STRLEN];
    
    ann_get_current_url(url, STRLEN, NULL, 0, m);
    
    clear();
    move(3, 0);
    prints("�������ļ����ӵ�ַ��");
    move(4, 1);
    prints("%s", url);
    pressanykey();
}

void ann_attach_link_num(char* buf,int buf_len,char *ext, int len,long attachpos,void* arg)
{
    char bap[PATHLEN];
    
    ann_get_current_url(buf, buf_len, ext, len, arg);
    if(attachpos != -1)
    {
        sprintf(bap, "&ap=%ld", attachpos);
        strcat(buf, bap);
    }
}

void ann_attach_link(char* buf,int buf_len,long attachpos,void* arg)
{
    char *fname=(char *)arg;
    /*
     *if (normal_board(currboard->filename)) {
     * @todo: generate temp sid
     */
      snprintf(buf,buf_len-9,"http://%s/bbsanc.php?path=%s&ap=%ld",
        get_my_webdomain(0),fname+10,attachpos);
}

#ifdef FB2KPC
int AddPCorpus(void){
	FILE *fn;
	char    personalpath[PATHLEN], title[200];
	struct userec *lookupuser;

        if (!check_systempasswd()) {
                return 1;
        }
        clear();
        prints("���������ļ�");

		move(1,0);
		usercomplete( "������ʹ���ߴ���: ",title);
		if(title[0]=='\0')
                return 1;
		if(!getuser(title, &lookupuser))
				return 1;

	sprintf(personalpath,FB2KPC "/%c/%s", toupper(lookupuser->userid[0]),lookupuser->userid);
        if (dashd(personalpath)) {
			move(10,0);
			prints("���û��ĸ����ļ�Ŀ¼�Ѵ���\n");
			pressanykey();
		return 1;
	}
	
	move(4,0);
	if(askyn("ȷ��ҪΪ���û�����һ�������ļ���?",1)==0){
		return 1;
	}

    mkdir(personalpath, 0755);
    chmod(personalpath, 0755);

	move(7,0);
	prints("[ֱ�Ӱ� ENTER ��, �����ȱʡΪ: [32m%s �ĸ����ļ�[m]",lookupuser->userid);
	getdata(6, 0, "����������ļ�֮����: ", title, 39, DOECHO, NULL, true);
	if(title[0] == '\0')
		sprintf(title,"%s �ĸ����ļ�",lookupuser->userid);
	sprintf(personalpath, "%s/.Names", personalpath);
        if ((fn = fopen(personalpath, "w")) == NULL) {
              return -1;
        }
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s\n", title);
        fprintf(fn, "#\n");
        fclose(fn);

	move(15,0);
	prints("�Ѿ����������ļ�, �밴���������...");
	pressanykey();
	return 0;
}
#endif

void a_menu(maintitle, path, lastlevel, lastbmonly, father)
char *maintitle, *path;
int lastlevel, lastbmonly;
MENU *father;
{
    MENU me;
    char fname[PATHLEN];
#ifdef ANN_SHOW_WELCOME
	char welcome[PATHLEN+20];
#endif
    int ch;
    char *bmstr;
    char buf[STRLEN];
    int bmonly;
    int oldmode;
    int number = 0;
#ifdef NEW_HELP
    int oldhelpmode = helpmode;
#endif

    bzero(&me, sizeof(me));
    modify_user_mode(CSIE_ANNOUNCE);
#ifdef NEW_HELP
	helpmode = HELP_ANNOUNCE;
#endif
    me.path = path;
#ifdef ANN_SHOW_WELCOME
	strcpy(welcome,path);
	strcat(welcome,"/welcome");
	if(dashf(welcome))
		show_help(welcome);
#endif
    strcpy(me.mtitle, maintitle);
    me.level = lastlevel;
    bmonly = lastbmonly;
	if(father){
		father->nowmenu = &me;
		me.father = father;
	}
    a_loadnames(&me, getSession());           /* Load .Names */

    strcpy(buf, me.mtitle);
#ifdef FB2KPC
	if(!strncmp(FB2KPC,me.path,strlen(FB2KPC))){
		if(fb2kpc_is_owner(me.path))
			me.level |= PERM_BOARDS;
		else if(bmonly==1 && !(me.level & PERM_BOARDS))
			return;
	}else{
#endif
    bmstr = strstr(buf, "(BM:");
    if (bmstr != NULL) {
        if (chk_currBM(bmstr + 4, getCurrentUser()) || HAS_PERM(getCurrentUser(), PERM_SYSOP))
            me.level |= PERM_BOARDS;
        else if (bmonly == 1 && !(me.level & PERM_BOARDS))
            return;
    }
#ifdef FB2KPC
	}
#endif

    if (strstr(me.mtitle, "(BM: BMS)") || strstr(me.mtitle, "(BM: SECRET)") || strstr(me.mtitle, "(BM: SYSOPS)"))
        bmonly = 1;

    strcpy(buf, me.mtitle);
    bmstr = strstr(buf, "(BM:");

    me.page = 9999;
    me.now = 0;
    while (1) {
        if (me.now >= me.num && me.num > 0) {
            me.now = me.num - 1;
        } else if (me.now < 0) {
            me.now = 0;
        }
        if (me.now < me.page || me.now >= me.page + A_PAGESIZE) {
            me.page = me.now - (me.now % A_PAGESIZE);

			if( ! (me.level & PERM_BOARDS) )
				a_fmode_show=1;

            a_showmenu(&me);
        }
        if (ann_quick_view && (me.now < me.num)) {
            if (M_ITEM(&me,me.now)->host == NULL) {
                snprintf(fname, sizeof(fname), "%s/%s", path, M_ITEM(&me,me.now)->fname);
                if (dashf(fname)) {
                    draw_content(fname, NULL);
                    outs("\x1b[m");
                } else {
                    int i; char buf[128];
                    move(BBS_PAGESIZE / 2+3, 0);
                    sprintf(buf, "\x1b[44m%-80.80s\033[m\n", "����һ��Ŀ¼");
                    prints(buf);
                    for(i=BBS_PAGESIZE / 2+4;i<t_lines-1;i++) {
                        move(i,0);
                        clrtoeol();
                    }
                }
            }
        }
        move(3 + me.now - me.page, 0);
        prints("->");

        ch = igetkey();
        move(3 + me.now - me.page, 0);
        prints("  ");
        if (ch == 'Q' || ch == 'q' || ch == KEY_LEFT || ch == EOF)
            break;
      EXPRESS:                 /* Leeward 98.09.13 */
        switch (ch) {
        case KEY_REFRESH:
            a_showmenu(&me);
            break;
        case Ctrl('Z'):
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
            break;
        case ',':
            ann_quick_view = !ann_quick_view;
            if (!ann_quick_view) me.page = 9999;
            break;
        case KEY_UP:
        case 'K':
        case 'k':
            if (--me.now < 0)
                me.now = me.num - 1;
            break;
        case KEY_DOWN:
        case 'J':
        case 'j':
            if (++me.now >= me.num)
                me.now = 0;
            break;
        case KEY_PGUP:
        case Ctrl('B'):
            if (me.now >= A_PAGESIZE)
                me.now -= A_PAGESIZE;
            else if (me.now > 0)
                me.now = 0;
            else
                me.now = me.num - 1;
            break;
        case KEY_PGDN:
        case Ctrl('F'):
        case ' ':
            if (me.now < me.num - A_PAGESIZE)
                me.now += A_PAGESIZE;
            else if (me.now < me.num - 1)
                me.now = me.num - 1;
            else
                me.now = 0;
            break;
		case KEY_HOME:
			me.now = 0;
			break;
		case KEY_END:
			me.now = me.num - 1;
			break;
        case Ctrl('Q'):    /* pig2532: show file info */
            if(!me.num)
                break;
            ann_showinfo(&me);
            me.page = 9999;
            break;
        case Ctrl('C'):
        case Ctrl('P'):
            if(!HAS_PERM(getCurrentUser(),PERM_POST)||!M_ITEM(&me,me.now))
                break;
            sprintf(fname,"%s/%s",path,M_ITEM(&me,me.now)->fname);
            if(!dashf(fname))
                break;
            if(me.now<me.num){
                do{
                    const struct boardheader *bh;
                    char bname[32],ans[4];
                    clear();
                    move(1,0);
                    if(!get_a_boardname(bname,"������Ҫת��������������: ")||!(bh=getbcache(bname)))
                        break;
                    move(2,0);
                    clrtobot();
                    if(!haspostperm(getCurrentUser(),bname)){
                        ans[0]=(HAS_PERM(getCurrentUser(),PERM_LOGINOK)?'1':'0');
                        ans[1]=(('0'+'1')-ans[0]);
                        sprintf(genbuf,"\n\n    ��Ŀǰ�޷��ڸ���������������!\n\n    �������������ԭ�������\033[%c;33m����ķ���Ȩ����\033[m����\033[%c;33m����δͨ��ע��\033[m,\n    ��δͨ��ע����û�����\033[%c;33m���˹�����\033[m����дע�����������ע��:)\n\n    ���س�����>��...\033[0;33m<Enter>\033[m",ans[0],ans[1],ans[1]);
                        prints("%s",genbuf);
                        WAIT_RETURN;
                        break;
                    }
                    if(checkreadonly(bname)){
                        prints("\n\n    %s\033[0;33m<Enter>\033[m","\033[1;33mĿ�İ���ĿǰΪ\033[1;31mֻ��\033[1;33mģʽ, ȡ��ת�ز���...\033[m");
                        WAIT_RETURN;
                        break;
                    }
#ifdef NEWSMTH
                    if(!check_score_level(getCurrentUser(),bh)){
                        prints("\n\n    \033[1;33m%s\033[0;33m<Enter>\033[m","���Ļ��ֲ�����Ŀ�����������趨, ��ʱ�޷���Ŀ��������ת������...");
                        WAIT_RETURN;
                        break;
                    }
#endif /* NEWSMTH */
                    if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)&&deny_me(getCurrentUser()->userid,bname)){
                        prints("\n\n    \033[1;33m%s\033[0;33m<Enter>\033[m","���ѱ�������Աȡ����Ŀ�İ���ķ���Ȩ��...");
                        WAIT_RETURN;
                        break;
                    }
                    sprintf(genbuf,"ȷ��ת���� %s �� %s(L)վ�ڷ��� (A)ȡ������ [A]: ",bh->filename,
                        (!(bh->flag&BOARD_OUTFLAG)?"":"(S)ת�ŷ��� "));
                    clrtoeol();
                    getdata(1,0,genbuf,ans,2,DOECHO,NULL,true);
                    switch(ans[0]){
                        case 'S':
                        case 's':
                            ans[0]=(!(bh->flag&BOARD_OUTFLAG)?'L':'S');
                            break;
                        case 'L':
                        case 'l':
                            ans[0]='L';
                            break;
                        default:
                            ans[0]=0;
                            break;
                    }
                    move(3,0);
                    if(!ans[0]){
                        prints("\033[1;33m%s\033[0;33m<Enter>\033[m","ȡ��ת�ز���...");
                        WAIT_RETURN;
                        break;
                    }
                    post_cross(getCurrentUser(),bh,"",M_ITEM(&me, me.now)->title,fname,0,false,ans[0],5,getSession());
                    prints("\033[1;32m%s\033[0;33m<Enter>\033[m","ת�����!");
                    WAIT_RETURN;
                }
                while(0);
                update_endline();
                me.page=9999;
            }
            break;
        case 'h':
            show_help("help/announcereadhelp");
            me.page = 9999;
            break;
        case '\n':
        case '\r':
            if (number > 0) {
                me.now = number - 1;
                number = 0;
                continue;
            }
        case 'R':
        case 'r':
        case KEY_RIGHT:
            if (me.now < me.num) {
                if (M_ITEM(&me,me.now)->host != NULL) {
                    /*
                     * gopher(me.item[ me.now ]->host,me.item[ me.now ]->fname,
                     * me.item[ me.now ]->port,me.item[ me.now ]->title); 
                     */
                    me.page = 9999;
                    break;
                } else
                    snprintf(fname, sizeof(fname), "%s/%s", path, M_ITEM(&me,me.now)->fname);
                if (dashf(fname)) {
                    /*
                     * ansimore( fname, true ); 
                     */
                    /*
                     * Leeward 98.09.13 �����ܡ�
                     * �����ϣ��¼�ͷֱ����ת��ǰ����һ�� 
                     */
					//register_attach_link(ann_attach_link, fname);
					register_attach_link(ann_attach_link_num, &me);
                    ansimore_withzmodem(fname, false, M_ITEM(&me,me.now)->title);
					register_attach_link(NULL,NULL);
                    move(t_lines - 1, 0);
                    prints("\033[1m\033[44m\033[31m[�Ķ�����������]  \033[33m���� Q,�� �� ��һ������ U,���� ��һ������ <Enter>,<Space>,�� \033[m");
                    switch (ch = igetkey()) {
                    case KEY_DOWN:
                    case ' ':
                    case '\n':
                        if (++me.now >= me.num)
                            me.now = 0;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    case KEY_UP:
                        if (--me.now < 0)
                            me.now = me.num - 1;
                        ch = KEY_RIGHT;
                        goto EXPRESS;
                    case Ctrl('Y'):
                        zsend_file(fname, M_ITEM(&me,me.now)->title);
                        break;
                    case Ctrl('Z'):
                    case 'h':
                        goto EXPRESS;
                    default:
                        break;
                    }
                } else if (dashd(fname)) {
                    a_menu(M_ITEM(&me,me.now)->title, fname, me.level, bmonly, &me);
					me.nowmenu = NULL;
                    a_loadnames(&me, getSession());   /* added by bad 03-2-10 */
                }
                me.page = 9999;
            }
            break;
        case '/':
            if (a_menusearch(path, NULL, me.level))
                me.page = 9999;
            break;
        case 'F':
            if (me.now < me.num && HAS_PERM(getCurrentUser(), PERM_BASIC) && HAS_PERM(getCurrentUser(), PERM_LOGINOK)) {
                a_forward(path, M_ITEM(&me,me.now));
                me.page = 9999;
            }
            break;
        case 'o':
            t_friends();
            me.page = 9999;
            break;              /*Haohmaru 98.09.22 */
        case 'v':
            i_read_mail();
            modify_user_mode(CSIE_ANNOUNCE);
            me.page = 9999;
            break;
        case 'u':
            clear();
            modify_user_mode(QUERY);
            t_query(NULL);
            modify_user_mode(CSIE_ANNOUNCE);
            me.page = 9999;
            break;              /*Haohmaru.99.11.29 */
		case 'U':		/* pig2532 2005.12.10 */
			board_query();
			me.page = 9999;
			break;
        case '!':
            Goodbye();
            me.page = 9999;
            break;              /*Haohmaru 98.09.24 */
            /*
             * case 'Z':
             * if( me.now < me.num && HAS_PERM(getCurrentUser(), PERM_BASIC ) ) {
             * sprintf( fname, "%s/%s", path, me.item[ me.now ]->fname );
             * a_download( fname );
             * me.page = 9999;
             * }
             * break;
             */
        case Ctrl('Y'):
            if (me.now < me.num) {
                if (M_ITEM(&me,me.now)->host != NULL) {
                    /*
                     * gopher(me.item[ me.now ]->host,me.item[ me.now ]->fname,
                     * me.item[ me.now ]->port,me.item[ me.now ]->title); 
                     */
                    me.page = 9999;
                    break;
                } else
                    sprintf(fname, "%s/%s", path, M_ITEM(&me,me.now)->fname);
                if (dashf(fname)) {
                    zsend_file(fname, M_ITEM(&me,me.now)->title);
                    me.page = 9999;
                }
            }
            break;
          case 'l':		/* by pig2532 on 2005.12.01 */
               oldmode = uinfo.mode;
               show_allmsgs();
      	       modify_user_mode(oldmode);
               me.page = 9999;
      	       break;
          case 'w':                  /* by pig2532 on 2005.11.30 */
          	oldmode = uinfo.mode;
          	if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
          	    break;
            s_msg();
          	modify_user_mode(oldmode);
            me.page = 9999;
            break;
        }
        if (ch >= '0' && ch <= '9') {
            number = number * 10 + (ch - '0');
            ch = '\0';
        } else {
            number = 0;
        }
        if (me.level & PERM_BOARDS)
            a_manager(&me, ch);
#ifdef ANN_GUESTBOOK
		else if(ch=='a' && strstr(me.mtitle,"<guestbook>"))
			a_newitem(&me, ADDITEM);
#endif
    }
    a_freenames(&me);
#ifdef NEW_HELP
	helpmode = oldhelpmode;
#endif
}

int Announce(void){
    sprintf(genbuf,"%s ������������",BBS_FULL_NAME);
    a_menu(genbuf,"0Announce",(HAS_PERM(getCurrentUser(),PERM_ANNOUNCE)?PERM_BOARDS:0),0,NULL);
    clear();
    return 0;
}

int set_import_path(char* path)
{
    int i;
    i = a_select_path(true);
    if (i == 0)
        return 1;
    import_path_select = i;
    i--;
    if (import_path[i][0] != '\0') {
        strncpy(path, import_path[i], MAXPATH);
        return 0;
    }
     return 2;
}
