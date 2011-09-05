#undef KILLER_DEBUG 

#define MAX_ROOM 10
#define MAX_GAMETYPE 10
#define MAX_PEOPLE 50
#define MAX_PLAYER 30
#define MAX_MSG 2000

#define MAX_KILLER 4
#define MAX_POLICE 4

#define ROOM_LOCKED 01
#define ROOM_SECRET 02
#define ROOM_DENYSPEC 04
#define ROOM_SPECBLIND 010

struct room_struct {
	int w;
	int style;				  /* 0 - chat room 1 - killer room */
	char name[14];
	char title[NAMELEN];
	int op;
	unsigned int level;
	int flag;
	int numplayer;
	int maxplayer;
	int numspectator;
};

#define PEOPLE_KILLER 0x1
#define PEOPLE_MAGIC  0x2
#define PEOPLE_FOREST 0x4
#define PEOPLE_BADMAN 0x8
#define PEOPLE_DOCTOR 0x10
#define PEOPLE_SHOOTER 0x20
#define PEOPLE_POLICE 0x40
#define PEOPLE_GOODMAN 0x80
#define PEOPLE_BUTTERFLY 0x100
#define PEOPLE_CROW 0x200

#define MAX_ROLE 20
#define MAX_DAY 20
#define S_KILLER 1
#define S_MAGIC  2
#define S_FOREST 3
#define S_BADMAN 4
#define S_DOCTOR 5
#define S_SHOOTER 6
#define S_POLICE 7
#define S_GOODMAN 8
#define S_BUTTERFLY 9
#define S_CROW 0

//幸好PEOPLE_BAD和PEOPLE_GOOD定义得比较晚
#define PEOPLE_BAD (PEOPLE_KILLER|PEOPLE_MAGIC|PEOPLE_FOREST|PEOPLE_BADMAN)
#define PEOPLE_SEENBAD (PEOPLE_KILLER|PEOPLE_MAGIC|PEOPLE_FOREST)
#define PEOPLE_GOOD (PEOPLE_DOCTOR|PEOPLE_SHOOTER|PEOPLE_POLICE|PEOPLE_GOODMAN|PEOPLE_BUTTERFLY)
#define PEOPLE_CARD (PEOPLE_BAD|PEOPLE_GOOD|PEOPLE_CROW)

#define PEOPLE_ALIVE 0x1000
#define PEOPLE_DENYSPEAK 0x2000
#define PEOPLE_VOTED  0x4000
#define PEOPLE_HIDEKILLER 0x8000
#define PEOPLE_SURRENDER 0x10000
#define PEOPLE_MULE 0x20000
#define PEOPLE_LASTWORDS 0x40000

#define PEOPLE_TESTED 0x80000

#define SIGN_KILLER "*"
#define SIGN_MAGIC "M"
#define SIGN_FOREST "F"
#define SIGN_BADMAN "B"
#define SIGN_BUTTERFLY ">"
#define SIGN_SHOOTER "!"
#define SIGN_POLICE "@"
#define SIGN_DOCTOR "+"
#define SIGN_GOODMAN "G"
#define SIGN_CROW "?"

struct people_struct {
	int style;
	char id[IDLEN + 2];
	char nick[NAMELEN];
	long flag;
	int pid;
	int vote;
	int vnum;
};

// 2晚上->3死者遗言->4指证->5辩护->6投票->7被关留言(被关者是杀手无留言)
#define INROOM_STOP 1
//晚上
#define INROOM_NIGHT 2
//白天
#define INROOM_DAY 3
//指证阶段
#define INROOM_CHECK 4
//辩护阶段
#define INROOM_DEFENCE 5
//投票阶段
#define INROOM_VOTE 6
//死者留遗言
#define INROOM_DARK 7
//早上的遗言
#define INROOM_DAWN 8

struct inroom_struct {
	int w;
	int status;
	int day;
	int draw_counter;
	int draw_check_day;
	int surrenderflag;
	
	int killernum;
	int policenum;
	int gametype; //1-> normal game, 2-> butterfly 9/10, 3-> butterfly traditional, 4-> tsinghua 
	int votetype; //1->双轮分票，2->单轮直投，平票不死，3->单轮直投，平票重投
	
	int victim;   //vote to die
	int action_target [MAX_PLAYER];
	int action_target_tmp [MAX_PLAYER];
	int deadlist [MAX_PLAYER];
	int deadnum;
	int deadpointer;
	int mulelist [MAX_PLAYER];
	int mulenum;
	int secretvotelist [MAX_PLAYER];
	int pinhole[MAX_PLAYER];
	int upgrade; //killer->killer:0, magic->killer:1, forest->killer:2
	int nokill;

	//day setting
        int turn; // 轮到某人发言
	int round; // round of vote

	char voted[MAX_PLAYER];
	int numvoted;
	
	struct people_struct peoples[MAX_PEOPLE];
	char msgs[MAX_MSG][100];
	int msgpid[MAX_MSG];
	int msgi;
	
	char killers[MAX_PLAYER];
	char polices[MAX_PLAYER];
	char crows[MAX_PLAYER];
	char special[MAX_ROLE];
	
	char seq_detect[MAX_PLAYER];
	char seq_action[MAX_ROLE][MAX_DAY];
	
	char gamename[80];
	time_t lastturntime;
	time_t lastactiontime;
	time_t lastautotime;
	int time_turnlimit;
	int time_actionlimit;
	int time_autolimit;
};
struct game_struct {
	int w;

	void (* start_game)();
	void (* vote_result)();
	int (* check_win)();

	void (* goto_night)();
	void (* goto_day)();
	void (* goto_check)();
	void (* goto_defence)();
	void (* goto_vote)();
	void (* goto_dark)();
	
	void (* ctrl_s_night)();
	void (* ctrl_s_day)();
	void (* ctrl_s_check)();
	void (* ctrl_s_defence)();
	void (* ctrl_s_vote)();
	void (* ctrl_s_dark)();

	void (* ctrl_t_night)();
	void (* ctrl_t_day)();
	void (* ctrl_t_check)();
	void (* ctrl_t_defence)();
	void (* ctrl_t_vote)();
	void (* ctrl_t_dark)();

	void (* ctrl_g_night)();
	void (* ctrl_g_day)();
	void (* ctrl_g_check)();
	void (* ctrl_g_defence)();
	void (* ctrl_g_vote)();
	void (* ctrl_g_dark)();

	void (* timeout_night)();
	void (* timeout_day)();
	void (* timeout_check)();
	void (* timeout_defence)();
	void (* timeout_vote)();
	void (* timeout_dark)();
};
	


	
struct room_struct *rooms;
struct inroom_struct *inrooms;
//static struct game_struct *thisgame;
//struct game_struct *gamesettinglist;

struct killer_record {	
	int w;					  //0 - 平民胜利 1 - 杀手胜利
	time_t t;
	int peoplet;
	char id[MAX_PLAYER][IDLEN + 2];
	int st[MAX_PLAYER];		 // 0 - 活着平民 1 - 死了平民 2 - 活着杀手 3 - 死了杀手 4 - 其他情况
};

int myroom, mypos;

#define RINFO inrooms[myroom]
#define PINFO(x) RINFO.peoples[x]

time_t lasttalktime;
int denytalk;

//strings 

#define MAX_FUDGE 5
#define MAX_POEM 10

char * fudge[MAX_FUDGE] = {"我是杀手！", "我发誓，我说的全都是真的！","我没有被森林老人禁言！",
				"法官大人，饶了我吧。", "大家都别走，我报告！"};
char * poem[MAX_POEM] = {"人生自古谁无死，留取丹心照汗青！", 
			"砍头不要紧，只要主义真！杀了我一个，还有后来人！",
			"生命诚可贵，爱情价更高，若为自由故，两者皆可抛！",
			"力拔山兮气盖世，时不利兮骓不逝，骓不逝兮...",
			"三十功名尘与土，八千里路云和月！",
			"两个黄鹂鸣翠柳，一行白鹭上青天。",
			"春眠不觉晓，处处闻啼鸟。",
			"白毛浮绿水，红掌拨青波。",
			"月明星稀，乌鹊南飞，绕树三匝，无枝可依...",
			"天高皇帝远，民少相公多。一日三遍打，不反待如何！"};



#define GAME_ORIGINAL 	1
#define GAME_CROW 	2
#define GAME_BUTTERFLY 	3
#define GAME_TSINGHUA 	4
#define GAME_JOKER1	5
#define GAME_JOKERPIG	6


//joker RINFO aliases
//
// 当使用扑克游戏的时候
// nokill为上一个出牌的人（争上游类型）或者第一个出牌的人（桥牌类型）
// turn为目前出牌人的序号，numvoted为出牌人的座位号(1-6,1-4)
// secretvotelist为从座位号得到序号的数组，mulelist为从序号得到座位号的数组
// seq_action为所有人的牌
// round为当前牌的类型，-1表示将领牌 victim为当前牌的大小
// deadnum为出完牌的人数， deadlist为出完牌的人员名单
// mulenum记录胜利一方
// day 记录人数


#define RDECKS			RINFO.seq_action
#define RDECKS_COUNT(x)		PINFO((x)).vnum
#define CURRENT_TABLEPOS	RINFO.numvoted
#define CURRENT_POS		RINFO.turn
#define CURRENT_LEADER		RINFO.nokill
#define CURRENT_JOKER_TYPE	RINFO.round
#define CURRENT_JOKER_VALUE	RINFO.victim
#define MAX_JOKER_PLAYER	RINFO.day
#define FINISH_LIST		RINFO.deadlist
#define FINISH_NUM		RINFO.deadnum
#define ONTABLE_LIST		RINFO.seq_detect
#define ONTABLE_TYPE_LIST	RINFO.pinhole
#define ONTABLE_NUM		RINFO.deadnum
#define POS_LIST		RINFO.secretvotelist
#define TABLE_POS_LIST		RINFO.mulelist
#define FINISH_WINNER		RINFO.mulenum
#define JOKER_VALUE_STYLE	RINFO.draw_counter
#define JOKER_VALUE_LEVEL	RINFO.upgrade
#define JOKER_VALID_STYLE	RINFO.draw_check_day
#define JOKER_TYPE_STYLE	RINFO.deadpointer

#define JOKERPIG_DOUBLE_FLAG 	RINFO.polices
#define JOKERPIG_POINTCARD_LIST 	RINFO.killers
#define JOKERPIG_DOUBLE_FIRSTROUND	RINFO.voted
#define JOKER_POINT_RECORD_TMP  RINFO.action_target_tmp
#define JOKER_POINT_RECORD  	RINFO.action_target


#define TIMEOUT_TURN	1
#define TIMEOUT_ACTION	2
#define TIMEOUT_AUTO	3


