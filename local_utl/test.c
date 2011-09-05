#include "bbs.h"

#include <sys/types.h>
#include <dirent.h>


void viewtitle(int id, char *board)
{
	char filename[100];
	int i, j, m, att_size;
	fileheader_t fh;
	int fd;
    struct stat st;
	char *p, *start, *f;
    off_t fsize;


	int att_num = 2;

    sprintf(filename, BBSHOME "/boards/%s/" DOT_DIR, board);
    if ((fd = open(filename, O_RDONLY)) < 0){
        printf("file open error: %s\n",filename);
		return;
	}
    fstat(fd, &st);
    int total = st.st_size / sizeof(fh);

	for(i=total-1;i>=0;i--){
    	lseek(fd, i * sizeof(fh), SEEK_SET);
		if(read(fd,(void *)&fh,sizeof(fileheader_t))<=0){
			printf("lseek error!\n");
			break;
		}
		if(fh.id==id)
			break;
	}
	if(i<0){
		printf("file not found\n");
		return;
	}
	close(fd);


    sprintf(filename, BBSHOME "/boards/%s/%s", board, fh.filename);
    j = safe_mmapfile(filename, O_RDONLY, PROT_READ, MAP_SHARED, &p, &fsize, NULL);
	if(!j)
		return;

	start = p;
	printf("file size:%d\n", (int)fsize);
	printf("size:%ld:\n%s-------END--------\n",fh.attachment, start);

	if(fh.attachment>0){
		printf("attachment:%ld\n", fh.attachment);
		p += fh.attachment;



		for(i=1; i<=att_num;i++)
		{
			p--;
			for(m=0;m<10;m++)
				printf("%d\t", *(p+m));
			printf("\n");
			p++;
    		if (memcmp(p, ATTACHMENT_PAD, ATTACHMENT_SIZE))
    	    	return;
			p += ATTACHMENT_SIZE;
			f = p;
			p += (strlen(p)+1);
    		memcpy(&att_size, p, sizeof(int));
			att_size = ntohl(att_size); 
			p += sizeof(int);
			printf("attach name: %s size: %d\n", f, att_size);
			p += att_size;
		}
	}
    end_mmapfile((void *) p, fsize, -1);
}

void viewboard(char *filename)
{
	fileheader_t fh;
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	while(read(fd,(void *)&fh,sizeof(fileheader_t))>0){
		if(strncmp(fh.title, "Re: ", 4)==0)
			continue;
		printf("%u,%u:%s\t%s\t%s\t%ld\n",fh.id, fh.groupid,fh.filename,fh.title,fh.owner,fh.attachment);
	}
	close(fd);
}

void viewtop(char *filename)
{
	struct posttop fh;
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	while(read(fd,(void *)&fh,sizeof(struct posttop))>0){
		printf("%u,%u:%s\t%d\n",fh.number, fh.groupid,fh.board,(int)fh.date);
	}
	close(fd);
}


void viewboards(char *filename)
{
	struct boardheader fh;
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	while(read(fd,(void *)&fh,sizeof(struct boardheader))>0){
		printf("%s,%s:%s\t%d\t%d\n",fh.filename, fh.BM,fh.title,fh.level,fh.flag);
	}
	close(fd);
}

int getlastpost1(const char *board, int *lastpost, int *total)
{
    struct fileheader fh;
    struct stat st;
    char filename[STRLEN * 2];
    int fd, atotal;

    sprintf(filename, BBSHOME "/boards/%s/" DOT_DIR, board);
	printf("p:%s\n",filename);
    if ((fd = open(filename, O_RDWR)) < 0)
        return 0;
    fstat(fd, &st);
    atotal = st.st_size / sizeof(fh);

	printf("atotal:%u\n",atotal);

    if (atotal <= 0) {
        *lastpost = 0;
        *total = 0;
        close(fd);
        return 0;
    }
    *total = atotal;
    lseek(fd, (atotal - 1) * sizeof(fh), SEEK_SET);
    if (read(fd, &fh, sizeof(fh)) > 0) {
        *lastpost = fh.id;
    }
    close(fd);
    return 0;
}

void viewuser(char *userid){
	struct userdata udata;
	int tuid=0;


    if (init_all()) {
        printf("init data fail\n");
        return ;
    }

	//if (!(tuid = getuser(userid, &lookupuser))) {
	if (!(tuid = getuser(userid, NULL))) {
		printf("no such user\n");
		return;
	}


	read_userdata(userid,&udata);
	printf("userid:%s\n",udata.userid);
	printf("realname:%s\n",udata.realname);
	printf("realemail:%s\n",udata.realemail);
	printf("email:%s\n",udata.email);
	printf("reg_email:%s\n",udata.reg_email);
}

int main(int argc, char **argv)
{
	char filename[100];
	if(argc == 2 && strcmp(argv[1],"-brd")==0){
		sprintf(filename,"/home/lilachome/.BOARDS");
		viewboards(filename);
	}else if(argc==3){
		if(strcmp(argv[1],"-dir")==0){
			sprintf(filename,BBSHOME "/boards/%s/.DIR",argv[2]);
			viewboard(filename);
		}
		else if(strcmp(argv[1],"-top")==0){
			sprintf(filename,BBSHOME "/etc/posts/day.%s",argv[2]);
			viewtop(filename);
		}
		else if(strcmp(argv[1],"-last")==0){
			int total,last;
			getlastpost1(argv[2],&last,&total);
			printf("last:%u\t,total:%u\n",last,total);
		}
		else if(strcmp(argv[1],"-user")==0){
			viewuser(argv[2]);
		}
	}else if(argc==4){
		if(strcmp(argv[1],"-file")==0){
			viewtitle(atoi(argv[2]), argv[3]);
		}
	}else{
		printf("arg error\n");
	}

//	remove_ipctl("levram");
//	remove_ipctl("marvel");
	return 0;
}
