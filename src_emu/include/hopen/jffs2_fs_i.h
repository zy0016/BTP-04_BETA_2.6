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
 * $Source: /cvs/hopencvs/src/include/hopen/jffs2_fs_i.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/


#ifndef _HOPEN_JFFS2_FS_I
#define _HOPEN_JFFS2_FS_I

struct jffs2_inode_info {
	/* The highest (datanode) version number used for this ino */
	unsigned long highest_version;

	/* List of data fragments which make up the file */
	struct jffs2_node_frag *fraglist;

	/* There may be one datanode which isn't referenced by any of the
	   above fragments, if it contains a metadata update but no actual
	   data - or if this is a directory inode */
	/* This also holds the _only_ dnode for symlinks/device nodes, 
	   etc. */
	struct jffs2_full_dnode *metadata;

	/* Directory entries */
	struct jffs2_full_dirent *dents;

	/* Some stuff we just have to keep in-core at all times, for each inode. */
	struct jffs2_inode_cache *inocache;

	/* Keep a pointer to the last physical node in the list. We don't 
	   use the doubly-linked lists because we don't want to increase
	   the memory usage that much. This is simpler */
	//	struct jffs2_raw_node_ref *lastnode;
	unsigned short flags;
	unsigned char  usercompr;
};

#define JFFS2_INODE_INFO(i)		((struct jffs2_inode_info *)(i)->i_data)

#endif /* _JFFS2_FS_I */

