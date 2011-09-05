#ifndef XXJH_TCPLIB_H
#define XXJH_TCPLIB_H

#include "bbs.h"

/**
 * tcplib �õ��Ĵ����.
 */
enum TCPLIB_ERRNO
{
	ERR_TCPLIB_CONNECT    = -2,	 /** ����ʧ�� */
	ERR_TCPLIB_TIMEOUT    = -3,	 /** ��ʱ */
	ERR_TCPLIB_RECVFAILED = -4,	 /** ���ձ���ʧ�� */
	ERR_TCPLIB_SENTFAILED = -5,	 /** ���ͱ���ʧ�� */
	ERR_TCPLIB_VERSION    = -10, /** �汾���� */
	ERR_TCPLIB_HEADER     = -11, /** ����ͷ��ʽ���� */
	ERR_TCPLIB_TOOLONG    = -12, /** �������ݹ��� */
	ERR_TCPLIB_OPENFILE   = -20, /** ���ļ����� */
	ERR_TCPLIB_RESUMEPOS  = -21, /** ����λ�ô��� */
    ERR_TCPLIB_OTHERS     = -100 /** �������󣬿ɴ� errno ��ô�����Ϣ */
};

#ifdef __cplusplus
const int TCPLIB_TRUE  = 1;
const int TCPLIB_FALSE = 0;
const int TIME_CNV_RATIO   = 1000;	/* ʱ��ת���ʣ���->���룬����->΢�� */
#else
#define TCPLIB_TRUE  1
#define TCPLIB_FALSE 0
#define TIME_CNV_RATIO   1000	/* ʱ��ת���ʣ���->���룬����->΢�� */

#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * ���ڰѺ���ֵת���� timeval �ṹ��.
 * �����߱����ṩһ�� timeval �ṹ��Ļ�������
 * ���Ѹû���������ʼ��ַ����.
 * 
 * @param ms ��ת���ĺ���ֵ.
 * @param tv ָ�� timeval �ṹ���ָ��.
 * @see TimevalToMs()
 */
void MsToTimeval(const int ms, struct timeval *tv);

/**
 * ���ڰ� timeval �ṹ��ת���ɺ���ֵ.
 * 
 * @param tv ָ���ת���� timeval �ṹ���ָ��.
 * @return ת����ĺ�����ֵ
 * @see MsToTimeval()
 */
int TimevalToMs(const struct timeval *tv);

/**
 * �������� timeval �ṹ������.
 * �������ṩ���� timeval �ṹ��ָ�������ת��ʱ��
 * �ɵ�һ��������ȥ�ڶ�����������������ʱ����ת��
 * �������ڵ�һ������.
 * 
 * @param ltv �� timeval �ṹ��ָ�루�ڼ��ŵ���ߣ��൱�ڱ�������
 * @param rtv �� timeval �ṹ��ָ�루�ڼ��ŵ��ұߣ��൱�ڼ�����
 */
void TimevalSubtract(struct timeval *ltv, 
							 const struct timeval *rtv);

/**
 * �������͵� TCP ���Ӻ���.
 * ��������÷����׼�� connect() �����ǳ����ƣ�Ψһ�����������û�
 * �����Լ�ָ�� TCP ���ӹ��̵ĳ�ʱʱ�䡣��������ʱ����û�ܳɹ���
 * ������Ŀ�꣬�����س�ʱ����ǰ������������������ connect() ��ȫ
 * ��ͬ�����һ�������ǳ�ʱʱ�䡣�������������ʵ����ο� UNPv1e2
 * �� 411 ҳ��
 * 
 * @param sockfd socket ������
 * @param saptr ָ����Ŀ��������ַ�ṹ��ָ��
 * @param salen ��ַ�ṹ�ĳ���
 * @param msec ���ӹ��̵ĳ�ʱʱ�䣬��λ����
 * @return ==0 ���ӳɹ�
 *         <0  ����ʧ��
 * @see connect()
 */
int NonBlockConnect(int sockfd, const struct sockaddr *saptr,
		socklen_t salen, const int msec);

int NonBlockConnectEx(int sockfd, const struct sockaddr *saptr,
		socklen_t salen, const int msec, const int conn);

/**
 * ����ʱ���Ƶ� TCP ���Ӻ���.
 * ��������Ƕ� NonBlockConnect() �ķ�װ���û�ֻ���ṩĿ��������IP
 * ��ַ���˿ں��Լ���ʱʱ�䣬�Ϳ�����ɸ��ӵ� TCP ���ӹ��̡�
 * 
 * @param ip Ŀ�������� IP ��ַ
 * @param port Ŀ�������Ķ˿ں�
 * @param msec ���ӹ��̵ĳ�ʱʱ�䣬��λ����
 * @return >=0 ���ӳɹ�������ֵΪ���ں�Ŀ������ͨѶ�� socket ������
 *         <0  ����ʧ��
 * @see NonBlockConnect()
 */
int DoConnect(const char *ip, const int port, const int msec);

/**
 * �Ͽ� TCP ���ӵĺ���.
 * 
 * @param conn_fd ���Ͽ��� TCP ���ӵ�������
 */
void DoDisconnect(int conn_fd);

/**
 * �����ź��ж�Ӱ��� select() ����.
 * ���������Ϊ SignalSafeSelect()������˼�壬����֪����������ǲ���
 * �ź��жϵ�Ӱ��ġ���ʵ�����ǶԱ�׼ select() �ķ�װ����С�ĵؼ��
 * select() ���صĴ���ֵ��������ִ����������ź��ж�����ģ����Ե���
 * ���ź��жϴ������µ��� select()�����������ʹ�÷����� select()
 * ��ȫһ�¡�
 * 
 * @param nfds ��Ҫ������������Ŀ
 * @param rs ���ڼ���Ƿ�ɶ�������������
 * @param ws ���ڼ���Ƿ��д������������
 * @param es ���ڼ���Ƿ��쳣������������
 * @param tv ��ʱʱ��
 * @return >0  �Ѿ�׼����(�ɶ�����д���쳣�����ǵ����)������������
 *         ==0 ��ʱ
 *         <0  ����
 * @see select()
 */
int SignalSafeSelect(int nfds, fd_set *rs, fd_set *ws, fd_set *es,
		struct timeval *tv);

/**
 * ����ȫ�� accept() ����.
 * ����˼�壬SafeAccept() ��ʾ��������ǡ���ȫ�ġ�����Ȼ�������Ǿ���
 * �İ�ȫ�����Ǳȱ�׼�� accept() Ҫ��ȫ��������������ź��жϵ�Ӱ�죬
 * ����С�ĵش����� accept() ���صĴ�������������÷��� accept() 
 * ��ȫ��ͬ����������ʵ�֣���ο� UNPv1e2 �� 422 ҳ��
 * 
 * @param sockfd Listening socket ������
 * @param cliaddr �ͻ��˵ĵ�ַ�ṹ
 * @param addrlen �ͻ��˵ĵ�ַ�ṹ�ĳ���
 * @return >=0 ������������ɹ�������ֵΪ���ںͿͻ���ͨѶ�� socket
 *             ������
 *         <0  ����
 * @see accept()
 */
int SafeAccept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);

/**
 * ����ʱ���Ƶı��ķ��ͺ���.
 */
int DoSendData(int sockfd, const void *buf, const size_t len, int msec);

/**
 * ����ʱ���Ƶı��Ľ��պ���.
 */
int DoRecvData(int sockfd, void *buf, const size_t len, int msec);

#ifdef __cplusplus
}
#endif

#endif /* XXJH_TCPLIB_H */

