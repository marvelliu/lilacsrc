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
    
    2002/08/05 checked global variable.
    2001/5/6 modified by wwj
*/
#include "bbs.h"
#ifdef lint
#include <sys/uio.h>
#endif                          /* 
                                 */
/********************snow*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef SOLARIS
#include <sys/filio.h>
#endif

#include "chat.h"
extern char page_requestor[];
extern char *modestring();
extern struct UTMPFILE *utmpshm;

#define s_lines         (t_lines-2)
#define b_lines         (t_lines-1)
#define screen_lines    (t_lines-4)
int chat_cmd(chatcontext * pthis, char *buf);
static void set_rec(chatcontext * pthis, const char *arg);

#define CHAT_LOGIN_OK       "OK"
#define CHAT_LOGIN_EXISTS   "EX"
#define CHAT_LOGIN_INVALID  "IN"
#define CHAT_LOGIN_BOGUS    "BG"
static const char *msg_seperator =
    "������������������������������������������������������������������������������";
static const char *msg_shortulist =
    "\033[33m\033[44m ʹ���ߴ���    Ŀǰ״̬  �� ʹ���ߴ���    Ŀǰ״̬  �� ʹ���ߴ���    Ŀǰ״̬ \033[m";
static int ent_chat(int chatnum);       /* ���������� */
int chat_waitkey(chatcontext * pthis)
{
    char ch;

    outs("           \033[5;31m�� ���ո��������Q��ȡ�� ��\033[m");
    add_io(0, 0);
    ch = igetkey();
    add_io(pthis->cfd, 0);
    return strchr(" \r\n", ch) != NULL;
}
static void printchatline(chatcontext * pthis, const char *str)
{                               /*��ʾһ�У�������ָʾ�� */
    char tmpstr[256];
    const char *p;
    int i;
    int len;                    /* add by KCN for disable long line */
    int inesc;

    if (pthis->outputcount < 0)
        return;                 /* by wwj 2001/5/9 */
    p = str;
    i = 0;
    len = 0;
    inesc = 0;
    while (*p != 0) {
        if (len >= 79) {
            tmpstr[strlen(tmpstr) - (len - 79)] = 0;
            break;
        }
        if (*p == '%') {
            if (*(p + 1) == 0) {
                tmpstr[i] = '%';
                i++;
                p++;
                len++;
            } else if (*(p + 1) == '%') {
                tmpstr[i] = '%';
                i++;
                p++;
                p++;
                len++;
            } else if (*(p + 1) > '0' && *(p + 1) <= '7') {
                tmpstr[i++] = '\033';
                tmpstr[i++] = '[';
                tmpstr[i++] = '3';
                tmpstr[i++] = *(p + 1);
                tmpstr[i++] = 'm';
                p++;
                p++;
            } else if (*(p + 1) == '0') {
                tmpstr[i++] = '\033';
                tmpstr[i++] = '[';
                tmpstr[i++] = '0';
                tmpstr[i++] = 'm';
                p++;
                p++;
            } else {
                tmpstr[i] = '%';
                i++;
                p++;
                tmpstr[i] = *p;
                i++;
                p++;
                len += 2;
            }
        } else {
            if (inesc) {
                if (isalpha(*p))
                    inesc = 0;
                /* ���ô�������ESC������� KCN */
            } else {
                if (*p == '\033')
                    inesc = 1;
                else
                    len++;
            }
            tmpstr[i] = *p;
            i++;
            p++;
        }
    }
    tmpstr[i++] = '\033';
    tmpstr[i++] = '[';
    tmpstr[i++] = '0';
    tmpstr[i++] = 'm';
    tmpstr[i] = 0;
    move(pthis->chatline, 0);
    clrtoeol();
    if (pthis->outputcount++ == screen_lines - 1) {
        if (!chat_waitkey(pthis))
            pthis->outputcount = -1;
        else
            pthis->outputcount = 1;
        move(pthis->chatline, 0);
        clrtoeol();
    }
    outs(tmpstr);               /* snow change at 10.25 */
    outc('\n');
    if (pthis->rec)
        fprintf(pthis->rec, "%s\n", str);
    if (++pthis->chatline == s_lines)
        pthis->chatline = 2;
    move(pthis->chatline, 0);
    clrtoeol();
    outs("==>");
}

void chat_clear(chatcontext * pthis)
{                               /* clear chat */
    for (pthis->chatline = 2; pthis->chatline < s_lines; pthis->chatline++) {
        move(pthis->chatline, 0);
        clrtoeol();
    }
    pthis->chatline = s_lines - 1;
    printchatline(pthis, "");
    pthis->outputcount = 0;
}

void print_chatid(chatcontext * pthis)
{
    move(b_lines, 0);
    outs(pthis->chatid);
    outc(':');
}
int chat_send(chatcontext * pthis, const char *buf)
{
    int len;
    char gbuf[256];

    for (len = 0; len < (int) sizeof(gbuf) - 1 && buf[len]; len++)
        gbuf[len] = buf[len];
    gbuf[len++] = '\n';
    return (send(pthis->cfd, gbuf, len, 0) == len);     /* ͨ��socket send */
}

/* buffered recv  2001/5/6 -- wwj */
int chat_recv(chatcontext * pthis, char *buf, int sz)
{
    int len;

    do {
        if (buf && pthis->bufptr) {     /* ��������������У��ȷ��ظ�user */
            len = sz;
            if (len > pthis->bufptr)
                len = pthis->bufptr;
            memcpy(buf, pthis->buf, len);
            if (len < pthis->bufptr) {
                memcpy(pthis->buf, &pthis->buf[len], pthis->bufptr - len);
            }
            pthis->bufptr -= len;
            return len;
        }
        len =
            recv(pthis->cfd, &pthis->buf[pthis->bufptr],
                 sizeof(pthis->buf) - pthis->bufptr, 0);
        if (len <= 0)
            return -1;
        pthis->bufptr += len;
    } while (buf);
    return 0;
}

int chat_parse(chatcontext * pthis)
{
    int len;
    char *bptr;

    if (!pthis->bufptr || pthis->buf[pthis->bufptr - 1] != 0) {
        if (chat_recv(pthis, NULL, 0) < 0)
            return -1;
    }
    len = 0;
    bptr = pthis->buf;
    while (bptr < pthis->buf + pthis->bufptr) {
        for (len = 0; bptr + len < pthis->buf + pthis->bufptr; len++) {
            if (!bptr[len]) {
                len = -len;
                break;
            }
        }
        if (len > 0)
            break;
        len = -len + 1;         /* skip 0 */
        if (*bptr == '/') {     /* ����server���������� */
            switch (bptr[1]) {
            case 'p':
                /* add by KCN for list long emote */
                /* chat_waitkey(pthis);  removed by wwj */
                break;
            case 'c':
                chat_clear(pthis);
                break;
            case 'n':
                strncpy(pthis->chatid, bptr + 2, 8);
                pthis->chatid[8] = 0;
                print_chatid(pthis);
                clrtoeol();
                uinfo.in_chat = true;
                strcpy(uinfo.chatid, pthis->chatid);
                UPDATE_UTMP_STR(chatid, uinfo);
                break;
            case 'r':
                strncpy(pthis->chatroom, bptr + 2, IDLEN - 1);
                break;
            case 'z':
                strncpy(pthis->chatroom, bptr + 2, IDLEN - 1);
                /* fall throw */
            case 't':
                move(0, 0);
                clrtoeol();
                if (bptr[1] == 't')
                    strcpy(pthis->topic, bptr + 2);
                prints
                    ("\033[44m\033[33m ���䣺 \033[36m%-10s  \033[33m���⣺\033[36m%-51s\033[31m%2s\033[m",
                     pthis->chatroom, pthis->topic,
                     (pthis->rec) ? "¼" : "  ");
                break;
            }
        } else {
            printchatline(pthis, bptr);
        }
        bptr += len;
        len = 0;
    }
    if (len > 0) {
        memcpy(pthis->buf, bptr, pthis->bufptr - (bptr - pthis->buf));
        pthis->bufptr -= bptr - pthis->buf;
    } else {
        pthis->bufptr = 0;
    }
    return 0;
}

int chat_checkparse(chatcontext * pthis)
{
    long cnt;
    int result = 0;

    cnt = 0;
    if (pthis->bufptr
        || ((result = ioctl(pthis->cfd, FIONREAD, &cnt)) == 0 && cnt)) {
        do {
            if (chat_parse(pthis) == -1)
                return 0;
        } while (pthis->bufptr);        /* read all data from server */
    }
    if (result != 0)
        return 0;
    return 1;
}

/* KCN add chat 2 */
int ent_chat2(void){
    return ent_chat(2);
}

int ent_chat1(void){
    return ent_chat(1);
}

/* 2001/5/6 --wwj, �޸� ent_chat ���� */
int ent_chat_conn(chatcontext * pthis, int chatnum)
{
    struct sockaddr_in sin;
    int ch;
    char inbuf[128], *ptr;

    memset(&sin, 0, sizeof sin);
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
/*
    sin.sin_addr.s_addr = inet_addr("166.111.8.237");
*/
    if (chatnum == 1)
        sin.sin_port = htons(CHATPORT3);
    else
        sin.sin_port = htons(CHATPORT2);
    pthis->cfd = socket(sin.sin_family, SOCK_STREAM, 0);
    if (connect(pthis->cfd, (struct sockaddr *) &sin, sizeof sin)) {    /*������� chatd ʧ�ܣ�������chatd */
        close(pthis->cfd);
        switch (ch = fork()) {
        case -1 /*fork failure */ :
            bbslog("chatd", "fork error");
            break;
        case 0 /*fork success */ :
            bbslog("chatd", "fork success");
            prints("����������...");
            if (chatnum == 1)
                system("bin/chatd");
            else
                system("bin/chatd 2");
            exit(1);
        default:
            bbslog("chatd", "fork par-proc");
            /* The chat daemon forks so we can wait on it here. */
            waitpid(ch, NULL, 0);
        }
        pthis->cfd = socket(sin.sin_family, SOCK_STREAM, 0);
        if ((connect(pthis->cfd, (struct sockaddr *) &sin, sizeof sin))) {
            close(pthis->cfd);
            bbslog("chatd", "connect2 failed %d", errno);
            return -1;
        }
    }
    /* Leeward 98.04.26 */
    move(3, 0);
    clrtoeol();
    move(4, 0);                 /* Leave line 3 for error message while entering chat room */
    prints
        ("�����ַ� \033[1m\033[37m*\033[m �ٰ� \033[1m\033[37mEnter\033[m ����ȡ�����������ҡ�                                    ");
    clrtoeol();
    move(5, 0);
    clrtoeol();                 /* Clear line 5 for good looking */
    while (1) {
		int gdataret;
        gdataret = getdata(2, 0, "������������ţ�", inbuf, 9, DOECHO, NULL, true);
        if (gdataret == -1 || '*' == inbuf[0]) {  /* Leeward 98.04.26 */
            close(pthis->cfd);
            return 0;
        }
        if (inbuf[0] != '\0' && inbuf[0] != '\n' && inbuf[0] != '/') {
            strncpy(pthis->chatid, inbuf, 8);
        } else {
            strncpy(pthis->chatid, getCurrentUser()->userid, 8);
        }
        pthis->chatid[8] = '\0';
        sprintf(inbuf, "/! %d %d %s %s", uinfo.uid, getCurrentUser()->userlevel,
                getCurrentUser()->userid, pthis->chatid);
        chat_send(pthis, inbuf);        /* send user info to chatd , and chatd will check it */
        if (chat_recv(pthis, inbuf, 3) != 3) {
            close(pthis->cfd);
            return 0;
        }
        if (!strcmp(inbuf, CHAT_LOGIN_OK))
            break;
        else if (!strcmp(inbuf, CHAT_LOGIN_EXISTS))
            ptr = "��������Ѿ���������";
        else if (!strcmp(inbuf, CHAT_LOGIN_INVALID))
            ptr = "��������Ǵ����";
        else
            ptr =
                "�Ѿ���һ�����������������ˣ�����������˳�BBS���µ�¼������������������վ����";
        move(3, 0);
        prints(ptr);
        clrtoeol();
        bell();
    }
    return 1;
}
static int ent_chat(int chatnum)
{                               /* ���������� */
    chatcontext *pthis;
    char inbuf[128];
    int ch, cmdpos;
    int currchar;
    int modified;               /* the line is modified? -- wwj */
    int newmail;
    int page_pending = false;
    int chatting = true;
#ifdef NEW_HELP
	int oldhelpmode=helpmode;
#endif

    if (!strcmp(getCurrentUser()->userid, "guest"))
        return -1;
    pthis = (chatcontext *) malloc(sizeof(chatcontext));
    bzero(pthis, sizeof(chatcontext));
    if (!pthis)
        return -1;
    modify_user_mode(CHAT1);
    ch = ent_chat_conn(pthis, chatnum);
    if (ch != 1) {
        free(pthis);
        return ch;
    }
#ifdef NEW_HELP
	helpmode=HELP_CHAT;
#endif
    add_io(pthis->cfd, 0);
    modified = newmail = cmdpos = currchar = 0;
    /* update uinfo */
    uinfo.in_chat = true;
    strcpy(uinfo.chatid, pthis->chatid);
    UPDATE_UTMP(in_chat, uinfo);
    UPDATE_UTMP_STR(chatid, uinfo);
    /* initiate screen */
    clear();
    pthis->chatline = 2;
    move(s_lines, 0);
    outs(msg_seperator);
    move(1, 0);
    outs(msg_seperator);
    print_chatid(pthis);
    memset(inbuf, 0, 80);
    /* chat begin */
    while (chatting) {
        if (chat_checkparse(pthis) == 0)
            break;
        move(b_lines, currchar + 10);
        pthis->outputcount = 0;
        ch = igetkey();
        if (ch==KEY_TALK) {
            int talkpage = servicepage(0, pthis->buf);

            if (talkpage != page_pending) {
            	bell();
            	oflush();
                printchatline(pthis, pthis->buf);
                page_pending = talkpage;
            }
        }
        if (chat_checkparse(pthis) == 0)
            break;
        if (ch == I_OTHERDATA)
            continue;
        switch (ch) {
        case KEY_UP:
        case KEY_DOWN:
            if (cmdpos == pthis->cmdpos) {
                strcpy(pthis->lastcmd[cmdpos], inbuf);
                modified = 0;
            }
            if (ch == KEY_UP) {
                if (cmdpos != (pthis->cmdpos + 1) % MAXLASTCMD) {
                    int i = (cmdpos + MAXLASTCMD - 1) % MAXLASTCMD;

                    if (pthis->lastcmd[i][0])
                        cmdpos = i;
                }
            }
            if (ch == KEY_DOWN) {
                if (cmdpos != pthis->cmdpos)
                    cmdpos = (cmdpos + 1) % MAXLASTCMD;
            }
            strcpy(inbuf, pthis->lastcmd[cmdpos]);
            if (cmdpos == pthis->cmdpos) {
                modified = 1;
            }
            move(b_lines, 10);
            clrtoeol();
            ch = inbuf[69];
            inbuf[69] = 0;
            outs(inbuf);
            inbuf[69] = ch;
            currchar = strlen(inbuf);
            continue;
#ifdef CHINESE_CHARACTER
        case Ctrl('R'):
			SET_CHANGEDEFINE(getCurrentUser(), DEF_CHCHAR);
        	continue;
#endif        	
        case KEY_LEFT:
            if (currchar)
                --currchar;
#ifdef CHINESE_CHARACTER
			if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
				int i,j=0;
				for(i=0;i<currchar;i++)
					if(j) j=0;
					else if(inbuf[i]<0) j=1;
				if(j) {
					currchar--;
				}
			}
#endif
            continue;
        case KEY_RIGHT:
            if (inbuf[currchar])
                ++currchar;
#ifdef CHINESE_CHARACTER
			if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
				int i,j=0;
				for(i=0;i<currchar;i++)
					if(j) j=0;
					else if(inbuf[i]<0) j=1;
				if(j) {
		            if (inbuf[currchar])
        		        ++currchar;
				}
			}
#endif
            continue;
        case KEY_ESC:
        case Ctrl('X'):
            inbuf[0] = 0;
            currchar = 0;
            move(b_lines, currchar + 10);
            clrtoeol();
            modified = 1;
            continue;
        case Ctrl('A'):
            currchar = 0;
            continue;
        case Ctrl('E'):
            currchar = strlen(inbuf);
            continue;
        }
        if (!newmail && chkmail(0)) {   /* check mail */
            newmail = 1;
            printchatline(pthis, "\033[32m*** \033[31m����������������...\033[m");
        }
        if (isprint2(ch))
        {
            if (currchar < 126) {       /* δ��һ��,print it */
                modified = 1;
                if (inbuf[currchar]) {  /* insert */
                    int i;

                    for (i = currchar; inbuf[i] && i < 127; i++);
                    inbuf[i + 1] = '\0';
                    for (; i > currchar; i--)
                        inbuf[i] = inbuf[i - 1];
                } else {        /* append */
                    inbuf[currchar + 1] = '\0';
                }
                inbuf[currchar] = ch;
                ch = inbuf[69]; /* save the end of line */
                inbuf[69] = 0;
                move(b_lines, currchar + 10);
                outs(&inbuf[currchar++]);
                inbuf[69] = ch;
            }
            continue;
        }
        if (ch == '\n' || ch == '\r') {
            if (currchar) {
                if (modified) {
                    /* add to command history */
                    ch = sizeof(pthis->lastcmd[pthis->cmdpos]) - 1;
                    strncpy(pthis->lastcmd[pthis->cmdpos], inbuf, ch);
                    pthis->lastcmd[pthis->cmdpos][ch] = 0;
                    pthis->cmdpos = (pthis->cmdpos + 1) % MAXLASTCMD;
                    cmdpos = pthis->cmdpos;
                } else {        /* use history, so can +1 */
                    cmdpos = (cmdpos + 1) % MAXLASTCMD;
                }
                if (inbuf[0] == '/' && Isspace(inbuf[1])) {     /* discard / b */
                    printchatline(pthis,
                                  "\x1b[37m*** \x1b[32m��������ȷ��ָ�ʹ��/hѰ�����\x1b[37m ***\x1b[m");
                } else {
                    chatting = chat_cmd(pthis, inbuf);  /*local����� */
                    if (chatting == 0)
                        chatting = chat_send(pthis, inbuf);
                    if (inbuf[0] == '/') {
                        ch = 1;
                        while (inbuf[ch] != '\0' && inbuf[ch] != ' ')
                            ch++;
                        if (ch > 1) {
                            if (!strncasecmp(inbuf, "/bye", ch))
                                break;
                            if (!strncasecmp(inbuf, "/exit", ch))
                                break;  /*added by alex, 96.9.5 */
                        }
                    }
                }
                modified = 0;
                inbuf[0] = '\0';
                currchar = 0;
                move(b_lines, 10);
                clrtoeol();
            }
            continue;
        }
        if (ch == Ctrl('H') || ch == '\177') {  /*Backspace */
            if (currchar) {
                currchar--;
                inbuf[127] = '\0';
                memcpy(&inbuf[currchar], &inbuf[currchar + 1],
                       127 - currchar);
                move(b_lines, currchar + 10);
                clrtoeol();
                ch = inbuf[69]; /* save the end of line */
                inbuf[69] = 0;
                outs(&inbuf[currchar]);
                inbuf[69] = ch;
            }
#ifdef CHINESE_CHARACTER
			if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
				int i,j=0;
				for(i=0;i<currchar;i++)
					if(j) j=0;
					else if(inbuf[i]<0) j=1;
				if(j) {
	                currchar--;
	                inbuf[127] = '\0';
	                memcpy(&inbuf[currchar], &inbuf[currchar + 1],
	                       127 - currchar);
	                move(b_lines, currchar + 10);
	                clrtoeol();
	                ch = inbuf[69]; /* save the end of line */
	                inbuf[69] = 0;
	                outs(&inbuf[currchar]);
	                inbuf[69] = ch;
				}
			}
#endif
            continue;
        }
        if (ch == Ctrl('Z')) {
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
            inbuf[0] = '\0';
            currchar = 0;
            move(b_lines, 10);
            clrtoeol();
            continue;
        }
        if (ch == Ctrl('C') /*|| ch == Ctrl('D') */ ) { /* ^C �˳� */
            chat_send(pthis, "/b");
            if (pthis->rec)
                set_rec(pthis,NULL);
            break;
        }
    }
    /* chat end */
    if(pthis->rec)set_rec(pthis,NULL);
    close(pthis->cfd);
    add_io(0, 0);
    uinfo.in_chat = false;
    uinfo.chatid[0] = '\0';
    UPDATE_UTMP(in_chat, uinfo);
    UPDATE_UTMP(chatid[0], uinfo);
    clear();
    free(pthis);
#ifdef NEW_HELP
	helpmode=oldhelpmode;
#endif
    return 0;
}

/*
int printuserent(chatcontext * pthis, struct user_info *uentp)
{
    static char uline[256];
    static int cnt;
    char pline[50];

    if (!uentp) {
        if (cnt)
            printchatline(pthis, uline);
        bzero(uline, 256);
        cnt = 0;
        return 0;
    }
    if (!uentp->active || !uentp->pid)
        return 0;
    if (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible)
        return 0;
#if 0
    if (kill(uentp->pid, 0) == -1)
        return 0;
#endif
    sprintf(pline, " %s%-13s\033[m%c%-10s",
            myfriend(uentp->uid, NULL) ? "\033[32m" : "", uentp->userid,
            uentp->invisible ? '#' : ' ', modestring(uentp->mode,
                                                     uentp->destuid, 0,
                                                     NULL));
    if (cnt < 2)
        strcat(pline, "��");
    strcat(uline, pline);
    if (++cnt == 3) {
        printchatline(pthis, uline);
        memset(uline, 0, 256);
        cnt = 0;
    }
    return 0;
}
*/
int print_friend_ent(struct user_info *uentp, chatcontext * pthis, int pos)
{                               /* print one user & status if he is a friend */
    char pline[50],buf[80];

    if (!uentp->active || !uentp->pid)
        return 0;
    if (!HAS_PERM(getCurrentUser(), PERM_SEECLOAK) && uentp->invisible)
        return 0;
#if 0
    if (kill(uentp->pid, 0) == -1)
        return 0;
#endif                          /* 
                                 */
    if (!myfriend(uentp->uid, NULL,getSession()))
        return 0;
    sprintf(pline, " %-13s%c%-10s", uentp->userid,
            uentp->invisible ? '#' : ' ', modestring(buf,uentp->mode,
                                                     uentp->destuid, 0,
                                                     NULL));
    if (pthis->apply_count < 2)
        strcat(pline, "��");
    strcat(pthis->apply_buf, pline);
    if (pthis->apply_count == 2) {
        printchatline(pthis, pthis->apply_buf);
        memset(pthis->apply_buf, 0, 256);
        pthis->apply_count = 0;
    } else
        pthis->apply_count++;
    return COUNT;
}
void chat_help(chatcontext * pthis, const char *arg)
{
    char buf[256];
    FILE *fp;

    if (strstr(arg, "op")) {    /* op's help */
        if ((fp = fopen("help/chatophelp", "r")) == NULL)
            return;
        while (fgets(buf, 255, fp) != NULL) {
            printchatline(pthis, buf);
        }
        fclose(fp);
    } else {
        if ((fp = fopen("help/chathelp", "r")) == NULL)
            /* user's help */
            return;
        while (fgets(buf, 255, fp) != NULL) {
            char *ptr;

            ptr = strstr(buf, "\n");
            *ptr = '\0';
            printchatline(pthis, buf);
        }
        fclose(fp);
    }
}
void call_user(chatcontext * pthis, const char *arg)
{                               /* invite user online to chatroom */
    char msg[STRLEN * 2];
    struct user_info *uin;
    int state;

    if (!*arg) {
        printchatline(pthis,
                      "\033[37m*** \033[32m��������Ҫ����� ID\033[37m ***\033[m");
        return;
    }
    if (!strcasecmp(arg, getCurrentUser()->userid))
        sprintf(msg, "\033[32m�㲻�������Լ���\033[m");
    else if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
        /* Leeward 98.07.30 */
        sprintf(msg, "\033[32m��û�з���Ϣ��Ȩ��\033[m");
    else {
        uin = t_search(arg, false);
        if (uin == NULL)
            sprintf(msg, "\033[32m%s\033[37m ��û����վ\033[m", arg);
        else if (LOCKSCREEN == uin->mode)
            /* Leeward 98.02.28 */
            sprintf(msg,
                    "\033[32m%s\033[37m�Ѿ�������Ļ�����Ժ�����������(��)д��.\n",
                    uin->userid);
        else if (!canmsg(getCurrentUser(), uin))
            sprintf(msg, "%s �Ѿ��رս���ѶϢ�ĺ�����.\n", uin->userid);
        else {
            if (get_unreadcount(uin->userid) > MAXMESSAGE)
                sprintf(msg,
                        "�Է�����һЩѶϢδ�������Ժ�����������(��)д��...\n");
            else {
                sprintf(msg, "������㳡�� %s ������ ������",
                        pthis->chatroom);
                /* ��������msg��Ŀ��uid 1998.7.5 by dong */
                strcpy(getSession()->MsgDesUid, uin->userid);
                state = do_sendmsg(uin, msg, 1);
                if (state == 1)
                    sprintf(msg, "\033[37m�Ѿ��������� %s ��\033[m",
                            uin->userid);
                else if (state == -2)
                    sprintf(msg, "\033[37m�Է��Ѿ�������...\033[m");
                else
                    sprintf(msg, "\033[37�Է������������Ϣ...\033[m");
            }
        }
    }
    printchatline(pthis, msg);
}
void chat_sendmsg(chatcontext * pthis, const char *arg)
{                               /* send msg in chatroom , by alex, 96.9.5 */
    char userid[IDLEN + 1];
    char msg[STRLEN * 2];
    struct user_info *uin;
    int state;

    nextword(&arg, userid, sizeof(userid));
    if (!userid[0]) {
        printchatline(pthis,
                      "\x1b[37m*** \x1b[32m��������Ҫ����Ϣ�� ID\x1b[37m ***\x1b[m");
        return;
    } else if (!*arg) {
        printchatline(pthis,
                      "\x1b[37m*** \x1b[32m��������Ҫ������Ϣ\x1b[37m ***\x1b[m");
        return;
    } else if (!HAS_PERM(getCurrentUser(), PERM_PAGE)) {     /* Leeward 98.07.30 */
        sprintf(msg, "\x1b[32m��û�з���Ϣ��Ȩ��\x1b[m");
    } else {
        uin = t_search(userid, false);
        if (uin == NULL) {
            sprintf(msg, "\x1b[32m%s\x1b[37m ��û����վ\x1b[m", userid);
        } else if (LOCKSCREEN == uin->mode)
            /* Leeward 98.02.28 */
            sprintf(msg,
                    "\x1b[32m%s\x1b[37m�Ѿ�������Ļ�����Ժ��ٷ������(��)д��.\n",
                    uin->userid);
        else {
            if (!canmsg(getCurrentUser(), uin))
                sprintf(msg,
                        "\x1b[32m%s\x1b[37m�Ѿ��رս���ѶϢ�ĺ�����.\n",
                        uin->userid);
            else if (false == canIsend2(getCurrentUser(),uin->userid))
                sprintf(msg, "\x1b[32m%s\x1b[37m�ܾ��������ѶϢ.\n", uin->userid);     /*Haohmaru.99.6.6,����Ƿ�ignore */
            else {
                if (get_unreadcount(uin->userid) > MAXMESSAGE)
                    sprintf(msg,
                            "�Է�����һЩѶϢδ�������Ժ��ٷ������(��)д��...\n");
                else {
                    /* ��������msg��Ŀ��uid 1998.7.5 by dong */
                    strcpy(getSession()->MsgDesUid, uin->userid);
                    state = do_sendmsg(uin, arg, 2);
                    if (state == 1)
                        sprintf(msg,
                                "\x1b[37m�Ѿ�����Ϣ�� \x1b[32m%s\x1b[37m ��\x1b[m",
                                uin->userid);
                    else if (state == -2)
                        sprintf(msg, "\x1b[37m�Է��Ѿ�������...\x1b[m");
                    else
                        sprintf(msg, "\033[37�Է������������Ϣ...\033[m");
                }
            }
        }
    }
    printchatline(pthis, msg);
}
int c_cmpuids(int uid, struct user_info *up)
{
    return (uid == up->uid);
}
int chat_status(struct user_info *uentp, chatcontext * pthis)
{
    char tmpstr[31],buf[80],buf2[80];
    char *lpTmp;

    if (strlen(genbuf)>t_columns) 
    	return QUIT;
    if (uentp->invisible == 1) {
        if (HAS_PERM(getCurrentUser(), PERM_SEECLOAK)) {
            sprintf(genbuf + strlen(genbuf), "\x1b[32m#\x1b[m");
        } else
            return 0;
    }
    lpTmp = (char *) idle_str(buf,uentp);
    if (uentp->in_chat) {       /* add by Luzi 1997.11.18 */
        int res;

        sprintf(tmpstr, "/q %s", uentp->userid);
        chat_send(pthis, tmpstr);
        res = chat_recv(pthis, tmpstr, 30);
        if (res <= 0)
            return -1;
        tmpstr[res] = '\0';
        if (tmpstr[0] == '1') {
            sprintf(genbuf + strlen(genbuf), "'%s' room as '%s'",
                    tmpstr + 1, uentp->chatid);
            if (lpTmp[0] != ' ')
                sprintf(genbuf + strlen(genbuf), "[%s];", lpTmp);
            else
                strcat(genbuf, " ;");
            return COUNT;
        }
    }
    sprintf(genbuf, "%s%-8s", genbuf, modestring(buf2,uentp->mode, uentp->destuid, 0,        /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                                 (uentp->in_chat ? uentp->
                                                  chatid : NULL)));
    if (lpTmp[0] != ' ')
        sprintf(genbuf + strlen(genbuf), "[%s];", lpTmp);
    else
        strcat(genbuf, " ;");
    return COUNT;
}
static void query_user(chatcontext * pthis, const char *userid)
{
    int tuid = 0;
    char qry_mail_dir[STRLEN], inbuf[STRLEN * 2];
    char *newline;
    time_t exit_time, temp;
    struct userec *lookupuser;

    if (!(tuid = getuser(userid, &lookupuser))) {
        printchatline(pthis, "\033[32m���ID�����ڣ�\033[m");
        return;
    }
    setmailfile(qry_mail_dir, lookupuser->userid, DOT_DIR);
    /*---	modified by period	2000-11-02	hide posts/logins	---*/

    sprintf(genbuf, "%s (%s):      %s", lookupuser->userid,
            lookupuser->username,
            (check_query_mail(qry_mail_dir, NULL)) ? "������" : "    ");
    printchatline(pthis, genbuf);
    sprintf(genbuf, "����վ %d �Σ������ %d ƪ���£�������[%d]%s",
            lookupuser->numlogins, lookupuser->numposts,
            compute_user_value(lookupuser),
            (lookupuser->userlevel & PERM_SUICIDE) ? " (��ɱ��)" : " ");
    printchatline(pthis, genbuf);

    strcpy(inbuf, ctime(&(lookupuser->lastlogin)));
    if ((newline = strchr(genbuf, '\n')) != NULL)
        *newline = '\0';
    strcpy(genbuf, "��ǰ״̬��");
    if (apply_utmpuid((APPLY_UTMP_FUNC) chat_status, tuid, (char *) pthis)) {
        char buf[1024];

	lookupuser->lasthost[IPLEN-1] = '\0';
        sprintf(buf, "Ŀǰ��������: ���� %s ����ʱ�� %s" /*\n" */ ,
                (lookupuser->lasthost[0] == '\0' /* || DEFINE(getCurrentUser(),DEF_HIDEIP) */ ? "(����)" : SHOW_USERIP(lookupuser, lookupuser->lasthost)), inbuf);    /*Haohmaru.99.12.18 */
        printchatline(pthis, buf);
        printchatline(pthis, genbuf);
    } else {
        lookupuser->lasthost[IPLEN-1] = '\0';  
        sprintf(genbuf, "�ϴ���������  %s ʱ��Ϊ %s " /*\n" */ ,
                (lookupuser->lasthost[0] == '\0' /* || DEFINE(getCurrentUser(),DEF_HIDEIP) */ ? "(����)" : SHOW_USERIP(lookupuser, lookupuser->lasthost)), inbuf);    /* Haohmaru.99.12.18 */
        printchatline(pthis, genbuf);
        /* �������ʱ�� Luzi 1998/10/23 */
        exit_time = get_exit_time(lookupuser, genbuf);
        if ((newline = strchr(genbuf, '\n')) != NULL)
            *newline = '\0';
        if (exit_time > lookupuser->lastlogin)
            strcpy(inbuf, genbuf);
        /*Haohmaru.98.12.04.�Ͳ˵���ѯ���һ�� */
        if (exit_time <= lookupuser->lastlogin)
            /*
               || (uin.active && uin.pid
               && (!uin.invisible || (uin.invisible && HAS_PERM(getCurrentUser(),PERM_SEECLOAK)))))
             */
            strcpy(inbuf, "�����߻���߲���");
        if (exit_time <= lookupuser->lastlogin) {       /* && (uin.invisible&& !HAS_PERM(getCurrentUser(),PERM_SEECLOAK))) */
            temp = lookupuser->lastlogin + (lookupuser->numlogins % 7) + 5;
            strcpy(inbuf, ctime(&temp));        /*Haohmaru.98.12.04.�������û�����ȥ����ʱ�������ʱ����5���� */
            if ((newline = strchr(inbuf, '\n')) != NULL)
                *newline = '\0';
        }
        /*       else strcpy(inbuf,"[��ǳ����߲���]"); */
        sprintf(genbuf, "����ʱ��Ϊ %s " /*\n" */ , inbuf);
        printchatline(pthis, genbuf);
    }
#ifdef DEBUG
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        sprintf(genbuf, "%d", tuid);
        printchatline(pthis, genbuf);
    }
#endif                          /* 
                                 */
}
void call_query(chatcontext * pthis, const char *arg)
                                                            /* by alex, 1996.9.5 */
 /* by Luzi, 1997.11.30 */
{
    if (!*arg) {
        printchatline(pthis, "*** ������Ҫ��ѯ��ID ***");
    } else {
        query_user(pthis, arg);
    }
}
void call_query_ByChatid(chatcontext * pthis, const char *arg)
{                               /* add by dong, 1998.9.12 */
    char uident[32];
    char tmpstr[40];
    int res;

    if (!*arg) {
        printchatline(pthis, "*** ������Ҫ��ѯ��chat ID ***");
        return;
    }
    strncpy(uident, arg, 32);
    uident[31] = '\0';
    /* get user id from the chat id */
    sprintf(tmpstr, "/qc %s", uident);
    chat_send(pthis, tmpstr);
    res = chat_recv(pthis, tmpstr, 40);
    if (res <= 0)
        return;
    tmpstr[res] = '\0';
    if (tmpstr[0] == '1') {
        sprintf(uident, "%s", tmpstr + 1);
    } else {
        sprintf(genbuf, "\033[32m���chat ID�����ڣ�\033[m");
        printchatline(pthis, genbuf);
        return;
    }
    query_user(pthis, uident);
}
void chat_date(chatcontext * pthis, const char *arg)
{
    time_t thetime;

    time(&thetime);
    sprintf(genbuf, " %s��׼ʱ��: \033[32m%s\033[m", BBS_FULL_NAME,
            Cdate(thetime));
    printchatline(pthis, genbuf);
}
void chat_friends(chatcontext * pthis, const char *arg)
{
    /* printchatline(pthis,"");   moved by wwj */
    int num, i;
    struct user_info* u;

    pthis->apply_count = 0;
    sprintf(genbuf, "\033[1m�� ��ǰ���ϵĺ����б� ��\033[m");
    printchatline(pthis, genbuf);
    printchatline(pthis, msg_shortulist);
    pthis->apply_buf = genbuf;
    pthis->apply_buf[0] = 0;
    num = 0;
    u=get_utmpent(getSession()->utmpent);
    for (i = 0; i < u->friendsnum; i++) {
        num +=
            apply_utmpuid((APPLY_UTMP_FUNC) print_friend_ent,
                          u->friends_uid[i], (char *) pthis);
    }
    if (pthis->apply_count)
        printchatline(pthis, pthis->apply_buf);
    if (num == 0) {
        printchatline(pthis, "\033[1mû������������\033[m");
    }
}
void set_rec(chatcontext * pthis, const char *arg)
{                               /* set recorder */
    char fname[STRLEN];
    time_t now;

    now = time(0);
    /*        if(!HAS_PERM(getCurrentUser(),PERM_SYSOP))
       return; */

    /*sprintf(fname, "tmp/%s.chat", getCurrentUser()->userid);*/
    sethomefile(fname,getCurrentUser()->userid,"chatrec");

    if (!pthis->rec) {
        if ((pthis->rec = fopen(fname, "a")) == NULL)
            return;
        printchatline(pthis, "\033[5m\033[32mRecord Start ...\033[m");
        move(0, 0);
        clrtoeol();
        prints
            ("\033[44m\033[33m ���䣺 \033[36m%-10s  \033[33m���⣺\033[36m%-51s\033[31m%2s\033[m",
             pthis->chatroom, pthis->topic, (pthis->rec) ? "¼" : "  ");
        fprintf(pthis->rec,
                "������: %s (%s) ����: %s\n��  ��: %s\x1b[m\n\n",
                getCurrentUser()->userid, getCurrentUser()->username,
                pthis->chatroom, pthis->topic);
        fprintf(pthis->rec, "������ %s", getCurrentUser()->userid);
        fprintf(pthis->rec, "��¼�£�ʱ�䣺 %s", ctime(&now));
        bbslog("user", "start record room %s", pthis->chatroom);
    } else {
        move(0, 0);
        clrtoeol();
        prints
            ("\033[44m\033[33m ���䣺 \033[36m%-10s  \033[33m���⣺\033[36m%-51s\033[31m%2s\033[m",
             pthis->chatroom, pthis->topic, (pthis->rec) ? "¼" : "  ");
        fprintf(pthis->rec,
                "������: %s (%s) ����: %s\n��  ��: %s\x1b[m\n\n",
                getCurrentUser()->userid, getCurrentUser()->username,
                pthis->chatroom, pthis->topic);
        printchatline(pthis, "\033[5m\033[32mRecord Stop ...\033[m");
        fprintf(pthis->rec, "����ʱ�䣺%s\n", ctime(&now));
        fclose(pthis->rec);
        pthis->rec = NULL;
        mail_file(getCurrentUser()->userid, fname, getCurrentUser()->userid,
                  "¼�����", 1, NULL);
	my_unlink(fname);
        bbslog("user", "stop record room %s", pthis->chatroom);
    }
}
void setpager(chatcontext * pthis, const char *arg)
{
    char buf[STRLEN];

    t_pager();                  /*toggle pager */
    sprintf(buf, "\033[32m*** \033[31m�� %s �˺�����\033[m",
            (uinfo.pager & ALL_PAGER) ? "��" : "�ر�");
    printchatline(pthis, buf);
}
void call_kickoff(chatcontext * pthis, const char *arg)
{                               /* kick ID off BBS, by Luzi, 97.11.1 */
    char msg[STRLEN];
    struct user_info *uin;

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        printchatline(pthis, "*** �㲻��վ�� ***");
        return;
    }
    if (!*arg) {
        printchatline(pthis, "*** ��������Ҫ����վ�� ID ***");
        return;
    } else if (!strcasecmp(arg, getCurrentUser()->userid))
        sprintf(msg, "*** Faint!��Ҫ���Լ�����վ����***");
    else {
        uin = t_search(arg, false);
        if (uin == NULL) {
            sprintf(msg, "%s ��û����վ", arg);
        } else {
            bbslog("user", "kick %s out of bbs", uin->userid);
            kill(uin->pid, SIGHUP);
            return;
        }
    }
    printchatline(pthis, msg);
}
void call_listen(chatcontext * pthis, const char *arg)
{                               /* added by Luzi 1997.11.28 */
    char path[40];
    char uident[IDLEN + 1];
    char ignoreuser[IDLEN + 1];
    int nIdx;

    if (!*arg) {
        printchatline(pthis, "*** �������û���ID ***");
    } else if (!strcasecmp(arg, getCurrentUser()->userid))
        printchatline(pthis, "*** �������Լ���ID ***");
    else {
        strncpy(uident, arg, IDLEN + 1);
        uident[IDLEN] = 0;
        if (!searchuser(uident))
            /* change getuser -> searchuser, by dong, 1999.10.26 */
            printchatline(pthis, "*** û�����ID ***");
        else {
            sethomefile(path, getCurrentUser()->userid, "/ignores");
            nIdx =
                search_record(path, ignoreuser, IDLEN + 1,
                              (RECORD_FUNC_ARG) cmpinames, uident);
            if (nIdx <= 0)
                printchatline(pthis,
                              "*** ���û�������ѶϢû�б����԰� ***");
            else if (delete_record(path, IDLEN + 1, nIdx, NULL, NULL) == 0) {
                bbslog("user", "listen %s", uident);
                printchatline(pthis,
                              "*** �ѻָ��Ը��û�����ѶϢ�Ľ��� ***");
                sprintf(uident, "/listen %s\n", uident);
                chat_send(pthis, uident);
            } else {
                bbslog("3error", "listen %s failed", uident);
                printchatline(pthis, "*** system error ***");
            }
        }
    }
}
void call_ignore(chatcontext * pthis, const char *arg)
{                               /* added by Luzi 1997.11.28 */
    char buf[STRLEN], buf2[76];
    FILE *fp;
    char uident[IDLEN + 1];
    char path[40];
    char ignoreuser[IDLEN + 1];
    int nIdx;

    sethomefile(path, getCurrentUser()->userid, "/ignores");
    if (!*arg) {
        nIdx = 0;
        if ((fp = fopen(path, "r")) != NULL) {
            strcpy(buf2, "��������ѶϢ���û�ID�б�");
            while (fread(buf, IDLEN + 1, 1, fp) > -0) {
                if (nIdx % 4 == 0) {
                    printchatline(pthis, buf2);
                    *buf2 = '\0';
                }
                nIdx++;
                sprintf(buf2 + strlen(buf2), "  %-13s", buf);
            }
            fclose(fp);
        }
        if (nIdx > 0)
            printchatline(pthis, buf2);
        else
            printchatline(pthis, "*** ��δ�趨�����û������� ***");
    } else if (!strcasecmp(arg, getCurrentUser()->userid))
        printchatline(pthis, "*** �޷������Լ�����Ϣ ***");
    else {
        strncpy(uident, arg, IDLEN + 1);
        uident[IDLEN] = 0;
        if (!searchuser(uident))
            /* change getuser -> searchuser, by dong, 1999.10.26 */
            printchatline(pthis, "*** û����� ID ***");
        else {
            nIdx =
                search_record(path, ignoreuser, IDLEN + 1,
                              (RECORD_FUNC_ARG) cmpinames, uident);
            if (nIdx > 0)
                printchatline(pthis, "*** ��ID�Ѿ��������� ***");
            else {
                fp = fopen(path, "r");
                if (fp != NULL) {
                    fseek(fp, 0, SEEK_END);
                    if (ftell(fp) >= (IDLEN + 1) * MAX_IGNORE) {
                        fclose(fp);
                        printchatline(pthis, "*** �����û��������� ***");
                        return;
                    }
                    fclose(fp);
                }
                if (append_record(path, uident, IDLEN + 1) == 0) {
                    printchatline(pthis, "*** �����Ѿ��趨 ***");
                    bbslog("user", "ignore %s", uident);
                    sprintf(buf, "/ignore %s", uident);
                    chat_send(pthis, buf);
                } else {
                    printchatline(pthis, "*** ϵͳ���� ***");
                    bbslog("3error", "ignore %s failed", uident);
                }
            }
        }
    }
}
void call_alias(chatcontext * pthis, const char *arg)
{                               /* added by Luzi 1998.01.25 */
    char buf[128], buf2[200];
    FILE *fp;
    char path[40];
    char emoteid[40];
    int nIdx;

    nextword(&arg, emoteid, sizeof(emoteid));
    sethomefile(path, getCurrentUser()->userid, "/emotes");
    if (!emoteid[0]) {
        if ((fp = fopen(path, "r")) == NULL) {
            printchatline(pthis, "*** ��û���Զ����emote ***");
            return;
        }
        nIdx = 0;
        printchatline(pthis, "���û��Զ���emote�б�");
        while (fread(buf, 128, 1, fp) > 0) {
            printchatline(pthis, buf);
        }
        fclose(fp);
    } else {
        nIdx = 0;
        if ((fp = fopen(path, "r")) != NULL) {
            while (1) {
                const char *tmpbuf;
                char tmpemote[40];

                if (fread(buf, 128, 1, fp) == 0) {
                    nIdx = 0;
                    break;
                }
                tmpbuf = buf;
                nextword(&tmpbuf, tmpemote, sizeof(tmpemote));
                nIdx++;
                if (strcasecmp(emoteid, tmpemote) == 0)
                    break;
            }
            fclose(fp);
        }
        if (nIdx > 0) {
            if (*arg) {
                printchatline(pthis, "*** ��emote�Ѿ���������� ***");
                return;
            }
            if (delete_record(path, 128, nIdx, NULL, NULL) == 0) {
                printchatline(pthis, "*** ���Զ���emote�Ѿ���ɾ���� ***");
                sprintf(buf, "/alias_del %s", emoteid);
                chat_send(pthis, buf);
            } else {
                bbslog("3error", "delete alias %s fail", emoteid);
                printchatline(pthis, "*** system error ***");
            }
        } else if (!*arg)
            printchatline(pthis, "*** ��ָ��emote��Ӧ���ִ� ***");
        else {
            fp = fopen(path, "r");
            if (fp != NULL) {
                fseek(fp, 0, SEEK_END);
                if (ftell(fp) >= 128 * MAX_EMOTES) {
                    fclose(fp);
                    printchatline(pthis,
                                  "*** �û��Զ���emote���б����� ***");
                    return;
                }
                fclose(fp);
            }
            sprintf(buf, "%s %s", emoteid, arg);
            if (append_record(path, buf, 128) == 0) {
                printchatline(pthis, "*** �Զ���emote�Ѿ��趨 ***");
                sprintf(buf2, "/alias_add %s", buf);
                chat_send(pthis, buf2);
            } else {
                bbslog("3error", "add alias %s fail", emoteid);
                printchatline(pthis, "*** ϵͳ���� ***");
            }
        }
    }
}
void call_mail(chatcontext * pthis, const char *arg)
{                               /* added by Luzi, 1997/12/22 */
    fileheader mailheader;
    FILE *fpin;
    char b2[STRLEN];
    char *t;
	char direct[PATHLEN];

    if (chkmail() == 0) {       /* check mail */
        printchatline(pthis, "\033[32m*** û���µ��ż� ***\033[m");
        return;
    }

    setmailfile(direct, getCurrentUser()->userid, DOT_DIR);
    fpin = fopen(direct, "rb");
    if (fpin == NULL)
        return;
    printchatline(pthis, "\033[32m����ǰ�µ��ż����¡�\033[m");
    while (fread(&mailheader, sizeof(fileheader), 1, fpin)) {
        if ((mailheader.accessed[0] & FILE_READ) == 0) {
            strcpy(b2, mailheader.owner);
            if ((t = strchr(b2, ' ')) != NULL)
                *t = '\0';
            sprintf(genbuf, "\033[31m %-20.20s �� %.46s \033[m", b2,
                    mailheader.title);
            printchatline(pthis, genbuf);
        }
    }
    fclose(fpin);
}

/* 
 2001/5/6  modified by wwj
 �����ҿ�msg�ĺ��� 
 */
void chat_show_allmsgs(chatcontext * pthis, const char *arg)
{
    char fname[STRLEN];
    FILE *fp;
    char buf[MAX_MSG_SIZE], showmsg[MAX_MSG_SIZE*2];
    int line, cnt, i, j=0, count;
    struct msghead head;

    line = atoi(arg);
    if (line < screen_lines - 1)
        line = screen_lines - 1;
    if (line > 300)
        line = 300;
	gettmpfilename( fname, "chatmsg" );
    //sprintf(fname, "tmp/%s.msg", getCurrentUser()->userid);
    fp = fopen(fname, "w");
    count = get_msgcount(0, getCurrentUser()->userid);
    for(i=0;i<count;i++) 
    if(i>=count-line)
    {
        j++;
        load_msghead(0, getCurrentUser()->userid, i, &head);
        load_msgtext(getCurrentUser()->userid, &head, buf);
        translate_msg(buf, &head, showmsg,getSession());
        fprintf(fp, "%s", showmsg);
    }
    fclose(fp);
    if (count) {
        fp = fopen(fname, "rb");
        sprintf(buf, "����� %d ����Ϣ��", j);
        printchatline(pthis, buf);
        while (!feof(fp)) {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            if (!buf[0])
                break;
            cnt = strlen(buf) - 1;
            buf[cnt] = 0;       /* delete \n */
            printchatline(pthis, buf);
        }
        fclose(fp);
    } else {
        printchatline(pthis, "***** û���κε�ѶϢ���ڣ���*****");
    }
    unlink(fname);
}
static const struct chat_command chat_cmdtbl[] = {
    {"pager", setpager, 1},
    {"help", chat_help, 1},
    {"clear", (void (*)(chatcontext *, const char *)) chat_clear, 1},
    {"date", chat_date, 1},
    {"g", chat_friends, 1},
    {"send", chat_sendmsg, 1},  /* by alex , 96.9.5 */
    {"seemsg", chat_show_allmsgs, 1},   /* added by snow at 1998.10.24 */
    {"set", set_rec, 1},
    {"call", call_user, 1},
    {"query", call_query, 1},   /* modify by Luzi 1997.11.18 */
    {"qc", call_query_ByChatid, 2},     /* added by dong 1998.9.12 */
    {"x", call_kickoff, 1},
    {"ignore", call_ignore, 6}, /* added by Luzi 1997.11.28 */
    {"listen", call_listen, 6}, /* added by Luzi 1997.11.28 */
    {"mail", call_mail, 4},     /* added by Luzi 1997.12.22 */
/*    {"alias",call_alias,5},    added by Luzi 1998.01.25 */
    {NULL, NULL, 1}
};
int chat_cmd_match(const char *buf, const char *str, short nLenth)
{
    short i = 0;

    while (*str && *buf && !isspace(*buf)) {
        if (tolower(*buf++) != *str++)
            return 0;
        i++;
    }
    return (i >= nLenth);
}
int chat_cmd(chatcontext * pthis, char *buf)
{
    int i, j;

    if (*buf++ != '/')
        return 0;
    for (i = 0; chat_cmdtbl[i].cmdname; i++) {
        if (chat_cmd_match
            (buf, chat_cmdtbl[i].cmdname, chat_cmdtbl[i].nCmdLenth))
            break;
    }
    if (!chat_cmdtbl[i].cmdname)
        return 0;
    for (j = 0; buf[j]; j++) {
        if (buf[j] == '\t' || buf[j] == '\r' || buf[j] == '\n') {
            buf[j] = ' ';
        }
    }
    while (*buf && *buf != ' ')
        buf++;
    while (*buf && *buf == ' ')
        buf++;                  /* chop head */
    /* chop tail */
    j = strlen(buf) - 1;
    while (j > 0 && buf[j] == ' ')
        j--;
    buf[j + 1] = 0;
    chat_cmdtbl[i].cmdfunc(pthis, buf);
    return 1;
}
