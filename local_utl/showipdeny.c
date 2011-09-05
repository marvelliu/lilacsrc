#include "bbs.h"

struct userec deliveruser;

int main(int argc, char **argv)
{
    FILE *fp, *fp2;
    char fn[80],fn2[80];
    char id[IDLEN+2];
    char * p;
    int i,ip[4],t,count=0;
    long j;
    time_t tt;

    if (init_all()) {
        printf("init data fail\n");
        return -1;
    }

    strcpy(fn, ".IPdenys");
    strcpy(fn2, "tmp/showipdeny.txt");
    fp=fopen(fn, "r");
    if(fp) {
        fp2=fopen(fn2, "w");
        while(!feof(fp)) {
            if(fscanf(fp, "%d %ld %d.%d.%d.%d %d", &i, &j, &ip[0], &ip[1], &ip[2], &ip[3], &t)<=0) break;
            if(t<=10) continue;
            count++;
            tt=(time_t) j;
            p = ctime(&tt);
            p[19]=0; p+=4;
            if(t>=100000)
                fprintf(fp2, "%s ���� %d.%d.%d.%d �ѱ��Զ����һСʱ.\n", p, ip[0],ip[1],ip[2],ip[3]);
            else
            if(i==0)
                fprintf(fp2, "%s ���� %d.%d.%d.%d ��������ʱ��̫��.һСʱ�ڹ�����%d��.\n", p, ip[0],ip[1],ip[2],ip[3],t+1);
            else
                fprintf(fp2, "%s ���� %d.%d.%d.%d ���ӹ���Ƶ��.    һСʱ�ڹ�����%d��.\n", p, ip[0],ip[1],ip[2],ip[3],t);
        }
        fclose(fp);
        fclose(fp2);
        unlink(fn);
        if(count==0) {
            unlink(fn2);
        }
        else {
            bzero(&deliveruser, sizeof(struct userec));
            strcpy(deliveruser.userid, "deliver");
            deliveruser.userlevel = -1;
            strcpy(deliveruser.username, "�Զ�����ϵͳ");
            setCurrentUser( &deliveruser);
            strcpy(getSession()->fromhost, "����");
            tt = time(0);
            p = ctime(&tt);
            p[19]=0; p+=4;
            sprintf(fn, "%s �� IP �����¼", p);
            post_file(&deliveruser, NULL, fn2, "surrender", fn, 0, 1,getSession());
            unlink(fn2);
        }
    }

    strcpy(fn, ".IDdenys");
    strcpy(fn2, "tmp/showiddeny.txt");
    fp=fopen(fn, "r");
    if(fp) {
        count=0;
        fp2=fopen(fn2, "w");
        while(!feof(fp)) {
            if(fscanf(fp, "%d %ld %s %d", &i, &j, id, &t)<=0) break;
            if(t<=5) continue;
            count++;
            tt=(time_t) j;
            p = ctime(&tt);
            p[19]=0; p+=4;
            if(i==0)
                fprintf(fp2, "%s %s ��������ʱ��̫��.һСʱ�ڹ�����%d��.\n", p, id,t+1);
            else
                fprintf(fp2, "%s %s ���ӹ���Ƶ��.    һСʱ�ڹ�����%d��.\n", p, id,t);
        }
        fclose(fp);
        fclose(fp2);
        unlink(fn);
        if(count==0) {
            unlink(fn2);
        }
        else {
            bzero(&deliveruser, sizeof(struct userec));
            strcpy(deliveruser.userid, "deliver");
            deliveruser.userlevel = -1;
            strcpy(deliveruser.username, "�Զ�����ϵͳ");
            setCurrentUser(&deliveruser);
            strcpy(getSession()->fromhost, "����");
            tt = time(0);
            p = ctime(&tt);
            p[19]=0; p+=4;
            sprintf(fn, "%s �� ID �����¼", p);
            post_file(&deliveruser, NULL, fn2, "surrender", fn, 0, 1, getSession());
            unlink(fn2);
        }
    }
    return 0;
}
