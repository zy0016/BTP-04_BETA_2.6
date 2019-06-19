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
#ifndef  CODE_H
#define  CODE_H

#include    "setting.h"

#define CODEFILENAME  "/mnt/flash/codefile.ini"
#define     SN_PWS                  "Password"      
#define     KN_PWS_PHONE            "kn_pws_phone"  
#define     KN_PWS_OPEN             "kn_pws_open"
#define     KN_PWS_LOCKPERIOD       "kn_pws_lockperiod" 
#define     KN_PMS_PS				"kn_ps" 
#define		KN_PMS_PHONELOCK        "kn_pws_phonelock" 
#define     KN_PWS_PSCODE			"kn_pscode"
#define		KN_PWS_PSDATA			"kn_psdata"
#define     KN_PWS_NETSIMLOCK       "kn_netsimlock"
#define     KN_PWS_NETSIMLOCKCODE   "kn_netsimlockcode"
#define     KN_PWS_NCK               "kn_nck"
#define     KN_PWS_SPSIMLOCK         "kn_spsimlock"
#define     KN_PWS_SPCODE            "kn_spcode"
#define		KN_PWS_SPCK              "kn_spck"
#define     KN_PWS_NCKENABLE         "kn_nck_enable"
#define     KN_PWS_SPCKENABLE        "kn_spck_enable"
#define     KN_NCK_REMAIN            "kn_nck_remain"
#define		KN_SPCK_REMAIN           "kn_spck_remain"
#define     KN_NCK_BLOCK             "kn_nck_block"
#define     KN_SPCK_BLOCK            "kn_spck_block"
#define     KN_SL_OPRNAME            "kn_sl_oprname"

#define     PASSWORD_DEFAULT        "0000"   
#define     LOCKPEROID              "0"     

#define     SWITCH_PINCODE_REQUEST  "Please switch PIN \r\ncode request on"
#define     PINCODE_CHANGED         "PIN code\r\nchanged"
#define     WRONG_PINCODE           "PIN code\r\nwrong"
#define     CODESNOTMATCH           "Codes do\r\nnot match"
#define     PIN_BLOCKED             "PIN blocked"
#define     PIN2CODE_CHANGED        "PIN2 code\r\nchanged"
#define     PIN2_BLOCKED            "PIN2 blocked"
#define		WRONG_PIN2				"PIN2 code\r\nwrong"
#define     WRONG_PUKCODE           "PUK code\r\nwrong"
#define     WRONG_PUK2CODE          "PUK2 code\r\nwrong"
#define		FEATURE_NOTAVAILABLE    "Feature not\r\navailable on SIM"
#define		PHONELOCK_CHANGED       "Phone lock code\r\nchanged"
#define		WRONG_PHONELOCK         "Phone lock\r\ncode wrong"
#define     READING_SIMCARD         "Reading SIM card.\r\nPlease wait..."
#define     INSERT_SIMCARD          "Insert SIM card"
#define     SIM_BLOCKED             "SIM blocked"
#define     PUK2_BLOCKED            "PUK2 blocked"
#define		NOTDISABLE_PINREQUEST   "Cannot disable\r\nPIN request"
#define     FIXEDDIAL_DELETE        "Deleted"
#define     NOTALLOWEDNOSIM         "Not allowed.\r\nNo SIM card on phone."
#define     NOTDEFINDNAME           "Please define name or number"

#define     IDS_OK                  "OK"
#define		IDS_BACK				"Back"
#define     IDS_CANCEL              "Cancel"
#define     IDS_ON					"On"
#define     IDS_OFF					"Off"
#define     IDS_YES                 "Yes"
#define     IDS_NO                  "No"

#define		PUK2CODE                "Enter PUK2 code:"
#define		PIN2CODE				"Enter PIN2 code:"
#define		PHONELOCK               "Enter Phone\r\nlock code:"
#define     PUKCODE					"Enter PUK code:"
#define		PINCODE					"Enter PIN code"

#define     PIN						"PIN code"
#define		PIN2_CODE				"PIN2 code"
#define		PUK						"PUK code"
#define		PUK2_CODE				"PUK2 code"
#define		PHONELOCKCODE            "Phone lock code"

#define     TITLECAPTION            "Security"
#define     SECURITYQUIT            "Back"
#define     SECURITYCODE            "Codes and locks"
#define     SECURITYPINREQUEST      "PIN code request"
#define     SECURITYPIN             "PIN code"
#define     SECURITYPIN2            "PIN2 code"
#define     SECURITYPHOENLOCK       "Phone lock"
#define     SECURITYSIMLOCK         "Lock if SIM changed"
#define     SECURITYFIXEDDIAL       "Fixed dialing"

#define     PUKNUMS                  10      //max limit for input puk
#define     EDITMAXLEN               8   
#define     EDITMINLEN               4   
#define     EDITX                    20
#define     PINCODE_MAXLIMIT         3 

#define		REQUEST_PIN				1
#define		REQUEST_PIN2			2
#define		REQUEST_PUK				3
#define		REQUEST_PUK2			4
#define		REQUEST_LOCKCODE		5


#define		WIN_X					0
#define		ITEMWINDTH				PLX_WIN_WIDTH
#define		ITEMHIGH				50
#define		WIN_Y					0

#define		CALLBACK_NEWCODE        WM_USER + 100 //callback from new code view 
#define     WM_PIN_CONFIRM          WM_USER + 101
#define     WM_PIN_QUERY            WM_USER + 102
#define     CALLBACK_CODE_REMAIN    WM_USER + 103 //callback code remain times
#define     CALLBACK_CODE           WM_USER + 105 
#define		CALLBACKFDSET           WM_USER + 201//FD set callback
#define		GETPSCODE				WM_USER + 202

typedef struct
{
	BOOL 	CHV1Status;			//CHV1 status
	BOOL 	blockCHV1;		//Unblock CHV1 status
	BOOL  	CHV2Status;			//CHV2 status
	BOOL	blockCHV2;		//Unblock CHV2 status
	BOOL    blockPUK1;    //PUK1 blockde
	BOOL    blockPUK2;   //PUK2 blocked
}PINSstatus;

#define DEFAULT_TIMEOUT  20


#endif
