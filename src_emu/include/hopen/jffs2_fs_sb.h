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
 * $Source: /cvs/hopencvs/src/include/hopen/jffs2_fs_sb.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/08/19 02:29:26 $
 * 
\**************************************************************************/

#ifndef _HOPEN_JFFS2_FS_SB
#define _HOPEN_JFFS2_FS_SB

#include <hopen/mutex.h>
#include <hopen/list.h>

#define INOCACHE_HASHSIZE 32

#define JFFS2_SB_FLAG_RO		 1
#define JFFS2_SB_FLAG_MOUNTING	 4

/* A struct for the overall file system control.  Pointers to
   jffs2_sb_info structs are named `c' in the source code.  
   Nee jffs_control
*/

struct jffs2_eraseblock;
struct jffs2_inode_cache;

struct jffs2_sb_info {
	struct mtd_info *mtd;

	unsigned long highest_ino;
	unsigned int  flags;

	unsigned long gc_minfree_threshold;	/* GC trigger thresholds */
	unsigned long gc_maxdirty_threshold;

	struct mutex erase_completion_lock;
	struct mutex  alloc_mutex;
	unsigned long flash_size;
	unsigned long used_size;
	unsigned long dirty_size;
	unsigned long free_size;
	unsigned long erasing_size;
	unsigned long bad_size;
	unsigned long sector_size;
	unsigned long min_free_size;
	unsigned long max_chunk_size;

	unsigned long nr_free_blocks;
	unsigned long nr_erasing_blocks;

	unsigned long reserved_deletion;	//reserved blocks allow deleting
	unsigned long reserved_write;		//reserved blocks allow writing
	unsigned long reserved_gctrigger;	//reserved blocks for triggering gc
	unsigned long reserved_gcmerge;		//reserved blocks allow gc merge
	unsigned long reserved_gcbad;		//reserved blocks for gc bad
	unsigned long nospace_dirty_size;
	
	struct wait_queue * erase_wait;

	unsigned long nr_blocks;
	struct jffs2_eraseblock *blocks;	/* The whole array of blocks. Used for getting blocks 
										 * from the offset (blocks[ofs / sector_size]) */
	struct jffs2_eraseblock *nextblock;	/* The block we're currently filling */

	struct jffs2_eraseblock *gcblock;	/* The block we're currently garbage-collecting */

	unsigned long in_scan_flag;

	struct list_head clean_list;		/* Blocks 100% full of clean data */
	struct list_head dirty_list;		/* Blocks with some dirty space */
	struct list_head erasing_list;		/* Blocks which are currently erasing */
	struct list_head erase_pending_list;	/* Blocks which need erasing */
	struct list_head erase_complete_list;	/* Blocks which are erased and need the clean marker written to them */
	struct list_head free_list;		/* Blocks which are free and ready to be used */
	struct list_head bad_list;		/* Bad blocks. */
	struct list_head bad_used_list;		/* Bad blocks with valid data in. */
	struct jffs2_inode_cache *inocache_list[INOCACHE_HASHSIZE];
};

#define JFFS2_SB_INFO(sb) ((struct jffs2_sb_info *)(sb)->s_data)

#define OFNI_BS_2SFFJ(c)  ((struct super_block *)(((char *)c) - ((char *)(&((struct super_block *)NULL)->s_data))))

#endif /* _HOPEN_JFFS2_FB_SB */
