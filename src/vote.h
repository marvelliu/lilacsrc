#ifndef _VOTE_H
#define _VOTE_H

#define VOTE_YN         (1)
#define VOTE_SINGLE     (2)
#define VOTE_MULTI      (3)
#define VOTE_VALUE      (4)
#define VOTE_ASKING     (5)

struct ballot {
    char uid[IDLEN+2];            /* ͶƱ��       */
    unsigned int voted;         /* ͶƱ������   */
    char msg[3][STRLEN];        /* ��������     */
};

struct votebal {
    char userid[IDLEN + 1];
    char title[STRLEN];
    char type;
    char items[32][38];
    int maxdays;
    int maxtkt;
    int totalitems;
    time_t opendate;
};

/*
struct votebal
{
        char            userid[IDLEN+1];
        char            title[STRLEN];
        char            type;
        char            items[32][38];
        int             maxdays;
        int             maxtkt;
        int             totalitems;
        time_t          opendate;
        int             stay;
        int             day;
        unsigned int    numlogins;
        unsigned int    numposts;
}
;
*/
struct votelimit {              /*Haohmaru.99.11.17.���ݰ���������������ж��Ƿ��ø�ʹ����ͶƱ */
    int stay;
    int day;
    unsigned int numlogins;
    unsigned int numposts;
};

char *vote_type[] = { "�Ƿ�", "��ѡ", "��ѡ", "����", "�ʴ�" };

#endif
