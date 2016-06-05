#ifndef _TCP_H
#define _TCP_H
#include <pthread.h>

extern const int MAX_LINE;
extern const int PORT;
extern const int BACKLOG;
extern const int LISTENQ;
extern const int MAX_CONNECT;
extern char Device_IP[32]; // ipv4
extern int sockfd;

//#define IP "192.168.23.1"
//#define IP "172.18.208.28"
#define IP "192.168.1.113"
//#define IP "127.0.0.1"
//#define DEBUG_WITHOUT_SERVER

int device_regist();
int get_device_ip(char *ipbuf, int sockfd);
void tcp_connect(char *ipaddr);
void *tcp_check_function(void *ptr);
int tcp_check_pthread_creat();
int tcp_sent_you_can_see_me();

extern pthread_t tcp_check_pthread;
#endif
