/*
 * Header for MultiMediaCard (MMC)
 *
 * Copyright 2002 Hewlett-Packard Company
 *
 * Use consistent with the GNU GPL is permitted,
 * provided that this copyright notice is
 * preserved in its entirety in all copies and derived works.
 *
 * HEWLETT-PACKARD COMPANY MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
 * AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
 * FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 * Many thanks to Alessandro Rubini and Jonathan Corbet!
 *
 * Based strongly on code by:
 *
 * Author: Yong-iL Joh <tolkien@mizi.com>
 * Date  : $Date: 2003/08/13 04:41:50 $ 
 *
 * Author:  Andrew Christian
 *          15 May 2002
 */

#ifndef MMC_MMC_PROTOCOL_H
#define MMC_MMC_PROTOCOL_H

#undef CONFIG_MMC_DEBUG

/* Standard MMC clock speeds */
#define MMC_CLOCK_SLOW    400000      /* 400 kHz for initial setup */
#define MMC_CLOCK_FAST  20000000      /* 20 MHz for maximum for normal operation */

/* Extra MMC commands for state control */
/* Use negative numbers to disambiguate */
#define MMC_CIM_RESET            -1

/* Standard MMC commands (3.1)           type  argument     response */
   /* class 1 */
#define	MMC_GO_IDLE_STATE         0   /* bc                          */
#define MMC_SEND_OP_COND          1   /* bcr  [31:0] OCR         R3  */
#define MMC_ALL_SEND_CID          2   /* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR               4   /* bc   [31:16] RCA            */
#define MMC_SELECT_CARD           7   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID             10   /* ac   [31:16] RCA        R2  */
#define MMC_READ_DAT_UNTIL_STOP  11   /* adtc [31:0] dadr        R1  */
#define MMC_STOP_TRANSMISSION    12   /* ac                      R1b */
#define MMC_SEND_STATUS	         13   /* ac   [31:16] RCA        R1  */
#define MMC_GO_INACTIVE_STATE    15   /* ac   [31:16] RCA            */

  /* class 2 */
#define MMC_SET_BLOCKLEN         16   /* ac   [31:0] block len   R1  */
#define MMC_READ_SINGLE_BLOCK    17   /* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK  18   /* adtc [31:0] data addr   R1  */

  /* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 20   /* adtc [31:0] data addr   R1  */

  /* class 4 */
#define MMC_SET_BLOCK_COUNT      23   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_BLOCK          24   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */
#define MMC_PROGRAM_CID          26   /* adtc                    R1  */
#define MMC_PROGRAM_CSD          27   /* adtc                    R1  */

  /* class 6 */
#define MMC_SET_WRITE_PROT       28   /* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT       29   /* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT      30   /* adtc [31:0] wpdata addr R1  */

  /* class 5 */
#define MMC_ERASE_GROUP_START    35   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END      36   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE                37   /* ac                      R1b */

  /* class 9 */
#define MMC_FAST_IO              39   /* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE         40   /* bcr                     R5  */

  /* class 7 */
#define MMC_LOCK_UNLOCK          42   /* adtc                    R1b */

  /* class 8 */
#define MMC_APP_CMD              55   /* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD              56   /* adtc [0] RD/WR          R1b */

  /* Advanced Command (used by sd card) */
#define SD_APP_OP_COND           41   /* bcr  [31:0] OCR         R3  */


/* Don't change the order of these; they are used in dispatch tables */
enum mmc_rsp_t {
	RESPONSE_NONE   = 0,
	RESPONSE_R1     = 1,
	RESPONSE_R1B    = 2,
	RESPONSE_R2_CID = 3,
	RESPONSE_R2_CSD  = 4,
	RESPONSE_R3      = 5,
	RESPONSE_R4      = 6,
	RESPONSE_R5      = 7
};


/*
  MMC status in R1
  Type
  	e : error bit
	s : status bit
	r : detected and set for the actual command response
	x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
  	a : according to the card state
	b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
	c : clear by read
 */

#define R1_OUT_OF_RANGE		(1 << 31)	/* er, c */
#define R1_ADDRESS_ERROR	(1 << 30)	/* erx, c */
#define R1_BLOCK_LEN_ERROR	(1 << 29)	/* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)	/* er, c */
#define R1_ERASE_PARAM		(1 << 27)	/* ex, c */
#define R1_WP_VIOLATION		(1 << 26)	/* erx, c */
#define R1_CARD_IS_LOCKED	(1 << 25)	/* sx, a */
#define R1_LOCK_UNLOCK_FAILED	(1 << 24)	/* erx, c */
#define R1_COM_CRC_ERROR	(1 << 23)	/* er, b */
#define R1_ILLEGAL_COMMAND	(1 << 22)	/* er, b */
#define R1_CARD_ECC_FAILED	(1 << 21)	/* ex, c */
#define R1_CC_ERROR		(1 << 20)	/* erx, c */
#define R1_ERROR		(1 << 19)	/* erx, c */
#define R1_UNDERRUN		(1 << 18)	/* ex, c */
#define R1_OVERRUN		(1 << 17)	/* ex, c */
#define R1_CID_CSD_OVERWRITE	(1 << 16)	/* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP	(1 << 15)	/* sx, c */
#define R1_CARD_ECC_DISABLED	(1 << 14)	/* sx, a */
#define R1_ERASE_RESET		(1 << 13)	/* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)    	((x & 0x00001E00) >> 9)	/* sx, b (4 bits) */
#define R1_READY_FOR_DATA	(1 << 8)	/* sx, a */
#define R1_APP_CMD		(1 << 7)	/* sr, c */

enum card_state {
	CARD_STATE_EMPTY = -1,
	CARD_STATE_IDLE	 = 0,
	CARD_STATE_READY = 1,
	CARD_STATE_IDENT = 2,
	CARD_STATE_STBY	 = 3,
	CARD_STATE_TRAN	 = 4,
	CARD_STATE_DATA	 = 5,
	CARD_STATE_RCV	 = 6,
	CARD_STATE_PRG	 = 7,
	CARD_STATE_DIS	 = 8,
};

/* These are unpacked versions of the actual responses */

struct mmc_response_r1 {
	unsigned char  cmd;
	unsigned long status;
};

struct mmc_cid {
	unsigned char  mid;
	unsigned short oid;
	unsigned char  pnm[7];   // Product name (we null-terminate)
	unsigned char  prv;
	unsigned long psn;
	unsigned char  mdt;
};

struct mmc_csd {
	unsigned char  csd_structure;
	unsigned char  spec_vers;
	unsigned char  taac;
	unsigned char  nsac;
	unsigned char  tran_speed;
	unsigned short ccc;
	unsigned char  read_bl_len;
	unsigned char  read_bl_partial;
	unsigned char  write_blk_misalign;
	unsigned char  read_blk_misalign;
	unsigned char  dsr_imp;
	unsigned short c_size;
	unsigned char  vdd_r_curr_min;
	unsigned char  vdd_r_curr_max;
	unsigned char  vdd_w_curr_min;
	unsigned char  vdd_w_curr_max;
	unsigned char  c_size_mult;
	union {
		struct { /* MMC system specification version 3.1 */
			unsigned char  erase_grp_size;  
			unsigned char  erase_grp_mult; 
		} v31;
		struct { /* MMC system specification version 2.2 */
			unsigned char  sector_size;
			unsigned char  erase_grp_size;
		} v22;
	} erase;
	unsigned char  wp_grp_size;
	unsigned char  wp_grp_enable;
	unsigned char  default_ecc;
	unsigned char  r2w_factor;
	unsigned char  write_bl_len;
	unsigned char  write_bl_partial;
	unsigned char  file_format_grp;
	unsigned char  copy;
	unsigned char  perm_write_protect;
	unsigned char  tmp_write_protect;
	unsigned char  file_format;
	unsigned char  ecc;
};

struct mmc_response_r3 {  
	unsigned long ocr;
}; 

#define MMC_VDD_145_150	0x00000001	/* VDD voltage 1.45 - 1.50 */
#define MMC_VDD_150_155	0x00000002	/* VDD voltage 1.50 - 1.55 */
#define MMC_VDD_155_160	0x00000004	/* VDD voltage 1.55 - 1.60 */
#define MMC_VDD_160_165	0x00000008	/* VDD voltage 1.60 - 1.65 */
#define MMC_VDD_165_170	0x00000010	/* VDD voltage 1.65 - 1.70 */
#define MMC_VDD_17_18	0x00000020	/* VDD voltage 1.7 - 1.8 */
#define MMC_VDD_18_19	0x00000040	/* VDD voltage 1.8 - 1.9 */
#define MMC_VDD_19_20	0x00000080	/* VDD voltage 1.9 - 2.0 */
#define MMC_VDD_20_21	0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22	0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23	0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24	0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25	0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26	0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27	0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28	0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29	0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30	0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31	0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32	0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33	0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34	0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35	0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36	0x00800000	/* VDD voltage 3.5 ~ 3.6 */
#define MMC_CARD_BUSY	0x80000000	/* Card Power up status bit */


/* CSD field definitions */
 
#define CSD_STRUCT_VER_1_0  0           /* Valid for system specification 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1  1           /* Valid for system specification 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2  2           /* Valid for system specification 3.1       */

#define CSD_SPEC_VER_0      0           /* Implements system specification 1.0 - 1.2 */
#define CSD_SPEC_VER_1      1           /* Implements system specification 1.4 */
#define CSD_SPEC_VER_2      2           /* Implements system specification 2.0 - 2.2 */
#define CSD_SPEC_VER_3      3           /* Implements system specification 3.1 */


#define PERROR(format,arg...) printk(KERN_ERR __FILE__ ":%s - " format "\n", __FUNCTION__ , ## arg)
#define PALERT(format,arg...) printk(KERN_ALERT __FILE__ ":%s - " format "\n", __FUNCTION__ , ## arg)

#ifdef CONFIG_MMC_DEBUG
#ifndef CONFIG_MMC_DEBUG_VERBOSE
#define CONFIG_MMC_DEBUG_VERBOSE 3
#endif
extern int g_mmc_debug;
#define DEBUG(n, format, args...)	\
	if (n <=  g_mmc_debug) {	\
		printk(KERN_DEBUG "[" __FILE__ "] %s " format "\n", __FUNCTION__ , ## args);	\
	}
#define START_DEBUG(n) do { if (n <= g_mmc_debug)
#define END_DEBUG      } while (0)
#else
#define DEBUG(n, args...)
#define START_DEBUG(n)
#define END_DEBUG
#endif /* CONFIG_MMC_DEBUG */

#endif  /* MMC_MMC_PROTOCOL_H */

