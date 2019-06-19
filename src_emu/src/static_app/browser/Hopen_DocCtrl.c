/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : abstract
 *
 * Purpose  : 实现对窗口系统的封装
 *            
\**************************************************************************/
#include "window.h"
#include "string.h"

	#ifdef _WALDI_
		#include "imeasp.h"
	#else
		#include "imesys.h"
	#endif

#include "hpimage.h"
#include "plx_pdaex.h"

#ifndef _WALDI_
#include "winpda.h"
#endif

#include "PWBE.h"
#include "UIData.h"
#include "wUipub.h"
#include "stdio.h"

#define BRSVIEWCLASS   "BrowserEngineView"

extern HWND hViewWnd;
extern BOOL bConnectted;
extern int rbtnTxtID;
extern BOOL bHistory;
extern HBITMAP hwmlconnect, hwmldisconnect;
typedef struct tag_MsgMap
{
	UINT org_msg;
	UINT wb_msg;
}MSG_MAP;

MSG_MAP MsgMapTable[]=
{
	{WM_LBUTTONDOWN,	WBET_PENDOWN},
	{WM_HSCROLL,		WBET_HORZSCROLL},
	{WM_VSCROLL,		WBET_VERTSCROLL},
	{WM_KEYDOWN,		WBET_KEYDOWN},
	{WM_CHAR,			WBET_CHAR},
};

MSG_MAP ScrollMap[] =
{
	{SB_LINEUP,			WBSCROLL_LINEUP},
	{SB_LINEDOWN,		WBSCROLL_LINEDOWN}, 
	{SB_PAGEUP,			WBSCROLL_PAGEUP}, 
	{SB_PAGEDOWN,		WBSCROLL_PAGEDOWN}, 
	{SB_LINELEFT,		WBSCROLL_LINELEFT}, 
	{SB_LINERIGHT,		WBSCROLL_LINERIGHT}, 
	{SB_PAGELEFT,		WBSCROLL_PAGELEFT}, 
	{SB_PAGERIGHT,		WBSCROLL_PAGERIGHT}, 
	{SB_THUMBPOSITION,	WBSCROLL_THUMBPOSITION}, 
	{SB_THUMBTRACK,		WBSCROLL_THUMBTRACK},
};

MSG_MAP VKeyMap[] =
{
	{VK_UP,			WBKEY_UP},
	{VK_DOWN,		WBKEY_DOWN},	
	{VK_RIGHT,		WBKEY_RIGHT},
	{VK_LEFT,		WBKEY_LEFT},
	{VK_PRIOR,		WBKEY_PAGEUP},
	{VK_NEXT,		WBKEY_PAGEDOWN},
	{VK_RETURN,		WBKEY_F5},
	{VK_TAB,		WBKEY_TAB},
	{VK_F10,		WBKEY_BACK},
};


extern HBROWSER  pUIBrowser;

extern int WB_InitFont(void *hWnd);
extern void  WB_ReleaseFont(void *hWnd);

BOOL UI_MessageMap(UINT *msg, unsigned long * wParam, unsigned long * lParam)
{
	int i = 0;
	UINT wbmsg = 0;
	int scrl = 0;
	int keyVal = 0;

	int num = sizeof(MsgMapTable) / sizeof(MSG_MAP);

	while (i < num)
	{
		if (MsgMapTable[i].org_msg == *msg)
		{
			wbmsg = MsgMapTable[i].wb_msg;
			break;
		}
		i++;
	}
	if(i >= num)
		return FALSE;


	if (*msg == WM_VSCROLL || *msg == WM_HSCROLL)
	{
		i = 0;
		num = sizeof(ScrollMap) / sizeof(MSG_MAP);
		while (i < num)
		{
			if (ScrollMap[i].org_msg == (int) LOWORD(*wParam))
			{
				scrl = ScrollMap[i].wb_msg;
				break;
			}
			i++;
		}
		if(i >= num)
			return FALSE;

		*lParam = HIWORD(*wParam); 
		*wParam = scrl;
	}

	if (*msg == WM_KEYDOWN)
	{
		i = 0;
		num = sizeof(VKeyMap) / sizeof(MSG_MAP);
		while (i < num)
		{
			if (VKeyMap[i].org_msg == (int) LOWORD(*wParam))
			{
				keyVal = VKeyMap[i].wb_msg;
				break;
			}
			i++;
		}
		
		if(i >= num)
			return FALSE;

		*wParam = keyVal;
	}

	if (*msg == WM_LBUTTONDOWN)
	{
		int x = LOWORD(*lParam), y = HIWORD(*lParam);
		*wParam = x;
		*lParam = y;
	}

	*msg = wbmsg;

	return TRUE;
}

extern UIDATA brsUIData;
extern HWND hwndIEBMView;
extern HWND hBMlist;
LRESULT CALLBACK BrowserViewProc(HWND hWnd, UINT message, 
								 WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;	
	LRESULT lResult = TRUE;
	
	switch (message) 
	{
	case WM_PAINT:
		{						
			if(bConnectted)
			{	
				SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
					MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmlconnect);
			}
			else 
			{
				SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
					MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmldisconnect);
			}
			
			BeginPaint(hWnd, &ps);
			WB_BeginDisplay((void *)&brsUIData, NULL);//(WB_RECT *)&(ps.rcPaint));
			
			PWBE_RefreshPaint(pUIBrowser, NULL);//(WB_RECT *)&(ps.rcPaint));
			
			WB_EndDisplay((void *)&brsUIData);		
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CREATE:	
		{
			SCROLLINFO  scinfo;
	
			scinfo.cbSize = sizeof(SCROLLINFO);
			scinfo.fMask = SIF_ALL;
			scinfo.nMax =  100;
			scinfo.nMin =  0;
			scinfo.nPage = 0;
			scinfo.nPos =	0;
			scinfo.nTrackPos = 0;

			SetScrollInfo(hWnd, SB_VERT, &scinfo, 1);	
	
			scinfo.nMax = -1;

			SetScrollInfo(hWnd, SB_HORZ, &scinfo, 1);
			EnableScrollBar(hWnd, SB_HORZ, ESB_DISABLE_BOTH);

			WB_InitFont((void*)hWnd);
		}
		break;

	case WM_LBUTTONDOWN:		
	case WM_COMMAND:		
	case WM_TIMER:		
	case WM_ACTIVATE:		
	case WM_SIZE:		
	case WM_HSCROLL:
	case WM_VSCROLL:			
	case WM_CHAR:
		if (UI_MessageMap(&message, &wParam, &lParam))
		{
			PWBE_ProcessEvent(pUIBrowser, message, wParam, lParam);
		}
		break;

	case WM_KEYDOWN:
		{
            switch (wParam)
            {
            case VK_DOWN:
			case VK_UP:	
				printf("__________________Hopen_DocCtrl.c  WM_KEYDOWN______________________\r\n");
			case VK_LEFT:
			case VK_RIGHT:
			case VK_RETURN:
				if (UI_MessageMap(&message, &wParam, &lParam))
				{
					PWBE_ProcessEvent(pUIBrowser, message, wParam, lParam);
				}
				break;

			case VK_F10:
				{
					char sztmp[10] = {0};

					SendMessage(GetWAPFrameWindow(), PWM_GETBUTTONTEXT, 0, (LPARAM)sztmp);
					if (strcmp(sztmp, STR_WAPWINDOW_BACK) == 0)
					{
						PWBE_PrevPage(pUIBrowser);
					}
					else if (strcmp(sztmp, STR_WAPWINDOW_CANCEL) == 0)
					{
						HWND child, prevchl;
						int k = 0;
						
						child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
						while(child != NULL)
						{
							prevchl = GetWindow(child, GW_HWNDPREV);
							child = prevchl;
							if(IsWindow(child))
								k++;
						}
						printf("\r\n++++++++++++Hopen_DocCtrl.c k = %d++++++++++++++++\r\n", k);
						if(k <= 0 && IsWindowVisible(GetWAPFrameWindow()))
						{
							PWBE_Stop(pUIBrowser); 
							if(bHistory == 0)
								rbtnTxtID = 0;
							else
								rbtnTxtID = 2;
							WML_BackOrExit(rbtnTxtID, TRUE);
							SetWindowText(GetWAPFrameWindow(), STR_WAPWML_CAPTION);
						}
						else
						{
							if(bHistory == 0)
								rbtnTxtID = 0;
							else
								rbtnTxtID = 2;
						}
					}
					else if (strcmp(sztmp, STR_WAPWINDOW_EXIT) == 0)
					{
						PostMessage(GetParent(hWnd), WM_CLOSE, NULL, NULL);
					}
					else if(strcmp(sztmp, WML_CLEARINPUT) == 0)
					{
						PWBE_ProcessEvent(pUIBrowser, WBET_CHAR, WBKEY_BACK, 0);
					}
				}
				break;

			case VK_F5:
				return PDADefWindowProc(GetWAPFrameWindow(), message, wParam, lParam);

            default:
                return PDADefWindowProc(hWnd, message, wParam, lParam);
            }     
        }
        break;

	case WM_DESTROY:
		WB_ReleaseFont((void*)hWnd);
		UnregisterClass(BRSVIEWCLASS, NULL);
		break;
		
	default:
		return PDADefWindowProc((void*)hWnd, message, wParam, lParam);
	}

	return lResult;
}

HWND CreateViewPort(HWND hUIWnd, int x, int y, int width, int height, HINSTANCE hInstance)
{
	DWORD   style;
	WNDCLASS  wc;
	HWND    hWnd;	

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = BrowserViewProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = BRSVIEWCLASS;
	
	if (!RegisterClass(&wc)) 
		return NULL;			
	
	style = WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|WS_CLIPCHILDREN;

	hWnd = CreateWindow(BRSVIEWCLASS, "browserview", 
		style,
		x, y, width, height,
		hUIWnd, NULL, hInstance, NULL);	

	return hWnd;	
}

void UI_SetScrollPos(void *pUIData, WB_SBTYPE iBar, WB_PAGEINFO *pPageInfo, WB_SBSTATE iState)
{
	SCROLLINFO  scinfo;
	HWND hWnd = ((PUIDATA)pUIData)->hwnd;
	
	scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask = SIF_ALL;
	scinfo.nMax =  pPageInfo->nMax;
	scinfo.nMin =  0;
	scinfo.nPage = pPageInfo->nPage;
	scinfo.nPos =	pPageInfo->nPos;
	scinfo.nTrackPos = 0;
	
	if(iBar == WBSBT_HORZ)
	{
		SetScrollInfo(hWnd, SB_HORZ, &scinfo, 1);
		if(iState == WBSBS_DISABLE)
			EnableScrollBar(hWnd, SB_HORZ, ESB_DISABLE_BOTH);
		if(iState == WBSBS_ENABLE)
			EnableScrollBar(hWnd, SB_HORZ, ESB_ENABLE_BOTH);
	}
	else if(iBar == WBSBT_VERT)
	{
		SetScrollInfo(hWnd, SB_VERT, &scinfo, 1);	
		if(iState == WBSBS_DISABLE)
		{
			HDC hdc = ((PUIDATA)pUIData)->hdc;
			RECT rc;
			printf("\r\n\r\n@@@@@@@@@@@@@@@@@@@ scroll disable @@@@@@@@@@@@@@@@@@@\r\n\r\n");
			printf("\r\n@@@@@@@@@ %08x @@@@@@@@@@@@\r\n", hdc);
			GetClientRect(hWnd, &rc);
			printf("\r\n@@@@@@@@@@@ top = %d, bottom = %d, right = %d, left = %d @@@@@@@@@@@@@@\r\n", 
					rc.top, rc.bottom, rc.right, rc.left);
			EnableScrollBar(hWnd, SB_VERT, ESB_DISABLE_BOTH);
			rc.left = rc.right - 32;
			FillRect(hdc, &rc, GetStockObject(WHITE_BRUSH));
		}
		if(iState == WBSBS_ENABLE)
		{
			printf("\r\n\r\n@@@@@@@@@@@@@@@ scroll enable @@@@@@@@@@@@@\r\n\r\n");
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
		}
	}
}



