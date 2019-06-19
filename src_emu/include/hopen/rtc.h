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
 * $Source: /cvs/hopencvs/src/include/hopen/rtc.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/06/16 06:09:36 $
 * 
\**************************************************************************/

#ifndef _HOPEN_RTC_H
#define _HOPEN_RTC_H

#include <hopen/ioctl.h>

/*
 * The struct used to pass data via the following ioctl. Similar to the
 * struct tm in <time.h>, but it needs to be here so that the kernel 
 * source is self contained, allowing cross-compiles, etc. etc.
 */

struct rtc_time {
	int tm_msec;
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;	/* Day of week.	[0-6] */
};

#define  RTC_IOC_MAGIC        	  'p'

#define  RTC_AIE_OFF       	  _IO(RTC_IOC_MAGIC, 0)
#define  RTC_AIE_ON          	  _IO(RTC_IOC_MAGIC, 1)
#define  RTC_ALM_READ          	  _IOR(RTC_IOC_MAGIC, 2, struct rtc_time)
#define  RTC_RD_TIME          	  _IOR(RTC_IOC_MAGIC, 3, struct rtc_time)
#define  RTC_ALM_SET          	  _IOW(RTC_IOC_MAGIC, 4, struct rtc_time)
#define  RTC_SET_TIME          	  _IOW(RTC_IOC_MAGIC, 5, struct rtc_time)
#define  RTC_EPOCH_READ		  _IOR(RTC_IOC_MAGIC, 6, long)
#define  RTC_EPOCH_SET            _IOW(RTC_IOC_MAGIC, 7, long)
#define  RTC_SET_TIMER			_IOW(RTC_IOC_MAGIC, 8, long)
#define  RTC_DEL_TIMER			_IOW(RTC_IOC_MAGIC, 9, long)
#define  RTC_READ_TIMER			_IOR(RTC_IOC_MAGIC, 10, long)
#define  RTC_READ_EVENT			_IOR(RTC_IOC_MAGIC, 11, long)

#define  ENABLE_WATCHDOG_REBOOT       _IO(RTC_IOC_MAGIC, 20)
#define  WATCHDOG_TIMEOUT_VALUE       _IO(RTC_IOC_MAGIC, 21)
#define  DISABLE_WATCHDOG    _IO(RTC_IOC_MAGIC, 22)
#define  ENABLE_WATCHDOG	  _IO(RTC_IOC_MAGIC, 23)
#define  CLEAR_STARTUPFLAG 	  _IO(RTC_IOC_MAGIC, 24)
#define  SET_STARTUPFLAG	  _IO(RTC_IOC_MAGIC, 25)
#define  GET_STARTUPFLAG	  _IO(RTC_IOC_MAGIC, 26)
#define  WATCHDOG_RECORD      _IO(RTC_IOC_MAGIC, 27)
#define	 RTC_START_TIMER	  _IO(RTC_IOC_MAGIC, 8)
#define RTC_STOP_TIMER		   _IO(RTC_IOC_MAGIC, 9)
#define RTC_RESET_TIMER		   _IO(RTC_IOC_MAGIC, 10)
#define RTC_GET_TIMER		   _IOR(RTC_IOC_MAGIC, 11, struct rtc_time)
#define GET_WATCHDOG_STATE	   _IO(RTC_IOC_MAGIC, 12)

int  rtc_register_handler(void (*irq_handler)());
int  rtc_unregister_handler(void (*irq_handler)());

#endif /* _HOPEN_RTC_H */
