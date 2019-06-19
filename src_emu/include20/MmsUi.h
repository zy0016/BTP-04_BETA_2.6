/**************************************************************************\
  *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
* Model   :		mms ui
 *
 * Purpose :  the interface for other apps
 *  
 * Author  :  
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Workfile::                                                      $
 * $Revision::    $     $Date::			                            $
 * 
\**************************************************************************/
#define  MMS_CALLEDIT_NEW		0	//new 
#define  MMS_CALLEDIT_OPEN		1	//edit draft
#define  MMS_CALLEDIT_REPLY		2	//reply mms
#define  MMS_CALLEDIT_REPLYALL	3   //reply all 
#define  MMS_CALLEDIT_TRAN		4	//forward mms
#define  MMS_CALLEDIT_IMAGE     5   //send image as mms
#define  MMS_CALLEDIT_SOUND     6   //send audio as mms
#define  MMS_CALLEDIT_TEXT      7   //send text as mms's text
#define  MMS_CALLEDIT_MOBIL     8   //mobile num as mms's receiver
#define  NOTI_CALLEDIT_REPLY	9	//»Ø¸´notification
//#define  MMS_CALLEDIT_TEMPLATE  10  //send template
#define  MMS_CALLEDIT_TEMPOPEN  11  //edit template
#define  MMS_CALLEDIT_QTEXT     12   
#define  MMS_CALLEDIT_VCARD     13
#define  MMS_CALLEDIT_VCAL		14
#define  MMS_CALLEDIT_MULTIIMAGE        15
/*********************************************************************\
* Function	   APP_EditMMS
* Purpose      call mms to send pictures or sound etc.
* Params	   hwnd: the handle of the current window
               msg£º the return message
               nKind£ºrefer to the macro defination
               name£º the filename 
* Return	 	void
**********************************************************************/
void APP_EditMMS(HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
                    PSTR  InsertFileName);
