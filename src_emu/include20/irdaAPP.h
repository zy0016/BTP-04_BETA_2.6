/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   : IrDA.h
*
* Purpose : IrDA.c 外部接口头文件
*  
* Author  : 张佳雨
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    1.0   $Date:: 2003-10-12                          $
* 
\**************************************************************************/
#define    OBEX_NUM   2

//红外传输数据函数（联系人、日程调用）
extern BOOL App_IrdaPutData(unsigned char *strName, unsigned char *pDataBuffer, int iDataLength);
//服务器端初始化函数（设置调用）
extern BOOL App_IrdaServerStart(int ObexService[], int iServiceNum);
//服务器端数据处理函数    
BOOL App_IrdaDataProcesse(int ObexService[], int iServiceNum);
