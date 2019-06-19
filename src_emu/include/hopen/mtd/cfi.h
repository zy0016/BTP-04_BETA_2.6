
/* Common Flash Interface structures 
 * See http://support.intel.com/design/flash/technote/index.htm
 * $Id: cfi.h,v 1.3 2003/07/31 01:59:45 TangJie Exp $
 */

#ifndef __MTD_CFI_H__
#define __MTD_CFI_H__

#include <hopen/mtd/flashchip.h>
#include <hopen/mtd/cfi_endian.h>

/*
 * You can optimize the code size and performance by defining only 
 * the geometry(ies) available on your hardware.
 * CFIDEV_INTERLEAVE_n, where  represents the interleave (number of chips to fill the bus width)
 * CFIDEV_BUSWIDTH_n, where n is the bus width in bytes (1, 2 or 4 bytes)
 *
 * By default, all (known) geometries are supported.
 */

#ifndef CONFIG_MTD_CFI_GEOMETRY

#define CFIDEV_INTERLEAVE_1 (1)
#define CFIDEV_INTERLEAVE_2 (2)
#define CFIDEV_INTERLEAVE_4 (4)

#define CFIDEV_BUSWIDTH_1 (1)
#define CFIDEV_BUSWIDTH_2 (2)
#define CFIDEV_BUSWIDTH_4 (4)

#else

#ifdef CONFIG_MTD_CFI_I1
#define CFIDEV_INTERLEAVE_1 (1)
#endif
#ifdef CONFIG_MTD_CFI_I2
#define CFIDEV_INTERLEAVE_2 (2)
#endif
#ifdef CONFIG_MTD_CFI_I4
#define CFIDEV_INTERLEAVE_4 (4)
#endif

#ifdef CONFIG_MTD_CFI_B1
#define CFIDEV_BUSWIDTH_1 (1)
#endif
#ifdef CONFIG_MTD_CFI_B2
#define CFIDEV_BUSWIDTH_2 (2)
#endif
#ifdef CONFIG_MTD_CFI_B4
#define CFIDEV_BUSWIDTH_4 (4)
#endif

#endif

/*
 * The following macros are used to select the code to execute:
 *   cfi_buswidth_is_*()
 *   cfi_interleave_is_*()
 *   [where * is either 1, 2 or 4]
 * Those macros should be used with 'if' statements.  If only one of few
 * geometry arrangements are selected, they expand to constants thus allowing
 * the compiler (most of them being 0) to optimize away all the unneeded code,
 * while still validating the syntax (which is not possible with embedded 
 * #if ... #endif constructs).
 */

#ifdef CFIDEV_INTERLEAVE_1
# ifdef CFIDEV_INTERLEAVE
#  undef CFIDEV_INTERLEAVE
#  define CFIDEV_INTERLEAVE (cfi->interleave)
# else
#  define CFIDEV_INTERLEAVE CFIDEV_INTERLEAVE_1
# endif
# define cfi_interleave_is_1() (CFIDEV_INTERLEAVE == CFIDEV_INTERLEAVE_1)
#else
# define cfi_interleave_is_1() (0)
#endif

#ifdef CFIDEV_INTERLEAVE_2
# ifdef CFIDEV_INTERLEAVE
#  undef CFIDEV_INTERLEAVE
#  define CFIDEV_INTERLEAVE (cfi->interleave)
# else
#  define CFIDEV_INTERLEAVE CFIDEV_INTERLEAVE_2
# endif
# define cfi_interleave_is_2() (CFIDEV_INTERLEAVE == CFIDEV_INTERLEAVE_2)
#else
# define cfi_interleave_is_2() (0)
#endif

#ifdef CFIDEV_INTERLEAVE_4
# ifdef CFIDEV_INTERLEAVE
#  undef CFIDEV_INTERLEAVE
#  define CFIDEV_INTERLEAVE (cfi->interleave)
# else
#  define CFIDEV_INTERLEAVE CFIDEV_INTERLEAVE_4
# endif
# define cfi_interleave_is_4() (CFIDEV_INTERLEAVE == CFIDEV_INTERLEAVE_4)
#else
# define cfi_interleave_is_4() (0)
#endif

#ifndef CFIDEV_INTERLEAVE
#error You must define at least one interleave to support!
#endif

#ifdef CFIDEV_BUSWIDTH_1
# ifdef CFIDEV_BUSWIDTH
#  undef CFIDEV_BUSWIDTH
#  define CFIDEV_BUSWIDTH (map->buswidth)
# else
#  define CFIDEV_BUSWIDTH CFIDEV_BUSWIDTH_1
# endif
# define cfi_buswidth_is_1() (CFIDEV_BUSWIDTH == CFIDEV_BUSWIDTH_1)
#else
# define cfi_buswidth_is_1() (0)
#endif

#ifdef CFIDEV_BUSWIDTH_2
# ifdef CFIDEV_BUSWIDTH
#  undef CFIDEV_BUSWIDTH
#  define CFIDEV_BUSWIDTH (map->buswidth)
# else
#  define CFIDEV_BUSWIDTH CFIDEV_BUSWIDTH_2
# endif
# define cfi_buswidth_is_2() (CFIDEV_BUSWIDTH == CFIDEV_BUSWIDTH_2)
#else
# define cfi_buswidth_is_2() (0)
#endif

#ifdef CFIDEV_BUSWIDTH_4
# ifdef CFIDEV_BUSWIDTH
#  undef CFIDEV_BUSWIDTH
#  define CFIDEV_BUSWIDTH (map->buswidth)
# else
#  define CFIDEV_BUSWIDTH CFIDEV_BUSWIDTH_4
# endif
# define cfi_buswidth_is_4() (CFIDEV_BUSWIDTH == CFIDEV_BUSWIDTH_4)
#else
# define cfi_buswidth_is_4() (0)
#endif

#ifndef CFIDEV_BUSWIDTH
#error You must define at least one bus width to support!
#endif

typedef unsigned long cfi_word;
/* NB: these values must represents the number of bytes needed to meet the 
 *     device type (x8, x16, x32).  Eg. a 32 bit device is 4 x 8 bytes. 
 *     These numbers are used in calculations.
 */
#define CFI_DEVICETYPE_X8  (8 / 8)
#define CFI_DEVICETYPE_X16 (16 / 8)
#define CFI_DEVICETYPE_X32 (32 / 8)

#ifndef __GNUC__
#define __attribute__(x)
#endif

/* NB: We keep these structures in memory in HOST byteorder, except
 * where individually noted.
 */

/* Basic Query Structure */
struct cfi_ident {
  unsigned char  qry[3];
  unsigned short P_ID;
  unsigned short P_ADR;
  unsigned short A_ID;
  unsigned short A_ADR;
  unsigned char  VccMin;
  unsigned char  VccMax;
  unsigned char  VppMin;
  unsigned char  VppMax;
  unsigned char  WordWriteTimeoutTyp;
  unsigned char  BufWriteTimeoutTyp;
  unsigned char  BlockEraseTimeoutTyp;
  unsigned char  ChipEraseTimeoutTyp;
  unsigned char  WordWriteTimeoutMax;
  unsigned char  BufWriteTimeoutMax;
  unsigned char  BlockEraseTimeoutMax;
  unsigned char  ChipEraseTimeoutMax;
  unsigned char  DevSize;
  unsigned short InterfaceDesc;
  unsigned short MaxBufWriteSize;
  unsigned char  NumEraseRegions;
  unsigned long EraseRegionInfo[0]; /* Not host ordered */
} __attribute__((packed));

/* Extended Query Structure for both PRI and ALT */

struct cfi_extquery {
  unsigned char  pri[3];
  unsigned char  MajorVersion;
  unsigned char  MinorVersion;
} __attribute__((packed));

/* Vendor-Specific PRI for Intel/Sharp Extended Command Set (0x0001) */

struct cfi_pri_intelext {
  unsigned char  pri[3];
  unsigned char  MajorVersion;
  unsigned char  MinorVersion;
  unsigned long FeatureSupport;
  unsigned char  SuspendCmdSupport;
  unsigned short BlkStatusRegMask;
  unsigned char  VccOptimal;
  unsigned char  VppOptimal;
  unsigned char  NumProtectionFields;
  unsigned short ProtRegAddr;
  unsigned char  FactProtRegSize;
  unsigned char  UserProtRegSize;
} __attribute__((packed));

struct cfi_pri_query {
  unsigned char  NumFields;
  unsigned long ProtField[1]; /* Not host ordered */
} __attribute__((packed));

struct cfi_bri_query {
  unsigned char  PageModeReadCap;
  unsigned char  NumFields;
  unsigned long ConfField[1]; /* Not host ordered */
} __attribute__((packed));

struct cfi_intelext_blockinfo {
  unsigned short NumIdentBlocks;
  unsigned short BlockSize;
  unsigned short MinBlockEraseCycles;
  unsigned char  BitsPerCell;
  unsigned char  BlockCap;
}__attribute__((packed));

struct cfi_intelext_regioninfo {
  unsigned short NumIdentPartitions;
  unsigned char  NumOpAllowed;
  unsigned char  NumOpAllowedSimProgMode;
  unsigned char  NumOpAllowedSimEraMode;
  unsigned char  NumBlockTypes;
  struct cfi_intelext_blockinfo BlockTypes[1];
} __attribute__((packed));

#define P_ID_NONE 0
#define P_ID_INTEL_EXT 1
#define P_ID_AMD_STD 2
#define P_ID_INTEL_STD 3
#define P_ID_AMD_EXT 4
#define P_ID_MITSUBISHI_STD 256
#define P_ID_MITSUBISHI_EXT 257
#define P_ID_RESERVED 65535


#define CFI_MODE_CFI	0
#define CFI_MODE_JEDEC	1

struct cfi_private {
	unsigned short cmdset;
	void *cmdset_priv;
	int interleave;
	int device_type;
	int cfi_mode;		/* Are we a JEDEC device pretending to be CFI? */
	int addr_unlock1;
	int addr_unlock2;
	int fast_prog;
	struct mtd_info *(*cmdset_setup)(struct map_info *);
	struct cfi_ident *cfiq; /* For now only one. We insist that all devs
				  must be of the same type. */
	int mfr, id;
	int numchips;
	unsigned long chipshift; /* Because they're of the same type */
	const char *im_name;	 /* inter_module name for cmdset_setup */
	struct flchip chips[0];  /* per-chip data structure for each chip */
};

#define MAX_CFI_CHIPS 8 /* Entirely arbitrary to avoid realloc() */

/*
 * Returns the command address according to the given geometry.
 */
static inline unsigned long cfi_build_cmd_addr(unsigned long cmd_ofs, int interleave, int type)
{
	return (cmd_ofs * type) * interleave;
}

/*
 * Transforms the CFI command for the given geometry (bus width & interleave.
 */
static inline unsigned long cfi_build_cmd(unsigned char cmd, struct map_info *map, struct cfi_private *cfi)
{
	unsigned long val = 0;

	if (cfi_buswidth_is_1()) {
		/* 1 x8 device */
		val = cmd;
	} else if (cfi_buswidth_is_2()) {
		if (cfi_interleave_is_1()) {
			/* 1 x16 device in x16 mode */
			val = cpu_to_cfi16(cmd);
		} else if (cfi_interleave_is_2()) {
			/* 2 (x8, x16 or x32) devices in x8 mode */
			val = cpu_to_cfi16((cmd << 8) | cmd);
		}
	} else if (cfi_buswidth_is_4()) {
		if (cfi_interleave_is_1()) {
			/* 1 x32 device in x32 mode */
			val = cpu_to_cfi32(cmd);
		} else if (cfi_interleave_is_2()) {
			/* 2 x16 device in x16 mode */
			val = cpu_to_cfi32((cmd << 16) | cmd);
		} else if (cfi_interleave_is_4()) {
			/* 4 (x8, x16 or x32) devices in x8 mode */
			val = (cmd << 16) | cmd;
			val = cpu_to_cfi32((val << 8) | val);
		}
	}
	return val;
}
#define CMD(x)  cfi_build_cmd((x), map, cfi)

/*
 * Read a value according to the bus width.
 */

static inline unsigned long cfi_read(struct map_info *map, unsigned long addr)
{
	if (cfi_buswidth_is_1()) {
		return map->read8(map, addr);
	} else if (cfi_buswidth_is_2()) {
		return map->read16(map, addr);
	} else if (cfi_buswidth_is_4()) {
		return map->read32(map, addr);
	} else {
		return 0;
	}
}

/*
 * Write a value according to the bus width.
 */

static inline void cfi_write(struct map_info *map, unsigned long val, unsigned long addr)
{
	if (cfi_buswidth_is_1()) {
		map->write8(map, val, addr);
	} else if (cfi_buswidth_is_2()) {
		map->write16(map, val, addr);
	} else if (cfi_buswidth_is_4()) {
		map->write32(map, val, addr);
	}
}

/*
 * Sends a CFI command to a bank of flash for the given geometry.
 *
 * Returns the offset in flash where the command was written.
 * If prev_val is non-null, it will be set to the value at the command address,
 * before the command was written.
 */
static inline unsigned long cfi_send_gen_cmd(unsigned char cmd, unsigned long cmd_addr, unsigned long base,
				struct map_info *map, struct cfi_private *cfi,
				int type, unsigned long *prev_val)
{
	unsigned long val;
	unsigned long addr = base + cfi_build_cmd_addr(cmd_addr, CFIDEV_INTERLEAVE, type);

	val = cfi_build_cmd(cmd, map, cfi);

	if (prev_val)
		*prev_val = cfi_read(map, addr);

	cfi_write(map, val, addr);

	return addr - base;
}

static inline unsigned char cfi_read_query(struct map_info *map, unsigned long addr)
{
	if (cfi_buswidth_is_1()) {
		return map->read8(map, addr);
	} else if (cfi_buswidth_is_2()) {
		return cfi16_to_cpu(map->read16(map, addr));
	} else if (cfi_buswidth_is_4()) {
		return cfi32_to_cpu(map->read32(map, addr));
	} else {
		return 0;
	}
}

static inline void cfi_udelay(int us)
{
		udelay(us);
}
static inline void cfi_spin_lock(spinlock_t *mutex)
{
	spin_lock_bh(mutex);
}

static inline void cfi_spin_unlock(spinlock_t *mutex)
{
	spin_unlock_bh(mutex);
}


#endif /* __MTD_CFI_H__ */
