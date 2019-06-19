   /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Notebook
 *
 * Purpose  : deal with file
 *            
\**************************************************************************/

#include "window.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "Setting.h"
#include "time.h"
#include "hpdef.h"
#include "pubapp.h"
#include "plx_pdaex.h"
#include "stdlib.h"


#include "TextFile.h"
#include "Notepad.h"


//////////////////////////////////////////////////////////////////////////////
//
//			Variables
//
//////////////////////////////////////////////////////////////////////////////
static PTEXTINFONODE pTextNodeHead, pTextNodeEnd, pTextNodeCur;

//////////////////////////////////////////////////////////////////////////////
//
//			Functions
//
//////////////////////////////////////////////////////////////////////////////
static PTEXTINFONODE FindInfoNode(PTEXTINFO pTextInfo);
static int DeleteTextNode(PTEXTINFONODE pNode);
static int SaveTextFile(PCSTR szFilename, PCSTR szContent, DWORD dwMode);
static int ReadTextFile(PCSTR szFilename, char * szContent, DWORD dwReadLen);



//////////////////////////////////////////////////////////////////////////////
// Function	GetTextFileInfo
// Purpose	get file's length and time
// Params	
// Return	success return 1
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int GetTextFileInfo(PCSTR szFilename, struct stat *buff)
{
	int state;

	state = stat(szFilename, buff);
	if (state == 0)
		return 1;
	else 
		return 0;

}
//////////////////////////////////////////////////////////////////////////////
// Function	SaveTextFile
// Purpose	save text
// Params	
// Return	success return 1
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int SaveTextFile(PCSTR szFilename, PCSTR szContent, DWORD dwMode)
{

	int hf;
	DWORD dwCreateMode;
	size_t lenUTF, writeLen;
	char * pContentUTF;

	switch(dwMode)
	{
	case SAVE_MODE_NEW:		
		dwCreateMode = O_WRONLY | O_CREAT;
		break;
	case SAVE_MODE_OVERWRITE:
		dwCreateMode = O_RDWR | O_TRUNC;
		break;
	case SAVE_MODE_APPEND:		
		dwCreateMode = O_RDWR;
		break;
	default:					
		return 0;
		break;
	}


	hf = open(szFilename, dwCreateMode,0);
	if(-1 == hf)
		return 0;
	if(SAVE_MODE_APPEND == dwMode)
		lseek(hf, 0, SEEK_END);

	lenUTF = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szContent, -1, NULL, 0);
	pContentUTF = (char *)malloc(lenUTF);
    if(pContentUTF == NULL)
	{
	   close(hf);
	   return 0;
	}
	MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szContent, -1, (LPSTR)pContentUTF, lenUTF);
		
	if(lenUTF > 0)
		writeLen = write(hf, (PVOID)pContentUTF, lenUTF);
	else
		writeLen = 0;
	if(pContentUTF)
		free(pContentUTF);
	close(hf);	
	if(lenUTF != writeLen)	
		return 0;
	return 1;
}


//////////////////////////////////////////////////////////////////////////////
// Function	GetTextFileLength
// Purpose  get file's length
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int GetTextFileLength(PCSTR szFilename)
{

	struct stat buff;
	int len;

	GetTextFileInfo(szFilename, &buff);
	len = buff.st_size;	
	
	return(len);
}

//////////////////////////////////////////////////////////////////////////////
// Function	GetTextFileTime
// Purpose	get file's time
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int GetTextFileTime(PCSTR szFilename, PSYSTEMTIME pSysTime)
{
	struct stat buff;
	struct tm *temptime;
	if(GetTextFileInfo(szFilename, &buff))
	{
		temptime = localtime(&buff.st_mtime);
		pSysTime->wYear = temptime->tm_year + 1900;
		pSysTime->wMonth = temptime->tm_mon + 1;
		pSysTime->wDay = temptime->tm_mday;
		pSysTime->wHour = temptime->tm_hour;
		pSysTime->wMinute = temptime->tm_min;
		pSysTime->wDayOfWeek = 0;
		pSysTime->wMilliseconds = 0;
		pSysTime->wSecond = 0;
	}		
	else
	{
		pSysTime->wYear = 1900;
		pSysTime->wMonth = 0;
		pSysTime->wDay = 0;
		pSysTime->wHour = 0;
		pSysTime->wMinute = 0;
		pSysTime->wDayOfWeek = 0;
		pSysTime->wMilliseconds = 0;
		pSysTime->wSecond = 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Function	DeleteText
// Purpose	delete file
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int DeleteText(PTEXTINFO pInfo)
{
	if(remove(pInfo->szFileName) == 0)
	{
	
		if(DeleteTextNode(FindInfoNode(pInfo)))
		{
		
			return 1;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Function	GetAllTextNode
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int GetAllTextNode(char* pNotePath)
{
	TEXTINFO TxtInfo;
	char szFilename[TEXT_FILENAME_LENGTH + 1];
	struct dirent *pdirent =NULL;
	DIR *pdir = NULL;
	struct stat buff;
	int nFileCount = 0;

	chdir(pNotePath);
	pdir = opendir(pNotePath);
	if(pdir == NULL)	
		return 0;		
	pdirent = readdir(pdir);
	if(pdirent == NULL)
	{
		closedir(pdir);
		return 0;
	}
	
	do 
	{
		memset(&TxtInfo, 0, sizeof(TEXTINFO));
		strcpy(szFilename,pNotePath);
		strcat(szFilename,pdirent->d_name);
		
		if(IsWantedFile(szFilename, TEXT_FILE_TYPE))
		{	
			nFileCount++;
			GetTextFileInfo(szFilename, &buff);
			TxtInfo.stTime = buff.st_mtime;
			TxtInfo.dwTextLength = buff.st_size;
			strcpy(TxtInfo.szFileName, szFilename);
			if(ReadText(&TxtInfo, TxtInfo.szTextName, TEXT_NAME_LENGTH) > -1)
				AddTextNode(&TxtInfo);
		}
		pdirent = readdir(pdir);
	}while(pdirent !=NULL );

	closedir(pdir);
	return nFileCount;
}

//////////////////////////////////////////////////////////////////////////////
// Function	ReleaseAllTextInfo
// Purpose	release all information about files释放所有文本信息
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void ReleaseAllTextInfo()
{
	PTEXTINFONODE pTextInfo, pNextTextInfo;
	if(NULL == pTextNodeHead && NULL == pTextNodeEnd)
		return;

	pNextTextInfo = pTextNodeHead;
	while (NULL != pNextTextInfo) {
		pTextInfo = pNextTextInfo;
		pNextTextInfo = pTextInfo->pNextInfo;
		free(pTextInfo);
		pTextInfo = NULL;
	}
	pTextNodeHead = NULL;
	pTextNodeEnd = NULL;
	pTextNodeCur = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Function	AddTextNode
// Purpose	add information about file
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int AddTextNode(PTEXTINFO pText)
{
	PTEXTINFONODE pInfoNode;

	pInfoNode = (PTEXTINFONODE)malloc(sizeof(TEXTINFONODE));
	if (NULL == pInfoNode) 	
		return 0;
	memcpy(&pInfoNode->TextInfo, pText, sizeof(TEXTINFO));
	if(NULL == pTextNodeEnd){	
		pTextNodeHead = pTextNodeEnd = pInfoNode;
		pInfoNode->pPreInfo = NULL;
		pInfoNode->pNextInfo = NULL;
	}
	else
	{
		pInfoNode->pNextInfo = NULL;
		pInfoNode->pPreInfo = pTextNodeEnd;
		pTextNodeEnd->pNextInfo = pInfoNode;
		pTextNodeEnd = pInfoNode;
	}
	pTextNodeCur = pInfoNode;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Function	DeleteTextNode
// Purpose	delete information about a file
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int DeleteTextNode(PTEXTINFONODE pNode)
{
	if(NULL == pNode)
		return 0;

	if(pTextNodeHead == pNode){
		pTextNodeHead = pNode->pNextInfo;
		pTextNodeCur = pTextNodeHead;
	}
	if(pTextNodeEnd == pNode){
		pTextNodeEnd = pNode->pPreInfo;
		pTextNodeCur = pTextNodeEnd;
	}

	if(NULL != pNode->pPreInfo){
		if(NULL != pNode->pNextInfo){
			pNode->pNextInfo->pPreInfo = pNode->pPreInfo;
		}
	}
	else{
		if(NULL != pNode->pNextInfo){
			pNode->pNextInfo->pPreInfo = NULL;
		}
	}

	if(NULL != pNode->pNextInfo){
		pTextNodeCur = pNode->pNextInfo;	
		if(NULL != pNode->pPreInfo)
			pNode->pPreInfo->pNextInfo = pNode->pNextInfo;
	}
	else{
		pTextNodeCur = NULL;	
		if(NULL != pNode->pPreInfo){
			pNode->pPreInfo->pNextInfo = NULL;
		}
	}
	if (NULL != pNode) 
	{
		free(pNode);
		pNode = NULL;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Function	FindInfoNode
// Purpose	file a node
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
PTEXTINFONODE FindInfoNode(PTEXTINFO pTextInfo)
{
	PTEXTINFONODE pMove;

	pMove = pTextNodeHead;
	while (pMove != NULL){
		if(pTextInfo == &pMove->TextInfo){
			pTextNodeCur = pMove;
			return pMove;
		}
		pMove = pMove->pNextInfo;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Function	MoveToFirstTextInfo
// Purpose	move to first node
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int MoveToFirstTextNode()
{
	pTextNodeCur = pTextNodeHead;
	if(NULL == pTextNodeCur)
		return (0);
	return (1);
}



//////////////////////////////////////////////////////////////////////////////
// Function	MoveToNextTextInfo
// Purpose	move next node's information
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int MoveToNextTextNode()
{
	if(NULL == pTextNodeCur)
		return (0);
	if(NULL == pTextNodeCur->pNextInfo)
		return (0);
	pTextNodeCur = pTextNodeCur->pNextInfo;
	return (1);
}



//////////////////////////////////////////////////////////////////////////////
// Function	GetCurTextInfo
// Purpose	get current node's information
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
PTEXTINFO GetCurTextInfo(){
	return (&pTextNodeCur->TextInfo);
}

//////////////////////////////////////////////////////////////////////////////
// Function	SaveText
// Purpose	save text
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
PTEXTINFO SaveText(PTEXTINFO pInfo, PCSTR szContent, char *pNewFileName, char* pDesPath, DWORD dwMode)
{
	char szPathFilename[_MAX_PATH];
	struct stat buff;	
	
	if(SAVE_MODE_NEW == dwMode)
	{
		sprintf(pInfo->szFileName, "%s%s", pDesPath, pNewFileName);//".", TEXT_FILE_SUFFIX
	}	
	
	if(!SaveTextFile(pInfo->szFileName, szContent, dwMode))
		return 0;
	if(!GetTextFileInfo(pInfo->szFileName, &buff))
		return 0;
	if(SAVE_MODE_OVERWRITE == dwMode)
	{
		sprintf(szPathFilename, "%s%s", pDesPath, pNewFileName);//nRand, ".", TEXT_FILE_SUFFIX		
		rename(pInfo->szFileName, szPathFilename);
		strcpy(pInfo->szFileName, szPathFilename);
	
	}
	
	pInfo->stTime = buff.st_mtime;
	pInfo->dwTextLength = buff.st_size;
	ReadText(pInfo, pInfo->szTextName, TEXT_NAME_LENGTH);	
	if(SAVE_MODE_NEW == dwMode)
	{
		if(!AddTextNode(pInfo))
			return 0;
		return (GetCurTextInfo());
	}
	return (pInfo);
}

//////////////////////////////////////////////////////////////////////////////
// Function	ReadText
// Purpose	read text
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int ReadText(PTEXTINFO pInfo, char * szContent, DWORD dwReadLen)
{
	int hf;
	int ret;
	DWORD dwToRead;
	int nRet;
	char* pAllContentUTF8 = NULL;	
	char* pAllContentLocal = NULL;


	hf = open(pInfo->szFileName, O_RDONLY, 0);
	if(-1 == hf)
		return (-1);
   if(dwReadLen > NOTEPAD_MAX_CONTENT)
	{
		dwToRead = NOTEPAD_MAX_CONTENT;
	}
	else
		dwToRead = dwReadLen;

	pAllContentUTF8 = (char*)malloc(pInfo->dwTextLength);
	if(pAllContentUTF8 == NULL)
	{
		close(hf);
		return (-1);
	}
	memset(pAllContentUTF8, 0, pInfo->dwTextLength);
	ret = read(hf, pAllContentUTF8, pInfo->dwTextLength);	
	close(hf);
	if(0 > ret)
	{
		if(pAllContentUTF8)
			free(pAllContentUTF8);
		return (-1);
	
	}
	memset(szContent, 0, dwToRead);
	nRet = UTF8ToMultiByte(CP_ACP, 0, pAllContentUTF8, -1, NULL, 0, NULL, NULL);
	pAllContentLocal = (char *)malloc(nRet + 1);
	if(pAllContentLocal == NULL)
	{
		if(pAllContentUTF8)
			free(pAllContentUTF8);
		return (-1);
	
	}
	memset(pAllContentLocal, 0, (nRet +1));
	UTF8ToMultiByte(CP_ACP, 0, pAllContentUTF8, -1, pAllContentLocal, (nRet + 1), NULL, NULL);
	memcpy(szContent, pAllContentLocal, dwToRead);
	szContent[dwToRead] = '\0';
	if(pAllContentUTF8)
		free(pAllContentUTF8);
	if(pAllContentLocal)
		free(pAllContentLocal);
	return (ret);
}

//////////////////////////////////////////////////////////////////////////////
// Function	IsEnoughFreeSpace
// Purpose	whether has enough space to save current file
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int IsEnoughFreeSpaceToSaveText(int nlength)
{
	int nAllNoteMem = 0;
	nAllNoteMem = Notepad_MemManage();
	if(nAllNoteMem/1024 < 300) //Pre-allocation for Notepad 300kb		
	{
		if(SPACE_AVAILABLE == IsFlashAvailable(nlength/1024 + 1))
			return 1;
		else
			return 0;	
	}
	else
		return 0;
}




//////////////////////////////////////////////////////////////////////////////
// Function	GetTimeStr
// Purpose	get time sting in a format which is made sure in setup application
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
void GetTimeStr(SYSTEMTIME FileSysTime, char *cResult)
{
	char cTime[10];
	char cDate[20];

	GetTimeDisplay(FileSysTime, cTime, cDate);
	strcpy(cResult, cDate);
	 strcat(cResult, "  ");
	strcat(cResult, cTime);
}
//////////////////////////////////////////////////////////////////////////////
// Function	Notepad_MemManage
// Purpose	get file size
// Params	
// Return	all file use space.
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int Notepad_MemManage(void)
{

	char cFilename[TEXT_FILENAME_LENGTH + 1]; //should be 19+18+1
	struct dirent *pdirent =NULL;
	DIR *pdir = NULL;
	int nAllSize = 0;
	
	chdir(TEXT_PATH_FLASH);
	pdir = opendir(TEXT_PATH_FLASH);
	if(pdir == NULL)
	{
		mkdir(TEXT_PATH_FLASH, 0);
		nAllSize = 0;
		return nAllSize;
	}
	pdirent = readdir(pdir);
	if(pdirent == NULL)
	{
		nAllSize = 0;
		closedir(pdir);
		return nAllSize;
	}
	do {
		strcpy(cFilename,pdirent->d_name);
		if(IsWantedFile(cFilename, TEXT_FILE_TYPE))	
			nAllSize =  nAllSize + GetTextFileLength(cFilename);
		pdirent = readdir(pdir);
	}while(pdirent !=NULL );
	
	closedir(pdir);
	return nAllSize;
}
//////////////////////////////////////////////////////////////////////////////
// Function	IsWantedFile
// Purpose	judge whether to need this file type or not
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
int IsWantedFile(char * FileName, char * pWantedFile)
{
	char	*Suffix;
	Suffix = strrchr(FileName, '.');
	if (!Suffix) {
		
		return 0;
	}
	if ((strcmp(Suffix, pWantedFile) == 0))
	{		
		return 1;
	}
	else
		return 0;
}
