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
 * Author  :    
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hal-armv/map.h,v $
 * $Name:  $
 *
 * $Revision: 1.1 $     $Date: 2003/09/03 01:07:37 $
 * 
\**************************************************************************/

#ifndef _HAL_MAP_H_
#define _HAL_MAP_H_

/*
 * Domain numbers
 *
 *  DOMAIN_IO     - domain 0 includes all IO only
 *  DOMAIN_KERNEL - domain 0 includes all kernel memory only
 *  DOMAIN_USER   - domain 0 includes all user memory only
 */
#define DOMAIN_USER		0
#define DOMAIN_KERNEL	0
#define DOMAIN_IO		0

struct map_desc {
	unsigned long virtual;
	unsigned long physical;
	unsigned long length;
	int domain:4,
	    prot_read:1,
	    prot_write:1,
	    cacheable:1,
	    bufferable:1,
	    last:1;
};

#define LAST_DESC	{0,0,0,0,0,0,0,0,1}

extern void iotable_init(struct map_desc *);

#endif //_HAL_MAP_H_
