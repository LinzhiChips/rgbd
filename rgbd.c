/*
 * rgbd.c - RGB and Ethernet LED control
 *
 * Copyright (C) 2021 Linzhi Ltd.
 *
 * This work is licensed under the terms of the MIT License.
 * A copy of the license can be found in the file COPYING.txt
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "rgbd.h"
#include "led.h"
#include "mqtt.h"


bool verbose = 0;


static void daemonize(void)
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}
	if (pid)
		exit(0);
	if (setsid() < 0) {
		perror("setsid");
		exit(1);
	}
	(void) close(0);
	(void) close(1);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-b] [-v] [color|eon|eoff]\n\n"
"-b  fork and run in backgound (default: don't fork)\n"
"-v  verbose operation\n"
"color is any combination of R, G, and B, or - for darkness\n"
"eon   enables the Ethernet LEDs\n"
"eoff  disables the Ethernet LEDs\n"
    , name);
	exit(1);
}


int main(int argc, char **argv)
{
	const char *gen, *rev;
	bool background = 0;
	bool r, g, b;
	int c;

	gen = getenv("BOARD_GENERATION");
	rev = getenv("BOARD_REVISION");
	if (!gen || !rev) {
		fprintf(stderr, "environment variables BOARD_GENERATION and "
		    "BOARD_REVISION must be set\n");
		exit(1);
	}

	while ((c = getopt(argc, argv, "bv")) != EOF)
		switch (c) {
		case 'b':
			background = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			usage(*argv);
		}

	led_init(atoi(gen), *rev == '_' ? atoi(rev + 1) : atoi(rev));
	switch (argc - optind) {
	case 0:
		led_default();
		mqtt_init();
		if (background)
			daemonize();
		mqtt_loop();
		break;
	case 1:
		if (!strcmp(argv[optind], "eon")) {
			ether_set(1);
			break;
		}
		if (!strcmp(argv[optind], "eoff")) {
			ether_set(0);
			break;
		}
		decode_rgb(argv[optind], &r, &g, &b);
		led_set(r, g, b);
		break;
	default:
		usage(*argv);
	}

	return 0;
}
