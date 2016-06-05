#include "gtk.h"
#include "gstreamer.h"
#include "tcp.h"
#include <gtk/gtk.h>

void init(void);

static void print_hello(GtkWidget *widget, gpointer data) {
  g_print("Hello World\n");
  get_device_ip(Device_IP, sockfd);
  set_pipeline_playing();
  tcp_sent_you_can_see_me();
}

void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  GtkWidget *button_image_1;
  GtkWidget *button_image_2;
  button_image_1 = gtk_image_new_from_file("./res/button_1.jpg");

  if (button_image_1 == NULL) {
    g_print("button_image_1");
    return;
  }

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add(GTK_CONTAINER(window), button_box);

  button = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  gtk_button_set_image(GTK_BUTTON(button), button_image_1);
  g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;
  gst_init(&argc, &argv);
  init();
  app = gtk_application_new("com.celitea", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
