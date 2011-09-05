#ifndef IPLOOK_H
#define IPLOOK_H

#ifdef __cplusplus
extern "C" {
#endif 
/* 
 *  *
 *  *   IP ��ѯ�ӿڣ� kxn@smth
 *  *    
 *  *     @param  const char *ipstr      IP �ַ���
 *  *     @param  char ** area           ���� area ���ַ���ָ��
 *  *     @param  char ** location       ���� location ���ַ���ָ��
 *  *     
 *  *     @result int                    0 ���ѯ�ɹ��� -1 ���Ҳ���
 *  * 
 *  * */
	
int LookIP(const char *ipstr, char **area, char **location);

/*
 *  IP ��Ϣ���ڣ��� QQWry.Dat ��ȡ��
 */  

char *GetIPInfoVersion();

/*
 *  IP ��Ϣ�������� convert ����
 */

int GetIPInfoCount();

#ifdef __cplusplus
};
#endif
	
#endif 

