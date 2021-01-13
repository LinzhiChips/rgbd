/*
 * gpio.c - Access to Zynq GPIOs
 *
 * Copyright (C) 2021 Linzhi Ltd.
 *
 * This work is licensed under the terms of the MIT License.
 * A copy of the license can be found in the file COPYING.txt
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "gpio.h"


#if 0
#define	debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define	debug(...)
#endif


volatile void *gpio_base;


static int fd = -1;


static size_t round_down_to_page(size_t bytes)
{
	int page = getpagesize();

	debug("\tround_down 0x%x -> 0x%x [%d]\n",
	    (unsigned) bytes, (unsigned) (bytes - (bytes % page)), page);
	return bytes - (bytes % page);
}


static size_t round_up_to_page(size_t bytes)
{
	int page = getpagesize();

	debug("\tround_up 0x%x -> 0x%x [%d]\n", (unsigned) bytes,
	    (unsigned) (round_down_to_page(bytes + page - 1)), page);
	return round_down_to_page(bytes + page - 1);
}


static size_t round_size_to_page(size_t addr, size_t size)
{
	debug("\tround_size 0x%x+0x%x -> 0x%x\n",
	    (unsigned) addr, (unsigned) size,
	    (unsigned) (round_up_to_page(addr + size) -
	    round_down_to_page(addr)));
	return round_up_to_page(addr + size) - round_down_to_page(addr);
}


static size_t page_offset(size_t addr)
{
	int page = getpagesize();

	debug("\tround_up 0x%x -> 0x%x\n", (unsigned) addr,
	     (unsigned) (addr % page));
	return addr % page;
}


volatile void *map_regs(size_t addr, size_t size)
{
	volatile void *map;

	debug("map_regs 0x%x addr 0x%u size\n",
	  (unsigned) addr, (unsigned) size);
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	map = mmap(NULL, round_size_to_page(addr, size),
	    PROT_READ | PROT_WRITE, MAP_SHARED, fd, round_down_to_page(addr));
	if (map == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	return map + page_offset(addr);
}


void unmap_regs(volatile void *map, size_t addr, uint32_t size)
{
	debug("unmap_regs\n");
	if (munmap((void *) map - page_offset(addr),
	    round_size_to_page(addr, size)) < 0) {
		perror("munmap");
		exit(1);
	}
	if (close(fd) < 0) {
		perror("close");
		exit(1);
	}
}


/*
 * Should we also set the GPIO clock gate (APER_CLK_CTRL.GPIO_CPU_1XCLKACT)
 * here ?
 *
 * Alternatively, enabling any GPIO (e.g., PS_DEBUG or the RGB LED controls)
 * through the sysfs user space interface will also enable the clock.
 */

void gpio_open(void)
{
	gpio_base = map_regs(GPIO_BASE, GPIO_SIZE);
}


void gpio_close(void)
{
	unmap_regs(gpio_base, GPIO_BASE, GPIO_SIZE);
}
