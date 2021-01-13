#
# Copyright (C) 2021 Linzhi Ltd.
#
# This work is licensed under the terms of the MIT License.
# A copy of the license can be found in the file COPYING.txt
#

NAME = rgbd

CFLAGS = -g -Wall -Wextra -Wshadow -Wno-unused-parameter \
         -Wmissing-prototypes -Wmissing-declarations \
	 -I../libcommon -pthread
override CFLAGS += $(CFLAGS_OVERRIDE)
LDLIBS = -L../libcommon -lcommon -lmosquitto -pthread

OBJS = $(NAME).o led.o gpio.o mqtt.o

include Makefile.c-common


.PHONY:		all spotless

all::		$(NAME)

$(NAME):	$(OBJS)

spotless::
		rm -f $(NAME)
