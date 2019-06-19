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

#ifndef _HMAP_ASYNC_H_
#define _HMAP_ASYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

void * hm_AddAsyncEvent (int fd, int (*handle)(void * param, int event), void * param);
int hm_DeleteAsyncEvent (void * handle);
int hm_SetAsyncSelect (void * handle, unsigned mask);

#define ASYNC_DATAIN    0x01
#define ASYNC_DATAOUT   0x02
#define ASYNC_ERROR     0x04

int hm_PollAsyncEvent (long timeout);

#ifdef __cplusplus
}
#endif

#endif // _HMAP_ASYNC_H_

