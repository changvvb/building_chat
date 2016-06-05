#include "gstreamer.h"
#include "servo.h"
#include "tcp.h"
void init(void) {
  tcp_connect(IP);
  servo_init();
  // tcp_check_pthread_creat();
}
