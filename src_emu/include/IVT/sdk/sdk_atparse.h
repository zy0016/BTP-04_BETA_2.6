/********************************************************************
  Copyright (C), 2005, IVT Corporation
  Module name:		sdk_atparse.h
  Author:       guozhengfei
  Version:        
  Date: 
  Abstract:   This module includes all of the functions which parse the AT command.
  Revision History:        

********************************************************************/

#ifndef _sdk_ATPARSE_H
#define _sdk_ATPARSE_H

#include "sdk_def.h"

#define MAX_AT_BUF_SIZE 40
#define MAX_BUF_SIZE 16

typedef unsigned char  uint8;
typedef signed char    int8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned int   uint32;
typedef signed int     int32;

typedef unsigned        char    uint8_t;
typedef unsigned short  int		uint16_t;
typedef unsigned long   int     uint24_t;

typedef struct
{
    uint24_t lap;		/* Lower Address Part 00..23 */
    uint8_t  uap;               /* upper Address Part 24..31 */
    uint16_t nap;		/* Non-significant    32..47 */
} BD_ADDR_T;

const uint8 *parseData(const uint8 *s, const uint8 *e, const BD_ADDR_T *addr);
static uint8 my_toupper(uint8 c);
uint8 Convert2Hex(const uint8*data, uint8 i,uint8 length);
uint32 ConvertHex(const uint8*data, uint16 length);

typedef void (AT_Resp_Func)(BTUINT8 *parg, BTUINT32 arg);

struct sequence
{
  const uint8 *data;
  uint16 length;
};

struct Inquiry
{
  uint16 duration;
  uint16 mode;
  uint16 size;
};

struct Pincode
{
  struct sequence pincode;
};
struct Pair
{
  struct sequence lap;
  struct sequence nap;
  struct sequence uap;
};
struct Class
{
  struct sequence cls;
};
struct Slave
{
  uint16 enable;
};
struct Auto
{
  uint16 enable;
};
struct Dial
{
  struct sequence number;
};
struct RegCallingNum
{
  struct sequence callingnum;
};
struct CallID
{
  struct sequence phonenum;
};
struct SetSecuMode
{
  uint16 mode;
};
struct Dtmf
{
  struct sequence key;
};
struct Mode
{
  uint16 mode;
};
struct LocalName
{
  struct sequence name;
};
void handleInquiry(const BD_ADDR_T *, const struct Inquiry *);
void handlePincode(const BD_ADDR_T *, const struct Pincode *);
void handlePair(const BD_ADDR_T *, const struct Pair *);
void handleVersion(const BD_ADDR_T *);
void handleClass(const BD_ADDR_T *, const struct Class *);
//void handleSlave(const BD_ADDR_T *, const struct Slave *);
void handleCancel(const BD_ADDR_T *);
//void handleAuto(const BD_ADDR_T *, const struct Auto *);
void handleName(const BD_ADDR_T *, const struct Pair *);
void handleJoin(const BD_ADDR_T *, const struct Pair *);
void handleLeave(const BD_ADDR_T *);
void handleTransfer(const BD_ADDR_T *, const struct Pair *);
void handleCallID(const BD_ADDR_T *, const struct CallID * num);
void handleDial(const BD_ADDR_T *, const struct Dial *);
void handleAccept(const BD_ADDR_T *);
void handleHungup(const BD_ADDR_T *);
void handleInter(const BD_ADDR_T *, const struct Pair *);
//void handleRegCallingNum(const BD_ADDR_T *, const struct RegCallingNum *);
void handleRssi(const BD_ADDR_T *, const struct Pair *);
void handleSetSecuMode(const BD_ADDR_T *, const struct SetSecuMode *);
void handleDtmf(const BD_ADDR_T *, const struct Dtmf *);
void handleReset(const BD_ADDR_T *);
void handleMode(const BD_ADDR_T *, const struct Mode *);
void handleLocalName(const BD_ADDR_T *, const struct LocalName *);
void handleGetBd(const BD_ADDR_T *);
void handleGetLocalName(const BD_ADDR_T *);
void handlePairClear(const BD_ADDR_T *, const struct Pair *);
void handleGetMode(const BD_ADDR_T *);
void handleUnrecognised(const uint8 *data, const BD_ADDR_T *addr);

void ATInquiryInd(BTDEVHDL dev_hdl);
void ATInqCompInd(void);
void ATPinReqInd(BTDEVHDL dev_hdl);
BTUINT8  ATConnReqInd(BTDEVHDL dev_hdl, BTUINT32 dev_class);
void ATConnCompleteInd(BTDEVHDL dev_hdl);
void RegATIndCallback(void);
void UnRegATIndCallback(void);
void Ctpicp_Callback(BTUINT16 event, BTUINT8 *parg, BTUINT32 arg);
void ATSendResp(BTUINT8 resp[],BTUINT16 resp_len);
void SendBdResp(const BD_ADDR_T * addr);
void SendHex(BTUINT32 i);
void Send2Hex(BTUINT16 i);
void AT_gethandle(BTUINT8 * far_addr,const struct Pair * pairaddr);
#endif
