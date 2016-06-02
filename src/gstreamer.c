#include "gstreamer.h"
#include "tcp.h"
#include <gst/gst.h>

GstElement *pipeline;

int create_pipeline() {
  char pipeline_buf[1024];
  sprintf(
      pipeline_buf,
      "v4l2src ! queue ! "
      // "video/x-raw-yuv,width=320,height=240,framerate=(fraction)15/1 ! "
      "videoflip method=clockwise  ! x264enc speed-preset=ultrafast pass=qual "
      "quantizer=10 tune=zerolatency ! rtph264pay ! udpsink "
      "host=%s port=1111 alsasrc device=hw:1 ! queue ! "
      "audioconvert  ! rtpL16pay  ! udpsink host=%s port=2222",
      Device_IP, Device_IP);
  printf("%s\n", pipeline_buf);
  pipeline = gst_parse_launch(pipeline_buf, NULL);
}

void set_pipeline_playing(void) {
  GstStateChangeReturn ret;
  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    return;
  }
}
