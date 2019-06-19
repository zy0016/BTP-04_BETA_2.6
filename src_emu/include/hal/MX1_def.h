
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	 sys\drivers\
 *
 * Filename:     MX1-def.h
 *
 * Purpose :     
 *  
 * Author  :     jietang
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision: 1.3 $     $Date: 2003/09/26 07:34:10 $
 *
 * $Name:  $
 * 
\**************************************************************************/

/******************************************************************************

 C   H E A D E R   F I L E

 (c) Copyright Motorola Semiconductors Hong Kong Limited 2001-2002
 ALL RIGHTS RESERVED

*******************************************************************************

 Project Name : DBMX1 System Test Program
 Project No.  : 
 Title        : 
 Template Ver : 0.3
 File Name    : MX1_def.h  
 Last Modified: Nov 14, 2001

 Description  : Definition header for DBMX1 silicon. 

 Assumptions  : NA
 
 Dependency Comments : NA

 Project Specific Data : NA

******************************************************************************/
#ifndef MX1_DEF_INC
#define MX1_DEF_INC

#include "config.h"

#ifndef IO_ADDRESS
#ifdef __MMU__
#define io_p2v(x)	( (PORTMAP_BASE) - (PORT_BASE) + (x) )
#define io_v2p(x)	( (x) - (PORTMAP_BASE) + (PORT_BASE) )
#else
#define io_p2v(x)	( (unsigned long)(x) )
#define io_v2p(x)	( (unsigned long)(x) )
#endif
#define IO_ADDRESS(x)		(io_p2v(x))
#endif

/*************************** Header File Includes ****************************/
/********************************* Constants *********************************/
/******************************** Enumeration ********************************/
/****************************** Basic Data types *****************************/
/************************* Structure/Union Data types ************************/
/********************************** Macros ***********************************/

/* common define for MX1 */
//$0000 0000 - $000F FFFF Double Map Image 1 MB
//$0010 0000 - $001F FFFF Bootstrap ROM 1 MB
//$0020 0000 - $0020 0FFF AIPI1 4 KB
//$0020 1000 - $0020 1FFF WatchDog 4 KB
//$0020 2000 - $0020 2FFF TIMER1 4 KB
//$0020 3000 - $0020 3FFF TIMER2 4 KB
//$0020 4000 - $0020 4FFF RTC 4 KB
//$0020 5000 - $0020 5FFF LCD 4 KB
//$0020 6000 - $0020 6FFF UART1 4 KB
//$0020 7000 - $0020 7FFF UART2 4 KB
//$0020 8000 - $0020 8FFF PWM 4 KB
//$0020 9000 - $0020 9FFF DMA 4 KB
//$0020 A000 - $0020 AFFF Reserved 4 KB
//$0020 B000 - $0020 BFFF Reserved 4 KB
//$0020 C000 - $0020 CFFF Reserved 4 KB
//$0020 D000 - $0020 DFFF Reserved 4 KB
//$0020 E000 - $0020 EFFF Reserved 4 KB
//$0020 F000 - $0020 FFFF Reserved 4 KB
//$0021 0000 - $0021 0FFF AIPI2 4 KB
//$0021 1000 - $0021 1FFF SIM 4 KB
//$0021 2000 - $0021 2FFF USBD 4 KB
//$0021 3000 - $0021 3FFF CSPI 4 KB
//$0021 4000 - $0021 4FFF MMC 4 KB
//$0021 5000 - $0021 5FFF ASP 4 KB
//$0021 6000 - $0021 6FFF BTA 4 KB
//$0021 7000 - $0021 7FFF I2C 4 KB
//$0021 8000 - $0021 8FFF SSI 4 KB
//$0021 9000 - $0021 9FFF Reserved 4 KB
//$0021 A000 - $ 0021 AFFF Memory Stick 4 KB
//$0021 B000 - $0021 BFFF CRM 4 KB
//$0021 C000 - $ 0021 CFFF GPIO 4 KB
//$0021 D000 - $0021 DFFF Reserved 4 KB
//$0021 E000 - $0021 EFFF Reserved 4 KB
//$0021 F000 - $0021 FFFF Reserved 4 KB
//$0022 0000 - $0022 0FFF WEIM 4 KB
//$0022 1000 - $0022 1FFF SDRAMC 4 KB
//$0022 2000 - $0022 2FFF DSPA 4 KB
//$0022 3000 - $0022 3FFF AITC 4 KB
//$0022 4000 - $0022 4FFF CSI 4 KB
//$0022 5000 - $0022 5FFF Reserved 4 KB
//$0022 6000 - $0022 6FFF Reserved 4 KB
//$0022 7000 - $002F FFFF Reserved 868 KB

// ; Memory Map
// ; ----------
// ; $0020_0XXX aipi1
// ; $0020_1XXX Watchdog (wdt_ip)
// ; $0020_2XXX Timer1 (timer_ip1)
// ; $0020_3XXX Timer2 (timer_ip2)
// ; $0020_4XXX RTC (rtc_ip)
// ; $0020_5XXX LCD (lcd_ip)
// ; $0020_6XXX UART1 (uart_ip1)
// ; $0020_7XXX UART2 (uart_ip2)
// ; $0020_8XXX PWM (pwm_ip)
// ; $0020_9XXX DMA (dma_ip)
// ; $0020_AXXX Reserved
// ; $0020_BXXX Reserved
// ; $0020_CXXX Reserved
// ; $0020_DXXX Reserved
// ; $0020_EXXX Reserved
// ; $0020_FXXX Reserved
// ; 
// ; $0021_0XXX AIPI2 (dbmx_aipi2)
// ; $0021_1XXX SIM (sim_ip)
// ; $0021_2XXX USBD (usbd_ip)
// ; $0021_3XXX CSPI (cspi_ip)
// ; $0021_4XXX MMC (mmc_ip)
// ; $0021_5XXX ASP (asp_ip)
// ; $0021_6XXX BTA (bta_ip)
// ; $0021_7XXX I2C (i2c)
// ; $0021_8XXX SSI (ssi_ip)
// ; $0021_9XXX Video Port
// ; $0021_AXXX Memory Stick
// ; $0021_BXXX Clock & Reset (crm)
// ; $0021_CXXX GPIO (gpio_ip)
// ; $0021_DXXX Reserved
// ; $0021_EXXX Reserved
// ; $0021_FXXX Reserved
// ;
// ; $0022_0XXX Weim
// ; $0022_1XXX SDRAMC
// ; $0022_2XXX DSPA
// ; $0022_3XXX AITC
// ; $0022_4XXX Ext1
// ; $0022_5XXX Ext2
// ; $0022_6XXX Ext3

// ; ARM Program Status Register
// ;  31  30  29  28  27             7   6   5  4
// ; ----------------------------------------------------------
// ; | N | Z | C | V | Q |         | I | F | T |  |  |  |  |  |
// ; ----------------------------------------------------------
//
// ;---------------------------------------;
// ;ARM modes                              ;
// ;---------------------------------------;
#define MODE_USR                0x10
#define MODE_FIQ                0x11
#define MODE_IRQ                0x12
#define MODE_SVC                0x13
#define MODE_ABT                0x17
#define MODE_UND                0x1B
#define MODE_SYS                0x1F
#define MODE_BITS               0x1F
#define TBIT                    (1:SHL:5)
#define FBIT                    (1:SHL:6)
#define IBIT                    (1:SHL:7)
//
// ;---------------------------------------;
// ; ARM interrupts                        ;
// ;---------------------------------------;
#define INTERRUPT_BITS          0xC0
#define ENABLE_IRQ              0x0
#define ENABLE_FIQ              0x0
#define DISABLE_FIQ             0x40
#define DISABLE_IRQ             0x80
//
// ;---------------------------------------;
// ; ARM FLAGS                             ;
// ;---------------------------------------;
#define FLAG_BITS               0xF0000000
#define NFLAG                   0x80000000
#define ZFLAG                   0x40000000
#define CFLAG                   0x20000000
#define VFLAG                   0x10000000
//
// ;---------------------------------------;
// ; STACK                                 ;
// ;---------------------------------------;
#define SVC_STACK               TOP_OF_STACK            // ; 003F_FDFF - 0040_0000 (512B)
#define IRQ_STACK               (TOP_OF_STACK-0x200)    // ; 003F_FBFF - 003F_FE00 (512B)
#define FIQ_STACK               (TOP_OF_STACK-0x400)    // ; 003F_F9FF - 003F_FC00 (512B)
#define USR_STACK               (TOP_OF_STACK-0x600)    // ; 003F_F7FF - 003F_FA00 (512B)
#define UND_STACK               (TOP_OF_STACK-0x800)    // ; 003F_F7FF - 003F_FA00 (512B)

// ;---------------------------------------;
// ; AIPI1                                 ;
// ; $0020_0000 to $0020_0FFF              ;
// ;---------------------------------------;
#define AIPI1_BASE_ADDR         IO_ADDRESS(0x00200000)              // ; Peripheral Size Reg 0
#define AIPI1_PSR0              AIPI1_BASE_ADDR        // ; Peripheral Size Reg 1
#define AIPI1_PSR1              (AIPI1_BASE_ADDR+0x04)
#define AIPI1_PAR               (AIPI1_BASE_ADDR+0x08)  // ; Peripheral Access Reg
//
// ;---------------------------------------;
// ; AIPI2                                 ;
// ; $0021_0000 to $0021_0FFF              ;
// ;---------------------------------------;
#define AIPI2_BASE_ADDR         IO_ADDRESS(0x00210000)             
#define AIPI2_PSR0              AIPI2_BASE_ADDR        // ; Peripheral Size Reg 0
#define AIPI2_PSR1              (AIPI2_BASE_ADDR+0x04)  // ; Peripheral Size Reg 1
#define AIPI2_PAR               (AIPI2_BASE_ADDR+0x08)  // ; Peripheral Access Reg
//
/* Register base address */
#define PLL_BASE                IO_ADDRESS(0x0021B000)
//
/* Reset Module*/
#define PLL_CSCR                (PLL_BASE+0x00) //Clock Source Control Register
#define PLL_PCDR                (PLL_BASE+0x20) //Peripherial Clock Divider Register
/* PLL & Clock Controller */
#define PLL_MCTL0               (PLL_BASE+0x04) //MCU PLL Control Register 0
#define PLL_MCTL1               (PLL_BASE+0x08) //MCU PLL Control Register 1
#define PLL_UPCTL0              (PLL_BASE+0x0C) //USB PLL Control Register 0
#define PLL_UPCTL1              (PLL_BASE+0x10) //USB PLL Control Register 1
/* System Control */
#define PLL_RSR                 (PLL_BASE+0x800) //Reset Source Register
#define PLL_SIDR                (PLL_BASE+0x804) //Silicon ID Register
#define PLL_FMCR                (PLL_BASE+0x808) //Function Muxing Control Register
#define PLL_GPCR                (PLL_BASE+0x80C) //Global Peripherial Control Regiser
// ;---------------------------------------;
// ; AITC                                  ;
// ; $0022_3000 to $0022_3FFF              ;
// ;---------------------------------------;

#define AITC_BASE_ADDR          IO_ADDRESS(0x00223000)
#define AITC_INTCNTL            (AITC_BASE_ADDR+0x00) //Interrupt Control Register
#define AITC_NIMASK             (AITC_BASE_ADDR+0x04) //Normal Interrupt Mask Register
#define AITC_INTENNUM           (AITC_BASE_ADDR+0x08) //Interrupt Enable Number Register
#define AITC_INTDISNUM          (AITC_BASE_ADDR+0x0C) //Interrupt Disable Number Register
#define AITC_INTENABLEH         (AITC_BASE_ADDR+0x10) //Interrupt Enable Register High
#define AITC_INTENABLEL         (AITC_BASE_ADDR+0x14) //Interrupt Enable Register Low
#define AITC_INTTYPEH           (AITC_BASE_ADDR+0x18)
#define AITC_INTTYPEL           (AITC_BASE_ADDR+0x1C)
#define AITC_NIPRIORITY7        (AITC_BASE_ADDR+0x20)
#define AITC_NIPRIORITY6        (AITC_BASE_ADDR+0x24)
#define AITC_NIPRIORITY5        (AITC_BASE_ADDR+0x28)
#define AITC_NIPRIORITY4        (AITC_BASE_ADDR+0x2C)
#define AITC_NIPRIORITY3        (AITC_BASE_ADDR+0x30)
#define AITC_NIPRIORITY2        (AITC_BASE_ADDR+0x34)
#define AITC_NIPRIORITY1        (AITC_BASE_ADDR+0x38)
#define AITC_NIPRIORITY0        (AITC_BASE_ADDR+0x3C)
#define AITC_NIVECSR            (AITC_BASE_ADDR+0x40)
#define AITC_FIVECSR            (AITC_BASE_ADDR+0x44)
#define AITC_INTSRCH            (AITC_BASE_ADDR+0x48)
#define AITC_INTSRCL            (AITC_BASE_ADDR+0x4C)
#define AITC_INTFRCH            (AITC_BASE_ADDR+0x50)
#define AITC_INTFRCL            (AITC_BASE_ADDR+0x54)
#define AITC_NIPNDH             (AITC_BASE_ADDR+0x58)
#define AITC_NIPNDL             (AITC_BASE_ADDR+0x5C)
#define AITC_FIPNDH             (AITC_BASE_ADDR+0x60)
#define AITC_FIPNDL             (AITC_BASE_ADDR+0x64)


#define INTCNTL (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x00))
#define NIMASK  (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x04))
#define INTENNUM        (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x08))
#define INTDISNUM       (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x0C))
#define INTENABLEH      (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x10))
#define INTENABLEL      (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x14))
#define INTTYPEH        (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x18))
#define INTTYPEL        (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x1C))
#define NIPRIORITY7     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x20))
#define NIPRIORITY6     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x24))
#define NIPRIORITY5     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x28))
#define NIPRIORITY4     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x2C))
#define NIPRIORITY3     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x30))
#define NIPRIORITY2     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x34))
#define NIPRIORITY1     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x38))
#define NIPRIORITY0     (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x3C))
#define NIVECSR (*(volatile int *) (AITC_BASE_ADDR + 0x40))
#define FIVECSR (*(volatile int *) (AITC_BASE_ADDR + 0x44))
#define INTSRCH (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x48))
#define INTSRCL (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x4C))
#define INTFRCH (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x50))
#define INTFRCL (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x54))
#define NIPNDH  (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x58))
#define NIPNDL  (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x5C))
#define FIPNDH  (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x60))
#define FIPNDL  (*(volatile unsigned int *) (AITC_BASE_ADDR + 0x64)) 

//
// ;---------------------------------------;
// ; ASP                                   ;
// ; $0021_5000 to $0021_5FFF              ;
// ;---------------------------------------;
#define ASP_BASE_ADDR           IO_ADDRESS(0x00215000)                
#define ASP_PADFIFO             (ASP_BASE_ADDR+0x00)                
#define ASP_VADFIFO             (ASP_BASE_ADDR+0x04)
#define ASP_VDAFIFO             (ASP_BASE_ADDR+0x08)               
#define ASP_VADCOEF             (ASP_BASE_ADDR+0x0C)                
#define ASP_ACNTLCR             (ASP_BASE_ADDR+0x10)                
#define ASP_PSMPLRG             (ASP_BASE_ADDR+0x14)                
#define ASP_ICNTLR              (ASP_BASE_ADDR+0x18)                
#define ASP_ISTATR              (ASP_BASE_ADDR+0x1C)                
#define ASP_VADGAIN             (ASP_BASE_ADDR+0x20)                
#define ASP_VDAGAIN             (ASP_BASE_ADDR+0x24)                
#define ASP_VDACOEF             (ASP_BASE_ADDR+0x28)                
#define ASP_CLKDIV              (ASP_BASE_ADDR+0x2C)                
#define ASP_CMPCNTL             (ASP_BASE_ADDR+0x30)                
#define ASP_PTRREG              (ASP_BASE_ADDR+0x34)    
//
// ;---------------------------------------;
// ; BOOT ROM                              ;
// ;---------------------------------------;
#define BOOTROM_ADDR_BOT        0x00000000
#define BOOTROM_PHY_SIZE        0x0000028C
#define BOOTROM_ASS_SIZE        0x00100000
//
// ;---------------------------------------;
// ; BTA_WRAPPER                           ;
// ; $0021_6000 to $0021_6FFF              ;
// ;---------------------------------------;
// ; Sequencer
#define BTA_BASE_ADDR           IO_ADDRESS(0x00216000)                
#define BTA_COMMAND             BTA_BASE_ADDR                
#define BTA_STATUS              BTA_BASE_ADDR                
#define BTA_PACKETHEAD          (BTA_BASE_ADDR+0x04)                
#define BTA_PAYLOADHEAD         (BTA_BASE_ADDR+0x08)                
//
// ; BT clocks
#define BTA_NATIVECNT           (BTA_BASE_ADDR+0x0C)                
#define BTA_ESTCNT              (BTA_BASE_ADDR+0x10)                
#define BTA_OFFSETCNT           (BTA_BASE_ADDR+0x14)                
#define BTA_NATIVECLK_L         (BTA_BASE_ADDR+0x18)                
#define BTA_NATIVECLK_H         (BTA_BASE_ADDR+0x1C)                
#define BTA_ESTCLK_L            (BTA_BASE_ADDR+0x20)                
#define BTA_ESTCLK_H            (BTA_BASE_ADDR+0x24)                
#define BTA_OFFSETCLK_L         (BTA_BASE_ADDR+0x28)                
#define BTA_OFFSETCLK_H         (BTA_BASE_ADDR+0x2C)                
//
// ; BT pipeline
#define BTA_HECCRC              (BTA_BASE_ADDR+0x30)                
#define BTA_WHITE               (BTA_BASE_ADDR+0x34)                
#define BTA_ENCRYPT             (BTA_BASE_ADDR+0x38)                
//
// ; Radio Control
#define BTA_CORR_TIME           (BTA_BASE_ADDR+0x40) 
#define BTA_RF_GPO              (BTA_BASE_ADDR+0x48) 
#define BTA_RSSI                (BTA_BASE_ADDR+0x4C) 
#define BTA_TIME_AB             (BTA_BASE_ADDR+0x50) 
#define BTA_TIME_CD             (BTA_BASE_ADDR+0x54) 
#define BTA_PWM_TX              (BTA_BASE_ADDR+0x58) 
#define BTA_RF_CTRL             (BTA_BASE_ADDR+0x5C) 
#define BTA_RF_STATUS           (BTA_BASE_ADDR+0x5C) 
#define BTA_RX_TIME             (BTA_BASE_ADDR+0x60) 
#define BTA_TX_TIME             (BTA_BASE_ADDR+0x64) 
// ; Bit Reverse 
#define BTA_WORD_REV            (BTA_BASE_ADDR+0x178) 
#define BTA_BYTE_REV            (BTA_BASE_ADDR+0x17C)                 
//
// ; Timer
#define BTA_TIMER               (BTA_BASE_ADDR+0x68)                
//
// ; Correlator
#define BTA_THRESHOLD           (BTA_BASE_ADDR+0x6C)                
#define BTA_CORR_MAX            (BTA_BASE_ADDR+0x6C)                
#define BTA_SYNCHWORD_0         (BTA_BASE_ADDR+0x70)                
#define BTA_SYNCHWORD_1         (BTA_BASE_ADDR+0x74)                
#define BTA_SYNCHWORD_2         (BTA_BASE_ADDR+0x78)                
#define BTA_SYNCHWORD_3         (BTA_BASE_ADDR+0x7C)                
//
// ; BitBUF
#define BTA_BUF_WORD_0          (BTA_BASE_ADDR+0x80)                
#define BTA_BUF_WORD_1          (BTA_BASE_ADDR+0x84)                
#define BTA_BUF_WORD_2          (BTA_BASE_ADDR+0x88)                
#define BTA_BUF_WORD_3          (BTA_BASE_ADDR+0x8C)                
#define BTA_BUF_WORD_4          (BTA_BASE_ADDR+0x90)                
#define BTA_BUF_WORD_5          (BTA_BASE_ADDR+0x94)                
#define BTA_BUF_WORD_6          (BTA_BASE_ADDR+0x98)                
#define BTA_BUF_WORD_7          (BTA_BASE_ADDR+0x9C)                
#define BTA_BUF_WORD_8          (BTA_BASE_ADDR+0xA0)                
#define BTA_BUF_WORD_9          (BTA_BASE_ADDR+0xA4)                
#define BTA_BUF_WORD_10         (BTA_BASE_ADDR+0xA8)                
#define BTA_BUF_WORD_11         (BTA_BASE_ADDR+0xAC)                
#define BTA_BUF_WORD_12         (BTA_BASE_ADDR+0xB0)                
#define BTA_BUF_WORD_13         (BTA_BASE_ADDR+0xB4)                
#define BTA_BUF_WORD_14         (BTA_BASE_ADDR+0xB8)                
#define BTA_BUF_WORD_15         (BTA_BASE_ADDR+0xBC)                
#define BTA_BUF_WORD_16         (BTA_BASE_ADDR+0xC0)                
#define BTA_BUF_WORD_17         (BTA_BASE_ADDR+0xC4)                
#define BTA_BUF_WORD_18         (BTA_BASE_ADDR+0xC8)                
#define BTA_BUF_WORD_19         (BTA_BASE_ADDR+0xCC)                
#define BTA_BUF_WORD_20         (BTA_BASE_ADDR+0xD0)                
#define BTA_BUF_WORD_21         (BTA_BASE_ADDR+0xD4)                
#define BTA_BUF_WORD_22         (BTA_BASE_ADDR+0xD8)                
#define BTA_BUF_WORD_23         (BTA_BASE_ADDR+0xDC)                
#define BTA_BUF_WORD_24         (BTA_BASE_ADDR+0xE0)                
#define BTA_BUF_WORD_25         (BTA_BASE_ADDR+0xE4)                
#define BTA_BUF_WORD_26         (BTA_BASE_ADDR+0xE8)                
#define BTA_BUF_WORD_27         (BTA_BASE_ADDR+0xEC)                
#define BTA_BUF_WORD_28         (BTA_BASE_ADDR+0xF0)                
#define BTA_BUF_WORD_29         (BTA_BASE_ADDR+0xF4)                
#define BTA_BUF_WORD_30         (BTA_BASE_ADDR+0xF8)                
#define BTA_BUF_WORD_31         (BTA_BASE_ADDR+0xFC)                
//                                                 
// ; Wakeup                                        
#define BTA_WU_1                (BTA_BASE_ADDR+0x100) 
#define BTA_WU_2                (BTA_BASE_ADDR+0x104) 
#define BTA_WU_3                (BTA_BASE_ADDR+0x108) 
#define BTA_WU_DELTA3           (BTA_BASE_ADDR+0x108) 
#define BTA_WU_4                (BTA_BASE_ADDR+0x10C) 
#define BTA_WU_DELTA4           (BTA_BASE_ADDR+0x10C) 
#define BTA_WU_CTRL             (BTA_BASE_ADDR+0x110) 
#define BTA_WU_STATUS           (BTA_BASE_ADDR+0x110) 
#define BTA_WU_COUNT            (BTA_BASE_ADDR+0x114) 
//
// ; Clock control
#define BTA_CLK_CTRL            (BTA_BASE_ADDR+0x118)                
//
// ; SPI
#define BTA_SPI_WORD_0          (BTA_BASE_ADDR+0x120)                
#define BTA_SPI_WORD_1          (BTA_BASE_ADDR+0x124)                
#define BTA_SPI_WORD_2          (BTA_BASE_ADDR+0x128)                
#define BTA_SPI_WORD_3          (BTA_BASE_ADDR+0x12C)                
#define BTA_SPI_WADDR           (BTA_BASE_ADDR+0x130)                
#define BTA_SPI_RADDR           (BTA_BASE_ADDR+0x134)                
#define BTA_SPI_CTRL            (BTA_BASE_ADDR+0x138)                
#define BTA_SPI_STATUS          (BTA_BASE_ADDR+0x138) 
//                                                 
// ; Frequency Hopping                             
#define BTA_HOPWORDS_0          (BTA_BASE_ADDR+0x140)                
#define BTA_FREQ_OUT            (BTA_BASE_ADDR+0x140)                
#define BTA_HOPWORDS_1          (BTA_BASE_ADDR+0x144)                
#define BTA_HOPWORDS_2          (BTA_BASE_ADDR+0x148)                
#define BTA_HOPWORDS_3          (BTA_BASE_ADDR+0x14C)                
#define BTA_HOPWORDS_4          (BTA_BASE_ADDR+0x150)                
//
// ; Interrupt
#define BTA_INTERRUPT           (BTA_BASE_ADDR+0x160)                
//
// ; MLSE
#define BTA_SYNC_METRIC         (BTA_BASE_ADDR+0x170)                
#define BTA_SYNC_FC             (BTA_BASE_ADDR+0x174)                
//
// ;---------------------------------------;
// ; Clock & Reset (CRM)                   ;
// ; $0021_B000 to $0021_BFFF              ;
// ;---------------------------------------;
#define CRM_BASE_ADDR           IO_ADDRESS(0x0021B000)                
#define CRM_CSCR                CRM_BASE_ADDR           // ; Clock Source Control Reg
#define CRM_MPCTL0              (CRM_BASE_ADDR+0x04)    // ; MCU PLL Control Reg      
#define CRM_MPCTL1              (CRM_BASE_ADDR+0x08)  // ; MCU PLL & System Clk Ctl Reg
#define CRM_UPCTL0              (CRM_BASE_ADDR+0x0C)    // ; USB PLL Control Reg 0
#define CRM_UPCTL1              (CRM_BASE_ADDR+0x10)    // ; USB PLL Control Reg 1
#define CRM_PCDR                (CRM_BASE_ADDR+0x20)  // ; Perpheral Clock Divider Reg
//
#define CRM_RSR                 (CRM_BASE_ADDR+0x800)   // ; Reset Source Reg 
#define CRM_SIDR                (CRM_BASE_ADDR+0x804)   // ; Silicon ID Reg 
#define CRM_FMCR                (CRM_BASE_ADDR+0x808)   // ; Functional Muxing Control Reg 
#define CRM_GPCR                (CRM_BASE_ADDR+0x80C)   // ; Global Control Reg 
// ;---------------------------------------;
// ; CSI
// ; $0022_4000 to $0022_4FFF              ;
// ;---------------------------------------;
#define CSI_BASE_ADDR           IO_ADDRESS(0x00224000)
#define CSI_CTRL_REG1           (CSI_BASE_ADDR+0x00)
#define CSI_CTRL_REG2           (CSI_BASE_ADDR+0x04)
#define CSI_STS_REG             (CSI_BASE_ADDR+0x08)
#define CSI_STAT_FIFO           (CSI_BASE_ADDR+0x0C)
#define CSI_RX_FIFO             (CSI_BASE_ADDR+0x10)
//
// ;---------------------------------------;
// ; CSPI                                  ;
// ; $0021_3000 to $0021_3FFF              ;
// ;---------------------------------------;
#define CSPI_BASE_ADDR          IO_ADDRESS(0x00213000)
#define CSPI_SPIRXD             CSPI_BASE_ADDR
#define CSPI_SPITXD             (CSPI_BASE_ADDR+0x04)
#define CSPI_SPICONT1           (CSPI_BASE_ADDR+0x08)
#define CSPI_INTCS              (CSPI_BASE_ADDR+0x0C)
#define CSPI_SPITEST            (CSPI_BASE_ADDR+0x10)
#define CSPI_SPISPCR            (CSPI_BASE_ADDR+0x14)
#define CSPI_SPIDMA             (CSPI_BASE_ADDR+0x18)
#define CSPI_SPIRESET           (CSPI_BASE_ADDR+0x1C)
//
// ;---------------------------------------;
// ; DMA                                   ;
// ; $0020_9000 to $0020_9FFF              ;
// ;---------------------------------------;
#define DMA_BASE_ADDR           IO_ADDRESS(0x00209000)                
#define DMA_SYS_BASE            (DMA_BASE_ADDR+0x000)
#define DMA_M2D_BASE            (DMA_BASE_ADDR+0x040)
#define DMA_CH0_BASE            (DMA_BASE_ADDR+0x080)
#define DMA_CH1_BASE            (DMA_BASE_ADDR+0x0C0)
#define DMA_CH2_BASE            (DMA_BASE_ADDR+0x100)
#define DMA_CH3_BASE            (DMA_BASE_ADDR+0x140)
#define DMA_CH4_BASE            (DMA_BASE_ADDR+0x180)
#define DMA_CH5_BASE            (DMA_BASE_ADDR+0x1C0)
#define DMA_CH6_BASE            (DMA_BASE_ADDR+0x200)
#define DMA_CH7_BASE            (DMA_BASE_ADDR+0x240)
#define DMA_CH8_BASE            (DMA_BASE_ADDR+0x280)
#define DMA_CH9_BASE            (DMA_BASE_ADDR+0x2C0)
#define DMA_CH10_BASE           (DMA_BASE_ADDR+0x300)
#define DMA_TST_BASE            (DMA_BASE_ADDR+0x340)
//
#define DMA_DCR                 (DMA_SYS_BASE+0x000)
#define DMA_ISR                 (DMA_SYS_BASE+0x004)
#define DMA_IMR                 (DMA_SYS_BASE+0x008)
#define DMA_BTOSR               (DMA_SYS_BASE+0x00C)
#define DMA_RTOSR               (DMA_SYS_BASE+0x010)
#define DMA_TESR                (DMA_SYS_BASE+0x014)
#define DMA_BOSR                (DMA_SYS_BASE+0x018)
#define DMA_BTOCR               (DMA_SYS_BASE+0x01C)
//
#define DMA_WSRA                (DMA_M2D_BASE+0x000)
#define DMA_XSRA                (DMA_M2D_BASE+0x004)
#define DMA_YSRA                (DMA_M2D_BASE+0x008)
#define DMA_WSRB                (DMA_M2D_BASE+0x00C)
#define DMA_XSRB                (DMA_M2D_BASE+0x010)
#define DMA_YSRB                (DMA_M2D_BASE+0x014)
//
#define DMA_SAR0                (DMA_CH0_BASE+0x000)
#define DMA_DAR0                (DMA_CH0_BASE+0x004)
#define DMA_CNTR0               (DMA_CH0_BASE+0x008)
#define DMA_CCR0                (DMA_CH0_BASE+0x00C)
#define DMA_RSSR0               (DMA_CH0_BASE+0x010)
#define DMA_BLR0                (DMA_CH0_BASE+0x014)
#define DMA_RTOR0               (DMA_CH0_BASE+0x018)
#define DMA_BUCR0               (DMA_CH0_BASE+0x018)
//
#define DMA_SAR1                (DMA_CH1_BASE+0x000)
#define DMA_DAR1                (DMA_CH1_BASE+0x004)
#define DMA_CNTR1               (DMA_CH1_BASE+0x008)
#define DMA_CCR1                (DMA_CH1_BASE+0x00C)
#define DMA_RSSR1               (DMA_CH1_BASE+0x010)
#define DMA_BLR1                (DMA_CH1_BASE+0x014)
#define DMA_RTOR1               (DMA_CH1_BASE+0x018)
#define DMA_BUCR1               (DMA_CH1_BASE+0x018)
//
#define DMA_SAR2                (DMA_CH2_BASE+0x000)
#define DMA_DAR2                (DMA_CH2_BASE+0x004)
#define DMA_CNTR2               (DMA_CH2_BASE+0x008)
#define DMA_CCR2                (DMA_CH2_BASE+0x00C)
#define DMA_RSSR2               (DMA_CH2_BASE+0x010)
#define DMA_BLR2                (DMA_CH2_BASE+0x014)
#define DMA_RTOR2               (DMA_CH2_BASE+0x018)
#define DMA_BUCR2               (DMA_CH2_BASE+0x018)
//
#define DMA_SAR3                (DMA_CH3_BASE+0x000)
#define DMA_DAR3                (DMA_CH3_BASE+0x004)
#define DMA_CNTR3               (DMA_CH3_BASE+0x008)
#define DMA_CCR3                (DMA_CH3_BASE+0x00C)
#define DMA_RSSR3               (DMA_CH3_BASE+0x010)
#define DMA_BLR3                (DMA_CH3_BASE+0x014)
#define DMA_RTOR3               (DMA_CH3_BASE+0x018)
#define DMA_BUCR3               (DMA_CH3_BASE+0x018)
//
#define DMA_SAR4                (DMA_CH4_BASE+0x000)
#define DMA_DAR4                (DMA_CH4_BASE+0x004)
#define DMA_CNTR4               (DMA_CH4_BASE+0x008)
#define DMA_CCR4                (DMA_CH4_BASE+0x00C)
#define DMA_RSSR4               (DMA_CH4_BASE+0x010)
#define DMA_BLR4                (DMA_CH4_BASE+0x014)
#define DMA_RTOR4               (DMA_CH4_BASE+0x018)
#define DMA_BUCR4               (DMA_CH4_BASE+0x018)
//
#define DMA_SAR5                (DMA_CH5_BASE+0x000)
#define DMA_DAR5                (DMA_CH5_BASE+0x004)
#define DMA_CNTR5               (DMA_CH5_BASE+0x008)
#define DMA_CCR5                (DMA_CH5_BASE+0x00C)
#define DMA_RSSR5               (DMA_CH5_BASE+0x010)
#define DMA_BLR5                (DMA_CH5_BASE+0x014)
#define DMA_RTOR5               (DMA_CH5_BASE+0x018)
#define DMA_BUCR5               (DMA_CH5_BASE+0x018)
//
#define DMA_SAR6                (DMA_CH6_BASE+0x000)
#define DMA_DAR6                (DMA_CH6_BASE+0x004)
#define DMA_CNTR6               (DMA_CH6_BASE+0x008)
#define DMA_CCR6                (DMA_CH6_BASE+0x00C)
#define DMA_RSSR6               (DMA_CH6_BASE+0x010)
#define DMA_BLR6                (DMA_CH6_BASE+0x014)
#define DMA_RTOR6               (DMA_CH6_BASE+0x018)
#define DMA_BUCR6               (DMA_CH6_BASE+0x018)
//
#define DMA_SAR7                (DMA_CH7_BASE+0x000)
#define DMA_DAR7                (DMA_CH7_BASE+0x004)
#define DMA_CNTR7               (DMA_CH7_BASE+0x008)
#define DMA_CCR7                (DMA_CH7_BASE+0x00C)
#define DMA_RSSR7               (DMA_CH7_BASE+0x010)
#define DMA_BLR7                (DMA_CH7_BASE+0x014)
#define DMA_RTOR7               (DMA_CH7_BASE+0x018)
#define DMA_BUCR7               (DMA_CH7_BASE+0x018)
//
#define DMA_SAR8                (DMA_CH8_BASE+0x000)
#define DMA_DAR8                (DMA_CH8_BASE+0x004)
#define DMA_CNTR8               (DMA_CH8_BASE+0x008)
#define DMA_CCR8                (DMA_CH8_BASE+0x00C)
#define DMA_RSSR8               (DMA_CH8_BASE+0x010)
#define DMA_BLR8                (DMA_CH8_BASE+0x014)
#define DMA_RTOR8               (DMA_CH8_BASE+0x018)
#define DMA_BUCR8               (DMA_CH8_BASE+0x018)
//
#define DMA_SAR9                (DMA_CH9_BASE+0x000)
#define DMA_DAR9                (DMA_CH9_BASE+0x004)
#define DMA_CNTR9               (DMA_CH9_BASE+0x008)
#define DMA_CCR9                (DMA_CH9_BASE+0x00C)
#define DMA_RSSR9               (DMA_CH9_BASE+0x010)
#define DMA_BLR9                (DMA_CH9_BASE+0x014)
#define DMA_RTOR9               (DMA_CH9_BASE+0x018)
#define DMA_BUCR9               (DMA_CH9_BASE+0x018)
//
#define DMA_SAR10               (DMA_CH10_BASE+0x000)
#define DMA_DAR10               (DMA_CH10_BASE+0x004)
#define DMA_CNTR10              (DMA_CH10_BASE+0x008)
#define DMA_CCR10               (DMA_CH10_BASE+0x00C)
#define DMA_RSSR10              (DMA_CH10_BASE+0x010)
#define DMA_BLR10               (DMA_CH10_BASE+0x014)
#define DMA_RTOR10              (DMA_CH10_BASE+0x018)
#define DMA_BUCR10              (DMA_CH10_BASE+0x018)
//                              
#define DMA_TCR                 (DMA_TST_BASE+0x000)
#define DMA_TFIFOA              (DMA_TST_BASE+0x004)
#define DMA_TDRR                (DMA_TST_BASE+0x008)
#define DMA_TDIPR               (DMA_TST_BASE+0x00C)
#define DMA_TFIFOB              (DMA_TST_BASE+0x010)
//                              
// ;---------------------------------------;
// ; DSPA                                  ;
// ; $0022_2000 to $0022_2FFF              ;
// ;---------------------------------------;
#define DSPA_BASE_ADDR          IO_ADDRESS(0x00222000)
//                              
#define DSPA_MAC_MOD            (DSPA_BASE_ADDR+0x0000)
#define DSPA_MAC_CTRL           (DSPA_BASE_ADDR+0x0004)
#define DSPA_MAC_MULT           (DSPA_BASE_ADDR+0x0008)
#define DSPA_MAC_ACCU           (DSPA_BASE_ADDR+0x000C)
#define DSPA_MAC_INTR           (DSPA_BASE_ADDR+0x0010)
#define DSPA_MAC_INTR_MASK      (DSPA_BASE_ADDR+0x0014)
#define DSPA_MAC_FIFO           (DSPA_BASE_ADDR+0x0018)
#define DSPA_MAC_FIFO_STAT      (DSPA_BASE_ADDR+0x001C)
#define DSPA_MAC_BURST          (DSPA_BASE_ADDR+0x0020)
#define DSPA_MAC_BIT_SEL        (DSPA_BASE_ADDR+0x0024)
//
#define DSPA_MAC_XBASE          (DSPA_BASE_ADDR+0x0200)
#define DSPA_MAC_XINDEX         (DSPA_BASE_ADDR+0x0204)
#define DSPA_MAC_XLENGTH        (DSPA_BASE_ADDR+0x0208)
#define DSPA_MAC_XMODIFY        (DSPA_BASE_ADDR+0x020C)
#define DSPA_MAC_XINCR          (DSPA_BASE_ADDR+0x0210)
#define DSPA_MAC_XCOUNT         (DSPA_BASE_ADDR+0x0214)
//
#define DSPA_MAC_YBASE          (DSPA_BASE_ADDR+0x0300)
#define DSPA_MAC_YINDEX         (DSPA_BASE_ADDR+0x0304)
#define DSPA_MAC_YLENGTH        (DSPA_BASE_ADDR+0x0308)
#define DSPA_MAC_YMODIFY        (DSPA_BASE_ADDR+0x030C)
#define DSPA_MAC_YINCR          (DSPA_BASE_ADDR+0x0310)
#define DSPA_MAC_YCOUNT         (DSPA_BASE_ADDR+0x0314)
//
#define DSPA_DCTCTRL            (DSPA_BASE_ADDR+0x0400)
#define DSPA_DCTVER             (DSPA_BASE_ADDR+0x0404)
#define DSPA_DCTIRQENA          (DSPA_BASE_ADDR+0x0408)
#define DSPA_DCTIRQSTAT         (DSPA_BASE_ADDR+0x040C)
#define DSPA_DCTSRCADD          (DSPA_BASE_ADDR+0x0410)
#define DSPA_DCTDESADD          (DSPA_BASE_ADDR+0x0414)
#define DSPA_DCTFIFO            (DSPA_BASE_ADDR+0x0500)
//
// ;---------------------------------------;
// ; ESRAM                                 ;
// ; $0030_D000 to $0031_FFFF              ;
// ;---------------------------------------;
#define eSRAM_ADDR_BOT          0x00300000      // ; Bottom of eSRAM
#define eSRAM_ADDR_TOP          0x00319000      // ; Top of physical eSRAM
#define eSRAM_ADDR_LMT          0x003FFFFF      // ; Limit of allocated eSRAM
#define eSRAM_PHY_SIZE          0x00018000      // ; Physical size of eSRAM (96k)
#define eSRAM_ASS_SIZE          0x00100000      // ; Assigned size of eSRAM (1M)
#define eSRAM_8K_SIZE           0x00008000      // ; 8k byte size for boundary between RAM1 & RAM2
//
// ;---------------------------------------;
// ; GPIO - PTA                            ;
// ; $0021_C000 to $0021_C0FF              ;
// ;---------------------------------------;
#define PTA_BASE_ADDR           IO_ADDRESS(0x0021C000)                
#define PTA_DDIR                PTA_BASE_ADDR                
#define PTA_OCR1                (PTA_BASE_ADDR+0x04)                
#define PTA_OCR2                (PTA_BASE_ADDR+0x08)                
#define PTA_ICONFA1             (PTA_BASE_ADDR+0x0C)                
#define PTA_ICONFA2             (PTA_BASE_ADDR+0x10)                
#define PTA_ICONFB1             (PTA_BASE_ADDR+0x14)                
#define PTA_ICONFB2             (PTA_BASE_ADDR+0x18)                
#define PTA_DR                  (PTA_BASE_ADDR+0x1C)                
#define PTA_GIUS                (PTA_BASE_ADDR+0x20)                
#define PTA_SSR                 (PTA_BASE_ADDR+0x24)                
#define PTA_ICR1                (PTA_BASE_ADDR+0x28)                
#define PTA_ICR2                (PTA_BASE_ADDR+0x2C)                
#define PTA_IMR                 (PTA_BASE_ADDR+0x30)                
#define PTA_ISR                 (PTA_BASE_ADDR+0x34)                
#define PTA_GPR                 (PTA_BASE_ADDR+0x38)                
#define PTA_SWR                 (PTA_BASE_ADDR+0x3C)                
#define PTA_PUEN                (PTA_BASE_ADDR+0x40)  
//
// ;---------------------------------------;
// ; GPIO - PTB                            ;       
// ; $0021_C100 to $0021_C1FF              ;       
// ;---------------------------------------;       
#define PTB_BASE_ADDR           IO_ADDRESS(0x0021C100)                
#define PTB_DDIR                PTB_BASE_ADDR                
#define PTB_OCR1                (PTB_BASE_ADDR+0x04)                
#define PTB_OCR2                (PTB_BASE_ADDR+0x08)                
#define PTB_ICONFA1             (PTB_BASE_ADDR+0x0C)                
#define PTB_ICONFA2             (PTB_BASE_ADDR+0x10)                
#define PTB_ICONFB1             (PTB_BASE_ADDR+0x14)                
#define PTB_ICONFB2             (PTB_BASE_ADDR+0x18)                
#define PTB_DR                  (PTB_BASE_ADDR+0x1C)                
#define PTB_GIUS                (PTB_BASE_ADDR+0x20)                
#define PTB_SSR                 (PTB_BASE_ADDR+0x24)                
#define PTB_ICR1                (PTB_BASE_ADDR+0x28)                
#define PTB_ICR2                (PTB_BASE_ADDR+0x2C)                
#define PTB_IMR                 (PTB_BASE_ADDR+0x30)                
#define PTB_ISR                 (PTB_BASE_ADDR+0x34)                
#define PTB_GPR                 (PTB_BASE_ADDR+0x38)                
#define PTB_SWR                 (PTB_BASE_ADDR+0x3C)                
#define PTB_PUEN                (PTB_BASE_ADDR+0x40)

// ;---------------------------------------;
// ; GPIO - PTC                            ;
// ; $0021_C200 to $0021_C2FF              ;
// ;---------------------------------------;
#define PTC_BASE_ADDR           IO_ADDRESS(0x0021C200)               
#define PTC_DDIR                PTC_BASE_ADDR                
#define PTC_OCR1                (PTC_BASE_ADDR+0x04)                
#define PTC_OCR2                (PTC_BASE_ADDR+0x08)                
#define PTC_ICONFA1             (PTC_BASE_ADDR+0x0C)                
#define PTC_ICONFA2             (PTC_BASE_ADDR+0x10)                
#define PTC_ICONFB1             (PTC_BASE_ADDR+0x14)                
#define PTC_ICONFB2             (PTC_BASE_ADDR+0x18)                
#define PTC_DR                  (PTC_BASE_ADDR+0x1C)                
#define PTC_GIUS                (PTC_BASE_ADDR+0x20)                
#define PTC_SSR                 (PTC_BASE_ADDR+0x24)                
#define PTC_ICR1                (PTC_BASE_ADDR+0x28)                
#define PTC_ICR2                (PTC_BASE_ADDR+0x2C)                
#define PTC_IMR                 (PTC_BASE_ADDR+0x30)                
#define PTC_ISR                 (PTC_BASE_ADDR+0x34)                
#define PTC_GPR                 (PTC_BASE_ADDR+0x38)                
#define PTC_SWR                 (PTC_BASE_ADDR+0x3C)                
#define PTC_PUEN                (PTC_BASE_ADDR+0x40)                

// ;---------------------------------------;
// ; GPIO - PTD                            ;
// ; $0021_C300 to $0021_C3FF              ;
// ;---------------------------------------;
#define PTD_BASE_ADDR           IO_ADDRESS(0x0021C300)                
#define PTD_DDIR                PTD_BASE_ADDR                
#define PTD_OCR1                (PTD_BASE_ADDR+0x04)                
#define PTD_OCR2                (PTD_BASE_ADDR+0x08)                
#define PTD_ICONFA1             (PTD_BASE_ADDR+0x0C)                
#define PTD_ICONFA2             (PTD_BASE_ADDR+0x10)                
#define PTD_ICONFB1             (PTD_BASE_ADDR+0x14)                
#define PTD_ICONFB2             (PTD_BASE_ADDR+0x18)                
#define PTD_DR                  (PTD_BASE_ADDR+0x1C)                
#define PTD_GIUS                (PTD_BASE_ADDR+0x20)                
#define PTD_SSR                 (PTD_BASE_ADDR+0x24)                
#define PTD_ICR1                (PTD_BASE_ADDR+0x28)                
#define PTD_ICR2                (PTD_BASE_ADDR+0x2C)                
#define PTD_IMR                 (PTD_BASE_ADDR+0x30)                
#define PTD_ISR                 (PTD_BASE_ADDR+0x34)                
#define PTD_GPR                 (PTD_BASE_ADDR+0x38)                
#define PTD_SWR                 (PTD_BASE_ADDR+0x3C)                
#define PTD_PUEN                (PTD_BASE_ADDR+0x40) 
//
// ;---------------------------------------;
// ; LCDC                                  ;
// ; $0020_5000 to $0020_5FFF              ;
// ;---------------------------------------;
#define LCDC_BASE_ADDR          IO_ADDRESS(0x00205000)                
#define LCDC_PAL_ADDR           IO_ADDRESS(0x00205800)               
#define LCDC_ssa                LCDC_BASE_ADDR                
#define LCDC_xymax              (LCDC_BASE_ADDR+0x04)                
#define LCDC_vpw                (LCDC_BASE_ADDR+0x08)                
#define LCDC_hcc_xy             (LCDC_BASE_ADDR+0x0C)                
#define LCDC_hcc_w              (LCDC_BASE_ADDR+0x10)                
#define LCDC_chcc               (LCDC_BASE_ADDR+0x14)                
#define LCDC_con                (LCDC_BASE_ADDR+0x18)                
#define LCDC_hsyn               (LCDC_BASE_ADDR+0x1C)                
#define LCDC_vsyn               (LCDC_BASE_ADDR+0x20)                
#define LCDC_pan                (LCDC_BASE_ADDR+0x24)                
#define LCDC_gpm                (LCDC_BASE_ADDR+0x28)                
#define LCDC_pwm                (LCDC_BASE_ADDR+0x2C)                
#define LCDC_dma                (LCDC_BASE_ADDR+0x30)                
#define LCDC_self               (LCDC_BASE_ADDR+0x34)                
#define LCDC_int                (LCDC_BASE_ADDR+0x38)                
#define LCDC_status             (LCDC_BASE_ADDR+0x40)                
#define LCDC_ram_0              LCDC_PAL_ADDR
#define LCDC_ram_1              (LCDC_PAL_ADDR+0x04)                
#define LCDC_ram_2              (LCDC_PAL_ADDR+0x08)                
#define LCDC_ram_3              (LCDC_PAL_ADDR+0x0C)                
#define LCDC_ram_4              (LCDC_PAL_ADDR+0x10)                
#define LCDC_ram_5              (LCDC_PAL_ADDR+0x14)                
#define LCDC_ram_6              (LCDC_PAL_ADDR+0x18)                
#define LCDC_ram_7              (LCDC_PAL_ADDR+0x1C)                
#define LCDC_ram_8              (LCDC_PAL_ADDR+0x20)                
#define LCDC_ram_9              (LCDC_PAL_ADDR+0x24)                
#define LCDC_ram_a              (LCDC_PAL_ADDR+0x28)                
#define LCDC_ram_b              (LCDC_PAL_ADDR+0x2C)                
#define LCDC_ram_c              (LCDC_PAL_ADDR+0x30)                
#define LCDC_ram_d              (LCDC_PAL_ADDR+0x34)                
#define LCDC_ram_e              (LCDC_PAL_ADDR+0x38)                
#define LCDC_ram_f              (LCDC_PAL_ADDR+0x3c) 
//
// ;---------------------------------------;
// ; I2C                                   ;
// ; $0021_7000 to $0021_7FFF              ;
// ;---------------------------------------;
//
#define I2C_BASE_ADDR           IO_ADDRESS(0x00217000) 
#define I2C_IADR                I2C_BASE_ADDR 
#define I2C_IFDR                (I2C_BASE_ADDR+0x04)
#define I2C_I2CR                (I2C_BASE_ADDR+0x08)
#define I2C_I2SR                (I2C_BASE_ADDR+0x0C) 
#define I2C_I2DR                (I2C_BASE_ADDR+0x10) 
//
// ;---------------------------------------;
// ; SSI                                   ;
// ; $0021_8000 to $0021_8FFF              ;
// ;---------------------------------------;
#define SSI_BASE_ADDR           IO_ADDRESS(0x00218000)                
#define SSI_STX                 SSI_BASE_ADDR                
#define SSI_SRX                 (SSI_BASE_ADDR+0x04)                
#define SSI_SCSR                (SSI_BASE_ADDR+0x08) 
#define SSI_STCR                (SSI_BASE_ADDR+0x0C)   
#define SSI_SRCR                (SSI_BASE_ADDR+0x10)               
#define SSI_STCCR               (SSI_BASE_ADDR+0x14)                
#define SSI_SRCCR               (SSI_BASE_ADDR+0x18)                
#define SSI_STSR                (SSI_BASE_ADDR+0x1C)               
#define SSI_SFCSR               (SSI_BASE_ADDR+0x20)                
#define SSI_STR                 (SSI_BASE_ADDR+0x24)                
#define SSI_SOR                 (SSI_BASE_ADDR+0x28)                    
//
// ;---------------------------------------;
// ; MMC all the address used for SDHC     ;
// ; $0021_4000 to $0021_4FFF              ;
// ;---------------------------------------;
//
#define MMC_BASE_ADDR           IO_ADDRESS(0x00214000)                
#define MMC_STR_STP_CLK         MMC_BASE_ADDR                
#define MMC_STATUS              (MMC_BASE_ADDR+0x04)
#define MMC_CLK_RATE            (MMC_BASE_ADDR+0x08)
#define MMC_CMD_DAT_CONT        (MMC_BASE_ADDR+0x0C)
#define MMC_RESPONSE_TO         (MMC_BASE_ADDR+0x10)
#define MMC_READ_TO             (MMC_BASE_ADDR+0x14)                
#define MMC_BLK_LEN             (MMC_BASE_ADDR+0x18)                
#define MMC_NOB                 (MMC_BASE_ADDR+0x1C)                
#define MMC_REV_NO              (MMC_BASE_ADDR+0x20)                
#define MMC_INT_MASK            (MMC_BASE_ADDR+0x24)
#define MMC_CMD                 (MMC_BASE_ADDR+0x28)
#define MMC_ARGH                (MMC_BASE_ADDR+0x2C)
#define MMC_ARGL                (MMC_BASE_ADDR+0x30)
#define MMC_RES_FIFO            (MMC_BASE_ADDR+0x34)                
#define MMC_BUFFER_ACCESS       (MMC_BASE_ADDR+0x38)                
#define MMC_BUF_PART_FULL       (MMC_BASE_ADDR+0x3C)  
#define MMC_OTHERS_REGS_1       (MMC_BASE_ADDR+0x40)  
#define MMC_OTHERS_REGS_2       (MMC_BASE_ADDR+0x80)  
#define MMC_OTHERS_REGS_3       (MMC_BASE_ADDR+0x100)  
#define MMC_OTHERS_REGS_4       (MMC_BASE_ADDR+0x200)  
#define MMC_OTHERS_REGS_5       (MMC_BASE_ADDR+0x400)  
#define MMC_OTHERS_REGS_6       (MMC_BASE_ADDR+0x800)  
//
// ;---------------------------------------;
// ; MSHC                                  ;
// ; $0021_A000 to $0021_AFFF              ;
// ;---------------------------------------;
#define MSHC_BASE_ADDR          IO_ADDRESS(0x0021A000)
#define MSHC_MSCMD              MSHC_BASE_ADDR                  // ; Command Reg
#define MSHC_MSCS               (MSHC_BASE_ADDR+0x02)   // ; Control/Status Reg
#define MSHC_MSDATA             (MSHC_BASE_ADDR+0x04)   // ; Tx/Rx FIFO Reg
#define MSHC_MSICS              (MSHC_BASE_ADDR+0x06)   // ; Interrupt Control/Status Reg
#define MSHC_MSPPCD             (MSHC_BASE_ADDR+0x08)   // ; Parallel Port Control/Data Reg
#define MSHC_MSC2               (MSHC_BASE_ADDR+0x0A)   // ; Control2 Reg
#define MSHC_MSACMD             (MSHC_BASE_ADDR+0x0C)   // ; Auto Command Reg
#define MSHC_MSFAECS            (MSHC_BASE_ADDR+0x0E)   // ; FIFO Acc Err Ctrl/Status Reg
#define MSHC_MSCLKD             (MSHC_BASE_ADDR+0x10)   // ; Serial Clock Div Control Reg
#define MSHC_MSDRQC             (MSHC_BASE_ADDR+0x12)   // ; DMA Request Control Reg
//
// ;---------------------------------------;
// ; PORTHOLE                              ;
// ;---------------------------------------;
#define EVENT_PORTHOLE          0x12301000  // ; User Event Porthole
#define EVENT_PH                0x12301000
#define EVENT                   0x12301000
#define PASS_PORTHOLE           0x12301004
#define PASS_PH                 0x12301004
#define FAIL_PORTHOLE           0x12301008
#define FAIL_PH                 0x12301008
#define STOP_PORTHOLE           0x1230100C
#define STOP_PH                 0x1230100C
#define FINISH_PORTHOLE         0x12301010
#define FINISH_PH               0x12301010
//
#define EXP_DATA_PH             0x12301014
#define CODE_DATA_PH            0x12301018
#define ADDR_DATA_PH            0x1230101C
#define ACT_DATA_PH             0x12301020
//
#define EVENT_VALUE             0x0000beef  // ; Code execution complete trigger
#define USER_EVENT0_DATA        0xbadbeef0
#define USER_EVENT1_DATA        0xbadbeef1
#define USER_EVENT2_DATA        0xbadbeef2
#define USER_EVENT3_DATA        0xbadbeef3
#define USER_EVENT4_DATA        0xbadbeef4
#define USER_EVENT5_DATA        0xbadbeef5
#define USER_EVENT6_DATA        0xbadbeef6
#define USER_EVENT7_DATA        0xbadbeef7
#define USER_EVENT8_DATA        0xbadbeef8
#define USER_EVENT9_DATA        0xbadbeef9
#define USER_EVENT10_DATA       0xbadbeefa
#define USER_EVENT11_DATA       0xbadbeefb
#define USER_EVENT12_DATA       0xbadbeefc
#define USER_EVENT13_DATA       0xbadbeefd
#define USER_EVENT14_DATA       0xbadbeefe
#define USER_EVENT15_DATA       0xbadbeeff
#define MCU_JNT_HALT            0xdeaddead
//
// ;---------------------------------------;
// ; PWM                                   ;
// ; $0020_8000 to $0020_8FFF              ;
// ;---------------------------------------;
#define PWM1_BASE_ADDR          IO_ADDRESS(0x00208000)                
#define PWMC1                   PWM1_BASE_ADDR                
#define PWMS1                   (PWM1_BASE_ADDR+0x04)                
#define PWMP1                   (PWM1_BASE_ADDR+0x08)                
#define PWMCNT1                 (PWM1_BASE_ADDR+0x0C)                
#define PWMTST1                 (PWM1_BASE_ADDR+0x10)                

// ;---------------------------------------;
// ; RAM                                        ;
// ; $0030_0100 to $0031_FFFF              ;
// ; $0030_0000 to $0030_00FF for PORTHOLE ;
// ;---------------------------------------;
#define RAM1_BASE_ADDR          0x00300100
#define RAM1_END_ADDR           (RAM1_BASE_ADDR+0x0FF00)
#define RAM2_BASE_ADDR          0x00310000
#define RAM2_END_ADDR           (RAM2_BASE_ADDR+0x10000)
#define TOP_OF_STACK            0x00320000
//
// ;---------------------------------------;
// ; RTC                                   ;
// ; $0020_4000 to $0020_4FFF              ;
// ;---------------------------------------;
#define RTC_BASE_ADDR           IO_ADDRESS(0x00204000)                
#define RTC_HOURMIN             RTC_BASE_ADDR                
#define RTC_SECOND              (RTC_BASE_ADDR+0x04)                
#define RTC_ALRM_HM             (RTC_BASE_ADDR+0x08)                
#define RTC_ALRM_SEC            (RTC_BASE_ADDR+0x0C)                
#define RTC_RTCCTL              (RTC_BASE_ADDR+0x10)                
#define RTC_RTCISR              (RTC_BASE_ADDR+0x14)                
#define RTC_RTCIENR             (RTC_BASE_ADDR+0x18)                
#define RTC_STPWCH              (RTC_BASE_ADDR+0x1C)                
#define RTC_DAYR                (RTC_BASE_ADDR+0x20)                
#define RTC_DAYALARM            (RTC_BASE_ADDR+0x24)                
#define RTC_TEST1               (RTC_BASE_ADDR+0x28)                
#define RTC_TEST2               (RTC_BASE_ADDR+0x2C)                
#define RTC_TEST3               (RTC_BASE_ADDR+0x30)
//
// ;---------------------------------------;
// ; SRAMC                                 ;
// ; $0022_1000 to $0022_1FFF              ;
// ;---------------------------------------;
#define SDRAMC_BASE_ADDR        IO_ADDRESS(0x00221000)                
#define SDRAMC_SDCTL0           SDRAMC_BASE_ADDR                
#define SDRAMC_SDCTL1           (SDRAMC_BASE_ADDR+0x04)              
//
// ;---------------------------------------;
// ; SIM                                   ;
// ; $0021_1000 to $0021_21FF              ;
// ;---------------------------------------;
#define SIM_BASE_ADDR           IO_ADDRESS(0x00211000)                
#define SIM_PORT_CNTL           SIM_BASE_ADDR                
#define SIM_CNTL                SIM_BASE_ADDR+0x04)                
#define SIM_RCV_THRESHOLD       SIM_BASE_ADDR+0x08)                
#define SIM_ENABLE              SIM_BASE_ADDR+0x0C)                
#define SIM_XMT_STATUS          (SIM_BASE_ADDR+0x10)                
#define SIM_RCV_STATUS          (SIM_BASE_ADDR+0x14)                
#define SIM_INT_MASK            (SIM_BASE_ADDR+0x18)                
#define SIM_PORT_XMT_BUF        (SIM_BASE_ADDR+0x1C)                
#define SIM_PORT_RCV_BUF        (SIM_BASE_ADDR+0x20)                
#define SIM_PORT_DETECT         (SIM_BASE_ADDR+0x24)                
#define SIM_XMT_THRESHOLD       (SIM_BASE_ADDR+0x28)               
#define SIM_GUARD_CNTL          (SIM_BASE_ADDR+0x2C)                
#define SIM_OD_CONFIG           (SIM_BASE_ADDR+0x30)                
#define SIM_RESET_CNTL          (SIM_BASE_ADDR+0x34)               
#define SIM_CHAR_WAIT           (SIM_BASE_ADDR+0x38)               
#define SIM_GPCNT               (SIM_BASE_ADDR+0x3C)               
#define SIM_DIVISOR             (SIM_BASE_ADDR+0x40)   
//
// ;---------------------------------------;
// ; TIMER1                                ;
// ; $0020_2000 to $0020_2FFF              ;
// ;---------------------------------------;
#define TIMER1_BASE_ADDR        IO_ADDRESS(0x00202000)                
#define TIMER1_TCTL1            TIMER1_BASE_ADDR                
#define TIMER1_TPRER1           (TIMER1_BASE_ADDR+0x04)                
#define TIMER1_TCMP1            (TIMER1_BASE_ADDR+0x08)                
#define TIMER1_TCR1             (TIMER1_BASE_ADDR+0x0C)                
#define TIMER1_TCN1             (TIMER1_BASE_ADDR+0x10)                
#define TIMER1_TSTAT1           (TIMER1_BASE_ADDR+0x14)                

// ;---------------------------------------;
// ; TIMER2                                ;
// ; $0020_3000 to $0020_3FFF              ;
// ;---------------------------------------;
#define TIMER2_BASE_ADDR        IO_ADDRESS(0x00203000)                
#define TIMER2_TCTL2            TIMER2_BASE_ADDR                
#define TIMER2_TPRER2           (TIMER2_BASE_ADDR+0x04)                
#define TIMER2_TCMP2            (TIMER2_BASE_ADDR+0x08)                
#define TIMER2_TCR2             (TIMER2_BASE_ADDR+0x0C)                
#define TIMER2_TCN2             (TIMER2_BASE_ADDR+0x10)                
#define TIMER2_TSTAT2           (TIMER2_BASE_ADDR+0x14)                
//
// ;---------------------------------------;
// ; TUBE                                  ;
// ; CS5 address space                     ;
// ; $1600_0000 to $16FF_FFFF              ;
// ;---------------------------------------;
#define TubeBase                0x16000000
#define TUBEDataOff             0               // ; Register offsets
#define TUBEDataMask            15              // ; Data
#define CR                      13              // ;Carriage return char
#define LF                      10              // ;Line Feed char
#define CTRLD                   4               // ;Control D char
//
// ;---------------------------------------;
// ; UART1                                 ;
// ; $0020_6000 to $0020_6FFF              ;
// ;---------------------------------------;
#define UART1_BASE_ADDR         IO_ADDRESS(0x00206000)                
#define UART1_RXDATA            UART1_BASE_ADDR                
#define UART1_TXDATA            (UART1_BASE_ADDR+0x40)                
#define UART1_CR1               (UART1_BASE_ADDR+0x80)                
#define UART1_CR2               (UART1_BASE_ADDR+0x84)                
#define UART1_CR3               (UART1_BASE_ADDR+0x88)                
#define UART1_CR4               (UART1_BASE_ADDR+0x8C)                
#define UART1_FCR               (UART1_BASE_ADDR+0x90)                
#define UART1_SR1               (UART1_BASE_ADDR+0x94)                
#define UART1_SR2               (UART1_BASE_ADDR+0x98)                
#define UART1_ESC               (UART1_BASE_ADDR+0x9C)                
#define UART1_TIM               (UART1_BASE_ADDR+0xA0)                
#define UART1_BIR               (UART1_BASE_ADDR+0xA4)                
#define UART1_BMR               (UART1_BASE_ADDR+0xA8)                
#define UART1_BRC               (UART1_BASE_ADDR+0xAC)                
#define UART1_BIPR1             (UART1_BASE_ADDR+0xB0)                
#define UART1_BMPR1             (UART1_BASE_ADDR+0xB4)                
#define UART1_BIPR2             (UART1_BASE_ADDR+0xB8)                
#define UART1_BMPR2             (UART1_BASE_ADDR+0xBC)                
#define UART1_BIPR3             (UART1_BASE_ADDR+0xC0)                
#define UART1_BMPR3             (UART1_BASE_ADDR+0xC4)                
#define UART1_BIPR4             (UART1_BASE_ADDR+0xC8)                
#define UART1_BMPR4             (UART1_BASE_ADDR+0xCC)                
#define UART1_TS                (UART1_BASE_ADDR+0xD0)                
//
// ;---------------------------------------;
// ; UART2                                 ;
// ; $0020_7000 to $0020_7FFF              ;
// ;---------------------------------------;
#define UART2_BASE_ADDR         IO_ADDRESS(0x00207000)                
#define UART2_RXDATA            UART2_BASE_ADDR                
#define UART2_TXDATA            (UART2_BASE_ADDR+0x40)
#define UART2_CR1               (UART2_BASE_ADDR+0x80)                
#define UART2_CR2               (UART2_BASE_ADDR+0x84)                
#define UART2_CR3               (UART2_BASE_ADDR+0x88)                
#define UART2_CR4               (UART2_BASE_ADDR+0x8C)                
#define UART2_FCR               (UART2_BASE_ADDR+0x90)                
#define UART2_SR1               (UART2_BASE_ADDR+0x94)                
#define UART2_SR2               (UART2_BASE_ADDR+0x98)                
#define UART2_ESC               (UART2_BASE_ADDR+0x9C)                
#define UART2_TIM               (UART2_BASE_ADDR+0xA0)                
#define UART2_BIR               (UART2_BASE_ADDR+0xA4)                
#define UART2_BMR               (UART2_BASE_ADDR+0xA8)                
#define UART2_BRC               (UART2_BASE_ADDR+0xAC)                
#define UART2_BIPR1             (UART2_BASE_ADDR+0xB0)                
#define UART2_BMPR1             (UART2_BASE_ADDR+0xB4)                
#define UART2_BIPR2             (UART2_BASE_ADDR+0xB8)                
#define UART2_BMPR2             (UART2_BASE_ADDR+0xBC)                
#define UART2_BIPR3             (UART2_BASE_ADDR+0xC0)                
#define UART2_BMPR3             (UART2_BASE_ADDR+0xC4)                
#define UART2_BIPR4             (UART2_BASE_ADDR+0xC8)                
#define UART2_BMPR4             (UART2_BASE_ADDR+0xCC)                
#define UART2_TS                (UART2_BASE_ADDR+0xD0) 

// ;---------------------------------------;
// ; USBD                                  ;
// ; $0021_2000 to $0021_2FFF              ;
// ;---------------------------------------;
#define USBD_BASE_ADDR          IO_ADDRESS(0x00212000)                
#define USBD_FRAME              USBD_BASE_ADDR                
#define USBD_SPEC               (USBD_BASE_ADDR+0x04)                
#define USBD_STAT               (USBD_BASE_ADDR+0x08)                
#define USBD_CTRL               (USBD_BASE_ADDR+0x0C)                
#define USBD_DADR               (USBD_BASE_ADDR+0x10)                
#define USBD_DDAT               (USBD_BASE_ADDR+0x14)                
#define USBD_INTR               (USBD_BASE_ADDR+0x18)                
#define USBD_MASK               (USBD_BASE_ADDR+0x1C)                
#define USBD_MCTL               (USBD_BASE_ADDR+0x20)                
#define USBD_ENABLE             (USBD_BASE_ADDR+0x24)                
//
#define USBD_EP0_STAT           (USBD_BASE_ADDR+0x30)                
#define USBD_EP0_INTR           (USBD_BASE_ADDR+0x34)                
#define USBD_EP0_MASK           (USBD_BASE_ADDR+0x38)                
#define USBD_EP0_FDAT           (USBD_BASE_ADDR+0x3C)                
#define USBD_EP0_FSTAT          (USBD_BASE_ADDR+0x40)                
#define USBD_EP0_FCTRL          (USBD_BASE_ADDR+0x44)                
#define USBD_EP0_LFRP           (USBD_BASE_ADDR+0x48)                
#define USBD_EP0_LFWP           (USBD_BASE_ADDR+0x4C)                
#define USBD_EP0_FALRM          (USBD_BASE_ADDR+0x50)                
#define USBD_EP0_FRDP           (USBD_BASE_ADDR+0x54)                
#define USBD_EP0_FWDP           (USBD_BASE_ADDR+0x58)                
//
#define USBD_EP1_STAT           (USBD_BASE_ADDR+0x60)                
#define USBD_EP1_INTR           (USBD_BASE_ADDR+0x64)                
#define USBD_EP1_MASK           (USBD_BASE_ADDR+0x68)                
#define USBD_EP1_FDAT           (USBD_BASE_ADDR+0x6C)                
#define USBD_EP1_FSTAT          (USBD_BASE_ADDR+0x70)                
#define USBD_EP1_FCTRL          (USBD_BASE_ADDR+0x74)                
#define USBD_EP1_LFRP           (USBD_BASE_ADDR+0x78)                
#define USBD_EP1_LFWP           (USBD_BASE_ADDR+0x7C)                
#define USBD_EP1_FALRM          (USBD_BASE_ADDR+0x80)                
#define USBD_EP1_FRDP           (USBD_BASE_ADDR+0x84)                
#define USBD_EP1_FWDP           (USBD_BASE_ADDR+0x88)                
//
#define USBD_EP2_STAT           (USBD_BASE_ADDR+0x90)                
#define USBD_EP2_INTR           (USBD_BASE_ADDR+0x94)                
#define USBD_EP2_MASK           (USBD_BASE_ADDR+0x98)                
#define USBD_EP2_FDAT           (USBD_BASE_ADDR+0x9C)                
#define USBD_EP2_FSTAT          (USBD_BASE_ADDR+0xA0)                
#define USBD_EP2_FCTRL          (USBD_BASE_ADDR+0xA4)                
#define USBD_EP2_LFRP           (USBD_BASE_ADDR+0xA8)                
#define USBD_EP2_LFWP           (USBD_BASE_ADDR+0xAC)                
#define USBD_EP2_FALRM          (USBD_BASE_ADDR+0xB0)                
#define USBD_EP2_FRDP           (USBD_BASE_ADDR+0xB4)
#define USBD_EP2_FWDP           (USBD_BASE_ADDR+0xB8)
//                                                 
#define USBD_EP3_STAT           (USBD_BASE_ADDR+0xC0)                
#define USBD_EP3_INTR           (USBD_BASE_ADDR+0xC4)                
#define USBD_EP3_MASK           (USBD_BASE_ADDR+0xC8)                
#define USBD_EP3_FDAT           (USBD_BASE_ADDR+0xCC)                
#define USBD_EP3_FSTAT          (USBD_BASE_ADDR+0xD0)                
#define USBD_EP3_FCTRL          (USBD_BASE_ADDR+0xD4)                
#define USBD_EP3_LFRP           (USBD_BASE_ADDR+0xD8)                
#define USBD_EP3_LFWP           (USBD_BASE_ADDR+0xDC)                
#define USBD_EP3_FALRM          (USBD_BASE_ADDR+0xE0)                
#define USBD_EP3_FRDP           (USBD_BASE_ADDR+0xE4)                
#define USBD_EP3_FWDP           (USBD_BASE_ADDR+0xE8)                
//
#define USBD_EP4_STAT           (USBD_BASE_ADDR+0xF0)                
#define USBD_EP4_INTR           (USBD_BASE_ADDR+0xF4)                
#define USBD_EP4_MASK           (USBD_BASE_ADDR+0xF8)                
#define USBD_EP4_FDAT           (USBD_BASE_ADDR+0xFC)                
#define USBD_EP4_FSTAT          (USBD_BASE_ADDR+0x100)                
#define USBD_EP4_FCTRL          (USBD_BASE_ADDR+0x104)                
#define USBD_EP4_LFRP           (USBD_BASE_ADDR+0x108)                
#define USBD_EP4_LFWP           (USBD_BASE_ADDR+0x10C)                
#define USBD_EP4_FALRM          (USBD_BASE_ADDR+0x110)                
#define USBD_EP4_FRDP           (USBD_BASE_ADDR+0x114)                
#define USBD_EP4_FWDP           (USBD_BASE_ADDR+0x118)                
#define USBD_EP5_STAT           (USBD_BASE_ADDR+0x120)                
#define USBD_EP5_INTR           (USBD_BASE_ADDR+0x124)                
#define USBD_EP5_MASK           (USBD_BASE_ADDR+0x128)                
#define USBD_EP5_FDAT           (USBD_BASE_ADDR+0x12C)                
#define USBD_EP5_FSTAT          (USBD_BASE_ADDR+0x130)                
#define USBD_EP5_FCTRL          (USBD_BASE_ADDR+0x134)                
#define USBD_EP5_LFRP           (USBD_BASE_ADDR+0x138)                
#define USBD_EP5_LFWP           (USBD_BASE_ADDR+0x13C)                
#define USBD_EP5_FALRM          (USBD_BASE_ADDR+0x140)                
#define USBD_EP5_FRDP           (USBD_BASE_ADDR+0x144)                
#define USBD_EP5_FWDP           (USBD_BASE_ADDR+0x148)                
//
// ;---------------------------------------;
// ; CSPI_2                                ;
// ; $0021_9000 to $0021_9FFF              ;
// ;---------------------------------------;
#define CSPI_2_BASE_ADDR        IO_ADDRESS(0x00219000)
#define CSPI_2_SPIRXD           CSPI_2_BASE_ADDR
#define CSPI_2_SPITXD           (CSPI_2_BASE_ADDR+0x04) 
#define CSPI_2_SPICONT1         (CSPI_2_BASE_ADDR+0x08) 
#define CSPI_2_INTCS            (CSPI_2_BASE_ADDR+0x0C) 
#define CSPI_2_SPITEST          (CSPI_2_BASE_ADDR+0x10) 
#define CSPI_2_SPISPCR          (CSPI_2_BASE_ADDR+0x14) 
#define CSPI_2_SPIDMA           (CSPI_2_BASE_ADDR+0x18) 
#define CSPI_2_SPIRESET         (CSPI_2_BASE_ADDR+0x1C)
//
// ;---------------------------------------;
// ; WDT                                   ;
// ; $0020_1000 to $0020_1FFF              ;
// ;---------------------------------------;
#define WDOG_BASE_ADDR          IO_ADDRESS(0x00201000)                
#define WDOG_WCR                WDOG_BASE_ADDR                
#define WDOG_WSR                (WDOG_BASE_ADDR+0x04)                
#define WDOG_WSTR               (WDOG_BASE_ADDR+0x08)

// ;---------------------------------------;
// ; WEIM                                  ;
// ; $0022_0000 to $0022_0FFF              ;
// ;---------------------------------------;
#define EIM_BASE_ADDR           IO_ADDRESS(0x00220000)                
#define EIM_CS0H                EIM_BASE_ADDR                
#define EIM_CS0L                (EIM_BASE_ADDR+0x04)                
#define EIM_CS1H                (EIM_BASE_ADDR+0x08)                
#define EIM_CS1L                (EIM_BASE_ADDR+0x0C)                
#define EIM_CS2H                (EIM_BASE_ADDR+0x10)                
#define EIM_CS2L                (EIM_BASE_ADDR+0x14)                
#define EIM_CS3H                (EIM_BASE_ADDR+0x18)                
#define EIM_CS3L                (EIM_BASE_ADDR+0x1C)                
#define EIM_CS4H                (EIM_BASE_ADDR+0x20)                
#define EIM_CS4L                (EIM_BASE_ADDR+0x24)                
#define EIM_CS5H                (EIM_BASE_ADDR+0x28)                
#define EIM_CS5L                (EIM_BASE_ADDR+0x2C)                
#define EIM                     (EIM_BASE_ADDR+0x30)
//
// ;---------------------------------------;
// ; WEIM                                  ;
// ; $0022_0000 to $0022_0FFF              ;
// ;---------------------------------------;
// ; External Memory address (64M Bytes each)
// ;
#define CSD0_BASE_ADDR          0x08000000 // ; SDRAM              
#define CSD0_END_ADDR           (CSD0_BASE_ADDR+0x3FFFFFF)
#define CSD1_BASE_ADDR          0x0C000000 // ; SDRAM              
#define CSD1_END_ADDR           (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS0_BASE_ADDR           0x10000000 // ; CS0              
#define CS0_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS1_BASE_ADDR           0x12000000 // ; CS1              
#define CS1_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS2_BASE_ADDR           0x13000000 // ; CS2              
#define CS2_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS3_BASE_ADDR           0x14000000 // ; CS3              
#define CS3_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS4_BASE_ADDR           0x15000000 // ; CS4              
#define CS4_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
#define CS5_BASE_ADDR           0x16000000 // ; CS5              
#define CS5_END_ADDR            (CSD0_BASE_ADDR+0x3FFFFFF)
//
// ;---------------------------------------;
// ; SDRAMC                                ;
// ; $0022_0000 to $0022_0FFF              ;
// ;---------------------------------------;
//
#define SDRAM0_BASE             0x08000000
#define SDRAM1_BASE             0x0C000000
#define SDRAM0_END_ADDR         0x08FFFF00
#define SDRAM1_END_ADDR         0x0CFFFF00


//
// ;---------------------------------------;
// ; eSRAM                                  ;
// ; $0010_0000 to $0010_0474              ;
// ;---------------------------------------;
#define eSRAM_BUF_1	    	  	  (eSRAM_ADDR_BOT+0x0001F024)
#define eSRAM_BUF_2	    	      (eSRAM_ADDR_BOT+0x0001F088)
#define eSRAM_BUF_3	    	      (eSRAM_ADDR_BOT+0x0001F20C)
#define eSRAM_BUF_4	    	      (eSRAM_ADDR_BOT+0x0001F800)
//
// ;---------------------------------------;
// ; BROM                                  ;
// ; $0010_0000 to $0010_0474              ;
// ;---------------------------------------;
#define BROM_BASE_ADDR            0x00100000
#define BROM_INSBUF_1	    	  (BROM_BASE_ADDR+0x04) 
#define BROM_INSBUF_2	    	  (BROM_BASE_ADDR+0x08)
#define BROM_INSBUF_3	    	  (BROM_BASE_ADDR+0x0C)
#define BROM_INSBUF_4	    	  (BROM_BASE_ADDR+0x10)
//
// ;---------------------------------------;
// ; SSRAM  1M on EVB                      ;
// ; $1200_0000 to $120F_FFFF              ;
// ;---------------------------------------;
#define SSRAM_BASE_ADDR            0x12000000
#define SSRAM_BUF_1	    	  	   (SSRAM_BASE_ADDR+0x000FF024)
#define SSRAM_BUF_2	    	       (SSRAM_BASE_ADDR+0x000FF088)
#define SSRAM_BUF_3	    	       (SSRAM_BASE_ADDR+0x000FF20C)
#define SSRAM_BUF_4	    	       (SSRAM_BASE_ADDR+0x000FF800)
//
// ;---------------------------------------;
// ; SDRAM  16M on EVB                     ;
// ; $0800_0000 to $08FF_FFFF              ;
// ;---------------------------------------;
#define SDRAM_BASE_ADDR            0x08000000
#define SDRAM_BUF_1	    	   	   (SDRAM_BASE_ADDR+0x00008804) 
#define SDRAM_BUF_2	    	       (SDRAM_BASE_ADDR+0x00107708)
#define SDRAM_BUF_3	    	       (SDRAM_BASE_ADDR+0x0020660C)
#define SDRAM_BUF_4	    	       (SDRAM_BASE_ADDR+0x00305510)
#define SDRAM_BUF_5	    	   	   (SDRAM_BASE_ADDR+0x00488004) 
#define SDRAM_BUF_6	    	       (SDRAM_BASE_ADDR+0x00577308)
#define SDRAM_BUF_7	    	       (SDRAM_BASE_ADDR+0x0066600C)
#define SDRAM_BUF_8	    	       (SDRAM_BASE_ADDR+0x00755010)
#define SDRAM_BUF_9	    	   	   (SDRAM_BASE_ADDR+0x00804404)
#define SDRAM_BUF_10	    	   (SDRAM_BASE_ADDR+0x00903308)
#define SDRAM_BUF_11    	       (SDRAM_BASE_ADDR+0x00A0220C)
#define SDRAM_BUF_12    	       (SDRAM_BASE_ADDR+0x00B01110)
#define SDRAM_BUF_13    	   	   (SDRAM_BASE_ADDR+0x00C44004)
#define SDRAM_BUF_14    	       (SDRAM_BASE_ADDR+0x00D33008)
#define SDRAM_BUF_15    	       (SDRAM_BASE_ADDR+0x00E2200C)
#define SDRAM_BUF_16    	       (SDRAM_BASE_ADDR+0x00F11010)
//

#endif
