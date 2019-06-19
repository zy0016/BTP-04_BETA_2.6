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

#ifndef		_BTLOWCALL_H_
#define		_BTLOWCALL_H_

#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "vfs.h"
#include "fcntl.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "window.h"

#include "hpdef.h"
#include "pubapp.h"

#include "BtDeviceNode.h"
#include "BtPasscode.h"

#include "sdk_ui.h"

#define WM_GETREMOTEDEVINFO            (WM_USER+0x500)  //�õ�Զ���豸��Ϣ(Get����)
#define WM_FOUNDDEVICE                 (WM_USER+0x501)  //ʵʱ��ʾ�ҵ����豸
#define WM_UPDATEREMOTEDEVINFO         (WM_USER+0x502)  //�����豸������(Update����)
#define WM_UPDATEREMOTEDEVCOMPLETED    (WM_USER+0x503)  


#define BTSTATUSFILENAME	     	"/mnt/flash/bluetooth/btstatus/btstatus.txt"
#define BTPAIREDDEVINFOFILENAME		"/mnt/flash/bluetooth/btstatus/btpaired.txt"

#define MAX_PHONENAME_LEN		31

BOOL  bBluetoothStatus;  //ȫ�ֱ��������Bluetooth��ǰ״̬������Ĭ��ֵΪFALSE
BOOL  bVisibilityStatus; //ȫ�ֱ��������Visibility��ǰ״̬������Ĭ��ֵΪFALSE
char  szPhoneName[MAX_PHONENAME_LEN]; //ȫ�ֱ��������Visibility��ǰ״̬������Ĭ��ֵΪMy Benefon 

typedef enum
{
    MOBILEPHONE=1,
	HANDHELD,
	PC,
	HEADSET,
	OTHER
}BTDEVICECLASS;

#define MAX_DEV_NUM 10

void BT_Init();

void SwitchOnBlueTooth();
void SwitchOffBlueTooth();

int WriteStatusFile(char *StatusFileName);

BOOL StartSearchDevice(BTUINT32 device_class,HWND hParentWnd);
void GetRemoteDevicesInfo(BTDEVHDL dev_hdl);
BOOL PairDevice(BTDEVHDL dev_hdl);
BOOL UnPairDevice(BTDEVHDL dev_hdl);

void GetDeviceClass(BTDEVHDL dev_hdl, BTUINT32* pdev_class);
int DistinguishDevCls(BTUINT32 BtDeviceClass);

void AuthorGrantAccess(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl);

void StopDiscoverDevice(void);
void StopPairDevice(BTDEVHDL dev_hdl);
void StopBtFileTransfer(BTCONNHDL conn_hdl);
void StopBtOppTransfer(BTCONNHDL conn_hdl);

BOOL Get_DeviceLocalAddr(char *addr);  //����Ӧ�õ���

void BreakAllConnections(void);
BOOL BtIsActiveConnect();
void DisConnection(BTCONNHDL conn_hdl);

void AuthoriseDevService(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl);
void UnauthoriseDevService(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl);

void BtPincodeFailed(BTDEVHDL dev_hdl);
void BtPincodeSuccess(BTDEVHDL dev_hdl);

BOOL ConnToBtDevice(BTDEVHDL BtDevHandle,BTCONNHDL* conn_hdl);
BOOL BtSendFile(BTCONNHDL conn_hdl, char *szBtAppFileName);
void CancelBtFileTransfer(BTCONNHDL conn_hdl);

void DelRemoteDevice(BTDEVHDL dev_hdl);

BOOL HandleReceivedFile(BTUINT8 *pFilePathName);

void SetAuthoriseDevice(BTDEVHDL dev_hdl);
void SetUnauthoriseDevice(BTDEVHDL dev_hdl);

BTDEVHDL GetDeviceHandle(BTUINT8 *bd_addr);
void GetDeviceName(BTDEVHDL dev_hdl,BTUINT8 DevName[],BTUINT16 len);
void GetDeviceBdAddr(BTDEVHDL dev_hdl,BTUINT8* bd_addr);

BOOL IsExsitInPairedDev(BTUINT8* bd_addr);
void UpdateRemoteDevices(HWND hParentWnd);

int GetBtPreAcceptFileSize(); //�õ�׼�����յ��ļ���С

#endif
