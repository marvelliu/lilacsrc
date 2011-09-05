#include "bbs.h"

#include <sys/types.h>
#include <dirent.h>

#define HOME "/home/bbsdata/home/"
int modify_user(const char *userid){
    int id;
    struct userec *lookupuser;

	//struct userdata ud;
    if (!(id = getuser(userid, &lookupuser))) {
        printf(MSG_ERR_USERID);
        return -1;
	}

	struct tm *time_str;
	time_t t = lookupuser->lastlogin;
	time_str = localtime(&t);
	time_str->tm_year=2007-1900;
	time_str->tm_mon=10-1;
	lookupuser->lastlogin=mktime(time_str);


	//lookupuser->stay=10000;
	//read_userdata(u->userid, &ud);
	//ud->numlogins = 100;
	//lookupuser->userdefine[1] |= DEF_HIDEIP;
	
	printf("modify user: %s\n", userid);
    update_user(lookupuser, id, 1);
	return 0;
}

int modify_alluser(){
	DIR* dp;
	struct dirent *dirp;
	char ptr[1024], tmp[1024], user[1024], i;
	struct stat statbuf;

	if(lstat(HOME, &statbuf)<0){
		printf(HOME " not exist!\n");
		return -1;
	}
	if(S_ISDIR(statbuf.st_mode)==0){
		printf(HOME " not a dir!\n");
		return -1;
	}

	strcpy(tmp, HOME);
	for(i='A';i<='Z';i++){
		tmp[strlen(HOME)] = i;
		tmp[strlen(HOME)+1] = 0;
		//printf("open dir %s\n", tmp);
		if((dp = opendir(tmp))==NULL){
			//printf("%s open error!\n", tmp);
			continue;
		}
		while((dirp = readdir(dp))!=NULL){
			if(strcmp(dirp->d_name,".") == 0 || strcmp(dirp->d_name,"..") == 0)
				continue;
			sprintf(ptr, "%s/%s", tmp, dirp->d_name);
			strcpy(user,dirp->d_name);
			//printf("open dir %s\n", ptr);
			if(lstat(ptr, &statbuf)<0){
				printf("%s not exist!\n", ptr);
				continue;
			}
			if(S_ISDIR(statbuf.st_mode)==0){
				printf("%s not a dir!\n", ptr);
				continue;
			}
			printf("modify user: %s\n", user);
			modify_user(user);
		}
	}
	return 0;

}
int main(int argc, char **argv)
{
	if(argc<2){
		printf("format error\n");
		return -1;
	}
	if (init_all()) {
		printf("init data fail\n");
		return -2; 
	}
	if(strcmp(argv[1],"all")==0)
		return modify_alluser();
	else
		return modify_user(argv[1]);
}
