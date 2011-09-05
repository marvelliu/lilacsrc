#include "service.h"
#include "bbs.h"

/*--------------------------------------------------------------------------*/
/*  С�������趨                      					    */
/*--------------------------------------------------------------------------*/
struct chicken {
/*---����������---*///14
	char name[20];		/*��    �� */
	char birth[21];		/*��    �� */
	int year;		/*����  �� */
	int month;		/*����  �� */
	int day;		/*����  �� */
	int sex;		/*��    �� 1:��   2:��  */
	int death;		/*1:  ���� 2:���� 3:��� */
	int nodone;		/*1:  δ�� */
	int relation;		/*���˹�ϵ */
	int liveagain;		/*������� */
	int dataB;
	int dataC;
	int dataD;
	int dataE;

/*---����Ĳ���---*///12
	int hp;			/*��    �� */
	int maxhp;		/*������� */
	int weight;		/*��    �� */
	int tired;		/*ƣ �� �� */
	int sick;		/*��    �� */
	int shit;		/*�� �� �� */
	int wrist;		/*��    �� */
	int bodyA;
	int bodyB;
	int bodyC;
	int bodyD;
	int bodyE;

/*---���۵Ĳ���---*///9
	int social;		/*�罻���� */
	int family;		/*�������� */
	int hexp;		/*ս������ */
	int mexp;		/*ħ������ */
	int tmpA;
	int tmpB;
	int tmpC;
	int tmpD;
	int tmpE;

/*---ս���ò���---*///13
	int mp;			/*��    �� */
	int maxmp;		/*����� */
	int attack;		/*�� �� �� */
	int resist;		/*�� �� �� */
	int speed;		/*��    �� */
	int hskill;		/*ս������ */
	int mskill;		/*ħ������ */
	int mresist;		/*��ħ���� */
	int magicmode;		/*ħ����̬ */
	int fightB;
	int fightC;
	int fightD;
	int fightE;

/*---�����Ĳ���---*///10
	int weaponhead;		/*ͷ������ */
	int weaponrhand;	/*�������� */
	int weaponlhand;	/*�������� */
	int weaponbody;		/*�������� */
	int weaponfoot;		/*�ŵ����� */
	int weaponA;
	int weaponB;
	int weaponC;
	int weaponD;
	int weaponE;

/*---����������---*///17
	int toman;		/*���˽��� */
	int character;		/*�� �� �� */
	int love;		/*��    �� */
	int wisdom;		/*��    �� */
	int art;		/*�������� */
	int etchics;		/*��    �� */
	int brave;		/*��    �� */
	int homework;		/*ɨ��ϴ�� */
	int charm;		/*��    �� */
	int manners;		/*��    �� */
	int speech;		/*̸    �� */
	int cookskill;		/*��    � */
	int learnA;
	int learnB;
	int learnC;
	int learnD;
	int learnE;

/*---��״̬��ֵ---*///11
	int happy;		/*�� �� �� */
	int satisfy;		/*�� �� �� */
	int fallinlove;		/*����ָ�� */
	int belief;		/*��    �� */
	int offense;		/*��    �� */
	int affect;		/*��    �� */
	int stateA;
	int stateB;
	int stateC;
	int stateD;
	int stateE;

/*---�ԵĶ�����---*///9
	int food;		/*ʳ    �� */
	int medicine;		/*��    ֥ */
	int bighp;		/*�� �� �� */
	int cookie;		/*��    ʳ */
	int ginseng;		/*ǧ���˲� */
	int snowgrass;		/*��ɽѩ�� */
	int eatC;
	int eatD;
	int eatE;

/*---ӵ�еĶ���---*///8
	int book;		/*��    �� */
	int playtool;		/*��    �� */
	int money;		/*��    Ǯ */
	int thingA;
	int thingB;
	int thingC;
	int thingD;
	int thingE;

/*---��ȭ�Ĳ���---*///2
	int winn;
	int losee;

/*---�μ�����--*///16
	int royalA;		/*from���� */
	int royalB;		/*from���� */
	int royalC;		/*from���� */
	int royalD;		/*from�� */
	int royalE;		/*from��˾ */
	int royalF;		/*from���� */
	int royalG;		/*from���� */
	int royalH;		/*from���� */
	int royalI;		/*fromС�� */
	int royalJ;		/*from���� */
	int seeroyalJ;		/*�Ƿ��Ѿ����������� */
	int seeA;
	int seeB;
	int seeC;
	int seeD;
	int seeE;

/*---���----*///2
	int wantend;		/*20���� */
	/*1:��Ҫ��δ�� 2:��Ҫ���ѻ�  3:��Ҫ�ҵ������� 4:Ҫ��δ��  5:Ҫ���ѻ� 6:Ҫ�ҵ������� */
	int lover;		/*���� */
	/*0:û�� 1:ħ�� 2:���� 3:A 4:B 5:C 6:D 7:E */

 /*-------��������--------*/
	int workA;		/*���� */
	int workB;		/*��ķ */
	int workC;		/*�õ� */
	int workD;		/*ũ�� */
	int workE;		/*���� */
	int workF;		/*���� */
	int workG;		/*��̯ */
	int workH;		/*��ľ */
	int workI;		/*���� */
	int workJ;		/*���� */
	int workK;		/*���� */
	int workL;		/*��Ĺ */
	int workM;		/*�ҽ� */
	int workN;		/*�Ƽ� */
	int workO;		/*�Ƶ� */
	int workP;		/*ҹ */
	int workQ;
	int workR;
	int workS;
	int workT;
	int workU;
	int workV;
	int workW;
	int workX;
	int workY;
	int workZ;

 /*-------�Ͽδ���--------*/
	int classA;
	int classB;
	int classC;
	int classD;
	int classE;
	int classF;
	int classG;
	int classH;
	int classI;
	int classJ;
	int classK;
	int classL;
	int classM;
	int classN;
	int classO;

/*---С����ʱ��---*///1
	int bbtime;
};
typedef struct chicken chicken;

/*--------------------------------------------------------------------------*/
/*  ѡ�����趨                                                              */
/*--------------------------------------------------------------------------*/
struct pipcommands {
	int (*fptr) ();
	int key1;
	int key2;
};
typedef struct pipcommands pipcommands;

/*--------------------------------------------------------------------------*/
/*  ��������趨                                                            */
/*--------------------------------------------------------------------------*/
struct playrule {
	//char *name;           /*����*/  
	char name[20];
	int hp;			/*���� */
	int maxhp;
	 /*MAXHP*/ int mp;	/*���� */
	int maxmp;
	 /*MAXMP*/ int attack;	/*���� */
	int resist;		/*���� */
	int speed;		/*�ٶ� */
	int money;		/*�Ʊ� */
	char special[6];	/*�ر� *//*1:�»� 2:�±� */
	int map;		/*ͼ�� */
	int death;		/*���� */
};
typedef struct playrule playrule;

/* d.special�Ķ���            */
/* 00000 ��һϵ��ħ���˺���С */
/* 10000 ��һϵ��ħ���˺��ϴ� */
/* 11000 һ��ϵ��ħ���˺��ϴ� */
/* ��������                    */
/* ��ϵ ��ϵ ��ϵ ��ϵ ��ϵ */
/*struct playrule goodmanlist[] = {
}*/

#define PIP_BADMAN       (3)	/* �����������  */

/*--------------------------------------------------------------------------*/
/*  ħ�������趨                                                            */
/*--------------------------------------------------------------------------*/
struct magicset {
	//char *name;           /*ħ��������*/ 
	char name[20];
	int needmp;		/*��Ҫ�ķ��� */
	int hpmode;		/*0:���� 1:���� 2:���ֵ */
	int hp;			/*ɱ�������� */
	int tiredmode;		/*0:���� 1:���� 2:��Сֵ */
	int tired;		/*ƣ�Ͷȸı� */
	int map;		/*ͼ�� */
};
typedef struct magicset magicset;

/*--------------------------------------------------------------------------*/
/*  �μ����������趨                                                        */
/*--------------------------------------------------------------------------*/
struct royalset {
	//char *num;          /*����*/
	//char *name;           /*����������*/ 
	char num[2];
	char name[20];
	int needmode;		/*��Ҫ��mode *//*0:����Ҫ 1:���� 2:̸�� */
	int needvalue;		/*��Ҫ��value */
	int addtoman;		/*���������� */
	int maxtoman;		/*����� */
	//char *words1;
	//char *words2;
	char words1[40];
	char words2[40];
};
typedef struct royalset royalset;

struct goodsofpip {
    int num;        /*��� */
    char *name;     /*���� */
    char *msgbuy;       /*���� */
    char *msguse;       /*˵�� */
    int money;      /*��Ǯ */
    int change;     /*�ı��� */
    int pic1;
    int pic2;
};
typedef struct goodsofpip goodsofpip;

struct weapon {
    char *name;     /*���� */
    int needmaxhp;      /*��Ҫhp */
    int needmaxmp;      /*��Ҫmp */
    int needspeed;      /*��Ҫ��speed */
    int attack;     /*���� */
    int resist;     /*���� */
    int speed;      /*�ٶ� */
    int cost;       /*��� */
    int sell;       /*���� */
    int special;        /*�ر� */
    int map;        /*ͼ�� */
};
typedef struct weapon weapon;

struct newendingset {
    char *girl;     /*Ů����ֵ�ְҵ */
    char *boy;      /*������ֵ�ְҵ */
    int grade;      /*���� */
};
typedef struct newendingset newendingset;

#undef MAPLE /* etnlegend, 2006.04.08, ��������˰�... */

#define MSG_UID "������ʹ���ߴ��ţ�"
#define ERR_UID "����û�����������"
#define b_lines t_lines-1
SMTH_API int t_lines;

int show_system_pic(int);
void temppress(const char*);

int clrchyiuan(int,int);
int count_tired(int,int,char*,int,int);
int lose();
int pip_basic_feed();
int pip_buy_goods_new(int,const struct goodsofpip*,int*);
int pip_data_list();
int pipdie(const char*,int);
int pip_endingall_purpose(char*,int*,int*,int*,int);
int pip_endingart(char*,int*,int*,int*);
int pip_endingblack(char*,int*,int*,int*);
int pip_endingcombat(char*,int*,int*,int*);
int pip_ending_decide(char*,char*,char*,int*,int*);
int pip_endingfamily(char*,int*,int*,int*);
int pip_endingmagic(char*,int*,int*,int*);
int pip_ending_screen();
int pip_endingsocial(char*,int*,int*,int*);
int pip_fight_bad(int);
int pip_fight_main(int,const struct playrule*,int);
int pip_future_decide(int*);
int pip_game_over(int);
int pip_go_palace_screen(const struct royalset*);
int pip_live_again();
void pip_log_record(char*);
int pip_magic_doing_menu(const struct magicset*);
int pip_magic_menu();
int pip_mainmenu(int);
int pip_main_menu();
int pip_marriage_offer();
int pip_marry_decide();
int pip_max_worktime(int*);
int pip_new_game();
int pip_practice_function(int,int,int,int,int*,int*,int*,int*,int*);
int pip_practice_gradeup(int,int,int);
int pip_read(char*);
int pip_read_backup();
void pip_read_file();
int pip_results_show_ending(int,int,int,int,int);
int pip_time_change(time_t);
int pip_vs_man(int,const struct playrule*,int);
int pip_weapon_doing_menu(int,int,const struct weapon*);
void pip_write_file();
int show_badman_pic(int);
int show_basic_pic(int);
int show_buy_pic(int);
int show_die_pic(int);
int show_ending_pic(int);
int show_feed_pic(int);
int show_fight_pic(int);
int show_file(char*,int,int,int);
int show_guess_pic(int);
int show_job_pic(int);
int show_palace_pic(int);
int show_play_pic(int);
int show_practice_pic(int);
int show_resultshow_pic(int);
int show_special_pic(int);
int show_usual_pic(int);
int show_weapon_pic(int);
int situ();
int tie();
int win();

