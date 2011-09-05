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

*/

/*
    checked Global variable
*/
#include "bbs.h"
#include "lilacfuncs.h"

#define  EMAIL          0x0001
#define  NICK           0x0002
#define  REALNAME       0x0004
#define  ADDR           0x0008
#define  REALEMAIL      0x0010
#define  BADEMAIL       0x0020
#define  NEWREG         0x0040

char *Ctime();

extern struct user_info uinfo;
extern time_t login_start_time;
extern int convcode;            /* KCN,99.09.05 */

void new_register()
{
    struct userec newuser;
    int allocid, do_try, flag, usernum;
    char buf[STRLEN];

/* temp !!!!!*/
/*    prints("Sorry, we don't accept newusers due to system problem, we'll fixit ASAP\n");
    oflush();
    sleep(2);
    exit(-1);
*/
    memset(&newuser, 0, sizeof(newuser));
    getdata(0, 0, "ʹ��GB�����Ķ�?(\xa8\xcf\xa5\xce BIG5\xbd\x58\xbe\x5c\xc5\xaa\xbd\xd0\xbf\xefN)(Y/N)? [Y]: ", buf, 4, DOECHO, NULL, true);
    if (*buf == 'n' || *buf == 'N')
        if (!convcode)
            switch_code();

    ansimore("etc/register", false);
    do_try = 0;
    while (1) {
        if (++do_try >= 10) {
            prints("\n��������̫����  <Enter> ��...\n");
            refresh();
            longjmp(byebye, -1);
        }
        getdata(0, 0, "���������: ", newuser.userid, IDLEN + 1, DOECHO, NULL, true);
        flag = 1;
        if (id_invalid(newuser.userid) == 1) {
            prints("�ʺű�����Ӣ����ĸ��������ɣ����ҵ�һ���ַ�������Ӣ����ĸ!\n");
            /*                prints("�ʺű�����Ӣ����ĸ�����֣������ʺŵ�һ������Ӣ����ĸ!\n"); */
            flag = 0;
        }
        if (flag) {
            if (strlen(newuser.userid) < 2) {
                prints("����������������Ӣ����ĸ!\n");
            } else if ((*newuser.userid == '\0') || bad_user_id(newuser.userid) || (strcasecmp(newuser.userid, "SYSOPS") == 0) || (strcasecmp(newuser.userid, "BMS") == 0)) {
                prints("��Ǹ����վ�ݲ��ṩ���ʺ�ע�ᡣ\n");
            } else if ((usernum = searchuser(newuser.userid)) != 0) {   /*( dosearchuser( newuser.userid ) ) midified by dong , 1998.12.2, change getuser -> searchuser , 1999.10.26 */
                prints("���ʺ��Ѿ�����ʹ��\n");
            } else {
                /*---	---*/
                struct stat lst;
                time_t lnow;

                lnow = time(NULL);
                sethomepath(genbuf, newuser.userid);
                if (!stat(genbuf, &lst) && S_ISDIR(lst.st_mode)
                    && (lnow - lst.st_ctime < SEC_DELETED_OLDHOME /* 3600*24*30 */ )) {
                    prints("Ŀǰ�޷�ע���ʺ�%s������ϵͳ������Ա��ϵ��\n", newuser.userid);
                    sprintf(genbuf, "IP %s new id %s failed[home changed in past 30 days]", getSession()->fromhost, newuser.userid);
                    bbslog("user","%s",genbuf);
                }
                else{
                    /* etnlegend, 2006.10.14, ���û����ܼ̳�ԭ��ͬ���û��ż�... */
                    sethomepath(genbuf,newuser.userid);
                    my_f_rm(genbuf);
                    setmailpath(genbuf,newuser.userid);
                    my_f_rm(genbuf);
                    break;
                }
            }
        }
    }

    newuser.firstlogin = newuser.lastlogin = time(NULL) - 13 * 60 * 24;
    do_try = 0;
    while (1) {
        char passbuf[STRLEN], passbuf2[STRLEN];

        if (++do_try >= 10) {
            prints("\n��������̫����  <Enter> ��...\n");
            refresh();
            longjmp(byebye, -1);
        }
        getdata(0, 0, "���趨��������: ", passbuf, 39, NOECHO, NULL, true);
        if (strlen(passbuf) < 4 || !strcmp(passbuf, newuser.userid)) {
            prints("����̫�̻���ʹ���ߴ�����ͬ, ����������\n");
            continue;
        }
        getdata(0, 0, "��������һ���������: ", passbuf2, 39, NOECHO, NULL, true);
        if (strcmp(passbuf, passbuf2) != 0) {
            prints("�����������, ��������������.\n");
            continue;
        }

        setpasswd(passbuf, &newuser);
        break;
    }

    if ( searchuser(newuser.userid) != 0) {   
        prints("���ʺ��Ѿ�����ʹ��\n");
        refresh();
        longjmp(byebye, -1);
    }

    newuser.userlevel = PERM_BASIC;
    newuser.userdefine[0] = -1;
    newuser.userdefine[1] = -1;
/*   newuser.userdefine&=~DEF_MAILMSG;
    newuser.userdefine&=~DEF_EDITMSG; */
    SET_UNDEFINE(&newuser, DEF_NOTMSGFRIEND);
    if (convcode)
        SET_UNDEFINE(&newuser, DEF_USEGB);

	SET_UNDEFINE(&newuser, DEF_SHOWREALUSERDATA);

    newuser.exittime = time(NULL) - 100;
    /*newuser.unuse2 = -1;*/
    newuser.flags |= PAGER_FLAG;
    newuser.title = 0;
    newuser.firstlogin = newuser.lastlogin = time(NULL);

    allocid = getnewuserid2(newuser.userid);
    if (allocid > MAXUSERS || allocid <= 0) {
        prints("��Ǹ, ����ĳЩϵͳԭ��, �޷�ע���µ��ʺ�.\n\r");
        oflush();
        sleep(2);
        exit(1);
    }
    newbbslog(BBSLOG_USIES, "APPLY: %s uid %d from %s", newuser.userid, allocid, getSession()->fromhost);

    update_user(&newuser, allocid, 1);

    if (!dosearchuser(newuser.userid)) {
        /* change by KCN 1999.09.08
           fprintf(stderr,"User failed to create\n") ;
         */
        prints("User failed to create %d-%s\n", allocid, newuser.userid);
        oflush();
        exit(1);
    }
    bbslog("user","%s","new account");
}

/*����� #TH ��β��realemail���ʺ��Զ�ͨ��ע��Ĺ���  by binxun
*/
int invalid_realmail(userid, email, msize)
    char *userid, *email;
    int msize;
{
    FILE *fn;
    char ans[4], fname[STRLEN];
    char genbuf[STRLEN];

#ifdef LILAC_REG_MARVEL
	if(!email)
		return 1;
	sethomefile(genbuf, getCurrentUser()->userid, ".activeinfo");
	if (dashf(genbuf))
	{
       /*if (lilac_auto_register(userid, email, msize, getSession()) < 0)    // ����Զ�ע��
	   {
           move(t_lines - 2, 0);
		   prints("��ϲ�㣬����ʺ��Ѿ�����!");
		   pressanykey();
           return 3;
	   }
       else
           return 0;   //success
		   */
	}
#endif

/*
    if ((emailfile = sysconf_str("EMAILFILE")) == NULL)
        return 0;
*/

    if (strchr(email, '@') && valid_ident(email))
        return 0;
    /*
       ansimore( emailfile, false );
       getdata(t_lines-1,0,"��Ҫ���� email-post ��? (Y/N) [Y]: ",
       ans,2,DOECHO,NULL,true);
       while( *ans != 'n' && *ans != 'N' ) {
     */
    sprintf(fname, "tmp/email/%s", userid);
    if ((fn = fopen(fname, "r")) != NULL) {
        fgets(genbuf, STRLEN, fn);
        fclose(fn);
        strtok(genbuf, "\n");
        if (!valid_ident(genbuf)) {
        } else if (strchr(genbuf, '@') != NULL) {
            unlink(fname);
            strncpy(email, genbuf, msize);
            move(10, 0);
            prints("������!! ����ͨ�������֤, ��Ϊ��վ����. \n");
            prints("         ��վΪ�����ṩ�Ķ������, \n");
            prints("         ����Mail,Post,Message,Talk ��. \n");
            prints("  \n");
            prints("������,  ���Ĵ����һ��, \n");
            prints("         �����ĵط�, ���� sysop ������, \n");
            prints("         ��վ����ר��Ϊ�����. \n");
            getdata(18, 0, "�밴 <Enter>  <<  ", ans, 2, DOECHO, NULL, true);
            return 0;
        }
    }
    return 1;
}

void check_register_info()
{
    const char *newregfile;
    //int perm;
    char buf[STRLEN];
	char career[STRLEN];
	char phone[40];

    clear();
    sprintf(buf, "%s", email_domain());
    if (!(getCurrentUser()->userlevel & PERM_BASIC)) {
        getCurrentUser()->userlevel = PERM_DENYMAIL|PERM_DENYRELAX;
        return;
    }
    /*urec->userlevel |= PERM_DEFAULT; */
    //perm = PERM_DEFAULT & sysconf_eval("AUTOSET_PERM",PERM_DEFAULT);

//    invalid_realmail(getCurrentUser()->userid,curruserdata.realemail,STRLEN - 16);
    invalid_realmail(getCurrentUser()->userid,getSession()->currentmemo->ud.realemail,STRLEN - 16);

    do_after_login(getCurrentUser(),getSession()->utmpent,0);

    /*    if( sysconf_str( "IDENTFILE" ) != NULL ) {  commented out by netty to save time */
    while (strlen(getCurrentUser()->username) < 2) {
        getdata(2, 0, "�����������ǳ�:(����," DEFAULT_NICK ") << ", buf, NAMELEN, DOECHO, NULL, true);
        strcpy(getCurrentUser()->username, buf);
        strcpy(uinfo.username, buf);
        UPDATE_UTMP_STR(username, uinfo);
    }
//    if (strlen(curruserdata.realname) < 2) {
    if (strlen(getSession()->currentmemo->ud.realname) < 2) {
        move(3, 0);
        prints("������������ʵ����: (վ����������ܵ� !)\n");
        getdata(4, 0, "> ", buf, NAMELEN, DOECHO, NULL, true);
//        strcpy(curruserdata.realname, buf);
        strcpy(getSession()->currentmemo->ud.realname, buf);
    }
//    if (strlen(curruserdata.address) < 6) {
    if (strlen(getSession()->currentmemo->ud.address) < 6) {
        move(5, 0);
//        prints("��Ŀǰ��д�ĵ�ַ�ǡ�%s��������С�� \033[1m\033[37m6\033[m��ϵͳ��Ϊ����ڼ�̡�\n", curruserdata.address[0] ? curruserdata.address : "�յ�ַ");  /* Leeward 98.04.26 */
        prints("��Ŀǰ��д�ĵ�ַ�ǡ�%s��������С�� \033[1m\033[37m6\033[m��ϵͳ��Ϊ����ڼ�̡�\n", getSession()->currentmemo->ud.address[0] ? getSession()->currentmemo->ud.address : "�յ�ַ");  /* Leeward 98.04.26 */
        getdata(6, 0, "����ϸ��д����סַ��", buf, NAMELEN, DOECHO, NULL, true);
//        strcpy(curruserdata.address, buf);
        strcpy(getSession()->currentmemo->ud.address, buf);
    }





	/* ����ת��ID��Ĵ���   by binxun 2003-5-23 */
	sethomefile(buf,getCurrentUser()->userid,"conveyID");
	if(dashf(buf))
	{
	
        move(6,0);
		prints("��ID����������ת�ø���,��ϲ����ô�ID,����д��������.");
		getCurrentUser()->firstlogin = getCurrentUser()->lastlogin; /* atppp 20050312 */
		do{
		    getdata(7,0,"ѧУϵ����λȫ��(���嵽����):",career,STRLEN,DOECHO,NULL,true);
		}while(strlen(career) < 4);
        do{
		    getdata(8,0,"������ϵ�绰����Email:",phone,40,DOECHO,NULL,true);
		}while(strlen(phone) < 6);

             unlink(buf);   
        sprintf(buf,"%s$%s@ת��", career,phone);
		if(strlen(buf) >= STRLEN - 16)sprintf(buf,"%s@ת��",phone);
//		strncpy(curruserdata.realemail,buf,STRLEN-16);
		strncpy(getSession()->currentmemo->ud.realemail,buf,STRLEN-16);
//		curruserdata.realemail[STRLEN-16-1]='\0';
		getSession()->currentmemo->ud.realemail[STRLEN-16-1]='\0';
//		write_userdata(getCurrentUser()->userid,&curruserdata);
		write_userdata(getCurrentUser()->userid,&(getSession()->currentmemo->ud));
	}

#ifdef HAVE_BIRTHDAY
	if (!is_valid_date(getSession()->currentmemo->ud.birthyear+1900, 
				getSession()->currentmemo->ud.birthmonth,
				getSession()->currentmemo->ud.birthday))
	{
		time_t now;
		struct tm *tmnow;

		now = time(0);
		tmnow = localtime(&now);
		clear();
		buf[0] = '\0';
		move(0, 0);
		prints("���Ǽ�鵽��Ĳ���ע�����ϲ�����ȫ��Ϊ�˸��õ�Ϊ���ṩ���Ի��ķ���");
		move(1, 0);
		prints("ϣ������д�������ϡ�");
		while (buf[0] < '1' || buf[0] > '2')
		{
			getdata(2, 0, "����������Ա�: [1]�е� [2]Ů�� (1 or 2): ",
					buf, 2, DOECHO, NULL, true);
		}
		switch (buf[0])
		{
		case '1':
			getSession()->currentmemo->ud.gender = 'M';
			break;
		case '2':
			getSession()->currentmemo->ud.gender = 'F';
			break;
		}
		move(4, 0);
		prints("���������ĳ�������");
		while (getSession()->currentmemo->ud.birthyear < tmnow->tm_year - 98
			   || getSession()->currentmemo->ud.birthyear > tmnow->tm_year - 3)
		{
			buf[0] = '\0';
			getdata(5, 0, "��λ����Ԫ��: ", buf, 5, DOECHO, NULL, true);
			if (atoi(buf) < 1900)
				continue;
			getSession()->currentmemo->ud.birthyear = atoi(buf) - 1900;
		}
		while (getSession()->currentmemo->ud.birthmonth < 1 
				|| getSession()->currentmemo->ud.birthmonth > 12)
		{
			buf[0] = '\0';
			getdata(6, 0, "������: (1-12) ", buf, 3, DOECHO, NULL, true);
			getSession()->currentmemo->ud.birthmonth = atoi(buf);
		}
		do
		{
			buf[0] = '\0';
			getdata(7, 0, "������: (1-31) ", buf, 3, DOECHO, NULL, true);
			getSession()->currentmemo->ud.birthday = atoi(buf);
		} while (!is_valid_date(getSession()->currentmemo->ud.birthyear + 1900,
					getSession()->currentmemo->ud.birthmonth,
					getSession()->currentmemo->ud.birthday));
		write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
	}
#endif


#ifdef LILAC_REG_MARVEL

	if(!lilac_validid(getSession()->currentmemo->ud.ICQ)
		&& !HAS_PERM(getCurrentUser(),PERM_LOGINOK)){
		do{ 
			getdata(8, 0, "�������֤��: << ", buf, STRLEN, DOECHO, NULL, true);
			strcpy(getSession()->currentmemo->ud.ICQ, buf);
		}while (!lilac_validid(getSession()->currentmemo->ud.ICQ)); 
	} 

	if (getCurrentUser()->numlogins == 1) {
		// For new users, the default is PERM_NOIPCTRL
		getCurrentUser()->userlevel |= PERM_NOIPCTRL;
	}

	/*
	if(!lilac_validmail(getSession()->currentmemo->ud.reg_email) 
			&& !HAS_PERM(getCurrentUser(),PERM_LOGINOK) 
	//		&& HAS_PERM(getCurrentUser(),PERM_NOIPCTRL)
			&& (!verify_email_file_exist(getSession()) && !verified_email_file_exist(getSession()))
				){
		do{
			getdata(9, 0, "������ʵmail[���ڼ���id]: << ", buf, STRLEN, DOECHO, NULL, true);
			strcpy(getSession()->currentmemo->ud.email, buf);
			verify_email(getSession());
		}while (!lilac_validmail(getSession()->currentmemo->ud.email) ); 
		prints("һ�⼤�����Ѿ����͵���������(%s)��ע�����. ",buf);
		pressanykey();
	}
	*/

	if(!lilac_validmail(getSession()->currentmemo->ud.reg_email) 
			&& HAS_PERM(getCurrentUser(),PERM_LOGINOK) 
		){
		if(!verify_email_file_exist(getSession()) )
			prints("�𾴵��û�����������Ҫ�ṩ�����һع��ܣ�\n��������Ҫ�ṩһ����ʵ�����䣬\n�����ڵ�¼֮��ѡ��\n�۸��˹�������->�ۼ��������\n");
		else
			prints("�𾴵��û�������δ�����������䣬���������ڵ�¼֮��\n����ѡ��۸��˹�������->��������\n");
		pressanykey();
	}

#endif

#ifdef NEW_COMERS
	if (getCurrentUser()->numlogins == 1)
	{
		FILE *fout;
		char buf2[STRLEN];

		gettmpfilename( buf, "newcomer" );
		//sprintf(buf, "tmp/newcomer.%s", getCurrentUser()->userid);
		if ((fout = fopen(buf, "w")) != NULL)
		{
			fprintf(fout, "��Һ�,\n\n");
			fprintf(fout, "���� %s (%s), ���� %s\n", getCurrentUser()->userid,
					getCurrentUser()->username, SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
			fprintf(fout, "����%s������վ����, ���Ҷ��ָ�̡�\n",
#ifdef HAVE_BIRTHDAY
//					(curruserdata.gender == 'M') ? "С��" : "СŮ��");
					(getSession()->currentmemo->ud.gender == 'M') ? "С��" : "СŮ��");
#else
                                        "С��");
#endif
#ifndef NEWSMTH
			move(9, 0);
			prints("��������̵ĸ��˼��, ��վ����ʹ���ߴ���к�\n");
			prints("(�������, д���ֱ�Ӱ� <Enter> ����)....");
			getdata(11, 0, ":", buf2, 75, DOECHO, NULL, true);
			if (buf2[0] != '\0')
			{
				fprintf(fout, "\n\n���ҽ���:\n\n");
				fprintf(fout, "%s\n", buf2);
				getdata(12, 0, ":", buf2, 75, DOECHO, NULL, true);
				if (buf2[0] != '\0')
				{
					fprintf(fout, "%s\n", buf2);
					getdata(13, 0, ":", buf2, 75, DOECHO, NULL, true);
					if (buf2[0] != '\0')
					{
						fprintf(fout, "%s\n", buf2);
					}
				}
			}
#endif
			fclose(fout);
			sprintf(buf2, "������·: %s", getCurrentUser()->username);
			post_file(getCurrentUser(), "", buf, "newcomers", buf2, 0, 2, getSession());
			unlink(buf);
		}
		pressanykey();
	}
#endif



    if (!strcmp(getCurrentUser()->userid, "SYSOP")) {
        getCurrentUser()->userlevel = ~0;
        getCurrentUser()->userlevel &= ~PERM_SUICIDE;        /* Leeward 98.10.13 */
        getCurrentUser()->userlevel &= ~(PERM_DENYMAIL|PERM_DENYRELAX);       /* Bigman 2000.9.22 */
        getCurrentUser()->userlevel &= ~PERM_JURY;       /* �������ٲ� */
    }


    if (!(getCurrentUser()->userlevel & PERM_LOGINOK)) {
        if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
            return;
#ifdef LILAC_REG_MARVEL
        //if (!invalid_realmail(getCurrentUser()->userid, getSession()->currentmemo->ud.realemail, STRLEN - 16)){
        if (!invalid_realmail(getCurrentUser()->userid, getSession()->currentmemo->ud.reg_email, STRLEN - 16)){
			getCurrentUser()->userlevel |= PERM_LOGINOK ;

			struct userec *lookupuser;
			getuser(getCurrentUser()->userid, &lookupuser);
			if(!(lookupuser->flags & GIVEUP_FLAG))
				getCurrentUser()->userlevel |= (PERM_DEFAULT | PERM_NOIPCTRL );
		}
#else
        if (!invalid_realmail(getCurrentUser()->userid, getSession()->currentmemo->ud.realemail, STRLEN - 16)) {
            getCurrentUser()->userlevel |= PERM_DEFAULT;
            /*
            if (HAS_PERM(getCurrentUser(), PERM_DENYPOST) && !HAS_PERM(getCurrentUser(), PERM_SYSOP))
                getCurrentUser()->userlevel &= ~PERM_POST;
            */
        } 

#endif
		else {
            /* added by netty to automatically send a mail to new user. */
            /* begin of check if local email-addr  */
            /*       if (
               (!strstr( urec->email, "@bbs.") ) &&
               (!strstr( urec->email, ".bbs@") )&&
               (!invalidaddr(urec->email))&&
               sysconf_str( "EMAILFILE" )!=NULL) 
               {
               move( 15, 0 );
               prints( "���ĵ�������  ����ͨ��������֤...  \n" );
               prints( "      SYSOP ����һ����֤�Ÿ���,\n" );
               prints( "      ��ֻҪ����, �Ϳ��Գ�Ϊ��վ�ϸ���.\n" );
               getdata( 19 ,0, "��Ҫ SYSOP ����һ������?(Y/N) [Y] << ", ans,2,DOECHO,NULL,true);
               if ( *ans != 'n' && *ans != 'N' ) {
               code=(time(0)/2)+(rand()/10);
               sethomefile(genbuf,urec->userid,"mailcheck");
               if((dp=fopen(genbuf,"w"))==NULL)
               {
               fclose(dp);
               return;
               }
               fprintf(dp,"%9.9d\n",code);
               fclose(dp);
               sprintf( genbuf, "/usr/lib/sendmail -f SYSOP.bbs@%s %s ", 
               email_domain(), urec->email );
               fout = popen( genbuf, "w" );
               fin  = fopen( sysconf_str( "EMAILFILE" ), "r" );
               if ((fin != NULL) && (fout != NULL)) {
               fprintf( fout, "Reply-To: SYSOP.bbs@%s\n", email_domain());
               fprintf( fout, "From: SYSOP.bbs@%s\n",  email_domain() ); 
               fprintf( fout, "To: %s\n", urec->email);
               fprintf( fout, "Subject: @%s@[-%9.9d-]firebird mail check.\n", urec->userid ,code);
               fprintf( fout, "X-Forwarded-By: SYSOP \n" );
               fprintf( fout, "X-Disclaimer: None\n");
               fprintf( fout, "\n");
               fprintf(fout,"���Ļ����������£�\n",urec->userid);
               fprintf(fout,"ʹ���ߴ��ţ�%s (%s)\n",urec->userid,urec->username);
               fprintf(fout,"��      ����%s\n",urec->realname);
               fprintf(fout,"��վλ��  ��%s\n",urec->lasthost);
               fprintf(fout,"�����ʼ�  ��%s\n\n",urec->email);
               fprintf(fout,"�װ��� %s(%s):\n",urec->userid,urec->username);
               while (fgets( genbuf, 255, fin ) != NULL ) {
               if (genbuf[0] == '.' && genbuf[ 1 ] == '\n')
               fputs( ". \n", fout );
               else fputs( genbuf, fout );
               }
               fprintf(fout, ".\n");                                    
               fclose( fin );
               fclose( fout );                                     
               }
               getdata( 20 ,0, "���Ѽĳ�, SYSOP ����������Ŷ!! �밴 <Enter> << ", ans,2,DOECHO,NULL ,true);
               }
               }else
               {
               showansi=1;
               if(sysconf_str( "EMAILFILE" )!=NULL)
               {
               prints("\n��ĵ����ʼ���ַ ��\033[33m%s\033[m��\n",urec->email);
               prints("���� Unix �ʺţ�ϵͳ����Ͷ�����ȷ���ţ��뵽\033[32m������\033[m���޸�..\n");
               pressanykey();
               }
               }
               deleted by alex, remove email certify */

            clear();            /* Leeward 98.05.14 */
            move(12, 0);
            prints("�����ڻ�û��ͨ�������֤��û��talk,mail,message,post��Ȩ�ޡ�\n");
            prints("�����Ҫ��Ϊ%s��ע��" NAME_USER_SHORT "��", BBS_FULL_NAME);
            prints("�뵽\033[33;1m���˹�����\033[m����ϸ��ʵ����д\033[32m ע�ᵥ,\033[m\n");
            prints("��" NAME_SYSOP_GROUP "���ͨ���Ժ󣬾Ϳ��Գ�Ϊ��վ��ע��" NAME_USER_SHORT ".\n");
            prints("\n���˵�  -->  I) ���˹�����  -->  F) ��дע�ᵥ\n");
            pressreturn();
        }
        /* end of check if local email-addr */
        /*  above lines added by netty...  */
    }
#ifdef LILAC_REG_MARVEL
   	else{
		//By Marvel ��PERM_LOGINOKȨ���ж�
		// �ⲿ����ʲô��˼����ʱȥ���ɣ��� MARVEL
		//struct userec *lookupuser;
		//getuser(getCurrentUser()->userid, &lookupuser);
		//getCurrentUser()->userlevel |= (PERM_DEFAULT | PERM_NOIPCTRL );
	}


#endif
//    	curruserdata.realemail[STRLEN -16 - 1] = '\0';  //�������
    	getSession()->currentmemo->ud.realemail[STRLEN -16 - 1] = '\0';  //�������
//	write_userdata(getCurrentUser()->userid, &curruserdata);
	write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
    newregfile = sysconf_str("NEWREGFILE");
    /*if (getCurrentUser()->lastlogin - getCurrentUser()->firstlogin < REGISTER_WAIT_TIME && !HAS_PERM(getCurrentUser(), PERM_SYSOP) && newregfile != NULL) {
        getCurrentUser()->userlevel &= ~(perm);
        ansimore(newregfile, true);
    }��ע�͵�*/
}

/* ת��ID     by binxun  ... 2003.5 */
int ConveyID(void){
    FILE* fn = NULL;
	long now;
	char buf[STRLEN],filename[STRLEN],systembuf[STRLEN];
	int i;

    //���Ȩ��
        if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS) || HAS_PERM(getCurrentUser(), PERM_OBOARDS) || HAS_PERM(getCurrentUser(), PERM_ACCOUNTS)
        || HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)
        || HAS_PERM(getCurrentUser(), PERM_JURY) || HAS_PERM(getCurrentUser(), PERM_SUICIDE) || HAS_PERM(getCurrentUser(), PERM_CHATOP) || (!HAS_PERM(getCurrentUser(), PERM_POST))
        || HAS_PERM(getCurrentUser(), PERM_DENYMAIL)
        || HAS_PERM(getCurrentUser(), PERM_DENYRELAX)) {
        clear();
        move(11, 28);
		prints("\033[1;33m������������,����ת��ID!\033[m");
        pressanykey();
        return -1;
    }

	//������ʾ��Ϣ

    clear();
    move(1, 0);
    prints("ѡ��ת��ID��,���û���վ��д������,���ɻ���û�Ȩ��!\n\n");
    prints("\033[1;31m�ر�����! !ת��ID��ԭ����һ�и������϶���������!!\033[m\n\n");
    prints("\033[1;31m��ID����վ�������µ���������10��������������Ϊ0��\033[m\n");
    prints("\033[1;31m��ID�������ļ���ʱ�佫�����û�����ת��ʱ�̿�ʼ���¼��㡣\033[m\n");
    prints("\033[1;31m����ת�ú��ID���������޸�ת��ǰ����������£����ܹ���ת��ǰ�������Blog��\033[m\n");
    prints("\033[1;31mͬʱ�����ų�ת��ID���������������δ�����Ĳ���֮����\033[m\n\n");
    prints("\033[1;31mһ��ȷ��ת�ã������ɻָ����������ؿ��ǡ�\033[m\n");
    move(12,0);

    if (askyn("��ȷ��Ҫת����� ID ��", 0) == 1) {
        clear();
        getdata(0, 0, "������ԭ����(������ȷ�Ļ������̶���): ", buf, 39, NOECHO, NULL, true);   /*Haohmaru,98.10.12,check the passwds */
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return -1;
        }

        //��¼������Ϣ
        now = time(0);
		gettmpfilename( filename, "convey" );
        //sprintf(filename, "tmp/%s.tmp", getCurrentUser()->userid);
        fn = fopen(filename, "w");
		if(fn){
			fprintf(fn,"\033[1m %s \033[m �� \033[1m%24.24s\033[m ת��ID��,�������������ϣ��뱣��...",getCurrentUser()->userid,ctime(&now));
			getuinfo(fn, getCurrentUser());
			fprintf(fn, "\n                     \033[1m ϵͳ�Զ�����ϵͳ��\033[m\n");
			fclose(fn);
			sprintf(buf, "%s ת��ID�ı�������", getCurrentUser()->userid);
			post_file(getCurrentUser(), "", filename, "Registry", buf, 0, 1,getSession());
			unlink(filename);
		}
		else{
		    move(15,0);
			prints("����������ʱ�ļ�!ת��IDʧ��,����SYSOP��ϵ.");
			return -1;
		}

		//������д��ڵ������ļ�,����
		setmailpath(buf,getCurrentUser()->userid);
		sprintf(systembuf,"/bin/rm -fr %s",buf);
		system(systembuf);
		sethomepath(buf,getCurrentUser()->userid);
		sprintf(systembuf,"/bin/rm %s/*",buf);
		system(systembuf);
		sprintf(systembuf,"/bin/rm %s/.*",buf);
        system(systembuf);

		//����ת��ID�ļ�
        sethomefile(filename,getCurrentUser()->userid,"conveyID");
		if((fn=fopen(filename,"w")) != NULL){
		    fprintf(fn,"Convey ID at %s",ctime(&now));
			fclose(fn);
		}
		else{
		    move(15,0);
		    prints("��������ת��ID�ļ�!ת��IDʧ��,����SYSOP��ϵ.");
			return -1;
		}
		getCurrentUser()->userlevel = 0;
		getCurrentUser()->userlevel |= PERM_BASIC;

		getCurrentUser()->numposts = 0;
		if(getCurrentUser()->numlogins > 10)getCurrentUser()->numlogins = 10;
		getCurrentUser()->stay = 0;
		strncpy(getCurrentUser()->username,getCurrentUser()->userid,IDLEN);
		SET_UNDEFINE(getCurrentUser(),DEF_NOTMSGFRIEND);
		SET_UNDEFINE(getCurrentUser(),DEF_SHOWREALUSERDATA);

        if (convcode)
            SET_UNDEFINE(getCurrentUser(),DEF_USEGB);

        getCurrentUser()->flags |= PAGER_FLAG;
        getCurrentUser()->title = 0;
		for(i = 0; i < MAXCLUB>>5 ; i++){
		    getCurrentUser()->club_read_rights[i] = 0;
			getCurrentUser()->club_write_rights[i] = 0;
		}
		getCurrentUser()->signature = 0;
		getCurrentUser()->usedspace = 0;

		//clear �û���Ϣ
//		bzero(&curruserdata,sizeof(struct userdata));
		bzero(&(getSession()->currentmemo->ud),sizeof(struct userdata));
//		strcpy(curruserdata.userid,getCurrentUser()->userid);
		strcpy(getSession()->currentmemo->ud.userid,getCurrentUser()->userid);
//		write_userdata(getCurrentUser()->userid,&curruserdata);
		write_userdata(getCurrentUser()->userid,&(getSession()->currentmemo->ud));

        move(12,0);
		prints("ת��ID�ɹ�,���϶�����,������ID��.");
        pressanykey();
		//����
        abort_bbs(0);
    }
    return 0;
}

/*�趨ID���뱣�� by binxun 2003.10 */
int ProtectID(void){
	char buf[STRLEN],print_buf[STRLEN];
	struct protect_id_passwd protect;
	FILE* fp;
	
	clear();
	if(!HAS_PERM(getCurrentUser(),PERM_LOGINOK)) {
        	move(11, 28);
		prints("\033[1;33m����δͨ�������֤,�����趨���뱣��!\033[m");
        	pressanykey();
		return -1;
        }
	
	sethomefile(buf,getCurrentUser()->userid,"protectID");
	if(dashf(buf)) {
        	move(11, 28);
		prints("\033[1;33m���Ѿ��趨���뱣������,�����ٸ����趨!\033[m");
        	pressanykey();
		return -1;
	}
	    
	move(1, 0);
    	prints("ѡ�����뱣�����ܺ�,��������������������뱻���õ������,���������趨����Ϣ");
	move(2, 0);
	prints("�����һ��Լ�������.");
    	move(4, 0);
    	prints("��Щ�趨����Ϣ����:\033[1;31m ����/����/Email/������ʾ����/�����\033[m");
    	move(6,0);
	//prints("�������Ϣ�������, ���뱣������һ���ɹ��򿪺�, �������ٸ�����Щ��Ϣ,�м�,�м�!");
	//move(7,0);
	prints("�һ�����ʱ,��Ҫ ����/����/����� ���趨��ȫһ��,�ŻὫ�����뷢�� Email .");

	move(8,0);
    	if (askyn("��ȷ��Ҫ�����뱣��������", 0) == 0) {
		return -1;
	}
	
	clear();
	memset(&protect, 0 , sizeof(struct protect_id_passwd));
	//�������������Ϣ

	move(1,0);
	prints("�������޸�,ֱ�Ӱ� <ENTER> ����ʹ�� [] �ڵ����ϡ�\n");
	
	sprintf(print_buf,"������������ʵ����: [%s]",getSession()->currentmemo->ud.realname);
	getdata(3, 0, print_buf, buf, NAMELEN, DOECHO, NULL, true);
	if(buf[0])
		strncpy(protect.name,buf,NAMELEN);
	else
		strncpy(protect.name,getSession()->currentmemo->ud.realname,NAMELEN);
	
#ifdef HAVE_BIRTHDAY
	move(4,0);
	prints("���������ĳ�������: ");
	
	sprintf(print_buf,"��λ����Ԫ��: [%d]",getSession()->currentmemo->ud.birthyear);
	while (protect.birthyear > 2010 || protect.birthyear < 1900) {
		getdata(5, 0, print_buf, buf, 5, DOECHO, NULL, true);
		if(buf[0]) protect.birthyear = atoi(buf);
		else
			protect.birthyear = getSession()->currentmemo->ud.birthyear;
	}

	sprintf(print_buf,"������: [%d]",getSession()->currentmemo->ud.birthmonth);
	while (protect.birthmonth < 1 || protect.birthmonth > 12) {
		getdata(6, 0, print_buf, buf, 3, DOECHO, NULL, true);
		if(buf[0]) protect.birthmonth = atoi(buf);
		else
			protect.birthmonth = getSession()->currentmemo->ud.birthmonth;
	}
	
	sprintf(print_buf,"������: [%d]",getSession()->currentmemo->ud.birthday);
	while (protect.birthday < 1 || protect.birthday > 31) {
		getdata(7, 0, print_buf, buf, 3, DOECHO, NULL, true);
		if(buf[0]) protect.birthday = atoi(buf);
		else
			protect.birthday = getSession()->currentmemo->ud.birthday;
	}
#endif

	sprintf(print_buf,"����Email: ");
	do {
		getdata(8, 0, print_buf, buf, STRLEN, DOECHO, NULL, true);
	} while(!strchr(buf,'@'));
	strncpy(protect.email, buf, STRLEN);
	
	sprintf(print_buf,"������ʾ����: ");
	do {
		getdata(9, 0, print_buf, buf, STRLEN, DOECHO, NULL, true);
	} while(!buf[0]);
	strncpy(protect.question, buf, STRLEN);
	
	sprintf(print_buf,"�����(�����ĸ��ַ�): ");
	do {
		getdata(10, 0, print_buf, buf, STRLEN, DOECHO, NULL, true);
	} while(strlen(buf) < 4);
	strncpy(protect.answer, buf, STRLEN);

	if (askyn("��ȷ��Ҫ�趨��", 0) == 1) {	
		move(12,0);
		sethomefile(buf,getCurrentUser()->userid,"protectID");	
		
		fp = fopen(buf,"w");
		if(!fp) {
			prints("���ܴ��ļ�,����SYSOP��ϵ.");	
			return 0;
		}
		fwrite(&protect,sizeof(struct protect_id_passwd),1,fp);
		fclose(fp);
		
		prints("���뱣���Ѿ��趨");
		pressanykey();
	}
    return 0;
}
