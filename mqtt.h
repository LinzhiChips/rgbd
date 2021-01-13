/*
 * mqtt.h - MQTT subscription and message processing
 *
 * Copyright (C) 2021 Linzhi Ltd.
 *
 * This work is licensed under the terms of the MIT License.
 * A copy of the license can be found in the file COPYING.txt
 */

#ifndef RGBD_MQTT_H
#define	RGBD_MQTT_H

void mqtt_init(void);
void mqtt_loop(void);

#endif /* !RGBD_MQTT_H */
