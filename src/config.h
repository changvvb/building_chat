/*
 * config.h 包含该tcp/ip套接字编程所需要的基本头文件，与server.c
 * client.c位于同一目录下
*/
#ifndef _CONFIG_H
#define _CONFIG_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern char Device_IP[32];
#define DEBUG

#endif
