// gpio_test.c

#include <wiringPi.h>
int main() {
  // 初始化wiringPi
  wiringPiSetup();

  int i = 0;
  // 设置IO口全部为输出状态
  for (i = 0; i < 8; i++)
    pinMode(i, OUTPUT);

  for (;;) {
    for (i = 0; i < 8; i++) {
      // 点亮500ms 熄灭500ms
      digitalWrite(i, HIGH);
      delay(500);
      digitalWrite(i, LOW);
      delay(500);
    }
  }

  return 0;
}
