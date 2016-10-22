#include "gstreamer.h"
#include "tcp.h"
#include <gst/gst.h>

GstElement *doorbell_sound_pipeline;

int create_doorbell_sound_pipeline() {
  char pipeline_buf[1024];
  sprintf(pipeline_buf, "filesrc location=./res/doorbell_sound.wav ! "
                        "decodebin ! audioconvert ! audioresample ! "
                        "autoaudiosink");
  printf("%s\n", pipeline_buf);
  doorbell_sound_pipeline = gst_parse_launch(pipeline_buf, NULL);
}

void set_doorbell_sound_pipeline_playing(void) {
  GstStateChangeReturn ret;
  ret = gst_element_set_state(doorbell_sound_pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    return;
  }
}

void set_doorbell_sound_pipeline_pause(void) {
  GstStateChangeReturn ret;
  ret = gst_element_set_state(doorbell_sound_pipeline, GST_STATE_READY);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the GST_STATE_READY state.\n");
    return;
  }
  ret = gst_element_set_state(doorbell_sound_pipeline, GST_STATE_PAUSED);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    return;
  }
}
