#include <stdio.h>
#include "iplook.h"

void dolook(char *str)
{
	char *area, *location;
	if (0 != LookIP(str,&area,&location)) {
		printf("%s λ�ò���\n",str);
	}
	else printf("%s λ�� %s �� %s\n", str,area,location);
}
int main(int argc, char *argv[])
{
	char buf[255];
	printf("IP ��Ϣ��ѯ��%s������ IP ���� %d ��\n", GetIPInfoVersion(), GetIPInfoCount()); 
	if (argc>1) {
		dolook(argv[1]);
	} else {
		fgets(buf,255,stdin);
		strtok(buf,"\r\n \t");
		dolook(buf);
	}
}
