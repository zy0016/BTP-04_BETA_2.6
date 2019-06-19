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

#ifndef _SENDMSGBUG_

#define _SENDMSGBUG_

#include <window/window.h>
typedef struct tagRecord
{
    WNDPROC  lpnfWndProc;
    long        message;    /* Message command                  */
    long      wParam;     /* First parameter of message       */
    long      lParam;     /* Second parameter of message      */
}RECORDMSG, *PRECORDMSG;
typedef struct tagSendMsgBuf
{
    unsigned int nPosition;
    char         pRet[12];
    RECORDMSG   Msg[10];
}SENDMSGBUF;

SENDMSGBUF SendMsgBuf;

#endif
