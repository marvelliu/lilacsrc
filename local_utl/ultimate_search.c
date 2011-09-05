/* etnlegend, 2006.09.14, ȫվ����ȫ������ϵͳ ... */

/*
 * ������:          ultimate_search [ѡ��] <ģʽ��>
 *   <ģʽ��>       ��Ҫȫ���������ַ���
 *
 * [ѡ��]           [˵��]                              [��ͻ]
 *
 *   {ʱ�䷶Χ}     ������                              Ĭ��
 *     -r <n>       ��� <n> ��                         ������ -f �� -t ͬʱʹ��
 *     -f <time>    �� <time> ��ָ����ʱ�俪ʼ          ������ -r ͬʱʹ��
 *     -t <time>    �� <time> ��ָ����ʱ�����          ������ -r ͬʱʹ��
 *
 *        <time> �ĸ�ʽΪ YYYYMMDDHHMMSS (������ʱ����)
 *
 *   {���淶Χ}     ȫվ��������                        Ĭ��
 *     -a           ȫ������                            ������ -b �� -u ͬʱʹ��
 *     -b <board>   �� <board> ����                     ������ -a �� -u ͬʱʹ��
 *     -u <user>    �� <user> �û��ɶ��İ���            ������ -a �� -b ͬʱʹ��
 *
 *   {�û���Χ}     ȫվ�û�                            Ĭ��
 *     -p <poster>  �� <poster> ���������              ��
 *
 *   {λ�÷�Χ}     ���浱ǰ����                        Ĭ��
 *     -d           ��������վ����                      ��
 *     -j           ������ɾ������                      ��
 *     -e           ���������浱ǰ����                  ������ -d �� -j ͬʱʹ��
 *
 *   {��������}     ����ȫ��ƥ��λ��                    Ĭ��
 *                  �������Ķ�����֤                    Ĭ��
 *                  ��Сд����                          Ĭ��
 *     -s           ������ÿƪ���µĵ�һ��ƥ��λ��      ��
 *     -n           ���������Ķ�����֤                  ��
 *     -i           ��Сд������                        ��
 *
 *   {���ѡ��}     �������ǰĿ¼�� res_<mark>.us �ļ� Ĭ��
 *                  ������Ϥ�������                    Ĭ��
 *     -o <file>    ������ļ� <file> ��                ��
 *     -q           ����ģʽ                            ��
 *
 *        <mark> Ϊ���е�ǰ�����ʱ��� ( UNIX ��ʽ)
 *
 *   {����ѡ��}     ��������ʱ���������Ϣ              Ĭ��
 *     -h           ���������Ϣ                        ��
 *
*/

#include "bbs.h"

#define PARAM_R                 0x0001
#define PARAM_F                 0x0002
#define PARAM_T                 0x0004
#define PARAM_A                 0x0008
#define PARAM_B                 0x0010
#define PARAM_U                 0x0020
#define PARAM_P                 0x0040
#define PARAM_D                 0x0080
#define PARAM_J                 0x0100
#define PARAM_E                 0x0200
#define PARAM_S                 0x0400
#define PARAM_N                 0x0800
#define PARAM_I                 0x1000
#define PARAM_O                 0x2000
#define PARAM_Q                 0x4000
#define LENGTH                  1024
#define BOUND                   _POSIX_PATH_MAX
#define ISSET(param)            (flag&(param))

typedef int (*APPLY_BIDS_FUNC) (
    int(*)(const struct boardheader*,int,void*),
    void*
);
typedef int (*APPLY_RECORD_FUNC) (
    const char*,
    int(*)(const struct fileheader*,int,const struct boardheader*),
    int,
    const struct boardheader*,
    int,
    bool
);

static const APPLY_BIDS_FUNC    APPLY_BIDS          =   (APPLY_BIDS_FUNC)   apply_bids;
static const APPLY_RECORD_FUNC  APPLY_RECORD        =   (APPLY_RECORD_FUNC) apply_record;

static FILE                     *out;
static const struct userec      *user;
static const char               *P;
static const char               *L;
static const char               *mode;
static char                     post[OWNER_LEN];
static int                      flag;
static int                      current;
static int                      count;
static int                      number;
static time_t                   from;
static time_t                   to;
static time_t                   mark;
static size_t                   size;

static inline int usage(void){
    fprintf(stderr,"\n%s\n",
        "������:          ultimate_search [ѡ��] <ģʽ��>\n"
        "  <ģʽ��>       ��Ҫȫ���������ַ���\n"
        "\n"
        "[ѡ��]           [˵��]                              [��ͻ]\n"
        "\n"
        "  {ʱ�䷶Χ}     ������                              Ĭ��\n"
        "    -r <n>       ��� <n> ��                         ������ -f �� -t ͬʱʹ��\n"
        "    -f <time>    �� <time> ��ָ����ʱ�俪ʼ          ������ -r ͬʱʹ��\n"
        "    -t <time>    �� <time> ��ָ����ʱ�����          ������ -r ͬʱʹ��\n"
        "\n"
        "       <time> �ĸ�ʽΪ YYYYMMDDHHMMSS (������ʱ����)\n"
        "\n"
        "  {���淶Χ}     ȫվ��������                        Ĭ��\n"
        "    -a           ȫ������                            ������ -b �� -u ͬʱʹ��\n"
        "    -b <board>   �� <board> ����                     ������ -a �� -u ͬʱʹ��\n"
        "    -u <user>    �� <user> �û��ɶ��İ���            ������ -a �� -b ͬʱʹ��\n"
        "\n"
        "  {�û���Χ}     ȫվ�û�                            Ĭ��\n"
        "    -p <poster>  �� <poster> ���������              ��\n"
        "\n"
        "  {λ�÷�Χ}     ���浱ǰ����                        Ĭ��\n"
        "    -d           ��������վ����                      ��\n"
        "    -j           ������ɾ������                      ��\n"
        "    -e           ���������浱ǰ����                  ������ -d �� -j ͬʱʹ��\n"
        "\n"
        "  {��������}     ����ȫ��ƥ��λ��                    Ĭ��\n"
        "                 �������Ķ�����֤                    Ĭ��\n"
        "                 ��Сд����                          Ĭ��\n"
        "    -s           ������ÿƪ���µĵ�һ��ƥ��λ��      ��\n"
        "    -n           ���������Ķ�����֤                  ��\n"
        "    -i           ��Сд������                        ��\n"
        "\n"
        "  {���ѡ��}     �������ǰĿ¼�� res_<mark>.us �ļ� Ĭ��\n"
        "                 ������Ϥ�������                    Ĭ��\n"
        "    -o <file>    ������ļ� <file> ��                ��\n"
        "    -q           ����ģʽ                            ��\n"
        "\n"
        "       <mark> Ϊ���е�ǰ�����ʱ��� ( UNIX ��ʽ)\n"
        "\n"
        "  {����ѡ��}     ��������ʱ���������Ϣ              Ĭ��\n"
        "    -h           ���������Ϣ                        ��\n"
    );
    return 0;
}

static inline int set_pattern(const char *s){
#define ALPHA(c)    (!((c)<'A')&&!((c)>'Z'))
    static char pattern[LENGTH];
    static int initialized,i;
    if(!initialized){
        if(!ISSET(PARAM_I)){
            snprintf(pattern,LENGTH,"%s",s);
            size=strlen(pattern);
        }
        else{
            for(size=0;s[size];size++){
                pattern[size]=toupper(s[size]);
                if(ALPHA(pattern[size]))
                    i++;
            }
            if(!i)
                flag&=(~PARAM_I);
        }
        P=pattern;
        initialized=1;
    }
    return 0;
#undef ALPHA
}

static inline int set_link(const char *s){
    static char link[LENGTH];
    static int initialized,i,j;
    if(!initialized){
        for(i=0,j=1;s[j];j++){
            while(i>0&&s[i]!=s[j])
                i=link[i-1];
            if(s[i]==s[j])
                i++;
            link[j]=i;
        }
        L=link;
        initialized=1;
    }
    return 0;
}

static inline int process_article(const struct fileheader *f,int n,const struct boardheader *b){
    static const struct flock lck_set={.l_type=F_RDLCK,.l_whence=SEEK_SET,.l_start=0,.l_len=0,.l_pid=0};
    static const struct flock lck_clr={.l_type=F_UNLCK,.l_whence=SEEK_SET,.l_start=0,.l_len=0,.l_pid=0};
    static struct stat st;
    static struct tm *p;
    static char name[BOUND];
    static int fd,i,j,k,l;
    static time_t timestamp;
    static const char *S,*M,*N;
    static void *vp;
    do{
        if((timestamp=get_posttime(f))<from||timestamp>to)
            break;
        if(ISSET(PARAM_P)&&strcmp(f->owner,post))
            break;
        setbfile(name,b->filename,f->filename);
        if(stat(name,&st)==-1||!S_ISREG(st.st_mode)||st.st_size<size)
            break;
        if((fd=open(name,O_RDONLY
#ifdef O_NOATIME
            |O_NOATIME
#endif /* O_NOATIME */
            ,0644))==-1)
            break;
        if(fcntl(fd,F_SETLKW,&lck_set)==-1){
            close(fd);
            break;
        }
        vp=mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
        fcntl(fd,F_SETLKW,&lck_clr);
        close(fd);
        if((S=(const char*)vp)==MAP_FAILED)
            break;
        for(p=NULL,j=0,i=0;S[i]&&i<st.st_size;i++){
#define EQUAL(cp,cs)    (((cp)==(cs))||(ISSET(PARAM_I)&&((cp)==toupper(cs))))
            while(j>0&&!EQUAL(P[j],S[i]))
                j=L[j-1];
            if(EQUAL(P[j],S[i]))
                j++;
            if(!P[j]){
                M=&S[l=((i-j)+1)];
                if(!ISSET(PARAM_N)){
                    for(k=0,N=M;!(N<S);N--)
                        if((*N)&0x80)
                            k++;
                    if(!(k&0x01))
                        continue;
                }
                if(!p&&!(p=localtime(&timestamp)))
                    continue;
                count++;
                fprintf(out,"%6d %-20.20s %4d %4s %04d%02d%02d%02d%02d%02d %-17.17s %6d %-13.13s %s\n",
                    n,b->filename,current,mode,(p->tm_year+1900),(p->tm_mon+1),(p->tm_mday),
                    (p->tm_hour),(p->tm_min),(p->tm_sec),f->filename,l,f->owner,f->title);
                if(ISSET(PARAM_S))
                    break;
                j=L[j-1];
            }
#undef EQUAL
        }
        munmap(vp,st.st_size);
        number++;
    }
    while(0);
    return 0;
}

static inline int process_board(const struct boardheader *b,int n,void *v){
    static char name[BOUND];
    do{
        if(ISSET(PARAM_A))
            break;
        if(ISSET(PARAM_U)){
            if(!check_read_perm(user,b))
                return -1;
            break;
        }
        if(ISSET(PARAM_B))
            break;
        if(!public_board(b))
            return -2;
    }
    while(0);
    current=n;
    if(!ISSET(PARAM_Q))
        fprintf(stdout,"���ڴ������ %-29.29s ... ",b->filename);
    if(!ISSET(PARAM_E)){
        mode="����";
        setbdir(DIR_MODE_NORMAL,name,b->filename);
        APPLY_RECORD(name,process_article,sizeof(struct fileheader),b,0,true);
    }
    if(ISSET(PARAM_D)){
        mode="����";
        setbdir(DIR_MODE_DELETED,name,b->filename);
        APPLY_RECORD(name,process_article,sizeof(struct fileheader),b,0,true);
    }
    if(ISSET(PARAM_J)){
        mode="��ɾ";
        setbdir(DIR_MODE_JUNK,name,b->filename);
        APPLY_RECORD(name,process_article,sizeof(struct fileheader),b,0,true);
    }
    if(!ISSET(PARAM_Q))
        fprintf(stdout,"%s\n","�������!");
    return 0;
}

int main(int argc,char **argv){
#define EXIT(msg)  do{fprintf(stderr,"%s\n",(msg));if(out)fclose(out);exit(__LINE__);}while(0)
    const struct boardheader *board;
    char name[BOUND],path[BOUND];
    const char *desc;
    int ret;
    double cost;
    if(!getcwd(path,BOUND))
        EXIT("��ȡ��ǰ����Ŀ¼ʱ��������");
    if(chdir(BBSHOME)==-1)
        EXIT("�л�����Ŀ¼ʱ��������...");
    if((mark=time(NULL))==(time_t)(-1))
        EXIT("��ȡʱ��ʱ��������...");
    resolve_ucache();
    resolve_boards();
    to=mark;
    opterr=0;
    while((ret=getopt(argc,argv,"r:f:t:ab:u:p:djesnio:qh"))!=-1){
        switch(ret){
#define CHECK_CONFLICT(param)   do{if(ISSET(param))EXIT("������ѡ�����ڳ�ͻ...");}while(0)
#define CHECK_DEPENDENCE(param) do{if(!ISSET(param))EXIT("������ѡ���ȱ������...");}while(0)
#define CHECK_DUP(param)        do{if(ISSET(param))EXIT("������ѡ���д����ظ�...");}while(0)
#define SET(param)              do{CHECK_DUP(param);flag|=(param);}while(0)
            case 'r':
                CHECK_CONFLICT(PARAM_F|PARAM_T);
                SET(PARAM_R);
                do{
                    struct tm t,*p;
                    int n;
                    if(!isdigit(optarg[0]))
                        EXIT("ѡ�� -r �Ĳ����޷�����...");
                    n=atoi(optarg);
                    if(!(p=localtime(&mark)))
                        EXIT("����ʱ��ʱ��������...");
                    memcpy(&t,p,sizeof(struct tm));
                    t.tm_hour=0;
                    t.tm_min=0;
                    t.tm_sec=0;
                    if((from=mktime(&t))==(time_t)(-1))
                        EXIT("�趨ʱ��ʱ��������...");
                }
                while(0);
                break;
#define PARSE2(p)   ((((p)[0]*10)+((p)[1]*1))-('0'*11))
#define PARSE4(p)   ((PARSE2(p)*100)+(PARSE2(&(p)[2])*1))
            case 'f':
                CHECK_CONFLICT(PARAM_R);
                SET(PARAM_F);
                do{
                    struct tm t;
                    int i;
                    for(i=0;optarg[i];i++)
                        if(!isdigit(optarg[i]))
                            break;
                    if(i!=14)
                        EXIT("ѡ�� -f �Ĳ����޷�����...");
                    memset(&t,0,sizeof(struct tm));
                    t.tm_year=(PARSE4(optarg)-1900);
                    t.tm_mon=(PARSE2(&optarg[4])-1);
                    t.tm_mday=PARSE2(&optarg[6]);
                    t.tm_hour=PARSE2(&optarg[8]);
                    t.tm_min=PARSE2(&optarg[10]);
                    t.tm_sec=PARSE2(&optarg[12]);
                    if((from=mktime(&t))==(time_t)(-1))
                        EXIT("�趨ʱ��ʱ��������...");
                }
                while(0);
                break;
            case 't':
                CHECK_CONFLICT(PARAM_R);
                SET(PARAM_T);
                do{
                    struct tm t;
                    int i;
                    for(i=0;optarg[i];i++)
                        if(!isdigit(optarg[i]))
                            break;
                    if(i!=14)
                        EXIT("ѡ�� -t �Ĳ����޷�����...");
                    memset(&t,0,sizeof(struct tm));
                    t.tm_year=(PARSE4(optarg)-1900);
                    t.tm_mon=(PARSE2(&optarg[4])-1);
                    t.tm_mday=PARSE2(&optarg[6]);
                    t.tm_hour=PARSE2(&optarg[8]);
                    t.tm_min=PARSE2(&optarg[10]);
                    t.tm_sec=PARSE2(&optarg[12]);
                    if((from=mktime(&t))==(time_t)(-1))
                        EXIT("�趨ʱ��ʱ��������...");
                }
                while(0);
                break;
#undef PARSE2
#undef PARSE4
            case 'a':
                CHECK_CONFLICT(PARAM_B|PARAM_U);
                SET(PARAM_A);
                break;
            case 'b':
                CHECK_CONFLICT(PARAM_A|PARAM_U);
                SET(PARAM_B);
                if(!(current=getbid(optarg,&board)))
                    EXIT("ѡ�� -b ��ָ���İ����޷���ȡ...");
                break;
            case 'u':
                CHECK_CONFLICT(PARAM_A|PARAM_B);
                SET(PARAM_U);
                do{
                    struct userec *u;
                    if(!getuser(optarg,&u))
                        EXIT("ѡ�� -u ��ָ�����û��޷���ȡ...");
                    user=u;
                }
                while(0);
                break;
            case 'p':
                SET(PARAM_P);
                snprintf(post,OWNER_LEN,"%s",optarg);
                break;
            case 'd':
                SET(PARAM_D);
                break;
            case 'j':
                SET(PARAM_J);
                break;
            case 'e':
                CHECK_DEPENDENCE(PARAM_D|PARAM_J);
                SET(PARAM_E);
                break;
            case 's':
                SET(PARAM_S);
                break;
            case 'n':
                SET(PARAM_N);
                break;
            case 'i':
                SET(PARAM_I);
                break;
            case 'o':
                SET(PARAM_O);
                if(optarg[0]!='/')
                    snprintf(name,BOUND,"%s/%s",path,optarg);
                else
                    snprintf(name,BOUND,"%s",optarg);
                break;
            case 'q':
                SET(PARAM_Q);
                break;
            case 'h':
                usage();
                return 0;
            default:
                usage();
                EXIT("����ʶ���ѡ��...");
                break;
#undef CHECK_CONFLICT
#undef CHECK_DEPENDENCE
#undef CHECK_DUP
#undef SET
        }
    }
    if(from>to){
        usage();
        EXIT("��ǰʱ���趨���Ϸ�...");
    }
    if(!ISSET(PARAM_Q)&&setvbuf(stdout,NULL,_IONBF,BUFSIZ))
        EXIT("�����ļ�����ʱ��������...");
    if((argc-optind)!=1){
        usage();
        EXIT("����ʶ��Ĳ���...");
    }
    set_pattern(argv[optind]);
    set_link(argv[optind]);
    if(!size)
        EXIT("ģʽ������Ϊ�մ�...");
    if(!ISSET(PARAM_O))
        snprintf(name,BOUND,"%s/res_%lu.us",path,mark);
    if(!(out=fopen(name,"w")))
        EXIT("���ļ�ʱ��������...");
    fprintf(out,"%6s %-20.20s %4s %4s %-14.14s %-17.17s %6s %-13.13s %s\n",
        "���º�","��������"," BID","λ��","����ʱ��","�ļ���","ƫ����","����","����");
    if(!(P[0]&0x80))
        flag|=PARAM_N;
    if(ISSET(PARAM_B))
        process_board(board,current,NULL);
    else
        APPLY_BIDS(process_board,NULL);
    fclose(out);
    cost=difftime(time(NULL),mark);
    if(cost>86400){
        cost/=86400;
        desc="��";
    }
    else if(cost>3600){
        cost/=3600;
        desc="Сʱ";
    }
    else if(cost>60){
        cost/=60;
        desc="����";
    }
    else
        desc="��";
    fprintf(stdout,"\n���������! ������ %d ƪ����, ��� %d ��ƥ��, ��ʱ %.2lf %s!\n",
        number,count,cost,desc);
    return 0;
#undef EXIT
}

