/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements Common Dialog Proc.
 *            
\**************************************************************************/

#include "window.h"
#include "hpwin.h"
#include "hpfile.h"
#include "string.h"
#include "commdlg.h"

#define DLG_ITEMS_NUM       15
#define ID_STATIC_FIND      201
#define ID_COMBO_FIND        202
#define ID_BUTTON_UP        203
#define ID_LISTBOX          204 
#define ID_STATIC_FILENAME   205
#define ID_STATIC_FILESTYLE 206
#define ID_EDIT_FILENAME    207
#define ID_COMBO_FILESTYLE  208

#define MAX_PATH            256

static BOOL SetCurDir (char *path);
static BOOL CALLBACK FileDlgProc (HWND hDlg, UINT wMsgCmd, WPARAM wParam,
                                  LPARAM lParam);
static BOOL RefreshCombox (HWND hDlg, LPARAM lParam);
static char *GetParentDir(char *path);

typedef struct 
{
    DWORD   type;               // Dialog template type, must be zero
    DWORD   style;              // Dialog style
    DWORD   dwExStyle;          // Dialog extend style
    DWORD   cdit;               // Count of dialog control items
    short   x;                  // x-coordinates of dialog left-top corner
    short   y;                  // y-coordinates of dialog left-top corner
    short   cx;                 // width of dialog left-top corner
    short   cy;                 // height of dialog left-top corner
    PCSTR   lpszMenuName;       // Dialog menu name
    PCSTR   lpszClassName;      // Dialog class name
    PCSTR   lpszWindowText;     // Dialog title
    DLGITEMTEMPLATE dlgItems[DLG_ITEMS_NUM];
} TESTDLGTEMPLATE;

const TESTDLGTEMPLATE FileDlgTemplate = 
{    
    0, WS_VISIBLE | WS_DLGFRAME | WS_CAPTION, 0, DLG_ITEMS_NUM,
        30, 30, 430, 250, NULL, NULL, "打开",
        
    {
       {WS_VISIBLE, 0,
            20, 8, 40, 20,
            ID_STATIC_FIND, "STATIC",
            "搜索",
        },
            
        {WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, 0,
        65, 6, 270, 117,
        ID_COMBO_FIND, "COMBOBOX",
        "",
        },
        
        {WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0,
         343, 6, 70, 22,
         ID_BUTTON_UP, "BUTTON",
         "向上",
        },
                
        {WS_VISIBLE | LBS_MULTICOLUMN | LBS_SORT | WS_TABSTOP | WS_HSCROLL, 0,
         10, 32, 410, 120,
         ID_LISTBOX, "LISTBOX",
         "",
        },
        
        {WS_VISIBLE, 0,
         20, 170, 50, 22,
	 ID_STATIC_FILENAME, "STATIC",
         "文件名",
        },
                        
        {WS_VISIBLE, 0,
         20, 200, 50, 22,
         ID_STATIC_FILESTYLE, "STATIC",
         "文件类型",
        },
                            
        {WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP, 0,
         92, 168, 224, 22,
         ID_EDIT_FILENAME, "EDIT",
         "",
        },
                                
        {WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, 0,
         92, 198, 224, 80,
         ID_COMBO_FILESTYLE, "COMBOBOX",
         "",
        },
                                    
        {WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, 0,
         336, 168, 74, 22,
         IDOK, "BUTTON",
         "打开",
        },
                                        
        {WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0,
         336, 198, 74, 22,
         IDCANCEL, "BUTTON",
         "取消",
        }
    }
};

static char * GetParentDir(char *path)
{
    static char s[MAX_PATH];
    char *p = NULL;
    int len = strlen (path);
    
    if (strcmp(path, "/") == 0)
        return path;
    
    if (path[len - 1] == '/')
    {
        p = path + (len - 2);	//eat up a "/"
        while (*p != '/')
            p--;
        memcpy (s, path, p - path + 1);
        s[p - path + 1] = '\0';
        return s;
    }
    
    if (PLXOS_IsFileType(path, PLXFS_IFDIR))
    {
        p = path + (len - 1);
        while (*p != '/')
            p--;
        memcpy (s, path, p - path + 1);
        s[p - path + 1] = '\0';
        return s;
    }
    else if (PLXOS_IsFileType(path, PLXFS_IFREG))
    {
        p = path + (len - 1);
        //eat up two "/"
        while (*p != '/')
            p--;
        p--;
        while (*p != '/')
            p--;
        memcpy (s, path, p - path + 1);
        s[p - path + 1] = '\0';
        return s;
    }
    else
    {
        return NULL;
    }
}


static BOOL RefreshCombox(HWND hDlg, LPARAM lParam)
{
    int index, nlen;
    char pBuffer[MAX_PATH];

    strncpy(pBuffer, (char *)lParam, MAX_PATH);
    nlen = strlen(pBuffer);
    index = nlen - 1;
    while (pBuffer[index] != '/')
    {
        index --;
    }
    if ((pBuffer[index] == '/') && (pBuffer[index - 1] == '.')
        && (pBuffer[index - 2] == '.'))
    {
        while (pBuffer[index - 3] != '/')
		index --;
        pBuffer[index - 2] = '\0';
    }
    else if ((pBuffer[index] == '/') && 
        (pBuffer[index - 1] == '.'))
        return FALSE;
    else 
        pBuffer[index + 1] = '\0';
    index = SendDlgItemMessage (hDlg, ID_COMBO_FIND,
        CB_FINDSTRINGEXACT, 0, (LPARAM) pBuffer);
    if (index < 0)
    {
        SendDlgItemMessage (hDlg, ID_COMBO_FIND,
            CB_ADDSTRING, 0, (LPARAM) pBuffer);
        index =	SendDlgItemMessage(hDlg, ID_COMBO_FIND,
			CB_GETCOUNT, 0, 0);
        SendDlgItemMessage(hDlg, ID_COMBO_FIND,
			CB_SETCURSEL, index - 1, 0);
    }
    else
        SendDlgItemMessage (hDlg, ID_COMBO_FIND, CB_SETCURSEL, index, 0);
    
    return TRUE;
}

//----------------------------------------------------------
//
// FUNCION: GetOpenFileName  
//
// Comment:
// open File dialog, return true or false 
//
//----------------------------------------------------------


BOOL WINAPI
GetOpenFileName (LPOPENFILENAME lpofn)
{
    int nResult;
    
    nResult = DialogBoxIndirectParam (NULL, (LPDLGTEMPLATE) & FileDlgTemplate,
				    lpofn->hwndOwner, FileDlgProc,
                    (LPARAM) lpofn);
    return nResult;
}

static BOOL CALLBACK FileDlgProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, 
                                 LPARAM lParam)
{
    char    pBuffer[MAX_PATH];
    static  LPOPENFILENAME  lpofn;
    char *  SecondFilter;
    char    FirstFilter[MAX_PATH];
    int     nStrLen;
    static int init = 1, curlist = -1;
    char Filename[MAX_PATH];
    int index, retval;
    int nlen, i;
    char * p;
    char Curpath[MAX_PATH];
    static char cur_path[MAX_PATH];
    static int indinlist;

    
    switch (wMsgCmd)
    {
    case WM_INITDIALOG :
        lpofn = (LPOPENFILENAME)lParam;
        SecondFilter = lpofn->lpstrFilter;
        
        while (*SecondFilter != '\0')
        {
            strcpy(FirstFilter, SecondFilter);
            nStrLen = strlen(FirstFilter);
            SecondFilter = SecondFilter + nStrLen + 1;
            strcat(FirstFilter, " ");
            strcat(FirstFilter, SecondFilter);
            SendDlgItemMessage(hDlg, ID_COMBO_FILESTYLE, CB_ADDSTRING, NULL,
                (LPARAM)FirstFilter);
            nStrLen = strlen(SecondFilter);
            SecondFilter = SecondFilter + nStrLen + 1;
        }
        
        if (lpofn->lpstrInitialDir)
        {
            strcpy(Curpath, lpofn->lpstrInitialDir);
        }
        else
        {
            retval = PLXOS_GetCurrentDirectory(Curpath, MAX_PATH);
            if (retval > MAX_PATH -1)
            {
                ASSERT(0);
                break;
            }
        }
        if (Curpath[strlen(Curpath) - 1] != '/')
            strcat(Curpath, "/");
        strcpy(cur_path, Curpath);
        RefreshCombox(hDlg, (LPARAM) Curpath);
        
        SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_SETCURSEL, 0, NULL);
        //index =SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETCURSEL, 0, NULL);
        SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETLBTEXT, 0,
            (LPARAM) FirstFilter);
        nlen = strlen (FirstFilter);
        for (i = nlen; i >= 0; i--)
        {
            if (FirstFilter[i] == ' ')
                break;
        }
        p = FirstFilter + i + 1;
        strcat(Curpath, p);
        SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
        SendDlgItemMessage (hDlg, ID_LISTBOX, LB_DIR,
            DDL_READONLY | DDL_DIRECTORY, (LPARAM) Curpath);
        indinlist = 0;
        
        
        return FALSE;
        
    case WM_COMMAND:

        switch (LOWORD (wParam))
        {
        case ID_COMBO_FILESTYLE:
            if (HIWORD (wParam) == CBN_SELCHANGE)
            {

                index = SendDlgItemMessage(hDlg, ID_COMBO_FIND, 
                    CB_GETCURSEL, NULL, NULL);
                SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETLBTEXT, 
                    (WPARAM)index, (LPARAM)FirstFilter);
                index = SendDlgItemMessage(hDlg, ID_COMBO_FILESTYLE, 
                    CB_GETCURSEL, NULL, NULL);
                SendDlgItemMessage(hDlg, ID_COMBO_FILESTYLE, CB_GETLBTEXT, 
                    (WPARAM)index, (LPARAM)pBuffer);
                nlen = strlen (pBuffer);
                for (i = nlen; i >= 0; i--)
                {
                    if (pBuffer[i] == ' ')
                        break;
                }
                p = pBuffer + i + 1;
                strcat(FirstFilter, p);
                SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
                SendDlgItemMessage(hDlg, ID_LISTBOX, LB_DIR, 
                    DDL_READONLY | DDL_DIRECTORY, (LPARAM)FirstFilter);
            }
            break;
            
        case ID_COMBO_FIND:
            if (HIWORD (wParam) == CBN_SELCHANGE)
            {
                index = SendDlgItemMessage(hDlg, ID_COMBO_FIND, 
                    CB_GETCURSEL, NULL, NULL);
                SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETLBTEXT, 
                    (WPARAM)index, (LPARAM)FirstFilter);
                
                index =SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETCURSEL, NULL,
                    NULL);
                SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETLBTEXT,
                    (WPARAM) index, (LPARAM) pBuffer);
                
                nlen = strlen (pBuffer);
                for (i = nlen; i >= 0; i--)
                {
                    if (pBuffer[i] == ' ')
                        break;
                }
                p = pBuffer + i + 1;
                strcat(FirstFilter, p);
                
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_DIR, 
                    DDL_READONLY | DDL_DIRECTORY, (LPARAM) FirstFilter);
            }
            break;
        case ID_BUTTON_UP:
            if (HIWORD (wParam) == BN_CLICKED)
            {
                char parent_path[MAX_PATH];

                index = SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETCURSEL,
                    NULL, NULL);
                SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETLBTEXT,
                    (WPARAM)index, (LPARAM)cur_path);
                strcpy(parent_path, GetParentDir(cur_path));
		
                if (parent_path == NULL)
                    break;
                strcpy(cur_path, parent_path);
                RefreshCombox(hDlg, (LPARAM) parent_path);
                index =SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETCURSEL, NULL,
                    NULL);
                SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE, CB_GETLBTEXT,
                    (WPARAM) index, (LPARAM) pBuffer);
                nlen = strlen (pBuffer);
                for (i = nlen; i >= 0; i--)
                {
                    if (pBuffer[i] == ' ')
                        break;
                }
                p = pBuffer + i + 1;
                strcat(parent_path, p);
                
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_DIR,
                    DDL_READONLY | DDL_DIRECTORY, (LPARAM) parent_path);
                
                
                
            }
            break;

        case IDOK:
            if (HIWORD (wParam) == BN_CLICKED)
            {
                SendDlgItemMessage (hDlg, ID_EDIT_FILENAME, WM_GETTEXT,
                    MAX_PATH, (LPARAM) Filename);
                if (Filename[0] != '\0')
                {
                    index = SendDlgItemMessage(hDlg, ID_COMBO_FIND, 
                        CB_GETCURSEL, NULL, NULL);
                    SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETLBTEXT, 
                        (WPARAM)index, (LPARAM)pBuffer);

                    
                    if (Filename[0] != '/')
                    {
                        if (pBuffer[strlen (pBuffer) - 1] != '/')
                            strcat (pBuffer, "/");
                        strcat (pBuffer, Filename);
                    }
                    else
                    {
                        if (pBuffer[strlen (pBuffer) - 1] != '/')
                            strcat (pBuffer, Filename);
                        else
                        {
                            pBuffer[strlen (pBuffer) - 1] = '\0';
                            strcat(pBuffer, Filename);
                        }

                    }
                    
                    if (PLXOS_IsFileType(pBuffer, PLXFS_IFREG))
                    {
                        strcpy (lpofn->lpstrFile, pBuffer);
                        EndDialog (hDlg, TRUE);
                        return TRUE;
                    }
                    else if (PLXOS_IsFileType(pBuffer, PLXFS_IFDIR)||PLXOS_IsFileType(pBuffer, PLXFS_IFLNK) ) 
                    {
                        
                        if (pBuffer[strlen (pBuffer) - 1] != '/')
                            strcat (pBuffer, "/");
                        
                        index = SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE,
                            CB_GETCURSEL, 0, NULL);
                        SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE,
                            CB_GETLBTEXT, index,
                            (LPARAM) FirstFilter);
                        nlen = strlen (FirstFilter);
                        for (i = nlen; i >= 0; i--)
                        {
                            if (FirstFilter[i] == ' ')
                                break;
                        }
                        p = FirstFilter + i + 1;
                        strcat(pBuffer, p);
                        SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
                        SendDlgItemMessage (hDlg, ID_LISTBOX, LB_DIR,
                            DDL_READONLY | DDL_DIRECTORY, (LPARAM) pBuffer);
                        
                        RefreshCombox(hDlg, (LPARAM)pBuffer);

                        break;
                    }
                }
            }
            break;
            
        case IDCANCEL:
            EndDialog (hDlg, FALSE);
            return TRUE;
            
        case ID_LISTBOX:

            if (HIWORD(wParam) == LBN_SETFOCUS)
            {
                index = SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0);
                if (index == -1)
                    SendDlgItemMessage (hDlg, ID_LISTBOX, LB_SETCURSEL, indinlist, 0);
            }
            else if (HIWORD(wParam) == LBN_KILLFOCUS)
            {
                indinlist = SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0);
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_SETCURSEL, -1, 0);
            }
            
            if (HIWORD (wParam) == LBN_SELCHANGE)
            {
                index = SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0);
                if (index == -1)
                    break;
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETTEXT, index,
                    (LPARAM) Filename);
                SendDlgItemMessage (hDlg, ID_EDIT_FILENAME, WM_SETTEXT, 0,
                    (LPARAM) Filename);
            }
            if (HIWORD (wParam) == LBN_DBLCLK)
            {
                int index;
                
                index = SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0);
                SendDlgItemMessage (hDlg, ID_LISTBOX, LB_GETTEXT, index,
                    (LPARAM) Filename);
                
                if (Filename[0] != '\0')
                {
                    index = SendDlgItemMessage(hDlg, ID_COMBO_FIND, 
                        CB_GETCURSEL, NULL, NULL);
                    SendDlgItemMessage(hDlg, ID_COMBO_FIND, CB_GETLBTEXT, 
                        (WPARAM)index, (LPARAM)pBuffer);
                    
                    if (Filename[0] != '/')
                    {
                        if (pBuffer[strlen (pBuffer) - 1] != '/')
                            strcat (pBuffer, "/");
                        strcat (pBuffer, Filename);
		    }
		    else
		    {
			    if (pBuffer[strlen(pBuffer) - 1] == '/')
				    pBuffer[strlen(pBuffer) - 1] = '\0';
			    strcat(pBuffer, Filename);
		    }
                    
                    if (PLXOS_IsFileType(pBuffer, PLXFS_IFREG))
                    {
                        strcpy (lpofn->lpstrFile, pBuffer);
                        EndDialog (hDlg, TRUE);
                        return TRUE;
                    }
                    else if (PLXOS_IsFileType(pBuffer, PLXFS_IFDIR)||PLXOS_IsFileType(pBuffer, PLXFS_IFLNK) ) 
                    {
                        
                        if (pBuffer[strlen (pBuffer) - 1] != '/')
                            strcat (pBuffer, "/");
                        
                        index = SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE,
                            CB_GETCURSEL, 0, NULL);
                        SendDlgItemMessage (hDlg, ID_COMBO_FILESTYLE,
                            CB_GETLBTEXT, index,
                            (LPARAM) FirstFilter);
                        nlen = strlen (FirstFilter);
                        for (i = nlen; i >= 0; i--)
                        {
                            if (FirstFilter[i] == ' ')
                                break;
                        }
                        p = FirstFilter + i + 1;
                        strcat(pBuffer, p);
//                        SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, NULL, NULL);
        
	
//			SendDlgItemMessage (hDlg, ID_LISTBOX, LB_DIR,
  //                          DDL_READONLY | DDL_DIRECTORY, (LPARAM) pBuffer);
                        RefreshCombox(hDlg, (LPARAM)pBuffer);                       
                        
                        break;
                    }
                }
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}


//----------------------------------------------------------
//
// FUNCION: GetSaveFileName  
//
// Comment:
// open save dialog, return true or false 
//
//----------------------------------------------------------

BOOL WINAPI
GetSaveFileName (LPOPENFILENAME lpofn)
{
    
    return FALSE;
}

/* end of commdlg.c */

