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

#include "MulLang.h"
#include "window.h"
#include "malloc.h"
#include "string.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define ML_STRING_LEN       256
#define PATH_MAXLEN         256
#define ML_FREE(p)          do { if(p) free(p); p=NULL;} while(0)
#define FILE_ROM_MULTILANGUAGE      "/rom/multilanguage.ml"  
#define FILE_FLASH_MULTILANGUAGE    "/mnt/flash/multilanguage.ml"  
#define PATH_DIR_ML                 "/mnt/flash/ml" 
#define PATH_FILE_LAUNGUAGE         "/mnt/flash/ml/language.inf"

#define ML_OFFSET_KEYNUM		     7			//存放key数量的偏移量
#define ML_OFFSET_LANGUAGENUM       27			//存放语言数量的偏移量
#define ML_OFFSET_KEYINDEX  		15			//key偏移区入口偏移量
#define ML_OFFSET_KEY   			19			//key字符串区入口偏移量
#define ML_OFFSET_LANGUAGE  		31			//语言名称入口偏移量

typedef struct tagML_Header{ 
    DWORD   dwSize; 				//   size
    int     nLanguageCount;         //   language count
    int     nKeyCount;		        //   key count
    DWORD   dwLanguageOffset;	    //   language offset
    DWORD   dwKeyIndexOffset;		//   key index offset
    DWORD 	dwKeyOffset;			//   key info offset
}ML_HEADER;

static TCHAR** szLanguages;
static WCHAR** szStringTable;
static BYTE* pbyFile;

static TCHAR g_szActiveLanguage[ML_STRING_LEN];
static int nCurLanguage;
static MSG Msg;
static BOOL bLoad;
static ML_HEADER header;

static BOOL ML_Read(TCHAR* pszLanguage,int nMaxLen);
BOOL ML_Write(TCHAR* pszLanguage);
static BOOL CALLBACK DispatchLanguageChangedMsg(HWND hWnd,LPARAM lParam );
static int  ML_SetActiveLanguage(const TCHAR* pszLanguage,BOOL bAPP);
/*********************************************************************\
* Function	ML_Init
* Purpose   Load string data
* Params
*		
* Return    
**********************************************************************/
int ML_Init(void)
{
    int f;
    int i,j;
    LPBYTE tmp;
    DWORD* pindex;
    char   mask[10];
    struct stat statbuf;
    int nLanguageSize,nStringTableSize;
    TCHAR sztemp[ML_STRING_LEN];

    bLoad = FALSE;
    g_szActiveLanguage[0] = 0;    
    nCurLanguage = 0;
    memset(&statbuf,0,sizeof(struct stat));

    f = open(FILE_FLASH_MULTILANGUAGE,O_RDONLY);

    if(f == -1)
    {
        f = open(FILE_ROM_MULTILANGUAGE,O_RDONLY);

        if(f == -1)
            return FALSE;
        
        stat(FILE_ROM_MULTILANGUAGE,&statbuf);
    }
    else
        stat(FILE_FLASH_MULTILANGUAGE,&statbuf);

    memset(&header,0,sizeof(ML_HEADER));

    if(read(f,mask,3) == -1)
    {
        close(f);
        return FALSE;
    }

    if(stricmp(mask,"ML") != 0)
        return FALSE;
 
    lseek(f,ML_OFFSET_KEYNUM,SEEK_SET);
    read(f,&(header.nKeyCount),sizeof(int));
    lseek(f,ML_OFFSET_KEYINDEX,SEEK_SET);
    read(f,&(header.dwKeyIndexOffset),sizeof(DWORD));
    lseek(f,ML_OFFSET_KEY,SEEK_SET);
    read(f,&(header.dwKeyOffset),sizeof(DWORD));
    lseek(f,ML_OFFSET_LANGUAGENUM,SEEK_SET);
    read(f,&(header.nLanguageCount),sizeof(int));
    header.dwLanguageOffset = ML_OFFSET_LANGUAGE;
    
    header.nLanguageCount--; // key

    header.dwSize = statbuf.st_size;

    nLanguageSize = header.nLanguageCount * sizeof(TCHAR*);
    nStringTableSize = (1+header.nLanguageCount) * header.nKeyCount * sizeof(WCHAR*);

    szLanguages = (TCHAR**)malloc(nLanguageSize);
    szStringTable = (WCHAR**)malloc(nStringTableSize);
    pbyFile = (BYTE*)malloc(header.dwSize);
    if(szLanguages == NULL || szStringTable == NULL || pbyFile == NULL)
    {
        close(f);
        ML_FREE(szLanguages);
        ML_FREE(szStringTable);
        ML_FREE(pbyFile);
        return FALSE;
    }
    memset(szLanguages,0,nLanguageSize);
    memset(szStringTable,0,nStringTableSize);
    memset(pbyFile,0,header.dwSize);

    lseek(f,0,SEEK_SET);

    if(read(f,pbyFile,header.dwSize) == -1)
    {
        close(f);
        ML_FREE(szLanguages);
        ML_FREE(szStringTable);
        ML_FREE(pbyFile);
        return FALSE;
    }

    close(f);

    tmp = pbyFile+header.dwLanguageOffset;
    if(tmp && tmp<(pbyFile+header.dwSize))
        tmp += strlen(tmp) + 1;

    for(i=0 ; i<header.nLanguageCount ; i++)
    {
        if(tmp && tmp<(pbyFile+header.dwSize))
        {
            szLanguages[i] = (TCHAR*)tmp;
            tmp += strlen(tmp) + 1;
        }
        else
        {
            ML_FREE(szLanguages);
            ML_FREE(szStringTable);
            ML_FREE(pbyFile);
            return FALSE;
        }
    }

    pindex = (DWORD*)(pbyFile+header.dwKeyIndexOffset);
    tmp = pbyFile+header.dwKeyOffset;
    for(i=0 ; i<header.nKeyCount ; i++)
    {
        tmp = pbyFile+pindex[i];
        for(j=0 ; j<=header.nLanguageCount ; j++)
        {
            if(tmp && tmp<(pbyFile+header.dwSize))
            {
                szStringTable[i*(header.nLanguageCount+1)+j] = (WCHAR*)tmp;
                tmp += strlen(tmp) + 1;
            }
        }
    }
    
    bLoad = TRUE;
    
    sztemp[0] = 0;

    if(ML_Read(sztemp,ML_STRING_LEN))
    {
        ML_SetActiveLanguage(sztemp,FALSE);
    }

    return TRUE;
}
/*********************************************************************\
* Function	ML_Destroy
* Purpose   Free
* Params
*		
* Return    
**********************************************************************/
void ML_Destroy(void)
{
    if(bLoad == TRUE)
    {  
        ML_FREE(szLanguages);
        ML_FREE(szStringTable);
        ML_FREE(pbyFile);
        bLoad = FALSE;
        g_szActiveLanguage[0] = 0;    
        nCurLanguage = 0;
    }
}
/*********************************************************************\
* Function	SetActiveLanguage
* Purpose   Set active Language 
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
int SetActiveLanguage(const TCHAR* pszLanguage)
{
    return(ML_SetActiveLanguage(pszLanguage,TRUE));
}
/*********************************************************************\
* Function	ML_SetActiveLanguage
* Purpose   Set active Language 
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
static int ML_SetActiveLanguage(const TCHAR* pszLanguage,BOOL bAPP)
{
    int i;
    int nRet = FALSE;

    if(bLoad == FALSE)
        return FALSE;

    for(i = 0 ; i < header.nLanguageCount ; i++)
    {
        if(stricmp((const char*)pszLanguage,(const char*)szLanguages[i]) == 0)
        {
            nCurLanguage = i+1;
            nRet = TRUE;
            break;
        }
    }

    if(!nRet)
        return FALSE;

    if(bAPP)
    {
        if(ML_Write((TCHAR*)pszLanguage) == FALSE)
            return FALSE;
        
        strncpy(g_szActiveLanguage,(TCHAR*)pszLanguage,ML_STRING_LEN);
        g_szActiveLanguage[ML_STRING_LEN-1] = 0;
                
        //WM_LANGUAGECHANGED send to all of popup windows
        memset(&Msg,0,sizeof(MSG));
        Msg.message = WM_LANGUAGECHANGED;
        Msg.wParam = NULL;
        Msg.lParam = NULL;
        
        EnumWindows(DispatchLanguageChangedMsg,(LPARAM)&Msg);
        
        return TRUE;
    }
    else
    {        
        strncpy(g_szActiveLanguage,(TCHAR*)pszLanguage,ML_STRING_LEN);
        g_szActiveLanguage[ML_STRING_LEN-1] = 0;

        return TRUE;
    }

}
/*********************************************************************\
* Function	 DispatchLanguageChangedMsg
* Purpose    callback function of EnumWindows   
* Parameters
*            hwnd 
*            [in] Handle to a top-level window. 
*            lParam 
*            [in] Specifies the application-defined value given in EnumWindows
*            or EnumDesktopWindows. 
*
* Return     
**********************************************************************/
static BOOL CALLBACK DispatchLanguageChangedMsg(HWND hWnd,LPARAM lParam )
{
    WNDPROC pfnWndProc;
    PMSG    pMsg;

    pMsg = (PMSG)lParam;

    pfnWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    if (pfnWndProc)
        pfnWndProc(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    
    return TRUE;
}
/*********************************************************************\
* Function	GetActiveLanguage
* Purpose   Set active Language 
* Params
*		
* Return    
**********************************************************************/
const TCHAR* GetActiveLanguage(void)
{
    return(g_szActiveLanguage);
}

/*********************************************************************\
* Function	Translate
* Purpose   Translate the key to string 
* Params
*		    pStr : the key
*           
* Return    the string
**********************************************************************/
int ML_stricmp(const char * dst, const char * src)
{
    int f,l;
    do {
        if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
            f -= ('A' - 'a');
        
        if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
            l -= ('A' - 'a');
    } while ( f && (f == l) );
    
    return(f - l);
}
/*********************************************************************\
* Function	Translate
* Purpose   Translate the key to string 
* Params
*		    pStr : the key
*           
* Return    the string
**********************************************************************/
const TCHAR* Translate(const TCHAR* pStr)
{
    int nLanguage;
    int nmiddle,nstart,nend;
    int nCmpRet;
    int nRecordSize;
    TCHAR* PSZCMPSTRING;

    if(bLoad == FALSE)
        return pStr;

    nLanguage = header.nLanguageCount;
    nRecordSize = nLanguage+1;
    
    nstart = 0;
    nend = header.nKeyCount;
    nend -= 1;

    while(nend >= nstart)
    {
        nmiddle = (nend+nstart)/2;
        PSZCMPSTRING = (TCHAR*)szStringTable[nmiddle*nRecordSize];
        nCmpRet = ML_stricmp(pStr,PSZCMPSTRING);
        if(nCmpRet == 0)
        {
            return((const TCHAR*)szStringTable[nmiddle*nRecordSize+nCurLanguage]);
        }
        else if(nCmpRet > 0)
            nstart = nmiddle+1;
        else
            nend = nmiddle-1;
    }

    return pStr;
}
/*********************************************************************\
* Function	GetLanguageInfo
* Purpose   Get Language count and string
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
int GetLanguageInfo(TCHAR*** pszLanguage,int* pnCount)
{
    if(bLoad == FALSE)
        return FALSE;

    if(pszLanguage == NULL)
    {
        *pnCount = header.nLanguageCount;
        return TRUE;
    }
    else
    {
        *pszLanguage = szLanguages;
        return TRUE;
    }

    return TRUE;
}
/*********************************************************************\
* Function	ML_Write
* Purpose   Save Current Languange Setting
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
BOOL ML_Write(TCHAR* pszLanguage)
{
    char szOldPath[PATH_MAXLEN];
    int  f;

    szOldPath[0] = 0;
    
    mkdir(PATH_DIR_ML,S_IRWXU);

    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_ML);

    remove(PATH_FILE_LAUNGUAGE);

    f = open(PATH_FILE_LAUNGUAGE, O_RDWR|O_CREAT, S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }

    write(f,pszLanguage,(strlen(pszLanguage)+1));

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	ML_Read
* Purpose   Read Current Languange Setting
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
static BOOL ML_Read(TCHAR* pszLanguage,int nMaxLen)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    struct stat buf;
    DWORD  dwFileSize;

    szOldPath[0] = 0;
    
    mkdir(PATH_DIR_ML,S_IRWXU);

    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_ML);

    memset(&buf,0,sizeof(struct stat));

    stat(PATH_FILE_LAUNGUAGE,&buf);

    if(buf.st_size == 0)
    {
        chdir(szOldPath);

        return FALSE;
    }
    
    f = open(PATH_FILE_LAUNGUAGE,O_RDONLY);

    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }

    dwFileSize = (buf.st_size) > (unsigned long)(nMaxLen) ? (unsigned long)nMaxLen : buf.st_size;
    
    read(f,pszLanguage,dwFileSize);

    close(f);
        
    chdir(szOldPath);

    return TRUE;
}

