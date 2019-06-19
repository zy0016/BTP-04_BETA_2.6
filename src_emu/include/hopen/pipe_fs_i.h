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
 * $Source: /cvs/hopencvs/src/include/hopen/pipe_fs_i.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_PIPE_FS_I_H
#define _HOPEN_PIPE_FS_I_H

#include <hopen/inode.h>

struct pipe_inode_info {
	char * base;
	struct wait_queue * wait;
//	struct mutex write_mutex;
//	unsigned int lock;
	unsigned int start;
	short rd_openers;
	short wr_openers;
	short readers;
	short writers;
};

#define PIPE_WAIT(inode)	((*(struct pipe_inode_info *)(inode).i_data).wait)
#define PIPE_BASE(inode)	((*(struct pipe_inode_info *)(inode).i_data).base)
#define PIPE_START(inode)	((*(struct pipe_inode_info *)(inode).i_data).start)
#define PIPE_LEN(inode)		((inode).i_size)
#define PIPE_RD_OPENERS(inode)	((*(struct pipe_inode_info *)(inode).i_data).rd_openers)
#define PIPE_WR_OPENERS(inode)	((*(struct pipe_inode_info *)(inode).i_data).wr_openers)
#define PIPE_READERS(inode)	((*(struct pipe_inode_info *)(inode).i_data).readers)
#define PIPE_WRITERS(inode)	((*(struct pipe_inode_info *)(inode).i_data).writers)
#define PIPE_SIZE(inode)	PIPE_LEN(inode)

//#define PIPE_WRITE_MUTEX(inode)	((*(struct pipe_inode_info *)(inode).i_data).write_mutex)
//#define PIPE_LOCK(inode)	((*(struct pipe_inode_info *)(inode).i_data).lock)

#define PIPE_EMPTY(inode)	(PIPE_SIZE(inode)==0)
#define PIPE_FULL(inode)	(PIPE_SIZE(inode)==PIPE_BUF)
#define PIPE_FREE(inode)	(PIPE_BUF - PIPE_LEN(inode))
#define PIPE_END(inode)		((PIPE_START(inode)+PIPE_LEN(inode))&(PIPE_BUF-1))
#define PIPE_MAX_RCHUNK(inode)	(PIPE_BUF - PIPE_START(inode))
#define PIPE_MAX_WCHUNK(inode)	(PIPE_BUF - PIPE_END(inode))

#endif
