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

#ifndef _VCOMM_H_
#define _VCOMM_H_

#ifdef __cplusplus
}
#endif

/* Initialize Vcomm */
void VCOMM_Init (void * hInstance);

void * VcomOpen (const char * name, int async);
void VcomClose (void * vcom_handle );
int VcomSetSpeed (void * vcom_handle, int speed );
int VcomGetSpeed (void * vcom_handle );
int VcomSetcallback (void * vcom_handle, void (*func)(void * param, int event), void * param);
int VcomRead (void * vcom_handle, void * pbuf, int buflen);
int VcomWrite (void * vcom_handle, const void * pbuf, int buflen);
int VcomDumpBuffer(void* vcom_handle);

#ifdef __cplusplus
}
#endif

#define VCOMM_DATAIN    1
#define VCOMM_DATAOUT	2
#define VCOMM_MODEM	3

#endif // _VCOMM_H_

