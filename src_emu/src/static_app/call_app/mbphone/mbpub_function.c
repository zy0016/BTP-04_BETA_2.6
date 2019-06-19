   
#include "stdio.h"
#include "stdlib.h"
#include "window.h"
#include "string.h"
#include "pubapp.h"
#include "pmi.h"
#include "MBPublic.h"
#include "mullang.h"
#include "prioman.h"
#include "hpimage.h"
#include "winpda.h"
#include "imesys.h"
#include "plx_pdaex.h"

static const char * pRomPath1       = "/rom";
static const char * pRomPath2       = "rom:";
static const char * pInputStr       = "英数";
static const char * pChStr          = "十";
static const char * pChEng          = "H";
static const char * pChNewLine      = "\n";
static const char * pChPoint        = ".";
static const char * pChEnd          = "\0";
static const char * pString1        = "%ld";
static const char * pChNumer0       = "0";
static const char * pMinus          = "-";
static const char   cCh_Well        = '#';
static const char   cCh_Plus        = '+';
static const char   cCh_Asterisk    = '*';
static const char   cCh_0           = '\0';
static const char   cCh_Num0        = '0';
static const char   cCh_Num9        = '9';
static const char   cCh_Point       = '.';
static const char   cCh_n           = '\n';
static const char   cCh_r           = '\r';
static const char   cCh_Space       = ' ';
static const int    pChinese        = 0x80;

static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);
static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);
static  int     GetCharactorWidth(HDC hdc,const char * ch); 
static  int     GetStringLanguage(const char * pString);    


static BOOL bFWD_UNCDTIONAL;
static BOOL bFWD_BUSY;      
static BOOL bFWD_NOREPLY;   
static BOOL bFWD_NOREACH;   
extern BOOL GetHeadSetStatus(void);
extern int  ReadFromNetWorkFile(void);
static  void    SetCallForwardIcon(BOOL bForward)
{

}
BOOL    GetForward_Uncdtional(void)
{
    return (bFWD_UNCDTIONAL);
}
BOOL    GetForward_Condition(void)
{
    return (bFWD_BUSY || bFWD_NOREPLY || bFWD_NOREACH);
}

void    SetForward_All(BOOL bForward)
{
    bFWD_UNCDTIONAL = bForward;
    bFWD_BUSY       = bForward;
    bFWD_NOREPLY    = bForward;
    bFWD_NOREACH    = bForward;

    SetCallForwardIcon(bForward);
}
void    SetForward_Uncdtional(BOOL bForward)
{
    
    
    
    
    bFWD_UNCDTIONAL  = bForward;
    if (bForward)
    {
        bFWD_BUSY    = FALSE;
        bFWD_NOREPLY = FALSE;
        bFWD_NOREACH = FALSE;
        SetCallForwardIcon(bForward);
        DlmNotify(PS_SETCALLDIVERT,ICON_SET);
    }
    else
    {
        DlmNotify(PS_SETCALLDIVERT,ICON_CANCEL);
    }
}
void    SetForward_Condition(BOOL bForward)
{
    bFWD_BUSY    = bForward;
    bFWD_NOREPLY = bForward;
    bFWD_NOREACH = bForward;
    SetCallForwardIcon(bForward);
}
void    SetForward_Busy(BOOL bForward)
{
    bFWD_BUSY = bForward;   
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReply(BOOL bForward)
{
    bFWD_NOREPLY = bForward;
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReach(BOOL bForward)
{
    bFWD_NOREACH = bForward;
    SetCallForwardIcon(bForward);
}


BOOL    FileIfExist(const char * pFileName)
{

    char *  p = NULL;
    int     iLen;
	FILE * file;


    iLen = strlen(pFileName);
    if (iLen == 0)
        return FALSE;

	file = fopen( pFileName, "r");
	if (file == NULL) 
		return FALSE;
	else{
		fclose(file);
		return TRUE;
	}

}

int     GetInputSoftKeyboardPlace(void)
{
    return 105;
}

int     GetScreenUsableWH1(int iwh)
{
	int ih = 164;
    switch (iwh)
    {
    case SM_CXSCREEN:
        return GetSystemMetrics(SM_CXSCREEN);
    case SM_CYSCREEN:
        return ih;
    }
    return (0);
}



void    GetScreenUsableWH2(int * pwidth,int * pheight)
{
    if (pwidth != NULL)
        * pwidth = GetScreenUsableWH1(SM_CXSCREEN);

    if (pheight != NULL)
        * pheight = GetScreenUsableWH1(SM_CYSCREEN);
}


int     GetEditControlHeight(void)
{
    return (GetCharactorHeight(NULL) + 5);
}

int     GetButtonControlHeight(void)
{
    return (GetCharactorHeight(NULL) + 14);
}

int     GetRadioButtonControlWidth(const char * pString)
{
    return (GetStringWidth(NULL,pString) + 15);
}

char *  RemoveStringEndNewLine(char * pStr)
{
    char * pEnd;
    int iLen;

    if (pStr == NULL)
        return NULL;
    iLen = strlen(pStr);
    if (iLen == 0)
        return pStr;

    pEnd = pStr + iLen - 1;

    while (((* pEnd == cCh_n) || (* pEnd == cCh_r)) && (iLen-- > 0))
        * pEnd-- = cCh_0;
    
    return pStr;
}

char *  RemoveStringEndSpace(char * pStr)
{
    char * pEnd;
    int iLen;

    if (pStr == NULL)
        return NULL;
    iLen = strlen(pStr);
    if (iLen == 0)
        return pStr;

    pEnd = pStr + iLen - 1;

    while ((* pEnd == cCh_Space) && (iLen-- > 0))
        * pEnd-- = cCh_0;

    return pStr;
}

int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat)
{
    char *  pDesstr = NULL;
    int     iRowNum = 0,ilen,iSourLen;
    SIZE    size;
    RECT    sRect;

    if ((NULL == cSourstr) || (NULL == pRect))
        return 0;

    iSourLen = strlen(cSourstr);
    if (iSourLen == 0)
    {
        if (cDesstr != NULL)
            strcpy(cDesstr,"");
        return 0;
    }
    if ((pRect->left < 0) || (pRect->right < 0) || (pRect->left >= pRect->right))
        return 0;

    ilen = iSourLen * (strlen(pChNewLine) + 1) + 1;
    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        if ((cDesstr == NULL) && (NULL != hdc))
            DrawText(hdc,cSourstr,iSourLen,(RECT *)pRect,uFormat);
        else
            strcpy(cDesstr,cSourstr);

        return 0;
    }
    memset(pDesstr,0x00,ilen);

    memcpy(&sRect,pRect,sizeof(RECT));

    switch (GetStringLanguage(cSourstr))
    {
    case 0:
        iRowNum = StrAutoNewLine_eng(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChEng,strlen(pChEng),&size);
        break;
    case 1:
        iRowNum = StrAutoNewLine_chn(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChStr,strlen(pChStr),&size);
        break;
    default:
        free(pDesstr);
        return 0;
    }
    ilen = strlen(pDesstr);

    if (cDesstr != NULL)
    {
        strcpy(cDesstr,pDesstr);
    }
    else
    {
        if (hdc != NULL)
        {
            if (iRowNum != 0)
                sRect.bottom = sRect.top + iRowNum * size.cy + 5;

            DrawText(hdc,pDesstr,strlen(pDesstr),&sRect,uFormat);
        }
    }
    free(pDesstr);

    return (ilen);
}

static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCharactorWidth;
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth;
    char *  p = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;
    iRowNum     = 1;
    iCopyWidth  = 0;

    while(iCopyNum < iSourlen)
    {

		if (p ==NULL)
			break;
        
        iCharactorWidth = GetCharactorWidth(hdc,p);
        if (iRectWidth - iCopyWidth > iCharactorWidth)
        {
            if ((unsigned char)(* p) > pChinese)
            {
                strncat(cDesstr,p,2);
                iCopyNum += 2;
                p += 2;
            }
            else
            {
                strncat(cDesstr,p,1);
                iCopyNum++;
                p++;
            }
            iCopyWidth += iCharactorWidth;
        }
        else if (iRectWidth - iCopyWidth == iCharactorWidth)
        {
            if ((unsigned char)(* p) > pChinese)
            {
                strncat(cDesstr,p,2);
                iCopyNum += 2;
                p += 2;
            }
            else
            {
                strncat(cDesstr,p,1);
                iCopyNum++;
                p++;
            }
            strcat(cDesstr,pChNewLine);
            iCopyWidth = 0;
            iRowNum++;
        }
        else if (iRectWidth - iCopyWidth < iCharactorWidth)
        {
            if (iCopyNum == 0)
            {
                
                
                return (0);
            }
            strcat(cDesstr,pChNewLine);
            iCopyWidth = 0;
            iRowNum++;
        }
    }
    strcat(cDesstr,pChEnd);

    return (iRowNum);
}

static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth,iWordWidth,iWordLength,iCopyNumExt;
    char *  p = NULL,* pNextSpace = NULL,* pScan = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;
    iRowNum     = 1;
    iCopyWidth  = 0;

    while (iCopyNum < iSourlen)
    {

		if (p ==NULL)
			break;
        

        pScan       = p;
        pNextSpace  = strchr(p,cCh_Space);

        iWordWidth  = 0;

        if (pNextSpace != NULL)
        {
            iWordLength = abs(pScan - pNextSpace);

            while (pScan != pNextSpace)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);
                pScan++;
            }
        }
        else
        {
            iWordLength = iSourlen - iCopyNum;

            while (*pScan != 0)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);
                pScan++;
            }
        }
        
        if (iRectWidth - iCopyWidth > iWordWidth)
        {
            strncat(cDesstr,p,iWordLength + 1);
            iCopyNum   += (iWordLength + 1);
            iCopyWidth += (iWordWidth + GetCharactorWidth(hdc,pNextSpace));

            p += (iWordLength + 1);
        }
        else if(iRectWidth - iCopyWidth == iWordWidth)
        {
            strncat(cDesstr,p,iWordLength + 1);
            strcat(cDesstr,pChNewLine);

            iCopyNum += (iWordLength + 1);
            iRowNum++;
            iCopyWidth = 0;

            p += (iWordLength + 1);
        }
        else
        {
            if ((iCopyNum == 0) || 
                (iRectWidth - iCopyWidth == iRectWidth))
            {
                iCopyNumExt = 0;
                
                while (iCopyNumExt <= iWordLength)
                {

					if (p ==NULL)
						break;
                    if ((* p == 0xa) || (* p == 0xd))
                    {
                        p++;
                        iCopyNum++;
                        iCopyNumExt++;
                        continue;
                    }
                    iWordWidth = GetCharactorWidth(hdc,p);
                    if (iRectWidth - iCopyWidth > iWordWidth)
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        iCopyWidth += iWordWidth;
                    }
                    else if (iRectWidth - iCopyWidth == iWordWidth)
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        strcat(cDesstr,pChNewLine);
                        iCopyWidth = 0;
                        iRowNum++;
                    }
                    else if (iRectWidth - iCopyWidth < iWordWidth)
                    {
                        if (iCopyNum == 0)
                        {
                            return 0;
                        }
                        strcat(cDesstr,pChNewLine);
                        iCopyWidth = 0;
                        iRowNum++;
                        iCopyNumExt++;
                    }
                }
            }
            else
            {
                strcat(cDesstr,pChNewLine);
                iRowNum++;
                iCopyWidth = 0;
            }
        }
    }
    strcat(cDesstr,pChEnd);

    return (iRowNum);
}

static  int     GetCharactorWidth(HDC hdc,const char * ch)
{
    SIZE size;

    if ( ch == NULL )
        return 0;

    if ((unsigned char)(* ch) > pChinese)
        GetTextExtent(hdc,ch,2,&size);
    else
        GetTextExtent(hdc,ch,1,&size);

    return (size.cx);
}

int     GetStringWidth(HDC hdc,const char * pString)
{
    SIZE size = {0,0};
    int iLength;
    if (pString == NULL)
        return -1;

    iLength = strlen(pString);
    if (0 == iLength)
        return 0;
    GetTextExtentPoint(hdc,pString,iLength,&size);
    return size.cx;
}

int     GetCharactorHeight(HWND hWnd)
{
    HDC hdc;
    SIZE size = {0,0};

    if (hWnd != NULL)
    {
        hdc = GetDC(hWnd);
        GetTextExtent(hdc,pChStr,2,&size);
        ReleaseDC(hWnd,hdc);
    }
    else
    {
        GetTextExtent(NULL,pChStr,2,&size);
    }
    return (size.cy);
}

static  int     GetStringLanguage(const char * pString)
{
    char * pch = NULL;

    if (NULL == pString)
        return -1;

    pch = (char *)pString;
    while (* pch)
    {
        if ((unsigned char)(* pch) > pChinese)
            return 1;

        pch++;
    }
    return 0;
}



BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count)
{
    int i;
    int iControlNum;            
    int ixStep;                 
    unsigned int iScreenWidth;  
    unsigned int iControlsWidth;

    if (pConX == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);
    iScreenWidth    = GetScreenUsableWH1(SM_CXSCREEN);
    iControlsWidth  = iControlNum * iControlW;
    if (iControlsWidth > iScreenWidth)
        return FALSE;

    ixStep = (iScreenWidth - iControlsWidth) / (iControlNum + 1);
    for (i = 0;i < iControlNum ; i++)
        * pConX++ = ixStep + i * (iControlW + ixStep);

    return TRUE;
}

BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count)
{
    int i;
    int iControlNum;                
    int iyStep;                     
    unsigned int iControlsHeight;   
    unsigned int iScreenHeight;     

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  
    iControlsHeight = iControlNum * iControlH;          

    if (iScreenHeight < iControlsHeight)
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);
    for (i = 0;i < iControlNum;i++)
        * piConY++ = iyStep + i * (iControlH + iyStep);

    return TRUE;
}

BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace)
{
    int i,iyStep,iControlNum;
    unsigned int iScreenHeight;     
    unsigned int iControlsHeight;   

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  
    iControlsHeight = iSpace + iControlNum * iControlH; 

    if (iScreenHeight < iControlsHeight)
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);

    switch (iDirect)
    {
    case 1:
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep) + iSpace;
        return TRUE;

    case -1:
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep);
        return TRUE;

    default:
        return FALSE;
    }
}

void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout)
{
    HDC     hdc;
    char *  pDesstr = NULL;
    int     ilen;
    const RECT rect = {80,120,210,180};

    if (szPrompt == NULL)
        return;
    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        PLXTipsWin(NULL, NULL, 0, (char *)szPrompt,NULL, Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);
        return;
    }
    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;

    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        PLXTipsWin(NULL, NULL, 0, (char *)szPrompt,NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
        return;
    }
    memset(pDesstr,0x00,ilen);
    
    hdc = GetDC(hwnd);
    StrAutoNewLine(hdc,szPrompt,pDesstr,&rect,0);
    ReleaseDC(hwnd,hdc);

    PLXTipsWin(NULL, NULL, 0, pDesstr,NULL, Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);

    free(pDesstr);
}

BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel)
{
    HDC     hdc;
    char *  pDesstr = NULL;
    int     ilen;
    BOOL    bres;
    const RECT rect = {80,120,210,180};

    if (szPrompt == NULL)
        return FALSE;
    
    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        return (PLXConfirmWin(NULL, hwnd, szPrompt,Notify_Alert,szCaption,CONFIRM_OK,CONFIRM_CANCEL));
    }

    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;

    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        bres = PLXConfirmWin(NULL, hwnd, szPrompt,Notify_Alert,szCaption,CONFIRM_OK,CONFIRM_CANCEL);
        return (bres);
    }
    memset(pDesstr,0x00,ilen);

    hdc = GetDC(hwnd);
    StrAutoNewLine(hdc,szPrompt,pDesstr,&rect,0);
    ReleaseDC(hwnd,hdc);

    bres = PLXConfirmWin(NULL, hwnd, pDesstr,Notify_Alert,szCaption,CONFIRM_OK,CONFIRM_CANCEL);

    free(pDesstr);

    return(bres);
}

BOOL    StrIsNumber(const char * pStr)
{
    char * p = (char *)pStr;

    if (strlen(pStr) == 0)
        return FALSE;

    while (* p)
    {
        if ((* p < cCh_Num0) || (* p > cCh_Num9))
            return FALSE;
        p++;
    }
    return TRUE;
}

BOOL    StrIsDouble(const char * pStr)
{
    char * p = (char *)pStr;
    int iPointNum = 0;

    if (strlen(pStr) == 0)
        return FALSE;

    if (* p == cCh_Point)
        return FALSE;

    while (* p)
    {
        if (((* p < cCh_Num0) || (* p > cCh_Num9)) && (* p != cCh_Point))
            return FALSE;
        if (* p == cCh_Point)
            iPointNum++;
        p++;
    }
    if (iPointNum > 1)
        return FALSE;

    p--;
    if (* p == cCh_Point)
        return FALSE;

    return TRUE;
}

char *  MB_Mtoa(double dNumber,char * pNumber)
{
    int iDec,iSign;
    char * pBuffer = NULL;

    pBuffer = ecvt(dNumber,10,&iDec,&iSign);

    if (iSign < 0)
        strcat(pNumber,pMinus);

    if (iDec > 0)
    {
        strncat(pNumber,pBuffer,iDec);  
        strcat(pNumber,pChPoint);       
        strcat(pNumber,pBuffer + iDec); 
    }
    else
    {
        strcat(pNumber,pChNumer0);  
        strcat(pNumber,pChPoint);   
        while (iDec++ < 0)          
            strcat(pNumber,pChNumer0);

        strcat(pNumber,pBuffer);    
    }
    return pNumber;
}

double  MB_atof(const char * pString)
{
    double result;
    double dMult = 1.0;
    int iLen;
    BOOL bMult = FALSE;
    char * pNewString = NULL;

    iLen = strlen(pString);
    if (iLen == 0)
        return 0.0;

    pNewString = (char *)malloc(iLen + 1);
    if (pNewString == NULL)
        return 0.0;

    memset(pNewString,0x00,iLen + 1);

    while (* pString)
    {
        if (* pString == cCh_Point)
        {
            bMult = TRUE;
        }
        else
        {
            if (bMult)
                dMult *= 10;

            strncat(pNewString,pString,1);
        }
        pString++;
    }
    result = atol(pNewString) / dMult;

    free(pNewString);
    return result;
}

void    MB_DealDouble(char * pSour,unsigned int iBit)
{
    char * p;

    if (pSour == NULL)
        return;
    p = strstr(pSour,pChPoint);
    if (p == NULL)
        return;

    if (iBit == 0)
    {
        * p = cCh_0;
        return;
    }

    p++;
    if (strlen(p) > iBit)
    {
        p += iBit;
        * p = cCh_0;
    }
}

int     InStrArray(const char * pSearchStr,const char ** pArray)
{
    char ** p;
    int iIndex = 0;

    if (strlen(pSearchStr) == 0)
        return -1;

    p = (char**)pArray;
    while (**p)
    {
        if (strcmp(* p,pSearchStr) == 0)
            return iIndex;

        p++;
        iIndex++;
    }
    return -1;
}

int     InIntArray(int iSearchInt,const int * pArray,size_t count)
{
    int iArrayNum,i;
    int * p = NULL;

    p = (int*)pArray;
    iArrayNum = count / sizeof(int);

    for (i = 0;i < iArrayNum;i++)
    {
        if (* p == iSearchInt)
            return i;
        p++;
    }
    return -1;
}

int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2)
{
    if (pTime1->wYear < pTime2->wYear)
        return -1;
    if (pTime1->wYear > pTime2->wYear)
        return 1;
    
    if (pTime1->wMonth < pTime2->wMonth)
        return -1;
    if (pTime1->wMonth > pTime2->wMonth)
        return 1;
    
    if (pTime1->wDay < pTime2->wDay)
        return -1;
    if (pTime1->wDay > pTime2->wDay)
        return 1;
    
    if (pTime1->wHour < pTime2->wHour)
        return -1;
    if (pTime1->wHour > pTime2->wHour)
        return 1;
    
    if (pTime1->wMinute < pTime2->wMinute)
        return -1;
    if (pTime1->wMinute > pTime2->wMinute)
        return 1;
    
    if (pTime1->wSecond < pTime2->wSecond)
        return -1;
    if (pTime1->wSecond > pTime2->wSecond)
        return 1;
    
    return 0;
}

void    MBPlayMusic(int iMult)
{
    if (0 == iMult)
    {
        PrioMan_CallMusic(PRIOMAN_PRIORITY_PHONE,0);
        return;
    }
    PlayClewMusic(1);
}

BOOL    EarphoneStatus(void)
{
    return (GetHeadSetStatus());
}

void    PlayClewMusic(unsigned int iPlayCount)
{
    
}

BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber, ABINFO *pabInfo)
{
    int     iSour;

    
    memset(pabInfo,0x00,sizeof(ABINFO));

    if (MB_GetInfoFromPhonebook(pNumber,pabInfo))
    {
        iSour = strlen(pabInfo->szName);
        if (iSour > 0)  
        {
            strncpy(pPhoneItem->cName,pabInfo->szName,
                sizeof(pPhoneItem->cName) > iSour ? iSour + 1 : sizeof(pPhoneItem->cName) - 1);
        }
        else            
        {
            memset(pPhoneItem->cName,0x00,sizeof(pPhoneItem->cName));
        }
        return TRUE;
    }
    else
    {
        memset(pPhoneItem->cName,0x00,sizeof(pPhoneItem->cName));
        return FALSE;
    }
}

BOOL    IfValidPhoneNumber(const char * pPhoneNumber)
{
    char * p;

    if (NULL == pPhoneNumber)
        return FALSE;
    if (strlen(pPhoneNumber) == 0)
        return FALSE;

    p = (char *)pPhoneNumber;
    while (* p)
    {
        if ((cCh_Num0 <= * p) && (* p <= cCh_Num9) || 
            (cCh_Well == * p) || (cCh_Plus == * p) || 
            (cCh_Asterisk == * p) || (EXTENSIONCHAR == * p))
            p++;
        else
            return FALSE;
    }
    return TRUE;
}

#define MBWAIT_TITLE    ML("Handledata") 
#define MBWAIT_STATE    ML("Pleasewait") 

static const char * pMBWaitClassName = "MBWaitWindow";
#ifndef _EMULATE_
static const char * pMBWaitPic = "ROM:/net_search.gif";
#else
static const char * pMBWaitPic = "ROM:/net_search.gif";
#endif

static HWND         hMBWaitWindow = NULL;
static HINSTANCE    hInstance;
static LRESULT MBWaitAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL    MBWaitWindow(void)
{
    WNDCLASS wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = MBWaitAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pMBWaitClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hMBWaitWindow = CreateWindow(pMBWaitClassName,MBWAIT_TITLE, 
        WS_CAPTION |PWS_STATICBAR|PWS_NOTSHOWPI, 
        PLX_WIN_POSITION, NULL, NULL, NULL, NULL);

    if (NULL == hMBWaitWindow)
    {
        UnregisterClass(pMBWaitClassName,NULL);
        return FALSE;
    }
    ShowWindow(hMBWaitWindow,SW_SHOW);
    UpdateWindow(hMBWaitWindow);
    return TRUE;    
}
static LRESULT MBWaitAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HGIFANIMATE hgifanimate;
    static  RECT        rectState;
    static  const int   iPicY = 10;
    static  SIZE        size = {0,0};
            int         isw,iFontHeight;
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        hgifanimate = NULL;
        isw         = GetScreenUsableWH1(SM_CXSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);

        if ((size.cx == 0) || (size.cy == 0))
            GetImageDimensionFromFile(pMBWaitPic,&size);

        hgifanimate = StartAnimatedGIFFromFile(hWnd,pMBWaitPic,
            (isw - size.cx) / 2,iPicY,DM_NONE);

        SetRect(&rectState,0,size.cy + iPicY,isw,size.cy + iPicY + iFontHeight * 2);
        break;

    case WM_TODESKTOP:
        return TRUE;

    case WM_DESTROY :
        EndAnimatedGIF(hgifanimate);
        hMBWaitWindow = NULL;
        UnregisterClass(pMBWaitClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint(hWnd,NULL);
        DrawText(hdc,MBWAIT_STATE,strlen(MBWAIT_STATE),&rectState,DT_CENTER);
        PaintAnimatedGIF(hdc,hgifanimate);
        EndPaint(hWnd,NULL);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

BOOL    MBWaitWindowState(BOOL bState)
{
    if (bState)
    {
        MBWaitWindow();
    }
    else
    {
        if (IsWindow(hMBWaitWindow))
            DestroyWindow(hMBWaitWindow);
    }
    return TRUE;
}



/*

BOOL    MB_GetInfoFromPhonebook(const char * pPhone, ABINFO * pInfo)
{

    BOOL bPhoneBook = FALSE;
    

    if (bPhoneBook)
    {
        if (FileIfExist(pInfo->szIcon))
            printf("Portrait %s is exist\r\n",pInfo->szIcon);
        else
            printf("Portrait %s isn't exist\r\n",pInfo->szIcon);
    }
    return (bPhoneBook);

}*/


/*
int     MB_GetDirectDial(WORD wDDIdx, char * pNo)
{
    return -1;
    
}

int     MB_GetSIMRcdByOder(int iOrder,PPHONEBOOK pRtnRcd)
{
    return -1;
}*/


static  BOOL        bScreen;
static  DWORD       bLcdState;
void    DisableState(void)
{
    bScreen   = DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,FALSE);
    bLcdState = DlmNotify(PMM_NEWS_GET_FLAGS,0);

    DlmNotify(PMM_NEWS_DISABLE, PMF_AUTO_SHUT_DOWN |
                                PMF_AUTO_SHUT_KEYLIGHT |
                                PMF_AUTO_SHUT_MAIN_LIGHT |
                                PMF_AUTO_SHUT_CPU);

    DlmNotify(PMM_NEWS_DISABLE,PMF_AUTO_SHUT_DOWN);

    f_DisablePowerOff(INCALL);
#ifndef _EMULATE_
    DHI_OpenAudioRoute();
#endif

    PhoneSetChannel(TRUE);
}
void    EnableState(void)
{
    if (bScreen)
        DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,TRUE);

    DlmNotify(PMM_NEWS_ENABLE,bLcdState);

    f_EnablePowerOff(INCALL);
#ifndef _EMULATE_
    DHI_CloseAudioRoute();
#endif

    PhoneSetChannel(FALSE);
}

void    PhoneSetChannel(BOOL bSet)
{
    printf("mb SetChannel:%d\r\n",bSet);
#ifndef _EMULATE_
    
    
#endif
}
void    PhoneSetChannelCalling(BOOL bSet)
{
    printf("mb SetChannelCalling:waveOutSetChannel(NULL,TRUE,0x80000000)\r\n");
#ifndef _EMULATE_
    
#endif
}

void InitPaint(INITPAINTSTRUCT * ppaintif, BOOL bphoto, BOOL bicon)
{
	memset(ppaintif, 0, sizeof(INITPAINTSTRUCT));
	SetRect(&(ppaintif->subcap), SUB_CAPTION_RECT);
//	if (bicon)
//	{
		SetRect(&(ppaintif->subcapleft), SUB_CAPTION_ICON_LEFT_RECT);
		SetRect(&(ppaintif->subcapright), SUB_CAPTION_ICON_RIGHT_RECT);
//	}
	SetRect(&(ppaintif->subcapmid), SUB_CAPTION_ICON_MIDDLE_RECT);
	if (bphoto)
	{
		SetRect(&(ppaintif->photorect), PHOTO_RECT);
		SetRect(&(ppaintif->line1), LINE1_RECT);
        SetRect(&(ppaintif->line2), LINE2_RECT);
	}
	else
	{
		SetRect(&(ppaintif->line1), LINE1_RECT);
        SetRect(&(ppaintif->line2), LINE2_RECT);
	}
	SetRect(&(ppaintif->line3), LINE3_RECT);
	SetRect(&(ppaintif->line4), LINE4_RECT);
}

void InitPhoto(INITPAINTSTRUCT * ppaintif, SIZE *psize)
{
	int tx1, tx2, ty1, ty2, tcx, tcy;
	RECT defaultrect;
	SetRect(&(defaultrect), PHOTO_RECT);
	tcx = defaultrect.right - defaultrect.left;
	tcy = defaultrect.bottom- defaultrect.top;
	if (psize->cy < tcy && psize->cx < tcx)
	{
		tx1 = (176 - psize->cx)/2;
		tx2 = tx1+psize->cx;
		ty1 = 35+ (tcy-  psize->cy)/2;
		ty2 = ty1 + psize->cy;
	}
	else if (psize->cy >= tcy && psize->cx < tcx) 
	{
		tcx = (psize->cx * tcy) / psize->cy;
		tx1 = (176-tcx)/2;
		tx2 = tx1+tcx;
		ty1 = defaultrect.top;
		ty2 = defaultrect.bottom;
	}
	else if (psize->cy < tcy && psize->cx >= tcx) 
	{
		tcy = (psize->cy * tcx)/psize->cx;
		ty1 = 35+ (defaultrect.bottom- defaultrect.top - tcy)/2;
		ty2 = ty1 + tcy;
		tx1 = defaultrect.left;
		tx2 = defaultrect.right;		
	}
	else if (psize->cy >= tcy && psize->cx >= tcx) 
	{
		tcx = (psize->cx * tcy) / psize->cy;
		tx1 = (176-tcx)/2;
		tx2 = tx1+tcx;
		ty1 = defaultrect.top;
		ty2 = defaultrect.bottom;
	}
	SetRect(&(ppaintif->photorect), tx1, ty1, tx2, ty2);
}

void InitTxt(HDC hdc, char * srctxt, char * showtxt, int width, BOOL fend)
{
	int nFit;
	char * suspension = "...";
	HFONT holdfont;
	HFONT hnewfont;
	SIZE  sussize;
	char * p;

	if (srctxt != NULL)
		p = srctxt;
	else
		p = NULL;
	if (GetFontHandle(&hnewfont, LARGE_FONT))
	{
		holdfont = SelectObject(hdc, hnewfont);
	}
	GetTextExtentExPoint(hdc, srctxt, strlen(srctxt), width, &nFit, NULL, NULL);
	
	if (strlen(srctxt) > (unsigned int)nFit) 
	{
		GetTextExtentPoint32(hdc,suspension,strlen(suspension),&sussize);
		GetTextExtentExPoint(hdc, srctxt, strlen(srctxt), width-sussize.cx, &nFit, NULL, NULL);
		if (fend)
		{
			strncpy(showtxt, srctxt, nFit);
			showtxt[nFit] = '\0';
			strcat(showtxt, suspension);
		}
		else
		{
			int leftlen;
			leftlen = strlen(srctxt) - nFit;
			p = p+leftlen;
			sprintf(showtxt, "%s%s", suspension, p);
		}		
	}
	else
	{
		strcpy(showtxt, srctxt);
	}
	if (GetFontHandle(&hnewfont, LARGE_FONT))
	{
		SelectObject(hdc, holdfont);
	}		
}

int MB_GetSendLocalNumber(void)
{
    int iLocal = 0;
//#ifndef _EMULATE_
    iLocal = ReadFromNetWorkFile();
//#endif
    return iLocal;
}

int GetNetworkStatus(void)//获得网络状态
{
    //define newwork status
    //#define NONETWORK   0
    //#define HOMEZONE    1
    //#define ROAMING     2
    //#define REJECTSIMCARD 3
    //#define NOSERVICE   4
    //#define ONLY_EMERGENCYCALL 5
    return PM_GetNetWorkStatus();
}
