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

#define NDIS_PACKET_TYPE_DIRECTED           0x0001
#define NDIS_PACKET_TYPE_MULTICAST          0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST      0x0004
#define NDIS_PACKET_TYPE_BROADCAST          0x0008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING     0x0010
#define NDIS_PACKET_TYPE_PROMISCUOUS        0x0020
#define NDIS_PACKET_TYPE_SMT                0x0040
#define NDIS_PACKET_TYPE_MAC_FRAME          0x8000
#define NDIS_PACKET_TYPE_FUNCTIONAL         0x4000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL     0x2000
#define NDIS_PACKET_TYPE_GROUP              0x1000

#define OID_GEN_SUPPORTED_LIST              0x00010101
#define OID_GEN_HARDWARE_STATUS             0x00010102
#define OID_GEN_MEDIA_SUPPORTED             0x00010103
#define OID_GEN_MEDIA_IN_USE                0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD           0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE          0x00010106
#define OID_GEN_LINK_SPEED                  0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE       0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE        0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE         0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE          0x0001010B
#define OID_GEN_VENDOR_ID                   0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION          0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER       0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD           0x0001010F
#define OID_GEN_DRIVER_VERSION              0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE          0x00010111
#define OID_GEN_PROTOCOL_OPTIONS            0x00010112
#define OID_GEN_MAC_OPTIONS                 0x00010113

#define OID_GEN_XMIT_OK                     0x00020101
#define OID_GEN_RCV_OK                      0x00020102
#define OID_GEN_XMIT_ERROR                  0x00020103
#define OID_GEN_RCV_ERROR                   0x00020104
#define OID_GEN_RCV_NO_BUFFER               0x00020105

#define OID_GEN_DIRECTED_BYTES_XMIT         0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT        0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT        0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT       0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT        0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT       0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV          0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV         0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV         0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV        0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV         0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV        0x0002020C

#define OID_GEN_RCV_CRC_ERROR               0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH       0x0002020E


//
// 802.3 Objects (Ethernet)
//

#define OID_802_3_PERMANENT_ADDRESS         0x01010101
#define OID_802_3_CURRENT_ADDRESS           0x01010102
#define OID_802_3_MULTICAST_LIST            0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE         0x01010104

#define OID_802_3_RCV_ERROR_ALIGNMENT       0x01020101
#define OID_802_3_XMIT_ONE_COLLISION        0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS      0x01020103

#define OID_802_3_XMIT_DEFERRED             0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS       0x01020202
#define OID_802_3_RCV_OVERRUN               0x01020203
#define OID_802_3_XMIT_UNDERRUN             0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE    0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST       0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS      0x01020207

//
// WAN objects
//

#define OID_WAN_PERMANENT_ADDRESS           0x04010101
#define OID_WAN_CURRENT_ADDRESS             0x04010102
#define OID_WAN_QUALITY_OF_SERVICE          0x04010103
#define OID_WAN_PROTOCOL_TYPE               0x04010104
#define OID_WAN_MEDIUM_SUBTYPE              0x04010105
#define OID_WAN_HEADER_FORMAT               0x04010106

#define OID_WAN_GET_INFO                    0x04010107
#define OID_WAN_SET_LINK_INFO               0x04010108
#define OID_WAN_GET_LINK_INFO               0x04010109

#define OID_WAN_LINE_COUNT                  0x0401010A

#define OID_WAN_GET_BRIDGE_INFO             0x0401020A
#define OID_WAN_SET_BRIDGE_INFO             0x0401020B
#define OID_WAN_GET_COMP_INFO               0x0401020C
#define OID_WAN_SET_COMP_INFO               0x0401020D
#define OID_WAN_GET_STATS_INFO              0x0401020E
#ifdef _POSIXCOMP_
typedef struct
{
	int (*LIB_CreateTask)(char * pname, unsigned int prio,
		void* (*entry)(void *), unsigned long stksize, void * para);
} HOPENENTRY;
unsigned long NE_Init( char* pFile, char* pNdisNum, HOPENENTRY* Entry );
#else
unsigned long NE_Init( char* pFile, char* pNdisNum );
#endif
int NE_OpenAdapter( unsigned long hVxDHd, int filter, void (* RecvCallBack)(char*, int) );
int NE_SendPacket( unsigned long hVxD,char *pbuf,int len );
int NE_GetHardEtherAddr( unsigned long hVxD, unsigned char* petheraddr, int len );
int EI_ShutDown();
