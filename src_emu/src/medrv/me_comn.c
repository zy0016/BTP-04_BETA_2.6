/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "stdlib.h"

#include "malloc.h"
#include "window.h"
#include "string.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"

#ifndef NULL
#define NULL	(void *)0
#endif

#define	RESULT_BUFSIZE	    1000
static char me_result_buf[RESULT_BUFSIZE];

typedef struct tagME_MemBlock
{
	char*	p;
	int		len;
	int		offset;

}ME_MemBlock, *PME_MemBlock;

ME_MemBlock	ME_RcvBuf;

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
void ME_MemInitial( void )
{
	if ( ME_RcvBuf.len > RESULT_BUFSIZE )
		free( ME_RcvBuf.p );

	ME_RcvBuf.p = me_result_buf;
	memset( ME_RcvBuf.p, 0, RESULT_BUFSIZE );
	ME_RcvBuf.len = RESULT_BUFSIZE;
	ME_RcvBuf.offset = 0;

	return;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
BOOL ME_Malloc( int len )
{
	char* current;

	if ( ME_RcvBuf.len == RESULT_BUFSIZE )
	{
		current = ME_RcvBuf.p;

		ME_RcvBuf.p = malloc( len );

		if ( NULL == ME_RcvBuf.p )
		{
			ME_RcvBuf.p = current;
			return FALSE;
		}

		memset( ME_RcvBuf.p, 0, len );
		memcpy( ME_RcvBuf.p, current, ME_RcvBuf.offset );
		memset( current, 0, RESULT_BUFSIZE );
	}
	else
	{
		current = ME_RcvBuf.p;

		ME_RcvBuf.p = realloc (ME_RcvBuf.p, len);

		if ( NULL == ME_RcvBuf.p )
		{
			ME_RcvBuf.p = current;
			return FALSE;
		}
		memset( &ME_RcvBuf.p[ME_RcvBuf.len], 0, len-ME_RcvBuf.len );
	}

	ME_RcvBuf.len = len;

	return TRUE;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int ME_MemAssign( char* data, int datalen )
{
	if ( ME_RcvBuf.offset + datalen >=  ME_RcvBuf.len )
	{
		if ( !ME_Malloc( ME_RcvBuf.len + RESULT_BUFSIZE ) )
			return -1;
	}

	memcpy( &ME_RcvBuf.p[ME_RcvBuf.offset], data, datalen );

	ME_RcvBuf.offset += datalen;

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
char* ME_MemGetPntr(void)
{
	return ME_RcvBuf.p;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_MemGetOffset(void)
{
	return ME_RcvBuf.offset;
}

/***************************************************************
* Purpose: 从给定字符串中得到指定的整型值
* 
* Parameter:
*	pSrc:		给定字符串
*	pnResult:	得到的整型值
*	Identf:		验证给定字符串是否合法的标识符，如"+CPBR:"
*	index:		需要得到的变量索引，字符串型变量不参与计算，索引从1开始
*  
* Return Value:
*	!=-1:  解析成功
*	-1: 解析失败
*****************************************************************/
int ME_GetIntSection( char* pSrc, char* Identf, int index )
{
	char* pBegin = pSrc;
	char *pEnd = NULL;
	char pResult[20];
	int nResult = 0;
	int nSectionNum = 0;
	int nSecLen;

	if (strlen(Identf) > 0 )
	{
		pBegin = strstr(pSrc, Identf);
		if ( pBegin == NULL )
			return -1;
		pBegin += (strlen(Identf));
	}

	while ( (pEnd = strchr(pBegin, ',')) != NULL )
	{
		while (*pBegin == ' ')
			pBegin++;
		if ((pEnd == pBegin))
		{
			pBegin++;
			continue;
		}
		if (*pBegin == '"')
		{
			pBegin = pEnd+1;
			continue;
		}
		nSectionNum++;
		if (nSectionNum == index)
		{
			nSecLen = pEnd-pBegin;
			memcpy(pResult, pBegin, nSecLen);
			pResult[nSecLen] = '\0';
			nResult = atoi(pResult);
			return nResult;
		}
		pBegin = pEnd+1;
	}

	if (strlen(pBegin) > 0 )
	{
		while (*pBegin == ' ')
			pBegin++;

		if ( *pBegin != '"' && nSectionNum == index-1)
		{
			nSectionNum++;
			nSecLen = strlen(pBegin);
			memcpy(pResult, pBegin, nSecLen);
			pResult[nSecLen] = '\0';
			nResult = atoi(pResult);
			
			return (nResult);
		}
	}

	return -1;
}

/***************************************************************
* Purpose: 从给定字符串中得到指定的字符串
* 
* Parameter:
*	pSrc:		给定字符串
*	pResult:	得到的字符串
*	Identf:		验证给定字符串是否合法的标识符，如"+CPBR:"
*	index:		需要得到的变量索引，整型变量不参与计算，索引从1开始
*  
* Return Value:
*	TRUE:  解析成功
*	FALSE: 解析失败
*****************************************************************/
int ME_GetStrSection(char* pSrc, char* pResult, int nLen, char* Identf, int index)
{
	char* pBegin = pSrc;
	char* pEnd = NULL;
	char *pFirstQuota, *pSecondQuota;
	int nSectionNum = 0;
	int nSecLen;

	if ( strlen(Identf) > 0 ) {
		pBegin = strstr(pSrc, Identf);
		if ( pBegin == NULL )
			return -1;
		pBegin += (strlen(Identf));
	}

	pFirstQuota = strchr (pBegin, '"');
	if ( pFirstQuota == NULL )
		return -1;
	pSecondQuota = strchr (pFirstQuota+1, '"');
	if ( pSecondQuota == NULL )
		return -1;

	while ( (pEnd=strchr(pBegin, ',')) != NULL ) {
		if (pEnd < pSecondQuota) {
			pBegin = pEnd+1;
			continue;
		}
		if ((pEnd == pBegin)) {
			pBegin++;
			continue;
		}

		pBegin = pEnd+1;
		nSectionNum++;

		if ( nSectionNum == index ) {
			nSecLen = pSecondQuota - pFirstQuota - 1;
            if ( nLen < nSecLen + 1 ) {
                memcpy (pResult, pFirstQuota+1, nLen-1);
                pResult[nLen-1] = '\0';
                return nLen;
            }
			if ( nSecLen < 0 ) {
				memset(pResult, 0, nLen);
				return -1;
			}
			else {
				memcpy (pResult, pFirstQuota+1, nSecLen);
				pResult[nSecLen] = 0;
				return nSecLen+1;
			}
		}
		pFirstQuota = strchr(pBegin, '"');
		if (!pFirstQuota)
			break;
		pSecondQuota = strchr(pFirstQuota+1, '"');
		if (!pSecondQuota)
			break;
	}

	if ( pFirstQuota && pSecondQuota && nSectionNum == index-1 ) {
		nSecLen = pSecondQuota-pFirstQuota-1;

        if ( nLen < nSecLen +1 ) {
            memcpy (pResult, pFirstQuota+1, nLen-1);
            pResult[nLen-1] = '\0';
            return nLen;
        }

		if (nSecLen < 0) {
			memset(pResult, 0, nLen);
			return -1;
		}
		else {
			memcpy(pResult, pFirstQuota+1, nSecLen);
			pResult[nSecLen] = 0;
			return nSecLen+1;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_CurCalls_Dec( char *Scr, PCALLS_INFO pCallInfo )
{
	//+CLCC: 1,0,0,0,0,"01062629970",129,"01062629970+507"
	//+CLCC: 2,1,5,0,0,"13641190277",161
	char	*pBegin;
	int		type;
	char	TempText[100];

	if (strstr(Scr, "+CLCC: ") != Scr)
		return -1;

	pBegin	= Scr + strlen( "+CLCC: ");

	if ( pBegin == NULL )
		return -1;

	pCallInfo->index = ME_GetIntSection( pBegin, "", 1 );
	pCallInfo->Dir = ME_GetIntSection( pBegin, "", 2 );
	pCallInfo->Stat = ME_GetIntSection( pBegin, "", 3 );
	pCallInfo->Mode = ME_GetIntSection( pBegin, "", 4 );
	pCallInfo->Mpty = ME_GetIntSection( pBegin, "", 5 );

	type = ME_GetIntSection( pBegin, "", 6);

	if ( 0 > ME_GetStrSection (pBegin, pCallInfo->PhoneNum, ME_PHONENUM_LEN, "", 1) )
        memset (pCallInfo->PhoneNum, '\0', ME_PHONENUM_LEN);

	if ( 0 > ME_GetStrSection (pBegin, TempText, ME_PHONETXT_LEN, "", 2) )
		memset (pCallInfo->Text, '\0', ME_PHONETXT_LEN);
	else
		PhoneText_Decode (pCallInfo->Text, ME_PHONETXT_LEN, TempText);

	return 0;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_PhoneBook_Dec( char *Scr, PPHONEBOOK pDesPB )
{
	//+CPBR: 4,"+32498",145,"dsafa"
	char	*pBegin, *pEnd, *pSpe;
	int		len;
	int		type;
	char	TempText[100];
	char	PDUText[100];
	char	WideText[100];

	if (strstr(Scr, "+CPBR: ") != Scr && strstr(Scr, "+CPBP: ") != Scr
		&& strstr(Scr, "+CPBF: ") != Scr && strstr(Scr, "+CPBN: ") != Scr)
		return -1;

	memset(TempText, 0, 100);
	memset(PDUText, 0, 100);
	memset(WideText, 0, 100);

	pBegin	= Scr + strlen( "+CPBR: ");
	pEnd	= strstr( pBegin, "," );
	len = pEnd - pBegin;

	*pEnd = '\0';

	pDesPB->Index = atoi( pBegin );

	pBegin	= strstr( pEnd+1, "\"" );
	pEnd	= strstr( pBegin+1, "\"" );
	len = pEnd - pBegin;

	type = ME_GetIntSection( pBegin, "", 1 );

	if ( type == 145 && *(pBegin+1) != '+' )
	{
		*(pDesPB->PhoneNum) = '+';
		memcpy( (pDesPB->PhoneNum)+1, pBegin+1, len-1 );
		pDesPB->PhoneNum[len]=0;
	}
	else
	{
		memcpy( pDesPB->PhoneNum, pBegin+1, len-1 );
		pDesPB->PhoneNum[len-1]=0;
	}

	pBegin	= strstr( pEnd+1, "\"" );
	while( NULL != (pSpe =strstr( pEnd+1, "\"" )) )
		pEnd = pSpe;

	if (pEnd != pBegin)
		len = pEnd - pBegin;
	else
		len = strlen(pBegin);
	memcpy(TempText, pBegin+1, len-1);
	TempText[len-1] = 0;

	PhoneText_Decode( pDesPB->Text, ME_PHONETXT_LEN, TempText );

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_NetOpratorDec( NETOPRATOR* pNetOp, char* pBuf, int BufCount )
{
	char Section[100];
	char *pBegin, *pEnd;
	int nCount;

	if ( BufCount <= 0 )
		return 0;

	nCount = 0;

	pBegin = strchr(pBuf, '(');

	if (pBegin != NULL )
	{
		while ( (pEnd = strchr(pBegin, ')')) != NULL )
		{
			memcpy(Section, pBegin, pEnd-pBegin+1);

			Section[pEnd-pBegin+1] = 0;
			if ( ME_GetStrSection(Section, pNetOp[nCount].alpha_oper, ME_OPRATOR_LEN, "(", 1 ) == -1 )
				break;

			ME_GetStrSection(Section, pNetOp[nCount].num_oper, ME_OPRATOR_LEN, "(", 2);
			pNetOp[nCount].stat = ME_GetIntSection(Section, "(", 1);

			nCount++;

			if ( nCount >= BufCount )
				return nCount;

			pBegin = strchr(pEnd, '(');

			if (pBegin == NULL)
				break;
		}
	}

	return nCount;
}

void PhoneText_Decode( char* pDst, int nDstLen, char* pSrc )
{
	char PDUStr[100], WideStr[100];

	memset( PDUStr, 0, sizeof(PDUStr) );
	memset( WideStr, 0, sizeof(WideStr) );

    UCS2Decode (WideStr, pSrc, strlen(pSrc));
    WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)WideStr, -1,
			(LPSTR)pDst, nDstLen, 0, 0);

	return;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int PhoneText_Encode( char* pDesStr, char* pSrcStr, unsigned int DestLen )
{
	char WideText[100];
	char PDUText[100];
	int len;
	unsigned int i = 0;

	while ( pSrcStr[i] != '\0' )
	{
		if ( (BYTE)(pSrcStr[i]) >= 0xA0 )
			break;

		i++;
	}

	if ( i == strlen(pSrcStr) )
	{
		if ( strlen(pSrcStr) >= DestLen )
		{
			strcpy( pDesStr, "" );
			return 0;
		}
		strcpy( pDesStr, pSrcStr );
		return 0;
	}
	else
	{
		memset (WideText, 0, sizeof(WideText));
		memset (PDUText, 0, sizeof(PDUText));

		len = MultiByteToWideChar (CP_ACP, 0, pSrcStr, strlen(pSrcStr), (LPWSTR)WideText, sizeof(WideText));

        UCS2Encode (PDUText, WideText, len*2);

		if ( strlen( PDUText ) >= DestLen )
		{
			strcpy( pDesStr, "" );
			return 0;
		}

		strcpy( pDesStr, PDUText );
		return 1;
	}
}

int ServCell_Decode( PSCELL_INFO pDest, char* pSource, int Status )
{
	char* p, *q;
	char* token[20];
	int i = 0;

	if ( Status == ME_RS_SEARCHING )
		return -1;

	q = strstr( pSource, "^SMONC:" );
	p = strchr( pSource, '\r');

	if ( p == NULL )
		return -1;

	p += 2;

	if ( p == NULL )
		return -1;

	token[0] = strtok( p, " " );

	while ( token[i] != NULL && i < 20 )
	{
		i++;
		token[i] = strtok( NULL, " " );
	}

	strcpy( pDest->BCH, token[0] );
	strcpy( pDest->RSSI, token[1] );
	strcpy( pDest->BRXL, token[2] );
	strcpy( pDest->MCC, token[3] );
	strcpy( pDest->MNC, token[4] );
	strcpy( pDest->LAC, token[5] );

	strcpy( pDest->CELLID, token[6] );
	strcpy( pDest->NCC, token[7] );
	strcpy( pDest->BCC, token[8] );
	strcpy( pDest->PWR, token[9] );
	strcpy( pDest->RXLev, token[10] );
	strcpy( pDest->C1, token[11] );
	
	
	if ( Status == ME_RS_SUCCESS )
	{
		strcpy( pDest->TCH, token[13] );
		strcpy( pDest->TS, token[14] );
		strcpy( pDest->TA, token[15] );
		strcpy( pDest->PWRLev, token[16] );
		strcpy( pDest->TRXL, token[17] );
		strcpy( pDest->Q, token[18] );
		strcpy( pDest->Chmod, token[19] );
	}

	q += strlen( "^SMONC:" )+1;

	p = q;

	token[0] = strtok( p, "," );
	i = 0;

	while ( token[i] != NULL && i < 9 )
	{
		i++;
		token[i] = strtok( NULL, "," );
	}

	strcpy( pDest->BSIC, token[4] );
	strcpy( pDest->C2, token[8] );

	return sizeof( SCELL_INFO );
}

int NeibCell_Decode( PNCELL_INFO pDest, char* pSource )
{
	char* p;
	char* token[8];
	int i = 0, j = 0;

	p = strchr( pSource, '\r');

	if ( p == NULL )
		return -1;

	p += 2;

	if ( p == NULL )
		return -1;

	token[0] = strtok( p, " \r\n" );
	while ( j < 6 )
	{
		if ( j > 0 )
			token[0] = strtok( NULL, " \r\n" );
		i = 0;

		while ( token[i] != NULL && i < 7 )
		{
			i++;
			token[i] = strtok( NULL, " \r\n" );
		}

		strcpy( pDest[j].BCH, token[0] );
		strcpy( pDest[j].BRXL, token[2] );
		strcpy( pDest[j].C1, token[6] );
		strcpy( pDest[j].C2, token[7] );
		j++;
	}

	return 6*sizeof( NCELL_INFO );
}

static int ConvertUcs2 (char * dst, const char* src, int srclen )
{
	int i;
	char c1, c2;

	for ( i = 0; i < srclen; i+=2 ) {
		c1 = src[i];
		c2 = src[i+1];
		dst[i] = c2;
		dst[i+1] = c1;
	}
	return 0;
}

int String2Bytes (char* dst, const char* src, int srclen)
{
    unsigned char b;
	int i;

	for ( i = 0; i < srclen; i += 2 ) {
		if ( *src >= '0' && *src <= '9' )
			b = (*src - '0') << 4;
		else
			b = (*src - 'A' + 10) << 4;

		src++;
		if( *src >= '0' && *src <= '9' )
			b |= *src - '0';
		else
			b |= *src - 'A' + 10;
		src++;
		*dst++ = b;
	}
	return srclen/2;
}

int Bytes2String (char* dst, const char* src, int srclen)
{
	static const char tab[] = "0123456789ABCDEF";
    unsigned char b;
	int i;

	for ( i = 0; i < srclen; i++ ) {
        b = *src ++;
		*dst++ = tab[(b >> 4) & 0x0f];
		*dst++ = tab[b & 0x0f];
	}
	return srclen * 2;
}

int	UCS2Decode(char * dst, char * src, int len)
{
    String2Bytes (dst, src, len);
    ConvertUcs2 (dst, dst, len/2);

    return len/2;
}

int UCS2Encode (char * dst, char * src, int len)
{
    ConvertUcs2 (src, src, len);
    Bytes2String (dst, src, len);

    return len*2;
}

char * AOCHexEncode (unsigned long value)
{
    static char aocHex[12];
    unsigned char b[3];
    memset (aocHex, 0, sizeof(aocHex));

    b[0] = (unsigned char)((value >> 16) & 0xff);
    b[1] = (unsigned char)((value >> 8 ) & 0xff);
    b[2] = (unsigned char)(value & 0xff);
    Bytes2String (aocHex, b, 3);

    return aocHex;
}

unsigned long AOCHexDecode (char * str)
{
    unsigned long retval = 0, t = 0;
    unsigned char b[3];

    String2Bytes (b, str, 6);

    retval |= b[2];
    t = b[1];
    retval |= t << 8;
    t = b[0];
    retval |= t << 16;

    return retval;
}
typedef struct 
{
   int iErrorID;
   char * pErrorInfo;
}ME_CMEErrorInfo;

static const ME_CMEErrorInfo me_cmeerrorinfo[] =
{
    {0  ,"phone failure"},
    {1  ,"no connection to phone"},
    {2  ,"phone-adapter link reserved"},
    {3  ,"Operation not allowed"},
    {4  ,"Operation not supported"},
    {5  ,"PH-SIM PIN required"},
    {6  ,"PH-FSIM PIN required"},
    {7  ,"PH-FSIM PUK required"},
    {10	,"SIM not inserted"    },
    {11	,"SIM PIN required"    },
    {12	,"SIM PUK required"    },
    {13	,"SIM failure"         },
    {14	,"SIM busy"            },
    {15	,"SIM wrong"           },
    {16	,"Incorrect password"  },
    {17	,"SIM PIN2 required"   },
    {18	,"SIM PUK2 required"   },
    {20	,"Memory full"                                  },
    {21	,"invalid index"                                },
    {22	,"not found"                                    },
    {23	,"Memory failure"                               },
    {24	,"text strng too long"                          },
    {25	,"invalid characters in text string"            },
    {26	,"dial string too long"},
    {27	,"invalid characters in dial string"},
    {30	,"no network service"                          },
    {31	,"Network timeout"                             },
    {32	,"Network not allowed emergency calls only"    },
    {40	,"Network personalization PIN required"        },
    {41	,"Network personalization PUK required"        },
    {42	,"Network subset personalization PIN requred"  },
    {43	,"Network subset personalization PUK required" },
    {44	,"service provider personalization PIN required"},
    {45	,"service provider personalization PUK required"},
    {46	,"Corporate pe sonalization PIN required"},
    {47	,"Corporate personalizaton PUK required" },
    {48	,"Master Phone Code requred"             },
    {100,"unknown"},
    {132,"service option not supported"},
    {133,"requested service option not subscribed" },
    {134,"service option temporarily out of order" },
    {256,"Operation temporary not allowed"},
    {257,"call barred" },
    {258,"phone is busy"},
    {259,"user abort" },
    {260,"invalid dail string"},
    {261,"ss not executed"},
    {262,"SIM blocked"},
    {263,"Invad Block"},
    {-1,NULL}
};
/*********************************************************************\
* Function	   GetCMEErrorInfo
* Purpose      根据传进来的错误码,返回错误字符串
* Params	   iErrorID:错误码,pErrorInfo:返回错误字符串的缓冲区,Bufflen:缓冲区长度
* Return	   返回错误字符串的长度
* Remarks	   
**********************************************************************/
int GetCMEErrorInfo(int iErrorID,char * pErrorInfo,unsigned int Bufflen)
{
    unsigned int i = 0,errorlen;
    while (me_cmeerrorinfo[i].iErrorID != -1)
    {
        if (me_cmeerrorinfo[i].iErrorID == iErrorID)
        {
            errorlen = strlen(me_cmeerrorinfo[i].pErrorInfo);
            if (pErrorInfo != NULL)
            {
                memset(pErrorInfo,0x00,Bufflen);
                strncpy(pErrorInfo,me_cmeerrorinfo[i].pErrorInfo,
                    errorlen > Bufflen ? Bufflen - 1 : errorlen);
            }
            return (errorlen);
        }
        i++;
    }
    return -1;
}
