/* etnlegend, 2006.09.10, 导出用户身份设定数据... */
#include "bbs.h"


int main(int argc, char **argv){
    struct userec* user;
    struct userdata ud;

    if(chdir(BBSHOME)==-1)
    {
        printf("unable to chdir %s\n", BBSHOME);
        return -1;
    }
    resolve_ucache();


    char* userid = argv[1];
    if(!getuser(userid, &user)){
        fprintf(stderr, "%s\n", "locate specified user ...");
        return -1;
    }
    read_userdata(userid, &ud);
    

    fprintf(stdout,"%s\n","---- Current User Title Configuration ----");
    printf("userid:%s\n", userid);
    printf("realname:%s\n", ud.realname);
    printf("email:%s\n", ud.email);
    printf("reg_email:%s\n", ud.reg_email);


//    struct usermemo *memo;
//    read_user_memo(userid, &memo);
//    printf("email:%s\n", memo->ud.email);
//    printf("reg_email:%s\n", memo->ud.reg_email);


    printf("\n");

    return 0;
}

