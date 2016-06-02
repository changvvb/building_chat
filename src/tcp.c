#include "tcp.h"
#include "config.h"

const int MAX_LINE = 2048;
const int PORT = 10000;
const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;

char Device_IP[32]; // ipv4

void fix_string(char *str) {
  char *p;
  for (p = str; *p != '\n' && *p != '\r'; p++)
    ;
  *p = '\0';
}

/*
return 0: OK
       1: failed
sent MEDIA_DEVICE
*/
int first_request(int sockfd) {
  char RevBuf[32];
  write(sockfd, "MEDIA_DEVICE\r\n", 24);
  while (!read(sockfd, RevBuf, 32)) {
    /* code */
    printf("RevBuf : %s\n", RevBuf);
  }
  if (strcmp("0", RevBuf) == 0)
    return 0;
  else
    return 1;
}

/*sent GET_IP_FROM_PI*/
int get_device_ip(char *ipbuf, int sockfd) {
  write(sockfd, "GET_IP_FROM_PI\r\n", 24);
  while (!read(sockfd, ipbuf, 32))
    printf("%s\n", ipbuf);
  printf("recive done\n ip:%s", ipbuf);
  fix_string(ipbuf);
}

void tcp_client(char *ipaddr) {
  /*声明套接字和链接服务器地址*/
  int sockfd;
  struct sockaddr_in servaddr;

  /*判断是否为合法输入*/
  if (ipaddr == NULL) {
    perror("ipaddr == NULL");
    exit(1);
  } // if

  /*(1) 创建套接字*/
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(1);
  } // if

  /*(2) 设置链接服务器地址结构*/
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) < 0) {
    printf("inet_pton error for %s\n", ipaddr);
    exit(1);
  } // if

  /*(3) 发送链接服务器请求*/
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect error");
    exit(1);
  } // if

  /*(4) 消息处理*/
  // write(sockfd, "Can you hear me?", 24);
  first_request(sockfd);
  printf("正在获取设备IP\n");
  get_device_ip(Device_IP, sockfd);
  /*(5) 关闭套接字*/
  close(sockfd);
}
