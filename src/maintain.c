/*
 * Pirate Bulletin Board System Copyright (C) 1990, Edward Luke,
 * lush@Athena.EE.MsState.EDU Eagles Bulletin Board System Copyright (C)
 * 1992, Raymond Rocker, rocker@rock.b11.ingr.com Guy Vega,
 * gtvega@seabass.st.usm.edu Dominic Tynes, dbtynes@seabass.st.usm.edu
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 1, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 */

#include "bbs.h"

//Add by Marvel
int gen_x_deny_notify(char *level, char *uid, int deny);

char cexplain[STRLEN];
char *Ctime();
int modify_board(int bid);

static int sysoppassed = 0;

/* modified by wwj, 2001/5/7, for new md5 passwd */
void igenpass(const char *passwd, const char *userid, unsigned char md5passwd[]);

int check_systempasswd()
{
    FILE *pass;
    char passbuf[40], prepass[STRLEN];

    if ((sysoppassed) && (time(NULL) - sysoppassed < 60 * 60))
        return true;
    clear();
    if ((pass = fopen("etc/systempassword", "rb")) != NULL) {
        fgets(prepass, STRLEN, pass);
        prepass[strlen(prepass) - 1] = '\0';
        if (!strcmp(prepass, "md5")) {
            fread(&prepass[16], 1, 16, pass);
        }
        fclose(pass);

        getdata(1, 0, "������ϵͳ����: ", passbuf, 39, NOECHO, NULL, true);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return false;


        if (!strcmp(prepass, "md5")) {
            igenpass(passbuf, "[system]", (unsigned char *) prepass);
            passbuf[0] = (char) !memcmp(prepass, &prepass[16], 16);
        } else {
            passbuf[0] = (char) checkpasswd(prepass, passbuf);
        }
        if (!passbuf[0]) {
            move(2, 0);
            prints("ϵͳ�����������...");
            securityreport("ϵͳ�����������...", NULL, NULL, getSession());
            pressanykey();
            return false;
        }
    }
    sysoppassed = time(NULL);
    return true;
}

int setsystempasswd(void){
    FILE *pass;
    char passbuf[40], prepass[40];

    modify_user_mode(ADMIN);
    if (strcmp(getCurrentUser()->userid, "SYSOP"))
        return -1;
    if (!check_systempasswd())
        return -1;
    getdata(2, 0, "�������µ�ϵͳ����: ", passbuf, 39, NOECHO, NULL, true);
    getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 39, NOECHO, NULL, true);
    if (strcmp(passbuf, prepass))
        return -1;
    if ((pass = fopen("etc/systempassword", "w")) == NULL) {
        move(4, 0);
        prints("ϵͳ�����޷��趨....");
        pressanykey();
        return -1;
    }
    fwrite("md5\n", 4, 1, pass);

    igenpass(passbuf, "[system]", (unsigned char *) prepass);
    fwrite(prepass, 16, 1, pass);

    fclose(pass);
    move(4, 0);
    prints("ϵͳ�����趨���....");
    pressanykey();
    return 0;
}


void deliverreport(title, str)
char *title;
char *str;
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
	gettmpfilename( fname, "deliver" );
    //sprintf(fname, "tmp/deliver.%s.%05d", getCurrentUser()->userid, uinfo.pid);
    if ((se = fopen(fname, "w")) != NULL) {
        fprintf(se, "%s\n", str);
        fclose(se);
        post_file(getCurrentUser(), "", fname, currboard->filename, title, 0, 2, getSession());
        unlink(fname);
        modify_user_mode(savemode);
    }
}

void stand_title(title)
char *title;
{
    clear();
    prints("\x1b[7m%s\x1b[m", title);
}

int m_info(void){
    struct userec uinfo;
    int id;
    struct userec *lookupuser;
#ifdef SOURCE_PERM_CHECK
    if (!HAS_PERM(getCurrentUser(), PERM_ADMIN)) {
        move(3, 0);
        clrtobot();
        prints("��Ǹ, ��û�� ADMIN Ȩ��!");
        pressreturn();
        return 0;
    }
#endif
    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {        /* Haohmaru.98.12.19 */
        return -1;
    }
    clear();
    stand_title("�޸�ʹ���ߴ���");
    move(1, 0);
    usercomplete("������ʹ���ߴ���: ", genbuf);
    if (*genbuf == '\0') {
        clear();
        return -1;
    }
    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints(MSG_ERR_USERID);
        clrtoeol();
        pressreturn();
        clear();
        return -1;
    }
    uinfo = *lookupuser;

    move(1, 0);
    clrtobot();
    disply_userinfo(&uinfo, 1);
    uinfo_query(&uinfo, 1, id);
    return 0;
}

int m_newbrd(void){
    struct boardheader newboard;
    char vbuf[PATHLEN], buf[PATHLEN];
    int bid;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    memset(&newboard, 0, sizeof(newboard));
    prints("������������:");
    while (1) {
        getdata(3, 0, "����������:   ", newboard.filename, BOARDNAMELEN, DOECHO, NULL, true);
        if (newboard.filename[0] == '\0')
            return -1;
        if (valid_brdname(newboard.filename))
            break;
        prints("���Ϸ�����...");
    }
    strcpy(newboard.title, "0[����]      �����������ƴ���");
    strcpy(vbuf, "vote/");
    strcat(vbuf, newboard.filename);
    setbpath(buf, newboard.filename);
    if (getbid(newboard.filename, NULL) > 0) {
        prints("\n�����������Ѿ�����\n");
        pressreturn();
        clear();
        return -1;
    }
    if (mkdir(buf, 0755) == -1 || mkdir(vbuf, 0755) == -1) {
        prints("\n���󣺴���Ŀ¼ʧ�ܣ�����ϵ����վ����...\n");
        pressreturn();
        clear();
        return -1;
    }
#ifndef PERM_NEWBOARD
    newboard.level = PERM_SYSOP;
#else
    newboard.level = PERM_NEWBOARD;
#endif
    if (!HAS_PERM(getCurrentUser(), newboard.level)) newboard.level = 0;
    build_board_structure(newboard.filename);
    snprintf(newboard.ann_path,127,"%s/%s", groups[0], newboard.filename);
    newboard.ann_path[127]=0;
    
    if (add_board(&newboard) == -1) {
		currboard = bcache;
        prints("����������ʧ��! ���ܰ������Ѿ���ϵͳ���� %d\n", MAXBOARD);
        pressreturn();
        clear();
        return -1;
    }
    bid = getbid(newboard.filename, NULL);
    if (bid == 0) {
        prints("\nϵͳ��������ϵ����վ����...\n");
        pressreturn();
        clear();
        return -1;
    }
    edit_group(NULL, &newboard);
	currboard = bcache;
    prints("\n���������������س���������趨������������\n");
    sprintf(genbuf, "add brd %s", newboard.filename);
    bbslog("user", "%s", genbuf);
    {
        char secu[STRLEN];

        sprintf(secu, "�����°棺%s", newboard.filename);
        securityreport(secu, NULL, NULL, getSession());
    }
    pressreturn();
    clear();
    return modify_board(bid);
}





/*etnlegend,2005.07.01,�޸�����������*/
#define KEY_CANCEL '~'
#define EB_BUF_LEN 256
extern int in_do_sendmsg;
static int lastkey=0;
/*����Ȩ���ַ���*/
char* gen_permstr(unsigned int level,char* buf){
    int i;
    /*����bufӦ�þ����㹻�Ĵ�С*/
    sprintf(buf,"%s",XPERMSTR);
    for(i=0;i<NUMPERMS;i++)
        if(!(level&(1<<i)))
            buf[i]='-';
    return buf;
}
/*����list_select_loop����ṹ*/
struct _simple_select_arg{
    struct _select_item *items;
    int flag;
};
static int editbrd_on_select(struct _select_def *conf){
    return SHOW_SELECT;
}
static int editbrd_show(struct _select_def *conf,int i){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    outs((char*)((arg->items[i-1]).data));
    return SHOW_CONTINUE;
}
static int editbrd_key(struct _select_def *conf,int key){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    int i;
    lastkey=key;
    if(key==KEY_ESC||key==KEY_CANCEL)
        return SHOW_QUIT;
    for(i=0;i<conf->item_count;i++)
        if(toupper(key)==toupper(arg->items[i].hotkey)){
            conf->new_pos=i+1;
            return SHOW_SELCHANGE;
        }
    return SHOW_CONTINUE;
}
static int editbrd_refresh(struct _select_def *conf){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    move(arg->items[0].y,0);clrtobot();
    return SHOW_CONTINUE;
}
/*ѡ�������������򾫻�������*/
int select_group(int pos,int force){
    /*ʹ����SECNUM��*/
    struct _select_item sel[SECNUM+1];
    struct _select_def conf;
    struct _simple_select_arg arg;
    POINT pts[SECNUM];
    char menustr[SECNUM][64];
    int i;
    /*����˵���ʾ*/
    for(i=0;i<SECNUM;i++){
        sel[i].x=4;
        sel[i].y=i+4;
        sel[i].hotkey=((i<10)?('0'+i):('A'+i-10));
        sel[i].type=SIT_SELECT;
        sel[i].data=menustr[i];
        sprintf(menustr[i],"[%c] %-24s%-24s",sel[i].hotkey,secname[i][0],groups[i]);
        pts[i].x=sel[i].x;
        pts[i].y=sel[i].y;
    }
    sel[i].x=-1;sel[i].y=-1;sel[i].hotkey=-1;sel[i].type=0;sel[i].data=NULL;
    /*������ʾ��ǰ����*/
    if(!(pos<0)&&pos<SECNUM)
        sprintf(menustr[pos],"\033[1;36m[%c] %-24s%-24s\033[m",sel[pos].hotkey,secname[pos][0],groups[pos]);
    /*����select�ṹ*/
    arg.items=sel;
    arg.flag=SIF_SINGLE;
    bzero(&conf,sizeof(struct _select_def));
    conf.item_count=SECNUM;
    conf.item_per_page=18;
    conf.flag=LF_LOOP|LF_MULTIPAGE;
    conf.prompt="��";
    conf.item_pos=pts;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    /*��ʼλ��*/
    conf.pos=(!(pos<0)&&pos<SECNUM)?(pos+1):0;
    conf.on_select=editbrd_on_select;
    conf.show_data=editbrd_show;
    conf.key_command=editbrd_key;
    conf.show_title=editbrd_refresh;
    /*ѡ�����*/
    move(1,0);clrtobot();
    move(2,4);prints("\033[1;33m��ѡ�񾫻������ڷ���\033[m����\033[1;32mESC\033[0mȡ��");
    do
        i=list_select_loop(&conf);
    while(force&&i!=SHOW_SELECT);
    return i==SHOW_SELECT?conf.pos-1:-1;
}
int select_user_title(const char *name){
    struct _select_item sel[257];
    struct _select_def conf;
    struct _simple_select_arg arg;
    POINT pts[256];
    char menustr[256][USER_TITLE_LEN+10],*user_title;
    unsigned char title_buf[256];
    int i,pos;
    /*����˵���ʾ*/
    pos=0;
    sel[pos].x=4;sel[pos].y=4;sel[pos].hotkey=-1;sel[pos].type=SIT_SELECT;sel[pos].data=menustr[pos];
    sprintf(menustr[pos],"[ %3d ] ����ݶ���",pos);title_buf[pos]=0;
    pts[pos].x=sel[pos].x;pts[pos].y=sel[pos].y;
    for(pos++,i=0;i<255;i++){
        user_title=get_user_title(i+1);
        if(!*user_title)
            continue;
        if(name&&strcmp(user_title,name))
            continue;
        sel[pos].x=(pos%36<18?4:44);
        sel[pos].y=pos%18+4;
        sel[pos].hotkey=-1;
        sel[pos].type=SIT_SELECT;
        sel[pos].data=menustr[pos];
        sprintf(menustr[pos],"[ %3d ] <%s>",i+1,user_title);
        title_buf[pos]=i+1;
        pts[pos].x=sel[pos].x;
        pts[pos].y=sel[pos].y;
        pos++;
    }
    sel[pos].x=-1;sel[pos].y=-1;sel[pos].hotkey=-1;sel[pos].type=0;sel[pos].data=NULL;
    /*����select�ṹ*/
    arg.items=sel;
    arg.flag=SIF_SINGLE;
    bzero(&conf,sizeof(struct _select_def));
    conf.item_count=pos;
    conf.item_per_page=36;
    conf.flag=LF_LOOP|LF_MULTIPAGE;
    conf.prompt="��";
    conf.item_pos=pts;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    /*��ʼλ��*/
    conf.pos=(!name?1:2);
    conf.on_select=editbrd_on_select;
    conf.show_data=editbrd_show;
    conf.key_command=editbrd_key;
    conf.show_title=editbrd_refresh;
    /*ѡ���û����*/
    move(1,0);clrtobot();
    move(2,4);prints("\033[1;33m��ѡ���û����\033[m����\033[1;32mESC\033[0mȡ��");
    return (list_select_loop(&conf)==SHOW_SELECT?title_buf[conf.pos-1]:-1);
}
static char* strip_first(char *str){
    char *p,*s;
    if(!(s=strdup(str)))
        return NULL;
    if(!(p=strtok(s," "))){
        free(s);
        return NULL;
    }
    sprintf(str,"%s",p);
    free(s);
    return str;
}
static char* restrict_bm(char *retBM){
    char BM[BM_LEN],buf[EB_BUF_LEN],*p,*s,*tab[BM_LEN/2];
    int index,n;
    if(!*retBM)
        return retBM;
    snprintf(BM,BM_LEN,"%s",retBM);
    for(buf[0]=0,index=0,s=buf,p=strtok(BM," ");p;p=strtok(NULL," ")){
        for(n=(index-1);!(n<0);n--)
            if(!strcmp(p,tab[n]))
                break;
        if(!(n<0))
            continue;
        for(*s++=' ',tab[index++]=p;*p;*s++=*p++)
            continue;
    }
    *s++=0;
    snprintf(retBM,BM_LEN,"%s",&buf[1]);
    return retBM;
}
static int add_bm_to_BM_by_name(char *retBM,const char *name){
    char buf[EB_BUF_LEN];
    if(!*name)
        return 1;
    if(!*retBM){
        if(!(strlen(name)<BM_LEN))
            return 2;
        snprintf(retBM,BM_LEN,"%s",name);
        return 0;
    }
    snprintf(buf,EB_BUF_LEN,"%s %s",retBM,name);
    if(!(strlen(buf)<BM_LEN))
        return 3;
    snprintf(retBM,BM_LEN,"%s",buf);
    return 0;
}
static int del_bm_from_BM_by_index(char *retBM,int index){
    char BM[BM_LEN],buf[EB_BUF_LEN],*p;
    int n,l;
    if(!(index>0))
        return 1;
    snprintf(BM,BM_LEN,"%s",retBM);
    memcpy(buf,BM,BM_LEN*sizeof(char));
    for(p=strtok(BM," "),n=1;p&&(n<index);p=strtok(NULL," "),n++)
        continue;
    if(!p)
        return 2;
    l=strlen(p);
    p+=(buf-BM);
    if(!*(p+l))
        *(p-(p==buf?0:1))=0;
    else
        memmove(p,p+(l+1),(strlen(p)-l)*sizeof(char));
    snprintf(retBM,BM_LEN,"%s",buf);
    return 0;
}
static int del_bm_from_BM_by_name(char *retBM,const char *name){
    char BM[BM_LEN],buf[EB_BUF_LEN],*p,*tab[BM_LEN/2];
    int index,n,l;
    if(!*name)
        return 1;
    snprintf(BM,BM_LEN,"%s",retBM);
    memcpy(buf,BM,BM_LEN*sizeof(char));
    for(index=0,p=strtok(BM," ");p;p=strtok(NULL," "))
        if(!strcasecmp(p,name))
            tab[index++]=p;
    for(n=(index-1);!(n<0);n--){
        l=strlen(tab[n]);
        p=buf+(tab[n]-BM);
        if(!*(p+l))
            *(p-(p==buf?0:1))=0;
        else
            memmove(p,p+(l+1),(strlen(p)-l)*sizeof(char));
    }
    snprintf(retBM,BM_LEN,"%s",buf);
    return 0;
}
static int spmo_on_select(struct _select_def *conf){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    char ans[4];
    int i,new_index;
    void *p;
    move(arg->items[conf->pos-1].y,arg->items[conf->pos-1].x);clrtoeol();
    getdata(arg->items[conf->pos-1].y,arg->items[conf->pos-1].x,
        "\033[1;33m������˳���: \033[m",ans,3,DOECHO,NULL,true);
    strip_first(ans);
    if(!isdigit(ans[0]))
        return SHOW_REFRESH;
    if(!((new_index=atoi(ans))>0))
        new_index=1;
    if(new_index>conf->item_count)
        new_index=conf->item_count;
    if(new_index!=conf->pos){
        p=arg->items[conf->pos-1].data;
        if(new_index<conf->pos)
            for(i=conf->pos;i>new_index;i--)
                arg->items[i-1].data=arg->items[i-2].data;
        else
            for(p=arg->items[conf->pos-1].data,i=conf->pos;i<new_index;i++)
                arg->items[i-1].data=arg->items[i].data;
        arg->items[i-1].data=p;
        conf->pos=new_index;
        return SHOW_REFRESH;
    }
    return SHOW_REFRESH;
}

static int spmo_show(struct _select_def *conf,int i){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    char buf[EB_BUF_LEN];
    sprintf(buf,"\033[1;37m[\033[1;36m%02d\033[1;37m] \033[1;32m%s\033[m",
        i,(const char*)((arg->items[i-1]).data));
    outs(buf);
    return SHOW_CONTINUE;
}
static int spmo_key(struct _select_def *conf,int key){
    struct _simple_select_arg *arg=(struct _simple_select_arg*)conf->arg;
    int i;
    void *p;
    switch(key){
        case KEY_ESC:
        case KEY_CANCEL:
            return SHOW_QUIT;
        case '+':
            if(conf->pos!=1){
                p=arg->items[conf->pos-1].data;
                arg->items[conf->pos-1].data=arg->items[conf->pos-2].data;
                arg->items[conf->pos-2].data=p;
                conf->pos--;
                return SHOW_REFRESH;
            }
            break;
        case '-':
            if(conf->pos!=conf->item_count){
                p=arg->items[conf->pos-1].data;
                arg->items[conf->pos-1].data=arg->items[conf->pos].data;
                arg->items[conf->pos].data=p;
                conf->pos++;
                return SHOW_REFRESH;
            }
            break;
        case '*':
            if(conf->pos!=1){
                for(p=arg->items[conf->pos-1].data,i=conf->pos;i>1;i--)
                    arg->items[i-1].data=arg->items[i-2].data;
                arg->items[0].data=p;
                conf->pos=1;
                return SHOW_REFRESH;
            }
            break;
        case '/':
            if(conf->pos!=conf->item_count){
                for(p=arg->items[conf->pos-1].data,i=conf->pos;i<conf->item_count;i++)
                    arg->items[i-1].data=arg->items[i].data;
                arg->items[conf->item_count-1].data=p;
                conf->pos=conf->item_count;
                return SHOW_REFRESH;
            }
            break;
        default:
            break;
    }
    return SHOW_CONTINUE;
}
int select_process_bm_order(char *retBM){
    struct _select_item sel[BM_LEN/2+1];
    struct _select_def conf;
    struct _simple_select_arg arg;
    POINT pts[BM_LEN/2];
    char BM[BM_LEN],buf[EB_BUF_LEN],*p,*s,*id[BM_LEN/2];
    int n,count;
    snprintf(BM,BM_LEN,"%s",retBM);
    for(count=0,p=strtok(BM," ");p;p=strtok(NULL," "))
        id[count++]=p;
    if(!count)
        return 1;
    for(n=0;n<count;n++){
        sel[n].x=8;
        sel[n].y=n%12+11;
        sel[n].hotkey=-1;
        sel[n].type=SIT_SELECT;
        sel[n].data=id[n];
        pts[n].x=sel[n].x;
        pts[n].y=sel[n].y;
    }
    sel[n].x=-1;sel[n].y=-1;sel[n].hotkey=-1;sel[n].type=0;sel[n].data=NULL;
    arg.items=sel;
    arg.flag=SIF_SINGLE;
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=count;
    conf.item_per_page=12;
    conf.flag=LF_LOOP|LF_MULTIPAGE;
    conf.prompt="��";
    conf.item_pos=pts;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.pos=1;
    conf.on_select=spmo_on_select;
    conf.show_data=spmo_show;
    conf.key_command=spmo_key;
    conf.show_title=editbrd_refresh;
    list_select_loop(&conf);
    for(n=0,p=buf;n<count;n++)
        for(*p++=' ',s=sel[n].data;*s;*p++=*s++)
            continue;
    *p++=0;
    snprintf(retBM,BM_LEN,"%s",&buf[1]);
    return 0;
}
int select_process_bm_list(char *retBM){
    struct _select_item sel[4];
    struct _select_def conf;
    struct _simple_select_arg arg;
    POINT pts[3];
    char BM[BM_LEN],ans[BM_LEN];
    int no_save;
    snprintf(BM,BM_LEN,"%s",retBM);
    sel[0].x=4;sel[0].y=6;sel[0].hotkey='O';sel[0].type=SIT_SELECT;
    sel[0].data="\033[1;37m[\033[1;31mO\033[1;37m] �޸�������������Ա�б�˳�� "
        " \033[0;33m<Enter> \033[1;37m[[\033[1;32m+\033[1;37m][\033[1;32m-\033[1;37m]"
        "[\033[1;32m*\033[1;37m][\033[1;32m/\033[1;37m][\033[1;32mEnter\033[1;37m]]\033[m";
    sel[1].x=4;sel[1].y=7;sel[1].hotkey='R';sel[1].type=SIT_SELECT;
    sel[1].data="\033[1;37m[\033[1;31mR\033[1;37m] �޸�������������Ա�б����� "
        " \033[0;33m<Enter> \033[1;37m[[\033[1;32m+\033[1;37m][\033[1;32m-\033[1;37m]"
        "[\033[1;32m/\033[1;37m][\033[1;32m...\033[1;37m]]\033[m";
    sel[2].x=4;sel[2].y=8;sel[2].hotkey='Z';sel[2].type=SIT_SELECT;
    sel[2].data="\033[1;37m[\033[1;31mZ\033[1;37m] ����������������Ա�б�����  \033[0;33m<Enter>\033[1;37m []\033[m";
    sel[3].x=-1;sel[3].y=-1;sel[3].hotkey=-1;sel[3].type=0;sel[3].data=NULL;
    pts[0].x=sel[0].x;pts[0].y=sel[0].y;
    pts[1].x=sel[1].x;pts[1].y=sel[1].y;
    pts[2].x=sel[2].x;pts[2].y=sel[2].y;
    arg.items=sel;
    arg.flag=SIF_SINGLE;
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=3;
    conf.item_per_page=3;
    conf.prompt="��";
    conf.item_pos=pts;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.pos=1;
    conf.on_select=editbrd_on_select;
    conf.show_data=editbrd_show;
    conf.key_command=editbrd_key;
    conf.show_title=editbrd_refresh;
    no_save=0;
    while(1){
        move(1,0);clrtobot();
        move(2,4);outs("\033[1;33m��ѡ���������\033[1;37m [�� \033[1;32mESC\033[1;37m �������޸ķ���, "
            "�� \033[1;32m~\033[1;37m �������޸ķ���]\033[m");
        move(4,4);prints("\033[1;37m��ǰ�б�: [\033[1;32m%s\033[1;37m]\033[m",BM);
        conf.flag=LF_LOOP;
        if(list_select_loop(&conf)==SHOW_QUIT){
            no_save=(lastkey==KEY_CANCEL);
            break;
        }
        if(conf.pos==1)
            select_process_bm_order(BM);
        else if(conf.pos==2){
            move(sel[1].y,0);clrtoeol();
            if(getdata(sel[1].y,4,"\033[1;33m�����б�: \033[m",ans,BM_LEN,DOECHO,NULL,true)==-1)
                continue;
            switch(ans[0]){
                case 0:
                    break;
                case ' ':
                    BM[0]=0;
                    break;
                case '+':
                    strip_first(&ans[1]);
                    add_bm_to_BM_by_name(BM,&ans[1]);
                    break;
                case '-':
                    strip_first(&ans[1]);
                    del_bm_from_BM_by_name(BM,&ans[1]);
                    break;
                case '/':
                    strip_first(&ans[1]);
                    if(isdigit(ans[1]))
                        del_bm_from_BM_by_index(BM,atoi(&ans[1]));
                    break;
                default:
                    snprintf(BM,BM_LEN,"%s",ans);
                    break;
			}
        }
        else if(conf.pos==3)
            restrict_bm(BM);
        else
            break;
    }
    if(!no_save)
        snprintf(retBM,BM_LEN,"%s",BM);
    return 0;
}
/*�޸�����������ά��������*/
int m_editbrd(void){
    char buf[EB_BUF_LEN];
    int pos = 0, i;
    const struct boardheader *bhptr=NULL;
    
    /*���ϵͳ���벢�޸�״̬*/
    if(!check_systempasswd())
        return -1;
    modify_user_mode(ADMIN);
    clear();
    move(0,0);prints("\033[1;32m�޸�������˵�����趨\033[m");
    move(1,0);clrtobot();
    make_blist(0);
    
    in_do_sendmsg=1;
    i = namecomplete("����������������: ",buf);
    in_do_sendmsg=0;
    if(i=='#'){
        if(!HAS_PERM(getCurrentUser(),PERM_ADMIN)){
            move(2,0);prints("ʹ�þ�Ԯģʽ�޸�������������ҪADMINȨ��...");
            WAIT_RETURN;clear();
            return -1;
        }
        /*��Ԯģʽ*/
        getdata(2,0,"������������˳���(��������ֱ�ӻس�): ",buf,8,DOECHO,NULL,true);
        pos=atoi(buf);
        if(!pos){
            getdata(3,0,"����������������������: ",buf,128,DOECHO,NULL,true);
            if(!*buf){
                move(4,0);prints("ȡ��...");
                WAIT_RETURN;clear();
                return -1;
            }
            pos=getbid(buf,NULL);
            if(!pos){
                move(4,0);prints("���������������!");
                WAIT_RETURN;clear();
                return -1;
            }
        }
        else{
            bhptr=getboard(pos);
            if(!(bhptr&&bhptr->filename[0])){
                move(3,0);prints("�����������˳���!");
                WAIT_RETURN;clear();
                return -1;
            }
        }
    }
    else{
        /*����ģʽ*/
        if(!*buf){
            move(2,0);prints("ȡ��...");
            WAIT_RETURN;clear();
            return -1;
        }
        pos = getbnum_safe(buf,getSession());
        if(!pos){
            move(2,0);prints("���������������!");
            WAIT_RETURN;clear();
            return -1;
        }
    }
    return modify_board(pos);
}
int modify_board(int bid){
#define MB_ITEMS 24
    FILE *fp;
    struct _select_item sel[MB_ITEMS+1];
    struct _select_def conf;
    struct _simple_select_arg arg;
    POINT pts[MB_ITEMS];
    struct boardheader bh,newbh;
    char buf[EB_BUF_LEN],src[EB_BUF_LEN],dst[EB_BUF_LEN],menustr[MB_ITEMS][EB_BUF_LEN],orig[MB_ITEMS][EB_BUF_LEN],*ptr;
    int i,loop,section,currpos,ret;
    unsigned int annstat,change,error;
    const struct boardheader *bhptr=NULL;
    const char *menuldr[MB_ITEMS]={
        "[1]����������:","[2]����������:","[3]������˵��:","[4]����������:","[5]����������:",
        "[6]ת�ű�ǩ  :","[7]����������:","[8]����������:","[9]ͳ��������:","[A]ͳ��ʮ��  :",
        "[B]Ŀ¼������:","[C]����Ŀ¼  :","[D]����ת��  :","[E]�ϴ�����  :","[F]E-mail����:",
        "[G]���ɻظ�  :","[H]������Club:","[I]д����Club:","[J]����Club  :","[K]������λ��:",
        "[L]Ȩ������  :","[M]�������  :","[N]��������  :","[Q][�˳�]    :"
    };
    change=0;loop=1;
    /*ѡ��������*/
    clear();
    move(0,0);prints("\033[1;32m�޸�������˵�����趨\033[m");
    move(1,0);clrtobot();

    bhptr = getboard(bid);
    if(!bhptr){
        move(2,0);prints("���������������!");
        WAIT_RETURN;clear();
        return -1;
    }
    memcpy(&bh,bhptr,sizeof(struct boardheader));
    sprintf(buf,"\033[1;33m���������: %-4.4d\033[m",bid);
    move(0,40);prints(buf);
    if(bh.clubnum){
        sprintf(buf,"\033[1;33m���ֲ����: %-3.3d\033[m",bh.clubnum);
        move(0,60);prints(buf);
    }
    /*��ȡ���������ݲ�����˵���ʽ*/
    memcpy(&newbh,&bh,sizeof(struct boardheader));
    /*�˵���λ*/
    for(i=0;i<MB_ITEMS;i++){
        if(i<13){
            sel[i].x=2;
            sel[i].y=i+2;
        }
        else if(i<19){
            sel[i].x=42;
            sel[i].y=i-4;
        }
        else if(i==22){
            sel[i].x=42;
            sel[i].y=17;
        }
        else if(i==MB_ITEMS-1){
            sel[i].x=2;
            sel[i].y=18;
        }
        else{
            sel[i].x=2;
            sel[i].y=i-4;
        }
        sel[i].type=SIT_SELECT;
        sel[i].data=menustr[i];
        pts[i].x=sel[i].x;
        pts[i].y=sel[i].y;
    }
    /*�˵�����*/
    /*����������*/
    sel[0].hotkey='1';
    sprintf(menustr[0],"%-15s%s",menuldr[0],bh.filename);
    /*����������*/
    sel[1].hotkey='2';
    sprintf(menustr[1],"%-15s%s",menuldr[1],bh.BM);
    /*������˵��*/
    sel[2].hotkey='3';
    sprintf(menustr[2],"%-15s%s",menuldr[2],&bh.title[13]);
    /*����������*/
    sel[3].hotkey='4';
    sprintf(menustr[3],"%-15s<%c>",menuldr[3],bh.title[0]);
    /*����������*/
    sel[4].hotkey='5';
    sprintf(menustr[4],"%-15s<%-6.6s>",menuldr[4],&bh.title[1]);
    /*ת�ű�ǩ*/
    sel[5].hotkey='6';
    sprintf(menustr[5],"%-15s<%-6.6s>",menuldr[5],&bh.title[7]);
    /*����������*/
    sel[6].hotkey='7';
    sprintf(buf,"%s",bh.des);
    for(ptr=&buf[0];*ptr;ptr++)
        if(*ptr==10)
            *ptr=32;
    sprintf(menustr[6],"%-15s%s",menuldr[6],buf[0]?buf:"<��>");
    if(strlen(menustr[6])>76)
        sprintf(&menustr[6][73],"...");
    /*����������*/
    sel[7].hotkey='8';
    sprintf(menustr[7],"%-15s%s",menuldr[7],(bh.flag&BOARD_ANNONY)?"��":"��");
    /*ͳ��������*/
    sel[8].hotkey='9';
    sprintf(menustr[8],"%-15s%s",menuldr[8],(bh.flag&BOARD_JUNK)?"��":"��");
    /*ͳ��ʮ��*/
    sel[9].hotkey='A';
    sprintf(menustr[9],"%-15s%s",menuldr[9],(bh.flag&BOARD_POSTSTAT)?"��":"��");
    /*Ŀ¼������*/
    sel[10].hotkey='B';
    sprintf(menustr[10],"%-15s%s",menuldr[10],(bh.flag&BOARD_GROUP)?"��":"��");
    /*����Ŀ¼*/
    sel[11].hotkey='C';
    sprintf(menustr[11],"%-15s%s",menuldr[11],
        bh.group?(!(bhptr=getboard(bh.group))?"�쳣":bhptr->filename):"��");
    /*����ת��*/
    sel[12].hotkey='D';
    sprintf(menustr[12],"%-15s%s",menuldr[12],(bh.flag&BOARD_OUTFLAG)?"��":"��");
    /*�ϴ�����*/
    sel[13].hotkey='E';
    sprintf(menustr[13],"%-15s%s",menuldr[13],(bh.flag&BOARD_ATTACH)?"��":"��");
    /*E-mail����*/
    sel[14].hotkey='F';
    sprintf(menustr[14],"%-15s%s",menuldr[14],(bh.flag&BOARD_EMAILPOST)?"��":"��");
    /*���ɻظ�*/
    sel[15].hotkey='G';
    sprintf(menustr[15],"%-15s%s",menuldr[15],(bh.flag&BOARD_NOREPLY)?"��":"��");
    /*������Club*/
    sel[16].hotkey='H';
    sprintf(menustr[16],"%-15s%s",menuldr[16],(bh.flag&BOARD_CLUB_READ)?"��":"��");
    /*д����Club*/
    sel[17].hotkey='I';
    sprintf(menustr[17],"%-15s%s",menuldr[17],(bh.flag&BOARD_CLUB_WRITE)?"��":"��");
    /*����Club*/
    sel[18].hotkey='J';
    sprintf(menustr[18],"%-15s%s",menuldr[18],
        (bh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((bh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
    /*������λ��*/
    sel[19].hotkey='K';
    annstat=check_ann(&bh);
    section=(annstat&0x020000)?-1:(annstat&0xFFFF);
    sprintf(menustr[19],"%-15s%s <%s>",menuldr[19],
        !(annstat&~0xFFFF)?"��Ч":(annstat&0x040000?"��Ч":"�쳣"),bh.ann_path);
    if(strlen(menustr[19])>76)
        sprintf(&menustr[19][72],"...>");
    /*Ȩ������*/
    sel[20].hotkey='L';
    sprintf(menustr[20],"%-15s%s <%s>",menuldr[20],
        (bh.level&~PERM_POSTMASK)?((bh.level&PERM_POSTMASK)?"��������":"��ȡ����"):"������",
        gen_permstr(bh.level,buf));
    /*�������*/
    sel[21].hotkey='M';
    sprintf(menustr[21],"%-15s%s <%d>",menuldr[21],
        bh.title_level?get_user_title(bh.title_level):"������",(unsigned char)bh.title_level);
    /*��������*/
    sel[22].hotkey='N';
#ifdef NEWSMTH
    sprintf(menustr[22],"%-15s%s <%d>",menuldr[22],(bh.score_level?"��������":"������"),bh.score_level);
#else /* NEWSMTH */
    sprintf(menustr[22],"%-15s%s <%d>",menuldr[22],"��Чѡ��",bh.score_level);
#endif /* NEWSMTH */
    /*�˳�*/
    sel[MB_ITEMS-1].hotkey='Q';
    sprintf(menustr[MB_ITEMS-1],"%-15s%s",menuldr[MB_ITEMS-1],change?"\033[1;31m���޸�\033[m":"δ�޸�");
    sel[MB_ITEMS].x=-1;sel[MB_ITEMS].y=-1;sel[MB_ITEMS].type=0;sel[MB_ITEMS].hotkey=-1;sel[MB_ITEMS].data=NULL;
    /*����*/
    memcpy(orig,menustr,MB_ITEMS*EB_BUF_LEN*sizeof(char));
    currpos=MB_ITEMS;
    /*�޸İ�������*/
    while(loop){
        move(1,0);
        clrtobot();
        move(23,0);
        prints("  ����ָ�ԭ�趨[ѡ��ǰ��\033[1;32m~\033[m]    ɾ���趨[\033[1;32m����ո�\033[m]    ȡ��[\033[1;32mESC\033[m]");
        /*����select�ṹ*/
        arg.items=sel;
        arg.flag=SIF_SINGLE;
        bzero(&conf,sizeof(struct _select_def));
        conf.item_count=MB_ITEMS;
        conf.item_per_page=MB_ITEMS;
        conf.flag=LF_LOOP;
        conf.prompt="��";
        conf.item_pos=pts;
        conf.arg=&arg;
        conf.title_pos.x=-1;
        conf.title_pos.y=-1;
        /*��ǰλ��*/
        conf.pos=currpos;
        conf.on_select=editbrd_on_select;
        conf.show_data=editbrd_show;
        conf.key_command=editbrd_key;
        /*ѡ��*/
        sprintf(menustr[MB_ITEMS-1],"%-15s%s",menuldr[MB_ITEMS-1],
            change?"\033[1;31m���޸�\033[m (�� \033[1;32m~\033[m �ָ��������趨)":"δ�޸�");
        ret=list_select_loop(&conf);
        currpos=conf.pos;
        /*����SHOW_QUITʱ*/
        if(ret==SHOW_QUIT){
            /*ȡ�������޸�*/
            if(lastkey==KEY_CANCEL&&((change&(1<<(currpos-1)))||currpos==MB_ITEMS)){
                switch(currpos-1){
                    /*���������ƻ򾫻���λ��*/
                    case 0:
                    case 19:
                        sprintf(newbh.filename,"%s",bh.filename);
                        sprintf(newbh.ann_path,"%s",bh.ann_path);
                        sprintf(menustr[0],"%s",orig[0]);
                        sprintf(menustr[19],"%s",orig[19]);
                        section=(annstat&0x020000)?-1:(annstat&0xFFFF);
                        change&=~(1<<0);
                        change&=~(1<<19);
                        break;
                    /*����������*/
                    case 1:
                        sprintf(newbh.BM,"%s",bh.BM);
                        sprintf(menustr[1],"%s",orig[1]);
                        change&=~(1<<1);
                        break;
                    /*������˵��*/
                    case 2:
                        sprintf(&newbh.title[13],"%s",&bh.title[13]);
                        sprintf(menustr[2],"%s",orig[2]);
                        change&=~(1<<2);
                        break;
                    /*����������*/
                    case 3:
                        newbh.title[0]=bh.title[0];
                        sprintf(menustr[3],"%s",orig[3]);
                        change&=~(1<<3);
                        break;
                    /*����������*/
                    case 4:
                        memcpy(&newbh.title[1],&bh.title[1],6);
                        sprintf(menustr[4],"%s",orig[4]);
                        change&=~(1<<4);
                        break;
                    /*ת�ű�ǩ*/
                    case 5:
                        memcpy(&newbh.title[7],&bh.title[7],6);
                        sprintf(menustr[5],"%s",orig[5]);
                        change&=~(1<<5);
                        break;
                    /*����������*/
                    case 6:
                        sprintf(newbh.des,"%s",bh.des);
                        sprintf(menustr[6],"%s",orig[6]);
                        change&=~(1<<6);
                        break;
                    /*����Ŀ¼*/
                    case 11:
                        newbh.group=bh.group;
                        sprintf(menustr[11],"%s",orig[11]);
                        change&=~(1<<11);
                        break;
                    /*Ȩ������*/
                    case 20:
                        newbh.level=bh.level;
                        sprintf(menustr[20],"%s",orig[20]);
                        change&=~(1<<20);
                        break;
                    /*�������*/
                    case 21:
                        newbh.title_level=bh.title_level;
                        sprintf(menustr[21],"%s",orig[21]);
                        change&=~(1<<21);
                        break;
                    /*��������*/
                    case 22:
#ifdef NEWSMTH
                        newbh.score_level=bh.score_level;
                        sprintf(menustr[22],"%s",orig[22]);
                        change&=~(1<<22);
#endif /* NEWSMTH */
                        break;
                    /*ȫ������*/
                    case MB_ITEMS-1:
                        memcpy(&newbh,&bh,sizeof(struct boardheader));
                        memcpy(menustr,orig,MB_ITEMS*EB_BUF_LEN*sizeof(char));
                        section=(annstat&0x020000)?-1:(annstat&0xFFFF);
                        change=0;
                        break;
                    default:
                        break;
                }
            }
            /*�����޸��˳�*/
            if(lastkey==KEY_ESC){
                if(change){
                    move(20,0);clrtoeol();
                    getdata(20,2,"\033[1;31m�����޸��˳�? [N]: \033[m",buf,2,DOECHO,NULL,true);
                    if(buf[0]!='y'&&buf[0]!='Y')
                        continue;
                }
                return -1;
            }
            continue;
        }
        /*����SHOW_SELECTʱ*/
        switch(currpos-1){
            /*����������*/
            case 0:
                move(2,0);clrtoeol();getdata(2,2,"����������������: ",buf,BOARDNAMELEN,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf||!strcmp(buf,newbh.filename))
                    break;
                /*Ŀ���������Ѿ�����*/
                if(strcasecmp(buf,bh.filename)&&getbid(buf,NULL)>0){
                    move(2,0);clrtoeol();getdata(2,2,"\033[1;31m����: ���������Ѿ�����!\033[m",buf,1,NOECHO,NULL,true);
                    break;
                }
                /*�������������ƺ��зǷ��ַ�*/
                if(strchr(buf,'/')||strchr(buf,' ')){
                    move(2,0);clrtoeol();getdata(2,2,"\033[1;31m����: �����������к��зǷ��ַ�!\033[m",buf,1,NOECHO,NULL,true);
                    break;
                }
                sprintf(newbh.filename,"%s",buf);
                /*����޸�״̬*/
                if(strcmp(bh.filename,newbh.filename)){
                    sprintf(menustr[0],"%-15s\033[1;32m%s\033[m",menuldr[0],newbh.filename);
                    change|=(1<<0);
                }
                else{
                    sprintf(menustr[0],"%s",orig[0]);
                    change&=~(1<<0);
                }
                /*�޸ľ�����λ��*/
                if((annstat&0x020000)&&!strcmp(bh.ann_path,newbh.ann_path))
                    section=select_group(section,1);
                sprintf(newbh.ann_path,"%s/%s",groups[section],newbh.filename);
                newbh.ann_path[127]='\0';
                /*����޸�״̬*/
                if(strcmp(bh.ann_path,newbh.ann_path)){
                    sprintf(menustr[19],"%-15s\033[1;32m%s\033[m <%s>",menuldr[19],
                        (annstat&0x020000)?"����":"����",newbh.ann_path);
                    if(strlen(menustr[19])>86)
                        sprintf(&menustr[19][82],"...>");
                    change|=(1<<19);
                }
                else{
                    sprintf(menustr[19],"%s",orig[19]);
                    change&=~(1<<19);
                }
                break;
            /*����������*/
            case 1:
                //snprintf(newbh.BM,BM_LEN,"%s",bh.BM);
                select_process_bm_list(newbh.BM);
                /*����޸�״̬*/
                if(strcmp(bh.BM,newbh.BM)){
                    sprintf(menustr[1],"%-15s\033[1;32m%s\033[m",menuldr[1],newbh.BM);
                    change|=(1<<1);
                }
                else{
                    sprintf(menustr[1],"%s",orig[1]);
                    change&=~(1<<1);
                }
                break;
            /*������˵��*/
            case 2:
                move(4,0);clrtoeol();getdata(4,2,"������������˵��: ",buf,48,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                if(*buf==' ')
                    newbh.title[13]='\0';
                else
                    sprintf(&newbh.title[13],"%s",buf);
                /*����޸�״̬*/
                if(strcmp(&bh.title[13],&newbh.title[13])){
                    sprintf(menustr[2],"%-15s\033[1;32m%s\033[m",menuldr[2],&newbh.title[13]);
                    change|=(1<<2);
                }
                else{
                    sprintf(menustr[2],"%s",orig[2]);
                    change&=~(1<<2);
                }
                break;
            /*����������*/
            case 3:
                move(5,0);clrtoeol();getdata(5,2,"����������������(1�ַ�����): ",buf,2,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                newbh.title[0]=buf[0];
                /*����޸�״̬*/
                if(bh.title[0]!=newbh.title[0]){
                    sprintf(menustr[3],"%-15s\033[1;32m<%c>\033[m",menuldr[3],newbh.title[0]);
                    change|=(1<<3);
                }
                else{
                    sprintf(menustr[3],"%s",orig[3]);
                    change&=~(1<<3);
                }
                break;
            /*����������*/
            case 4:
                move(6,0);clrtoeol();getdata(6,2,"����������������(4�ַ�����): ",buf,5,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                /*���Ȳ���ʱ����*/
                if((i=strlen(buf))<4)
                    while(i!=4)
                        buf[i++]=' ';
                newbh.title[1]='[';newbh.title[6]=']';memcpy(&newbh.title[2],buf,4);
                /*����޸�״̬*/
                if(strncmp(&bh.title[1],&newbh.title[1],6)){
                    sprintf(menustr[4],"%-15s\033[1;32m<%-6.6s>\033[m",menuldr[4],&newbh.title[1]);
                    change|=(1<<4);
                }
                else{
                    sprintf(menustr[4],"%s",orig[4]);
                    change&=~(1<<4);
                }
                break;
            /*ת�ű�ǩ*/
            case 5:
                move(7,0);clrtoeol();getdata(7,2,"������ת�ű�ǩ(6�ַ�����;<#1>˫��ת��,<#2>����ת��): ",buf,7,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                /*Ԥ����ת�ű��*/
                if(buf[0]=='#'){
                    switch(buf[1]){
                        /*˫��ת�ű��*/
                        case '1':
                            sprintf(buf," ��   ");
                            break;
                        /*����ת�ű��*/
                        case '2':
                            sprintf(buf," ��   ");
                            break;
                        /*��ת�ű��*/
                        default:
                            sprintf(buf,"      ");
                            break;
                    }
                }
                /*���Ȳ���ʱ����*/
                if((i=strlen(buf))<6)
                    while(i!=6)
                        buf[i++]=' ';
                memcpy(&newbh.title[7],buf,6);
                /*����޸�״̬*/
                if(strncmp(&bh.title[7],&newbh.title[7],6)){
                    sprintf(menustr[5],"%-15s\033[1;32m<%-6.6s>\033[m",menuldr[5],&newbh.title[7]);
                    change|=(1<<5);
                }
                else{
                    sprintf(menustr[5],"%s",orig[5]);
                    change&=~(1<<5);
                }
                break;
            /*����������*/
            case 6:
                move(1,0);clrtobot();sprintf(buf,"%s",newbh.des);
                /*��������*/
                multi_getdata(8,0,72,"����������������: \n",buf,195,8,false,0);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                if(*buf==' ')
                    buf[0]=0;
                else
                    for(ptr=&buf[0];*ptr;ptr++)
                        if(*ptr==10)
                            *ptr=32;
                sprintf(newbh.des,"%s",buf);
                /*����޸�״̬*/
                if(strcmp(bh.des,newbh.des)){
                    sprintf(menustr[6],"%-15s\033[1;32m%s\033[m",menuldr[6],newbh.des[0]?newbh.des:"<��>");
                    if(strlen(menustr[6])>86)
                        sprintf(&menustr[6][80],"...\033[m");
                    change|=(1<<6);
                }
                else{
                    sprintf(menustr[6],"%s",orig[6]);
                    change&=~(1<<6);
                }
                break;
            /*����������*/
            case 7:
                newbh.flag^=BOARD_ANNONY;
                /*����޸�״̬*/
                if((bh.flag&BOARD_ANNONY)^(newbh.flag&BOARD_ANNONY)){
                    sprintf(menustr[7],"%-15s\033[1;32m%s\033[m",menuldr[7],(newbh.flag&BOARD_ANNONY)?"��":"��");
                    change|=(1<<7);
                }
                else{
                    sprintf(menustr[7],"%s",orig[7]);
                    change&=~(1<<7);
                }
                break;
            /*ͳ��������*/
            case 8:
                newbh.flag^=BOARD_JUNK;
                /*����޸�״̬*/
                if((bh.flag&BOARD_JUNK)^(newbh.flag&BOARD_JUNK)){
                    sprintf(menustr[8],"%-15s\033[1;32m%s\033[m",menuldr[8],(newbh.flag&BOARD_JUNK)?"��":"��");
                    change|=(1<<8);
                }
                else{
                    sprintf(menustr[8],"%s",orig[8]);
                    change&=~(1<<8);
                }
                break;
            /*ͳ��ʮ��*/
            case 9:
                newbh.flag^=BOARD_POSTSTAT;
                /*����޸�״̬*/
                if((bh.flag&BOARD_POSTSTAT)^(newbh.flag&BOARD_POSTSTAT)){
                    sprintf(menustr[9],"%-15s\033[1;32m%s\033[m",menuldr[9],(newbh.flag&BOARD_POSTSTAT)?"��":"��");
                    change|=(1<<9);
                }
                else{
                    sprintf(menustr[9],"%s",orig[9]);
                    change&=~(1<<9);
                }
                break;
            /*Ŀ¼������*/
            case 10:
                newbh.flag^=BOARD_GROUP;
                /*����޸�״̬*/
                if((bh.flag&BOARD_GROUP)^(newbh.flag&BOARD_GROUP)){
                    sprintf(menustr[10],"%-15s\033[1;32m%s\033[m",menuldr[10],(newbh.flag&BOARD_GROUP)?"��":"��");
                    change|=(1<<10);
                }
                else{
                    sprintf(menustr[10],"%s",orig[10]);
                    change&=~(1<<10);
                }
                break;
            /*����Ŀ¼*/
            case 11:
                move(13,0);clrtoeol();getdata(13,2,"����������Ŀ¼: ",buf,BOARDNAMELEN,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                if(*buf==' ')
                    newbh.group=0;
                else{
                    i=getbnum_safe(buf,getSession());
                    if(!i){
                        move(13,0);clrtoeol();getdata(13,2,"\033[1;31m����: �����������������!\033[m",buf,1,NOECHO,NULL,true);
                        break;
                    }
                    else if(!(getboard(i)->flag&BOARD_GROUP)){
                        move(13,0);clrtoeol();getdata(13,2,"\033[1;31m����: ���������������Ŀ¼!\033[m",buf,1,NOECHO,NULL,true);
                        break;
                    }
                    else
                        newbh.group=i;
                }
                /*����޸�״̬*/
                if(bh.group!=newbh.group){
                    sprintf(menustr[11],"%-15s\033[1;32m%s\033[m",menuldr[11],
                        newbh.group?(!(bhptr=getboard(newbh.group))?"�쳣":bhptr->filename):"��");
                    change|=(1<<11);
                }
                else{
                    sprintf(menustr[11],"%s",orig[11]);
                    change&=~(1<<11);
                }
                break;
            /*����ת��*/
            case 12:
                newbh.flag^=BOARD_OUTFLAG;
                /*����޸�״̬*/
                if((bh.flag&BOARD_OUTFLAG)^(newbh.flag&BOARD_OUTFLAG)){
                    sprintf(menustr[12],"%-15s\033[1;32m%s\033[m",menuldr[12],(newbh.flag&BOARD_OUTFLAG)?"��":"��");
                    change|=(1<<12);
                }
                else{
                    sprintf(menustr[12],"%s",orig[12]);
                    change&=~(1<<12);
                }
                break;
            /*�ϴ�����*/
            case 13:
                newbh.flag^=BOARD_ATTACH;
                /*����޸�״̬*/
                if((bh.flag&BOARD_ATTACH)^(newbh.flag&BOARD_ATTACH)){
                    sprintf(menustr[13],"%-15s\033[1;32m%s\033[m",menuldr[13],(newbh.flag&BOARD_ATTACH)?"��":"��");
                    change|=(1<<13);
                }
                else{
                    sprintf(menustr[13],"%s",orig[13]);
                    change&=~(1<<13);
                }
                break;
            /*E-mail����*/
            case 14:
                newbh.flag^=BOARD_EMAILPOST;
                /*����޸�״̬*/
                if((bh.flag&BOARD_EMAILPOST)^(newbh.flag&BOARD_EMAILPOST)){
                    sprintf(menustr[14],"%-15s\033[1;32m%s\033[m",menuldr[14],(newbh.flag&BOARD_EMAILPOST)?"��":"��");
                    change|=(1<<14);
                }
                else{
                    sprintf(menustr[14],"%s",orig[14]);
                    change&=~(1<<14);
                }
                break;
            /*���ɻظ�*/
            case 15:
                newbh.flag^=BOARD_NOREPLY;
                /*����޸�״̬*/
                if((bh.flag&BOARD_NOREPLY)^(newbh.flag&BOARD_NOREPLY)){
                    sprintf(menustr[15],"%-15s\033[1;32m%s\033[m",menuldr[15],(newbh.flag&BOARD_NOREPLY)?"��":"��");
                    change|=(1<<15);
                }
                else{
                    sprintf(menustr[15],"%s",orig[15]);
                    change&=~(1<<15);
                }
                break;
            /*������Club*/
            case 16:
                newbh.flag^=BOARD_CLUB_READ;
                /*����޸�״̬*/
                if((bh.flag&BOARD_CLUB_READ)^(newbh.flag&BOARD_CLUB_READ)){
                    sprintf(menustr[16],"%-15s\033[1;32m%s\033[m",menuldr[16],(newbh.flag&BOARD_CLUB_READ)?"��":"��");
                    change|=(1<<16);
                }
                else{
                    sprintf(menustr[16],"%s",orig[16]);
                    change&=~(1<<16);
                }
                /*�Ǿ��ֲ�ʱȡ�����ؾ��ֲ���ǩ*/
                if(!(newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE)))
                    newbh.flag&=~BOARD_CLUB_HIDE;
                /*����޸�״̬*/
                if((bh.flag&BOARD_CLUB_HIDE)^(newbh.flag&BOARD_CLUB_HIDE)){
                    sprintf(menustr[18],"%-15s\033[1;32m%s\033[m",menuldr[18],
                        (newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((newbh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
                    change|=(1<<18);
                }
                else{
                    sprintf(menustr[18],"%-15s%s",menuldr[18],
                        (newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((newbh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
                    change&=~(1<<18);
                }
                break;
            /*д����Club*/
            case 17:
                newbh.flag^=BOARD_CLUB_WRITE;
                /*����޸�״̬*/
                if((bh.flag&BOARD_CLUB_WRITE)^(newbh.flag&BOARD_CLUB_WRITE)){
                    sprintf(menustr[17],"%-15s\033[1;32m%s\033[m",menuldr[17],(newbh.flag&BOARD_CLUB_WRITE)?"��":"��");
                    change|=(1<<17);
                }
                else{
                    sprintf(menustr[17],"%s",orig[17]);
                    change&=~(1<<17);
                }
                /*�Ǿ��ֲ�ʱȡ�����ؾ��ֲ���ǩ*/
                if(!(newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE)))
                    newbh.flag&=~BOARD_CLUB_HIDE;
                /*����޸�״̬*/
                if((bh.flag&BOARD_CLUB_HIDE)^(newbh.flag&BOARD_CLUB_HIDE)){
                    sprintf(menustr[18],"%-15s\033[1;32m%s\033[m",menuldr[18],
                        (newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((newbh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
                    change|=(1<<18);
                }
                else{
                    sprintf(menustr[18],"%-15s%s",menuldr[18],
                        (newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((newbh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
                    change&=~(1<<18);
                }
                break;
            /*����Club*/
            case 18:
                /*�Ǿ��ֲ�*/
                if(!(newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE)))
                    break;
                newbh.flag^=BOARD_CLUB_HIDE;
                /*����޸�״̬*/
                if((bh.flag&BOARD_CLUB_HIDE)^(newbh.flag&BOARD_CLUB_HIDE)){
                    sprintf(menustr[18],"%-15s\033[1;32m%s\033[m",menuldr[18],(newbh.flag&BOARD_CLUB_HIDE)?"��":"��");
                    change|=(1<<18);
                }
                else{
                    sprintf(menustr[18],"%-15s%s",menuldr[18],
                        (newbh.flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))?((newbh.flag&BOARD_CLUB_HIDE)?"��":"��"):"��Чѡ��");
                    change&=~(1<<18);
                }
                break;
            /*������λ��*/
            case 19:
                if((i=select_group(section,0))==-1)
                    break;
                else
                    section=i;
                sprintf(newbh.ann_path,"%s/%s",groups[section],newbh.filename);
                newbh.ann_path[127]='\0';
                /*����޸�״̬*/
                if(strcmp(bh.ann_path,newbh.ann_path)){
                    sprintf(menustr[19],"%-15s\033[1;32m%s\033[m <%s>",menuldr[19],
                        (annstat&0x020000)?"����":"����",newbh.ann_path);
                    if(strlen(menustr[19])>86)
                        sprintf(&menustr[19][82],"...>");
                    change|=(1<<19);
                }
                else{
                    sprintf(menustr[19],"%s",orig[19]);
                    change&=~(1<<19);
                }
                break;
            /*Ȩ������*/
            case 20:
                move(16,0);clrtoeol();getdata(16,2,"�趨{��ȡ(R)|����(P)}Ȩ�����ƻ�����趨(C): ",buf,2,DOECHO,NULL,true);
                i=0;
                switch(buf[0]){
                    case 'r':
                    case 'R':
                        newbh.level&=~PERM_POSTMASK;
                        break;
                    case 'p':
                    case 'P':
                        newbh.level|=PERM_POSTMASK;
                        break;
                    case 0:
                        break;
                    default:
                        i=1;
                        break;
                }
                /*ȡ���޸�*/
                if(i)
                    break;
                move(1,0);clrtobot();
                move(2,0);prints("�趨%sȨ��",newbh.level&PERM_POSTMASK?"����":"��ȡ");
                newbh.level=setperms(newbh.level,0,"Ȩ��",NUMPERMS,showperminfo,NULL);
                /*����޸�״̬*/
                if(bh.level!=newbh.level){
                    sprintf(menustr[20],"%-15s\033[1;32m%s\033[m <%s>",menuldr[20],
                        (newbh.level&~PERM_POSTMASK)?((newbh.level&PERM_POSTMASK)?"��������":"��ȡ����"):"������",
                        gen_permstr(newbh.level,buf));
                    change|=(1<<20);
                }
                else{
                    sprintf(menustr[20],"%s",orig[20]);
                    change&=~(1<<20);
                }
                break;
            /*�������*/
            case 21:
                move(17,0);clrtoeol();getdata(17,2,"�趨�������{(ְ��)|(#���)|(@)}: ",buf,USER_TITLE_LEN,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                if(buf[0]=='#'){
                    i=atoi(&buf[1]);
                    sprintf(&buf[128],"%d",i);
                    if(i>255||strcmp(&buf[1],&buf[128])){
                        move(17,0);clrtoeol();getdata(17,2,"\033[1;31m����: �������Խ���Ƿ�!\033[m",buf,1,NOECHO,NULL,true);
                        break;
                    }
                    if(i&&!*get_user_title(i)){
                        move(17,0);clrtoeol();
                        getdata(17,2,"\033[1;33m��ʾ: Ŀǰ�����������Ӧ���û���ݲ�����,{ȷ��(Y)|ȡ��(N)}? [N]: \033[m",
                            buf,2,DOECHO,NULL,true);
                        if(buf[0]!='y'&&buf[0]!='Y')
                            break;
                    }
                }
                else if(buf[0]=='@'){
                    i=select_user_title(NULL);
                    if(i==-1)
                        break;
                }
                else{
                    if(buf[0]==' '&&!buf[1])
                        i=0;
                    else{
                        unsigned char count,first;
                        for(count=0,first=0,i=0;i<255;i++)
                            if(!strcmp(get_user_title(i+1),buf)&&!count++)
                                first=i+1;
                        if(!count){
                            move(17,0);clrtoeol();getdata(17,2,"\033[1;31m����: Ŀǰ��δ���ƴ��û����!\033[m",
                                buf,1,NOECHO,NULL,true);
                            break;
                        }
                        i=(count==1?first:select_user_title(buf));
                        if(i==-1)
                            break;
                    }
                }
                newbh.title_level=i;
                /*����޸�״̬*/
                if(bh.title_level!=newbh.title_level){
                    sprintf(menustr[21],"%-15s\033[1;32m%s\033[m <%d>",menuldr[21],
                        newbh.title_level?get_user_title(newbh.title_level):"������",(unsigned char)newbh.title_level);
                    change|=(1<<21);
                }
                else{
                    sprintf(menustr[21],"%s",orig[21]);
                    change&=~(1<<21);
                }
                break;
            /*��������*/
            case 22:
#ifdef NEWSMTH
                move(17,42);clrtoeol();getdata(17,42,"�������������: ",buf,8,DOECHO,NULL,true);
                /*ȡ���޸�*/
                if(!*buf)
                    break;
                trimstr(buf);
                if(!buf[0]){
                    newbh.score_level=0;
                }
                else{
                    for(i=0;buf[i];i++)
                        if(!isdigit(buf[i]))
                            break;
                    if(buf[i]){
                        move(17,42);clrtoeol();getdata(17,42,"\033[1;31m����: �Ƿ��Ļ���ֵ!\033[m",buf,1,DOECHO,NULL,true);
                        break;
                    }
                    newbh.score_level=atoi(buf);
                }
                /*����޸�״̬*/
                if(newbh.score_level!=bh.score_level){
                    sprintf(menustr[22],"%-15s\033[1;32m%s\033[m <%d>",menuldr[22],
                        (newbh.score_level?"��������":"������"),newbh.score_level);
                    change|=(1<<22);
                }
                else{
                    sprintf(menustr[22],"%s",orig[22]);
                    change&=~(1<<22);
                }
#endif /* NEWSMTH */
                break;
            /*�˳�*/
            case MB_ITEMS-1:
                if(change){
                    /*��ͻ��⼰ȷ��*/
                    if(change&(1<<0)){
                        sprintf(src,"boards/%s",bh.filename);
                        sprintf(dst,"boards/%s",newbh.filename);
                        if(!dashd(src)){
                            move(20,0);clrtoeol();
                            getdata(20,2,"\033[1;36mȷ��: Դ������Ŀ¼������,�Ƿ񴴽�? [Y]: \033[m",buf,2,DOECHO,NULL,true);
                            if(buf[0]=='n'||buf[0]=='N')
                                break;
                        }
                        if(dashd(dst)){
                            move(20,0);clrtoeol();
                            getdata(20,2,"\033[1;36mȷ��: Ŀ��������Ŀ¼�Ѵ���,�Ƿ񸲸�? [Y]: \033[m",buf,2,DOECHO,NULL,true);
                            if(buf[0]=='n'||buf[0]=='N')
                                break;
                        }
                    }
                    if(change&(1<<19)){
                        sprintf(dst,"0Announce/groups/%s",newbh.ann_path);
                        if(dashd(dst)){
                            move(20,0);clrtoeol();
                            getdata(20,2,"\033[1;36mȷ��: Ŀ�ľ�����Ŀ¼�Ѵ���,�Ƿ񸲸�? [Y]: \033[m",buf,2,DOECHO,NULL,true);
                            if(buf[0]=='n'||buf[0]=='N')
                                break;
                        }
                    }
                    move(20,0);clrtoeol();
                    getdata(20,2,"\033[1;31mȷ���޸�����������? [N]: \033[m",buf,2,DOECHO,NULL,true);
                    if(buf[0]!='y'&&buf[0]!='Y')
                        break;
                    loop=0;
                }
                else{
                    clear();
                    return -1;
                }
                break;
            default:
                break;
        }
    }
    /*ִ���޸Ĳ���*/
    error=0;
    if(change&(1<<0)){
        sprintf(src,"boards/%s",bh.filename);
        sprintf(dst,"boards/%s",newbh.filename);
        if(dashd(dst))
            error|=my_f_rm(dst);
        if(dashd(src))
            error|=rename(src,dst);
        else{
            error|=mkdir(dst,0755);
            build_board_structure(newbh.filename);
        }
        sprintf(src,"vote/%s",bh.filename);
        sprintf(dst,"vote/%s",newbh.filename);
        if(dashd(dst))
            my_f_rm(dst);
        if(dashd(src))
            rename(src,dst);
    }
    error|=edit_group(&bh,&newbh);
    set_board(bid,&newbh,&bh);
    /*���ɰ�ȫ��˺���־*/
    sprintf(src,"tmp/edit_board_log_%ld_%d",time(NULL),(int)getpid());
    if(!(fp=fopen(src,"w"))){
        sprintf(buf,"�޸�������: <%4.4d,%#6.6x> %s%c-> %s",bid,change,bh.filename,(change&(1<<0))?32:0,newbh.filename);
        securityreport(buf,NULL,NULL, getSession());
    }
    else{
        sprintf(buf,"�޸�����������: %s%c-> %s",bh.filename,(change&(1<<0))?32:0,newbh.filename);
        write_header(fp,getCurrentUser(),0,"syssecurity",buf,0,0,getSession());
        fprintf(fp,"\033[1;33m[������ <id=%d> �����޸���ϸ]\033[m\n\n",bid);
        for(i=0;i<MB_ITEMS-1;i++){
            if(change&(1<<i))
                fprintf(fp,"  %s\n  \033[1;32m%s\033[m\n\n",orig[i],menustr[i]);
        }
        fclose(fp);
        post_file(getCurrentUser(),"",src,"syssecurity",buf,0,-1,getSession());
        unlink(src);
    }
    newbbslog(BBSLOG_USER,"edit_board: %s <%4.4d,%#6.6x>",bh.filename,bid,change);
    move(20,0);clrtoeol();
    move(20,2);prints(error?"\033[1;33m�������,�븴��ȷ�ϲ������!\033m":"\033[1;32m�����ɹ�!\033[m");
    WAIT_RETURN;clear();
    return 0;
#undef MB_ITEMS
}
#undef KEY_CANCEL
#undef EB_BUF_LEN
/*END - etnlegend,2005.07.01,�޸�����������*/

//etnlegend,2005.09.18,��ѯϵͳ��¼��Ϊ���Ĳ�ѯ�ͷ��Ų�ѯ��������
int searchtrace(void){
    struct userec *user;
    char buf[256],fn_buf[256],ans[4];
    if(!check_systempasswd())
        return -1;
    clear();move(0,0);prints("\033[1;32m��ѯϵͳ��¼\033[m");
    modify_user_mode(ADMIN);
    move(1,0);usercomplete("��ѯ�û�: ",buf);
    if(!buf[0]){
        move(2,0);prints("ȡ��...");
        WAIT_RETURN;clear();
        return -1;
    }
    if(!getuser(buf,&user)){
        move(2,0);prints("�Ƿ��û�...");
        WAIT_RETURN;clear();
        return -1;
    }
    sprintf(fn_buf,"tmp/searchtrace_%ld_%d",time(NULL), (int)getpid());
    sprintf(buf,"��ѯ %s ����(P)/����(M)��¼ [P]: ",user->userid);
    move(2,0);clrtobot();
    if(getdata(2,0,buf,ans,2,DOECHO,NULL,true)==-1){
		clear();
		return -1;
	}
    if(!ans[0]||ans[0]=='p'||ans[0]=='P'){
#ifdef NEWPOSTLOG
        {
            FILE *fp;
            MYSQL s;
            MYSQL_RES *res;
            MYSQL_ROW row;
            if(!(fp=fopen(fn_buf,"w"))){
                move(3,0);prints("������ʱ�ļ�����...");
                WAIT_RETURN;clear();
                return -1;
            }
            mysql_init(&s);
            if(!my_connect_mysql(&s)){
                fclose(fp);unlink(fn_buf);
                move(3,0);prints("MySQL ����: %s",mysql_error(&s));
                WAIT_RETURN;clear();
                return -1;
            }
            sprintf(buf,"SELECT * FROM postlog WHERE userid='%s' ORDER BY time;",user->userid);
            if(mysql_real_query(&s,buf,strlen(buf))){
                fclose(fp);unlink(fn_buf);mysql_close(&s);
                move(3,0);prints("MySQL ����: %s",mysql_error(&s));
                WAIT_RETURN;clear();
                return -1;
            }
            fprintf(fp,"\033[1;32m�û� \033[1;33m%s\033[1;32m ���ڷ��ļ�¼\033[m\n",user->userid);
            res=mysql_store_result(&s);
            while(!!(row=mysql_fetch_row(res)))
                fprintf(fp,"%s: %-20s %s\n",row[4],row[2],row[3]);
            mysql_free_result(res);
            mysql_close(&s);
            fclose(fp);
        }
#else
        sprintf(buf,"grep -aw '^\\[.*\\] %s posted' user.log > %s",user->userid,fn_buf);
        system(buf);
#endif
        sprintf(buf,"��ѯ�û� %s ���ڷ��ļ�¼",user->userid);
    }
    else if(ans[0]=='m'||ans[0]=='M'){
#ifdef NEWSMTH
        if(!HAS_PERM(getCurrentUser(),PERM_SYSOP)){
            move(3,0);prints("��ǰ�û������в�ѯ�û����ڷ��ż�¼��Ȩ��...\033[0;33m<Enter>\033[m");
            WAIT_RETURN;clear();
            return -1;
        }
#endif
        sprintf(buf,
            "grep -awE '^\\[.*\\] %s mailed(\\(www\\))?' user.log | grep -awEv '^\\[.*\\] %s mailed(\\(www\\))? %s.?$' > %s",
            user->userid,user->userid,user->userid,fn_buf);
        system(buf);
        sprintf(buf,"��ѯ�û� %s ���ڷ��ż�¼",user->userid);
    }
    else{
        move(3,0);prints("ȡ��...");
        WAIT_RETURN;clear();
        return -1;
    }
    mail_file(getCurrentUser()->userid,fn_buf,getCurrentUser()->userid,&buf[4],BBSPOST_MOVE,NULL);
    securityreport(buf,user,NULL, getSession());
    newbbslog(BBSLOG_USER,"query_user_log: <%c> %s",(toupper(ans[0])=='M'?'M':'P'),user->userid);
    move(3,0);prints("\033[1;36m%s �ѻؼ�, �����ż�...\033[m",&buf[4]);
    WAIT_RETURN;clear();
    return 0;
}


void trace_state(flag, name, size)
int flag, size;
char *name;
{
    char buf[STRLEN];

    if (flag != -1) {
        sprintf(buf, "ON (size = %d)", size);
    } else {
        strcpy(buf, "OFF");
    }
    prints("%s��¼ %s\n", name, buf);
}

int m_trace(void){
    struct stat ostatb, cstatb;
    int otflag, ctflag, done = 0;
    char ans[3];
    char *msg;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("Set Trace Options");
    while (!done) {
        move(2, 0);
        otflag = stat("trace", &ostatb);
        ctflag = stat("trace.chatd", &cstatb);
        prints("Ŀǰ�趨:\n");
        trace_state(otflag, "һ��", ostatb.st_size);
        trace_state(ctflag, "����", cstatb.st_size);
        move(9, 0);
        prints("<1> �л�һ���¼\n");
        prints("<2> �л������¼\n");
        getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, NULL, true);

        switch (ans[0]) {
        case '1':
            if (otflag) {
                f_touch("trace");
                msg = "һ���¼ ON";
            } else {
                f_mv("trace", "trace.old");
                msg = "һ���¼ OFF";
            }
            break;
        case '2':
            if (ctflag) {
                f_touch("trace.chatd");
                msg = "�����¼ ON";
            } else {
                f_mv("trace.chatd", "trace.chatd.old");
                msg = "�����¼ OFF";
            }
            break;
        default:
            msg = NULL;
            done = 1;
        }
        move(t_lines - 2, 0);
        if (msg) {
            prints("%s\n", msg);
            bbslog("user", "%s", msg);
        }
    }
    clear();
    return 0;
}

int valid_userid(ident)         /* check the user has registed, added by dong, 1999.4.18 */
char *ident;
{
    if (strchr(ident, '@') && valid_ident(ident))
        return 1;
    return 0;
}

int check_proxy_IP(const char *ip,char *reason){
    FILE *fp;
    char buf[128],*p;
    int ip_len,buf_len,comp_len,ret;
    if(!(fp=fopen("etc/proxyIP","r")))
        return -1;
    ip_len=strlen(ip);
    ret=0;
    while(fgets(buf,128,fp)){
        if((p=strchr(buf,'\n')))
            *p=0;
        if((p=strchr(buf,' '))){
            *p=0;
            if(reason)
                strcpy(reason,&p[1]);
        }
        buf_len=(p-buf);
        if((comp_len=ip_len)>buf_len)
            comp_len=buf_len;
        if(!strncmp(ip,buf,comp_len)){
            ret=comp_len;
            break;
        }
    }
    fclose(fp);
    return ret;
}

int apply_reg(regfile, fname, pid, num)
/* added by Bigman, 2002.5.31 */
/* ����ָ������ע�ᵥ */
char *regfile, *fname;
long pid;
int num;
{
    FILE *in_fn, *out_fn, *tmp_fn;
    char fname1[STRLEN], fname2[STRLEN];
    int sum, fd;
    char *ptr;

    strcpy(fname1, "reg.ctrl");

    if ((in_fn = fopen(regfile, "r+")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", regfile);
        pressreturn();
        return -1;
    }

    fd = fileno(in_fn);
    flock(fd, LOCK_EX);

    if ((out_fn = fopen(fname, "w")) == NULL) {
        move(2, 0);
        flock(fd, LOCK_UN);
        fclose(in_fn);
        prints("ϵͳ����, �޷�д��ʱע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }
    sum = 0;

    while (fgets(genbuf, STRLEN, in_fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "----")) != NULL)
            sum++;

        fputs(genbuf, out_fn);

        if (sum >= num)
            break;
    }
    fclose(out_fn);

    if (sum >= num) {
        sum = 0;

		gettmpfilename( fname2, "reg" );
        //sprintf(fname2, "tmp/reg.%ld", pid);

        if ((tmp_fn = fopen(fname2, "w")) == NULL) {
            prints("���ܽ�����ʱ�ļ�:%s\n", fname2);
            flock(fd, LOCK_UN);
            fclose(in_fn);
            pressreturn();
            return -1;
        }

        while (fgets(genbuf, STRLEN, in_fn) != NULL) {
            if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
                sum++;
            fputs(genbuf, tmp_fn);

        }

        flock(fd, LOCK_UN);

        fclose(in_fn);
        fclose(tmp_fn);

        if (sum > 0) {
            f_rm(regfile);
            f_mv(fname2, regfile);
        } else
            f_rm(regfile);

        f_rm(fname2);

    }

    else
        f_rm(regfile);

    if ((out_fn = fopen(fname1, "a")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷�����ע������ļ�: %s\n", fname1);
        pressreturn();
        return -1;
    }

    fd = fileno(out_fn);

    flock(fd, LOCK_UN);
    fprintf(out_fn, "%ld\n", pid);
    flock(fd, LOCK_UN);
    fclose(out_fn);

    return (0);
}

int restore_reg(long pid)
/* added by Bigman, 2002.5.31 */
/* �ָ����ߵ�ע���ļ� */
{
    FILE *fn, *freg;
    char *regfile, buf[STRLEN];
    int fd1, fd2;

    regfile = "new_register";

    sprintf(buf, "register.%ld", pid);

    if ((fn = fopen(buf, "r")) != NULL) {
        fd1 = fileno(fn);
        flock(fd1, LOCK_EX);

        if ((freg = fopen(regfile, "a")) != NULL) {
            fd2 = fileno(freg);
            flock(fd2, LOCK_EX);
            while (fgets(genbuf, STRLEN, fn) != NULL)
                fputs(genbuf, freg);
            flock(fd2, LOCK_UN);
            fclose(freg);

        }
        flock(fd1, LOCK_UN);
        fclose(fn);

        f_rm(buf);
    }

    return (0);
}
int check_reg(mod)
int mod;

/* added by Bigman, 2002.5.31 */
/* mod=0 ���reg_control�ļ� */
/* mod=1 �����˳�ɾ�����ļ� */
{
    FILE *fn1, *fn2;
    char fname1[STRLEN];
    char fname2[STRLEN];
    long myid;
    int flag = 0, fd;

    strcpy(fname1, "reg.ctrl");

    if ((fn1 = fopen(fname1, "r")) != NULL) {

        fd = fileno(fn1);
        flock(fd, LOCK_EX);

		gettmpfilename( fname2, "reg.c");
        //sprintf(fname2, "tmp/reg.c%ld", getpid());

        if ((fn2 = fopen(fname2, "w")) == NULL) {
            prints("���ܽ�����ʱ�ļ�:%s\n", fname2);
            flock(fd, LOCK_UN);
            fclose(fn1);
            pressreturn();
            return -1;
        } else {
            while (fgets(genbuf, STRLEN, fn1) != NULL) {

                myid = atol(genbuf);

                if (mod == 0) {
/*                    	if (myid==getpid())
                    {
                	prints("��ֻ��һ�����̽��������ʺ�");
                	pressreturn();
                	return -1;
                    }
*/

                    if (kill(myid, 0) == -1) {  /*ע���м�����ˣ��ָ� */
                        flag = 1;
                        restore_reg(myid);
                    } else {
                        fprintf(fn2, "%ld\n", myid);
                    }
                } else {
                    flag = 1;
                    if (myid != getpid())
                        fprintf(fn2, "%ld\n", myid);


                }

            }
            fclose(fn2);
        }
        flock(fd, LOCK_UN);
        fclose(fn1);

        if (flag == 1) {
            f_rm(fname1);
            f_mv(fname2, fname1);
        }
        f_rm(fname2);

    }

    return (0);
}

static const char *field[] = { "usernum", "userid", "realname", "career",
    "addr", "phone", "birth", NULL
};
static const char *reason[] = {
    "��������ʵ����(�������ƴ��).", "������ѧУ��ϵ������λ.",
    "����д������סַ����.", "����������绰(���޿��ú�����Email��ַ����).",
    "��ȷʵ����ϸ����дע�������.", "����������д���뵥.",
    "������Ӵ���ע��", "ͬһ���û�ע���˹���ID",
    NULL
};

#ifdef AUTO_CHECK_REGISTER_FORM
#include "checkreg.c"
#endif
int scan_register_form(logfile, regfile)
char *logfile, *regfile;
{
    static const char *finfo[] = { "�ʺ�λ��", "�������", "��ʵ����", "����λ",
        "Ŀǰסַ", "����绰", "��    ��", NULL
    };
    struct userec uinfo;
    FILE *fn, *fout, *freg;
    char fdata[8][STRLEN];
    char fname[STRLEN], buf[STRLEN], buff;
    char sender[IDLEN + 2];
    int  useproxy;

    /*
     * ^^^^^ Added by Marco 
     */
    char ans[5], *ptr, *uid;
    int n, unum, fd;
    int count, sum, total_num;  /*Haohmaru.2000.3.9.���㻹�ж��ٵ���û���� */
    char result[STRLEN];
    long pid;                   /* Added by Bigman: 2002.5.31 */

    uid = getCurrentUser()->userid;


    stand_title("�����趨������ע������");
/*    sprintf(fname, "%s.tmp", regfile);*/

    pid = getpid();
    sprintf(fname, "register.%ld", pid);

    move(2, 0);
    if (dashf(fname)) {
        restore_reg(pid);       /* Bigman,2002.5.31:�ָ����ļ� */
    }
/*    f_mv(regfile, fname);*/
/*����ע�ᵥ added by Bigman, 2002.5.31*/

/*ͳ���ܵ�ע�ᵥ�� Bigman, 2002.6.2 */
    if ((fn = fopen(regfile, "r")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }

    fd = fileno(fn);
    flock(fd, LOCK_EX);

    total_num = 0;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
            total_num++;
    }
    flock(fd, LOCK_UN);
    fclose(fn);

    apply_reg(regfile, fname, pid, 50);

    if ((fn = fopen(fname, "r")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }
    memset(fdata, 0, sizeof(fdata));
    /*
     * Haohmaru.2000.3.9.���㹲�ж��ٵ��� 
     */
    sum = 0;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
            sum++;
    }
    fseek(fn, 0, SEEK_SET);
    count = 1;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        struct userec *lookupuser;

        if ((ptr = (char *) strstr(genbuf, ": ")) != NULL) {
            *ptr = '\0';
            for (n = 0; field[n] != NULL; n++) {
                if (strcmp(genbuf, field[n]) == 0) {
                    strcpy(fdata[n], ptr + 2);
                    if ((ptr = (char *) strchr(fdata[n], '\n')) != NULL)
                        *ptr = '\0';
                }
            }
        } else if ((unum = getuser(fdata[1], &lookupuser)) == 0) {
            move(2, 0);
            clrtobot();
            prints("ϵͳ����, ���޴��ʺ�.\n\n");
            for (n = 0; field[n] != NULL; n++)
                prints("%s     : %s\n", finfo[n], fdata[n]);
            pressreturn();
            memset(fdata, 0, sizeof(fdata));
        } else {
            struct userdata ud;

            uinfo = *lookupuser;
            move(1, 0);
            prints("�ʺ�λ��     : %d   ���� %d ��ע�ᵥ����ǰΪ�� %d �ţ���ʣ %d ��\n", unum, total_num, count, sum - count + 1);    /*Haohmaru.2000.3.9.���㻹�ж��ٵ���û���� */
            count++;
            disply_userinfo(&uinfo, 2);
			
			read_userdata(lookupuser->userid, &ud);
			useproxy = check_proxy_IP(uinfo.lasthost, buf);
#ifdef AUTO_CHECK_REGISTER_FORM
{
struct REGINFO regform;
int ret;
char errorstr[100];
bzero(&regform,sizeof(regform));
errorstr[0]=0;
strncpy(regform.userid,lookupuser->userid,99);
strncpy(regform.realname,fdata[2],99);
strncpy(regform.career,fdata[3],99);
strncpy(regform.addr,fdata[4],99);
strncpy(regform.phone,fdata[5],99);
strncpy(regform.birth,fdata[6],99);
strncpy(regform.ip, uinfo.lasthost, 20);
ret=checkreg(regform, errorstr, useproxy);
if (ret==-2) {
#endif
            if(qqwry_search(result,uinfo.lasthost)==1){
                move(8,20);
                prints("\033[1;33m[%s]\033[m",result);
            }

            move(15, 0);
            printdash(NULL);
            for (n = 0; field[n] != NULL; n++) {
                /*
                 * added for rejection of register from proxy
                 */
                /*
                 * Bigman, 2001.11.9
                 */
                 clrtoeol();
#ifdef AUTO_CHECK_REGISTER_FORM
		 if (strstr(finfo[n],"��ʵ����")) continue;
#else
                if (n == 1) {
                    if (useproxy > 0)
                        prints("%s     : %s \033[33m%s\033[m\n", finfo[n], fdata[n], buf);
                    else
                        prints("%s     : %s\n", finfo[n], fdata[n]);
                } else
#endif
                    prints("%s     : %s\n", finfo[n], fdata[n]);
            }
            /*
             * if (uinfo.userlevel & PERM_LOGINOK) modified by dong, 1999.4.18 
             */
            if ((uinfo.userlevel & PERM_LOGINOK) || valid_userid(ud.realemail)) {
                move(t_lines - 2, 0);
                //move(t_lines - 1, 0);
                prints("���ʺŲ�������дע�ᵥ.\n");
                pressanykey();
                ans[0] = 'D';
            } else {
#ifdef AUTO_CHECK_REGISTER_FORM
                move(t_lines - 2, 0);
/*
		prints("%s�Զ����ע�ᵥ:%s %s\x1b[m",
	saveret==0?"\x1b[1;32m":(saveret==2?"\x1b[1;33m":"\x1b[1;31m"),
	saveret==0?"����Ϊ����ͨ��!":
	(saveret==2?"��������������":(saveret==-1?"���id��̫�ð�":"Ӧ���˻� ����:")),
	errorstr);
*/
		prints("\x1b[1;32mϵͳ����:\x1b[m%s",errorstr);
                move(t_lines - 1, 0);
#endif
                getdata(t_lines - 1, 0, "�Ƿ���ܴ����� (Y/N/Q/Del/Skip)? [S]: ", ans, 3, DOECHO, NULL, true);
            }
            move(2, 0);
            clrtobot();
#ifdef AUTO_CHECK_REGISTER_FORM
} else { //�Զ�����
	if (ret==-3) ans[0]='y';
	else ans[0]='n';
}
#endif
            switch (ans[0]) {
            case 'D':
            case 'd':
                break;
            case 'Y':
            case 'y':
			{
				struct usermemo *um;

				read_user_memo(uinfo.userid, &um);

                prints("����ʹ���������Ѿ�����:\n");
                n = strlen(fdata[5]);
                if (n + strlen(fdata[3]) > 60) {
                    if (n > 40)
                        fdata[5][n = 40] = '\0';
                    fdata[3][60 - n] = '\0';
                }
                strncpy(ud.realname, fdata[2], NAMELEN);
                strncpy(ud.address, fdata[4], NAMELEN);
#ifdef AUTO_CHECK_REGISTER_FORM
         if (ret==-2)
#endif
                sprintf(genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
#ifdef AUTO_CHECK_REGISTER_FORM
	else
		sprintf(genbuf, "%s$%s@SYSOP", fdata[3], fdata[5]);
#endif

		if(strlen(genbuf) >= STRLEN-16)
			sprintf(genbuf, "%s@%s",fdata[5],uid);
                strncpy(ud.realemail, genbuf, STRLEN - 16);
		ud.realemail[STRLEN - 16 - 1] = '\0';
                sprintf(buf, "tmp/email/%s", uinfo.userid);
                if ((fout = fopen(buf, "w")) != NULL) {
                    fprintf(fout, "%s\n", genbuf);
                    fclose(fout);
                }

                update_user(&uinfo, unum, 0);
                write_userdata(uinfo.userid, &ud);
				memcpy(&(um->ud), &ud, sizeof(ud));
				end_mmapfile(um, sizeof(struct usermemo), -1);
#ifdef AUTO_CHECK_REGISTER_FORM
         if (ret==-2)
         {
#endif
                strcpy(sender,getCurrentUser()->userid);
                sprintf(genbuf, "%s �� %s ͨ�����ȷ��.", uid, uinfo.userid);
#ifdef AUTO_CHECK_REGISTER_FORM
         }
         else
         {
                strcpy(sender,"SYSOP");
                sprintf(genbuf, "�Զ�������� �� %s ͨ�����ȷ��.", uinfo.userid);
	 }
#endif
         	mail_file(sender, "etc/s_fill", uinfo.userid, "�����㣬���Ѿ����ע�ᡣ", 0, NULL);
                securityreport(genbuf, lookupuser, fdata, getSession());
                if ((fout = fopen(logfile, "a")) != NULL) {
                    time_t now;

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(fout, "%s: %s\n", field[n], fdata[n]);
                    now = time(NULL);
                    fprintf(fout, "Date: %s\n", Ctime(now));
                    fprintf(fout, "Approved: %s\n", sender);
                    fprintf(fout, "----\n");
                    fclose(fout);
                }
                /*
                 * user_display( &uinfo, 1 ); 
                 */
                /*
                 * pressreturn(); 
                 */

                /*
                 * ����ע����Ϣ��¼ 2001.11.11 Bigman 
                 */
                sethomefile(buf, uinfo.userid, "/register");
                if ((fout = fopen(buf, "w")) != NULL) {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf(fout, "%s     : %s\n", finfo[n], fdata[n]);
                    fprintf(fout, "�����ǳ�     : %s\n", uinfo.username);
                    fprintf(fout, "�����ʼ����� : %s\n", ud.email);
                    fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
                    fprintf(fout, "ע������     : %s\n", ctime(&uinfo.firstlogin));
                    fprintf(fout, "ע��ʱ�Ļ��� : %s\n", uinfo.lasthost);
                    fprintf(fout, "Approved: %s\n", sender);
                    fclose(fout);
                }

                break;
			}
            case 'Q':
            case 'q':
                if ((freg = fopen(regfile, "a")) != NULL) {
                    fd = fileno(freg);
                    flock(fd, LOCK_EX);

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");
                    while (fgets(genbuf, STRLEN, fn) != NULL)
                        fputs(genbuf, freg);

                    flock(fd, LOCK_UN);
                    fclose(freg);
                }

                break;
            case 'N':
            case 'n':
                for (n = 0; field[n] != NULL; n++) {
#ifdef AUTO_CHECK_REGISTER_FORM
		 if (strstr(finfo[n],"��ʵ����")) continue;
#endif
                    prints("%s: %s\n", finfo[n], fdata[n]);
		}
                move(9, 0);
#ifdef AUTO_CHECK_REGISTER_FORM
              if (ret==-2) {
#endif
                prints("��ѡ��/�����˻������ԭ��, �� <enter> ȡ��.\n");
                for (n = 0; reason[n] != NULL; n++)
                    prints("%d) %s\n", n, reason[n]);
                getdata(10 + n, 0, "�˻�ԭ��: ", buf, STRLEN, DOECHO, NULL, true);
#ifdef AUTO_CHECK_REGISTER_FORM
              } else {
                buf[0]='!';
                strncpy(fdata[7],errorstr,STRLEN - 1);
                sprintf(genbuf, "�Զ��������ܾ� %s �����ȷ��.", uinfo.userid);
                securityreport(genbuf, lookupuser, fdata, getSession());
              }
#endif

                buff = buf[0];  /* Added by Marco */
                if (buf[0] != '\0') {
                    if (buf[0] >= '0' && buf[0] < '0' + n) {
                        strcpy(buf, reason[buf[0] - '0']);
                    }
                
#ifdef AUTO_CHECK_REGISTER_FORM
                   if (ret==-2)
                   {
#endif
                    strcpy(sender,getCurrentUser()->userid);
                    sprintf(genbuf, "<ע��ʧ��> - %s", buf);
#ifdef AUTO_CHECK_REGISTER_FORM
                   }
                   else
                   {
                    strcpy(sender,"SYSOP");
                    sprintf(genbuf, "<ע��ʧ��> - %s", errorstr);
                   }
#endif
                    strncpy(ud.address, genbuf, NAMELEN);
                    write_userdata(uinfo.userid, &ud);
                    update_user(&uinfo, unum, 0);

                    /*
                     * ------------------- Added by Marco 
                     */
                    switch (buff) {
                    case '0':
                        mail_file(sender, "etc/f_fill.realname", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '1':
                        mail_file(sender, "etc/f_fill.unit", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '2':
                        mail_file(sender, "etc/f_fill.address", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '3':
                        mail_file(sender, "etc/f_fill.telephone", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '4':
                        mail_file(sender, "etc/f_fill.real", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '5':
                        mail_file(sender, "etc/f_fill.chinese", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '6':
                        mail_file(sender, "etc/f_fill.proxy", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '7':
                        mail_file(sender, "etc/f_fill.toomany", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    default:
                        mail_file(sender, "etc/f_fill", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    }
                    /*
                     * -------------------------------------------------------
                     */
                    /*
                     * user_display( &uinfo, 1 ); 
                     */
                    /*
                     * pressreturn(); 
                     */
                    break;
                }
                move(10, 0);
                clrtobot();
                prints("ȡ���˻ش�ע�������.\n");
                /*
                 * run default -- put back to regfile 
                 */
            default:
                if ((freg = fopen(regfile, "a")) != NULL) {
                    fd = fileno(freg);
                    flock(fd, LOCK_EX);

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");

                    flock(fd, LOCK_UN);
                    fclose(freg);
                }
            }
            memset(fdata, 0, sizeof(fdata));
#ifdef AUTO_CHECK_REGISTER_FORM
}
#endif
        }
    }                           /* while */

    check_reg(1);               /* Bigman:2002.5.31 */

    fclose(fn);
    unlink(fname);
    return (0);
}

int m_register(void){
    FILE *fn;
    char ans[3], *fname;
    int x, y, wid, len;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();

    if (check_reg(0) != 0)
        return (-1);            /* added by Bigman, 2002.5.31 */

    stand_title("�趨ʹ����ע������");
    move(2, 0);

    fname = "new_register";

    if ((fn = fopen(fname, "r")) == NULL) {
        prints("Ŀǰ������ע������.");
        pressreturn();
    } else {
        y = 2, x = wid = 0;
        while (fgets(genbuf, STRLEN, fn) != NULL && x < 65) {
            if (strncmp(genbuf, "userid: ", 8) == 0) {
                move(y++, x);
                prints(genbuf + 8);
                len = strlen(genbuf + 8);
                if (len > wid)
                    wid = len;
                if (y >= t_lines - 2) {
                    y = 2;
                    x += wid + 2;
                }
            }
        }
        fclose(fn);
        getdata(t_lines - 1, 0, "�趨������ (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
        if (ans[0] == 'Y' || ans[0] == 'y') {
            {
                char secu[STRLEN];

                sprintf(secu, "�趨ʹ����ע������");
                securityreport(secu, NULL, NULL, getSession());
            }
            scan_register_form("register.list", fname);
        }
    }
    clear();
    return 0;
}

int m_stoplogin(void){
    char ans[4];
    modify_user_mode(ADMIN);
    if(!check_systempasswd())
        return -1;
    getdata(t_lines - 1, 0, "��ֹ��¼�� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        if (vedit("NOLOGIN", false, NULL, NULL, 0) == -1)
            unlink("NOLOGIN");
    }
    return 0;
}

/* czz added 2002.01.15 */
int inn_start(void){
    char ans[4], tmp_command[80];
    getdata(t_lines - 1, 0, "����ת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, BBSHOME "/innd/innbbsd");
        system(tmp_command);
    }
    return 0;
}

int inn_reload(void){
    char ans[4], tmp_command[80];
    getdata(t_lines - 1, 0, "�ض������� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, BBSHOME "/innd/ctlinnbbsd reload");
        system(tmp_command);
    }
    return 0;
}

int inn_stop(void){
    char ans[4], tmp_command[80];
    getdata(t_lines - 1, 0, "ֹͣת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, BBSHOME "/innd/ctlinnbbsd shutdown");
        system(tmp_command);
    }
    return 0;
}

/* added end */
/* ���Ȩ�޹���*/
int x_deny(void){
    int sel;
    char userid[IDLEN + 2];
    struct userec *lookupuser;
    const int level[] = {
        PERM_BASIC,
        PERM_POST,
        PERM_DENYMAIL,
        PERM_CHAT,
        PERM_PAGE,
        PERM_DENYRELAX,
        -1
    };
    const int normal_level[] = {
        PERM_BASIC,
        PERM_POST,
        0,
        PERM_CHAT,
        PERM_PAGE,
        0,
        -1
    };

    const struct _select_item level_conf[] = {
        {3, 6, -1, SIT_SELECT, (void *) "1)��¼Ȩ��"},
        {3, 7, -1, SIT_SELECT, (void *) "2)��������Ȩ��"},
        {3, 8, -1, SIT_SELECT, (void *) "3)����Ȩ��"},
        {3, 9, -1, SIT_SELECT, (void *) "4)����������Ȩ��"},
        {3, 10, -1, SIT_SELECT, (void *) "5)��������Ȩ��"},
        {3, 11, -1, SIT_SELECT, (void *) "6)��������Ȩ��"},
        {3, 12, -1, SIT_SELECT, (void *) "7)��һ��ID"},
        {3, 13, -1, SIT_SELECT, (void *) "8)�˳�"},
        {-1, -1, -1, 0, NULL}
    };
    static const unsigned int GIVEUP_PERM[GIVEUPINFO_PERM_COUNT]={
        PERM_BASIC,PERM_POST,PERM_CHAT,PERM_PAGE,PERM_DENYMAIL,PERM_DENYRELAX};

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    move(0, 0);
    clear();

    while (1) {
        int i,basicperm,s[GIVEUPINFO_PERM_COUNT];

#ifdef LILAC
		int isdeny = 2;
#endif
        move(1, 0);

        usercomplete("������ʹ�����ʺ�:", genbuf);
        strncpy(userid, genbuf, IDLEN + 1);
        if (userid[0] == '\0') {
            clear();
            return 0;
        }

        if (!(getuser(userid, &lookupuser))) {
            move(3, 0);
            prints("����ȷ��ʹ���ߴ���\n");
            clrtoeol();
            pressreturn();
            clear();
            continue;
        }
        get_giveupinfo(lookupuser,s);
        for(basicperm=0,i=0;i<GIVEUPINFO_PERM_COUNT;i++)
            if(s[i]>0)
                basicperm|=GIVEUP_PERM[i];
        move(3, 0);
        clrtobot();

        for (i = 0; level[i] != -1; i++)
            if ((lookupuser->userlevel & level[i]) != normal_level[i]) {
                move(6 + i, 40);
                if (level[i] & basicperm)
                    prints("������");
                else
                    prints("�����");
            }
        sel = simple_select_loop(level_conf, SIF_NUMBERKEY | SIF_SINGLE, 0, 6, NULL);
        if (sel == i + 2)
            break;
        if (sel > 0 && sel <= i) {
            /*char buf[40];  commented by binxun*/
            /*---------*/
            char buf[STRLEN]; /*buf is too small...changed by binxun .2003/05/11 */
            /*---------*/
            char reportbuf[120];

            move(40, 0);
            if ((lookupuser->userlevel & level[sel - 1]) == normal_level[sel - 1]) {
                sprintf(buf, "���Ҫ���%s��%s", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                if (askyn(buf, 0) != 0) {
                    sprintf(reportbuf, "���%s��%s ", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    lookupuser->userlevel ^= level[sel - 1];
                    securityreport(reportbuf, lookupuser, NULL, getSession());
#ifdef LILAC
					gen_x_deny_notify((char *) level_conf[sel - 1].data + 2, lookupuser->userid, 1);
#endif
                    break;
                }
            } else {
                if (!(basicperm & level[sel - 1])) {
                    sprintf(buf, "���Ҫ�⿪%s��%s ���", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    sprintf(reportbuf, "�⿪%s��%s ���", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
#ifdef LILAC
					isdeny = 0;
#endif
                } else {
                    sprintf(buf, "���Ҫ�⿪%s��%s ����", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    sprintf(reportbuf, "�⿪%s��%s ����", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
#ifdef LILAC
					isdeny = 2;
#endif
                }
                if (askyn(buf, 0) != 0) {
                    lookupuser->userlevel ^= level[sel - 1];
                    securityreport(reportbuf, lookupuser, NULL, getSession());
                    save_giveupinfo(lookupuser,s);
#ifdef LILAC
					if (isdeny < 2)
						gen_x_deny_notify((char *) level_conf[sel - 1].data + 2, lookupuser->userid, isdeny);
#endif
                    break;
                }
            }
        }
    }
    return 0;
}

int set_BM(void){
//etnlegend ��д, 2005.05.26 �ύ
    char bname[STRLEN],genbuf[1024],*p;
    int pos,flag=0,n,id,brd_num;
    struct boardheader bh;
    const struct boardheader *cache_ptr;
    struct userec *user,uinfo;

    modify_user_mode(ADMIN);
    if(!check_systempasswd()){
        return -1;
    }
    clear();
    stand_title("�������");
    move(1,0);
    make_blist(0);
    namecomplete("��������������: ",bname);
    if(!*bname){
        move(2,0);
        prints("ȡ��...");
        pressreturn();
        clear();
        return -1;
    }
    pos=getboardnum(bname,&bh);
    if(!pos){
        move(2,0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    cache_ptr=getboard(pos);
    while(true){
        clear();
        stand_title("�������");
        move(1,0);
        prints("����������  : %s\n",bh.filename);
        prints("������˵��  : %s\n",bh.title);
        prints("����������Ա: %s\n",bh.BM);
        getdata(6,0,"(A)���Ӱ��� (D)ɾ������ (Q)�˳�?: [Q]",genbuf,2,DOECHO,NULL,true);
        if(*genbuf=='a'||*genbuf=='A')
            flag=1;
        else if(*genbuf=='d'||*genbuf=='D')
            flag=2;
        else{
            clear();
            return 0;
        }
        if(flag>0){
            if(flag==1)
                getdata(7,0,"������"NAME_USER_SHORT"ID: ",genbuf,IDLEN+1,DOECHO,NULL,true);
            else if(flag==2)
                getdata(7,0,"������"NAME_BM"ID�����: ",genbuf,IDLEN+1,DOECHO,NULL,true);
            /*Ϊ�Ժ�����flag==3֮�����׼����,ʡ�û��ø�...*/
            if(genbuf[0]=='\0'){
                clear();
                flag=0;
            }
            else if(flag==2&&((genbuf[0]>'0')&&!(genbuf[0]>'9'))){
                /*9����Ź��˰�?Ҫ���г���10�������İ��������ʵʵ���ֶ���id��...*/
                n=genbuf[0]-'0';
                p=bh.BM;
                if(!*p)
                    flag=0;
                if(n>1&&flag)
                    for(n--;n;n--,p++){
                        p=strchr(p,' ');
                        if(!p){
                            flag=0;
                            break;
                        }
                    }
                if(flag){
                    sscanf(p,"%s",genbuf);
                    if(!(id=getuser(genbuf,&user))){
                        prints("\n\033[1;31m��Ӧ��ŵİ���id�Ƿ�!\033[m");
                        if(askyn("�Ƿ�����",false)){
                            struct boardheader oldbh;
                            char title[80];
                            memcpy(&oldbh,&bh,sizeof(struct boardheader));
                            if(strlen(p)==strlen(genbuf))
                                (p==bh.BM)?(bh.BM[0]=NULL):(*--p=NULL);
                            else
                                memmove(p,p+strlen(genbuf)+1,strlen(p)-strlen(genbuf));
                            edit_group(&oldbh,&bh);
                            set_board(pos,&bh,NULL);
                            newbbslog(BBSLOG_USER,"setBM: clear invalid BM %s on %s",genbuf,bh.filename);
                            snprintf(title,80,"���� %s ��Ƿ����� %s",bh.filename,genbuf);
                            securityreport(title,NULL,NULL, getSession());
                        }
                        else{
                            clrtoeol();pressreturn();clear();
                        }
                        flag=0;
                    }
                }
                else{
                    prints("\n\033[1;31mδ�ҵ���Ӧ��ŵİ���!\033[m");
                    clrtoeol();pressreturn();clear();
                }
            }
            else if(!(id=getuser(genbuf,&user))){
                prints("\n\033[1;31m�Ƿ�ID!\033[m");
                clrtoeol();
                pressreturn();
                clear();
                flag = 0;
            }
            else if(flag==1&&chk_BM_instr(bh.BM,user->userid)){
                prints("\033[1;31m����:\033[m\n%s �Ѿ��Ǹð����,�޷�����!",user->userid);
                clrtoeol();pressreturn();clear();
                flag=0;
            }
            else if(flag==2&&!chk_BM_instr(bh.BM,user->userid)){
                prints("\033[1;31m����:\033[m\n%s ���Ǹð����,�޷�ɾ��!",user->userid);
                clrtoeol();pressreturn();clear();
                flag=0;
            }
            if(flag>0){
                uinfo=*user;
                disply_userinfo(&uinfo,1);
                brd_num=0;
                if(!(user->userlevel&PERM_BOARDS)){
                     move(22,0);clrtoeol();//���찡,���Ӧ������"����ע������Ѿ����"�����һ�е�λ��,��ô�Ḳ����...
                     prints("\033[1;33m�û� \033[1;32m%s\033[1;33m ���ǰ���!\033[m",user->userid);
                }
                else{
                    const struct boardheader *bptr;
                    for(n=0;n<get_boardcount();n++){
                        bptr=getboard(n+1);
                        if(chk_BM_instr(bptr->BM,user->userid)){
                            move(++brd_num,56);
                            prints("* %-32s",bptr->filename);
                        }
                    }
                    move(22,0);clrtoeol();
                    prints("\033[1;33m�û� \033[1;32m%s\033[1;33m Ϊ�Ҳ� \033[1;32m%d\033[1;33m ������İ���:\033[m",
                        user->userid,brd_num);
                }
                getdata(t_lines-1,0,"ȷ��������û�(Y/N)?: [N]",genbuf,2,DOECHO,NULL,true);
                if(*genbuf=='y'||*genbuf=='Y'){
                    if(flag==1){
                        if(add_bm(user,&bh,pos,-1)==2){
                            clear();move(3,0);
                            prints("\033[1;31m����:\033[m\n�޷����� %s ,�����ַ������!",user->userid);
                            pressreturn();clear();
                            continue;
                        }
                        sprintf(genbuf,"���� %s �İ��� %s ",bh.filename,user->userid);
                    }
                    else if(flag==2){
                        del_bm(user,&bh,pos,brd_num);
                        sprintf(genbuf,"��ȥ %s �İ��� %s ",bh.filename,user->userid);
                    }
                    securityreport(genbuf,user,NULL, getSession());
                    newbbslog(BBSLOG_USER,"setBM: %s <%c> %s",bh.filename,(flag==1?'+':'-'),user->userid);
                    memcpy(bh.BM,cache_ptr->BM,BM_LEN);
#ifdef SMTH
                    if(flag==2)
                        process_no_bm(&bh);//stiger, 2005.09.05, �����ް���ʱ����ϵͳ֪ͨ
#endif
                }
            }
        }
    }
}

/* etnlegend, 2005.11.27, �趨�ض��û����з����ض��������վȨ�޽ӿ� */
int edit_board_delete_read_perm(void){
    FILE *fp;
    struct stat st;
    struct flock lc;
    struct userec *user;
    const struct boardheader *board;
    char buf[256],datafile[256],fn[256],userid[16],ans[4];
    int i,count,off,fd,ret;
    time_t mtime;
    void *p,*ptr;
    modify_user_mode(ADMIN);
    if(!check_systempasswd())
        return -1;
    clear();
    move(0,0);
    prints("\033[1;32m%s\033[m","[�趨վ����������ض��������վȨ��]");
    move(1,0);
    usercomplete("������վ�������û���: ",userid);
    move(2,0);
    clrtobot();
    if(!userid[0]){
        prints("%s\033[0;33m<Enter>\033[m","ȡ��...");
        WAIT_RETURN;
        clear();
        return -1;
    }
    if(!getuser(userid,&user)){
        prints("%s\033[0;33m<Enter>\033[m","������û���...");
        WAIT_RETURN;
        clear();
        return -1;
    }
    if(!HAS_PERM(user,PERM_BOARDS)||!user->title){
        prints("%s\033[0;33m<Enter>\033[m","��ѡ���û������з����ض��������վ��Ȩ�޺����...");
        WAIT_RETURN;
        clear();
        return -1;
    }
    move(1,0);
    clrtoeol();
    move(0,40);
    prints("\033[1;33m<%s>\033[m",user->userid);
    sethomefile(datafile,user->userid,"board_delete_read");
    while(1){
        move(2,0);
        clrtobot();
        getdata(2,0,
            "\033[1;37mѡ����� ����[\033[1;33mV\033[1;37m]/���[\033[1;33mC\033[1;37m]/�޸�[\033[1;33mE\033[1;37m]/����[Q] [Q]: \033[m",
            ans,2,DOECHO,NULL,true);
        if(ans[0]=='v'||ans[0]=='V'){
            if(stat(datafile,&st)||!S_ISREG(st.st_mode)||(fd=open(datafile,O_RDONLY,0644))==-1){
                move(4,0);
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","��ѡ���û�δ�趨Ϊ�ɷ����ض��������վ...");
                WAIT_RETURN;
                continue;
            }
            lc.l_type=F_RDLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            if(fcntl(fd,F_SETLKW,&lc)==-1){
                close(fd);
                continue;
            }
            p=mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
            lc.l_type=F_UNLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            fcntl(fd,F_SETLKW,&lc);
            close(fd);
            if(p==MAP_FAILED)
                continue;
            for(off=0,count=0,i=0;i<MAXBOARD;i++){
                if(!((i>>3)<st.st_size))
                    break;
                if(((unsigned char*)p)[i>>3]&(1<<(i&0x07))){
                    off=(count%(t_lines-7));
                    if(!off){
                        if(count){
                            move(t_lines-2,0);
                            prints("%s","\033[1;37m�� \033[1;32m<Space>\033[1;37m ���������Ļ� \033[1;32m<Enter>\033[1;37m ������: \033[m");
                            do{
                                ans[0]=igetch();
                                if(ans[0]==10||ans[0]==13){
                                    ans[1]=0;
                                    break;
                                }
                                if(ans[0]==32){
                                    ans[1]=1;
                                    break;
                                }
                            }
                            while(1);
                            if(!ans[1])
                                break;
                        }
                        move(4,0);
                        clrtobot();
                    }
                    if(!(board=getboard(i+1))||!(board->filename[0])||st.st_mtime<board->createtime)
                        continue;
                    move(4+off,0);
                    sprintf(buf,"\033[1;37m[%03d] \033[1;%dm%-32.32s%s\033[m",count+1,(!(count%2)?36:33),
                        board->filename,&(board->title[13]));
                    prints("%s",buf);
                    count++;
                }
            }
            munmap(p,st.st_size);
            if(!off&&(count-1)&&!ans[1])
                continue;
            move(t_lines-2,0);
            clrtoeol();
            prints("%s","\033[1;37m�� \033[1;32m<Enter>\033[1;37m ������: \033[m");
            WAIT_RETURN;
        }
        else if(ans[0]=='c'||ans[0]=='C'){
            getdata(3,0,"\033[1;31mȷ��ɾ����ѡ���û������ض��������վ�б� [y/N]: \033[m",ans,2,DOECHO,NULL,true);
            if(!(ans[0]=='y'||ans[0]=='Y'))
                continue;
            if(stat(datafile,&st)||!S_ISREG(st.st_mode)||(fd=open(datafile,O_RDONLY,0644))==-1){
                move(4,0);
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","��ѡ���û�δ�趨Ϊ�ɷ����ض��������վ...");
                WAIT_RETURN;
                continue;
            }
            lc.l_type=F_RDLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            if(fcntl(fd,F_SETLKW,&lc)==-1){
                close(fd);
                continue;
            }
            p=mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
            lc.l_type=F_UNLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            fcntl(fd,F_SETLKW,&lc);
            close(fd);
            if(p==MAP_FAILED)
                continue;
            sprintf(fn,"tmp/set_board_delete_read_perm_log_%ld_%d",time(NULL),(int)getpid());
            if(!(fp=fopen(fn,"w"))){
                munmap(p,st.st_size);
                continue;
            }
            sprintf(buf,"ɾ�� %s �����ض��������վ�б�",user->userid);
            fprintf(fp,"%s","\033[1;33m[ԭ����Ȩ���б�]\033[m\n\n");
            for(i=0;i<MAXBOARD;i++){
                if(!((i>>3)<st.st_size))
                    break;
                if(((unsigned char*)p)[i>>3]&(1<<(i&0x07))){
                    if(!(board=getboard(i+1))||!(board->filename[0])||st.st_mtime<board->createtime)
                        continue;
                    fprintf(fp,"%s\n",board->filename);
                }
            }
            fclose(fp);
            munmap(p,st.st_size);
            unlink(datafile);
            newbbslog(BBSLOG_USER,"board_delete_read_perm: delete <%s>",user->userid);
            post_file(getCurrentUser(),"",fn,"syssecurity",buf,0,2,getSession());
            unlink(fn);
            move(5,0);
            if(stat(datafile,&st)||!S_ISREG(st.st_mode))
                prints("\033[1;33m%s\033[0;33m<Enter>\033[m","ɾ���ɹ�!");
            else
                prints("\033[1;31m%s\033[0;33m<Enter>\033[m","ɾ��ʧ��...");
            WAIT_RETURN;
        }
        else if(ans[0]=='e'||ans[0]=='E'){
            sprintf(fn,"tmp/set_board_delete_read_perm_data_%ld_%d",time(NULL),(int)getpid());
            if(stat(datafile,&st)||!S_ISREG(st.st_mode)||(fd=open(datafile,O_RDONLY,0644))==-1)
                unlink(fn);
            else{
                lc.l_type=F_RDLCK;
                lc.l_whence=SEEK_SET;
                lc.l_start=0;
                lc.l_len=0;
                lc.l_pid=0;
                if(fcntl(fd,F_SETLKW,&lc)==-1){
                    close(fd);
                    continue;
                }
                p=mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
                lc.l_type=F_UNLCK;
                lc.l_whence=SEEK_SET;
                lc.l_start=0;
                lc.l_len=0;
                lc.l_pid=0;
                fcntl(fd,F_SETLKW,&lc);
                close(fd);
                if(p==MAP_FAILED)
                    continue;
                if(!(fp=fopen(fn,"w"))){
                    munmap(p,st.st_size);
                    continue;
                }
                for(i=0;i<MAXBOARD;i++){
                    if(!((i>>3)<st.st_size))
                        break;
                    if(((unsigned char*)p)[i>>3]&(1<<(i&0x07))){
                        if(!(board=getboard(i+1))||!(board->filename[0])||st.st_mtime<board->createtime)
                            continue;
                        fprintf(fp,"%s\n",board->filename);
                    }
                }
                fclose(fp);
                munmap(p,st.st_size);
            }
            saveline(2,0,NULL);
            modify_user_mode(EDITANN);
            vedit(fn,0,NULL,NULL,0);
            modify_user_mode(ADMIN);
            clear();
            move(0,0);
            prints("\033[1;32m%s\033[m","[�趨վ����������ض��������վȨ��]");
            move(0,40);
            prints("\033[1;33m<%s>\033[m",user->userid);
            saveline(2,1,NULL);
            getdata(3,0,"\033[1;31mȷ���޸���ѡ���û������ض��������վ�б� [y/N]: \033[m",ans,2,DOECHO,NULL,true);
            if(!(ans[0]=='y'||ans[0]=='Y')){
                unlink(fn);
                continue;
            }
            if(!(p=malloc(((MAXBOARD>>3)+1)*sizeof(unsigned char)))){
                unlink(fn);
                continue;
            }
            memset(p,0,((MAXBOARD>>3)+1)*sizeof(unsigned char));
            if(!(fp=fopen(fn,"r"))){
                free(p);
                unlink(fn);
                continue;
            }
            while(fgets(buf,256,fp)){
                buf[strlen(buf)-1]=0;
                trimstr(buf);
                if(!(i=getbid(buf,NULL)))
                    continue;
                i--;
                ((unsigned char*)p)[i>>3]|=(1<<(i&0x07));
            }
            fclose(fp);
            unlink(fn);
            if((fd=open(datafile,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1){
                free(p);
                continue;
            }
            lc.l_type=F_WRLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            if(fcntl(fd,F_SETLKW,&lc)==-1){
                close(fd);
                free(p);
                continue;
            }
            for(ret=0,ptr=p,off=((MAXBOARD>>3)+1)*sizeof(unsigned char);off>0&&ret!=-1;ptr=(char *)ptr + ret,off-=ret)
                ret=write(fd,ptr,off);
            lc.l_type=F_UNLCK;
            lc.l_whence=SEEK_SET;
            lc.l_start=0;
            lc.l_len=0;
            lc.l_pid=0;
            fcntl(fd,F_SETLKW,&lc);
            close(fd);
            sprintf(fn,"tmp/set_board_delete_read_perm_log_%ld_%d",time(NULL),(int)getpid());
            if(!(fp=fopen(fn,"w"))){
                free(p);
                continue;
            }
            sprintf(buf,"�޸� %s �����ض��������վ�б�",user->userid);
            fprintf(fp,"%s","\033[1;33m[�·���Ȩ���б�]\033[m\n\n");
            for(mtime=time(NULL),i=0;i<MAXBOARD;i++){
                if(((unsigned char*)p)[i>>3]&(1<<(i&0x07))){
                    if(!(board=getboard(i+1))||!(board->filename[0])||mtime<board->createtime)
                        continue;
                    fprintf(fp,"%s\n",board->filename);
                }
            }
            fclose(fp);
            free(p);
            newbbslog(BBSLOG_USER,"board_delete_read_perm: edit <%s>",user->userid);
            post_file(getCurrentUser(),"",fn,"syssecurity",buf,0,2,getSession());
            unlink(fn);
            move(5,0);
            prints("\033[1;33m%s\033[0;33m<Enter>\033[m","�޸ĳɹ�!");
            WAIT_RETURN;
        }
        else
            break;
    }
    return 0;
}


//Add by Marvel
int gen_x_deny_notify(char *level, char *uid, int deny)
{
	char filename[256];
	char rbuff[512], buffer[128];
	struct userec saveuser;
	struct userec *saveptr;
	FILE *fn;
	int i;
	time_t now;

	now = time(NULL);

	gettmpfilename(filename, "deny");
	
	fn = fopen(filename, "w+");
	memcpy(&saveuser, getCurrentUser(), sizeof(struct userec));
	saveptr = getCurrentUser();
	getCurrentUser() = &saveuser;
	if (deny)
	{
		sprintf(buffer, "%s����� %s ", uid, level);
		rbuff[0] = 0;
		while (!rbuff[0])
		{
			move(0, 0);
			char genbuf[4];
			clear();
			prints("�������� %s �� %s �����ɣ�CTRL + Q ����", uid, level);
			i = multi_getdata(1, 0, 79, NULL, rbuff, 512, 11, false, 0);
			if ( rbuff[0] ==0 )
			{
				getdata(i + 2, 0, "��û������������,��������������������? (Y)�ǵ�, �������� (N)�������������� [Y]: ", genbuf, 2, DOECHO, NULL, 1);
				if (genbuf[0] == 'y' || genbuf[0] == 'Y')
					continue;
				else
					break;
			}
		}
	}
	else
		sprintf(buffer, "%s����� %s ", uid, level);
	
	fprintf(fn, "������: SYSOP (System Operator) \n");
	fprintf(fn, "��  ��: %s\n", buffer);
	fprintf(fn, "����վ: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
	fprintf(fn, "��  Դ: %s\n", NAME_BBS_ENGLISH);
	fprintf(fn, "\n");

	if (deny)
	{
		fprintf(fn, "\n���˵��:\n\n");
		fprintf(fn, "\x1b[4m%s\x1b[m \n\n�Һ��ź���֪ͨ������� \x1b[4m%s\x1b[m ��ȡ����!\n", rbuff, level);
		fprintf(fn, "������Է�����κ����飬�뵽 Complain �����ߡ� \n");
		fprintf(fn, "������ϸ�Ķ���վrules�����վ�棬���ڵ��ں���complain�������⡣ \n ");
	}
	else
	{
		fprintf(fn, "�ǳ����˵�֪ͨ�������  \x1b[4m%s\x1b[m �ѱ��ָ���\n", level);
		fprintf(fn, "ϣ������ϸ�Ķ���վrules����ع涨�������ٴ��ܵ����ƵĴ�����\n");
	}
	fprintf(fn, "\n");
	
	fprintf(fn, 
		"                            " NAME_BBS_CHINESE NAME_SYSOP_GROUP "ֵ��վ��\x1b[4m%s\x1b[m\n", 
		getCurrentUser()->userid);
	fprintf(fn, "                              %s\n", ctime(&now));
	strcpy(getCurrentUser()->userid, "SYSOP");
	strcpy(getCurrentUser()->username, NAME_SYSOP);
	fclose(fn);
	mail_file(getCurrentUser()->userid, filename, uid, buffer, 0, NULL);
	
	fn = fopen(filename, "w+");
	if (deny)
		fprintf(fn, "\n���˵��:\n\n\x1b[4m%s\x1b[m\n\n��������ԭ��, \x1b[4m%s\x1b[4m �� \x1b[4m%s\x1b[m ��ȡ�����뵽complain�����߻������������\n\n", rbuff, uid, level);
	else
		fprintf(fn, "\n\x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m ���ָ�! \n\n", uid, level);

	fprintf(fn, "                            " NAME_BBS_CHINESE NAME_SYSOP_GROUP "ֵ��վ��\x1b[4m%s\x1b[m\n", saveptr->userid);
	fprintf(fn, "                              %s\n", ctime(&now));
    fclose(fn);
	sprintf(rbuff, "�����桿%s", buffer);
    post_file(getCurrentUser(), "", filename, "Announce", rbuff, 0, 2, getSession());
	getCurrentUser() = saveptr;
	unlink(filename);
	return 0;
}
