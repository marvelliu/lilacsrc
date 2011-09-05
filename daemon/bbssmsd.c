#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "bbs.h"

#if HAVE_MYSQL_SMTH == 1
#include "mysql.h"
#endif

#ifdef SMS_SUPPORT
#undef perror
#undef printf

void * buf;
int sockfd;
int sn=0;
struct header h;
int running;
#if HAVE_MYSQL_SMTH == 1
MYSQL mysql_s;
extern int save_smsmsg_nomysqlconnect(MYSQL *s, char *uident, struct msghead *head, char *msgbuf, int readed);
#endif

#define THRESHHOLE 60

time_t lastsendtime;
time_t lastrcvtime;

void do_exit_sig(int sig)
{
    running=0;
}

void save_daemon_pid()
{
    FILE *fp;

    if ((fp = fopen("var/bbssmsd.pid", "w")) == NULL) {
        perror("fopen");
        exit(-1);
    }
    fprintf(fp, "%d", getpid());
    fclose(fp);
}

void init_memory()
{
    void * p;
    int iscreate;

    iscreate = 0;
    p = attach_shm("SMS_SHMKEY", 8914, SMS_SHM_SIZE+sizeof(struct sms_shm_head), &iscreate);
    getSession()->head = (struct sms_shm_head *) p;
    buf = p+sizeof(struct sms_shm_head);
    if (iscreate == 0)
        bbslog("4system", "loaded an existed smsshm");
    getSession()->head->sem = 0;
    getSession()->head->total = 0;
    getSession()->head->length = 0;
}

void start_daemon()
{
    setgid(BBSGID);
    setuid(BBSUID);
    chdir(BBSHOME);
/*    n = getdtablesize();
    if (fork())
        exit(0);
    if (setsid() == -1) {
        perror("setsid");
        exit(-1);
    }
    signal(SIGHUP, SIG_IGN);*/
    if (fork())
        exit(0);
    save_daemon_pid();
}

void loginas(char* user, char* pass)
{
    struct header h;
    struct LoginPacket p;
    strcpy(p.user, user);
    strcpy(p.pass, pass);
    h.Type = CMD_LOGIN;
    long2byte(sn++, h.SerialNo);
    long2byte(sizeof(p), h.BodyLength);
    long2byte(0, h.pid);
    newbbslog(BBSLOG_SMS,"send CMD_LOGIN");
    write(sockfd, &h, sizeof(h));
    write(sockfd, &p, sizeof(p));
}

int sendtouser(struct GWSendSMS * h, char* buf)
{
    int uid;
    char * uident;
    struct user_info * uin;
    struct msghead hh;
    uid = smsnumber2uid(h->UserID);
    uident = getuserid2(uid);
    if(uident == NULL)
        return -1;
    uin = t_search(uident, NULL);

    hh.frompid = -1;
    hh.mode = 6;
    hh.sent = 0;
    hh.time = time(0);
    strncpy(hh.id, h->SrcMobileNo, IDLEN+2);
    hh.id[IDLEN+1] = 0;

    newbbslog(BBSLOG_SMS,"receive %s from %s for %s",buf,h->SrcMobileNo, uident);
    if(uin == NULL){
		hh.topid = -1;
#if HAVE_MYSQL_SMTH == 1
		save_smsmsg_nomysqlconnect(&mysql_s, uident, &hh, buf, 0);
#endif
		{
			struct header hr;
			struct BBSSendSMS h4;
			char *buf1;
			int number;
			struct user_info uintest;
			struct userdata ud;

			if( read_userdata(uident, &ud) )
				return -1;

			if( ! ud.mobileregistered )
				return -1;

			if( ! (ud.smsdef & SMSDEF_AUTOTOMOBILE ) )
				return -1;

			buf1 = (char *)malloc( strlen(buf) + strlen(h->SrcMobileNo) +3 );
			if( buf1 == NULL )
				return -1;

			bzero(&hr,sizeof(hr));
			bzero(&h4,sizeof(h4));

			sprintf(buf1, "%s: %s", h->SrcMobileNo, buf);
			hr.Type = CMD_BBSSEND;
			long2byte(sizeof(h4)+strlen(buf1)+1, hr.BodyLength);
			long2byte(strlen(buf1)+1, h4.MsgTxtLen);
			strncpy(h4.SrcMobileNo, ud.mobilenumber, MOBILE_NUMBER_LEN);
			h4.SrcMobileNo[MOBILE_NUMBER_LEN-1]=0;
			strncpy(h4.DstMobileNo, ud.mobilenumber,MOBILE_NUMBER_LEN);
			h4.DstMobileNo[MOBILE_NUMBER_LEN-1]=0;
			uintest.uid = uid;
			uid2smsid( &uintest, h4.SrccUserID);
			number = uid2smsnumber( &uintest );
			long2byte(number, h4.UserID);

                write(sockfd, &hr, sizeof(hr));
                write(sockfd, &h4, sizeof(h4));
                write(sockfd, buf1, byte2long(h4.MsgTxtLen));

			free( buf1 );
		}
        return -1;
	}

    hh.topid = uin->pid;
    save_msgtext(uident, &hh, buf, getSession());
#if HAVE_MYSQL_SMTH == 1
	save_smsmsg_nomysqlconnect(&mysql_s, uident, &hh, buf, 1);
#endif
    kill(uin->pid, SIGUSR2);
    return 0;
}

int requiretouser(struct RequireBindPacket * h, unsigned int sn)
{
    char * uident;
    char buf[200];
    struct usermemo *pum;
    struct userdata ud;
    int uid;
    /* �����ȷ���1��ͷ��uid*/
    uid=smsid2uid(h->cUserID);
    uident = getuserid2(uid);
    if (uident==NULL) return 1;

    if (read_user_memo(uident, &pum)>0) {
        memcpy(&ud,&pum->ud,sizeof(ud));
        if (strncmp(ud.mobilenumber, h->MobileNo, MOBILE_NUMBER_LEN-1)) {
            newbbslog(BBSLOG_SMS,"Bind error:mobilephone %s for %s",ud.mobilenumber,uident);
            return 2;
        }
        if(h->Bind==0) {
            pum->ud.mobileregistered=true;
            ud.mobileregistered=true;
            sprintf(buf, "����ʺ��Ѿ���%s�󶨣�",ud.mobilenumber);
            newbbslog(BBSLOG_SMS,"Bind mobilephone %s for %s",ud.mobilenumber,uident);
        }
        else {
            pum->ud.mobileregistered=false;
            ud.mobileregistered=false;
            sprintf(buf, "����ʺ��Ѿ�ȡ����%s�İ󶨣�",ud.mobilenumber);
            newbbslog(BBSLOG_SMS,"UnBind mobilephone %s for %s",ud.mobilenumber,uident);
        }
	end_mmapfile(getSession()->currentmemo, sizeof(struct usermemo), -1);
       write_userdata(uident, &ud);
       mail_file("deliver", "", uident, buf, BBSPOST_COPY, NULL);
       return 0;
    }
    return 3;
}

void processremote()
{
    unsigned int pid;
    struct header reth;
    char fn[80];
    char buf[1024*10];
    FILE* fp;
    struct RequireBindPacket h1;
    struct GWSendSMS h2;
    long2byte(byte2long(h.SerialNo), reth.SerialNo);
    long2byte(0, reth.BodyLength);
    pid=byte2long(h.pid);
    sprintf(fn, "tmp/%d.res", pid);
    switch(h.Type) {
        case CMD_OK:
            newbbslog(BBSLOG_SMS,"get CMD_OK");
            if(pid) {
                fp=fopen(fn, "w");
                fprintf(fp, "1\n0");
                fclose(fp);
                kill(pid, SIGUSR1);
            }
            break;
       case CMD_ERR:
	case CMD_ERR_HEAD_LENGTH:
	case CMD_ERR_DB:
	case CMD_ERR_SMS_VALIDATE_FAILED:
	case CMD_ERR_LENGTH:
	case CMD_ERR_NO_VALIDCODE:
	case CMD_ERR_NO_SUCHMOBILE:
	case CMD_ERR_REGISTERED:
	case CMD_EXCEEDMONEY_LIMIT:
	    newbbslog(BBSLOG_SMS,"get CMD_ERR");
            if(pid) {
                fp=fopen(fn, "w");
                fprintf(fp, "0\n%d", h.Type);
                fclose(fp);
                kill(pid, SIGUSR1);
            }
            break;
        case CMD_REQUIRE: {
               struct header* pheader;
               struct ReplyBindPacket rp;
               pheader=(struct header*)buf;
               read(sockfd, &h1, sizeof(h1));
               rp.isSucceed = requiretouser(&h1, byte2long(h.SerialNo));
               //Copy a reth struct in reply packet
               memcpy(&reth,&h,sizeof(h));
               reth.Type = CMD_REPLY;
               long2byte(sizeof(rp), reth.BodyLength);
               
               write(sockfd, &reth, sizeof(reth));
			   write(sockfd, &rp,sizeof(rp));
            }
            break;
        case CMD_GWSEND:
            read(sockfd, &h2, sizeof(h2));
            read(sockfd, buf, byte2long(h2.MsgTxtLen));
            buf[byte2long(h2.MsgTxtLen)] = 0;
            if(sendtouser(&h2, buf)) {
                reth.Type = CMD_ERR;
            }
            else {
                reth.Type = CMD_OK;
            }
            write(sockfd, &reth, sizeof(reth));
            break;
	case CMD_LINK:
	    break;
    }
}

int keepLink(){
struct header head;
head.Type=CMD_LINK;
long2byte(0,head.BodyLength);
write(sockfd,&head,sizeof(head));
return 0;
}

void getbuf(void * h, int s)
{
    if(getSession()->head->length<s) return;
    if(h)
        memcpy(h, buf, s);
    memcpy(buf, buf+s, getSession()->head->length-s);
    getSession()->head->length-=s;
}

void processbbs()
{
    struct header h;
    struct RegMobileNoPacket h1;
    struct CheckMobileNoPacket h2;
    struct UnRegPacket h3;
    struct BBSSendSMS h4;
    struct ReplyBindPacket h5;
    if(getSession()->head->sem) return;
    if(!getSession()->head->total) return;
    getSession()->head->sem=1;
    while(getSession()->head->total) {
        getSession()->head->total--;
        getbuf(&h, sizeof(h));
        if(h.Type!=CMD_REPLY)
            long2byte(sn++, h.SerialNo);
        switch(h.Type) {
            case CMD_REG:
                newbbslog(BBSLOG_SMS,"send CMD_REG");
                getbuf(&h1, sizeof(h1));
                write(sockfd, &h, sizeof(h));
                write(sockfd, &h1, sizeof(h1));
                break;
            case CMD_CHECK:
                newbbslog(BBSLOG_SMS,"send CMD_CHECK");
                getbuf(&h2, sizeof(h2));
                write(sockfd, &h, sizeof(h));
                write(sockfd, &h2, sizeof(h2));
                break;
            case CMD_UNREG:
                newbbslog(BBSLOG_SMS,"send CMD_UNREG");
                getbuf(&h3, sizeof(h3));
                write(sockfd, &h, sizeof(h));
                write(sockfd, &h3, sizeof(h3));
                break;
            case CMD_REPLY:
                newbbslog(BBSLOG_SMS,"send CMD_REPLY");
                getbuf(&h5, sizeof(h5));
                write(sockfd, &h, sizeof(h));
                write(sockfd, &h5, sizeof(h5));
                break;
            case CMD_BBSSEND:
                newbbslog(BBSLOG_SMS,"send CMD_BBSSEND");
                getbuf(&h4, sizeof(h4));
                write(sockfd, &h, sizeof(h));
                write(sockfd, &h4, sizeof(h4));
                write(sockfd, buf, byte2long(h4.MsgTxtLen));
                getbuf(NULL, byte2long(h4.MsgTxtLen));
                break;
	    default:
		getSession()->head->sem=0;
		return;
        }
    }
    time(&lastsendtime);

    getSession()->head->sem=0;
}

#if HAVE_MYSQL_SMTH == 1
int sms_init_mysql(){
	mysql_init(&mysql_s);
	if (! my_connect_mysql(&mysql_s) ){
		newbbslog(BBSLOG_SMS,"connect mysql error:%s",mysql_error(&mysql_s));
		mysql_close(&mysql_s);
		exit(0);
	}
	return 1;
}
#endif

int main()
{
    struct sockaddr_in addr;
    fd_set readset;
    struct timeval to;
    int rc,remain=0,retr;
    struct sigaction act;
    time_t now;

    init_sessiondata(getSession());
    start_daemon();
#if HAVE_MYSQL_SMTH == 1
	sms_init_mysql();
#endif
    load_sysconf();
    resolve_ucache();
    resolve_utmp();
    init_memory();
    running=1;
    errno=0;

    bzero(&act, sizeof(act));
    act.sa_handler = do_exit_sig;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGABRT, &act, NULL);
    
    while (running) {
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1) {
        bbslog("3error","smsd:Unable to create socket.\n");
        printf("Unable to Unable create socket.\n");
	sleep(5);
	continue;
	/*
        shmdt(head);
        buf=NULL;
        return -1;
	*/
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(sysconf_str("SMS_ADDRESS"));
    addr.sin_port=htons(sysconf_eval("SMS_PORT", 4002));
    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr))<0) {
        close(sockfd);
        bbslog("3error","smsd:Unable to connect.\n");
        newbbslog(BBSLOG_SMS,"Unable to connect.");
	sleep(5);
	continue;
	/*
        shmdt(head);
        buf=NULL;
        return -1;
	*/
    }
    loginas(sysconf_str("SMS_USERNAME"),sysconf_str("SMS_PASSWORD"));
    time(&lastsendtime);
    time(&lastrcvtime);

    while(1) {
	FD_ZERO(&readset);
        FD_SET(sockfd, &readset);
	to.tv_sec = 1;
	to.tv_usec = 0;
        if((retr=select(sockfd+1, &readset, NULL, NULL, &to))<0) break;
        if(retr) {
            if (FD_ISSET(sockfd, &readset)) {
                rc = read(sockfd, ((void*)&h)+remain, sizeof(h)-remain);
				/* add by roy 2003.7.30 */
				if (rc==0) {
					break;
				}
                if(rc<0) break;
                remain+=rc;
                if(remain==sizeof(h)) {
                    remain=0;
                    processremote();
		    time(&lastrcvtime);
                }
            }
        } 
        processbbs();
	time(&now);
	if ((now-lastrcvtime)> 3 * THRESHHOLE) {
		break;
	}
	if ((now-lastsendtime)>=THRESHHOLE) {
		keepLink();
		time(&lastsendtime);
	}
	
    }
    
    close(sockfd);
        }
#if HAVE_MYSQL_SMTH == 1
	mysql_close(&mysql_s);
#endif
    shmdt(getSession()->head);
    buf=NULL;
    return 0;
}
#else
int main()
{
   return 0;
}
#endif
