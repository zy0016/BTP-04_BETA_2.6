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

#ifndef _WINSOCK_H_
#define _WINSOCK_H_

/* include header files here*/
#include<hopen/sockios.h>
#include<sys/select.h>
#include<window.h>
#include<socket.h>
#include<sockaddr.h>
#include<inet.h>

/**************************************************************************\
*
*	CONSTANT DEFINITION
*
\**************************************************************************/

#ifndef INVALID_SOCKET
#define INVALID_SOCKET	(-1)
#endif

/*
 * Define flags to be used with the WSAAsyncSelect() call.
 */
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20


/*
 * WSAGETASYNCERROR is intended for use by the Windows Sockets application
 * to extract the error code from the lParam in the response
 * to a WSAGetXByY().
 */
#define WSAGETASYNCERROR(lParam)           HIWORD(lParam)
/*
 * WSAGETSELECTEVENT is intended for use by the Windows Sockets application
 * to extract the event code from the lParam in the response
 * to a WSAAsyncSelect().
 */
#define WSAGETSELECTEVENT(lParam)           LOWORD(lParam)
/*
 * WSAGETSELECTERROR is intended for use by the Windows Sockets application
 * to extract the error code from the lParam in the response
 * to a WSAAsyncSelect().
 */
#define WSAGETSELECTERROR(lParam)           HIWORD(lParam)

/*
 * Define constant based on rfc883, used by gethostbyxxxx() calls.
 */
#define MAXGETHOSTSTRUCT    1024


/* define constant here*/
#define accept				WSAAccept
#define bind				WSABind
#define closesocket			WSAClosesocket
#define connect				WSAConnect
#ifdef  ioctlsocket
#undef  ioctlsocket
#define ioctlsocket			WSAIoctlsocket
#endif
#define getpeername			WSAGetpeername
#define getsockname			WSAGetsockname
#define getsockopt			WSAGetsockopt
#define htonl				WSAHtonl
#define htons				WSAHtons
#define inet_addr			WSAInet_addr
#define inet_ntoa			WSAInet_ntoa
#define listen				WSAListen
#define ntohl				WSANtohl
#define ntohs				WSANtohs
#define recv				WSARecv
#define recvfrom			WSARecvfrom
#define select				WSASelect
#define send				WSASend
#define sendto				WSASendto
#define setsockopt			WSASetsockopt
#define shutdown			WSAShutdown
#define socket				WSASocket
#define gethostbyaddr		WSAGethostbyaddr
#define gethostbyname		WSAGethostbyname
#define gethostname			WSAGethostname
#define getservbyport		WSAGetservbyport
#define getservbyname		WSAGetservbyname
#define getprotobynumber	WSAGetprotobynumber
#define getprotobyname		WSAGetprotobyname

#define SockStartup			WSAStartup
#define SockCleanup			WSACleanup

/**************************************************************************\
*
*	DATA STRUCTURE & DATA TYPE DEFINITION
*
\**************************************************************************/

typedef int (*POSTMSG)(void*, unsigned, long, long);

/**************************************************************************\
*
*	FUNCTION PROTOTYPE DECLARATION
*
\**************************************************************************/

/* winsock initial function */
int		WSAWinsockInit	(void);
int		WSASockInstance	(POSTMSG pPostMsg);

/* socket function prototypes */
SOCKET	WSAAccept		(SOCKET s, struct sockaddr* addr, int* addrlen);
int		WSABind			(SOCKET s, struct sockaddr* addr, int namelen);
int		WSAClosesocket	(SOCKET s);
int		WSAConnect		(SOCKET s, struct sockaddr* name, int namelen);
int		WSAIoctlsocket	(SOCKET s, long cmd, unsigned long* argp);
int		WSAGetpeername	(SOCKET s, struct sockaddr* name,int* namelen);
int		WSAGetsockname	(SOCKET s, struct sockaddr* name, int* namelen);
int		WSAGetsockopt	(SOCKET s, int level, int optname, char* optval, int*optlen);

unsigned long  WSAHtonl	(unsigned long hostlong);
unsigned short WSAHtons	(unsigned short hostshort);
unsigned long  WSAInet_addr(char* cp);
char*	WSAInet_ntoa	(struct in_addr in);

int		WSAListen		(SOCKET s, int backlog);
unsigned long  WSANtohl	(unsigned long netlong);
unsigned short WSANtohs	(unsigned short netshort);
int		WSARecv			(SOCKET s, char* buf, int len, int flags);
int		WSARecvfrom		(SOCKET s, char* buf, int len, int flags, 
						 struct sockaddr* from, int* fromlen);
int		WSASelect		(int nfds, fd_set* readfds, fd_set* writefds, 
						 fd_set* exceptfds, struct timeval* timeout);
int		WSASend			(SOCKET s, char* buf, int len, int flags);
int		WSASendto		(SOCKET s, char* buf, int len, int flags,
						 struct sockaddr* to, int tolen);
int		WSASetsockopt	(SOCKET s, int level, int optname,
						 char* optval, int optlen);
int		WSAShutdown		(SOCKET s, int how);
SOCKET	WSASocket		(int af, int type, int protocol);

/* Database function prototypes */
struct hostent* WSAGethostbyaddr(char* addr, int len, int type);
struct hostent* WSAGethostbyname(char* name);
int		WSAGethostname	(char* name, int namelen);

/*	Microsoft Windows Extension function prototypes */
int		WSAStartup		(unsigned long wVersionRequired, PSOCKDATA lpWSAData);
int		WSACleanup		(void);
void	WSASetLastError	(int iError);
int		WSAGetLastError	(void);

HANDLE	WSAAsyncGetHostByName(HWND hWnd, unsigned int wMsg,
							  char* name, char* buf, int buflen);

HANDLE	WSAAsyncGetHostByAddr(HWND hWnd, unsigned int wMsg,
							  char* addr, int len, int type,
							  char* buf, int buflen);

int		WSACancelAsyncRequest(HANDLE hAsyncTaskHandle);

int		WSAAsyncSelect(SOCKET s, HWND hWnd, unsigned int wMsg, long lEvent);

/**************************** End Of Head File****************************/
#endif	/* _WINSOCK_H_*/
