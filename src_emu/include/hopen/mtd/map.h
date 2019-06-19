

#ifndef __MTD_MAP_H__
#define __MTD_MAP_H__

#include <hopen/list.h>

struct map_info {
	char *name;
	unsigned long size;
	int buswidth; /* in octets */
	unsigned char (*read8)(struct map_info *, unsigned long);
	unsigned short (*read16)(struct map_info *, unsigned long);
	unsigned long (*read32)(struct map_info *, unsigned long);  
	/* If it returned a 'long' I'd call it readl.
	 * It doesn't.
	 * I won't.
	 * dwmw2 */
	
	void (*copy_from)(struct map_info *, void *, unsigned long, ssize_t);
	void (*write8)(struct map_info *, unsigned char, unsigned long);
	void (*write16)(struct map_info *, unsigned short, unsigned long);
	void (*write32)(struct map_info *, unsigned long, unsigned long);
	void (*copy_to)(struct map_info *, unsigned long, const void *, ssize_t);

	void (*set_vpp)(struct map_info *, int);
	/* We put these two here rather than a single void *map_priv, 
	   because we want mappers to be able to have quickly-accessible
	   cache for the 'currently-mapped page' without the _extra_
	   redirection that would be necessary. If you need more than
	   two longs, turn the second into a pointer. dwmw2 */
	unsigned long map_priv_1;
	unsigned long map_priv_2;
	void *fldrv_priv;
	struct mtd_chip_driver *fldrv;
};


struct mtd_chip_driver {
	struct mtd_info *(*probe)(struct map_info *map);
	void (*destroy)(struct mtd_info *);
	char *name;
	struct list_head list;
};

void register_mtd_chip_driver(struct mtd_chip_driver *);
void unregister_mtd_chip_driver(struct mtd_chip_driver *);

struct mtd_info *do_map_probe(const char *name, struct map_info *map);


/*
 * Destroy an MTD device which was created for a map device.
 * Make sure the MTD device is already unregistered before calling this
 */
static inline void map_destroy(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;

	if (map->fldrv->destroy)
		map->fldrv->destroy(mtd);
	kfree(mtd, sizeof(struct mtd_info));
}

#define ENABLE_VPP(map) do { if(map->set_vpp) map->set_vpp(map, 1); } while(0)
#define DISABLE_VPP(map) do { if(map->set_vpp) map->set_vpp(map, 0); } while(0)

#endif /* __MTD_MAP_H__ */
