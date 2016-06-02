CFLASS = -Wall -g
GST_CFLAGS = `pkg-config "gstreamer-0.10" "gstreamer-interfaces-0.10" --cflags --libs`
GTK_CFLAGS = `pkg-config  "gtk+-3.0" --cflags --libs`

default: build_chat

build_chat: gtk.o gst.o tcp.o
	gcc $(GST_CFLAGS)  $(GTK_CFLAGS) $(CFLASS) $^ -o build_chat

tcp.o: src/tcp.c
	gcc $(CFLASS)  $^ -c -o tcp.o
gst.o: src/gstreamer.c
	gcc $(CFLASS) $(GST_CFLAGS) $^ -c -o gst.o
gtk.o: src/gtk.c
		gcc $(CFLASS) $(GTK_CFLAGS) $^ -c -o gtk.o

clean:
	rm *.o
