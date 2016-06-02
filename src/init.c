#include "gstreamer.h"
#include "tcp.h"
void init(void) {
  tcp_connect(IP);
  create_pipeline();
}
