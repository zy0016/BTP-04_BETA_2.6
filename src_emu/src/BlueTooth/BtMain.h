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

#ifndef		_BTMAIN_H_
#define		_BTMAIN_H_

#include "BtLowCall.h"

//Icon path
#define BTSETTINGSICON           "/rom/bluetooth/bluetooth_settings.bmp"
#define BTPAIRDEVICEICON         "/rom/bluetooth/bluetooth_paireddevices-a.bmp"
#define BTPAIREDDEVICEICON       "/rom/bluetooth/bluetooth_paireddevices-b.bmp"
#define BTNEWDEVICEICON          "/rom/bluetooth/bluetooth_newdevice.bmp"
#define BTSEARCHDEVICEICON       "/rom/bluetooth/bluetooth_search.bmp"
#define BTAUTHORISEDICON         "/rom/bluetooth/bluetooth_authorised.bmp"
#define BTDEVICECATEGORY1        "/rom/bluetooth/bluetooth_devicecategory1.bmp"
#define BTDEVICECATEGORY2        "/rom/bluetooth/bluetooth_devicecategory2.bmp"
#define BTDEVICECATEGORY3        "/rom/bluetooth/bluetooth_devicecategory3.bmp"
#define BTDEVICECATEGORY4A       "/rom/bluetooth/bluetooth_devicecategory4-a.bmp"
#define BTDEVICECATEGORY4B       "/rom/bluetooth/bluetooth_devicecategory4-b.bmp"

#define WM_SETBTSTATE (WM_USER+0x101)         //用于Bluetooth激活与未激活状态改变的消息
#define WM_SETVISIBILITYSTATE (WM_USER+0x102) //用于Visibility激活与未激活状态改变的消息

#define WM_GETPASSWORD				   (WM_USER+0x220)
#define WM_CONFIRMCONNECT              (WM_USER+0x221)
#define WM_DEALCONNECT                 (WM_USER+0x222)
#define WM_CONFIRMRECVDATA             (WM_USER+0x223)
#define WM_DEALRECVDATA                (WM_USER+0x224)
#define WM_PINCODEFAILED               (WM_USER+0x225)
#define WM_PINCODESUCCESS              (WM_USER+0x226)

#define CANCELPAIRING                  (WM_USER+0x400)

//#define  BT_DEBUG_INFO(x)				printf("\r\n"#x#"\r\n")
#define  DEBUG							printf
        
#endif

		
