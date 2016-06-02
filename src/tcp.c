#include "tcp.h"
#include "config.h"

const int MAX_LINE = 2048;
const int PORT = 10000;

/*the tcp socket file fd*/
int sockfd;
char Device_IP[32]; // ipv4

/*convert '\n' or '\r' to '\0'*/
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
int device_regist(int sockfd) {
  if (is_connected(sockfd) != 0)
    return -1;
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

/*
describe： Get device ip from server
return: -1 connect to server failed
        0  OK
*/
/*sent GET_IP_FROM_PI*/
int get_device_ip(char *ipbuf, int sockfd) {
#ifdef DEBUG_WITHOUT_SERVER
  strcpy(ipbuf, "127.0.0.1");
  return 0;
#endif
  if (is_connected(sockfd) != 0) {
    return -1;
  }
  write(sockfd, "GET_IP_FROM_PI\r\n", 24);
  while (!read(sockfd, ipbuf, 32))
    printf("%s\n", ipbuf);
#ifdef DEBUG
  printf("recive done\n ip:%s", ipbuf);
#endif
  fix_string(ipbuf);
  return 0;
}

void tcp_connect(char *ipaddr) {
  /*声明套接字和链接服务器地址*/
  struct sockaddr_in servaddr;
// ipaddr = IP;
#ifdef DEBUG
  printf("%s", ipaddr);
#endif
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
  device_regist(sockfd);
  get_device_ip(Device_IP, sockfd);
  /*(5) 关闭套接字*/
  close(sockfd);
}

/*
describe: test if tcp conection is alive
return: 0 connected
        1 disconnected
*/
int is_connected(int sockfd) {
  int error = 0;
  socklen_t len = sizeof(error);
  int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
#ifdef DEBUG
  printf("\nis_connected: retval: %d", retval);
#endif
  return retval;
}
