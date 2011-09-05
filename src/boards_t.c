/*
		scan complete for global variable
*/

#include "bbs.h"
#include "read.h"

extern time_t login_start_time;
static int yank_flag=0;

#define favbrd_list_t (*(getSession()->favbrd_list_count))
extern int do_select(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
extern int modify_board(int bid);
static int check_newpost(struct newpostdata *ptr);

int EGroup(const char *cmd){
    char buf[STRLEN];
    const char *prefix;
    sprintf(buf,"EGROUP%c",cmd[0]);
    prefix=sysconf_str(buf);
    return choose_board((DEFINE(getCurrentUser(),DEF_NEWPOST)?1:0),prefix,0,0);
}

int ENewGroup(const char *cmd){
    return choose_board((DEFINE(getCurrentUser(),DEF_NEWPOST)?1:0),NULL,-2,0);
}

static int clear_all_board_read_flag_func(struct boardheader *bh,void* arg)
{
#ifdef HAVE_BRC_CONTROL
    if (brc_initial(getCurrentUser()->userid, bh->filename, getSession()) != 0)
        brc_clear(0, getSession());
#endif
    return 0;
}

int clear_all_board_read_flag(void){
    char ans[4];
    struct boardheader* save_board;
    int bid;

    getdata(t_lines - 1, 0, "������е�δ�����ô(Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {

        bid=currboardent;
        save_board=currboard;
        apply_boards(clear_all_board_read_flag_func,NULL);
        currboard=save_board;
        currboardent=bid;
    }
    return 0;
}

int Boards(void){
    return choose_board(0,NULL,0,0);
}

int New(void){
    return choose_board(1,NULL,0,0);
}

int unread_position(dirfile, ptr)
char *dirfile;
struct newpostdata *ptr;
{
#ifdef HAVE_BRC_CONTROL
    struct fileheader fh;
    int id;
    int fd, offset, step, num;

    num = ptr->total + 1;
    if (ptr->unread && (fd = open(dirfile, O_RDWR)) > 0) {
        if (!brc_initial(getCurrentUser()->userid, ptr->name, getSession())) {
            num = 1;
        } else {
            offset = (int) ((char *) &(fh.id) - (char *) &(fh));
            num = ptr->total - 1;
            step = 4;
            while (num > 0) {
                lseek(fd, offset + num * sizeof(fh), SEEK_SET);
                if (read(fd, &id, sizeof(unsigned int)) <= 0 || !brc_unread(id, getSession()))
                    break;
                num -= step;
                if (step < 32)
                    step += step / 2;
            }
            if (num < 0)
                num = 0;
            while (num < ptr->total) {
                lseek(fd, offset + num * sizeof(fh), SEEK_SET);
                if (read(fd, &id, sizeof(unsigned int)) <= 0 || brc_unread(id, getSession()))
                    break;
                num++;
            }
        }
        close(fd);
    }
    if (num < 0)
        num = 0;
    return num;
#else
    return 0;
#endif
}


/* Select the fav path
seperated by pig2532@newsmth */


/* etnlegend, 2006.05.25, ѡ����˶�����Ŀ¼����, ��Ҫ�� MULTIPAGE �� HOTKEY ... */

#define FSP_TITLE_ROW 2
#define FSP_ROW (FSP_TITLE_ROW+2)
#define FSP_COL 4
#define FSP_LINES (t_lines-(FSP_ROW+2))

static int fsp_select(struct _select_def *conf){
    return SHOW_SELECT;
}

static int fsp_show(struct _select_def *conf,int index){
    outs((((struct _select_item*)(conf->arg))[index-1]).data);
    return SHOW_CONTINUE;
}

static int fsp_key(struct _select_def *conf,int key){
    int index;
    if(key==KEY_ESC||key==KEY_RIGHT)
        return SHOW_QUIT;
    for(index=0;index<FSP_LINES;index++){
        if(toupper(key)==toupper((((struct _select_item*)(conf->arg))[(conf->page_pos-1)+index]).hotkey)){
            conf->new_pos=(conf->page_pos+index);
            return SHOW_SELCHANGE;
        }
    }
    return SHOW_CONTINUE;
}

static int fsp_refresh(struct _select_def *conf){
    clear();
    move(FSP_TITLE_ROW,FSP_COL);
    prints("\033[1;32m%s \033[1;33m[%s]\033[m",
        "��ѡ����˶�����Ŀ¼","<Enter>��<Right>��ѡ��/<Esc>��<Left>��ȡ��");
    return SHOW_CONTINUE;
}

static int fav_select_path(void){
#define FSP_FREE()                      \
    do{                                 \
        struct _select_item *cur;       \
        if(!sel)                        \
            break;                      \
        if(pts)                         \
            free(pts);                  \
        for(cur=sel;cur->data;cur++)    \
            free(cur->data);            \
        free(sel);                      \
    }while(0)
#define FSP_MALLOC(ptr,size)            \
    do{                                 \
        if(!((ptr)=malloc(size))){      \
            FSP_FREE();                 \
            return -1;                  \
        }                               \
        if((void*)ptr==(void*)sel)      \
            memset(ptr,0,size);         \
    }while(0)
#define FSP_MENUSTR_SIZE 80
#define FSP_HOTKEY(index) ((((index%FSP_LINES)<10)?'0':('A'-10))+(index%FSP_LINES))
    struct _select_def conf;
    struct _select_item *sel;
    POINT *pts;
    int index,ret;
    load_myboard(getSession(),0);
    sel=NULL;
    pts=NULL;
    FSP_MALLOC(sel,((getSession()->mybrd_list_t+1)*sizeof(struct _select_item)));
    FSP_MALLOC(pts,(getSession()->mybrd_list_t*sizeof(POINT)));
    for(index=0;index<getSession()->mybrd_list_t;index++){
        FSP_MALLOC(sel[index].data,FSP_MENUSTR_SIZE);
        sel[index].x=FSP_COL;
        sel[index].y=(FSP_ROW+index);
        sel[index].type=SIT_SELECT;
        sel[index].hotkey=FSP_HOTKEY(index);
        snprintf(sel[index].data,FSP_MENUSTR_SIZE,"[\033[1;36m%c\033[m] %s",sel[index].hotkey,
            (!index?"���˶�������Ŀ¼":getSession()->mybrd_list[index].title));
        pts[index].x=sel[index].x;
        pts[index].y=sel[index].y;
    }
    sel[index].x=-1;
    sel[index].y=-1;
    sel[index].type=0;
    sel[index].hotkey=-1;
    sel[index].data=NULL;
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=getSession()->mybrd_list_t;
    conf.item_per_page=FSP_LINES;
    conf.flag=LF_LOOP|LF_MULTIPAGE|LF_VSCROLL;
    conf.prompt="��";
    conf.item_pos=pts;
    conf.arg=sel;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.pos=1;
    conf.page_pos=1;
    conf.on_select=fsp_select;
    conf.show_data=fsp_show;
    conf.key_command=fsp_key;
    conf.show_title=fsp_refresh;
    ret=list_select_loop(&conf);
    FSP_FREE();
    return ((ret==SHOW_SELECT)?(conf.pos-1):-1);
#undef FSP_FREE
#undef FSP_MALLOC
#undef FSP_MENUSTR_SIZE
#undef FSP_HOTKEY
}

#undef FSP_TITLE_ROW
#undef FSP_ROW
#undef FSP_COL
#undef FSP_LINES

/* END -- etnlegend, 2006.05.25, ѡ����˶�����Ŀ¼����, ��Ҫ�� MULTIPAGE �� HOTKEY ... */


/* Add board to fav
seperated by pig2532@newsmth
parameters:
    i: board id
return:
    0: success
    1: fav board exists
    2: error board
*/
static int fav_add_board(int i, int favmode, int favnow)
{
	if(i<=0){
		return 2;
	}else if(IsFavBoard(i-1, getSession(), favmode, favnow)){
		return 1;
	}else{
       	addFavBoard(i - 1, getSession(), favmode, favnow);
		save_favboard(favmode, getSession());
		return 0;
	}
}

int show_boardinfo(const char *bname)
{
    const struct boardheader *bp;
    int bid;
    char ch;

    bid = getbid(bname, &bp);
	if(bid==0)
		return 0;

	clear();

	move(2,0);
	prints("\033[1;33m��������\033[m: %s %s\n\n", bp->filename, bp->title+1);
	prints("\033[1;33m�������\033[m: %s \n", bp->BM);
#ifdef NEWSMTH
	prints("\033[1;31m����web��ַ\033[m: http://%s.board.newsmth.net/ \n", bp->filename);
#endif
	prints("\033[1;33m����ؼ���\033[m: %s \n\n", bp->des);
    prints("\033[1;31m%s\033[m�������� \033[1;31m%s\033[mͳ��ʮ��\n", 
        (bp->flag & BOARD_JUNK) ? "��" : "", (bp->flag & BOARD_POSTSTAT) ? "��" : "");
    prints("\033[1;31m%s\033[m������ת�� \033[1;31m%s\033[m��ճ������ \033[1;31m%s\033[m��re��\n\n", 
			(bp->flag & BOARD_OUTFLAG) ? "" : "��",
			(bp->flag & BOARD_ATTACH) ? "" : "��",
			(bp->flag & BOARD_NOREPLY) ? "��" : "");

    /* etnlegend, ��ѯ��������������ʾ ... */
    if(HAS_PERM(getCurrentUser(),PERM_SYSOP)){
        move(15,0);
        prints("\033[1;33m��������\033[m: %s%s\n",
            ((bp->flag&BOARD_CLUB_HIDE)&&(bp->flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE)))?"����":"",
            ((bp->flag&BOARD_CLUB_READ)&&(bp->flag&BOARD_CLUB_WRITE))?"��д���ƾ��ֲ�":
            ((bp->flag&BOARD_CLUB_READ)?"�����ƾ��ֲ�":
            ((bp->flag&BOARD_CLUB_WRITE)?"д���ƾ��ֲ�":"��")));
        prints("\033[1;33mȨ������\033[m: %s <%s>\n",
            (bp->level&~PERM_POSTMASK)?((bp->level&PERM_POSTMASK)?"��������":"��ȡ����"):"������",
            gen_permstr(bp->level,genbuf));
        prints("\033[1;33m�������\033[m: %s <%d>\n",bp->title_level?get_user_title(bp->title_level):"������",
            (unsigned char)bp->title_level);
#ifdef NEWSMTH
        prints("\033[1;33m��������\033[m: %s <%d>\n",bp->score_level?"��������":"������",bp->score_level);
#endif /* NEWSMTH */
        prints("\033[1;33m�������\033[m: %d\n",bid);
    }
    /* END -- etnlegend, ��ѯ��������������ʾ ... */

    move(t_lines - 1, 0);
    prints("\033[m\033[44m        ��ӵ����˶�����[\033[1;32ma\033[m\033[44m]");
    clrtoeol();
    resetcolor();
    ch = igetkey();
    switch(toupper(ch)) {
    case 'A':
	{
		int i,ret;
   		i=getbid(bname, NULL);
   		if (i<=0)
            return 1;
        if((ret=fav_select_path()) >= 0)
        {
            ret = fav_add_board(i, 1, ret);
            switch(ret) {
            case 0:
                move(2, 0);
                prints("�Ѿ��� %s ��������ӵ����˶�����.", bname);
                clrtoeol();
                pressreturn();
                return 2;
            case 1:
                move(2, 0);
                prints("�Ѵ��ڸ�������.");
                clrtoeol();
                pressreturn();
		break;
            case 2:
                move(2, 0);
                prints("����ȷ��������.");
                clrtoeol();
                pressreturn();
		break;
            }
        }
	}
	default:
        break;
    }
    return 1;
}

/* etnlegend, 2005.10.31, ��ѯ�ڰ��û� */
struct inc_container{
    void *vp;
    int size;
    int curr;
};
static int inc_container_init(struct inc_container *ic,size_t item_size){
#define INCLIST_INIT_SIZE 32
    if(!ic)
        return 1;
    if(!(ic->vp=malloc(INCLIST_INIT_SIZE*item_size)))
        return 2;
    ic->size=INCLIST_INIT_SIZE;
    ic->curr=0;
    return 0;
#undef INCLIST_INIT_SIZE
}
static int inc_container_step(struct inc_container *ic,size_t item_size){
    void *vp;
    if(!ic)
        return 1;
    if(!(vp=realloc(ic->vp,((2*ic->size)*item_size))))
        return 2;
    ic->vp=vp;
    ic->size*=2;
    return 0;
}
static int inc_container_append(struct inc_container *ic,size_t item_size,void *data){
    if(!ic)
        return 1;
    if(!(ic->curr<ic->size)&&inc_container_step(ic,item_size))
        return 2;
    memmove(vpo(ic->vp,(ic->curr*item_size)),data,item_size);
    ic->curr++;
    return 0;
}
static int inc_container_free(struct inc_container *ic){
    if(!ic)
        return 1;
    free(ic->vp);
    return 0;
}
struct bol_arg{
    int uid;
    int mode;
    char from[IPLEN];
};
static int gen_board_online_list(int bid,struct inc_container *ic){
    const struct user_info *ui_list;
    struct bol_arg data;
    int i,uid_guest;
    if(!ic)
        return 1;
    if(inc_container_init(ic,sizeof(struct bol_arg)))
        return 2;
    for(ui_list=get_utmpent(1),i=0;i<USHM_SIZE;i++){
        if(ui_list[i].active&&ui_list[i].currentboard==bid){
            data.uid=ui_list[i].uid;
            data.mode=ui_list[i].mode;
            snprintf(data.from,IPLEN,"%s",ui_list[i].from);
            if(inc_container_append(ic,sizeof(struct bol_arg),&data)){
                inc_container_free(ic);
                return 3;
            }
        }
    }
    if((uid_guest=searchuser("guest"))){
        resolve_guest_table();
        for(i=0;i<MAX_WWW_GUEST;i++){
            if((wwwguest_shm->use_map[(i>>5)]&(1<<(i&0x1F)))&&(wwwguest_shm->guest_entry[i].currentboard==bid)){
                data.uid=uid_guest;
                data.mode=WEBEXPLORE;
                snprintf(data.from,IPLEN,"%s",inet_ntoa(wwwguest_shm->guest_entry[i].fromip));
                if(inc_container_append(ic,sizeof(struct bol_arg),&data)){
                    inc_container_free(ic);
                    return 5;
                }
            }
        }
    }
    return 0;
}
static int show_board_online_list(struct inc_container *ic,int class){
#define SHOW_ONLINE_LIST_ROWS (t_lines-5)
    extern int ingetdata;
    struct bol_arg *p;
    char buf[128],c;
    const char *userid;
    int n,page,size,pos_f,pos_l,cols,curr_row,curr_col;
    page=0;
    cols=(!class?3:
#ifndef HAVE_IPV6_SMTH
        2
#else /* ! HAVE_IPV6_SMTH */
        1
#endif /* HAVE_IPV6_SMTH */
    );
    size=(SHOW_ONLINE_LIST_ROWS*cols);
    p=(struct bol_arg*)(ic->vp);
    do{
        clear();
        move(0,0);
        pos_f=(page*size);
        pos_l=(pos_f+size>ic->curr)?ic->curr:(pos_f+size);
        prints("\033[1;32m[���������б�: �� \033[1;33m%d\033[1;32m λ�û�"
            "/��ǰ�� \033[1;33m%d - %d\033[1;32m λ]\033[m",ic->curr,pos_f+(pos_l>pos_f?1:0),pos_l);
        for(n=pos_f;n<pos_l;n++){
            sprintf(buf,"\033[1;37m%s \033[1;36m<%s>\033[m",
            (userid=getuserid2(p[n].uid))?userid:"<�Ƿ��û�>",(!class?ModeType(p[n].mode):p[n].from));
            curr_row=(2+(n-pos_f)%SHOW_ONLINE_LIST_ROWS);
            curr_col=(((n-pos_f)/SHOW_ONLINE_LIST_ROWS)*(72/cols));
            move(curr_row,curr_col);
            prints("%s",buf);
        }
        move(t_lines-2,0);
        sprintf(buf,"\033[1;32m�� \033[1;33m<Esc>\033[1;32m �˳��� \033[%d;33m<Enter>/<N>\033[1;32m ���ҳ"
            "�� \033[%d;33m<P>\033[1;32m ��ǰ��ҳ: ",(pos_l==ic->curr?0:1),(!pos_f?0:1));
        prints("%s",buf);
        ingetdata=1;
        do{
            c=igetkey();
            switch(c){
                case KEY_ESC:
                    break;
                case '\n':
                case '\r':
                case 'n':
                case 'N':
                    if(pos_l==ic->curr){
                        if(c=='\n'||c=='\r'){
                            c=KEY_ESC;
                            break;
                        }
                        continue;
                    }
                    page++;
                    break;
                case 'p':
                case 'P':
                    if(!pos_f)
                        continue;
                    page--;
                    break;
                default:
                    continue;
            }
            break;
        }
        while(true);
        ingetdata=0;
    }
    while(c!=KEY_ESC);
    return 0;
#undef SHOW_ONLINE_LIST_ROWS
}
int func_board_online_list(const char *name,int class){
    struct inc_container ic;
    int bid;
    if(!(bid=getbid(name,NULL)))
        return 1;
    if(gen_board_online_list(bid,&ic))
        return 2;
    show_board_online_list(&ic,class);
    inc_container_free(&ic);
    return 0;
}
/* END - etnlegend, 2005.10.31, ��ѯ�ڰ��û� */

/* etnlegend, 2005.10.16, ��ѯ�������� */
int query_bm_core(const char *userid,int limited){
    struct userec *user;
    const struct boardheader *bh;
    char buf[16];
    int line,count,n;
    clear();
    move(0,0);
    prints("\033[1;32m[��ѯ��ְ����]\033[m");
    if(!userid||!*userid){
        move(1,0);
        usercomplete("��ѯ�û�: ",buf);
        move(1,0);
        clrtobot();
    }
    else
        sprintf(buf,"%s",userid);
    if(!*buf||!getuser(buf,&user)){
        move(1,0);
        prints("\033[1;31mȡ����ѯ��Ƿ��û�...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;
        clear();
        return FULLUPDATE;
    }
    sprintf(buf,"%s",user->userid);
    move(1,0);
    prints("\033[1;37m�û� \033[1;33m%s\033[m %s\033[1;37m�����������а���İ����б���\033[m",
        buf,HAS_PERM(user,PERM_BOARDS)?"\033[1;37m�а���Ȩ��":"\033[1;31m�ް���Ȩ��");
    for(line=3,count=0,n=0;n<get_boardcount();n++){
        if(!(bh=getboard(n+1))||!*(bh->filename))
            continue;
        if(limited&&!check_read_perm(getCurrentUser(),bh))
            continue;
        if(chk_BM_instr(bh->BM,buf)){
            count++;
            if(!(line<t_lines-3)){
                int key;
                move(line+1,0);
                prints("\033[1;32m�� \033[1;33m<Enter>/<Space>\033[1;32m ������ѯ���� \033[1;33m<Esc>/<Q>\033[1;32m ������ѯ: \033[m");
                do{
                    key=igetch();
                }
                while(key!=13&&key!=32&&key!=27&&key!=113&&key!=81);
                if(key==13||key==32){
                    line=2;
                    move(line++,0);
                    clrtobot();
                }
                else
                    break;
            }
            move(line++,0);
            sprintf(genbuf,"\033[1;37m[%02d] %s %-32s %-32s\033[m",count,
                (line%2)?"\033[1;33m":"\033[1;36m",bh->filename,bh->title+13);
            prints("%s",genbuf);
        }
    }
    move(line+1,0);
    clrtoeol();
    prints("\033[1;32m��ѯ%s\033[1;32m���: ��ѯ�� \033[1;33m%d\033[1;32m ����ְ����...\033[1;37m<Enter>\033[m",
        (n<get_boardcount())?"\033[1;33mδ":"��",count-((n<get_boardcount())?1:0));
    WAIT_RETURN;
    clear();
    return FULLUPDATE;
}

int query_bm(void){
    return query_bm_core(NULL,!HAS_PERM(getCurrentUser(),PERM_SYSOP));
}
/* END - etnlegend, 2005.10.16, ��ѯ�������� */

static int check_newpost(struct newpostdata *ptr)
{
    struct BoardStatus *bptr;

    if (ptr->dir)
        return 0;

    ptr->total = ptr->unread = 0;

    bptr = getbstatus(ptr->pos+1);
    if (bptr == NULL)
        return 0;
    ptr->total = bptr->total;
    ptr->currentusers = bptr->currentusers;

#ifdef HAVE_BRC_CONTROL
    if (!brc_initial(getCurrentUser()->userid, ptr->name, getSession())) {
        ptr->unread = 1;
    } else {
        if (brc_board_unread(ptr->pos+1, getSession())) {
            ptr->unread = 1;
        }
    }
#endif
    ptr->lastpost = bptr->lastpost;
    return 1;
}


enum board_mode {
    BOARD_BOARD,
    BOARD_BOARDALL,
    BOARD_FAV
};

struct favboard_proc_arg {
    struct newpostdata *nbrd;
    int favmode;
    int newflag;
    enum board_mode yank_flag;
    int father; /*���游��㣬������ղؼУ���fav_father,
    ����ǰ���Ŀ¼����group���*/
    bool reloaddata;
    int select_group; //ѡ����һ��Ŀ¼
	/* stiger: select_group:  
	   �� select_loop�˳�������SHOW_SELECT��ʱ��
	   select_group 0: ����s����Ŀ¼
	                1: s�������Ŀ¼
					<0, s����allboardĿ¼, i=0-select_group�� favboard_list[i-1]
					                       ����i=-1-select_group �� favboard_list[i]
	*/

    const char* boardprefix;
    /*����search_board��ʱ�򻺴�*/
    int loop_mode;
    int find;
    char bname[BOARDNAMELEN + 1];
    int bname_len;
    const char** namelist;
};

static int search_board(int *num, struct _select_def *conf, int key)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    int n, ch, tmpn = false;
    extern int ingetdata;
    ingetdata = true;

    *num=0;
    if (arg->find == true) {
        bzero(arg->bname, BOARDNAMELEN);
        arg->find = false;
        arg->bname_len = 0;
    }
    if (arg->namelist==NULL) {
    	arg->namelist=(const char**)malloc(MAXBOARD*sizeof(char*));
        memset(arg->namelist, 0, MAXBOARD*sizeof(char*));
    	conf->get_data(conf,-1,-1);
    }
    while (1) {
        move(t_lines - 1, 0);
        clrtoeol();
        prints("������Ҫ��Ѱ�� board ���ƣ�%s", arg->bname);
        if (key == -1)
            ch = igetkey();
        else {
            ch = key;
            key = -1;
        }

        if (ch == KEY_REFRESH)
            break;
        if (isprint2(ch)) {
            arg->bname[arg->bname_len++] = ch;
            for (n = 0; n < conf->item_count; n++) {
                if (!(arg->namelist[n])) continue;
                if ((!strncasecmp(arg->namelist[n], arg->bname, arg->bname_len))&&*num==0) {
                    tmpn = true;
                    *num = n;
                }
                if (!strcmp(arg->namelist[n], arg->bname)) {
        			ingetdata = false;
                    return 1 /*�ҵ����Ƶİ棬�����ػ� */ ;
				}
            }
            if (tmpn) {
        		ingetdata = false;
                return 1;
			}
            if (arg->find == false) {
                arg->bname[--arg->bname_len] = '\0';
            }
            continue;
        } else if (ch == Ctrl('H') || ch == KEY_LEFT || ch == KEY_DEL || ch == '\177') {
            arg->bname_len--;
            if (arg->bname_len< 0) {
                arg->find = true;
                break;
            } else {
                arg->bname[arg->bname_len]=0;
                continue;
            }
        } else if (ch == '\t') {
            arg->find = true;
            break;
        } else if (Ctrl('Z') == ch) {
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
            break;
        } else if (ch == '\n' || ch == '\r' || ch == KEY_RIGHT) {
            arg->find = true;
            break;
        }
        bell();
    }
    if (arg->find) {
        move(t_lines - 1, 0);
        clrtoeol();
        ingetdata = false;
        return 2 /*������ */ ;
    }
    ingetdata = false;
    return 1;
}

static int fav_show(struct _select_def *conf, int pos)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    struct newpostdata *ptr;
    char buf[LENGTH_SCREEN_LINE];
    bool isdir=false;

    ptr = &arg->nbrd[pos-(conf->page_pos)];
    if ((ptr->dir == 1)&&arg->favmode) {        /* added by bad 2002.8.3*/
        if (ptr->tag < 0)
            prints("       ");
        else if (!arg->newflag){
                isdir=true;
		if(arg->favmode == 1)
            	    prints(" %4d  ��  <Ŀ¼>  ", pos);
		else
            	    prints(" %4d  ��  %-17s  ", pos ,ptr->BM);
		}else{
		    if(arg->favmode == 1)
            	        prints(" %4d  ��  <Ŀ¼>  ", ptr->total);
	            else
            	        prints(" %4d  ��  %-17s  ", ptr->total, ptr->BM);
		}
    } else {
	if ((ptr->dir==1)||(ptr->flag&BOARD_GROUP)) {
            prints(" %4d  �� ", ptr->total);
            isdir=true;
	} else {
        if (!arg->newflag){
			check_newpost(ptr);
            prints(" %4d  %s ", pos, ptr->unread?"��" : "��");     /*zap��־ */
		}
        else if (ptr->zap && !(ptr->flag & BOARD_NOZAPFLAG)) {
            /*
             * ptr->total = ptr->unread = 0;
             * prints( "    -    -" ); 
             */
            /*
             * Leeward: 97.12.15: extended display 
             */
            check_newpost(ptr);
            prints(" %4d%s%s ", ptr->total, ptr->total > 9999 ? " " : "  ", ptr->unread ? "��" : "��"); /*�Ƿ�δ�� */
        } else {
            if (ptr->total == -1) {
                /*refresh();*/
                check_newpost(ptr);
            }
            prints(" %4d%s%s ", ptr->total, ptr->total > 9999 ? " " : "  ", ptr->unread ? "��" : "��"); /*�Ƿ�δ�� */
        }
      }
    }
    /*
     * Leeward 98.03.28 Displaying whether a board is READONLY or not 
     */
    if (ptr->dir == 2)
        sprintf(buf, "%s(%d)", ptr->title, ptr->total);
    else if ((ptr->dir >= 1)&&arg->favmode)
        sprintf(buf, "%s", ptr->title); /* added by bad 2002.8.3*/
    else if (true == checkreadonly(ptr->name))
        sprintf(buf, "[ֻ��] %s", ptr->title + 8);
    else
        sprintf(buf, "%s", ptr->title + 1);

    if ((ptr->dir >= 1)&&arg->favmode)          /* added by bad 2002.8.3*/
        prints("%-50s", buf);
    else {
        char flag[20], onlines[20];
        char f;
        char tmpBM[BM_LEN + 1];

        strncpy(tmpBM, ptr->BM, BM_LEN);
        tmpBM[BM_LEN] = 0;

        if ((ptr->flag & BOARD_CLUB_READ) && (ptr->flag & BOARD_CLUB_WRITE))
            f = 'A';
        else if (ptr->flag & BOARD_CLUB_READ)
            f = 'c';
        else if (ptr->flag & BOARD_CLUB_WRITE)
            f = 'p';
        else
            f = ' ';
        if (ptr->flag & BOARD_CLUB_HIDE) {
            sprintf(flag,"\x1b[1;31m%c\x1b[m",f);
        } else if (f!=' ') {
           sprintf(flag,"\x1b[1;33m%c\x1b[m",f);
        } else {
            sprintf(flag,"%c",f);
        }
        if (!isdir) {
            sprintf(onlines, "%4d", ptr->currentusers>9999?9999:ptr->currentusers);
        } else {
            sprintf(onlines, "%s", "    ");
        }
        prints("%c%-16s%s%s%-32s %s %-12s", ((ptr->zap && !(ptr->flag & BOARD_NOZAPFLAG)) ? '*' : ' '), 
                ptr->name, (ptr->flag & BOARD_VOTEFLAG) ? "\033[31;1mV\033[m" : " ", flag, buf, 
                onlines, ptr->BM[0] <= ' ' ? "����������" : strtok(tmpBM, " "));
    }
    prints("\n");
    return SHOW_CONTINUE;
}

static int fav_prekey(struct _select_def *conf, int *command)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    struct newpostdata *ptr;

    if (arg->loop_mode) {
        int tmp, num;
/* search_board�����⣬Ŀǰ����ֻ��һҳ�����ݣ�ֻ��search
һҳ*/
        tmp = search_board(&num, conf, *command);
        if (tmp == 1) {
            conf->new_pos = num + 1;
            arg->loop_mode = 1;
            move(t_lines - 1, 0);
            clrtoeol();
            prints("������Ҫ��Ѱ�� board ���ƣ�%s", arg->bname);
            return SHOW_SELCHANGE;
        } else {
            arg->find = true;
            arg->bname_len = 0;
            arg->loop_mode = 0;
            conf->new_pos = num + 1;
            return SHOW_REFRESH;
        }
    }

    if (!arg->loop_mode) {
        int y,x;
        getyx(&y, &x);
        update_endline();
        move(y, x);
    }
    ptr = &arg->nbrd[conf->pos - conf->page_pos];
    switch (*command) {
    case 'e':
    case 'q':
        *command = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *command = KEY_UP;
        break;
    case ' ':
    case 'N':
        *command = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *command = KEY_DOWN;
        break;
	case 'r':
		*command = KEY_RIGHT;
		break;
    };
    return SHOW_CONTINUE;
}

static int fav_gotonextnew(struct _select_def *conf)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    int tmp, num,page_pos=conf->page_pos;

        /*��Ѱ��һ��δ���İ���*/
    if (arg->newflag) {
      num = tmp = conf->pos;
      while(num<=conf->item_count) {
          while ((num <= page_pos+conf->item_per_page-1)&&num<=conf->item_count) {
              struct newpostdata *ptr;
  
              ptr = &arg->nbrd[num - page_pos];
              if ((ptr->total == -1) && (ptr->dir == 0))
                  check_newpost(ptr);
              if (ptr->unread)
                  break;
                  num++;
          }
          if ((num <= page_pos+conf->item_per_page-1)&&num<=conf->item_count) {
              conf->pos = num;
              conf->page_pos=page_pos;
  	     return SHOW_DIRCHANGE;
  	 }
          page_pos+=conf->item_per_page;
          num=page_pos;
          (*conf->get_data)(conf, page_pos, conf->item_per_page);
      }
      if (page_pos!=conf->page_pos)
        (*conf->get_data)(conf, conf->page_pos, conf->item_per_page);
    }
    return SHOW_REFRESH;
}

static int fav_onselect(struct _select_def *conf)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    char buf[STRLEN];
    struct newpostdata *ptr;

    ptr = &arg->nbrd[conf->pos - conf->page_pos];

    if (arg->select_group) return SHOW_SELECT; //select a group
    arg->select_group=0;
	if (ptr->dir==1 && ptr->pos==-1 && ptr->flag==0xffffffff && ptr->tag==-1)
		return SHOW_CONTINUE;
    if ((ptr->dir == 1)||((arg->favmode)&&(ptr->flag&BOARD_GROUP))) {        /* added by bad 2002.8.3*/
        return SHOW_SELECT;
    } else {
        const struct boardheader *bh;
        int tmp, bid;

        if ((bid = getbid(ptr->name, &bh)) != 0 && check_read_perm(getCurrentUser(), bh)) {
            int returnmode;

            currboardent=bid;
            currboard=(struct boardheader*)getboard(bid);

#ifdef HAVE_BRC_CONTROL
            brc_initial(getCurrentUser()->userid, ptr->name, getSession());
#endif
            memcpy(currBM, ptr->BM, BM_LEN - 1);
            if (DEFINE(getCurrentUser(), DEF_FIRSTNEW)&&(getPos(DIR_MODE_NORMAL,currboard->filename,currboard)==0)) {
                setbdir(DIR_MODE_NORMAL, buf, currboard->filename);
                tmp = unread_position(buf, ptr);
                savePos(DIR_MODE_NORMAL,currboard->filename,tmp+1,currboard);
            }
            while (1) {
                returnmode=Read();
                if (returnmode==CHANGEMODE) { //select another board
                    if (currboard->flag&BOARD_GROUP) {
			arg->select_group=1;
                        return SHOW_SELECT;
                    }
                } else break;
            }
            (*conf->get_data)(conf, conf->page_pos, conf->item_per_page);
            modify_user_mode(SELECT);
            if (arg->newflag) { /* �����readnew�Ļ�����������һ��δ���� */
                return fav_gotonextnew(conf);
            }
        }
        return SHOW_REFRESH;
    }
}

static int admin_utils_board(struct newpostdata *data,struct favboard_proc_arg *arg,void *varg){
#define AU_LIBRARY  "admin/libadmin_utils.so"
#define AU_FUNCTION "process_key_board"
    typedef int (*FUNC_ADMIN)(struct newpostdata*,struct favboard_proc_arg*,void*);
    FUNC_ADMIN function;
    void *handle;
    if(!HAS_PERM(getCurrentUser(),PERM_SYSOP))
        return -1;
    if(!(function=(FUNC_ADMIN)dl_function(AU_LIBRARY,AU_FUNCTION,&handle)))
        return -1;
    (*function)(data,arg,varg);
    dlclose(handle);
    return 0;
#undef AU_LIBRARY
#undef AU_FUNCTION
}

static int fav_key(struct _select_def *conf, int command)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    struct newpostdata *ptr;

    ptr = &arg->nbrd[conf->pos - conf->page_pos];
    switch (command) {
    case Ctrl('Z'):
        r_lastmsg();            /* Leeward 98.07.30 support msgX */
        break;
    case ';':        /* ai 2006.9.11:ֻ�����棬��Xֻ��Yȡ��ֻ����Ϊ;���ؼ�ֻ��/ȡ��ֻ����������ȷ�� */
        do{
            char buf[STRLEN]; /* etnlegend: ���Ҫ����ǰ��... */
            if(!HAS_PERM(getCurrentUser(),PERM_SYSOP|PERM_OBOARDS)||ptr->dir)
                break;
            if(!strcmp(ptr->name,"syssecurity")||!strcmp(ptr->name,"Filter"))
                break;          /* Leeward 98.04.01 */
            move(t_lines-1,0);clrtoeol();
            if(checkreadonly(ptr->name)){   /* �жϰ��������ֻ����ȡ��, �����ֻ�� */
              	sprintf(buf, "ȡ��ֻ������ %s, �Ƿ�ȷ��",ptr->name);
                if(!askyn(buf,false))
                    return FULLUPDATE;
                board_setreadonly(ptr->name,0);
                /* Bigman 2000.12.11:ϵͳ��¼ */
                sprintf(genbuf,"�⿪ֻ�������� %s ",ptr->name);
                securityreport(genbuf,NULL,NULL, getSession());
                sprintf(genbuf, " readable board %s",ptr->name);
                newbbslog(BBSLOG_USER,"%s",genbuf);
                clrtobot();
                return SHOW_REFRESHSELECT;
            }
            else{
              	sprintf(buf,"ֻ������ %s, �Ƿ�ȷ��",ptr->name);
                if(!askyn(buf,false))
                    return FULLUPDATE;
                if(ptr->name[0]){
                    board_setreadonly(ptr->name,1);
                    /* Bigman 2000.12.11:ϵͳ��¼ */
                    sprintf(genbuf,"ֻ�������� %s ",ptr->name);
                    securityreport(genbuf,NULL,NULL, getSession());
                    sprintf(genbuf," readonly board %s",ptr->name);
                    newbbslog(BBSLOG_USER,"%s",genbuf);
                    clrtobot();
                    return SHOW_REFRESHSELECT;
                }
            }
        }
        while(0);
        break;
    case 'E':
        if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)||ptr->dir)
            break;
        do{
            int bid;
            if(!(bid=getbnum_safe(ptr->name,getSession()))){
                move(t_lines-1,0);
                clrtoeol();
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���������޸ĸð����Ȩ��!");
                WAIT_RETURN;
                return FULLUPDATE;
            }
            modify_board(bid);
        }
        while(0);
        return FULLUPDATE;
    case 'L':
    case 'l':                  /* Luzi 1997.10.31 */
        if (uinfo.mode != LOOKMSGS) {
            show_allmsgs();
            return SHOW_REFRESH;
        }
        break;
    case 'W':
    case 'w':                  /* Luzi 1997.10.31 */
        if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
            break;
        s_msg();
        return SHOW_REFRESH;
    case 'u':                  /*Haohmaru.99.11.29 */
        {
            int oldmode = uinfo.mode;

            clear();
            modify_user_mode(QUERY);
            t_query(NULL);
            modify_user_mode(oldmode);
            return SHOW_REFRESH;
        }
	case 'U':		/* pig2532 2005.12.10 */
		board_query();
		if (BOARD_FAV == arg->yank_flag)
			return SHOW_DIRCHANGE;
		else
			return SHOW_REFRESH;
	/*add by stiger */

    case 'H':
		if(read_hot_info()==CHANGEMODE)
            ReadBoard();
    	return SHOW_REFRESH;

	/* add end */
    case '!':
        Goodbye();
        return SHOW_REFRESH;
    case 'O':
    case 'o':                  /* Luzi 1997.10.31 */
        {                       /* Leeward 98.10.26 fix a bug by saving old mode */
            int savemode;

            savemode = uinfo.mode;

            if (!HAS_PERM(getCurrentUser(), PERM_BASIC))
                break;
            t_friends();
            modify_user_mode(savemode);
            return SHOW_REFRESH;
        }
    case 'C':
    case 'c':                  /*�Ķ�ģʽ */
        if (arg->newflag == 1)
            arg->newflag = 0;
        else
            arg->newflag = 1;
        return SHOW_REFRESH;
    case 'h':
        show_help("help/boardreadhelp");
        return SHOW_REFRESH;
	case Ctrl('A'):
        if (ptr->dir)
            break;
        {
            int oldmode;
            oldmode = uinfo.mode;
            modify_user_mode(QUERYBOARD);
            show_boardinfo(ptr->name);
            modify_user_mode(oldmode);
        }
		return SHOW_REFRESH;
    /* etnlegend, 2005.10.31, ��ѯ�ڰ��û� */
    case ':':
        if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)||ptr->dir||(ptr->flag&BOARD_GROUP))
            break;
        else{
            char ans[4];
            getdata(t_lines-1,0,"\033[1;37m��״̬[M]/��Դ[F]��ʾ [M]: \033[m",ans,2,DOECHO,NULL,true);
            func_board_online_list(ptr->name,(toupper(ans[0])=='F'));
        }
        return SHOW_REFRESH;
    /* END - etnlegend, 2005.10.31, ��ѯ�ڰ��û� */
    case '/':                  /*����board */
        {
            int tmp, num;

            tmp = search_board(&num, conf , -1);
            if (tmp == 1) {
                conf->new_pos = num + 1;
                arg->loop_mode = 1;
                move(t_lines - 1, 0);
                clrtoeol();
                prints("������Ҫ��Ѱ�� board ���ƣ�%s", arg->bname);
                return SHOW_SELCHANGE;
            } else {
                arg->find = true;
                arg->bname_len = 0;
                arg->loop_mode = 0;
                conf->new_pos = num + 1;
                return SHOW_REFRESH;
            }
        }
    case 'S':

		if (!(getCurrentUser()->flags & BRDSORT_FLAG)){
			getCurrentUser()->flags |= BRDSORT_FLAG;
			getCurrentUser()->flags &= ~BRDSORT1_FLAG;
		}else if(getCurrentUser()->flags & BRDSORT1_FLAG){
			getCurrentUser()->flags &= ~BRDSORT_FLAG;
			getCurrentUser()->flags &= ~BRDSORT1_FLAG;
		}else{
			getCurrentUser()->flags |= BRDSORT1_FLAG;
		}
        /*����ʽ */
        return SHOW_DIRCHANGE;
    case 's':                  /* sort/unsort -mfchen */
	{
		int tmp=1;

        if (do_select(0, NULL, &tmp) == CHANGEMODE) {
			if (tmp>0){
				arg->select_group = 0 - tmp;
				return SHOW_SELECT;
			}
            if (!(currboard->flag&BOARD_GROUP)) {
                while (1) {
                    int returnmode;
                    returnmode=Read();
                    if (returnmode==CHANGEMODE) { //select another board
                        if (currboard->flag&BOARD_GROUP) {
                            arg->select_group=1;
                            return SHOW_SELECT;
                        }
                    } else break;
                }
            }
            else {
				arg->select_group=1;
                return SHOW_SELECT;
            }
        }
        modify_user_mode(arg->newflag ? READNEW : READBRD);
        return SHOW_REFRESH;
	}
	case Ctrl('E'):
		{
			int newlevel;
			int oldlevel;
			if(arg->yank_flag != BOARD_FAV)
				return SHOW_CONTINUE;
			if(arg->favmode != 2 && arg->favmode!=3 )
				return SHOW_CONTINUE;
			if(!HAS_PERM(getCurrentUser(),PERM_SYSOP))
				return SHOW_CONTINUE;
			if(! ptr->dir)
				return SHOW_CONTINUE;
			if(ptr->tag <= 0 || ptr->tag >= favbrd_list_t)
				return SHOW_CONTINUE;
			clear();
			oldlevel = getSession()->favbrd_list[ptr->tag].level;
            newlevel = setperms(oldlevel, 0, "Ȩ��", NUMPERMS, showperminfo, NULL);
			if( newlevel != oldlevel){
				getSession()->favbrd_list[ptr->tag].level = newlevel;
				save_favboard(2, getSession());
				return SHOW_DIRCHANGE;
			}else
				return SHOW_REFRESH;

		}
    case Ctrl('O'):
        if (BOARD_FAV == arg->yank_flag && (arg->favmode==2 || arg->favmode==3) && (HAS_PERM(getCurrentUser(),PERM_SYSOP)) ){
            char bname[STRLEN];
            int i = 0, ret;
            extern int in_do_sendmsg;
            extern int super_select_board(char*);

           	if (getSession()->favbrd_list[getSession()->favnow].bnum >= MAXBOARDPERDIR) {
               	move(2, 0);
               	clrtoeol();
               	prints("�Ѿ�������(%d)��", FAVBOARDNUM);
               	pressreturn();
               	return SHOW_REFRESH;
           	}

            move(0, 0);
            clrtoeol();
            prints("����������Ӣ���� (��Сд�Կɣ����հ׼���Tab���Զ���Ѱ): ");
            clrtoeol();

            make_blist(0);
            in_do_sendmsg=1;
            if(namecomplete(NULL,bname)=='#')
                super_select_board(bname);
            in_do_sendmsg=0;

            CreateNameList();   /*  free list memory. */
            if (*bname)
                i = getbnum_safe(bname,getSession());
            if (i==0)
                return SHOW_REFRESH;
        	ret = fav_add_board(i, arg->favmode, getSession()->favnow);

            switch(ret) {
            case 0:
                arg->reloaddata=true;
                return SHOW_DIRCHANGE;
                break;
            case 1:
                move(2, 0);
                prints("�Ѵ��ڸ�������.");
                clrtoeol();
                pressreturn();
                return SHOW_REFRESH;
            case 2:
                move(2, 0);
                prints("����ȷ��������.");
                clrtoeol();
                pressreturn();
                return SHOW_REFRESH;
            }
        }
        break;
    case 'a':
        {
            char bname[STRLEN];
            int i = 0, ret;
            extern int in_do_sendmsg;
            extern int super_select_board(char*);

        	if (BOARD_FAV == arg->yank_flag && arg->favmode==1) {
            	move(0, 0);
            	clrtoeol();
            	prints("����������Ӣ���� (��Сд�Կɣ����հ׼���Tab���Զ���Ѱ): ");
            	clrtoeol();

            	make_blist(0);
            	in_do_sendmsg=1;
            	if(namecomplete(NULL,bname)=='#')
                	super_select_board(bname);
            	in_do_sendmsg=0;

            	CreateNameList();   /*  free list memory. */
            	if (*bname)
                	i = getbnum_safe(bname,getSession());
            	if (i==0)
					return SHOW_REFRESH;

				ret = getSession()->favnow;
			}else{
				if(!ptr->name || !ptr->name[0])
					return SHOW_CONTINUE;
    			i=getbid(ptr->name, NULL);
    			if (i<=0)
                	return SHOW_CONTINUE;

                ret = fav_select_path();
                if(ret < 0)
                {
                    return SHOW_REFRESH;
                }
			}

	       	ret = fav_add_board(i, 1, ret);

            switch(ret) {
            case 0:
                arg->reloaddata=true;
                return SHOW_DIRCHANGE;
                break;
            case 1:
                move(2, 0);
                prints("�Ѵ��ڸ�������.");
                clrtoeol();
                pressreturn();
                return SHOW_REFRESH;
            case 2:
                move(2, 0);
                prints("����ȷ��������.");
                clrtoeol();
                pressreturn();
                return SHOW_REFRESH;
            }
        }
        break;
    case 'A':                  /* added by bad 2002.8.3*/
        if (BOARD_FAV == arg->yank_flag) {
            char bname[STRLEN];

			if ((arg->favmode == 2 || arg->favmode == 3) && !HAS_PERM(getCurrentUser(),PERM_SYSOP))
				return SHOW_REFRESH;


            if (favbrd_list_t >= FAVBOARDNUM) {
                move(2, 0);
                clrtoeol();
                prints("�������Ű����Ѿ�������(%d)��", FAVBOARDNUM);
                pressreturn();
                return SHOW_REFRESH;
            }
            move(0, 0);
            clrtoeol();
            getdata(0, 0, "����������Ŀ¼��: ", bname, 41, DOECHO, NULL, true);
            if (bname[0]) {
                addFavBoardDir(bname, getSession());
                save_favboard(arg->favmode, getSession());
                arg->reloaddata=true;
                return SHOW_DIRCHANGE;
            }
            return SHOW_REFRESH;	/* add by pig2532 on 2005.12.3 */
        }
        break;
    case 't':
        if (BOARD_FAV == arg->yank_flag) {
            char bname[20];

			if ((arg->favmode == 2 || arg->favmode == 3) && !HAS_PERM(getCurrentUser(),PERM_SYSOP))
				return SHOW_REFRESH;

			if (arg->favmode == 1)
				return SHOW_REFRESH;

            if (ptr->dir == 1 && ptr->tag >= 0) {
                move(0, 0);
                clrtoeol();
                getdata(0, 0, "����������Ӣ��Ŀ¼��(����sѡ��): ", bname, 19, DOECHO, NULL, true);
                if (bname[0]) {
                    changeFavBoardDirEname(ptr->tag, bname, getSession());
                    save_favboard(arg->favmode, getSession());
		}
                return SHOW_REFRESH;
            }
        }
        break;
    case 'T':                  /* added by bad 2002.8.3*/
#ifdef BM_CHANGE_BOARD_TITLE
        if(!(ptr->dir)){
            struct boardheader bh,newbh;
            char buf[STRLEN],title[STRLEN];
            int bid,row,col;
            if(!(bid=getboardnum(ptr->name,&bh))||!chk_currBM(bh.BM,getCurrentUser()))
                return SHOW_REFRESH;
            getyx(&row,&col);
            sprintf(buf,"\033[1;33m���� \033[1;32m%s\033[1;33m �������: \033[m",ptr->name);
            strnzhcpy(title,&bh.title[13],48);
            move(row,0);
            clrtoeol();
            getdata(row,0,buf,title,48,DOECHO,NULL,false);
            if(title[0]&&strcmp(title,&bh.title[13])){
#ifdef FILTER
                if(check_badword_str(title,strlen(title),getSession())){
                    move(row,0);
                    clrtoeol();
                    prints("\033[1;33m%s\033[0;33m<Enter>\033[m","������İ�������п��ܺ��в�ǡ������, ����ȡ��...");
                    WAIT_RETURN;
                    return SHOW_REFRESH;
                }
#endif /* FILTER */
                newbh=bh;
                if(!(bid=getboardnum(ptr->name,&bh))||memcmp(&newbh,&bh,sizeof(struct boardheader))){
                    move(row,0);
                    clrtoeol();
                    prints("\033[1;33m%s\033[0;33m<Enter>\033[m","���޸ĵİ��������Ѿ������ı�, ����ȡ��...");
                    WAIT_RETURN;
                    return SHOW_REFRESH;
                }
                strcpy(&newbh.title[13],title);
                edit_group(&bh,&newbh);
                set_board(bid,&newbh,NULL);
                newbbslog(BBSLOG_USER,"BM_CHANGE_BOARD_TITLE: %s<%d> %s",bh.filename,bid,title);
#ifdef BM_CHANGE_BOARD_TITLE_LOG
                do{
                    FILE *fp;
                    char *desc;
                    time_t current;
                    current=time(NULL);
                    if(!(desc=ctime(&current)))
                        break;
                    sprintf(buf,"log/BMCBT/%c/%s",(!isalnum(bh.filename[0])?'_':toupper(bh.filename[0])),bh.filename);
                    if(!(fp=fopen(buf,"a")))
                        break;
                    fprintf(fp,"%-12.12s %-20.20s %s\n",getCurrentUser()->userid,&desc[4],title);
                    fclose(fp);
                }
                while(0);
#endif /* BM_CHANGE_BOARD_TITLE_LOG */
                move(row,0);
                clrtoeol();
                prints("\033[1;32m%s\033[0;33m<Enter>\033[m","������ִ��!");
                WAIT_RETURN;
            }
            return SHOW_REFRESH;
        }
#endif /* BM_CHANGE_BOARD_TITLE */
        if(arg->yank_flag==BOARD_FAV){
            if((arg->favmode==2||arg->favmode==3)&&!HAS_PERM(getCurrentUser(),PERM_SYSOP))
                return SHOW_REFRESH;
            if(ptr->dir==1&&!(ptr->tag<0)){
                char title[STRLEN];
                move(0,0);
                clrtoeol();
                strnzhcpy(title,ptr->title,41);
                getdata(0,0,"������������Ŀ¼��: ",title,41,DOECHO,NULL,false);
                if(title[0]&&strcmp(title,ptr->title)){
                    changeFavBoardDir(ptr->tag,title,getSession());
                    save_favboard(arg->favmode,getSession());
                }
                return SHOW_REFRESH;
            }
        }
        break;
    case 'm':
        if (arg->yank_flag == BOARD_FAV) {

			if ((arg->favmode == 2 || arg->favmode == 3) && !HAS_PERM(getCurrentUser(),PERM_SYSOP))
				return SHOW_REFRESH;

            if (getCurrentUser()->flags & BRDSORT_FLAG) {
                move(0, 0);
                prints("����ģʽ�²����ƶ�������'S'���л�!");
                pressreturn();
            } else {
                    int p, q;
                    char ans[5];
					int gdataret;

					if( ptr->dir )
						p=ptr->pos;
					else
						p=ptr->tag;

                    move(0, 0);
                    clrtoeol();
                    gdataret = getdata(0, 0, "�������ƶ�����λ��:", ans, 4, DOECHO, NULL, true);
					if(gdataret == -1){
						return SHOW_REFRESH;
					}
                    q = atoi(ans) - 1;
                    if (q < 0 || q >= conf->item_count) {
                        move(2, 0);
                        clrtoeol();
                        prints("�Ƿ����ƶ�λ�ã�");
                        pressreturn();
                    } else {
                        arg->father=MoveFavBoard(p, q, getSession());
                        save_favboard(arg->favmode, getSession());
                        arg->reloaddata=true;
                        return SHOW_DIRCHANGE;
                    }
            }
            return SHOW_REFRESH;
        }
        break;

    /* etnlegend, 2006.05.25, ���˶�����֧����Ŀ�ƶ� ... */
    case 'M':
        if(arg->yank_flag==BOARD_FAV){
#define FAV_M_MSG(str)                                          \
    do{                                                         \
        move(t_lines-1,4);                                      \
        clrtoeol();                                             \
        prints("\033[1;31m%s\033[0;33m<Enter>\033[m",(str));    \
        WAIT_RETURN;                                            \
        return SHOW_REFRESH;                                    \
    }while(0)
            int path,item,favid;
            if(arg->favmode==2||arg->favmode==3)
                FAV_M_MSG("��Ŀ�ƶ��������ڸ��˶�����ģʽ����Ч...");
            if((path=fav_select_path())==-1)
                FAV_M_MSG("ȡ������...");
            if(path==getSession()->favnow)
                FAV_M_MSG("Ŀ��Ŀ¼Ϊ��ǰĿ¼...");
            item=getSession()->favbrd_list[getSession()->favnow].bid[(ptr->dir?ptr->pos:ptr->tag)];
            if(ptr->dir){
                for(favid=path;favid>0;favid=FavGetFather(favid,getSession()))
                    if(favid==(-item))
                        FAV_M_MSG("Ŀ��Ŀ¼Ϊ����Ŀ¼������Ŀ¼...");
                getSession()->favbrd_list[(-item)].father=path;
            }
            DelFavBoard(item,getSession());
            if(!IsFavBoard(item,getSession(),arg->favmode,path))
                addFavBoard(item,getSession(),arg->favmode,path);
            save_favboard(arg->favmode,getSession());
            arg->reloaddata=true;
            return SHOW_DIRCHANGE;
#undef FAV_M_MSG
        }
        break;
    /* END -- etnlegend, 2006.05.25, ���˶�����֧����Ŀ�ƶ� ... */

    case 'd':
        if (BOARD_FAV == arg->yank_flag) {

			if ((arg->favmode == 2 || arg->favmode == 3) && !HAS_PERM(getCurrentUser(),PERM_SYSOP))
				return SHOW_REFRESH;

            if (ptr->tag >= 0){
                move(0, 0);
                clrtoeol();
                if( ! askyn("ȷ��ɾ����", 0) )
					return SHOW_REFRESH;
				if(ptr->dir)
					DelFavBoardDir(ptr->pos,getSession()->favnow, getSession());
				else
                	DelFavBoard(ptr->pos, getSession());
                save_favboard(arg->favmode, getSession());
                arg->father=getSession()->favnow;
                arg->reloaddata=true;
                return SHOW_DIRCHANGE;
            }
            return SHOW_REFRESH;
        }
        break;
    case 'y':
        if (arg->yank_flag < BOARD_FAV) {
                                /*--- Modified 4 FavBoard 2000-09-11	---*/
            arg->yank_flag = 1 - arg->yank_flag;
            arg->reloaddata=true;
            return SHOW_DIRCHANGE;
        }
        break;
    case 'z':                  /* Zap */
        if (arg->yank_flag < BOARD_FAV) {
                                /*--- Modified 4 FavBoard 2000-09-11	---*/
            if (HAS_PERM(getCurrentUser(), PERM_BASIC) && !(ptr->flag & BOARD_NOZAPFLAG)) {
                ptr->zap = !ptr->zap;
                ptr->total = -1;
                getSession()->zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
                getSession()->zapbuf_changed = 1;
                return SHOW_REFRESHSELECT;
            }
        }
        break;
    case 'v':                  /*Haohmaru.2000.4.26 */
        if(!strcmp(getCurrentUser()->userid, "guest") || !HAS_PERM(getCurrentUser(), PERM_BASIC)) return SHOW_CONTINUE;
        clear();
		if (HAS_MAILBOX_PROP(&uinfo, MBP_MAILBOXSHORTCUT))
			MailProc();
		else
        	m_read();
        return SHOW_REFRESH;
    case Ctrl('S'):
        if(!admin_utils_board(ptr,arg,NULL))
            return SHOW_REFRESH;
        break;
    }
    return SHOW_CONTINUE;
}

static int fav_refresh(struct _select_def *conf)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
    struct newpostdata *ptr;
	int sort;

    clear();
    ptr = &arg->nbrd[conf->pos - conf->page_pos];
    if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR)) {
        if (arg->yank_flag == BOARD_FAV){
			if(arg->favmode == 2 || arg->favmode == 3){
            	docmdtitle("[�������б�]",
                       "  \033[m��ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32me\x1b[m] �Ķ�[\x1b[1;32m��\x1b[m,\x1b[1;32mr\x1b[m] ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32m��\x1b[m] ��ӵ��ղؼ�[\x1b[1;32ma\x1b[m] ����[\x1b[1;32mS\x1b[m] ����[\x1b[1;32mh\x1b[m]");
			}else{
            	docmdtitle("[���˶�����]",
                       "  \033[m��ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32me\x1b[m] �Ķ�[\x1b[1;32m��\x1b[m,\x1b[1;32mr\x1b[m] ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32m��\x1b[m] ���[\x1b[1;32ma\x1b[m,\x1b[1;32mA\x1b[m] �ƶ�[\x1b[1;32mm\x1b[m] ɾ��[\x1b[1;32md\x1b[m] ����[\x1b[1;32mS\x1b[m] ����[\x1b[1;32mh\x1b[m]");
			}
		}else
            docmdtitle("[�������б�]",
                       "  \033[m��ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32me\x1b[m] �Ķ�[\x1b[1;32m��\x1b[m,\x1b[1;32mr\x1b[m] ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32m��\x1b[m] �г�[\x1b[1;32my\x1b[m] ����[\x1b[1;32mS\x1b[m] ��Ѱ[\x1b[1;32m/\x1b[m] �л�[\x1b[1;32mc\x1b[m] ����[\x1b[1;32mh\x1b[m]");
    } else {
        if (arg->yank_flag == BOARD_FAV){
			if(arg->favmode == 2 || arg->favmode == 3){
            	docmdtitle("[�������б�]", "  \033[m��ѡ��[��,e] �Ķ�[��,r] ѡ��[��,��] ��ӵ��ղؼ�[a] ����[S] ����[h]");
			}else{
            	docmdtitle("[���˶�����]", "  \033[m��ѡ��[��,e] �Ķ�[��,r] ѡ��[��,��] ���[a,A] �ƶ�[m] ɾ��[d] ����[S] ����[h]");
			}
		}else
            docmdtitle("[�������б�]", "  \033[m��ѡ��[��,e] �Ķ�[��,r] ѡ��[��,��] �г�[y] ����[S] ��Ѱ[/] �л�[c] ����[h]");
    }
    move(2, 0);
    setfcolor(WHITE, DEFINE(getCurrentUser(), DEF_HIGHCOLOR));
    setbcolor(BLUE);
    clrtoeol();
	sort = (getCurrentUser()->flags & BRDSORT_FLAG) ? ( (getCurrentUser()->flags&BRDSORT1_FLAG)+1):0;
    prints("  %s %s����������%s       V ��� ת��  %-20s %s����%s ��  ��     ", arg->newflag ? "ȫ�� δ��" : "��� δ��", (sort==1)?"\033[36m":"", (sort==1)?"\033[44;37m":"", "��  ��  ��  ��", (sort & BRDSORT1_FLAG)?"\033[36m":"", (sort & BRDSORT1_FLAG)?"\033[44;37m":"");
    resetcolor();
    if (!arg->loop_mode)
        update_endline();
    else {
            move(t_lines - 1, 0);
            clrtoeol();
            prints("������Ҫ��Ѱ�� board ���ƣ�%s", arg->bname);
    }
    return SHOW_CONTINUE;
}

static int fav_getdata(struct _select_def *conf, int pos, int len)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
	int sort;

    if (arg->reloaddata) {
    	arg->reloaddata=false;
    	if (arg->namelist) 	{
    		free(arg->namelist);
    		arg->namelist=NULL;
    	}
    }

	sort = (getCurrentUser()->flags & BRDSORT_FLAG) ? ( (getCurrentUser()->flags&BRDSORT1_FLAG)+1):0;
    if (pos==-1) 
        fav_loaddata(NULL, arg->father,1, conf->item_count,sort,arg->namelist, getSession());
    else{
		if((arg->favmode==2 || arg->favmode==3)){
    		if(!strcmp(getSession()->favbrd_list[arg->father].ename, "HotBoards")) sort=BRDSORT1_FLAG+1;
		}
        conf->item_count = fav_loaddata(arg->nbrd, arg->father,pos, len,sort,NULL, getSession());
	}
    return SHOW_CONTINUE;
}

static int boards_getdata(struct _select_def *conf, int pos, int len)
{
    struct favboard_proc_arg *arg = (struct favboard_proc_arg *) conf->arg;
	int sort;

    if (arg->reloaddata) {
    	arg->reloaddata=false;
    	if (arg->namelist) 	{
    		free(arg->namelist);
    		arg->namelist=NULL;
    	}
    }
	sort = (getCurrentUser()->flags & BRDSORT_FLAG) ? ( (getCurrentUser()->flags&BRDSORT1_FLAG)+1):0;
    if (pos==-1) 
         load_boards(NULL, arg->boardprefix,arg->father,1, conf->item_count,sort,arg->yank_flag,arg->namelist, getSession());
    else
         conf->item_count = load_boards(arg->nbrd, arg->boardprefix,arg->father,pos, len,sort,arg->yank_flag,NULL, getSession());
    return SHOW_CONTINUE;
}
int choose_board(int newflag, const char *boardprefix,int group,int favmode)
{
/* ѡ�� �棬 readnew��readboard */
    struct _select_def favboard_conf;
    struct favboard_proc_arg arg;
    POINT *pts;
    int i;
    struct newpostdata *nbrd;
    int favlevel = 0;           /*��ǰ���� */
    int favlist[FAVBOARDNUM];   /* ���浱ǰ��group ��Ϣ���ղؼ���Ϣ*/
    int sellist[FAVBOARDNUM];   /*����Ŀ¼�ݹ���Ϣ */
    int oldmode;
    int changelevel=-1; /*��������һ����Ŀ¼ת����mode,���Ǵ��ղؼн����˰���Ŀ¼*/
    int selectlevel=-1; /*��������һ��������sĿ¼*/
	int oldfavmode=favmode;
	int lastloadfavmode = favmode;
    int perform_select;

    oldmode = uinfo.mode;
    modify_user_mode(SELECT);
#ifdef NEW_HELP
	if(favmode)
		helpmode = HELP_GOODBOARD;
	else
		helpmode = HELP_BOARD;
#endif
    clear();
    //TODO: ���ڴ�С��̬�ı�������������bug
    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);

    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 1;
        pts[i].y = i + 3;
    };

    nbrd = (struct newpostdata *) malloc(sizeof(*nbrd) * BBS_PAGESIZE);
    arg.nbrd = nbrd;
    sellist[0] = 1;
    arg.favmode = favmode;
    if (favmode)
        favlist[0] = 0;
    else
        favlist[0] = group;
    arg.namelist=NULL;
    while (1) {
        bzero((char *) &favboard_conf, sizeof(struct _select_def));
        arg.father = favlist[favlevel];
        if (favmode) {
            arg.newflag = 1;
            arg.yank_flag = BOARD_FAV;
        } else {
            arg.newflag = newflag;
            arg.yank_flag = yank_flag;
        }
        if (favmode) {
            getSession()->favnow = favlist[favlevel];
        } else {
            arg.boardprefix=boardprefix;
	 }
        arg.find = true;
        arg.loop_mode = 0;
		arg.select_group =0;
		if (favmode != 0 && lastloadfavmode != favmode){
 			load_favboard(favmode, getSession());
			lastloadfavmode = favmode;
		}
		arg.favmode = favmode;

        favboard_conf.item_per_page = BBS_PAGESIZE;
        favboard_conf.flag = LF_NUMSEL | LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;     /*|LF_HILIGHTSEL;*/
        favboard_conf.prompt = ">";
        favboard_conf.item_pos = pts;
        favboard_conf.arg = &arg;
        favboard_conf.title_pos.x = 0;
        favboard_conf.title_pos.y = 0;
        favboard_conf.pos = sellist[favlevel];
        favboard_conf.page_pos = ((sellist[favlevel]-1)/BBS_PAGESIZE)*BBS_PAGESIZE+1;
        if (arg.namelist) {
        	free(arg.namelist);
        	arg.namelist=NULL;
        }
        arg.reloaddata=false;

        if (favmode)        
            favboard_conf.get_data = fav_getdata;
        else
            favboard_conf.get_data = boards_getdata;

        perform_select=1;
        (*favboard_conf.get_data)(&favboard_conf,favboard_conf.page_pos,BBS_PAGESIZE);
        if(favboard_conf.item_count==0){
            if(arg.favmode||arg.yank_flag==BOARD_BOARDALL)
                perform_select=0;
            else{
                char ans[4];
                getdata(t_lines-1,0,"��ǰλ��û�п���ʾ��������, �Ƿ�鿴�ѱ�ȡ�����ĵ�������? (Y/N) [N]: ",
                    ans,2,DOECHO,NULL,true);
                if(toupper(ans[0])!='Y')
                    perform_select=0;
                else{
                    arg.yank_flag=BOARD_BOARDALL;
                    (*favboard_conf.get_data)(&favboard_conf,favboard_conf.page_pos,BBS_PAGESIZE);
                    if(favboard_conf.item_count==0)
                        perform_select=0;
                }
            }
        }

        fav_gotonextnew(&favboard_conf);
        favboard_conf.on_select = fav_onselect;
        favboard_conf.show_data = fav_show;
        favboard_conf.pre_key_command = fav_prekey;
        favboard_conf.key_command = fav_key;
        favboard_conf.show_title = fav_refresh;

        update_endline();

        if(perform_select==0||list_select_loop(&favboard_conf)==SHOW_QUIT){
            /*�˳�һ��Ŀ¼*/
            favlevel--;
            if (favlevel == -1)
                break;
            if (favlevel==changelevel){ //�Ӱ���Ŀ¼�����ղؼ�
                favmode=oldfavmode;
				if (favmode){
					selectlevel=-1;
				}
			}
        } else {
            /*ѡ����һ��Ŀ¼,SHOW_SELECT��ע���и����裬Ŀ¼�����
            �������FAVBOARDNUM������selist�����
            ע�⣬arg->select_group��������ʾ��select�˰��滹����s��ת�ġ�
            �����s��ת��arg->select_group=true,����arg->select_group=false;
            TODO: ��̬����sellist
            */
	/* stiger: select_group:  
	   �� select_loop�˳�������SHOW_SELECT��ʱ��
	   select_group 0: ����s����Ŀ¼
	                1: s�������Ŀ¼
					<0, s����allboardĿ¼, i=0-select_group�� favboard_list[i-1]
					                       ����i=-1-select_group �� favboard_list[i]
	*/
            sellist[favlevel] = favboard_conf.pos;

            if ((selectlevel==-1)||(arg.select_group==0))
                favlevel++;
	    	else
				favlevel=selectlevel; /*�˻ص���һ�ε�Ŀ¼*/

			//ԭ����favboard
            if (favmode) {
				//�������Ŀ¼
                if (arg.select_group > 0 || (arg.select_group==0 && (nbrd[favboard_conf.pos - favboard_conf.page_pos].flag!=0xffffffff) ) ) {
                    //s�������Ŀ¼
                    if (arg.select_group > 0 ) //select����� 
                        favlist[favlevel] = currboardent;
                    //��s�������Ŀ¼
                    else
                        favlist[favlevel] = nbrd[favboard_conf.pos - favboard_conf.page_pos].pos+1;

                    changelevel=favlevel-1;
                    favmode=0;
				//�����ղؼ�Ŀ¼
                } else{
					//s�����
					//���빫���ղؼ�Ŀ¼
					if (arg.select_group < 0){
                    	favlist[favlevel] = -1 - arg.select_group ;
						changelevel=favlevel-1;
						favmode=2;
					}else{
                        favlist[favlevel] = nbrd[favboard_conf.pos - favboard_conf.page_pos].tag;
					}
				}
            }
			//ԭ������ favboard
            else {
                if (arg.select_group > 0) //select�����
                    favlist[favlevel] = currboardent;
				else if (arg.select_group < 0){
                   	favlist[favlevel] = -1 - arg.select_group ;
					changelevel=favlevel-1;
					favmode=2;
				}else
                favlist[favlevel] = nbrd[favboard_conf.pos - favboard_conf.page_pos].pos+1;
            }
            if (arg.select_group != 0 ) //select�����
		    selectlevel=favlevel;
            sellist[favlevel] = 1;
        };
        clear();
    }
    free(nbrd);
    free(pts);
    if (arg.namelist) {
    	free(arg.namelist);
    	arg.namelist=NULL;
    }
    save_zapbuf(getSession());
    modify_user_mode(oldmode);
    return 0;
}

extern int mybrd_list_t;

int FavBoard(void){
    load_favboard(1,getSession());
    return choose_board(1,NULL,0,1);
}

int AllBoard(void){
    load_favboard(2,getSession());
    return choose_board(1,NULL,0,2);
}

int WwwBoard(void){
    load_favboard(3,getSession());
    return choose_board(1,NULL,0,3);
}

