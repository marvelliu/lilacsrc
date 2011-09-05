#ifndef LILAC_FUNCS
#define LILAC_FUNCS
int lilac_auto_register(char *userid, char *email, int msize, session_t *session);
int lilac_ocpost(char *userid, char *email, int msize,  session_t *session);
void trimlfrt(char *str);
int lilac_validmail(char *email);
int lilac_validid(char *id);
void genactivecode(unsigned char activecode[]);
int send_verifymail_id(char *from, char *to, char *msgfile,	char *subject, char *replace, char *replacewith,char *name, char *addr, char *tele, char *userid);

int send_verifymail(char *from, char *to, char *msgfile, char *subject, char *replace, char *replacewith, char *name, char *addr, char *tele, session_t *session);
int create_activecode(int mod, char *name, char *addr, char *telephone, char *email, session_t *session);
int test_and_remove_file(char *userid, char *fn);
int lilac_test_and_rename_file(char *userid, char *fromfn, char *tofn);
int echo_verify_message_id( char *msgfile, char *subject, char *replace, char *replacewith,	char *name, char *addr, char *tele, char *userid, char *filename);
int generate_active_url(char *name, char *addr, char *telephone, char *email, char *filename, session_t *session);
int get_active_url(char *name, char *addr, char *telephone, char *email, char* url, session_t *session);

//For email validcation
int verify_email(session_t *session);
int verify_email_by_info(char *name, char *userid, char *email);
int password_email_by_info(char *name, char *userid, char *email);
int send_verify_email_mail(char *from, char *to, char *msgfile, char *subject, char *replace, char *replacewith, char *name, char *userid);
int verify_email_file_exist(session_t *session);
int verify_email_file_exist_by_userid(char *userid);
int verified_email_file_exist(session_t *session);
int verified_email_file_exist_by_userid(char *userid);

//For IP Control
//int ipctrl_check(char *sip, int type);
int ipctrl_check(char *sip, int type, session_t *session);
int load_ipctrl_conf();
void *get_ipctrl_shm();
int ipctrl_check_reg(char *sip, int type);
#ifdef HOROSCOPE
char   * horoscope(unsigned char month, unsigned char day);
#endif

int get_user_score(const char* userid);
int get_recommend_info(int num, char *info);
int get_digest_info(int num, char *boardname, fileheader_t *files);
void merge(struct favbrd_struct *brdlist1, int brdlist_t1, 
		struct favbrd_struct *brdlist2, int brdlist_t2,
		struct favbrd_struct *brdlist, int * brdlist_t, char *name, int type);
#endif
