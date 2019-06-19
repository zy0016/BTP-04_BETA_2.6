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

#ifndef _SMSPUBCONTROL_H_
#define _SMSPUBCONTROL_H_

#define SMS_SETADDRESS         WM_USER + 105
#define SMS_SETCONTENT         WM_USER + 106
#define SMSN_DESTROY           WM_USER + 107

HWND CreateSMSEdit(HWND hWnd);

#endif //_SMSPUBCONTROL_H_