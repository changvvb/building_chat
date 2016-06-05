#include "servo.h"
#include "config.h"
#define ONPC 0
#if ONPC == 0
#include <wiringPi.h>
int horizontal_value = 500; // max 1000 min 0
int vertical_value = 500;
#define SERVO_HORIZONTAL 0
#define SERVO_VERTICAL 1
void servo_init() {
  int i;
  if (wiringPiSetup() == -1) {
    debug_print("server init error\n");
    exit(-1);
  }
  for (i = 1; i <= 4; i++)
    pinMode(i, OUTPUT);
  digitalWrite(1, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
}
// 1 right  2 left  3 up  4 down  0 nothing
void serov_change_direction(int dir) {
  debug_print("video run\n");
  if (dir == 4) {
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
  } else if (dir == 3) {
    digitalWrite(3, LOW);
    delay(100);
    digitalWrite(3, HIGH);
  } else if (dir == 2) {
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
  } else if (dir == 1) {
    digitalWrite(1, LOW);
    delay(100);
    digitalWrite(1, HIGH);
  }
}
#else
// 1 right  2 left  3 up  4 down  0 nothing
void serov_change_direction(int dir) {
  if (dir == RIGHT)
    debug_print("haha RIGHT\n");
  else if (dir == LEFT)
    debug_print("haha LEFT\n");
  else if (dir == DOWN)
    debug_print("haha DOWN\n");
  else if (dir == UP)
    debug_print("haha UP\n");
}

void servo_init() {}
#endif
