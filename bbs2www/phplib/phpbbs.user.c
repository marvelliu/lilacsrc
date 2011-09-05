#include "php_kbs_bbs.h"  
#include "lilacfuncs.h"

//暂时放这里
#define MANAGERSHIP(user) (getCurrentUser() && (HAS_PERM(getCurrentUser(), PERM_SYSOP) || !strcmp((user)->userid , getCurrentUser()->userid)))

void assign_user(zval * array, struct userec *user, int num)
{
    struct userdata ud;

    read_userdata(user->userid, &ud);
    add_assoc_long(array, "index", num);
    add_assoc_string(array, "userid", user->userid, 1);
    add_assoc_long(array, "firstlogin", user->firstlogin);
    add_assoc_long(array, "exittime", user->exittime);
//    add_assoc_stringl(array, "lasthost", user->lasthost, IPLEN, 1);
    add_assoc_string(array, "lasthost", MANAGERSHIP(user) ? user->lasthost: SHOW_USERIP(user, user->lasthost), 1);
    add_assoc_long(array, "numlogins", user->numlogins);
    add_assoc_long(array, "numposts", user->numposts);
    add_assoc_long(array, "flag1", user->flags);
    add_assoc_long(array, "title", user->title);
    add_assoc_string(array, "username", user->username, 1);
    add_assoc_stringl(array, "md5passwd", (char *) user->md5passwd, 16, 1);
    add_assoc_string(array, "realemail", ud.realemail, 1);
    add_assoc_long(array, "userlevel", user->userlevel);
    add_assoc_long(array, "lastlogin", user->lastlogin);
    add_assoc_long(array, "stay", user->stay);
    add_assoc_string(array, "realname", ud.realname, 1);
    add_assoc_string(array, "address", ud.address, 1);
    add_assoc_string(array, "email", ud.email, 1);
    add_assoc_long(array, "signature", user->signature);
    add_assoc_long(array, "signum", ud.signum);
    add_assoc_long(array, "userdefine0", user->userdefine[0]);
    add_assoc_long(array, "userdefine1", user->userdefine[1]);
#ifdef LILAC
	int exp = countexp(user);
    add_assoc_long(array, "exp", exp);
    add_assoc_long(array, "score", user->unused[6]);
#endif

#ifdef HAVE_BIRTHDAY
	add_assoc_long(array,"gender",ud.gender);
	add_assoc_long(array,"birthyear",ud.birthyear);
    add_assoc_long(array,"birthmonth",ud.birthmonth);
    add_assoc_long(array,"birthday", ud.birthday);
#endif

    add_assoc_string(array,"reg_email",ud.reg_email,1);
    add_assoc_long(array,"mobilderegistered", ud.mobileregistered);
    add_assoc_string(array, "mobilenumber", ud.mobilenumber,1);

#ifdef HAVE_WFORUM
    add_assoc_string(array,"OICQ",ud.OICQ,1);
    add_assoc_string(array,"ICQ",ud.ICQ,1);
    add_assoc_string(array,"MSN", ud.MSN,1);
    add_assoc_string(array,"homepage",ud.homepage,1);
    add_assoc_long(array,"userface_img", ud.userface_img);
	add_assoc_string(array,"userface_url", ud.userface_url,1);
	add_assoc_long(array,"userface_width", ud.userface_width);
	add_assoc_long(array,"userface_height", ud.userface_height);
	add_assoc_long(array,"group",ud.group);
    add_assoc_string(array,"country", ud.country,1);
    add_assoc_string(array,"province", ud.province,1);
    add_assoc_string(array,"city",ud.city,1);
    add_assoc_long(array,"shengxiao",ud.shengxiao);
    add_assoc_long(array,"bloodtype", ud.bloodtype);
    add_assoc_long(array,"religion",ud.religion);
    add_assoc_long(array,"profession",ud.profession);
    add_assoc_long(array,"married",ud.married);
    add_assoc_long(array,"education", ud.education);
    add_assoc_string(array,"graduateschool",ud.graduateschool,1);
    add_assoc_long(array,"character", ud.character);
	add_assoc_string(array,"photo_url", ud.photo_url,1);
	add_assoc_string(array,"telephone", ud.telephone,1);
#endif

#ifdef NEWSMTH
    if (MANAGERSHIP(user)) add_assoc_long(array,"score_user",user->score_user);
#endif
#ifdef HOROSCOPE 
	if( DEFINE(user, DEF_SHOWDETAILUSERDATA) )
		add_assoc_string(array,"horoscope", horoscope(ud.birthmonth, ud.birthday),1);
#endif
}


PHP_FUNCTION(bbs_getuser)
{
    long v1;
    struct userec *lookupuser;
    char *s;
    int s_len;
    zval *user_array;

    MAKE_STD_ZVAL(user_array);
    if (zend_parse_parameters(2 TSRMLS_CC, "sa", &s, &s_len, &user_array) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }

    if (s_len > IDLEN)
        s[IDLEN] = 0;
    v1 = getuser(s, &lookupuser);

    if (v1 == 0)
        RETURN_LONG(0);

    if (array_init(user_array) != SUCCESS)
        RETURN_LONG(0);
    assign_user(user_array, lookupuser, v1);
/*        RETURN_STRING(retbuf, 1);
 *        */
    RETURN_LONG(v1);
}








PHP_FUNCTION(bbs_checkpasswd)
{
    char *s;
    int s_len;
    char *pw;
    int pw_len;
    long ret;
    int unum = 0;
    long ismd5 = 0;
    struct userec *user;
    int ac = ZEND_NUM_ARGS();

    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "ss", &s, &s_len, &pw, &pw_len) != SUCCESS) {
        if (ac!= 3 || zend_parse_parameters(3 TSRMLS_CC, "ssl", &s, &s_len, &pw, &pw_len, &ismd5) != SUCCESS) {
            WRONG_PARAM_COUNT;
        }
    }
    if (s_len > IDLEN)
        s[IDLEN] = 0;
    if (pw_len > PASSLEN)
        pw[PASSLEN] = 0;
    if (pw[0] == '\0')
        ret = 1;
    else if ((s[0] != 0) && !(unum = getuser(s, &user)))
        ret = 2;
    else {
        if (s[0] == 0)
            user = getCurrentUser();
        if (user) {
            if (ismd5) {
                ismd5 = !(memcmp(pw, user->md5passwd, MD5PASSLEN));
            } else {
                ismd5 = checkpasswd2(pw, user);
            }
            if (ismd5) {
                ret = 0;
                if (s[0] != 0)
                    setcurrentuser(user, unum);
            } else {
                ret = 1;
                logattempt(user->userid, getSession()->fromhost, "www");
            }
        } else {
            ret = 1;
        }
    }
    RETURN_LONG(ret);
}

PHP_FUNCTION(bbs_checkuserpasswd){
    char *s;
    int s_len;
    char *pw;
    int pw_len;
    int unum;
    struct userec *user;

    if (zend_parse_parameters(2 TSRMLS_CC, "ss", &s, &s_len, &pw, &pw_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
    if (s_len > IDLEN)
        s[IDLEN] = 0;
    if (pw_len > PASSLEN)
        pw[PASSLEN] = 0;
	if (pw_len < 2) {
		RETURN_LONG(-1);
	}
    if ( !(unum = getuser(s, &user))) {
        RETURN_LONG(-2);
    }
	   if ( !checkpasswd2(pw, user)) {
        RETURN_LONG(-3);
    }
    RETURN_LONG(0);
}


PHP_FUNCTION(bbs_setuserpasswd){
    char *s;
    int s_len;
    char *pw;
    int pw_len;
    int unum;
    struct userec *user;

    if (zend_parse_parameters(2 TSRMLS_CC, "ss", &s, &s_len, &pw, &pw_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
    if (s_len > IDLEN)
        s[IDLEN] = 0;
    if (pw_len > PASSLEN)
        pw[PASSLEN] = 0;
	if (pw_len < 2) {
		RETURN_LONG(-1);
	}
    if ( !(unum = getuser(s, &user))) {
        RETURN_LONG(-2);
    }
	setpasswd(pw, user);
    RETURN_LONG(0);
}


/**
 * set password for user.
 * prototype:
 * string bbs_setpassword(string userid, string password)
 * @if userid=="" then user=getCurrentUser()
 * @return TRUE on success,
 *       FALSE on failure.
 * @author kcn
 */
PHP_FUNCTION(bbs_setpassword)
{
    char *userid;
    int userid_len;
    char *password;
    int password_len;
    int ac = ZEND_NUM_ARGS();
    struct userec *user;

    if (ac != 2 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userid, &userid_len, &password, &password_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (userid != 0) {
        if (getuser(userid, &user) == 0)
            RETURN_FALSE;
    } else
        user = getCurrentUser();
    if (setpasswd(password, user) != 1)
        RETURN_FALSE;
    RETURN_TRUE;
}



/**
 *  Function: 根据注册姓名和email生成新的密码.如果用户名为空,则生成一个密码.
 *   string bbs_findpwd_check(string userid,string realname,string email);
 *
 *   if failed. reaturn NULL string; or return new password.
 *              by binxun
 */
PHP_FUNCTION(bbs_findpwd_check)
{
    char*   userid,
	        *realname,
            *email;
	int     userid_len,
	        realname_len,
			email_len;
	char    pwd[30];
    struct userdata ud;
	struct userec* uc;

	int ac = ZEND_NUM_ARGS();
    chdir(BBSHOME);

    if (ac != 3 || zend_parse_parameters(3 TSRMLS_CC, "sss", &userid,&userid_len,&realname,&realname_len,&email,&email_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	pwd[0] = 0;
    if(userid_len > IDLEN)RETURN_LONG(1);

	//生成新密码
	srand(time(NULL));
	sprintf(pwd,"%d",rand());

	if(userid_len > 0){
        if(getuser(userid,&uc) == 0)RETURN_LONG(3);
	    if(read_userdata(userid,&ud)<0)RETURN_LONG(4);

	    if(!strncmp(userid,ud.userid,IDLEN) && !strncmp(email,ud.email,STRLEN))
	    {
		    setpasswd(pwd,uc);
	    }
	    else
	        RETURN_LONG(5);
	}

    RETURN_STRING(pwd,1);
}






PHP_FUNCTION(bbs_getwwwparameters)
{
	FILE* fn;
	char  buf[1024];
	
	sethomefile(buf,getCurrentUser()->userid,"www");
    if ((fn=fopen(buf,"r"))==NULL) {
        strcpy(buf, "0");
    } else {
        fgets(buf,1024,fn);
        fclose(fn);
    }
	RETURN_STRING(buf,1);
}

PHP_FUNCTION(bbs_setwwwparameters)
{
	char* wwwparameters;
	int   wwwparameters_len;
	FILE *fn;
	char  buf[201];
	
	int ac = ZEND_NUM_ARGS();
	
	if (ac != 1 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s" , &wwwparameters , &wwwparameters_len) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	
	if(wwwparameters_len > 200)
		RETURN_LONG(-1);
	sethomefile(buf,getCurrentUser()->userid,"www");
	if ((fn=fopen(buf,"w"))==NULL)
		RETURN_LONG(-10);
	fprintf(fn,"%s",wwwparameters);
	fclose(fn);
	RETURN_LONG(0);
}

PHP_FUNCTION(bbs_getuserparam){//这个函数总有一天要被我杀掉！！ - atppp
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	if (getCurrentUser()==NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(getCurrentUser()->userdefine[0]);
}

PHP_FUNCTION(bbs_setuserparam){
	long userparam0, userparam1, mailboxprop;
	if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(3 TSRMLS_CC, "lll", &userparam0, &userparam1, &mailboxprop) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	if (getCurrentUser()==NULL) {
		RETURN_LONG(-1);
	}
	getCurrentUser()->userdefine[0] = userparam0;
    getCurrentUser()->userdefine[1] = userparam1;
    getSession()->currentuinfo->mailbox_prop = update_mailbox_prop(getCurrentUser()->userid, mailboxprop);
    store_mailbox_prop(getCurrentUser()->userid);
	RETURN_LONG(0);
}




PHP_FUNCTION(bbs_user_setflag)
{
    struct userec *lookupuser;
    char *s;
    int s_len;
    long flag, set;

    if (ZEND_NUM_ARGS() != 3 || zend_parse_parameters(3 TSRMLS_CC, "sll", &s, &s_len, &flag, &set) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
    if( ! getuser(s,&lookupuser) ) {
        RETURN_FALSE;
    }
    if (set) {
        lookupuser->flags |= flag;
    } else {
        lookupuser->flags &= ~flag;
    }
    RETURN_TRUE;
}




PHP_FUNCTION(bbs_recalc_sig)
{
	struct userec newinfo;
	int unum;
	int sign;

    if( (unum = searchuser(getCurrentUser()->userid))==0)
		RETURN_LONG(-1);
	memcpy(&newinfo, getCurrentUser(), sizeof(struct userec));
    
	if( read_user_memo(getCurrentUser()->userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);

    (getSession()->currentmemo)->ud.signum = calc_numofsig(getCurrentUser()->userid);
	sign = (getSession()->currentmemo)->ud.signum;

    write_userdata(getCurrentUser()->userid,&((getSession()->currentmemo)->ud) );
	end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);


	if(sign>0 && newinfo.signature>0)
		RETURN_LONG(1);

	if(sign<=0 && newinfo.signature==0)
		RETURN_LONG(2);

	if(sign > 0)
    	newinfo.signature = 1;
	else
		newinfo.signature = 0;

	update_user(&newinfo, unum, 1);

	RETURN_LONG(3);

}



PHP_FUNCTION(bbs_modify_nick)
{
    struct userec newinfo;
    int unum;

    char* username;
    int username_len;
    int ac = ZEND_NUM_ARGS();
    long bTmp = 1;

    if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &username, &username_len) == FAILURE)
        if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "sl", &username, &username_len, &bTmp) == FAILURE)
    {
            WRONG_PARAM_COUNT;
    }
    process_control_chars(username,NULL);
	if( username_len >= NAMELEN)
       RETURN_LONG(-1);

    if (!bTmp) {
        if( (unum = searchuser(getCurrentUser()->userid))==0)
            RETURN_LONG(-1);
        memcpy(&newinfo, getCurrentUser(), sizeof(struct userec));
        if (strcmp(newinfo.username, username)) {
            strcpy(newinfo.username, username);
        	update_user(&newinfo, unum, 1);
        }
    }

	strcpy(getSession()->currentuinfo->username, username);
    UPDATE_UTMP_STR(username, (*(getSession()->currentuinfo)));

	RETURN_LONG(0);
}






PHP_FUNCTION(bbs_getuserlevel){
    struct userec* u;
	char* user;
	int uLen;
	char title[USER_TITLE_LEN];
    if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &user, &uLen) != SUCCESS) {
            WRONG_PARAM_COUNT;
    }
    if (getuser(user, &u)==0) {
		RETURN_LONG(-1);
	}
	uleveltochar(title,u);
	title[USER_TITLE_LEN-1]=0;
	RETURN_STRINGL(title,strlen(title),1);
}


PHP_FUNCTION(bbs_compute_user_value)
{
    struct userec *user;
	char *userid;
	int userid_len;

    if(zend_parse_parameters(1 TSRMLS_CC,"s",&userid,&userid_len)!=SUCCESS){
        WRONG_PARAM_COUNT;
    }
	if(userid_len>IDLEN)
        userid[IDLEN]=0;
	if(!getuser(userid,&user))
		RETURN_LONG(0);
	RETURN_LONG(compute_user_value(user));
}

PHP_FUNCTION(bbs_user_level_char)
{
	char *userid;
	int userid_len;
    struct userec *lookupuser;
	char permstr[USER_TITLE_LEN];

    if (zend_parse_parameters(1 TSRMLS_CC, "s", &userid, &userid_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }
	
	if( userid_len > IDLEN )
		userid[IDLEN]=0;

	if( getuser(userid, &lookupuser) == 0 )
		RETURN_LONG(0);

	permstr[0]=0;
	uleveltochar(permstr, lookupuser);
	RETURN_STRING(permstr, 1);

}



/*
	bbs_checkregstatus: 检查用户注册状态
		  Return Value: 0 已经注册成功
                        1 还没有到填写注册单的时间
                        2 需要填写注册单
						3 无效用户名
                        4 没有输入有效的激活EMAIL地址，注册单无效   LILAC_NEW_REG
						4 注册单正在等待处理                        LILAC_OLD_REG
                        5 系统错误
                        6 激活码已经发送，等待激活
                        7 已经激活成功，重新登陆获取权限
*/
PHP_FUNCTION(bbs_checkregstatus)
{
	char *userid;
	int   usernum, userid_len;
    struct userec *uc;
    char genbuf[STRLEN + 1];
    char *ptr;
	FILE *fn;

    int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &userid, &userid_len) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	//usernum = getusernum(userid);
	usernum = searchuser(userid);
	if (0 == usernum)
	{
		RETURN_LONG(3);
	}

    if (getuser(userid, &uc) == 0)
    {
	    RETURN_LONG(3);
	}
    if (HAS_PERM(uc, PERM_LOGINOK))
    {
	    RETURN_LONG(0);
	}
	if(time(NULL) - uc->firstlogin < REGISTER_WAIT_TIME) // 23Hours
	{
		RETURN_LONG(1);	//未满24小时
	}
#ifdef LILAC_REG_DEEM
	sethomefile(genbuf, uc->userid, ".needactive");
	if (dashf(genbuf))
	{
		RETURN_LONG(6);
	}

	sethomefile(genbuf, uc->userid, ".activeinfo");
	if (dashf(genbuf))
	{
		RETURN_LONG(7);
	}

	sethomefile(genbuf, uc->userid, "auto_register");
	if (dashf(genbuf))
	{
		RETURN_LONG(4);
	}
#else
	if ((fn = fopen("new_register", "r")) != NULL) 
	{
		while (fgets(genbuf, STRLEN, fn) != NULL) 
		{
			if ((ptr = strchr(genbuf, '\n')) != NULL)
				*ptr = '\0';
			if (strncmp(genbuf, "userid: ", 8) == 0 && strcmp(genbuf + 8, userid) == 0) 
			{
				fclose(fn);
				{
					RETURN_LONG(4);
                }
			}
		}
		fclose(fn);
	}
#endif

	RETURN_LONG(2);
}


PHP_FUNCTION(bbs_checkipctrl)
{
	char *from;
	int  from_len, perm;
    int ac = ZEND_NUM_ARGS();

	if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "sl", &from, &from_len, &perm) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	if (ipctrl_check(from, perm, getSession()) == 0 )
	{
		RETURN_LONG(0);
	}
	else
	{
		RETURN_LONG(1);
	}	

	RETURN_LONG(1);
}


PHP_FUNCTION(bbs_sendocactivecode)
{
	int ulen, alen, tlen, elen, uidlen;
	char *username, *addr, *telephone, *email, *userid;
    struct userec *uc;
    int ac = ZEND_NUM_ARGS();



	if (ac != 5 || zend_parse_parameters(5 TSRMLS_CC, "sssss", &userid, &uidlen, &username, &ulen,
							&addr, &alen, &telephone, &tlen, &email, &elen) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

    if (getuser(userid, &uc) == 0)
    {
	    RETURN_LONG(12);
	}
{
	int valid_regmail = lilac_validmail(email);
	if (valid_regmail == 0)
	{
		switch (test_and_remove_file(uc->userid, ".needactive"))
		{
		case -1:
        	RETURN_LONG(10);
			break;
		case 0:
        	RETURN_LONG(6);
			break;
		case 1:
        	RETURN_LONG(7);
			break;
		}
	}
	else
	{
        //本来这一段程序可以和注册激活公用的，可惜写的时候没有考虑userid信息
        //修改的话牵涉www和telnet，太麻烦，还不如直接拷贝过来了。:) luohao
		unsigned char ac[36];
		char buff[256], genbuf[256];
		FILE *fp;

		memset(ac, 0, 36);
		genactivecode(ac);
		//保存激活信息
		sethomefile(genbuf, uc->userid, ".needactive");
		if ( (fp=fopen(genbuf, "w")) == NULL)
			RETURN_LONG(9);

		fprintf(fp, "TIME:%d\n", (int)time(NULL));
		fprintf(fp, "ACCD:%s\n", ac);
		fprintf(fp, "NAME:%s\n", username);
		fprintf(fp, "ADDR:%s\n", addr);
		fprintf(fp, "TELE:%s\n", telephone);
		fprintf(fp, "MAIL:%s\n", email);
		fclose(fp);

		sprintf(buff, "http://%s/ocactive.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"), uc->userid, ac);

		send_verifymail_id("no-reply@lilacbbs.com", email,
			"/home/hitbbs/etc/f_fill.ocactive",
			"紫丁香社区BBS 校外用户发文权限激活信件", "[ACTIVE CODE HERE]",
			buff, username, addr, telephone, uc->userid);

	}	
}
	RETURN_LONG(8);  //正常结束
}


PHP_FUNCTION(bbs_doactive)
{
    char *uid;
	int uidlen;
    struct userec *uc;
    int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &uid, &uidlen) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
    if (getuser(uid, &uc) == 0)
    {
	    RETURN_LONG(12);
	}

    uidlen = lilac_test_and_rename_file(uc->userid, ".needactive", ".activeinfo");
    
    //By Marvel 老用户激活，设置ip权限
    struct usermemo *memo;
    uc->userlevel |= PERM_NOIPCTRL; 
    read_user_memo(uc->userid, &memo);
    write_userdata(getCurrentUser()->userid, &(memo->ud));

   	RETURN_LONG( uidlen );
}
PHP_FUNCTION(bbs_doactive2){
  char *uid,*reg_email;
  int uidlen,reg_email_len;
  struct userec *uc;
  int ac = ZEND_NUM_ARGS();
  if( ac !=2 || zend_parse_parameters(2 TSRMLS_CC,"ss",&uid,&uidlen,&reg_email,&reg_email_len) == FAILURE){
   WRONG_PARAM_COUNT;
  }
 //获取并写入用户基本数据
  if(getuser(uid,&uc) == 0) RETURN_LONG(12);
  struct userdata udata;
  read_userdata(uc->userid,&udata);
  strncpy(udata.reg_email,reg_email,reg_email_len);
  write_userdata(uc->userid,&(udata));
  //获取并写入用户扩展数据
  struct usermemo *memo;
  read_user_memo(uc->userid,&memo);
  write_userdata(getCurrentUser()->userid,&(memo->ud));
  RETURN_LONG(0);

}
PHP_FUNCTION(bbs_doocactive)
{
    char *uid;
	int uidlen;
    struct userec *uc;
    int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &uid, &uidlen) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

    if (getuser(uid, &uc) == 0)
    {
	    RETURN_LONG(12);
	}
    uidlen = lilac_test_and_rename_file(uc->userid, ".needactive", ".activeinfo");

	if (uidlen == 1 )
	{
		if (lilac_ocpost(uc->userid, "NOMAIL", 0, getSession()) == 0)
		{
			RETURN_LONG(0);  //0成功 -1失败
		}
		else
		{
			RETURN_LONG(-8);  //0成功 -1失败
		}
	}
	else if (uidlen == 0)
   		RETURN_LONG( -2 );  //.needactive不存在

   	RETURN_LONG( -3 );  //创建锁定失败

}


PHP_FUNCTION(bbs_doactiveolduser)
{
    char *uid;
		int uidlen;
		char url[100];
    struct userec *uc;
    struct usermemo *memo;
    int ac = ZEND_NUM_ARGS();

		if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &uid, &uidlen) == FAILURE)
		{
			WRONG_PARAM_COUNT;
		}
    if (getuser(uid, &uc) == 0)
    {
	    RETURN_LONG(12);
		}
	
    if (read_user_memo(uc->userid, &memo) <= 0) {
      sleep(1);
      exit(1);
    }
		get_active_url(memo->ud.realname, memo->ud.address, memo->ud.telephone, memo->ud.realemail, url, getSession());
	
	  RETURN_STRING( url,1 );
}


PHP_FUNCTION(bbs_doverifyemail)
{
    char *uid;
	int uidlen;
    struct userec *uc;
    int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &uid, &uidlen) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
    if (getuser(uid, &uc) == 0)
    {
	    RETURN_LONG(12);
	}

	struct userdata udata;
	read_userdata(uc->userid,&udata);
	strncpy(udata.reg_email,udata.email, STRLEN);
    write_userdata(uc->userid, &(udata));

    struct usermemo *memo;
    read_user_memo(uc->userid, &memo);
	strncpy(memo->ud.reg_email, memo->ud.email, STRLEN);
    write_userdata(uc->userid, &(memo->ud));

    uidlen = lilac_test_and_rename_file(uc->userid, ".verify_email", ".verified_email");

   	RETURN_LONG( uidlen );
}


PHP_FUNCTION(bbs_sendverifyemail)
{
	int tuid=0;
	struct userdata udata;
	char *userid, *email;
	int userid_len, email_len;

    int ac = ZEND_NUM_ARGS();
    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "ss", &userid, &userid_len, &email, &email_len) == FAILURE)
    {
            WRONG_PARAM_COUNT;
    }
    if(userid_len > IDLEN)
       RETURN_LONG(-1);

	if (!(tuid = getuser(userid, NULL))) {
       RETURN_LONG(-2);
	}

	read_userdata(userid,&udata);
	if(email != NULL)
	{
		strncpy(udata.email, email, STRLEN);
	    write_userdata(userid, &(udata));
	}
	verify_email_by_info(udata.realname, userid, udata.email);
    RETURN_LONG(0);
}



PHP_FUNCTION(bbs_dopasswordemail)
{
    char *uid;
	int uidlen;
    struct userec *uc;
    int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &uid, &uidlen) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
    if (getuser(uid, &uc) == 0)
    {
	    RETURN_LONG(12);
	}

    struct usermemo *memo;
    read_user_memo(uc->userid, &memo);
	strncpy(memo->ud.reg_email, memo->ud.email, STRLEN);
    write_userdata(uc->userid, &(memo->ud));

	struct userdata udata;
	read_userdata(uc->userid,&udata);
	strcpy(udata.reg_email,udata.email);
    write_userdata(uc->userid, &(udata));

    uidlen = lilac_test_and_rename_file(uc->userid, ".pass_email", ".passed_email");
    
   	RETURN_LONG( uidlen );
}


PHP_FUNCTION(bbs_sendpasswordemail)
{
	int tuid=0;
	struct userdata udata;
	char *userid;
	int userid_len;

    int ac = ZEND_NUM_ARGS();
    if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &userid, &userid_len) == FAILURE)
    {
            WRONG_PARAM_COUNT;
    }
    if(userid_len > IDLEN)
       RETURN_LONG(-1);

	if (!(tuid = getuser(userid, NULL))) {
       RETURN_LONG(-2);
	}

	read_userdata(userid,&udata);
	password_email_by_info(udata.realname, userid, udata.email);
    RETURN_LONG(0);
}




/**
 * set id card for user.
 */
PHP_FUNCTION(bbs_setidcard)
{
    char *userid;
    int userid_len;
    char *idcard;
    int idcard_len;
    int ac = ZEND_NUM_ARGS();
    struct userec *user;

    if (ac != 2 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss", &userid, &userid_len, &idcard, &idcard_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (userid != 0) {
        if (getuser(userid, &user) == 0)
            RETURN_FALSE;
    } else
        user = getCurrentUser();


    struct usermemo *memo;
    read_user_memo(userid, &memo);
	strncpy(memo->ud.ICQ, idcard, idcard_len);
    write_userdata(userid, &(memo->ud));

	struct userdata udata;
	read_userdata(userid,&udata);
	strncpy(udata.ICQ, idcard, idcard_len);
    write_userdata(userid, &(udata));

    RETURN_TRUE;
}

/*By Kaede*/
PHP_FUNCTION(modify_user_score){
   char *userid,*reason;
   int userid_len,reason_len,score;
   struct userec *lookupuser;
   //
   char fname[STRLEN];
   FILE* update_fd = fopen("etc/posts/update_score","w+");
  
   //
   if(ZEND_NUM_ARGS() != 3 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC,"sls",&userid,&userid_len,&score,&reason,&reason_len) == FAILURE){
      WRONG_PARAM_COUNT;
   }  
   getuser( userid, &lookupuser );
   if ( score == 0 )
       RETURN_FALSE;
   lookupuser->score += score;
   if ( score > 0 ){
        sprintf( fname, "[公告] 增加 %s 积分 %d 分", userid, score);
    }else{
	sprintf( fname, "[公告] 增加 %s 积分 %d 分", userid, score);
    }
   
   fprintf(update_fd, "说明:\n" );
   if ( reason != NULL )
		fprintf(update_fd, "        %s\n", reason);
   fclose(update_fd );
   post_file(NULL, "", "etc/posts/update_score", "ScoreRecords", fname, 0, 1, getSession());
   RETURN_TRUE;
  
}
