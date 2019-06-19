/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : SmartPhone Version_NOWIN
 *            
\**************************************************************************/

#include "window.h"
#include "debug.h"
#include "string.h"

#include "dialer.h"
#include "dialmain.h"
#include "setting.h"

int dial_GetEnvelope( int SelType, void *pSrcInfo, PDIALINFO pGetInfo );
static int dial_GetCommonInfo( int SelType, PDIALINFO pGetInfo );
static int dial_GetSelfDataInfo( void * pDial_Data, PDIALINFO pGetInfo );
static int dial_GetSelfGprsInfo( void * pDial_Gprs, PDIALINFO pGetInfo );
extern int Sett_GetDataCallAutoDisconn(void);

/*---------------------------------------------------------------
 *
 * dial_GetEnvelope
 *
 * retval:	0:success,	-1:Failure
 *
 *---------------------------------------------------------------*/

int dial_GetEnvelope( int SelType, void *pSrcInfo, PDIALINFO pGetInfo )
{
	pGetInfo->Valid = 0;

	// Repeat Dial Times
	pGetInfo->RepeatDialTimes = DIAL_DEFAUTORETRY;

	// Auto Hangup Timeout
	//pGetInfo->AutoHangupTimeout = Sett_GetDataCallAutoDisconn();

	switch(SelType)
	{
	case DIALDEF_WAP:
	case DIALDEF_MAIL:

		if( pSrcInfo )
			return -1;
		return dial_GetCommonInfo(SelType, pGetInfo);

	case DIALDEF_SELFGPRS:

		if (!pSrcInfo)
			return -1;
		return dial_GetSelfGprsInfo(pSrcInfo, pGetInfo);

	case DIALDEF_SELFDATA:

		if (!pSrcInfo)
			return -1;
		return dial_GetSelfDataInfo(pSrcInfo, pGetInfo);
	}
	return -1;
}


static int dial_GetSelfGprsInfo( void * pDial_Gprs , PDIALINFO pDialInfo )
{
	PDIALER_GPRSINFO pDial_GprsInfo;

	pDialInfo->SelMode = DIALER_RUNMODE_GPRS;

	pDial_GprsInfo = pDial_Gprs;
	strncpy( pDialInfo->PhoneNum, pDial_GprsInfo->APN, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->UserName, pDial_GprsInfo->UserID, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->PassWord, pDial_GprsInfo->PassWord, DIAL_MAXSTRLEN+1 );

	pDialInfo->Valid = 1;
	return 0;
}


static int dial_GetSelfDataInfo( void * pDial_Data , PDIALINFO pDialInfo )
{
	PDIALER_DATAINFO pDial_DataInfo;

	pDialInfo->SelMode = DIALER_RUNMODE_DATA;

	// Auto Hangup Timeout
	pDialInfo->AutoHangupTimeout = Sett_GetDataCallAutoDisconn();

	pDial_DataInfo = pDial_Data;
	strncpy( pDialInfo->PhoneNum, pDial_DataInfo->PhoneNum, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->UserName, pDial_DataInfo->UserName, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->PassWord, pDial_DataInfo->PassWord, DIAL_MAXSTRLEN+1 );

	pDialInfo->Valid = 1;
	return 0;
}


static int dial_GetCommonInfo( int SelType , PDIALINFO pDialInfo )
{
	UDB_ISPINFO		ISP_Info;

	if( !Setup_ReadDefDial(&ISP_Info, SelType ) )
		return -1;

	pDialInfo->SelMode = ISP_Info.DtType;
	if (1==pDialInfo->SelMode) 
		pDialInfo->AutoHangupTimeout = 0; //GPRS方式不按计时挂断
	else
		// Auto Hangup Timeout
		pDialInfo->AutoHangupTimeout = Sett_GetDataCallAutoDisconn();

	strncpy( pDialInfo->PhoneNum, ISP_Info.ISPPhoneNum1, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->UserName, ISP_Info.ISPUserName, DIAL_MAXSTRLEN+1 );
	strncpy( pDialInfo->PassWord, ISP_Info.ISPPassword, DIAL_MAXSTRLEN+1 );

	pDialInfo->Valid = 1;
	return 0;
}



/* End of program */
