/* iplen 16->46 */

#include "bbs.h"
#include "stdio.h"

struct userec1 {                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    char flags; /*һЩ��־����������������֮���*/
    unsigned char title; /*�û�����*/
    time_t firstlogin;
    char lasthost[16];
    unsigned int numlogins;
    unsigned int numposts;
#ifdef CONV_PASS
    char passwd[OLDPASSLEN];
    char unused_padding[2];
#endif
    char username[NAMELEN];
    unsigned int club_read_rights[MAXCLUB>>5];
    unsigned int club_write_rights[MAXCLUB>>5];
    unsigned char md5passwd[MD5PASSLEN];
    unsigned userlevel;
    time_t lastlogin;
    time_t stay;
    int signature;  
    unsigned int userdefine[2];
    time_t notedate;
    int noteline;
    int notemode;
    time_t exittime;
    /* ��������ת�Ƶ� userdata �ṹ�� */
    unsigned int usedspace;     /* used space of user's mailbox, in bytes */
#ifdef HAVE_USERMONEY
    int money;
    int score;
    char unused[20];
#endif
};

struct userec2 {                 /* Structure used to hold information in */
    char userid[IDLEN + 2];     /* PASSFILE */
    char flags; /*һЩ��־����������������֮���*/
    unsigned char title; /*�û�����*/
    time_t firstlogin;
    char lasthost[IPLEN];
    unsigned int numlogins;
    unsigned int numposts;
#ifdef CONV_PASS
    char passwd[OLDPASSLEN];
    char unused_padding[2];
#endif
    char username[NAMELEN];
    unsigned int club_read_rights[MAXCLUB>>5];
    unsigned int club_write_rights[MAXCLUB>>5];
    unsigned char md5passwd[MD5PASSLEN];
    unsigned userlevel;
    time_t lastlogin;
    time_t stay;
    int signature;  
    unsigned int userdefine[2];
    time_t notedate;
    int noteline;
    int notemode;
    time_t exittime;
    /* ��������ת�Ƶ� userdata �ṹ�� */
    unsigned int usedspace;     /* used space of user's mailbox, in bytes */
#ifdef HAVE_USERMONEY
    int money;
    int score;
    char unused[20];
#endif
};

int main(int argc , char* argv[])
{
        FILE * fp,*fp2;
        struct userec1 bh;
        struct userec2 bhnew;

        int i;

	if(argc !=3 )
	{
		printf("usage: convert Old_PASSWDS_FILE New_PASSWDS_FILE\n");
		exit(0);
	}
	if((fp=fopen(argv[2],"r"))!=NULL){
		printf("NEW FILE exist!");
		fclose(fp);
		exit(0);
	}
        if((fp = fopen(argv[1],"r")) == NULL)
        {
                printf("open .BOARDS file failed!");
                exit(0);
        }

        if((fp2 = fopen(argv[2],"w")) == NULL)
        {
                printf("cant create newboards file!");
                exit(0);
        }

        while( fread(&bh,sizeof(struct userec1),1,fp) ){
        	memset(&bhnew,0,sizeof(struct userec2));
			memcpy(&bhnew, &bh, sizeof(struct userec1));
			bhnew.numlogins = bh.numlogins;
			bhnew.numposts = bh.numposts;
#ifdef CONV_PASS
			memcpy(bhnew.passwd, bh.passwd, sizeof(bhnew.passwd));
			memcpy(bhnew.unused_padding, bh.unused_padding,  sizeof(bhnew.unused_padding));
#endif
			memcpy(bhnew.username, bh.username, sizeof(bhnew.username));
			memcpy(bhnew.club_read_rights, bh.club_read_rights, sizeof(bhnew.club_read_rights));
			memcpy(bhnew.club_write_rights, bh.club_write_rights, sizeof(bhnew.club_write_rights));
			memcpy(bhnew.md5passwd, bh.md5passwd, sizeof(bhnew.md5passwd));
			bhnew.userlevel	= bh.userlevel;		
			bhnew.lastlogin = bh.lastlogin;
			bhnew.stay = bh.stay;
			bhnew.signature = bh.signature;
			bhnew.userdefine[0] = bh.userdefine[0];
			bhnew.userdefine[1] = bh.userdefine[1];
			bhnew.notedate = bh.notedate;
			bhnew.noteline = bh.noteline;
			bhnew.notemode = bh.notemode;
			bhnew.exittime = bh.exittime;
			bhnew.usedspace = bh.usedspace;
#ifdef HAVE_USERMONEY
			bhnew.money = bh.money;
			bhnew.score = bh.score;
			memcpy(bhnew.unused, bh.unused, 20);
#endif
        	fwrite(&bhnew,sizeof(struct userec2),1,fp2);
		}

        fclose(fp2);
        fclose(fp);
        return 0;
}

