
/*  �µ�blog����ͳ��
    pig2532   2006-8
*/

#include <stdio.h>
#include <sys/stat.h>
#include "bbs.h"

#ifdef PERSONAL_CORP

struct blogstat {
    long uid; // �û����
    long visitcount; // �ܷ�����
    long postcount; // �ܷ�����
    long lastvisit; // ��ֹ�����ܷ�����
    long lastpost; // ��ֹ�����ܷ�����
    int visitstat[30]; // ���30�������
    int poststat[30]; // ���30�췢����
    long visit30; // ���30���ܷ�����
    long post30; // ���30���ܷ�����
    float scoreA; // A����֣��ܷ�����
    float scoreB; // B����֣�30�������
    float scoreC; // C����֣�30�췢����
    float scoreD; // D����֣�����ʱ��
    float score; // �ܻ���
};

void get_stat_data(char *statstr, long *lastnum, int *statnums) {
    int i;
    char *ph, *pch;
    
    ph = statstr;
    pch = strchr(statstr, '/');
    if(pch == NULL)
        return;
    *pch = '\0';
    *lastnum = atol(ph);
    for(i=0; i<30; i++) {
        ph = pch + 1;
        pch = strchr(ph, '/');
        if(pch == NULL)
            return;
        *pch = '\0';
        statnums[i] = atoi(ph);
    }
    return; 
}

void make_stat_data(char *statstr, long lastnum, int *statnums) {
    int i;
    char tmp[10];

    sprintf(statstr, "%ld/", lastnum);
    for(i=1; i<30; i++) {
        sprintf(tmp, "%d/", statnums[i]);
        strcat(statstr, tmp);
    }
    sprintf(tmp, "%d/", statnums[0]);
    strcat(statstr, tmp);
}

int compare_visitcount(const void *v1, const void *v2) {
    return ((struct blogstat *)v2)->visitcount - ((struct blogstat *)v1)->visitcount;
}

int compare_visit30(const void *v1, const void *v2) {
    return ((struct blogstat *)v2)->visit30 - ((struct blogstat *)v1)->visit30;
}

int compare_post30(const void *v1, const void *v2) {
    return ((struct blogstat *)v2)->post30 - ((struct blogstat *)v1)->post30;
}

int main(int argc, char *argv[]) {
    MYSQL s;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024], tmp1[256], tmp2[256];
    int usercount, i, j;
    struct blogstat *statdata = NULL;
    long now_timestamp;

    chdir(BBSHOME);
    setuid(BBSUID);
    setgid(BBSGID);

    mysql_init(&s);
    if(!my_connect_mysql_blog(&s)) {
        mysql_report_error(&s);
        return 0;
    }

    /* ����ͳ������ */
    strcpy(query, "DROP TABLE `statbak_yesterday`");
    mysql_real_query(&s, query, strlen(query));
    strcpy(query, "RENAME TABLE `statbak_today` to `statbak_yesterday`");
    mysql_real_query(&s, query, strlen(query));
    strcpy(query, "CREATE TABLE `statbak_today` SELECT `uid`,`visitstat`,`poststat`,`score` FROM `users`");
    mysql_real_query(&s, query, strlen(query));
    
    /* ��ȡ�û����� */
    strcpy(query, "SELECT COUNT(*) FROM `users`");
    if(mysql_real_query(&s, query, strlen(query))) {
        mysql_report_error(&s);
        mysql_close(&s);
        return 0;
    }
    res = mysql_store_result(&s);
    row = mysql_fetch_row(res);
    usercount = 0;
    if(row != NULL)
        usercount = atoi(row[0]);
    mysql_free_result(res);
    
    /* ��ȡͳ������ */
    statdata = (struct blogstat *)malloc(sizeof(struct blogstat) * usercount);
    if(statdata == NULL) {
        mysql_close(&s);
        goto finish;
    }
    strcpy(query, "SELECT `uid`,`visitcount`,`nodescount`,`visitstat`,`poststat`,`score` FROM `users`");
    if(mysql_real_query(&s, query, strlen(query))) {
        mysql_report_error(&s);
        mysql_close(&s);
        goto finish;
    }
    res = mysql_store_result(&s);

    for(i=0; i<usercount; i++) {
        row = mysql_fetch_row(res);
        if(row == NULL) {
            usercount = i;
            break;
        }
        bzero(&statdata[i], sizeof(struct blogstat));
        statdata[i].uid = atol(row[0]);
        statdata[i].visitcount = atol(row[1]);
        statdata[i].postcount = atol(row[2]);
        get_stat_data(row[3], &statdata[i].lastvisit, statdata[i].visitstat);
        get_stat_data(row[4], &statdata[i].lastpost, statdata[i].poststat);
        statdata[i].score = atoi(row[5]);
    }
    mysql_free_result(res);
    if(usercount == 0) {
        mysql_close(&s);
        goto finish;
    }
    
    /* ����ͳ������ */
    for(i=0; i<usercount; i++) {
        statdata[i].visitstat[0] = statdata[i].visitcount - statdata[i].lastvisit;
        statdata[i].poststat[0] = statdata[i].postcount - statdata[i].lastpost;
        statdata[i].lastvisit = statdata[i].visitcount;
        statdata[i].lastpost = statdata[i].postcount;
        statdata[i].visit30 = 0;
        for(j=0; j<30; j++)
            statdata[i].visit30 += statdata[i].visitstat[j];
        statdata[i].post30 = 0;
        for(j=0; j<30; j++)
            statdata[i].post30 += statdata[i].poststat[j];
    }
    
    /* ����A����� */
    qsort(statdata, usercount, sizeof(struct blogstat), compare_visitcount); 
    for(i=0; i<usercount; i++)
        statdata[i].scoreA = statdata[i].visitcount * 60.0 / statdata[0].visitcount;

    /* ����B����� */
    qsort(statdata, usercount, sizeof(struct blogstat), compare_visit30);
    for(i=0; i<usercount; i++)
        statdata[i].scoreB = (statdata[0].visit30 == 0)?0:(statdata[i].visit30 * 60.0 / statdata[0].visit30);

    /* ����C����� */
    qsort(statdata, usercount, sizeof(struct blogstat), compare_post30);
    for(i=0; i<usercount; i++)
        statdata[i].scoreC = (statdata[0].post30 == 0)?0:(statdata[i].post30 * 20.0 / statdata[0].post30);

    /* ����D����� */
    sprintf(query, "SELECT UNIX_TIMESTAMP()");
    if(mysql_real_query(&s, query, strlen(query))) {
        mysql_report_error(&s);
        mysql_close(&s);
        goto finish;
    }
    res = mysql_store_result(&s);
    row = mysql_fetch_row(res);
    if(row == NULL) {
        mysql_free_result(res);
        mysql_close(&s);
        goto finish;
    }
    now_timestamp = atol(row[0]);
    mysql_free_result(res);
    for(i=0; i<usercount; i++) {
        sprintf(query, "SELECT UNIX_TIMESTAMP(`created`) FROM `nodes` WHERE `uid`=%ld ORDER BY `created` DESC LIMIT 0,1", statdata[i].uid);
        if(mysql_real_query(&s, query, strlen(query))) {
            mysql_report_error(&s);
            mysql_close(&s);
            goto finish;
        }
        res = mysql_store_result(&s);
        row = mysql_fetch_row(res);
        if(row == NULL)
            statdata[i].scoreD = 0;
        else {
            statdata[i].scoreD = 30 - (now_timestamp - atol(row[0])) / 86400;
            if(statdata[i].scoreD < 0)
                statdata[i].scoreD = 0;
        }
        mysql_free_result(res);
    }

    /* �����д�����ݿ� */
    for(i=0; i<usercount; i++) {
        statdata[i].score = statdata[i].scoreA + statdata[i].scoreB + statdata[i].scoreC + statdata[i].scoreD;
        make_stat_data(tmp1, statdata[i].lastvisit, statdata[i].visitstat);
        make_stat_data(tmp2, statdata[i].lastpost, statdata[i].poststat);
        sprintf(query, "UPDATE `users` SET `visitstat`='%s',`poststat`='%s',`score`=%f WHERE `uid`=%ld", tmp1, tmp2, statdata[i].score, statdata[i].uid);
        mysql_real_query(&s, query, strlen(query));
    }
    
    mysql_close(&s);

finish:
    if(statdata)
        free(statdata);
    return 0;
}

#else /* PERSONAL_CORP */

int main() {
    return 0;
}

#endif /* PERSONAL_CORP */

