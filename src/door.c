#include "door.h"
#include "config.h"
#include <wiringPi.h>
void door_init()
{
    // if (wiringPiSetup() == -1) {
    //     debug_print("door init error\n");
    //     exit(-1);
    // }
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);
}

void door_open()
{
    digitalWrite(25, LOW);
    delay(3000);
    digitalWrite(25, HIGH);
}
