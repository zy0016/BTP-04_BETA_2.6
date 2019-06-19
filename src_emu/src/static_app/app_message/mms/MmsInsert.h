/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mms
 *
 * Purpose  : function declaration
 *            
\**************************************************************************/

#ifndef   MMSINSERT_
#define   MMSINSERT_
/*********************************************************************\
*
*           定义常量
*      
**********************************************************************/
#define RTN_ATTATCH  5
#define RTN_SLIDE    6


/*********************************************************************\
*
*           structure
*      
**********************************************************************/
typedef struct tagINSERTINFO 
{
    BOOL bHasImg;
    BOOL bHasSnd;
    BOOL bTxtFull;
    BOOL bSlideFull;
} INSERTINFO;
/*********************************************************************\
*
*           function declaration
*      
**********************************************************************/
void CallInsertBrow(HWND hWndParent, UINT returnmsg, INSERTINFO info);

#endif  MMSAPI_
