   /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : alert group select window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"
#include    "callselalertgrp.h"
#include    "PhonebookExt.h"



static const char * pClassName = "AlertGrpSelectionClass";

static int * iSelItemIndex;
static SCENEMODE sm;
static HWND         hAppWnd=NULL;
static HWND         hFrameWin=NULL;
static HWND         hFatherWnd=NULL;
static int          iCurIndex;    
static    int nGrpNum = 0;
static    GROUP_INFO * GrpInfo = NULL;
static int iHighLitPos = 0;
static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CreateControl (HWND hWnd, HWND *hGrpList);
static BOOL    LoadGrpList(HWND hList);
/***************************************************************
* Function  CallSelAlertGrp
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/

BOOL    CallSelAlertGrp(HWND hwndCall, HWND hFather, int icurindex)
{
    WNDCLASS    wc;
    RECT rClient;
    
    hFrameWin = hwndCall;
    iCurIndex = icurindex;
	hFatherWnd = hFather;
    GetSM(&sm, iCurIndex);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    GetClientRect(hFrameWin, &rClient);

    hAppWnd = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  

                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }    
   
    SetWindowText(hFrameWin,ML("Select groups"));
    
    SetFocus(hAppWnd);

    return (TRUE);
}
/***************************************************************
* Function  AppWndProc
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    int     nSelNum, i, j;
    static HWND     hGrpList;
    RECT rcClient;
    LRESULT         lResult;
    HDC hdc;
	int  OldStyle; 
    lResult = TRUE; 
    
    switch ( wMsgCmd )
    {
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetWindowText(hFrameWin,ML("Select groups"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		if (0 == nGrpNum)
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hFrameWin , PWM_SETBUTTONTEXT,1, (LPARAM)"");//LSK                
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");       
			break;
		}
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT,1, (LPARAM)ML("Save"));//LSK                
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");       
       
        break;
        
	case WM_PAINT:
		if (0 == nGrpNum)
		{
			BeginPaint(hWnd, NULL);
			hdc = GetDC(hWnd);
			GetClientRect(hWnd, &rcClient);
			OldStyle = SetBkMode(hdc, BM_TRANSPARENT);
			DrawText(hdc, ML("No groups"), -1,  &rcClient, DT_HCENTER | DT_VCENTER);
			SetBkMode(hdc, OldStyle);
			ReleaseDC(hWnd, hdc);		
			EndPaint(hWnd, NULL);		
		}
		else 
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
		break;

    case WM_CREATE:
		iHighLitPos = 0;
        APP_GetGroupInfo(NULL, &nGrpNum);
		
		SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_CANCEL,(LPARAM)ML("Cancel"));
		SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_SAVE), (LPARAM)ML("Save"));//LSK                
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
                        
        if ( 0 == nGrpNum )
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");       
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT,1, (LPARAM)"");//LSK                
			break;
		}
        
        GrpInfo = (GROUP_INFO*)malloc(nGrpNum * sizeof(GROUP_INFO));

        if(NULL == GrpInfo)
        {
            return FALSE;
        }
 
        memset(GrpInfo, 0, nGrpNum * sizeof(GROUP_INFO));

        APP_GetGroupInfo(GrpInfo, &nGrpNum);

        CreateControl(hWnd, &hGrpList);

        SendMessage(hGrpList, LB_BEGININIT, 0, 0);

        for (i=0;i<nGrpNum;i++)
        {
            SendMessage(hGrpList, LB_ADDSTRING, 0, (LPARAM)GrpInfo[i].szGroupName);
        }
        
        SendMessage(hGrpList, LB_ENDINIT, 0, 0);

		if (sm.iAlertFor == ALERTSELECT)
		{
			for (i=0;i<nGrpNum;i++)
			{
				for (j=0; j<SETT_MAX_ABGRP_NUM; j++)
				{
					if ( 0 == sm.iAlertGrpID[j] )
					{
						break;
					}
					else if (sm.iAlertGrpID[j] == GrpInfo[i].nGroupID)
					{
						SendMessage(hGrpList, LB_SETSEL, (WPARAM)TRUE, i);
						if (iHighLitPos == 0)
						{
							SendMessage(hGrpList, LB_SETCURSEL, i, 0);
							iHighLitPos++;
						}
					}
				}
			}
		}
		else
		{
			for (i = 0; i < nGrpNum; i++)
			{
				SendMessage(hGrpList, LB_SETSEL, (WPARAM)TRUE, i);
			}
		}

		if (iHighLitPos == 0)
		{
			SendMessage(hGrpList, LB_SETCURSEL, 0, 0);
		}
/*
        if(sm.iAlertFor == ALERTALL)
        {
        } //check all groups
        
        if(sm.iAlertFor == ALERTNO)
        {
        }  //check none
*/

        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_GRPLIST));
        break;
    
    case WM_DESTROY : //Quit the application;
        if(iSelItemIndex)
            free(iSelItemIndex);
        if(GrpInfo)
            free(GrpInfo);
        iSelItemIndex = NULL;
        GrpInfo = NULL;
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
			if (0 == nGrpNum)
				break;
            SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_SAVE, 0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
        
        case WM_COMMAND :
            switch (LOWORD(wParam))
            {
            case IDC_BUTTON_SAVE:
                if ( 0 == nGrpNum )
                {
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                }
                nSelNum = SendMessage(hGrpList, LB_GETSELCOUNT, 0, 0);

                iSelItemIndex = (int*)malloc(nSelNum * sizeof(int));

                if ( NULL == iSelItemIndex )
                    return FALSE;
                
                SendMessage(hGrpList, LB_GETSELITEMS, nSelNum, (LPARAM)iSelItemIndex);
                
                if(nSelNum > SETT_MAX_ABGRP_NUM) //too many groups
                    break;

                memset(sm.iAlertGrpID, 0, SETT_MAX_ABGRP_NUM * sizeof(int));
                
                for(i=0;i<nSelNum;i++)
                {                   
                    sm.iAlertGrpID[i] = GrpInfo[iSelItemIndex[i]].nGroupID; 
                }
                sm.iAlertFor = ALERTSELECT;
                SetSM(&sm, iCurIndex);
				PostMessage(hFrameWin, PWM_CLOSEWINDOW, (WPARAM)hFatherWnd, 0);
				PostMessage(hFatherWnd, WM_CLOSE, 0, 0);
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            default:
                break;
            }            
            break;

        default :
             lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
             break;
	}
return lResult;
}
/***************************************************************
* Function  CreateControl
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
static BOOL CreateControl (HWND hWnd, HWND *hGrpList)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    *hGrpList = CreateWindow("MULTILISTBOX", NULL, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right,rect.bottom,
        hWnd, (HMENU)IDC_GRPLIST, NULL, NULL);
    if ( *hGrpList == NULL )
        return FALSE;

    return TRUE;
}
