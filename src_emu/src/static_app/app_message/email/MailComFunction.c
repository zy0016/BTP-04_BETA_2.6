/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailComFunction.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailComFunction.h"

BOOL MAIL_CPU_TYPE;  //CPU type

int MAIL_DateCmpChar(char *FirstChar, char *SecondChar)
{
    char  DateBuf1[11];
    char  DateBuf2[11];
    SYSTEMTIME FirstSy;
    SYSTEMTIME SecondSy;
    unsigned long Value1, Value2;

    if((strlen(FirstChar) == 0) || (strlen(SecondChar) == 0))
        return Late;
    
    memset(&FirstSy, 0x0, sizeof(SYSTEMTIME));
    memset(&SecondSy, 0x0, sizeof(SYSTEMTIME));

    MAIL_CharToSystem(FirstChar, &FirstSy);
    MAIL_CharToSystem(SecondChar, &SecondSy);

    memset(DateBuf1, 0x0, 11);
    memset(DateBuf2, 0x0, 11);
    
    sprintf(DateBuf1, "%04d%02d%02d", (int)FirstSy.wYear,
        (int)FirstSy.wMonth, 
        (int)FirstSy.wDay
        );
    
    sprintf(DateBuf2, "%04d%02d%02d", (int)SecondSy.wYear,
        (int)SecondSy.wMonth, 
        (int)SecondSy.wDay
        );
    
    Value1 = atol(DateBuf1);
    Value2 = atol(DateBuf2);
    
    if (Value1 < Value2)
        return Early;
    else if (Value1 > Value2)
        return Late;
    else
    {
        memset(DateBuf1, 0, 11);
        memset(DateBuf2, 0, 11);
        
        sprintf(DateBuf1, "%02d%02d", (int)FirstSy.wHour, (int)FirstSy.wMinute);
        sprintf(DateBuf2, "%02d%02d", (int)SecondSy.wHour,   (int)SecondSy.wMinute);
        
        Value1 = atol(DateBuf1);
        Value2 = atol(DateBuf2);
        
        if (Value1 < Value2)
            return Early;
        else if (Value1 > Value2)
            return Late;
        else
            return Equality;
    }  
}
void MAIL_GetLocalDateString(char * szLocalDate)
{
    SYSTEMTIME      pSysTime;
    GetLocalTime(&pSysTime);
    sprintf(szLocalDate, "%04d-%02d-%02d %02d:%02d:%02d", 
        pSysTime.wYear,   pSysTime.wMonth, 
        pSysTime.wDay,    pSysTime.wHour,
        pSysTime.wMinute, pSysTime.wSecond);
}

void MAIL_SpecialChar(char *CharDate, SYSTEMTIME *SysDate)
{
    char    *pTempDate, *pPreDate;
    char    pTempMonth[4];
    int     i;

    //month
    if(*(CharDate + 3) != 0x20)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        
        return;
    }
    strncpy(pTempMonth, (CharDate + 4), 3);
    pTempMonth[3] = '\0';
    for(i=0; i<12; i++)
    {
        if(strcmp(nMonthEnString[i], pTempMonth) == 0)
        {
            SysDate->wMonth = (WORD)nMonthDaString[i];
            break;
        }
    }

    //day
    if(*(CharDate + 7) != 0x20)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        
        return;
    }
    pTempDate = CharDate + 8;
    pPreDate = pTempDate;
    while(*pPreDate != 0x20)
    {
        pPreDate ++;
    }
    if((pPreDate - pTempDate) > 2)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        return;
    }
    SysDate->wDay = (WORD)strtol(pTempDate, NULL, 3);

    //time
    if(strcmp((pPreDate + 1), ":"))
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        
        return;
    }
    SysDate->wHour = (WORD)atoi(pPreDate + 1);
    SysDate->wMinute = (WORD)atoi(pPreDate + 4);
    SysDate->wSecond = (WORD)atoi(pPreDate + 7);

    //year
    SysDate->wYear = (WORD)atoi(pPreDate + 10);

    return;
}

void MAIL_CharToSystem(char *CharDate, SYSTEMTIME *SysDate)
{
    int     nlen;
    char    *pTempDate, *pPreDate;
    char    pTempMonth[4];
    int     i;

    nlen = strlen(CharDate);
    //strlen("9 Jun 2003") == 10
    if(nlen < 10)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        
        return;
    }
    pTempDate = CharDate;

    //Mon, 9 Jun 2003 15:25:40 +0800
    //delete charachters before ',' 
    if(strstr(CharDate, ","))
    {
        while (*pTempDate != ',')
        {
            if(*pTempDate == 0x0)
            {
                MAIL_SpecialChar(CharDate, SysDate);
                return;
            }
            pTempDate ++;
        }
        pTempDate ++;
    }
   
    //day
    while(*pTempDate == 0x20)
    {
        pTempDate ++;
    }
    pPreDate = pTempDate;
    while(*pPreDate != 0x20)
    {
        pPreDate ++;
    }
    if((pPreDate - pTempDate) > 2)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        return;
    }

    SysDate->wDay = (WORD)strtol(pTempDate, NULL, 10);
    
    //month
    while(*pPreDate == 0x20)
    {
        pPreDate ++;
    }
    pTempDate = pPreDate;
    while(*pPreDate != 0x20)
    {
        pPreDate ++;
    }
    strncpy(pTempMonth, pTempDate, pPreDate - pTempDate);
    pTempMonth[3] = '\0';
    for(i=0; i<12; i++)
    {
        if(strcmp(nMonthEnString[i], pTempMonth) == 0)
        {
            SysDate->wMonth = (WORD)nMonthDaString[i];
            break;
        }
    }
    
    //year
    while(*pPreDate == 0x20)
    {
        pPreDate ++;
    }
    pTempDate = pPreDate;
    while(*pPreDate != 0x20)
    {
        pPreDate ++;
    }
    if((pPreDate - pTempDate) > 4)
    {
        SysDate->wYear = 1980;
        SysDate->wMonth = 1;
        SysDate->wDay = 1;
        return;
    }
    SysDate->wYear = (WORD)strtol(pTempDate, NULL, 10);

    //hour,minute,second
    while(*pPreDate == 0x20)
    {
        pPreDate ++;
    }
    SysDate->wHour = (WORD)atoi(pPreDate);
    SysDate->wMinute = (WORD)atoi(pPreDate + 3);
    SysDate->wSecond = (WORD)atoi(pPreDate + 6);

    return;
}

void MAIL_SystemToChar(SYSTEMTIME *SysDate, char *CharDate)
{
    char    szMonth[10] = {'\0'};
    char    szDay[10] = {'\0'};
    char    szHour[10] = {'\0'};
    char    szMinute[10] = {'\0'};
    char    szSecond[10] = {'\0'};
    char    szDayOfWeek[10] = {'\0'};
    int     tmp;
    
    if((SysDate->wDay - 10) < 0)
        sprintf(szDay, "0%d", SysDate->wDay);
    else
        sprintf(szDay, "%2d", SysDate->wDay);

    if((SysDate->wHour -10) < 0)
        sprintf(szHour, "0%d", SysDate->wHour);
    else
        sprintf(szHour, "%2d", SysDate->wHour);

    if((SysDate->wMinute -10) < 0)
        sprintf(szMinute, "0%d", SysDate->wMinute);
    else
        sprintf(szMinute, "%2d", SysDate->wMinute);

    if((SysDate->wSecond -10) < 0)
        sprintf(szSecond, "0%d", SysDate->wSecond);
    else
        sprintf(szSecond, "%2d", SysDate->wSecond);

    tmp = (int)SysDate->wMonth;
    sprintf(szMonth, "%s", nMonthEnString[tmp]);
    szMonth[3] = '\0';

    tmp = (int)SysDate->wDayOfWeek;
    sprintf(szDayOfWeek, "%s", nWeekEnString[tmp]);
    szDayOfWeek[3] = '\0';

    sprintf(CharDate, "%s, %s %s %4d %s:%s:%s +8000 (CST)", 
        szDayOfWeek, szDay, szMonth, SysDate->wYear, szHour, szMinute, szSecond);

    return;
}

/******************************************************************** 
* Function	   MAIL_GetNewFileName
*
* Purpose      Get a filename that doesn't exist.
* Return	   
* Remarks	   
\**********************************************************************/
void MAIL_GetNewFileName(char *szNewFilePath, int len)
{
    WORD wTimer;
    char szTmpSaveFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];

    srand(GetTickCount());
    while (1)
    {
        memset(szTmpSaveFile, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        wTimer = rand() & 0xFFFF;
        sprintf(szTmpSaveFile, szNewFilePath, wTimer);
        if(!MAIL_IsFileExist(szTmpSaveFile))
            break;
    }

    memset(szNewFilePath, 0x0, len);
    if((int)strlen(szTmpSaveFile) <= len)
    {
        strcpy(szNewFilePath, szTmpSaveFile);
        return;
    }
    return;
}

/******************************************************************** 
* Function	   MAIL_IsFileExist
* Purpose      判断文件是否存在
* Params	   文件名  
* Return	   TRUE:存在 FALSE 不存在
* Remarks	   
\**********************************************************************/
BOOL MAIL_IsFileExist(char *szFileName)
{
    int hFile;
    char szOutPath[PATH_MAXLEN];
    /*
    static struct dirent *dirinfo = NULL;
        static DIR *diropen = NULL;
    
        if(strlen(szFileName) == 0)
            return FALSE;
    
        chdir(MAIL_FILE_PATH);
        diropen = opendir(MAIL_FILE_PATH);
        if(diropen == NULL)
            return FALSE;
        while (dirinfo = readdir(diropen))
        {
            if(szFileName == dirinfo->d_name)
            {
                closedir(diropen);
                return TRUE;
            }
        }
        closedir(diropen);
        return FALSE;*/
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    hFile = GARY_open(szFileName, O_RDONLY, -1);
    
    if(hFile < 0)
    {
        chdir(szOutPath);
        return FALSE;
    }
    else
    {
        GARY_close(hFile);
        chdir(szOutPath);
        return TRUE;
    }
}

/*********************************************************************************
*  function: MAIL_JudgeCPUType()	
*									
*  purpose : get cpu type
*
*  return  : 
*           
**********************************************************************************/
void MAIL_JudgeCPUType(void)
{
    BYTE bByte;
    WORD wWord;

    wWord = 0x1234;
    bByte = (BYTE)(*((BYTE *)(wWord)));
    MAIL_CPU_TYPE = (bByte == 0x34) ? MAIL_CPU_REVERSE : MAIL_CPU_OBVERSE;
}

/**************************************************************************** 
* Function : MAIL_IsSpaceEnough
*
* Purpose  : Whether mail space enough
*			 true  == has spare space
*			 false == no enough space or reach the maximum number
*
\*****************************************************************************/
BOOL MAIL_IsSpaceEnough(void)
{
#ifndef _MAIL_EMU_
    unsigned int total;
    //file system space
    if(MAIL_IsFlashAvailable(MAX_MAIL_SIZE_K) != SPACE_AVAILABLE)
    {
        PLXTipsWin(NULL, NULL, NULL, IDP_MAIL_STRING_NOSPACE, NULL, Notify_Failure, IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    total = MAIL_GetMailTotalFile();
    
    if(total == -1)
    {
        //MAIL_PopMessage(NULL, ML("mail folder error"));
        return FALSE;
    }
    if(total >= MAX_MAIL_TOTAL_MAIL_NUM)
    {
        //MAIL_PopMessage(NULL, ML(MAIL_String[STR_MAIL_MACRO_MSG13]));
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#endif
    return TRUE;
}

int MAIL_IsFlashEnough()
{
    int nRet;
    
    //nRet = IsFlashAvailable(MIN_FREE_SPACE);
    nRet = MAIL_IsFlashAvailable(MAX_MAIL_SIZE_K);
    return nRet;
}

static int MAIL_IsFlashAvailable(int nSize)
{
    struct statfs *spaceinfo = NULL;
    int ret = -1;

    spaceinfo = (struct statfs *)malloc(sizeof(struct statfs));
    if(spaceinfo == NULL)
        return ret;

    memset(spaceinfo, 0x0, sizeof(struct statfs));
    statfs("/mnt/flash", spaceinfo);
    if(nSize > spaceinfo->f_bfree)
        ret = SPACE_CHECKERROR;
    else if(nSize == spaceinfo->f_bfree)
        ret = SPACE_NEEDCLEAR;
    else
        ret = SPACE_AVAILABLE;

    GARY_FREE(spaceinfo);
    spaceinfo = NULL;

    return ret;
}

int MAIL_GetMailTotalFile(void)
{
    unsigned int iMailTotal;
    static DIR  *diropen = NULL;
    
    iMailTotal = 0;
    chdir(MAIL_FILE_PATH);
    diropen = GARY_opendir(MAIL_FILE_PATH);
    if(diropen == NULL)
        return -1;

    while (readdir(diropen))
    {
        iMailTotal ++;
    }
    GARY_closedir(diropen);

    return iMailTotal;
}

int MAIL_StringCut(char *pStr, unsigned int nCutLen)
{
    unsigned int i;
    unsigned int len;

    i = 0;
    len = strlen(pStr);
    if(len < nCutLen)
    {
        return len;
    }
    while(i < nCutLen)
    {
        if(IsDBCSLeadByte(pStr[i]))
        {
            i += 2;
            if(i > nCutLen)
            {
                i -= 2;
                break;
            }
        }
        else
        {
            i ++;
        }
    }
    return i;
}

/*******************************************************************************
Function :    Mail_UpperCase
*********************************************************************************/    
void MAIL_UpperCase(char *pDest)
{
    int  i;
    char ch;
    
    for (i = 0; i < (int)strlen(pDest); i++)
    {     
        ch = pDest[i];
        
        if (ch >= 'a' && ch <= 'z')
            ch = ch - 32;
        
        pDest[i] = ch;
    }
    
}

/*******************************************************************************
Function :    MAIL_LowerCase
*********************************************************************************/    
void MAIL_LowerCase(char *pDest)
{
    int  i;
    char ch;
    
    for (i = 0; i < (int)strlen(pDest); i++)
    {     
        ch = pDest[i];
        
        if (ch >= 'A' && ch <= 'Z')
            ch = ch + 32;
        
        pDest[i] = ch;
    }
    
}

int MAIL_GetFileSuffix(void)
{
    return 3;
}

/*****************************************************************************\
* Function : MAIL_TrimString
*
* Purpose  : delete the anterior and the latter blank
*
\*****************************************************************************/
void MAIL_TrimString(char * pstr)
{
    int len;
    char *pTmp;
    
    len = strlen(pstr);
    //no blank in the front
    if((pstr[0] != 0x20) && (pstr[len -1] != 0x20))
        return ;
    
    pTmp = pstr;
    while(*pTmp == 0x20)
    {
        pTmp++;
    }
    //delete blank in the front
    strcpy(pstr,pTmp);
    len = strlen(pstr);
    
    while(pstr[len -1] == 0x20)
    {
        pstr[len - 1] = 0x0;
        len --;
    }
}

int MAIL_GetMailAttachmentNum(char *AttName)
{
    char *pTmp;
    int i;

    pTmp = AttName;
    i = 0;
    //showname###savename***showname###savename\0
    while(*pTmp != '\0')
    {      
        while (*pTmp != '#') 
        {
            if(*pTmp == '\0')
            {
                return i;
            }
            pTmp ++;
        }
        pTmp += 3;
        i ++;
    }
    return i;
}

int MAIL_GetAllMailAttSize(char *AttAll, int Num)
{
    int AttNum;
    int Size;
    char szSaveFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];

    AttNum = Num;
    Size = 0;
    while(AttNum)
    {
        int tmpSize;
        
        memset(szSaveFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetMailAttachmentSaveName(AttAll, szSaveFileName, AttNum - 1);
        tmpSize = MAIL_GetMailAttachmentSize(szSaveFileName);
        if(tmpSize == -1)
        {
            PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_RECERROR, IDP_MAIL_TITLE_EMAIL, 
                Notify_Info, IDP_MAIL_BOOTEN_OK, NULL, WAITTIMEOUT);
            return Size;
        }
        Size += tmpSize;
        AttNum --;
    }

    return Size;
}

int MAIL_GetMailAttachmentSize(char *AttName)
{
    char szOldPath[PATH_MAXLEN];
    struct stat SourceStat;
    
    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    if(stat(AttName, &SourceStat) == -1)
    {
        chdir(szOldPath);
        return -1;
    }
    chdir(szOldPath);
    
    return SourceStat.st_size;
}

BOOL MAIL_GetMailAttachmentSaveName(char *AttAllName, char *AttName, int num)
{
    char *pTmp;
    int i, j;
    
    pTmp = AttAllName;
    j = num;
    //showname###savename***showname###savename\0
    while(*pTmp != '\0')
    {
        while (*pTmp != '#') 
        {
            if(*pTmp == '\0')
            {
                return FALSE;
            }
            pTmp ++;
        }
        pTmp +=3;
        
        if(j == 0)
        {
            i = 0;
            while((*pTmp != '\0') && (*pTmp != NULL) && (*pTmp != '*'))
            {
                AttName[i] = *pTmp;
                pTmp ++;
                i ++;
            }
            AttName[i] = '\0';
            return TRUE;
        }
        j--;
    }
    return FALSE;
}

BOOL MAIL_GetMailAttachmentShowName(char *AttAllName, char *AttName, int num)
{
    char *pTmp;
    int i, j;
    
    pTmp = AttAllName;
    j = num;
    //showname###savename***showname###savename\0
    while(*pTmp != '\0')
    {
        if(j == 0)
        {
            i = 0;
            while((*pTmp != '\0') && (*pTmp != NULL) && (*pTmp != '#'))
            {
                AttName[i] = *pTmp;
                pTmp ++;
                i ++;
            }
            AttName[i] = '\0';
            return TRUE;
        }
        j--;

        while (*pTmp != '*')
        {
            if(*pTmp == '\0')
            {
                return FALSE;
            }
            pTmp ++;
        }
        pTmp +=3;
    }
    return FALSE;
}

/**************************************************************************** 
* Function : MAIL_GetFileType
*
* Purpose  : get file type according to file postfix
*
\*****************************************************************************/
char*  MAIL_GetFileType(char *FileName)
{
    char  *pTemp;
    int len;
    
    len = strlen(FileName);
    pTemp = FileName + len - 1;
    
    //if step == len, indicate that there is no '.' in the string;
    while(*pTemp != '.')
    {
        if(pTemp == FileName)
            break;
        pTemp --;
    }
    
    if(*pTemp == '.')
    {
        pTemp ++;
        return pTemp;
    }
    
    //abnormal
    return "ABCD";
}

/**************************************************************************** 
* Function: MAIL_IsAttSupported
*
* Purpose  : judge whether the attachment is supported
*
\*****************************************************************************/
BOOL MAIL_IsAttSupported(char *szAffixName, char *tmpType, int *type)
{
    int   i;
    char  *pDest = NULL;
    char  szUpper[100];

    memset(szUpper, 0x0, 100);
    
    pDest = MAIL_GetFileType(szAffixName);
    strcpy(szUpper, pDest);
    
    for (i = 0; i< MAX_MAIL_PIC_ATT_KIND ; i++)
    {
        if (stricmp(MAIL_AFFIX_PIC_EXTNAME[i], szUpper) == 0)
        {
            if(tmpType != NULL)
            {
                strcpy(tmpType, MAIL_AFFIX_PIC_EXTNAME[i]);
            }
            *type = INSERT_PIC;
            return TRUE;
        }
    }
    for (i = 0; i< MAX_MAIL_SOUND_ATT_KIND ; i++)
    {
        if (stricmp(MAIL_AFFIX_RING_EXTNAME[i], szUpper) == 0)
        {
            if(tmpType != NULL)
            {
                strcpy(tmpType, MAIL_AFFIX_RING_EXTNAME[i]);
            }
            *type = INSERT_RING;
            return TRUE;
        }
    }
    for (i = 0; i< MAX_MAIL_NODE_ATT_KIND ; i++)
    {
        if (stricmp("TXT", szUpper) == 0)
        {
            if(tmpType != NULL)
            {
                strcpy(tmpType, "TXT");
            }
            *type = INSERT_NODE;
            return TRUE;
        }
    }
    if (stricmp("vcs", szUpper) == 0)
    {
        if(tmpType != NULL)
        {
            strcpy(tmpType, "vcs");
        }
        *type = INSERT_VCAL;
        return TRUE;
    }
    if (stricmp("vcf", szUpper) == 0)
    {
        if(tmpType != NULL)
        {
            strcpy(tmpType, "vcf");
        }
        *type = INSERT_VCARD;
        return TRUE;
    }
    
    *type = -1;
    return FALSE;
}

void MAIL_GetFileNameFromPath(char *szFullName, char*szFileName)
{
    char *pTemp;
    int len;
    
    len = strlen(szFullName);
    if(len == 0)
    {
        return;
    }

    pTemp = szFullName + len - 1; //move to end
    while(pTemp != szFullName)
    {
        if( *pTemp == '/' || *pTemp == '\\' || *pTemp == ':')
        {
            break;
        }
        pTemp --;
    }
    
    if(pTemp != szFullName)
    {
        pTemp ++;
        //have path
        strcpy(szFileName, pTemp);
    }
    else
    {
        //no path
        strcpy(szFileName, pTemp);
    }

    return;
}

void MAIL_GetNewAffixFileName(char *pNewName)
{
    char	szNewFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];		
    WORD	wTimer;
    
    memset(szNewFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        
    srand(GetTickCount());
    wTimer = rand()&0xfffff;
    sprintf(szNewFileName, "%s%x%s", MAIL_ATTACH_FILE_NAME_HEAD, wTimer, MAIL_ATTA_SAVE_EXTNAME);
    while(1)
    {
        if(MAIL_IsFileExist(szNewFileName) == FALSE)
        {
            break;
        }
        wTimer = rand()&0xfffff;
        memset(szNewFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        sprintf(szNewFileName, "%s%x%s",MAIL_ATTACH_FILE_NAME_HEAD, wTimer, MAIL_ATTA_SAVE_EXTNAME);
    }
    strcpy(pNewName, szNewFileName); 
    return;
}

/******************************************************************** 
* Function	:	MAIL_DeleteAttachment
*	   
* Purpose	:   
*				
*
* Remarks	:  
\**********************************************************************/
BOOL MAIL_DeleteAttachment(char *DelAtt)
{
    char szOutPath[PATH_MAXLEN];    
    int i;
    char TempSavename[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char *pTmp;
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    pTmp = DelAtt;
    //showname###savename***showname###savename\0
    while(*pTmp != '\0')
    {
        while (*pTmp == '*')
        {
            pTmp ++;
        }
        
        while (*pTmp != '#') 
        {
            pTmp ++;
        }
        pTmp +=3;
        
        memset(TempSavename, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        i = 0;
        while((*pTmp != '\0') && (*pTmp != NULL) && (*pTmp != '*'))
        {
            TempSavename[i] = *pTmp;
            pTmp ++;
            i ++;
        }
        TempSavename[i] = '\0';
        if(remove(TempSavename))
        {
            chdir(szOutPath);
            return FALSE;
        }
    }
    chdir(szOutPath);
    
    return TRUE;
}

/******************************************************************** 
* Function	:	MAIL_DeleteFile
*	   
* Purpose	:   
*				
*
* Remarks	:  
\**********************************************************************/
BOOL MAIL_DeleteFile(char *DeleteFile)
{
    int hFile;
    MailFile tmpMailFile;
    char szOutPath[PATH_MAXLEN];
    char *pTempAtt;
    
    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    hFile = GARY_open(DeleteFile, O_RDONLY, -1);
    if(hFile < 0)
    {
        //PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Info, 
        //   IDP_MAIL_BOOTEN_OK, NULL, 20);
        printf("\r\nMAIL_DeleteFile : open error = %d\r\n", errno);

        chdir(szOutPath);
        return FALSE;
    }
    
    memset(&tmpMailFile, 0x0, sizeof(MailFile));
    if(read(hFile, &tmpMailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }

    if(tmpMailFile.AttNum > 0)
    {
        if(lseek(hFile, sizeof(MailFile) + tmpMailFile.TextSize, SEEK_SET) < 0)
        {
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        pTempAtt = (char *)malloc(tmpMailFile.AttFileNameLen + 1);
        if(pTempAtt == NULL)
        {
            GARY_close(hFile);
            chdir(szOutPath);
            return FALSE;
        }
        memset(pTempAtt, 0x0, tmpMailFile.AttFileNameLen + 1);
        read(hFile, pTempAtt, tmpMailFile.AttFileNameLen);

        MAIL_DeleteAttachment(pTempAtt);

        GARY_FREE(pTempAtt);
    }
    GARY_close(hFile);

    if(remove(DeleteFile))
    {
        chdir(szOutPath);
        return FALSE;
    }
    chdir(szOutPath);

    return TRUE;
}

BOOL MAIL_CopyMail(char *szDes, char *szSour)
{
    int hFile;
    int hf;
    MailFile tmpMailFile;
    char szOutPath[PATH_MAXLEN];
    
    char *pTempAtt;
    char *pTempText;
    char *pTmp;
    
    if(MAIL_IsFlashEnough() == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    hFile = GARY_open(szSour, O_RDONLY, -1);
    if(hFile < 0)
    {
        printf("\r\nMAIL_CopyMail1 : open error = %d\r\n", errno);
        
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        chdir(szOutPath);
        return FALSE;
    }
    hf = GARY_open(szDes, O_RDWR | O_CREAT, S_IRWXU);
    if(hf < 0)
    {
        printf("\r\nMAIL_CopyMail2 : open error = %d\r\n", errno);

        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_OPENERROR, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        GARY_close(hFile);
        chdir(szOutPath);
        return FALSE;
    }
    
    memset(&tmpMailFile, 0x0, sizeof(MailFile));
    //copy mail header
    if(read(hFile, &tmpMailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_close(hFile);
        GARY_close(hf);
        chdir(szOutPath);
        return FALSE;
    }

    memset(tmpMailFile.MsgId, 0x0, MAX_MAIL_MESSAGEID_LEN);
    MAIL_GenerateMailID(szDes, tmpMailFile.MsgId);
    if(tmpMailFile.ReadFlag == UNREAD)
    {
        tmpMailFile.ReadFlag = MU_STU_UNREAD;
    }
    else if(tmpMailFile.ReadFlag == READ)
    {
        tmpMailFile.ReadFlag = MU_STU_READ;
    }
    
    //copy mail attachment
    if(tmpMailFile.AttNum > 0)
    {
        char szOldAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
        char szShowFileName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
        char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
        int TempNum;
        int i;
        
        if(lseek(hFile, sizeof(MailFile) + tmpMailFile.TextSize, SEEK_SET) < 0)
        {
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        pTempAtt = (char *)malloc(tmpMailFile.AttFileNameLen + 1);
        if(pTempAtt == NULL)
        {
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        pTmp = (char *)malloc(tmpMailFile.AttFileNameLen + tmpMailFile.AttNum + 1);
        if(pTmp == NULL)
        {
            GARY_FREE(pTempAtt);
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        memset(pTempAtt, 0x0, tmpMailFile.AttFileNameLen + 1);
        memset(pTmp, 0x0, tmpMailFile.AttFileNameLen + tmpMailFile.AttNum + 1);
        read(hFile, pTempAtt, tmpMailFile.AttFileNameLen);

        TempNum = tmpMailFile.AttNum;
        i = 0;
        while(TempNum)
        {
            memset(szOldAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
            MAIL_GetMailAttachmentSaveName((char *)pTempAtt, szOldAttFileName, i);
            memset(szShowFileName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 1);
            MAIL_GetMailAttachmentShowName((char *)pTempAtt, szShowFileName, i);
            memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
            MAIL_GetNewAffixFileName(szNewAttFileName);
            //showname###savename***showname###savename\0
            strcat(pTmp, szShowFileName);
            strcat(pTmp, "###");
            strcat(pTmp, szNewAttFileName);

            MAIL_CopyFile(szOldAttFileName, szNewAttFileName);
            
            TempNum --;
            i ++;
            if(TempNum != 0)
            {
                strcat(pTmp, "***");
            }
        }
        tmpMailFile.AttFileNameLen = strlen(pTmp);
    }
    //write new file
    if(write(hf, &tmpMailFile, sizeof(MailFile)) != sizeof(MailFile))
    {
        GARY_FREE(pTempAtt);
        GARY_FREE(pTmp);
        GARY_close(hFile);
        GARY_close(hf);
        chdir(szOutPath);
        return FALSE;
    }
    //copy mail body(text)
    if(tmpMailFile.TextSize != 0)
    {
        pTempText = (char *)malloc(tmpMailFile.TextSize + 1);
        if(pTempText == NULL)
        {
            GARY_FREE(pTempAtt);
            GARY_FREE(pTmp);
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        memset(pTempText, 0x0, tmpMailFile.TextSize + 1);
        lseek(hFile, sizeof(MailFile), SEEK_SET);
        if(read(hFile, pTempText, tmpMailFile.TextSize) != (int)tmpMailFile.TextSize)
        {
            GARY_FREE(pTempAtt);
            GARY_FREE(pTmp);
            GARY_FREE(pTempText);
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        if(write(hf, pTempText, tmpMailFile.TextSize) != (int)tmpMailFile.TextSize)
        {
            GARY_FREE(pTempAtt);
            GARY_FREE(pTmp);
            GARY_FREE(pTempText);
            GARY_close(hFile);
            GARY_close(hf);
            chdir(szOutPath);
            return FALSE;
        }
        GARY_FREE(pTempText);
    }
    //write attachment
    if(tmpMailFile.AttNum > 0)
    {
        write(hf, pTmp, tmpMailFile.AttFileNameLen);
        GARY_FREE(pTempAtt);
        GARY_FREE(pTmp);
    }

    GARY_close(hFile);
    GARY_close(hf);
    chdir(szOutPath);
    
    return TRUE;
}

BOOL MAIL_CopyFile(char *pSour, char *pDes)
{
    char szOldPath[PATH_MAXLEN];
    BYTE *bCopy;
    int hfSour, hfDes;
    struct stat SourceStat;
    
    if(MAIL_IsFlashEnough() == 0)
    {
        PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_DRAFTFULL, NULL, Notify_Failure, 
            IDP_MAIL_BOOTEN_OK, NULL, 20);
        return FALSE;
    }
    
    szOldPath[0] = 0;
    getcwd(szOldPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);
    
    if(stat(pSour, &SourceStat) == -1)
    {
        chdir(szOldPath);
        return FALSE;
    }
    bCopy = (BYTE *)malloc(SourceStat.st_size);
    if(bCopy == NULL)
    {
        chdir(szOldPath);
        return FALSE;
    }
    hfSour = GARY_open(pSour, O_RDONLY, -1);
    if(hfSour < 0)
    {
        printf("\r\nMAIL_CopyFile1 : open error = %d\r\n", errno);

        chdir(szOldPath);
        return FALSE;
    }
    hfDes = GARY_open(pDes, O_RDWR | O_CREAT, S_IRWXU);
    if(hfDes < 0)
    {
        printf("\r\nMAIL_CopyFile2 : open error = %d\r\n", errno);

        GARY_close(hfSour);
        chdir(szOldPath);
        return FALSE;
    }
    if(read(hfSour, bCopy, SourceStat.st_size) != (int)SourceStat.st_size)
    {
        GARY_FREE(bCopy);
        GARY_close(hfSour);
        GARY_close(hfDes);
        chdir(szOldPath);
        return FALSE;
    }
    GARY_close(hfSour);
    if(write(hfDes, bCopy, SourceStat.st_size) != (int)SourceStat.st_size)
    {
        GARY_FREE(bCopy);
        GARY_close(hfDes);
        chdir(szOldPath);
        return FALSE;
    }
    GARY_FREE(bCopy);
    GARY_close(hfDes);
    chdir(szOldPath);
    return TRUE;
}

BOOL MAIL_CopyAttachment(char *szDes, char *szSour, int Num)
{
    char szOutPath[PATH_MAXLEN];
    char szOldAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char szShowFileName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
    char szNewAttFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    int TempNum;
    int i;

    memset(szOutPath, 0x0, PATH_MAXLEN);
    getcwd(szOutPath, PATH_MAXLEN);
    chdir(MAIL_FILE_PATH);

    TempNum = Num;
    i = 0;
    while(TempNum)
    {
        memset(szOldAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetMailAttachmentSaveName((char *)szSour, szOldAttFileName, i);
        memset(szShowFileName, 0x0, MAX_MAIL_SHOW_FILENAME_LEN + 1);
        MAIL_GetMailAttachmentShowName((char *)szSour, szShowFileName, i);
        memset(szNewAttFileName, 0x0, MAX_MAIL_SAVE_FILENAME_LEN + 1);
        MAIL_GetNewAffixFileName(szNewAttFileName);
        //showname###savename***showname###savename\0
        strcat(szDes, szShowFileName);
        strcat(szDes, "###");
        strcat(szDes, szNewAttFileName);

        if(!MAIL_CopyFile(szOldAttFileName, szNewAttFileName))
        {
            chdir(szOutPath);
            return FALSE;
        }
        
        TempNum --;
        i ++;
        if(TempNum != 0)
        {
            strcat(szDes, "***");
        }
    }
    chdir(szOutPath);
    
    return TRUE;
}

int MAIL_AnalyseMailAddr(char * strAddr)
{
    int		i;
    int		len;
    int		nMailAddrCount;
    
    len = strlen(strAddr);
    if(len == 0)
    {	
        return 0;
    }
    
    nMailAddrCount = 1;
    for(i = 0; i< len; i++)
    {
        if(strAddr[i] == ';')
        {
            nMailAddrCount ++;
        }
        if(strAddr[i] == ',')
        {
            strAddr[i] = ';';
            nMailAddrCount ++;
        }
    }

    if(strAddr[len - 1] == ';')
    {
        nMailAddrCount --;
    }
    
    return nMailAddrCount;    
}

BOOL MAIL_GetMailAddress(char *strAddr, char *MailAddr, int number)
{
    int i, j;
    int nMailAddrCount;
    int len;
    
    len = strlen(strAddr);
    if(len == 0)
    {	
        return FALSE;
    }
    
    nMailAddrCount = 0;
    for(i = 0; i< len; i++)
    {
        if(nMailAddrCount == number)
        {
            break;
        }
        if(strAddr[i] == ';')
        {
            nMailAddrCount ++;
        }
        if(strAddr[i] == ',')
        {
            strAddr[i] = ';';
            nMailAddrCount ++;
        }
    }
    if(i == len)
    {
        return FALSE;
    }
    
    j = 0;
    while((strAddr[i] != ';') && (strAddr[i] != ',') && (strAddr[i] != '\0'))
    {
        MailAddr[j] = strAddr[i];
        i ++;
        j ++;
    }
    if(strlen(MailAddr) == 0)
    {
        return FALSE;
    }

    return TRUE;
}

int MAIL_JudgeEdit()
{
    if(strlen(GlobalMailConfigActi.GLMailBoxName) == 0)
    {
        return -1;
    }
    if(GlobalMailConfigActi.GLConnect == 0)
    {
        return 0;
    }
    if(strcmp(GlobalMailConfigActi.GLMailAddr, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return 0;
    }
    if(strcmp(GlobalMailConfigActi.GLOutServer, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return 0;
    }
    if(strcmp(GlobalMailConfigActi.GLInServer, IDP_MAIL_VALUE_REQUIRED) == 0)
    {
        return 0;
    }
    if(strlen(GlobalMailConfigActi.GLLogin) == 0)
    {
        return 0;
    }
    /*if(strlen(GlobalMailConfigActi.GLPassword) == 0)
    {
        return 0;
    }*/

    return 1;
}

BOOL MAIL_ConvertPath(char *Des, char *Sour, int type)
{
    if(type == FLASH_COLON_TO_MNT)
    {
        if(!strstr(Sour, MAIL_PATH_FLASH_COLON))
        {
            return FALSE;
        }

        sprintf(Des, "%s%s", MAIL_PATH_FLASH_MNT, (Sour + 8));
        return TRUE;
    }
    else if(type == FLASH_MNT_TO_COLON)
    {
        if(!strstr(Sour, MAIL_PATH_FLASH_MNT))
        {
            return FALSE;
        }
        
        sprintf(Des, "%s%s", MAIL_PATH_FLASH_COLON, (Sour + 11));
        return TRUE;
    }
    else if(type == RAM_COLON_TO_MNT)
    {
        if(!strstr(Sour, MAIL_PATH_RAM_COLON))
        {
            return FALSE;
        }
        
        sprintf(Des, "%s%s", MAIL_PATH_RAM_MNT, (Sour + 4));
        return TRUE;
    }
    else if(type == RAM_MNT_TO_COLON)
    {
        if(!strstr(Sour, MAIL_PATH_RAM_MNT))
        {
            return FALSE;
        }
        
        sprintf(Des, "%s%s", MAIL_PATH_RAM_COLON, (Sour + 9));
        return TRUE;
    }
    return FALSE;
}

BOOL MAIL_CombineReplyRec(char **ReturnName, char *WholeName, char *IndiName)
{
    char *FirstState;
    char *SecondState;
    int len;

    FirstState = NULL;
    FirstState = strstr(WholeName, IndiName);
    if(FirstState == NULL)
    {
        len = strlen(WholeName);
        *ReturnName = malloc(len + 1);
        if(*ReturnName == NULL)
        {
            return FALSE;
        }
        memset(*ReturnName, 0x0, len + 1);
        strcpy(*ReturnName, WholeName);
        return TRUE;
    }

    SecondState = FirstState + strlen(IndiName);
    if(*SecondState == ';')
    {
        SecondState ++;
    }

    len = strlen(WholeName) - strlen(IndiName);
    *ReturnName = malloc(len + 1);
    if(*ReturnName == NULL)
    {
        return FALSE;
    }
    memset(*ReturnName, 0x0, len + 1);
    if(FirstState == WholeName)
    {
        if(*SecondState != '\0')
        {
            strcpy(*ReturnName, SecondState);
        }
        return TRUE;
    }
    
    strncpy(*ReturnName, WholeName, FirstState - WholeName);
    if(*SecondState != '\0')
    {
        strcat(*ReturnName, SecondState);
    }

    return TRUE;
}

void MAIL_CommaToSemicolon(char *WholeName)
{
    char *chComma;

    chComma = strstr(WholeName, ",");
    while(chComma)
    {
        *chComma = ';';
        chComma = NULL;
        chComma = strstr(WholeName, ",");
    }
    
    return;
}

void MAIL_InitConfigNode(MailConfigHeader *pHead, MailConfigNode *pNode, MailConfig *pFile)
{
    strcpy(pNode->GLMailBoxName, pFile->GLMailBoxName);
    pNode->GLConnect = pFile->GLConnect;
    strcpy(pNode->GLMailAddr, pFile->GLMailAddr);
    strcpy(pNode->GLOutServer, pFile->GLOutServer);
    pNode->GLSendMsg = pFile->GLSendMsg;
    pNode->GLCC = pFile->GLCC;
    strcpy(pNode->GLSignatrue, pFile->GLSignatrue);
    strcpy(pNode->GLLogin, pFile->GLLogin);
    strcpy(pNode->GLPassword, pFile->GLPassword);
    strcpy(pNode->GLInServer, pFile->GLInServer);
    pNode->GLBoxType = pFile->GLBoxType;
    pNode->GLSecurity = pFile->GLSecurity;
    pNode->GLRetrAtt = pFile->GLRetrAtt;
    pNode->GLRetrHead = pFile->GLRetrHead;
    strcpy(pNode->GLMailConfigSaveName, pFile->GLConfigSaveName);
    pNode->GLDefSelect = pFile->GLDefSelect;

    if(pFile->GLAutoRetrieval == MAIL_AUTORET_OFF)
    {
        pNode->GLAutoRetrieval = -1;
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_10MIN)
    {
        pNode->GLAutoRetrieval = 10;// * 60
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_30MIN)
    {
        pNode->GLAutoRetrieval = 30;// * 60
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_1H)
    {
        pNode->GLAutoRetrieval = 1 * 60;
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_2H)
    {
        pNode->GLAutoRetrieval = 2 * 60;
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_6H)
    {
        pNode->GLAutoRetrieval = 6 * 60;
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_12H)
    {
        pNode->GLAutoRetrieval = 12 * 60;
    }
    else if(pFile->GLAutoRetrieval == MAIL_AUTORET_24H)
    {
        pNode->GLAutoRetrieval = 24 * 60;
    }
    else
    {
        pNode->GLAutoRetrieval = -1;
    }

    pNode->GLConStatus = MAILBOX_CON_IDLE;
    pNode->GLConType = MAILBOX_CON_NONE;
    pNode->GLTimer = FALSE;

    if((pNode->GLAutoRetrieval != -1) && (GetSIMState() == 1))
    {
        SYSTEMTIME sy;
        int nDay;
        RTCTIME AlmTime;
        
        pNode->GLTimer = TRUE;

        GetLocalTime(&sy);
        sy.wMinute += pNode->GLAutoRetrieval;

        nDay = 0;
        while((int)sy.wMinute > 59)
        {
            sy.wHour ++;
            sy.wMinute -= 60;
        }
        while((int)sy.wHour > 23)
        {
            nDay ++;
            sy.wHour -= 24;
        }
        MAIL_DayIncrease(&sy, nDay);

        memset(&AlmTime, 0x0, sizeof(RTCTIME));
      
        AlmTime.v_nYear		= (u_INT2)sy.wYear;
        AlmTime.v_nMonth	= (u_INT1)sy.wMonth;
        AlmTime.v_nDay		= (u_INT1)sy.wDay;
        AlmTime.v_nHour		= (u_INT1)sy.wHour;
        AlmTime.v_nMinute	= (u_INT1)sy.wMinute;

        RTC_SetAlarms("Edit Email", (u_INT2)pNode->GLMailBoxID, ALMF_REPLACE | ALMF_RELATIVE, "Edit Email", &AlmTime);
        //SetRTCTimer(HwndMailGet, pNode->GLMailBoxID, pNode->GLAutoRetrieval * 1000, NULL);
    }
    return;
}

BOOL MAIL_AddDialWaitNode(MailConfigHeader *pHead, MailConfigNode *pNode, BYTE WaitType)
{
    MailConWaitNode *pTemp;
    MailConWaitNode *pNewNode;

    pNewNode = NULL;
    pNewNode = malloc(sizeof(MailConWaitNode));
    if(pNewNode == NULL)
    {
        return FALSE;
    }
    memset(pNewNode, 0x0, sizeof(MailConWaitNode));
    pNewNode->pNext = NULL;
    pNewNode->GLMailBoxID = pNode->GLMailBoxID;
    pNewNode->GLConType = WaitType;
    pNewNode->GLConnect = pNode->GLConnect;
    strcpy(pNewNode->GLMailBoxName, pNode->GLMailBoxName);

    //new node should add to pPreTemp->pNext
    pTemp = pHead->pWaitNext;
    if(pTemp == NULL)
    {
        //new node is the first node in list
        pHead->pWaitNext = pNewNode;        
        return TRUE;
    }

    while(pTemp->pNext != NULL)
    {
        if(pTemp->GLMailBoxID == pNewNode->GLMailBoxID)
        {
            if(pTemp->GLConType == pNewNode->GLConType)
            {
                return FALSE;
            }
            else if((pTemp->GLConType != MAIL_CON_TYPE_SEND) 
                && (pNewNode->GLConType != MAIL_CON_TYPE_SEND))
            {
                return FALSE;
            }
        }
        pTemp = pTemp->pNext;
    }

    pTemp->pNext = pNewNode;
    
    return TRUE;
}

void MAIL_DelDialWaitNode(MailConfigHeader *pHead, MailConWaitNode *pNode)
{
    MailConWaitNode *pCurTemp;
    MailConWaitNode *pNextTemp;

    pCurTemp = pHead->pWaitNext;
    if(pCurTemp == NULL)
    {   
        return;
    }

    if(pCurTemp->GLMailBoxID == pNode->GLMailBoxID)
    {
        pHead->pWaitNext = pCurTemp->pNext;
        GARY_FREE(pCurTemp);

        return;
    }
    
    pNextTemp = pCurTemp->pNext;
    while (pNextTemp != NULL)
    {
        if(pNextTemp->GLMailBoxID == pNode->GLMailBoxID)
        {
            pCurTemp->pNext = pNextTemp->pNext;
            GARY_FREE(pNextTemp);

            return;
        }
        pCurTemp = pNextTemp;
        pNextTemp = pCurTemp->pNext;
    }
    
    return;
}

void MAIL_DealNoReturnConnect()
{
    MailConWaitNode *pTemp;
    
    //GlobalMailConfigHead.GLNetUsed = FALSE;
    //GlobalMailConfigHead.GLNetFlag = 0;
    
    if(GetSIMState() == 0)
    {
        SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
        return;
    }

    if(GlobalMailConfigHead.pWaitNext != NULL)
    {
        GlobalMailConfigHead.GLNetUsed = FALSE;
        GlobalMailConfigHead.GLNetFlag = 0;

        //send next node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SEND)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SEND;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            break;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                    
                    if(!GlobalMailConfigHead.CurConnectConfig)
                    {
                        SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                        return;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                    return;
                }
            }
            pTemp = pTemp->pNext;
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            SendMessage(HwndMailSend, WM_MAIL_BEGIN_SEND, 0, 0);
            return;
        }
        
        //get show node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SHOWREC)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SHOWREC;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            break;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);

                    if(!GlobalMailConfigHead.CurConnectConfig)
                    {
                        SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                        return;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                    return;
                }
            }
            pTemp = pTemp->pNext;
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            MAIL_ConnectServer(NULL, MAIL_GET_CONNECT);
            return;
        }
        
        //hide get node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_HIDEREC)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            break;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                    
                    if(!GlobalMailConfigHead.CurConnectConfig)
                    {
                        SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                        return;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
                    return;
                }
            }
            pTemp = pTemp->pNext;
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            MAIL_ConnectServer(NULL, MAIL_GET_CONNECT);
            return;
        }
    }
    SendMessage(HwndMailDial, WM_MAIL_DIAL_HANDUP, MAIL_DIAL_HANDUP_NOW, 0);
    return;
}

BOOL MAIL_DealReturnConnect()
{
    MailConWaitNode *pTemp;
    
    if(GlobalMailConfigHead.pWaitNext != NULL)
    {
        GlobalMailConfigHead.GLNetUsed = FALSE;
        GlobalMailConfigHead.GLNetFlag = 0;

        //send next node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SEND)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SEND;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                            return FALSE;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    return TRUE;
                }
                else
                {
                    return TRUE;
                }
            }
            pTemp = pTemp->pNext;
        }
        
        //get show node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SHOWREC)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SHOWREC;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                            return FALSE;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    return TRUE;
                }
                else
                {
                    return TRUE;
                }
            }
            pTemp = pTemp->pNext;
        }
        
        //hide get node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_HIDEREC)
            {
                if(pTemp->GLConnect == GlobalMailConfigHead.GLConnect)
                {
                    GlobalMailConfigHead.GLNetUsed = TRUE;
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                    strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                    while(GlobalMailConfigHead.CurConnectConfig)
                    {
                        if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                        {
                            MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                            return FALSE;
                        }
                        GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                    }
                    return TRUE;
                }
                else
                {
                    return TRUE;
                }
            }
            pTemp = pTemp->pNext;
        }
    }
    return TRUE;
}

void MAIL_DealNextConnect()
{
    MailConWaitNode *pTemp;

    GlobalMailConfigHead.GLNetUsed = FALSE;
    GlobalMailConfigHead.GLNetFlag = 0;
    memset(GlobalMailConfigHead.GLMailBoxName, 0x0, MAX_MAIL_BOXNAME_LEN + 1);
    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
    GlobalMailConfigHead.GLConnect = 0;
    GlobalMailConfigHead.CurConnectConfig = NULL;

    if(GetSIMState() == 0)
    {
        return;
    }

    if(GlobalMailConfigHead.pWaitNext != NULL)
    {
        //send next node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SEND)
            {
                GlobalMailConfigHead.GLNetUsed = TRUE;
                GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SEND;
                strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                GlobalMailConfigHead.GLConnect = pTemp->GLConnect;
                
                GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                while(GlobalMailConfigHead.CurConnectConfig)
                {
                    if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                    {
                        MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                        pTemp = NULL;

                        break;
                    }
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                }
            }
            if(pTemp != NULL)
            {
                pTemp = pTemp->pNext;
            }
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            SendMessage(HwndMailSend, WM_MAIL_BEGIN_SEND, 0, 0);
            return;
        }
        
        //get show node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_SHOWREC)
            {
                GlobalMailConfigHead.GLNetUsed = TRUE;
                GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SHOWREC;
                strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                GlobalMailConfigHead.GLConnect = pTemp->GLConnect;
                
                GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                while(GlobalMailConfigHead.CurConnectConfig)
                {
                    if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                    {
                        MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                        pTemp = NULL;

                        break;
                    }
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                }
            }
            if(pTemp != NULL)
            {
                pTemp = pTemp->pNext;
            }
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            MAIL_ConnectServer(NULL, MAIL_GET_CONNECT);
            return;
        }
        
        //hide get node
        pTemp = GlobalMailConfigHead.pWaitNext;
        while(pTemp)
        {
            if(pTemp->GLConType & MAIL_CON_TYPE_HIDEREC)
            {
                GlobalMailConfigHead.GLNetUsed = TRUE;
                GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                strcpy(GlobalMailConfigHead.GLMailBoxName, pTemp->GLMailBoxName);
                GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                GlobalMailConfigHead.GLConnect = pTemp->GLConnect;
                
                GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.pNext;
                while(GlobalMailConfigHead.CurConnectConfig)
                {
                    if(GlobalMailConfigHead.CurConnectConfig->GLMailBoxID == pTemp->GLMailBoxID)
                    {
                        MAIL_DelDialWaitNode(&GlobalMailConfigHead, pTemp);
                        pTemp = NULL;

                        break;
                    }
                    GlobalMailConfigHead.CurConnectConfig = GlobalMailConfigHead.CurConnectConfig->pNext;
                }
            }
            if(pTemp != NULL)
            {
                pTemp = pTemp->pNext;
            }
        }
        if(GlobalMailConfigHead.GLNetUsed)
        {
            MAIL_ConnectServer(NULL, MAIL_GET_CONNECT);
            return;
        }
    }
    return;
}

void MAIL_TruncateRec(char *chDes, char *chSour)
{
    char *chStart;
    char *chEnd;

    chStart = NULL;
    chStart = strstr(chSour, "<");
    chEnd = NULL;
    chEnd = strstr(chSour, ">");
    if(chStart && chEnd)
    {
        strncpy(chDes, chStart + 1, (chEnd - chStart - 1));
        if((chEnd + 1) != "\0")
        {
            strcat(chDes, chEnd + 1);
        }
    }
    else
    {
        strcpy(chDes, chSour);
    }

    return;
}

BOOL MAIL_PlayAudio(HWND hWnd, char *audioFileName, int audioType, 
                    const char *audioBody, int audioSize)
{
    PM_PlayMusic pm;
    
    /*switch(audioType) 
    {
    case AUDIO_AMR: 
        pm.nRingType = PRIOMAN_RINGTYPE_AMR;
        break;
        
    case AUDIO_MIDI:
        pm.nRingType = PRIOMAN_RINGTYPE_MIDI;
        break;
        
    case AUDIO_WAV:
        pm.nRingType = PRIOMAN_RINGTYPE_WAVE;
        break;
        
    case AUDIO_MMF:
        pm.nRingType = PRIOMAN_RINGTYPE_MMF;
        break;
    }*/
    memset(&pm, 0x0, sizeof(PM_PlayMusic));

    pm.hCallWnd = hWnd;
    pm.nPriority = PRIOMAN_PRIORITY_EMAIL;
    pm.nVolume = PRIOMAN_VOLUME_DEFAULT;
    pm.nRingType = 1;
    pm.nRepeat = 1;
    pm.pMusicName = audioFileName;
    pm.pDataBuf = NULL;
    pm.nDataLen = 0;
    
    if (PrioMan_PlayMusic(&pm) != PRIOMAN_ERROR_SUCCESS)
        return FALSE;
    
    return TRUE;
}

/******************************************************************** 
* Function	   MAIL_StopPlay
* Purpose      stop a ring
**********************************************************************/

void MAIL_StopPlay(void)
{
    PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_EMAIL);
}

int GARY_open(const char *filename, int FirstArg, int SecondArg)
{
    int hFile;

    if (SecondArg == -1)
    {
        hFile = open(filename, FirstArg);
    }
    else
    {
        hFile = open(filename, FirstArg, SecondArg);
    }

    if(hFile >= 0)
        GlobalMailOpen ++;

    printf("GARY_MAIL:GlobalMailOpen = %d, handle = %d\r\n", GlobalMailOpen, hFile);

    return hFile;
}

int GARY_close(int hFile)
{
    GlobalMailOpen --;

    printf("GARY_MAIL:GlobalMailClose = %d, handle = %d\r\n", GlobalMailOpen, hFile);
    return close(hFile);
}

DIR* GARY_opendir(const char* szPath)
{
    GlobalMailOpendir ++;
    
    printf("GARY_MAIL:1GlobalMailOpendir = %d\r\n", GlobalMailOpendir);
    return opendir(szPath);
}

int GARY_closedir(DIR* dir)
{
    GlobalMailOpendir --;

    printf("GARY_MAIL:GlobalMailClosedir = %d\r\n", GlobalMailOpendir);
    return closedir(dir);
}

BOOL MAIL_DayIncrease(SYSTEMTIME *psystime, unsigned int day)
{
    int outday;
    WORD	   wOldYear;
    WORD       wOldMonth;
    WORD	   wOldDay;
    
    //save date
    wOldYear  = psystime->wYear;
    wOldMonth = psystime->wMonth;
    wOldDay   = psystime->wDay;
    
    psystime->wDay += day;
    outday = MAIL_AlmDaysOfMonth(psystime);
    while(psystime->wDay > outday)
    {
        psystime->wMonth ++;
        psystime->wDay -= outday;
        
        if (psystime->wMonth > 12)
        {
            psystime->wYear ++;
            psystime->wMonth = 1;
            
            if (psystime->wYear > MAX_YEAR)
            {
                psystime->wYear  = wOldYear;
                psystime->wMonth = wOldMonth;
                psystime->wDay   = wOldDay;
                return FALSE;
            }
        }
        outday = MAIL_AlmDaysOfMonth(psystime);
    }
    return TRUE;
}

int MAIL_AlmDaysOfMonth(SYSTEMTIME *pDate)
{
    int nDaysOfMonth;
    
    switch (pDate->wMonth)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:	// 1、3、5、7、8、10、12 = 31
        nDaysOfMonth = 31;
        break;
        
    case 2:      
        if (MAIL_IsLeapYear(pDate->wYear))
        {
            nDaysOfMonth = 29;
        }
        else
        {
            nDaysOfMonth = 28;
        }        
        break;
        
    default:       
        nDaysOfMonth = 30;
        break;
    }   
    return (nDaysOfMonth);
}

BOOL MAIL_IsLeapYear(WORD wYear)
{
    if ((wYear % 4 == 0 && wYear % 100 != 0) || (wYear % 400 == 0))
        return TRUE;
    else
        return FALSE;
}


