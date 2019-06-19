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

#include "pthread.h"
#include "mullang.h"
#include "msgunibox.h" 
#include "plx_pdaex.h"

#include "BtLowCall.h"
#include "BtMain.h"
#include "BtDeviceNode.h"
#include "BtPairedDevNode.h"
#include "BtFileOper.h"
#include "BtMsg.h"
#include "BtThread.h"
#include "BtString.h"
#include "hopen/info.h"

#include "sdk_fax.h"

BTDEVHDL g_rmt_dev_hdls[MAX_DEV_NUM];
BTINT32 g_rmt_dev_num;
static BTDEVHDL g_curr_dev;
static HANDLE g_inquiry_event;

static int nBtPreAcceptFileSize=0;

#define PATH_DIR_BTMAIN        "/mnt/flash/bluetooth"
#define PATH_DIR_RECVDATA      "/mnt/flash/bluetooth/inbox"
#define PATH_DIR_SENDDATA      "/mnt/flash/bluetooth/outbox"
#define PATH_DIR_PAIREDDEV     "/mnt/flash/ivtbt"
#define PATH_DIR_BTSTATUS      "/mnt/flash/bluetooth/btstatus"

typedef struct tagBTSETTINGSINFO  //蓝牙设置信息的结构
{
	BOOL  bBtIsOn;
	BOOL  bBtVisibilityIsOn;
	char  szBtPhoneName[MAX_PHONENAME_LEN];
}BTSETTINGSINFO, *PBTSETTINGSINFO;

pthread_mutex_t BtRecvData_mutex;  //接收蓝牙文件时需要的一个互斥锁
pthread_cond_t  BtRecvData_cond;  //接收蓝牙文件时需要的条件变量

extern DeviceNodeLink FoundDevListHead;
extern PPAIREDDEVNODE pPairedDevHead; 
extern int iActivePair;
extern pthread_mutex_t BtDeviceNodeListMutex;

void AppInquiryInd(BTDEVHDL dev_hdl);
void AppInqCompInd(void);
void AppPinReqInd(BTDEVHDL dev_hdl);
void AppLinkReqInd(BTDEVHDL dev_hdl);
void AppLinkKeyNotifInd(BTDEVHDL dev_hdl,BTUINT8 *link_key);
void AppConnEventInd(BTCONNHDL conn_hdl, BTUINT16 event, BTUINT8 *arg);
void AppAuthenticationFailInd(BTDEVHDL dev_hdl);

BTBOOL FTP_UIDealReceiveFileCB(PBtSdkFileTransferReqStru pFileInfo);
BTBOOL OPP_UIDealReceiveFileCB(PBtSdkFileTransferReqStru pFileInfo);

void  FTP_STATUSINFOCB(BTUINT8 first, BTUINT8 last, BTUINT8* filename, BTUINT32 filesize, BTUINT32 cursize);

// BTUINT8  AppConnReqInd(BTDEVHDL dev_hdl, BTUINT32 dev_class);
// void AppConnCompleteInd(BTDEVHDL dev_hdl);
void RegAppIndCallback(void);
void UnRegAppIndCallback(void);
void HFAG_Done();

void InitEnv();

void RegisterOPPService(void);
void RegisterFTPService(void);
void RegisterSPPCom6Service(void);
void RegisterSPPCom7Service(void);
void RegisterHSPAGService(void);
void RegisterHFPAGService(void);
void RegisterDUNService(void);
void StartDunService(void);
void RegisterBtComNotifyMsg(HWND hWnd,UINT msg);

void RegisterBtProfile();
void UnRegisterBtProfile();

void RegisterFAXService(void);

extern int IvtBtStackInit(void);
extern int IvtBtStackDone(void);

extern int setenv (const char *name, const char *value, int replace);
extern int chmod (const char * path, int mode);

extern void BtComeNewMsg(char* szRealFileName,char *szViewFileName);
extern int HFAG_Init();

extern HWND hBtMsgWnd;   // 回调函数调用的窗口句柄
BOOL bStopSearchDev=FALSE;  //是否停止查找

static HWND hBTSearchDevWnd;
//When bluetooth serial port connect/disconnect,notify progman so that progman
//can call the interface for PC-sync         --Allan
static HWND BtComNotifyWnd;   
static unsigned int BtComNotifyMsg;

void RegisterBtComNotifyMsg(HWND hWnd,UINT msg)
{
	BtComNotifyWnd=hWnd;
	BtComNotifyMsg=msg;
	return;
}
void AppConnEventInd(BTCONNHDL conn_hdl, BTUINT16 event, BTUINT8 *arg)
{
	BtSdkConnectionPropertyStru conn_prop;
	
	if(event == 1)
	{
		//BTUINT8 dev_name[BTSDK_DEVNAME_LEN];
		//printf("\r\n==============Connect event comming\r\n");
		Btsdk_GetConnectionProperty(conn_hdl, &conn_prop);
		//Btsdk_GetRemoteDeviceName(conn_prop.device_handle, dev_name, NULL);
		//printf("\r\nConnection come from %s\n", dev_name);
		if(conn_prop.service_class==BTSDK_CLS_SERIAL_PORT)
		{
			printf("\r\n==============Bluetooth serial Port connected\r\n");
			PostMessage(BtComNotifyWnd,BtComNotifyMsg,1,0);
		}

		if(conn_prop.service_class==BTSDK_CLS_OBEX_FILE_TRANS)
		{
		    Btsdk_FTPRegisterStatusCallback(conn_hdl, &FTP_STATUSINFOCB);
		}
		
	}
	else if(event == 2)
	{
		//printf("\r\n=========Disconnection Evend ID come\r\n");
		Btsdk_GetConnectionProperty(conn_hdl, &conn_prop);
		if(conn_prop.service_class==BTSDK_CLS_SERIAL_PORT)
		{
			printf("\r\n==============Bluetooth serial Port disconnected\r\n");
			PostMessage(BtComNotifyWnd,BtComNotifyMsg,0,0);
		}
	}
	else
	{
		printf("Unknown Connection ID :%d\n", event);
	}
	
}

 
void AppLinkKeyNotifInd(BTDEVHDL dev_hdl,BTUINT8 *link_key)
{
	BTUINT8 BdAddr[BTSDK_BDADDR_LEN];
	BTUINT8 DevName[BTSDK_DEVNAME_LEN];
	PPAIREDDEVNODE pPairedDevNewNode;
	BTUINT16 len;
	
    if(!iActivePair) 
       PostMessage(hBtMsgWnd, WM_PINCODESUCCESS, (WPARAM)NULL, (LPARAM)dev_hdl);
	
	GetDeviceBdAddr(dev_hdl,BdAddr);
	
	if(!IsExsitInPairedDev(BdAddr))
	{
		pPairedDevNewNode=(PPAIREDDEVNODE)malloc(sizeof(PAIREDDEVNODE));
		
		memset(pPairedDevNewNode,0,sizeof(PAIREDDEVNODE));

		memcpy(pPairedDevNewNode->PairedDevBdAddr,BdAddr,BTSDK_BDADDR_LEN);
		
		len = BTSDK_DEVNAME_LEN;
		if (Btsdk_GetRemoteDeviceName(dev_hdl, DevName, &len) != BTSDK_OK)
		{			
			if (Btsdk_UpdateRemoteDeviceName(dev_hdl, DevName, &len) != BTSDK_OK)
//  		        strcpy((char*)DevName, "Unknown");
				return;					
		}
		
		strcpy(pPairedDevNewNode->PairedDevName,DevName);
		
		pPairedDevNewNode->PairedDevHandle=dev_hdl;
		
		printf("Before AppendPairedDevNode \r\n");

		printf("\r\n###########DevName=%s############\r\n",DevName);
		
		if(pPairedDevHead==NULL)
			pPairedDevHead=AppendPairedDevNode(pPairedDevHead,pPairedDevNewNode);
		else
			AppendPairedDevNode(pPairedDevHead,pPairedDevNewNode);
	}

}

//When we receive AppInquiry event,a device found. This event maybe come in many times
//for maybe many device were found.
//In the BT thread we will build a Device Node List,which save the remote information.
void AppInquiryInd(BTDEVHDL dev_hdl)
{
	g_rmt_dev_hdls[g_rmt_dev_num++] = dev_hdl;
	
	if(bStopSearchDev)
		return;
		
	//If in the UI thread,the user stop searching, the UI thread will ignore this 
	//message because bStopSearchDev flag was set,so GetRemoteDeviceInfo Request 
	//to BT thread will not called. If user searching again immediately after stop
	//previous searching,bStopSearchDev will be set false again. If at this time we receive
	//AppInquiryInq event which actually is a event in the last searching,we will take 
	//it as the current event,and add new device node to DeviceNodeList, we think it
	//is reasonable,because we just append this new node to the tail of list. If we 
	//found the same device at this time,because the name is same,we'll ignore it.

 	PostMessage(hBTSearchDevWnd,WM_GETREMOTEDEVINFO,(WPARAM)NULL, (LPARAM)dev_hdl);
}

//When the inqury event come in, BT stack finish search new devie,we now need to 
//Update the remote device. At this time,the AppInquiryInd event will not be delivery
//to us.When we recieve this event,the UI thread send requst to BT thread. Bt thread
//then update remote device name in Device Node List.
void AppInqCompInd(void)
{
	SET_EVENT(g_inquiry_event);
	
	if(bStopSearchDev)
		return;

	PostMessage(hBTSearchDevWnd,WM_UPDATEREMOTEDEVINFO,(WPARAM)NULL,(LPARAM)NULL);
}

void AppPinReqInd(BTDEVHDL dev_hdl)
{	
	printf("================AppPinReqInd\r\n");
	PostMessage(hBtMsgWnd, WM_GETPASSWORD, (WPARAM)NULL, (LPARAM)dev_hdl);
}

void AppLinkReqInd(BTDEVHDL dev_hdl)
{
	Btsdk_SetRemoteDeviceLinkKey(dev_hdl, NULL);
}

void AppAuthenticationFailInd(BTDEVHDL dev_hdl)
{
    if(!iActivePair)   //别人配对我的情况
      PostMessage(hBtMsgWnd, WM_PINCODEFAILED, (WPARAM)NULL, (LPARAM)dev_hdl);
}

void  AuthorGrantAccess(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl)
{
	PostMessage(hBtMsgWnd, WM_CONFIRMCONNECT, (WPARAM)svc_hdl, (LPARAM)dev_hdl);
}

/*
void AppConnCompleteInd(BTDEVHDL dev_hdl)
{
	BTUINT8 dev_name[BTSDK_DEVNAME_LEN];
	
	Btsdk_GetRemoteDeviceName(dev_hdl, dev_name, NULL);
	printf("Connection Complete come from %s\n", dev_name);
}*/


void HFAG_Done()
{
 Btsdk_AGAP_APPRegCbk(NULL);
 
 Btsdk_AGAP_Done();
}

void RegAppIndCallback(void)
{
	BtSdkCallBackStru cb;
	
	cb.type = BTSDK_INQUIRY_RESULT_IND;
	cb.func = (void*)AppInquiryInd;
	Btsdk_RegisterSdkCallBack(&cb);

    cb.type = BTSDK_LINK_KEY_NOTIF_IND;
	cb.func = (void*)AppLinkKeyNotifInd;
	Btsdk_RegisterSdkCallBack(&cb);
	
	cb.type = BTSDK_INQUIRY_COMPLETE_IND;
	cb.func = (void*)AppInqCompInd;
	Btsdk_RegisterSdkCallBack(&cb);
	
	cb.type = BTSDK_PIN_CODE_IND;
	cb.func = (void*)AppPinReqInd;
	Btsdk_RegisterSdkCallBack(&cb);
	
	cb.type = BTSDK_LINK_KEY_REQ_IND;
	cb.func = (void*)AppLinkReqInd;
	Btsdk_RegisterSdkCallBack(&cb);

	cb.type =BTSDK_AUTHENTICATION_FAIL_IND;
	cb.func = (void*)AppAuthenticationFailInd;
    Btsdk_RegisterSdkCallBack(&cb);

	cb.type = BTSDK_AUTHORIZATION_IND;
	cb.func =(void*)AuthorGrantAccess;
	Btsdk_RegisterSdkCallBack(&cb);

	cb.type = BTSDK_CONNECTION_EVENT_IND;
	cb.func = AppConnEventInd;
	Btsdk_RegisterSdkCallBack(&cb);

	Btsdk_FTPRegisterDealReceiveFileCB(&FTP_UIDealReceiveFileCB);
	Btsdk_OPPRegisterDealReceiveFileCB(&OPP_UIDealReceiveFileCB);

	/*
	cb.type = BTSDK_CONNECTION_REQUEST_IND;
	cb.func = (void*)AppConnReqInd;
	Btsdk_RegisterSdkCallBack(&cb);*/
	
	/*
	cb.type = BTSDK_CONNECTION_COMPLETE_IND;
	cb.func = AppConnCompleteInd;
	Btsdk_RegisterSdkCallBack(&cb);*/	
}

void UnRegAppIndCallback(void)
{
	BtSdkCallBackStru cb;
	
	cb.type = BTSDK_INQUIRY_RESULT_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);

	cb.type = BTSDK_LINK_KEY_NOTIF_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);
	
	cb.type = BTSDK_INQUIRY_COMPLETE_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);
	
	cb.type = BTSDK_PIN_CODE_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);

	cb.type = BTSDK_LINK_KEY_REQ_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);

	cb.type =BTSDK_AUTHENTICATION_FAIL_IND;
	cb.func = NULL;
    Btsdk_RegisterSdkCallBack(&cb);
	
    cb.type = BTSDK_AUTHORIZATION_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);
		
	cb.type = BTSDK_CONNECTION_EVENT_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);

	Btsdk_FTPRegisterDealReceiveFileCB(NULL);
	Btsdk_OPPRegisterDealReceiveFileCB(NULL);
	
	/*
	cb.type = BTSDK_CONNECTION_REQUEST_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);*/
		
	/*
	cb.type = BTSDK_CONNECTION_COMPLETE_IND;
	cb.func = NULL;
	Btsdk_RegisterSdkCallBack(&cb);*/
		
}

void RegisterBtProfile()
{
	RegAppIndCallback();
	
	//SPP Service
	RegisterSPPCom6Service(); 
	
	//HFP Service
	HFAG_Init(); 
	
	//DUN Service
	RegisterDUNService();
	
	//FAX Service
	RegisterFAXService();
	
	//FTP Service
	RegisterFTPService(); 
	
	//OPP Service
	RegisterOPPService();
}

void UnRegisterBtProfile()
{
	BreakAllConnections();

	Btsdk_UnregisterOPPService();

	Btsdk_UnregisterFTPService();

	Btsdk_UnregisterDUNService();
	
	Btsdk_UnregisterFAXService();

    HFAG_Done();
	
	UnRegAppIndCallback();
    
	Btsdk_Done();

	IvtBtStackDone();
}


void BT_Init()
{
	FILE *fp;
	BTSETTINGSINFO BtSettingsInfo;
	int nReadLength;

	InitEnv();

    mkdir(PATH_DIR_RECVDATA,0x666);     //建立接收蓝牙数据的目录   
    mkdir(PATH_DIR_SENDDATA,0x666);     //建立被Opp传输协议使用的目录
    mkdir(PATH_DIR_PAIREDDEV,0x666);    //建立保存配对过的设备的目录
    mkdir(PATH_DIR_BTSTATUS,0x666);     //建立保存上次开机时蓝牙的各种状态
	
    fp=fopen(BTSTATUSFILENAME, "rb+");
	
	if(fp!=NULL)   //若文件存在
	{
		nReadLength=fread(&BtSettingsInfo,sizeof(BTSETTINGSINFO),1,fp);

		if(nReadLength!=1)    //没有读成功为出厂设置值
		{
			bBluetoothStatus=FALSE;    //出厂设置
			bVisibilityStatus=FALSE;   //出厂设置
			
			strcpy(szPhoneName,"My Benefon");  //出厂设置
		}
		else      //读成功
		{
            bBluetoothStatus=BtSettingsInfo.bBtIsOn;
            bVisibilityStatus=BtSettingsInfo.bBtVisibilityIsOn;
			strcpy(szPhoneName,BtSettingsInfo.szBtPhoneName);
		}


		if(bBluetoothStatus)
		{
			printf("Bluetooth Hardware Intializing....\n");
			
			{
				int fd;
				fd=open("/dev/info",O_RDWR);
				if(fd<0)
				{
					DEBUG("\r\nBluetooth:open /dev/info failure,switch on bluetooth failure\r\n");
					return;
				}
				else
				{
					DEBUG("\r\nBluetooth:open /dev/info success\r\n");
				}
				
				ioctl(fd,SET_BT_SWITCHON,0);
				close(fd);
			}
			
			IvtBtStackInit();

			Btsdk_Init();
			
			if (Btsdk_ResetHardware() == BTSDK_OK)
			{
				printf("Bt init ok\r\n");
				printf("Bluetooth : On\n");
				
				Btsdk_SetLocalClassDevice(BTSDK_PHONECLS_CORDLESS);
             // Btsdk_SetFixedPinCode("0000", 4);

				DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开
				
				if(bVisibilityStatus)
				{
					Btsdk_SetDiscoveryMode(BTSDK_DISCOVERY_DEFAULT_MODE);
					printf("Visibility : On\n");
				}
				else
				{
					Btsdk_SetDiscoveryMode(BTSDK_LIMITED_DISCOVERABLE);
					printf("Visibility : Off\n");
				}
				Btsdk_SetLocalName(szPhoneName, (BTUINT16)(sizeof(szPhoneName)));
				
				printf("Phone Name : %s\n",szPhoneName);
				
				Btsdk_SetSecurityMode(BTSDK_SECURITY_MEDIUM);
				
			    RegisterBtProfile();
				
			}
			else      //蓝牙硬件启动失败
			{
                bBluetoothStatus=FALSE;    //出厂设置
				bVisibilityStatus=FALSE;   //出厂设置
				
				strcpy(szPhoneName,"My Benefon");  //出厂设置
				
				printf("Reset hardware fail...\n");
				printf("Quitting....\n");
			}
		}
		else       //上次开机时蓝牙设置为关闭
		{
			DlmNotify(PS_SETBLUETOOTHON,ICON_CANCEL);  //通知程序管理器蓝牙关闭

			printf("Bluetooth do not switch on!\n");
		}

		fclose(fp);  
	}
	else          //第一次使用蓝牙
	{
		bBluetoothStatus=FALSE;    //出厂设置
        bVisibilityStatus=FALSE;   //出厂设置
		
		strcpy(szPhoneName,"My Benefon");  //出厂设置

		printf("The original settings of bluetooth :\n");
		printf("Bluetooth : Off\tVisibility : Off\tPhoneName : My Benefon\n");
		
	}
	
}

void SwitchOnBlueTooth()
{
	int fd;
		
 	printf("Bluetooth Hardware Intializing....\r\n");
 	
 	fd=open("/dev/info",O_RDWR);
	if(fd<0)
	{
		DEBUG("\r\nBluetooth:open /dev/info failure,switch on bluetooth failure\r\n");
		return;
	}
	else
	{
		DEBUG("\r\nBluetooth:open /dev/info success\r\n");
	}

	ioctl(fd,SET_BT_SWITCHON,0);
	close(fd);
	
	IvtBtStackInit();
	
	Btsdk_Init();
	
	if (Btsdk_ResetHardware() == BTSDK_OK)
	{
 		printf("Bt ResetHardware Ok\r\n");
		
		Btsdk_SetLocalClassDevice(BTSDK_PHONECLS_CORDLESS);
		
		if(bVisibilityStatus)
			Btsdk_SetDiscoveryMode(BTSDK_DISCOVERY_DEFAULT_MODE);
		else
			Btsdk_SetDiscoveryMode(BTSDK_LIMITED_DISCOVERABLE);
		
		Btsdk_SetLocalName(szPhoneName, (BTUINT16)(sizeof(szPhoneName)));
		
		Btsdk_SetSecurityMode(BTSDK_SECURITY_MEDIUM);
		
		RegisterBtProfile();	

		printf("Bt init ok\r\n");		
	}
	else
	{
		printf("Reset hardware fail...\n");
		printf("Quitting....\n");
	}
}

void SwitchOffBlueTooth()
{
	int fd;
	fd=open("/dev/info",O_RDWR);
	if(fd<0)
	{
		DEBUG("\r\nBluetooth:open /dev/info failure,switch off failure\r\n");
		return;
	}
	else
	{
		DEBUG("\r\nBluetooth:open /dev/info success\r\n");
	}

	ioctl(fd,SET_BT_SWITCHOFF,0);
	close(fd);
	
	UnRegisterBtProfile();
}

int WriteStatusFile(char *StatusFileName)   //把蓝牙设置信息写进文件中
{
	FILE *fp;
	BTSETTINGSINFO BtSettingsInfo;
	
	fp=fopen(StatusFileName,"wb+");
	
	if(fp== NULL)
		return 0;
	
    BtSettingsInfo.bBtIsOn=bBluetoothStatus;
    BtSettingsInfo.bBtVisibilityIsOn=bVisibilityStatus;
	strcpy(BtSettingsInfo.szBtPhoneName,szPhoneName);
	
	fwrite(&BtSettingsInfo,sizeof(BTSETTINGSINFO),1,fp);
	
	fclose(fp); 
    return 1;
}

BOOL StartSearchDevice(BTUINT32 device_class,HWND hParentWnd)
{
	hBTSearchDevWnd=hParentWnd;   //把窗口句柄设置为要查找设备的那个窗口

	memset(g_rmt_dev_hdls, 0, sizeof(g_rmt_dev_hdls));
	g_rmt_dev_num = 0;

	DEBUG("\r\nBluetooth: StartSearchDevice....in BT thread\r\n");

#ifndef _EMULATE_
	if (Btsdk_StartDiscoverDevice(device_class, 16, 10) == BTSDK_OK)
         return TRUE;
    else
	     return FALSE;
#else
	return TRUE;
#endif
}

void GetRemoteDevicesInfo(BTDEVHDL dev_hdl)
{	
    BTUINT8 BdAddr[BTSDK_BDADDR_LEN];
	BTUINT8 DevName[BTSDK_DEVNAME_LEN];
	BTUINT16 len;
	BTUINT32 cls;
	DeviceNodeLink FoundDevInfo;   

	char szTemp[32];
	char *p;
	int i,j;

	len = BTSDK_DEVNAME_LEN;

	memset(DevName,0,BTSDK_DEVNAME_LEN);
	memset(BdAddr,0,BTSDK_BDADDR_LEN);
	memset(szTemp,0,32);
	
	DEBUG("\r\nBluetooth:GetRemoteDeviceInfo and add them to the Device Node List in the BT thread\r\n");

	Btsdk_GetRemoteDeviceBDAddr(dev_hdl, BdAddr);   //找到的设备地址

	if (Btsdk_GetRemoteDeviceName(dev_hdl, DevName, &len)!= BTSDK_OK)
	{	
        p=szTemp;
		
		for(i = 5; i > 0; i--)
		{
			j=sprintf(p,"%02X:",BdAddr[i]);
			p=p+j;
		}
		
		sprintf(p,"%02X", BdAddr[0]);
		
		strcpy(DevName,szTemp);			      	   
	}

	DEBUG("\r\nBluetooth:Devname=%s\r\n",DevName);
		
	if(strlen(DevName)!=0)
	{		   
		if(FoundDevListHead!=NULL)
			if(IsExsitDeviceInList(FoundDevListHead,BdAddr))
				return;
			
        FoundDevInfo=(DeviceNodeLink)malloc(sizeof(DeviceNode));  //为找到的设备存放信息分配空间
			
		strcpy(FoundDevInfo->DevName,DevName);                    //找到的设备名字
			
		memcpy(FoundDevInfo->DevBdAddr,BdAddr,BTSDK_BDADDR_LEN);
			
		FoundDevInfo->DevHandle=dev_hdl;                //找到的设备Handle
			
		Btsdk_GetRemoteDeviceClass(dev_hdl, &cls);
		FoundDevInfo->DevClass=cls;                               //找到的设备class
		
		//If user stop searching device,while the BT thread still Get Remote device info ,there is two
		// thing should take into accout:
		//(1) if user stoped searching in UI first,the new device node should not append to device node list
		//(2) if the user stop searching at the time that we operate the Device Node List, something will be wrong
		//    ,because the UI thread and BT thread maybe operate the Device Node list at the same list. So 
		//    we need a mutex lock to protect the Device node list.
		//   -------------Allan

		pthread_mutex_lock(&BtDeviceNodeListMutex);
	
		if(bStopSearchDev)
		{
			pthread_mutex_unlock(&BtDeviceNodeListMutex);
			return;
		}

		if(FoundDevListHead==NULL)
			FoundDevListHead=Append_DeviceNodeList(FoundDevListHead,FoundDevInfo);
		else
			Append_DeviceNodeList(FoundDevListHead,FoundDevInfo);
		pthread_mutex_unlock(&BtDeviceNodeListMutex);			

	}
}

//得到其它蓝牙设备想要发送过来的文件的大小信息
void  FTP_STATUSINFOCB(BTUINT8 first, BTUINT8 last, BTUINT8* filename, BTUINT32 filesize, BTUINT32 cursize)
{
    if(first==1)
	{
		nBtPreAcceptFileSize=filesize;
	}
}

int GetBtPreAcceptFileSize()
{
	return nBtPreAcceptFileSize;
}

BTBOOL FTP_UIDealReceiveFileCB(PBtSdkFileTransferReqStru pFileInfo)  
{
	int rc;	 
		static char OldFileName[64]="";
		
	    if(strlen(OldFileName)==0)
			strcpy(OldFileName,pFileInfo->file_name);
	
// 	    PostMessage(hBtMsgWnd, WM_CONFIRMRECVDATA, (WPARAM)NULL, (LPARAM)pFileInfo);
		
		if(pFileInfo->operation==BTSDK_APP_EV_FTP_PUT)
		{			
			if(pFileInfo->flag == BTSDK_ER_CONTINUE)
			{
				printf("\n#####before WM_CONFIRMRECVDATA\n");
				
				printf("\n#####pFileInfo->file_name=%s\n",pFileInfo->file_name);

				PostMessage(hBtMsgWnd, WM_CONFIRMRECVDATA, (WPARAM)NULL, (LPARAM)pFileInfo);
				
				rc=pthread_mutex_lock(&BtRecvData_mutex);
				rc=pthread_cond_wait(&BtRecvData_cond, &BtRecvData_mutex);
				rc=pthread_mutex_unlock(&BtRecvData_mutex);
			}
			
			if (pFileInfo->flag == BTSDK_ER_SUCCESS)
			{
				// Prompt the successful information

				printf("\n#####In FTP_UIDealReceiveFileCB pFileInfo->flag==BTSDK_ER_SUCCESS\n");
				
				printf("\n#####pFileInfo->file_name=%s\n",pFileInfo->file_name);
				
				BtComeNewMsg(pFileInfo->file_name,OldFileName);		
				
				memset(OldFileName,0,64);  //本次接收成功后清空原来的文件名
			} 
		}
	
	return BTSDK_TRUE;	
	
}

BTBOOL OPP_UIDealReceiveFileCB(PBtSdkFileTransferReqStru pFileInfo)
{   
	int rc;	 
		static char OldFileName[64]="";
		
	    if(strlen(OldFileName)==0)
			strcpy(OldFileName,pFileInfo->file_name);
		
		if(pFileInfo->flag == BTSDK_ER_CONTINUE)
        {
			printf("\n#####before WM_CONFIRMRECVDATA\n");
			
			printf("\n#####pFileInfo->file_name=%s\n",pFileInfo->file_name);

			PostMessage(hBtMsgWnd, WM_CONFIRMRECVDATA, (WPARAM)NULL, (LPARAM)pFileInfo);
			
			rc=pthread_mutex_lock(&BtRecvData_mutex);
			rc=pthread_cond_wait(&BtRecvData_cond, &BtRecvData_mutex);
			rc=pthread_mutex_unlock(&BtRecvData_mutex);	
        }
		

		if (pFileInfo->flag == BTSDK_ER_SUCCESS)
		{			
			// Prompt the successful information

			printf("\n#####In OPP_UIDealReceiveFileCB pFileInfo->flag==BTSDK_ER_SUCCESS\n");
			
			printf("\n#####pFileInfo->file_name=%s\n",pFileInfo->file_name);
			
	        BtComeNewMsg(pFileInfo->file_name,OldFileName);
			
			memset(OldFileName,0,64); //本次接收成功后清空原来的文件名
			
		} 
	
	return BTSDK_TRUE;	
}

BOOL UnPairDevice(BTDEVHDL dev_hdl)
{
	BTUINT32 result;

	result = Btsdk_UnPairDevice(dev_hdl);
	if (result == BTSDK_OK)
		return TRUE;
	else
		return FALSE;
}

void GetDeviceClass(BTDEVHDL dev_hdl, BTUINT32* pdev_class)
{
  Btsdk_GetRemoteDeviceClass(dev_hdl, pdev_class);
}

int DistinguishDevCls(BTUINT32 BtDeviceClass)
{
	if((BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_PHONECLS_CELLULAR ||
		(BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_PHONECLS_CORDLESS ||
		(BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_PHONECLS_SMARTPHONE)
		return 1;
	
	else if((BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_COMPCLS_HANDHELD)
		return 2;
	
	else if((BtDeviceClass & 0x1F00)==BTSDK_DEVCLS_COMPUTER && 
		(BtDeviceClass & DEVICE_CLASS_MASK)!=BTSDK_COMPCLS_HANDHELD)
		return 3;
	
	else if((BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_AV_HEADSET||
		(BtDeviceClass & DEVICE_CLASS_MASK)==BTSDK_AV_HANDSFREE)
		return 4;
	
	else
		return 5;
	
}

void DelRemoteDevice(BTDEVHDL dev_hdl)
{
	BTUINT32 result;
	int i;
	
//	result  = Btsdk_DeleteRemoteDeviceByHandle(dev_hdl);
    result = Btsdk_UnPairDevice(dev_hdl);

	if (result == BTSDK_OK)
	{
		for (i = 0; i < g_rmt_dev_num; i++)
		{
			if (g_rmt_dev_hdls[i] == g_curr_dev)
			{
				break;
			}
		}
		if ( i != g_rmt_dev_num)
		{
			for (; i < g_rmt_dev_num - 1; i++)
			{
				g_rmt_dev_hdls[i] = g_rmt_dev_hdls[i + 1];
			}
			g_curr_dev = 0;
		}
	}
}

void RegisterOPPService(void)
{
	char inbox_path[BTSDK_PATH_MAXLENGTH];
	char outbox_path[BTSDK_PATH_MAXLENGTH];
	char own_card[BTSDK_CARDNAME_MAXLENGTH];

	memset(inbox_path, 0, BTSDK_PATH_MAXLENGTH);
	memcpy(inbox_path, PATH_DIR_RECVDATA, BTSDK_PATH_MAXLENGTH);

	memset(outbox_path, 0, BTSDK_PATH_MAXLENGTH);
	memcpy(outbox_path, PATH_DIR_SENDDATA, BTSDK_PATH_MAXLENGTH);

	memset(own_card, 0, BTSDK_CARDNAME_MAXLENGTH);
	memcpy(own_card, "/mnt/flash/bluetooth/outbox/mybenefon.vcf", BTSDK_CARDNAME_MAXLENGTH);

	Btsdk_RegisterOPPService(inbox_path, outbox_path, own_card);	
}

void RegisterFTPService(void)
{
	char root_dir[BTSDK_PATH_MAXLENGTH];

	memset(root_dir, 0, BTSDK_PATH_MAXLENGTH);
	memcpy(root_dir, PATH_DIR_RECVDATA, BTSDK_PATH_MAXLENGTH);

	Btsdk_RegisterFTPService(BTSDK_FTPDA_READWRITE, root_dir);
}

void RegisterSPPCom6Service(void)
{  
    Btsdk_RegisterSPPService(6);
}

void RegisterSPPCom7Service(void)
{
    Btsdk_RegisterSPPService(7);
}

void RegisterHSPAGService(void)
{
    Btsdk_RegisterHSPAGService();
}

void RegisterHFPAGService(void)
{
    Btsdk_RegisterHFPAGService();
}


void RegisterDUNService(void)
{
    Btsdk_RegisterDUNService(0);
}

void RegisterFAXService(void)
{
    Btsdk_RegisterFAXService(9);
}

void StartDunService(void)
{
	BTSVCHDL hSvc;
	BtSdkLocalServerAttrStru attr = {0};
	
	attr.mask = BTSDK_LSAM_SERVICENAME|BTSDK_LSAM_SECURITYLEVEL|BTSDK_LSAM_AUTHORMETHOD;
	attr.service_class = BTSDK_CLS_DIALUP_NET;
	attr.security_level = BTSDK_SSL_NO_SECURITY;
	attr.author_method = BTSDK_AUTHORIZATION_PROMPT;
	strcpy(attr.svc_name, "Dialup Port (COM9)");
	hSvc = Btsdk_AddServer(&attr);
	Btsdk_SetServiceSecurityLevel(hSvc, BTSDK_DEFAULT_SECURITY);
	Btsdk_SetAuthorizationMethod(hSvc, BTSDK_AUTHORIZATION_PROMPT);
	Btsdk_StartServer(hSvc);
}

void InitEnv()
{
	setenv("TMPDIR","/mnt/ram",TRUE);
	chmod(PATH_DIR_BTMAIN, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
}


void StopDiscoverDevice(void)
{
#ifndef _EMULATE_	
   Btsdk_StopDiscoverDevice();
#endif
}


void StopPairDevice(BTDEVHDL dev_hdl)
{
	Btsdk_UnPairDevice(dev_hdl);
}

void StopBtFileTransfer(BTCONNHDL conn_hdl)   //取消FTP文件传输   
{
	Btsdk_FTPCancelTransfer(conn_hdl);
}

void StopBtOppTransfer(BTCONNHDL conn_hdl)    //取消OPP文件传输
{
	Btsdk_OPPCancelTransfer(conn_hdl);
}

BOOL Get_DeviceLocalAddr(char *addr)
{
	int i,j;
    BTUINT8 loc_bd_addr[BTSDK_BDADDR_LEN];
	char szTemp[32];
	char *p;
    
	if(!bBluetoothStatus)
	{
		SwitchOnBlueTooth();

		if(Btsdk_GetLocalBDAddr(loc_bd_addr)==BTSDK_OK)
		{
			SwitchOffBlueTooth();

			p=szTemp;
			
			for(i = 5; i > 0; i--)
			{
				j=sprintf(p,"%02X:",loc_bd_addr[i]);
				p=p+j;
			}
			
			sprintf(p,"%02X", loc_bd_addr[0]);
			
			strcpy(addr,szTemp);
			
			printf("\r\nSuccess when Get BT Address\r\n");
			printf("\r\naddr=%s\r\n",addr);
			return TRUE;
		}
		else
		{
			printf("\r\nfailure when Get BT Address\r\n");
			return FALSE; 
		}
    }
	else
	{
        if(Btsdk_GetLocalBDAddr(loc_bd_addr)==BTSDK_OK)
		{			
			p=szTemp;
			
			for(i = 5; i > 0; i--)
			{
				j=sprintf(p,"%02X:",loc_bd_addr[i]);
				p=p+j;
			}
			
			sprintf(p,"%02X", loc_bd_addr[0]);
			
			strcpy(addr,szTemp);
			
			printf("\r\nSuccess when Get BT Address\r\n");
			printf("\r\naddr=%s\r\n",addr);
			return TRUE;
		}
		else
		{
			printf("\r\nfailure when Get BT Address\r\n");
			return FALSE; 
		}
	}

}


BOOL BtIsActiveConnect()   //判断当前设备是否有激活的连接
{
#ifndef _EMULATE_
	int InComingNum,OutgoingNum;
     
	InComingNum=Btsdk_GetAllIncomingConnections(NULL,0);
    OutgoingNum=Btsdk_GetAllIncomingConnections(NULL,0);

	if((InComingNum+OutgoingNum)>0)
		return TRUE;
	else
		return FALSE;     
#else
	
	return FALSE;

#endif
}

void BreakAllConnections(void)
{
	BTUINT32 num_o, num_i, i;
	BTCONNHDL *pconn_hdl;
	PBtSdkShortCutPropertyStru pshc_prop = 
		(PBtSdkShortCutPropertyStru)Btsdk_MallocMemory(sizeof(BtSdkShortCutPropertyStru));
	
	num_o = Btsdk_GetAllOutgoingConnections(NULL, 0);
	num_i = Btsdk_GetAllIncomingConnections(NULL, 0);
	if (num_o != 0 || num_i != 0)
	{
		pconn_hdl = (BTCONNHDL *)Btsdk_MallocMemory((num_o + num_i) * sizeof(BTCONNHDL));
		if (pconn_hdl != NULL)
		{
			num_o = Btsdk_GetAllOutgoingConnections(pconn_hdl, num_o);
			num_i = Btsdk_GetAllIncomingConnections(pconn_hdl + num_o, num_i);
			for(i = 0; i < num_o + num_i; i++)
			{
				Btsdk_DisconnectConnection(pconn_hdl[i]);
			}
			Btsdk_FreeMemory(pconn_hdl);
		}
	}
	Btsdk_FreeMemory(pshc_prop);
}

void BtPincodeFailed(BTDEVHDL dev_hdl)
{
	BTUINT8 dev_name[BTSDK_DEVNAME_LEN];
	BTUINT16 DevNameLen;
	char szPincodeFailed[128];

	memset(szPincodeFailed,0,128);
    memset(dev_name,0,BTSDK_DEVNAME_LEN);

    DevNameLen=BTSDK_DEVNAME_LEN;

	if(Btsdk_GetRemoteDeviceName(dev_hdl, dev_name, &DevNameLen)!=BTSDK_OK)
	{
		Btsdk_UpdateRemoteDeviceName(dev_hdl, dev_name, &DevNameLen);
	}
    
	if(strlen(dev_name)!=0)
	{
		strcpy(szPincodeFailed,dev_name);
		strcat(szPincodeFailed,IDP_BT_STRING_BTPASSCODEDENIED);
	}
	else
        strcpy(szPincodeFailed,IDP_BT_STRING_BTPASSCODEDENIED);

	PLXTipsWin(NULL,NULL,0,szPincodeFailed,IDP_BT_TITLE_BLUETOOTH,Notify_Info,IDP_BT_BUTTON_OK,NULL,20);	
}

void BtPincodeSuccess(BTDEVHDL dev_hdl)
{
	BTUINT8 dev_name[BTSDK_DEVNAME_LEN];
	BTUINT16 DevNameLen;
	char szPincodeSuccess[128];

    DevNameLen=BTSDK_DEVNAME_LEN;
	
	memset(szPincodeSuccess,0,128);
	memset(dev_name,0,BTSDK_DEVNAME_LEN);
	
	if(Btsdk_GetRemoteDeviceName(dev_hdl, dev_name, &DevNameLen)!=BTSDK_OK)
	{
		Btsdk_UpdateRemoteDeviceName(dev_hdl, dev_name, &DevNameLen);
	}
	
	if(strlen(dev_name)!=0)
	{
		strcpy(szPincodeSuccess,dev_name);
		strcat(szPincodeSuccess,IDP_BT_STRING_BTPASSCODEACCEPTED);
	}
	else
        strcpy(szPincodeSuccess,IDP_BT_STRING_BTPASSCODEACCEPTED);
	
	PLXTipsWin(NULL,NULL,0,szPincodeSuccess,IDP_BT_TITLE_BLUETOOTH,Notify_Info,IDP_BT_BUTTON_OK,NULL,20);
}

void SetAuthoriseDevice(BTDEVHDL dev_hdl)
{
	Btsdk_SetTrustedDevice((BTSVCHDL)NULL,dev_hdl,BTSDK_TRUSTED);	
}

void SetUnauthoriseDevice(BTDEVHDL dev_hdl)
{
	Btsdk_SetTrustedDevice((BTSVCHDL)NULL,dev_hdl,BTSDK_UNTRUSTED);
}

void DisConnection(BTCONNHDL conn_hdl)
{
	BtSdkConnectionPropertyStru conn_prop;
	BtSdkRemoteServiceAttrStru svc_attrib;
	
	Btsdk_GetConnectionProperty(conn_hdl,&conn_prop);
	Btsdk_GetRemoteServiceAttributes(conn_prop.service_handle,&svc_attrib);
	
	Btsdk_DisconnectConnection(conn_hdl);	
}

void CancelBtFileTransfer(BTCONNHDL conn_hdl)
{
   Btsdk_FTPCancelTransfer(conn_hdl);

}

BTDEVHDL GetDeviceHandle(BTUINT8 *bd_addr)
{
    BTDEVHDL DevHandle;

	DevHandle=Btsdk_GetDeviceHandle(bd_addr);

	return DevHandle;
}

void GetDeviceName(BTDEVHDL dev_hdl,BTUINT8 DevName[],BTUINT16 len)
{

	if (Btsdk_GetRemoteDeviceName(dev_hdl, DevName, &len) != BTSDK_OK)
	{
		if (Btsdk_UpdateRemoteDeviceName(dev_hdl, DevName, &len) != BTSDK_OK)
			return ;				
	}
	
}

void GetDeviceBdAddr(BTDEVHDL dev_hdl,BTUINT8* bd_addr)
{
	Btsdk_GetRemoteDeviceBDAddr(dev_hdl,bd_addr);
}

BOOL IsExsitInPairedDev(BTUINT8* bd_addr)
{
	PPAIREDDEVNODE pCurPairedPointer;
	
	pCurPairedPointer=pPairedDevHead;
	
	while(pCurPairedPointer!=NULL)
	{
		if(!strcmp(pCurPairedPointer->PairedDevBdAddr,bd_addr))
			return TRUE;
		else
            pCurPairedPointer=pCurPairedPointer->Next;
	}
	return FALSE;
}

void UpdateRemoteDevices(HWND hParentWnd)
{
	DeviceNodeLink Pointer;
	BTUINT16 len;
	BTDEVHDL DevHandle;

	static BTUINT8 DevName[BTSDK_DEVNAME_LEN];

	len=BTSDK_DEVNAME_LEN;

	Pointer=FoundDevListHead;

	while(Pointer!=NULL)
	{
        memset(DevName,0,BTSDK_DEVNAME_LEN);

        if(bStopSearchDev)  //Check if the user stop searching device
		   break;
        
        DevHandle=Pointer->DevHandle;
        if(Btsdk_UpdateRemoteDeviceName(DevHandle, DevName, &len)!= BTSDK_OK)
        {
           DEBUG("\r\nBluetooth:Can not update device name in BT thread\r\n");
        }
        else
	{
	    if(strlen(DevName)!=0)
	    {
		//Because the GUI thread maybe read data from the list,so we need to add the mutex lock
		pthread_mutex_lock(&BtDeviceNodeListMutex);
		strcpy(Pointer->DevName,DevName);
		pthread_mutex_unlock(&BtDeviceNodeListMutex);
	    }
	    else
	   {
		DEBUG("\r\nBluetooth:Can not update device name in BT thread\r\n");
	   }
	}
	
        DEBUG("\r\nBluetooth UpdateRemoteDevice,current DevName=%s\r\n",DevName);

	if(bStopSearchDev)
		return;

	//UI thread will read data from Device Node List, mutex lock is necessary
        PostMessage(hParentWnd,WM_FOUNDDEVICE,(WPARAM)NULL,(LPARAM)Pointer->DevName);
	Pointer=Pointer->Next;
	}

    PostMessage(hParentWnd,WM_UPDATEREMOTEDEVCOMPLETED,(WPARAM)NULL,(LPARAM)g_rmt_dev_num);
}

void AuthoriseDevService(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl)
{
   Btsdk_AuthorRsp(svc_hdl, dev_hdl, BTSDK_AUTHORIZATION_GRANT);
}

void UnauthoriseDevService(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl)
{
   Btsdk_AuthorRsp(svc_hdl, dev_hdl, BTSDK_AUTHORIZATION_DENY);
}
