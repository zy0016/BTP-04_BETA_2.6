 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mms
 *
 * Purpose  : define some functions in common
 *            
\**************************************************************************/

#include "MmsGlobal.h"
#include "MmsInterface.h"
#include "time.h"

#include "MmsDisplay.h"
#include "vcaglb.h"
#include "MmsUi.h"
#include "PWBE.h"
#include "vcardvcal.h"

#define ISDIGIT(c) \
		(c <= '9' && c >= '0')

#define ISCAPLETTER(c) \
		(c <= 'Z' && c >= 'A')

#define ISLETTER(c) \
		(c <= 'z' && c >= 'a')

extern BOOL 	MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);
extern void		MMSC_ModifyFolderUnread(int nFolder, int sigh, int nCount);
extern LRESULT MMSEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
extern LRESULT MMSSetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
extern LRESULT MMSDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
extern BOOL MMS_SaveObject(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen);

const MmsWndClass MMSWindowClass[MAX_CLASS_NUM] = 
{
	{
		NULL,
		NULL
	},
	{
		NULL,	//MMSListWndProc,					//1:list
		NULL,	//"MMSListClass"
	},
	{
		NULL,	// MMSPhraseWndProc,				//2:phrase
		NULL	// "MMSPhraseClass"
	},
	{
		MMSSetWndProc,								//3:setting
		"MMSSetClass"
	},

    {
		NULL,	//MMSImageWndProc,
		NULL	//"MMSImageclass"					//4:image list
	},
	{
		MMSEditWndProc,								//5:edit
		"MMSEditClass"
	},
	{
		MMSDisplayWndProc,
		"MMSDisplayclass"							//6:display
	},
    {
		NULL,	// MMSReportWndProc,
		NULL	// "MMSReportclass"					//7:report
	},
};

static char *strWeekDay[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Err"
};

static char *strMonth[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Err"
};

/*
**  Function : CreateFileName
**  Purpose  :
			create a filename , this function should ensure the file name is not existed in 
			this dir
**
**  Params   :
**      pszFileName : the file name created
*/

void MMS_CreateFileName(PSTR pszFileName, const int nFileType, const int nFolder)
{
    static BOOL bFirst = TRUE;
//	char pFilePathName[MAX_PATH_LEN];
    int         rand1, rand2;
	DIR *  dirtemp = NULL;
	struct dirent *dirinfo = NULL;
	int		bResend = 0;
   

    if (bFirst)
    {
        srand(GetTickCount());
        bFirst = FALSE;
    }

	chdir(FLASHPATH);
START:
	dirtemp = opendir(FLASHPATH);

	if(dirtemp == NULL)
		return;

	rand1 = rand() % 0xFFF;
	rand2 = rand() % 0xFFF;
	
	sprintf(pszFileName, "$%c%04d%d%03x%03x.ms$", nFileType, nFolder, bResend, rand1, rand2);
	
//	memset(pFilePathName, 0x0, MAX_PATH_LEN);
//	sprintf(pFilePathName, "%s%s", MMS_FILEPATH, pszFileName);
     
    while (dirinfo = readdir(dirtemp))
    { 
		if(dirinfo && dirinfo->d_name[0] && stricmp(dirinfo->d_name, pszFileName) == 0)
		{
			closedir(dirtemp);
			srand(GetTickCount());
			goto START;
		}			
    } 
	
	closedir(dirtemp);

/*    if (nFileType != TYPE_READREPLY)
	{
        MMSC_ModifyTotal(COUNT_ADD,0);
		MMSC_ModifyMsgCount(nFolder, COUNT_ADD, 0);
}*/
	if (nFileType != TYPE_READREPLY && nFileType != TYPE_FIRSTAOTORECV)
		MMSC_ModifyTotal(COUNT_ADD, 0);
	
	if(nFileType != TYPE_FIRSTAOTORECV)
		MMSC_ModifyMsgCount(nFolder, COUNT_ADD, 0);
}

/*********************************************************************\
* Function     MMS_CreateFile
* Purpose      open file
* Params       filename,dwmode, dwattrib
* Return       handle
**********************************************************************/
int MMS_CreateFile(PCSTR pFileName, DWORD dwMode)
{
    return open(pFileName, dwMode, S_IRWXU);
}
/*********************************************************************\
* Function     MMS_CloseFile
* Purpose      close file
* Params       handle
* Return       success or failure
**********************************************************************/
BOOL MMS_CloseFile(int hFile)
{
    return close(hFile);
}

/******************************************************************** 
* Function	   MMS_DeleteFile
* Purpose      delete a mms file and modify the total number
* Params	   filename(full path)
* Return	   success or failure
**********************************************************************/
BOOL MMS_DeleteFile(PCSTR filename)
{
    BOOL bRet;
    int  nType,nFolder;

	nType = filename[MMS_TYPE_POS];
	nFolder = MMS_GetFileFolder(filename);
		
    bRet = unlink(filename);
    
    if (bRet == 0 && nType != TYPE_READREPLY)
    {
        MMSC_ModifyTotal(COUNT_DEC, 0);
		MMSC_ModifyMsgCount(nFolder, COUNT_DEC, 0);
        if (nType == TYPE_UNRECV)
            MMSC_ModifyUnreceive(COUNT_DEC, 0);
        if (nType == TYPE_UNREAD)
		{
            MMSC_ModifyUnread(COUNT_DEC, 0);
			MMSC_ModifyFolderUnread(nFolder,COUNT_DEC, 0);
		}
    }

    return (bRet == 0);
}

/******************************************************************** 
        judge if there is available space to save mms
		if available then return true, else return false
**********************************************************************/
BOOL MMS_IsFlashEnough(void)
{
    int nRet;
	
    nRet = IsFlashAvailable(MIN_FREE_SPACE);

	if (nRet != SPACE_AVAILABLE)
	{
		MsgWin(NULL, NULL, 0, STR_MMS_NEGH, STR_MMS_MMS, Notify_Alert,
			STR_MMS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	return TRUE;
}

/******************************************************************** 
* Function	   rename a mms
* Params	   srcname:	the source file name
			   ctype:   the dest type
* Return	   true or false
**********************************************************************/
BOOL RenameMms(char *srcname, const int nFileType, const int nFolder)
{
    char	newname[MAX_FILENAME];
	char    szSource[MAX_PATH_LEN];
	char    szDest[MAX_PATH_LEN];
    int     folderId, nType,i,nOldFolder;
	DWORD   pHandle;

	if(nFileType == -1 && nFolder == -1)
		return TRUE;

	pHandle = GetHandleByName(srcname);

	nType = MMS_GetFileType(srcname);
    folderId =  MMS_GetFileFolder(srcname);

    if ((nFileType != -1 &&  nType == nFileType) 
		&& (nFolder != -1 && folderId == nFolder))
        return TRUE;

	for(i= 6; i < (int)strlen(srcname); i++)
	{
		newname[i -6] = srcname[i];
	}
	newname[strlen(srcname) -6] = 0;

	if(nFileType != -1)
		nType = nFileType;

	if(nFolder != -1)
	{
		nOldFolder = folderId;
		folderId = nFolder;
	}
	
	memset(szSource,0x0,MAX_PATH_LEN);
	memset(szDest, 0x0, MAX_PATH_LEN);

	sprintf(szSource, "%s%s", MMS_FILEPATH, srcname);
	
	sprintf(szDest, "%s$%c%04d%s", MMS_FILEPATH, nType,
	  folderId, newname);
	
    if (MMS_MoveFile(szSource, szDest) == FALSE)
	{
		return FALSE;
	}

	//strcpy(srcname, szDest);
	sprintf(srcname, "$%c%04d%s", nType, folderId, newname);

	if(nFolder !=-1)
	{
		MMSC_ModifyMsgCount(nOldFolder, COUNT_DEC, 0);
		MMSC_ModifyMsgCount(nFolder, COUNT_ADD, 0);
//		if(nType == TYPE_DRAFT)
//		{
//			MMSC_ModifyFolderUnread(nOldFolder, COUNT_DEC, 0);
//			MMSC_ModifyFolderUnread(nFolder, COUNT_ADD, 0);
//		}
	}
	if (pHandle)
		ModifyMsgNode((DWORD)pHandle, srcname, -1);
	return TRUE;
}

/*
 get the suffix of file name, if sucess then return >= 0,else return -1
 */
int MMS_GetFileSuffix(PCSTR pFileName, PSTR pSuffix)
{
	int i, len;

	len = strlen(pFileName);

	for (i = len - 1; i >= 0; i --)
	{
		if (pFileName[i] == '.' && i != len - 1)
		{
			strcpy(pSuffix, &pFileName[i]);
			return i;
		}
	}
	return 0;
}

int MMS_GetNameWithoutSuffix(PCSTR pFull, PSTR pName)
{
	int i, len;

	len = strlen(pFull);

	for (i = len - 1; i >= 0; i --)
	{
		if (pFull[i] == '.' && i != len - 1)
		{
			strncpy(pName, pFull, i);
			pName[i] = 0;
			return i;
		}
	}
	return 0;
}
/******************************************************************** 
*
* Function	   get the file type according to file name
* Params	   pFileName：file name
* Return	   Type
*
**********************************************************************/
int MMS_GetFileType(const char *pFileName)
{
   int     type;
 
   type = pFileName[MMS_TYPE_POS];

/*
          // change the case
          if (type >= 0x61 && type <= 0x7A)
              type -= 0x20;  */
        
   
   return type;
}
/******************************************************************** 
*              MMS_GetFileFolder
* Function	   get the folder id of a mms according to file name
* Params	   pFileName
* Return	   folderid
*
**********************************************************************/
int MMS_GetFileFolder(const char *pFileName)
{
   int     nFolder =  -1, i;
   char    szFolder[MAX_FOLDERNUM + 1];

   szFolder[0] = '\0';

   for(i = 0; i<MAX_FOLDERNUM;i++)
   {
	   szFolder[i] = pFileName[MMS_FOLDER_POS + i];
   }

   szFolder[MAX_FOLDERNUM] = 0;

   nFolder = atoi(szFolder);
         
   return nFolder;
}

/******************************************************************** 
  * Function	   get the mmft type acording to file name
    * Params	   pFile：file name
   * Return	       mmft type
 **********************************************************************/
int MMS_GetFileMmft(const char *pFileName)
{
   int     i;
   int     type;
   const int szFileType[] = {TYPE_SENDED, TYPE_UNSEND, TYPE_READED,TYPE_UNREAD, TYPE_DRAFT, 
							TYPE_UNRECV, TYPE_DELAYRECV, TYPE_READREPLY, TYPE_WAITINGSEND, TYPE_SENDING,
							TYPE_RECVING, TYPE_REPORTED, TYPE_REPORTING, TYPE_UNREPORT, TYPE_DEFERMENT,TYPE_FIRSTAOTORECV,TYPE_DELAYSEND };

    type = pFileName[MMS_TYPE_POS];

    for (i = 0; i < FILE_TYPE_NUM; i++)
    {
/*
        // change the case
        if (type >= 0x61 && type <= 0x7A)
            type -= 0x20;*/

        if(type == szFileType[i])
            return MMFT_SENDED << i;
    }

    return -1;
}

/******************************************************************** 
* Function	   MMS_ProcessListBoxScroll
* Purpose      deal with the arrow of ListBox
* Params	   hParent: the parent window handle of ListBox
               hList:   the handle of ListBox
               nItemNum：the items of ListBox
* Return	 	   void
**********************************************************************/
void MMS_ProcessListBoxScroll(HWND hParent, HWND hList, int nItemNum)
{
	int index;

	SendMessage(hParent, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN, MASKUP|MASKDOWN);
	index = SendMessage(hList, LB_GETCURSEL, 0, 0);
	
	if (nItemNum == 0 || nItemNum == 1)
		SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP|MASKDOWN);
    else
    {
        if (index == -1)
		{
            SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP|MASKDOWN);
		}
        
        if (index == nItemNum - 1)
		{
            SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
		}

        if (index == 0)
		{
            SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP);
		}
    }
	return ;
}
/*
 *	judge wether this charactor is chinese
 */
BOOL MMS_IsChinese(const BYTE c)
{
    if (c >= 0xa0 && c <= 0xfe)
        return TRUE;
    else
        return FALSE;
}

/**************************************************************************** 
* Funcation: EditMMSTilte
*
* Purpose  : trim the title
*
* Explain  :         
*           
\*****************************************************************************/
#ifndef MMS_18030
void MMS_EditTitle(char* pszTitle, int nLen)
{
	int nWordNum;
    char *tmp;
	
	nWordNum = 0;
    tmp = pszTitle;

	while(*tmp)
	{
		if (MMS_IsChinese(*tmp))
		{
			nWordNum = nWordNum+2;
			tmp++;
		}
		else
			nWordNum = nWordNum+1;

        tmp++;
	}

	if (nWordNum > nLen)
    {
		pszTitle[nLen] = '\0';
        pszTitle[nLen - 1] = '.';
        pszTitle[nLen - 2] = '.';
        pszTitle[nLen - 3] = '.';
    }
    else
        pszTitle[nLen] = 0;
	
}
#else
void MMS_EditTitle(char* pszTitle, int nLen)
{
	int len, nTitlelen;
    int nFit;

    len = nLen - 3;

    nTitlelen = strlen(pszTitle);

    GetTextExtentExPoint(NULL, pszTitle, nTitlelen, len * ENG_FONT_WIDTH, 
        &nFit, NULL, NULL);

	if (nFit < nTitlelen)
    {
        pszTitle[nFit] = '.';
        pszTitle[nFit + 1] = '.';
        pszTitle[nFit + 2] = '.';
		pszTitle[nFit + 3] = '\0';
    }
    else
        pszTitle[nFit] = 0;
	
}
#endif

/******************************************************************** 
* Function	   PhonenumToNameTitle
* Purpose      find the name in contacts of a number
* Params	   strPhonenum:the phone number
               strName:the name in contacts
               nLen:the length of list
* Return	   the actual length
**********************************************************************/
int PhonenumToNameTitle(char *strPhonenum, char *strName, int nLen)
{
    char	*addTokens;
    char    *dupPhonenum;
    int     phonelen;
    char    name[AB_MAXLEN_NAME];
    int     len = 0;
    int     oldLen;
    BOOL    bFind;

    name[0] = '\0';
    strName[0] = '\0';

    phonelen = strlen(strPhonenum);
    dupPhonenum = MMS_malloc(phonelen + 1);
    strcpy(dupPhonenum, strPhonenum);

    addTokens = MMS_chrtok(dupPhonenum, MMS_ASEPCHR, MMS_ASEPCHRF);
    
    while(addTokens != NULL)
    {
        if (*addTokens == '\0')
        {
            addTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
            continue;
        }
        bFind = MMS_FindNameInPB(addTokens, name, ADDR_UNKNOWN);

        if (bFind)
        {
            oldLen = len;
            len += strlen(name);
            if (len >= nLen)
            {
                name[nLen - oldLen - 1] = '\0';
                name[nLen - oldLen - 2] = '.';
                name[nLen - oldLen - 3] = '.';
                name[nLen - oldLen - 4] = '.';
                len = nLen;
                strcat(strName, name);
                addTokens = NULL;
            }
            else
            {
                strcat(strName, name);
                strcat(strName, MMS_ASEPSTR);
                len ++;
            }
        }
        else
        {
            oldLen = len;
            len += strlen(addTokens);
            if (len >= nLen)
            {
                addTokens[nLen - oldLen - 1] = '\0';
                addTokens[nLen - oldLen - 2] = '.';
                addTokens[nLen - oldLen - 3] = '.';
                addTokens[nLen - oldLen - 4] = '.';
                len = nLen;   
                strcat(strName, addTokens);
                addTokens = NULL;
            }
            else
            {
                strcat(strName, addTokens);
                strcat(strName, MMS_ASEPSTR);
                len ++;
            }
        }
        if (addTokens != NULL)
            addTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
    }
    
    if (len > 0)
        strName[len - 1] = '\0';

    MMS_free(dupPhonenum);
    return len;
}

/*********************************************************************\
* Function	   SysTimeToStr
* Purpose      change the system time to string
                "Tue, 25 Mar 2003 03:27:23 GMT"
* Params	   sysTime: system time
			   strTime: string
* Return	 	the length of string after changed
**********************************************************************/
int MMS_SysTimeToStr(SYSTEMTIME sysTime, char *strTime)
{
    int strTimeLen = 0;

    if (strTime == NULL)
        return -1;

   // assert(sysTime.wDayOfWeek < 8 && sysTime.wMonth < 13);//xlzhu

    if(sysTime.wDayOfWeek >= 8)
        sysTime.wDayOfWeek = 8;
    if(sysTime.wMonth >= 13)
        sysTime.wMonth = 13;

	strTimeLen = sprintf (strTime, "%s, %02d %s %04d %02d:%02d:%02d GMT", strWeekDay[sysTime.wDayOfWeek],
		    sysTime.wDay, strMonth[sysTime.wMonth - 1], sysTime.wYear, sysTime.wHour,
			sysTime.wMinute, sysTime.wSecond);
	
	return strTimeLen;
}

/*********************************************************************\
* Function	   StrToSysTime
* Purpose      change the string of time to system time
                "Tue, 25 Mar 2003 03:27:23 GMT"
* Params	   strTime: string
			   pSysTime: system time
* Return	 	
**********************************************************************/
void MMS_StrToSysTime(char *strTime, SYSTEMTIME *sysTime)
{
    int strTimeLen = 0;
	int len;
	int i;
	char tmp[10];
	char *p;

    if (strTime == NULL || sysTime == NULL)
        return;

    if (strTime[0] == '\0')
        return;

	while (*strTime == 0x20)
		strTime ++;

	//星期
	for (i = 0; i < 7; i++)
	{
		if (strnicmp(strTime, strWeekDay[i], 3) == 0)
		{
			sysTime->wDayOfWeek = i;
			break;
		}
	}

	//day
	strTime += 4;
	
	while (*strTime == 0x20)
		strTime ++;

	len = 0;

	p = strTime;
	while (*p != 0x20)
	{
		len ++;
		p ++;			
	}

	if(len < 3)
	{
		strncpy(tmp, strTime, len);
		tmp[len] = '\0';
		sysTime->wDay = atoi(tmp);
	}

	//month
	strTime += len;
	while (*strTime == 0x20)
		strTime ++;

	for (i = 0; i < 12; i++)
	{
		if (strnicmp(strTime, strMonth[i], 3) == 0)
		{
			sysTime->wMonth = i + 1;
			break;
		}
	}

	//year
	strTime += 3;
	while (*strTime == 0x20)
		strTime ++;

	len = 0;

	p = strTime;
	while (*p != 0x20)
	{
		len ++;
		p ++;			
	}

	if(len < 5)
	{
		strncpy(tmp, strTime, len);
		tmp[len] = '\0';
		sysTime->wYear = atoi(tmp);
	}

	//hour
	strTime += len;
	while (*strTime == 0x20)
		strTime ++;
	p = strTime;
	len = 0;
	while (*p != ':')
	{
		len ++;
		*p ++;
	}
	strncpy(tmp, strTime, len);
	tmp[len] = '\0';
	sysTime->wHour = atoi(tmp);

	//minutes
	strTime += len;
	strTime ++;
	while (*strTime == 0x20)
		strTime ++;
	p = strTime;
	len = 0;
	while (*p != ':')
	{
		len ++;
		*p ++;
	}
	strncpy(tmp, strTime, len);
	tmp[len] = '\0';
	sysTime->wMinute = atoi(tmp);

	//seconds
	strTime += len;
	strTime ++;
	while (*strTime == 0x20)
		strTime ++;
	p = strTime;
	len = 0;
	while (*p != 0x20)
	{
		len ++;
		*p ++;
	}
	strncpy(tmp, strTime, len);
	tmp[len] = '\0';
	sysTime->wSecond = atoi(tmp);

	strTime += len;
	while (*strTime == 0x20)
		strTime ++;
}


//(公历可以被4除尽，但如果可以被100除尽而不能被400除尽则不是闰年)
BOOL MMS_IsLeapYear(UINT year)
{
	if (year / 4 != 0)
		return FALSE;
	else
	{
		if ((year / 100 == 0) && (year / 400 != 0))
			return FALSE;
	}
	return TRUE;
}


// change the greenwich time to local time
void MMS_GreenwichToLocal(PSYSTEMTIME pTime, int dTime) 
{
	pTime->wHour += dTime;
	// 24 style
	if (pTime->wHour >= 24)
	{
		pTime->wHour = pTime->wHour - 24;

		// judge wether the day is the last day of the month
		switch(pTime->wMonth)
		{
			//大月
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (pTime->wDay == 31)
			{
				pTime->wDay = 1;
				// december
				if (pTime->wMonth == 12)
				{
					pTime->wMonth = 1;
					pTime->wYear ++;
				}
				else
					pTime->wMonth ++;
			}
			else
				pTime->wDay ++; 
			break;
			
			// February consider the leap year
		case 2:
			if ((MMS_IsLeapYear(pTime->wYear) && (pTime->wDay == 29))
				|| !(MMS_IsLeapYear(pTime->wYear) && (pTime->wDay == 28)))
			{
				pTime->wDay = 1;
				pTime->wMonth ++;
			}
			else					
				pTime->wDay ++; 
			break;
			
			// 小月
		default:
			if (pTime->wDay == 30)
			{
				pTime->wDay = 1;
				pTime->wMonth ++;
			}
			else
				pTime->wDay ++;
			break;
		}                   

		pTime->wDayOfWeek ++;
		if (pTime->wDayOfWeek == 7)
			pTime->wDayOfWeek = 0;
	}// end hour > 24
}
/*********************************************************************\
* Function	 FindContentByKey  
* Purpose    Find a keyword and the content
* Params	 pBuf: a buffer
             pField:    the keyword to search
             pLen: the length of the content
* Return	 content
**********************************************************************/
char *FindContentByKey(const char *pBuf, const char *pField, int *pLen)
{
	char* pDst;
    char* pEnd;

	pDst = strstr(pBuf, pField);
	if (pDst == NULL)
		return NULL;

	pDst = strchr(pDst, ':');
	if (pDst == NULL)
		return NULL;

	pDst++;         // pointer to the charactor rear the ":"
	while (*pDst == ' ')
		pDst++;

	pEnd = strstr(pDst, "/TYPE=PLMN");
	if (pEnd == NULL)
		pEnd = strstr(pDst, "\r\n");

	if (pEnd)
		*pLen = pEnd - pDst;
	else
		*pLen = pBuf + strlen(pBuf) - pDst;
	
	return pDst;
}

/******************************************************************** 
* Function	   MMS_IsMyfile()
* Purpose      judge wether the file is mms file according to the head
* Params	   hFile :the handle of the file
* Return	   bool
**********************************************************************/
BOOL MMS_IsMyfile(int  hFile)
{
    char head[MMS_HEADER_LEN];

    if (hFile == -1)
        return FALSE;
    read(hFile, &head, MMS_HEADER_LEN);
    if (strcmp(head, MMS_HEADER) == 0)
        return TRUE;
    return FALSE;
}

/******************************************************************** 
* Function	   MMS_WriteHead
* Purpose      write the head of mms file, including id,delivery or not,
			   the length of the msg, 6bytes
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int MMS_WriteHead(int hMmsFile, BYTE bDelivery)
{
	char szHead[10];
	memset(szHead, 0x0, 10);

    if (hMmsFile == -1)
        return 0;
		
    lseek(hMmsFile, 0, SEEK_SET);

	//%MMS + 0 , 5 bytes
	sprintf(szHead, "%s", MMS_HEADER);
    write(hMmsFile, szHead, MMS_HEADER_LEN);

	//write a sign of delivery
    write(hMmsFile, &bDelivery, 1);

    return MMS_HEADER_SIZE;
}


void MMS_SetPageNum(HWND hWnd, const char *str, int curSlide, int tNum)
{
	char sPage[SIZE_2];
	int len;

	len = sprintf(sPage, "%s             %d/%d", str, curSlide + 1, tNum);
//	ASSERT(len < SIZE_2);//xlzhu
	SetWindowText(hWnd, sPage);
}

/***
*char *strtok(string, control) - tokenize string with delimiter in control
*
*Purpose:
*       strtok considers the string to consist of a sequence of zero or more
*       text tokens separated by spans of one or more control chars. the first
*       call, with string specified, returns a pointer to the first char of the
*       first token, and will write a null char into string immediately
*       following the returned token. subsequent calls with zero for the first
*       argument (string) will work thru the string until no tokens remain. the
*       control string may be different from call to call. when no tokens remain
*       in string a NULL pointer is returned. remember the control chars with a
*       bit map, one bit per ascii char. the null char is always a control char.
*
*Entry:
*       char *string - string to tokenize, or NULL to get next token
*       char *control - string of characters to use as delimiters
*
*Exit:
*       returns pointer to first token in string, or if string
*       was NULL, to next token
*       returns NULL when no more tokens remain.
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/
char * MMS_strtok (char * string, const char * control )
{
        unsigned char *str;
        const unsigned char *ctrl = control;

        unsigned char map[32];
        int count;

        static char *nextoken;
        /* Clear control map */
        for (count = 0; count < 32; count++)
                map[count] = 0;

        /* Set bits in delimiter table */
        do {
                map[*ctrl >> 3] |= (1 << (*ctrl & 7));
        } while (*ctrl++);

        /* Initialize str. If string is NULL, set str to the saved
         * pointer (i.e., continue breaking tokens out of the string
         * from the last strtok call) */
        if (string)
                str = string;
        else
                str = nextoken;

        /* Find beginning of token (skip over leading delimiters). Note that
         * there is no token iff this loop sets str to point to the terminal
         * null (*str == '\0') */
        while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
                str++;

        string = str;

        /* Find the end of the token. If it is not the end of the string,
         * put a null there. */
        for ( ; *str ; str++ )
                if ( map[*str >> 3] & (1 << (*str & 7)) ) {
                        *str++ = '\0';
                        break;
                }

        /* Update nextoken (or the corresponding field in the per-thread data
         * structure */
        nextoken = str;

        /* Determine if a token has been found. */
        if ( string == str )
                return NULL;
        else
                return string;
}

char * MMS_chrtok (char * string, const char control1,  const char control2)
{
        unsigned char *str;
        const unsigned char ctrl1 = control1;
        const unsigned char ctrl2 = control2;

        static char *nextoken;

        /* Initialize str. If string is NULL, set str to the saved
         * pointer (i.e., continue breaking tokens out of the string
         * from the last strtok call) */
        if (string)
                str = string;
        else
                str = nextoken;

        string = str;

         /* Find the token. If it is not the end of the string,
         * put a null there. */
        for ( ; *str ; str++ )
        {
            if ( *str == ctrl1 || *str == ctrl2) 
            {
                *str++ = '\0';
                break;
            }
        }
        /* Update nextoken (or the corresponding field in the per-thread data
         * structure */
        nextoken = str;
        
        /* Determine if a token has been found. */
        if ( string == str )
            return NULL;
        else
            return string;
}


BOOL IsLegalChar(BYTE c)
{

	/*
	return	(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') 
		||(c >= '0' && c <= '9') || (c == ' ') ||(c == '_')
		||(c == '.')||(c == '@')||(c >= 0xa0 && c <= 0xfe); // chinese*/

	return  (c != '\\')&& (c != '/') && (c != ':') && (c != '*') 
		&& (c != '?') && (c!= '"') && (c!= '<') && (c != '>') && (c != '|');

}


BOOL IsLegalFileName(PCSTR  pName)
{
	char* pTemp;

    if (*pName == 0)
        return 0;
    
    if (*pName == 0x20)
        return 0;

	pTemp = (char*)pName;

	while( *pTemp && IsLegalChar(*pTemp) )
		pTemp ++ ;

	if( *pTemp)
		return 0;
	
	return 1;    
}


/******************************************************************** 
* Function	   MMS_ParseFileName
* Purpose      parse the file path and the file name
* Params	   pAll:		全路径
               pFileName:	文件名
               pPathName：	路径
* Return	 	   
* Remarks	   
**********************************************************************/
int MMS_ParseFileName(PCSTR pAll, PSTR pFileName, PSTR pPathName)
{
    int     len, i;

    if (pFileName == NULL || pPathName == NULL)
        return 0;

    len = strlen(pAll);

    for(i = len - 2; i >= 0; i--)
    {
        if (pAll[i] == 47 || (pAll[i] == 92)) // \ / :
		{
			strcpy(pFileName, &pAll[i+1]);
            strncpy(pPathName, pAll, i + 1);
            pPathName[i+1] = '\0';
			return i;
		}        
        else if (pAll[i] == 58)
        {
            strcpy(pFileName, &pAll[i+1]);
            strncpy(pPathName, pAll, (i + 1));
            pPathName[i + 1] = '\0';
			return i;
        }
    }

    // 如果没有找到/ | : 则认为整个的都是文件名,但长度不能超过
    if (len < MAX_FILENAME)
    {
        strcpy(pFileName, pAll);
        return len;
    }

    return 0;
}

void MMS_WaitWindowState(HWND hwnd, BOOL bShow)
{   
    static BOOL bWndShow = FALSE;

    if (bWndShow == bShow)
        return;
    else
    {
        bWndShow = bShow;
		WaitWindowState(hwnd, bShow);
    }
}

void MMS_InsertWindowState(HWND hwnd, BOOL bShow)
{   
    static BOOL bWndShow2 = FALSE;

    if (bWndShow2 == bShow)
        return;
    else
    {
        bWndShow2 = bShow;
		WaitWin(hwnd, bShow, ML("Inserting..."), ML("MMS"), ML(""), ML("Cancel"), NULL);
    }
}

/******************************************************************** 
* Function	   NullMsg
**********************************************************************/
void NullMsg(char *a, char *b, int c)
{

}

/*
 *	How many space has the mms used
 *	the return value's unit is K
 */
int MMS_GetUsedSpace(void)
{
    return APP_GetMMSSize();
}


int     MMS_StringCut(char * pStr, int nCutLen)
{
	int i = 0;
	int len = strlen(pStr);

	if(len < nCutLen)
		return len;

	while(i < nCutLen)
	{
		if (IsDBCSLeadByte(pStr[i]))
		{
			i += 2;
			if(i > nCutLen)
			{
				i = i - 2;
				break;
			}
		}
		else
			i++;
	}

	return i;

}

//得到文件名的后缀，成功返回1, 否则返回0
int GetFileNameSuffix(const PSTR pFileName, PSTR pSuffix)
{
    int i, len;

    len = strlen(pFileName);

    for (i = len - 1; i > 0; i--)
    {
        if (pFileName[i] == '.' && i != len - 1)
        {
            strcpy(pSuffix, &pFileName[i]);
     //       ASSERT(strlen(pSuffix) < 6);//xlzhu
            return i;
        }
    }
    return 0;
}

BOOL MMS_ModifyType(char *srcname, const int nFileType, int nFolder)
{
    return RenameMms(srcname, nFileType, nFolder);
}
BOOL MMS_ModifyFolder(char *srcname, const int iFolder)
{
	return RenameMms(srcname, -1, iFolder);
}

BOOL MMS_ModifyResend(char *srcname, BOOL bResend)
{
	char oldname[128];
	char newname[128];
	char o_fullpath[256];
	char n_fullpath[256];
	char b[16];

	strcpy(oldname, srcname);
	sprintf(b, "%d", bResend);

	if(oldname[MMS_RESEND_POS] == b[0])
		return FALSE;

	strcpy(newname, srcname);

	newname[MMS_RESEND_POS] = b[0];

	sprintf(o_fullpath, "%s%s", MMS_FILEPATH, oldname);
	sprintf(n_fullpath, "%s%s", MMS_FILEPATH, newname);
	if (MMS_MoveFile(o_fullpath, n_fullpath) == FALSE)
	{
		return FALSE;
	}

	strcpy(srcname, newname);

	return TRUE;
}

char* My_StrDup( const char* SrcStr )
{
    int len;    
    char* DstStr;
    
    if ( SrcStr == NULL ) 
        return NULL;
    
    len = strlen(SrcStr);
    
    DstStr = MMS_malloc( len + 1 );
    if (DstStr == NULL)
        return NULL;
    strcpy( DstStr, SrcStr);
    DstStr[len] = 0;
    
    return DstStr;
}

void SetFileTransferSleepFlag( BOOL bSleepFlag )
{
    return;
}

// 判断当前是否存在附件
mmsMetaNode* GetAttatchNotInSmil(mmsMetaNode *p)
{
    while (p)
    {
        if (p->inSlide == -1)
            return p;
        p = p->pnext;
    }
    return NULL;
}
/*********************************************************************\
* Function	   
* Purpose    delete all char:'\n'.  
* Params	   
* Return	 the new string's lenth	   
* Remarks	   
**********************************************************************/
int MMS_FormatStringNoRtn(char* pString)
{
    char* pDes = NULL;
    char* pSrc = NULL;
    char* pTemp = NULL;
    int i = 0;
    
    pSrc = pString;
    pTemp = MMS_malloc(strlen(pSrc) + 1);
    pDes = pTemp;

   // assert(pDes != NULL);//xlzhu

    while(*pSrc)
    {
        if(*pSrc == 0x0d)
        {
            if(*(pSrc+1) == 0x0a)
            {
                pSrc = pSrc + 2;
            }
            else
                pSrc ++;
        }
        else
        {
            *pDes++ = *pSrc++;
            i++;
        }
    }
    *pDes = '\0';

    strcpy(pString, pTemp);
    MMS_free(pTemp);

    return i;
}
int MMS_GetFileSize(const char * pFilename )
{
	struct stat *buf = NULL;
	int  iFileSize = -1;

	buf = MMS_malloc(sizeof(struct stat));
				
	if(buf == NULL)
	{
		return -1;
	}
				
	memset(buf, 0, sizeof(struct stat));
	stat(pFilename, buf);
				
	iFileSize = buf->st_size;
				
	MMS_free(buf);
	buf = NULL;
	
	return iFileSize;

}
//以下补充文件系统函数
#define BUF_SIZE 1024
BOOL MMS_CopyFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	int hfold,hfnew;
	char		buf[BUF_SIZE];
	DWORD nBytes, dwPointer = 0;
	
	hfold = open(pOldFileName, O_RDONLY);

	if(hfold == -1)
		return 0;

	hfnew = open(pNewFileName, O_RDWR|O_CREAT, S_IRWXU);

	if(hfnew == -1)
	{
		close(hfold);
		return 0;
	}
	nBytes = read(hfold, buf, BUF_SIZE);

	while (nBytes != 0 && nBytes != -1)
	{
		int ret;

		lseek(hfnew, dwPointer, SEEK_SET);
		ret = write(hfnew, buf, nBytes);
		
		if(ret != (int)nBytes)
		{
			close(hfnew);
			close(hfold);
			return 0;
		}
		dwPointer += nBytes; 
		lseek(hfold, dwPointer, SEEK_SET);
		nBytes = read(hfold, buf, BUF_SIZE);
					
	}
	close(hfold);
	close(hfnew);
	return 1;

}

BOOL MMS_MoveFile(PCSTR pOldFileName, PCSTR pNewFileName)
{

	MMS_CopyFile(pOldFileName, pNewFileName);
	unlink(pOldFileName);

	return TRUE;
}

#define BASE_YEAR		1980

#define YEAR_MASK		0xFE000000		/*	(1111 1110 0000 0000 0000 0000 00000000b)*/
#define MONTH_MASK		0x01E00000		/*	(0000 0001 1110 0000 0000 0000 00000000b)*/
#define DAY_MASK		0x001F0000		/*	(0000 0000 0001 1111 0000 0000 00000000b)*/
#define HOUR_MASK		0x0000F800		/*	(0000 0000 0000 0000 1111 1000 00000000b)*/
#define MINUTE_MASK		0x000007E0		/*	(0000 0000 0000 0000 0000 0111 11100000b)*/
#define SECOND_MASK		0x0000001F		/*	(0000 0000 0000 0000 0000 0000 00011111b)*/


#define YEAR_OFFSET		25
#define MONTH_OFFSET	21
#define DAY_OFFSET		16
#define HOUR_OFFSET		11
#define MINUTE_OFFSET	5
#define SECOND_OFFSET	0

void MMS_FTtoST(FILETIME* pFileTime, SYSTEMTIME* pSysTime)
{
	DWORD	time = pFileTime->dwLowDateTime;

	pSysTime->wYear		= (WORD)((( time & YEAR_MASK ) >> YEAR_OFFSET ) + BASE_YEAR);
	pSysTime->wMonth	= (WORD)(( time & MONTH_MASK ) >> MONTH_OFFSET );
	pSysTime->wDay		= (WORD)(( time & DAY_MASK ) >> DAY_OFFSET );
	pSysTime->wHour		= (WORD)(( time & HOUR_MASK ) >> HOUR_OFFSET );
	pSysTime->wMinute	= (WORD)(( time & MINUTE_MASK ) >> MINUTE_OFFSET );
	pSysTime->wSecond	= (WORD)(( time & SECOND_MASK ) << 1);
	pSysTime->wDayOfWeek= 0;
	pSysTime->wMilliseconds = 0;
}

void MMS_STtoFT(SYSTEMTIME* pSysTime, FILETIME* pFileTime)
{
	pFileTime->dwLowDateTime =  ((pSysTime->wYear-BASE_YEAR) << YEAR_OFFSET) & YEAR_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wMonth << MONTH_OFFSET) & MONTH_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wDay << DAY_OFFSET) & DAY_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wHour << HOUR_OFFSET) & HOUR_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wMinute << MINUTE_OFFSET) & MINUTE_MASK;
	pFileTime->dwLowDateTime |= (pSysTime->wSecond >> 1) & SECOND_MASK;

	pFileTime->dwHighDateTime = 0;
}
BOOL MMS_GetFileTime(char *pFileName, FILETIME *pFileTime)
{
	struct stat *buf = NULL;
	struct tm     tme, *ptime;
	SYSTEMTIME    systime;
   	
	buf = MMS_malloc(sizeof(struct stat));
				
	if(buf == NULL)
	{
		return -1;
	}
				
	memset(buf, 0, sizeof(struct stat));
	stat(pFileName, buf);
				
	ptime = &tme;

	ptime = localtime(&(buf->st_mtime));
	systime.wDay = (WORD)ptime->tm_mday;
	systime.wDayOfWeek = (WORD)ptime->tm_wday;
	systime.wHour = (WORD)ptime->tm_hour;
	systime.wMilliseconds = 0;
	systime.wMinute = (WORD)ptime->tm_min;
	systime.wMonth = (WORD)ptime->tm_mon + 1;
	systime.wSecond = (WORD)ptime->tm_sec;
	systime.wYear = (WORD)ptime->tm_year + 1900;
	
	MMS_STtoFT(&systime, pFileTime);
				
	MMS_free(buf);
	buf = NULL;
	
	return TRUE;

}

//空间判断返回状态值
#define SPACE_CHECKERROR     0   //失败
#define SPACE_AVAILABLE      1   //可用
#define SPACE_NEEDCLEAR      2   //碎片多需要整理
#define SPACE_NEEDDELFILE    3   //需要删除文件
static const char * pFlash2   = "/mnt/flash/";

static int IsFlashAvailable(int nSize)
{
	struct statfs  *spaceinfo = NULL;
	int ret = -1;

	
	spaceinfo = MMS_malloc(sizeof(struct statfs));
	if(spaceinfo == NULL)
		return ret;

	memset(spaceinfo, 0 , sizeof(struct statfs) );

	statfs(pFlash2, spaceinfo);

	if(nSize > spaceinfo->f_bfree)
		ret = SPACE_CHECKERROR;
	else if(nSize == spaceinfo->f_bfree)
		ret =  SPACE_NEEDCLEAR;
	else
		ret = SPACE_AVAILABLE;

	MMS_free(spaceinfo);
	spaceinfo = NULL;

	return ret;


}

BOOL MMS_IsEmail(const char* pString)
{
	char					At[2] = {(char)0xFD, 0};

	if(strstr(pString, At))
		return TRUE;
	else
		return FALSE;
}


#if 1
static BOOL MmsDes_OnCreate (HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static LRESULT MMSSelectDestinationProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MmsDes_OnKEY(HWND hWnd, WPARAM wParam, LPARAM lParam);
static char* MMS_DirGetFullPath(PMMS_DIRTREE pCurFolder);
static LRESULT MMSSelectFolderProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MmsSelFolder_FindChild(HWND hWnd, PMMS_DIRTREE pCurFolder);
static BOOL MmsSelFolder_RefreshList (HWND hWnd, PMMS_DIRTREE pCurFolder);
static BOOL MMS_SelectFolder(HWND hWndFrame, const char* pdata, int datalen, int pathType, int objectPath);
static BOOL MmsFolder_OnCreate (HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static BOOL MMS_DirRelease(PMMS_DIRTREE pCurFolder);
static BOOL MMS_DirAddChild(PMMS_DIRTREE pCurFolder, const char* pFolderName);
static BOOL MmsSelFolder_OnKEY(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL MmsSelFolder_Save (HWND hWnd, PMMS_DIRTREE pDir);
enum
{
	FLASH_FOLDER = 0,
	MEMCARD_FOLDER
};

enum
{
	MUSIC_TYPE = 0,
	PICTURE_TYPE,
	TEXT_TYPE
};

static char  obj_filename[257];

#define PIC_FLASH_ROOT_PATH		"mnt/flash/pictures"
#define PIC_MEMCARD_ROOT_PATH   "mnt/fat16/pictures"

#define TEXT_FLASH_ROOT_PATH	"mnt/flash/notepad"
#define TEXT_MEMCARD_ROOT_PATH	"mnt/fat16/notepad"
#define TEXT_MEMCARD_ROOT	"/mnt/fat16/notepad"

#define MUSIC_FLASH_ROOT_PATH	 "mnt/flash/audio"
#define MUSIC_MEMCARD_ROOT_PATH  "mnt/fat16/audio"

BOOL MMS_SelectDestination(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen, int metaType)
{
	WNDCLASS wc;
	HWND	hSelDest;
	RECT	rClient;
	MMS_DESTINATIONCREATEDATA CreateData;

	//if there is no memory-card, go to select folder

	strcpy(obj_filename, pName);

	if(metaType != META_AUDIO
		&& metaType != META_IMG
		&& metaType != META_TEXT)
	{
		MsgWin(NULL, NULL, 0, STR_MMS_ERRFORMAT, STR_MMS_MMS,
				Notify_Failure, STR_MMS_OK, NULL, WAITTIMEOUT);

		return FALSE;
	}

	if(metaType == META_TEXT)
			MMS_SelectFolder(hWndFrame, pdata, datalen, FLASH_FOLDER, TEXT_TYPE);
	
	if(!MMC_CheckCardStatus())
	{
		if(metaType == META_AUDIO)
			MMS_SelectFolder(hWndFrame, pdata, datalen, FLASH_FOLDER, MUSIC_TYPE);
		else if(metaType == META_IMG)
			MMS_SelectFolder(hWndFrame, pdata, datalen, FLASH_FOLDER, PICTURE_TYPE);
		else if(metaType == META_TEXT)
			MMS_SelectFolder(hWndFrame, pdata, datalen, FLASH_FOLDER, TEXT_TYPE);
		else
		{
			return FALSE;
		}
		return TRUE;
	}
	


	wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_DESTINATIONCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = MMSSelectDestinationProc;
    wc.lpszClassName = "MMSSelectDesClass";
    wc.hbrBackground = NULL;
        
    RegisterClass(&wc);

	GetClientRect(hWndFrame, &rClient);
	CreateData.hWndFrame = hWndFrame;
	CreateData.mediaType = metaType;
	CreateData.hWnd = hWnd;
	CreateData.msg = msg;
	CreateData.pData = pdata;
	CreateData.len = datalen;

	hSelDest = CreateWindow(
        "MMSSelectDesClass",
        STR_SELECT_DISTINATION,//MU_GetCurCaption(CreateData), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );

	SetWindowText(hWndFrame, STR_SELECT_DISTINATION);
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, MAKEWPARAM(IDB_EXIT, 0), (LPARAM)STR_MMS_CANCEL);
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	return TRUE;

}

static LRESULT MMSSelectDestinationProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
    PMMS_DESTINATIONCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
		lResult = MmsDes_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(pCreateData->hWndFrame, STR_SELECT_DISTINATION);
		SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, MAKEWPARAM(IDB_EXIT, 0), (LPARAM)STR_MMS_CANCEL);
		SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
		SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		break;

	case WM_KEYDOWN:
		lResult = MmsDes_OnKEY(hWnd, wParam, lParam);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		DeleteObject(pCreateData->hBitmapPhone);
		DeleteObject(pCreateData->hBitmapMemory);
		SendMessage(pCreateData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
		UnregisterClass("MMSSelectDesClass", NULL);
		break;
		
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

static BOOL MmsDes_OnCreate (HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	PMMS_DESTINATIONCREATEDATA pCreateData;
	RECT rect;
	HWND hList;
	char  szfree[64];

	pCreateData = GetUserData(hWnd);
	
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_DESTINATIONCREATEDATA));
	pCreateData->hBitmapPhone = LoadImage(NULL, MMS_PHONE_MEMORY_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	pCreateData->hBitmapMemory = LoadImage(NULL, MMS_MEMORY_CARD_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	GetClientRect(hWnd,&rect);

	hList = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_DESTINATION, 
        NULL, 
        NULL);

	SendMessage(hList, LB_INSERTSTRING, (WPARAM)0, (LPARAM) (LPCTSTR)STR_MMS_PHONEMEMORY);
    SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)pCreateData->hBitmapPhone);
    sprintf(szfree, "%dKB", GetAvailFlashSpace());
	SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)szfree);
	
	SendMessage(hList, LB_INSERTSTRING, (WPARAM)1, (LPARAM) (LPCTSTR)STR_MEMORY_CARD);
    SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)pCreateData->hBitmapMemory);
    sprintf(szfree, "%dKB", GetAvailMMCSpace());
	SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)szfree);

	SendMessage(hList, LB_SETCURSEL, 0, 0);
	SetFocus(hList);
	return TRUE;

}

static BOOL MmsDes_OnKEY(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_DESTINATIONCREATEDATA pCreateData;
	HWND hWndFrame;

	pCreateData = GetUserData(hWnd);
	hWndFrame = pCreateData->hWndFrame;

	switch(LOWORD(wParam))
	{	
	case VK_F10:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case VK_F5:
		{
			HWND hList;
			int nCurSel;

			hList = GetDlgItem(hWnd, IDC_DESTINATION);

			nCurSel = SendMessage(hList,LB_GETCURSEL,0,0);

			if(nCurSel == 0)
			{
				if(pCreateData->mediaType == META_AUDIO)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, FLASH_FOLDER, MUSIC_TYPE);	
				else if(pCreateData->mediaType == META_IMG)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, FLASH_FOLDER, PICTURE_TYPE);
				else if(pCreateData->mediaType == META_TEXT)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, FLASH_FOLDER, TEXT_TYPE);

			}

			if(nCurSel == 1)
			{
				if(pCreateData->mediaType == META_AUDIO)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, MEMCARD_FOLDER, MUSIC_TYPE);
				else if(pCreateData->mediaType == META_IMG)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, MEMCARD_FOLDER, PICTURE_TYPE);
				else if(pCreateData->mediaType == META_TEXT)
					MMS_SelectFolder(pCreateData->hWndFrame, pCreateData->pData, pCreateData->len, MEMCARD_FOLDER, TEXT_TYPE);

			}
			
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	}

	return TRUE;
}

/**************************************************************************************/
static BOOL MMS_SelectFolder(HWND hWndFrame, const char* pdata, int datalen, int pathType, int objectPath)
{
	char rootPath[128];
	PMMS_DIRTREE pRoot;
	MMS_FOLDERCREATEDATA CreateData;
	WNDCLASS wc;
	RECT rClient;

	if(pathType == FLASH_FOLDER)
	{
		if(objectPath == MUSIC_TYPE)
		{
			strcpy(rootPath, MUSIC_FLASH_ROOT_PATH);
		}
		else if(objectPath == PICTURE_TYPE)
		{
			strcpy(rootPath, PIC_FLASH_ROOT_PATH);
		}
		else if(objectPath == TEXT_TYPE)
		{
			strcpy(rootPath, TEXT_FLASH_ROOT_PATH);
		}
		else
			return FALSE;
	}
	else if(pathType == MEMCARD_FOLDER)
	{
		if(objectPath == MUSIC_TYPE)
		{
			strcpy(rootPath, MUSIC_MEMCARD_ROOT_PATH);
		}
		else if(objectPath == PICTURE_TYPE)
		{
			strcpy(rootPath, PIC_MEMCARD_ROOT_PATH);
		}
		else if(objectPath == TEXT_TYPE)
		{
			mkdir(TEXT_MEMCARD_ROOT, 0666);
			strcpy(rootPath, TEXT_MEMCARD_ROOT_PATH);
		}
	}


	pRoot = malloc(sizeof(MMS_DIRTREE));
	memset(pRoot, 0, sizeof(MMS_DIRTREE));
	strcpy(pRoot->szName, rootPath);
	
	wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_FOLDERCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = MMSSelectFolderProc;
    wc.lpszClassName = "MMSSelectFolderClass";
    wc.hbrBackground = NULL;
        
    RegisterClass(&wc);

	GetClientRect(hWndFrame, &rClient);
	CreateData.hWndFrame = hWndFrame;
	CreateData.pRoot = pRoot;
	CreateData.pData = pdata;
	CreateData.len = datalen;
	
	CreateWindow(
        "MMSSelectFolderClass",
        "",//MU_GetCurCaption(CreateData), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );

	SetWindowText(hWndFrame, STR_SELECT_FOLDER);
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, MAKEWPARAM(IDB_EXIT, 0), (LPARAM)STR_MMS_CANCEL);
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SAVE);
	return TRUE;
}

static LRESULT MMSSelectFolderProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
    PMMS_FOLDERCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
		lResult = MmsFolder_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;

	case PWM_SHOWWINDOW:
		{
			HWND hList;
			int index;

			hList = GetDlgItem(hWnd, IDC_FOLDER);
			index = SendMessage(hList,LB_GETCURSEL,0,0);

			SetWindowText(pCreateData->hWndFrame, STR_SELECT_FOLDER);
			SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, MAKEWPARAM(IDB_EXIT, 0), (LPARAM)STR_MMS_CANCEL);
			if(index == LB_ERR || index == 0)
				SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			else
				SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

			SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SAVE);
		}
		
		break;

	case WM_KEYDOWN:
		lResult = MmsSelFolder_OnKEY(hWnd, wParam, lParam);
		break;

	case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			int	codeNotify = HIWORD(wParam);
			
			switch(id)
			{
			case IDC_FOLDER:
				if(codeNotify == LBN_SELCHANGE)
				{
					HWND hList;
					int index;
					
					hList = GetDlgItem(hWnd, IDC_FOLDER);
					index = SendMessage(hList,LB_GETCURSEL,0,0);
					if(index == LB_ERR || index == 0)
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
					else
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
				}
				break;	
			}
		break;
		}

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		DeleteObject(pCreateData->hBitmapChild);
		DeleteObject(pCreateData->hBitmapOpen);
		MMS_DirRelease(pCreateData->pRoot);
		SendMessage(pCreateData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
		UnregisterClass("MMSSelectFolderClass", NULL);
		break;
		
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

static BOOL MmsFolder_OnCreate (HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	PMMS_FOLDERCREATEDATA pCreateData;
	RECT rect;
	HWND hList;
	BOOL rtn;

	pCreateData = GetUserData(hWnd);
	
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_FOLDERCREATEDATA));
	pCreateData->hBitmapOpen = LoadImage(NULL, MMS_OPEN_FOLDER_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	pCreateData->hBitmapChild = LoadImage(NULL, MMS_CHILD_FOLDER_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	GetClientRect(hWnd,&rect);

	hList = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_FOLDER, 
        NULL, 
        NULL);

	rtn = MmsSelFolder_FindChild(hWnd, pCreateData->pRoot);

	if(rtn)
		MmsSelFolder_RefreshList(hWnd, pCreateData->pRoot);


	SendMessage(hList, LB_SETCURSEL, 0, 0);
	SetFocus(hList);
	return TRUE;

}

static BOOL MmsSelFolder_OnKEY(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_FOLDERCREATEDATA pCreateData;
	HWND hWndFrame;

	pCreateData = GetUserData(hWnd);
	hWndFrame = pCreateData->hWndFrame;

	switch(LOWORD(wParam))
	{	
	case VK_F10:
		{
			HWND hList;
			PMMS_DIRTREE pDir;
			int nCurSel;

			hList = GetDlgItem(hWnd, IDC_FOLDER);

			nCurSel = SendMessage(hList,LB_GETCURSEL,0,0);
			if(nCurSel == -1)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}
			
			pDir = (PMMS_DIRTREE)SendMessage(hList, LB_GETITEMDATA, 0, 0);
			if(pDir == NULL || pDir == pCreateData->pRoot)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}

			MmsSelFolder_RefreshList(hWnd, pDir->pParent);

		}
		break;

	case VK_F5:
		{
			HWND hList;
			int nCurSel;
			PMMS_DIRTREE pDir;
			int rtn;

			hList = GetDlgItem(hWnd, IDC_FOLDER);

			nCurSel = SendMessage(hList,LB_GETCURSEL,0,0);
			if(nCurSel == -1)
				break;
			
			if(nCurSel == 0)
				break;

			pDir = (PMMS_DIRTREE)SendMessage(hList, LB_GETITEMDATA, nCurSel, 0);
			if(pDir == NULL)
				break;
			
			rtn = MmsSelFolder_FindChild(hWnd, pDir);

			if(rtn)
				MmsSelFolder_RefreshList(hWnd, pDir);
		}
		break;

	case VK_RETURN:
		{
			HWND hList;
			PMMS_DIRTREE pDir;

			hList = GetDlgItem(hWnd, IDC_FOLDER);

			pDir = (PMMS_DIRTREE)SendMessage(hList, LB_GETITEMDATA, 0, 0);
			if(pDir == NULL || (int)pDir == LB_ERR)
				break;
			
			MmsSelFolder_Save(hWnd, pDir);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	}

	return TRUE;
}

static BOOL MmsSelFolder_FindChild(HWND hWnd, PMMS_DIRTREE pCurFolder)
{
	struct dirent	*dirinfo = NULL;
	struct DIR		*dirtemp = NULL;
	char*			pFullPath;
	char*			pString;
	struct stat		buf;
	
	if(pCurFolder->pChild != NULL)
		return TRUE;

	pFullPath = MMS_DirGetFullPath(pCurFolder);

	dirtemp = opendir(pFullPath);
	if(dirtemp == NULL)
	{
		free(pFullPath);
		return FALSE;	
	}

	pString = malloc(strlen(pFullPath) + 512 + 1);
	if(pString == NULL)
	{
		free(pFullPath);
		closedir(dirtemp);
		return FALSE;	
	}

	do 
	{
		dirinfo = readdir(dirtemp);
		if (NULL == dirinfo)
			break;

		if(stricmp(dirinfo->d_name, ".") == 0 || stricmp(dirinfo->d_name, "..") == 0)
			continue;

		sprintf(pString, "%s/%s", pFullPath, dirinfo->d_name);

		stat(pString, &buf);

		if(S_ISDIR(buf.st_mode))
		{
			MMS_DirAddChild(pCurFolder, dirinfo->d_name);
		}


	} while(dirinfo);

	free(pFullPath);
	free(pString);
	closedir(dirtemp);
	return TRUE;
}

static BOOL MmsSelFolder_RefreshList(HWND hWnd, PMMS_DIRTREE pCurFolder)
{
	HWND hList;
	PMMS_DIRTREE pTemp;
	int i = 0;
	PMMS_FOLDERCREATEDATA pCreateData;
	char	pPathName[MAX_PATH_LEN];
    char	pFileName[MAX_FILENAME];
	char    pDisplayName[MAX_PATH_LEN];
//	int len;

	pCreateData = GetUserData(hWnd);

	hList = GetDlgItem(hWnd, IDC_FOLDER);

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	pTemp = pCurFolder;
	
	if(pTemp == pCreateData->pRoot)
	{
		MMS_ParseFileName(pTemp->szName, pFileName, pPathName);
		//len = UTF8ToMultiByte(CP_ACP, 0, pFileName, -1, pDisplayName, MAX_PATH_LEN - 1,NULL, NULL);
		//pDisplayName[len] = 0;
		strcpy(pDisplayName, pFileName);
		pDisplayName[0] = pDisplayName[0] - 0x20;		//to upcase
		SendMessage(hList, LB_INSERTSTRING, (WPARAM)i, (LPARAM) (LPCTSTR)pDisplayName);
	}
	else
	{
		//len = UTF8ToMultiByte(CP_ACP, 0, pTemp->szName, -1, pDisplayName, MAX_PATH_LEN - 1,NULL, NULL);
		//pDisplayName[len] = 0;
		strcpy(pDisplayName, pTemp->szName);
		SendMessage(hList, LB_INSERTSTRING, (WPARAM)i, (LPARAM) (LPCTSTR)pDisplayName);
	}
	
	SendMessage(hList, LB_SETITEMDATA, (WPARAM)i, (LPARAM)pTemp);
	SendMessage(hList, LB_SETIMAGE, (WPARAM)i, (LPARAM)pCreateData->hBitmapOpen);
	i++;
	pTemp = pTemp->pChild;

	while(pTemp)
	{
		//len = UTF8ToMultiByte(CP_ACP, 0, pTemp->szName, -1, pDisplayName, MAX_PATH_LEN - 1,NULL, NULL);
		//pDisplayName[len] = 0;
		strcpy(pDisplayName, pTemp->szName);
		
		SendMessage(hList, LB_INSERTSTRING, (WPARAM)i, (LPARAM) (LPCTSTR)pDisplayName);
		SendMessage(hList, LB_SETITEMDATA, (WPARAM)i, (LPARAM)pTemp);
		SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCreateData->hBitmapChild);
		i++;

		pTemp = pTemp->pNext;
	}
	
	SetFocus(hList);
	SendMessage(hList, LB_SETCURSEL, 0, 0);
	PostMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
	return TRUE;
}

static BOOL MmsSelFolder_Save(HWND hWnd, PMMS_DIRTREE pDir)
{
	char* pFullPath;
	char* pString;
	int len;
	PMMS_FOLDERCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

	pFullPath = MMS_DirGetFullPath(pDir);
	
	len = strlen(pFullPath) + strlen(obj_filename);

	pString = malloc(len + 16);

	sprintf(pString, "%s/%s", pFullPath, obj_filename);

	MMS_SaveObject(pCreateData->hWndFrame, NULL, 0, STR_RENAME_OBJECT, pString, pCreateData->pData, pCreateData->len);

	free(pFullPath);
	free(pString);

	return TRUE;
}	

static char* MMS_DirGetFullPath(PMMS_DIRTREE pCurFolder)
{
	char* pFullPath;
	char* pEnd;
	PMMS_DIRTREE pTemp;
	int len;

	pFullPath = malloc(1024);

	pEnd = pFullPath + 1024;

	pEnd--;

	*pEnd = 0;
	pTemp = pCurFolder;

	while(pTemp)
	{
		len = strlen(pTemp->szName);
		pEnd -= len;
		
		if(pEnd <= pFullPath)	//overflow
		{
			free(pFullPath);
			return NULL;
		}

		memcpy(pEnd, pTemp->szName, len);

		pEnd--;
		*pEnd = '/';

		pTemp = pTemp->pParent;
	}

	strcpy(pFullPath, pEnd);

	return pFullPath;
}

BOOL MMS_DirAddChild(PMMS_DIRTREE pCurFolder, const char* pFolderName)
{
	PMMS_DIRTREE pTemp;
	PMMS_DIRTREE pChild;

	pTemp = pCurFolder;
	
	pChild = malloc(sizeof(MMS_DIRTREE));
	memset(pChild, 0, sizeof(MMS_DIRTREE));
	strcpy(pChild->szName, pFolderName);
	pChild->pParent = pCurFolder;

	if(pCurFolder->pChild == NULL)
	{
		pCurFolder->pChild = pChild;
		return TRUE;
	}

	pTemp = pCurFolder->pChild;

	while(pTemp->pNext)
		pTemp = pTemp->pNext;

	pTemp->pNext = pChild;
	pChild->pPre = pTemp;
		
	return TRUE;
}

//release all child
BOOL MMS_DirRelease(PMMS_DIRTREE pCurFolder)
{
	PMMS_DIRTREE pTemp;
	PMMS_DIRTREE pTemp1;

	pTemp = pCurFolder->pChild;
	
	while(pTemp)
	{
		pTemp1 = pTemp->pNext;
		MMS_DirRelease(pTemp);
		pTemp = pTemp1;
	}

	free(pCurFolder);

	return TRUE;
}

#endif
