#ifndef __HAL_BULVERDE_H_
#define __HAL_BULVERDE_H_

/*
 * Bulverde Chip selects
 */

#define BULV_CS0_PHYS		0x00000000
#define BULV_CS1_PHYS		0x04000000
#define BULV_CS2_PHYS		0x08000000
#define BULV_CS3_PHYS		0x0C000000
#define BULV_CS4_PHYS		0x10000000
#define BULV_CS5_PHYS		0x14000000
#define BULV_PCMCIA0_PHYS	0x20000000
#define BULV_PCMCIA1_PHYS	0x30000000
#define BULV_SDRAM_PHYS		0xA0000000

/*
 * Intel Bulverde internal registers mappings:
 *
 * 0x40000000 - 0x41ffffff <--> 0xe0000000 - 0xe1ffffff		//Peripheral Modules Registers
 * 0x44000000 - 0x45ffffff <--> 0xe2000000 - 0xe3ffffff		//LCD Controller
 * 0x48000000 - 0x49ffffff <--> 0xe4000000 - 0xe5ffffff		//Memory Controller
 * 0x4c000000 - 0x4dffffff <--> 0xe6000000 - 0xe7ffffff		//USB Host Controller
 * 0x50000000 - 0x51ffffff <--> 0xe8000000 - 0xe9ffffff		//Camera Interface
 * 0x54000000 - 0x55ffffff <--> 0xea000000 - 0xebffffff		//reserved
 * 0x58000000 - 0x59ffffff <--> 0xec000000 - 0xedffffff		//Internal Memory Controll
 * 0x5c000000 - 0x5dffffff <--> 0xee000000 - 0xefffffff		//Internal Memory Storage
 */
#define __MMU__
#ifdef __MMU__
#define io_p2v(x)	( 0xe0000000 | ( (unsigned long)(x) & 0x01ffffff ) | (( (unsigned long)(x) & 0x1c000000 ) >> 1) )
#define io_v2p(x)	( 0x40000000 | ( (unsigned long)(x) & 0x01ffffff ) | (( (unsigned long)(x) & 0x0e000000 ) << 1) )
#else
#define io_p2v(x)	( (unsigned long)(x) )
#define io_v2p(x)	( (unsigned long)(x) )
#endif

#define IO_ADDRESS(x)		(io_p2v(x))

#define __REG(x) 		(*(volatile unsigned long*)(IO_ADDRESS(x)))
#define __REG2(x, y) 	__REG((unsigned long)(x) + (y))
#define __PREG(x)		io_v2p((unsigned long)&(x))


/* IO address base, virtual address */
#define BULV_DMAC_BASE		IO_ADDRESS(0x40000000)  // Direct Memory Access Controller registers
#define BULV_FFUART_BASE	IO_ADDRESS(0x40100000)  // Full Function UART
#define BULV_BTUART_BASE	IO_ADDRESS(0x40200000)  // Bluetooth UART
#define BULV_I2C_BASE 		IO_ADDRESS(0x40300000)	// Inter-Integrated Circuit (I2C) bus interface unit
#define BULV_I2S_BASE		IO_ADDRESS(0x40400000)  // Inter-Integrated Circuit Sound (I2S)
#define BULV_AC97_BASE		IO_ADDRESS(0x40500000)  // AC97 Controller Unit (ACUNIT)
#define BULV_UDC_BASE		IO_ADDRESS(0x40600000)  // Universal Serial Bus Device Controller
#define BULV_STUART_BASE	IO_ADDRESS(0x40700000)  // Standard UART
#define BULV_ICP_BASE		IO_ADDRESS(0x40800000)  // Fast Infrared Communications Port
#define BULV_RTC_BASE		IO_ADDRESS(0x40900000)  // realtime clock
#define BULV_OST_BASE		IO_ADDRESS(0x40A00000)  // OS Timers
#define BULV_PWM0_BASE		IO_ADDRESS(0x40B00000)  // pulse width modulator 0
#define BULV_PWM1_BASE		IO_ADDRESS(0x40C00000)  // pulse width modulator 1
#define BULV_IC_BASE 		IO_ADDRESS(0x40D00000)	// interrupt controller registers
#define BULV_GPIO_BASE		IO_ADDRESS(0x40E00000)	// gpio
#define BULV_PMRC_BASE		IO_ADDRESS(0x40F00000)	// Power Manager and Reset Control
#define BULV_SSP_BASE 		IO_ADDRESS(0x41000000)	// Serial Port Controller¡¯s (SSPC)
#define BULV_MMC_BASE		IO_ADDRESS(0x41100000)  // MultiMediaCard (MMC) controller
#define BULV_CLKM_BASE		IO_ADDRESS(0x41300000)  // Clocks Manager
#define BULV_MSL_BASE		IO_ADDRESS(0x41400000)  // Mobile Scalable Link registers
#define BULV_KEYPAD_BASE	IO_ADDRESS(0x41500000)  // Keypad Interface registers
#define BULV_USIM_BASE		IO_ADDRESS(0x41600000)  // USIM registers
#define BULV_SSP2_BASE		IO_ADDRESS(0x41700000)  // SSP2 registers
#define BULV_MS_BASE		IO_ADDRESS(0x41800000)  // Memory stick registers
#define BULV_SSP3_BASE		IO_ADDRESS(0x41900000)  // SSP3 registers

#define BULV_LCDC_BASE 		IO_ADDRESS(0x44000000)	// LCD controller registers
#define BULV_MC_BASE 		IO_ADDRESS(0x48000000)	// Memory Configuration Registers
#define BULV_USBH_BASE		IO_ADDRESS(0x4C000000)	// USB Host Registers
#define BULV_CI_BASE		IO_ADDRESS(0x50000000)	// Camera interface Registers
#define BULV_IMC_BASE		IO_ADDRESS(0x58000000)	// Internal Memory Control Registers
#define BULV_IMS_BASE		IO_ADDRESS(0x58000000)	// Internal Memory Storage

/* interrupt numbers */
#define BULV_INT_SSP3			0
#define BULV_INT_MSL			1
#define BULV_INT_USBH2			2
#define BULV_INT_USBH1			3
#define BULV_INT_KEYPAD			4
#define BULV_INT_MEM_STK		5
#define BULV_INT_PWR_I2C		6
#define BULV_INT_OST_4_11		7
#define BULV_INT_GPIO_0			8
#define BULV_INT_GPIO_1			9
#define BULV_INT_GPIO_X			10
#define BULV_INT_USBC			11
#define BULV_INT_PMU			12
#define BULV_INT_I2S			13
#define BULV_INT_AC97			14
#define BULV_INT_USIM			15
#define BULV_INT_SSP2			16
#define BULV_INT_LCD			17
#define BULV_INT_I2C			18
#define BULV_INT_ICP			19
#define BULV_INT_STUART			20
#define BULV_INT_BTUART			21
#define BULV_INT_FFUART			22
#define BULV_INT_MMC			23
#define BULV_INT_SSP1			24
#define BULV_INT_DMAC			25
#define BULV_INT_OST0			26
#define BULV_INT_OST1			27
#define BULV_INT_OST2			28
#define BULV_INT_OST3			29
#define BULV_INT_RTC_HZ			30
#define BULV_INT_RTC_AL			31
#define BULV_INT_CIF			33
#define BULV_INT_MAX			39

/*
 * Interrupt Controller
 */
#define ICIP		__REG(0x40D00000)  /* Interrupt Controller IRQ Pending Register */
#define ICMR		__REG(0x40D00004)  /* Interrupt Controller Mask Register */
#define ICLR		__REG(0x40D00008)  /* Interrupt Controller Level Register */
#define ICFP		__REG(0x40D0000C)  /* Interrupt Controller FIQ Pending Register */
#define ICPR		__REG(0x40D00010)  /* Interrupt Controller Pending Register */
#define ICCR		__REG(0x40D00014)  /* Interrupt Controller Control Register */
#define ICHP		__REG(0x40D00018)  /* Interrupt Controller Highest Priority Register */

#define ICIP2		__REG(0x40D0009C)  /* Interrupt Controller IRQ Pending Register 2 */
#define ICMR2		__REG(0x40D000A0)  /* Interrupt Controller Mask Register 2 */
#define ICLR2		__REG(0x40D000A4)  /* Interrupt Controller Level Register 2 */
#define ICFP2		__REG(0x40D000A8)  /* Interrupt Controller FIQ Pending Register 2 */
#define ICPR2		__REG(0x40D000AC)  /* Interrupt Controller Pending Register 2 */

#define IPR(x)		__REG2(0x40D0001C, (x) << 2) 
#define IPR2(x)		__REG2(0x40D000B0, ((x) & 0x1f) << 2) 

#define IPR_VAL			(1u << 31)	/* Valid bit */
#define IPR_PID_MASK	0x3f		/* Peripheral ID (mask) */

#define ICHP_VAL_IRQ	(1u << 31)	/* Valid IRQ */
#define ICHP_IRQ_MASK	(0x3F << 16)
#define ICHP_IRQ_SHIFT	16
#define ICHP_VAL_FIQ	(1u << 15)	/* Valid FIQ */
#define ICHP_FIQ_MASK	0x3F
#define ICHP_FIQ_SHIFT	0

/*
 * Core Clock
 */

#define CCCR		__REG(0x41300000)  /* Core Clock Configuration Register */
#define CKEN		__REG(0x41300004)  /* Clock Enable Register */
#define OSCC		__REG(0x41300008)  /* Oscillator Configuration Register */
#define CCSR		__REG(0x4130000C)  /* Oscillator Configuration Register */

#define CCCR_CPDIS			(1 << 31)	/* Core PLL Output Disable */
#define CCCR_PPDIS			(1 << 30)	/* Peripheral PLL Output Disable */
#define CCCR_LCD26			(1 << 27)	/* LCD Freq. 13MHz/26MHz */
#define CCCR_PLL_EARLY_EN	(1 << 26)	/* PLLs early enable */
#define CCCR_A				(1 << 25)	/* MEMC clock alternate */

#define CCCR_2N_MASK		0x0780		/* Turbo Mode to run mode ratio */
#define CCCR_2N_SHIFT		7			/* Turbo Mode to run mode ratio, shift bits */
#define CCCR_L_MASK			0x001f		/* Run mode to oscillator ratio */

#define CKEN24_CAMERA	(1 << 24) /* Camera interface Clock Enable */
#define CKEN23_SSP1		(1 << 23) /* SSP1 Clock Enable */
#define CKEN22_MEMC		(1 << 22) /* Memory Controller Clock Enable */
#define CKEN21_MEMSTICK	(1 << 21) /* Memory Stick Clock Enable */
#define CKEN20_IMEM		(1 << 20) /* Internal Memory Clock Enable */
#define CKEN19_KEYPAD	(1 << 19) /* Keypad Interface Clock Enable */
#define CKEN18_USIM		(1 << 18) /* USIM Clock Enable */
#define CKEN17_MSL		(1 << 17) /* MSL Clock Enable */
#define CKEN16_LCD		(1 << 16)	/* LCD Unit Clock Enable */
#define CKEN15_PI2C		(1 << 15) /* Power I2C Clock Enable */
#define CKEN14_I2C		(1 << 14)	/* I2C Unit Clock Enable */
#define CKEN13_FICP		(1 << 13)	/* FICP Unit Clock Enable */
#define CKEN12_MMC		(1 << 12)	/* MMC Unit Clock Enable */
#define CKEN11_USB		(1 << 11)	/* USB Unit Clock Enable */
#define CKEN10_USBHOST	(1 << 10) /* USB Host Clock Enable */
#define CKEN9_OST		(1 << 9) /* OS Timer Clock Enable */
#define CKEN8_I2S		(1 << 8)	/* I2S Unit Clock Enable */
#define CKEN7_BTUART	(1 << 7)	/* BTUART Unit Clock Enable */
#define CKEN6_FFUART	(1 << 6)	/* FFUART Unit Clock Enable */
#define CKEN5_STUART	(1 << 5)	/* STUART Unit Clock Enable */
#define CKEN4_SSP3		(1 << 4) /* SSP3 Clock Enable */
#define CKEN3_SSP2		(1 << 3)	/* SSP Unit Clock Enable */
#define CKEN2_AC97		(1 << 2)	/* AC97 Unit Clock Enable */
#define CKEN1_PWM1		(1 << 1)	/* PWM1 Clock Enable */
#define CKEN0_PWM0		(1 << 0)	/* PWM0 Clock Enable */

#define OSCC_OON	(1 << 1)	/* 32.768kHz OON (write-once only bit) */
#define OSCC_OOK	(1 << 0)	/* 32.768kHz OOK (read-only bit) */
#define OSCC_CRI	(1 << 2)	/* Clock Request Input */
#define OSCC_OSD	(1 << 3)	/* Processor Oscillator Stabilization Delay */

#define CCSR_CPDIS_S	(1 << 31)	/* Core PLL Output Disable Status*/
#define CCSR_PPDIS_S	(1 << 30)	/* Peripheral PLL Output Disable Status */
#define CCSR_CPLCK		(1 << 29)	/* Core PLL Lock */
#define CCSR_PPLCK		(1 << 28)	/* Peripheral PLL Lock */
#define CCSR_2N_S_MASK	0x0780		/* N Status */
#define CCSR_2N_S_SHIFT	7		/* N Status */
#define CCSR_L_S_MASK 	0x001f		/* L Status */

/*
 * OS Timer & Match Registers
 */

#define OSMR0		__REG(0x40A00000)  /* OS Timer Match Register 0 */
#define OSMR1		__REG(0x40A00004)  /* OS Timer Match Register 1 */
#define OSMR2		__REG(0x40A00008)  /* OS Timer Match Register 2 */
#define OSMR3		__REG(0x40A0000C)  /* OS Timer Match Register 3 */
#define OSCR0		__REG(0x40A00010)  /* OS Timer Counter Register */
#define OSSR		__REG(0x40A00014)  /* OS Timer Status Register */
#define OWER		__REG(0x40A00018)  /* OS Timer Watchdog Enable Register */
#define OIER		__REG(0x40A0001C)  /* OS Timer Interrupt Enable Register */

#define OMCR4		__REG(0x40A000C0)  /* OS Match Control Register 4 */
#define OMCR5		__REG(0x40A000C4)  /* OS Match Control Register 5 */
#define OMCR6		__REG(0x40A000C8)  /* OS Match Control Register 6 */
#define OMCR7		__REG(0x40A000CC)  /* OS Match Control Register 7 */
#define OMCR8		__REG(0x40A000D0)  /* OS Match Control Register 8 */
#define OMCR9		__REG(0x40A000D4)  /* OS Match Control Register 9 */
#define OMCR10		__REG(0x40A000D8)  /* OS Match Control Register 10 */
#define OMCR11		__REG(0x40A000DC)  /* OS Match Control Register 11 */

#define OSMR4		__REG(0x40A00080)  /* OS Timer Match Register 4 */
#define OSMR5		__REG(0x40A00084)  /* OS Timer Match Register 5 */
#define OSMR6		__REG(0x40A00088)  /* OS Timer Match Register 6 */
#define OSMR7		__REG(0x40A0008C)  /* OS Timer Match Register 7 */
#define OSMR8		__REG(0x40A00090)  /* OS Timer Match Register 8 */
#define OSMR9		__REG(0x40A00094)  /* OS Timer Match Register 9 */
#define OSMR10		__REG(0x40A00098)  /* OS Timer Match Register 10 */
#define OSMR11		__REG(0x40A0009C)  /* OS Timer Match Register 11 */

#define OSCR4		__REG(0x40A00040)  /* OS Timer Counter Register 4 */
#define OSCR5		__REG(0x40A00044)  /* OS Timer Counter Register 5 */
#define OSCR6		__REG(0x40A00048)  /* OS Timer Counter Register 6 */
#define OSCR7		__REG(0x40A0004C)  /* OS Timer Counter Register 7 */
#define OSCR8		__REG(0x40A00050)  /* OS Timer Counter Register 8 */
#define OSCR9		__REG(0x40A00054)  /* OS Timer Counter Register 9 */
#define OSCR10		__REG(0x40A00058)  /* OS Timer Counter Register 10 */
#define OSCR11		__REG(0x40A0005C)  /* OS Timer Counter Register 11 */

#define OWER_WME	(1 << 0)	/* Watchdog Match Enable */

/* OMCR4-11 bits */
#define OMCR_N			(1 << 9)	/* Snapshot Mode(OMCR9/11 only) (r/w) */
#define OMCR_CRES3		(1 << 8)	/* CRES Most significant bit(OMCR8/9/10/11) (r/w) */
#define OMCR_C			(1 << 7)	/* Channel Matach (r/w) */
#define OMCR_P			(1 << 6)	/* Periodic Timer (r/w) */
#define OMCR_S1			(1 << 5)	/* external sync control bit 1 (r/w) */
#define OMCR_S0			(1 << 4)	/* external sync control bit 0 (r/w) */
#define OMCR_R			(1 << 3)	/* Reset on match (r/w) */
#define OMCR_CRES_MASK	0x07		/* Counter Resolution (mask) */

/* Counter Resolution definition */
#define OMCR_CRES_DISABLE	0	/* counter disbale */
#define OMCR_CRES_32768		1	/* 1/32768th of a second */
#define OMCR_CRES_1MS		2	/* =>1 millisecond */
#define OMCR_CRES_1S		3	/* =>1 second */
#define OMCR_CRES_1US		4	/* =>1 microsecond */
#define OMCR_CRES_EXT		5	/* external supplied clock */

/* following settings applied to channel 8/9/10/11 only */
#define OMCR_CRES_SSP1		6	/* SSP1 frame detect */
#define OMCR_CRES_SSP2		7	/* SSP2 frame detect */
#define OMCR_CRES_SSP3		8	/* SSP3 frame detect, USE CRES3 */
#define OMCR_CRES_UDC		9	/* UDC frame detect, USE CRES3 */

#define OSMR0_3(x)	__REG2(0x40A00000, (x) << 2)
#define OMCR4_11(x)	__REG2(0x40A000C0, ((x) - 4) << 2)
#define OSMR4_11(x)	__REG2(0x40A00080, ((x) - 4) << 2)
#define OSCR4_11(x)	__REG2(0x40A00040, ((x) - 4) << 2)

/*
 * General Purpose I/O
 */

#define GPLR0		__REG(0x40E00000)  /* GPIO Pin-Level Register GPIO<31:0> */
#define GPLR1		__REG(0x40E00004)  /* GPIO Pin-Level Register GPIO<63:32> */
#define GPLR2		__REG(0x40E00008)  /* GPIO Pin-Level Register GPIO<80:64> */

#define GPDR0		__REG(0x40E0000C)  /* GPIO Pin Direction Register GPIO<31:0> */
#define GPDR1		__REG(0x40E00010)  /* GPIO Pin Direction Register GPIO<63:32> */
#define GPDR2		__REG(0x40E00014)  /* GPIO Pin Direction Register GPIO<80:64> */

#define GPSR0		__REG(0x40E00018)  /* GPIO Pin Output Set Register GPIO<31:0> */
#define GPSR1		__REG(0x40E0001C)  /* GPIO Pin Output Set Register GPIO<63:32> */
#define GPSR2		__REG(0x40E00020)  /* GPIO Pin Output Set Register GPIO<80:64> */

#define GPCR0		__REG(0x40E00024)  /* GPIO Pin Output Clear Register GPIO<31:0> */
#define GPCR1		__REG(0x40E00028)  /* GPIO Pin Output Clear Register GPIO <63:32> */
#define GPCR2		__REG(0x40E0002C)  /* GPIO Pin Output Clear Register GPIO <80:64> */

#define GRER0		__REG(0x40E00030)  /* GPIO Rising-Edge Detect Register GPIO<31:0> */
#define GRER1		__REG(0x40E00034)  /* GPIO Rising-Edge Detect Register GPIO<63:32> */
#define GRER2		__REG(0x40E00038)  /* GPIO Rising-Edge Detect Register GPIO<80:64> */

#define GFER0		__REG(0x40E0003C)  /* GPIO Falling-Edge Detect Register GPIO<31:0> */
#define GFER1		__REG(0x40E00040)  /* GPIO Falling-Edge Detect Register GPIO<63:32> */
#define GFER2		__REG(0x40E00044)  /* GPIO Falling-Edge Detect Register GPIO<80:64> */

#define GEDR0		__REG(0x40E00048)  /* GPIO Edge Detect Status Register GPIO<31:0> */
#define GEDR1		__REG(0x40E0004C)  /* GPIO Edge Detect Status Register GPIO<63:32> */
#define GEDR2		__REG(0x40E00050)  /* GPIO Edge Detect Status Register GPIO<80:64> */

#define GAFR0_L		__REG(0x40E00054)  /* GPIO Alternate Function Select Register GPIO<15:0> */
#define GAFR0_U		__REG(0x40E00058)  /* GPIO Alternate Function Select Register GPIO<31:16> */
#define GAFR1_L		__REG(0x40E0005C)  /* GPIO Alternate Function Select Register GPIO<47:32> */
#define GAFR1_U		__REG(0x40E00060)  /* GPIO Alternate Function Select Register GPIO<63:48> */
#define GAFR2_L		__REG(0x40E00064)  /* GPIO Alternate Function Select Register GPIO<79:64> */
#define GAFR2_U		__REG(0x40E00068)  /* GPIO Alternate Function Select Register GPIO<95:80>*/

#define GPLR3		__REG(0x40E00100)  /* GPIO Pin-Level Register GPIO<120:96> */
#define GPDR3		__REG(0x40E0010C)  /* GPIO Pin Direction Register GPIO<120:96> */
#define GPSR3		__REG(0x40E00118)  /* GPIO Pin Output Set Register GPIO<120:96> */
#define GPCR3		__REG(0x40E00124)  /* GPIO Pin Output Clear Register GPIO<120:96> */
#define GRER3		__REG(0x40E00130)  /* GPIO Rising-Edge Detect Register GPIO<120:96> */
#define GFER3		__REG(0x40E0013C)  /* GPIO Falling-Edge Detect Register GPIO<120:96> */
#define GEDR3		__REG(0x40E00148)  /* GPIO Edge Detect Status Register GPIO<120:96> */
#define GAFR3_L		__REG(0x40E0006C)  /* GPIO Alternate Function Select Register GPIO<111:96> */
#define GAFR3_U		__REG(0x40E00070)  /* GPIO Alternate Function Select Register GPIO<120:112> */

#define GPIO_bit(x)	(1 << ((x) & 0x1f))
#define GPLR(x)		__REG2(0x40E00000, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GPDR(x)		__REG2(0x40E0000C, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GPSR(x)		__REG2(0x40E00018, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GPCR(x)		__REG2(0x40E00024, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GRER(x)		__REG2(0x40E00030, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GFER(x)		__REG2(0x40E0003C, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GEDR(x)		__REG2(0x40E00048, (((x) < 96) ? (((x) & 0x60) >> 3) : 0x100))
#define GAFR(x)		__REG2(0x40E00054, ((x) & 0x70) >> 2)

/* GPIO alternate function mode & direction */

#define GPIO_IN				0x000
#define GPIO_OUT			0x080
#define GPIO_ALT_FN_1_IN	0x100
#define GPIO_ALT_FN_1_OUT	0x180
#define GPIO_ALT_FN_2_IN	0x200
#define GPIO_ALT_FN_2_OUT	0x280
#define GPIO_ALT_FN_3_IN	0x300
#define GPIO_ALT_FN_3_OUT	0x380
#define GPIO_MD_MASK_NR		0x07f
#define GPIO_MD_MASK_DIR	0x080
#define GPIO_MD_MASK_FN		0x300


/*
 * DMA Controller
 */

#define DRCMR(x)	__REG2(0x40000100, (((x) & 0x3f) << 2) | (((x) & 0x40) << 6))

#define DRCMR_MAPVLD	(1 << 7)	/* Map Valid (read / write) */
#define DRCMR_CHLNUM	0x1f		/* mask for Channel Number (read / write) */

#define DDADR(x)	__REG2(0x40000200, (x) << 4)

#define DDADR_DESCADDR	0xfffffff0	/* Address of next descriptor (mask) */
#define DDADR_BREN	(1 << 1)	/* Enable Discriptor Branching (read / write) */
#define DDADR_STOP	(1 << 0)	/* Stop (read / write) */

#define DSADR(x)	__REG2(0x40000204, (x) << 4)
#define DTADR(x)	__REG2(0x40000208, (x) << 4)

#define DCMD(x)		__REG2(0x4000020c, (x) << 4)

#define DCMD_INCSRCADDR	(1 << 31)	/* Source Address Increment Setting. */
#define DCMD_INCTRGADDR	(1 << 30)	/* Target Address Increment Setting. */
#define DCMD_FLOWSRC	(1 << 29)	/* Flow Control by the source. */
#define DCMD_FLOWTRG	(1 << 28)	/* Flow Control by the target. */
#define DCMD_CMPEN		(1 << 25)	/* Descriptor Compare Enable. */
#define DCMD_ADDRMODE	(1 << 23)	/* Address mode */
#define DCMD_STARTIRQEN	(1 << 22)	/* Start Interrupt Enable */
#define DCMD_ENDIRQEN	(1 << 21)	/* End Interrupt Enable */
#define DCMD_FLYBYS		(1 << 20)	/* Fly-by source */
#define DCMD_FLYBYT		(1 << 19)	/* Fly-by target */
#define DCMD_BURST8		(1 << 16)	/* 8 byte burst */
#define DCMD_BURST16	(2 << 16)	/* 16 byte burst */
#define DCMD_BURST32	(3 << 16)	/* 32 byte burst */
#define DCMD_WIDTH1		(1 << 14)	/* 1 byte width */
#define DCMD_WIDTH2		(2 << 14)	/* 2 byte width (HalfWord) */
#define DCMD_WIDTH4		(3 << 14)	/* 4 byte width (Word) */
#define DCMD_LENGTH		0x01fff		/* length mask (max = 8K - 1) */

/* default combinations */
#define DCMD_RXPCDR	(DCMD_INCTRGADDR|DCMD_FLOWSRC|DCMD_BURST32|DCMD_WIDTH4)
#define DCMD_RXMCDR	(DCMD_INCTRGADDR|DCMD_FLOWSRC|DCMD_BURST32|DCMD_WIDTH4)
#define DCMD_TXPCDR	(DCMD_INCSRCADDR|DCMD_FLOWTRG|DCMD_BURST32|DCMD_WIDTH4)

#define FLYCNFG			__REG(0x48000020)
#define FLYCNFG_FBPOL1	(1 << 16)	/* Fly-by DMA DVAL(1) polarity */
#define FLYCNFG_FBPOL0	(1 << 0)	/* Fly-by DMA DVAL(0) polarity */

#define DRQSR(x)		__REG2(0x400000e0, (x) << 2)

#define DRQSR_CLR		(1 << 8)	/* Clear Request */
#define DRQSR_REQPEND	0x1f		/* number of pending request (mask)*/

#define DCSR(x)		__REG2(0x40000000, (x) << 2)

#define DCSR_RUN		(1 << 31)	/* Run Bit (read / write) */
#define DCSR_NODESC		(1 << 30)	/* No-Descriptor Fetch (read / write) */
#define DCSR_STOPIRQEN	(1 << 29)	/* Stop Interrupt Enable (read / write) */
#define DCSR_EORIRQEN	(1 << 28)	/* End-Of_Receive Interrupt Enable (read / write) */
#define DCSR_EORJMPEN	(1 << 27)	/* Jump to Next Discriptor on EOR (read / write) */
#define DCSR_EORSTOPEN	(1 << 26)	/* Stop on EOR (read / write) */
#define DCSR_SETCMPST	(1 << 25)	/* Set Descriptor Compare Status (write) */
#define DCSR_CLRCMPST	(1 << 24)	/* Clear Descriptor Compare Status (write) */
#define DCSR_RASIRQEN	(1 << 23)	/* Request After Channel Stopped Interrupt Enable (read / write) */
#define DCSR_MASKRUN	(1 << 22)	/* Mask DSCR[RUN] (write) */
#define DCSR_CMPST		(1 << 10)	/* Descriptor Compare Status (r/w) */
#define DCSR_EORINT		(1 << 9)	/* End-Of-Request (r/w) */
#define DCSR_REQPEND	(1 << 8)	/* Request Pending (read-only) */
#define DCSR_RASINTR	(1 << 4)	/* Request After Channel Stopped (r/w) */
#define DCSR_STOPSTATE	(1 << 3)	/* Stop State (read-only) */
#define DCSR_ENDINTR	(1 << 2)	/* End Interrupt (read / write) */
#define DCSR_STARTINTR	(1 << 1)	/* Start Interrupt (read / write) */
#define DCSR_BUSERR		(1 << 0)	/* Bus Error Interrupt (read / write) */

#define DINT		__REG(0x400000f0)  /* DMA Interrupt Register */

#define DALIGN		__REG(0x400000a0)  /* DMA Alignment Register */

#define DPCSR		__REG(0x400000a4)  /* DMA Programmed I/O Control Status Register */

#define DPCSR_BRGSPLIT	(1 << 31)	/* Active posted writes and split reads */
#define DPCSR_BRGBUSY	(1 << 0)	/* Bridge busy status */

/*
 * UARTs
 */

/* Full Function UART (FFUART) */
#define FFUART		FFRBR
#define FFRBR		__REG(0x40100000)  /* Receive Buffer Register (read only) */
#define FFTHR		__REG(0x40100000)  /* Transmit Holding Register (write only) */
#define FFIER		__REG(0x40100004)  /* Interrupt Enable Register (read/write) */
#define FFIIR		__REG(0x40100008)  /* Interrupt ID Register (read only) */
#define FFFCR		__REG(0x40100008)  /* FIFO Control Register (write only) */
#define FFLCR		__REG(0x4010000C)  /* Line Control Register (read/write) */
#define FFMCR		__REG(0x40100010)  /* Modem Control Register (read/write) */
#define FFLSR		__REG(0x40100014)  /* Line Status Register (read only) */
#define FFMSR		__REG(0x40100018)  /* Modem Status Register (read only) */
#define FFSCR		__REG(0x4010001C)  /* Scratch Pad Register (read/write) */
#define FFISR		__REG(0x40100020)  /* Infrared Selection Register (read/write) */
#define FFDLL		__REG(0x40100000)  /* Divisor Latch Low Register (DLAB = 1) (read/write) */
#define FFDLH		__REG(0x40100004)  /* Divisor Latch High Register (DLAB = 1) (read/write) */
#define FFFOR		__REG(0x40100024)  /* Receive FIFO Ocuppancy (read/write) */
#define FFABR		__REG(0x40100028)  /* Auto_Baud Control (read/write) */
#define FFACR		__REG(0x4010002C)  /* Auto_Baud Count (read/write) */

/* Bluetooth UART (BTUART) */
#define BTUART		BTRBR
#define BTRBR		__REG(0x40200000)  /* Receive Buffer Register (read only) */
#define BTTHR		__REG(0x40200000)  /* Transmit Holding Register (write only) */
#define BTIER		__REG(0x40200004)  /* Interrupt Enable Register (read/write) */
#define BTIIR		__REG(0x40200008)  /* Interrupt ID Register (read only) */
#define BTFCR		__REG(0x40200008)  /* FIFO Control Register (write only) */
#define BTLCR		__REG(0x4020000C)  /* Line Control Register (read/write) */
#define BTMCR		__REG(0x40200010)  /* Modem Control Register (read/write) */
#define BTLSR		__REG(0x40200014)  /* Line Status Register (read only) */
#define BTMSR		__REG(0x40200018)  /* Modem Status Register (read only) */
#define BTSPR		__REG(0x4020001C)  /* Scratch Pad Register (read/write) */
#define BTISR		__REG(0x40200020)  /* Infrared Selection Register (read/write) */
#define BTDLL		__REG(0x40200000)  /* Divisor Latch Low Register (DLAB = 1) (read/write) */
#define BTDLH		__REG(0x40200004)  /* Divisor Latch High Register (DLAB = 1) (read/write) */
#define BTFOR		__REG(0x40200024)  /* Receive FIFO Ocuppancy (read/write) */
#define BTABR		__REG(0x40200028)  /* Auto_Baud Control (read/write) */
#define BTACR		__REG(0x4020002C)  /* Auto_Baud Count (read/write) */

/* Standard UART (STUART) */
#define STUART		STRBR
#define STRBR		__REG(0x40700000)  /* Receive Buffer Register (read only) */
#define STTHR		__REG(0x40700000)  /* Transmit Holding Register (write only) */
#define STIER		__REG(0x40700004)  /* Interrupt Enable Register (read/write) */
#define STIIR		__REG(0x40700008)  /* Interrupt ID Register (read only) */
#define STFCR		__REG(0x40700008)  /* FIFO Control Register (write only) */
#define STLCR		__REG(0x4070000C)  /* Line Control Register (read/write) */
#define STMCR		__REG(0x40700010)  /* Modem Control Register (read/write) */
#define STLSR		__REG(0x40700014)  /* Line Status Register (read only) */
#define STMSR		__REG(0x40700018)  /* Reserved */
#define STSPR		__REG(0x4070001C)  /* Scratch Pad Register (read/write) */
#define STISR		__REG(0x40700020)  /* Infrared Selection Register (read/write) */
#define STDLL		__REG(0x40700000)  /* Divisor Latch Low Register (DLAB = 1) (read/write) */
#define STDLH		__REG(0x40700004)  /* Divisor Latch High Register (DLAB = 1) (read/write) */
#define STFOR		__REG(0x40700024)  /* Receive FIFO Ocuppancy (read/write) */
#define STABR		__REG(0x40700028)  /* Auto_Baud Control (read/write) */
#define STACR		__REG(0x4070002C)  /* Auto_Baud Count (read/write) */

#define IER_DMAE	(1 << 7)	/* DMA Requests Enable */
#define IER_UUE		(1 << 6)	/* UART Unit Enable */
#define IER_NRZE	(1 << 5)	/* NRZ coding Enable */
#define IER_RTIOE	(1 << 4)	/* Receiver Time Out Interrupt Enable */
#define IER_MIE		(1 << 3)	/* Modem Interrupt Enable */
#define IER_RLSE	(1 << 2)	/* Receiver Line Status Interrupt Enable */
#define IER_TIE		(1 << 1)	/* Transmit Data request Interrupt Enable */
#define IER_RAVIE	(1 << 0)	/* Receiver Data Available Interrupt Enable */

#define IIR_FIFOES	(3 << 6)	/* FIFO Mode Enable Status */
#define IIR_EOC		(1 << 3)	/* DMA End of discrptor chain */
#define IIR_ABL		(1 << 3)	/* Auto baud lock */
#define IIR_TOD		(1 << 3)	/* Time Out Detected */
#define IIR_IID		0x06		/* Interrupt Source Encoded (mask) */
#define IIR_IP		(1 << 0)	/* Interrupt Pending (active low) */

#define FCR_ITL2	(1 << 7)	/* Interrupt Trigger Level */
#define FCR_ITL1	(1 << 6)	/* Interrupt Trigger Level */
#define FCR_BUS		(1 << 5)	/* 32-bit Peripheral BUS */
#define FCR_TRAIL	(1 << 4)	/* Trailing Bytes */
#define FCR_TIL		(1 << 4)	/* Transmit Interrupt Level */
#define FCR_RESETTF	(1 << 2)	/* Reset Transmitter FIFO */
#define FCR_RESETRF	(1 << 1)	/* Reset Receiver FIFO */
#define FCR_TRFIFOE	(1 << 0)	/* Transmit and Receive FIFO Enable */
#define FCR_ITL_1	(0)
#define FCR_ITL_8	(FCR_ITL1)
#define FCR_ITL_16	(FCR_ITL2)
#define FCR_ITL_32	(FCR_ITL2|FCR_ITL1)

#define FOR_BCNT	0x3f		/* byte count in receive FIFO (mask) */

#define LCR_DLAB	(1 << 7)	/* Divisor Latch Access Bit */
#define LCR_SB		(1 << 6)	/* Set Break */
#define LCR_STKYP	(1 << 5)	/* Sticky Parity */
#define LCR_EPS		(1 << 4)	/* Even Parity Select */
#define LCR_PEN		(1 << 3)	/* Parity Enable */
#define LCR_STB		(1 << 2)	/* Stop Bit */
#define LCR_WLS1	(1 << 1)	/* Word Length Select */
#define LCR_WLS0	(1 << 0)	/* Word Length Select */

#define LSR_FIFOE	(1 << 7)	/* FIFO Error Status */
#define LSR_TEMT	(1 << 6)	/* Transmitter Empty */
#define LSR_TDRQ	(1 << 5)	/* Transmit Data Request */
#define LSR_BI		(1 << 4)	/* Break Interrupt */
#define LSR_FE		(1 << 3)	/* Framing Error */
#define LSR_PE		(1 << 2)	/* Parity Error */
#define LSR_OE		(1 << 1)	/* Overrun Error */
#define LSR_DR		(1 << 0)	/* Data Ready */

#define BULV_MCR_AFE	(1 << 5)	/* Auto Flow Control Enable */
#define BULV_MCR_LOOP	(1 << 4)	/**/ 
#define BULV_MCR_OUT2	(1 << 3)	/* force MSR_DCD in loopback mode */
#define BULV_MCR_OUT1	(1 << 2)	/* force MSR_RI in loopback mode */
#define BULV_MCR_RTS	(1 << 1)	/* Request to Send */
#define BULV_MCR_DTR	(1 << 0)	/* Data Terminal Ready */

#define BULV_MSR_DCD	(1 << 7)	/* Data Carrier Detect */
#define BULV_MSR_RI		(1 << 6)	/* Ring Indicator */
#define BULV_MSR_DSR	(1 << 5)	/* Data Set Ready */
#define BULV_MSR_CTS	(1 << 4)	/* Clear To Send */
#define BULV_MSR_DDCD	(1 << 3)	/* Delta Data Carrier Detect */
#define BULV_MSR_TERI	(1 << 2)	/* Trailing Edge Ring Indicator */
#define BULV_MSR_DDSR	(1 << 1)	/* Delta Data Set Ready */
#define BULV_MSR_DCTS	(1 << 0)	/* Delta Clear To Send */

#define STISR_RXPL      (1 << 4)    /* Receive Data Polarity */
#define STISR_TXPL      (1 << 3)    /* Transmit Data Polarity */
#define STISR_XMODE     (1 << 2)    /* Transmit Pulse Width Select */
#define STISR_RCVEIR    (1 << 1)    /* Receiver SIR Enable */
#define STISR_XMITIR    (1 << 0)    /* Transmitter SIR Enable */

#define ABR_ABT			(1 << 3)	/* Baud Rate Calculation(formular/table) */
#define ABR_ABUP		(1 << 2)	/* Divisor Latch(Processor/UART) */
#define ABR_ABLIE		(1 << 1)	/* Auto-baud-lock interrupt enable */
#define ABR_ABE			(1 << 0)	/* Auto baud enable */

#define ACR_CNT			0xff		/* auto baud count value of (mask) */

/*
 * LCD Controller
 */

#define LCCR0		__REG(0x44000000)  /* LCD Controller Control Register 0 */
#define LCCR1		__REG(0x44000004)  /* LCD Controller Control Register 1 */
#define LCCR2		__REG(0x44000008)  /* LCD Controller Control Register 2 */
#define LCCR3		__REG(0x4400000C)  /* LCD Controller Control Register 3 */
#define LCCR4		__REG(0x44000010)  /* LCD Controller Control Register 4 */
#define LCCR5		__REG(0x44000014)  /* LCD Controller Control Register 5 */
#define FBR0		__REG(0x44000020)  /* DMA Channel 0 Frame Branch Register */
#define FBR1		__REG(0x44000024)  /* DMA Channel 1 Frame Branch Register */
#define FBR2		__REG(0x44000028)  /* DMA Channel 2 Frame Branch Register */
#define FBR3		__REG(0x4400002C)  /* DMA Channel 3 Frame Branch Register */
#define FBR4		__REG(0x44000030)  /* DMA Channel 4 Frame Branch Register */
#define LCSR1		__REG(0x44000034)  /* LCD Controller Status Register 1 */
#define LCSR0		__REG(0x44000038)  /* LCD Controller Status Register 0 */
#define LIIDR		__REG(0x4400003C)  /* LCD Controller Interrupt ID Register */
#define TRGBR		__REG(0x44000040)  /* TMED RGB Seed Register */
#define TCR			__REG(0x44000044)  /* TMED Control Register */

#define OVL1C1		__REG(0x44000050)  /* Overlay 1 Control Register 1 */
#define OVL1C2		__REG(0x44000060)  /* Overlay 1 Control Register 2 */
#define OVL2C1		__REG(0x44000070)  /* Overlay 2 Control Register 1 */
#define OVL2C2		__REG(0x44000080)  /* Overlay 2 Control Register 2 */

#define CCR			__REG(0x44000090)  /* Cursor Control Register */

#define CMDCR		__REG(0x44000100)  /* Command Control Register */
#define PRSR		__REG(0x44000104)  /* Panel Read Status Register */

#define FBR5		__REG(0x44000110)  /* DMA Channel 5 Frame Branch Register */
#define FBR6		__REG(0x44000114)  /* DMA Channel 6 Frame Branch Register */

#define FDADR(x)	__REG2(0x44000200, (x) << 4)  /* DMA Channel x Frame Descriptor Address Register */
#define FSADR(x)	__REG2(0x44000204, (x) << 4)  /* DMA Channel x Frame Source Address Register */
#define FIDR(x)		__REG2(0x44000208, (x) << 4)  /* DMA Channel x Frame ID Register */
#define LDCMD(x)	__REG2(0x4400020C, (x) << 4)  /* DMA Channel x Command Register */

#define LCDBSCNTR	__REG(0x48000054)  /* LCD Buffer Strength Control Register */

/* LCD Control Register 0 Bits */
#define LCCR0_LDDALT	(1 << 26)
#define LCCR0_OUC		(1 << 25)
#define LCCR0_CMDIM		(1 << 24)
#define LCCR0_RDSTM		(1 << 23)
#define LCCR0_LCDT		(1 << 22)
#define LCCR0_OUM		(1 << 21)
#define LCCR0_BSM0		(1 << 20)
#define LCCR0_PDD_MASK	0x000FF000
#define LCCR0_PDD_SHIFT	12
#define LCCR0_QDM		(1 << 11)
#define LCCR0_DIS		(1 << 10)
#define LCCR0_DPD		(1 << 9)
#define LCCR0_PAS		(1 << 7)
#define LCCR0_EOFM0		(1 << 6)
#define LCCR0_IUM		(1 << 5)
#define LCCR0_SOFM0		(1 << 4)
#define LCCR0_LDM		(1 << 3)
#define LCCR0_SDS		(1 << 2)
#define LCCR0_CMS		(1 << 1)
#define LCCR0_ENB		(1 << 0)


/* LCD control Register 1 Bits */
#define LCCR1_PPL_MASK 	0x000003FF
#define LCCR1_HSW_MASK 	0x0000FC00
#define LCCR1_ELW_MASK 	0x00FF0000
#define LCCR1_BLW_MASK	0xFF000000
#define LCCR1_PPL_SHIFT 0
#define LCCR1_HSW_SHIFT 10
#define LCCR1_ELW_SHIFT 16
#define LCCR1_BLW_SHIFT 24

/* LCD control Register 2 Bits */
#define LCCR2_LPP_MASK 	0x000003FF
#define LCCR2_VSW_MASK 	0x0000FC00
#define LCCR2_EFW_MASK 	0x00FF0000
#define LCCR2_BFW_MASK	0xFF000000
#define LCCR2_LPP_SHIFT 0
#define LCCR2_VSW_SHIFT 10
#define LCCR2_EFW_SHIFT 16
#define LCCR2_BFW_SHIFT 24

/* LCD control Register 3 Bits */
#define LCCR3_PCD_MASK 0x000000FF
#define LCCR3_ACB_MASK 0x0000FF00
#define LCCR3_API_MASK 0x000F0000
#define LCCR3_PCD_SHIFT 0
#define LCCR3_ACB_SHIFT 8
#define LCCR3_API_SHIFT 16

#define LCCR3_VSP 		(1 << 20)
#define LCCR3_HSP 		(1 << 21)
#define LCCR3_PCP 		(1 << 22)
#define LCCR3_OEP 		(1 << 23)
#define LCCR3_BPP_MASK 	0x07000000
#define LCCR3_BPP_SHIFT	24
#define LCCR3_DPC 		(1 << 27)
#define LCCR3_BPP3 		(1 << 29)
#define LCCR3_PDFOR_MASK	0xC0000000
#define LCCR3_PDFOR_SHIFT	30

/* LCD control Register 4 Bits */
#define LCCR4_PALFOR_MASK 	0x018000
#define LCCR4_PALFOR_SHIFT 	15
#define LCCR4_K3_MASK 	(7 << 6)
#define LCCR4_K3_SHIFT 	6
#define LCCR4_K2_MASK 	(7 << 3)
#define LCCR4_K2_SHIFT 	3
#define LCCR4_K1_MASK 	0x07
#define LCCR4_K1_SHIFT 	0

/* LCD control Register 5 Bits */
#define LCCR5_IUM	(1 << 24)
#define LCCR5_BSM	(1 << 16)
#define LCCR5_EOFM	(1 << 8)
#define LCCR5_SOFM	(1 << 0)

/* LCD Status Register 0 */
#define LCSR0_BERCH_MASK	0x70000000
#define LCSR0_BERCH_SHIFT	28
#define LCSR0_SINT 	(1 << 10)
#define LCSR0_BS0   (1 << 9)
#define LCSR0_EOF0  (1 << 8)
#define LCSR0_QD   	(1 << 7)
#define LCSR0_OU   	(1 << 6)
#define LCSR0_IU1  	(1 << 5)
#define LCSR0_IU0  	(1 << 4)
#define LCSR0_ABC  	(1 << 3)
#define LCSR0_BER  	(1 << 2)
#define LCSR0_SOF1 	(1 << 1)
#define LCSR0_LDD  	(1 << 0)


/*
 * Pulse Width Modulator
 */

#define PWMCR0		__REG(0x40B00000)  /* PWM 0 Control Register */
#define PWMDCR0		__REG(0x40B00004)  /* PWM 0 Duty Cycle Register */
#define PWMPCR0		__REG(0x40B00008)  /* PWM 0 Period Control Register */

#define PWMCR1		__REG(0x40C00000)  /* PWM 1 Control Register */
#define PWMDCR1		__REG(0x40C00004)  /* PWM 1 Duty Cycle Register */
#define PWMPCR1		__REG(0x40C00008)  /* PWM 1 Period Control Register */

#define PWMCR2		__REG(0x40B00010)  /* PWM 2 Control Register */
#define PWMDCR2		__REG(0x40B00014)  /* PWM 2 Duty Cycle Register */
#define PWMPCR2		__REG(0x40B00018)  /* PWM 2 Period Control Register */

#define PWMCR3		__REG(0x40C00010)  /* PWM 3 Control Register */
#define PWMDCR3		__REG(0x40C00014)  /* PWM 3 Duty Cycle Register */
#define PWMPCR3		__REG(0x40C00018)  /* PWM 3 Period Control Register */

/* PWM bits */
#define PWMCR_SD			(1 << 6)	/* PWM shutdown mode */
#define PWMCR_PRESCALE_MASK	0x3f

#define PWMDCR_FD			(1 << 10)	/* PWM Full Duty Cycle */
#define PWMDCR_DCYCLE_MASK	0x3ff

#define PWMPCR_PV_MASK		0x3ff

/*
 * MultiMediaCard (MMC) controller
 */

#define MMC_STRPCL	__REG(0x41100000)  /* Control to start and stop MMC clock */
#define MMC_STAT	__REG(0x41100004)  /* MMC Status Register (read only) */
#define MMC_CLKRT	__REG(0x41100008)  /* MMC clock rate */
#define MMC_SPI		__REG(0x4110000c)  /* SPI mode control bits */
#define MMC_CMDAT	__REG(0x41100010)  /* Command/response/data sequence control */
#define MMC_RESTO	__REG(0x41100014)  /* Expected response time out */
#define MMC_RDTO	__REG(0x41100018)  /* Expected data read time out */
#define MMC_BLKLEN	__REG(0x4110001c)  /* Block length of data transaction */
#define MMC_NOB		__REG(0x41100020)  /* Number of blocks, for block mode */
#define MMC_PRTBUF	__REG(0x41100024)  /* Partial MMC_TXFIFO FIFO written */
#define MMC_I_MASK	__REG(0x41100028)  /* Interrupt Mask */
#define MMC_I_REG	__REG(0x4110002c)  /* Interrupt Register (read only) */
#define MMC_CMD		__REG(0x41100030)  /* Index of current command */
#define MMC_ARGH	__REG(0x41100034)  /* MSW part of the current command argument */
#define MMC_ARGL	__REG(0x41100038)  /* LSW part of the current command argument */
#define MMC_RES		__REG(0x4110003c)  /* Response FIFO (read only) */
#define MMC_RXFIFO	__REG(0x41100040)  /* Receive FIFO (read only) */
#define MMC_TXFIFO	__REG(0x41100044)  /* Transmit FIFO (write only) */
#define MMC_RDWAIT		__REG(0x41100048)  /* SDIO read wait */
#define MMC_BLKS_REM	__REG(0x4110004C)  /* blocks not transferred */


/*
 * I2C registers
 */

#define IBMR		__REG(0x40301680)  /* I2C Bus Monitor Register - IBMR */
#define IDBR		__REG(0x40301688)  /* I2C Data Buffer Register - IDBR */
#define ICR			__REG(0x40301690)  /* I2C Control Register - ICR */
#define ISR			__REG(0x40301698)  /* I2C Status Register - ISR */
#define ISAR		__REG(0x403016A0)  /* I2C Slave Address Register - ISAR */

#define P_IBMR		__REG(0x40F00180)  /* I2C Bus Monitor Register - IBMR */
#define P_IDBR		__REG(0x40F00188)  /* I2C Data Buffer Register - IDBR */
#define P_ICR			__REG(0x40F00190)  /* I2C Control Register - ICR */
#define P_ISR		__REG(0x40F00198)  /* I2C Status Register - ISR */
#define P_ISAR		__REG(0x40F001A0)  /* I2C Slave Address Register - ISAR */

#define ICR_START	(1 << 0)	   /* start bit */
#define ICR_STOP	(1 << 1)	   /* stop bit */
#define ICR_ACKNAK	(1 << 2)	   /* send ACK(0) or NAK(1) */
#define ICR_TB		(1 << 3)	   /* transfer byte bit */
#define ICR_MA		(1 << 4)	   /* master abort */
#define ICR_SCLE	(1 << 5)	   /* master clock enable */
#define ICR_IUE		(1 << 6)	   /* unit enable */
#define ICR_GCD		(1 << 7)	   /* general call disable */
#define ICR_ITEIE	(1 << 8)	   /* enable tx interrupts */
#define ICR_IRFIE	(1 << 9)	   /* enable rx interrupts */
#define ICR_BEIE	(1 << 10)	   /* enable bus error ints */
#define ICR_SSDIE	(1 << 11)	   /* slave STOP detected int enable */
#define ICR_ALDIE	(1 << 12)	   /* enable arbitration interrupt */
#define ICR_SADIE	(1 << 13)	   /* slave address detected int enable */
#define ICR_UR		(1 << 14)	   /* unit reset */

#define ISR_RWM		(1 << 0)	   /* read/write mode */
#define ISR_ACKNAK	(1 << 1)	   /* ack/nak status */
#define ISR_UB		(1 << 2)	   /* unit busy */
#define ISR_IBB		(1 << 3)	   /* bus busy */	
#define ISR_SSD		(1 << 4)	   /* slave stop detected */
#define ISR_ALD		(1 << 5)	   /* arbitration loss detected */
#define ISR_ITE		(1 << 6)	   /* tx buffer empty */
#define ISR_IRF		(1 << 7)	   /* rx buffer full */
#define ISR_GCAD	(1 << 8)	   /* general call address detected */
#define ISR_SAD		(1 << 9)	   /* slave address detected */
#define ISR_BED		(1 << 10)	   /* bus error no ACK/NAK */


/*
 * Serial Audio Controller
 */

/* FIXME: This clash with SA1111 defines */
#ifndef CONFIG_SA1111
#define SACR0		__REG(0x40400000)  /* Global Control Register */
#define SACR1		__REG(0x40400004)  /* Serial Audio I 2 S/MSB-Justified Control Register */
#define SASR0		__REG(0x4040000C)  /* Serial Audio I 2 S/MSB-Justified Interface and FIFO Status Register */
#define SAIMR		__REG(0x40400014)  /* Serial Audio Interrupt Mask Register */
#define SAICR		__REG(0x40400018)  /* Serial Audio Interrupt Clear Register */
#define SADIV		__REG(0x40400060)  /* Audio Clock Divider Register. */
#define SADR		__REG(0x40400080)  /* Serial Audio Data Register (TX and RX FIFO access Register). */
#endif


/*
 * AC97 Controller registers
 */
#define GCR		__REG(0x4050000C)  /* Global Control Register */
#define GCR_DMAEN		(1 << 24)	/* DMA Enable */
#define GCR_CDONE_IE	(1 << 19)	/* Command Done Interrupt Enable */
#define GCR_SDONE_IE	(1 << 18)	/* Status Done Interrupt Enable */
#define GCR_SRDY_IE		(1 << 9)	/* Secondary Ready Interrupt Enable */
#define GCR_PRDY_IE		(1 << 8)	/* Primary Ready Interrupt Enable */
#define GCR_SRES_IE		(1 << 5)	/* Secondary Resume Interrupt Enable */
#define GCR_PRES_IE		(1 << 4)	/* Primary Resume Interrupt Enable */
#define GCR_ACOFF		(1 << 3)	/* AC-link Shut Off */
#define GCR_WRST		(1 << 2)	/* AC97 Warm Reset */
#define GCR_CRST		(1 << 1)	/* AC'97 Cold Reset (0 = active) */
#define GCR_GPI_IE			(1 << 0)	/* Codec GPI Interrupt Enable */

#define GSR		__REG(0x4050001C)  /* Global Status Register */
#define GSR_CDONE		(1 << 19)	/* Command Done */
#define GSR_SDONE		(1 << 18)	/* Status Done */
#define GSR_RCS			(1 << 15)	/* Read Completion Status */
#define GSR_B3S12		(1 << 14)	/* Bit 3 of slot 12 */
#define GSR_B2S12		(1 << 13)	/* Bit 2 of slot 12 */
#define GSR_B1S12		(1 << 12)	/* Bit 1 of slot 12 */
#define GSR_SRESINT		(1 << 11)	/* Secondary Resume Interrupt */
#define GSR_PRESINT		(1 << 10)	/* Primary Resume Interrupt */
#define GSR_SCRDY		(1 << 9)	/* Secondary Codec Ready */
#define GSR_PCRDY		(1 << 8)	/*  Primary Codec Ready */
#define GSR_MCINT		(1 << 7)	/* Mic In Interrupt */
#define GSR_POINT		(1 << 6)	/* PCM Out Interrupt */
#define GSR_PIINT		(1 << 5)	/* PCM In Interrupt */
#define GSR_ACOFFD		(1 << 3)	/* AC-Link Shut Off Done */
#define GSR_MOINT		(1 << 2)	/* Modem Out Interrupt */
#define GSR_MIINT		(1 << 1)	/* Modem In Interrupt */
#define GSR_GSCI		(1 << 0)	/* Codec GPI Status Change Interrupt */

#define POCR		__REG(0x40500000)  /* PCM Out Control Register */
#define POCR_FEIE	(1 << 3)	/* FIFO Error Interrupt Enable */
#define POCR_FSRIE	(1 << 1)	/* FIFO Service Request Interrupt Enable */

#define PICR		__REG(0x40500004)  /* PCM In Control Register */
#define PICR_FEIE		(1 << 3)	/* FIFO Error Interrupt Enable */
#define PICR_FSRIE	(1 << 1)	/* FIFO Service Request Interrupt Enable */

#define POSR		__REG(0x40500010)  /* PCM Out Status Register */
#define POSR_FIFOE	(1 << 4)	/* FIFO error */
#define POSR_FSR	(1 << 2)	/* FIFO Service Request */

#define PISR		__REG(0x40500014)  /* PCM In Status Register */
#define PISR_FIFOE	(1 << 4)	/* FIFO error */
#define PISR_EOC		(1 << 3)	/* DMA End of Chain Interrupt */
#define PISR_FSR		(1 << 2)	/* FIFO Service Request*/

#define CAR		__REG(0x40500020)  /* CODEC Access Register */
#define CAR_CAIP	(1 << 0)	/* Codec Access In Progress */

#define PCDR		__REG(0x40500040)  /* PCM FIFO Data Register */

#define MCCR		__REG(0x40500008)  /* Mic In Control Register */
#define MCCR_FEIE	(1 << 3)	/* FIFO Error Interrupt Enable */
#define MCCR_FSRIE	(1 << 1)	/* FIFO Service Request Interrupt Enable */

#define MCSR		__REG(0x40500018)  /* Mic In Status Register */
#define MCSR_FIFOE		(1 << 4)	/* FIFO error */
#define MCSR_EOC		(1 << 3)	/* DMA End of Chain Interrupt */
#define MCSR_FSR		(1 << 2)	/* FIFO Service Request*/

#define MCDR		__REG(0x40500060)  /* Mic-in FIFO Data Register */

#define MOCR		__REG(0x40500100)  /* Modem Out Control Register */
#define MOCR_FEIE	(1 << 3)	/* FIFO Error */
#define MOCR_FSRIE	(1 << 1)	/* FIFO Service Request Interrupt Enable */

#define MICR		__REG(0x40500108)  /* Modem In Control Register */
#define MICR_FEIE	(1 << 3)	/* FIFO Error */
#define MICR_FSR	(1 << 3)	/* FIFO Service Request Interrupt Enable */

#define MOSR		__REG(0x40500110)  /* Modem Out Status Register */
#define MOSR_FIFOE	(1 << 4)	/* FIFO error */
#define MOSR_FSR		(1 << 2)	/* FIFO Service Request*/

#define MISR		__REG(0x40500118)  /* Modem In Status Register */
#define MISR_FIFOE		(1 << 4)	/* FIFO error */
#define MISR_EOC		(1 << 3)	/* DMA End of Chain Interrupt */
#define MISR_FSR		(1 << 2)	/* FIFO Service Request*/

#define MODR		__REG(0x40500140)  /* Modem FIFO Data Register */

#define PAC_REG_BASE	__REG(0x40500200)  /* Primary Audio Codec */
#define SAC_REG_BASE	__REG(0x40500300)  /* Secondary Audio Codec */
#define PMC_REG_BASE	__REG(0x40500400)  /* Primary Modem Codec */
#define SMC_REG_BASE	__REG(0x40500500)  /* Secondary Modem Codec */


/*
 * USB Device Controller
 */

#define UDCCR		__REG(0x40600000)  /* UDC Control Register */
#define UDCCR_OEN			(1 << 31)	/* On-The-Go enable */
#define UDCCR_AALTHNP		(1 << 30)	/* A-Device Alternate Host Negotiation Protocol Port support */
#define UDCCR_AHNP			(1 << 29)	/* A-Device Host Negotiation Protocol support */
#define UDCCR_BHNP			(1 << 28)	/* B-Device Host Negotiation Protocol Enable */
#define UDCCR_DWRE			(1 << 16)	/* Device Remote Wakeup Feature Enable */
#define UDCCR_AIN_MASK		(0x03 << 11)	/* active UDC UDC Configuration Number */
#define UDCCR_AIN_SHIFT		11			/* active UDC Configuration Number */
#define UDCCR_AAISN_MASK	(0x07 << 5)	/* active UDC Alternate Interface Setting Number */
#define UDCCR_AAISN_SHIFT	5			/* active UDC Alternate Interface Setting Number */
#define UDCCR_SMAC			(1 << 4)	/* Switch Endpoint Memory to Active Configuration */
#define UDCCR_EMCE			(1 << 3)	/* Endpoint Memory Configuration Error */
#define UDCCR_UDR			(1 << 2)	/* Device resume */
#define UDCCR_UDA			(1 << 1)	/* UDC active */
#define UDCCR_UDE			(1 << 0)	/* UDC enable */

#define UDCICR0		__REG(0x40600004)  /* UDC Interrupt Control Register 0 */

#define UDCICR1		__REG(0x40600008)  /* UDC Interrupt Control Register 1 */
#define UDCICR1_IECC	(1 << 31)	/*Interrupt Enable - Configuration Change */
#define UDCICR1_IESOF	(1 << 30)	/*Interrupt Enable - Start of frame */
#define UDCICR1_IERU	(1 << 29)	/*Interrupt Enable - Resume */
#define UDCICR1_IESU	(1 << 28)	/*Interrupt Enable - Suspend */
#define UDCICR1_IERS	(1 << 27)	/*Interrupt Enable - Reset */

#define UDCOTGICR	__REG(0x40600018)  /* UDC On-The-Go Interrupt Control Register */
#define UDCOTGICR_IESF		(1 << 24)	/* OTG SET_FEATURE command received */
#define UDCOTGICR_IEXR		(1 << 17)	/* External Transceiver Interrupt Rising Edge Interrupt Enable */
#define UDCOTGICR_IEXF		(1 << 16)	/* External Transceiver Interrupt Falling Edge Interrupt Enable */
#define UDCOTGICR_IEVV40R	(1 << 9)	/* OTG Vbus Valid 4.0v Rising Edge Interrupt Enable */
#define UDCOTGICR_IEVV40F	(1 << 8)	/* OTG Vbus Valid 4.0v Falling Edge Interrupt Enable */
#define UDCOTGICR_IEVV44R	(1 << 7)	/* OTG Vbus Valid 4.4v Rising Edge Interrupt Enable */
#define UDCOTGICR_IEVV44F	(1 << 6)	/* OTG Vbus Valid 4.4v Falling Edge Interrupt Enable */
#define UDCOTGICR_IESVR		(1 << 5)	/* OTG Session Valid Rising Edge Interrupt Enable */
#define UDCOTGICR_IESVF		(1 << 4)	/* OTG Session Valid Falling Edge Interrupt Enable */
#define UDCOTGICR_IESDR		(1 << 3)	/* OTG A-Device SRP Detect Rising Edge Interrupt Enable */
#define UDCOTGICR_IESDF		(1 << 2)	/* OTG A-Device SRP Detect Falling Edge Interrupt Enable */
#define UDCOTGICR_IEIDR		(1 << 1)	/* OTG ID Change Rising Edge Interrupt Enable */
#define UDCOTGICR_IEIDF		(1 << 0)	/* OTG ID Change Falling Edge Interrupt Enable */

#define UP2OCR		__REG(0x40600020)  /* USB Port 2 Output Control Register */
#define UP2OCR_SEOS_MASK	(0x7 << 24)  /* Single-Ended Output Select(mask)*/
#define UP2OCR_SEOS_SHIFT	(24)  		/* Single-Ended Output Select(shift)*/
#define UP2OCR_HXOE			(1 << 17)  /* Host port 2 Transceiver Output Enable */
#define UP2OCR_HXS			(1 << 16)  /* Host port 2 Transceiver Output Select */
#define UP2OCR_IDON			(1 << 10)  /* OTG ID Read Enable */
#define UP2OCR_EXSUS		(1 << 9)  /* External Transceiver Suspend Enable */
#define UP2OCR_EXSP			(1 << 8)  /* External Transceiver Speed Control */
#define UP2OCR_DMPUBE		(1 << 7)  /* Host port 2 Transceiver D- Pull Up Bypass Enable */
#define UP2OCR_DPPUBE		(1 << 6)  /* Host port 2 Transceiver D+ Pull Up Bypass Enable */
#define UP2OCR_DMPUE		(1 << 5)  /* Host port 2 Transceiver D- Pull Up Enable */
#define UP2OCR_DPPUE		(1 << 4)  /* Host port 2 Transceiver D+ Pull Up Enable */
#define UP2OCR_DMPDE		(1 << 3)  /* Host port 2 Transceiver D- Pull Down Enable */
#define UP2OCR_DPPDE		(1 << 2)  /* Host port 2 Transceiver D+ Pull Down Enable */
#define UP2OCR_CPVPE		(1 << 1)  /* Charge Pump Vbus Pulse Enable */
#define UP2OCR_CPVEN		(1 << 0)  /* Charge Pump Vbus Enable */

#define UP3OCR		__REG(0x40600024)  /* USB Port 3 Output Control Register */
#define UP2OCR_CFG_H		(0 << 0)  /* external Host */
#define UP2OCR_CFG_D		(2 << 0)  /* external Device */

#define UDCISR0		__REG(0x4060000C)  /* UDC Interrupt status Register 0 */

#define UDCISR1		__REG(0x40600010)  /* UDC Interrupt Status Register 1 */
#define UDCISR1_IRCC	(1 << 31)	/*Interrupt Enable - Configuration Change */
#define UDCISR1_IRSOF	(1 << 30)	/*Interrupt Enable - Start of frame */
#define UDCISR1_IRRU	(1 << 29)	/*Interrupt Enable - Resume */
#define UDCISR1_IRSU	(1 << 28)	/*Interrupt Enable - Suspend */
#define UDCISR1_IRRS	(1 << 27)	/*Interrupt Enable - Reset */

#define UDCOTGISR	__REG(0x4060001C)  /* UDC On-The-Go Interrupt Control Register */
#define UDCOTGISR_IRSF		(1 << 24)	/* OTG SET_FEATURE command received */
#define UDCOTGISR_IRXR		(1 << 17)	/* External Transceiver Interrupt Rising Edge Interrupt Enable */
#define UDCOTGISR_IRXF		(1 << 16)	/* External Transceiver Interrupt Falling Edge Interrupt Enable */
#define UDCOTGISR_IRVV40R	(1 << 9)	/* OTG Vbus Valid 4.0v Rising Edge Interrupt Enable */
#define UDCOTGISR_IRVV40F	(1 << 8)	/* OTG Vbus Valid 4.0v Falling Edge Interrupt Enable */
#define UDCOTGISR_IRVV44R	(1 << 7)	/* OTG Vbus Valid 4.4v Rising Edge Interrupt Enable */
#define UDCOTGISR_IRVV44F	(1 << 6)	/* OTG Vbus Valid 4.4v Falling Edge Interrupt Enable */
#define UDCOTGISR_IRSVR		(1 << 5)	/* OTG Session Valid Rising Edge Interrupt Enable */
#define UDCOTGISR_IRSVF		(1 << 4)	/* OTG Session Valid Falling Edge Interrupt Enable */
#define UDCOTGISR_IRSDR		(1 << 3)	/* OTG A-Device SRP Detect Rising Edge Interrupt Enable */
#define UDCOTGISR_IRSDF		(1 << 2)	/* OTG A-Device SRP Detect Falling Edge Interrupt Enable */
#define UDCOTGISR_IRIDR		(1 << 1)	/* OTG ID Change Rising Edge Interrupt Enable */
#define UDCOTGISR_IRIDF		(1 << 0)	/* OTG ID Change Falling Edge Interrupt Enable */

#define UDCFNR		__REG(0x40600014)  /* UDC Frame Number Register */
#define UDCFNR_MASK	0x7ff

#define UDCCSR0		__REG(0x40600100)  /* UDC Endpoint 0 Control/Status Register */
#define UDCCSR0_ACM		(1 << 9)	/* ACK Control Mode */
#define UDCCSR0_AREN	(1 << 8)	/* ACK Response Enable */
#define UDCCSR0_SA		(1 << 7)	/* Setup active */
#define UDCCSR0_RNE		(1 << 6)	/* Receive FIFO no empty */
#define UDCCSR0_FST		(1 << 5)	/* Force stall */
#define UDCCSR0_SST		(1 << 4)	/* Sent stall */
#define UDCCSR0_DME		(1 << 3)	/* DMA Enable */
#define UDCCSR0_FTF		(1 << 2)	/* Flush Tx FIFO */
#define UDCCSR0_IPR		(1 << 1)	/* IN packet ready */
#define UDCCSR0_OPC		(1 << 0)	/* OUT packet Complete */

#define UDCCSR(x)		__REG2(0x40600100, (x) << 2)	/* UDC Endpoints x Control Status Register*/
#define UDCCSR_DPE		(1 << 9)	/* Data Packet Error */
#define UDCCSR_FEF		(1 << 8)	/* Flush Endpoint FIFO */
#define UDCCSR_SP		(1 << 7)	/* Short Packet Control/Status */
#define UDCCSR_BNEF		(1 << 6)	/* Buffer Not Empty/Full */
#define UDCCSR_FST		(1 << 5)	/* Force stall */
#define UDCCSR_SST		(1 << 4)	/* Sent stall */
#define UDCCSR_DME		(1 << 3)	/* DMA Enable */
#define UDCCSR_TRN		(1 << 2)	/* Tx/Rx NAK */
#define UDCCSR_PC		(1 << 1)	/* packet Complete */
#define UDCCSR_FS		1		/* FIFO Needs Service */

#define UDCBCR(x)		__REG2(0x40600200, (x) << 2)	/* UDC Endpoints x Byte Count Register*/
#define UDCBCR_MASK		0x3ff

#define UDCDR(x)		__REG2(0x40600300, (x) << 2)	/* UDC Endpoints x Data Register*/

#define UDCECR(x)		__REG2(0x40600400, (x) << 2)	/* UDC Endpoints x Control Register*/
#define UDCECR_CN_MASK		(0x3 << 25) /* Configuration Number */
#define UDCECR_CN_SHIFT		25 
#define UDCECR_IN_MASK		(0x7 << 22) /* Intertface Number */
#define UDCECR_IN_SHIFT		22 
#define UDCECR_AISN_MASK		(0x7 << 19) /* Alternate Interface Number */
#define UDCECR_AISN_SHIFT	19 
#define UDCECR_EN_MASK		(0xf << 15) /* Endpoint Number */
#define UDCECR_EN_SHIFT		15 
#define UDCECR_ET_MASK		(0x3 << 13) /* Endpoint Number */
#define UDCECR_ET_SHIFT		13 /* Endpoint Type */
#define UDCECR_ED			(1 << 12) /* Endpoint Direction 1 = IN */
#define UDCECR_ED_SHIFT		12 
#define UDCECR_MPS_MASK		(0x3ff << 2) /* Max packet size */
#define UDCECR_MPS_SHIFT		2 
#define UDCECR_DE			(1 << 1) /* Double buffer enable */
#define UDCECR_DE_SHIFT		1
#define UDCECR_EE			(1 << 0) /* Endpoint Enable */

#define	UDCECR_ED_IN			1
#define	UDCECR_ED_OUT		0
#define	UDCECR_DE_DBLBUF		1
#define	UDCECR_DE_SGLBUF		0
#define UDCECR_ET_INT		0x3 /* Endpoint Type - Interrupt */
#define UDCECR_ET_BULK		0x2 /* Endpoint Type - Bulk */
#define UDCECR_ET_ISO		0x1 /* Endpoint Type - isochronous */

#define UDCECR_SETUP(x, cn, in, aisn, en, et, ed, mps, de)	\
			UDCECR(x) = (((cn) << UDCECR_CN_SHIFT) | ((in) << UDCECR_IN_SHIFT) | \
				((aisn) << UDCECR_AISN_SHIFT) | ((en) << UDCECR_EN_SHIFT) |	\
				((et) << UDCECR_ET_SHIFT) | ((ed) << UDCECR_ED_SHIFT) | \
				((mps) << UDCECR_MPS_SHIFT) | \
				((de) << UDCECR_DE_SHIFT) | UDCECR_EE )

/* 
 * Keypad Interface
 */

#define KPC		__REG(0x41500000+0x00) /* Keypad Interface Control register */
#define KPDK 		__REG(0x41500000+0x08) /* Keypad Interface Direct Key register */
#define KPREC		__REG(0x41500000+0x10) /* Keypad Interface Rotary Encoder Count register */
#define KPMK		__REG(0x41500000+0x18) /* Keypad Interface Matrix Key register */
#define KPAS		__REG(0x41500000+0x20) /* Keypad Interface Automatic Scan register */
#define KPASMKP0	__REG(0x41500000+0x28) /* Keypad Interface Automatic Scan Multiple keypress register 0 */
#define KPASMKP1	__REG(0x41500000+0x30) /* Keypad Interface Automatic Scan Multiple keypress register 1 */
#define KPASMKP2	__REG(0x41500000+0x38) /* Keypad Interface Automatic Scan Multiple keypress register 2 */
#define KPASMKP3	__REG(0x41500000+0x40) /* Keypad Interface Automatic Scan Multiple keypress register 3 */
#define KPKDI		__REG(0x41500000+0x48) /* Keypad Interface Key Debounce Interval register */

#define KPC_AS		(1<<30)			/* Automatic Scan bit */
#define KPC_ASACT	(1<<29)			/* Automatic Scan on Activity bit */
#define KPC_MKRN_MASK	(0x07<<26)
#define KPC_MKRN_SHIFT	26			/* Matrix Keypad Row Number */
#define KPC_MKCN_MASK	(0x07<<23)
#define KPC_MKCN_SHIFT	23			/* Matrix Keypad Col Number */
#define KPC_MI		(1<<22)			/* Matrix Keypad Interrupt bit */
#define KPC_IMKP	(1<<21)			/* Ignore Multiple keypress */
#define KPC_MS0		(1<<13)			/* Manual Matrix Scan line 0 */
#define KPC_MS1		(1<<14)			/* Manual Matrix Scan line 1 */
#define KPC_MS2		(1<<15)			/* Manual Matrix Scan line 2 */
#define KPC_MS3		(1<<16)			/* Manual Matrix Scan line 3 */
#define KPC_MS4		(1<<17)			/* Manual Matrix Scan line 4 */
#define KPC_MS5		(1<<18)			/* Manual Matrix Scan line 5 */
#define KPC_MS6		(1<<19)			/* Manual Matrix Scan line 6 */
#define KPC_MS7		(1<<20)			/* Manual Matrix Scan line 7 */

#define KPC_ME		(1<<12)			/* Matrix Keypad Enable */
#define KPC_MIE		(1<<11)			/* Matrix Interrupt Enable */

#define KPC_DIE		1			/* Direct Keypad Interrupt Enable */
#define KPC_DE		(1<<1)			/* Direct Keypad Enable */
#define KPC_REE0	(1<<2)			/* Rotary Encoder0 Enable */
#define KPC_REE1	(1<<3)			/* Rotary Encoder1 Enable */
#define KPC_RE_ZERO_DEB	(1<<4)			/* Rotary Encoder Zero Debounce */
#define KPC_DI		(1<<5)			/* Direct Keypad Interrupt bit */
#define KPC_DKN_MASK	(0x07<<6)
#define KPC_DKN_SHIFT	6			/* Direct Key Number */
#define KPC_DK_DEB_SEL	(1<<9)			/* Direct Key Debounce Select */

#define KPMK_MKP	(1<<31)			/* Matrix key pressed since last read */

#define KPAS_MUKP_MASK	0x7C000000		/* Multiple Keys Pressed */
#define KPAS_MUKP_SHIFT	26
#define KPAS_SO		(1<<31)			/* Scan On bit */

#define KPKDI_DKDI_MASK	(0xFF<<8)		/* Direct key debounce interval in ms */
#define KPKDI_DKDI_SHIFT	8
#define KPKDI_MKDI_MASK	0xFF			/* Matrix key debounce interval in ms */
/*
 * Camera Interface
 */

#define CICR0		__REG(0x50000000)	/* Capture Interface Control Register 0 */
#define CICR1		__REG(0x50000004)	/* Capture Interface Control Register 1 */
#define CICR2		__REG(0x50000008)	/* Capture Interface Control Register 2 */
#define CICR3		__REG(0x5000000C)	/* Capture Interface Control Register 3 */
#define CICR4		__REG(0x50000010)	/* Capture Interface Control Register 4 */
#define CISR			__REG(0x50000014)	/* Capture Interface Status Register */
#define CIFR			__REG(0x50000018)	/* Capture Interface FIFO Control Register */
#define CITOR		__REG(0x5000001C)	/* Capture Interface Time-out Register */
#define CIBR0		__REG(0x50000028)	/* Capture Interface Receive Buffer Register 0 */
#define CIBR1		__REG(0x50000030)	/* Capture Interface Receive Buffer Register 1 */
#define CIBR2		__REG(0x50000038)	/* Capture Interface Receive Buffer Register 2 */
#define CICR0_DMAEN	(1<<31)		/* DMA request enable */
#define CICR0_PAR_EN	(1<<30)		/* Parity enable for embedded modes */
#define CICR0_SL_CAP_EN	(1<<29)		/* Capture enable for slave mode */
#define CICR0_ENB	(1<<28)		/* Camera interface enable */
#define CICR0_DIS	(1<<27)		/* Camera interface disable */
#define CICR0_SIM_MASK	(0x07<<24)
#define CICR0_SIM_SHIFT	24		/* Sensor interface mode */
#define CICR0_TOM	(1<<9)		/* Time-out mask */
#define CICR0_RDAVM	(1<<8)		/* Receive-data-available mask */
#define CICR0_FEM	(1<<7)		/* FIFO-empty mask */
#define CICR0_EOLM	(1<<6)		/* End-of-line mask */
#define CICR0_PERRM	(1<<5)		/* Parity-error mask */
#define CICR0_QDM	(1<<4)		/* Quick-disable mask */
#define CICR0_CDM	(1<<3)		/* Disable-done mask */
#define CICR0_SOFM	(1<<2)		/* Start-of-frame mask */
#define CICR0_EOFM	(1<<1)		/* End-of-frame mask */
#define CICR0_FOM	(1<<0)		/* FIFO overrun mask */

#define CICR1_TBIT	(1<<31)		/* Transparency bit */
#define CICR1_PPL_MASK	(2047<<15)
#define CICR1_PPL_SHIFT	15		/* Pixels per line for the frame */

#define CICR1_DW_MASK	0x07
#define CICR1_DW_4	0x0		/* 4 bits wide data from the sensor */
#define CICR1_DW_5	0x1		/* 5 bits wide data from the sensor */
#define CICR1_DW_8	0x2		/* 8 bits wide data from the sensor */
#define CICR1_DW_9	0x3		/* 9 bits wide data from the sensor */
#define CICR1_DW_A	0x4		/* 10 bits wide data from the sensor */

#define CICR1_RGB_BPP_MASK 0x0380	
#define CICR1_RGB_BPP_SHIFT 7

#define CICR1_COLOR_SP_MASK 0x18
#define CICR1_COLOR_SP_SHIFT 3

#define CICR2_BLW_MASK	0xFF000000	/* Beginning-of-line pixel clock wait count */
#define CICR2_BLW_SHIFT	24
#define CICR2_ELW_MASK	0x00FF0000	/* End-of-line pixel clock wait count */
#define CICR2_ELW_SHIFT	16		
#define CICR2_HSW_MASK	0x0000FC00	/* Horizontal sync pulse width */
#define CICR2_HSW_SHIFT	10
#define CICR2_BFPW_MASK	0x000001F8	/* Beginning-of-frame pixel clock wait count */
#define CICR2_BFPW_SHIFT	3
#define CICR2_FSW_MASK	0x07		/* Frame stabilization wait count */

#define CICR3_BFW_MASK	0xFF000000	/* Beginning-of-frame line clock wait count */
#define CICR3_BFW_SHIFT	24
#define CICR3_EFW_MASK	0x00FF0000	/* End-of-frame line clock wait count */
#define CICR3_EFW_SHIFT 16	
#define CICR3_VSW_MASK	0x0000F800	/* Vertical sync pulse width */
#define CICR3_VSW_SHIFT 11		
#define CICR3_LPF_MASK	0x000007FF	/* Lines per frame 8 */

#define CICR4_MCLK_DLY_MASK (0x07<<24)	/* MCLK Data Capture Delay */
#define CICR4_MCLK_DLY_SHIFT 24
#define CICR4_PCLK_EN	(1<<23)		/* Pixel clock enable */
#define CICR4_PCP	(1<<22)		/* Pixel clock polarity */
#define CICR4_HSP	(1<<21)		/* Horizontal sync polarity */
#define CICR4_VSP	(1<<20)		/* Vertical sync polarity */
#define CICR4_MCLK_EN	(1<<19)		/* MCLK enable */
#define CICR4_FR_RATE_MASK 0x0700	/* Frame capture rate */
#define CICR4_FR_RATE_SHIFT 8	
#define CICR4_DIV_MASK	0xFF		/* Clock divisor */	

#define CISR_FTO	(1<<15)		/* FIFO time-out */
#define CISR_RDAV_2	(1<<14)		/* Channel 2 receive data available */
#define CISR_RDAV_1	(1<<13)		/* Channel 1 receive data available */
#define CISR_RDAV_0	(1<<12)		/* Channel 0 receive data available */
#define CISR_FEMPTY_2	(1<<11)		/* Channel 2 FIFO empty */
#define CISR_FEMPTY_1	(1<<10)		/* Channel 1 FIFO empty */
#define CISR_FEMPTY_0	(1<<9)		/* Channel 0 FIFO empty */
#define CISR_EOL	(1<<8)		/* End of Line */
#define CISR_PAR_ERR	(1<<7)		/* Parity error */
#define CISR_CQD	(1<<6)		/* Camera interface quick disable */
#define CISR_CDD	(1<<5)		/* Camera interface disable done */
#define CISR_SOF	(1<<4)		/* Start of Frame */
#define CISR_EOF	(1<<3)		/* End of frame */
#define CISR_IFO_2	(1<<2)		/* FIFO overrun ofr Channel 2 */
#define CISR_IFO_1	(1<<1)		/* FIFO overrun ofr Channel 1 */
#define CISR_IFO_0	1		/* FIFO overrun ofr Channel 0 */


#define CIFR_FLVL2_MASK	0x3F800000	/* FIFO 2 level */
#define CIFR_FLVL1_MASK 0x007F0000	/* FIFO 1 level */
#define CIFR_FLVL0_MASK 0x0000FF00	/* FIFO 0 level */
#define CIFR_FLVL2_SHIFT 23		
#define CIFR_FLVL1_SHIFT 16
#define CIFR_FLVL0_SHIFT 8
#define CIFR_THL0_MASK	0x30		/* Threshold level for Channel 0 FIFO */
#define CIFR_THL0_SHIFT 4		
#define CIFR_RESETF	(1<<3)		/* Reset input FIFOs */
#define CIFR_FEN2	(1<<2)		/* FIFO enable for channel 2 */
#define CIFR_FEN1	(1<<1)		/* FIFO enable for channel 1 */
#define CIFR_FEN0	(1<<0)		/* FIFO enable for channel 0 */


/*
 * Fast Infrared Communication Port
 */

#define ICCR0		__REG(0x40800000)  /* ICP Control Register 0 */
#define ICCR1		__REG(0x40800004)  /* ICP Control Register 1 */
#define ICCR2		__REG(0x40800008)  /* ICP Control Register 2 */
#define ICDR		__REG(0x4080000c)  /* ICP Data Register */
#define ICSR0		__REG(0x40800014)  /* ICP Status Register 0 */
#define ICSR1		__REG(0x40800018)  /* ICP Status Register 1 */
#define ICFOR		__REG(0x4080001c)  /* ICP Status Register 1 */ //add by kcheng 04.04.01

#define ICCR0_AME       (1 << 7)           /* Adress match enable */
#define ICCR0_TIE       (1 << 6)           /* Transmit FIFO interrupt enable */ 
#define ICCR0_RIE       (1 << 5)           /* Recieve FIFO interrupt enable */
#define ICCR0_RXE       (1 << 4)           /* Receive enable */
#define ICCR0_TXE       (1 << 3)           /* Transmit enable */
#define ICCR0_TUS       (1 << 2)           /* Transmit FIFO underrun select */
#define ICCR0_LBM       (1 << 1)           /* Loopback mode */
#define ICCR0_ITR       (1 << 0)           /* IrDA transmission */

#define ICCR2_BUS       (1 << 5)           
#define ICCR2_TRA       (1 << 4)           
#define ICCR2_RXP       (1 << 3)           
#define ICCR2_TXP       (1 << 2)           

#define ICSR0_FRE       (1 << 5)           /* Framing error */
#define ICSR0_RFS       (1 << 4)           /* Receive FIFO service request */
#define ICSR0_TFS       (1 << 3)           /* Transnit FIFO service request */
#define ICSR0_RAB       (1 << 2)           /* Receiver abort */
#define ICSR0_TUR       (1 << 1)           /* Trunsmit FIFO underun */
#define ICSR0_EIF       (1 << 0)           /* End/Error in FIFO */

#define ICSR1_ROR       (1 << 6)           /* Receiver FIFO underrun  */
#define ICSR1_CRE       (1 << 5)           /* CRC error */
#define ICSR1_EOF       (1 << 4)           /* End of frame */
#define ICSR1_TNF       (1 << 3)           /* Transmit FIFO not full */
#define ICSR1_RNE       (1 << 2)           /* Receive FIFO not empty */
#define ICSR1_TBY       (1 << 1)           /* Tramsmiter busy flag */
#define ICSR1_RSY       (1 << 0)           /* Recevier synchronized flag */


/*
 * Real Time Clock
 */

#define RCNR		__REG(0x40900000)  /* RTC Count Register */
#define RTAR		__REG(0x40900004)  /* RTC Alarm Register */
#define RTSR		__REG(0x40900008)  /* RTC Status Register */
#define RTTR		__REG(0x4090000C)  /* RTC Timer Trim Register */


#define RTCPICR		__REG(0x40900034)  /* RTC Periodic Interrupt Counter register */
#define RTCPIAR		__REG(0x40900038)  /* RTC Periodic Interrupt Alarm register */



#define RTSR_PICE	(1 << 15)       /*Periodic Interrupt Count Enable for RTCPICR Count Register*/
#define RTSR_PIALE	(1 << 14)	/* Periodic Interrupt Alarm Enable */
#define RTSR_PIAL	(1 << 13)	/* RTC period alarm detected */


#define RTSR_HZE	(1 << 3)	/* HZ interrupt enable */
#define RTSR_ALE	(1 << 2)	/* RTC alarm interrupt enable */
#define RTSR_HZ		(1 << 1)	/* HZ rising-edge detected */
#define RTSR_AL		(1 << 0)	/* RTC alarm detected */


/*
 * Pulse Width Modulator
 */

#define PWM_CTRL0	__REG(0x40B00000)  /* PWM 0 Control Register */
#define PWM_PWDUTY0	__REG(0x40B00004)  /* PWM 0 Duty Cycle Register */
#define PWM_PERVAL0	__REG(0x40B00008)  /* PWM 0 Period Control Register */

#define PWM_CTRL1	__REG(0x40C00000)  /* PWM 1Control Register */
#define PWM_PWDUTY1	__REG(0x40C00004)  /* PWM 1 Duty Cycle Register */
#define PWM_PERVAL1	__REG(0x40C00008)  /* PWM 1 Period Control Register */



/*
 * Power Manager
 */

#define PMCR		__REG(0x40F00000)  /* Power Manager Control Register */
#define PSSR		__REG(0x40F00004)  /* Power Manager Sleep Status Register */
#define PSPR		__REG(0x40F00008)  /* Power Manager Scratch Pad Register */
#define PWER		__REG(0x40F0000C)  /* Power Manager Wake-up Enable Register */
#define PRER		__REG(0x40F00010)  /* Power Manager GPIO Rising-Edge Detect Enable Register */
#define PFER		__REG(0x40F00014)  /* Power Manager GPIO Falling-Edge Detect Enable Register */
#define PEDR		__REG(0x40F00018)  /* Power Manager GPIO Edge Detect Status Register */
#define PCFR		__REG(0x40F0001C)  /* Power Manager General Configuration Register */
#define PGSR0		__REG(0x40F00020)  /* Power Manager GPIO Sleep State Register for GP[31-0] */
#define PGSR1		__REG(0x40F00024)  /* Power Manager GPIO Sleep State Register for GP[63-32] */
#define PGSR2		__REG(0x40F00028)  /* Power Manager GPIO Sleep State Register for GP[84-64] */
#define PGSR3		__REG(0x40F0002C)  /* Power Manager GPIO Sleep State Register for GP[118-96] */
#define RCSR		__REG(0x40F00030)  /* Reset Controller Status Register */

#define    PSLR    __REG(0x40F00034)   	/* Power Manager Sleep Config Register */
#define    PSTR    __REG(0x40F00038)  	/*Power Manager Standby Config Register */
#define    PSNR    __REG(0x40F0003C)  	/*Power Manager Sense Config Register */
#define    PVCR    __REG(0x40F00040)  	/*Power Manager VoltageControl Register */
#define    PKWR    __REG(0x40F00050)   	/* Power Manager KB Wake-up Enable Reg */
#define    PKSR    __REG(0x40F00054)   	/* Power Manager KB Level-Detect Register */



#define PUCR_UDETS	(1<<5)		/* USIM detect Status */
#define PUCR_USIM115	(1<<3)		/* Allow UVS or UEN functionality for GPIO(115) */
#define PUCR_USIM114	(1<<2)		/* Allow UVS or UEN functionality for GPIO(114) */
#define PUCR_EN_UDET	1		/* Enable USIM Card Detect */

#define PSSR_OTGPH	(1<<6)
#define PSSR_RDH	(1 << 5)	/* Read Disable Hold */
#define PSSR_PH		(1 << 4)	/* Peripheral Control Hold */
#define PSSR_VFS	(1 << 2)	/* VDD Fault Status */
#define PSSR_BFS	(1 << 1)	/* Battery Fault Status */
#define PSSR_SSS	(1 << 0)	/* Software Sleep Status */

#define PCFR_DS		(1 << 3)	/* Deep Sleep Mode */
#define PCFR_FS		(1 << 2)	/* Float Static Chip Selects */
#define PCFR_FP		(1 << 1)	/* Float PCMCIA controls */
#define PCFR_OPDE	(1 << 0)	/* 3.6864 MHz oscillator power-down enable */

#define RCSR_GPR	(1 << 3)	/* GPIO Reset */
#define RCSR_SMR	(1 << 2)	/* Sleep Mode */
#define RCSR_WDR	(1 << 1)	/* Watchdog Reset */
#define RCSR_HWR	(1 << 0)	/* Hardware Reset */


/*
 * SSP Serial Port Registers
 */

#define SSCR0		__REG(0x41700000)  /* SSP Control Register 0 */
#define SSCR1		__REG(0x41700004)  /* SSP Control Register 1 */
#define SSSR		__REG(0x41700008)  /* SSP Status Register */
#define SSITR		__REG(0x4170000C)  /* SSP Interrupt Test Register */
#define SSDR		__REG(0x41700010)  /* (Write / Read) SSP Data Write Register/SSP Data Read Register */
#define SSPSP		__REG(0x4170002c)  

/*
 * I2S Register
 */

#define SACR0		__REG(0x40400000)  /* Global Control Register */
#define SACR1		__REG(0x40400004)  /* Serial Audio I 2 S/MSB-Justified Control Register */
#define SASR0		__REG(0x4040000C)  /* Serial Audio I 2 S/MSB-Justified Interface and FIFO Status Register */
#define SAIMR		__REG(0x40400014)  /* Serial Audio Interrupt Mask Register */
#define SAICR		__REG(0x40400018)  /* Serial Audio Interrupt Clear Register */
#define SADIV		__REG(0x40400060)  /* Audio Clock Divider Register. */
#define SADR		__REG(0x40400080)  /* Serial Audio Data Register (TX and RX FIFO access Register). */


/*
 * USB Host Controller
 */
#define UHCREV          __REG(0x4C000000)
#define UHCHCON         __REG(0x4C000004)
#define UHCCOMS         __REG(0x4C000008)
#define UHCINTS         __REG(0x4C00000C)
#define UHCINTE         __REG(0x4C000010)
#define UHCINTD         __REG(0x4C000014)
#define UHCHCCA         __REG(0x4C000018)
#define UHCPCED         __REG(0x4C00001C)
#define UHCCHED         __REG(0x4C000020)
#define UHCCCED         __REG(0x4C000024)
#define UHCBHED         __REG(0x4C000028)
#define UHCBCED         __REG(0x4C00002C)
#define UHCDHEAD        __REG(0x4C000030)
#define UHCFMI          __REG(0x4C000034)
#define UHCFMR          __REG(0x4C000038)
#define UHCFMN          __REG(0x4C00003C)
#define UHCPERS         __REG(0x4C000040)
#define UHCLST          __REG(0x4C000044)
#define UHCRHDA         __REG(0x4C000048)
#define UHCRHDB         __REG(0x4C00004C)
#define UHCRHS          __REG(0x4C000050)
#define UHCRHPS1        __REG(0x4C000054)
#define UHCRHPS2        __REG(0x4C000058)
#define UHCRHPS3        __REG(0x4C00005C)
#define UHCSTAT         __REG(0x4C000060)
#define UHCHR           __REG(0x4C000064)
#define UHCHIE          __REG(0x4C000068)
#define UHCHIT          __REG(0x4C00006C)

#define UHCRHPS(x)        __REG2( 0x4C000050, (x)<<2 )


#define UHCHR_FSBIR     (1<<0)
#define UHCHR_FHR       (1<<1)
#define UHCHR_CGR       (1<<2)
#define UHCHR_SSDC      (1<<3)
#define UHCHR_UIT       (1<<4)
#define UHCHR_SSE       (1<<5)
#define UHCHR_PSPL      (1<<6)
#define UHCHR_PCPL      (1<<7)
#define UHCHR_SSEP0     (1<<9)
#define UHCHR_SSEP1     (1<<10)
#define UHCHR_SSEP2     (1<<11)

#define UHCHIE_UPRIE    (1<<13)
#define UHCHIE_UPS2IE   (1<<12)
#define UHCHIE_UPS1IE   (1<<11)
#define UHCHIE_TAIE     (1<<10)
#define UHCHIE_HBAIE    (1<<8)
#define UHCHIE_RWIE     (1<<7)


/*
 * Memory controller
 */

#define MDCNFG		__REG(0x48000000)  /* SDRAM Configuration Register 0 */
#define MDREFR		__REG(0x48000004)  /* SDRAM Refresh Control Register */
#define MSC0		__REG(0x48000008)  /* Static Memory Control Register 0 */
#define MSC1		__REG(0x4800000C)  /* Static Memory Control Register 1 */
#define MSC2		__REG(0x48000010)  /* Static Memory Control Register 2 */
#define MECR		__REG(0x48000014)  /* Expansion Memory (PCMCIA/Compact Flash) Bus Configuration */
#define SXLCR		__REG(0x48000018)  /* LCR value to be written to SDRAM-Timing Synchronous Flash */
#define SXCNFG		__REG(0x4800001C)  /* Synchronous Static Memory Control Register */
#define SXMRS		__REG(0x48000024)  /* MRS value to be written to Synchronous Flash or SMROM */
#define MCMEM0		__REG(0x48000028)  /* Card interface Common Memory Space Socket 0 Timing */
#define MCMEM1		__REG(0x4800002C)  /* Card interface Common Memory Space Socket 1 Timing */
#define MCATT0		__REG(0x48000030)  /* Card interface Attribute Space Socket 0 Timing Configuration */
#define MCATT1		__REG(0x48000034)  /* Card interface Attribute Space Socket 1 Timing Configuration */
#define MCIO0		__REG(0x48000038)  /* Card interface I/O Space Socket 0 Timing Configuration */
#define MCIO1		__REG(0x4800003C)  /* Card interface I/O Space Socket 1 Timing Configuration */
#define MDMRS		__REG(0x48000040)  /* MRS value to be written to SDRAM */
#define BOOT_DEF	__REG(0x48000044)  /* Read-Only Boot-Time Register. Contains BOOT_SEL and PKG_SEL */





#define MDREFR_K0DB4    (1<<29)

#define MDREFR_K2FREE	(1 << 25)	/* SDRAM Free-Running Control */
#define MDREFR_K1FREE	(1 << 24)	/* SDRAM Free-Running Control */
#define MDREFR_K0FREE	(1 << 23)	/* SDRAM Free-Running Control */
#define MDREFR_SLFRSH	(1 << 22)	/* SDRAM Self-Refresh Control/Status */
#define MDREFR_APD		(1 << 20)	/* SDRAM/SSRAM Auto-Power-Down Enable */
#define MDREFR_K2DB2	(1 << 19)	/* SDCLK2 Divide by 2 Control/Status */
#define MDREFR_K2RUN	(1 << 18)	/* SDCLK2 Run Control/Status */
#define MDREFR_K1DB2	(1 << 17)	/* SDCLK1 Divide by 2 Control/Status */
#define MDREFR_K1RUN	(1 << 16)	/* SDCLK1 Run Control/Status */
#define MDREFR_E1PIN	(1 << 15)	/* SDCKE1 Level Control/Status */
#define MDREFR_K0DB2	(1 << 14)	/* SDCLK0 Divide by 2 Control/Status */
#define MDREFR_K0RUN	(1 << 13)	/* SDCLK0 Run Control/Status */
#define MDREFR_E0PIN	(1 << 12)	/* SDCKE0 Level Control/Status */

/*some generic routines*/
extern void bulv_gpio_config(int gpio_mode);

//volatile unsigned long *clkPtrReg;

static void bulv_clk_enable(unsigned clk)
{

	CKEN |= clk;
}

static  void bulv_clk_disable(unsigned clk)
{
	CKEN &= ~clk;
}

#endif //_BULVERDE_H_


