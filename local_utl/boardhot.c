/*
 * Copyright (C) 2008 by zyh
 * zhangyh07@gmain.com
 * 
 * 该程序用于统计每个版内的热门话题
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include "bbs.h"

#define TOPFILE "TOP.php"	//存放统计结果的php文件
#define TOPPATH "boards/"   //该文件的路径
#define HASHSIZE 257
#define TOPCOUNT 5		//热门话题的最大数目
#define DAY 30			//统计天数，1表示当天的热门话题

struct boardtop
{
	char boardcname[STRLEN];
	char boardename[STRLEN];
	char title[ARTICLE_TITLE_LEN];	//热点文章的标题
	unsigned int id;
	unsigned int groupid;
	int number;			//回复数
};

struct postrec_old 
{
	struct posttop pt;
	struct postrec_old *next; 
}*bucket[HASHSIZE], *toptmp[TOPCOUNT];

struct boardtop top[TOPCOUNT];		//存储版内的热点话题
struct fileheader fh[1];
struct boardheader bh[1];

int hash (unsigned int key)
{
	return key % HASHSIZE;
}

/*
 * 将帖子的信息加入hash表中
 */
void search (struct fileheader *f)
{
	int key;
	struct postrec_old *p, *q, *tmp;

	key = hash (f->groupid);
	p = bucket[key];
	tmp = p;

	/*寻找hash表中是否已经有同groupid的帖子*/
	while (p != NULL && p->pt.groupid != f->groupid)
	{
		tmp = p;
		p = p->next;
	}

	if (p == NULL)
	{
		/*将帖子加入到hash表中并设置回复数为1*/
		q = (struct postrec_old*)malloc (sizeof (struct postrec_old));

		q->pt.groupid = f->groupid;
		q->pt.number = 1;
		if (bucket[key] == NULL)
			bucket[key] = q;
		else
			tmp->next = q;

		q->next = NULL;
	}
	else
		(p->pt.number)++;	//回复数加1

	return;
}

/*
 * 用新帖子替换toptmp中回复数最少的帖子，
 * 并返回新的回复数最少的帖子的下标。
 */
int change (int min, struct postrec_old *t)
{
	int i;
	int num, tmp = 0;

	toptmp[min] = t;	//替换

	/*
	 * 找出替换后的回复数最少的帖子，
	 * 如果有回复数相同的情况，根据帖子的groupid(可以反映原帖发表的先后顺序)
	 * 确定发帖时间靠前的帖子较小。(留在toptmp中的为较新的帖子)
	 */
	num = toptmp[0]->pt.number;
	for (i = 0; i < TOPCOUNT; i++)
	{
		if (num > toptmp[i]->pt.number)
		{
			num = toptmp[i]->pt.number;
			tmp = i;
		}
		else if (num == toptmp[i]->pt.number)
		{
			if (toptmp[tmp]->pt.groupid > toptmp[i]->pt.groupid)
				tmp = i;
		}
	}

	return tmp;
}

/*
 * qsort的比较函数
 */
int cmp (const void *p1, const void *p2)
{
	struct posttop *t1, *t2;

	t1 = (struct posttop*)p1;
	t2 = (struct posttop*)p2;

	return t2->number - t1->number;
}

/*
 * 找出hash表中的热点话题
 */
int sort (void)
{
	int i;
	int full = 0;
	int num = 0;
	int min = 0;	//始终为toptmp中回复数最少的帖子的下标
	struct postrec_old *tmp;

	for (i = 0; i < HASHSIZE; i++)
	{
		tmp = bucket[i];
		while (tmp != NULL)
		{
			/*将遇到的帖子都装入toptmp中直到装满*/
			if (!full)
			{
				toptmp[num++] = tmp;
				min = (tmp->pt.number < toptmp[min]->pt.number) ? num-1 : min;
				if (num == TOPCOUNT)
					full = 1;
			}
			else
			{
				if (tmp->pt.number >= toptmp[min]->pt.number)
					min = change (min, tmp);	//用新帖子替换toptmp中回复数最少的帖子
			}
			tmp = tmp->next;
		}
	}

	if (full)
		num = TOPCOUNT;

	/*将信息写入top中*/
	for (i = 0; i < num; i++)
	{
		top[i].groupid = toptmp[i]->pt.groupid;
		top[i].id = top[i].groupid;
		top[i].number = toptmp[i]->pt.number;
	}

	qsort (top, num, sizeof (struct posttop), cmp);

	return num;
}

/*
 * 生成热点话题的函数
 */
int gen_board_hot (const char *filename)
{
	FILE *fp;
	int i, head;
	time_t now;

	time (&now);

	for (i = 0; i < HASHSIZE; i++)
		bucket[i] = NULL;

	if ((fp = fopen (filename, "r")) == NULL)
		return 0;

	/*将DAY天内的帖子信息加入hash表中*/
	head = fseek (fp, -sizeof (struct fileheader), SEEK_END);
	while (!head)
	{
		fread (fh, sizeof (struct fileheader), 1, fp);
		
		/*判断发帖时间是否符合要求*/
		if ((now - get_posttime (fh)) / (60*60*24) > DAY)
			break;

		search (fh);	//将帖子的信息加入hash表中

		head = fseek (fp, -2*sizeof (struct fileheader), SEEK_CUR);
	}
	fclose (fp);

	i = sort();		//找出hash表中的热点话题

	return i;
}

/*
 * 清空hash表
 */
void freebucket (void)
{
	int i;
	struct postrec_old *p, *tmp;

	for (i = 0; i < HASHSIZE; i++)
	{
		p = bucket[i];
		while (p != NULL)
		{
			tmp = p->next;
			free (p);
			p = tmp;
		}
	}

	return;
}

/*
 * 从后向前根据id找到相应的帖子
 */
void get_article_from_id (const char *filename, int num)
{
	int i, j = 0;	//j表示已经找到的帖子数量
	int head;	//用来判断是否到文件头部
	FILE *fp;

	if ((fp = fopen (filename, "r")) == NULL)
		return;

	head = fseek (fp, -sizeof (struct fileheader), SEEK_END);
	while (!head)
    {
		fread (fh, sizeof (struct fileheader), 1, fp);
		for (i = 0; i < num; i++)
		{
			if (fh->id == top[i].id)
			{
				strcpy (top[i].title, fh->title);
				j++;
			}
		}
		if (j == num)
			break;
		head = fseek (fp, -2*sizeof (struct fileheader), SEEK_CUR);
	}

	fclose (fp);

	return;
}

/*
 * 向php文件写入统计结果的格式
 */
void write_hot (FILE **fp, int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		fprintf (*fp, "array(\n");
		fprintf (*fp, "'bardcname' => '%s'\n", top[i].boardcname);
		fprintf (*fp, "'boardename' => '%s'\n", top[i].boardename);
		fprintf (*fp, "'title' => '%s'\n", top[i].title);
		fprintf (*fp, "'id' => '%d'\n", top[i].id);
		fprintf (*fp, "'gid' => '%d'\n", top[i].groupid);
		fprintf (*fp, "),\n");
	}

	return;
}

/*
 * main()函数读取版面的信息，然后生成每个版面的热点话题的id，
 * 根据话题的id找到该话题的标题，最后将生成结果写入文件中。
 */
int main (void)
{
	int i;
	int num;		//热点话题的实际数量(可能少于5个)
	char buf[40] = "boards";
	char curfile[80];
	FILE *fp, *hfp;
	char *tmp;

	chdir (BBSHOME);
	sprintf (curfile, ".BOARDS"); 
	if ((fp = fopen (curfile, "r")) == NULL)
		return 0;

	sprintf (curfile, "%s%s", TOPPATH, TOPFILE);
	if ((hfp = fopen (curfile, "w")) == NULL)
		return 0;

	fprintf (hfp, "<?php\nreturn array(\n");

	while (fread (bh, sizeof (struct boardheader), 1, fp) && bh->filename[0] != '\0')
	{
		sprintf (curfile, "%s/%s/.DIR", buf, bh->filename);

		num = gen_board_hot (curfile);		//生成热点话题的id
		freebucket ();

		if (num)
		{
			tmp = strtok (bh->title, " \t\n");
			tmp = strtok (NULL, " \t\n");
			for (i = 0; i < num; i++)
			{
				strcpy (top[i].boardename, bh->filename);
				strcpy (top[i].boardcname, tmp);
			}
		}

		sprintf (curfile, "%s/%s/.ORIGIN", buf, bh->filename);

		get_article_from_id (curfile, num);		//根据id找到相应的帖子
		write_hot (&hfp, num);					//将结果写入文件中
	}

	fprintf (hfp, ");\n?>\n");

	fclose (fp);
	fclose (hfp);

	return 0;
}
