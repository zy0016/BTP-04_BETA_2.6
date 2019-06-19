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

//#include "hmosal.h"
#include "plx_pdaex.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

#include "registry.h"

extern int Reg_Save();
PREG Reg_Curr_Open (int mode);
void Reg_Curr_Close(PREG pReg);
int Reg_Curr_List(PREG pReg, const char * path, int index, char * buf, int buflen);
int Reg_Curr_Chdir(PREG pReg, const char * path);
int Reg_Curr_MakeDir(PREG pReg, const char * path);
int Reg_Curr_RemoveDir(PREG pReg, const char * path);
int Reg_Curr_MakeKey(PREG pReg, const char * path, int type);
int Reg_Curr_DeleteKey(PREG pReg, const char * path);
int Reg_Curr_GetValue(PREG pReg, const char * path, long * pValue);
int Reg_Curr_SetValue(PREG pReg, const char * path, long value);
int Reg_Curr_GetKey(PREG pReg, const char * path, void * pBuf, int buflen);
int Reg_Curr_SetKey(PREG pReg, const char * path, void * pData, int datalen);

typedef struct {
    int     type;
} * REGHANDLE;

PREG Reg_Open(int registry, int mode)
{
    REGHANDLE pReg = NULL;

//    if ( registry == REG_CURR ) {
        pReg = Reg_Curr_Open(mode);
        if ( pReg )
            pReg->type = registry;
//    }
    return pReg;
}

void Reg_Close(PREG pReg)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
//        Reg_Curr_Close(pReg);
    if ( ((REGHANDLE)pReg)->type == REG_SAVE)
        Reg_Save();
    Reg_Curr_Close(pReg);
}

int Reg_List(PREG pReg, const char * path, int index, char * buf, int buflen)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_List(pReg, path, index, buf, buflen);
    return 0;
}

int Reg_Chdir(PREG pReg, const char * path)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_Chdir(pReg, path);
    return 0;
}

int Reg_MakeDir(PREG pReg, const char * path)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_MakeDir(pReg, path);
    return 0;
}

int Reg_RemoveDir(PREG pReg, const char * path)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_RemoveDir(pReg, path);
    return 0;
}

int Reg_MakeKey(PREG pReg, const char * path, int type)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_MakeKey(pReg, path, type);
    return 0;
}

int Reg_DeleteKey(PREG pReg, const char * path)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_DeleteKey(pReg, path);
    return 0;
}

int Reg_GetValue(PREG pReg, const char * path, long * pValue)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_GetValue(pReg, path, pValue);
    return 0;
}

int Reg_SetValue(PREG pReg, const char * path, long value)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_SetValue(pReg, path, value);
    return 0;
}

int Reg_GetKey(PREG pReg, const char * path, void * pBuf, int buflen)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_GetKey(pReg, path, pBuf, buflen);
    return 0;
}

int Reg_SetKey(PREG pReg, const char * path, void * pData, int datalen)
{
//    if ( ((REGHANDLE)pReg)->type == REG_CURR )
        return Reg_Curr_SetKey(pReg, path, pData, datalen);
    return 0;
}
