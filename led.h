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
