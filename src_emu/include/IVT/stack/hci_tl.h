/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
	hci_tl.h
    
Abstract:
	The module defines the macros and definitions about the transport layer
	
Author:
    
Revision History:
---------------------------------------------------------------------------*/

#ifndef HCI_TL_H
#define HCI_TL_H

#define TL_USB			0x00
#define TL_COM			0x01
#define TL_BCSP			0x02

#define FC_NONE         0x00
#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

#define LAN_NAME		"LAN"
#define USB_NAME		"USB"
#define PCMCIA_NAME		"PCMCIA"
#ifndef CONFIG_OS_WINCE
#define COM_NAME		"COM"
#define BCSP_NAME		"BCSP"
#else
#define COM_NAME		"H4_COM"
#define BCSP_NAME		"BCSP_COM"
#endif

/* the structure which discribes the default settings of the communication port */
struct CommPortSettingsStru
{
	DWORD baud_rate;
	UCHAR byte_size;
	UCHAR parity;
	UCHAR stop_bits;
	UCHAR flow_control;
};

typedef int (FunHandleRcvData) (int handle);
typedef int (FuncAddRcvData)(int handle, struct BuffStru *buf);
typedef void (FuncStopHCISco)(WORD sco_hdl);
typedef void (FuncStartHCISco)(WORD sco_hdl);

typedef void (FuncRegisterAddRcvData)(FuncAddRcvData *func);
typedef void (FuncRegisterHandleRcvData)(FunHandleRcvData *func);
typedef void (FuncSetCommSettings)(struct CommPortSettingsStru *cps);
typedef int (FuncOpenCommPort)(char *port_name);
typedef int (FuncCloseCommPort)(int handle);
typedef int (FuncConfigCommPort)(int handle);
typedef int (FuncSendDataToInt)(struct BuffStru *buf);
int GetTransportLayer(void);
int InitTransportLayer(void);
int FreeTransportLayer(void);
void GetDefaultCommSettings(char *tl);
void SetDefaultCommSettings(struct CommPortSettingsStru *cps);
void RegisterAddRcvData(FuncAddRcvData *func);
void RegisterHandleRcvData(FunHandleRcvData *func);
int OpenCommPort(char *port_name);
int CloseCommPort(int handle);
int ConfigCommPort(int handle);
int SendDataToInt(struct BuffStru *buf);
char *HexToByte(char *str, UCHAR *i);
void GetInquiryConfigParam(UCHAR *is_enable, UCHAR *inq_len, UCHAR *num_rsp, UCHAR *bd_addr,UCHAR *switch_scan);
void GetConnectConfigParam(WORD *pkt_type, UCHAR *switch_scan);
void GetResetConfigParam(UCHAR *is_reset);
void GetFlowControlConfigParam(UCHAR *is_enable);

UCHAR RegisterScoDataInd(UCHAR *func);

#if (defined(CONFIG_OS_WIN32) || defined(CONFIG_OS_WINCE))
void NotifyStackDeviceRemoveEvent(void);
#else
#define NotifyStackDeviceRemoveEvent
#endif

#ifdef CONFIG_OS_WIN32
void HciUSBUnavailable(void);
void StopUSBSco(WORD conn_hdl);
void StartUSBSco(WORD sco_hdl);
#else
#define HciUSBUnavailable
#define StopUSBSco(ch)
#define StartUSBSco(sh)
#endif

#endif
