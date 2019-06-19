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

#ifndef _X509GLB_H
#define _X509GLB_H

typedef int		X509HD; 

/* 提供给最终用户的API */
int		X509_Init(); 
X509HD  X509_EnumCers( int index );
X509HD  X509_GetCersHD( void* pHigh );
int		X509_GetIssuer( X509HD index, char* pName, int len );
int		X509_GetSubject( X509HD index, char* pName, int len );
int		X509_GetValidAfter( X509HD index, char* pName, int len );
int		X509_GetValidBefore( X509HD index, char* pName, int len );
int		X509_GetTBSCertInfo(X509HD index, char *buffer, int len);

X509HD ParseCertificate( void* pHigh, char* pdata, int len, int* result, int bNew, int *bTrust);
int	X509_GetJavaCert(X509HD index, char *pData, int DataLen);
int X509_OldtoNew(char *pSrc, int SrcLen, char *pDest, int DestLen, int bTrust);

/* 提供给最终用户的API结束 */

#endif
