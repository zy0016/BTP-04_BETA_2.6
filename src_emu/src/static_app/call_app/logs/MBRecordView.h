
#ifndef _MBRECORDVIEW_
#define _MBRECORDVIEW_

#include "RcCommon.h"

typedef enum
{
    MBRECORD_RS_MODIFY      = 2,    
    MBRECORD_RS_DIALUP      = 1,    
    MBRECORD_RS_NONE        = 0,    
    MBRECORD_RS_DELETE      = -1,   
    MBRECORD_RS_CREATEFAIL  = -2    
}MBRECORD_RS;

#define MBRECORD_TITLEMAXLEN            20
#define MBRECORD_CLEW_NUMMAXLEN         10
#define MBRECORD_CLEW_NAMEMAXLEN        10
#define MBRECORD_CLEW_TIMEMAXLEN        10
#define MBRECORD_CLEW_DURATIONMAXLEN    10
#define MBRECORD_CLEW_UNKNOWPEOPLE      10
typedef struct
{
    char cTitle[MBRECORD_TITLEMAXLEN + 1];              
    char cNumber[MBRECORD_CLEW_NAMEMAXLEN + 1];         
    char cName[MBRECORD_CLEW_NAMEMAXLEN + 1];           
    char cTime[MBRECORD_CLEW_TIMEMAXLEN + 1];           
    char cDuration[MBRECORD_CLEW_DURATIONMAXLEN + 1];   
}TITLEINFO;

typedef enum
{
    MBV_WS_NUMBER   = 1,
    MBV_WS_NAME     = 2,
    MBV_WS_TIME     = 4,
    MBV_WS_DURATION = 8 
}ITEMSTYLE;

typedef enum
{
    TS_NORMAL,          
    TS_NORMAL_WEEK,     
    TS_NORMAL_WEEK_M,   
    TS_NORMAL_M         
}TS_TIMESTYLE;

typedef struct
{
    HWND            hWnd;           
    unsigned int    index;          
    MBRECORDTYPE    MBRecordType;   
    TITLEINFO       TitleInfo;      
    DWORD           ItemStyle;      
    TS_TIMESTYLE    TimeStyle;      
    char            cUnknow[MBRECORD_CLEW_UNKNOWPEOPLE + 1];
}MBRECORD_VIEW;

MBRECORD_RS CallMBLogViewWindow(MBRECORD_VIEW * pMBRecord_Type);

#endif
