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

//#define IP "192.168.1.104"
//#define IP "172.18.208.28"
//#define IP "172.18.208.226"
#define IP "121.42.62.140"
//#define DEBUG_WITHOUT_SERVER

int device_regist();
int get_device_ip(char *ipbuf);
void get_pi_ip();
void tcp_connect(char *ipaddr);
void *tcp_check_function(void *ptr);
void tcp_check_pthread_creat();
void tcp_sent_you_can_see_me();

extern pthread_t tcp_check_pthread;
#endif
