#ifndef SERIAL_H
#define SERIAL_H

// 1 right  2 left  3 up  4 down  0 nothing
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4
#define NOTHING 0
void serov_change_direction(int dir);
void servo_init();

#define SEROV_TURN_RIGHT serov_change_direction(RIGHT)
#define SEROV_TURN_LEFT serov_change_direction(LEFT)
#define SEROV_TURN_UP serov_change_direction(UP)
#define SEROV_TURN_DOWN serov_change_direction(DOWN)

#endif
