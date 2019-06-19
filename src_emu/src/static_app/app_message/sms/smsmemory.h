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
#ifndef _SMS_MEMORY_H_
#define _SMS_MEMORY_H_
 
extern void SMSMM_Construct(void);
extern void SMSMM_Destroy(void);
extern void* SMSMM_malloc (void);
extern void SMSMM_free (void* memblock);

#endif //_SMS_MEMORY_H_
