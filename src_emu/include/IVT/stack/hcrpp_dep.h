/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hcrp_dep.h
Abstract:
	This file includes the definition of functions in the hcrp_dep.c.
Author:
    Ding Wenfang
Revision History: 2002.2
---------------------------------------------------------------------------*/
#ifndef HCRPP_DEP_H
#define HCRPP_DEP_H

/*--------------------------- dongle version ----------------------------*/

DWORD OpenLPT(void);
UCHAR Write2LPT(DWORD hdl,UCHAR * pBuf,WORD bytesToWrite);
UCHAR GetLPTStatus(DWORD hdl, UCHAR * lptStatus);
UCHAR * Get1284ID(DWORD hdl);
UCHAR ResetLpt(DWORD hdl);
UCHAR CloseLPT(DWORD hdl);

#endif
