#include "gstreamer.h"
#include "config.h"
#include "tcp.h"
#include <gst/gst.h>
#include "mqtt.h"

GstElement *pipeline;

int create_pipeline() {
  char pipeline_buf[1024];
  if (pipeline != NULL) {
    set_pipeline_stop();
    debug_print("Pipeline changing\n");
  }
  sprintf(
      pipeline_buf,
      "v4l2src ! queue ! "
      // "video/x-raw-yuv,width=320,height=240,framerate=(fraction)15/1 ! "
      "videoflip method=clockwise  ! x264enc speed-preset=ultrafast pass=qual "
      "quantizer=10 tune=zerolatency ! rtph264pay ! udpsink "
      "host=%s port=1111 alsasrc device=hw:1 ! queue ! "
      "audioconvert  ! rtpL16pay  ! udpsink host=%s port=2222",
      phone_ip_buffer, phone_ip_buffer);
  printf("%s\n", pipeline_buf);
  pipeline = gst_parse_launch(pipeline_buf, NULL);
}

void set_pipeline_playing(void) {
  GstStateChangeReturn ret;
  // create_pipeline();
  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    return;
  }
}

void set_pipeline_pause(void) {
  GstStateChangeReturn ret;
  // ret = gst_element_set_state(pipeline, GST_STATE_READY);
  // if (ret == GST_STATE_CHANGE_FAILURE) {
  //   g_printerr("Unable to set the pipeline to the GST_STATE_READY state.\n");
  //   return;
  // }
  ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the pause state.\n");
    return;
  }
}

void set_pipeline_stop(void) {
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
}
