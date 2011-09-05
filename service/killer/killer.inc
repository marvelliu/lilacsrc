//把函数的原型放这里

//refreshit.c
void refreshit_col2 (const int, const int , char * , int * );
void refreshit_col3 (const int , const int , char * , int * );
void refreshit_col7 (const int , const int , char * , int * );
void refreshit_cols_right(int , int * , int ,char * );
void refreshit_cols_fillblank (int , int * , char *);
void refreshit_cols_sn(int , int * , int , int , char* );
void refreshit_headlines();
void refreshit_cols_nick(int , int * , int , int , char* );
void refreshit();
//show_killerpolice.c
void show_kp_showseq_actionlist(char * , char *);
void show_kp_showrolelist(char * , char *);
void show_kp_showspecial(int, char *);
void show_kp_original();
void show_kp_crow();
void show_kp_butterfly();
void show_kp_tsinghua();
void show_killerpoilce();
//start_game.c
void start_game_crow();
void start_game_butterfly();
void start_game_original();
void start_game_tsinghua();
int start_game_give_card(long, char*);
int start_game_preparation();
int start_game_checktotal(int,int,int);
void start_game_autopost(int, int, int);

void start_game();
//goto_night.c
void goto_night_original();
void goto_night_tsinghua();
void goto_night_crow();
void goto_night_butterfly();
void goto_night_common();
void goto_night_msg(int,  char * , char * );
void goto_night();

//goto_day.c
void goto_day_original();
void goto_day_crow();
void goto_day_butterfly();
int goto_day_common(int);
void goto_day();

//goto_check.c
int look_for_next_speaker (int,int,int );
void goto_check_speak ();
void goto_check_original();
void goto_check_crow();
void goto_check_butterfly();
void goto_check_tsinghua();

//goto_defence.c
int kill_surrender();
void goto_defence();
void goto_defence_setvictim();
void goto_defence_original();
void goto_defence_speak();
//goto_vote.c
void goto_vote();
void goto_vote_original();
//check_win.c
int check_win_crow();
int check_win_butterfly();
int check_win_original();
void check_win_showkiller ();
void check_win_census(int *);
void check_win_goodwin();
void check_win_badwin1();
void check_win_badwin2();
void check_win_badwin3();
void check_win_draw();
void check_win_crowwin();
int check_win();
//vote_result.c
void vote_result_highest2(int *);
void vote_result();
void vote_result_original();
void vote_result_crow();
void vote_result_butterfly();
void vote_result_tsinghua();

int show_vote_result_list();

//join_room.c
void join_room_talk(char * );
void join_room(int, int);
//timeout.c
void set_turn_time();
void set_action_time();
int check_timeout_turn ();
int check_timeout_action ();
void timeout_callback ();
void timeout_defence();
void timeout_night();



//ctrls.c
void ctrl_s_callback(int);
void ctrl_s_night(int);
void ctrl_s_night_crow(int);
void ctrl_s_night_butterfly(int);
void ctrl_s_night_original(int);
void ctrl_s_night_tsinghua(int);
void ctrl_s_check(int);
void ctrl_s_check_crow(int);
void ctrl_s_check_butterfly(int);
void ctrl_s_check_original(int);
void ctrl_s_check_tsinghua(int);

void ctrl_s_defence (int);
void ctrl_s_defence_original(int);
void ctrl_s_vote (int);
int ctrl_s_night_common(int , long, int , int , char * );
void ctrl_s_callback ();
//ctrlg.c
void ctrl_g_night();
void ctrl_g_night_crow();
int ctrl_g_night_common(long , int, int);
void ctrl_g_callback ();
void ctrl_g_defence();
void ctrl_g_defence_original();
//ctrlt.c
void ctrl_t_callback(int);
//ctrltnight.c
void ctrl_t_night_crow();
void ctrl_t_night_butterfly();
void ctrl_t_night_original();
int random_sort (int *, int );
int get_s_type (int );
void answer_result_police();
void answer_result_crow();
void announce_death_mule();
int night_result_defunct(int , long , int);
int living_role (long );
int living_role_count (long );
int living_role_list (long, int * );
void night_result_pinned();
int night_result_hug (int * , int, int);
void night_result_butterfly();
void night_result_crow();
void ctrl_t_night();
//ctrtltday.c
void ctrl_t_day();
void ctrl_t_day_original();
void ctrl_t_day_crow();
void ctrl_t_day_butterfly();
void ctrl_t_day_common();

//ctrltcheck.c
void ctrl_t_check(int);
void ctrl_t_check_original();
void ctrl_t_check_crow();
void ctrl_t_check_butterfly();
void ctrl_t_check_tsinghua(int);
void ctrl_t_check_common();
int announce_surrender ();

//ctrltdefence.c
void ctrl_t_defence();
void ctrl_t_defence_original();
void ctrl_t_defence_common();
//normalmsg.c
int normalmsg_callback(char *);
int normalmsg_defence(char *);
//joker.c
void start_game_joker1();
void goto_defence_joker1();
int normalmsg_defence_joker1();
void ctrl_s_defence_joker1();
void ctrl_g_defence_joker1();
void ctrl_t_defence_joker1();

int check_win_joker1();
void check_win_goodwin_joker1(int);
void check_win_badwin_joker1(int);
void check_win_draw_joker1();
void joker1_pass_msg(int);
void joker1_next_player();
void joker1_next_round();
int isvalidjoker1(char);
int iswildcard(char);
void joker1_sort_by_value(char *, int);
void joker1_showdeck(int,int);
void joker1_getname(char, char *);
void joker1_getname2(char, char *);
int joker1_singlevalue(char);
int joker1_singlevalue2(char);
int joker1_multivalue(char *, int *, int *);
int joker1_havethissuit(char , char* , int * );
void joker1_showcurrent (int );
void joker1_timeout_skip();
//joker_common.c
int joker_getpos (int tablepos);
int joker_gettablepos (int pos);
void joker_getname (char num, char * name, int style);
int joker_single_num2value ( char num);
int joker_single_value2num ( char value, char suit,  int style);
int joker_single_name2value ( char card);
int joker_num2suit(char num);
int joker_card2suit (char card);
void joker_showdeck (int hand, int pos, int max_per_hand, int style);
int joker_isvalid (char c, char s );
void joker_sort (char * list, int count, int sort_style );
void joker_pass_msg (int pos, int style);
int joker_next_player(int mode, int style);
int joker_next_round(int style);
int joker_havethissuit(char cardname, char suitname, char* deck, int * mark, int count );
void joker_ontable_showcurrent(int pos, int style);
void joker_multiname(int type, int value, char * buf, int style);
void joker_showdeck_common(char * deck,int count, int pos, int style );
//jokerpig.c
void start_game_jokerpig();
void goto_night_jokerpig ();
void timeout_night_jokerpig (int type);
int normalmsg_night_jokerpig(char* card);
void ctrl_s_night_jokerpig(int select);
void ctrl_t_night_jokerpig();
void goto_defence_jokerpig ();
int normalmsg_defence_jokerpig (char * card);
void ctrl_s_defence_jokerpig();
void ctrl_g_defence_jokerpig();
void ctrl_t_defence_jokerpig();
void jokerpig_timeout_skip();
void vote_result_jokerpig();
int check_win_jokerpig();

void jokerpig_next_player(int mode);
int jokerpig_havethissuit(char card, char suit, char* deck, int * mark);
int isvalidjokerpig (char c, char s);
void jokerpig_sort_by_suit (char * list, int count);
void jokerpig_showcurrent (int pos);
void jokerpig_getdc (int doubleflag, char * cardname, int pcsn, char * buf);
void jokerpig_showdeck(int i,int pos);
void jokerpig_getname (char num, char * name);
void jokerpig_getname2 (char num, char * name);
int jokerpig_singlevalue (char num);
int jokerpig_singlevalue2 (char card);
int jokerpig_pcsn2num (int pcsn);
int jokerpig_ispointcard (char num);
int jokerpig_ispointcard2 (char card, char suit);
int jokerpig_isshowcard(char num);
int jokerpig_isshowcard2(char card, char suit);
long jokerpig_countpoint (char * list, int count, char * doubleflag);
int jokerpig_largest (char * list,int count);
void jokerpig_winpoints (char *list,  int who);
void jokerpig_showcurrent_doubleflags(int pos);
void jokerpig_showscore(int pos);
void jokerpig_showimage (char * buf, int which);




































