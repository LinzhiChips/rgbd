/*
 * led.c - LED control
 *
 * Copyright (C) 2021 Linzhi Ltd.
 *
 * This work is licensed under the terms of the MIT License.
 * A copy of the license can be found in the file COPYING.txt
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "linzhi/thread.h"

#include "gpio.h"
#include "led.h"


#define	MIO(n)	(n)
#define	BANK(n)	((n) >> 5)
#define	PIN(n)	((n) & 31)
#define	MASK(n)	(1 << ((n) & 31))


struct pattern {
	bool r, g, b;
	unsigned on_ms, off_ms;
};


static unsigned generation;
static unsigned red, green, blue;
static unsigned ether;
static pthread_t thread;
static bool running = 0;	/* thread is running */


void decode_rgb(const char *s, bool *r, bool *g, bool *b)
{
	*r = strchr(s, 'R');
	*g = strchr(s, 'G');
	*b = strchr(s, 'B');
}


#define	MIO_PIN_07	0xF800071C


static void mio7_tri(bool enable)
{
	volatile uint32_t *reg = map_regs(MIO_PIN_07, 4);

	if (enable)
		*reg |= 1;
	else
		*reg &= ~1;
	unmap_regs(reg, MIO_PIN_07, 4);
}


void led_set(bool r, bool g, bool b)
{
	/* Red: Z = on, 1 = off, 0 = overdrive */
	if (r) {
		gpio_dir_mask(BANK(red), MASK(red), 0);
	} else {
		gpio_dir_mask(BANK(red), MASK(red), MASK(red));
		gpio_sw(BANK(red), ~MASK(red), MASK(red));
	}

	/* Green: Z = on, 1 = off, 0 = overdrive */
	if (g) {
		if (generation == 2)
			mio7_tri(1);
		gpio_sw(BANK(green), ~MASK(green), 0);
	} else {
		if (generation == 2)
			mio7_tri(0);
		gpio_dir_mask(BANK(green), MASK(green), MASK(green));
		gpio_sw(BANK(green), ~MASK(green), MASK(green));
	}

	/* Blue: Z/1 = off, 0 = on */
	gpio_dir_mask(BANK(blue), MASK(blue), MASK(blue));
	gpio_sw(BANK(blue), ~MASK(blue), b ? 0 : MASK(blue));
}


void ether_set(bool on)
{
	gpio_dir_mask(BANK(ether), MASK(ether), MASK(ether));
	gpio_sw(BANK(ether), ~MASK(ether), on ? 0 : MASK(ether));
}


static void *blink_thread(void *arg)
{
	const struct pattern *p = arg;

	while (1) {
		
		led_set(p->r, p->g, p->b);
		usleep(p->on_ms * 1000);
		led_set(0, 0, 0);
		usleep(p->off_ms * 1000);
	}
	return NULL;
}


void led_blink(bool r, bool g, bool b, unsigned on_ms, unsigned off_ms)
{
	static struct pattern p;

	if (running) {
		thread_cancel(thread);
		thread_join(thread);
		running = 0;
	}

	p.r = r;
	p.g = g;
	p.b = b;
	p.on_ms = on_ms;
	p.off_ms = off_ms;

	if (off_ms) {
		thread = thread_create(blink_thread, &p, NULL);
		running = 1;
	} else {
		led_set(p.r, p.g, p.b);
	}
}


void led_default(void)
{
#if 0
	gpio_dir_mask(BANK(red), MASK(red), MASK(red));
	gpio_dir_mask(BANK(green), MASK(green), MASK(green));
	gpio_dir_mask(BANK(blue), MASK(blue), MASK(blue));
	gpio_dir_mask(BANK(ether), MASK(ether), MASK(ether));
#endif
	led_set(1, 1, 0);
	ether_set(1);
}


void led_init(unsigned gen, unsigned rev)
{
	generation = gen;
	switch (gen) {
	case 1:
		red = MIO(9);
		green = MIO(10);
		blue = MIO(11);
		ether = MIO(13);
		break;	
	case 2:
		red = MIO(6);
		green = MIO(7);
		blue = MIO(1);
		if (rev == 5)
			ether = MIO(3);
		else
			ether = MIO(0);
		break;
	default:
		fprintf(stderr, "unrecognized generation %u\n", gen);
		exit(1);
	}

	gpio_open();
}
