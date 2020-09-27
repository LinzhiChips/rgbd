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
