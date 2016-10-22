#include "gstreamer.h"
#include "gstreamer_doorbell_sound.h"
#include "gstreamer_receive.h"
#include "servo.h"
#include "tcp.h"
#include "mqtt.h"
#include "door.h"
#include "config.h"
void init(void)
{
    servo_init();
    door_init();
    if (mqtt_init() != 0) {
        printf("请检查您的网络！\n");
        exit(-1);
    }
    debug_print("mqtt init over");
    create_doorbell_sound_pipeline();
    debug_print("create_doorbell_sound_pipeline");
    create_receive_pipeline();
    debug_print("create_receive_pipeline");
    set_receive_pipeline_playing();
}
