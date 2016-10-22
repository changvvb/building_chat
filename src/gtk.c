#include "gtk.h"
#include "config.h"
#include "gstreamer.h"
#include "gstreamer_doorbell_sound.h"
#include "mqtt.h"
#include <gst/gst.h>
#include <gtk/gtk.h>
#include "servo.h"

void init(void);
/*globle variable*/
GtkWidget *window;
GtkWidget *fixed;
GtkWidget *label;
GtkWidget *point_image;

static void doorbell_button_clicked(GtkWidget *widget, gpointer data) {
    static int statue = 0;
    set_pipeline_playing();
    set_receive_pipeline_playing();
    send_you_can_see_me();
}

/*
static void serov_dir_up_clicked(GtkWidget* widget, gpointer data ) { SEROV_TURN_UP; }

static void serov_dir_down_clicked(GtkWidget* widget, gpointer data) { SEROV_TURN_DOWN; }

static void serov_dir_left_clicked(GtkWidget* widget, gpointer data) { SEROV_TURN_LEFT; }

static void serov_dir_right_clicked(GtkWidget* widget, gpointer data) { SEROV_TURN_RIGHT; }
*/

gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    guint width, height;
    GdkRGBA color;
    GtkStyleContext *context;
    int statue;
    if (data == NULL)
        statue = 1;
    else
        statue = *(int*)data;
    debug_print("data: %p\n",data);
    context = gtk_widget_get_style_context (widget);

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    gtk_render_background (context, cr, 0, 0, width, height);

    cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             0, 2 * G_PI);

    gtk_style_context_get_color (context,gtk_style_context_get_state (context),&color);
    debug_print("statue: %d\n",statue);
    if (statue)
        color.green = 1;
    else
        color.green = 0;
    color.red = 1 - color.green;
    color.blue = 0;
    color.alpha = 1;
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);

    return FALSE;
}

/*show network statue
statue 1  :     OK
statue 0  :     ERROR
*/
void show_network_statue(int statue) {
    // GtkWidget *drawing_area = gtk_drawing_area_new ();
    // gtk_widget_set_size_request (drawing_area, 30, 30);
    // //draw_callback(drawing_area,NULL,&statue);
    // g_signal_connect (G_OBJECT (drawing_area), "draw",
    //                   G_CALLBACK (draw_callback), &statue);
    // gtk_fixed_put(GTK_FIXED(fixed), drawing_area, 800-35, 350);
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *button;
    GtkWidget *button_image_1;
    GtkWidget *big_label;

    /*button_image_1*/
    button_image_1 = gtk_image_new_from_file(DOORBELL_BUTTON_PATH);

    /*box*/
    fixed = gtk_fixed_new();
    /*window*/
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "物联网对讲系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 600);
    gtk_container_add(GTK_CONTAINER(window), fixed);


    /*button servo dir*/
    /*
    GtkWidget* btn_up = gtk_button_new();
    gtk_button_set_label(btn_up,"UP");
    GtkWidget* btn_down = gtk_button_new();
    gtk_button_set_label(btn_down,"DOWN");
    GtkWidget* btn_left = gtk_button_new();
    gtk_button_set_label(btn_left,"LEFT");
    GtkWidget* btn_right = gtk_button_new();
    gtk_button_set_label(btn_right,"RIGHT");
    g_signal_connect(btn_up, "clicked", G_CALLBACK(serov_dir_up_clicked), NULL);
    g_signal_connect(btn_down, "clicked", G_CALLBACK(serov_dir_down_clicked),NULL);
    g_signal_connect(btn_left, "clicked", G_CALLBACK(serov_dir_left_clicked),NULL);
    g_signal_connect(btn_right, "clicked", G_CALLBACK(serov_dir_right_clicked),NULL);
    gtk_fixed_put(GTK_FIXED(fixed), btn_up, 20, 20);
    gtk_fixed_put(GTK_FIXED(fixed), btn_down, 20, 60);
    gtk_fixed_put(GTK_FIXED(fixed), btn_right, 20, 100);
    gtk_fixed_put(GTK_FIXED(fixed), btn_left, 20, 140);
    */

    /*button*/
    button = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_button_set_image(GTK_BUTTON(button), button_image_1);
    g_signal_connect(button, "clicked", G_CALLBACK(doorbell_button_clicked),
               NULL);
    gtk_fixed_put(GTK_FIXED(fixed), button, 337, 100);

    // /*red and green point image*/
    // point_image = gtk_image_new_from_file(GREEN_POINT_PATH);
    // gtk_fixed_put(GTK_FIXED(fixed), point_image, 800 - 35, 350);

    // GtkWidget *drawing_area = gtk_drawing_area_new ();
    // gtk_widget_set_size_request (drawing_area, 30, 30);
    // g_signal_connect (G_OBJECT (drawing_area), "draw",
    //                   G_CALLBACK (draw_callback), NULL);
    // gtk_fixed_put(GTK_FIXED(fixed), drawing_area, 200, 200);

    /*lable*/
    label = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(label), "网络连接：正常");
    gtk_fixed_put(GTK_FIXED(fixed), label, 800, 350);

    /*big lavel*/ big_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(big_label),
                   "<span foreground='red' underline='double' "
                   "underline_color='blue' "
                   "font_desc='32'>楼宇对讲系统</span>");
    gtk_fixed_put(GTK_FIXED(fixed), big_label, 415, 30);

    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_widget_show_all(GTK_WINDOW(window));
    show_network_statue(1);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    gst_init(&argc, &argv);
    init();
    debug_print("init over");
    //while(1);
    app = gtk_application_new("com.celitea", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
