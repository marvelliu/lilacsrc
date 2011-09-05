#ifndef _KBSBBS_ANN_H_
#define _KBSBBS_ANN_H_

/* etnlegend, 2006.10.14, �������ײ��������... */

#define MAXITEMS        1024
#define PATHLEN         256
#define MTITLE_LEN      128
#define ITITLE_LEN      88

typedef struct _ann_item{
    char    title[ITITLE_LEN];
    char    fname[STRLEN];
    char   *host;
    int     port;
    long    attachpos;
}
ITEM;

typedef struct _ann_menu{
    ITEM  **p_item[MAXITEMS];       /* �ɼ� ITEM ָ���б� */
    ITEM   *pool[MAXITEMS];         /* ȫ�� ITEM �б� */
    char    mtitle[MTITLE_LEN];     /* ��ǰ .Names �������� */
    char   *path;                   /* ��ǰ .Names �ļ�·�� */
    void   *nowmenu;
    void   *father;
    int     num;                    /* ��ǰ .Names �п���ʾ�� ITEM ����, ��Ӧ�� `item[]` ���� */
    int     page;
    int     now;                    /* ��ǰλ�� */
    int     level;
    int     total;                  /* ��ǰ .Names ������ ITEM ����, ��Ӧ�� `pool[]` ���� */
    time_t  modified_time;          /* ���� .Names ʱ�ļ���ʱ��� */
#ifdef ANN_COUNT
    int     count;
#endif /* ANN_COUNT */
}
MENU;

#define I_FREE(i)                                           \
    do{                                                     \
        if(i){                                              \
            free((i)->host);                                \
            free(i);                                        \
        }                                                   \
    }while(0)
#define I_ALLOC()       ((ITEM*)calloc(1,sizeof(ITEM)))

/* ȡ�˵�(m)�����(n)λ�õ���Ŀ */
#define M_ITEM(m,n)     (*((m)->p_item[n]))

enum ANN_SORT_MODE{
    ANN_SORT_BY_FILENAME,
    ANN_SORT_BY_TITLE,
    ANN_SORT_BY_BM,
    ANN_SORT_BY_FILENAME_R,
    ANN_SORT_BY_TITLE_R,
    ANN_SORT_BY_BM_R,
    ANN_SORT_UNKNOWN
};

#endif /* _KBSBBS_ANN_H_ */

