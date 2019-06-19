
#ifndef _MBRECORDLIST_
#define _MBRECORDLIST_

typedef enum
{
    MBL_WS_DEFAULT   = 0, 
    MBL_WS_COUNT     = 1, 
    MBL_WS_NOVSCROLL = 2  
}MBRECORDLIST_STYLE; 

typedef enum
{
    MBRECORDLIST_RS_REFRESH = 1,
    MBRECORDLIST_RS_NONE    = 0 
}MBRECORDLIST_RS;

MBRECORDLIST_RS     CallMBLogListExtWindow(HWND hwndCall, HWND hMainWnd, int itype,MBRECORDLIST_STYLE MBRecordList_Style, BOOL flag);

#endif
