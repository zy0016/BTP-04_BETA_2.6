/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : SIEMENS
 *            
\**************************************************************************/


#ifndef _DIAL_DIALER_H_
#define _DIAL_DIALER_H_


/*--- int DIALER_SelConnect( HWND, UINT, int SelectType, void *pDialInfo ); ---*/

// SelectType:
#define		DIALDEF_WAP			1
#define		DIALDEF_MAIL		2
#define		DIALDEF_SELFGPRS	3
#define		DIALDEF_SELFDATA	4

#define DIAL_MAXSTRLEN		31		// ISP name maxmum length

// when SelectType = DIALDEF_SELFGPRS
typedef struct TAGDIALER_GPRSINFO{
	char	APN[DIAL_MAXSTRLEN +1];			// APN
	char	UserID[DIAL_MAXSTRLEN +1];		// user ID
	char	PassWord[DIAL_MAXSTRLEN +1];	// password

}DIALER_GPRSINFO, *PDIALER_GPRSINFO;

// when SelectType = DIALDEF_SELFDATA
typedef struct TAGDIALER_DATAINFO{
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// phone number
	char	UserName[DIAL_MAXSTRLEN +1];	// user name
	char	PassWord[DIAL_MAXSTRLEN +1];	// password

}DIALER_DATAINFO, *PDIALER_DATAINFO;



/*---------------- DIALER_GetLineState() return value ------------------*/

#define DIALER_INVALIDSTAGE		0		// idle state
#define DIALER_CONNECTSTAGE		1		// connected state
#define DIALER_PPPSTAGE			2		// PPP negotiation
#define DIALER_NETWORKSTAGE		3		// net state
#define DIALER_HANGUPSTAGE		4		// hangup



/*------------ run time notify message -------------*/

// dial mode wParam
#define DIALER_RUNMODE_DATA		0		//data mode
#define DIALER_RUNMODE_GPRS		1		//GPRS mode

//lParam
enum Dialer_Run_MsgNotice {

// Function Return Value, not message
	DIALER_REQ_ACCEPT =0	,		// recept request
	DIALER_REQ_EXIST		,		// already dial 
	DIALER_REQ_REFUSE		,		// refuse request
	DIALER_MCH_FAILURE		,		// alloc multichinel failed
	DIALER_INVALID_ISP		,		// invalide ISP

// Normal Stage Message
	DIALER_RUN_DIALING		,		// Dial Stage
	DIALER_RUN_BUSYRETRY	,		// ReDial when Busy
	DIALER_RUN_LOGINING		,		// Login Stage
	DIALER_RUN_NEGOTIATING	,		// Negotiate Stage
	DIALER_RUN_HANGUPING	,		// Hangup Stage
	DIALER_RUN_CANCELING	,		// Try to Stop Dialing

// Result Message
	DIALER_RUN_NETCONNECT	,
	DIALER_RUN_NETBROKEN	,
	DIALER_RUN_NET_ABNORMAL	,
	DIALER_RUN_NETPAUSE		,
	DIALER_RUN_NETRESUME	,

// Error Code, Use Function:Dialer_GetError()
	DIALER_RUN_DIALFAIL		,		// dial failed
	DIALER_RUN_MODEMDUMB	,		// modem no echo
	DIALER_RUN_BUSYFAIL		,		// line busy
	DIALER_RUN_NODIALTONE	,		// no dial tone
	DIALER_RUN_NOCARRIER	,		// no carrier

	DIALER_RUN_LOGINFAIL	,		// long in failed
	DIALER_RUN_PPPFAIL		,		// PPP negotiate failed
	DIALER_RUN_PAPFAIL		,		// PAP negotiate failed
	DIALER_RUN_HANGUPFAIL	,		// hangup failed

	DIALER_RUN_USERCANCEL	,		// user cancel
	DIALER_RUN_SCREXP_ERROR	,		// script explainer error
	DIALER_RUN_SIGNAL_ERROR	,		// signal errror
	DIALER_RUN_GENERAL_ERROR,		// modual errror

	DIALER_RUN_NO_ERROR,

};


/*--------------- 接口函数声明 ------------------------*/

int DIALER_Initialize( void );
int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pDialInfo );
int DIALER_HangUp( void );
int DIALER_DialCancel( void );
int DIALER_GetLineState( void );
int Dialer_GetError( void );


int DIALER_NotifyRegister( HWND hWnd, int msg );
int DIALER_NotifyUNRegister( HWND hWnd, int msg );
int DIALER_CompareInfo( int SelectType, void *pSrcInfo );


#endif /* _DIAL_DIALER_H_ */
