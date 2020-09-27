#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mosquitto.h"

#include "linzhi/alloc.h"

#include "rgbd.h"
#include "led.h"
#include "mqtt.h"


#define	MQTT_TOPIC_RGB		"/sys/led/rgb"
#define	MQTT_TOPIC_ETHER	"/sys/led/ether"

#define	MQTT_HOST	"localhost"
#define	MQTT_PORT	1883


enum mqtt_qos {
	qos_be		= 0,
	qos_ack		= 1,
	qos_once	= 2
};


static struct mosquitto *mosq;


static void message(struct mosquitto *m, void *user,
    const struct mosquitto_message *msg)
{
	char *s;

	if (verbose)
		fprintf(stderr, "MQTT \"%s\": \"%.*s\"\n",
		    msg->topic, msg->payloadlen, (const char *) msg->payload);

	s = alloc_size(msg->payloadlen + 1);
        memcpy(s, msg->payload, msg->payloadlen);
        s[msg->payloadlen] = 0;

	if (!strcmp(msg->topic, MQTT_TOPIC_RGB)) {
		char rgb[4];
		unsigned on_ms, off_ms;
		bool r, g, b;

		switch (sscanf(s, "%3s %u %u", rgb, &on_ms, &off_ms)) {
		case 1:
			on_ms = off_ms = 0;
			/* fall through */
		case 3:
			decode_rgb(rgb, &r, &g, &b);
			led_blink(r, g, b, on_ms, off_ms);
			break;
		default:
			fprintf(stderr, "unrecognized LED setting \"%s\"\n", s);
		}
	}
        else if (!strcmp(msg->topic, MQTT_TOPIC_ETHER))
		ether_set(atoi(s));
        else
                fprintf(stderr, "unexpected %s: \"%s\"\n", msg->topic, s);
        free(s);
}


/* --- Initialization ------------------------------------------------------ */


static void subscribe(const char *topic, enum mqtt_qos qos)
{
	int res;

	res = mosquitto_subscribe(mosq, NULL, topic, qos_ack);
	if (res != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "mosquitto_subscribe: %s\n",
		    mosquitto_strerror(res));
		exit(1);
	}
}


static void connected(struct mosquitto *m, void *data, int result)
{
	if (result) {
		fprintf(stderr, "MQTT connect failed: %s",
		    mosquitto_strerror(result));
		exit(1);
	}
	subscribe(MQTT_TOPIC_RGB, qos_ack);
	subscribe(MQTT_TOPIC_ETHER, qos_ack);
}


static void disconnected(struct mosquitto *m, void *data, int result)
{
	int res;

	fprintf(stderr, "warning: reconnecting MQTT (disconnect reason %s)",
	    mosquitto_strerror(result));
	res = mosquitto_reconnect(mosq);
	if (res != MOSQ_ERR_SUCCESS)
		fprintf(stderr, "mosquitto_reconnect: %s",
		    mosquitto_strerror(res));
}


void mqtt_init(void)
{
	int res;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, 1, NULL);
	if (!mosq) {
		fprintf(stderr, "mosquitto_new failed\n");
		exit(1);
	}

	mosquitto_connect_callback_set(mosq, connected);
	mosquitto_disconnect_callback_set(mosq, disconnected);
	mosquitto_message_callback_set(mosq, message);

	res = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 3600);
	if (res != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "mosquitto_connect: %s",
		    mosquitto_strerror(res));
		exit(1);
	}
}


void mqtt_loop(void)
{
	int res;

	do res = mosquitto_loop_forever(mosq, -1, 1);
	while (res == MOSQ_ERR_SUCCESS);

	fprintf(stderr, "mosquitto_loop_forever: %s",
	    mosquitto_strerror(res));
	exit(1);
}
