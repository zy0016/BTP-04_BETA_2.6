/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/mtd/mtd.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2004/12/13 06:20:42 $
 * 
\**************************************************************************/

#ifndef _HOPEN_MTD_MTD_H__
#define _HOPEN_MTD_MTD_H__

struct erase_info_user {
	unsigned long start;
	unsigned long length;
};

struct mtd_oob_buf {
	unsigned long start;
	unsigned long length;
	unsigned char *ptr;
};

#ifndef MTD_CHAR_MAJOR
#define MTD_CHAR_MAJOR	4
#endif

#ifndef MTD_BLOCK_MAJOR
#define MTD_BLOCK_MAJOR 4
#endif

#define MAX_MTD_DEVICES 16

#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH	3
#define MTD_NANDFLASH	4
#define MTD_PEROM		5
#define MTD_OTHER		14
#define MTD_UNKNOWN		15


#define MTD_CLEAR_BITS		1       // Bits can be cleared (flash)
#define MTD_SET_BITS		2       // Bits can be set
#define MTD_ERASEABLE		4       // Has an erase function
#define MTD_WRITEB_WRITEABLE	8   // Direct IO is possible
#define MTD_VOLATILE		16      // Set for RAMs
#define MTD_XIP				32		// eXecute-In-Place possible
#define MTD_OOB				64		// Out-of-band data (NAND flash)
#define MTD_ECC				128		// Device capable of automatic ECC

// Some common devices / combinations of capabilities
#define MTD_CAP_ROM			0
#define MTD_CAP_RAM			(MTD_CLEAR_BITS|MTD_SET_BITS|MTD_WRITEB_WRITEABLE)
#define MTD_CAP_NORFLASH    (MTD_CLEAR_BITS|MTD_ERASEABLE)
#define MTD_CAP_NANDFLASH   (MTD_CLEAR_BITS|MTD_ERASEABLE|MTD_OOB)
#define MTD_WRITEABLE		(MTD_CLEAR_BITS|MTD_SET_BITS)


// Types of automatic ECC/Checksum available
#define MTD_ECC_NONE		0 	// No automatic ECC available
#define MTD_ECC_RS_DiskOnChip	1	// Automatic ECC on DiskOnChip
#define MTD_ECC_SW			2	// SW ECC for Toshiba & Samsung devices

struct mtd_info_user {
	unsigned char type;
	unsigned long flags;
	unsigned long size;		 // Total size of the MTD
	unsigned long erasesize;
	unsigned long oobblock;  // Size of OOB blocks (e.g. 512)
	unsigned long oobsize;   // Amount of OOB data per block (e.g. 16)
	unsigned long ecctype;
	unsigned long eccsize;
};

struct region_info_user {
	unsigned long offset;		/* At which this region starts, 
					 * from the beginning of the MTD */
	unsigned long erasesize;		/* For this region */
	unsigned long numblocks;		/* Number of blocks in this region */
	unsigned long regionindex;
};

#define MEMGETINFO              _IOR('M', 1, struct mtd_info_user)
#define MEMERASE                _IOW('M', 2, struct erase_info_user)
#define MEMWRITEOOB             _IOWR('M', 3, struct mtd_oob_buf)
#define MEMREADOOB              _IOWR('M', 4, struct mtd_oob_buf)
#define MEMLOCK                 _IOW('M', 5, struct erase_info_user)
#define MEMUNLOCK               _IOW('M', 6, struct erase_info_user)
#define MEMGETREGIONCOUNT		_IOR('M', 7, int)
#define MEMGETREGIONINFO		_IOWR('M', 8, struct region_info_user)
#define MEMWRITEDATA             _IOWR('M', 9, struct mtd_oob_buf)
#define MEMREADDATA              _IOWR('M', 10, struct mtd_oob_buf)

#define MTD_ERASE_PENDING      	0x01
#define MTD_ERASING				0x02
#define MTD_ERASE_SUSPEND		0x04
#define MTD_ERASE_DONE          0x08
#define MTD_ERASE_FAILED        0x10

struct erase_info {
	struct mtd_info *mtd;
	unsigned long addr;
	unsigned long len;
	unsigned long time;
	unsigned long retries;
	unsigned long dev;
	unsigned long cell;
	void (*callback) (struct erase_info *self);
	unsigned long priv;
	unsigned char state;
	struct erase_info *next;
};

struct mtd_erase_region_info {
	unsigned long offset;			/* At which this region starts, from the beginning of the MTD */
	unsigned long erasesize;		/* For this region */
	unsigned long numblocks;		/* Number of blocks of erasesize in this region */
};

struct iovec
{
	void *iov_base;	/* BSD uses caddr_t (1003.1g requires void *) */
	size_t iov_len;	/* Must be size_t (1003.1g) */
};


struct mtd_info {
	unsigned char type;
	unsigned long flags;
	unsigned long size;	 // Total size of the MTD

	/* "Major" erase size for the device. Nae users may take this
	 * to be the only erase size available, or may use the more detailed
	 * information below if they desire
	 */
	unsigned long erasesize;

	unsigned long oobblock;  // Size of OOB blocks (e.g. 512)
	unsigned long oobsize;   // Amount of OOB data per block (e.g. 16)
	unsigned long ecctype;
	unsigned long eccsize;

	// Kernel-only stuff starts here.
	char *name;
	int index;

	/* Data for variable erase regions. If numeraseregions is zero,
	 * it means that the whole device has erasesize as given above. 
	 */
	int numeraseregions;
	struct mtd_erase_region_info *eraseregions; 

	int (*erase) (struct mtd_info *mtd, struct erase_info *instr);

	int (*read) (struct mtd_info *mtd, unsigned long from, unsigned long len,
				unsigned long * retlen, unsigned char * buf);
	int (*write) (struct mtd_info *mtd, unsigned long to, unsigned long len,
				unsigned long * retlen, const unsigned char *buf);

	int (*read_ecc) (struct mtd_info *mtd, unsigned long from, unsigned long len,
				unsigned long * retlen, unsigned char *buf, unsigned char *eccbuf);
	int (*write_ecc) (struct mtd_info *mtd, unsigned long to, unsigned long len,
				unsigned long * retlen, const unsigned char *buf, unsigned char *eccbuf);

	int (*read_oob) (struct mtd_info *mtd, unsigned long from, unsigned long len,
				unsigned long * retlen, unsigned char * buf);
	int (*write_oob) (struct mtd_info *mtd, unsigned long to, unsigned long len,
				unsigned long * retlen, const unsigned char *buf);

	/* iovec-based read/write methods. We need these especially for NAND flash,
	   with its limited number of write cycles per erase.
	   NB: The 'count' parameter is the number of _vectors_, each of 
	   which contains an (ofs, len) tuple.
	*/
	int (*readv) (struct mtd_info *mtd, struct iovec *vecs, unsigned long count,
				unsigned long from, unsigned long * retlen);
	int (*writev) (struct mtd_info *mtd, const struct iovec *vecs, unsigned long count,
				unsigned long to, unsigned long *retlen);

	/* Sync */
	void (*sync) (struct mtd_info *mtd);

	/* Chip-supported device locking */
	int (*lock) (struct mtd_info *mtd, unsigned long ofs, unsigned long len);
	int (*unlock) (struct mtd_info *mtd, unsigned long ofs, unsigned long len);

	/* Power Management functions */
	int (*suspend) (struct mtd_info *mtd);
	void (*resume) (struct mtd_info *mtd);

	void *priv;
};


/* Kernel-side ioctl definitions */

extern int add_mtd_device(struct mtd_info *mtd);
extern int del_mtd_device(struct mtd_info *mtd);

extern struct mtd_info * get_mtd_device(struct mtd_info *mtd, int num);
extern void put_mtd_device(struct mtd_info * mtd);

/*
 * Debugging macro and defines
 */
#define MTD_DEBUG_LEVEL0	(0)		/* Quiet   */
#define MTD_DEBUG_LEVEL1	(1)		/* Audible */
#define MTD_DEBUG_LEVEL2	(2)		/* Loud    */
#define MTD_DEBUG_LEVEL3	(3)		/* Noisy   */

#endif /* _HOPEN_MTD_MTD_H__ */
