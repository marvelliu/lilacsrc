#include "bbs.h"

int do_userlist(struct user_info *uentp, char *arg, int t)
{
    char user_info_str[256 /*STRLEN*2 */ ], pagec;
	char modebuf[80],idlebuf[10];

    t++;
    if (!uentp->active || !uentp->pid) {
        printf(" %4d ��,�Ҹ���\n", t);
        return 0;
    }
    pagec = ' ';
    sprintf(user_info_str,
             /*---	modified by period	2000-10-21	�����û������Դ���1000��
                     " %3d%2s%s%-12.12s%s%s %-16.16s%s %-16.16s %c %c %s%-17.17s\033[m%5.5s\n",
             ---*/
            " %4d%2s%-12.12s %-16.16s %-16.16s %c %c %s%-11.11s\033[m%5.5s %d\n", 
            t, uentp->invisible ? "��" : "��", uentp->userid, uentp->username, uentp->from, pagec, ' ',
            uentp->invisible?(uentp->pid==1?"\033[33m":"\033[32m"):(uentp->pid==1?"\033[1;36m":""),
            modestring(modebuf,uentp->mode, uentp->destuid, 0,  /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                       (uentp->in_chat ? uentp->chatid : NULL)), idle_str(idlebuf,uentp), uentp->pid);
    printf("%s", user_info_str);
    return COUNT;
}

int main(argc, argv)
    int argc;
    char *argv[];
{
    if (init_all()) {
        printf("init data fail\n");
        return -1;
    }

    printf(" ���  �û�ID       �ǳ�             ��Դ                 ״̬       ����  PID\n");
    apply_ulist_addr((APPLY_UTMP_FUNC)do_userlist, NULL);

	return 0;
}
