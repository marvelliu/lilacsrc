#include "bbs.h"
#include "lilacfuncs.h"

#include "md5.h"

int lilac_auto_register(char *userid, char *email, int msize,  session_t *session)
{
    struct userdata ud;
    struct userec *uc;
    char genbuf[STRLEN];
    char buf[STRLEN];
    char fname[STRLEN];
    int unum;
    FILE *fout;
    struct userec deliveruser;

    bzero(&deliveruser, sizeof(struct userec));
    strcpy(deliveruser.userid, "deliver");
    deliveruser.userlevel = -1;
    strcpy(deliveruser.username, "�Զ�����ϵͳ");


    if ((unum = getuser(userid, &uc)) == 0)
        return -1;              //faild
    if (read_userdata(userid, &ud) < 0)
        return -1;

    sprintf(buf, "tmp/email/%s", userid);
    if ((fout = fopen(buf, "w")) != NULL) {
        fprintf(fout, "%s\n", email);
        fclose(fout);
    }

    if (write_userdata(userid, &ud) < 0)
        return -1;
    mail_file("deliver", "etc/s_fill", userid, "��ϲ��,���Ѿ����ע��.", 0, 0);
    //sprintf(genbuf,"deliver �� %s �Զ�ͨ�����ȷ��.",uinfo.userid);

    sprintf(fname, "tmp/security.%d", getpid());
    if ((fout = fopen(fname, "w")) != NULL) {
        fprintf(fout, "ϵͳ��ȫ��¼ϵͳ\n\033[32mԭ��%s�Զ�ͨ��ע��\033[m\n", userid);
        fprintf(fout, "������ͨ���߸�������");
        fprintf(fout, "\n\n���Ĵ���     : %s\n", ud.userid);
        fprintf(fout, "�����ǳ�     : %s\n", uc->username);
        fprintf(fout, "��ʵ����     : %s\n", ud.realname);
        fprintf(fout, "�����ʼ����� : %s\n", ud.email);
        fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
        fprintf(fout, "����λ     : %s\n", "");
        fprintf(fout, "Ŀǰסַ     : %s\n", ud.address);
        fprintf(fout, "����绰     : %s\n", "");
        fprintf(fout, "ע������     : %s", ctime(&uc->firstlogin));
        fprintf(fout, "����������� : %s", ctime(&uc->lastlogin));
        fprintf(fout, "������ٻ��� : %s\n", uc->lasthost);
        fprintf(fout, "��վ����     : %d ��\n", uc->numlogins);
        fprintf(fout, "������Ŀ     : %d(Board)\n", uc->numposts);
        fprintf(fout, "��    ��     : %s\n", "");

        fclose(fout);

        sprintf(genbuf, "%s �Զ�ͨ��ע��", ud.userid);
        post_file(&deliveruser, "", fname, "Registry", genbuf, 0, 1, session);
    }

    sethomefile(buf, userid, "auto_register");
    sethomefile(genbuf, userid, "register");
	f_mv(buf, genbuf);

/*
    sethomefile(buf, userid, ".need_active");
    sethomefile(genbuf, userid, ".activeinfo");
	f_mv(buf, genbuf);
*/
	return 0;
}

int lilac_ocpost(char *userid, char *email, int msize,  session_t *session)
{
    struct userdata ud;
    struct userec *uc;
    char genbuf[STRLEN];
    char buf[STRLEN];
    char fname[STRLEN];
    int unum;
    FILE *fout;
    struct userec deliveruser;

    bzero(&deliveruser, sizeof(struct userec));
    strcpy(deliveruser.userid, "deliver");
    deliveruser.userlevel = -1;
    strcpy(deliveruser.username, "�Զ�����ϵͳ");


    if ((unum = getuser(userid, &uc)) == 0)
        return -1;              //faild
    if (read_userdata(userid, &ud) < 0)
        return -1;

	uc->userlevel |= PERM_NOIPCTRL;

    sprintf(buf, "tmp/email/%s", userid);
    if ((fout = fopen(buf, "w")) != NULL) {
        fprintf(fout, "%s\n", email);
        fclose(fout);
    }

    if (write_userdata(userid, &ud) < 0)
        return -1;
    mail_file("deliver", "etc/s_ocactive", userid, "��ϲ��,���Ѿ����У�ⷢ��Ȩ��.", 0, 0);
    //sprintf(genbuf,"deliver �� %s �Զ�ͨ�����ȷ��.",uinfo.userid);

    sprintf(fname, "tmp/security.%d", getpid());
    if ((fout = fopen(fname, "w")) != NULL) {
        fprintf(fout, "ϵͳ��ȫ��¼ϵͳ\n\033[32mԭ��%s�Զ����У�ⷢ��Ȩ��\033[m\n", userid);
        fprintf(fout, "������ͨ���߸�������");
        fprintf(fout, "\n\n���Ĵ���     : %s\n", ud.userid);
        fprintf(fout, "�����ǳ�     : %s\n", uc->username);
        fprintf(fout, "��ʵ����     : %s\n", ud.realname);
        fprintf(fout, "�����ʼ����� : %s\n", ud.email);
        fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
        fprintf(fout, "����λ     : %s\n", "");
        fprintf(fout, "Ŀǰסַ     : %s\n", ud.address);
        fprintf(fout, "����绰     : %s\n", "");
        fprintf(fout, "ע������     : %s", ctime(&uc->firstlogin));
        fprintf(fout, "����������� : %s", ctime(&uc->lastlogin));
        fprintf(fout, "������ٻ��� : %s\n", uc->lasthost);
        fprintf(fout, "��վ����     : %d ��\n", uc->numlogins);
        fprintf(fout, "������Ŀ     : %d(Board)\n", uc->numposts);
        fprintf(fout, "��    ��     : %s\n", "");

        fclose(fout);

        sprintf(genbuf, "%s �Զ���÷���Ȩ��", ud.userid);
        post_file(&deliveruser, "", fname, "OutCampusPost", genbuf, 0, 1, session);
    }

/*
    sethomefile(buf, userid, ".need_active");
    sethomefile(genbuf, userid, ".activeinfo");
	f_mv(buf, genbuf);
*/
	return 0;
}
void trimlfrt(char *str)
{
	int len, i;
	len = strlen(str);

	for (i = 0; i < len; i++)
		if (str[i] == '\r' || str[i] == '\n')
		{
			str[i] = 0;
			return;
		}
}
//By Marvel 2007-06-08 ������������
int lilac_validmail(char *email)
{
	if(!email)
		return 0;
	if(strstr(email,"@")!=NULL)
		return 1;
	else
		return 0;
}


int lilac_validid(char *id)
{
	char *p = id, last;
	int i;
	int W[17]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
    char A[11]={'1','0','x','9','8','7','6','5','4','3','2'};
    long sum;
 
	if(strlen(id)==15)
		last = '1';
	else if(strlen(id)==18)
		last = *(id+17);
	else
		return 0;
	//���ǰ17λ
    for(i=1; i<=17 && *p!=NULL; i++,p++){
		if(!isdigit((int)*p))
			return 0;
	}   

//ref : http://blog.csdn.net/foxmail/archive/2004/06/25/26529.aspx
//    if(strlen(id)!=18)
//        return 0;
    sum=0;
    for(i=0;i<17;i++)
    {   
        sum = sum + W[i] * (id[i] - 0x30);
    }   
    
    sum = sum%11;
    
    if(A[sum]==id[17])
    {   
        return 1;
    }   
    else
    {   
        return 0;
    }
}

/*
int lilac_validmail(char *email)
{
	char regmail[16][STRLEN];
	char buff[STRLEN];
	int  i, cmail = 0;
	FILE *fp;

	if ( (fp = fopen("etc/REG_EMAIL", "r")) == NULL)
	{
		return -1;
	}
	memset(regmail, 0, sizeof(regmail));
	while (!feof(fp))
	{
		fgets(buff, STRLEN - 1, fp);
		trimlfrt(buff);
		if (strlen(buff) > 1)
			strcpy(regmail[cmail], buff);
		cmail++;
	}

	for (i = 0; i < cmail; i++)
		if (strlen(email) > strlen(regmail[i]))
		{
		if (regmail[i][0] != '!')
		{
			if (!strncasecmp(email + (strlen(email) - strlen(regmail[i])), regmail[i], strlen(regmail[i])))
				return 1;
		}
		else
		{
			if (!strncasecmp(email + (strlen(email) - strlen(regmail[i]) + 1), regmail[i] + 1, strlen(regmail[i]) - 1))
				return 1;
		}
		}

	return 0;
}
*/

void genactivecode(unsigned char activecode[])
{
    static const char passmagic[] = "hit deem MD5 account active code magic, //grin~~, 2005/6/22";
	char   md5s[16], buff[32];
	char   *symb = "0123456789ABCDEFGHIJKLMN";
	int i, j;
	long int myrand;
    MD5_CTX md5;

    MD5Init(&md5);
    MD5Update(&md5, (unsigned char *) passmagic, strlen(passmagic));
	snprintf(buff, 32, "%d", getpid());
    MD5Update(&md5, (unsigned char *) buff, strlen(buff));
    MD5Update(&md5, (unsigned char *) passmagic, strlen(passmagic));
	srand(time(NULL)*getpid());
	myrand = random();
    MD5Update(&md5, (unsigned char *) &myrand, sizeof(myrand));
    MD5Final((unsigned char *)md5s, &md5);

	j = 0;
	for (i = 0; i < 16; i++)
	{
		activecode[j]=( symb[md5s[i]&0x0f]);
		j++;
		activecode[j]=( symb[(md5s[i]&0xf0)>>4] );
		j++;
	}
}

int send_verifymail_id(char *from, char *to, char *msgfile, 
				char *subject, char *replace, char *replacewith,
				char *name, char *addr, char *tele, char *userid)
{
	FILE *fout, *fin;
	char genbuf[256];

	sprintf( genbuf, "/usr/lib/sendmail -f %s %s ", from, to );
	fout = popen( genbuf, "w" );
	fin  = fopen( msgfile, "r" );
	if ((fin != NULL) && (fout != NULL))
    {
		fprintf( fout, "Reply-To:%s\n", from);
		fprintf( fout, "From: %s\n",  from ); 
		fprintf( fout, "To: %s\n", to);
		fprintf( fout, "Subject: %s\n", subject);
		fprintf( fout, "X-Forwarded-By: SYSOP \n" );
		fprintf( fout, "X-Disclaimer: None\n");
		fprintf( fout, "\n");
		fprintf( fout, "\n");
		

		while (fgets( genbuf, 255, fin ) != NULL ) 
        {
			if (genbuf[0] == '.' && genbuf[ 1 ] == '\n')
			{
				fputs( ". \n", fout );
				continue;
			}
			if (!strncasecmp(genbuf, replace, strlen(replace)))
			{
				fprintf(fout, "�����ʺ���Ϣ: \n");
				fprintf(fout, "�û��ɣ�:  %s\n", userid);
				fprintf(fout, "�û�����:  %s\n", name);
				fprintf(fout, "ͨ�ŵ�ַ:  %s\n", addr);
				fprintf(fout, "��ϵ�绰:  %s\n", tele);
			
                fprintf(fout, "�����ַ:(����˵�ַ����ʺŵļ���)\n");
				fputs(replacewith, fout);
                fprintf(fout, "\n(�������˵�ַû�з�Ӧ���뽫������ַ\n���Ƶ�IE��ַ�����ʱ�վ��ɼ���!)\n");
				fputs("\n", fout);
				continue;
			}
			fputs( genbuf, fout );
		}
		fprintf(fout, ".\n");                                    
		fclose( fin );
		fclose( fout );                                     
		return 1;
	}
	return -1;
}

int send_verifymail(char *from, char *to, char *msgfile, 
				char *subject, char *replace, char *replacewith,
				char *name, char *addr, char *tele, session_t *session)
{

	return	send_verifymail_id(from, to, msgfile, 
				subject, replace, replacewith,
				name, addr, tele, session->currentuser->userid);

}

/*
 *  mod = 0 ע�ἤ��
 */
int create_activecode(int mod, char *name, char *addr, char *telephone, char *email, session_t *session)
{
	char ac[36], buff[256], genbuf[256];
	FILE *fp;


	memset(ac, 0, 36);
	genactivecode((unsigned char *)ac);
	//���漤����Ϣ��
	sethomefile(genbuf, session->currentuser->userid, ".needactive");
	if ( (fp=fopen(genbuf, "w")) == NULL)
		return 0;

	fprintf(fp, "TIME:%d\n", (int)time(NULL));
	fprintf(fp, "ACCD:%s\n", ac);
	fprintf(fp, "NAME:%s\n", name);
	fprintf(fp, "ADDR:%s\n", addr);
	fprintf(fp, "TELE:%s\n", telephone);
	fprintf(fp, "MAIL:%s\n", email);
	fclose(fp);

	sprintf(buff, "http://%s/active.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"), session->currentuser->userid, ac);

	return send_verifymail("no-reply@lilacbbs.net", email, 
					"etc/f_fill.newactive", 
				    "�϶�������BBS ��ע���û������ż�", "[ACTIVE CODE HERE]", 
					buff, name, addr, telephone, session);

}

int test_and_remove_file(char *userid, char *fn)
{
	char buff1[STRLEN], buff2[STRLEN];
	int fd;

	sethomefile(buff1, userid, ".active.lock");
	if ((fd = open(buff1, O_RDWR | O_CREAT, 0600)) == -1) {
       	return -1;
   	}
   	flock(fd, LOCK_EX);  //����һ�£�web�����ڸ��ļ�������ʱ��Ҳ��Ҫ��Ӧ����������

    sethomefile(buff2, userid, fn);
	if (dashf(buff2))  //������fn�Ļ�ֱ�ӷ��أ����õ��Ļ����ܲ����
	{
        unlink(buff2);
	}
	else
	{
    	flock(fd, LOCK_UN);  //����
    	close(fd);
        unlink(buff1);
		return 0;
	}

    flock(fd, LOCK_UN);  //ɾ����ɣ�����
    close(fd);
    unlink(buff1);

	return 1;
}

int lilac_test_and_rename_file(char *userid, char *fromfn, char *tofn)
{
	char buff1[STRLEN], buff2[STRLEN], buff3[STRLEN];
	int fd;

	sethomefile(buff1, userid, ".active.lock");
	if ((fd = open(buff1, O_RDWR | O_CREAT, 0600)) == -1) {
       	return -1;
   	}
   	flock(fd, LOCK_EX);  //����һ�£���дע�ᵥ�����ڸ��ļ�������ʱ��Ҳ��Ҫ��Ӧ����������

    sethomefile(buff2, userid, fromfn);
	if (dashf(buff2)) //��Ҫ�������� 
	{
		
        sethomefile(buff3, userid, tofn);
        rename(buff2, buff3);

	}
	else //������fromfn�Ļ�ֱ�ӷ��أ����õ��Ļ����ܲ����
	{
		//��Ȼ������Ŀ���ļ�������������Ҫȥ��
   		flock(fd, LOCK_UN);  //ɾ����ɣ�����
   		close(fd);
    	unlink(buff1);  //��������ļ���������������
		return 0;
	}
   	flock(fd, LOCK_UN);  //ɾ����ɣ�����
   	close(fd);
    unlink(buff1);  //��������ļ���������������

	return 1;
}



//By Marvel,����Ķ���Ϊ�˼������û�



int echo_verify_message_id( char *msgfile, 
				char *subject, char *replace, char *replacewith,
				char *name, char *addr, char *tele, char *userid, char *filename)
{
	FILE *fin, *fout;
	char genbuf[256];

	fin  = fopen( msgfile, "r" );
	fout  = fopen( filename, "w" );
	if ((fin != NULL) && (fout != NULL))
    {
		
		fprintf( fout," \t\t%s\n", subject);
		
		while (fgets( genbuf, 255, fin ) != NULL ) 
        {
			if (genbuf[0] == '.' && genbuf[ 1 ] == '\n')
			{
				fprintf( fout, ". \n");
				continue;
			}
			if (!strncasecmp(genbuf, replace, strlen(replace)))
			{
				fprintf( fout,"�����ʺ���Ϣ: \n");
				fprintf( fout,"�û��ɣ�:  %s\n", userid);
				fprintf( fout,"�û�����:  %s\n", name);
				fprintf( fout,"ͨ�ŵ�ַ:  %s\n", addr);
				fprintf( fout,"��ϵ�绰:  %s\n", tele);
				fprintf( fout,"�����ַ:(����˵�ַ����ʺŵļ���)\n");
				fprintf( fout,"%s",replacewith);
				fprintf( fout,"\n(�������˵�ַû�з�Ӧ���뽫������ַ\n���Ƶ�IE��ַ�����ʱ�վ��ɼ���!)\n");
				continue;
			}
			fprintf(fout,"%s", genbuf);
		}                              
		fclose( fin );                
		fclose( fout );                                   
		return 1;
	}else
		return -1;
}


int generate_active_url(char *name, char *addr, char *telephone, char *email, char *filename, session_t *session)
{
	char ac[36], buff[256], genbuf[256];
	FILE *fp;


	memset(ac, 0, 36);
	genactivecode((unsigned char *)ac);
	
	//���漤����Ϣ��
	sethomefile(genbuf, session->currentuser->userid, ".needactive");
	if ( (fp=fopen(genbuf, "w")) == NULL)
		return 0;

	fprintf(fp, "TIME:%d\n", (int)time(NULL));
	fprintf(fp, "ACCD:%s\n", ac);
	fprintf(fp, "NAME:%s\n", name);
	fprintf(fp, "ADDR:%s\n", addr);
	fprintf(fp, "TELE:%s\n", telephone);
	fprintf(fp, "MAIL:%s\n", email);
	fclose(fp);

	sprintf(buff, "http://%s/active.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"), session->currentuser->userid, ac);

	sprintf(filename,"/tmp/%s.very",session->currentuser->userid);
	int ret = echo_verify_message_id("etc/f_fill.newactive", 
				    "�϶�������BBS ע���û������ż�", "[ACTIVE CODE HERE]", 
					buff, name, addr, telephone, session->currentuser->userid,filename);
	return ret;

}

int get_active_url(char *name, char *addr, char *telephone, char *email, char* url, session_t *session)
{
	char ac[36], genbuf[256];
	FILE *fp;


	memset(ac, 0, 36);
	genactivecode((unsigned char *)ac);
	
	//���漤����Ϣ��
	sethomefile(genbuf, session->currentuser->userid, ".needactive");
	if ( (fp=fopen(genbuf, "w")) == NULL)
		return 0;

	fprintf(fp, "TIME:%d\n", (int)time(NULL));
	fprintf(fp, "ACCD:%s\n", ac);
	fprintf(fp, "NAME:%s\n", name);
	fprintf(fp, "ADDR:%s\n", addr);
	fprintf(fp, "TELE:%s\n", telephone);
	fprintf(fp, "MAIL:%s\n", email);
	fclose(fp);

	sprintf(url, "http://%s/active.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"),  session->currentuser->userid, ac);

	return 1;

}

int password_email_by_info(char *name, char *userid, char *email)
{
	char ac[36], buff[256], genbuf[256];
	FILE *fp;


	memset(ac, 0, 36);
	genactivecode((unsigned char *)ac);
	//���漤����Ϣ��
	sethomefile(genbuf, userid, ".pass_email");
	if ( (fp=fopen(genbuf, "w")) == NULL)
		return 0;

	fprintf(fp, "TIME:%d\n", (int)time(NULL));
	fprintf(fp, "ACCD:%s\n", ac);
	fprintf(fp, "NAME:%s\n", name);
	fprintf(fp, "MAIL:%s\n", email);
	fclose(fp);

	sprintf(buff, "http://%s/verifygetpass.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"), userid, ac);

	return send_verify_email_mail("no-reply@"MAIL_BBSDOMAIN, email, 
					"etc/f_fill.verifypass", 
				    "�϶�������BBS �û���������ȷ���ż�", "[ACTIVE CODE HERE]", 
					buff, name, userid);

}

int verify_email_by_info(char *name, char *userid, char *email)
{
	char ac[36], buff[256], genbuf[256];
	FILE *fp;


	memset(ac, 0, 36);
	genactivecode((unsigned char *)ac);
	//���漤����Ϣ��
	sethomefile(genbuf, userid, ".verify_email");
	if ( (fp=fopen(genbuf, "w")) == NULL)
		return 0;

	fprintf(fp, "TIME:%d\n", (int)time(NULL));
	fprintf(fp, "ACCD:%s\n", ac);
	fprintf(fp, "NAME:%s\n", name);
	fprintf(fp, "MAIL:%s\n", email);
	fclose(fp);

	sprintf(buff, "http://%s/verifyemail.php?uid=%s&ac=%s", sysconf_str("BBSDOMAIN"), userid, ac);

	return send_verify_email_mail("no-reply@"MAIL_BBSDOMAIN, email, 
					"etc/f_fill.verifyemail", 
				    "�϶�������BBS �û�����ȷ���ż�", "[ACTIVE CODE HERE]", 
					buff, name, userid);

}




//�������������ע�����û�ʱ��ȷ��email�ģ�damn ICP
int verify_email(session_t *session)
	//*name, char *email)
{
	struct userec *currentuser = session->currentuser;
	char *name = currentuser->username;
	char *userid = currentuser->userid;
	char *email = session->currentmemo->ud.email;

	return verify_email_by_info(name, userid, email);

}


int send_verify_email_mail(char *from, char *to, char *msgfile, 
				char *subject, char *replace, char *replacewith,
				char *name, char *userid)
{

	FILE *fout, *fin;
	char genbuf[256];

	sprintf( genbuf, "/usr/lib/sendmail -f %s %s ", from, to );
	fout = popen( genbuf, "w" );
	fin  = fopen( msgfile, "r" );
	if ((fin != NULL) && (fout != NULL))
    {
		fprintf( fout, "Reply-To:%s\n", from);
		fprintf( fout, "From: %s\n",  from ); 
		fprintf( fout, "To: %s\n", to);
		fprintf( fout, "Subject: %s\n", subject);
		fprintf( fout, "X-Forwarded-By: SYSOP \n" );
		fprintf( fout, "X-Disclaimer: None\n");
		fprintf( fout, "\n");
		fprintf( fout, "\n");
		

		while (fgets( genbuf, 255, fin ) != NULL ) 
        {
			if (genbuf[0] == '.' && genbuf[ 1 ] == '\n')
			{
				fputs( ". \n", fout );
				continue;
			}
			if (!strncasecmp(genbuf, replace, strlen(replace)))
			{
				fprintf(fout, "���ʺ���Ϣ: \n");
				fprintf(fout, "�û��ɣ�:  %s\n", userid);
				fprintf(fout, "�û�����:  %s\n", name);
			
                fprintf(fout, "ȷ�ϵ�ַ:(����˵�ַ����ʺŵ�����ȷ��)\n");
				fputs(replacewith, fout);
                fprintf(fout, "\n(�������˵�ַû�з�Ӧ���뽫������ַ\n���Ƶ�IE��ַ�����ʱ�վ��ɼ���!)\n");
				fputs("\n", fout);
				continue;
			}
			fputs( genbuf, fout );
		}
		fprintf(fout, ".\n");                                    
		fclose( fin );
		fclose( fout );                                     
		return 1;
	}
	return -1;
}

int verify_email_file_exist_by_userid(char *userid)
{
	char genbuf[256];
	sethomefile(genbuf, userid, ".verify_email");
	FILE *fp = fopen(genbuf,"r");
	if(fp == NULL)
		return 0;
	fclose(fp);
	return 1;
}
int verify_email_file_exist(session_t *session)
{
	return verify_email_file_exist_by_userid(session->currentuser->userid);
}


int verified_email_file_exist_by_userid(char *userid)
{
	char genbuf[256];
	sethomefile(genbuf, userid, ".verified_email");
	FILE *fp = fopen(genbuf,"r");
	if(fp == NULL)
		return 0;
	fclose(fp);
	return 1;
}

int verified_email_file_exist(session_t *session)
{
	return verified_email_file_exist_by_userid(session->currentuser->userid);
}


#ifndef HOROFUN
#define HOROFUN
char   * horoscope(month, day)
unsigned char    month, day;
{
	char   *name[12] = {
		"Ħ��", "ˮƿ", "˫��", "ĵ��", "��ţ", "˫��",
		"��з", "ʨ��", "��Ů", "���", "��Ы", "����"
	};
	switch (month) {
	case 1:
		if (day < 21)
			return (name[0]);
		else
			return (name[1]);
	case 2:
		if (day < 19)
			return (name[1]);
		else
			return (name[2]);
	case 3:
		if (day < 21)
			return (name[2]);
		else
			return (name[3]);
	case 4:
		if (day < 21)
			return (name[3]);
		else
			return (name[4]);
	case 5:
		if (day < 21)
			return (name[4]);
		else
			return (name[5]);
	case 6:
		if (day < 22)
			return (name[5]);
		else
			return (name[6]);
	case 7:
		if (day < 23)
			return (name[6]);
		else
			return (name[7]);
	case 8:
		if (day < 23)
			return (name[7]);
		else
			return (name[8]);
	case 9:
		if (day < 23)
			return (name[8]);
		else
			return (name[9]);
	case 10:
		if (day < 24)
			return (name[9]);
		else
			return (name[10]);
	case 11:
		if (day < 23)
			return (name[10]);
		else
			return (name[11]);
	case 12:
		if (day < 22)
			return (name[11]);
		else
			return (name[0]);
	}
	return ("����");
}
#endif

void merge(struct favbrd_struct *brdlist1, int brdlist_t1, 
		struct favbrd_struct *brdlist2, int brdlist_t2,
		struct favbrd_struct *brdlist, int * brdlist_t, char* name, int type)
{
	int i,j;
	if(type==1){
		memcpy(brdlist, brdlist2, sizeof(struct favbrd_struct) * brdlist_t2);
		*brdlist_t = brdlist_t2;
	}else if(type==2){
		memcpy(brdlist, brdlist1, sizeof(struct favbrd_struct) * brdlist_t1);
		*brdlist_t = brdlist_t1;
	}else if(type==0){
		//merge
		memcpy(brdlist, brdlist1, sizeof(struct favbrd_struct) * brdlist_t1);
		memcpy(brdlist+brdlist_t1, brdlist2, sizeof(struct favbrd_struct) * brdlist_t2);
		brdlist[0].bid[brdlist[0].bnum]= 0-brdlist_t1;
		(brdlist+brdlist_t1)->father = 0;
		brdlist[0].bnum++;
		if(name!=NULL)
			sprintf((brdlist+brdlist_t1)->title, "%s���ղؼ�",name);
		for(i=0;i<brdlist_t2;i++){
			for(j=0;j<brdlist[brdlist_t1+i].bnum;j++){
				int bid = brdlist[brdlist_t1+i].bid[j];
				if(bid>=0)
					continue;
				brdlist[brdlist_t1+i].bid[j] -= brdlist_t1;
				brdlist[brdlist_t1-bid].father = brdlist_t1+i;
			}
		}
		*brdlist_t= brdlist_t1+brdlist_t2;
	}
	return;
}

int get_recommend_info(int num, char *info)
{
    char filename[STRLEN * 2];
	fileheader_t fh;
	int fd;
    struct stat st;

    sprintf(filename, BBSHOME "/boards/%s/" DOT_DIR, RECMD_INFO_BOARD);
    if ((fd = open(filename, O_RDONLY)) < 0)
		return 0;
   	fstat(fd, &st);
   	int total = st.st_size / sizeof(fh);
	if(total >= RECMD_INFO_NUM)
   		lseek(fd, (total-num-1) * sizeof(fh), SEEK_SET);
	if(read(fd,(void *)&fh,sizeof(fileheader_t))<=0){
		close(fd);
		return 0;
	}
	if(strncmp(fh.title, "Re: ", 4)==0){
		close(fd);
		return 0;
	}

	strncpy(info, fh.title, ARTICLE_TITLE_LEN); 
	close(fd);
	return 1;
}

int get_digest_info(int num, char *boardname, fileheader_t *files)
{
    char filename[STRLEN * 2];
	fileheader_t fh;
	int fd;
    struct stat st;

	setbfile(filename, boardname, DIGEST_DIR);
    if ((fd = open(filename, O_RDONLY)) < 0)
		return 0;
   	fstat(fd, &st);
   	int total = st.st_size / sizeof(fh);
	if(total >= num)
   		lseek(fd, (total-num) * sizeof(fh), SEEK_SET);
	else
		num = total;

	int r;
	if((r=read(fd,files,sizeof(fileheader_t)*num))<=0){
		close(fd);
		return 0;
	}
	close(fd);
	return num;
}
