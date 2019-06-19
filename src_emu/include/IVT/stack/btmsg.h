/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    btmsg.h
Abstract:
	This file includes the definition of functions in back server.
Author:
    Gang He
Revision History:2000.9
---------------------------------------------------------------------------*/
#ifndef BTMSG_H
#define BTMSG_H

struct BtMsg {
	WORD id;
	UCHAR * arg;
};

void InitMsg(void);
void DoneMsg(void);
void SendMsg(WORD msgid, UCHAR * arg);
struct BtMsg * GetMsg(void);

void MsgThread(void);

#define DISCONN_L2CAP 58

#define CHECK_MSG(m,l) \
{ \
	WORD r;\
	m=GetMsg(); \
	while (m) { \
		if (m->id==0) { \
				BtListMemberFree((UCHAR*)m); \
				goto l; \
		} \
		if (g_finishing==0) { \
			if (m->id == DISCONN_L2CAP) {\
				L2CA_DisconnectReq((WORD*)(m->arg),&r);\
				FREE(m->arg);\
			}\
			else\
				ProcessMsg(m->id, m->arg); \
		} \
		BtListMemberFree((UCHAR*)m); \
		m=GetMsg(); \
	} \
}

#define CHECK_TRANSPORT()
#define CHECK_INPUT()

#endif 
