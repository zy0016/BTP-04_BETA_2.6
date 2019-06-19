/*
 * (C) 2001, 2001 Red Hat, Inc.
 * GPL'd
 * $Id: gen_probe.h,v 1.2 2003/06/26 03:37:08 LiChun_k Exp $
 */

#ifndef __MTD_GEN_PROBE_H__
#define __MTD_GEN_PROBE_H__

#include <hopen/mtd/flashchip.h>
#include <hopen/mtd/map.h> 
#include <hopen/mtd/cfi.h>

struct chip_probe {
	char *name;
	int (*probe_chip)(struct map_info *map, unsigned long base,
			  struct flchip *chips, struct cfi_private *cfi);

};

struct mtd_info *mtd_do_chip_probe(struct map_info *map, struct chip_probe *cp);

#endif /* __MTD_GEN_PROBE_H__ */
