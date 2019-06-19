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

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#define REG_TYPE_DIR            1
#define REG_TYPE_VALUE          2
#define REG_TYPE_STRING         3
#define REG_TYPE_DATA           4

#define REG_CURR    0
#define REG_SAVE    1

typedef void * PREG;

PREG Reg_Open(int registry, int mode);
void Reg_Close(PREG pReg);

int Reg_List(PREG pReg, const char * path, int index, char * buf, int buflen);
int Reg_Chdir(PREG pReg, const char * path);
int Reg_MakeDir(PREG pReg, const char * path);
int Reg_RemoveDir(PREG pReg, const char * path);
int Reg_MakeKey(PREG pReg, const char * path, int type);
int Reg_DeleteKey(PREG pReg, const char * path);
int Reg_GetValue(PREG pReg, const char * path, long * pValue);
int Reg_SetValue(PREG pReg, const char * path, long value);
int Reg_GetKey(PREG pReg, const char * path, void * pBuf, int buflen);
int Reg_SetKey(PREG pReg, const char * path, void * pData, int datalen);

#endif  //_REGISTRY_H_
