/* etnlegend, 2006.04.17, �û�Ȩ�޹����� */

/*
 * ������ʽ:        ulevel [ �û�ѡ�� ] [ ����ѡ�� ] [ ����ѡ�� ] [ ����ѡ�� ]
 *
 * �û�ѡ��:
 *   -a             ��������Ϊ�����û�, ��ѡ���� -u ѡ���, Ĭ������
 *   -u <user>      ��������Ϊ�û���Ϊ <user> ���û�, ��ѡ���� -a ѡ���, Ĭ�ϲ�����
 *
 * ����ѡ��:
 *   -p <perm>      ����Ȩ�޼��, ��������ΪȨ������ <perm> ָ�����û�, Ĭ�ϲ�����
 *                  <perm> ��һ��Ȩ�޷��ŵļ���, ���ַ� `&` ��ʼʱ��ʾ����Ȩ��֮�����߼����ϵ,
 *                  ��������������Ȩ�޶�����Ҫ��, Ĭ������¸���Ȩ��֮ǰ���߼����ϵ
 *   -t <tnum>      ������ݼ��, ��������Ϊ������� <tnum> ָ�����û�, Ĭ�ϲ�����
 *                  <tnum> �� `0 ~ 255` ֮�������, ��ʾ������
 *
 * ����ѡ��:
 *   -A <perm>      Ϊ��ǰ������������Ȩ�� <perm>, ��ѡ���� -S ѡ���, Ĭ�ϲ�����
 *   -D <perm>      Ϊ��ǰ���������Ƴ�Ȩ�� <perm>, ��ѡ���� -S ѡ���, Ĭ�ϲ�����
 *   -S <perm>      ����ǰ��������Ȩ������Ϊ <perm>, ��ѡ���� -A ѡ��� -D ѡ���, Ĭ�ϲ�����
 *   -T <tnum>      ����ǰ���������������Ϊ <tnum>, Ĭ�ϲ�����
 *
 * ����ѡ��:
 *   -h             ��ʾ�ĵ���Ϣ
 *
*/

#include "bbs.h"

#define UB_LEN      128                             /* ���������� */
#define UU_USR      0x0001                          /* �û�ѡ�� -u ���ñ�ǩ */
#define UC_PRM      0x0002                          /* ����ѡ�� -p ���ñ�ǩ */
#define UC_TTL      0x0004                          /* ����ѡ�� -t ���ñ�ǩ */
#define UO_ADD      0x0008                          /* ����ѡ�� -A ���ñ�ǩ */
#define UO_DEL      0x0010                          /* ����ѡ�� -D ���ñ�ǩ */
#define UO_SET      0x0020                          /* ����ѡ�� -S ���ñ�ǩ */
#define UO_TTL      0x0040                          /* ����ѡ�� -T ���ñ�ǩ */
#define UO_MSK      (UO_ADD|UO_DEL|UO_SET|UO_TTL)   /* ����ѡ������ */
#define UE_AND      0x0080                          /* ����ѡ�� -p ����ʱ���߼���ǩ */
#define UE_RVS      0x0100                          /* ����ѡ�� -A �� -D ͬʱ����ʱ��˳���ǩ */
#define UE_BTH      0x0200                          /* �û�ѡ�� -u �� -a ͬʱ�����ǵļ���ǩ */

static const char* const perms=XPERMSTR;            /* Ȩ�޷����б� */
static unsigned int flag;                           /* ѡ���ǩ */
static char user[IDLEN+1];                          /* �趨 -u ѡ��ʱ���û������� */
static unsigned int perm;                           /* �趨 -p ѡ��ʱ��Ȩ�޲��� */
static unsigned int tnum;                           /* �趨 -t ѡ��ʱ����ݲ��� */
static unsigned int permA;                          /* �趨 -A ѡ��ʱ��Ȩ�޲��� */
static unsigned int permD;                          /* �趨 -D ѡ��ʱ��Ȩ�޲��� */
static unsigned int permS;                          /* �趨 -S ѡ��ʱ��Ȩ�޲��� */
static unsigned int tnumT;                          /* �趨 -T ѡ��ʱ����ݲ��� */

/* ����ԭ�� */
#ifndef __nonnull
#define __nonnull(param)
#endif /* __nonnull */
static inline int u_usage(void);
static inline unsigned int u_gen_perm(const char*) __nonnull((1));
static inline unsigned int u_gen_title(const char*) __nonnull((1));
static inline int u_check_perm(const struct userec*) __nonnull((1));
static inline int u_check_title(const struct userec*) __nonnull((1));
static inline int u_add_perm(struct userec*) __nonnull((1));
static inline int u_del_perm(struct userec*) __nonnull((1));
static inline int u_set_perm(struct userec*) __nonnull((1));
static inline int u_set_title(struct userec*) __nonnull((1));
static inline int u_process_condition(const struct userec*) __nonnull((1));
static inline int u_process_operation(struct userec*) __nonnull((1));
static inline const char* u_gen_perms(unsigned int perm);
static inline int u_show_current(const struct userec*) __nonnull((1));
static inline int u_show_change(const struct userec*,unsigned int,unsigned int) __nonnull((1));
static inline int u_process(struct userec*,void*) __nonnull((1));

/* ���� -h ѡ�� */
static inline int u_usage(void){
    fprintf(stdout,"\n%s\n%s\n%s\n%s\n%s\n",
        "  ������ʽ:        ulevel [ �û�ѡ�� ] [ ����ѡ�� ] [ ����ѡ�� ] [ ����ѡ�� ]\n",
        "  �û�ѡ��:\n"
        "    -a             ��������Ϊ�����û�, ��ѡ���� -u ѡ���, Ĭ������\n"
        "    -u <user>      ��������Ϊ�û���Ϊ <user> ���û�, ��ѡ���� -a ѡ���, Ĭ�ϲ�����\n",
        "  ����ѡ��:\n"
        "    -p <perm>      ����Ȩ�޼��, ��������ΪȨ������ <perm> ָ�����û�, Ĭ�ϲ�����\n"
        "                   <perm> ��һ��Ȩ�޷��ŵļ���, ���ַ� `&` ��ʼʱ��ʾ����Ȩ��֮�����߼����ϵ,\n"
        "                   ��������������Ȩ�޶�����Ҫ��, Ĭ������¸���Ȩ��֮ǰ���߼����ϵ\n"
        "    -t <tnum>      ������ݼ��, ��������Ϊ������� <tnum> ָ�����û�, Ĭ�ϲ�����\n"
        "                   <tnum> �� `0 ~ 255` ֮�������, ��ʾ������\n",
        "  ����ѡ��:\n"
        "    -A <perm>      Ϊ��ǰ������������Ȩ�� <perm>, ��ѡ���� -S ѡ���, Ĭ�ϲ�����\n"
        "    -D <perm>      Ϊ��ǰ���������Ƴ�Ȩ�� <perm>, ��ѡ���� -S ѡ���, Ĭ�ϲ�����\n"
        "    -S <perm>      ����ǰ��������Ȩ������Ϊ <perm>, ��ѡ���� -A ѡ��� -D ѡ���, Ĭ�ϲ�����\n"
        "    -T <tnum>      ����ǰ���������������Ϊ <tnum>, Ĭ�ϲ�����\n",
        "  ����ѡ��:\n"
        "    -h             ��ʾ�ĵ���Ϣ\n");
    return 0;
}

/* ����Ȩ�޲��� */
static inline unsigned int u_gen_perm(const char *s){
    const char *p,*r;
    unsigned int ret;
    for(p=s,ret=0;*p;p++){
        if(!(r=strchr(perms,(*p))))
            continue;
        ret|=(1<<(r-perms));
    }
    return ret;
}

/* ������ݲ��� */
static inline unsigned int u_gen_title(const char *s){
    const char *p;
    unsigned int ret;
    for(p=s,ret=0;*p;p++)
        if(!isdigit(*p))
            return 0x80000100;
    return (!((ret=atoi(s))&~0xFF)?(ret):(0x80000200));
}

/* ���� -p ѡ�� */
static inline int u_check_perm(const struct userec *user){
    return (!(flag&UC_PRM)?(1):(!(flag&UE_AND)?(user->userlevel&perm):((user->userlevel&perm)==perm)));
}

/* ���� -t ѡ�� */
static inline int u_check_title(const struct userec *user){
    return (!(flag&UC_TTL)?(1):(user->title==tnum));
}

/* ���� -A ѡ�� */
static inline int u_add_perm(struct userec *user){
    if(flag&UO_ADD)
        user->userlevel|=permA;
    return 0;
}

/* ���� -D ѡ�� */
static inline int u_del_perm(struct userec *user){
    if(flag&UO_DEL)
        user->userlevel&=~permD;
    return 0;
}

/* ���� -S ѡ�� */
static inline int u_set_perm(struct userec *user){
    if(flag&UO_SET)
        user->userlevel=permS;
    return 0;
}

/* ���� -T ѡ�� */
static inline int u_set_title(struct userec *user){
    if(flag&UO_TTL)
        user->title=tnumT;
    return 0;
}

/* �������� */
static inline int u_process_condition(const struct userec *user){
    return (u_check_perm(user)&&u_check_title(user));
}

/* ������� */
static inline int u_process_operation(struct userec *user){
    if(!(flag&UE_RVS)){
        u_add_perm(user);
        u_del_perm(user);
    }
    else{
        u_del_perm(user);
        u_add_perm(user);
    }
    u_set_perm(user);
    u_set_title(user);
    return 0;
}

/* ����Ȩ���ַ��� */
static inline const char* u_gen_perms(unsigned int perm){
    static char buf[NUMPERMS+1];
    int i;
    for(i=0;i<NUMPERMS;i++)
        buf[i]=(!(perm&(1<<i))?('-'):perms[i]);
    buf[i]=0;
    return ((const char*)buf);
}

/* ���������ǰ */
static inline int u_show_current(const struct userec *user){
    fprintf(stdout,"%-12.12s  <%03u>  %s\n",user->userid,user->title,u_gen_perms(user->userlevel));
    return 0;
}

/* ����������� */
static inline int u_show_change(const struct userec *user,unsigned int save_perm,unsigned int save_tnum){
    fprintf(stdout,"%-12.12s  <%03u>  %s  ",user->userid,save_tnum,u_gen_perms(save_perm));
    fprintf(stdout,"=>  <%03u>  %s\n",user->title,u_gen_perms(user->userlevel));
    return 0;
}

/* ����һ�û� */
static inline int u_process(struct userec *user,void *arg){
    unsigned int save_perm;
    unsigned int save_tnum;
    if(!(user->userid[0])||!u_process_condition(user))
        return 0;
    if(!(flag&UO_MSK))
        u_show_current(user);
    else{
        save_perm=user->userlevel;
        save_tnum=user->title;
        u_process_operation(user);
        u_show_change(user,save_perm,save_tnum);
    }
    return 1;
}

/* ���������� */
int main(int argc,char **argv){
#define UE_QUIT(s) do{u_usage();fprintf(stderr,"error: %s\n",s);exit(__LINE__);}while(0)
    struct userec *p_user;
    int ret;
    opterr=0;
    while((ret=getopt(argc,argv,"au:p:t:A:D:S:T:h"))!=-1){
        switch(ret){
            case 'a':
                if(flag&(UE_BTH|UU_USR))
                    UE_QUIT("options conflict or overlap ...");
                flag|=UE_BTH;
                break;
            case 'u':
                if(flag&(UE_BTH|UU_USR))
                    UE_QUIT("options conflict or overlap ...");
                flag|=UU_USR;
                snprintf(user,IDLEN+1,"%s",optarg);
                break;
            case 'p':
                if(flag&UC_PRM)
                    UE_QUIT("options conflict or overlap ...");
                flag|=UC_PRM;
                (*optarg=='&')?(flag|=UE_AND):(flag&=~UE_AND);
                perm=u_gen_perm(optarg);
                break;
            case 't':
                if(flag&UC_TTL)
                    UE_QUIT("options conflict or overlap ...");
                flag|=UC_TTL;
                tnum=u_gen_title(optarg);
                if(tnum&~0xFF)
                    UE_QUIT("invalid title number ...");
                break;
            case 'A':
                if(flag&UO_ADD)
                    UE_QUIT("options conflict or overlap ...");
                flag|=UO_ADD;
                permA=u_gen_perm(optarg);
                (flag&UO_DEL)?(flag|=UE_RVS):(flag&=~UE_RVS);
                break;
            case 'D':
                if(flag&UO_DEL)
                    UE_QUIT("options conflict or overlap ...");
                flag|=UO_DEL;
                permD=u_gen_perm(optarg);
                (flag&UO_ADD)?(flag&=~UE_RVS):(flag|=UE_RVS);
                break;
            case 'S':
                if(flag&(UO_ADD|UO_DEL|UO_SET))
                    UE_QUIT("options conflict or overlap ...");
                flag|=UO_SET;
                permS=u_gen_perm(optarg);
                break;
            case 'T':
                if(flag&UO_TTL)
                    UE_QUIT("options conflict or overlap ...");
                flag|=UO_TTL;
                tnumT=u_gen_title(optarg);
                if(tnumT&~0xFF)
                    UE_QUIT("invalid title number ...");
                break;
            case 'h':
                u_usage();
                return 0;;
            default:
                UE_QUIT("unknown usage of options ...");
        }
    }
    if(optind!=argc)
        UE_QUIT("useless parameters ...");
    if(chdir(BBSHOME)==-1)
        UE_QUIT("change directory to BBSHOME ...");
    resolve_ucache();
    if(flag&UU_USR){
        if(!getuser(user,&p_user))
            UE_QUIT("locate specified user ...");
        u_process(p_user,NULL);
    }
    else
        apply_users(u_process,NULL);
    return 0;
#undef UE_QUIT
}

