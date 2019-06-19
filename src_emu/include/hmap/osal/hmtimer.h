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

#ifndef _HMAP_SYSTIME_H_
#define _HMAP_SYSTIME_H_

#ifdef __cplusplus
extern "C" {
#endif

struct daytime {
    unsigned short  year;
    unsigned char   month;
    unsigned char   date;
    unsigned char   hour;
    unsigned char   min;
    unsigned char   sec;
    unsigned char   day;        /* Day in week */
};

unsigned long hm_GetSystemClock(void);
void hm_GetSystemDayTime(struct daytime * p);
void hm_GetSystemLocalTime(struct daytime * p);

typedef struct hm_timer * HMTIMERHANDLE;

HMTIMERHANDLE hm_SetTimer (int timeout, void (*callback)(void * param), void * param);
void hm_KillTimer (HMTIMERHANDLE htimer);
long hm_GetTimeout (void);

#ifdef __cplusplus
}
#endif

#endif // _HMAP_SYSTIME_H_
