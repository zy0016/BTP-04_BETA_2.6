/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		watchdog.h
*
* Purpose :		
*  
* Author  : 		geng lei
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#define WATCHDOG_EN
//#undef  WATCHDOG_EN

typedef struct tagRecord
{
    unsigned long  lpnfWndProc;
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

typedef struct argrecord
{
    char *flag;
    char *buf;
    int  length;
}RECORDARG;

