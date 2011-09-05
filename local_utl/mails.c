/* etnlegend, 2006.04.08, �ż����͹��� */

/*
 * ������:          mails [options] <m_title> <m_path>
 * ������:
 *   <m_title>      �趨�ż��ı���Ϊ <m_title>, ����������Ч����Ϊ [MT_LEN] �ֽ�
 *   <m_path>       �趨�ż��ļ���·��, ���˲������ַ� `+` ��ʼ, ���ʾ����� [BBSHOME] �����·��
 * ѡ���:
 *   -p <perm>      ����Ȩ�޼��, �ż���ֻ���͸����� <perm> Ȩ�޵��û�, Ĭ�ϲ�����, 
                    <perm> ���ַ� `&` ��ʼʱ��ʾ����Ȩ��֮�����߼����ϵ, ��������������Ȩ�޶�����Ҫ��, 
                    Ĭ������¸���Ȩ��֮ǰ���߼����ϵ
 *   -u <user>      �����û����, �ż���ֻ���͸��û���Ϊ <user> ���û�, Ĭ�ϲ�����, ������ -l ѡ��ͬʱʹ��
 *   -l <list>      �����û����, �ż���ֻ���͸��ļ� <list> ָ�����û�, Ĭ�ϲ�����, ������ -u ѡ��ͬʱʹ��
 *   -m <mode>      �趨����ģʽ, ��Ч�趨ֵΪ `copy` �� `link`, `copy` ��ʾ�Ը���ģʽ�����ż�, 
 *                  `link` ��ʾ������ģʽ�����ż�, Ĭ��ģʽΪ `link`
 *   -s <name>      �趨�ż��ķ�����Ϊ <name>, Ĭ��Ϊ `SYSOP`, �����ߵ������Ч����Ϊ [IDLEN + 1] �ֽ�
 *   -v             ��ʾ������Ϣ, ����δ�����û���Ⲣ��Ŀ���û����� [MCV_THRESHOLD] ʱ��Ч
 *   -h             ��ʾ�ĵ���Ϣ
 *
*/

#include "bbs.h"

#define MT_LEN ARTICLE_TITLE_LEN                /* �ż����ⳤ�� */
#define MP_LEN _POSIX_PATH_MAX                  /* �ż�·������ */
#define MB_LEN 128                              /* ���������� */
#define MC_PERM 0x01                            /* Ȩ�޼���ǩ */
#define MC_USER 0x02                            /* �û�����ǩ */
#define MC_LIST 0x04                            /* �б����ǩ */
#define MC_MODE 0x08                            /* ����ģʽ��ǩ */
#define MC_NAME 0x10                            /* �������趨��ǩ */
#define MC_VERB 0x20                            /* ��ʾ������Ϣ */
#define MCP_AND_FLAG 0x80000000                 /* Ȩ�޼���趨Ϊ�߼���ʱ�ı�ǩ */
#define MCV_SEPS 20                             /* ������ʾ���¾��� */
#define MCV_THRESHOLD 1000                      /* ������ʾĿ���û��ż� */
#define MFR_DELIM " \t\n\r,;:"                  /* �ַ���������Ϸ� */

static const char* const perm_str=XPERMSTR;     /* Ȩ�޷����б� */
static char title[MT_LEN];                      /* �ż����� */
static char path[MP_LEN];                       /* �ż�·�� */
static unsigned int flag;                       /* ʹ��ѡ��ʱ��ѡ���ǩ */
static unsigned int perm;                       /* �趨Ȩ�޼��ʱ�ı����Ȩ�� */
static char user[IDLEN+1];                      /* �趨�û����ʱ���û��� */
static char list[MP_LEN];                       /* �趨�б���ʱ���б��ļ�·�� */
static char name[IDLEN+2];                      /* �趨������ʱ�ķ���������*/
static int count;                               /* ������ʾ��ǰ���� */
static int total;                               /* ������ʾ�ܼ��� */

/* ��ӡ�ĵ���Ϣ */
static inline int m_usage(void){
    fprintf(stdout,
        "  ������:          mails [options] <m_title> <m_path>\n"
        "  ������:\n"
        "    <m_title>      �趨�ż��ı���Ϊ <m_title>, ����������Ч����Ϊ [%d] �ֽ�\n"
        "    <m_path>       �趨�ż��ļ���·��, ���˲������ַ� `+` ��ʼ, ���ʾ����� [%s] �����·��\n"
        "  ѡ���:\n"
        "    -p <perm>      ����Ȩ�޼��, �ż���ֻ���͸����� <perm> Ȩ�޵��û�, Ĭ�ϲ�����, \n"
        "                    <perm> ���ַ� `&` ��ʼʱ��ʾ����Ȩ��֮�����߼����ϵ, ��������������Ȩ�޶�����Ҫ��, \n"
        "                    Ĭ������¸���Ȩ��֮ǰ���߼����ϵ\n"
        "    -u <user>      �����û����, �ż���ֻ���͸��û���Ϊ <user> ���û�, Ĭ�ϲ�����, ������ -l ѡ��ͬʱʹ��\n"
        "    -l <list>      �����û����, �ż���ֻ���͸��ļ� <list> ָ�����û�, Ĭ�ϲ�����, ������ -u ѡ��ͬʱʹ��\n"
        "    -m <mode>      �趨����ģʽ, ��Ч�趨ֵΪ `copy` �� `link`, `copy` ��ʾ�Ը���ģʽ�����ż�, \n"
        "                   `link` ��ʾ������ģʽ�����ż�, Ĭ��ģʽΪ `link`\n"
        "    -s <name>      �趨�ż��ķ�����Ϊ <name>, Ĭ��Ϊ `SYSOP`, �����ߵ������Ч����Ϊ [%d] �ֽ�\n"
        "    -v             ��ʾ������Ϣ, ����δ�����û���Ⲣ��Ŀ���û����� [%d] ʱ��Ч\n"
        "    -h             ��ʾ�ĵ���Ϣ\n",
        MT_LEN,BBSHOME,(IDLEN+1),MCV_THRESHOLD);
    return 0;
}

/* ����Ȩ�޼�� */
static inline int m_check_perm(const struct userec *user){
    return (!(flag&MC_PERM)?1:(!(perm&MCP_AND_FLAG)?(user->userlevel&perm):(((user->userlevel|MCP_AND_FLAG)&perm)==perm)));
}

/* ������ģʽ */
static inline int m_check_mode(void){
    return (!(flag&MC_MODE)?BBSPOST_LINK:BBSPOST_COPY);
}

/* �������� */
static inline int m_set_name(void){
    if(!(flag&MC_NAME))
        snprintf(name,IDLEN+2,"%s","SYSOP");
    return 0;
}

/* ������� */
static inline int m_count(struct userec *user,void *arg){
    if(m_check_perm(user))
        total++;
    return 0;
}

/* ������� */
static inline int m_verb(void){
    if((flag&MC_VERB)&&!(total<MCV_THRESHOLD)){
#define M_VERB_OUT(t,n) do{fprintf(stdout,(t),(n));fflush(stdout);return 0;}while(0)
        if(!count)
            M_VERB_OUT("%d%%",0);
        if(count==(total-1))
            M_VERB_OUT("...%d%%\n",100);
        if(!(count%(total/MCV_SEPS)))
            M_VERB_OUT("...%d%%",((100*count)/total));
#undef M_VERB_OUT
    }
    return 0;
}

/* �����ż����� */
static inline int m_send(struct userec *user,void *arg){
    if(m_check_perm(user)){
        mail_file(name,path,user->userid,title,m_check_mode(),NULL);
        m_verb();
        count++;
    }
    return 0;
}

int main(int argc,char **argv){
#define ME_QUIT(s) do{fprintf(stderr,"error: %s\n",s);exit(__LINE__);}while(0)
    FILE *fp;
    struct userec *p_user;
    struct stat st;
    char cwd[MP_LEN],buf[MB_LEN],*p,*q;
    int ret;
    if(!getcwd(cwd,MP_LEN))
        ME_QUIT("get current working directory ...");
    opterr=0;
    while((ret=getopt(argc,argv,"p:u:l:m:s:vh"))!=-1){
        switch(ret){
            case 'p':                           /* Ȩ�޼�� */
                flag|=MC_PERM;
                if(*optarg=='&')                /* ʹ���߼��뷽ʽ����Ȩ�޼�� */
                    perm|=MCP_AND_FLAG;
                for(p=optarg;*p;p++){
                    if(!(q=strchr(perm_str,*p)))
                        continue;
                    perm|=(1<<(q-perm_str));
                }
                break;
            case 'u':                           /* �û���� */
                flag|=MC_USER;
                snprintf(user,IDLEN+1,"%s",optarg);
                break;
            case 'l':                           /* �б��� */
                flag|=MC_LIST;
                if(*optarg=='/')
                    snprintf(list,MP_LEN,"%s",optarg);
                else
                    snprintf(list,MP_LEN,"%s/%s",cwd,optarg);
                break;
            case 'm':                           /* �趨����ģʽ */
                if(!strcasecmp(optarg,"copy"))
                    flag|=MC_MODE;
                break;
            case 's':                           /* �趨������ */
                flag|=MC_NAME;
                snprintf(name,IDLEN+2,"%s",optarg);
                break;
            case 'v':
                flag|=MC_VERB;
                break;
            case 'h':
                m_usage();
                return 0;
            default:
                m_usage();
                ME_QUIT("get options ...");
        }
    }
    if((flag&MC_USER)&&(flag&MC_LIST)){         /* ��ͻ��� */
        m_usage();
        ME_QUIT("options `-u` and `-l` conflict ...");
    }
    if((argc-optind)!=2){
        m_usage();
        ME_QUIT("get parameters ...");
    }
    snprintf(title,MT_LEN,"%s",argv[optind++]);
    if(argv[optind][0]=='+')
        snprintf(path,MP_LEN,"%s/%s",BBSHOME,&argv[optind][1]);
    else if(argv[optind][0]=='/')
        snprintf(path,MP_LEN,"%s",argv[optind]);
    else
        snprintf(path,MP_LEN,"%s/%s",cwd,argv[optind]);
    if(stat(path,&st)||!(st.st_mode&(S_IFREG|S_IFLNK)))
        ME_QUIT("get vaild file to mail ...");
    if(chdir(BBSHOME)==-1)
        ME_QUIT("change directory to BBSHOME ...");
    resolve_ucache();
    resolve_boards();
    resolve_utmp();
    m_set_name();
    if(flag&MC_USER){
        if(!getuser(user,&p_user))
            ME_QUIT("locate specified user ...");
        m_send(p_user,NULL);
    }
    else if(flag&MC_LIST){
        if(!(fp=fopen(list,"r")))
            ME_QUIT("open list file ...");
        while(fgets(buf,MB_LEN,fp))
            for(p=strtok(buf,MFR_DELIM);p;p=strtok(NULL,MFR_DELIM))
                if(getuser(p,&p_user))
                    m_send(p_user,NULL);
        fclose(fp);
    }
    else{
        if(flag&MC_VERB)
            apply_users(m_count,NULL);
        apply_users(m_send,NULL);
    }
    return 0;
#undef ME_QUIT
}

