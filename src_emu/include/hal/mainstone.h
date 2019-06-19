#ifndef _MAINSTONE_H_
#define _MAINSTONE_H_

#define MAINSTONE_ROM_PHYS		BULV_CS0_PHYS	/* phys 0x00000000 32M */
#define MAINSTONE_ROM_VIRT		(0xf0000000)	

#define MAINSTONE_FLASH_PHYS	BULV_CS1_PHYS	/* phys 0x04000000 32M */
#define MAINSTONE_FLASH_VIRT	(0xf2000000)	

#define MAINSTONE_FPGA_PHYS		BULV_CS2_PHYS	/* phys 0x08000000 1M*/
#define MAINSTONE_FPGA_VIRT		(0xf4000000)	

#define MAINSTONE_SRAM_PHYS		(0x0A000000)	/* phys 0x0A000000 CS2 2M*/
#define MAINSTONE_SRAM_VIRT		(0xf4100000)	

#define MAINSTONE_ETH_IO_PHYS	BULV_CS4_PHYS	/* phys 0x10000000 1M*/
#define MAINSTONE_ETH_IO_VIRT	(0xf4300000)	

#define MAINSTONE_ETH_ATTR_PHYS	(0x12000000)	/* phys 0x12000000 CS4 1M*/
#define MAINSTONE_ETH_ATTR_VIRT	(0xf4400000)	

#ifdef __MMU__

#define fpga_p2v(x)		(((unsigned long)(x)) - MAINSTONE_FPGA_PHYS + MAINSTONE_FPGA_VIRT)
#define fpga_v2p(x)		(((unsigned long)(x)) - MAINSTONE_FPGA_VIRT + MAINSTONE_FPGA_PHYS)
#define sram_p2v(x)		(((unsigned long)(x)) - MAINSTONE_SRAM_PHYS + MAINSTONE_SRAM_VIRT)
#define sram_v2p(x)		(((unsigned long)(x)) - MAINSTONE_SRAM_VIRT + MAINSTONE_SRAM_PHYS)

#else

#define fpga_p2v(x)		(x)
#define fpga_v2p(x)		(x)
#define sram_p2v(x)		(x)
#define sram_v2p(x)		(x)

#endif

#define __FPGA_REG16(x)		(*(volatile unsigned short *)fpga_p2v(x))
#define __FPGA_REG32(x)		(*(volatile unsigned long *)fpga_p2v(x))

/* FPGA registers */
#define FPGA_LEDDAT1	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0x10)	/* HEX LED Date register 1 */
#define FPGA_LEDDAT2	__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x14)	/* HEX LED Date register 2 */
#define FPGA_LEDCTRL	__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x40)	/* LED Control register */
#define FPGA_GPSWR		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x60)	/* General purpose switch register */
#define FPGA_MSCWR1		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x80)	/* Misc write register 1 */
#define FPGA_MSCWR2		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x84)	/* Misc write register 2 */
#define FPGA_MSCWR3		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x88)	/* Misc write register 3 */
#define FPGA_MSCRD1		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0x90)	/* Misc read register 1 */
#define FPGA_INTMSKENA	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0xC0)	/* Interrupt mask/enable register */
#define FPGA_INTSETCLR	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0xD0)	/* Interrupt set/clear register */
#define FPGA_PCMCIA0	__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0xE0)	/* PCMCIA socket status/control register */
#define FPGA_PCMCIA1	__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0xE4)	/* PCMCIA socket status/control register */
#define FPGA_REVID		__FPGA_REG16(MAINSTONE_FPGA_PHYS + 0xF0)	/* PCMCIA socket status/control register */
#define FPGA_SCRATCH1	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0x100)	/* scratch register 1*/
#define FPGA_SCRATCH2	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0x110)	/* scratch register 2*/
#define FPGA_SCRATCH3	__FPGA_REG32(MAINSTONE_FPGA_PHYS + 0x120)	/* scratch register 3*/

#define MAINSTONE_IRQ		BULV_INT_GPIO_0

/* interrupts */
#define MAINSTONE_INT_BASE		(BULV_INT_MAX + 1)
#define MAINSTONE_INT_MMC		((MAINSTONE_INT_BASE) + 0)
#define MAINSTONE_INT_USIM		((MAINSTONE_INT_BASE) + 1)
#define MAINSTONE_INT_USBC		((MAINSTONE_INT_BASE) + 2)
#define MAINSTONE_INT_ETHERNET	((MAINSTONE_INT_BASE) + 3)
#define MAINSTONE_INT_AC97		((MAINSTONE_INT_BASE) + 4)
#define MAINSTONE_INT_PENIRQ	((MAINSTONE_INT_BASE) + 5)
#define MAINSTONE_INT_MSINS		((MAINSTONE_INT_BASE) + 6)
#define MAINSTONE_INT_EXBRD		((MAINSTONE_INT_BASE) + 7)
#define MAINSTONE_INT_S0CD		((MAINSTONE_INT_BASE) + 9)
#define MAINSTONE_INT_S0STSCH	((MAINSTONE_INT_BASE) + 10)
#define MAINSTONE_INT_S0IRQ		((MAINSTONE_INT_BASE) + 11)
#define MAINSTONE_INT_S1CD		((MAINSTONE_INT_BASE) + 13)
#define MAINSTONE_INT_S1STSCH	((MAINSTONE_INT_BASE) + 14)
#define MAINSTONE_INT_S1IRQ		((MAINSTONE_INT_BASE) + 15)
#define MAINSTONE_INT_PMC		((MAINSTONE_INT_BASE) + 16)
#define MAINSTONE_INT_BTDTR		((MAINSTONE_INT_BASE) + 17)
#define MAINSTONE_INT_BTRI		((MAINSTONE_INT_BASE) + 18)
#define MAINSTONE_INT_MX		((MAINSTONE_INT_BASE) + 19)

#define MAINSTONE_INT_MAX		MAINSTONE_INT_MX

#define mainstone_mask_irq(n)	do{	\
									FPGA_INTMSKENA &= ~(1 << ((n) - MAINSTONE_INT_BASE)); \
								}while(0)

#define mainstone_unmask_irq(n)	do{	\
									FPGA_INTMSKENA |= (1 << ((n) - MAINSTONE_INT_BASE)); \
								}while(0)

#define mainstone_ack_irq(n)	do{	\
								FPGA_INTSETCLR &= ~(1 << ((n) - MAINSTONE_INT_BASE)); \
							}while(0)

/*
 * LED macros
 */
#define LED_ON(n)		(FPGA_LEDCTRL &= ~(1 << (n)))
#define LED_OFF(n)		(FPGA_LEDCTRL |= (1 << (n)))
#define LED_INVERT(n)	(FPGA_LEDCTRL ^= (1 << (n)))

#define HEXLED_ON(n)		(FPGA_LEDCTRL &= ~(1 << ((n) + 8)))
#define HEXLED_OFF(n)		(FPGA_LEDCTRL |= (1 << ((n) + 8)))
#define HEXLED_DOTON(n)		(FPGA_LEDDAT2 &= ~(1 << (n)))
#define HEXLED_DOTOFF(n)	(FPGA_LEDDAT2 |= (1 << (n)))
#define HEXLED_DPON(n)		(FPGA_LEDDAT2 &= ~(1 << ((n) + 8)))
#define HEXLED_DPOFF(n)		(FPGA_LEDDAT2 |= (1 << ((n) + 8)))

#define HEXLED_SET(x)		(FPGA_LEDDAT1 = (x))

/*
 * Switch macros
 */

#define GPSW_HEX0		(FPGA_GPSWR & 0x0f)
#define GPSW_HEX1		((FPGA_GPSWR & 0xf0) >> 4)
#define GPSW_HEX		(FPGA_GPSWR & 0xff)
#define GPSW_TEST(n)	(FPGA_GPSWR & (1u << ((n) + 8)))

/*
 * Misc write register 1 bits
 */
#define MSCWR1_CAMERA_ON	(1 << 15)	/* camera interface power control */
#define MSCWR1_CAMERA_SEL	(1 << 14)	/* camera interface Mux control */
#define MSCWR1_LCD_CTRL		(1 << 13)	/* general-purpose LCD control signal */
#define MSCWR1_MS_ON		(1 << 12)	/* Memory stick power control */
#define MSCWR1_MMC_ON		(1 << 11)	/* MMC/SD power control */
#define MSCWR1_MS_SEL		(1 << 10)	/* MMC/SD, Memory stick multiplexer control */
#define MSCWR1_BB_SEL		(1 << 9)	/* PCMCIA, Baseband multiplexer control */
#define MSCWR1_BT_OFF		(1 << 8)	/* BTUART transceiver control */
#define MSCWR1_BTDTR		(1 << 7)	/* BTUART DTR */
#define MSCWR1_IRDA_MD1		(1 << 6)	/* Irda transceiver mode control bit 1 */
#define MSCWR1_IRDA_MD0		(1 << 5)	/* Irda transceiver mode control bit 0 */
#define MSCWR1_IRDA_FIR		(1 << 4)	/* Irda FIR/SIR */
#define MSCWR1_GREENLED		(1 << 3)	/* LED D1 */
#define MSCWR1_PDC_CTL		(1 << 2)	/* reserved */
#define MSCWR1_MTR_ON		(1 << 1)	/* silent alert motor control */
#define MSCWR1_SYSRESET		(1 << 0)	/* system reset */

/*
 * Misc write register 2 bits
 */
#define MSCWR2_LEGACY_SEL	(1 << 8)	/* Legacy mode control */
#define MSCWR2_GRAPHICS_SEL	(1 << 7)	/* Marathon Graphics accelerator control */
#define MSCWR2_USB_OTG_RST	(1 << 6)	/* USB OTG external transceiver Reset */
#define MSCWR2_USB_OTG_SEL	(1 << 5)	/* USB OTG control */
#define MSCWR2_USBC_SC		(1 << 4)	/* USB client soft connect control */
#define MSCWR2_LINE2		(1 << 3)	/* Line out 2 amplifier control */
#define MSCWR2_LINE1		(1 << 2)	/* Line out 1 amplifier control */
#define MSCWR2_RADIO_PWR	(1 << 1)	/* radio module power control */
#define MSCWR2_RADIO_WAKE	(1 << 0)	/* radio module wakeup signal */

/*
 * Misc write register 3 bits
 */
#define MSCWR3_GPIO_RESET_EN	(1 << 2)	/* gpio reset enable */
#define MSCWR3_GPIO_RESET		(1 << 1)	/* gpio reset */
#define MSCWR3_COMMS_SW_RESET	(1 << 0)	/* communication processor reset control */

/*
 * Misc read register 1 bits
 */
#define MSCRD1_FPG_PRES		(1 << 11)	/* Processor card present */
#define MSCRD1_POLL_FLAG	(1 << 10)	/* Marathon poll */
#define MSCRD1_PENIRQ		(1 << 9)	/* ADI7873 touch screen nPENIRQ signal */
#define MSCRD1_MS_CD		(1 << 8)	/* memory stick detection */
#define MSCRD1_MMC_CD		(1 << 7)	/* MMC/SD card detection */
#define MSCRD1_USIM_CD		(1 << 6)	/* USIM detection */
#define MSCRD1_USB_CBL		(1 << 5)	/* usb client cable connected */
#define MSCRD1_TS_BUSY		(1 << 4)	/*  ADI7873 touch screen busy */
#define MSCRD1_BTDSR		(1 << 3)	/* BTUART DSR */
#define MSCRD1_BTRI			(1 << 2)	/* BTUART RING */
#define MSCRD1_BTDCD		(1 << 1)	/* BTUART DCD */
#define MSCRD1_MMC_WP		(1 << 0)	/* MMC/SD write protect */


#endif //_MAINSTONE_H_
