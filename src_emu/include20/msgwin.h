#ifndef __MSG_WIN
#define __MSG_WIN
/*************************************************************************
                                                                          
   SEF CONFIDENTIAL AND PROPRIETARY                                       
                                                                          
   This source is the sole property of SEF Inc. Reproduction or           
   utilization of this source in whole or in part is forbidden without    
   the written consent of SEF Inc.                                        
                                                                          
   (c) Copyright SEF Inc.,         2002.  All Rights Reserved.            
                                                                          
**************************************************************************
                                                                          
   MsgWin.h : Header file of MsgWin.c                      
                                                                          
**************************************************************************
   Date       Author           Reference
   ========   =============    ==========================
   02-06-22   Kevin Yuan       CR - BJC00036
              Show tip message with a modalless window
              - Initially created

*************************************************************************/
/*---------- Dependencies-------- --------------------------------------*/

/*---------- Global Definitions and Declarations -----------------------*/

/*---------- Constant / Macro Definitions ------------------------------*/

/*---------- Type Declarations -----------------------------------------*/

/*---------- Variable Declarations -------------------------------------*/

/*---------- function Declarations -------------------------------------*/
/*************************************************************************
  Function   :MsgWin
--------------------------------------------------------------------------
  Description:To show a message box with user specified prompt,caption and 
              time
--------------------------------------------------------------------------
  Input      :szPrompt     ----    address of text in message box
              szCaption	   ----    address of title of message box
              nTimeout	   ----    destroy the window after nTimeout*100 
			           millisecond.If nTimeout = 0 never destroy
                                   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
extern void		MsgWin( char *szPrompt, char *szCaption, int nTimeout );
extern void		MsgWinEx( HWND hCall, char *szPrompt, char *szCaption, int nTimeout );

//#define			WM_MSGWINOK				WM_USER+4321
#endif /* End of #ifndef __MSG_WIN */
