#ifndef __LILAC_SEARCH_H__
#define __LILAC_SEARCH_H__

#define MAX_KEEP 100
#define MAX_CACHE 64

typedef struct {
	char id[24];
	char keyword[40];
	int  day;
}MY_QUERY;

typedef struct {
	char id[14];
	char title[ARTICLE_TITLE_LEN];
	char date[12];
	char board[32];
	char filename[FILENAME_LEN];
}MY_RESULT;

typedef struct {
	int spos;
	int sbid;
	int epos;
	int ebid;
}POS_CACHE;


int do_search(int forward);
#endif
