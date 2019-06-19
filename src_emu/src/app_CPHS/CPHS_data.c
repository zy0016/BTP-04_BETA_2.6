#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "str_plx.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "me_wnd.h"

static  int iRecSize = 0, iContentLen = 0, iRecNum = 0, iRecIndex = 1;
static int iIndexFlag = 0;
static char mn_recode[20];
static char MailboxNum_line1[40];    
static char MailboxNum_line2[40];    
static char MailboxNum_Data[40];    
static char MailboxNum_Fax[40];    

extern void ParseMailboxNumber (unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData);
extern BOOL SMS_ParseMailboxNumber(unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData);


int GetMailboxNumInfo(HWND hWnd, int imsg)
{ 
    return ME_GetCPHSparameters(hWnd, imsg, 0x6F17);         
    
}
BOOL CPHS_Mailbox_init_space(HWND hWnd, int imsg)  
{
    CPHS_struct result;    

    memset(&result, 0, sizeof(CPHS_struct));

    if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
		return FALSE;
    
    if(result.Type == RecordContent)
    {
        iRecSize = result.Record_Len;
        iContentLen = result.Content_Len;
        iRecNum = iContentLen / iRecSize;
	
		PostMessage(hWnd, imsg, 0, 0);
		
		return TRUE;
    }
    else
	{
		return FALSE;
	}


}      
BOOL GetMailboxNumResult(HWND hWnd, int imsg, int decodemsg) 
{    
	int i;
    for (i = 1; i <= min(iRecNum, 4); i++)
    {
        ME_ReadCPHSRecord( hWnd, imsg + i - 1, 0x6F17, iRecSize, i); //read the record of iRecIndex
    }
    
    return TRUE;
}
int GetMailboxData(HWND hWnd, int iIndex, WPARAM wParam)
{
	char szSrc[52];
	char szDes[26];
	int	 nLen;
	
	memset(szDes, 0, 26);
	memset(szSrc, 0, 52);

	if (wParam == ME_RS_SUCCESS)
	{
		nLen = ME_GetResult(szSrc, 52);
		SMS_ParseMailboxNumber(szSrc, nLen, szDes);
	}

	if (strlen(szDes) == 0)
		return -1;

	switch(iIndex)
	{
	case 1:
		memset(MailboxNum_line1, 0 ,sizeof(MailboxNum_line1));
		strcpy(MailboxNum_line1, szDes);
		break;
	case 2:
		memset(MailboxNum_line2, 0 ,sizeof(MailboxNum_line2));
		strcpy(MailboxNum_line2, szDes);
		break;
	case 3:
		memset(MailboxNum_Data, 0 ,sizeof(MailboxNum_Data));
		strcpy(MailboxNum_Data, szDes);
		break;
	case 4:
		memset(MailboxNum_Fax, 0 ,sizeof(MailboxNum_Fax));
		strcpy(MailboxNum_Fax, szDes);
		break;
	default:
		break;
	}

	return 0;
}
void ShowMailboxNum(HWND hControl1, HWND hControl2, HWND hControl3, HWND hControl4)
{
    SendMessage(hControl1, SSBM_ADDSTRING, 0, (LPARAM)MailboxNum_line1);
    SendMessage(hControl2, SSBM_ADDSTRING, 0, (LPARAM)MailboxNum_line2);
    SendMessage(hControl3, SSBM_ADDSTRING, 0, (LPARAM)MailboxNum_Data);
    SendMessage(hControl4, SSBM_ADDSTRING, 0, (LPARAM)MailboxNum_Fax);
}
void CPHS_Mailbox_free_space(void)
{
}
void ParseInfoNumber(CPHS_INFONUM_ENTRY * DesData, BYTE * SrcData, unsigned int DataLen)
{
    BYTE * pDataLoc = NULL; 
    BYTE cur;
    unsigned int nAlphaTagByteNum;
    int Digit_1, Digit_2, Digit_3;

    pDataLoc = SrcData;

    cur = *pDataLoc; //first byte = number of bytes in the alpha-tag section;

    nAlphaTagByteNum = (unsigned int)(cur);
    
    pDataLoc++;

    cur = *(pDataLoc); //second byte: indicate Network Specific 
                           //                      Premium Srvice
                           //                      Index Level Identifier
    
    DesData -> iIndexLev = (unsigned int)(cur & 0x0F);  
    
    pDataLoc++;

//    GSMToMultiByte(pDataLoc, nAlphaTagByteNum, (LPWSTR)DesData->szEntryName, 0, NULL, NULL);

    strncpy(DesData->szEntryName, pDataLoc, nAlphaTagByteNum); //copy the char string

    pDataLoc += (nAlphaTagByteNum + 2); //pDataLoc point to digits section

    cur = * pDataLoc;

    Digit_1 = (int)(cur & 0x0F);
    
    cur = cur >> 4;

    cur = cur & 0x0F;

    Digit_2 = (int)cur;
       
    pDataLoc++;

    cur = * (pDataLoc);

    cur = cur & 0x0F;
    
    Digit_3 = (int)cur;

    DesData -> nEntryNo = Digit_3 * 100 + Digit_2 * 10 + Digit_1;    
}
