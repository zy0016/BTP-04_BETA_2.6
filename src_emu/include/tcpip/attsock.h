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


#ifndef __ATTSOCKET_H
#define __ATTSOCKET_H

//#include "hp_kern.h"

#ifndef _LINUX_SYNC_SELECT_

#define select(nfds, rfds, wfds, efds, t)		attsockselect(nfds, rfds, wfds, efds, t)

#ifndef _POSIXCOMP_
#define fd_set		att_fd_set
#define timeval		att_timeval

#define FD_CLR		SL_FDClr
#define FD_ISSET	SL_FDisSet
#define FD_SET		SL_FDSet
#define FD_ZERO		SL_FDZero
#endif /* _POSIXCOMP_ */

#endif /* _LINUX_SYNC_SELECT_ */


#ifdef _POSIXCOMP_
#define fd_set		att_fd_set
#define timeval
#else
#endif


/* 默认标记设置 */
#define DEFAULT_FLAGS		0

/* TCP port numbers */
#define	IPPORT_ECHO			7	/* Echo data port 			*/
#define	IPPORT_DISCARD		9	/* Discard data port 		*/
#define	IPPORT_FTPD			20	/* FTP Data port 			*/
#define	IPPORT_FTP			21	/* FTP Control port 		*/
#define	IPPORT_TELNET		23	/* Telnet port 				*/
#define IPPORT_SMTP			25	/* Mail port 				*/
#define	IPPORT_MTP			57	/* Secondary telnet protocol 	*/
#define	IPPORT_FINGER		79	/* Finger port 				*/
#define IPPORT_HTTP			80	/* Http Browser Port		*/	
#define	IPPORT_TTYLINK		87	/* Chat port 				*/
#define IPPORT_POP			109	/* pop2 port 				*/
#define	IPPORT_NNTP			119	/* Netnews port 			*/
#define	IPPORT_LOGINSERVER	513	/* BSD rlogin port 			*/
#define	IPPORT_TERM			5000	/* Serial interface server port	*/

/* UDP port numbers */
#define IPPROT_DAYTIME		13
#define	IPPORT_DOMAIN		53
#define	IPPORT_BOOTPS		67
#define	IPPORT_BOOTPC		68
#define	IPPORT_PHOTURIS		468	/* Photuris Key management 	*/
#define	IPPORT_EFSSERVER	520
#define	IPPORT_REMOTE		1234	/* Pulled out of the air 		*/
#define	IPPORT_BSR			5000	/* BSR X10 interface server port (UDP) */

/*SOCKET中间的标记字段*/
#define	P_PERMENTLENT		0

/*SOCKET中间的family设置*/
/*定义家族数值保证兼容*/
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */
#define AF_FIREFOX      19              /* FireFox */
#define AF_UNKNOWN1     20              /* Somebody is using this! */
#define AF_BAN          21              /* Banyan */

#define AF_MAX          22

/*协议设置*/
#define DEFAULT_PROTOCAL	0

/*SOCKET中间的mode设置*/
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

/*listen指定的最大队列长度*/
#define SOMAXCONN           15


/*异步消息选择定义（asyncselect）*/
#define	FD_READ				0x01
#define	FD_WRITE			0x02
#define	FD_OOB				0x04
#define	FD_ACCEPT			0x08
#define	FD_CONNECT			0x10
#define	FD_CLOSE			0x20


/***************************************\
		SOCKET中间的错误信息设置

	-1				: global error num
	-100 —— -200	: Socket Error Num
\***************************************/

#define INVALID_SOCKET		-1			/* 无效SOKCET */	
#define SOCKET_ERROR		-1			/* Gloabl Error */

#ifndef	EWOULDBLOCK
#define	EWOULDBLOCK			-100		/*阻塞错误			*/
#define	ENOTCONN			-101		/*未连接错误		*/
#define ESOCKTNOSUPPORT		-102		/*本地址族中不支持该类型套接口*/
#define	EAFNOSUPPORT		-103		/*家族不支持		*/
#define	EISCONN				-104		/*已连接错误		*/
#define	EOPNOTSUPP			-105		/*选项不支持		*/
#define	EALARM				-106		/*警报				*/
#define	EABORT				-107		/*放弃				*/
#undef	EINTR
#define	EINTR				-108		/*用户调用放弃		*/
#define	ECONNREFUSED		-109		/*连接被拒绝		*/
#define EMSGSIZE			-110		/*消息长度错误		*/
#define	EADDRINUSE			-111		/*地址被占用		*/
#define	EMIN				-112		/*最小值溢出		*/
#define	EMAX				-113		/*最大值溢出		*/
#define	ESOCKTABLEFULL		-114		/*SOCKET表满		*/
#define	ENOMEMORY			-115		/*内存不够			*/
#define	ENOTSOCK			-116		/*SOCKET句柄无效	*/
#define	EINVALIDPARA		-117		/*参数无效			*/
#define	EADDRFORMAT			-118		/*地址格式不对		*/
#define	EPROTNOSUPPORT		-119		/*协议不支持		*/
#define ELOWSYSRESOURCE		-120		/*系统资源不够		*/
#define ETIMEOUT			-121		/*超时错误			*/
#define ERESET				-122		/*连接被复位		*/
#define EINPROGRESS			-123		/*一个阻塞的调用正在被执行 */
#define ECONNABORTED		-124		/*连接被放弃		*/
#define ENETDOWN			-125		/*网络不通			*/
#define ENOTINITIALISED		-126		/*未初始化（SockStartup没有被调用） */
#define EINVALID            -127        /* 无效操作 */
#define EUNKNOWERROR		-199		/*未知错误			*/
#define EMAXSOCKERRORNUM	-200		/*最大错误数值*/
#endif
/*Error code about name query:*/
#define	NO_ERROR			0
#define	FORMAT_ERROR		1
#define	SERVER_FAIL			2
#define	NAME_ERROR			3
#define	NOT_IMPL			4
#define	REFUSED				5
#define RUNING_ERROR		6
#define NOBUF_ERROR			7
#define TIMEOUT_ERROR		8
#define	USERCANCLE_ERROR	9

typedef int		SOCKET;		/*SOCKET句柄结构*/

/*定义一批字符串替换，用于本系统与WINSOCK的兼容*/

#define WSAGetLastError				OS_GetError
#define	WSAAsyncSelect				asyncselect
#define	WSAAsyncGetHostByName		asyncGetHostByName
#define	WSAAsyncGetHostByAddr		asyncGetHostByAddr
#define WSAStartup					SockStartup
#define WSACleanup					SockCleanup
#define WSADESCRIPTION_LEN			DESCRIPTION_LEN
#define WSASYS_STATUS_LEN			SYS_STATUS_LEN
#define WSACancelAsyncRequest		asyncCancelAsyncRequest

/*定义结构保证与WINSOCK兼容*/
struct linger
{
    unsigned short l_onoff;            // option on/off
    unsigned short l_linger;           // linger time
};
typedef struct linger LINGER;


//用户协议设置
#define	TCP_IP				2

/*统一地址结构		*/
struct sockaddr 
{
	unsigned short	sa_family;
    char    sa_data[22];
};
typedef struct sockaddr SOCKADDR;

struct in_addr 
{
    union 
	{
		struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { unsigned short s_w1,s_w2; } S_un_w;
		unsigned long S_addr;
    } S_un;
};
typedef struct in_addr IN_ADDR;

/*INTERNET地址结构	*/
struct sockaddr_in 
{
    unsigned short  sin_family;
    unsigned short  sin_port;
    struct	in_addr sin_addr;
	char unused[16];
};
typedef struct sockaddr_in SOCKADDR_IN;

/* 组播地址 */
struct ip_mreq
{
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

#define INADDR_ANY              (unsigned long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (unsigned long)0xffffffff
#define INADDR_NONE             0xffffffff

SOCKET socket(int af, int type, int protocol);
int closesocket(SOCKET s);
int asyncselect(SOCKET s, HWND hWnd, unsigned int wMsg, long lEvent);
int getsockname(SOCKET s, struct sockaddr* addr, int* addrlen);
int gethostname( char* name, int namelen );
int getpeername(SOCKET s, struct sockaddr* name,int* namelen);
int getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen);
int ioctlsocket(SOCKET s, long cmd, unsigned long* parg);
int setsockopt(SOCKET s, int level, int optname, char* optval, int optlen);

//Function in socketf.c
SOCKET accept( SOCKET s, struct sockaddr* addr, int* addrlen );
int bind( SOCKET s, const struct sockaddr* addr, int addrlen );
int connect( SOCKET s, const struct sockaddr* addr, int addrlen );
int listen( SOCKET s, int backlog );
int recv( SOCKET s, char* pbuf, int len, int flags );
int recvfrom( SOCKET s, char* pbuf, int len, int flags, 
			 struct sockaddr* from, int* fromlen );
int send( SOCKET s, const char* pbuf, int len, int flags);
int sendto( SOCKET s, const char* pbuf, int len, int flags,
		   const struct sockaddr* to, int tolen);
int shutdown( SOCKET s, int how );

/*SOCKET选项级别以及协议数值的设置（getsockopt和setsockopt）*/
#define	SOL_SOCKET      0xffff          /*Socket 级的选项。	*/
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_IGMP            2               /* group management protocol */
#define IPPROTO_GGP             3               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */

#define IPPROTO_RAW             255             /* raw IP packet */
#define IPPROTO_MAX             256

#define	IPPROTO_PPP				100				/*PPP 级的选项。		*/
/*与socket有关的或一些全局作用的选项（getsockopt和setsockopt）*/
#define SO_BROADCAST			1	/*允许传送广播信息*/
#define SO_DEBUG				2	/*记录调试信息*/
#define SO_DONTLINGER			3	/*在有未发送数据的socket上执行close操作时
										不等待数据发送出去*/
#define SO_KEEPALIVE			4	/*周期性的发送kick数据以保持连接*/
#define SO_LINGER				5	/*在有未发送数据的socket上执行close操作时
										必须等待数据发送出去*/
#define SO_OOBINLINE			6	/*接收带外数据（第一版不支持）*/
#define SO_RCVBUF				7	/*设置接收缓冲区的长度*/
#define SO_SENDBUF				8	/*设置发送缓冲区的长度*/
#define SO_RCVTIMEO				9	/*设置接收数据超时的时间，
									第一版不支持socket级的超时控制*/
#define SO_SNDTIMEO				10	/*设置发送数据超时的时间，
									第一版不支持socket级的超时控制*/
#define SO_RCVLOWAT				11	/*设置接收数据的低水平数值，在第一版无意义*/
#define SO_SNDLOWAT  			12	/*设置发送数据的低水平数值，在第一版无意义*/
#define TCP_NODELAY				13	/*禁止发送coalescing的Nagle算法*/
#define SO_REUSEADDR			14	/* 允许地址复用 */
#define SO_ACCEPTCONN			15	
#define SO_ERROR				16
#define SO_MAX_MSG_SIZE			17	

#define IP_MULTICAST_IF			20	/* 设置组播地址 */
#define IP_ADD_MEMBERSHIP		21	/* 加入组播组 */
#define IP_DROP_MEMBERSHIP		22	/* 离开组播组 */
#define IP_MULTICAST_TTL		23	/* 设置组播生存时间 */
#define IP_MULTICAST_LOOP		24	/* 设置组播数据的回送 */

/*用于函数（ioctlsocket）的控制命令*/
#define	FIONBIO					1
#define	FIONREAD				2
#define	SIOCATMARK				3

/*用于函数shutdown()的断联控制方式*/
#define SD_RECEIVE				1 /*不允许后继的recv操作	*/
#define	SD_SEND					2 /* 不允许后继的send操作	*/
#define SD_BOTH					3 /* 断开连接并禁止读、写操作 */	

/* 用于函数接收和发送 */
#define MSG_OOB				0x01	/* 带外数据 */
#define MSG_PEEK			0x02	/* 检测数据 */

/*******************************************************\
	Define those function of socket lib
\*******************************************************/
/*用于定义域名查询的缓存长度。*/
#define MAXGETHOSTSTRUCT	100

struct hostent 
{
	char*	h_name;
	char**	h_aliases;
	short	h_addrtype;
	short	h_length;
	char**	h_addr_list;
};
typedef struct hostent HOSTENT;
/*得到消息中间Param的参数意义*/

//#define WSAGETEVENT(lParam)			loword(lParam)
//#define WSAGETSELECTERROR(lParam)	hiword(lParam)

/*SOCKET启动初始化及退出函数*/
/*所需结构*/
#define DESCRIPTION_LEN      256
#define SYS_STATUS_LEN       128
typedef struct tagSOCKData 
{
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription[DESCRIPTION_LEN+1];
        char                    szSystemStatus[SYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char*					lpVendorInfo;
} SOCKDATA;
typedef SOCKDATA	WSADATA;
typedef SOCKDATA*	PSOCKDATA;
/* 加入结构和定义支持同步检测函数select */
#define FD_SETSIZE	64
typedef struct tagfd_set 
{
	unsigned long fd_count;			/* how many are SET? */
	SOCKET fd_array[FD_SETSIZE];	/* an array of SOCKETs */
}att_fd_set;
 
struct att_timeval 
{
	long tv_sec;         /* seconds */
	long tv_usec;        /* and microseconds */
};

/*函数*/
int SockStartup( DWORD wVersionRequested, PSOCKDATA lpSOCKData );
int SockCleanup (void); 
/*SOCKET启动初始化及退出函数到此结束*/
/* 公开函数 */
struct hostent* gethostbyname(char* sname);
struct hostent* gethostbyaddr( char* addr, int len, int type );

HANDLE asyncGetHostByName( HWND hWnd, unsigned int wMsg, 
							  const char* name, char* buf, 
							  int buflen ); 
HANDLE asyncGetHostByAddr( unsigned long hWnd, unsigned long wMsg, char *addr, 
							int len, int type, char* buf, int buflen );

int asyncCancelAsyncRequest(HANDLE hAsyncTaskHandle);
char* inet_ntoa ( struct in_addr in);
unsigned long inet_addr( const char* cp );
unsigned short htons(unsigned int port);
unsigned long htonl(unsigned long port);
unsigned short ntohs(unsigned int port);
unsigned long ntohl(unsigned long port);
int MakeIPAddr( struct sockaddr * paddr, char * ipaddr, int port );

int SL_FDClr( SOCKET s, att_fd_set* pset );
int SL_FDisSet( SOCKET s, att_fd_set* pset );
int SL_FDSet( SOCKET s, att_fd_set* pset );
int SL_FDZero( att_fd_set* pset );

int attsockselect( int nfds, att_fd_set* readfds, att_fd_set* writefds, att_fd_set* exceptfds, 
		   const struct att_timeval* timeout );
int scancleblock( SOCKET s );
/*制作一个错误码处理函数，将相应错误码以字符串方式返回*/
char* dealerror();

#define MAX_HOST_NAME	60		/* 本机名字的最大长度 */
typedef struct ping_param
{
	char	dest[MAX_HOST_NAME];/* 主机名字			*/
	int		len;				/* 主机地址长度		*/
	short	resolve;			/* 是否解析主机地址 */
	short	times;				/* 发送的次数		*/
	short	buflen;				/* 发送缓冲区大小	*/
	short	frag;				/* 设置不要分片		*/
	short	route;				/* 记录路由			*/
	short	loose;				/* 松散路由			*/
	short	strict;				/* 严格路由			*/
	short	timestamp;			/* 记录时间戳		*/
	char	host[MAX_HOST_NAME];/* 主机名字			*/
	unsigned long addr;
	unsigned char ttl;			/* time to live		*/
	unsigned char tos;			/* 设置服务质量		*/
	unsigned char unused[2];
}PING_PARAM;

int ping_execute(PING_PARAM params, void *call_back);

void TCPIP_Init( void* hWnd );

#endif
