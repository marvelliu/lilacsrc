#include "bbs.h"

#include <sys/types.h>
#include <dirent.h>


void doit(char *filename)
{
	fileheader_t fh;
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	while(read(fd,(void *)&fh,sizeof(fileheader_t))>0){
		printf("%u,%u:%s\t%s\t%s\t%d\n",fh.id, fh.groupid,fh.filename,fh.title,fh.owner,atol(fh.filename+2));
	}
	close(fd);
}

void doit1(char *filename)
{
	struct posttop fh;
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	while(read(fd,(void *)&fh,sizeof(struct posttop))>0){
		printf("%u,%u:%s\t%d\n",fh.number, fh.groupid,fh.board,fh.date);
	}
	close(fd);
}


void doit2(char *filename)
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

    sprintf(filename, "/home/lilachome/boards/%s/" DOT_DIR, board);
	printf("%s\n",filename);
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

int main(int argc, char **argv)
{
	char filename[100];
	if(argc == 2 && strcmp(argv[1],"-brd")==0){
		sprintf(filename,"/home/lilachome/.BOARDS");
		doit2(filename);
	}else if(argc==3){
		if(strcmp(argv[1],"-dir")==0){
			sprintf(filename,"/home/bbsdata/boards/%s/.DIR",argv[2]);
			doit(filename);
		}
		else if(strcmp(argv[1],"-top")==0){
			sprintf(filename,"/home/lilachome/etc/posts/day.%s",argv[2]);
			doit1(filename);
		}
		else if(strcmp(argv[1],"-last")==0){
			int total,last;
			getlastpost1(argv[2],&last,&total);
			printf("last:%u\t,total:%u\n",last,total);
		}
	}else{
		printf("arg error\n");
	}

//	remove_ipctl("levram");
//	remove_ipctl("marvel");
}
