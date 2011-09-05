#include "bbs.h"
#ifdef lint
#include <sys/uio.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "screen.h"
#define MAXMESSAGE 5

int RMSG = false;
extern int RUNSH;
extern struct screenline *big_picture;
static int RMSGCount = 0;       /* Leeward 98.07.30 supporting msgX */
extern int i_timeout;

#ifdef SMS_SUPPORT
int do_send_sms_func(char * dest, char * msgstr);
#endif

int get_msg(char * uid, char * msg, int line, int sms)
{
    char genbuf[3];
    int i;
	int gdataret;

    move(line, 0);
    clrtoeol();
    if(sms)
        prints("�����Ÿ�:%-12s    ������������ݣ�Ctrl+Q ����:", uid);
    else
        prints("�����Ÿ�:%-12s    �������������ݣ�Ctrl+Q ����:", uid);
    msg[0] = 0;
    while (1) {
        i = multi_getdata(line+1, 0, 79, NULL, msg, MAX_MSG_SIZE, 11, false,0);
        if (msg[0] == '\0')
            return false;

		if(sms){
			char tmp[100];
			sprintf(tmp,"ȷ��Ҫ�ͳ���(��%d�ֽ�,������ǰ��׺)(Y)�ǵ� (N)��Ҫ (E)�ٱ༭? [Y]: ", (int)strlen(msg));
        	gdataret = getdata(line + i + 1, 0, tmp, genbuf, 2, DOECHO, NULL, 1);
		}else
        	gdataret = getdata(line + i + 1, 0, "ȷ��Ҫ�ͳ���(Y)�ǵ� (N)��Ҫ (E)�ٱ༭? [Y]: ", genbuf, 2, DOECHO, NULL, 1);

		if(gdataret == -1) return false;
        if (genbuf[0] == 'e' || genbuf[0] == 'E')
            continue;
        if (genbuf[0] == 'n' || genbuf[0] == 'N')
            return false;
        if (genbuf[0] == 'G') {
            if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
                return 2;
            else
                return true;
        } else
            return true;
    }
}

int s_msg(void){
    return do_sendmsg(NULL, NULL, 0);
}

extern bool inremsg;
extern bool in_do_sendmsg;

int do_sendmsg(uentp, msgstr, mode)
struct user_info *uentp;
const char msgstr[MAX_MSG_SIZE];
int mode;
{
    char uident[STRLEN];
    struct user_info *uin;
    char buf[MAX_MSG_SIZE];
    int Gmode = 0;
    int result;

    inremsg = true;

    if ((mode == 0) || (mode == 3)) {
        modify_user_mode(MSGING);
    }
    if (uentp == NULL) {
        move(1, 0);
        clrtobot();
        prints("��ѶϢ��: ");
        creat_list();
		in_do_sendmsg=true;
#ifdef SMS_SUPPORT
        if( namecomplete(NULL, uident) == '#' ){
			in_do_sendmsg=false;
            inremsg = false;
    		if(!getSession()->currentmemo->ud.mobileregistered)
				return 0;
            getdata(1, 0, "�Ͷ��Ÿ�:", uident, MOBILE_NUMBER_LEN+1, true, NULL, true);
			if( uident[0] == 0)
				return 0;
			if( !isdigit( uident[0] ) ){
				char mobile[STRLEN];
				if( get_al_mobile( uident, mobile, getSession()) == NULL || *mobile==0 || !isdigit(mobile[0]) ){
	        		move(2,0);
        			prints("������ֻ���!");
        			pressreturn();
					return 0;
				}
				strncpy(uident, mobile, 14);
				uident[14]=0;
			}
			return do_send_sms_func(uident, NULL);
		}
#else
		        namecomplete(NULL, uident);
#endif
		in_do_sendmsg=0;
        if (uident[0] == '\0') {
            clear();
            inremsg = false;
            return 0;
        }
        uin = t_search(uident, false);
        if (uin == NULL) {
            move(2, 0);
            prints("�Է�Ŀǰ�������ϣ�����ʹ���ߴ����������...");
            pressreturn();
            move(2, 0);
            clrtoeol();
            inremsg = false;
            return -1;
        }
        if (!canmsg(getCurrentUser(), uin)) {
            move(2, 0);
            prints("�Է��Ѿ��رս���ѶϢ�ĺ�����...");
            pressreturn();
            move(2, 0);
            clrtoeol();
            inremsg = false;
            return -1;
        }
        /*
         * ��������msg��Ŀ��uid 1998.7.5 by dong 
         */
        strcpy(getSession()->MsgDesUid, uident);
        /*
         * uentp = uin; 
         */

    } else {
        /*
         * if(!strcasecmp(uentp->userid,getCurrentUser()->userid))  rem by Haohmaru,�����ſ����Լ����Լ���msg
         * return 0;    
         */ uin = uentp;
        strcpy(uident, uin->userid);
        /*
         * strcpy(getSession()->MsgDesUid, uin->userid); change by KCN,is wrong 
         */
    }

    /*
     * try to send the msg 
     */
    result = sendmsgfunc(uin, msgstr, mode, uinfo.pid, getSession());

    switch (result) {
    case 1:                    /* success */
        if (mode == 0) {
            clear();
        }
        inremsg = false;
        return 1;
        break;
    case -1:                   /* failed, reason in msgerr */
        if (mode == 0) {
            move(2, 0);
            clrtobot();
            prints(getSession()->msgerr);
            pressreturn();
            move(2, 0);
            clrtoeol();
        }
        inremsg = false;
        return -1;
        break;
    case 0:                    /* message presending test ok, get the message and resend */
        if (mode == 4)
            return 0;
        Gmode = get_msg(uident, buf, 1, 0);
        if (!Gmode) {
            move(1, 0);
            clrtoeol();
            move(2, 0);
            clrtoeol();
            inremsg = false;
            return 0;
        }
        break;
    default:                   /* unknown reason */
        inremsg = false;
        return result;
        break;
    }
    /*
     * resend the message 
     */
    result = sendmsgfunc(uin, buf, mode, uinfo.pid, getSession());

    switch (result) {
    case 1:                    /* success */
        if (mode == 0) {
            clear();
        }
        inremsg = false;
        return 1;
        break;
    case -1:                   /* failed, reason in msgerr */
        if (mode == 0) {
            move(2, 0);
            clrtobot();
            prints(getSession()->msgerr);
            pressreturn();
            move(2, 0);
            clrtoeol();
        }
        inremsg = false;
        return -1;
        break;
    default:                   /* unknown reason */
        inremsg = false;
        return result;
        break;
    }
}



int show_allmsgs(void){
    char buf[MAX_MSG_SIZE], showmsg[MAX_MSG_SIZE*2], chk[STRLEN];
    int oldmode, count, i, j, page, ch, y, all=0, reload=0;
    struct msghead head;

    if(!HAS_PERM(getCurrentUser(), PERM_PAGE)) return -1;
    oldmode = uinfo.mode;
    modify_user_mode(LOOKMSGS);
//    set_alarm(0, 0, NULL, NULL);

    page = 0;
    count = get_msgcount(0, getCurrentUser()->userid);
    while(1) {
        if(reload) {
            reload = 0;
            page = 0;
            count = get_msgcount(all?2:0, getCurrentUser()->userid);
        }
        clear();
        if(count==0) {
            move(5,30);
            prints("\033[mû���κε�ѶϢ���ڣ���");
            i = 0;
        }
        else {
            y = 0;
            i = page;
            load_msghead(all?2:0, getCurrentUser()->userid, i, &head);
            load_msgtext(getCurrentUser()->userid, &head, buf);
            j = translate_msg(buf, &head, showmsg, getSession());
            while(y<=t_lines-1) {
                y+=j; i++;
                prints("%s", showmsg);
                if(i>=count) break;
                load_msghead(all?2:0, getCurrentUser()->userid, i, &head);
                load_msgtext(getCurrentUser()->userid, &head, buf);
                j = translate_msg(buf, &head, showmsg, getSession());
            }
        }
        move(t_lines-1,0);
        if(!all)
            prints("\033[1;44;32mѶϢ�����   ����<\033[37mr\033[32m> ���<\033[37mc\033[32m> �Ļ�����<\033[37mm\033[32m> ��Ѷ��<\033[37mi\033[32m> ѶϢ����<\033[37ms\033[32m>        ʣ��:%4d ", count-i);
        else
            prints("\033[1;44;32mѶϢ�����   ����<\033[37mr\033[32m> ���<\033[37mc\033[32m> �Ļ�����<\033[37mm\033[32m> ��Ѷ��<\033[37mi\033[32m> ѶϢ����<\033[37ms\033[32m> ȫ��<\033[37ma\033[32m>     %4d ", count-i);
        clrtoeol();
        resetcolor();
reenter:
        ch = igetkey();
        switch(ch) {
            case 'r':
            case 'R':
            case 'q':
            case 'Q':
            case KEY_LEFT:
            case '\r':
            case '\n':
                goto outofhere;
            case KEY_UP:
                if(page>0) page--;
                break;
            case KEY_DOWN:
                if(page<count-1) page++;
                break;
            case KEY_PGDN:
            case ' ':
            case KEY_RIGHT:
                if(page<count-11) page+=10;
                else page=count-1;
                break;
            case KEY_PGUP:
                if(page>10) page-=10;
                else page=0;
                break;
            case KEY_HOME:
            case Ctrl('A'):
                page=0;
                break;
            case KEY_END:
            case Ctrl('E'):
                page=count-1;
                break;
            case 'i':
            case 'I':
            case 's':
            case 'S':
                reload = 1;
                count = get_msgcount(0, getCurrentUser()->userid);
                if(count==0) break;
                move(t_lines-1, 0);
                clrtoeol();
                getdata(t_lines-1, 0, "������ؼ���:", chk, 50, true, NULL, true);
                if(chk[0]) {
                    int fd, fd2;
                    char fname[STRLEN], fname2[STRLEN];
                    size_t bm_search[256];
                    struct msghead head;
                    int i;
                    int init=false;
                    sethomefile(fname, getCurrentUser()->userid, "msgindex");
                    sethomefile(fname2, getCurrentUser()->userid, "msgindex3");
                    fd = open(fname, O_RDONLY, 0644);
                    fd2 = open(fname2, O_WRONLY|O_CREAT, 0644);
                    write(fd2, &i, 4);
                    lseek(fd, 4, SEEK_SET);
                    for(i=0;i<count;i++) {
                        read(fd, &head, sizeof(struct msghead));
                        if(toupper(ch)=='S') load_msgtext(getCurrentUser()->userid, &head, buf);
                        if((toupper(ch)=='I'&&!strncasecmp(chk, head.id, IDLEN))
                          ||(toupper(ch)=='S'&&bm_strcasestr_rp(buf, chk, bm_search, &init) != NULL))
                            write(fd2, &head, sizeof(struct msghead));
                    }
                    close(fd2);
                    close(fd);
                    all = 1;
                }
                break;
            case 'c':
            case 'C':
                if (count!=0) {
					move(t_lines-1, 0);
                	clrtoeol();
                	if (askyn("ȷ��ɾ������ѶϢ��", false))
                    	clear_msg(getCurrentUser()->userid);
				}
                goto outofhere;
            case 'a':
            case 'A':
                if(all) {
                    sethomefile(buf, getCurrentUser()->userid, "msgindex3");
                    unlink(buf);
                    all = 0;
                    reload = 1;
                }
                break;
            case 'm':
            case 'M':
                if(count!=0)mail_msg(getCurrentUser(), getSession());
                goto outofhere;
            default:
                goto reenter;
        }
    }
outofhere:
    
    if(all) {
        sethomefile(buf, getCurrentUser()->userid, "msgindex3");
        unlink(buf);
    }
    clear();
    modify_user_mode(oldmode);
//    R_monitor(NULL);
    return 0;
}


int dowall(struct user_info *uin, char *buf2)
{
    if (!uin->active || !uin->pid)
        return -1;
    /*---	������ǰ���ڷ���Ϣ��	period	2000-11-13	---*/
    if (getpid() == uin->pid)
        return -1;

    move(1, 0);
    clrtoeol();
    prints("\033[32m���� %s �㲥.... Ctrl-D ֹͣ�Դ�λ User �㲥��\033[m", uin->userid);
    refresh();
    if (strcmp(uin->userid, "guest")) { /* Leeward 98.06.19 */
        /*
         * ��������msg��Ŀ��uid 1998.7.5 by dong 
         */
        strcpy(getSession()->MsgDesUid, uin->userid);

        do_sendmsg(uin, buf2, 3);       /* �㲥ʱ���ⱻ����� guest ��� */
    }
    return 0;
}


int wall(void){
    char buf2[MAX_MSG_SIZE];

    if (check_systempasswd() == false)
        return 0;
    modify_user_mode(MSGING);
    move(2, 0);
    clrtobot();
    if (!get_msg("����ʹ����", buf2, 1, 0)) {
        move(1, 0);
        clrtoeol();
        move(2, 0);
        clrtoeol();
        return 0;
    }
    move(t_lines-1,0);
    clrtoeol();
    if (!askyn("���Ҫ�㲥ô", 0)) {
        clear();
        return 0;
    }
    if (apply_ulist((APPLY_UTMP_FUNC) dowall, buf2) == -1) {
        move(2, 0);
        prints("û���κ�ʹ��������\n");
        pressanykey();
    }
    sprintf(genbuf, "%s �㲥:%s", getCurrentUser()->userid, buf2);
    securityreport(genbuf, NULL, NULL, getSession());
    prints("\n�Ѿ��㲥���....\n");
    pressanykey();
    return 0;
}

int msg_count;

void r_msg_sig(int signo)
{
    msg_count++;
    signal(SIGUSR2, r_msg_sig);
}

void r_msg()
{
    int y, x, ch, i, ox, oy, tmpansi, pid, oldi;
    char savebuffer[25][LINELEN*3];
    char buf[MAX_MSG_SIZE+100], outmsg[MAX_MSG_SIZE*2], uid[14];
    struct user_info * uin;
    struct msghead head;
    int now, count, canreply, first=1;
    int hasnewmsg;
    int savemode;

    noscroll();
    savemode=uinfo.mode;
    modify_user_mode(MSGING);
    getyx(&y, &x);
    tmpansi = showansi;
    showansi = 1;
    oldi = i_timeout;
    set_alarm(0, 0, NULL, NULL);
    RMSG = true;
    RMSGCount++;
    for(i=0;i<=23;i++)
        saveline(i, 0, savebuffer[i]);

    hasnewmsg=get_unreadcount(getCurrentUser()->userid);
    if ((savemode == POSTING || savemode == SMAIL) && !DEFINE(getCurrentUser(), DEF_LOGININFORM)) {      /*Haohmaru.99.12.16.������ʱ����msg */
        move(0, 0);
        if (hasnewmsg) {
            prints("\033[1m\033[33m�����µ�ѶϢ���뷢�������º� Ctrl+Z ��ѶϢ\033[m");
            move(y, x);
            refresh();
            sleep(1);
        } else {
            prints("\033[1mû���κ��µ�ѶϢ����!\033[m");
            move(y, x);
            refresh();
            sleep(1);
        }
        clrtoeol();
        goto outhere;
    }
    count = get_msgcount(1, getCurrentUser()->userid);
    if (!count) {
        move(0, 0);
        prints("\033[1mû���κε�ѶϢ���ڣ���\033[m");
        clrtoeol();
        move(y, x);
        refresh();
        sleep(1);
        goto outhere;
    }

    now = get_unreadmsg(getCurrentUser()->userid);
    if(now==-1) now = count-1;
    else {
        load_msghead(1, getCurrentUser()->userid, now, &head);
        while(head.topid!=uinfo.pid&&now<count-1){
            now = get_unreadmsg(getCurrentUser()->userid);
            load_msghead(1, getCurrentUser()->userid, now, &head);
        };
    }
    while(1){
        int reg=0;
        load_msghead(1, getCurrentUser()->userid, now, &head);
        load_msgtext(getCurrentUser()->userid, &head, buf);
        translate_msg(buf, &head, outmsg, getSession());
        
        if (first&&hasnewmsg&&DEFINE(getCurrentUser(), DEF_SOUNDMSG)&&(head.mode!=6))
            bell();
        move(0,0);
        if(head.mode==6&&(!strcmp(outmsg,"REQUIRE:BIND")||!strcmp(outmsg,"REQUIRE:UNBIND"))) {
            if(!strcmp(outmsg,"REQUIRE:BIND")) {
                sprintf(outmsg, "ע���ֻ��� %s �� (y/N)\n", head.id);
                reg=1;
            }
            else {
                sprintf(outmsg, "ȡ��ע���ֻ��� %s �� (y/N)\n", head.id);
                reg=2;
            }
        }
        prints("%s", outmsg);

        if(first) {
            int x,y;
            getyx(&y,&x);
            if(!reg)
                prints("\033[m �� %d ����Ϣ / �� %d ����Ϣ", now+1, count);
            clrtoeol();
            do{
                ch = igetkey();
            }while(!DEFINE(getCurrentUser(), DEF_IGNOREMSG)&&ch!=Ctrl('Z')&&ch!='r'&&ch!='R');
            first = 0;
            move(y, x);
        }
        
        strncpy(uid, head.id, IDLEN+2);
        pid = head.frompid;
        uin = t_search(uid, pid);
#ifdef SMS_SUPPORT
        if(head.mode==6) canreply = 1;
        else
#endif
        if(head.mode==3||uin==NULL||uin->mode==BBSNET||uin->mode==TETRIS||uin->mode==WINMINE) canreply = 0;
        else canreply = 1;
        
        clrtoeol();
        if(!reg) {
        if(canreply)
            prints("\033[m �� %d ����Ϣ / �� %d ����Ϣ, �ظ� %-12s\n", now+1, count, uid);
        else {
            if(uin)
                prints("\033[m �� %d ����Ϣ / �� %d ����Ϣ,�����л�,Enter����, ����Ϣ�޷��ظ�", now+1, count);
            else
                prints("\033[m �� %d ����Ϣ / �� %d ����Ϣ,�����л�,Enter����, �û�%s����վ,�޷��ظ�", now+1, count, uid);
        }
        }
        getyx(&oy, &ox);
        
        if(canreply) {
            ch = -multi_getdata(oy, ox, 79, NULL, buf, 1024, 11, true, 0);
            if(ch<0) oy-=ch+1;
        }
        else {
            do {
                ch = igetkey();
            } while(ch!=KEY_UP&&ch!=KEY_DOWN&&ch!='\r'&&ch!='\n');
        }
        switch(ch) {
            case Ctrl('Z'):
                ch = '\n';
                break;
            case KEY_UP:
                now--;
                if(now<0) now=count-1;
                break;
            case KEY_DOWN:
                now++;
                if(now>=count) now=0;
                break;
            default:
                if(canreply) {
                    if(buf[0]) {
                        strcpy(getSession()->MsgDesUid, uid);
                        pid = head.frompid;
                        uin = t_search(uid, pid);
                        if((uin==NULL || uin->mode == BBSNET || uin->mode==TETRIS || uin->mode==WINMINE) && head.mode!=6) {
                            i=-1;
                            strcpy(getSession()->msgerr, "�Է��Ѿ�����....");
							/* stiger, 20051008, mail msg to local mailbox */
							msgmail(uid, buf);
                        }
                        else {
#ifdef SMS_SUPPORT
                            if(head.mode==6) {
                                if(!reg) {
                                    i = do_send_sms_func(uid, buf);
                                    if(!i) i=1;
                                    else {
                                        i=0;
                                        sprintf(getSession()->msgerr, "�޷��� %s �����ֻ�����", uid);
                                    }
                                }
                                else {
                                    i = DoReplyCheck(uid, head.frompid, toupper(buf[0])=='Y', getSession());
                                    if(!i) {
                                        if(reg==1) {
//                                            curruserdata.mobileregistered = 0;
//                                            strcpy(curruserdata.mobilenumber, uid);
                                            getSession()->currentmemo->ud.mobileregistered = 0;
                                            strcpy(getSession()->currentmemo->ud.mobilenumber, uid);
                                        }
                                        else {
//                                            curruserdata.mobileregistered = 0;
                                            getSession()->currentmemo->ud.mobileregistered = 0;
                                        }
//                                        write_userdata(getCurrentUser()->userid, &curruserdata);
                                        write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
                                        sprintf(getSession()->msgerr, "%s �ɹ�", (reg==1)?"ע��":"ȡ��ע��");
                                    }
                                    else sprintf(getSession()->msgerr, "%s ʧ��", (reg==1)?"ע��":"ȡ��ע��");
                                    i = 0;
                                }
                            }
                            else
#endif
                                i = sendmsgfunc(uin, buf, 4, uinfo.pid, getSession());
                        }
                        buf[0]=0;
                        if(i==1) strcpy(buf, "\033[1m�����ͳ�ѶϢ��\033[m");
                        else if(i!=0) strcpy(buf, getSession()->msgerr);
                        if(buf[0]) {
                            int j=i;
                            if(i!=1&&i!=0) {
                                move(oy+1, 0);
                                prints("%s �����������", buf);
                                igetkey();
                                saveline(oy+1, 1, savebuffer[oy+1]);
                            }
                            else {
                                for(i=0;i<=oy;i++)
                                    saveline(i, 1, savebuffer[i]);
                                move(0,0);
                                clrtoeol();
                                prints("%s", buf);
                            }
                            if(j==1) {
                                refresh();
                                sleep(1);
                            }
                        }
                    }
                    ch = '\n';
                }
                break;
        }
        for(i=0;i<=oy;i++)
            saveline(i, 1, savebuffer[i]);
        if (ch=='\r'||ch=='\n') {
        	// make a tag for msg end
//        	prints("\x1b[m�ѷ�����Ϣ");
        	break;
        }
    }


outhere:
    for(i=0;i<=23;i++)
        saveline(i, 1, savebuffer[i]);
    showansi = tmpansi;
    move(y,x);
    if(oldi)
        R_monitor(NULL);
    RMSGCount--;
    if (0 == RMSGCount)
        RMSG = false;
    modify_user_mode(savemode);
    return;
}

void r_lastmsg()
{
    r_msg();
}

int myfriend_wall(struct user_info *uin, char *buf, int i)
{
    if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid || !canmsg(getCurrentUser(), uin))
        return -1;
    if (myfriend(uin->uid, NULL, getSession())) {
        move(1, 0);
        clrtoeol();
        prints("\x1b[1;32m������ѶϢ�� %s...  \x1b[m", uin->userid);
        refresh();
        strcpy(getSession()->MsgDesUid, uin->userid);
        do_sendmsg(uin, buf, 5);
    }
    return 0;
}

int friend_wall(void){
    char buf[MAX_MSG_SIZE];

    if (uinfo.invisible) {
        move(2, 0);
        prints("��Ǹ, �˹���������״̬�²���ִ��...\n");
        pressreturn();
        return 0;
    }
    modify_user_mode(MSGING);
    move(2, 0);
    clrtobot();
    if (!get_msg("�ҵĺ�����", buf, 1, 0))
        return 0;
    if (apply_ulist((APPLY_UTMP_FUNC)myfriend_wall, buf) == -1) {
        move(2, 0);
        prints("���Ͽ���һ��\n");
        pressanykey();
    }
    move(6, 0);
    prints("ѶϢ�������...");
    pressanykey();
    return 1;
}

#ifdef SMS_SUPPORT

int register_sms(void){
    char ans[4];
    char buf2[80];
    clear();
    prints("ע���ֻ���\n\nע������ֻ���֮�������bbs�Ϸ��ͺͽ����ֻ�����\n");
    move(4,0);
    if(getSession()->currentmemo->ud.mobileregistered) {
        prints("���Ѿ�ע���ֻ����ˡ�ÿһ���˺�ֻ��ע��һ���ֻ��š�\n");
        pressreturn();
        return -1;
    }
    if(getSession()->currentmemo->ud.mobilenumber[0]&&strlen(getSession()->currentmemo->ud.mobilenumber)==11) {
        sprintf(buf2, "��������ֻ�����%s���Ƿ�����ע�᣿[Y/n]", getSession()->currentmemo->ud.mobilenumber);
        getdata(3, 0, buf2, ans, 3, 1, 0, 1);
        if(toupper(ans[0])!='N') getSession()->currentmemo->ud.mobilenumber[0]=0;
    }
    if(!getSession()->currentmemo->ud.mobilenumber[0]||strlen(getSession()->currentmemo->ud.mobilenumber)!=11) {
        getdata(4, 0, "�������ֻ���: ", getSession()->currentmemo->ud.mobilenumber, 17, 1, 0, 1);
        if(!getSession()->currentmemo->ud.mobilenumber[0]||strlen(getSession()->currentmemo->ud.mobilenumber)!=11) {
            move(5, 0);
            prints("������ֻ���");
            pressreturn();
            return -1;
        }
        move(5, 0);
        uid2smsid(&uinfo, buf2);
        prints("��������ֻ���%s����:\nZCYH %s\n��ע���û����ɹ�֮���㽫�յ�ȷ����",
            sysconf_str("SMS_NUMBER"),buf2);
        pressreturn();
     }
    return 0;
/*
    sms_init_memory();
    smsuin = &uinfo;
//        if(DoReg(curruserdata.mobilenumber)) {
        if(DoReg(getSession()->currentmemo->ud.mobilenumber)) {
            signal(SIGUSR1, talk_request);
            move(5, 0);
            prints("����ע����ʧ��");
            pressreturn();
            shmdt(head);
            smsbuf=NULL;
            return -1;
        }
        signal(SIGUSR1, talk_request);
        move(5, 0);
        prints("����ע����ɹ�");
    }
    getdata(6, 0, "���������ע����: ", valid, 11, 1, 0, 1);
    if(!valid[0]) return -1;
//    if(DoCheck(curruserdata.mobilenumber, valid)) {
    if(DoCheck(getSession()->currentmemo->ud.mobilenumber, valid)) {
        signal(SIGUSR1, talk_request);
        move(7, 0);
        prints("ע������ʧ��");
        pressreturn();
        shmdt(head);
        smsbuf=NULL;
        return -1;
    }
    signal(SIGUSR1, talk_request);
//    curruserdata.mobileregistered = 1;
    getSession()->currentmemo->ud.mobileregistered = 1;
//    write_userdata(getCurrentUser()->userid, &curruserdata);
    write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
    move(7, 0);
    prints("�ֻ�ע��ɹ��� �������bbs�Ϸ��Ͷ�������");
    pressreturn();
    shmdt(head);
    smsbuf=NULL;
    return 0;
    */
}

int unregister_sms(void){
    char ans[4];
    char buf2[80];
    int rr;
    sms_init_memory(getSession());
    getSession()->smsuin = &uinfo;
    clear();
    prints("ȡ��ע���ֻ���");
    move(4,0);
//    if(!curruserdata.mobileregistered) {
    if(!getSession()->currentmemo->ud.mobileregistered) {
        prints("����δע���ֻ���");
        pressreturn();
        shmdt(getSession()->head);
        getSession()->smsbuf=NULL;
        return -1;
    }
//    sprintf(buf2, "��������ֻ�����%s���Ƿ�ȡ��ע�᣿[y/N]", curruserdata.mobilenumber);
    sprintf(buf2, "��������ֻ�����%s���Ƿ�ȡ��ע�᣿[y/N]", getSession()->currentmemo->ud.mobilenumber);
    getdata(3, 0, buf2, ans, 3, 1, 0, 1);
    if(toupper(ans[0])=='Y') {
        
//        rr = DoUnReg(curruserdata.mobilenumber);
        rr = DoUnReg(getSession()->currentmemo->ud.mobilenumber, getSession());
        if(rr&&rr!=CMD_ERR_NO_SUCHMOBILE) {
            signal(SIGUSR1, talk_request);
            move(5, 0);
            prints("������ȡ��ע��ʧ��");
            pressreturn();
            shmdt(getSession()->head);
//	    curruserdata.mobileregistered = 0;

	    getSession()->currentmemo->ud.mobileregistered = 0;
        getSession()->currentmemo->ud.mobilenumber[0]=0;
//	    write_userdata(getCurrentUser()->userid, &curruserdata);
	    write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
            getSession()->smsbuf=NULL;
            return -1;
        }
        signal(SIGUSR1, talk_request);
        move(5, 0);
        prints("ȡ��ע��ɹ�");
//        curruserdata.mobilenumber[0]=0;
        getSession()->currentmemo->ud.mobilenumber[0]=0;
//        curruserdata.mobileregistered = 0;
        getSession()->currentmemo->ud.mobileregistered = 0;
//        write_userdata(getCurrentUser()->userid, &curruserdata);
        write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
    }
    shmdt(getSession()->head);
    getSession()->smsbuf=NULL;
    return 0;
}

int do_send_sms_func(char * dest, char * msgstr)
{
    char uident[STRLEN];
    struct userdata udata;
    char buf[MAX_MSG_SIZE];
    int oldmode;
    int ret;
    bool cansend=true;
    struct userec * ur;

//    if(!curruserdata.mobileregistered) {
    if(!getSession()->currentmemo->ud.mobileregistered) {
        move(1, 0);
        clrtoeol();
        prints("����δע���ֻ��ţ��޷������˷��Ͷ���");
        pressreturn();
        /*
        getdata(2, 0, "�Ƿ�����ע���ֻ���? (y/N)", ans, 3, 1, 0, 1);
        move(1, 0);
        clrtoeol();
        move(2, 0);
        if(toupper(ans[0])=='Y') {
            if(!register_sms())
                goto checksmsagain;
        }
        */
        return -1;
    }
    
    sms_init_memory(getSession());
    getSession()->smsuin = &uinfo;
    inremsg = true;

    oldmode = uinfo.mode;
    modify_user_mode(MSGING);
    if (dest == NULL) {
        move(1, 0);
        clrtobot();
        getdata(1, 0, "�Ͷ��Ÿ�: ", uident, 15, 1, 0, 1);
        if (uident[0] == '\0') {
            clear();
            modify_user_mode(oldmode);
            inremsg = false;
            return -1;
        }
    }
    else
        strcpy(uident, dest);
    if(isdigit(uident[0])) {
        int i;
        cansend=cansend&&(strlen(uident)==11);
        for(i=0;i<strlen(uident);i++)
            cansend=cansend&&(isdigit(uident[i]));
        if(cansend)
            strcpy(udata.mobilenumber, uident);
    }
    else {
	if (dest==NULL) {
        move(2,0);
        prints("������ֻ���!");
        pressreturn();
        move(2, 0);
        clrtoeol();
        modify_user_mode(oldmode);
        inremsg = false;
        return -1;
        }
        getuser(uident, &ur);
        if(ur)
            strcpy(uident, ur->userid);
        if(read_userdata(uident,&udata))
            cansend=false;
        else {
            cansend=udata.mobileregistered&&(strlen(udata.mobilenumber)==11);
        }
    }
    if(!cansend) {
        move(2, 0);
        prints("�Է���δע���ֻ��ţ������ֻ������������...");
        pressreturn();
        move(2, 0);
        clrtoeol();
        modify_user_mode(oldmode);
        inremsg = false;
        return -1;
    }

    if(msgstr==NULL) {
        if(!get_msg(uident, buf, 1, 1)) {
            move(1, 0);
            clrtoeol();
            move(2, 0);
            clrtoeol();
            modify_user_mode(oldmode);
            inremsg = false;
            return -1;
        }
    }
    else
        strcpy(buf, msgstr);

	if( strlen(buf) + strlen(getSession()->currentmemo->ud.smsprefix) + strlen(getSession()->currentmemo->ud.smsend) < MAX_MSG_SIZE ){
		int i,i1,j;

		i=strlen(buf);
		i1=strlen(getSession()->currentmemo->ud.smsprefix);
		for(j= i+i1; j>=i1; j--){
			buf[j] = buf[j-i1];
		}
		for(j=0;j<i1;j++)
			buf[j] = getSession()->currentmemo->ud.smsprefix[j];
		strcat(buf, getSession()->currentmemo->ud.smsend);

	}

//    ret = DoSendSMS(curruserdata.mobilenumber, udata.mobilenumber, buf);
    ret = DoSendSMS(getSession()->currentmemo->ud.mobilenumber, udata.mobilenumber, buf, getSession());
    signal(SIGUSR1, talk_request);
    if(ret==CMD_ERR_SMS_VALIDATE_FAILED) {
//        curruserdata.mobilenumber[0]=0;
        getSession()->currentmemo->ud.mobilenumber[0]=0;
//        curruserdata.mobileregistered = 0;
        getSession()->currentmemo->ud.mobileregistered = 0;
//        write_userdata(getCurrentUser()->userid, &curruserdata);
        write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
    }
    if(ret) {
        clrtoeol();
        prints("����ʧ��....");
        if (ret==CMD_EXCEEDMONEY_LIMIT)
            prints("����ÿ���޶�!");
        pressreturn();
    }
    else {
        struct msghead h;
        struct user_info *uin=NULL;
        h.frompid = uinfo.pid;
        h.topid = -1;
        if(!isdigit(uident[0])) {
            uin = t_search(uident, false);
            if(uin) h.topid = uin->pid;
        }
        h.mode = 6;
        h.sent = 1;
        h.time = time(0);
        strcpy(h.id, uident);
        save_msgtext(getCurrentUser()->userid, &h, buf, getSession());
#if HAVE_MYSQL_SMTH == 1
        save_smsmsg(getCurrentUser()->userid, &h, buf, 1, getSession());
#endif
        if(!isdigit(uident[0])) {
            h.sent = 0;
            strcpy(h.id, getCurrentUser()->userid);
            save_msgtext(uident, &h, buf, getSession());
#if HAVE_MYSQL_SMTH == 1
        	save_smsmsg(uident, &h, buf, 1, getSession());
#endif
            if(uin) kill(uin->pid, SIGUSR2);
        }
    }

    modify_user_mode(oldmode);
    inremsg = false;
    return ret;
}

int send_sms(void){
    return do_send_sms_func(NULL,NULL);
}

#endif

