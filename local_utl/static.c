/* static.c : ��վ�˴�ͳ�� */
/*Modify By SmallPig*/
#include <time.h>
#include <stdio.h>
#define MAX_LINE        15
#define STATIC_COUNT_FILE "etc/posts/stat_count"
#include "bbs.h"
struct {
    int no[24];                 /* ���� */
    int sum[24];                /* �ܺ� */
} st;


int main(int argc,char* argv[])
{
    FILE *fp;
    char buf[256], *p;
    char date[80];
    time_t now;
    int hour, max = 0, item, total = 0;
    int totaltime = 0;
    int i, j;
    struct tm *date_tm;
    int fd;
    struct tm now_tm,last_tm;
    char *blk[10] = {
        /* ����̫�ѿ��� modified by Czz */
        "��", "��", "�x", "�y", "�z",
        "�{", "�|", "�}", "�~", "��",
        /* modified end */
    };

    struct {
    	int month_count[12]; /*ÿ��ͳ��*/
    	int day_count[31]; /*ÿ��ͳ��*/
    	time_t counttime; /*�ϴ�ͳ��ʱ��*/
    } total_st,old_st;
    if (init_all()) {
        printf("init data fail\n");
        return -1;
    }


    //����ԭ����ͳ������
    bzero(&total_st,sizeof(total_st));
    fd=open(STATIC_COUNT_FILE,O_RDONLY);
    if (fd!=-1) {
	read(fd,&total_st,sizeof(total_st));
    	close(fd);
    }
    if ((fp = fopen("usies", "r")) == NULL) {
        printf("cann't open usies\n");
        return 1;
    }

    now = time(0);
    date_tm = localtime(&now);
    sprintf(date, "%02u/%02u", date_tm->tm_mon + 1, date_tm->tm_mday);

    while (fgets(buf, 256, fp)) {
        hour = atoi(buf + 7);
        if (hour < 0 || hour > 23) {
            printf("%s", buf);
            continue;
        }
        if (strncmp(buf + 1, date, 5))
            continue;
        if (strstr(buf, "ENTER")) {
            st.no[hour]++;
            continue;
        }
        if ((p = (char *) strstr(buf + 40, "Stay:"))!=NULL) {
            st.sum[hour] += atoi(p + 6);
            continue;
        }
    }
    fclose(fp);

    for (i = 0; i < 24; i++) {
        total += st.no[i];
        totaltime += st.sum[i];
        if (st.no[i] > max)
            max = st.no[i];
    }

    item = max / MAX_LINE + 1;

    /*�ۼ���ǰ��ͳ������*/
    old_st=total_st;
    now_tm=*date_tm;
    if (total_st.counttime==0) {
    	/*��һ��ͳ��*/
    	total_st.day_count[now_tm.tm_mday-1]=total;
    	total_st.month_count[now_tm.tm_mon]=total;
    	total_st.counttime=now;
    } else {
      date_tm=localtime(&total_st.counttime);
      last_tm=*date_tm;
      if (last_tm.tm_year!=now_tm.tm_year) {
	      	/*����һ����*/
           bzero(&total_st,sizeof(total_st));
           total_st.month_count[now_tm.tm_mon]=total;
      }
      else if (last_tm.tm_mon!=now_tm.tm_mon) {
      /*	���·��ˣ����ÿ��ͳ��*/
          int i;
          for (i=0;i<31;i++)
          	total_st.day_count[i]=0;
      }
      if ((last_tm.tm_mday==now_tm.tm_mday)&&(last_tm.tm_mon==now_tm.tm_mon)) {
      	/*������ִ�У���ô����ͳ�Ƽ�ȥ�ϴμӵģ�������ε�*/
           total_st.month_count[date_tm->tm_mon]=total_st.month_count[date_tm->tm_mon]-
           	total_st.day_count[date_tm->tm_mday-1]+total;
      	}
      total_st.day_count[now_tm.tm_mday-1]=total;
      total_st.counttime=now;
    }

    /*�����ۼ�����*/
    substitute_record(STATIC_COUNT_FILE,&total_st,sizeof(total_st),1);
    if (now_tm.tm_hour==23) {
    	/*����12����ʾ�ۼ�����*/
	    if ((fp = fopen("0Announce/bbslists/count", "w")) == NULL) {
	        printf("Cann't open count\n");
	    }
	    /*��ʾͳ������*/
	    if (fp!=NULL) {
	    	int count,total,total2,count2;
	    	int i;
	        fprintf(fp, "\n\x1b[1;36m                  %s��վ����ͳ��\n",BBS_FULL_NAME);
/*��ͳ��*/
	        fprintf(fp, "\n\x1b[1;36m                  ÿ��ͳ������\n");
	       
                fprintf(fp,"\n\x1b[m        1        2        3        4        5        6| ��      �� ƽ     ��\n");
		fprintf(fp,"------------------------------------------------------+---------------------\n");
		count=0;
		total=0;
		for (i=0;i<6;i++) {
			if (total_st.month_count[i]!=0) {
				total+=total_st.month_count[i];
				count++;
			}
			fprintf(fp,"%9d",total_st.month_count[i]);
	        }
		fprintf(fp,"|%11d %9d\n",total,count==0?0:total/count);
		total2=total;
		count2=count;
		count=0;
		total=0;
		
		fprintf(fp,"\n        7        8        9       10       11       12| ��      �� ƽ     ��\n");
		fprintf(fp,"------------------------------------------------------+---------------------\n");
		for (i=6;i<12;i++) {
			if (total_st.month_count[i]!=0) {
				total+=total_st.month_count[i];
				count++;
			}
			fprintf(fp,"%9d",total_st.month_count[i]);
	        }
	        fprintf(fp,"|%11d %9d\n",total,count==0?0:total/count);

                fprintf(fp,"����: %d\nÿ����ƽ��: %d\n",
			total+total2,count+count2==0?0:(total+total2)/(count+count2));
	        fprintf(fp, "\n\x1b[1;36m                  ����ÿ��ͳ������\x1b[m\n");
		fprintf(fp,"\n      1      2      3      4      5      6      7      8      9     10     11");
		fprintf(fp,"\n-----------------------------------------------------------------------------\n");
                for (i=0;i<11;i++) {
		    if (total_st.day_count[i]!=0) {
                        total+=total_st.day_count[i];
		        count++;
		    }
		    fprintf(fp,"%7d",total_st.day_count[i]);
		}
		fprintf(fp,"\n\n     12     13     14     15     16     17     18     19     20     21     22");
		fprintf(fp,"\n-----------------------------------------------------------------------------\n");
                for (i=11;i<22;i++) {
		    if (total_st.day_count[i]!=0) {
                        total+=total_st.day_count[i];
		        count++;
		    }
		    fprintf(fp,"%7d",total_st.day_count[i]);
		}
		fprintf(fp,"\n\n     23     24     25     26     27     28     29     30     31");
		fprintf(fp,"\n-----------------------------------------------------------------------------\n");
                for (i=22;i<31;i++) {
		    if (total_st.day_count[i]!=0) {
                        total+=total_st.day_count[i];
		        count++;
		    }
		    fprintf(fp,"%7d",total_st.day_count[i]);
		}
                fprintf(fp,"\n����: %d\nÿ��ƽ��: %d\n",
			total,count==0?0:total/count);
		fclose(fp);
	    } else {
		 post_file(NULL, "", "0Announce/bbslists/countlogins", "BBSLists", "�޷����ļ�", 0, 1, getSession());
	    }
    }
    
   /*��ʾ���ڵ�����*/
    if ((fp = fopen("0Announce/bbslists/countlogins", "w")) == NULL) {
        printf("Cann't open countlogins\n");
        return 1;
    }

    fprintf(fp, "\n\x1b[36m    ������������������������ ���� \033[01m\033[37m1000\033[00m\033[36m ������ʾǧλ���� ������������������������\n");
    for (i = max / item + 1; i >= 0; i--) {
        if ((i) * item >= 10000) {
            fprintf(fp, "\x1b[34m%-6d\x1b[33m", (i) * item);
        } else {
            fprintf(fp, "\x1b[34m%4d\x1b[36m��\x1b[33m", (i) * item);
        }
        for (j = 0; j < 24; j++) {
            if ((item * (i) > st.no[j]) && (item * (i - 1) <= st.no[j]) && st.no[j]) {
                /* ÿһǧ��һ����ɫ Czz 020507 */
                if (st.no[j] >= 5000)
                    /*fprintf(fp, "\x1b[35m###\x1b[33m"); Leeward 97.12.08 */
                    fprintf(fp, "\033[1m\x1b[36m%-3d\033[m\x1b[33m", (st.no[j]) % 1000);
                else if (st.no[j] >= 4000)
                    fprintf(fp, "\033[1m\x1b[35m%-3d\033[m\x1b[33m", (st.no[j]) % 1000);
                else if (st.no[j] >= 3000)
                    fprintf(fp, "\033[1m\x1b[34m%-3d\033[m\x1b[33m", (st.no[j]) % 1000);
                else if (st.no[j] >= 2000)
                    fprintf(fp, "\033[1m\x1b[33m%-3d\033[m\x1b[33m", (st.no[j]) % 1000);
                else if (st.no[j] >= 1000)
                    fprintf(fp, "\033[1m\x1b[32m%-3d\033[m\x1b[33m", (st.no[j]) % 1000);
                else
                    fprintf(fp, "\x1b[31m%-3d\x1b[33m", (st.no[j]));
                continue;
            }
            if (st.no[j] - item * i < item && item * i < st.no[j])
                fprintf(fp, "%s ", blk[((st.no[j] - item * i) * 10) / item]);
            else if (st.no[j] - item * i >= item)
                fprintf(fp, "%s ", blk[9]);
            else
                fprintf(fp, "   ");
        }
        fprintf(fp, "\x1b[36m��\n");
    }
    fprintf(fp, "   \x1b[36m" " ��������\x1b[37m   %s  ��վ�˴α�   \x1b[36m������\x1b[37m%s\x1b[36m������\n"
/*"    \x1b[34m  0  1  2  3  4  5  6  7  8  9  10 11 \x1b[31m12 13 14 15 16 17 18 19 20 21 22 23 \n\n" *//* Leeward 98.02.27 */
            "    \x1b[34m  1  2  3  4  5  6  7  8  9  10 11 12 \x1b[31m13 14 15 16 17 18 19 20 21 22 23 24\n\n" "               \x1b[36m 1 \x1b[33m�~\x1b[36m = \x1b[37m%-5d \x1b[36m�ܹ���վ�˴Σ�\x1b[37m%-9d\x1b[36mƽ��ʹ��ʱ�䣺\x1b[37m%d\x1b[m \n\n", 
            BBS_FULL_NAME, Ctime(now), item, total, total==0?0:(totaltime/total+1));    /* Leeward 98.09.24 add the 2nd \n for SHARE MEM in ../main.c */

    fclose(fp);

    if (now_tm.tm_hour==23) {
    	sprintf(buf,"%d��%2d��%2d������ͳ��",now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday);
    	post_file(NULL, "", "0Announce/bbslists/countlogins", "BBSLists", buf, 0, 1, getSession());
    	sprintf(buf,"%d��%2d��%2d���ۼ�����ͳ��",now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday);
	post_file(NULL, "", "0Announce/bbslists/count", "BBSLists", buf, 0, 1, getSession());
    }
    return 0;
}

