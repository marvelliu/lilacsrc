#include "bbs.h"
#ifndef USE_FILE_COPY
#define USE_HARD_LINK
#else
#undef USE_HARD_LINK
#endif
#define TRIES_ON_SAME_NAME 4
static const char* const suffix="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
/*��ָ����.DIR�ṹ���ö��ַ����Ҿ���ָ��id��fileheader��λ��*/
unsigned int getipos(struct fileheader *dir,unsigned int size,unsigned int id){
    /*��������Ϊ[0,size],����dir[size](ͨ���ǵ�ǰfileheader����)*/
    unsigned int l,h,m;
    /*�߽����*/
    if(dir[h=size].id==id)
        return size;
    if(dir[l=0].id==id)
        return 0;
    /*��������ַ�*/
    while((m=(l+h)/2)!=l)
        if(dir[m].id<id)
            l=m;
        else if(dir[m].id>id)
            h=m;
        else
            return m;
    return (unsigned int)-1;
}
/*��ָ����.DIR�ṹ��,�ӵ�ǰλ��������ҵ�һ������ָ��groupid��fileheader��λ��*/
unsigned int getgpos(struct fileheader *dir,unsigned int size,unsigned int groupid){
    unsigned int i;
    for(i=size-1;i!=(unsigned int)-1;i--)
        if(dir[i].groupid==groupid)
            return i;
    return (unsigned int)-1;
}
/*��ʼ��*/
int initdir(char *prog,char *path,char *board,unsigned int **offset,unsigned int *size,struct fileheader **dir,char **p){
    struct stat st;
    int filedes;
    /*��.DIR�ļ�*/
    sprintf(path,"boards/%s/.DIR",board);
    if(stat(path,&st)||!S_ISREG(st.st_mode)||(filedes=open(path,O_RDONLY))==-1){
        printf("[%s] �� %s ����...\n",prog,path);
        return 0x21;
    }
    /*����.DIR��С,����Ϊfileheader����*/
    if(st.st_size%sizeof(struct fileheader)){
        printf("[%s] �ļ� %s ��, ���޸����ٽ��а���ϲ�����...\n",prog,path);
        return 0x22;
    }
    (*size)=st.st_size/sizeof(struct fileheader);
    /*���븨����ƫ�Ʊ�ռ�*/
    if((*size)&&!((*offset)=(unsigned int*)malloc((*size)*sizeof(unsigned int)))){
        printf("[%s] ���븨����ռ�ʧ��...\n",prog);
        return 0x23;
    }
    /*ӳ��.DIR�ļ�*/
    if((*size)&&((*dir)=mmap(NULL,(*size)*sizeof(struct fileheader),PROT_READ|PROT_WRITE,MAP_PRIVATE,filedes,0))==(void*)-1){
        printf("[%s] ���� %s �ļ�ӳ��ʧ��...\n",prog,path);
        return 0x24;
    }
    close(filedes);
    (*p)=path+strlen(path)-4;
    return 0;
}
/*�ϲ����洦��*/
void process(char *prog,struct fileheader *ddir,struct fileheader *dir,unsigned int *offset,
    unsigned int dpos,unsigned int pos,char *dpath,char *dp,char *path,char *p){
    unsigned int ret;
    memcpy(&ddir[dpos],&dir[pos],sizeof(struct fileheader));
    /*���뵱ǰ��Ӧid�������µ�˳��id*/
    offset[pos]=dpos;
    ddir[dpos].id=dpos+1;
    /*Ѱ�Ҷ�Ӧ��groupid*/
    if((ret=getipos(dir,pos,ddir[dpos].groupid))!=(unsigned int)-1)/*groupid��Ӧ��fileheader����*/
        ddir[dpos].groupid=offset[ret]+1;
    else if((ret=getgpos(dir,pos,ddir[dpos].groupid))!=(unsigned int)-1)/*Ѱ����ǰ�����ͬ����fileheader*/
        ddir[dpos].groupid=ddir[offset[ret]].groupid;
    else
        ddir[dpos].groupid=ddir[dpos].id;/*����groupid=id*/
    /*Ѱ�Ҷ�Ӧ��reid*/
    if(ddir[dpos].groupid==ddir[dpos].id)/*������ʼ����,reid=id*/
        ddir[dpos].reid=ddir[dpos].id;
    else if((ret=getipos(dir,pos,ddir[dpos].reid))!=(unsigned int)-1)/*reid��Ӧ��fileheader����*/
        ddir[dpos].reid=offset[ret]+1;
    else
        ddir[dpos].reid=ddir[dpos].id;/*����reid=id*/
    /*�Զ�ʧ������ʼ���¶����ֵ��µ�������ʼ�ı��⴦��*/
    if(ddir[dpos].groupid==ddir[dpos].id&&!strncmp(ddir[dpos].title,"Re: ",4))
        memmove(ddir[dpos].title,ddir[dpos].title+4,ARTICLE_TITLE_LEN-4);
    sprintf(p,"%s",ddir[dpos].filename);
    sprintf(dp,"%s",ddir[dpos].filename);
#ifdef USE_HARD_LINK
    /*Ӳ���Ӷ�Ӧ�ļ�*/
    if(link(path,dpath)){
        if(errno==EEXIST){
            char *ptr;int i;
            ptr=ddir[dpos].filename+strlen(ddir[dpos].filename)-2;
            srand(getpid());
            /*ʧ�ܵ�ʱ����ͬһʱ����ϸ�����׺*/
            for(i=0;i<TRIES_ON_SAME_NAME;i++){
                *ptr=suffix[rand()%62];*(ptr+1)=suffix[rand()%62];
                sprintf(dp,"%s",ddir[dpos].filename);
                if(!link(path,dpath)){
                    printf("[%s] ��������Ӳ����: %s -> %s\n",prog,path,ddir[dpos].filename);
                    return;
                }
            }
        }
        printf("[%s] Ӳ���� %d (%s) ʧ��...\n",prog,pos,path);
    }
#else
    /*���ƶ�Ӧ�ļ�*/
    if(f_cp(path,dpath,O_EXCL)==-1){
        char *ptr;int i;
        ptr=ddir[dpos].filename+strlen(ddir[dpos].filename)-2;
        srand(getpid());
        /*ʧ�ܵ�ʱ����ͬһʱ����ϸ�����׺*/
        for(i=0;i<TRIES_ON_SAME_NAME;i++){
            *ptr=suffix[rand()%62];*(ptr+1)=suffix[rand()%62];
            sprintf(dp,"%s",ddir[dpos].filename);
            if(f_cp(path,dpath,O_EXCL)!=-1){
                printf("[%s] ���������ļ�: %s -> %s\n",prog,path,ddir[dpos].filename);
                return;
            }
        }
        printf("[%s] ���� %d (%s) ʧ��...\n",prog,pos,path);
    }
#endif
    return;
}
/*���һ��.DIR�ṹ�Ƿ�����id��˳���ϵ*/
int checkseq(struct fileheader *dir,unsigned int size){
    unsigned int i;
    for(i=1;i<size;i++)
        if(!(dir[i-1].id<dir[i].id))
            return 0;
    return 1;
}
/*�Բ�����id˳���ϵ��.DIR�ṹ����˳��Ԥ����*/
int seqdir(struct fileheader *dir,unsigned int size){
    unsigned int i,j;
    unsigned int *ioffset,*goffset;
    /*����������,�ֱ�����Ѱ��groupid��reid��Ӧ��ϵ*/
    if(!(ioffset=(unsigned int*)malloc(2*size*sizeof(unsigned int))))
        return 0;
    goffset=ioffset+size;
    for(i=0;i<size;i++){
        ioffset[i]=dir[i].id;
        goffset[i]=dir[i].groupid;
        dir[i].id=i+1;
        /*��Ӧreid*/
        for(j=i-1;j!=(unsigned int)-1;j--)
            if(ioffset[j]==dir[i].reid){
                dir[i].reid=j+1;
                break;
            }
        if(j==(unsigned int)-1)
            dir[i].reid=dir[i].id;
        /*��Ӧgroupid*/
        for(j=0;!(j>i);j++)
            if(goffset[j]==dir[i].groupid){
                dir[i].groupid=j+1;
                break;
            }
    }
    free(ioffset);
    return 1;
}
/*������*/
int main(int argc,char **argv){
    FILE *fp;
    struct stat st;
    struct fileheader *dir1,*dir2,*dir;
    struct boardheader bh;
    struct BoardStatus *bs;
    char path1[512],path2[512],path[512],buf[512],*p1,*p2,*p;
    int ret;
    unsigned int size1,size2,pos1,pos2,pos,*offset1,*offset2;
    //��ʼ����������
    if(argc!=4){
        printf("[%s] ����Ĳ����ṹ...\nUSAGE: %s <srcboard1> <srcboard2> <dstdir>\n",argv[0],argv[0]);
        return 0x11;
    }
    if(chdir(BBSHOME)){
        printf("[%s] �л��� BBS ��Ŀ¼ %s ����...\n",argv[0],BBSHOME);
        return 0x12;
    }
    /*��ʼ��Դ��������*/
    if((ret=initdir(argv[0],path1,argv[1],&offset1,&size1,&dir1,&p1))!=0)
        return ret;
    if((ret=initdir(argv[0],path2,argv[2],&offset2,&size2,&dir2,&p2))!=0)
        return ret;
    /*���˳�򲢽��п��ܱ�Ҫ��˳��*/
    if(!checkseq(dir1,size1)&&!seqdir(dir1,size1)){
        printf("[%s] ���� %s .DIR �ṹ����, �޸�ʧ��...\n",argv[0],argv[1]);
        return 0x51;
    }
    if(!checkseq(dir2,size2)&&!seqdir(dir2,size2)){
        printf("[%s] ���� %s .DIR �ṹ����, �޸�ʧ��...\n",argv[0],argv[2]);
        return 0x51;
    }
    /*����Ŀ��Ŀ¼,��pid���б�ʶ����*/
    sprintf(path,"boards/%s",argv[3]);
    sprintf(buf,"boards/%s_%d",argv[3], (int)getpid());
    if(!stat(path,&st)&&S_ISDIR(st.st_mode))
        rename(path,buf);
    else
        unlink(path);
    if(mkdir(path,(S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH))){
        printf("[%s] ��������Ŀ¼ %s ����...\n",argv[0],path);
        return 0x31;
    }
    build_board_structure(argv[3]);
    p=path+strlen(path);*p++='/';
    sprintf(p,".DIR");
    /*׼��Ŀ��.DIR�ļ���д����*/
    if(!(fp=fopen(path,"wb"))){
        printf("[%s] ���� %s ����...\n",argv[0],path);
        return 0x32;
    }
    /*����Ŀ��.DIR�ṹ��Ҫ���ڴ�ռ�*/
    if(!(dir=(struct fileheader*)malloc((size1+size2)*sizeof(struct fileheader)))){
        printf("[%s] �����ڴ�ռ�ʧ��...\n",argv[0]);
        return 0x33;
    }
    //������
    pos1=0;pos2=0;pos=0;
    while(true){
        /*�Ƚ�ʱ���*/
        if(pos1<size1&&pos2<size2) {
            //ret=strcmp(&dir1[pos1].filename[2],&dir2[pos2].filename[2]);
            ret = get_posttime(&dir1[pos1]) - get_posttime(&dir2[pos2]);
        } else if(pos1<size1)/*Դ����2�Ѿ��������*/
            ret=-1;
        else if(pos2<size2)/*Դ����1�Ѿ��������*/
            ret=1;
        else/*done*/
            break;
        /*����ǰfileheader*/
        if(!(ret>0)){/*Դ����1*/
            process(argv[0],dir,dir1,offset1,pos,pos1,path,p,path1,p1);
            pos1++;pos++;
        }
        else{/*Դ����2*/
            process(argv[0],dir,dir2,offset2,pos,pos2,path,p,path2,p2);
            pos2++;pos++;
        }
    }
    /*д��Ŀ��.DIR*/
    if(fwrite(dir,sizeof(struct fileheader),(size1+size2),fp)!=(size1+size2)){
        printf("[%s] д���ļ�����...\n",argv[0]);
        return 0x41;
    }
    /*�趨δ�����*/
    resolve_boards();
    ret=getboardnum(argv[3],&bh);
    if(ret){
        bh.idseq=size1+size2;
        set_board(ret,&bh,NULL);
        bs=getbstatus(ret);
        bs->total=size1+size2;
        bs->lastpost=size1+size2;
        bs->updatemark=true;
        bs->updatetitle=true;
        bs->updateorigin=true;
        bs->nowid=size1+size2;
        bs->toptitle=0;
    }  
    /*����*/
    fclose(fp);
    munmap(dir1,size1*sizeof(struct fileheader));
    if(size1)
        free(offset1);
    munmap(dir2,size2*sizeof(struct fileheader));
    if(size2)
        free(offset2);
    free(dir);
    return 0;
}
