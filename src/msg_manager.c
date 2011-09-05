#include "bbs.h"
#if HAVE_MYSQL_SMTH == 1
#include "mysql.h"

#ifdef SMS_SUPPORT

/******************************************************
 * mysql �ṹ:
 *
 * mysql> desc smsmsg;
 * +-----------+--------------------+------+-----+---------+----------------+
 * | Field     | Type               | Null | Key | Default | Extra          |
 * +-----------+--------------------+------+-----+---------+----------------+
 * | id        | int(10)            |      | PRI | NULL    | auto_increment |
 * | userid    | varchar(13) binary |      | MUL |         |                |
 * | dest      | varchar(13)        |      | MUL |         |                |
 * | timestamp | timestamp(14)      | YES  | MUL | NULL    |                |
 * | type      | int(11)            |      |     | 0       |                |
 * | context   | text               | YES  | MUL | NULL    |                |
 * | level     | int(11)            | YES  |     | NULL    |                |
 * | readed    | int(1)             |      | MUL | 0       |                |
 * +-----------+--------------------+------+-----+---------+----------------+
 *
 *                                                               stiger
 ******************************************************/

struct smsmsg * s_m;
int sm_num=0;
//int sm_start=0;
char sm_dest[13];
int sm_type=-1;
char sm_msgtxt[30];
int sm_desc=0;

static int set_smsg_select(struct _select_def *conf)
{
	clear();
	move(0,0);
	prints("%s����:%s   ʱ��:%s", s_m[conf->pos-conf->page_pos].type?"��":"��", s_m[conf->pos-conf->page_pos].dest, s_m[conf->pos-conf->page_pos].time);
	prints("\n����:\n");
	outline(s_m[conf->pos-conf->page_pos].context);
	pressanykey();

	if( ! s_m[conf->pos-conf->page_pos].readed ){
		if( sign_smsmsg_read( s_m[conf->pos-conf->page_pos].id , getSession())){
			s_m[conf->pos-conf->page_pos].readed = 1;
		}
	}

	return SHOW_REFRESH;
}

static int set_smsg_show(struct _select_def *conf, int i)
{
	char title[41];
	char *c;

	if(strlen(s_m[i-conf->page_pos].context) > 40){
		strncpy(title, s_m[i-conf->page_pos].context, 37);
		title[37]='.';
		title[38]='.';
		title[39]='.';
	}else{
		strcpy(title, s_m[i-conf->page_pos].context);
	}
	title[40]=0;

	if((c = strchr(title, '\n') )!= NULL) *c=0;
	if((c = strchr(title, '\r') )!= NULL) *c=0;

	prints(" %s%-3d %-13s %-14s %s %-40s%s",s_m[i-conf->page_pos].readed?"":"\033[1;5m", i , s_m[i-conf->page_pos].dest, s_m[i-conf->page_pos].time, s_m[i-conf->page_pos].type?"\033[1;32m��\033[m":"\033[1;33m��\033[m", title, s_m[i-conf->page_pos].readed?"":"\033[m");
	return SHOW_CONTINUE;
}

static int set_smsg_prekey(struct _select_def *conf, int *key)
{
	switch (*key) {
			/*
	case KEY_PGDN:
	case KEY_DOWN:
	{
		if( *key == KEY_DOWN ){
			if( conf->pos != BBS_PAGESIZE )
				break;
		}
		if( sm_num >= BBS_PAGESIZE ){
			sm_start += sm_num - 1;
			if( *key == KEY_DOWN )
				conf->pos = 1;
			return SHOW_DIRCHANGE;
		}
		else if( conf->pos != sm_num ){
			conf->new_pos = sm_num;
			return SHOW_SELCHANGE;
		}
		break;
	}
	case KEY_PGUP:
	case KEY_UP:
	{
		if( *key == KEY_UP ){
			if( conf->pos != 1)
				break;
		}
		if( sm_start > 0 ){
			sm_start -= BBS_PAGESIZE;
			if(sm_start < 0)
				sm_start = 0;
			if( *key == KEY_UP )
				conf->pos = BBS_PAGESIZE;
			return SHOW_DIRCHANGE;
		}else if( conf->pos != 1){
			conf->new_pos = 1;
			return SHOW_SELCHANGE;
		}
		break;
	}*/
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

static int set_smsg_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[���Ź���]","����[\033[1;32mh\033[m] ɾ��[\033[1;32md\033[m] ѡ��[\033[1;32ms\033[m] �ظ�����[\033[1;32mS\033[m] ���Ķ���Ϣǰ/��׺[\033[1;32mz\033[m]");
	move(2,0);
	prints("\033[0;1;37;44m  %-4s %-13s %-14s %-2s %-40s\033[m","���","����","ʱ��", "��", "����");
	update_endline();
	return SHOW_CONTINUE;
}

static int set_smsg_getdata(struct _select_def *conf,int pos,int len)
{
	int i;

	for(i=0;i<sm_num;i++){
		if( s_m[i].context ) free(s_m[i].context);
	}
	bzero( s_m, sizeof(struct smsmsg) * BBS_PAGESIZE );

	if( conf->item_count - conf->page_pos < BBS_PAGESIZE )
		conf->item_count = count_sql_smsmsg( getCurrentUser()->userid, sm_dest, 0, 0, sm_type, 0, sm_msgtxt, getSession());

	i = get_sql_smsmsg(s_m, getCurrentUser()->userid, sm_dest, 0, 0, sm_type, 0, conf->page_pos-1, BBS_PAGESIZE,sm_msgtxt,sm_desc, getSession());

	if( i <= 0){

		conf->page_pos = 0;
		sm_dest[0]=0;
		sm_type = -1;
		sm_msgtxt[0]=0;
		
		i = get_sql_smsmsg(s_m, getCurrentUser()->userid, sm_dest, 0, 0, sm_type, 0, conf->page_pos-1, BBS_PAGESIZE,sm_msgtxt,sm_desc, getSession());

		if(i <= 0)
			return SHOW_QUIT;
	}

	return SHOW_CONTINUE;
}


static int set_smsg_key(struct _select_def *conf, int key)
{
	switch(key){
	case 'd':
	{
		char sql[100];
		MYSQL s;
		char ans[4];

        move(2,0);
		clrtoeol();
		ans[0]=0;
        getdata(2, 0, "ɾ��������Ϣ(Y/N) [N]: ", ans, 3, DOECHO, NULL, true);
		if(ans[0] != 'y' && ans[0]!='Y')
			return SHOW_REFRESH;

		mysql_init(&s);
		if (! my_connect_mysql(&s) ){
			clear();
			prints("%s\n",mysql_error(&s));
			pressanykey();
			mysql_close(&s);
			return SHOW_REFRESH;
		}

                sprintf(sql,"DELETE FROM smsmsg WHERE id=%d;",s_m[conf->pos-conf->page_pos].id);


		if( mysql_real_query( &s, sql, strlen(sql) ) ){
			clear();
			prints("%s\n",mysql_error(&s));
			pressanykey();
			mysql_close(&s);
			return SHOW_REFRESH;
		}

		mysql_close(&s);

		return SHOW_DIRCHANGE;
	}
	case 's':
	{
		char ans[40];

		clear();
		move(0,0);
		ans[0]=0;
		prints("                                 ��������ѡ��\n");
		prints("\033[1;31m------------------------------------------------------------------------\033[m\n");
        getdata(2, 0, "ѡ��ȫ�������밴\033[1;32m1\033[m,��������ѡ���밴\033[1;32m2\033[m,ȡ��ֱ�ӻس�(1/2/0) [0]: ", ans, 3, DOECHO, NULL, true);
		if( ans[0] == '1' ){
			conf->page_pos = 0;
			sm_dest[0]=0;
			sm_type = -1;
			sm_msgtxt[0]=0;
			return SHOW_DIRCHANGE;
		}else if(ans[0] == '2'){
			move(3,0);
			getdata(3,0,"������Ҫѡ��Ķ�����Դ(�س�ѡ������):",ans,15, DOECHO,NULL,true);
			strncpy(sm_dest, ans, 13);
			sm_dest[12]=0;

			move(4,0);
			getdata(4,0,"������Ҫѡ��Ķ������(1��ʾ��,2��ʾ��,�س�����) [0]:",ans,3, DOECHO,NULL,true);
			if( ans[0] == '1' )
				sm_type = 1;
			else if(ans[0] == '2')
				sm_type = 0;
			else sm_type = -1;

			move(5,0);
			getdata(5,0,"�����뿪ʼ��ʾ�Ķ������ [0]:",ans,5, DOECHO,NULL,true);
			conf->page_pos = atoi(ans);
			if(conf->page_pos <= 0) conf->page_pos=1;
			conf->pos=conf->page_pos;

			move(6,0);
			getdata(6,0,"������Ҫ�������ݰ�������(�س�ѡ������):",ans,21, DOECHO,NULL,true);
			strncpy(sm_msgtxt, ans, 21);
			sm_msgtxt[20]=0;

			return SHOW_DIRCHANGE;
		}else{
			return SHOW_REFRESH;
		}
		break;
	}
	case 'a':
	{
		conf->page_pos = 0;
		sm_dest[0]=0;
		sm_type = -1;
		sm_msgtxt[0]=0;
		return SHOW_DIRCHANGE;
	}
	case 'S':
	{
		if( ! isdigit(s_m[conf->pos-conf->page_pos].dest[0]) )
			return SHOW_CONTINUE;
		clear();
		do_send_sms_func(s_m[conf->pos-conf->page_pos].dest, NULL);
		pressanykey();
		return SHOW_REFRESH;
	}
	case 'r':
	{
		sm_desc = ! sm_desc;
		return SHOW_DIRCHANGE;
	}
	case 'z':
	{
		char ans[42];

		clear();
		prints("�޸Ķ���Ϣǰ׺/��׺,��Щ���ڷ��͵Ķ���Ϣ����ǰ/����ʾ,ռ����Ϣ�ֽ�");

		if(getSession()->currentmemo->ud.smsprefix[0])
			strcpy(ans, getSession()->currentmemo->ud.smsprefix);
		else
			ans[0]=0;
		move(2,0);
		prints("�������µ�ǰ׺:");
		multi_getdata(3, 0, 79, NULL, ans, 41, 6, false, 0);
		if(ans[0]){
			strncpy(getSession()->currentmemo->ud.smsprefix, ans, 40);
			getSession()->currentmemo->ud.smsprefix[40]=0;
		}else
			getSession()->currentmemo->ud.smsprefix[0]=0;

		if(getSession()->currentmemo->ud.smsend[0])
			strcpy(ans, getSession()->currentmemo->ud.smsend);
		else
			ans[0]=0;
		move(10,0);
		prints("�������µĺ�׺:");
		multi_getdata(11, 0, 79, NULL, ans, 41, 6, false, 0);
		if(ans[0]){
			strncpy(getSession()->currentmemo->ud.smsend, ans, 40);
			getSession()->currentmemo->ud.smsend[40]=0;
		}else
			getSession()->currentmemo->ud.smsend[0]=0;

		write_userdata( getCurrentUser()->userid, &(getSession()->currentmemo->ud) );

		move(18,0);
		prints("�޸ĳɹ�");
		pressreturn();

		return SHOW_REFRESH;
	}
	case 'h':
	{
		clear();
		prints("           ���Ź���������\n\n");
		prints("   d   ɾ������\n");
		prints("   s   ����ѡ��\n");
		prints("   a   ��ʾ���ж���\n");
		prints("   S   �ظ�����\n");
		prints("   r   ��������\n");
		prints("   z   ���Ķ���ǰ/��׺\n");
		pressreturn();

		return SHOW_REFRESH;
	}
	default:
		break;
	}
	return SHOW_CONTINUE;
}

int smsmsg_read(void){
	struct _select_def group_conf;
	int i;
	POINT *pts;
	
	sm_dest[0]=0;

	s_m = (struct smsmsg *) malloc( sizeof(struct smsmsg) * BBS_PAGESIZE );
	if( s_m == NULL)
		return -1;

	bzero(&group_conf,sizeof(struct _select_def));

    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
	group_conf.item_per_page = BBS_PAGESIZE;
    group_conf.flag = LF_VSCROLL | LF_BELL | LF_MULTIPAGE | LF_LOOP;
    group_conf.prompt = "��";
    group_conf.item_pos = pts;
	group_conf.title_pos.x = 0;
	group_conf.title_pos.y = 0;
	group_conf.pos=1;
	group_conf.page_pos=1;

	group_conf.on_select = set_smsg_select;
	group_conf.show_data = set_smsg_show;
	group_conf.pre_key_command = set_smsg_prekey;
	group_conf.show_title = set_smsg_refresh;
	group_conf.get_data = set_smsg_getdata;
	group_conf.key_command = set_smsg_key;


	bzero( s_m, sizeof(struct smsmsg) * BBS_PAGESIZE );
	group_conf.item_count = count_sql_smsmsg( getCurrentUser()->userid, NULL, 0, 0, -1, 0, NULL, getSession());
	i = get_sql_smsmsg(s_m, getCurrentUser()->userid, NULL, 0, 0, -1, 0, 0, BBS_PAGESIZE, NULL,sm_desc, getSession());
	
	if(i <= 0){
		free(s_m);
		return -1;
	}

	clear();
	list_select_loop(&group_conf);

	for(i=0;i<sm_num;i++){
		if( s_m[i].context ) free(s_m[i].context);
	}

	free(pts);
	free(s_m);
	s_m = NULL;

	chk_smsmsg(1, getSession());

	return 1;
}

#endif
#endif
