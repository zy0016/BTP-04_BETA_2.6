/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : smscount.c
 *
 * Purpose  : Counts the number of characters in SMS string.
 *            
\**************************************************************************/

#include "pubapp.h"
#include "assert.h"
#include "malloc.h"
#include "string.h"
//#include "hp_diag.h"

/**********************************************************************
* Function     CountSmsChar
* Purpose      Counts the number of characters in SMS string.
* Params       pszText
*                [in] SMS text, it is a null-terminated string.
*              puCount
*                [in/out] Pointer to an UINT variable that receives the
*                result of counting.
* Return       SMS sending mode. The following values are defined.
*              Value                 Meaning
*              SMS_MODE_GSM          Send by coding in GSM.
*              SMS_MODE_UNICODE      Send by coding in Unicode.
* Remarks      Returns -1 if not successfully.
**********************************************************************/

int CountSmsChar(PCSTR pszText, PUINT puCount)
{
    PSTR pchThis = NULL;
    UINT uSBCCount = 0, uDBCCount = 0, uVarCount = 0;
    int nRet = 0, nSmsMode = SMS_MODE_GSM;

    if (puCount == NULL)
    {
        return -1;
    }

    if (pszText == NULL)
    {
        *puCount = 0;
        return SMS_MODE_GSM;
    }

    pchThis = (PSTR)pszText;

    while ((*pchThis) != '\0')
    {
        if (!IsGB18030LeadChar(*pchThis)) // Single Byte Character
        {
            nRet = IsGSMCode(*pchThis, 0);

            switch (nRet)
            {
            case 0:
                if ((*pchThis) == 0x60)
                {
                    // Grave Mark '`' is a Unicode sign.
                    uDBCCount ++;
                }
                else
                {
                    // Single byte character can NOT transfer
                    // into non-GSM multiple byte character.
                    ASSERT(nRet > 0);
                    uSBCCount ++;
                }

            case 1:
                // Transfer into single byte GSM character, e.g. ASCII.
                uSBCCount ++;
                break;
                
            case 2:
                // Transfer into varible byte GSM character, e.g. [ ] { }
                // uVarCount means in Unicode mode it stands its actual value,
                // but in GSM mode, it stands number of double byte GSM
                // charactor, it must be duplicated.
                uVarCount ++;
                break;
            }

            pchThis ++;
        }
        else if (IsGB18030Code2(*(pchThis + 1))) // Double Byte Character
        {
            nRet = IsGSMCode(*pchThis, *(pchThis + 1));
            
            switch (nRet)
            {
            case 0:
                // Can't transfer into GSM, it must be double byte
                // character, e.g. Chinese character
                uDBCCount ++;
                break;
                
            case 1:
                // Transfer into single byte GSM character,
                // it must be ISGSMDBCode, begin with 0xAA, 0xAB
                uSBCCount ++;                                    
                break;
                
            case 2:
                // GSM extension, a character stands two bytes,
                uVarCount ++;
                break;
            }

            pchThis += 2;
        }
        else if (IsGB18030Code4(*(pchThis + 1))) // Quad Byte Character
        {
            uDBCCount ++;     // Double byte character, e.g. Chinese
            pchThis += 4;
        }
        else   // Error Code : 0x81 <= p1 <= 0xFE && 0x00 < p2 < 0x30
        {
            ASSERT(0);
            uDBCCount ++;
            pchThis += 2;
        }
    }

    if (uDBCCount == 0)
    {
        *puCount = uSBCCount + 2 * uVarCount;
        nSmsMode = SMS_MODE_GSM;
    }
    else
    {
        *puCount = uSBCCount + uDBCCount + uVarCount;
        nSmsMode = SMS_MODE_UNICODE;
    }

    return nSmsMode;
}
