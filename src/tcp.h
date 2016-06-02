#ifndef _TCP_H
#define _TCP_H

extern const int MAX_LINE;
extern const int PORT;
extern const int BACKLOG;
extern const int LISTENQ;
extern const int MAX_CONNECT;
extern char Device_IP[32]; // ipv4

//#define IP "192.168.23.1"
#define IP "127.0.0.1"
#define DEBUG_WITHOUT_SERVER
int device_regist(int sockfd);
int get_device_ip(char *ipbuf, int sockfd);
void tcp_connect(char *ipaddr);
int is_connected(int sockfd);
#endif
