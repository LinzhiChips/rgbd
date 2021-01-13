/*
 * led.h - LED control
 *
 * Copyright (C) 2021 Linzhi Ltd.
 *
 * This work is licensed under the terms of the MIT License.
 * A copy of the license can be found in the file COPYING.txt
 */

#ifndef RGBD_LED_H
#define	RGBD_LED_H

#include <stdbool.h>


void decode_rgb(const char *s, bool *r, bool *g, bool *b);

void led_set(bool r, bool g, bool b);
void ether_set(bool on);

void led_blink(bool r, bool g, bool b, unsigned on_ms, unsigned off_ms);

void led_default(void);
void led_init(unsigned gen, unsigned rev);

#endif /* !RGBD_LED_H */
