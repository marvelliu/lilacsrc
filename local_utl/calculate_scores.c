/* solit, 2008.04.15, �����û�����... */
#include "bbs.h"
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define CA_DEBUG
#define CA_LOG
 
int score_update_from_file( const char* filename );

struct userscore{
	char userid[IDLEN+2];
	int base_score; //��������,��ͨ�����ֻ�����ʽ����
	int ex_score;   //�������,���ֽ��׻�һ����ܶ�
	int old_th;     //���յ�������ʱ��
	int old_tt;     //���յ���վ�ܴ���
	int old_tp;	//���յķ�����������
	short int rank;       //��������
};
struct score_sort{
	int i;
	struct userscore* s;
};
static struct userec *userlist[MAXUSERS];
static struct userscore scorelist[MAXUSERS];
static struct score_sort p_scorelist[MAXUSERS]; //ָ��ԭ��¼����
static int count;
int is_first_init;
FILE* logfd;
static time_t curtime;
struct tm* local_time;

static int create_userlist(struct userec *user,void *varg){
	userlist[count++]=user;
	return 0;
}

int read_userscore_from_file(const char* us_file,  struct userscore us[])
{
	int fd;
	int flag;
//	struct stat buf;
//#ifdef CA_LOG
//	struct tm* local_time = localtime(&curtime);
//#endif
	bzero(us, sizeof(struct userscore)*MAXUSERS);
	if( (fd = open(us_file, O_RDONLY, 0644)) < 0 ){
		if ( (fd = open(us_file, O_RDWR| O_CREAT, 0655)) < 0 )//��������ļ������ڣ��򴴽�һ��
			return -1;
		write(fd, us, sizeof(struct userscore)*MAXUSERS);
		fchown(fd,BBSUID,BBSGID);
		is_first_init = 1;
		close(fd);
		return fd;
	}
	/*
	flag = fstat( fd, &buf );
	if ( buf.st_ctime/3600/24 == curtime/3600/24 )
	{
#ifdef CA_LOG
		fprintf(logfd, "[%02u/%02u %02u:%02u]:\t\tCan't caculate scores second time a day\n", 
				local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min);
		fflush(logfd);
		fclose(logfd);
#endif
		exit(1);
	}*/
	flag = read(fd, us, sizeof(struct userscore)*MAXUSERS);
	close(fd);
	return flag;
}
int write_userscore(const char* us_file, struct userscore us[])
{
	size_t flag;
	int fd;
	if( (fd = open(us_file, O_WRONLY, 0644)) < 0 ){
		return -1;
	}
	flag = write(fd, us, sizeof(struct userscore)*MAXUSERS);
	fchown(fd,BBSUID,BBSGID);
	close(fd);
	if(flag>0)
		return 1;
	else
		return -1;
}
int calculate_base_score(int idx, struct userec* user)
{
	int td = 0; //ע��������
	int tmp_td;
	float th; //����վʱ��
	int tt; //����վ����
	int tp; //��������
	int d; //��վ����
	float h; //ÿ����վСʱ
	int t; //��վ����
	int p; //��������
	int y[2]={0}; //y=sqrt(d+h/24+t/5+5*p/50)
	int dify=0;
	int l=0;//����
	float tmp;
	td = difftime(curtime,user->firstlogin)/3600/24 + 1;
	tmp_td = 0;
	th = user->stay/3600;
	tt = user->numlogins;
	tp = user->numposts;
	scorelist[idx].old_th = th;
	scorelist[idx].old_tt = tt;
	scorelist[idx].old_tp = tp;
#ifdef CA_DEBUG
	if(td<0)
		printf("error: %s\n", ctime(&user->firstlogin));
	else
		printf("ע��������: %d\n", td);
	printf("����վʱ��: %f Сʱ\n", th);
	printf("����վ����: %d\n",tt);
	printf("��������Ŀ: %d\n", tp);
#endif
	if( tt<td && tp<td )
	{
		td = tp>tt?tp:tt;
		th = td>th?td:th;
		printf("��Ծ����: %d\n", td);
		printf("��Ծʱ��: %d\n", td);
	}
	else if( tt/td < 2 || tp/td < 4 )
	{
		td = td/4*3;
	}

	if( td <= 0 )
		return 0;
	l=0;
	for(d=0, h=0, t=0, p=0; 
			d<=td;
			d++,h+=(th/(float)td),t+=(tt/td),p+=(tp/td))
	{
		y[0] = y[1];
		y[1] = sqrt((double)d+h/24.f+(double)t/5.f+5.f*(double)p/50.f);
		if(d>=2)
		{
			tmp = 50.f*log(((double)d+1.f)*(double)(y[1]-y[0]));
			dify=(int)round(tmp);
			if(dify<0)dify=0;
			l+=dify;
		}
	}
	/*
	for(i=1;i<td;i++)
	{
		tmp = 50.f*log(((double)i+1.f)*(double)(y[i+1]-y[i]));
		dify[i]=(int)round(tmp);
		if(dify[i]<0)
			dify[i]=0;
		l=l+dify[i];
	}*/

	return l;
}
int calculate_ex_score(int idx, struct userec* user)
{

	int td = 0; //ע��������
	float th, tth[3]; //����վʱ��
	int tt, ttt[3]; //����վ����
	int tp, ttp[3]; //��������
	int l;          //�ܻ���
	td = difftime(curtime,user->firstlogin)/3600/24 + 1;
	th = user->stay/3600;
	tt = user->numlogins;
	tp = user->numposts;
	tth[0] = th - scorelist[idx].old_th;
	ttt[0] = tt - scorelist[idx].old_tt;
	ttp[0] = tp - scorelist[idx].old_tp;

#ifdef CA_DEBUG
	printf("��������ʱ��: %fСʱ\n", tth[0]);
	printf("�������ߴ���: %d    \n", ttt[0]);
	printf("���շ�������: %d    \n", ttp[0]);

#endif

	if( ttt[0] == 0 || ttp[0] == 0 )
		return 0;
	if( td == 0 || td == 1 )
		return 0;
	tth[1] = (td-2)*(scorelist[idx].old_th)/(td-1);
	ttt[1] = (td-2)*(scorelist[idx].old_tt)/(td-1);
	ttp[1] = (td-2)*(scorelist[idx].old_tp)/(td-1);

	tth[2] = (td-1)*th/td;
	ttt[2] = (td-1)*tt/td;
	ttp[2] = (td-1)*tp/td;

	l = (int)round( 50.f * log(td) * ( sqrt( td + tth[2]/24 + ttt[2]/5 + 5 * ttp[2]/50)
				- sqrt( td - 1 + tth[1]/24 + ttt[1]/5 + 5 * ttp[1]/50)));

	//���ݵ��յ�th,tt,tp, �����ռ���ʹ��
	scorelist[idx].old_th = th;
	scorelist[idx].old_tt = tt;
	scorelist[idx].old_tp = tp;

	//������������100������0
	l = l>100?100:l;
	l = l>0?l:0;

	return l;
}
int cmp_base_score(const void *e1, const void *e2)
{
	return (*(struct score_sort *)e2).s->base_score - (*(struct score_sort *)e1).s->base_score;
}
int cmp_ex_socre(const void *e1, const void *e2)
{
	return (*(struct score_sort *)e2).s->ex_score - (*(struct score_sort *)e1).s->ex_score;
}
int main(int argc, char* argv[])
{
	static int idx;
	int flag;
	int ex_score = 0;	
	int num_of_users = 0;
	int is_update = 0;
	FILE* top50_fd;
	char fname[STRLEN];
	//FILE* top100ex_fd;

#ifdef CA_LOG
	char* log_file_name = (char *)malloc(sizeof(char) * 20);
#endif

    if(setgid(BBSGID)==-1)
        exit(8);
    if(setuid(BBSUID)==-1)
        exit(8);

	curtime = time(NULL);
	local_time = localtime(&curtime);
	is_first_init = 0;

	count = 0;
	if(chdir(BBSHOME)==-1)
		return -1;
	if (init_all()) {
		printf("init data fail\n");
		return -2; 
	}
	apply_users(create_userlist,NULL);

	if( argc == 2)
	{
		if ( strncmp(argv[1], "--init", 6) == 0 ) 
			is_first_init = 1; 
		if ( strncmp( argv[1], "--update", 8 ) == 0 )
			is_update = 1;
	}


#ifdef CA_LOG
	sprintf( log_file_name, "scores.log.%d%02d%02d", 
			1900+local_time->tm_year, 1+local_time->tm_mon, local_time->tm_mday);
	logfd = fopen( log_file_name, "r" );
	if (logfd > 0)
	{
		fclose(logfd);
		logfd = fopen( log_file_name, "a+" );
		fprintf(logfd, "[%02u/%02u %02u:%02u]:\t\tCan't caculate scores second time a day\n", 
				local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min);
		fclose(logfd);
		exit(1);
	}
	logfd = fopen( log_file_name, "a+" );
#endif

	flag = read_userscore_from_file(".SCORES", scorelist);
	/*�����ֲ����ļ������û�������Ӧ�Ļ�������*/
	if ( score_update_from_file("score_update.txt") == 0 )
	{
#ifdef CA_LOG
		fprintf( logfd, "No score update today\n" );
#endif
	}
	if ( is_update )
	{
		exit(1);
	}
	/*���ļ�����*/
	
	for(idx=0;idx<count;idx++)
	{
		if(userlist[idx]->userid[0])
		{
			
#ifdef CA_DEBUG
			printf("------------------------\n");
			printf("[%s]\n", userlist[idx]->userid);
			printf("[%s]\n", userlist[idx]->username);
			printf("��ʼ���֣�%d\n", scorelist[idx].base_score);
#endif
			
			ex_score = 0;
			if(is_first_init)
			{
				//��һ�δ����ļ���ʱ�򣬰Ѷ�Ӧ���û�ID������ȥ
				strncpy(scorelist[idx].userid, userlist[idx]->userid, sizeof(scorelist[idx].userid) - 1);
				scorelist[idx].userid[sizeof(scorelist[idx].userid) - 1] = '\0';
				//�������������
				scorelist[idx].base_score = calculate_base_score(idx, userlist[idx]);
				scorelist[idx].ex_score = 0;
			}
			else 
			{
				if( scorelist[idx].userid[0] ){
					if( strcmp(userlist[idx]->userid, scorelist[idx].userid) != 0 )
					{
					//�ⲿ��������֤�û���ID�Ƿ����
#ifdef CA_DEBUG
						printf("user id error\n");
#endif
						strncpy(scorelist[idx].userid, userlist[idx]->userid, sizeof(scorelist[idx].userid) - 1);
						scorelist[idx].userid[sizeof(scorelist[idx].userid) - 1] = '\0';
						scorelist[idx].base_score = 0;
						scorelist[idx].ex_score = 0;
						continue;
					}
				}
				else
				{
					//˵����������ע���û�����Ҫ�����û�ID
					strncpy(scorelist[idx].userid, userlist[idx]->userid, sizeof(scorelist[idx].userid) - 1);
					scorelist[idx].userid[sizeof(scorelist[idx].userid) - 1] = '\0';
					scorelist[idx].base_score = 0;
					scorelist[idx].ex_score = 0;
					continue;//���û������ټ��������
				}
				//���ǵ�һ�δ����ļ������������û��������������
				if( strcmp("guest",scorelist[idx].userid)==0 )
				{
					scorelist[idx].base_score = 0;
					scorelist[idx].ex_score = 0;
					continue;
				}
				scorelist[idx].base_score = userlist[idx]->score;
				ex_score = calculate_ex_score(idx, userlist[idx]);
				scorelist[idx].base_score += ex_score;
				scorelist[idx].ex_score = ex_score;
			}
#ifdef CA_DEBUG
			printf("���ӻ���: %d\n", scorelist[idx].ex_score);
			printf("�ܻ���: %d\n", scorelist[idx].base_score);
#endif
			p_scorelist[num_of_users].s = &scorelist[idx];
			p_scorelist[num_of_users].i = idx;
			num_of_users ++;
		}
		else
		{
			if(is_first_init)
			{
				scorelist[idx].base_score = 0;
				scorelist[idx].ex_score = 0;
				userlist[idx]->score_rank = 0;
				userlist[idx]->score = 0;
			}
		}
	}


/*******************************************************
 *  ����ʹ�ÿ⺯��qsort�Ի��ֽ�������
 *  ͬʱ�����־
 *  ��ÿ��Top50���������������������ĵ�ScoreRecords��
 ******************************************************/

	// ��������
	qsort(p_scorelist, num_of_users, sizeof(struct score_sort), cmp_base_score);
	//�����־
	flag = write_userscore(".SCORES", scorelist);
	for(idx=0; idx<num_of_users; idx++)
	{
		p_scorelist[idx].s->rank = idx;
#ifdef CA_LOG
		fprintf(logfd, "[%02u/%02u %02u:%02u]:\t",
				local_time->tm_mon+1, local_time->tm_mday, 
				local_time->tm_hour, local_time->tm_min);
		fprintf(logfd,"%-16s", p_scorelist[idx].s->userid);
		fprintf(logfd,"=%-10d",p_scorelist[idx].s->base_score);
		fprintf(logfd,"+%-10d",p_scorelist[idx].s->ex_score);
		fprintf(logfd,"rank=%-6d",p_scorelist[idx].s->rank);
		userlist[ p_scorelist[idx].i ]->score_rank = (num_of_users-idx)*100000/num_of_users;
		fprintf(logfd,"%d.%d%%\n", userlist[ p_scorelist[idx].i ]->score_rank/1000, userlist[ p_scorelist[idx].i ]->score_rank%1000);
		userlist[ p_scorelist[idx].i ]->score = p_scorelist[idx].s->base_score;
#endif
	}
#ifdef CA_DEBUG
	if(flag<=0)
		printf("write error\n");
#endif
#ifdef CA_LOG
	fflush(logfd);
	fclose(logfd);
#endif
	//������ScoreRecords
	top50_fd = fopen("etc/posts/top50score","w+");
	fprintf(top50_fd, "���� ����            �ǳ�                                     ����      \n");
	fprintf(top50_fd, "==== ======================================================== ==========\n");
	for(idx=0; idx<50; idx++)
		fprintf(top50_fd, "[%2d] %-16s%-40s %-10d\n", 
				idx+1, p_scorelist[idx].s->userid, 
				userlist[ p_scorelist[idx].i ]->username,
				p_scorelist[idx].s->base_score);
	fflush(top50_fd);
	fclose(logfd);
	sprintf(fname, "%d��%2d��%2d�ջ�����������Top 50", local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday);
//	post_file(NULL, "", "etc/posts/top50score", "BBSLists", fname, 0, 1, getSession());
	post_file(NULL, "", "etc/posts/top50score", "ScoreRecords", fname, 0, 1, getSession());

	//��������
	qsort(p_scorelist, num_of_users, sizeof(struct score_sort), cmp_ex_socre);
	//������ScoreRecords
	top50_fd = fopen("etc/posts/top50exscore","w+");
	fprintf(top50_fd, "���� ����            �ǳ�                                     ����      \n");
	fprintf(top50_fd, "==== ======================================================== ==========\n");
	for(idx=0; idx<50; idx++)
		fprintf(top50_fd, "[%2d] %-16s%-40s %-10d\n", 
				idx+1, p_scorelist[idx].s->userid, 
				userlist[ p_scorelist[idx].i ]->username,
				p_scorelist[idx].s->ex_score);
	fflush(top50_fd);
	fclose(logfd);
	sprintf(fname, "%d��%2d��%2d�ջ�����������Top 50", local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday);
//	post_file(NULL, "", "etc/posts/top50exscore", "BBSLists", fname, 0, 1, getSession());
	post_file(NULL, "", "etc/posts/top50exscore", "ScoreRecords", fname, 0, 1, getSession());

	return 0;
}

int get_user_score(const char* userid)
{
	struct userscore u_score[MAXUSERS];
	int fd;
	int uidx;
	int flag;
	uidx = searchuser(userid) - 1; 
	fd = open(".SCORES", O_RDONLY, 0655);
	if(fd < 0)
	{
		return 0;
	}
	flag = read(fd, u_score, sizeof(struct userscore)*MAXUSERS);
	close(fd);
	if(flag <= 0)
	{
		return 0;
	}
	if(u_score[uidx].userid[0] == 0)
	{
		return 0;
	}
	return u_score[uidx].base_score + u_score[uidx].ex_score;
}
int update_user_score(const char* userid, int score)
{
	int uidx = searchuser(userid) - 1;
	
	if(uidx < 0)
		return -1;
	userlist[uidx]->score += score;
	scorelist[uidx].base_score = userlist[uidx]->score;
	
	return uidx;
}
int score_update_from_file( const char* filename )
{
	FILE* fd = fopen(filename,"r");
	FILE* update_fd;
	char  uid[IDLEN+2];
	char  reason[400];
	char  fname[STRLEN];
	int   score;
	int   flag;

	if ( fd == NULL )
	{
		printf("Cannot read file %s", filename);
		return 0;
	}

#ifdef CA_LOG
	fprintf( logfd, "------------------------------------------\n" );
#endif
	while ( fgets( reason, 400, fd ) )
	{
		update_fd = fopen("etc/posts/update_score","w+");
		sscanf( reason, "%s %d", uid, &score );
		if ( fgets( reason, 400, fd ) == NULL )
		{
			return 0;
		}
		if ( score > 0)
		{
#ifdef CA_LOG
			fprintf( logfd, "%s  +%d \n", uid, score );
#endif
			sprintf( fname, "[����] ���� %s ���� %d ��", uid, score );
		}
		else
		{
#ifdef CA_LOG
			fprintf( logfd, "%s  %d \n", uid, score );
#endif
			sprintf( fname, "[����] �۳� %s ���� %d ��", uid, score );

		}
			update_user_score( (const char*)uid, score );
		fprintf( update_fd, "˵����\n");
		fprintf( update_fd, "        %s\n", reason);
		fclose(update_fd);
		post_file(NULL, "", "etc/posts/update_score", "ScoreRecords", fname, 0, 1, getSession());
	}
#ifdef CA_LOG
	fprintf( logfd, "------------------------------------------\n" );
#endif
	fclose(fd);
	sprintf( fname, "%s.%04u%02u%02u.bak", 
			filename, local_time->tm_year + 1900,
			local_time->tm_mon + 1,
			local_time->tm_mday
			);
	flag = rename( filename, fname );
	if ( flag < 0 )
	{
		perror( "rename successfully\n" );
	}
	return 1;
}

