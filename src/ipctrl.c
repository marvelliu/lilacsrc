//#include "bbs.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "bbs.h"


typedef struct __IPCTRL {
	unsigned int ip;
	unsigned int mask;
    char         perm[8];
} IPCTRL;



extern int ipctrl_check(char *sip, int type, session_t* session);
extern int load_ipctrl_conf();

void *get_ipctrl_shm()
{
	int  shmid;
	void *shmptr;
	
	shmid = shmget(IP_CTRL_SHM_KEY, sizeof(IPCTRL)*MAX_IPCTRL + 8, 0);
	if (shmid < 0)
	{
		shmid = shmget(IP_CTRL_SHM_KEY, sizeof(IPCTRL)*MAX_IPCTRL + 8, IPC_CREAT | 0660);
		if (shmid < 0)
			return NULL;
	}
	shmptr = (void *)shmat(shmid, 0, 0);
	if (shmptr == (void *)-1)
		return NULL;

	return shmptr;
}

int load_ipctrl_conf()
{
	FILE *fp;
	char buff[128], *sptr, *tok;
	int ip[4], count = 0;
	IPCTRL ipctl;

	if ( (fp = fopen("etc/ipctrl", "r")) == NULL )	
		return -1;

	sptr = (char *)get_ipctrl_shm();
	if (sptr == NULL)
	{
		fclose(fp);
		return -1;
	}
	memset(sptr, 0, sizeof(IPCTRL)*MAX_IPCTRL + 8);
	
	while (!feof(fp))
	{
		fgets(buff, 128, fp);
		tok = (char *)strtok(buff, " \r\n\t");
		if (!tok || (tok[0] == '#'))
			continue;
		
		memset((char *)&ipctl, 0, sizeof(ipctl));

		sscanf(tok, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    	ipctl.ip = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
		
		tok = (char *)strtok(NULL, " \r\n\t");
		if (!tok)
			continue;
		sscanf(tok, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    	ipctl.mask = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
		
		while ( (tok = (char *)strtok(NULL, " \r\n\t")) != NULL)
		{
			if (strstr(tok, "READ"))
				ipctl.perm[IPCTRL_READ] = 1;
			else if (strstr(tok, "WRITE"))
				ipctl.perm[IPCTRL_WRITE] = 1;
			else if (strstr(tok, "REG"))
				ipctl.perm[IPCTRL_REG] = 1;
		}
		memcpy(sptr+8 + count*sizeof(ipctl), (char *)&ipctl, sizeof(ipctl));
		count++;
	}
	*(int *)(sptr+4) = count;
	fclose(fp)	;

	shmdt(sptr);
	return 1;
}

int ipctrl_check(char *sip, int type, session_t *session)
{
	int count, i, ip[4];
	IPCTRL *ipctl;
	char *shmptr;
	unsigned int ips;
/*
	if ( HAS_PERM(currentuser, PERM_WELCOME|PERM_BOARDS|PERM_ACCOUNTS|
					PERM_SYSOP|PERM_ANNOUNCE|PERM_OBOARDS|PERM_ACBOARD|
                    PERM_ADMIN|PERM_NOIPCTRL) )
*/
	if (type != 2)
		if ( HAS_PERM(session->currentuser, PERM_BOARDS|PERM_NOIPCTRL) )
			return 1;

	shmptr = (char *)get_ipctrl_shm();
	if (!shmptr)
		return 1;

	sscanf(sip, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    ips = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
	
	count = *(int *)(shmptr+4);
	for (i = 0; i < count; i++)
	{
		ipctl = (IPCTRL *)(shmptr+8 + i * sizeof(IPCTRL));
		if ( ((ipctl->ip & ipctl->mask) == (ips & ipctl->mask)) || ipctl->mask == 0)
		{
			if (ipctl->perm[type])
			{
				shmdt(shmptr);
				return 1;
			}
			else
			{
				shmdt(shmptr);
				return 0;
			}
		}
	}
	shmdt(shmptr);
	return 0;
}

int ipctrl_check_reg(char *sip, int type)
{
	int count, i, ip[4];
	IPCTRL *ipctl;
	char *shmptr;
	unsigned int ips;

	shmptr = (char *)get_ipctrl_shm();
	if (!shmptr)
		return 1;

	sscanf(sip, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    ips = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];
	
	count = *(int *)(shmptr+4);
	for (i = 0; i < count; i++)
	{
		ipctl = (IPCTRL *)(shmptr+8 + i * sizeof(IPCTRL));
		if ( ((ipctl->ip & ipctl->mask) == (ips & ipctl->mask)) || ipctl->mask == 0)
		{
			if (ipctl->perm[type])
			{
				shmdt(shmptr);
				return 1;
			}
			else
			{
				shmdt(shmptr);
				return 0;
			}
		}
	}
	shmdt(shmptr);
	return 0;
}
#if 0
main()
{
	load_ipctrl_conf();
	printf("202.118.228.2 %d\n", ipctrl_check("202.118.228.2", 0));
	printf("202.118.224.2 %d\n", ipctrl_check("202.118.224.2", 1));
	printf("202.118.236.2 %d\n", ipctrl_check("202.118.236.2", 2));
	printf("202.11.228.2 %d\n", ipctrl_check("202.11.228.2", 0));
	printf("202.11.228.2 %d\n", ipctrl_check("202.11.228.2", 1));
	printf("202.11.228.2 %d\n", ipctrl_check("202.11.228.2", 2));
}
#endif
