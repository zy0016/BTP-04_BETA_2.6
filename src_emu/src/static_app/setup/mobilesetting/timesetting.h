/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setup
 *
 * Purpose  : 
 *
 * Autor    :
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#ifndef     _TIMESETTING_H
#define     _TIMESETTING_H

#define     IDC_BUTTON_QUIT             104
#define     IDC_BUTTON_SET              105
#define     IDC_TIMESRC                 109
#define     IDC_TIME                   110
#define     IDC_TIMEZONE               111
#define     IDC_TIMEFORM               112
#define     IDC_DATE                   113
#define     IDC_DATEFORM               114

typedef struct _PDASetRegionTime
{
	int   year;
	int   month;
	int   day;
	int   hour;
	int   minute;
	int   zone;
	float difftime;
}PDACLOCK_SETREGIONTIME;




#define     MAX_ITEMNUM                 6
#define     TIMER_REFRESH           WM_USER + 100
#define     TIMER_ID     WM_USER + 101

#endif
