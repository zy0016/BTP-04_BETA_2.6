/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Copyright (c) 1999-2002 IVT Corporation
*
* All rights reserved.
* 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    g_hidp.h

Abstract:
	This file includes all needed outside header files and some important header files.

Author:
    Yongping Zeng

Revision History:
2002.3
	create the initiator version
2003.3
	adapted for SOC stack by luo xiqiong
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef G_HIDP_H
#define G_HIDP_H

#include "global.h"
#include "hci_ui.h"
#include "gap_ui.h"
#include "l2capui.h"
#include "l2func.h"
#include "btinit.h"

#include "hid_ui.h"
#include "hidp_ui.h"

#ifdef CONFIG_HID_CLIENT
extern struct HID_ClntStru *g_hid_clnt_info;
#endif /* CONFIG_HID_CLIENT */

#ifdef CONFIG_HID_SERVER
extern struct HID_SvrStru *g_hid_svr_info;
#endif /* CONFIG_HID_SERVER */

#endif
