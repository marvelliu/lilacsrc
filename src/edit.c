/*
        �����ʽ���� Firebird BBS ��ר�õ� Editor ����ӭ��λʹ��
        ������κ������� Mail �� SmallPig.bbs@bbs.cs.ccu.edu.tw
        ���ǵ� 140.123.101.78(bbs.cs.ccu.edu.tw) Post ���⡣
*/

#include "bbs.h"
#include "edit.h"

#define MAX_EDIT_LINE 20000

extern int temp_numposts;       /*Haohmaru.99.4.02.�ð���ˮ���˿�ȥ��//grin */
extern int local_article; //0:ת�Ű���Ĭ��ת�ţ�1:��ת�Ű��棬2:ת�Ű���Ĭ�ϲ�ת�� - atppp
char save_title[STRLEN];
int in_mail;

void vedit_key();
static struct textline *firstline = NULL;
static struct textline *lastline = NULL;
static struct textline *currline = NULL;
static int first_mark_line;
static int currpnt = 0;
static char searchtext[80];
static int editansi = 0;
static int marknum;
static int moveln = 0;
static int ismsgline;
static struct textline *top_of_win = NULL;
static int curr_window_line, currln;
static int insert_character = 1;
/* for copy/paste */
static struct textline *mark_begin, *mark_end;
static int mark_on;

void msgline();

inline static void CLEAR_MARK() {
    mark_on = 0;
    mark_begin = mark_end = NULL;
}

int myy=-1, myx, outii, outjj;
bool show_eof=false, auto_newline=true;

void display_buffer()
{
    struct textline *p;
    int ii, i, j, ch, y, shift=0;

    clear();
    y = 0; outii = -1;
    if (!auto_newline) {
        shift = currpnt/(scr_cols-5)*(scr_cols-5);
    }
    for (p = top_of_win, ii = 0; ii < t_lines - 1; ii++) {
        if (p) {
            if (editansi)
                prints("%s", p->data);
            else {
                if (p->attr & M_MARK) prints("%s", ANSI_REVERSE);
                j = 0; ch = 0;
                for (i = 0; i < p->len; i++) {
                    if (ch) ch = 0;
                    else if (p->data[i]<0) ch = 1;
                    if (auto_newline)
                    if (j >= scr_cols || (j >= scr_cols-1 && ch)) {
                        outc('\n');
                        y++;
                        j = 0;
                    }
                    if (p == currline && i == currpnt) {
                        myy = y; myx = j;
                    }
                    if (y >= scr_lns-1) {
                        outii = ii; outjj = i;
                        break;
                    }
                    if (i>=shift) {
                        if (i==shift&&p->data[i]<0&&!ch)
                            outc(' ');
                        else if (p->data[i]==27) {
                            setfcolor(YELLOW, 0);
                            outc('*');
                            resetcolor();
                        }
                        else outc(p->data[i]);
                        j++;
                    }
                }
                if (p == currline && i == currpnt) {
                    myy = y; myx = j;
                }
                if (show_eof) {
                    setfcolor((p->next)?YELLOW:GREEN, 0);
                    outc('~');
                }
                resetcolor();
            }
            p = p->next;
        } else
            prints("%s~", ANSI_RESET);
        outc('\n');
        y++;
        if (y >= scr_lns-1) {
            if (outii==-1) {
                outii = ii+1;
                outjj = 0;
            }
            break;
        }
    }

    msgline();
}

void top_show(char *prompt)
{
    if (editansi) {
        prints(ANSI_RESET);
    }
    move(0, 0);
    clrtoeol();
    prints("\x1b[7m%s\x1b[m", prompt);
}

int ask(char *prompt)
{
    int ch;

    top_show(prompt);
    ch = igetkey();
    move(0, 0);
    clrtoeol();
    return (ch);
}

static int Origin(struct textline *text);
static int process_ESC_action(int action, int arg);

void msgline()
{
    char buf[256], buf2[STRLEN * 2];
    void display_buffer();

    extern int talkrequest;
    int tmpshow;
    time_t now;

    if (ismsgline <= 0)
        return;
    now = time(0);
    tmpshow = showansi;
    showansi = 1;
    if (talkrequest) {
        talkreply();
        clear();
        showansi = 0;
        display_buffer();
        showansi = 1;
    }
    if (DEFINE(getCurrentUser(),DEF_HIGHCOLOR))
        strcpy(buf, "\033[1;33m\033[44m");
    else
        strcpy(buf, "\033[33m\033[44m");
    if (chkmail())
        strcat(buf, "��\033[32m��\033[33m��");
    else
        strcat(buf, "��  ��");

    /* Leeward 98.07.30 Change hot key for msgX */
    /*strcat(buf," \033[31mCtrl-Z\033[33m ���         "); */
    strcat(buf, " \033[31mCtrl-Q\033[33m ���    ");
    sprintf(buf2, " ״̬ [\033[32m%s\033[33m][\033[32m%d\033[33m,\033[32m%d\033[33m][\033[32m%c\033[33m][\033[32m%c\033[33m]     ʱ��", insert_character ? "����" : "�滻", currln + 1, currpnt + 1,
        show_eof?'~':' ', auto_newline?' ':'X');
    strcat(buf, buf2);
    sprintf(buf2, "\033[33m\033[44m��\033[32m%.16s\033[33m��", ctime(&now));
    strcat(buf, buf2);
    move(t_lines - 1, 0);
    prints("%s", buf);
    clrtoeol();
    resetcolor();
    showansi = tmpshow;
}

void domsg()
{
    int x, y;
    int tmpansi;

    tmpansi = showansi;
    showansi = 1;
    getyx(&x, &y);
    msgline();

    move(x, y);
    showansi = tmpansi;
    return;
}

/* ������*/
void indigestion(int i)
{
    prints("SERIOUS INTERNAL INDIGESTION CLASS %d\n", i);
    oflush();
}

/* ��ǰnum ��,ͬʱ������ʵ�ƶ���������moveln*/
struct textline *back_line(struct textline *pos,int num)
{
    moveln = 0;
    while (num-- > 0)
        if (pos && pos->prev) {
            pos = pos->prev;
            moveln++;
        }

    return pos;
}

/* ���num ��,ͬʱ������ʵ�ƶ���������moveln*/
struct textline *forward_line(struct textline * pos, int num)
{
    moveln = 0;
    while (num-- > 0)
        if (pos && pos->next) {
            pos = pos->next;
            moveln++;
        }
    return pos;
}

void countline()
{
    struct textline *pos;

    pos = firstline;
    moveln = 0;
    while (pos != lastline) {
        pos = pos->next;
        moveln++;
    }
}

int getlineno()
{
    int cnt = 0;
    struct textline *p = currline;

    while (p != top_of_win) {
        if (p == NULL)
            break;
        cnt++;
        p = p->prev;
    }
    return cnt;
}

char *killsp(char * s)
{
    while (*s == ' ')
        s++;
    return s;
}

struct textline *alloc_line()
{
    struct textline *p;

    p = (struct textline *) malloc(sizeof(*p));
    if (p == NULL) {
        indigestion(13);
        abort_bbs(0);
    }
    p->next = NULL;
    p->prev = NULL;
    p->data = malloc(WRAPMARGIN+5);
    p->maxlen = WRAPMARGIN;
    p->data[0] = '\0';
    p->len = 0;
    p->attr = 0;                /* for copy/paste */
    return p;
}

struct textline *alloc_line_w(int w)
{
    struct textline *p;

    p = (struct textline *) malloc(sizeof(*p));
    if (p == NULL) {
        indigestion(13);
        abort_bbs(0);
    }
    p->next = NULL;
    p->prev = NULL;
    p->maxlen = (w/WRAPMARGIN+1)*WRAPMARGIN+5;
    if (p->maxlen<WRAPMARGIN) p->maxlen = WRAPMARGIN;
    p->data = malloc(p->maxlen+1);
    p->data[0] = '\0';
    p->len = 0;
    p->attr = 0;                /* for copy/paste */
    return p;
}

/*
  Appends p after line in list.  keeps up with last line as well.
 */

void goline(n)
    int n;
{
    struct textline *p = firstline;
    int count;

    if (n < 0)
        n = 1;
    if (n == 0)
        return;
    for (count = 1; count < n; count++) {
        if (p) {
            p = p->next;
            continue;
        } else
            break;
    }
    if (p) {
        currln = n - 1;
        curr_window_line = 0;
        top_of_win = p;
        currline = p;
    } else {
        top_of_win = lastline;
        currln = count - 2;
        curr_window_line = 0;
        currline = lastline;
    }
    if (Origin(currline)) {
        currline = currline->prev;
        top_of_win = currline;
        curr_window_line = 0;
        currln--;
    }
    if (Origin(currline->prev)) {
        currline = currline->prev->prev;
        top_of_win = currline;
        curr_window_line = 0;
        currln -= 2;
    }


}

void go()
{
    char tmp[8];
    int line;

    set_alarm(0, 0, NULL, NULL);
    getdata(t_lines-1, 0, "����Ҫ�����ڼ���: ", tmp, 7, DOECHO, NULL, 1);
    domsg();
    if (tmp[0] == '\0')
        return;
    line = atoi(tmp);
    goline(line);
    return;
}


void searchline(text)
    char text[STRLEN];
{
    int tmpline;
    int addr;
    int tt;

    struct textline *p = currline;
    int count = 0;

    tmpline = currln;
    for (;; p = p->next) {
        count++;
        if (p) {
            if (count == 1)
                tt = currpnt;
            else
                tt = 0;
            if (strstr(p->data + tt, text)) {
                addr = (POINTDIFF) strstr(p->data + tt, text) - (POINTDIFF) p->data + strlen(text);
                currpnt = addr;
                break;
            }
        } else
            break;
    }
    if (p) {
        currln = currln + count - 1;
        curr_window_line = 0;
        top_of_win = p;
        currline = p;
    } else {
        goline(currln + 1);
    }
    if (Origin(currline)) {
        currline = currline->prev;
        top_of_win = currline;
        curr_window_line = 0;
        currln--;
    }
    if (Origin(currline->prev)) {
        currline = currline->prev->prev;
        top_of_win = currline;
        curr_window_line = 0;
        currln -= 2;
    }

}

void search()
{
    char tmp[STRLEN];

	tmp[0]='\0';
    set_alarm(0, 0, NULL, NULL);
    getdata(t_lines-1, 0, "��Ѱ�ִ�: ", tmp, 65, DOECHO, NULL, 0);
    domsg();
    if (tmp[0] == '\0')
        return;
    else
        strcpy(searchtext, tmp);

    searchline(searchtext);
    return;
}


void append(struct textline * p, struct textline * line)
{
    p->next = line->next;
    if (line->next)
        line->next->prev = p;
    else
        lastline = p;
    line->next = p;
    p->prev = line;
}

/*
  delete_line deletes 'line' from the list and maintains the lastline, and 
  firstline pointers.
 */

void delete_line(struct textline * line)
{
    /* if single line */
    if (!line->next && !line->prev) {
        line->data[0] = '\0';
        line->len = 0;
        CLEAR_MARK();
        return;
    }
#define ADJUST_MARK(p, q) if(p == q) p = (q->next) ? q->next : q->prev

    ADJUST_MARK(mark_begin, line);
    ADJUST_MARK(mark_end, line);

    if (line->next)
        line->next->prev = line->prev;
    else
        lastline = line->prev;  /* if on last line */

    if (line->prev)
        line->prev->next = line->next;
    else
        firstline = line->next; /* if on first line */
    free(line->data);
    free(line);
}

/*
  split splits 'line' right before the character pos
 */

void split(struct textline * line, int pos)
{
    struct textline *p;

    countline();
    if (moveln>MAX_EDIT_LINE) {
        return;
    }
        
    if (pos > line->len) {
        return;
    }
    p = alloc_line_w(line->len - pos);

    p->len = line->len - pos;
    line->len = pos;
    strcpy(p->data, (line->data + pos));
    p->attr = line->attr;       /* for copy/paste */
    *(line->data + pos) = '\0';
    append(p, line);
    if (line == currline && pos <= currpnt) {
        currline = p;
        currpnt -= pos;
        curr_window_line++;
        currln++;
    }
}

/*
  join connects 'line' and the next line.  It returns true if:
  
  1) lines were joined and one was deleted
  2) lines could not be joined
  3) next line is empty

  returns false if:

  1) Some of the joined line wrapped
 */

int join(struct textline * line)
{
    if (!line->next)
        return true;
    if (line->maxlen<=line->len+line->next->len+5) {
        int ml;
        char *q;
        ml = ((line->len+line->next->len)/WRAPMARGIN+1)*WRAPMARGIN+5;
        if (ml<WRAPMARGIN) ml = WRAPMARGIN;
        q = (char *) malloc(ml+1);
        memcpy(q, line->data, line->len+1);
        free(line->data);
        line->data = q;
        line->maxlen = ml;
    }
    strcat(line->data, line->next->data);
    line->len += line->next->len;
    delete_line(line->next);
    return true;
}

void insert_char(int ch)
{
    int i;
    struct textline *p = currline;

    if (currpnt > p->len) {
        indigestion(1);
        return;
    }
    if (currpnt > MAX_EDIT_LINE)
        return;
    if (currpnt < p->len && !insert_character) {
        p->data[currpnt++] = ch;
    } else {
        for (i = p->len; i >= currpnt; i--)
            p->data[i + 1] = p->data[i];
        p->data[currpnt] = ch;
        p->len++;
        currpnt++;
    }
    if (p->len >= p->maxlen-5) {
        char *q = (char *)malloc(p->maxlen+WRAPMARGIN+5);
        memcpy(q, p->data, p->len+1);
        free(p->data);
        p->data = q;
        p->maxlen += WRAPMARGIN;
    }
}

void ve_insert_str(char * str)
{
    while (*str)
        insert_char(*(str++));
}

void delete_char()
{
    int i;

    if (currline->len == 0)
        return;
    if (currpnt >= currline->len) {
        indigestion(1);
        return;
    }
    for (i = currpnt; i != currline->len; i++)
        currline->data[i] = currline->data[i + 1];
    currline->len--;
}

void vedit_init()
{
    struct textline *p = alloc_line();

    first_mark_line = 0;
    firstline = p;
    lastline = p;
    currline = p;
    currpnt = 0;
    marknum = 0;
    process_ESC_action('M', '0');
    top_of_win = p;
    curr_window_line = 0;
    currln = 0;
    CLEAR_MARK();
}

void insert_to_fp(FILE * fp)
{
    int ansi = 0;
    struct textline *p;

    for (p = firstline; p; p = p->next)
        if (p->data[0]) {
            fprintf(fp, "%s\n", p->data);
            if (strchr(p->data, '\033'))
                ansi++;
        }
    if (ansi)
        fprintf(fp, "%s\n", ANSI_RESET);
}

void insertch_from_fp(int ch)
{
    if (isprint2(ch) || ch == 27)
        insert_char(ch);
    else if (ch == Ctrl('I'))
        do {
            insert_char(' ');
        } while ((currpnt & 0x7) && (currpnt <= MAX_EDIT_LINE));
    else if (ch == '\n')
        split(currline, currpnt);
}
long insert_from_fp(FILE *fp, long * attach_length)
{
    int matched;
    char* ptr;
    off_t size;
	long ret=0;

	if( attach_length ) *attach_length=0;
    matched=0;
    BBS_TRY {
        if (safe_mmapfile_handle(fileno(fp), PROT_READ, MAP_SHARED, &ptr, & size) == 1) {
            char* data;
            long not;
            data=ptr;
            for (not=0;not<size;not++,data++) {
                if (*data==0) {
                    matched++;
                    if (matched==ATTACHMENT_SIZE) {
                        int d;
						long attsize;
						char *sstart = data;
                        data++; not++;
						if(ret == 0)
							ret = data - ptr - ATTACHMENT_SIZE + 1;
                        while(*data){
							data++;
							not++;
						}
                        data++;
                        not++;
                        memcpy(&d, data, 4);
                        attsize = htonl(d);
                        data+=4+attsize-1;
                        not+=4+attsize-1;
                        matched = 0;
						if( attach_length) *attach_length += data - sstart + ATTACHMENT_SIZE;
                    }
                    continue;
                }
                insertch_from_fp(*data);
            }
        }
	else
	{
		BBS_RETURN(-1);
	}
    }
    BBS_CATCH {
    }
    BBS_END;
    end_mmapfile((void *) ptr, size, -1);

    if(ret <= 0) return 0;
    return ret;
}

long read_file(char *filename,long *attach_length)
{
    FILE *fp;
    long ret = 0;
	struct stat fs;

    if (currline == NULL)
        vedit_init();
    if ((fp = fopen(filename, "r+b")) == NULL) {
        if ((fp = fopen(filename, "w+")) != NULL) {
            fclose(fp);
            return 0;
        }
        indigestion(4);
        abort_bbs(0);
    }
	fstat(fileno(fp), &fs);
	if (fs.st_size != 0)
    	ret=insert_from_fp(fp, attach_length);
    fclose(fp);
    return ret;
}

#define KEEP_EDITING -2

int valid_article(pmt, abort)
    char *pmt, *abort;
{
    struct textline *p = firstline;
    char ch;
    int total, lines, len, sig, y;
	int gdataret;
    int temp=0;

    if (uinfo.mode == POSTING) {
        total = lines = len = sig = 0;
        while (p != NULL) {
            if (!sig) {
                ch = p->data[0];
                if (strcmp(p->data, "--") == 0)
                    sig = 1;
                else if ((ch != ':' && ch != '>' && ch != '=') && (strlen(p->data) > 2)) {
                    lines++;
                    len += strlen(p->data);
                }
            }
            total++;
            p = p->next;
        }
        y = 2;
        if (total > 20 + lines * 3) {
            move(y, 0);
            prints("\t��ƪ���µ�������ǩ��������Զ�������ĳ���.\n");
            y += 1;
        }
        if (total!=lines) 
            lines--; /*�����re�ĺ�ǩ������Ӧ�ü���һ��*/
        temp = 0;
        if (len < 8 || lines==0 || (lines<=2 && (len/lines) < 16)) {
            move(y, 0);
            prints("\t��ƪ���·ǳ����, ϵͳ��Ϊ��ˮ����.\n");
            /*Haohmaru.99.4.02.�ð���ˮ���˿�ȥ��//grin */
            y += 3;
            temp = 1;
        }

        if (local_article == 2)
            strcpy(pmt, "(L)վ��, (S)ת��, (F)�Զ����з���, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [L]: ");
        else if (local_article == 0)
            strcpy(pmt, "(S)ת��, (L)վ��, (F)�Զ����з���, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [S]: ");
        else //local_article == 1
            strcpy(pmt, "(L)վ��, (F)�Զ����з���, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [L]: ");
    }
    gdataret = getdata(0, 0, pmt, abort, 2, DOECHO, NULL, true);
	if(gdataret == -1){
		abort[0]='a';
		abort[1]='\0';
	}
    return temp;
}

void bbsmain_add_loginfo(FILE *fp, struct userec *user, char *currboard, int Anony)
{                               /* POST ���һ�� ��� */
    int color, noidboard;

    noidboard = (anonymousboard(currboard) && Anony);   /* etc/anonymous�ļ��� ����������� */
    color = (user->numlogins % 7) + 31; /* ��ɫ����仯 */
    if ((getSession()->currentmemo->ud.signum == 0) ||        /* �ж��Ƿ��Ѿ� ���� ǩ���� */
        user->signature == 0 || noidboard) {
        fputs("\n--\n", fp);
    } else {                    /*Bigman 2000.8.10�޸�,���ٴ��� */
        fprintf(fp, "\n");
    }
    /*
     * ��Bigman����:2000.8.10 Announce�������������� 
     */
    if (!strcmp(currboard, "Announce"))
        fprintf(fp, "\033[m\033[1;%2dm�� ��Դ:��%s %s��[FROM: %s]\033[m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, BBS_FULL_NAME);
    else
        fprintf(fp, "\n\033[m\033[1;%2dm�� ��Դ:��%s %s��[FROM: %s]\033[m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, (noidboard) ? NAME_ANONYMOUS_FROM : SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
    return;
}

/*etnlegend, 2005.09.26, �༭����ʱ�ṩ���������ӿ�*/
#define EA_PER_PAGE 10                              //��ʾ�����б�ʱ��ҳ��С
#define EA_TMP_DIR "tmp"                            //��ʱ�ļ�Ŀ¼
static long edit_attach(char *fn){
    struct ea_attach_info ai[MAXATTACHMENTCOUNT];
    struct stat st;
    char buf[256],fn_tmp[64],ans[4],*filename;
    unsigned char loop,b_attach,u_sysop,changed;
    int fd,fd_origin,count,page,n,choice;
    long ret,size,offset;
    if(!fn||!*fn)
        return -1;
    clear();
    move(0,0);prints("\033[1;32m[��������]\033[m");
    if(stat(fn,&st)||!S_ISREG(st.st_mode)){         //��ȡ�ļ���Ϣ
        move(3,0);prints("\033[1;31m��ȡ�ļ���Ϣʱ��������...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -2;
    }
    sprintf(fn_tmp,"%s/edit_attach.%d",EA_TMP_DIR,(int)getpid());
    if((fd=open(fn_tmp,O_RDWR|O_CREAT|O_TRUNC,0644))==-1){
        move(3,0);prints("\033[1;31m������ʱ�ļ�ʱ��������...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -3;
    }
    flock(fd,LOCK_EX);
    if((fd_origin=open(fn,O_RDWR,0644))==-1){
        close(fd);unlink(fn_tmp);
        move(3,0);prints("\033[1;31m���ļ�ʱ��������...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -4;
    }
    flock(fd_origin,LOCK_SH);                       //��������
    ret=ea_dump(fd_origin,fd,0);                    //�����ļ�
    flock(fd_origin,LOCK_UN);                       //�������
    if(ret==-1){                                    //��������
        close(fd);close(fd_origin);unlink(fn_tmp);
        move(3,0);prints("\033[1;31m�����ļ�ʱ��������...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -5;
    }
    ret=ea_locate(fd,ai);                           //��ȡ������Ϣ
    if(ret==-1){
        close(fd);close(fd_origin);unlink(fn_tmp);
        move(3,0);prints("\033[1;31m��ȡ�ļ�ʱ��������...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -6;
    }
    offset=ret;                                     //������ʼλ��
    for(size=0,count=0;count<MAXATTACHMENTCOUNT&&ai[count].name[0];count++)
        size+=ai[count].size;
    b_attach=((currboard->flag&BOARD_ATTACH)?1:0);  //��ǰ�����Ƿ������ϴ�����
    u_sysop=((getCurrentUser()->userlevel&PERM_SYSOP)?1:0);
    if(!count&&!b_attach&&!u_sysop){
        close(fd);close(fd_origin);unlink(fn_tmp);
        move(3,0);prints("\033[1;33m��ǰ�����޸����ҵ�ǰ���治�����ϴ�����...\033[1;37m<Enter>\033[m");
        WAIT_RETURN;clear();
        return -7;
    }
    page=0;loop=1;changed=0;
    while(loop){                                    //��ѭ��
        move(3,0);clrtobot();
        if(!(count>EA_PER_PAGE))
            sprintf(buf,
                "\033[1;36m�� \033[1;37m%d\033[1;36m ����/�� \033[1;37m%ld\033[1;36m �ֽ�\033[m",
                count,size);
        else
            sprintf(buf,
                "\033[1;36m�� \033[1;37m%d\033[1;36m ����/�� \033[1;37m%ld\033[1;36m �ֽ�/�� \033[1;37m%d\033[1;36m ҳ\033[m",
                count,size,page+1);
        prints("\033[1;36m[��ǰ���¸����б�: %s\033[1;36m]\033[m",buf);
        for(n=0;n<EA_PER_PAGE&&(page*EA_PER_PAGE+n)<count;n++){
            sprintf(buf,"\033[1;37m[%02d]: %-60.60s %7dB\033[m",page*EA_PER_PAGE+n+1,
                ai[page*EA_PER_PAGE+n].name,ai[page*EA_PER_PAGE+n].size);
            move(5+n,0);prints("%s",buf);           //��ʾ��ǰ��ҳ�ĸ�����Ϣ
        }
        sprintf(buf,                                //������ʾ����ѡ��
            "\033[%d;33m(A)���� \033[%d;33m(D)ɾ�� \033[%d;33m(P|N)��ҳ \033[1;33m(E)���� \033[1;37m[E]: \033[m",
            (!(count<MAXATTACHMENTCOUNT&&(b_attach||u_sysop))?0:1),(!count?0:1),(!(count>EA_PER_PAGE)?0:1));
        getdata(6+n,0,buf,ans,2,DOECHO,NULL,true);
        switch(ans[0]){
            case 'a':                               //���Ӹ���
            case 'A':
                if(!(count<MAXATTACHMENTCOUNT&&(b_attach||u_sysop))){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;36m��ǰ�����޷����Ӹ���...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                if(chdir(EA_TMP_DIR)==-1){          //�л�����Ŀ¼����ʱĿ¼
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m�л�����ʱĿ¼ʧ��...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                filename=bbs_zrecvfile();           //�ϴ�����
                if(chdir(BBSHOME)==-1){             //�л�����Ŀ¼�� BBS ��Ŀ¼
                    close(fd);close(fd_origin);
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m��������: �л��� BBS ��Ŀ¼ʧ��...\033[m");
                    abort_bbs(0);                   //����
                }
                if(!filename||!*filename){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;33mȡ��...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                sprintf(buf,"%s/%s",EA_TMP_DIR,filename);
                *filename=0;                        //���һ����ʾ����
                if(stat(buf,&st)||!S_ISREG(st.st_mode)){
                    unlink(buf);
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m�����ļ��ϴ�ʧ��...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                if((size+st.st_size)>MAXATTACHMENTSIZE&&!u_sysop){
                    unlink(buf);
                    move(6+n,0);clrtoeol();
                    prints("\033[1;33m�ø���������ǰ���¸�����С����...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                ret=ea_append(fd,ai,buf,buf);
                if(ret==-2){
                    close(fd);close(fd_origin);unlink(fn_tmp);
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m��������: ��ȡ�ļ�ʧ��...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;clear();
                    return -8;
                }
                if(ret==-1||ret!=st.st_size){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31mд���ļ�ʱ��������...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                move(6+n,0);clrtoeol();
                prints("\033[1;32m���ظ��� %s �ɹ�!\033[1;37m<Enter>\033[m",ai[count].name);
                WAIT_RETURN;
                size+=ret;count++;
                changed++;
                break;
            case 'd':                               //ɾ������
            case 'D':
                if(!count){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;36m��ǰ�����޷�ɾ������...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                sprintf(buf,
                    "\033[1;33m��������ɾ���ĸ������ \033[1;31m[%02d]: \033[m",
                    count);                         //Ĭ��Ϊɾ�����һ������
                getdata(6+n,0,buf,ans,3,DOECHO,NULL,true);
                choice=(!ans[0]?count:atoi(ans));
                if(!(choice>0)||choice>count){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m����Ƿ�...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                sprintf(buf,
                    "\033[1;33mɾ�� \033[1;31m[%02d] %s \033[1;37m[N]: \033[m",
                    choice,ai[choice-1].name);
                getdata(6+n,0,buf,ans,2,DOECHO,NULL,true);
                if(!(ans[0]=='y'||ans[0]=='Y'))     //ȡ��
                    break;
                ret=ea_delete(fd,ai,choice);
                if(ret==-2){
                    close(fd);close(fd_origin);unlink(fn_tmp);
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31m��������: ��ȡ�ļ�ʧ��...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;clear();
                    return -9;
                }
                if(ret==-1){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;31mд���ļ�ʱ��������...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                move(6+n,0);clrtoeol();
                prints("\033[1;32mɾ���ɹ�!\033[1;37m<Enter>\033[m");
                WAIT_RETURN;
                size-=ret;count--;
                if(page>0&&!(count>page*EA_PER_PAGE))
                    page--;                         //�Զ�������ҳ
                changed++;
                break;
            case 'p':                               //��ǰ��ҳ
            case 'P':
                if(!page){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;36m��ǰλ���޷���ǰ��ҳ...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                page--;
                break;
            case 'n':                               //���ҳ
            case 'N':
                if(!(count>(page+1)*EA_PER_PAGE)){
                    move(6+n,0);clrtoeol();
                    prints("\033[1;36m��ǰλ���޷����ҳ...\033[1;37m<Enter>\033[m");
                    WAIT_RETURN;break;
                }
                page++;
                break;
            default:                                //��������
                loop=0;
                break;
        }
    }
    if(changed){
        flock(fd_origin,LOCK_EX);                   //��������
        ret=ea_dump(fd,fd_origin,0);                //��д
        if(ret==-1){
            close(fd);close(fd_origin);unlink(fn_tmp);
            move(8+n,0);prints("\033[1;31mд���ļ�ʱ��������...\033[1;37m<Enter>\033[m");
            WAIT_RETURN;clear();
            return -10;
        }
    }
    close(fd);close(fd_origin);unlink(fn_tmp);clear();
    return (!count?0:offset);
}
#undef EA_PER_PAGE
#undef EA_TMP_DIR

int write_file(char* filename,int saveheader,long* effsize,long* pattachpos, long attach_length, int add_loginfo)
{
    struct textline *p = firstline;
    FILE *fp=NULL;
    char abort[6];
    int aborted = 0;
    int temp;
    int do_edit_attach=0;
    long sign_size;
    int ret=0;
    extern char quote_title[120], quote_board[120];
    extern int Anony;
    int long_flag; //for long line check
#ifdef FILTER
    int filter = 0;
#endif

    char p_buf[100];

    set_alarm(0, 0, NULL, NULL);
    clear();

    if (uinfo.mode != CCUGOPHER) {
        /*
           if ( uinfo.mode == EDITUFILE||uinfo.mode==CSIE_ANNOUNCE||
           uinfo.mode == EDITSFILE)
           strcpy(p_buf,"(S)���浵��, (A)�����༭, (E)�����༭? [S]: " );
           else if ( uinfo.mode == SMAIL )
           strcpy(p_buf,"(S)�ĳ�, (A)ȡ��, or (E)�ٱ༭? [S]: " );
           else if ( local_article == 1 )
           strcpy(p_buf,"(L)��ת��, (S)ת��, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [L]: ");
           else
           strcpy(p_buf,"(S)ת��, (L)��ת��, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [S]: ");
         */
        if (uinfo.mode == POSTING) {
            /*strcpy(p_buf,"(S)����, (F)�Զ����з���, (A)ȡ��, (T)���ı��� or (E)�ٱ༭? [S]: "); */
            move(4, 0);         /* Haohmaru 99.07.17 */
#ifdef NEWSMTH
            prints("        ��ע�⣬��վ�涨��ͬ���������������һ���ڲ����ڱ�վ������\n"
                   "    ����ת�� 5 �λ� 5 �����ϣ�Υ���߽�������������ѻ�������Ȩ�ޡ�\n"
                   "    ��ϸ�涨����� sysop �澫������ˮľ�����ʺŹ���취����\n\n"
                   "        ���ҹ�ͬά�� BBS �Ļ�������ʡϵͳ��Դ��лл������");
#else
            prints
                ("��ע�⣺��վվ��涨��ͬ�����ݵ������Ͻ��� 5 (��)���������������ظ�������\n\nΥ���߳��������»ᱻɾ��֮�⣬��������������������µ�Ȩ������ϸ�涨����գ�\n\n    Announce ���վ�棺������ת�����������µĹ涨����\n\n���ҹ�ͬά�� BBS �Ļ�������ʡϵͳ��Դ��лл������\n\n");
#endif
        } else if (uinfo.mode == SMAIL)
            strcpy(p_buf, "(S)�ĳ�, (F)�Զ����мĳ�, (A)ȡ��, or (E)�ٱ༭? [S]: ");
        else if (uinfo.mode == IMAIL)
            sprintf(p_buf, "%s Internet �ż㣺(S)�ĳ�, (F)�Զ����мĳ�, (A)ȡ��, or (E)�ٱ༭? [S]: ", BBS_FULL_NAME);      /* Leeward 98.01.17 Prompt whom you are writing to */
        /*    sprintf(p_buf,"�� %s ���ţ�(S)�ĳ�, (F)�Զ����мĳ�, (A)ȡ��, or (E)�ٱ༭? [S]: ", lookupuser->userid ); 
           Leeward 98.01.17 Prompt whom you are writing to */
        else if(uinfo.mode==EDIT)
            sprintf(p_buf,"%s","(S)���浵��, (F)�Զ����з���, (A)�����༭, (E)�����༭, (C)��������? [S]: ");
            /*
             *  etnlegend, 2005.09.26, �༭����ʱ�ṩ���������ӿ�
             *  ���� C ������ڱ��������Ĵ�������ȫ��ͬ������ S �����
             *  �������µ��ı����ݵ��޸Ľ����沢�����޸ı��(��������˵Ļ�)
             *  ���ں���ĩβʱ���� edit_attach ������ʵ�ָ����Ĳ���
             */
        else if(uinfo.mode==POSTCROSS)
            sprintf(p_buf,"%s","(S)����༭ (F)�Զ����б���༭ (E)�����༭ (A)ȡ������ [S]: ");
            /* etnlegend, 2006.10.25, ת������ʱ�޸�����... ��ط��� READING ò�Ʋ�̫³��... */
        else
            strcpy(p_buf, "(S)���浵��, (F)�Զ����д洢, (A)�����༭, (E)�����༭? [S]: ");

        ret = valid_article(p_buf, abort);

        if((uinfo.mode==EDIT)&&(abort[0]=='c'||abort[0]=='C')){
            do_edit_attach=1;
            abort[0]=0;
        }
        if (abort[0] == '\0') {
            if (local_article == 0)
                abort[0] = 's';
            else
                abort[0] = 'l';
        }
        /* Removed by flyriver, 2002.7.1, no need to modify abort[0] here. */
        /*if(abort[0]!='T' && abort[0]!='t' && abort[0]!='F' && abort[0]!='f' && abort[0]!='A' && abort[0]!='a' &&abort[0]!='E' &&abort[0]!='e' && abort[0] != 'L' && abort[0] != 'l')
           abort[0]='s'; */
    } else
        abort[0] = 'a';

#ifdef FILTER
    if (((abort[0] != 'a')&&(abort[0] != 'e'))&&
        (uinfo.mode==EDIT)) {
    while (p != NULL) {
        if(check_badword_str(p->data, strlen(p->data), getSession())) {
            abort[0] = 'e';
            filter = 1;
            break;
        }
        p = p->next;
    }
    p = firstline;
    }
#endif
    if (abort[0] == 'a' || abort[0] == 'A') {
        struct stat stbuf;

        clear();
        if (uinfo.mode != CCUGOPHER) {
            prints("ȡ��...\n");
            refresh();
            sleep(1);
        }
        if (stat(filename, &stbuf) || stbuf.st_size == 0)
            unlink(filename);
        aborted = -1;
    } else if (abort[0] == 'e' || abort[0] == 'E') {
#ifdef FILTER
        if (filter) {
            clear();
            move (3, 0);
            prints ("\n\n            �ܱ�Ǹ�����Ŀ��ܺ��в��������ݣ������±༭...\n");
            pressreturn();
        }
#endif
        domsg();
        return KEEP_EDITING;
    } else if ( (abort[0] == 't' || abort[0] == 'T') && uinfo.mode == POSTING) {
        char buf[STRLEN];

        buf[0] = 0;             /* Leeward 98.08.14 */
        move(1, 0);
        prints("�ɱ���: %s", save_title);
		strcpy(buf,save_title);
        getdata(2, 0, "�±���: ", buf, STRLEN, DOECHO, NULL, 0);
        if (buf[0]!=0) {
            /* if (strcmp(save_title, buf))
                local_article = 0; */ // û��Ҫ�˰ɣ�����Ĭ���趨 - atppp
            strncpy(save_title, buf, STRLEN);
            strncpy(quote_title, buf, STRLEN);
        }
    } else if (abort[0] == 'f' || abort[0] == 'F') {
        // ����Ĭ�� - atppp
    } else if ((local_article == 2) && (abort[0] == 's' || abort[0] == 'S')) {
        local_article = 0;
    } else if ((local_article == 0) && (abort[0] != 'l' && abort[0] != 'L')) {
        // local_article = 0
    } else {                    /* Added by flyriver, 2002.7.1, local save */
        abort[0] = 'l';
        local_article = 1;
    }
    if (local_article == 2) local_article = 1; // ������һ�� 2 �� 1 �Ѿ�û�������ˣ���������ĳ���Ҫ�� 1��atppp
    firstline = NULL;
    if (!aborted) {
        if (pattachpos && *pattachpos) {
            char buf[MAXPATH];
            int fsrc,fdst;
            snprintf(buf,MAXPATH,"%s.attach",filename);
            if ((fsrc = open(filename, O_RDONLY)) >=0) {
                if ((fdst = open(buf, O_WRONLY|O_CREAT , 0600)) >= 0) {
                    char* src=(char*)malloc(10240);
                    long ret;
					long lsize = attach_length;
					long ndread;
                    lseek(fsrc,*pattachpos-1,SEEK_SET);
                    do {
						if( lsize > 10240 )
							ndread = 10240;
						else
							ndread = lsize;
                        ret = read(fsrc, src, ndread);
                        if (ret <= 0)
                            break;
						lsize -= ret;
                    } while (write(fdst, src, ret) > 0 && lsize > 0);
                    close(fdst);
                    free(src);
                }
                close(fsrc);
            }
        }
        if ((fp = fopen(filename, "w")) == NULL) {
            indigestion(5);
            abort_bbs(0);
        }
    /* ����ת�ű�� czz 020819 */
        if (saveheader) {
            if (local_article == 1)
                write_header(fp, getCurrentUser(), in_mail, quote_board, quote_title, Anony, 0, getSession());
            else
                write_header(fp, getCurrentUser(), in_mail, quote_board, quote_title, Anony, 2, getSession());
        }
    }
    if (effsize)
        *effsize=0;
    sign_size=0;
    temp=0;  /*�����Ƿ����ǩ����λ��*/
    while (p != NULL) {
        struct textline *v = p->next;

        if( (!aborted)&&(aborted!=-1)) {
//            long_flag = (strlen(p->data) >= WRAPMARGIN -2 );
            long_flag = 0;
            if (p->next != NULL || p->data[0] != '\0') {
                if (!strcmp(p->data,"--"))
                    temp=1;
                if (effsize) {
                    if (!strcmp(p->data,"--")) {
/*ע�⴦��
--
fsdfa
--
�����*/
                        *effsize+=sign_size;
                        sign_size=2;
                    } else if( strncmp(p->data,"\xa1\xbe \xd4\xda",5) && strncmp(p->data,": ",2)){
                    /*�������ǩ�����ָ���*/
                        if (sign_size!=0) /*�ڿ��ܵ�ǩ������*/
                            sign_size+=strlen(p->data);
                        else
                              *effsize+=strlen(p->data);
                    }
                }
                if (!temp&&(abort[0] == 'f' || abort[0] == 'F')) {       /* Leeward 98.07.27 ֧���Զ����� */
                    unsigned char *ppt = (unsigned char *) p->data;     /* ���д� */
                    unsigned char *pp = (unsigned char *) ppt;  /* ���� */
                    unsigned const int LINE_LEN=78;
                    unsigned int LLL = LINE_LEN;      /* ����λ�� */
                    unsigned char *ppx, cc;
                    int ich, lll;

                    while (strlen((char *) pp) > LLL) {
                        lll = 0;
                        ppx = pp;
                        ich = 0;
                        do {
                            if ((ppx = (unsigned char *) strstr((char *) ppx, "\033[")) != NULL) {
                                int ich=0;
                                if (ppx-pp-lll>LLL) break; //�Ѿ������㹻���е�λ�ã����ü�����\033
                                while (!isalpha(*(ppx+ich))&&(*(ppx+ich)!=0))
                                    ich++;
                                if (ich > 0)
                                    ich++;
                                else
                                    ich = 2;
                                lll += ich;
                                ppx += 2;
                                ich = 0;
                            }
                        } while (ppx); //������ɫ�ַ����ֽ�������lll
                        ppt += LLL + lll; //Ӧ�����еĵط�

                        if (ppt - pp > strlen((char *)pp)) break; //����λ�ó������ַ�������
                        if ((*ppt) > 127) {     /* �����ں����м����� ,KCN:������faint*/
                            for (ppx = ppt - 1, ich = 0; ppx >= pp; ppx--)
                                if ((*ppx) < 128)
                                    break;
                                else
                                    ich++;
                            if (ich % 2)
                                ppt--;
                        } else if (*ppt) {
                            for (ppx = ppt - 1, ich = 0; ppx >= pp; ppx--)
                                if ((*ppx) > 127 || ' ' == *ppx)
                                    break;
                                else
                                    ich++;
                            if (ppx > pp && ich < 16)
                                ppt -= ich;
                        }

                        cc = *ppt; //����һ���ַ�
                        *ppt = 0;
                        if (':' == p->data[0] && ':' != *pp)
                            fprintf(fp, ": ");
                        fprintf(fp, "%s", pp);
                        if (cc) //������
                            fprintf(fp, "\n");
                        *ppt = cc;
                        pp = ppt;
                        LLL = LINE_LEN;
                    }
                    LLL = LINE_LEN ;
                    if (':' == p->data[0] && ':' != *pp) //����ʣ����ַ�
                        fprintf(fp, ": ");
                    fprintf(fp, "%s", pp);
                    if (long_flag){	/* �����ǰ����ϵͳ�Զ��ضϵ�һ�������� */
                    	    LLL = LLL - strlen((char *)pp); //����һ�е������ȼ���
			}else{
			    fprintf(fp,"\n");
			}
                } else
                    fprintf(fp, "%s\n", p->data);
                }
            }
        free(p->data);
        free(p);
        p = v;
    }
    if (!aborted) {
		if(add_loginfo)
			bbsmain_add_loginfo(fp, getCurrentUser(), quote_board, Anony);
        fclose(fp);
        if (pattachpos && *pattachpos) {
            char buf[MAXPATH];
            struct stat st;
            snprintf(buf,MAXPATH,"%s.attach",filename);
            stat(filename,&st);
            *pattachpos=st.st_size+1;
            f_catfile(buf,filename);
            f_rm(buf);
				/*
            struct stat st;
            stat(filename,&st);
            *pattachpos=st.st_size+1;
            f_catfile(tmpattachfile,filename);
            f_rm(tmpattachfile);
			*/
        }
    }
    currline = NULL;
    lastline = NULL;
    firstline = NULL;
    if (abort[0] == 'l' || abort[0] == 'L' || local_article == 1) {
        sprintf(genbuf, "local_article = %u", local_article);
        bbslog("user","%s",genbuf);
        local_article = 0;
        if (aborted != -1)
            aborted = 1;        /* aborted = 1 means local save */
    }
    if ((uinfo.mode == POSTING) /* && strcmp(currboard->filename, "test") */) { /*Haohmaru.99.4.02.�ð���ˮ���˿�ȥ��//grin */
        if (ret)
            temp_numposts++;
        if (temp_numposts > 20)
            Net_Sleep((temp_numposts - 20) * 1 + 1);
    }
    if(do_edit_attach && askyn("\n\n�����б�״̬�� Ctrl+Q ������ʾ web ��ʽ�༭�����ĵ�ַ��\n��ȷ����ʹ������ zmodem ��ʽ�༭������", 0)){
        long offset;
        offset=edit_attach(filename);
        if(!(offset<0)&&pattachpos)
            *pattachpos=offset;
    }
    return aborted;
}

void keep_fail_post()
{
    char filename[STRLEN];
    char tmpbuf[30];
    struct textline *p = firstline;
    FILE *fp;

    sethomepath(tmpbuf, getCurrentUser()->userid);
    sprintf(filename, "%s/%s.deadve", tmpbuf, getCurrentUser()->userid);
    if ((fp = fopen(filename, "w")) == NULL) {
        indigestion(5);
        return;
    }
    while (p != NULL) {
        struct textline *v = p->next;

        if (p->next != NULL || p->data[0] != '\0')
            fprintf(fp, "%s\n", p->data);
        free(p->data);
        free(p);
        p = v;
    }
	fclose(fp);
    return;
}


/*Function Add by SmallPig*/
static int Origin(struct textline *text)
{
    char tmp[STRLEN];

    if (uinfo.mode != EDIT)
        return 0;
    if (!text)
        return 0;
    sprintf(tmp, "�� ��Դ:��%s ", BBS_FULL_NAME);
    if (strstr(text->data, tmp) && *text->data != ':')
        return 1;
    else
        return 0;
}

int vedit_process_ESC(arg)
    int arg;                    /* ESC + x */
{
    int ch2, action;

#define WHICH_ACTION_COLOR "(M)���鴦�� (I/E)��ȡ/д������� (C)ʹ�ò�ɫ (F/B/R)ǰ��/����/��ԭɫ��"
#define WHICH_ACTION_MONO  "(M)���鴦�� (I/E)��ȡ/д������� (C)ʹ�õ�ɫ (F/B/R)ǰ��/����/��ԭɫ��"

#define CHOOSE_MARK     "(0)ȡ����� (1)�趨��ͷ (2)�趨��β (3)���Ʊ������ "
#define FROM_WHICH_PAGE "��ȡ�������ڼ�ҳ? (0-7) [Ԥ��Ϊ 0]"
#define SAVE_ALL_TO     "����ƪ����д��������ڼ�ҳ? (0-7) [Ԥ��Ϊ 0]"
#define SAVE_PART_TO    "����ƪ����д��������ڼ�ҳ? (0-7) [Ԥ��Ϊ 0]"
#define FROM_WHICH_SIG  "ȡ��ǩ�����ڼ�ҳ? (0-7) [Ԥ��Ϊ 0]"
#define CHOOSE_FG       "ǰ����ɫ? 0)�� 1)�� 2)�� 3)�� 4)���� 5)�ۺ� 6)ǳ�� 7)�� "
#define CHOOSE_BG       "������ɫ? 0)�� 1)�� 2)�� 3)�� 4)���� 5)�ۺ� 6)ǳ�� 7)�� "
#define CHOOSE_ERROR    "ѡ�����"

    switch (arg) {
    case 'Z':
    case 'z':
        if (uinfo.mode == POSTING && (currboard->flag&BOARD_ATTACH || HAS_PERM(getCurrentUser(),PERM_SYSOP))) {
            struct ea_attach_info ai[MAXATTACHMENTCOUNT];
            int nUpload = 0;
            char ans[8];
            ans[0]='\0';
            nUpload = process_upload(nUpload, 15, ans, ai);
            while(1) {
                getdata(t_lines - 1, 0, "�����������", ans, 4, DOECHO, NULL, true);
                switch(ans[0]) {
                    case 'u':
                    case 'U':
                        nUpload = process_upload(nUpload, 15, ans, ai);
                        continue;
                    default:
                        break;
                }
                break;
            }
        }
        return 0;
    case 'A':
    case 'a':
        show_eof = !show_eof;
        return 0;
    case 'M':
    case 'm':
        ch2 = ask(CHOOSE_MARK);
        action = 'M';
        break;
    case 'I':
    case 'i':                  /* import */
        ch2 = ask(FROM_WHICH_PAGE);
        action = 'I';
        break;
    case 'E':
    case 'e':                  /* export */
        ch2 = ask(mark_on ? SAVE_PART_TO : SAVE_ALL_TO);
        action = 'E';
        break;
    case 'S':
    case 's':                  /* signature */
        ch2 = '0';
        action = 'S';
        break;
    case 'F':
    case 'f':
        ch2 = ask(CHOOSE_FG);
        action = 'F';
        break;
    case 'B':
    case 'b':
        ch2 = ask(CHOOSE_BG);
        action = 'B';
        break;
    case 'R':
    case 'r':
        ch2 = '0';              /* not used */
        action = 'R';
        break;
    case 'D':
    case 'd':
        ch2 = '4';
        action = 'M';
        break;
    case 'N':
    case 'n':
        ch2 = '0';
        action = 'N';
        break;
    case 'G':
    case 'g':
        ch2 = '1';
        action = 'G';
        break;
    case 'L':
    case 'l':
        ch2 = '0';              /* not used */
        action = 'L';
        break;
    case 'C':
    case 'c':
        ch2 = '0';              /* not used */
        action = 'C';
        break;
    case 'Q':
    case 'q':                  /* Leeward 98.07.30 Change hot key for msgX */
        marknum = 0;
        ch2 = '0';              /* not used */
        action = 'M';
        break;
    default:
        return 0;
    }

    if (strchr("IES", action) && (ch2 == '\n' || ch2 == '\r'))
        ch2 = '0';

    if (ch2 >= '0' && ch2 <= '7')
        return process_ESC_action(action, ch2);
    else {
        return ask(CHOOSE_ERROR);
    }

}

int mark_block()
{
    struct textline *p;
    int pass_mark = 0;

    first_mark_line = 0;
    if (mark_begin == NULL && mark_end == NULL)
        return 0;
    if (mark_begin == mark_end) {
        mark_begin->attr |= M_MARK;
        return 1;
    }
    if (mark_begin == NULL || mark_end == NULL) {
        if (mark_begin != NULL)
            mark_begin->attr |= M_MARK;
        else
            mark_end->attr |= M_MARK;
        return 1;
    } else {
        for (p = firstline; p != NULL; p = p->next) {
            if (p == mark_begin || p == mark_end) {
                pass_mark++;
                p->attr |= M_MARK;
                continue;
            }
            if (pass_mark == 1)
                p->attr |= M_MARK;
            else {
                first_mark_line++;
                p->attr &= ~(M_MARK);
            }
            if (pass_mark == 2)
                first_mark_line--;
        }
        return 1;
    }
}

void process_MARK_action(arg, msg)
    int arg;                    /* operation of MARK */
    char *msg;                  /* message to return */
{
    struct textline *p;
    int dele_1line;

    switch (arg) {
    case '0':                  /* cancel */
        for (p = firstline; p != NULL; p = p->next)
            p->attr &= ~(M_MARK);
        CLEAR_MARK();
        break;
    case '1':                  /* mark begin */
        mark_begin = currline;
        mark_on = mark_block();
        if (mark_on)
            strcpy(msg, "������趨���");
        else
            strcpy(msg, "���趨��ͷ���, ���޽�β���");
        break;
    case '2':                  /* mark end */
        mark_end = currline;
        mark_on = mark_block();
        if (mark_on)
            strcpy(msg, "������趨���");
        else
            strcpy(msg, "���趨��β���, ���޿�ͷ���");
        break;
    case '3':                  /* copy mark */
        if (mark_on && !(currline->attr & M_MARK)) {
            for (p = firstline; p != NULL; p = p->next) {
                if (p->attr & M_MARK) {
                    ve_insert_str(p->data);
                    split(currline, currpnt);
                }
            }
        } else
            bell();
        strcpy(msg, "��Ǹ������");
        break;
    case '4':                  /* delete mark */
        dele_1line = 0;
        if (mark_on && (currline->attr & M_MARK)) {
            if (currline == firstline)
                dele_1line = 1;
            else
                dele_1line = 2;
        }
        for (p = firstline; p != NULL; p = p->next) {
            if (p->attr & M_MARK) {
                currline = p;
                p=p->prev;
                vedit_key(Ctrl('Y'));
                if (p==NULL) {
                    p=firstline;
                    if (p==NULL);
                        break;
                }
            }
        }
        process_ESC_action('M', '0');
        marknum = 0;
        if (dele_1line == 0 || dele_1line == 2) {
            if (first_mark_line == 0)
                first_mark_line = 1;
            goline(first_mark_line);
        } else
            goline(1);
        break;
    default:
        strcpy(msg, CHOOSE_ERROR);
    }
    strcpy(msg, "\0");
}

static int process_ESC_action(int action, int arg)
/* valid action are I/E/S/B/F/R/C */
/* valid arg are    '0' - '7' */
{
    int newch = 0;
    char msg[80], buf[80];
    char filename[80];
    FILE *fp;

    msg[0] = '\0';
    switch (action) {
    case 'L':
        if (ismsgline >= 1) {
            ismsgline = 0;
            move(t_lines - 1, 0);
            clrtoeol();
        } else
            ismsgline = 1;
        break;
    case 'M':
        process_MARK_action(arg, msg);
        break;
    case 'I':
        sprintf(filename, "tmp/clip/%s.%c", getCurrentUser()->userid, arg);
        if ((fp = fopen(filename, "r")) != NULL) {
            insert_from_fp(fp,NULL);
            fclose(fp);
            sprintf(msg, "��ȡ���������� %c ҳ", arg);
        } else
            sprintf(msg, "�޷�ȡ���������� %c ҳ", arg);
        break;
    case 'G':
        go();
        break;
    case 'E':
        sprintf(filename, "tmp/clip/%s.%c", getCurrentUser()->userid, arg);
        if ((fp = fopen(filename, "w")) != NULL) {
            if (mark_on) {
                struct textline *p;

                for (p = firstline; p != NULL; p = p->next)
                    if (p->attr & M_MARK)
                        fprintf(fp, "%s\n", p->data);
            } else
                insert_to_fp(fp);
            fclose(fp);
            sprintf(msg, "�������������� %c ҳ", arg);
        } else
            sprintf(msg, "�޷������������� %c ҳ", arg);
        break;
    case 'N':
        searchline(searchtext);
        break;
    case 'S':
        search();
        break;
    case 'F':
        sprintf(buf, "%c[3%cm", 27, arg);
        ve_insert_str(buf);
        break;
    case 'B':
        sprintf(buf, "%c[4%cm", 27, arg);
        ve_insert_str(buf);
        break;
    case 'R':
        ve_insert_str(ANSI_RESET);
        break;
    case 'C':
        editansi = showansi = 1;
        clear();
        display_buffer();
        strcpy(msg, "����ʾ��ɫ�༭�ɹ��������лص�ɫģʽ");
    }

    if (msg[0] != '\0') {
        if (action == 'C') {    /* need redraw */
            move(t_lines - 2, 0);
            clrtoeol();
            prints("\033[1m%s%s%s\033[m", msg, ", �밴��������ر༭����...", ANSI_RESET);
            pressanykey();
            newch = '\0';
            editansi = showansi = 0;
        } else
            newch = ask(strcat(msg, "��������༭��"));
    } else
        newch = '\0';
    return newch;
}

void input_tools()
{
    char *msg =
        "�������빤��: 1.�Ӽ��˳�  2.һ������  3.���£ã�  4.����б�� ��[Q]";
    char *ansi1[7][10] = {
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"}
    };

    char *ansi2[7][10] = {
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
    };

    char *ansi3[7][10] = {
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "�J"},
        {"�Y", "�K", "�L", "�M", "�N", "�O", "�P", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"}
    };

    char *ansi4[7][10] = {
        {"�x", "�y", "�z", "�{", "�|", "�}", "�~", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "�u", "�v", "�w", "��"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"},
        {"��", "�I", "�J", "�K", "�L", "��", "��", "��", "��", "��"},
        {"��", "��", "��", "��", "��", "��", "�q", "�r", "�s", "�t"},
        {"��", "��", "��", "��", "��", "��", "��", "�H", "��", "�I"},
        {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"}
    };

    char buf[128], tmp[10];
    char *(*show)[10];
    int ch, i, page;

    move(t_lines - 1, 0);
    clrtoeol();
    prints("%s", msg);
    ch = igetkey();

    if (ch < '1' || ch > '4')
        return;

    switch (ch) {
    case '1':
        show = ansi1;
        break;
    case '2':
        show = ansi2;
        break;
    case '3':
        show = ansi3;
        break;
    case '4':
    default:
        show = ansi4;
        break;
    }

    page = 0;
    for (;;) {
        buf[0] = '\0';

        sprintf(buf, "��%dҳ:", page + 1);
        for (i = 0; i < 10; i++) {
            sprintf(tmp, "%d%s%s ", i, ".", show[page][i]);
            strcat(buf, tmp);
        }
        strcat(buf, "(P:��  N:��)[Q]\0");
        move(t_lines - 1, 0);
        clrtoeol();
        prints("%s", buf);
        ch = igetkey();

        if (tolower(ch) == 'p'||ch == KEY_UP) {
            if (page)
                page -= 1;
        } else if (tolower(ch) == 'n'||ch == KEY_DOWN) {
            if (page != 6)
                page += 1;
        } else if (ch < '0' || ch > '9') {
            buf[0] = '\0';
            break;
        } else {
            char *ptr = show[page][ch - '0'];
            while (*ptr) {
                insert_char(*ptr);
                ptr++;
            }
            break;
        }
        buf[0] = '\0';
    }
}
void vedit_key(int ch)
{
    int i;

#define NO_ANSI_MODIFY  if(no_touch) { warn++; break; }

    static int lastindent = -1;
    int no_touch, warn;

    if (ch == Ctrl('P') || ch == KEY_UP || ch == Ctrl('N') || ch == KEY_DOWN) {
        if (lastindent == -1)
            lastindent = currpnt;
    } else
        lastindent = -1;

    no_touch = (editansi && strchr(currline->data, '\033')) ? 1 : 0;
    warn = 0;


    if (ch < 0x100 && isprint2(ch)) {
        if (no_touch)
            warn++;
        else
            insert_char(ch);
    } else
        switch (ch) {
        case Ctrl('Z'):
            r_lastmsg();        /* Leeward 98.07.30 support msgX */
            break;
        case Ctrl('I'):
            NO_ANSI_MODIFY;
            do {
                insert_char(' ');
            } while ((currpnt & 0x7) && (currpnt <= MAX_EDIT_LINE));
            break;
        case '\r':
        case '\n':
            NO_ANSI_MODIFY;
            split(currline, currpnt);
            break;
        case Ctrl('G'):        /* redraw screen */
            go();
            break;
            /* Leeward 98.07.30 Change hot key for msgX */
            /*case Ctrl('Z'):  call help screen */
        case Ctrl('N'):
            show_eof = !show_eof;
            break;
        case Ctrl('P'):
            auto_newline = !auto_newline;
            break;
        case Ctrl('Q'):        /* call help screen */
            show_help("help/edithelp");
            break;
        case Ctrl('R'):
#ifdef CHINESE_CHARACTER
            SET_CHANGEDEFINE(getCurrentUser(), DEF_CHCHAR);
            break;
#endif            
        case KEY_LEFT:         /* backward character */
            if (currpnt > 0) {
                currpnt--;
            } else if (currline->prev) {
                curr_window_line--;
                currln--;
                currline = currline->prev;
                currpnt = currline->len;
            }
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j)
                    if (currpnt > 0)
                        currpnt--;
            }
#endif
            break;
            /*case Ctrl('Q'):  Leeward 98.07.30 Change hot key for msgX
               process_ESC_action('M', '0');
               marknum=0;            
               break; */
        case Ctrl('C'):
            process_ESC_action('M', '3');
            break;
        case Ctrl('U'):
            if (marknum == 0) {
                marknum = 1;
                process_ESC_action('M', '1');
            } else
                process_ESC_action('M', '2');
            clear();
            break;
        case KEY_RIGHT:        /* forward character */
            if (currline->len != currpnt) {
                currpnt++;
            } else if (currline->next) {
                currpnt = 0;
                curr_window_line++;
                currln++;
                currline = currline->next;
                if (Origin(currline)) {
                    curr_window_line--;
                    currln--;
                    currline = currline->prev;
                }
            }
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j)
                    if (currline->len != currpnt)
                        currpnt++;
            }
#endif
            break;
        case KEY_UP:           /* Previous line */
            if (currline->prev) {
                currln--;
                curr_window_line--;
                currline = currline->prev;
                currpnt = (currline->len > lastindent) ? lastindent : currline->len;
            }
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j)
                    if (currpnt > 0)
                        currpnt--;
            }
#endif
            break;
        case KEY_DOWN:         /* Next line */
            if (currline->next) {
                currline = currline->next;
                curr_window_line++;
                currln++;
                if (Origin(currline)) {
                    currln--;
                    curr_window_line--;
                    currline = currline->prev;
                }
                currpnt = (currline->len > lastindent) ? lastindent : currline->len;
            }
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j)
                    if (currpnt > 0)
                        currpnt--;
            }
#endif
            break;
        case Ctrl('B'):
        case KEY_PGUP:         /* previous page */
            top_of_win = back_line(top_of_win, 22);
            currline = back_line(currline, 22);
            currln -= moveln;
            curr_window_line = getlineno();
            if (currpnt > currline->len)
                currpnt = currline->len;
            break;
        case Ctrl('F'):
        case KEY_PGDN:         /* next page */
            top_of_win = forward_line(top_of_win, 22);
            currline = forward_line(currline, 22);
            currln += moveln;
            curr_window_line = getlineno();
            if (currpnt > currline->len)
                currpnt = currline->len;
            if (Origin(currline->prev)) {
                currln -= 2;
                curr_window_line = 0;
                currline = currline->prev->prev;
                top_of_win = lastline->prev->prev;
            }
            if (Origin(currline)) {
                currln--;
                curr_window_line--;
                currline = currline->prev;
            }
            break;
        case Ctrl('A'):
        case KEY_HOME:         /* begin of line */
            currpnt = 0;
            break;
        case Ctrl('E'):
        case KEY_END:          /* end of line */
            currpnt = currline->len;
            break;
        case Ctrl('S'):        /* start of file */
            top_of_win = firstline;
            currline = top_of_win;
            currpnt = 0;
            curr_window_line = 0;
            currln = 0;
            break;
        case Ctrl('T'):        /* tail of file */
            top_of_win = back_line(lastline, 22);
            countline();
            currln = moveln;
            currline = lastline;
            curr_window_line = getlineno();
            currpnt = 0;
            if (Origin(currline->prev)) {
                currline = currline->prev->prev;
                currln -= 2;
                curr_window_line -= 2;
            }
            break;
        case Ctrl('O'):
            input_tools();
            break;
        case KEY_INS:          /* Toggle insert/overwrite */
            insert_character = !insert_character;
            break;
        case Ctrl('H'):
        case '\177':           /* backspace */
            NO_ANSI_MODIFY;
            if (currpnt == 0) {
                struct textline *p;

                if (!currline->prev) {
                    break;
                }
                currln--;
                curr_window_line--;
                currline = currline->prev;
                currpnt = currline->len;

                /* Modified by cityhunter on 1999.10.22 */
                /* for the bug of edit two page article */

                if (curr_window_line < 0) {     /*top_of_win = top_of_win->next; */
                    top_of_win = currline;
                    curr_window_line = 0;
                }

                /* end of this modification             */
                if (*killsp(currline->next->data) == '\0') {
                    delete_line(currline->next);
                    break;
                }
                p = currline;
                while (!join(p)) {
                    p = p->next;
                    if (p == NULL) {
                        indigestion(2);
                        abort_bbs(0);
                    }
                }
                break;
            }
            currpnt--;
            delete_char();
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j) {
                    currpnt--;
                    delete_char();
                }
            }
#endif
            break;
        case Ctrl('D'):
        case KEY_DEL:          /* delete current character */
            NO_ANSI_MODIFY;
            if (currline->len == currpnt) {
                struct textline *p = currline;

                if (!Origin(currline->next)) {
                    while (!join(p)) {
                        p = p->next;
                        if (p == NULL) {
                            indigestion(2);
                            abort_bbs(0);
                        }
                    }
                } else if (currpnt == 0)
                    vedit_key(Ctrl('K'));
                break;
            }
#ifdef CHINESE_CHARACTER
            if (DEFINE(getCurrentUser(), DEF_CHCHAR)) {
                int i,j=0;
                for(i=0;i<currpnt+1;i++)
                    if(j) j=0;
                    else if(currline->data[i]<0) j=1;
                if(j)
                    delete_char();
            }
#endif
            delete_char();
            break;
        case Ctrl('Y'):        /* delete current line */
            /* STONGLY coupled with Ctrl-K */
            no_touch = 0;       /* ANSI_MODIFY hack */
            currpnt = 0;
            if (currline->next) {
                if (Origin(currline->next) && !currline->prev) {
                    currline->data[0] = '\0';
                    currline->len = 0;
                    break;
                }
            } else if (currline->prev != NULL) {
                currline->len = 0;
            } else {
                currline->len = 0;
                currline->data[0] = '\0';
                break;
            }
            currline->len = 0;
            vedit_key(Ctrl('K'));
            break;
        case Ctrl('K'):        /* delete to end of line */
            NO_ANSI_MODIFY;
            if (currline->prev == NULL && currline->next == NULL) {
                currline->data[0] = '\0';
                currpnt = 0;
                break;
            }
            if (currline->next) {
                if (Origin(currline->next) && currpnt == currline->len && currpnt != 0)
                    break;
                if (Origin(currline->next) && currline->prev == NULL) {
                    vedit_key(Ctrl('Y'));
                    break;
                }
            }
            if (currline->len == 0) {
                struct textline *p = currline->next;

                if (!p) {
                    p = currline->prev;
                    if (!p) {
                        break;
                    }
                    if (curr_window_line > 0)
                        curr_window_line--;
                    currln--;
                }
                if (currline == top_of_win)
                    top_of_win = p;
                delete_line(currline);
                currline = p;
                if (Origin(currline)) {
                    currline = currline->prev;
                    curr_window_line--;
                    currln--;
                }
                break;
            }
            if (currline->len == currpnt) {
                struct textline *p = currline;

                while (!join(p)) {
                    p = p->next;
                    if (p == NULL) {
                        indigestion(2);
                        abort_bbs(0);
                    }
                }
                break;
            }
            currline->len = currpnt;
            currline->data[currpnt] = '\0';
            break;
        default:
            break;
        }

    if (curr_window_line < 0) {
        curr_window_line = 0;
        if (!top_of_win->prev) {
            indigestion(6);
        } else
            top_of_win = top_of_win->prev;
    }
    if (curr_window_line >= t_lines - 1 || (outii!=-1&&curr_window_line>=outii)) {
        if (outii!=-1&&curr_window_line>=outii) i = curr_window_line - outii;
        else i = curr_window_line - t_lines + 1;
        for (; i >= 0; i--) {
            curr_window_line--;
            if (!top_of_win->next) {
                indigestion(7);
            } else
                top_of_win = top_of_win->next;
        }
    }
}

extern int icurrchar, ibufsize;

static int raw_vedit(char *filename,int saveheader,int headlines,long* eff_size,long* pattachpos,int add_loginfo)
{
    int newch, ch = 0, foo;
    struct textline *st_tmp, *st_tmp2;
    long attach_length;
    long ret;

    if (pattachpos != NULL) {
        ret=read_file(filename,&attach_length);
	    *pattachpos=ret;
    } else {
        // TODO: add zmodem upload
        ret=read_file(filename,NULL);
    }
    if (ret<0) return -1;
    top_of_win = firstline;
    if (headlines > 0) { /* 20050207 atppp */
        /* ����headlines ָ��������ͷ����Ϣ Luzi 1999/1/8 */
        for (newch = 0; newch < headlines; newch++)
            if (top_of_win->next)
                top_of_win = top_of_win->next;
        if (top_of_win == firstline) headlines = 0;
    }
    currline = top_of_win;
    st_tmp2 = firstline;
    st_tmp = currline->prev;    /* ��������ָ�룬���޸ı༭��һ�еĵ�ָ�� */
    currline->prev = NULL;
    firstline = currline;

    curr_window_line = 0;
    currln = 0;
    currpnt = 0;
    display_buffer();
    while (1) {
        newch = '\0';
        switch (ch) {
        case EOF:
        case Ctrl('W'):
        case Ctrl('X'):        /* Save and exit */
            if (headlines) {
                st_tmp->next = firstline;       /* �˳�ʱ��ָ�ԭ���� */
                firstline->prev = st_tmp;
                firstline = st_tmp2;
            }
            foo = write_file(filename, saveheader, eff_size,pattachpos,attach_length, add_loginfo);
            if (foo != KEEP_EDITING)
                return foo;
            if (headlines) {
                firstline = st_tmp->next;       /* �����༭���ٴ��޸ĵ�һ�е�ָ�� */
                firstline->prev = NULL;
            }
            break;
        case KEY_ESC:
            if (KEY_ESC_arg == KEY_ESC)
                insert_char(KEY_ESC);
            else {
                newch = vedit_process_ESC(KEY_ESC_arg);
                clear();
            }
            break;
        case KEY_REFRESH:
            break;
        default:
            vedit_key(ch);
        }
        if (newch==0&&ibufsize == icurrchar) {
            display_buffer();
            move(myy, myx);
        }

        ch = (newch != '\0') ? newch : igetkey();
    }
}

int vedit(char *filename,int saveheader,long* eff_size,long *pattachpos,int add_loginfo)
{
    int ans, t;
    long attachpos=0;
#ifdef NEW_HELP
	int oldhelpmode=helpmode;
#endif

    t = showansi;
    showansi = 0;
    ismsgline = (DEFINE(getCurrentUser(), DEF_EDITMSG)) ? 1 : 0;
    domsg();
#ifdef NEW_HELP
	helpmode = HELP_EDIT;
#endif
    ans = raw_vedit(filename, saveheader, 0,eff_size,pattachpos?pattachpos:&attachpos, add_loginfo);
#ifdef NEW_HELP
	helpmode = oldhelpmode;
#endif
    showansi = t;
    return ans;
}

int vedit_post(char *filename,int saveheader,long* eff_size,long* pattachpos)
{
    int ans, t;

    t = showansi;
    showansi = 0;
    ismsgline = (DEFINE(getCurrentUser(), DEF_EDITMSG)) ? 1 : 0;
    domsg();
    ans = raw_vedit(filename, saveheader, 4, eff_size,pattachpos, 0);   /*Haohmaru.99.5.5.Ӧ�ñ���һ������ */
    showansi = t;
    return ans;
}
