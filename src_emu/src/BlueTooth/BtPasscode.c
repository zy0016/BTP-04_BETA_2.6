/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "imesys.h"
#include "pubapp.h"

#include "BtPasscode.h"
#include "BtString.h"

BOOL BtEnterPinCode(BTDEVHDL dev_hdl)
{
	char szDeviceName[64]; 
	char szComEnterPass[128];       //组合的字符串Enter Bluetooth passcode
	char szCurPasscode[16];

	int i,j;
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];
	char szTemp[32];
	char *p;
	
    BTUINT16 iDevNameLen;   

	iDevNameLen = BTSDK_DEVNAME_LEN;

	memset(szDeviceName,0,64);
    memset(szComEnterPass,0,128);
	memset(szCurPasscode,0,16);

// 	printf("\n$$$$$$$$$$$$$$Pincode Callback$$$$$$$$$$$$$$$$$$\n");
	
	if(Btsdk_GetRemoteDeviceName(dev_hdl, szDeviceName, &iDevNameLen)!=BTSDK_OK)
	{
// 		Btsdk_UpdateRemoteDeviceName(dev_hdl, szDeviceName, &iDevNameLen);
		GetDeviceBdAddr(dev_hdl,bd_addr);

		p=szTemp;
		
		for(i = 5; i > 0; i--)
		{
			j=sprintf(p,"%02X:",bd_addr[i]);
			p=p+j;
		}
		
		sprintf(p,"%02X", bd_addr[0]);
		
		strcpy(szDeviceName,szTemp);
	}

    printf("\nRemote DeviceName=%s\n",szDeviceName);

//	if(strlen(szDeviceName)!=0)  //如果能够取得设备名显示设备名
//	{	
		strcpy(szComEnterPass,szDeviceName);
		
		strcat(szComEnterPass,IDP_BT_STRING_BTENTERPASSCODE);
//	}
//	else                        //如果取不到设备名不显示设备名
//      strcpy(szComEnterPass,IDP_BT_STRING_BTENTERPASSCODENODEV);
        

	if(SSPLXVerifyPassword(NULL, NULL, szComEnterPass, 
		szCurPasscode, MAKEWPARAM(1,16), IDP_BT_BUTTON_OK, IDP_BT_BUTTON_CANCEL, -1))
	{
       Btsdk_SetRemoteDevicePinCode(dev_hdl, szCurPasscode, (BTUINT16)strlen(szCurPasscode));
	}
    else
	{
       Btsdk_SetRemoteDevicePinCode(dev_hdl, "", 0);
	}

	return TRUE;

}
