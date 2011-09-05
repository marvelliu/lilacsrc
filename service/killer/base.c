#define BBSMAIN
#include "bbs.h"

// 是否南开BBS
#undef NANKAI
#undef QNGY

#ifdef QNGY
// 是否自动转贴
#define AUTOPOST
#endif

#ifdef AUTOPOST
#define KILLERBOARD "Killer"
#endif // AUTOPOST

#ifndef NANKAI

#define K_HAS_PERM HAS_PERM
#define CURRENTUSER (getCurrentUser())
//#define CURRENTUSER (currentuser)
extern int kicked;

#define DEFAULT_SIGNAL talk_request

#define k_getdata getdata

#define k_attach_shm attach_shm

#define k_mail_file mail_file

int strlen2(char *s)
{
	int p;
	int inansi;
	p=0;
	inansi=0;
	while (*s)
	{
		if (*s=='\x1b')
		{
			inansi=1;
		}
		else
		if (inansi && *s=='m')
		{
			inansi=0;
		}
		else
		if (!inansi)
		{
			p++;
		}
		s++;
	}
	return p;
}

#else // NANKAI

#ifndef BBSPOST_MOVE
#define BBSPOST_MOVE -1
#endif // POSTFILE_MOVE

#define DEFAULT_SIGNAL SIG_DFL

#define k_attach_shm attach_shm2

#define K_HAS_PERM(x,y) HAS_PERM(y)
#define CURRENTUSER (&currentuser)
extern unsigned char scr_cols;

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

int kicked;

int k_getdata(int line,int col,char * prompt,char * buf,int len,int echo,void *nouse,int clearlabel)
{
	buf[0] = '\0';
	getdata(line, col, prompt, buf, len, echo, clearlabel);
}

int k_mail_file(char *fromid, char *tmpfile, char *userid, char *title, int unlinkmode, struct fileheader *fh)
{
	mail_file(tmpfile, userid, title);
	unlink(tmpfile);
}

#endif // NANKAI

#define k_setfcolor(x,y) kf_setfcolor(disp,x,y)

void kf_setfcolor(char * disp, int i,int j)
{
	char str[20];
	if (j)
		sprintf(str,"\x1b[%d;%dm",i+30,j);
	else
		sprintf(str,"\x1b[%dm",i+30);
	strcat(disp,str);
}

//#define k_resetcolor() strcat(disp,"\x1b[m")
#define k_resetcolor() kf_resetfcolor(disp)
void kf_resetfcolor(char * disp)
{
	char str[20];
	sprintf(str,"\x1b[m");
	strcat(disp,str);
}

