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
* �������ܣ�����һ������
* ���������nDataLen     : Url�����ܳ���
nRecvDataLen : �յ������ݳ���
urltype      : url��������(���ȼ���)
url          : url��ַ(������0��β)
* ������������
* ����    ��0���ɹ�
1��ʧ��
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
* �������ܣ��õ�һ������
* ���������url          : url��ַ(������0��β)
* ����������pReturn      : һ�����ʽṹ
* ����    ��0���ɹ�
1��ʧ��
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
* �������ܣ��޸ı��ʵ�nRecvDatalen�ֶ�ֵ
* ���������nRecvDataLen : �յ������ݳ���             
url          : url��ַ(������0��β)
* ������������
* ����    ��0���ɹ�
1��ʧ��
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
* �������ܣ�ɾ��һ������
* ���������url          : url��ַ(������0��β)
* ������������
* ����    ��0���ɹ�
1��ʧ��
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
* �������ܣ�������ʾ��
* �����������
* ������������
* ����    ��0���ɹ�
1��ʧ��
*****************************************************************/
void WIE_RatioClear()
{
    WIE_Account = 0;
    return;
}

/*****************************************************************
* �������ܣ��жϱ��ʱ��Ƿ�Ϊ��
* �����������
* ������������
* ����    ��0����
1���ǿ�
*****************************************************************/
int WIE_RationEmpty()
{
    if (WIE_Account == 0)
        return 0;
    else
        return 1;
}
