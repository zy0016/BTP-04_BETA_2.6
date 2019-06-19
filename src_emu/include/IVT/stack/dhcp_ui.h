#ifndef DHCP_H_
#define DHCP_H_

#include "global.h"

//	server information
#define ETH_MAC_ADDR_LEN	6
#define DOMAIN_NAME_LEN		40

#define u_int8_t	UCHAR
#define u_int16_t	WORD
#define u_int32_t	UINT

#define DHCP_SUCCESS	BT_SUCCESS
#define DHCP_FAIL		BT_FAIL

/*
typedef	unsigned char	u_int8_t;
typedef	unsigned short	u_int16_t;
typedef	unsigned long	u_int32_t;

typedef	unsigned char UCHAR;
typedef	unsigned short	WORD;
#define ULONG	u_int32_t
#define DHCP_SUCCESS	1
#define DHCP_FAIL		0
#define NULL			0

#ifndef BOOL
#define BOOL			u_int8_t
#define TRUE		1
#define FALSE		0
#endif
*/

typedef UCHAR (FUNC_DHCP_SENDPACKET)(UCHAR* buf, WORD len);

struct DHCPServerInfo
{
	// please notice that all values in this struct are in network byte order
	u_int32_t	StartIp; // the dhcp alloc start ip.
	u_int32_t	LeaseTime;	// lease time
	u_int32_t	RenewTime;	// renew time
	u_int32_t	Netmask;	// net mask
	u_int32_t	Router;		// route
	u_int32_t	NameServer;	// dns
	u_int32_t	BroadcastAddr;		// broad address
	u_int32_t	ServerIP;			// server ip
	u_int8_t	localMac[ETH_MAC_ADDR_LEN];	// local mac address
	u_int8_t	Domain[DOMAIN_NAME_LEN];	// domain name.	
	FUNC_DHCP_SENDPACKET* func;
};

extern ULONG	NAT_Host2NetL(ULONG HostLong);
extern ULONG	NAT_Net2HostL(ULONG NetLong);
extern WORD		NAT_Host2NetW(WORD HostWord);
extern WORD		NAT_Net2HostW(WORD NetWord);
extern WORD		NAT_UDPCheckSum(struct udphdr *_udphdr, struct iphdr *_iphdr, WORD udpDataLenInPacket);
extern WORD		NAT_CheckSum(WORD *addr, WORD len);
extern UCHAR	NAT_SendPacket2Lan(UCHAR* Packet, WORD dataLen);

// function.
BOOL DHCP_Start(struct DHCPServerInfo *info);			// start dhcp server
void DHCP_Stop();									// stop dhcp server
UCHAR DHCP_DealInPacket(UCHAR* packet, WORD len);	// deal packet.

#endif // end #define DHCP_H_
