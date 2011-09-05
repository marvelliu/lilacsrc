
/********��2003��1��21�յİ���ṹת��1��21���Ժ�İ���ṹ,
  ע�⵽2003��2��27������ת�䣬��cnv_bh2.c
  ********/

#include "bbs.h"
#include "stdio.h"

struct boardheader1 {            /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char unused[STRLEN - BM_LEN];
    char BM[BM_LEN - 1];
    char unused_flag;
    char title[STRLEN];
    unsigned level;
    unsigned int nowid;
    unsigned int clubnum; /*����Ǿ��ֲ�������Ǿ��ֲ����*/
    unsigned int flag;
};

struct boardheader2{
        char filename[STRLEN];
        char BM[BM_LEN];
        char title[STRLEN];
        unsigned level;
        unsigned int nowid;
        unsigned int clubnum;
        unsigned int flag;
        unsigned int adv_club;
        char des[200];
};

int main(int argc , char* argv[])
{
        FILE * fp,*fp2;
        struct boardheader1 bh[MAXBOARD];
        struct boardheader2 bhnew[MAXBOARD];

        int i;

	if(argc !=3 )
	{
		printf("usage: convert Old_BOARDS_FILE New_BOARDS_FILE\n");
		exit(0);
	}
        if((fp = fopen(argv[1],"r")) == NULL)
        {
                printf("open .BOARDS file failed!");
                exit(0);
        }

        if((fp2 = fopen(argv[2],"w")) == NULL)
        {
                printf("cant create newboards file!");
                exit(0);
        }

        memset((void*)bh,0,MAXBOARD*sizeof(struct boardheader1));
        memset((void*)bhnew,0,MAXBOARD*sizeof(struct boardheader2));

        fread((void*)bh,sizeof(struct boardheader1),MAXBOARD,fp);

        for(i = 0;i < MAXBOARD; i++)
        {
                strncpy(bhnew[i].filename,bh[i].filename,STRLEN);
                strncpy(bhnew[i].BM,bh[i].BM,BM_LEN-1);
                //if(bh[i].title[0] == '2')bh[i].title[0] = 'H';
                //else if(bh[i].title[0] > '2' && bh[i].title[0] <= '9')
                //       bh[i].title[0] -=1;
                strncpy(bhnew[i].title,bh[i].title,STRLEN);
                bhnew[i].level = bh[i].level;
                bhnew[i].nowid = bh[i].nowid;
                bhnew[i].clubnum = bh[i].clubnum;
                bhnew[i].flag = bh[i].flag;
        }

        fwrite(bhnew,sizeof(struct boardheader2),MAXBOARD,fp2);

        fclose(fp2);
        fclose(fp);
        return 0;
}

