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
#include "string.h"
#include "UrlRatio.h"

#define WIE_MAXRATIONUM           10
static WIEURLRATIO WIE_RatioArray[WIE_MAXRATIONUM];
static int WIE_Account = 0;

/*****************************************************************
* 函数介绍：插入一个比率
* 传入参数：nDataLen     : Url数据总长度
nRecvDataLen : 收到的数据长度
urltype      : url数据类型(优先级别)
url          : url地址(必须以0结尾)
* 传出参数：无
* 返回    ：0：成功
1：失败
*****************************************************************/
int WIE_RatioInsert(int nDataLen, int nRecvDataLen, int urltype, char* url)
{
    if (WIE_Account == WIE_MAXRATIONUM)
        return 1;

    if (strlen(url) > WIE_MAXURLLEN)
        return 1;
    
    WIE_RatioArray[WIE_Account].nDataLen     = nDataLen;
    WIE_RatioArray[WIE_Account].nRecvDataLen = nRecvDataLen;
    WIE_RatioArray[WIE_Account].urltype      = urltype;
    strcpy(WIE_RatioArray[WIE_Account].szUrl, url);    
    WIE_Account++;
    
    return 1;
}

/*****************************************************************
* 函数介绍：得到一个比率
* 传入参数：url          : url地址(必须以0结尾)
* 传出参数：pReturn      : 一个比率结构
* 返回    ：0：成功
1：失败
*****************************************************************/
int WIE_RatioGet(char *url, PWIEURLRATIO pReturn)
{
    int i;
    for (i = 0; i < WIE_Account; i++)
    {
        if (strcmp(url, WIE_RatioArray[i].szUrl) == 0)
        {            
            pReturn->nDataLen     = WIE_RatioArray[i].nDataLen;
            pReturn->nRecvDataLen = WIE_RatioArray[i].nRecvDataLen;
            pReturn->urltype      = WIE_RatioArray[i].urltype;
            strcpy(pReturn->szUrl, WIE_RatioArray[i].szUrl);
            return 0;            
        }
    }
    
    return 1;
}

/*****************************************************************
* 函数介绍：修改比率的nRecvDatalen字段值
* 传入参数：nRecvDataLen : 收到的数据长度             
url          : url地址(必须以0结尾)
* 传出参数：无
* 返回    ：0：成功
1：失败
*****************************************************************/
int WIE_RatioAddRcv(char *url, int nRcvdataLen)
{
    int i;
    
    if ( WIE_Account == 0 )
        return 1;
    
    for (i = 0; i < WIE_Account; i++)
    {
        if (strcmp(url, WIE_RatioArray[i].szUrl) == 0)
        {        
            WIE_RatioArray[i].nRecvDataLen = WIE_RatioArray[i].nRecvDataLen + nRcvdataLen;
            return 0;
        }
    }
    
    return 1;
}

/*****************************************************************
* 函数介绍：删除一个比率
* 传入参数：url          : url地址(必须以0结尾)
* 传出参数：无
* 返回    ：0：成功
1：失败
*****************************************************************/
int WIE_RatioDelete(char *url)
{
    int i;
    
    if (WIE_Account == 0)
        return 1;
    
    for (i = 0; i < WIE_Account; i++)
    {
        if (strcmp(url, WIE_RatioArray[i].szUrl) == 0)
        {
            WIE_RatioArray[i].nDataLen     = WIE_RatioArray[WIE_Account].nDataLen;
            WIE_RatioArray[i].nRecvDataLen = WIE_RatioArray[WIE_Account].nRecvDataLen;
            WIE_RatioArray[i].urltype      = WIE_RatioArray[WIE_Account].urltype;
            strcpy(WIE_RatioArray[i].szUrl, WIE_RatioArray[WIE_Account].szUrl);            
            WIE_Account--;            
            return 0;
        }
    }
    
    return 1;
}

/*****************************************************************
* 函数介绍：情况比率句柄
* 传入参数：无
* 传出参数：无
* 返回    ：0：成功
1：失败
*****************************************************************/
void WIE_RatioClear()
{
    WIE_Account = 0;
    return;
}

/*****************************************************************
* 函数介绍：判断比率表是否为空
* 传入参数：无
* 传出参数：无
* 返回    ：0：空
1：非空
*****************************************************************/
int WIE_RationEmpty()
{
    if (WIE_Account == 0)
        return 0;
    else
        return 1;
}
