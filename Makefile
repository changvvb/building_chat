CC = gcc
CFLASS = -Wall -g
GST_CFLAGS = `pkg-config "gstreamer-0.10" "gstreamer-interfaces-0.10" --cflags --libs`
GTK_CFLAGS = `pkg-config  "gtk+-3.0" --cflags --libs`
CLIB = -lpthread -lwiringPi -lwiringPiDev
default: build_chat

build_chat: gtk.o gst.o tcp.o init.o servo.o
	$(CC) $(GST_CFLAGS) $(CLIB)  $(GTK_CFLAGS) $(CFLASS) $^ -o build_chat

init.o: src/init.c
	$(CC) $(CFLASS) $^ -c -o init.o
tcp.o: src/tcp.c
	$(CC) $(CFLASS) $(CLIB)  $^ -c -o tcp.o
gst.o: src/gstreamer.c
	$(CC) $(CFLASS) $(GST_CFLAGS) $^ -c -o gst.o
gtk.o: src/gtk.c
	$(CC) $(CFLASS) $(GTK_CFLAGS) $^ -c -o gtk.o
servo.o: src/servo.c
	$(CC) $(CFLASS) $^ -c -o servo.o
clean:
	rm *.o
