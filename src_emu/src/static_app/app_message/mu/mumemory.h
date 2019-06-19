/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef _MU_MEMORY_H_
#define _MU_MEMORY_H_
 
extern void MM_Construct(void);
extern void MM_Destroy(void);
extern void* MM_malloc (void);
extern void MM_free (void* memblock);

#endif //_MU_MEMORY_H_
