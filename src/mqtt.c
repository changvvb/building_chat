
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"

#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>

#include "servo.h"
#include "door.h"
#include "config.h"
#include "mqtt.h"
#include "gstreamer.h"

#if defined(WIN32)
#include <windows.h>
#define sleep Sleep
#else
#include <sys/time.h>
#include <stdlib.h>
#endif


volatile int toStop = 0;
const char* PHONE_IP_TOPIC = "/phone/ip";
const char* PHONE_IP_ANSWER_TOPIC = "/phone/ip/answer";
const char* RASPBERRYPI_CAMERA_TOPIC = "/raspberrypi/camera";
const char*	RASPBERRYPI_IP_ANSWER_TOPIC = "/raspberrypi/ip/answer";
const char*	RASPBERRYPI_IP_TOPIC = "/raspberrypi/ip";
const char* RASPBERRYPI_VIDEO_STOP_TOPIC = "/raspberrypi/video/stop";
const char* RASPBERRYPI_VIDEO_STOP_ANSWER_TOPIC = "/raspberrypi/video/stop/answer";
const char* RASPBERRYPI_KEEPALIVE_TOPIC = "/raspberrypi/keepalive";
const char* RASPBERRYPI_DOOR_OPEN = "/raspberrypi/door/open";
MQTTClient* p_client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
char phone_ip_buffer[64];
char ip_buffer[64];

struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	int qos;
	int retained;
	char* username;
	char* password;
	char* host;
	char* port;
	int showtopics;
	int keepalive;
} opts =
{
	"raspberrypi", 0, "\n", 2,0, "raspberrypi", "raspberrypi", "iot.celitea.cn", "1883", 1, 10
};

void get_pi_ip(char * ip_buffer) {
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "wlan0" */
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);
	sprintf(ip_buffer, "%s",
	      inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

void send_you_can_see_me(void)
{
	int rcp = 0;
	rcp = MQTTClient_publish(*p_client, RASPBERRYPI_IP_TOPIC, strlen(ip_buffer), ip_buffer, opts.qos, opts.retained, NULL);
	if (rcp != 0)
		debug_print("MQTTClient_publish failed: %d",rcp);
}

void myconnect(MQTTClient* p_client, MQTTClient_connectOptions* opts)
{
	int rc = 0;
	debug_print("p_client:  %p, opts: %p \n",p_client,opts);
	// if(!MQTTClient_isConnected (*client))
	// {
	// 	MQTTClient_disconnect (*client, 5);
	// 	debug_print("mqtt client is connected, then disconnect it\n");
	// }
	// else
	// {
	// 	debug_print("mqtt client id disconnected\n");
	// }
	if ((rc = MQTTClient_connect(*p_client, opts)) != 0)
	{
		//MQTTClient_disconnect (client, 5);
		printf("Failed to connect, return code %d\n", rc);
		//exit(EXIT_FAILURE);
	}
	show_network_statue(1);
}


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;

	MQTTClient_disconnect(*p_client, 0);
	MQTTClient_destroy(p_client);
	printf("Stopping\n");
}

#define CMP(x) (!strcmp("" #x "", message->payload))

void * thread_receive_message(void * arg) {
	int rc = 0,rcp = 0;
	char* topicName = NULL;
	int topicLen;
	MQTTClient_message* message = NULL;
	if(arg == NULL)
	{
		debug_print("arg === NULL\n");
		return ;
	}
	MQTTClient *p_client = (MQTTClient*)arg;
	while (!toStop)
	{
		rc = MQTTClient_receive(*p_client, &topicName, &topicLen, &message, 1000);
		if (message)
		{
			printf("topicName: %s\n", topicName);
			*((char*)message->payload + message->payloadlen) = '\0';
			if (0 == strcmp (topicName,RASPBERRYPI_CAMERA_TOPIC))
			{
				if (CMP(UP))
		          SEROV_TURN_UP;
		        else if (CMP(DOWN))
		          SEROV_TURN_DOWN;
		        else if (CMP(RIGHT))
		          SEROV_TURN_RIGHT;
		        else if (CMP(LEFT))
		          SEROV_TURN_LEFT;
				else
					debug_print("message error: topic %s, message %s\n", topicName, (char*)message->payload);
			}
			else if (0 == strcmp (topicName,PHONE_IP_TOPIC))
			{
				//向手机发布主题，自己的IP的地址
				strcpy(phone_ip_buffer,(char*)message->payload);
				get_pi_ip(ip_buffer);
				rcp = MQTTClient_publish(*p_client, RASPBERRYPI_IP_ANSWER_TOPIC, strlen(ip_buffer), ip_buffer, opts.qos, opts.retained, NULL);
				if (rcp != 0) {
					debug_print("MQTTClient_publish failed: %d",rcp);
					continue;
				}
				create_pipeline();
		        set_pipeline_playing();
			}
			else if (0 == strcmp (topicName,PHONE_IP_ANSWER_TOPIC))
			{
				debug_print("PHONE_IP_ANSWER_TOPIC\n");
			}
			else if (0 == strcmp (topicName,RASPBERRYPI_VIDEO_STOP_TOPIC))
			{
				debug_print("RASPBERRYPI_VIDEO_STOP_TOPIC\n");
				rcp = MQTTClient_publish(*p_client, RASPBERRYPI_VIDEO_STOP_ANSWER_TOPIC, strlen("STOPED"), "STOPED", opts.qos, opts.retained, NULL);
				if (rcp != 0) {
					debug_print("MQTTClient_publish failed: %d",rcp);
				}
				debug_print("it will stop vidio!!!\n");
		        set_pipeline_pause();
			}
			else if (0 == strcmp(topicName, RASPBERRYPI_DOOR_OPEN))
			{
				debug_print("RASPBERRY_DOOR_OPEN");
				set_pipeline_pause();
				door_open();
			}

			if (opts.showtopics)
				printf("%s\t", topicName);
			if (opts.nodelimiter)
				printf("%.*s", message->payloadlen, (char*)message->payload);
			else
				printf("%.*s%s", message->payloadlen, (char*)message->payload, opts.delimiter);

			fflush(stdout);
			MQTTClient_freeMessage(&message);
			MQTTClient_free(topicName);
			printf("\n");
			show_network_statue(1);
		}
		if (rc != 0)
		{
			debug_print("rc == %d\n",rc);
			show_network_statue(0);
			myconnect(p_client, &conn_opts);
		}
		else
			show_network_statue(1);
	}
}



void* pthread_keepalive()
{
	// if(pthread_create(&keepalive_pthread, NULL, pthread_keepalive, NULL) != 0)
	// {
	//
	// }
	// for(;;)
	// {
	//
	// }
}



int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	debug_print("messageArrived\n");
	return 0;
}



MQTTClient client;
int mqtt_init()
{
	int rc = 0;
	char url[100];

	pthread_t keepalive_pthread;
	pthread_t receive_message_pthread;
	sprintf(url, "%s:%s", opts.host, opts.port);

	rc = MQTTClient_create(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	p_client = &client;

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.reliable = 0;
	conn_opts.cleansession = 1;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;

	if ((rc = MQTTClient_connect(client, &conn_opts)) != 0)
	{
		printf("Failed to connect, return code %d\n", rc);
	}
	if(rc == 0)
		rc = MQTTClient_subscribe(client, RASPBERRYPI_DOOR_OPEN, opts.qos);
	if (rc == 0)
		rc = MQTTClient_subscribe(client, PHONE_IP_TOPIC, opts.qos);
	if (rc == 0)
    	rc = MQTTClient_subscribe(client, PHONE_IP_ANSWER_TOPIC, opts.qos);
	if (rc == 0)
		rc = MQTTClient_subscribe(client, RASPBERRYPI_CAMERA_TOPIC, opts.qos);
	if (rc == 0)
		rc = MQTTClient_subscribe(client, RASPBERRYPI_VIDEO_STOP_TOPIC, opts.qos);
	if (rc == 0)
		rc = MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);
	if (rc == 0)
		rc = pthread_create(&receive_message_pthread, NULL, thread_receive_message, (void*)p_client);
	 if (rc == 0)
	 	rc = pthread_create(&keepalive_pthread, NULL, pthread_keepalive, NULL);
	else {
		printf("mqtt init failed\n");
		return -1;
	}
	return EXIT_SUCCESS;
}
