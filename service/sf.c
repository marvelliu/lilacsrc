#include "service.h"
#include "bbs.h"

#ifndef WORDLEN
#define WORDLEN 100
#endif
#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif
int save_y, save_x;
char save_scr[LINEHEIGHT][LINELEN*3];
SMTH_API struct user_info uinfo;
SMTH_API int t_lines;


int replaceline(char *buf1, char *buf2, char *starttag, char *endtag)
{
	char *p1, *p2;
	p1 = buf1;
	buf2[0]=0;
	while(p1!=NULL)
	{
		p2 = p1;
		p1 = strstr(p2,starttag);
		if(!p1){
			strcat(buf2,p2);
			break;
		}
		*p1=0;
		strcat(buf2,p2);
		p1 += strlen(starttag);

		p2 = p1;
		p1 = strstr(p2,endtag);
		if(!p1){
			prints("解析数据出错:未找到结束tag");
	    	pressanykey();
			return -1;
		}
		*p1=0;
		sprintf(buf2,"%s\033[1;31m%s\033[m",buf2,p2);
		p1 += strlen(endtag);
	}
	return 0;
}

int convert_sf_rawfile(char *tmpfile1,char *tmpfile2){
	FILE *fp1, *fp2;
	char buf1[BUF_SIZE],buf2[BUF_SIZE],tmpbuf[BUF_SIZE];
	char *line;
	int i, j, qnum=0, rnum=0;

	fp1 = fopen(tmpfile1, "r");
	fp2 = fopen(tmpfile2, "w");

	if(!fp1 || !fp2){
		prints("打开文件出错");
    	pressanykey();
		return -1;
	}
	for(i=1;i<16;i++){
		line = fgets(buf1, BUF_SIZE, fp1); 
		if(feof(fp1)){
			prints("文件格式出错");
	    	pressanykey();
			return -1;
		}

		if(i==10 && (rnum = atol(line))<0){
			prints("分析结果：搜索结果非正数");
    		pressanykey();
			return -1;
		}
		if(i==15){
			strcpy(buf2, "您搜索的");
			if((qnum = atol(line))<=0){
				prints("分析结果：关键词个数非正数");
		    	pressanykey();
				return -1;
			}
		}
	}

	for(i=0;i<qnum;i++){
		line = fgets(buf1, BUF_SIZE, fp1); 
		if(feof(fp1)){
			prints("文件格式出错");
	    	pressanykey();
			return -1;
		}
		line[strlen(line)-1]=' ';
		strcat(buf2, line);
	}
	buf2[strlen(buf2)-1]='\0';
	sprintf(buf2,"%s共有%d个结果：\n", buf2, rnum);
	fputs(buf2,fp2);
	
	rnum = (rnum>20)?20:rnum;
	j=0;
	while(!feof(fp1) && j<rnum){
		j++;
		sprintf(buf2,"\n\n\n\033[1;32m结果%d\033[m：\n", j);
		fputs(buf2,fp2);

		for(i=1;i<17;i++){
			line = fgets(buf1, BUF_SIZE, fp1); 
			if(i==4){
				if(replaceline(buf1,buf2,"<font color='red'>","</font>"))
					return -1;
				fputs(buf2,fp2);
			}else if(i==5){
				tmpbuf[0]='\0';
				line[strlen(line)-1]=0;
				sprintf(tmpbuf,"类型：%s", line);
				strcpy(buf2,tmpbuf);
			}else if(i==6){
				line[strlen(line)-1]=0;
				sprintf(tmpbuf,"%s\t大小：%s",buf2, line);
				strcpy(buf2,tmpbuf);
			}else if(i==7){
				line[strlen(line)-1]=0;
				sprintf(tmpbuf,"%s\t时间：%s",buf2, line);
				strcpy(buf2,tmpbuf);
				fputs(buf2,fp2);
			}else if(i==8){
				sprintf(buf2,"位置：%s",buf1);
				fputs(buf2,fp2);
			}else if(i==16){
				if(replaceline(buf1,buf2,"<font color='#CC9999'>","</font>"))
					return -1;
				sprintf(tmpbuf,"地址：%s",buf2);
				fputs(tmpbuf,fp2);
			}
		}
	}
	fclose(fp1);
	fclose(fp2);
	return 0;
}

void do_search_sf(char *query, char *tmpfile1, char *tmpfile2)
{
	char genbuf[WORDLEN],buf[1024];
	char *p1,*p2;
	if(query == NULL)
		return;
	p1=p2=query;
	do{ 
		p2 = strstr(p1," ");
		if(p2){
			*p2++='\0';
		}   
		strcat(genbuf,p1);
		strcat(genbuf,"+");
		p1=p2;
	}while(p2);
	genbuf[strlen(genbuf)-1] = '\0';
	sprintf(buf,"/usr/bin/wget -q -O %s \"http://61.167.60.4/search.php?RAW&String=%s\"",tmpfile1,genbuf);
	//sprintf(buf,"/usr/bin/wget -q -O %s \"http://sf.hit.edu.cn/search.php?RAW&String=%s\"",tmpfile1,genbuf);
	if(system(buf)<0){
		prints("下载SF搜索结果出错");
    	pressanykey();
		return;
	}
	if(convert_sf_rawfile(tmpfile1,tmpfile2)<0){
		prints("解析搜索结果出错");
    	pressanykey();
		return;
	}
	ansimore(tmpfile2,true);
    pressanykey();
	return;
}

void search_sf_main()
{
    int oldmode;
    int i;
    char word[WORDLEN], _tmpfile1[1024],_tmpfile2[1024];
	char *tmpfile1 = _tmpfile1;
	char *tmpfile2 = _tmpfile2;

	strcpy(tmpfile1,BBSHOME "/tmp/SFOXXXXXX");
	strcpy(tmpfile2,BBSHOME "/tmp/SFNXXXXXX");
	if(mkstemp(tmpfile1) == -1){
		prints("创建临时文件失败");
    	pressanykey();
		return;
	}
	if(mkstemp(tmpfile2) == -1){
		prints("创建临时文件失败");
    	pressanykey();
		unlink(tmpfile1);
		return;
	}
    noscroll();
    oldmode = uinfo.mode;
    modify_user_mode(DICT);
    getyx(&save_y, &save_x);
    for(i=0;i<t_lines;i++)
        saveline(i, 0, save_scr[i]);

    while (1) {
        int len=0;
    	clear();
		move(1,10);
		prints("搜索SF");
        move(3,0);
        word[0]=0;
        while(1) {
            int ch;
            move(2,0);
			prints("请输入关键字:%s", word);
            clrtoeol();
            ch = igetkey();
            clear();
            if(ch==13||ch==10) break;
            else if(((ch==KEY_DEL)||(ch==8))&&len) {
                len--; word[len]=0;
            }
            else if(ch==KEY_TAB&&len) {
            }
            else if(isprint2(ch)&&!strchr("';\\\"/",ch)) {
	        if (len==WORDLEN-1) continue;
                word[len]=ch;
                len++;
                word[len]=0;
            }
		}
        if (word[0]==0) break;
        if (strchr(word,'\'')) break;
		do_search_sf(word, tmpfile1, tmpfile2);
	}

    for(i=0;i<t_lines;i++)
        saveline(i, 1, save_scr[i]);
    move(save_y, save_x);
	unlink(tmpfile1);
	unlink(tmpfile1);
    return;
}
