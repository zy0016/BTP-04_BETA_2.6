#ifndef _LUBBOCK_H_
#define _LUBBOCK_H_

#include <hal/pxa_def.h>

#define LUBBOCK_FLASH0_PHYS		PXA_CS0_PHYS
#define LUBBOCK_FLASH0_VIRT		(0xf0000000)	/* phys 0x00000000 */

#define LUBBOCK_FLASH1_PHYS		PXA_CS1_PHYS
#define LUBBOCK_FLASH1_VIRT		(0xf2000000)	/* phys 0x04000000 */

#define LUBBOCK_FPGA_PHYS		PXA_CS2_PHYS
#define LUBBOCK_FPGA_VIRT		(0xf4400000)	/* phys 0x08000000 */

#define LUBBOCK_SRAM_PHYS		(0x0a000000)
#define LUBBOCK_SRAM_VIRT		(0xf4500000)	/* phys 0x0a000000 */

#define LUBBOCK_ETH_IO_PHYS		PXA_CS3_PHYS
#define LUBBOCK_ETH_IO_VIRT		(0xf4600000)	/* phys 0x0c000000 */

#define LUBBOCK_ETH_ATTR_PHYS	(0x0e000000)
#define LUBBOCK_ETH_ATTR_VIRT	(0xf4700000)	/* phys 0x0e000000 */

#define LUBBOCK_SA1111_BASE		(0xf4000000)	/* phys 0x10000000 */
#define LUBBOCK_SA1111_PHYS		(0x10000000)	/* phys 0x10000000 */

#define LUB_P2V(x)		(((unsigned long)(x)) - LUBBOCK_FPGA_PHYS + LUBBOCK_FPGA_VIRT)
#define LUB_V2P(x)		(((unsigned long)(x)) - LUBBOCK_FPGA_VIRT + LUBBOCK_FPGA_PHYS)

#define SRAM_P2V(x)		(((unsigned long)(x)) - LUBBOCK_SRAM_PHYS + LUBBOCK_SRAM_VIRT)
#define SRAM_V2P(x)		(((unsigned long)(x)) - LUBBOCK_SRAM_VIRT + LUBBOCK_SRAM_PHYS)

#define __LUB_REG(x)		(*((volatile unsigned short *)LUB_P2V(x)))
#define __LUB_REG32(x)		(*((volatile unsigned long *)LUB_P2V(x)))


/* FPGA register physical addresses */
#define _LUB_WHOAMI			(LUBBOCK_FPGA_PHYS + 0x000)
#define _LUB_HEXLED			(LUBBOCK_FPGA_PHYS + 0x010)
#define _LUB_DISC_BLNK_LED	(LUBBOCK_FPGA_PHYS + 0x040)
#define _LUB_CONF_SWITCHES	(LUBBOCK_FPGA_PHYS + 0x050)
#define _LUB_USER_SWITCHES	(LUBBOCK_FPGA_PHYS + 0x060)
#define _LUB_MISC_WR		(LUBBOCK_FPGA_PHYS + 0x080)
#define _LUB_MISC_RD		(LUBBOCK_FPGA_PHYS + 0x090)
#define _LUB_IRQ_MASK_EN	(LUBBOCK_FPGA_PHYS + 0x0C0)
#define _LUB_IRQ_SET_CLR	(LUBBOCK_FPGA_PHYS + 0x0D0)
#define _LUB_GP				(LUBBOCK_FPGA_PHYS + 0x100)

/* FPGA register virtual addresses */
#define LUB_WHOAMI			__LUB_REG(_LUB_WHOAMI)
#define LUB_HEXLED			__LUB_REG32(_LUB_HEXLED)		
#define LUB_DISC_BLNK_LED	__LUB_REG(_LUB_DISC_BLNK_LED)	
#define LUB_CONF_SWITCHES	__LUB_REG(_LUB_CONF_SWITCHES)	
#define LUB_USER_SWITCHES	__LUB_REG(_LUB_USER_SWITCHES)	
#define LUB_MISC_WR			__LUB_REG(_LUB_MISC_WR)	
#define LUB_MISC_RD			__LUB_REG(_LUB_MISC_RD)		
#define LUB_IRQ_MASK_EN		__LUB_REG(_LUB_IRQ_MASK_EN)
#define LUB_IRQ_SET_CLR		__LUB_REG(_LUB_IRQ_SET_CLR)		
#define LUB_GP				__LUB_REG(_LUB_GP)	

/* GPIOs */

#define GPIO_LUBBOCK_IRQ		0

/* interrupts */
#define LUB_INT_BASE			32
#define LUB_INT_MMC_IN			(LUB_INT_BASE + 0)
#define LUB_INT_SA1111			(LUB_INT_BASE + 1)
#define LUB_INT_USB_INSERT		(LUB_INT_BASE + 2)
#define LUB_INT_ETH				(LUB_INT_BASE + 3)
#define LUB_INT_UCB1400			(LUB_INT_BASE + 4)
#define LUB_INT_BBPEN			(LUB_INT_BASE + 5)
#define LUB_INT_USB_REMOVE		(LUB_INT_BASE + 6)
#define LUB_INT_END				LUB_INT_USB_REMOVE
/*
 * LED macros
 */

#define LEDS_BASE LUB_DISC_BLNK_LED

// 8 discrete leds available for general use:

#define LED_D28	0x1
#define LED_D27	0x2
#define LED_D26	0x4
#define LED_D25	0x8
#define LED_D24	0x10
#define LED_D23	0x20
#define LED_D22	0x40
#define LED_D21	0x80

/* Note: bits [15-8] are used to enable/blank the 8 7 segment hex displays so
*  be sure to not monkey with them here.
*/

#define HEARTBEAT_LED	LED_D27
#define SYS_BUSY_LED    LED_D28
#define HEXLEDS_BASE LUB_HEXLED

#define HEARTBEAT_LED_ON()  (LEDS_BASE &= ~HEARTBEAT_LED)
#define HEARTBEAT_LED_OFF() (LEDS_BASE |= HEARTBEAT_LED)
#define SYS_BUSY_LED_OFF()  (LEDS_BASE |= SYS_BUSY_LED)
#define SYS_BUSY_LED_ON()   (LEDS_BASE &= ~SYS_BUSY_LED)

// use x = D26-D21 for these, please...
#define DISCRETE_LED_ON(x) (LEDS_BASE &= ~(x))
#define DISCRETE_LED_OFF(x) (LEDS_BASE |= (x))

static __inline__ void lubbock_mask_irq(int irq)
{
	irq -= LUB_INT_BASE;
	LUB_IRQ_MASK_EN &= ~(0x01 << irq);
}

static __inline__ void lubbock_unmask_irq(int irq)
{
	irq -= LUB_INT_BASE;
	LUB_IRQ_MASK_EN |= (0x01 << irq);
}

#endif //_LUBBOCK_H_
