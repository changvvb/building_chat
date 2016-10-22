CC = gcc
CFLASS = -Wall -g
GST_CFLAGS = `pkg-config "gstreamer-0.10" "gstreamer-interfaces-0.10" --cflags --libs`
GTK_CFLAGS = `pkg-config  "gtk+-3.0" --cflags --libs`
MQTT_LIB = -lpaho-mqtt3
CLIB = -lpthread -lwiringPi -lwiringPiDev -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs
default: building_chat

building_chat: gstreamer_receive.o gtk.o gst.o  init.o door.o servo.o gstreamer_doorbell_sound.o  mqtt.o
	$(CC) $(GST_CFLAGS) $(CLIB)  $(GTK_CFLAGS) $(CFLASS) $^ -o build_chat

gstreamer_doorbell_sound.o: src/gstreamer_doorbell_sound.c
	$(CC) $(CFLASS) $(GST_CFLAGS) $^ -c -o gstreamer_doorbell_sound.o
init.o: src/init.c
	$(CC) $(CFLASS) $^ -c -o init.o
# tcp.o: src/tcp.c
	# $(CC) $(CFLASS) $(GTK_CFLAGS) $(CLIB)  $^ -c -o tcp.o
gst.o: src/gstreamer.c
	$(CC) $(CFLASS) $(GST_CFLAGS) $^ -c -o gst.o
gtk.o: src/gtk.c
	$(CC) $(CFLASS) $(GTK_CFLAGS) $^ -c -o gtk.o
servo.o: src/servo.c
	$(CC) $(CFLASS) $^ -c -o servo.o
door.o: src/door.c
	$(CC) $(CFLASS) $^ -c -o door.o
gstreamer_receive.o: src/gstreamer_receive.c
	$(CC) $(CFLASS) $(GST_CFLAGS) $^ -c -o gstreamer_receive.o
mqtt.o: src/mqtt.c
	$(CC) $(CFLASS)  $^ -c -o mqtt.o
clean:
	rm *.o
