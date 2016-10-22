#include "tcp.h"
#include "config.h"
#include "gstreamer.h"
#include "gstreamer_receive.h"
#include "gtk.h"
#include "servo.h"
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

const int PORT = 10000;
/*the tcp socket file fd*/
int sockfd;
int device_is_connected;
char RevBuf[128];
char SendBuf[128];
#define REV_BUF_LENGTH 128
char Device_IP[32]; // ipv4
pthread_t tcp_check_pthread;
pthread_t tcp_get_commond_pthread;

ssize_t readline(int fd, char *vptr, size_t maxlen) {
  ssize_t n, rc;
  char c, *ptr;

  ptr = vptr;
  for (n = 1; n < maxlen; n++) {
    if ((rc = read(fd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break; /* newline is stored, like fgets() */
    } else if (rc == 0) {
      *ptr = 0;
      return (n - 1); /* EOF, n - 1 bytes were read */
    } else
      return (-1); /* error, errno set by read() */
  }

  *ptr = 0; /* null terminate like fgets() */
  return (n);
}

/*convert '\n' or '\r' to '\0'*/
void fix_string(char *str) {
  char *p;
  int num;
  for (p = str, num = 0; *p != '\n' && *p != '\r'; p++, num++)
    if (num >= 32)
      return;
  *p = '\0';
}

/*
return 0: OK
       1: failed
sent MEDIA_DEVICE
*/
int device_regist() {
  // if (is_connected(sockfd) != 0) {
  //   debug_print("socket is not connected\n");
  //   return -1;
  // }
  char RevBuf[32];
  write(sockfd, "MEDIA_DEVICE\r\n", 24);
  readline(sockfd, RevBuf, 32);
  debug_print("RevBuf:%s\n", RevBuf);
  if (strcmp("RIGHT", RevBuf) == 0) {
    debug_print("regist done");
    return 0;
  } else
    return 1;
}

/*
describe： Get device ip from server
return: -1 connect to server failed
        0  OK
*/
/*sent GET_IP_FROM_PI*/
int get_device_ip(char *ipbuf) {
#ifdef DEBUG_WITHOUT_SERVER
  debug_print("TEST IP: 127.0.0.1");
  strcpy(ipbuf, "127.0.0.1");
  return 0;
#endif
  // if (is_connected(sockfd) != 0) {
  //   return -1;
  // }
  write(sockfd, "GET_IP_FROM_PI\r\n", 24);
  // if (readline(sockfd, RevBuf, 32) != 0) {
  //   if (strstr)
  //     strcpy(ipbuf, RevBuf);
  //   debug_print("device ip:%s\n", ipbuf);
  // }
  // debug_print("recive done\n ip:%s", ipbuf);
  fix_string(ipbuf);
  return 0;
}

void get_pi_ip() {
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "wlan0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);
  sprintf(SendBuf, "\r\nIP_FROM_PI %s\r\n",
          inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  write(sockfd, SendBuf, strlen(SendBuf));
}

void tcp_connect(char *ipaddr) {
  /*声明套接字和链接服务器地址*/
  struct sockaddr_in servaddr;
  // ipaddr = IP;
  debug_print("server IP: %s\n", ipaddr);
  /*判断是否为合法输入*/
  if (ipaddr == NULL) {
    printf("ipaddr == NULL");
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

  debug_print("connected\n");

  /*(4) 消息处理*/
  device_regist();
  tcp_check_pthread_creat();
  get_device_ip(Device_IP);
  // /*(5) 关闭套接字*/
  // close(sockfd);
}

/*
describe: test if tcp conection is alive
return: 0 connected
        -1 disconnected
*/
int is_connected() {
  int retval;
  retval = write(sockfd, "\r\nCONNECTION_TEST\r\n", 16);
  return 0;
  /*
  if ((retval = readline(sockfd, RevBuf, REV_BUF_LENGTH)) == -1) {
    debug_print("is not connected\n");
    debug_print("retval: %d\n", retval);
    return -1;
  } else {
    debug_print("RevBuf:%s\n", RevBuf);
    debug_print("retval: %d\n", retval);
    return 0;
}*/
  debug_print("\nis_connected: retval: %d\n", retval);
}

int thread_exit = 0;

void tcp_sent_you_can_see_me() {
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "wlan0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);
  sprintf(SendBuf, "\r\nIP_FROM_PI %s\r\nYOU_CAN_SEE_ME_NOW\r\n",
          inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  debug_print("this device ip %s",
              inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  write(sockfd, SendBuf, strlen(SendBuf));
}

void quit_handle(int i) {
  i = i;
  thread_exit = 1;
  debug_print("quit_handle called\n");
  exit(1);
}

#define CMP(x) (!strcmp("" #x "", RevBuf))

void *tcp_get_commond() {
  char *str;
  signal(SIGINT, quit_handle);
  while (!thread_exit) {
    if (readline(sockfd, RevBuf, REV_BUF_LENGTH) != 0) {
      fix_string(RevBuf);
      debug_print("RevBuf:%s\n", RevBuf);
      if (CMP(CAMERA_UP))
        SEROV_TURN_UP;
      else if (CMP(CAMERA_DOWN))
        SEROV_TURN_DOWN;
      else if (CMP(CAMERA_RIGHT))
        SEROV_TURN_RIGHT;
      else if (CMP(CAMERA_LEFT))
        SEROV_TURN_LEFT;
      /**/
      else if (CMP(VIDEO_START)) {
        get_device_ip(Device_IP);
        create_pipeline();
        set_pipeline_playing();
        debug_print("it will play vidio!!!\n");
      } else if (CMP(VIDEO_STOP)) {
        debug_print("it will stop vidio!!!\n");
        // set_receive_pipeline_stop();
        set_pipeline_pause();
      } else if (CMP(GET_IP)) {
        get_pi_ip();
        // create_receive_pipeline();
        set_receive_pipeline_playing();
      } /*else if (CMP(VIDEO_START))
          get_device_ip(Device_IP);
          */
      else if ((str = strstr(RevBuf, "DEVICE_IP:")) != NULL) {
        debug_print("%s\n", str + strlen("DEVICE_IP") + 1);
        strcpy(Device_IP, str + strlen("DEVICE_IP") + 1);
        // create_pipeline();
      }
    }
  }
  debug_print("comond check exit\n");
}

void *tcp_check_function(void *ptr) {
  int connected_ret;

  while (1) {
    connected_ret = is_connected();
    if (connected_ret != 0) {
      debug_print("tcp_check: \ntcp_connect error");
      tcp_connect(IP);
    } else {
      debug_print("connect\n");
    }
    sleep(2);
  }
  debug_print("tcp_check_function called finish\n");
}

void tcp_check_pthread_creat() {
  int iret;
  // iret1 = pthread_create(&tcp_check_pthread, NULL, tcp_check_function, NULL);
  iret = pthread_create(&tcp_get_commond_pthread, NULL, tcp_get_commond, NULL);
  if (iret != 0) {
    debug_print("thread created error");
  }
}
