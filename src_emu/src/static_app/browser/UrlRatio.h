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

#ifndef URLRATIO_H
#define URLRATIO_H

#define WIE_MAXURLLEN  100
typedef struct tagWIEURLRatio 
{
    int nDataLen;
    int nRecvDataLen;
    int urltype;
    char szUrl[WIE_MAXURLLEN];
}WIEURLRATIO, *PWIEURLRATIO;

int WIE_RatioInsert(int nDataLen, int RecvDataLen, int urltype, char* url);
int WIE_RatioGet(char *url, PWIEURLRATIO pReturn);
int WIE_RatioDelete(char *url);
int WIE_RatioAddRcv(char *url, int nRcvdataLen);
void WIE_RatioClear();
int WIE_RationEmpty();


#endif
