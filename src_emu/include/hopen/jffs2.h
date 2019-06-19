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
 * $Source: /cvs/hopencvs/src/include/hopen/jffs2.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001 Red Hat, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * The original JFFS, from which the design for JFFS2 was derived,
 * was designed and implemented by Axis Communications AB.
 *
 * The contents of this file are subject to the Red Hat eCos Public
 * License Version 1.1 (the "Licence"); you may not use this file
 * except in compliance with the Licence.  You may obtain a copy of
 * the Licence at http://www.redhat.com/
 *
 * Software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
 * See the Licence for the specific language governing rights and
 * limitations under the Licence.
 *
 * The Original Code is JFFS2 - Journalling Flash File System, version 2
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License version 2 (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of the
 * above.  If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the RHEPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GPL.  If you do not delete the
 * provisions above, a recipient may use your version of this file
 * under either the RHEPL or the GPL.
 *
 */

#ifndef _HOPEN_JFFS2_H__
#define _HOPEN_JFFS2_H__

#define JFFS2_SUPER_MAGIC		0x72b6

/* Values we may expect to find in the 'magic' field */
#define JFFS2_OLD_MAGIC_BITMASK	0x1984
#define JFFS2_MAGIC_BITMASK		0x1985
#define KSAMTIB_CIGAM_2SFFJ		0x5981 /* For detecting wrong-endian fs */
#define JFFS2_EMPTY_BITMASK		0xffff
#define JFFS2_DIRTY_BITMASK		0x0000

/* We only allow a single char for length, and 0xFF is empty flash so
   we don't want it confused with a real length. Hence max 254.
*/
#define JFFS2_MAX_NAME_LEN		254

/* How small can we sensibly write nodes? */
#define JFFS2_MIN_DATA_LEN		128

/* The max data node size */
#define JFFS2_MAX_DATA_LEN		4096

#define JFFS2_COMPR_NONE		0x00
#define JFFS2_COMPR_ZERO		0x01
#define JFFS2_COMPR_RTIME		0x02
#define JFFS2_COMPR_RUBINMIPS	0x03
#define JFFS2_COMPR_COPY		0x04
#define JFFS2_COMPR_DYNRUBIN	0x05
#define JFFS2_COMPR_ZLIB		0x06
/* Compatibility flags. */
#define JFFS2_COMPAT_MASK		0xc000      /* What do to if an unknown nodetype is found */
#define JFFS2_NODE_ACCURATE		0x2000
/* INCOMPAT: Fail to mount the filesystem */
#define JFFS2_FEATURE_INCOMPAT	0xc000
/* ROCOMPAT: Mount read-only */
#define JFFS2_FEATURE_ROCOMPAT	0x8000
/* RWCOMPAT_COPY: Mount read/write, and copy the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_COPY		0x4000
/* RWCOMPAT_DELETE: Mount read/write, and delete the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_DELETE	0x0000

#define JFFS2_NODETYPE_DIRENT (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 1)
#define JFFS2_NODETYPE_INODE (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 2)
#define JFFS2_NODETYPE_CLEANMARKER (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 3)

// Maybe later...
//#define JFFS2_NODETYPE_CHECKPOINT (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 3)
//#define JFFS2_NODETYPE_OPTIONS (JFFS2_FEATURE_RWCOMPAT_COPY | JFFS2_NODE_ACCURATE | 4)

/* Same as the non_ECC versions, but with extra space for real 
 * ECC instead of just the checksum. For use on NAND flash 
 */
//#define JFFS2_NODETYPE_DIRENT_ECC (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 5)
//#define JFFS2_NODETYPE_INODE_ECC (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 6)

#define JFFS2_INO_FLAG_PREREAD	  1	/* Do read_inode() for this one at 
					   mount time, don't wait for it to 
					   happen later */
#define JFFS2_INO_FLAG_USERCOMPR  2	/* User has requested a specific 
					   compression type */


struct jffs2_unknown_node
{
	/* All start like this */
	unsigned short magic;
	unsigned short nodetype;
	unsigned long totlen; /* So we can skip over nodes we don't grok */
	unsigned long hdr_crc;
};

struct jffs2_raw_dirent
{
	unsigned short magic;
	unsigned short nodetype;	/* == JFFS_NODETYPE_DIRENT */
	unsigned long totlen;
	unsigned long hdr_crc;
	unsigned long pino;
	unsigned long version;
	unsigned long ino;			/* == zero for unlink */
	unsigned long mctime;
	unsigned char nsize;
	unsigned char type;
	unsigned char unused[2];
	unsigned long node_crc;
	unsigned long name_crc;
};

/* The JFFS2 raw inode structure: Used for storage on physical media.  */
/* The uid, gid, atime, mtime and ctime members could be longer, but 
   are left like this for space efficiency. If and when people decide
   they really need them extended, it's simple enough to add support for
   a new type of raw node.
*/
struct jffs2_raw_inode
{
	unsigned short magic;      /* A constant magic number.  */
	unsigned short nodetype;   /* == JFFS_NODETYPE_INODE */
	unsigned long totlen;      /* Total length of this node (inc data, etc.) */
	unsigned long hdr_crc;
	unsigned long ino;         /* Inode number.  */
	unsigned long version;     /* Version number.  */
	unsigned long mode;        /* The file's type or mode.  */
	unsigned short uid;        /* The file's owner.  */
	unsigned short gid;        /* The file's group.  */
	unsigned long isize;       /* Total resultant size of this inode (used for truncations)  */
//	unsigned long atime;       /* Last access time.  */
	unsigned long mtime;       /* Last modification time.  */
//	unsigned long ctime;       /* Change time.  */
	unsigned long offset;      /* Where to begin to write.  */
	unsigned int  csize;       /* (Compressed) data size */
	unsigned int  dsize;	   /* Size of the node's data. (after decompression) */
	unsigned char compr;       /* Compression algorithm used */
	unsigned char usercompr;	  /* Compression algorithm requested by the user */
	unsigned short flags;	   /* See JFFS2_INO_FLAG_* */
	unsigned long data_crc;    /* CRC for the (compressed) data.  */
	unsigned long node_crc;    /* CRC for the raw inode (excluding data)  */
//	unsigned char data[csize];
};

union jffs2_node_union {
	struct jffs2_raw_inode i;
	struct jffs2_raw_dirent d;
	struct jffs2_unknown_node u;
};

#endif /* __LINUX_JFFS2_H__ */
