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

#ifndef _HMAP_ASOCKET_H_
#define _HMAP_ASOCKET_H_

#ifdef WIN32
#include    <windows.h>
#else
#include    <bsdsocket.h>
typedef int SOCKET;
#endif

SOCKET ASYNC_socket (int domain, int type, int protocol);
SOCKET ASYNC_accept (SOCKET s, struct sockaddr *addr, int * addrlen);
int ASYNC_bind (SOCKET s, struct sockaddr *my_addr, int addrlen);
int ASYNC_connect (SOCKET s, struct sockaddr *serv_addr, int addrlen);
int ASYNC_listen (SOCKET s, int backlog);
int ASYNC_recv (SOCKET s, void *buf, size_t len, unsigned int flags);
int ASYNC_recvfrom (SOCKET s, void *buf, size_t len, unsigned int flags,
        struct sockaddr *from, int * fromlen);
int ASYNC_send (SOCKET s, const void *msg, int len, unsigned int flags);
int ASYNC_sendto (SOCKET s, const void *msg, int len, unsigned int flags,
        const struct sockaddr *to, int tolen);
int ASYNC_getsockopt (SOCKET s, int level, int optname, void *optval, int *optlen);
int ASYNC_setsockopt (SOCKET s, int level, int optname,
        const void *optval, int optlen);
int ASYNC_setsockopt (SOCKET s, int level, int optname,
        const void *optval, int optlen);
int ASYNC_getsockname (SOCKET s, struct sockaddr *name, int * namelen);
int ASYNC_getpeername (SOCKET s, struct sockaddr *name, int * namelen);
int ASYNC_shutdown (SOCKET s, int how);
int ASYNC_closesocket (SOCKET s);

typedef void (*ASYNC_CALLBACK)(void * param, int result, int data1, long data2);

void * ASYNC_gethostbyname (const char * name, void * buf, int buflen,
                            ASYNC_CALLBACK callback, void * param);
int ASYNC_setcallback (SOCKET s, ASYNC_CALLBACK callback, void * param);

enum async_result { async_done = 1, async_error };

#define ASYNC_RECV      1
#define ASYNC_SEND      2
#define ASYNC_CONNECT   3
#define ASYNC_ACCEPT    4

/* Define error number */
#define EASYNC      0x1234

#endif /* _HMAP_ASOCKET_H_ */
