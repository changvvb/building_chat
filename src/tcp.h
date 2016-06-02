#ifndef _TCP_H
#define _TCP_H

extern const int MAX_LINE;
extern const int PORT;
extern const int BACKLOG;
extern const int LISTENQ;
extern const int MAX_CONNECT;
extern char Device_IP[32]; // ipv4

#define IP "192.168.23.1"

int first_request(int sockfd);
int get_device_ip(char *ipbuf, int sockfd);
void tcp_client(char *ipaddr);
#endif
