/*****
stiger:    2004.2

�������ʹ���ʵ�SysTrace

crontab:  2 0 * * * /home/bbs/bin/bonlinelog

******/


#include <time.h>
#include <stdio.h>
#include "bbs.h"

int main(int argc, char **argv){
	char path[256];
	char title[256];
	struct stat st;
	time_t now;
	struct tm t;
	int before=0;

chdir(BBSHOME);

if(argc > 1) before=atoi(argv[1]);

	now=time(0)-86400*before;
	localtime_r( &now, &t);

    if (init_all()) {
        printf("init data fail\n");
        return -1;
    }

sprintf(path, "%s/%d/%d/%d_boarduse.visit", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1, t.tm_mday);

if( stat( path, &st) >= 0){
    sprintf(title, "%d��%2d��%2d�հ���ʹ������(��������)", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    post_file(NULL, "", path, "SysTrace", title, 0, 1, getSession());
}

sprintf(path, "%s/%d/%d/%d_boarduse.total", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1, t.tm_mday);

if( stat( path, &st) >= 0){
    sprintf(title, "%d��%2d��%2d�հ���ʹ������(��ʱ������)", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    post_file(NULL, "", path, "SysTrace", title, 0, 1, getSession());
}

    return 0;
}
