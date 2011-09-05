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
/* KCN modified 2001.5.10 */ 

#include "bbs.h"
#include "lilacfuncs.h"
extern time_t login_start_time;

void disply_userinfo(struct userec *u, int real)
{
    int num, diff;
	struct userdata ud;

	read_userdata(u->userid, &ud);
    move(real >= 1 ? 2 : 3, 0);
    clrtobot();
    prints("���Ĵ���     : %s\n", u->userid);

    prints("�����ǳ�     : %s\n", u->username);
#ifdef LILAC
    if (HAS_PERM(getCurrentUser(), PERM_ACCOUNTS) )
	{
#endif
    if (real != 2)  //ע�ᵥ����ʾ��ʵ����
        prints("��ʵ����     : %s\n", ud.realname);
    prints("��ססַ     : %s\n", ud.address);
    if(real)
	prints("ע��E-mail   : %s\n",ud.reg_email);
    else
    	prints("�����ʼ����� : %s\n", ud.email);

	/*�������յ���ʾ added by binxun 2003.5.20*/
#ifdef HAVE_BIRTHDAY
    prints("�����Ա�     : %s\n",(ud.gender=='M')?"��":"Ů");
	prints("��������     : %d-%d-%d\n",ud.birthyear+1900,ud.birthmonth,ud.birthday);
#endif
#ifdef ZIXIA
    prints("������������ : %d\n",u->altar);
#endif

#ifdef LILAC
    prints("�������֤�� : %s\n",ud.ICQ);
#else
    prints("����ְ��     : %s\n",get_user_title(u->title));
#endif

    if (real) {
        prints("ԭʼע������ : %s\n", ud.realemail);
    }
#ifdef LILAC
	}
#endif
    prints("ע������     : %s", ctime(&u->firstlogin));
    prints("����������� : %s", ctime(&u->lastlogin));
    if (real) {
        prints("������ٻ��� : %s\n", u->lasthost);
    }

    prints("��վ����     : %d ��\n", u->numlogins);
    if (real)
        prints("������Ŀ     : %d ƪ\n", u->numposts);

        /*       if( real ) {
           prints("������Ŀ     : %d / %d (Board/1Discuss)\n",
           u->numposts, post_in_tin( u->userid ));
           }  removed by stephen 2000-11-02 */
        /*    } */
#if 0
        {
           int exp;
           exp=countexp(u);
           prints("����ֵ       : %d(%s)\n",exp,c_exp(exp));
           exp=countperf(u);
           prints("����ֵ       : %d(%s)\n",exp,c_perf(exp));
        }
#endif
        prints("��վ��ʱ��   : %d Сʱ %d ����\n", u->stay / 3600, (u->stay / 60) % 60);
#ifdef NEWSMTH
    if(u->score_user>publicshm->us_sample[1])
        sprintf(genbuf,"%d -- RANKING %.2lf%%",u->score_user,100*us_ranking(u->score_user));
    else
        sprintf(genbuf,"%d -- RANKING %.1lf%%",u->score_user,100*us_ranking(u->score_user));
    prints("����         : %s\n",genbuf);
#else
#ifdef LILAC
	if (real) {
	prints("���֤��     : %s\n", ud.ICQ);
#ifdef LILAC_SCORE
	sprintf(genbuf,"%d  %.3lf%%\n", u->score, (float)u->score_rank/1000);
    prints("�û�����     : %s", genbuf);
#endif
	}    
#else
    prints("\n");
#endif
#endif
    if (real) {
        strcpy(genbuf, XPERMSTR);
        for (num = 0; num < (int) strlen(genbuf); num++)
            if (!(u->userlevel & (1 << num)))
                genbuf[num] = '-';
        genbuf[num] = '\0';
        prints("ʹ����Ȩ��   : %s\n", genbuf);
    } else {
        diff = (time(0) - login_start_time) / 60;
        prints("ͣ���ڼ�     : %d Сʱ %02d ��\n", diff / 60, diff % 60);
        prints("��Ļ��С     : %dx%d\n", t_lines, t_columns);
    }
    prints("\n");
    if (u->userlevel & PERM_LOGINOK) {
        prints("  ����ע������Ѿ����, ��ӭ���뱾վ.\n");
    } else if (u->lastlogin - u->firstlogin < 3 * 86400) {
        prints("  ������·, ���Ķ� Announce ������.\n");
    } else {
        prints("  ע����δ�ɹ�, ��ο���վ��վ����˵��.\n");
    }
}
int uinfo_query(struct userec *u, int real, int unum) 
{
    struct userec newinfo;
	const char *emailfile;
    char ans[3], buf[STRLEN], genbuf[STRLEN];
    int i, fail = 0, netty_check = 0, tmpchange = 0;
    FILE * fin, *fout, *dp;
    time_t code;
	struct userdata ud;
	struct usermemo *um;
	
	time_t now;
	struct tm *tmnow;

	now = time(0);
	tmnow = localtime(&now);

    memcpy(&newinfo, u, sizeof(struct userec));
	read_userdata(u->userid, &ud);
	read_user_memo(u->userid, &um);
	//memcpy(&ud, &(getSession()->currentmemo->ud), sizeof(ud));
    getdata(t_lines - 1, 0, real ? "��ѡ�� (0)���� (1)�޸����� (2)�趨���� (3) �� ID (4) �Ļ���==> [0]" : "��ѡ�� (0)���� (1)�޸����� (2)�趨���� ==> [0]", ans, 2, DOECHO, NULL, true);
    clear();
    i = 3;
    move(i++, 0);
    if (ans[0] != '3' || real)
        prints("ʹ���ߴ���: %s\n", u->userid);
    switch (ans[0]) {
    case '1':
        move(1, 0);
        prints("�������޸�,ֱ�Ӱ� <ENTER> ����ʹ�� [] �ڵ����ϡ�\n");
        sprintf(genbuf, "�ǳ� [%s]: ", u->username);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(newinfo.username, buf, NAMELEN);
        sprintf(genbuf, "��ʵ���� [%s]: ", ud.realname);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(ud.realname, buf, NAMELEN);
        sprintf(genbuf, "��ס��ַ [%s]: ", ud.address);
        getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(ud.address, buf, NAMELEN);
        sprintf(genbuf, "�������� [%s]: ", ud.email);
        getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
        if (buf[0])
             {

                /*netty_check = 1; */
                /* ȡ��email ��֤, alex , 97.7 */
                strncpy(ud.email, buf, STRLEN);
            }
		/* �����޸����յ� added by binxun 2003.5.20 */
#ifdef  HAVE_BIRTHDAY
        sprintf(genbuf, "�����Ա�: [1]�� [2]Ů (1 or 2)[%d]",(ud.gender=='M')?1:2);
	    do{
            getdata(i, 0, genbuf,buf, 2, DOECHO, NULL, true);
			if(buf[0]==0)break;
        }while (buf[0] < '1' || buf[0] > '2');
        i++;
		switch (buf[0])
		{
		case '1':
			ud.gender = 'M';
			break;
		case '2':
			ud.gender = 'F';
			break;
		default:
		    break;
		}

		prints("���������ĳ�������");
		i++;
        do{
			buf[0] = '\0';
			sprintf(genbuf, "��λ����Ԫ��: [%d]: ", ud.birthyear+1900);
			getdata(i, 0, genbuf, buf, 5, DOECHO, NULL, true);
			if(buf[0]) {
				if (atoi(buf) < 1900)continue;
				ud.birthyear = atoi(buf) - 1900;
			}
		}while (ud.birthyear < tmnow->tm_year - 98 || ud.birthyear > tmnow->tm_year - 3);
		i++;

        do{
			buf[0] = '\0';
			sprintf(genbuf, "������: (1-12) [%d]: ", ud.birthmonth);
			getdata(i, 0, genbuf, buf, 3, DOECHO, NULL, true);
			if(buf[0]) {
				ud.birthmonth = atoi(buf);
			}
		}while (ud.birthmonth < 1 || ud.birthmonth > 12);
		i++;

        do
		{
			buf[0] = '\0';
			sprintf(genbuf, "������: (1-31) [%d]: ", ud.birthday);
			getdata(i, 0, genbuf, buf, 3, DOECHO, NULL, true);
			if((buf[0] && atoi(buf) >= 1 && atoi(buf) <= 31))
				ud.birthday = atoi(buf);
		}while (!is_valid_date(ud.birthyear + 1900,
					ud.birthmonth,
					ud.birthday));
		i++;
#endif

        if (real) {
	usertitle:
	     sprintf(genbuf, "��ǰְ��: %s[%d](����ֱ������ְ������0��ȥְ��): ", get_user_title(u->title),u->title);
         getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
         if (buf[0]) {
            if (!strcmp(buf, "0")) {
                newinfo.title = 0;
            } else {
                unsigned char ititle,tflag;
                ititle = atoi(buf);
                if (ititle > 0) {
                    newinfo.title = ititle;
                } else {
                    ititle=0;tflag=0;
                    do{
                        ititle++;
                        if(!strcmp(buf,get_user_title(ititle))){
                            newinfo.title=ititle;
                            tflag=1;
                            break;
                        }
                    }while(ititle<255);
                    if(!tflag){
                        prints("ְ�����û�д�ְ�������޸��û�ְ���\n");
                        pressreturn();
                        i--;
                        goto usertitle;
                    }
                }
            }
        }
            sprintf(genbuf, "��ʵEmail[%s]: ", ud.realemail);
            getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
            if (buf[0])
                strncpy(ud.realemail, buf, STRLEN - 16);
            sprintf(genbuf, "���ߴ��� [%d]: ", u->numlogins);
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (atoi(buf) > 0)
                newinfo.numlogins = atoi(buf);
            sprintf(genbuf, "������Ŀ [%d]: ", u->numposts);
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
             {
                int lres;

                lres = atoi(buf);
                if (lres > 0 || ('\0' == buf[1] && '0' == *buf))
                    newinfo.numposts = lres;
            }
            
                /*            if( atoi( buf ) > 0 ) newinfo.numposts = atoi( buf ); */ 
                sprintf(genbuf, "��ע��������ǰ" REGISTER_WAIT_TIME_NAME " [Y/N]");
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (buf[0] == 'y' || buf[0] == 'Y')
                newinfo.firstlogin -= REGISTER_WAIT_TIME;
            sprintf(genbuf, "�����������������Ϊ������[Y/N]");
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (buf[0] == 'y' || buf[0] == 'Y')
                newinfo.lastlogin = time(0);
        }
        break;
    case '3':
        if (!real) {
			end_mmapfile(um, sizeof(struct usermemo), -1);
            clear();
            return 0;
        }
        
            /* Bigman 2000.10.2 �޸�ʹ����IDλ������ */ 
            getdata(i++, 0, "�µ�ʹ���ߴ���: ", genbuf, 16, DOECHO, NULL, true);
	 if (strncmp(genbuf,u->userid, IDLEN+2)&&!strncasecmp(genbuf,u->userid, IDLEN+2)) tmpchange = 1;
        if (*genbuf != '\0') {
            if (searchuser(genbuf) && !tmpchange) {
                prints("\n����! �Ѿ���ͬ�� ID ��ʹ����\n");
                fail++;
                break;
            } else {
                strncpy(newinfo.userid, genbuf, IDLEN + 2);
				strncpy(ud.userid, genbuf, IDLEN + 2);
            }
        } 
        break;
	case '4':
		if (!real) {
			end_mmapfile(um, sizeof(struct usermemo), -1);
			clear();
			return 0;
		}
		sprintf( genbuf, "�µĻ���[%d]: ", u->score );
		getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
		
		int new_score = atoi(buf);
		if ( new_score > 0 || ('\0' == buf[1] && '0' == *buf) )
			newinfo.score = new_score;
		int sc = new_score - u->score;
		if ( sc >= 0 )
		{
			sprintf( genbuf, "�������� %d \n������˴λ��ֲ���������: ", new_score - u->score );
		}
		else
		{
			sprintf( genbuf, "���ּ��� %d \n������˴λ��ֲ���������: ", u->score - new_score );
		}
		getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
		FILE* score_fd = fopen("etc/posts/update_score","w+");
		fprintf( score_fd, "˵����\n");
		fprintf( score_fd, "        %s\n", buf );
		fclose( score_fd );

		break;
        
            /* fall throw, must change passwd for newid, by wwj 2001/5/7 */ 
    case '2':
        if (!real) {
            getdata(i++, 0, "������ԭ����: ", buf, 39, NOECHO, NULL, true);
            if (*buf == '\0' || !checkpasswd2(buf, u)) {
                prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
                fail++;
                break;
            }
        }
        getdata(i++, 0, "���趨������: ", buf, 39, NOECHO, NULL, true);
        if (buf[0] == '\0') {
            prints("\n\n�����趨ȡ��, ����ʹ�þ�����\n");
            if(ans[0]!='3')
                fail++;
            break;
        }
        getdata(i++, 0, "����������������: ", genbuf, 39, NOECHO, NULL, true);
        if (strcmp(buf, genbuf)) {
            prints("\n\n�������벻һ��, �޷��趨�����롣\n");
            fail++;
            break;
        }
        
            /*      Added by cityhunter to deny others to modify SYSOP's passwd */ 
            if (real && (strcmp(u->userid, "SYSOP") == 0))
             {
            prints("\n\n����!ϵͳ��ֹ�޸�SYSOP������," NAME_POLICE "��������·�� :)");
            pressreturn();
            clear();
			end_mmapfile(um, sizeof(struct usermemo), -1);
            return 0;
            }
        
            /* end of this addin */ 
            setpasswd(buf, &newinfo);
        break;
    default:
        clear();
		end_mmapfile(um, sizeof(struct usermemo), -1);
        return 0;
    }
    if (fail != 0) {
        pressreturn();
        clear();
		end_mmapfile(um, sizeof(struct usermemo), -1);
        return 0;
    }
    for (;;)
	{
        getdata(t_lines - 1, 0, "ȷ��Ҫ�ı���?  (Yes or No): ", ans, 2, DOECHO, NULL, true);
        if (*ans == 'n' || *ans == 'N')
            break;
        if (*ans == 'y' || *ans == 'Y') {
			if (!real) {
                update_user(&newinfo, getSession()->currentuid, 1);
                if (strcmp(newinfo.username, uinfo.username)) {
                    strcpy(uinfo.username, newinfo.username);
                    UPDATE_UTMP_STR(username, uinfo);
                }
            }
	     if (strcmp(u->userid, newinfo.userid)) {
                char src[STRLEN], dst[STRLEN];
                if (id_invalid(newinfo.userid) == 1) {
                    prints("�ʺű�����Ӣ����ĸ��������ɣ����ҵ�һ���ַ�������Ӣ����ĸ!\n");
                    pressanykey();
                } else {
                    setmailpath(src, u->userid);
                    setmailpath(dst, newinfo.userid);
                
                    /*
                       sprintf(genbuf,"mv %s %s",src, dst);
                     */ 
                    f_mv(src, dst);
                    sethomepath(src, u->userid);
                    sethomepath(dst, newinfo.userid);
                
                    /*
                       sprintf(genbuf,"mv %s %s",src ,dst);
                     */ 
                    f_mv(src, dst);
                    sprintf(src, "tmp/email/%s", u->userid);
                    unlink(src);
                    setuserid(unum, newinfo.userid);
                }
            }
            
                /* added by netty to automatically send a mail to new user. */ 
                if ((netty_check == 1))
                 {
                if ((strchr(ud.email, '@') != NULL) && 
                     (!strstr(ud.email, "@firebird.cs")) && (!strstr(ud.email, "@bbs.")) && (!invalidaddr(ud.email)) && (!strstr(ud.email, ".bbs@"))) {
                    if ((emailfile = sysconf_str("EMAILFILE")) != NULL)
                         {
                        code = (time(0) / 2) + (rand() / 10);
                        sethomefile(genbuf, u->userid, "mailcheck");
                        if ((dp = fopen(genbuf, "w")) == NULL)
                             {
                            fclose(dp);
                            return -1;
                            }
                        fprintf(dp, "%9.9lu\n", code);
                        fclose(dp);
                        sprintf(genbuf, "/usr/lib/sendmail -f SYSOP@%s %s ", email_domain(), ud.email);
                        fout = popen(genbuf, "w");
                        fin = fopen(emailfile, "r");
                        if (fin == NULL || fout == NULL)
                            return -1;
                        fprintf(fout, "Reply-To: SYSOP@%s\n", email_domain());
                        fprintf(fout, "From: SYSOP@%s\n", email_domain());
                        fprintf(fout, "To: %s\n", ud.email);
                        fprintf(fout, "Subject: @%s@[-%9.9lu-]firebird mail check.\n", u->userid, code);
                        fprintf(fout, "X-Forwarded-By: SYSOP \n");
                        fprintf(fout, "X-Disclaimer: None\n");
                        fprintf(fout, "\n");
                        fprintf(fout, "���Ļ����������£�\n");
                        fprintf(fout, "ʹ���ߴ��ţ�%s (%s)\n", u->userid, u->username);
                        fprintf(fout, "��      ����%s\n", ud.realname);
                        fprintf(fout, "��վλ��  ��%s\n", u->lasthost);
                        fprintf(fout, "�����ʼ�  ��%s\n\n", ud.email);
                        fprintf(fout, "�װ��� %s(%s):\n", u->userid, u->username);
                        while (fgets(genbuf, 255, fin) != NULL) {
                            if (genbuf[0] == '.' && genbuf[1] == '\n')
                                fputs(". \n", fout);
                            
                            else
                                fputs(genbuf, fout);
                        }
                        fprintf(fout, ".\n");
                        fclose(fin);
                        pclose(fout);
                        }
                } else
                     {
                    if (sysconf_str("EMAILFILE") != NULL)
                         {
                        move(t_lines - 5, 0);
                        prints("\n��ĵ����ʼ���ַ ��\033[33m%s\033[m��\n", ud.email);
                        prints("���� Unix �ʺţ�ϵͳ����Ͷ�����ȷ���ţ��뵽\033[32m������\033[m���޸�..\n");
                        pressanykey();
                        }
                    }
                }
			char fname[100];
			bool sflag = false;
			if ( u->score < newinfo.score )
			{
				sflag = true;
				sprintf( fname, "[����] ���� %s ���� %d ��", u->userid, newinfo.score - u->score );
				post_file(NULL, "", "etc/posts/update_score", "ScoreRecords", fname, 0, 1, getSession());
			}
			else if( u->score > newinfo.score )
			{
				sflag = true;
				sprintf( fname, "[����] ���� %s ���� %d ��", u->userid, u->score - newinfo.score );
				post_file(NULL, "", "etc/posts/update_score", "ScoreRecords", fname, 0, 1, getSession());
			}
            update_user(&newinfo, unum, 1);
			
			if(strcmp(um->ud.realname, ud.realname)){
    			newbbslog(BBSLOG_USER, "ChangeName '%s' to '%s'", um->ud.realname, ud.realname);
			}
			memcpy(&(um->ud), &ud, sizeof(ud));
			write_userdata(newinfo.userid, &ud);
            if (real)
			{
                char secu[STRLEN];

                if (strcmp(u->userid, newinfo.userid))
                    sprintf(secu, "%s �� ID �� %s ��Ϊ %s", u->userid, getCurrentUser()->userid, newinfo.userid);   /*Haohmaru.99.5.6 */
                
                else if ( sflag )
					sprintf( secu, "�޸� %s �Ļ���", u->userid );
				else
                    sprintf(secu, "�޸� %s �Ļ������ϻ����롣", u->userid);
                securityreport(secu, &newinfo, NULL, getSession());
            }
            break;             /*Haohmaru.98.01.10.faint...Luzi�Ӹ�forѭ��Ҳ��break! */
        }
	} /* for(;;) */
    clear();
	end_mmapfile(um, sizeof(struct usermemo), -1);
    return 0;
}
int x_info(void){
    modify_user_mode(GMENU);
    disply_userinfo(getCurrentUser(), 1);
    if (!strcmp("guest", getCurrentUser()->userid)) {
        pressreturn();
        return -1;
    }
    uinfo_query(getCurrentUser(), 0, getSession()->currentuid);
    return 0;
}

    void getfield(line, info, desc, buf, len)  int line, len;
    char *info, *desc, *buf;


{
    char prompt[STRLEN];

    
        /*    sprintf( genbuf, "  ԭ���趨: %-46.46s (%s)", buf, info ); */ 
        sprintf(genbuf, "  ԭ���趨: %-20.20s (%s)", buf, info);
    move(line, 0);
    prints(genbuf);
    sprintf(prompt, "  %s: ", desc);
    getdata(line + 1, 0, prompt, genbuf, len, DOECHO, NULL, true);
    if (genbuf[0] != '\0') {
        strncpy(buf, genbuf, len);
    }
    move(line, 0);
    clrtoeol();
    prints("  %s: %s\n", desc, buf);
    clrtoeol();
}
int x_fillform(void){
    char rname[NAMELEN], addr[STRLEN];
    char phone[STRLEN], career[STRLEN], birth[STRLEN];
    char ans[5], *mesg, *ptr;
    FILE * fn;
    time_t now;
    struct userdata ud;

    modify_user_mode(NEW);
    move(3, 0);
    clrtobot();
    if (!strcmp("guest", getCurrentUser()->userid)) {
        prints("��Ǹ, ���� new ����һ�����ʺź����������.");
        pressreturn();
        return -1;
    }
    if (getCurrentUser()->userlevel & PERM_LOGINOK) {
        prints("�������ȷ���Ѿ��ɹ�, ��ӭ���뱾վ������.");
        pressreturn();
        return -1;
    }


#ifdef LILAC_REG_MARVEL
	if(!lilac_validmail(getSession()->currentmemo->ud.reg_email)){
        prints("Emailû�б���֤�����������޷���дע����Ϣ\n");
		getdata(t_lines - 1, 0, "���·�����֤�ż� (Y/N)? [N]: ", ans, 3, DOECHO, NULL, true);
		if(ans[0]!='y' && ans[0]!='Y')
        	return -1;
        genbuf[0] = '\0';
        while (!lilac_validmail(genbuf) ) {
            getfield(10, "���ڼ������ID", "Email", getSession()->currentmemo->ud.email, STRLEN);
        }
        verify_email(getSession());
		prints("һ�⼤�����Ѿ����͵���������(%s)��ע�����. ", genbuf);
        pressreturn();
        return 0;

    }
#endif

    if ((time(0) - getCurrentUser()->firstlogin) < REGISTER_WAIT_TIME)
	{
        prints("���״ε��뱾վδ��" REGISTER_WAIT_TIME_NAME "...");
        prints("�����Ĵ���Ϥһ�£�����" REGISTER_WAIT_TIME_NAME "�Ժ�����дע�ᵥ��");
        pressreturn();
        return -1;
	}
    
	if ((fn = fopen("new_register", "r")) != NULL) {
        while (fgets(genbuf, STRLEN, fn) != NULL) {
            if ((ptr = strchr(genbuf, '\n')) != NULL)
                *ptr = '\0';
            if (strncmp(genbuf, "userid: ", 8) == 0 && strcmp(genbuf + 8, getCurrentUser()->userid) == 0) {
                fclose(fn);
                prints("վ����δ��������ע�����뵥, �����ĵȺ�.");
                pressreturn();
                return -1;
            }
        }
        fclose(fn);
    }
    
	/* added by KCN 1999.10.25 */ 
	ansimore("etc/register.note", false);
    getdata(t_lines - 1, 0, "��ȷ��Ҫ��дע�ᵥ�� (Y/N)? [N]: ", ans, 3, DOECHO, NULL, true);
    if (ans[0] != 'Y' && ans[0] != 'y')
        return -1;
//    memcpy(&ud,&curruserdata,sizeof(ud));
    memcpy(&ud,&(getSession()->currentmemo->ud),sizeof(ud));
    strncpy(rname, ud.realname, NAMELEN);
    strncpy(addr, ud.address, STRLEN);
    career[0] = phone[0] = birth[0] = '\0';
    clear();
    while (1) {
        move(3, 0);
        clrtoeol();
        prints("%s ����, ���ʵ��д���µ�����(��ʹ������):\n", getCurrentUser()->userid);
        genbuf[0] = '\0';      /*Haohmaru.99.09.17.�������ݲ��ù��� */
        while (strlen(genbuf) < 3) {
            getfield(6, "��������,��������ĺ�������ƴ��", "��ʵ����", rname, NAMELEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 2) {
            getfield(8, "ѧУϵ����λȫ�Ƽ���������", "����λ", career, STRLEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 6) {
            getfield(10, "����嵽���һ����ƺ���", "Ŀǰסַ", addr, STRLEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 2) {
            getfield(12, "����������ʱ��,���޿��ú�����Email��ַ����", "����绰", phone, STRLEN);
        }
#ifndef HAVE_BIRTHDAY
        getfield(14, "��.��.��(��Ԫ)", "��������", birth, STRLEN);
#endif
        mesg = "���������Ƿ���ȷ, �� Q ����ע�� (Y/N/Quit)? [N]: ";
        getdata(t_lines - 1, 0, mesg, ans, 3, DOECHO, NULL, true);
        if (ans[0] == 'Q' || ans[0] == 'q')
            return -1;
        if (ans[0] == 'Y' || ans[0] == 'y')
            break;
    }
    strncpy(ud.realname, rname, NAMELEN);
    strncpy(ud.address, addr, STRLEN);
	write_userdata(getCurrentUser()->userid, &ud);
//	memcpy(&curruserdata,&ud,sizeof(ud));
	memcpy(&(getSession()->currentmemo->ud),&ud,sizeof(ud));
    if ((fn = fopen("new_register", "a")) != NULL) {
        now = time(NULL);
        fprintf(fn, "usernum: %d, %s", getSession()->currentuid, ctime(&now));
        fprintf(fn, "userid: %s\n", getCurrentUser()->userid);
        fprintf(fn, "realname: %s\n", rname);
        fprintf(fn, "career: %s\n", career);
        fprintf(fn, "addr: %s\n", addr);
        fprintf(fn, "phone: %s\n", phone);
#ifndef HAVE_BIRTHDAY
        fprintf(fn, "birth: %s\n", birth);
#else
        fprintf(fn, "birth: %d-%d-%d\n", ud.birthyear, ud.birthmonth, ud.birthday);
#endif
        fprintf(fn, "----\n");
        fclose(fn);
    }
    return 0;
}

/* etnlegend, 2006.09.21, �޸��û����Ͻӿ�... */

#define MU_LENGTH                       256
#define MU_ITEM                         20
#define MU_KEY_RESET                    '~'
#define MU_GEN_N(s)                     (s)
#define MU_GEN_R(s)                     ("\033[1;31m" s "\033[m")
#define MU_GEN_Y(s)                     ("\033[1;33m" s "\033[m")
#define MU_GEN_G(s)                     ("\033[1;32m" s "\033[m")
#define MU_GEN_C(s)                     ("\033[1;36m" s "\033[m")
#define MU_MSG(f,s)                     (MU_GEN_##f(s))
#define MU_PUT(r,s)                                                                             \
    do{                                                                                         \
        move((r),0);                                                                            \
        clrtoeol();                                                                             \
        move((r),2);                                                                            \
        prints("%s\033[0;33m<Enter>\033[m",(s));                                                \
        WAIT_RETURN;                                                                            \
    }                                                                                           \
    while(0)
#define MU_GET_CORE(r,p,b,l,t)                                                                  \
    do{                                                                                         \
        if(r==(MU_ITEM+2)){                                                                     \
            move((MU_ITEM+1),0);                                                                \
            clrtobot();                                                                         \
        }                                                                                       \
        else{                                                                                   \
            move((r),0);                                                                        \
            clrtoeol();                                                                         \
        }                                                                                       \
        getdata((r),2,(p),(b),((l)+1),(t),NULL,true);                                           \
    }                                                                                           \
    while(0)
#define MU_GET(r,p,b,l)                 MU_GET_CORE(r,p,b,l,DOECHO)
#define MU_GETPWD(r,p,b,l)              MU_GET_CORE(r,p,b,l,NOECHO)

struct mu_item{
    int x;
    int y;
    int key;
    int access;
    const char *prefix;
    const char *menu;
};

struct mu_arg{
    enum {MU_MENU_INIT,MU_MENU_SELECT,MU_MENU_RESET,MU_MENU_QUIT} type;
    const unsigned int *access;
    const struct mu_item *item;
};

struct mu_so_arg{
    char from[18][IPLEN];
    int count;
};

struct mu_so_key{
    const char *value;
    int count;
};

static inline int mu_digit_string(const char *s){
    while(isdigit(*s++))
        continue;
    return (!*--s);
}

static inline int mu_show_help(int n){
    static const char *help[MU_ITEM]={
        "ѡ�����ɽ����û������޸���ز���...",
        "ѡ�����ɽ����û������޸���ز���...",
        "ѡ�����ɽ����û��ǳ��޸���ز���...",
        "ѡ�����ɽ����û���ʵ�����޸���ز���...",
        "ѡ�����ɽ����û��Ա��޸���ز���...",
        "ѡ�����ɽ����û����������޸���ز���...",
        "ѡ�����ɽ����û�ͨѶ��ַ�޸���ز���...",
        "ѡ�����ɽ����û������ʼ���ַ�޸���ز���...",
        "ѡ�����ɽ����û���ϵ�绰�޸���ز���...",
        "ѡ�����ɽ����û�����޸���ز���...",
        "ѡ�����ɽ����û�ԭʼע�������޸���ز���...",
        "ѡ�����ɽ����û�ע��ʱ�������ز���...",
        "ѡ�����ɽ����û��������ʱ�������ز���...",
        "ѡ�����ɽ����û���ǰ��¼��Դ�鿴��ز���...",
        "ѡ�����ɽ����û���¼ͳ���޸���ز���...",
        "ѡ�����ɽ����û���������ͳ���޸���ز���...",
        "ѡ�����ɽ����û��վ�����ʱ��鿴��ز���...",
        "ѡ�����ɽ����û������޸���ز���...",
        "ѡ�����ɽ����û�Ȩ���޸���ز���...",
        "<Enter> ѡ��, <Esc> �˳�, <~> �ָ�ԭ�趨..."
    };
    char buf[MU_LENGTH];
    snprintf(buf,MU_LENGTH,"  [����] %-.64s",help[n]);
    move((t_lines-1),0);
    clrtoeol();
    prints("\033[1;34;47m%s\033[K\033[m",buf);
    return 0;
}

static inline int mu_show_hint(const char *h){
    char buf[MU_LENGTH];
    snprintf(buf,MU_LENGTH,"  ԭʼ�趨ֵ [%-.64s]",h);
    move((t_lines-1),0);
    clrtoeol();
    prints("\033[0;32;47m%s\033[K\033[m",buf);
    return 0;
}

static inline int mu_get_time_desc(time_t *timestamp,char *buf,int from,int length){
    char *p;
    if(!(p=ctime(timestamp))){
        buf[0]=0;
        return -1;
    }
    snprintf(buf,length,"%s",&p[from]);
    if((p=strchr(buf,'\n')))
        *p=0;
    return 0;
}

static int mu_show_online_list(struct user_info *info,void *varg,int pos){
    struct mu_so_arg *arg=(struct mu_so_arg*)varg;
    char buf[MU_LENGTH];
#ifndef HAVE_IPV6_SMTH
    char desc[32];
#endif /* ! HAVE_IPV6_SMTH */
    if(!(info->active))
        return 0;
    if(!(arg->count<18))
        return 0;
    move((arg->count+4),0);
#ifndef HAVE_IPV6_SMTH
    mu_get_time_desc(&(info->logintime),desc,0,32);
    snprintf(buf,MU_LENGTH,"[%02d]  %-24.24s%-32.32s%s",(arg->count+1),info->from,desc,ModeType(info->mode));
#else /* HAVE_IPV6_SMTH */
    snprintf(buf,MU_LENGTH,"[%02d]  %-50.50s%s",(arg->count+1),info->from,ModeType(info->mode));
#endif /* ! HAVE_IPV6_SMTH */
    prints((info->invisible?"\033[1;33m%s\033[m":"\033[1;37m%s\033[m"),buf);
    snprintf(arg->from[arg->count++],IPLEN,"%s",info->from);
    return 0;
}

static int mu_show_online_generate_user(struct userec *user,void *varg){
    struct mu_so_key *key=(struct mu_so_key*)varg;
    if(!(user->userid[0]))
        return 0;
    if(!strcmp(key->value,user->lasthost)&&(key->count<100)){
        move((3+(key->count%20)),(2+((key->count/20)<<4)));
        prints("\033[1;37m%s\033[m",user->userid);
        key->count++;
    }
    return 0;
}

static int mu_show_online_generate_utmp(struct user_info *info,void *varg,int pos){
    struct mu_so_key *key=(struct mu_so_key*)varg;
    if(!(info->active))
        return 0;
    if(!strcmp(key->value,info->from)&&(key->count<100)){
        move((3+(key->count%20)),(2+((key->count/20)<<4)));
        prints((info->invisible?"\033[1;32m%s\033[m":"\033[1;37m%s\033[m"),info->userid);
        key->count++;
    }
    return 0;
}

static inline int mu_show_online(int uid,int mode){
    struct mu_so_arg arg;
    struct mu_so_key key;
    const struct userec *urec;
    char buf[MU_LENGTH];
    int use_utmp_method;
    move(2,0);
    clrtobot();
    if(!(urec=getuserbynum(uid))||!(urec->userid[0])){
        MU_PUT(2,MU_MSG(R,"�޷���ȡ�������û�����Ϣ..."));
        return -1;
    }
#ifndef HAVE_IPV6_SMTH
    snprintf(buf,MU_LENGTH,"%-6.6s%-24.24s%-32.32s%s","���","��Դ��ַ","��¼ʱ��","��ǰ״̬");
    prints("\033[0;32;47m%s\033[K\033[m",buf);
    snprintf(buf,MU_LENGTH,"%-6.6s%-24.24s%-32.32s%s","[00]",urec->lasthost,"[�޷�ȷ��]","[�������]");
    move(3,0);
    prints("\033[1;33;46m%s\033[K\033[m",buf);
#else /* HAVE_IPV6_SMTH */
    snprintf(buf,MU_LENGTH,"%-6.6s%-50.50s%s","���","��Դ��ַ","��¼ʱ��");
    prints("\033[0;32;47m%s\033[K\033[m",buf);
    snprintf(buf,MU_LENGTH,"%-6.6s%-50.50s%s","[00]",urec->lasthost,"[���������Դ]");
    move(3,0);
    prints("\033[1;33;46m%s\033[K\033[m",buf);
#endif /* ! HAVE_IPV6_SMTH */
    memset(&arg,0,sizeof(struct mu_so_arg));
    apply_utmp(mu_show_online_list,0,urec->userid,&arg);
    move((t_lines-1),0);
    if(!mode){
        prints("\033[0;32;47m%s\033[K\033[m","��ʾ���, �� <Enter> ���˳�...");
        WAIT_RETURN;
        return 0;
    }
    prints("\033[0;32;47m%s\033[K\033[m","��ʾ���, �� <Enter> ���˳�, �� <A>/<L> ��������Դ����...");
    use_utmp_method=0;
    do{
        switch(igetkey()){
            case '\n':
            case '\r':
                return 1;
            case 'A':
            case 'a':
                break;
            case 'L':
            case 'l':
                use_utmp_method=1;
                break;
            default:
                continue;
        }
        break;
    }
    while(1);
    move((t_lines-1),0);
    clrtoeol();
    getdata((t_lines-1),0,MU_MSG(Y,"��������Ҫ���з�������Ŀ��� [0]: "),buf,3,DOECHO,NULL,true);
    trimstr(buf);
    if(isdigit(buf[0])){
        buf[0]=atoi(buf);
        buf[0]=((buf[0]>arg.count)?0:buf[0]);
    }
    else
        buf[0]=0;
    key.value=(buf[0]?arg.from[buf[0]-1]:urec->lasthost);
    key.count=0;
    move(2,0);
    clrtobot();
    snprintf(buf,MU_LENGTH,"����Ϊ%s��¼��ԴΪ %s ���û�...",(use_utmp_method?"��ǰ����":"�������"),key.value);
    prints("\033[0;32;47m%s\033[K\033[m",buf);
    if(use_utmp_method)
        apply_ulist(mu_show_online_generate_utmp,&key);
    else
        apply_users(mu_show_online_generate_user,&key);
    move((t_lines-1),0);
    prints("\033[0;32;47m%s\033[K\033[m","��ʾ���, �� <Enter> ���˳�...");
    WAIT_RETURN;
    return 2;
}

static inline int mu_generate_level(int row,int col,unsigned int *level,unsigned int init){
#define MU_GL_N                         "\033[1;33m%c\033[m"
#define MU_GL_A                         "\033[1;32m%c\033[m"
#define MU_GL_D                         "\033[1;31m%c\033[m"
#define MU_GL_U                         "\033[1;34m%c\033[m"
#define MU_GL_B                         "\033[1;37m%c\033[m"
#define MU_GL_C(t)                      ("\033[4m" MU_GL_##t)
    int i,key,next;
    unsigned int perm,mask;
    *level=(perm=init);
    move(row,col);
    prints(MU_GL_B,'[');
    for(i=0,mask=0x01;i<NUMPERMS;i++,mask<<=1)
        prints(((perm&mask)?MU_GL_N:MU_GL_U),XPERMSTR[i]);
    prints(MU_GL_B,']');
    next=0;
    do{
        i=next;
        mask=(1<<i);
        move(row,(col+(i+1)));
        if((perm^init)&mask)
            prints(((perm&mask)?MU_GL_C(A):MU_GL_C(D)),XPERMSTR[i]);
        else
            prints(((perm&mask)?MU_GL_C(N):MU_GL_C(U)),XPERMSTR[i]);
        move(row,(col+(NUMPERMS+2)));
        clrtoeol();
        prints(" \033[1;36m%s\033[m",permstrings[i]);
        move(row,2);
        do{
            switch((key=igetkey())){
                case KEY_ESC:
                    return -1;
                case '\n':
                case '\r':
                    *level=perm;
                    return 0;
                case ' ':
                    perm^=mask;
                    break;
                case KEY_LEFT:
                    next=((i==0)?(NUMPERMS-1):(i-1));
                    break;
                case KEY_RIGHT:
                    next=((i==(NUMPERMS-1))?0:(i+1));
                    break;
                case KEY_HOME:
                    next=0;
                    if(next==i)
                        continue;
                    break;
                case KEY_END:
                    next=(NUMPERMS-1);
                    if(next==i)
                        continue;
                    break;
                default:
                    for(next=(NUMPERMS-1);!(next<0);next--)
                        if(key==XPERMSTR[next])
                            break;
                    if(next==-1&&isalpha(key)){
                        for(next=(NUMPERMS-1);!(next<0);next--)
                            if(toupper(key)==toupper(XPERMSTR[next]))
                                break;
                    }
                    if(next==-1){
                        next=i;
                        continue;
                    }
                    if(next==i)
                        continue;
                    break;
            }
            break;
        }
        while(1);
        if(next!=i){
            move(row,(col+(i+1)));
            if((perm^init)&mask)
                prints(((perm&mask)?MU_GL_A:MU_GL_D),XPERMSTR[i]);
            else
                prints(((perm&mask)?MU_GL_N:MU_GL_U),XPERMSTR[i]);
        }
    }
    while(1);
    return 1;
#undef MU_GL_N
#undef MU_GL_A
#undef MU_GL_D
#undef MU_GL_U
#undef MU_GL_B
#undef MU_GL_C
}

static inline int mu_process_directories(const char *ouserid,const char *nuserid){
    char src[MU_LENGTH],dst[MU_LENGTH];
    setmailpath(dst,nuserid);
    setmailpath(src,ouserid);
    my_f_rm(dst);
    if(f_mv(src,dst)==-1)
        return -1;
    sethomepath(dst,nuserid);
    sethomepath(src,ouserid);
    my_f_rm(dst);
    if(f_mv(src,dst)==-1){
        return -1;
    }
    snprintf(dst,MU_LENGTH,"tmp/email/%s",ouserid);
    f_rm(dst);
    return 0;
}

static int mu_menu_on_select(struct _select_def *conf){
    struct mu_arg *arg=(struct mu_arg*)conf->arg;
    const struct mu_item *item=&(arg->item[conf->pos-1]);
    if(item->access){
        arg->type=MU_MENU_SELECT;
        return SHOW_SELECT;
    }
    return SHOW_CONTINUE;
}

static int mu_menu_show(struct _select_def *conf,int pos){
    struct mu_arg *arg=(struct mu_arg*)conf->arg;
    const struct mu_item *item=&(arg->item[pos-1]);
    move(item->y,item->x);
    prints((item->access?"[\033[1;31m%c\033[m]%s: %s":"[\033[0;33m%c\033[m]%s: %s"),item->key,item->prefix,item->menu);
    return SHOW_CONTINUE;
}

static int mu_menu_pre_key(struct _select_def *conf,int *key){
    struct mu_arg *arg=(struct mu_arg*)conf->arg;
    int pos=conf->pos;
    unsigned int current=(1<<(pos-1));
    unsigned int access=((*(arg->access))&(~current));
    switch(*key){
        case KEY_UP:
            if(!(access&(current-1))){
                pos=MU_ITEM;
                current=(1<<(pos-1));
            }
            while(!(access&current)){
                pos--;
                current>>=1;
            }
            break;
        case KEY_DOWN:
            if(!(access&~(current-1))){
                pos=1;
                current=(1<<(pos-1));
            }
            while(!(access&current)){
                pos++;
                current<<=1;
            }
            break;
        default:
            return SHOW_CONTINUE;
    }
    conf->new_pos=pos;
    return SHOW_SELCHANGE;
}

static int mu_menu_key(struct _select_def *conf,int key){
    struct mu_arg *arg=(struct mu_arg*)conf->arg;
    const struct mu_item *item=arg->item;
    int value,index;
    if(key==KEY_ESC){
        arg->type=MU_MENU_QUIT;
        return SHOW_QUIT;
    }
    if(key==MU_KEY_RESET&&item[conf->pos-1].access){
        arg->type=MU_MENU_RESET;
        return SHOW_QUIT;
    }
    for(value=toupper(key),index=0;index<MU_ITEM;index++)
        if(item[index].access&&item[index].key==value){
            conf->new_pos=(index+1);
            return SHOW_SELCHANGE;
        }
    return SHOW_CONTINUE;
}

static int mu_menu_on_change(struct _select_def *conf,int new_pos){
    mu_show_help(new_pos-1);
    return SHOW_CONTINUE;
}

int modify_userinfo(int uid,int mode){
#define MU_ACCESS_USER                  0x000921FE
#define MU_ACCESS_ADMIN                 0x000FFFFF
#define MU_ACCESS_READ                  0x00092000
#define MU_ACCESS(d)                    (access&(1<<(d)))
#define MU_SIZE(t,f)                    (sizeof(((const struct t*)0)->f))
#define MU_SIZE_user(f)                 MU_SIZE(userec,f)
#define MU_SIZE_data(f)                 MU_SIZE(userdata,f)
#define MU_DIFF_str(x,y,l)              (strncmp((const void*)(x),(const void*)(y),(l)))
#define MU_DIFF_val(x,y,l)              ((x)!=(y))
#define MU_MENUFORM(d,c,f,s)                                                                    \
    do{                                                                                         \
        snprintf(menu[d],MU_LENGTH,MU_MSG(c,f),(s));                                            \
    }                                                                                           \
    while(0)
#define MU_CAST_0(d,i,m,f,s)                                                                    \
    do{                                                                                         \
        MU_MENUFORM(d,G,f,n##i.m);                                                              \
    }                                                                                           \
    while(0)
#define MU_CAST_buf(d,i,m,f,s)                                                                  \
    do{                                                                                         \
        MU_MENUFORM(d,G,f,s);                                                                   \
    }                                                                                           \
    while(0)
#define MU_SET(d,i,m,t,f,s)                                                                     \
    do{                                                                                         \
        if(MU_DIFF_##t(n##i.m,o##i.m,MU_SIZE_##i(m))){                                          \
            MU_CAST_##s(d,i,m,f,s);                                                             \
            change|=(1<<(d));                                                                   \
        }                                                                                       \
        else{                                                                                   \
            snprintf(menu[d],MU_LENGTH,"%s",omenu[d]);                                          \
            change&=(~(1<<(d)));                                                                \
        }                                                                                       \
    }                                                                                           \
    while(0)
#define MU_RESET(d,i,m)                                                                         \
    do{                                                                                         \
        if(change&(1<<(d))){                                                                    \
            memcpy(&(n##i.m),&(o##i.m),MU_SIZE_##i(m));                                         \
            snprintf(menu[d],MU_LENGTH,"%s",omenu[d]);                                          \
            change&=(~(1<<(d)));                                                                \
        }                                                                                       \
    }                                                                                           \
    while(0)
#define MU_SHOW_HINT(d)                                                                         \
    do{                                                                                         \
        mu_show_hint(omenu[d]);                                                                 \
    }                                                                                           \
    while(0)
#define MU_GET_TIME(e)                                                                          \
    do{                                                                                         \
        mu_get_time_desc(&(e),buf,0,MU_LENGTH);                                                 \
    }                                                                                           \
    while(0)
#define MU_VERIFY(d,i,m,t)                                                                      \
    do{                                                                                         \
        if(!(change&(1<<(d))))                                                                  \
            break;                                                                              \
        if(MU_DIFF_##t(v##i.m,o##i.m,MU_SIZE_##i(m)))                                           \
            verify|=(1<<(d));                                                                   \
    }                                                                                           \
    while(0)
#define MU_EXEC(d,i,m)                                                                          \
    do{                                                                                         \
        if(!(change&(1<<(d))))                                                                  \
            break;                                                                              \
        memcpy(&(v##i.m),&(n##i.m),MU_SIZE_##i(m));                                             \
    }                                                                                           \
    while(0)
    static const char *prefix[MU_ITEM]={
        "�û�����","�û�����","�û��ǳ�","��ʵ����","�û��Ա�",
        "��������","ͨ�ŵ�ַ","�����ʼ�","��ϵ�绰","�û����",
        "ע������","ע��ʱ��","�������","��¼��Դ","��¼ͳ��",
        "����ͳ��","����ͳ��","�û�����","�û�Ȩ��","��������"
    };
    static const char *title="\033[1;32m[�趨�û�����]\033[m";
#if !defined(HAVE_BIRTHDAY)||!defined(NEWSMTH)
    static const char *invalid="\033[0;33m<��ǰվ���趨��֧�ָ����>\033[m";
#endif /* !HAVE_BIRTHDAY||!NEWSMTH */
    static const char *md5_mask="<MD5 ���ܹ�������>";
    FILE *fp;
    struct mu_item item[MU_ITEM+1];
    struct _select_def conf;
    struct mu_arg arg;
    struct userec ouser,nuser,vuser,*urec;
    struct userdata odata,ndata,vdata;
    struct usermemo *memo;
    POINT loc[MU_ITEM];
    char omenu[MU_ITEM][MU_LENGTH],menu[MU_ITEM][MU_LENGTH],buf[MU_LENGTH],name[MU_LENGTH];
    unsigned char uc,uf;
    int i,j,k,loop,pos,mail;
    unsigned int access,change,verify,level;
    time_t current;
    switch(mode){
        case 0:
            access=MU_ACCESS_USER;
            break;
        case 1:
            access=MU_ACCESS_ADMIN;
            break;
        case 2:
            access=MU_ACCESS_READ;
            break;
        default:
            return -2;
    }
#ifndef HAVE_BIRTHDAY
    access&=(~0x00000030);
#endif /* ! HAVE_BIRTHDAY */
#ifndef NEWSMTH
    access&=(~0x00020000);
#endif /* ! NEWSMTH */
    modify_user_mode(!mode?GMENU:ADMIN);
    clear();
    move(0,0);
    prints("%s",title);
    if(!(urec=getuserbynum(uid))||!(urec->userid[0])){
        MU_PUT(2,MU_MSG(R,"��������û���Ϣ����ȷ..."));
        return -3;
    }
    if(!mode&&strcmp(urec->userid,getCurrentUser()->userid)){
        MU_PUT(2,MU_MSG(R,"��ǰ�ĵ������ʹ���..."));
        return -4;
    }
    memcpy(&ouser,urec,sizeof(struct userec));
    if(read_userdata(urec->userid,&odata)==-1){
        MU_PUT(2,MU_MSG(R,"��ȡ�û�����ʱ��������..."));
        return -5;
    }
    memcpy(&nuser,&ouser,sizeof(struct userec));
    memcpy(&ndata,&odata,sizeof(struct userdata));
    for(i=0;i<MU_ITEM;i++){
        loc[i].x=2;
        loc[i].y=(2+i);
        item[i].x=loc[i].x;
        item[i].y=loc[i].y;
        item[i].key=('A'+i);
        item[i].access=MU_ACCESS(i);
        item[i].prefix=prefix[i];
        item[i].menu=menu[i];
    }
    item[i].x=-1;
    item[i].y=-1;
    item[i].key=-1;
    item[i].access=0;
    item[i].prefix=NULL;
    item[i].menu=NULL;
    MU_MENUFORM(0,N,"%s",nuser.userid);
    MU_MENUFORM(1,N,"%s",md5_mask);
    MU_MENUFORM(2,N,"%s",nuser.username);
    MU_MENUFORM(3,N,"%s",ndata.realname);
#ifndef HAVE_BIRTHDAY
    MU_MENUFORM(4,N,"%s",invalid);
    MU_MENUFORM(5,N,"%s",invalid);
#else /* HAVE_BIRTHDAY */
    MU_MENUFORM(4,N,"%s",(ndata.gender=='M'?"��":"Ů"));
    snprintf(buf,MU_LENGTH,"%04d �� %02d �� %02d ��",(ndata.birthyear+1900),
        ndata.birthmonth,ndata.birthday);
    MU_MENUFORM(5,N,"%s",buf);
#endif /* ! HAVE_BIRTHDAY */
    MU_MENUFORM(6,N,"%-.64s",ndata.address);
    MU_MENUFORM(7,N,"%-.64s",ndata.email);
    MU_MENUFORM(8,N,"%-.64s",ndata.telephone);
    snprintf(buf,MU_LENGTH,"[%s] <%u>",(nuser.title?get_user_title(nuser.title):"��"),nuser.title);
    MU_MENUFORM(9,N,"%s",buf);
    MU_MENUFORM(10,N,"%-.64s",ndata.realemail);
    MU_GET_TIME(nuser.firstlogin);
    MU_MENUFORM(11,N,"%s",buf);
    MU_GET_TIME(nuser.lastlogin);
    MU_MENUFORM(12,N,"%s",buf);
    MU_MENUFORM(13,N,"%s",nuser.lasthost);
    MU_MENUFORM(14,N,"%u",nuser.numlogins);
    MU_MENUFORM(15,N,"%u",nuser.numposts);
    if(nuser.stay>3600)
        snprintf(buf,MU_LENGTH,"%ld Сʱ %2ld ����",(nuser.stay/3600),((nuser.stay%3600)/60));
    else
        snprintf(buf,MU_LENGTH,"%ld ����",(nuser.stay/60));
    MU_MENUFORM(16,N,"%s",buf);
#ifndef NEWSMTH
    MU_MENUFORM(17,N,"%s",invalid);
#else /* NEWSMTH */
    snprintf(buf,MU_LENGTH,((nuser.score_user>publicshm->us_sample[1])?"%d <RANKING %.2lf%%>":
        "%d <RANKING %.1lf%%>"),nuser.score_user,(100*us_ranking(nuser.score_user)));
    MU_MENUFORM(17,N,"%s",buf);
#endif /* ! NEWSMTH */
    MU_MENUFORM(18,N,"<%s>",gen_permstr(nuser.userlevel,buf));
    memcpy(omenu,menu,(MU_ITEM*MU_LENGTH*sizeof(char)));
    arg.type=MU_MENU_INIT;
    arg.access=&access;
    arg.item=item;
    memset(&conf,0,sizeof(struct _select_def));
    conf.item_count=MU_ITEM;
    conf.item_per_page=MU_ITEM;
    conf.prompt="��";
    conf.item_pos=loc;
    conf.arg=&arg;
    conf.title_pos.x=-1;
    conf.title_pos.y=-1;
    conf.on_select=mu_menu_on_select;
    conf.show_data=mu_menu_show;
    conf.pre_key_command=mu_menu_pre_key;
    conf.key_command=mu_menu_key;
    conf.on_selchange=mu_menu_on_change;
    loop=1;
    pos=MU_ITEM;
    mail=0;
    change=0;
    do{
        move(1,0);
        clrtobot();
        conf.pos=pos;
        conf.flag=LF_LOOP;
        if(change)
            MU_MENUFORM((MU_ITEM-1),R,"%s","<���޸�>");
        else
            MU_MENUFORM((MU_ITEM-1),N,"%s","<δ�޸�>");
        mu_show_help(conf.pos-1);
        list_select_loop(&conf);
        pos=conf.pos;
        if(arg.type==MU_MENU_QUIT){
            if(change){
                MU_GET((MU_ITEM+2),MU_MSG(Y,"�����޸Ĳ��˳�? [N]: "),buf,1);
                if(toupper(buf[0])!='Y')
                    continue;
            }
            return -1;
        }
        else if(arg.type==MU_MENU_SELECT){
            switch((i=(pos-1))){
#define MU_CURR_ROW                     (i+2)
#define MU_BREAK_TRIM(s)                if(!((s)[0]))break;trimstr(s)
#define MU_TRIM_BREAK(s)                trimstr(s);if(!((s)[0]))break
                case 0:
                    MU_SHOW_HINT(i);
                    if(!strcmp(nuser.userid,getCurrentUser()->userid)){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"�޷��޸ĵ�ǰ��¼���û���..."));
                        break;
                    }
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ��û���: "),buf,IDLEN);
                    if(!buf[0]||!strcmp(buf,nuser.userid))
                        break;
                    if(id_invalid(buf)){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"������û������Ϸ�..."));
                        break;
                    }
                    if(strcasecmp(buf,ouser.userid)&&searchuser(buf)){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"������û����Ѿ�����..."));
                        break;
                    }
                case 1:
                    if(!mode){
                        MU_GETPWD(MU_CURR_ROW,MU_MSG(Y,"������ԭ����: "),&buf[40],38);
                        if(!buf[40])
                            break;
                        if(!checkpasswd2(&buf[40],&ouser)){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"��������벻��ȷ..."));
                            break;
                        }
                    }
                    j=0;
                    while(!j){
                        MU_GETPWD(MU_CURR_ROW,MU_MSG(Y,"�������µ�����: "),&buf[40],38);
                        if(!buf[40]){
                            j++;
                            continue;
                        }
                        MU_GETPWD(MU_CURR_ROW,MU_MSG(Y,"���ظ������µ�����: "),&buf[80],38);
                        if(!buf[80]){
                            j++;
                            continue;
                        }
                        if(strcmp(&buf[40],&buf[80])){
                            MU_PUT(MU_CURR_ROW,MU_MSG(R,"������������벻ƥ��, ����������..."));
                            continue;
                        }
                        if(!mode&&(strlen(&buf[40])<6)){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�µ�������ڼ��, ���������� 6 λ..."));
                            continue;
                        }
                        break;
                    }
                    if(j)
                        break;
                    if(!i){
                        snprintf(nuser.userid,(IDLEN+2),"%s",buf);
                        snprintf(ndata.userid,(IDLEN+2),"%s",buf);
                        snprintf(buf,80,"%s \033[0;33m[�����߳����û�ȫ����¼]\033[m",nuser.userid);
                        MU_SET(0,user,userid,str,"%s",buf);
                    }
                    setpasswd(&buf[80],&nuser);
                    snprintf(buf,MU_LENGTH,"%s",md5_mask);
                    MU_SET(1,user,md5passwd,str,"%s",buf);
                    break;
                case 2:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ��ǳ�: "),buf,(NAMELEN-1));
                    if(!buf[0])
                        break;
                    snprintf(nuser.username,NAMELEN,"%s",buf);
                    MU_SET(i,user,username,str,"%s",0);
                    break;
                case 3:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ���ʵ����: "),buf,(NAMELEN-1));
                    MU_BREAK_TRIM(buf);
                    snprintf(ndata.realname,NAMELEN,"%s",buf);
                    MU_SET(i,data,realname,str,"%s",0);
                    break;
#ifdef HAVE_BIRTHDAY
                case 4:
                    ndata.gender=(ndata.gender=='M'?'F':'M');
                    snprintf(buf,MU_LENGTH,"%s",(ndata.gender=='M'?"��":"Ů"));
                    MU_SET(i,data,gender,val,"%s",buf);
                    break;
                case 5:
                    /* �������¶�! �����пշǰ������� birth ��һ�𲻿�... */
#define MU_PARSE2(p)                    ((((p)[0]*10)+((p)[1]*1))-('0'*11))
#define MU_PARSE4(p)                    ((MU_PARSE2(p)*100)+(MU_PARSE2(&(p)[2])*1))
#define MU_MON_CORR(m)                  (((abs(abs(((m)<<1)-15)-5))>>1)&0x01)
#define MU_FEB_CORR(y)                  (2-(!(y%400)?1:(!(y%100)?0:(!(y%4)?1:0))))
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µĳ�������(YYYYMMDD): "),buf,8);
                    MU_TRIM_BREAK(buf);
                    if(!mu_digit_string(buf)){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"����ĳ������ڲ�����Ԥ����ʽ..."));
                        break;
                    }
                    j=MU_PARSE4(buf);
                    buf[0]=MU_PARSE2(&buf[4]);
                    buf[1]=MU_PARSE2(&buf[6]);
                    if((j<1910||j>2010)||(buf[0]<1||buf[0]>12)||(buf[1]<1
                        ||buf[1]>(31-(MU_MON_CORR(buf[0])+(buf[0]==2?MU_FEB_CORR(j):0))))){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"����ĳ������ڲ��Ϸ�..."));
                        break;
                    }
                    ndata.birthyear=(j-1900);
                    ndata.birthmonth=buf[0];
                    ndata.birthday=buf[1];
                    snprintf(buf,MU_LENGTH,"%04d �� %02d �� %02d ��",(ndata.birthyear+1900),
                        ndata.birthmonth,ndata.birthday);
                    MU_SET(i,data,birthyear,val,"%s",buf);
                    if(change&(1<<5))
                        break;
                    MU_SET(i,data,birthmonth,val,"%s",buf);
                    if(change&(1<<5))
                        break;
                    MU_SET(i,data,birthday,val,"%s",buf);
#undef MU_PARSE2
#undef MU_PARSE4
#undef MU_MON_CORR
#undef MU_FEB_CORR
                    break;
#endif /* HAVE_BIRTHDAY */
                case 6:
                    MU_SHOW_HINT(i);
                    MU_PUT(MU_CURR_ROW,MU_MSG(Y,"�������µ�ͨ�ŵ�ַ..."));
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,": "),buf,64);
                    MU_BREAK_TRIM(buf);
                    snprintf(ndata.address,STRLEN,"%s",buf);
                    MU_SET(i,data,address,str,"%s",0);
                    break;
                case 7:
                    MU_SHOW_HINT(i);
                    MU_PUT(MU_CURR_ROW,MU_MSG(Y,"�������µĵ����ʼ���ַ..."));
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,": "),buf,64);
                    MU_BREAK_TRIM(buf);
                    snprintf(ndata.email,STRLEN,"%s",buf);
                    MU_SET(i,data,email,str,"%s",0);
                    break;
                case 8:
                    MU_SHOW_HINT(i);
                    MU_PUT(MU_CURR_ROW,MU_MSG(Y,"�������µ���ϵ�绰..."));
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,": "),buf,64);
                    MU_BREAK_TRIM(buf);
                    snprintf(ndata.telephone,STRLEN,"%s",buf);
                    MU_SET(i,data,telephone,str,"%s",0);
                    break;
                case 9:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ�ְ��{<����>|<#���>|<@>}: "),buf,(USER_TITLE_LEN-1));
                    MU_TRIM_BREAK(buf);
                    if(buf[0]=='@'&&!buf[1]){
                        j=select_user_title(NULL);
                        if(j==-1)
                            break;
                    }
                    else if(buf[0]=='#'){
                        if(!mu_digit_string(&buf[1])||(j=atoi(&buf[1]))>255){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����ְ����Ų��Ϸ�..."));
                            break;
                        }
                        if(j&&!*get_user_title(j)){
                            MU_GET(MU_CURR_ROW,MU_MSG(Y,"��ǰ��Ŷ�Ӧ��ְ�񲻴���, �Ƿ����? [N]: "),buf,1);
                            if(toupper(buf[0])!='Y')
                                break;
                        }
                    }
                    else{
                        for(uc=0,uf=0,j=1;j<256;j++)
                            if(!strcmp(get_user_title(j),buf)&&!uc++)
                                uf=j;
                        if(!uc){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"��ǰ�����ְ��������δ����..."));
                            break;
                        }
                        j=(uc==1?uf:select_user_title(buf));
                        if(j==-1)
                            break;
                    }
                    nuser.title=j;
                    snprintf(buf,MU_LENGTH,"[%s] <%u>",(nuser.title?get_user_title(nuser.title):"��"),nuser.title);
                    MU_SET(i,user,title,val,"%s",buf);
                    break;
                case 10:
                    MU_SHOW_HINT(i);
                    MU_PUT(MU_CURR_ROW,MU_MSG(Y,"�������µ�ԭʼע������..."));
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,": "),buf,60);
                    MU_BREAK_TRIM(buf);
                    snprintf(ndata.realemail,(STRLEN-16),"%s",buf);
                    MU_SET(i,data,realemail,str,"%s",0);
                    break;
                case 11:
                    MU_SHOW_HINT(i);
                    current=time(NULL);
                    if(!(nuser.firstlogin)){
                        MU_GET(MU_CURR_ROW,MU_MSG(Y,"���û�ע��ʱ�����, �Ƿ���������? [Y]: "),buf,1);
                        if(toupper(buf[0])=='N')
                            break;
                    }
                    else if((nuser.userlevel&PERM_LOGINOK)||(current-nuser.firstlogin)>REGISTER_WAIT_TIME){
                        MU_PUT(MU_CURR_ROW,MU_MSG(C,"���û�����������ע��ʱ��ı�Ҫ..."));
                        break;
                    }
                    else{
                        MU_GET(MU_CURR_ROW,MU_MSG(Y,"�Ƿ��������ø��û���ע��ʱ��(�Ա�����дע�ᵥ)? [N]: "),buf,1);
                        if(toupper(buf[0])!='Y')
                            break;
                    }
                    nuser.firstlogin=(current-REGISTER_WAIT_TIME);
                    MU_GET_TIME(nuser.firstlogin);
                    MU_SET(i,user,firstlogin,val,"%s",buf);
                    break;
                case 12:
                    MU_SHOW_HINT(i);
                    current=time(NULL);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�Ƿ��������ø��û����������ʱ��? [N]: "),buf,1);
                    if(toupper(buf[0])!='Y')
                        break;
                    nuser.lastlogin=current;
                    MU_GET_TIME(nuser.lastlogin);
                    MU_SET(i,user,lastlogin,val,"%s",buf);
                    break;
                case 13:
                    mu_show_online(uid,mode);
                    break;
                case 14:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µĵ�¼����{<N>|<+N>|<-N>}: "),buf,9);
                    MU_TRIM_BREAK(buf);
                    if(buf[0]=='+'||buf[0]=='-'){
                        if(!mu_digit_string(&buf[1])){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        j=atoi(buf);
                        if((nuser.numlogins+j)<0)
                            nuser.numlogins=0;
                        nuser.numlogins+=j;
                    }
                    else{
                        if(!mu_digit_string(buf)){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        nuser.numlogins=atoi(buf);
                    }
                    MU_SET(i,user,numlogins,val,"%u",0);
                    break;
                case 15:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ���������{<N>|<+N>|<-N>}: "),buf,9);
                    MU_TRIM_BREAK(buf);
                    if(buf[0]=='+'||buf[0]=='-'){
                        if(!mu_digit_string(&buf[1])){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        j=atoi(buf);
                        if((nuser.numposts+j)<0)
                            nuser.numposts=0;
                        nuser.numposts+=j;
                    }
                    else{
                        if(!mu_digit_string(buf)){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        nuser.numposts=atoi(buf);
                    }
                    MU_SET(i,user,numposts,val,"%u",0);
                    break;
                case 16:
                    current=time(NULL);
                    j=(int)((((double)nuser.stay)/((double)(current-nuser.firstlogin)))*1440);
                    if(j<60)
                        snprintf(buf,MU_LENGTH,"\033[1;33m�վ�����ʱ��Ϊ \033[1;31m%d\033[1;33m ����...\033[m",j);
                    else
                        snprintf(buf,MU_LENGTH,"\033[1;33m�վ�����ʱ��Ϊ \033[1;31m%d\033[1;33m Сʱ "
                            "\033[1;31m%2d\033[1;33m ����...\033[m",(j/60),(j%60));
                    MU_PUT(MU_CURR_ROW,buf);
                    break;
#ifdef NEWSMTH
                case 17:
                    MU_SHOW_HINT(i);
                    MU_GET(MU_CURR_ROW,MU_MSG(Y,"�������µ��û���������{<N>|<+N>|<-N>}: "),buf,9);
                    MU_TRIM_BREAK(buf);
                    if(buf[0]=='+'||buf[0]=='-'){
                        if(!mu_digit_string(&buf[1])){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        j=atoi(buf);
                        if((nuser.score_user+j)<0)
                            nuser.score_user=0;
                        nuser.score_user+=j;
                    }
                    else{
                        if(!mu_digit_string(buf)){
                            MU_PUT(MU_CURR_ROW,MU_MSG(C,"�����������ʽ���Ϸ�..."));
                            break;
                        }
                        nuser.score_user=atoi(buf);
                    }
                    snprintf(buf,MU_LENGTH,((nuser.score_user>publicshm->us_sample[1])?"%d <RANKING %.2lf%%>":
                        "%d <RANKING %.1lf%%>"),nuser.score_user,(100*us_ranking(nuser.score_user)));
                    MU_SET(i,user,score_user,val,"%s",buf);
                    break;
#endif /* NEWSMTH */
                case 18:
                    MU_SHOW_HINT(i);
                    if(mu_generate_level(MU_CURR_ROW,15,&level,nuser.userlevel)==-1)
                        break;
                    if(level!=ouser.userlevel&&level!=nuser.userlevel){
                        MU_GET(MU_CURR_ROW,MU_MSG(Y,"�Ƿ�����û�����Ȩ���޸�֪ͨ? [Y]: "),buf,1);
                        mail=(toupper(buf[0])!='N');
                    }
                    nuser.userlevel=level;
                    buf[0]='<';
                    gen_permstr(nuser.userlevel,&buf[1]);
                    snprintf(&buf[NUMPERMS+1],(MU_LENGTH-(NUMPERMS+1)),"> \033[0;33m[%s֪ͨ]\033[m",(mail?"����":"����"));
                    MU_SET(i,user,userlevel,val,"%s",buf);
                    break;
#undef MU_CURR_ROW
#undef MU_BREAK_TRIM
#undef MU_TRIM_BREAK
                case (MU_ITEM-1):
                    if(!change)
                        return 0;
                    MU_GET((MU_ITEM+2),MU_MSG(Y,"ִ���޸Ĳ��˳�? [N]: "),buf,1);
                    loop=(!(toupper(buf[0])=='Y'));
                    continue;
                default:
                    break;
            }
        }
        else if(arg.type==MU_MENU_RESET){
            switch((i=(pos-1))){
                case 0:
                case 1:
                    MU_RESET(i,user,userid);
                    MU_RESET(i,user,md5passwd);
                    break;
                case 2:
                    MU_RESET(i,user,username);
                    break;
                case 3:
                    MU_RESET(i,data,realname);
                    break;
#ifdef HAVE_BIRTHDAY
                case 4:
                    MU_RESET(i,data,gender);
                case 5:
                    MU_RESET(i,data,birthyear);
                    MU_RESET(i,data,birthmonth);
                    MU_RESET(i,data,birthday);
                    break;
#endif /* HAVE_BIRTHDAY */
                case 6:
                    MU_RESET(i,data,address);
                    break;
                case 7:
                    MU_RESET(i,data,email);
                    break;
                case 8:
                    MU_RESET(i,data,telephone);
                    break;
                case 9:
                    MU_RESET(i,user,title);
                    break;
                case 10:
                    MU_RESET(i,data,realemail);
                    break;
                case 11:
                    MU_RESET(i,user,firstlogin);
                    break;
                case 12:
                    MU_RESET(i,user,lastlogin);
                    break;
                case 13:
                    MU_RESET(i,user,lasthost);
                    break;
                case 14:
                    MU_RESET(i,user,numlogins);
                    break;
                case 15:
                    MU_RESET(i,user,numposts);
                    break;
                case 16:
                    MU_RESET(i,user,stay);
                    break;
#ifdef NEWSMTH
                case 17:
                    MU_RESET(i,user,score_user);
                    break;
#endif /* NEWSMTH */
                case 18:
                    MU_RESET(i,user,userlevel);
                    break;
                case (MU_ITEM-1):
                    if(change){
                        memcpy(&nuser,&ouser,sizeof(struct userec));
                        memcpy(&ndata,&odata,sizeof(struct userdata));
                        memcpy(menu,omenu,(MU_ITEM*MU_LENGTH*sizeof(char)));
                        change=0;
                    }
                    break;
                default:
                    break;
            }
        }
        else
            continue;
    }
    while(loop);
    memcpy(&vuser,urec,sizeof(struct userec));
    if(read_userdata(urec->userid,&vdata)==-1){
        MU_PUT((MU_ITEM+2),MU_MSG(R,"У���û�����ʱ������������..."));
        return -6;
    }
    verify=0;
    MU_VERIFY(0,user,userid,str);
    MU_VERIFY(1,user,md5passwd,str);
    MU_VERIFY(2,user,username,str);
    MU_VERIFY(3,data,realname,str);
#ifdef HAVE_BIRTHDAY
    MU_VERIFY(4,data,gender,val);
    MU_VERIFY(5,data,birthyear,val);
    MU_VERIFY(5,data,birthmonth,val);
    MU_VERIFY(5,data,birthday,val);
#endif /* HAVE_BIRTHDAY */
    MU_VERIFY(6,data,address,str);
    MU_VERIFY(7,data,email,str);
    MU_VERIFY(8,data,telephone,str);
    MU_VERIFY(9,user,title,val);
    MU_VERIFY(10,data,realemail,str);
    MU_VERIFY(11,user,firstlogin,val);
    MU_VERIFY(12,user,lastlogin,val);
    MU_VERIFY(13,user,lasthost,str);
    MU_VERIFY(14,user,numlogins,val);
    MU_VERIFY(15,user,numposts,val);
    MU_VERIFY(16,user,stay,val);
#ifdef NEWSMTH
    MU_VERIFY(17,user,score_user,val);
#endif /* NEWSMTH */
    MU_VERIFY(18,user,userlevel,val);
    if(verify){
        MU_GET((MU_ITEM+2),MU_MSG(Y,"�����û������Ѿ������仯, �Ƿ�ǿ���޸�? [N]: "),buf,1);
        if(toupper(buf[0])!='Y'){
            MU_PUT((MU_ITEM+2),MU_MSG(Y,"ȡ���޸Ĳ���..."));
            return -7;
        }
        if(verify&0x01)
            change&=(~0x03);
    }
    memcpy(&vuser,urec,sizeof(struct userec));
    if(read_userdata(urec->userid,&vdata)==-1){
        MU_PUT((MU_ITEM+2),MU_MSG(R,"�޸��û�����ʱ������������..."));
        return -8;
    }
    MU_EXEC(3,data,realname);
#ifdef HAVE_BIRTHDAY
    MU_EXEC(4,data,gender);
    MU_EXEC(5,data,birthyear);
    MU_EXEC(5,data,birthmonth);
    MU_EXEC(5,data,birthday);
#endif /* HAVE_BIRTHDAY */
    MU_EXEC(6,data,address);
    MU_EXEC(7,data,email);
    MU_EXEC(8,data,telephone);
    MU_EXEC(9,user,title);
    MU_EXEC(10,data,realemail);
    MU_EXEC(11,user,firstlogin);
    MU_EXEC(12,user,lastlogin);
    MU_EXEC(13,user,lasthost);
    MU_EXEC(14,user,numlogins);
    MU_EXEC(15,user,numposts);
    MU_EXEC(16,user,stay);
#ifdef NEWSMTH
    MU_EXEC(17,user,score_user);
#endif /* NEWSMTH */
    MU_EXEC(18,user,userlevel);
    if(change&0x04){
        if(mode)
            update_username(urec->userid,NULL,nuser.username);
        else{
            update_username(urec->userid,vuser.username,nuser.username);
            strncpy(uinfo.username,nuser.username,NAMELEN);
        }
        MU_EXEC(2,user,username);
    }
    i=(verify&0x01);
    do{
        if(change&0x01){
            if(strcasecmp(vuser.userid,nuser.userid)&&searchuser(nuser.userid)){
                i=2;
                break;
            }
            for(j=0;(k=kick_user_utmp(uid,NULL,0))==10;j+=k)
                continue;
            if(j+=k)
                newbbslog(BBSLOG_USER,"modify_userinfo_kick: %s <%d> [ %d logins ]",urec->userid,uid,j);
            setuserid(uid,nuser.userid);
            if(searchuser(nuser.userid)!=uid){
                i=3;
                break;
            }
            if(mu_process_directories(vuser.userid,nuser.userid)==-1){
                i=4;
                break;
            }
            MU_EXEC(0,user,userid);
            MU_EXEC(0,data,userid);
        }
    }
    while(0);
    if(!i)
        MU_EXEC(1,user,md5passwd);
    else
        change&=(~0x03);
    memcpy(urec,&vuser,sizeof(struct userec));
    if(write_userdata(urec->userid,&vdata)==-1){
        MU_PUT((MU_ITEM+2),MU_MSG(R,"��д�û�����ʱ������������..."));
        return -9;
    }
    j=read_user_memo(urec->userid,&memo);
    if(!(j<0)&&(j!=sizeof(struct usermemo)))
        end_mmapfile(memo,sizeof(struct usermemo),-1);
    if(j==sizeof(struct usermemo)){
        memcpy(&(memo->ud),&vdata,sizeof(struct userdata));
        end_mmapfile(memo,sizeof(struct usermemo),-1);
    }
    else{
        sethomefile(buf,urec->userid,"usermemo");
        unlink(buf);
    }
    if(mail&&(change&(1<<18))){
        snprintf(name,MU_LENGTH,"tmp/modify_userinfo_%lu_%d.mail",time(NULL),(int)getpid());
        if((fp=fopen(name,"w"))){
            write_header(fp,getCurrentUser(),1,NULL,"[ϵͳ] �û�Ȩ���޸�֪ͨ",0,0,getSession());
            fprintf(fp,"\033[1;37m[%s]\033[m\n\n","�û�Ȩ���޸���ϸ");
            fprintf(fp,"  \033[1;33mԭ�û�Ȩ��״̬: \033[0;33m<%s>\033[m\n",gen_permstr(ouser.userlevel,buf));
            fprintf(fp,"  \033[1;33m���û�Ȩ��״̬: \033[1;32m<%s>\033[m\n\n",gen_permstr(vuser.userlevel,buf));
            fprintf(fp,"\033[1;37m[%s]\033[m\n\n","�漰�޸ĵ�Ȩ��λ˵��");
            for(level=(ouser.userlevel^vuser.userlevel),j=0;j<NUMPERMS;j++)
                if(level&(1<<j))
                    fprintf(fp,"  %s<%c> \033[1;33m%s\033[m\n",((vuser.userlevel&(1<<j))?"\033[1;32m+":"\033[1;31m-"),
                        XPERMSTR[j],permstrings[j]);
            fprintf(fp,"\n\033[1;37m[%s]\033[m\n","�����쳣����뼰ʱ�ظ�������ϵվ����Ա�跨���!");
            fclose(fp);
            mail_file(getCurrentUser()->userid,name,urec->userid,"[ϵͳ] �û�Ȩ���޸�֪ͨ",BBSPOST_MOVE,NULL);
            unlink(name);
        }
    }
    if(mode){
        snprintf(name,MU_LENGTH,"tmp/modify_userinfo_%lu_%d.log",time(NULL),(int)getpid());
        if(!(fp=fopen(name,"w"))){
            snprintf(buf,MU_LENGTH,"�趨�û�����: <%d,%#6.6x> %s%c-> %s",uid,change,ouser.userid,((change&0x01)?' ':0),nuser.userid);
            securityreport(buf,NULL,NULL,getSession());
        }
        else{
            sprintf(buf,"�趨�û�����: %s%c-> %s",ouser.userid,((change&0x01)?' ':0),nuser.userid);
            write_header(fp,getCurrentUser(),0,"syssecurity",buf,0,0,getSession());
            fprintf(fp,"\033[1;37m[�û� <uid=\033[1;31m%d\033[1;37m> �����޸���ϸ]\033[m\n\n",uid);
            for(j=0;j<(MU_ITEM-1);j++){
                if(change&(1<<j))
                    fprintf(fp," \033[1;33m[%-8.8s]: \033[0;33m%s\033[m\n%-13.13s\033[1;32m%s\033[m\n\n",prefix[j],omenu[j],"",menu[j]);
            }
            fclose(fp);
            post_file(getCurrentUser(),"",name,"syssecurity",buf,0,-1,getSession());
            unlink(name);
        }
        newbbslog(BBSLOG_USER,"modify_userinfo: %s <%d,%#6.6x>",urec->userid,uid,change);
    }
    switch(i){
        case 1:
            MU_PUT((MU_ITEM+2),MU_MSG(Y,"�������! ��ȡ�����ƺ�������޸�(�û����ݱ仯)..."));
            return 2;
        case 2:
            MU_PUT((MU_ITEM+2),MU_MSG(Y,"�������! ��ȡ�����ƺ�������޸�(�Ѵ���ͬ���û�)..."));
            return 3;
        case 3:
            MU_PUT((MU_ITEM+2),MU_MSG(R,"�������! ��ȡ�����ƺ�������޸�(ϵͳ����)..."));
            return 4;
        case 4:
            MU_PUT((MU_ITEM+2),MU_MSG(R,"�������! ��ȡ�����ƺ�������޸�(�ļ�����)..."));
            return 5;
        case 0:
            MU_PUT((MU_ITEM+2),MU_MSG(G,"�������! ���в������ѳɹ�ִ��!"));
            break;
        default:
            MU_PUT((MU_ITEM+2),MU_MSG(Y,"�������!"));
            return 6;
    }
    return 1;
#undef MU_ACCESS_USER
#undef MU_ACCESS_ADMIN
#undef MU_ACCESS_READ
#undef MU_ACCESS
#undef MU_SIZE
#undef MU_SIZE_user
#undef MU_SIZE_data
#undef MU_DIFF_str
#undef MU_DIFF_val
#undef MU_MENUFORM
#undef MU_CAST_0
#undef MU_CAST_buf
#undef MU_SET
#undef MU_RESET
#undef MU_SHOW_HINT
#undef MU_GET_TIME
#undef MU_VERIFY
#undef MU_EXEC
}

int modify_userinfo_current(void){
    return modify_userinfo(uinfo.uid,0);
}

int modify_userinfo_admin(void){
#ifdef HAVE_STRICT_USERINFO
#define MUA_PERM    (PERM_ADMIN)
#else /* HAVE_STRICT_USERINFO */
#define MUA_PERM    (PERM_SYSOP|PERM_ADMIN)
#endif /* HAVE_STRICT_USERINFO */
    char name[STRLEN];
    int uid,mode;
    clear();
#ifndef SOURCE_PERM_CHECK
    mode=1;
#else /* SOURCE_PERM_CHECK */
    if(!HAS_PERM(getCurrentUser(),MUA_PERM)){
        MU_PUT(2,MU_MSG(R,"��ǰ�û������в����û����ϵ�Ȩ��..."));
        return -0xFF;
    }
    mode=(HAS_PERM(getCurrentUser(),PERM_ADMIN)?1:2);
#endif /* ! SOURCE_PERM_CHECK */
    if(!check_systempasswd())
        return -0xFE;
    clear();
    move(0,0);
    prints("\033[1;32m[%s�û�����]\033[m",((mode==1)?"�޸�":"����"));
    move(1,0);
    usercomplete("�������û���: ",name);
    move(1,0);
    clrtobot();
    if(!name[0]){
        MU_PUT(2,MU_MSG(Y,"ȡ��..."));
        return -0xFD;
    }
    if(!(uid=searchuser(name))){
        MU_PUT(2,MU_MSG(R,"������û���..."));
        return -0xFC;
    }
    return modify_userinfo(uid,mode);
#undef MUA_PERM
}

#undef MU_LENGTH
#undef MU_ITEM
#undef MU_KEY_RESET
#undef MU_GEN_N
#undef MU_GEN_R
#undef MU_GEN_Y
#undef MU_GEN_G
#undef MU_GEN_C
#undef MU_MSG
#undef MU_PUT
#undef MU_GET_CORE
#undef MU_GET
#undef MU_GETPWD

/* END -- etnlegend, 2006.09.21, �޸��û����Ͻӿ�... */

