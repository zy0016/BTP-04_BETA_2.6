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
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "window.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"


struct SMS_FO
{
	BYTE MTI;
	BYTE RD;
	BYTE MMS;
	BYTE VPF;
	BYTE SRR;
	BYTE SRI;
	BYTE SRQ;
	BYTE UDHI;
	BYTE RP;
};

static int	gsm8Bytes_7Bytes (char * dst, char * src, int srclen, int offset);
static int	gsm7Bytes_8Bytes (char * dst, char * src, int srclen, int offset);
static int  FOData_Decode (BYTE src, struct SMS_FO* dst);
static BYTE	HexStr2BYTE (char *str);
static int	HexString2DWORD (char *str);
static int	Analyse_PhoneNum (char *pContext, char *pPDUStr, int type);

#define ANALYSE_CODE				0
#define ANALYSE_DECODE				1

#define ME_CODELEN_STRLEN			4


/*------------------------------------------------------
 *
 * Code Transmitted Context to PDU String
 * "0891683108100005F011000D91683146110972F70008000A4E2D658777ED6D88606F"
 *
 *----------------------------------------------------*/
int	ME_TranStr_Code (PSMS_SEND pSMS_Send, char* pDesStr, int Length)
{
	int		len, offset;
	int		SCALength, SMSLength;
	char	PhoneNum[ME_PHONENUM_LEN+8],TempPhoneNum[ME_PHONENUM_LEN+8];
	char	Context[ME_SMS_PDU_LEN],TempContext[ME_SMS_PDU_LEN];
	char	ContextLen[ME_CODELEN_STRLEN];
	char	EndCode[4];

	memset (pDesStr, 0, Length);

	// Create PDU "SCA"
	if ( pSMS_Send->SCA[0] == '+' )
	{
		strcpy( TempPhoneNum, "91" );
		offset = 1;
	}
	else
	{
		strcpy( TempPhoneNum, "81" );
		offset = 0;
	}

	Analyse_PhoneNum ((pSMS_Send->SCA)+offset, TempPhoneNum+2, ANALYSE_CODE);
	SCALength = strlen (TempPhoneNum)/2;

	sprintf (PhoneNum, "%02x", SCALength);	// SCA Length
	strcat (PhoneNum, TempPhoneNum);		// SCA PhoneNum
	strcpy (pDesStr, PhoneNum);


	// SMS Param
	if ( !pSMS_Send->ReplyPath )
	{
		if ( pSMS_Send->Report )				//Status Report
		{
			if ( pSMS_Send->udhl > 0 )
				strcat( pDesStr, "7100" );
			else
				strcat( pDesStr, "3100");
		}
		else
		{
			if ( pSMS_Send->udhl > 0 )
				strcat( pDesStr, "5100" );
			else
				strcat( pDesStr, "1100" );
		}
	}
	else
	{
		if ( pSMS_Send->Report )				//Status Report
		{
			if ( pSMS_Send->udhl > 0 )
				strcat( pDesStr, "F100" );
			else
				strcat( pDesStr, "B100" );
		}
		else
		{
			if ( pSMS_Send->udhl > 0 )
				strcat( pDesStr, "D100" );
			else
				strcat( pDesStr, "9100" );
		}
	}

	// PhoneNum
	if( pSMS_Send->PhoneNum[0] == '+' )
	{
		strcpy( TempPhoneNum, "91" );
		offset = 1;
	}
	else
	{
		strcpy( TempPhoneNum, "81" );
		offset = 0;
	}

	Analyse_PhoneNum( (pSMS_Send->PhoneNum)+offset, TempPhoneNum+2, ANALYSE_CODE );
	len = strlen( (pSMS_Send->PhoneNum)+offset );

	sprintf( PhoneNum, "%02x", len );	// PhoneNum Length
	strcat( PhoneNum, TempPhoneNum );	// PhoneNum PhoneNum
	strcat( pDesStr, PhoneNum );

	// TP-PID
	switch ( pSMS_Send->PID )	//PID
	{
	case PID_TELEX:
		strcat( pDesStr, "21" );
		break;
	case PID_TELEFAX:
		strcat( pDesStr, "22" );
		break;
	case PID_VOICE:
		strcat( pDesStr, "24" );
		break;
	case PID_ERMES:
		strcat( pDesStr, "25" );
		break;
	case PID_NPAGING:
		strcat( pDesStr, "26" );
		break;
	case PID_X400:
		strcat( pDesStr, "31" );
		break;
	case PID_EMAIL:
		strcat( pDesStr, "32" );
		break;
	default:
		strcat( pDesStr, "00" );
		break;
	}


    // TP-DCS
	switch (pSMS_Send->dcs)
	{
	case DCS_GSM:
		strcat (pDesStr, "00");
		break;
	case DCS_8BIT:
		strcat (pDesStr, "04");
		break;
	case DCS_UCS2:
		strcat (pDesStr, "08");
		break;
	default:
		strcat (pDesStr, "0C");
		break;
	}

    // TP-VP
	switch ( pSMS_Send->VPvalue )
	{
	case SMS_1HOUR:
        strcat (pDesStr, "0B");
		break;

	case SMS_6HOUR:
        strcat (pDesStr, "47");
		break;

	case SMS_24HOUR:
        strcat (pDesStr, "A7");
		break;

    case SMS_72HOUR:
        strcat(pDesStr,"A9");
        break;

	case SMS_1WEEK:
        strcat (pDesStr, "AD");
		break;

	case SMS_MAXTIME:
        strcat (pDesStr, "FF");
		break;

	default:
		return -1;
	}

	// TP-UDL
	len = 0;
	if ( pSMS_Send->udhl > 0 )
	{
		// we must calculate TP-UDHL itself
		if ( DCS_GSM != pSMS_Send->dcs )
		{
			len += pSMS_Send->udhl + pSMS_Send->ConLen + 1;
		}
		else
		{
			if ( (pSMS_Send->udhl+1) % 7 == 0 )
				len += (pSMS_Send->udhl+1)*8/7;
			else
				len += (pSMS_Send->udhl+1)*8/7+1;

			len += pSMS_Send->ConLen;
		}
	}
	else
	{
		len += pSMS_Send->ConLen;
	}

	sprintf( ContextLen, "%02x", len );
	strcat( pDesStr, ContextLen );					// Context Length

	memset( TempContext, 0, ME_SMS_PDU_LEN );
	memset( Context, 0, ME_SMS_PDU_LEN );

	/* TP-UDHL */
	if ( pSMS_Send->udhl > 0 )
	{
		sprintf( ContextLen, "%02x", pSMS_Send->udhl );
		strcat( pDesStr, ContextLen );
        Bytes2String (TempContext, pSMS_Send->UDH, pSMS_Send->udhl);
		strcat( pDesStr, TempContext);
	}

	memset( TempContext, 0, ME_SMS_PDU_LEN );

	/* TP-UD */
	switch ( pSMS_Send->dcs ) {
	case DCS_GSM:
		if ( pSMS_Send->udhl == 0 || (pSMS_Send->udhl+1)%7 == 0 )
			gsm8Bytes_7Bytes (TempContext, pSMS_Send->Context, pSMS_Send->ConLen, 0 );
		else
			gsm8Bytes_7Bytes (TempContext, pSMS_Send->Context, pSMS_Send->ConLen,(7-(((pSMS_Send->udhl+1)*8)%7)));

		if ( pSMS_Send->ConLen % 8 == 0 )
			len = pSMS_Send->ConLen*7/8;
		else
			len = pSMS_Send->ConLen*7/8+1;

		Bytes2String (Context, TempContext, len);

		break;

	case DCS_8BIT:

        Bytes2String (Context, pSMS_Send->Context, pSMS_Send->ConLen);

		break;

	case DCS_UCS2:

        UCS2Encode (Context, pSMS_Send->Context, pSMS_Send->ConLen);

		break;

	}
	strcat( pDesStr, Context );						// Context

	SMSLength = strlen( pDesStr )/2 - SCALength-1;	// Calculate SMS Length

	EndCode[0]=0x1A;	EndCode[1]=0;
	strcat( pDesStr,EndCode );						// Send Command End_Code

	strupr( pDesStr );

	return SMSLength;
}

/*------------------------------------------------------
 *
 * PDU String to Code Transmitted Context
 * +CMGR: 1,,38
 * "0891683108100005F0040D91683108317090F20000100180512510231441E19058341E9149E592D9743EA151E9940A"
 * "0891683108100005F04406A177848800F530202151016023550B05040B8423F000037102028A808E0400003D9D880681043E4D086983687474703A2F2F3231382E3230302E3234342E3139372F7761732F3032313231353136353739313030303030373030303030303330313400"
 *----------------------------------------------------*/
int	ME_RecStr_Code( PSMS_INFO pDesSMS_Read, char* pPDUStr, int stat )
{
	int		Len, Offset;
	int		PDU_Offset;

	char	PhoneLen[ME_CODELEN_STRLEN], PhoneNum[ME_PHONENUM_LEN];
	char	SendTime[ME_SMS_TIME_LEN];
	char	ContextLen[ME_CODELEN_STRLEN];
	char	Context[ME_SMS_PDU_LEN],TempContext[ME_SMS_PDU_LEN];
	struct SMS_FO fo;
	BYTE FoData;

	if( strlen( pPDUStr)== 0 )
		return -1;

	memset(SendTime, 0, ME_SMS_TIME_LEN);
	memset(Context, 0, ME_SMS_PDU_LEN);
	memset(TempContext, 0, ME_SMS_PDU_LEN);

	memcpy( PhoneLen, pPDUStr, 2 );				// SCA Number
	PhoneLen[2] = 0;
	Len = HexString2DWORD( PhoneLen )*2;

	PDU_Offset = 2;

	if ( Len > 0 )
	{
		if( memcmp( pPDUStr+PDU_Offset, "91", 2 ) ==0 )
		{
			strcpy( pDesSMS_Read->SCA, "+" );
			Offset = 1;
		}
		else
			Offset = 0;

		PDU_Offset += 2;
		Len -= 2;

		if ( Len > 0 )
		{
			memcpy (PhoneNum, pPDUStr+PDU_Offset, Len);
			PhoneNum[Len]=0;

			Analyse_PhoneNum (pDesSMS_Read->SCA+Offset, PhoneNum, ANALYSE_DECODE);
			PDU_Offset += Len;
		}
	}

	memcpy (PhoneNum, pPDUStr+PDU_Offset, 2);
	PhoneNum[2] = 0;
	FoData = HexStr2BYTE (PhoneNum);

	FOData_Decode( FoData, &fo );

	if ( stat == SMS_UNSEND || stat == SMS_SENDED )
	{
		if ( fo.MTI != 1 )
			return 0;
		
		pDesSMS_Read->Type = SMS_SUBMIT;

		PDU_Offset += 4;

		memcpy( PhoneLen, pPDUStr+PDU_Offset, 2 );	// Sender Number
		PhoneLen[2] = 0;
		Len = HexString2DWORD( PhoneLen );
		if( Len%2 != 0 )
			Len+=1;

		PDU_Offset += 2;
		if( memcmp( pPDUStr+PDU_Offset, "91", 2 ) ==0 )
		{
			strcpy( pDesSMS_Read->SenderNum, "+" );
			Offset =1;
		}
		else
			Offset =0;

		PDU_Offset +=2;

		if ( Len > 0 )
		{
			memcpy( PhoneNum, pPDUStr+PDU_Offset, Len );
			PhoneNum[Len] = 0;
			Analyse_PhoneNum( pDesSMS_Read->SenderNum +Offset, PhoneNum, ANALYSE_DECODE );
			PDU_Offset += Len;
		}

		PDU_Offset += 2;	// PID

		if ( pPDUStr[PDU_Offset+1] >='0' && pPDUStr[PDU_Offset+1] <='3' )
			pDesSMS_Read->dcs = DCS_GSM;				// GSM 7 bits
		else if ( pPDUStr[PDU_Offset+1] >='4' && pPDUStr[PDU_Offset+1] <='7' )
			pDesSMS_Read->dcs = DCS_8BIT;				// Ascii 8 bits
		else if ( pPDUStr[PDU_Offset+1] >='8' && pPDUStr[PDU_Offset+1] <='B')
			pDesSMS_Read->dcs = DCS_UCS2;				// UCS2 16 bits

		if ( pPDUStr[PDU_Offset] == 'F' || pPDUStr[PDU_Offset] == '1' || pPDUStr[PDU_Offset] == '3')
		{
			char dcs = pPDUStr[PDU_Offset+1];
			switch (dcs)
			{
			case '0':
			case '4':
			case '8':
			case 'C':
				pDesSMS_Read->Class = SMS_CLASS_0;
				break;
			case '1':
			case '5':
			case '9':
			case 'D':
				pDesSMS_Read->Class = SMS_CLASS_1;
				break;
			case '2':
			case '6':
			case 'A':
			case 'E':
				pDesSMS_Read->Class = SMS_CLASS_2;
				break;
			case '3':
			case '7':
			case 'B':
			case 'F':
				pDesSMS_Read->Class = SMS_CLASS_3;
				break;
			}
		}
		else
			pDesSMS_Read->Class = SMS_CLASS_NONE;

		PDU_Offset += 2;

		FoData &= 0x18;

		if ( FoData == 0 )
			PDU_Offset = PDU_Offset;
		else if ( FoData == 0x10 )
			PDU_Offset += 2;
		else
			PDU_Offset += 14;
		
		memcpy( ContextLen, pPDUStr+PDU_Offset, 2 );		// Context length
		ContextLen[2]=0;
		Len = HexString2DWORD( ContextLen );
		pDesSMS_Read->ConLen = Len;
		Len *= 2;

		if( Len > ME_SMS_PDU_LEN )
			Len = ME_SMS_PDU_LEN;

		if ( Len == 0 )
		{
			memset( pDesSMS_Read->Context, 0, ME_SMS_CONTEXT_LEN );
			return Len;
		}
		PDU_Offset += 2;

		/* TP-UDH */
		pDesSMS_Read->udhl = 0;

		if ( fo.UDHI )
		{
			memcpy( ContextLen, pPDUStr+PDU_Offset, 2 );		// Context length
			ContextLen[2]=0;
			pDesSMS_Read->udhl = HexString2DWORD( ContextLen );
			PDU_Offset += 2;

			memcpy( Context, pPDUStr+PDU_Offset, pDesSMS_Read->udhl*2 );
			Context[pDesSMS_Read->udhl*2] = '\0';
            String2Bytes (pDesSMS_Read->UDH, Context, strlen(Context));
			PDU_Offset += pDesSMS_Read->udhl*2;
		}

		strcpy( Context, pPDUStr+PDU_Offset );				// Context
		/* TP-UD */
		if( pDesSMS_Read->dcs == DCS_UCS2 )
		{
            UCS2Decode (pDesSMS_Read->Context, Context, strlen(Context));
			if ( pDesSMS_Read->udhl > 0 )
				pDesSMS_Read->ConLen -= pDesSMS_Read->udhl+1;
		}
		if (pDesSMS_Read->dcs == DCS_8BIT)
		{
            String2Bytes (pDesSMS_Read->Context, Context, strlen(Context));
			if ( pDesSMS_Read->udhl > 0 )
				pDesSMS_Read->ConLen -= pDesSMS_Read->udhl+1;
		}
		if( pDesSMS_Read->dcs == DCS_GSM )
		{
			int srclen;
            srclen = String2Bytes (TempContext, Context, strlen(Context));
			if ( pDesSMS_Read->udhl == 0 || (pDesSMS_Read->udhl+1)%7 == 0 )
				gsm7Bytes_8Bytes (pDesSMS_Read->Context, TempContext, srclen, 0);
			else
				gsm7Bytes_8Bytes (pDesSMS_Read->Context, TempContext, srclen, 7-(((pDesSMS_Read->udhl+1)*8)%7));

			if ( pDesSMS_Read->udhl > 0 )
			{
				Len = ((pDesSMS_Read->udhl+1)%7 == 0) ? (pDesSMS_Read->udhl+1)*8/7 : (pDesSMS_Read->udhl+1)*8/7+1;
				pDesSMS_Read->ConLen -= Len;
			}
		}
		return 0;
	}

	if ( fo.MTI == 2 )
		pDesSMS_Read->Type = STATUS_REPORT;
	else if ( fo.MTI == 1 )
		pDesSMS_Read->Type = SUBMIT_REPORT;
	else
		pDesSMS_Read->Type = SMS_DELIVER;

	PDU_Offset += 2;							// some settings

	if ( pDesSMS_Read->Type == STATUS_REPORT )
    {
        char cMR[3] = "";
        memcpy(cMR,pPDUStr + PDU_Offset,2);
        pDesSMS_Read->MR = HexStringToInt(cMR);
        PDU_Offset += 2;						//MR
    }
	memcpy( PhoneLen, pPDUStr+PDU_Offset, 2 );	// Sender Number
	PhoneLen[2] = 0;
	Len = HexString2DWORD( PhoneLen );
	if( Len%2 != 0 )
		Len+=1;

	PDU_Offset += 2;
	if( memcmp( pPDUStr+PDU_Offset, "91", 2 ) ==0 )
	{
		strcpy( pDesSMS_Read->SenderNum, "+" );
		Offset =1;
	}
	else
		Offset =0;

	PDU_Offset +=2;

	if ( Len > 0 )
	{
		memcpy( PhoneNum, pPDUStr+PDU_Offset, Len );
		PhoneNum[Len] = 0;
		Analyse_PhoneNum( pDesSMS_Read->SenderNum +Offset, PhoneNum, ANALYSE_DECODE );
		PDU_Offset += Len;
	}

	if ( pDesSMS_Read->Type == STATUS_REPORT )
	{
		PDU_Offset += 14;

		memcpy( SendTime, pPDUStr+PDU_Offset, 14 );			// Send Time
		SendTime[14] = 0;
		Analyse_PhoneNum( pDesSMS_Read->SendTime, SendTime, ANALYSE_DECODE );

		PDU_Offset += 14;

		memcpy(PhoneNum, pPDUStr+PDU_Offset, 2);

		PhoneNum[2] = 0;

		pDesSMS_Read->Status = (int)HexStr2BYTE( PhoneNum );

		return 0;
	}

	PDU_Offset += 2;		// PID

							//DCS	
	if ( pPDUStr[PDU_Offset+1] >='0' && pPDUStr[PDU_Offset+1] <='3' )
		pDesSMS_Read->dcs = DCS_GSM;				// GSM 7 bits
	else if ( pPDUStr[PDU_Offset+1] >='4' && pPDUStr[PDU_Offset+1] <='7' )
		pDesSMS_Read->dcs = DCS_8BIT;				// Ascii 8 bits
	else if ( pPDUStr[PDU_Offset+1] >='8' && pPDUStr[PDU_Offset+1] <='B')
		pDesSMS_Read->dcs = DCS_UCS2;				// UCS2 16 bits



	if ( pPDUStr[PDU_Offset] == 'F' || pPDUStr[PDU_Offset] == '1' || pPDUStr[PDU_Offset] == '3')
	{
		char dcs = pPDUStr[PDU_Offset+1];
		switch (dcs)
		{
		case '0':
		case '4':
		case '8':
		case 'C':
			pDesSMS_Read->Class = SMS_CLASS_0;
			break;
		case '1':
		case '5':
		case '9':
		case 'D':
			pDesSMS_Read->Class = SMS_CLASS_1;
			break;
		case '2':
		case '6':
		case 'A':
		case 'E':
			pDesSMS_Read->Class = SMS_CLASS_2;
			break;
		case '3':
		case '7':
		case 'B':
		case 'F':
			pDesSMS_Read->Class = SMS_CLASS_3;
			break;
		}
	}
	else
		pDesSMS_Read->Class = SMS_CLASS_NONE;
			
	PDU_Offset += 2;

	memcpy( SendTime, pPDUStr+PDU_Offset, 14 );			// Send Time
	SendTime[14] = 0;
	Analyse_PhoneNum( pDesSMS_Read->SendTime, SendTime, ANALYSE_DECODE );
	PDU_Offset += 14;

	memcpy( ContextLen, pPDUStr+PDU_Offset, 2 );		// Context length
	ContextLen[2]=0;
	Len = HexString2DWORD( ContextLen );
	pDesSMS_Read->ConLen = Len;
	Len *= 2;

	if( Len > ME_SMS_PDU_LEN )
		Len = ME_SMS_PDU_LEN;

	if ( Len == 0 )
	{
		memset( pDesSMS_Read->Context, 0, ME_SMS_CONTEXT_LEN );
		return Len;
	}
	PDU_Offset += 2;

	/* TP-UDH */
	pDesSMS_Read->udhl = 0;

	if ( fo.UDHI )
	{
		memcpy( ContextLen, pPDUStr+PDU_Offset, 2 );		// Context length
		ContextLen[2]=0;
		pDesSMS_Read->udhl = HexString2DWORD( ContextLen );
		PDU_Offset += 2;

		memcpy( Context, pPDUStr+PDU_Offset, pDesSMS_Read->udhl*2 );
		Context[pDesSMS_Read->udhl*2] = '\0';
        String2Bytes (pDesSMS_Read->UDH, Context, strlen(Context));
		PDU_Offset += pDesSMS_Read->udhl*2;
	}

	strcpy( Context, pPDUStr+PDU_Offset );				// Context
	/* TP-UD */
	if( pDesSMS_Read->dcs == DCS_UCS2 )
	{
		UCS2Decode (pDesSMS_Read->Context, Context, strlen(Context));
		if ( pDesSMS_Read->udhl > 0 )
			pDesSMS_Read->ConLen -= pDesSMS_Read->udhl+1;
	}
	if (pDesSMS_Read->dcs == DCS_8BIT)
	{
        String2Bytes (pDesSMS_Read->Context, Context, strlen(Context));
		if ( pDesSMS_Read->udhl > 0 )
			pDesSMS_Read->ConLen -= pDesSMS_Read->udhl+1;
	}
	if( pDesSMS_Read->dcs == DCS_GSM )
	{
		int srclen;
        srclen = String2Bytes (TempContext, Context, strlen(Context));
		if ( pDesSMS_Read->udhl == 0 || (pDesSMS_Read->udhl+1)%7 == 0 )
			gsm7Bytes_8Bytes (pDesSMS_Read->Context, TempContext, srclen, 0);
		else
			gsm7Bytes_8Bytes (pDesSMS_Read->Context, TempContext, srclen, 7-((pDesSMS_Read->udhl+1)*8%7));

		if ( pDesSMS_Read->udhl > 0 )
		{
			Len = ((pDesSMS_Read->udhl+1)%7 == 0) ? (pDesSMS_Read->udhl+1)*8/7 : (pDesSMS_Read->udhl+1)*8/7+1;
			pDesSMS_Read->ConLen -= Len;
		}
	}

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int gsm8Bytes_7Bytes (char * dst, char * src, int srclen, int offset)
{
	DWORD b[8], A, B;
	int i,j, LopI, ModI;
	unsigned char left = 0;

	LopI = srclen/8;
	ModI = srclen%8;

	if(ModI)
		LopI++;
	
	for ( j = 0; j < LopI; j++ )
	{
		A=B=0;
		for(i=0;i<8;i++)
		{
			if(ModI && j==LopI-1 && i>= ModI)
				b[i]=0;
			else
				b[i]=(char)src[8*j+i];
		}

		if ( offset < 4 )
			A = ((b[0]<<offset)|left) + (b[1]<<(7+offset)) + (b[2]<<(14+offset)) + (b[3]<<(21+offset)) + (b[4]<<(28+offset));
		else 
			A = ((b[0]<<offset)|left) + (b[1]<<(7+offset)) + (b[2]<<(14+offset)) + (b[3]<<(21+offset));

		left = (unsigned char)b[3]>>(7-offset);

		B = ((b[4]<<offset)|left) + (b[5]<<(7+offset)) + (b[6]<<(14+offset)) + (b[7]<<(21+offset));
		B = B>>4;

		left = (unsigned char)b[7]>>(7-offset);

		dst[7*j] = (char)A;
		dst[7*j+1] = (char)(A>>8);
		dst[7*j+2] = (char)(A>>16);
		dst[7*j+3] = (char)(A>>24);
		dst[7*j+4] = (char)B;
		dst[7*j+5] = (char)(B>>8);
		dst[7*j+6] = (char)(B>>16);
	}


	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BYTE HexStr2BYTE(char *str)
{
	BYTE	b1='0', ch;
	BYTE	code;
	
	ch = str[0];
		
	if(ch>='A' && ch<='F')
	{
		b1=ch-'A'+10;
	}
	if(ch>='a' && ch<='f')
	{
		b1=ch-'a'+10;
	}
	if(ch>='0' && ch<='9')
	{
		b1=ch-'0';
	}

	code = (b1<<4);

	ch = str[1];
		
	if(ch>='A' && ch<='F')
	{
		b1=ch-'A'+10;
	}
	if(ch>='a' && ch<='f')
	{
		b1=ch-'a'+10;
	}
	if(ch>='0' && ch<='9')
	{
		b1=ch-'0';
	}
	code += b1;

	return code;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int gsm7Bytes_8Bytes(char * dst, char * src, int srclen, int offset )
{
	DWORD b[8], A, B;
	int i,j, LopI, ModI;
	unsigned char next = 0;

	LopI= srclen/7;
	ModI= srclen%7;

	if(ModI)
		LopI++;

	for (j = 0; j < LopI; j++)
	{
		A=B=0;
		for(i=0;i<7;i++)
		{
			if(ModI && j==LopI-1 && i>= ModI)
				b[i] = 0;
			else
				b[i] = (unsigned char)src[7*j+i];
		}

		A = b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
		B = b[3] + (b[4]<<8) + (b[5]<<16) + (b[6]<<24);
		B = B>>4;

		dst[8*j]   = (char)((A>>offset)&0x7F);
		dst[8*j+1] = (char)((A>>(7+offset) )&0x7F);
		dst[8*j+2] = (char)((A>>(14+offset))&0x7F);
		dst[8*j+3] = (char)((A>>(21+offset))&0x7F);
		if ( offset > 4 )
		{
			next = (unsigned char)src[7*j+4];
			dst[8*j+3] |= ((unsigned char)(next<<(7-offset+4)))&0x7F;
		}
		dst[8*j+4] = (char)((B>>offset)&0x7F);
		dst[8*j+5] = (char)((B>>(7+offset) )&0x7F);
		dst[8*j+6] = (char)((B>>(14+offset))&0x7F);
		dst[8*j+7] = (char)((B>>(21+offset))&0x7F);
		next = (unsigned char)src[7*(j+1)];
		dst[8*j+7] |= ((unsigned char)(next<<(7-offset)))&0x7F;
	}

	return 0;
}


/*--------------------------------------------------------------------
 *
 * Usage: decode a length value: char *str("0A")-> return int=10
 * rule: str lenght =2
 * example:telephone length
 *
 *-----------------------------------------------------------------*/

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int HexString2DWORD(char *str)
{
	BYTE	b1='0', ch;
	int		i;
	int		ret,code,len;
	
	ret = 0;

	len = strlen(str);

	if(len>2)
		return 0;

	i=len-1;
	while(i>=0)
	{
		ch = str[i];
		if(ch=='x'||ch=='X')  // no use in this condition, just ensure
			break;

		if(ch>='A' && ch<='F')
		{
			b1=ch-'A'+10;
		}
		if(ch>='a' && ch<='f')
		{
			b1=ch-'a'+10;
		}
		if(ch>='0' && ch<='9')
		{
			b1=ch-'0';
		}
		code = b1;
		ret+=code<<  ((len-1-i)*4);
		i--;
	}
	return ret;
	
}


/*--------------------------------------------------------
 *
 * Analyse telephone number decode/code
 *
 *------------------------------------------------------*/

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	Analyse_PhoneNum( char *pContext, char *pPDUStr, int type )
{
	int i;
	switch( type )
	{
	case ANALYSE_CODE:		//pContext->pPDUStr
		for( i=0; ;i+=2 )
		{
			if( pContext[i] == 0 )
			{
				pPDUStr[i]=0;
				break;
			}
			if ( pContext[i] == '*' )
				pPDUStr[i+1] = 'A';
			else if ( pContext[i] == '#' )
				pPDUStr[i+1] = 'B';
			else if ( pContext[i] == 'a' )
				pPDUStr[i+1] = 'C';
			else if ( pContext[i] == 'b' )
				pPDUStr[i+1] = 'D';
			else if ( pContext[i] == 'c' )
				pPDUStr[i+1] = 'E';
			else
				pPDUStr[i+1] = pContext[i];

			// when strlen == odd len
			if( pContext[i+1] == 0 )
			{
				pPDUStr[i] = 'F';
				pPDUStr[i+2] =	0;
				break;
			}
			if ( pContext[i+1] == '*' )
				pPDUStr[i] = 'A';
			else if ( pContext[i+1] == '#' )
				pPDUStr[i] = 'B';
			else if ( pContext[i+1] == 'a' )
				pPDUStr[i] = 'C';
			else if ( pContext[i+1] == 'b' )
				pPDUStr[i] = 'D';
			else if ( pContext[i+1] == 'c' )
				pPDUStr[i] = 'E';
			else
				pPDUStr[i] = pContext[i+1];
		}
		break;

	case ANALYSE_DECODE:
		for( i=0; ;i+=2 )
		{
			if( pPDUStr[i] == 0 )
			{
				pContext[i] = 0;
				if( pContext[i-1]=='F')
					pContext[i-1] = 0;
				break;
			}
			if ( pPDUStr[i+1] == 'A' )
				pContext[i] = '*';
			else if ( pPDUStr[i+1] == 'B' )
				pContext[i] = '#';
			else if ( pPDUStr[i+1] == 'C' )
				pContext[i] = 'a';
			else if ( pPDUStr[i+1] == 'D' )
				pContext[i] = 'b';
			else if ( pPDUStr[i+1] == 'E' )
				pContext[i] = 'c';
			else
				pContext[i] = pPDUStr[i+1];

			if ( pPDUStr[i] == 'A' )
				pContext[i+1] = '*';
			else if ( pPDUStr[i] == 'B' )
				pContext[i+1] = '#';
			else if ( pPDUStr[i] == 'C' )
				pContext[i+1] = 'a';
			else if ( pPDUStr[i] == 'D' )
				pContext[i+1] = 'b';
			else if ( pPDUStr[i] == 'E' )
				pContext[i+1] = 'c';
			else
				pContext[i+1] = pPDUStr[i];
		}
		break;
	}
	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return 
* Remarks	   
**********************************************************************/
static int FOData_Decode( BYTE src, struct SMS_FO* dst )
{
	int ret;

	memset( dst, 0, sizeof(struct SMS_FO) );

	dst->MTI = src&0x03;//00000011

	if ( dst->MTI == 2 )
	{
		ret = STATUS_REPORT;
		dst->SRQ = src&0x20;//00100000
		dst->MMS = src&0x04;//00000100
		dst->UDHI = src&0x40;//01000000
	}
	else if ( dst->MTI == 1 )
	{
		ret = SUBMIT_REPORT;
		dst->UDHI = src&0x40;//01000000
	}
	else
	{
		ret = SMS_DELIVER;
		dst->MMS = src&0x04;//00000100
		dst->SRI = src&0x20;//00100000
		dst->UDHI = src&0x40;//01000000
		dst->RP = src&0x80;//10000000
	}

	return ret;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_RecStr_Flag( PSMS_INFO pSMSRead, char* SMS_Prompt )
{
	// +CMGR: 1,,22
	int tag;

	if( strlen(SMS_Prompt)== 0 )
		return -1;

	if ( strstr( SMS_Prompt, "+CMGR:" ) != NULL )
		tag = 0;
	else if ( strstr( SMS_Prompt, "^SMGR:" ) != NULL )
		tag = 1;
	else
		return -1;

	if ( tag == 0 )
		pSMSRead->Stat = ME_GetIntSection( SMS_Prompt, "+CMGR:", 1 );
	else
		pSMSRead->Stat = ME_GetIntSection( SMS_Prompt, "^SMGR:", 1 );

	return pSMSRead->Stat;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int ME_RecLstStr_Code( PSMS_INFO pSMSInfo, char* pSmsContext, int Count )
{
	int	 i, tag;
	char *begin, *end;

	if( strlen(pSmsContext)== 0 )
		return -1;

	if ( strstr( pSmsContext, "+CMGL:" ) != NULL )
		tag = 0;
	else if ( strstr( pSmsContext, "^SMGL:" ) != NULL )
		tag = 1;
	else
		return 0;

	end = (char*)pSmsContext-2;

	for ( i=0; i < Count; i++ )
	{
		begin = end + 2;

		while ( *begin == '\r')
			begin += 2;

		if ( strlen( begin ) == 0 )
			return i;

		end = strchr( begin, '\r' );

		if ( end == NULL )
			return i;

		*end = '\0';

		if ( tag == 0 )
		{
			pSMSInfo[i].Index = ME_GetIntSection( begin, "+CMGL:", 1);
			pSMSInfo[i].Stat = ME_GetIntSection( begin, "+CMGL:", 2);
		}
		else
		{
			pSMSInfo[i].Index = ME_GetIntSection( begin, "^SMGL:", 1);
			pSMSInfo[i].Stat = ME_GetIntSection( begin, "^SMGL:", 2);
		}

		if ( pSMSInfo[i].Index == -1 || pSMSInfo[i].Stat == -1 )
			return i;

		begin = end + 2;

		while ( *begin == '\r')
			begin += 2;

		if ( (strstr( begin, "+CMGL:" ) == begin && tag == 0)
			|| (strstr( begin, "^SMGL:" ) == begin && tag == 1))
		{
			end = begin-2;
			continue;
		}

		if ( strlen( begin ) == 0 )
			return i;

		end = strchr( begin, '\r' );

		if ( end == NULL )
			return i;

		*end = '\0';

		ME_RecStr_Code( &pSMSInfo[i], begin, pSMSInfo[i].Stat );
	}

	return i;
}
int HexStringToInt(const char *pString)//将2位16进制字符串转换成int型变量.
{
    char cString[3] = "";
    char * p = cString;
    int iTenDigit,iSingDigit;

    if (strlen(pString) > 2)
        return -1;

    strncpy(cString,pString,2);
    strupr(cString);

    if (('0' <= *p) && (*p <= '9'))
        iTenDigit = *p - '0';
    else if (('A' <= *p) && (*p <= 'F'))
        iTenDigit = *p - 'A' + 10;
    else
        return -1;

    p++;
    if (*p == '\0')
        return iTenDigit * 16;

    if (('0' <= *p) && (*p <= '9'))
        iSingDigit = *p - '0';
    else if (('A' <= *p) && (*p <= 'F'))
        iSingDigit = *p - 'A' + 10;
    else
        return -1;

    return iTenDigit * 16 + iSingDigit;
}
//+CRSM: 144,0,FFFFFFFFFFFFFFFFFFFFFFFFFFE1FFFFFFFFFFFFFFFFFFFFFFFF089168310810000
//5F01F0E0020F0A7
int AnalyseCPHSRecordSMS(SMS_CPHS_INFO * psms_chps_info,char * str)
{
    char * pCphs = str;
    char * p,cExcursion[30] = "",cSCA[ME_PHONENUM_LEN + 1] = "";
    unsigned int iSCALen;
    int iIndex,i;
    char dcs;

    memset(psms_chps_info,0x00,sizeof(SMS_CPHS_INFO));

    p = strstr(pCphs,",");
    if (p == NULL)
        return -1;
    pCphs = ++p;
    p = strstr(pCphs,",");
    if (p == NULL)
        return -1;
    pCphs = ++p;
    //pCphs 指向crsm字段的response部分,第二个逗号后边的部分
    strncpy(cExcursion,pCphs + 26,2);
    psms_chps_info->ParameterIndicators = HexStringToInt(cExcursion);

    pCphs += 28;
    for (i = 0;i < 24;i++)//记录DestinationAddress
    {
        if (*(pCphs + i) != 'F')
            strncat(psms_chps_info->DestinationAddress,pCphs + i,1);
        else
            break;
    }

    pCphs += 24;
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);//08
    iSCALen = HexStringToInt(cExcursion) * 2;//短信中心号码长度
    pCphs += 2;
    if (iSCALen > 0)
    {
        if (strstr(pCphs,"91") != NULL)
        {
            strcpy(psms_chps_info->SCA,"+");
            iIndex = 1;
            pCphs += 2;
        }
        else
        {
            iIndex = 0;
        }
        iSCALen -= 2;
        if (iSCALen > 0)
        {
            strncpy(cSCA,pCphs,iSCALen);
            Analyse_PhoneNum(psms_chps_info->SCA + iIndex,cSCA,ANALYSE_DECODE);
        }
    }

    if (iIndex == 1)
        pCphs += 20;
    else
        pCphs += 22;

    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    if (strcmp(cExcursion,"21") == 0)
        psms_chps_info->ProtocolIdentifier = PID_TELEX;
    else if (strcmp(cExcursion,"22") == 0)
        psms_chps_info->ProtocolIdentifier = PID_TELEFAX;
    else if (strcmp(cExcursion,"24") == 0)
        psms_chps_info->ProtocolIdentifier = PID_VOICE;
    else if (strcmp(cExcursion,"25") == 0)
        psms_chps_info->ProtocolIdentifier = PID_ERMES;
    else if (strcmp(cExcursion,"26") == 0)
        psms_chps_info->ProtocolIdentifier = PID_NPAGING;
    else if (strcmp(cExcursion,"31") == 0)
        psms_chps_info->ProtocolIdentifier = PID_X400;
    else if (strcmp(cExcursion,"32") == 0)
        psms_chps_info->ProtocolIdentifier = PID_EMAIL;
    else
        psms_chps_info->ProtocolIdentifier = PID_IMPLICIT;

    pCphs += 2;
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    dcs = cExcursion[1];
    switch (dcs)
    {
    case '0':
    case '4':
    case '8':
    case 'C':
        psms_chps_info->DataCodingScheme = SMS_CLASS_0;
        break;
    case '1':
    case '5':
    case '9':
    case 'D':
        psms_chps_info->DataCodingScheme = SMS_CLASS_1;
        break;
    case '2':
    case '6':
    case 'A':
    case 'E':
        psms_chps_info->DataCodingScheme = SMS_CLASS_2;
        break;
    case '3':
    case '7':
    case 'B':
    case 'F':
        psms_chps_info->DataCodingScheme = SMS_CLASS_3;
        break;
    }

    pCphs += 2;
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    if (strcmp(cExcursion,"0B") == 0)
        psms_chps_info->ValidityPeriod = SMS_1HOUR;
    else if (strcmp(cExcursion,"47") == 0)
        psms_chps_info->ValidityPeriod = SMS_6HOUR;
    else if (strcmp(cExcursion,"A7") == 0)
        psms_chps_info->ValidityPeriod = SMS_24HOUR;
    else if (strcmp(cExcursion,"A9") == 0)
        psms_chps_info->ValidityPeriod = SMS_72HOUR;
    else if (strcmp(cExcursion,"AD") == 0)
        psms_chps_info->ValidityPeriod = SMS_1WEEK;
    else if (strcmp(cExcursion,"FF") == 0)
        psms_chps_info->ValidityPeriod = SMS_MAXTIME;
    else
        psms_chps_info->ValidityPeriod = -1;

    return sizeof(SMS_CPHS_INFO);
}
