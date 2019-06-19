/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : Cell Broadcasting Message Decode
 *            
\**************************************************************************/

#include "window.h"
#include "me_wnd.h"
#include "string.h"
#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define CBMSG_LEN   88

static int CBS_Str2Bytes (uint8* dst, const char* src, int srclen);
static int CBS_DePacking (char *dst, char *src, int src_len);

/*********************************************************************
* Function	   CBS_Decode
* Purpose      Decode CBS Message
* Params	   
* Return	   TRUE or FALSE
* Remarks	   
**********************************************************************/
int CBS_Decode (CBS_INFO * info, char * pdu, int pdulen)
{
    uint8 data[CBMSG_LEN];
    int len, off=0, codegroup,code;

    if ( !pdu || pdulen <= 6 || pdulen > CBMSG_LEN*2)
        return FALSE;

    len = CBS_Str2Bytes (data, pdu, pdulen);

    info->SNum = data[0];
    info->SNum = (info->SNum << 8) | data[1];

    info->MId = data[2];
    info->MId = (info->MId << 8) | data[3];

    codegroup = data[4]>>4;

    if ( 0x00 == codegroup ) {
        info->Code = CBS_ALPHA_GSM;
        if ( (data[4] & 0x0f) == 0x0f )
            info->Lang = CBS_LANG_RESERVE;
        else {
            code = data[4] & 0x0f;
            switch (code) {
            case 0:
                info->Lang = CBS_LANG_GERMAN;
            	break;
            case 1:
                info->Lang = CBS_LANG_ENGLISH;
            	break;
            case 2:
                info->Lang = CBS_LANG_ITALIAN;
            	break;
            case 3:
                info->Lang = CBS_LANG_FRENCH;
            	break;
            case 4:
                info->Lang = CBS_LANG_SPANISH;
            	break;
            case 5:
                info->Lang = CBS_LANG_DUTCH;
            	break;
            case 6:
                info->Lang = CBS_LANG_SWEDISH;
            	break;
            case 7:
                info->Lang = CBS_LANG_DANISH;
            	break;
            case 8:
                info->Lang = CBS_LANG_PORTUGUESE;
            	break;
            case 9:
                info->Lang = CBS_LANG_FINNISH;
            	break;
            case 10:
                info->Lang = CBS_LANG_NORWEGIAN;
            	break;
            case 11:
                info->Lang = CBS_LANG_GREEK;
            	break;
            case 12:
                info->Lang = CBS_LANG_TURKISH;
            	break;
            case 13:
                info->Lang = CBS_LANG_HANGARIAN;
            	break;
            case 14:
                info->Lang = CBS_LANG_POLISH;
            	break;
            default:
                info->Lang = CBS_LANG_RESERVE;
                break;
            }
        }
        info->Class = CBS_NOCLASS;
    }
    else if ( 0x01 == codegroup ) {
        if ( (data[4] & 0x01) == 0 ) {
            info->Code = CBS_ALPHA_GSM;
            info->Lang = CBS_LANG_RESERVE;
        }
        else if ( (data[4] & 0x01) == 1 ) {
            info->Code = CBS_ALPHA_UCS2;
            info->Lang = CBS_LANG_RESERVE;
            off = 2;
        }
        else {
            info->Code = CBS_ALPHA_GSM;
            info->Lang = CBS_LANG_RESERVE;
        }
        info->Class = CBS_NOCLASS;
    }
    else if ( 0x02 == codegroup ) {
        if ( data[4] == 0 )
            info->Lang = CBS_LANG_CZECH;
        else
            info->Lang = CBS_LANG_RESERVE;
        info->Code = CBS_ALPHA_GSM;
        info->Class = CBS_NOCLASS;
    }
    else if ( 0x03 == codegroup ) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
        info->Class = CBS_NOCLASS;
    }
    else if ( codegroup >= 0x04 && codegroup <= 0x07 ) {
        if ( data[4] & 0x10 ) {
            if ( (data[4] & 0x03) == 0 )
                info->Class = CBS_CLASS0;
            else if ( (data[4] & 0x03) == 1 )
                info->Class = CBS_CLASS1;
            else if ( (data[4] & 0x03) == 2 )
                info->Class = CBS_CLASS2;
            else
                info->Class = CBS_CLASS3;
        }
        else
            info->Class = CBS_NOCLASS;
        if ( ((data[4] & 0x0C) >> 2) == 2 )
            info->Code = CBS_ALPHA_UCS2;
        else if ( ((data[4] & 0x0C) >> 2) == 1 )
            info->Code = CBS_ALPHA_DATA;
        else
            info->Code = CBS_ALPHA_GSM;

        info->Lang = CBS_LANG_RESERVE;
    }
    else if ( codegroup >= 0x08 && codegroup <= 0x0D) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
        info->Class = CBS_NOCLASS;
    }
    else if ( codegroup == 0x0E ) {
        info->Code = CBS_ALPHA_GSM;
        info->Lang = CBS_LANG_RESERVE;
        info->Class = CBS_NOCLASS;
    }
    else {
        if ( (data[4] & 0x03) == 0 )
            info->Class = CBS_CLASS0;
        else if ( (data[4] & 0x03) == 1 )
            info->Class = CBS_CLASS1;
        else if ( (data[4] & 0x03) == 2 )
            info->Class = CBS_CLASS2;
        else
            info->Class = CBS_CLASS3;

        if ( data[4] & 0x04 )
            info->Code = CBS_ALPHA_DATA;
        else
            info->Code = CBS_ALPHA_GSM;

        info->Lang = CBS_LANG_RESERVE;
    }

    info->PageNum = data[5] & 0x0f;
    info->PageRef = data[5] >> 4;

    if ( info->Code == CBS_ALPHA_GSM ) {
        info->DataLen = CBS_DePacking (info->Data, &data[6], len - 6);
    }
    else {
        memcpy (info->Data, &data[6+off], len - 6 - off);
        info->DataLen = len - 6 - off;
    }

    return TRUE;
}

static int CBS_Str2Bytes (uint8* dst, const char* src, int srclen)
{
    int i;
    for ( i = 0; i < srclen; i += 2 ) {
        if( *src >= '0' && *src <= '9' )
            *dst = (*src - '0') << 4;
        else
            *dst = (*src - 'A' + 10) << 4;

        src++;

        if( *src >= '0' && *src <= '9' )
            *dst |= *src - '0';
        else
            *dst |= *src - 'A' + 10;

        src++;
        dst++;
    }

    return srclen / 2;
}

static int CBS_DePacking (char *dst, char *src, int src_len)
{
	DWORD b[8], A, B;
	int i,j, len, LopI, ModI;

	len = src_len;

	LopI = len/7;

	ModI = len%7;

	if(ModI)
		LopI++;

	for( j = 0; j < LopI; j++)
	{
		A = B = 0;

		for( i = 0; i < 7; i++ )
		{
			if( ModI && j == LopI-1 && i >= ModI )
				b[i] = 0;
			else
				b[i] = (BYTE)src[7*j+i];
		}

		A = b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
		B = b[3] + (b[4]<<8) + (b[5]<<16) + (b[6]<<24);
		B = B>>4;

		dst[8*j]   = (char)(A&0x7F);
		dst[8*j+1] = (char)((A>>7 )&0x7F);
		dst[8*j+2] = (char)((A>>14)&0x7F);
		dst[8*j+3] = (char)((A>>21)&0x7F);
		dst[8*j+4] = (char)(B&0x7F);
		dst[8*j+5] = (char)((B>>7 )&0x7F);
		dst[8*j+6] = (char)((B>>14)&0x7F);
		dst[8*j+7] = (char)((B>>21)&0x7F);
	}

	return src_len/7*8;
}

