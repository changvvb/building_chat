#include "config.h"
#include "gstreamer.h"
#include "tcp.h"
#include <gst/gst.h>

GstElement *receive_pipeline;

int create_receive_pipeline() {
  char pipeline_buf[1024];
  sprintf(pipeline_buf, "udpsrc caps=\"application/x-rtp, media=(string)audio, "
                        "clock-rate=(int)44100, encoding-name=(string)L16, "
                        "encoding-params=(string)1, channels=(int)1, "
                        "payload=(int)96\" port=1111 ! rtpL16depay ! queue ! "
                        "alsasink");
  printf("%s\n", pipeline_buf);
  receive_pipeline = gst_parse_launch(pipeline_buf, NULL);
}

void set_receive_pipeline_playing(void) {
  GstStateChangeReturn ret;
  // set_receive_pipeline_stop();
  // create_receive_pipeline();

  debug_print("will play recieve\n");
  ret = gst_element_set_state(receive_pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    return;
  }
}

void set_receive_pipeline_pause(void) {
  GstStateChangeReturn ret;
  ret = gst_element_set_state(receive_pipeline, GST_STATE_READY);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the GST_STATE_READY state.\n");
    return;
  }
  ret = gst_element_set_state(receive_pipeline, GST_STATE_PAUSED);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the GST_STATE_PAUSED state.\n");
    return;
  }
}

void set_receive_pipeline_stop(void) {
  gst_element_set_state(receive_pipeline, GST_STATE_NULL);
  gst_object_unref(receive_pipeline);
}
