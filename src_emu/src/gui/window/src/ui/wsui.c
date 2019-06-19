/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements init, exit and some control functions about UI.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "hpfile.h"
#include "string.h"

/*************************************************************************/
/*  窗口系统界面对象窗口注册                                             */
/*************************************************************************/

#define WININI_NAME     "win.ini"
#define SYSLANGUEINFO   "LANGUE ="
#define MAX_LANGUENAME  16


// 窗口系统界面对象窗口注册函数原型


#if (!NOMENUS)
BOOL MENU_RegisterClass(void);
#endif

BOOL DEFWND_RegisterClass(void);
BOOL DEFDLG_RegisterClass(void);

BOOL BUTTON_RegisterClass(void);
BOOL EDIT_RegisterClass(void);
BOOL SCROLL_RegisterClass(void);
BOOL LISTBOX_RegisterClass(void);
BOOL STATIC_RegisterClass(void);
BOOL COMBOBOX_RegisterClass(void);
BOOL SPINBOX_RegisterClass(void);
BOOL NUMSPINBOX_RegisterClass(void);
BOOL STRSPINBOX_RegisterClass(void);
BOOL MULTILISTBOX_RegisterClass(void);

static int GetSystemLangue(void);
int         SysLangue;
HINSTANCE   hInsMsgBox;

/*
**  Function : RegisterSystemClass
**  Purpose  :
**      Register system class when the window system initializing.
*/
BOOL WSUI_Init(void)
{
    char PathName[MAX_PATH];
    

    if (!DEFWND_RegisterClass())
    {
        return FALSE;
    }

#if (!NOMENUS)

    if (!MENU_RegisterClass())
        return FALSE;

#endif // NOMENUS

    if (!DEFDLG_RegisterClass())
        return FALSE;

    if (!BUTTON_RegisterClass())
        return FALSE;

    if (!EDIT_RegisterClass())
        return FALSE;

    if (!SCROLL_RegisterClass())
        return FALSE;
    
    if (!LISTBOX_RegisterClass())
        return FALSE;
    
    if (!STATIC_RegisterClass())
        return FALSE;
    
    if (!COMBOBOX_RegisterClass())
        return FALSE;

    if (!NUMSPINBOX_RegisterClass())
        return FALSE;
    
    if (!STRSPINBOX_RegisterClass())
        return FALSE;

    if (!MULTILISTBOX_RegisterClass())
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);

    SysLangue = GetSystemLangue();
    
    hInsMsgBox = NULL;
    if (SysLangue == LANGUE_CHINESE)
    {
        strcat(PathName, "msgchn.res");
        hInsMsgBox = RegisterResource(NULL, PathName, 0, TRUE);
    }
    else if (SysLangue == LANGUE_ENGLISH)
    {
        strcat(PathName, "msgeng.res");
        hInsMsgBox = RegisterResource(NULL, PathName, 0, TRUE);
    }

    return TRUE;
}

void WSUI_Exit(void)
{
    UnregisterResource(hInsMsgBox);
}

static int GetSystemLangue(void)
{
    int     hFile;
    char    *pFile;
    char    *pInfoStart, *pInfoEnd;
    int     nInfoLength;
    DWORD   nFileSize;
    char    FilePath[128];
    char    LangueName[MAX_LANGUENAME];
    int     langue;

    /* get init file name of PLX GUI System */
    PLXOS_GetInitFileName(FilePath);

    hFile = PLXOS_CreateFile(FilePath, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        /* no init file, set default language to ENGLISH */
        return LANGUE_ENGLISH;
    }

    nFileSize = PLXOS_GetFileSize(hFile);
    pFile = (char *)MemAlloc(nFileSize + 1);
    ASSERT(pFile);

    PLXOS_ReadFile(hFile, pFile, nFileSize);
    *(pFile + nFileSize) = '\0';

    pInfoStart = strstr(pFile, SYSLANGUEINFO);
    if (pInfoStart == NULL)
    {
        MemFree(pFile);
        PLXOS_CloseFile(hFile);
        return LANGUE_ENGLISH;
    }

    pInfoEnd = strchr(pInfoStart, '\r');
    if (pInfoEnd == NULL)
        pInfoEnd = pFile + nFileSize;
    pInfoStart += strlen(SYSLANGUEINFO);
    while (*pInfoStart == ' ')
        pInfoStart ++;
    nInfoLength = pInfoEnd - pInfoStart;
    memset(LangueName, 0, MAX_LANGUENAME);
    if (nInfoLength <= MAX_LANGUENAME)
    {
        memcpy(LangueName, pInfoStart, nInfoLength);
        LangueName[nInfoLength] = 0;
    }
    if (!stricmp(LangueName, "CHINESE"))
        langue = LANGUE_CHINESE;
    else
        langue = LANGUE_ENGLISH;

    MemFree(pFile);
    PLXOS_CloseFile(hFile);
    return langue;
}
