#ifndef GPIO_H
#define	GPIO_H

#include <stdint.h>


#define	GPIO_BASE	0xe000a000
#define	GPIO_SIZE	0x2e8

#define	MASK_DATA_LSW(n) \
			(*(volatile uint32_t *) (gpio_base + 0x000 + 8 * (n)))
#define	MASK_DATA_MSW(n) \
			(*(volatile uint32_t *) (gpio_base + 0x004 + 8 * (n)))
#define	MASK_DATA_0_LSW	(*(volatile uint32_t *) (gpio_base + 0x000))
#define	MASK_DATA_0_MSW	(*(volatile uint32_t *) (gpio_base + 0x004))
#define	MASK_DATA_1_LSW	(*(volatile uint32_t *) (gpio_base + 0x008))
#define	MASK_DATA_1_MSW	(*(volatile uint32_t *) (gpio_base + 0x00c))
#define	MASK_DATA_2_LSW	(*(volatile uint32_t *) (gpio_base + 0x010))
#define	MASK_DATA_2_MSW	(*(volatile uint32_t *) (gpio_base + 0x014))
#define	MASK_DATA_3_LSW	(*(volatile uint32_t *) (gpio_base + 0x018))
#define	MASK_DATA_3_MSW	(*(volatile uint32_t *) (gpio_base + 0x01c))

#define	DATA(n)		(*(volatile uint32_t *) (gpio_base + 0x040 + 4 * (n)))
#define	DATA_0		(*(volatile uint32_t *) (gpio_base + 0x040))
#define	DATA_1		(*(volatile uint32_t *) (gpio_base + 0x044))
#define	DATA_2		(*(volatile uint32_t *) (gpio_base + 0x048))
#define	DATA_3		(*(volatile uint32_t *) (gpio_base + 0x04c))

#define	DATA_RO(n)	(*(volatile uint32_t *) (gpio_base + 0x060 + 4 * (n)))
#define	DATA_0_RO	(*(volatile uint32_t *) (gpio_base + 0x060))
#define	DATA_1_RO	(*(volatile uint32_t *) (gpio_base + 0x064))
#define	DATA_2_RO	(*(volatile uint32_t *) (gpio_base + 0x068))
#define	DATA_3_RO	(*(volatile uint32_t *) (gpio_base + 0x06c))

#define	DIRM(n)		(*(volatile uint32_t *) \
			    (gpio_base + 0x204 + 0x40 * (n)))
#define	OEN(n)		(*(volatile uint32_t *) \
			    (gpio_base + 0x208 + 0x40 * (n)))
#define	DIRM_0		(*(volatile uint32_t *) (gpio_base + 0x204))
#define	OEN_0		(*(volatile uint32_t *) (gpio_base + 0x208))
#define	DIRM_1		(*(volatile uint32_t *) (gpio_base + 0x244))
#define	OEN_1		(*(volatile uint32_t *) (gpio_base + 0x248))
#define	DIRM_2		(*(volatile uint32_t *) (gpio_base + 0x284))
#define	OEN_2		(*(volatile uint32_t *) (gpio_base + 0x288))
#define	DIRM_3		(*(volatile uint32_t *) (gpio_base + 0x2c4))
#define	OEN_3		(*(volatile uint32_t *) (gpio_base + 0x2c8))


extern volatile void *gpio_base;


static inline void gpio_dir(uint8_t bank, uint32_t out_mask)
{
	DIRM(bank) = out_mask;
	OEN(bank) = out_mask;
}


static inline void gpio_dir_mask(uint8_t bank, uint32_t mask, uint32_t out_mask)
{
	DIRM(bank) = (DIRM(bank) & ~mask) | out_mask;
	OEN(bank) = (OEN(bank) & ~mask) | out_mask;
}


static inline void gpio_msw(uint8_t bank, uint16_t mask, uint16_t value)
{
	MASK_DATA_MSW(bank) = mask << 16 | value;
}


static inline void gpio_lsw(uint8_t bank, uint16_t mask, uint16_t value)
{
	MASK_DATA_LSW(bank) = mask << 16 | value;
}


static inline void gpio_sw(uint8_t bank, uint32_t mask, uint32_t value)
{
	if ((mask & 0xffff) != 0xffff)
		gpio_lsw(bank, mask & 0xffff, value & 0xffff);
	if ((mask >> 16) != 0xffff)
		gpio_msw(bank, mask >> 16, value >> 16);
}


static inline void gpio_out(uint8_t bank, uint32_t value)
{
	DATA(bank) = value;
}


static inline uint32_t gpio_in(uint8_t bank)
{
	return DATA_RO(bank);
}


volatile void *map_regs(size_t addr, size_t size);
void unmap_regs(volatile void *map, size_t addr, uint32_t size);

void gpio_open(void);
void gpio_close(void);

#endif /* !GPIO_H */
