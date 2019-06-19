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

#include "plx_pdaex.h"

#include "string.h"
#include "version.h"

#define		SOFTWAREVERSION		"OS:Hopen 3.0 \nAPPVersion:Alpha2.0 \n2005.10.18"
#define		HARDWAREVERSION		"Intel bulverde"

int		GetSoftWareVersion(char * strVersion)//获得软件版本
{
	if(strVersion != NULL)
	{
		strcpy(strVersion,BTP_04_VERSION);
		strVersion[6] = 0x11;
		strVersion[11] = 0x11;
		//UTF8ToMultiByte(CP_ACP,0,BTP_04_VERSION,strlen(BTP_04_VERSION),strVersion, 49,NULL,NULL);
		return (strlen(strVersion));
	}
	else
		return (strlen(BTP_04_VERSION));
}
int		GetHardWareVersion(char * strVersion)//获得硬件版本
{
	if(strVersion != NULL)
	{
		strcpy(strVersion,HARDWAREVERSION);
		strVersion[6] = 0x11;
		strVersion[11] = 0x11;
		//UTF8ToMultiByte(CP_ACP,0,BTP_04_VERSION,strlen(BTP_04_VERSION),strVersion, 49,NULL,NULL);
		return (strlen(strVersion));
	}
	else
		return (strlen(HARDWAREVERSION));
}
int		GetGSMVersion(char * strVersion)//获得软件版本
{
	if(strVersion != NULL)
	{
		strcpy(strVersion,"MC55 Revision 02.50");
		return (strlen("MC55 Revision 02.50"));
	}
	else
		return (strlen("MC55 Revision 02.50"));
}
int		GetGPSVersion(char * strVersion)//获得软件版本
{
	if(strVersion != NULL)
	{
		strcpy(strVersion,"MC55 Revision 02.50");
		return (strlen("MC55 Revision 02.50"));
	}
	else
		return (strlen("MC55 Revision 02.50"));
}
int		GetBluetoothVersion(char * strVersion)//获得软件版本
{
	if(strVersion != NULL)
	{
		strcpy(strVersion,BTP_04_VERSION);
		strVersion[6] = 0x11;
		strVersion[11] = 0x11;
		//UTF8ToMultiByte(CP_ACP,0,BTP_04_VERSION,strlen(BTP_04_VERSION),strVersion, 49,NULL,NULL);
		//MultiByteToGSM(BTP_04_VERSION, -1, (LPWSTR)strVersion, 29, NULL, FALSE);
		return (strlen(strVersion));
	}
	else
		return (strlen(BTP_04_VERSION));
}
