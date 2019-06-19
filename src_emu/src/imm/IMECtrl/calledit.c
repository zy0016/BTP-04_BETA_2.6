/**************************************************************************\
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Model   : call edit
*
* Purpose : call edit support inputting from right bottom
*  
* Author  : 
*
\**************************************************************************/
#include "window.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "string.h"
#include "stdlib.h"

#include "imm.h"
#define CLEAR_TEXT      ML("Clear")
#define EXIT_TEXT       ML("Cancel")
#define EDIT_SPACE      5
#define CURSOR_WIDTH    2
#define DEFUAT_LENGTH   41
#define EN_HOME         0x0502
#define EN_END          0x0503
#define EN_MIDDLE       0x0504
#define EN_EMPTYTEXT    0x0505
#define MAXLINE         9
#define EXC             (-1)
typedef struct
{
    BYTE    bFocus;                 //是聚焦窗口
    BYTE    bMultiLine;             //该窗口是多行编辑窗口
    BYTE    bModify;                //编辑框的内容是否被修改
    BYTE    bLimitText;				//采用了EM_LIMITTEXT消息
	BYTE    bCursorOn;
	BYTE    bReadOnly;              //编辑框的内容是否只读
	BOOL    bCreating;
    WORD    wID;                    //窗口的控制 ID 号
    WORD    wLength;                //当前窗口中正文的长度
    IWORD   wWindowWidth;           //窗口的宽度
    IWORD   wWindowHeight;          //窗口的高度
    IWORD   wLineChars[MAXLINE];             //窗口横向可容纳的字符数
    IWORD   wWindowLines;           //窗口纵向可容纳的行数
    IWORD   wLineCount;             //正文所站行数
    WORD    wCursorPos;             //光标在缓冲区中的的位置
    WORD    wCursorLine;            //光标在显示窗口中的行数(参考点右下)
    IWORD   wCursorCol;             //光标在显示窗口中的列数(参考点右下)
	IWORD   wTextUpCol;
	IWORD   WTextHeight;
    LPSTR   pEditText;              //指向缓冲区的指针
    WORD    wMaxLength;             //最大允许正文长度
    WORD    wMaxEditLength;         //最大允许用户输入正文长度
	WORD    wWindowStart;           //显示窗口的第一个字在缓冲区中的位置(参考点右下)
    HWND    hWnd;
    HWND    hParentWnd;
	LONG	widthoffont;
	LONG	heightoffont;
    DWORD   wState;                 //style
	WORD    imeID;
    HFONT   hfont;
	HFONT   holdfont;
	char    chPrefix;
} CALLEDITDATA, *PCALLEDITDATA;

LRESULT CALLBACK CALLEDIT_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CALLEdit_Init(HWND hWnd, PCALLEDITDATA p, LPARAM lParam);
static void EraseCaret(HWND hWnd, PCALLEDITDATA p);
static int ComputeLineChars(PCALLEDITDATA p, HDC hDC, int linestart);
static void DisplayText(HWND hWnd, PCALLEDITDATA p, HDC hDC);
static void CallEdit_GetChar(PCALLEDITDATA p, HWND hWnd, WORD wCode);
static int GetPosByXY(PCALLEDITDATA p, LONG x, LONG y);
static void PaintText(PCALLEDITDATA p, HWND hWnd, HDC hDC);
static void SetSoftCursorPos(PCALLEDITDATA p, HWND hWnd, HDC hDC);
static void DisplayCaret(HWND hWnd, PCALLEDITDATA p, HDC hDC);
static void CallEdit_DeleteChar(PCALLEDITDATA p, HWND hWnd);
static BOOL ProcessKey(HWND hWnd, PCALLEDITDATA p, WORD vkey, WPARAM wParam, LPARAM lParam);
static BOOL MoveCursorOneLine(PCALLEDITDATA p, int nFlag);
static BOOL MoveCursorHorz(PCALLEDITDATA p, int nFlag);
static	BOOL	FIFOChar(LPSTR	szText, char lastch, int pos);

extern BOOL GetFontHandle(HFONT *hFont, int nType);
//extern BOOL ImmGetImeIDByName( LPWORD pID, LPSTR pName );
//extern BOOL ImcShowImeByID( HIMC hImc, WORD imeID, BOOL bShow );

BOOL CALLEDIT_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC ;       // | CS_DEFWIN;//CS_PARENTDC;
    wc.lpfnWndProc      = CALLEDIT_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(CALLEDITDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "CALLEDIT";

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

LRESULT CALLBACK CALLEDIT_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT         lResult;
    PCALLEDITDATA   p;
    HDC             hDC;
//    SIZE			size;
//    RECT            rcClient;
    PAINTSTRUCT     ps;
//    int             nOldTop;
 //   HBRUSH          hBrush;
//    int             oldCursorline;
//	POINT pt;
	WORD  wCode;
    
    lResult = 0;

    // Gets the user data pointer of the specified window, if the result 
    // pointer is NULL, indicats that the window handle is a invalid 
    // handle(maybe the window has been destroyed), just return.

    p = (PCALLEDITDATA)GetUserData(hWnd);
    if (!p)
        return 0;

    switch (wMsgCmd)
    {
    case WM_CREATE :
        lResult = (LRESULT)CALLEdit_Init(hWnd, p, lParam);    
        break;

    case WM_DESTROY:
		hDC = GetDC(hWnd);
/*
				if (GetFontHandle(&p->hfont, 1))
				{
					p->holdfont = SelectObject(hDC, p->hfont);
				}
				GetTextExtentPoint32(hDC, "T", 1, &size);*/
		
		if (NULL != p->holdfont) 
			SelectObject(hDC, p->holdfont);
		ReleaseDC(hWnd, hDC); 
		if (p->pEditText)
			free((HANDLE)p->pEditText);
		ImcShowImeByID(NULL, p->imeID, FALSE);
        break;

    case WM_KEYDOWN : 
		if (FALSE ==p->bReadOnly)
		{
			switch ((WORD)wParam)
			{
			case VK_1:
			case VK_2:
			case VK_3:
			case VK_4:
			case VK_5:
			case VK_6:
			case VK_7:
			case VK_8:
			case VK_9:
			case VK_0:
			case VK_F3:
			case VK_F4:
				{
					SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
					return lResult;
				}
				break;
			case VK_UP:
			case VK_DOWN:
			case VK_PAGEDOWN:
			case VK_PAGEUP:
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_TAB:
			case VK_F5:
			case VK_F1:
			case VK_F2:
				if((wParam == VK_F5 || wParam == VK_RETURN || wParam == VK_F1
					|| wParam == VK_F2) && p->hParentWnd)
				{
					SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
					return lResult;
				}
				break;
			default:
				break;
			}
			ProcessKey(hWnd, p, (WORD)wParam, wParam, lParam);
		}
		else
		{
			switch ((WORD)wParam)
			{
			case VK_RETURN:
			case VK_F10:
			case VK_F1:
			case VK_F2:
				SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
				return lResult;
			default:
				DestroyWindow(p->hParentWnd);
				break;
			}
		}
        break;

    case WM_PAINT :
		hDC = BeginPaint(hWnd, &ps);
		PaintText(p, hWnd, hDC);	
        DisplayCaret(hWnd, p, hDC);
        OS_UpdateScreen(0, 0, 0, 0);
        EndPaint(hWnd, &ps);
        break;

    case WM_CHAR :
        if (wParam == VK_RETURN || p->bReadOnly || wParam == VK_BACK)
            return 0;
		// The prefix character does not count towards the total length.
/*
		if ((p->chPrefix != '\0')
			&& (strchr((const char*)p->pEditText, p->chPrefix) == NULL)
			&& (p->wLength >= p->wMaxEditLength - 1)
			&& !(((char)wParam == p->chPrefix)
			&& (p->wCursorPos == 0)))
		return 0;*/

		// Any character can't be inputed before the prefix character.
		if ((p->chPrefix != '\0')
			&& (strchr((const char*)p->pEditText, p->chPrefix) != NULL)
			&& (p->wCursorPos == 0))
		return 0;

		wCode = (WORD)wParam;  
        if ((wCode == '*') && (GetFocus() == hWnd))
        {
            SendMessage(p->hParentWnd, WM_KEYDOWN, VK_F3, 0);
        }
        else if (wCode == '#')
        {
            SendMessage(p->hParentWnd, WM_KEYDOWN, VK_F4, 0);
        }

		//达到用户所限制的字符数
		if (p->wLength >= p->wMaxEditLength )
        {
			FIFOChar(p->pEditText, (char)wCode, p->wCursorPos);
            SendMessage(GetParent(hWnd), WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
            //break;
        }
/*
		if (p->hParentWnd && p->bFocus)
			SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);*/
		else
		{
			CallEdit_GetChar(p, hWnd, wCode);
		}
		InvalidateRect(hWnd,NULL,TRUE);
		SendMessage(p->hParentWnd, WM_COMMAND, 
			MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
		if (p->wCursorPos  == p->wLength) 
		{
			SendMessage(p->hParentWnd, WM_COMMAND, 
				MAKELONG(p->wID, EN_HOME), (LPARAM)hWnd);
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
			break;
		}
		if (p->wCursorPos  == 0) 
		{
			SendMessage(p->hParentWnd, WM_COMMAND, 
				MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
			break;
		}
		SendMessage(p->hParentWnd, WM_COMMAND, 
			MAKELONG(p->wID, EN_MIDDLE), (LPARAM)hWnd);
		SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
        break;

	case WM_SETFOCUS :
		ImcShowImeByID(NULL, p->imeID, TRUE);
		DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        p->bFocus = 1;
//		hDC = GetDC(hWnd);		
        if (p->heightoffont > p->wWindowHeight)
            CreateCaret(hWnd, NULL, CURSOR_WIDTH, p->wWindowHeight + EDIT_SPACE);
        else
            CreateCaret(hWnd, NULL, CURSOR_WIDTH, p->heightoffont + EDIT_SPACE);
//		PaintText(p, hWnd, hDC);	
//        DisplayCaret(hWnd, p, hDC);
//        ReleaseDC(hWnd, hDC);		
        SendMessage(p->hParentWnd, WM_COMMAND, 
            MAKELONG(p->wID, EN_SETFOCUS), (LPARAM)hWnd);
		if (p->bCreating)
		{
			p->bCreating = FALSE;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (p->wLength == 0)
		{
			//PostMessage(p->hParentWnd, WM_CLOSE, 0, 0);
            PostMessage(p->hParentWnd, WM_COMMAND,
                MAKEWPARAM(p->wID,EN_EMPTYTEXT), (LPARAM)hWnd);
		}
		else
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}
        break;
		
    case WM_KILLFOCUS :
		ImcShowImeByID(NULL, p->imeID, FALSE);
		DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        p->bFocus = 0;
        DestroyCaret();
		SendMessage(p->hParentWnd, WM_COMMAND, 
			MAKELONG(p->wID, EN_KILLFOCUS), (LPARAM)hWnd);
        break;

	case EM_GETSEL:
		{
			WORD pos;
			pos = p->wCursorPos;
			lResult = (pos<<16) + pos;
		}
        break;
/*
        
            case EM_SETSEL:        
                break;*/
        

    case EM_LIMITTEXT:
		{
			int cchMax;
			cchMax = (int)wParam;//text length can be input by user, in characters 
			if (cchMax >DEFUAT_LENGTH) 
			{
				SendMessage(p->hParentWnd, WM_COMMAND, 
				MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
				p->wMaxEditLength = DEFUAT_LENGTH;
			}
			else
				p->wMaxEditLength = cchMax;
        }
		break;

    case WM_GETTEXT :
		{
			int nTextMax;
			PSTR pszText;
			int len;
			//Specifies the maximum number of characters to be copied, 
			//including the terminating null character. 
			nTextMax = (int)wParam;
			pszText =  (PSTR)lParam;
			
			if(!pszText)
			{
				lResult = 0;
				break;
			}
			
			len = p->wLength;
			
			if (len >= nTextMax)
				len = nTextMax - 1;
			if (len < 0)
			{
				lResult = 0;
				break;
			}
			
			if (p->pEditText)
			{
				memcpy(pszText, p->pEditText, len);
				*(pszText + len) = 0;
				lResult = len;
			}
			else
			{
				//Make sure we return an empty string
				pszText[0] = 0;
				lResult = 0;
			}
		}
        break;

    case WM_SETTEXT :
		{
			PSTR pszText;
			int len;
			pszText = (PSTR)lParam;
			if (!pszText)
				break;
			len = strlen(pszText);
			if (len >p->wMaxEditLength)
				len = p->wMaxEditLength;
			p->wLength = len;
			memcpy(p->pEditText, pszText, len);
			p->pEditText[p->wLength] =0;
			if (p->wLength == 1 && strchr(p->pEditText, '*') != NULL)
			{
				p->wLength = 0;
				p->pEditText[p->wLength] =0;
				p->wCursorPos = p->wLength;
				keybd_event(VK_F3, 0, 0, 0);
			}
			else
			{
				p->wCursorPos = p->wLength;
				InvalidateRect(hWnd,NULL,TRUE);
				if (p->bReadOnly == FALSE)
					SendMessage(p->hParentWnd, WM_COMMAND, 
					MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
			}
		}
		break;
		
    case WM_GETTEXTLENGTH :
		lResult = p->wLength;
        break;

    default :
        return DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

static BOOL CALLEdit_Init(HWND hWnd, PCALLEDITDATA p, LPARAM lParam)
{
    RECT            rcClient;
    PCREATESTRUCT   pCreateData;
    SIZE            size;
    HDC             hDC;

    //structure with creation data 
    pCreateData = (PCREATESTRUCT)lParam;
    hDC = GetDC(hWnd);
	if (GetFontHandle(&p->hfont, 1))
	{
		p->holdfont = SelectObject(hDC, p->hfont);
	}
    GetTextExtentPoint32(hDC, "T", 1, &size);
//	if (NULL != p->holdfont) 
//		SelectObject(hDC, p->holdfont);
    ReleaseDC(hWnd, hDC);    
    pCreateData->style |= WS_CLIPCHILDREN;
    
    //Initializes the internal data of edit control
    memset(p, 0, sizeof(CALLEDITDATA));
    GetClientRect(hWnd, &rcClient);
    p->wWindowWidth     = (IWORD)(rcClient.right - rcClient.left);
    p->wWindowHeight    = (IWORD)(rcClient.bottom - rcClient.top);
	p->hWnd = hWnd;
	p->widthoffont = size.cx;
	p->heightoffont = size.cy;
    
    if (p->wWindowWidth < 0)
        p->wWindowWidth = 0;
	
    if (p->wWindowHeight < 0)
        p->wWindowHeight = 0;
	
    p->wID = (WORD)(DWORD)pCreateData->hMenu;
    p->hParentWnd = pCreateData->hwndParent;
	p->wState = pCreateData->style;

	p->bReadOnly = (p->wState & ES_READONLY) ? 1 : 0;

	if (p->wState & ES_MULTILINE)
    {
        p->bMultiLine = 1;
		p->wWindowLines = p->wWindowHeight / (p->heightoffont + EDIT_SPACE);
		p->wMaxLength = p->wMaxEditLength = DEFUAT_LENGTH;
    }
	
    if (pCreateData->lpszName)
       p->wLength = strlen(pCreateData->lpszName);
    
	if (p->wLength > p->wMaxLength)
    {
        p->wLength = p->wMaxLength;
    }
	p->pEditText = (PSTR)malloc(p->wMaxLength + 2);
	if (!p->pEditText)
	{
		SendMessage(p->hParentWnd, WM_COMMAND, 
			MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
		return FALSE;
	}
	p->pEditText[0] = 0;
	
    if (p->wLength)
    {
		strncpy(p->pEditText, pCreateData->lpszName, p->wLength);
		p->pEditText[p->wLength] = 0;
		if (FALSE == p->bReadOnly) 
			SendMessage(p->hParentWnd, WM_COMMAND, 
			MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
    }
	p->wCursorPos = p->wLength;
	if (!ImmGetImeIDByName(&p->imeID, "Phone"))
	{
		p->imeID = 0;
		p->chPrefix = '\0';
		return FALSE;
	}
	p->chPrefix = '+';
	p->bCreating = TRUE;
//	ImcShowImeByID(NULL, p->imeID, TRUE);
    //Creating succeed.
	InvalidateRect(hWnd,NULL,TRUE);
    return TRUE;
}

static void PaintText(PCALLEDITDATA p, HWND hWnd, HDC hDC)
{
	char drawchar[1];
	SIZE charsize;
	RECT rcClient;
	RECT rcChar;
	int i=0, j=0, w, r, b, upcol;
	
	if (p->wLength <= 0)
		return;

	drawchar[0] = 0;
	
	i=p->wLength-1;
	GetClientRect(hWnd, &rcClient);
	for( j = 0; j< p->wWindowLines; j++)
    {
		w= CURSOR_WIDTH;
		r= rcClient.right;
		b= rcClient.bottom;
		upcol = 0;
		do 
		{					
			drawchar[0] = p->pEditText[i];
			drawchar[1] = 0;
			GetTextExtentExPoint(hDC, (PCSTR)drawchar, 1, 0, NULL, NULL, &charsize);
			rcChar.bottom = b - EDIT_SPACE*(j+1) - p->heightoffont*j;
			rcChar.right = r - CURSOR_WIDTH;
			rcChar.top  = rcChar.bottom - p->heightoffont;
			rcChar.left = rcChar.right - charsize.cx;
			w += charsize.cx;
			if (w >p->wWindowWidth) 
			{
				p->wLineChars[j] = upcol;
				break;
			}
			r -=charsize.cx;
			if ( r < 0) 
				break ;
			DrawText(hDC, (PCSTR)drawchar, 1, &rcChar, DT_TOP|DT_HCENTER);
			upcol++;
			i--;
			if (i<0) 
			{
				p->wLineChars[j] = upcol;
				p->wLineCount =  (IWORD)j;
				p->WTextHeight = (IWORD)rcChar.top;
//				p->wTextUpCol = upcol -1;
				return;
			}
		} while (w < p->wWindowWidth);
	}
	p->wLineCount = (IWORD)j;
	p->WTextHeight = (IWORD)rcChar.top;
	
//	p->wTextUpCol = upcol -1;
	return;
}

static void DisplayCaret(HWND hWnd, PCALLEDITDATA p, HDC hDC)
{

    SetSoftCursorPos(p, hWnd, hDC);
    
    if (p->wCursorPos <= p->wLength && p->wCursorPos >= 0)
    {
		ShowCaret(hWnd);
		p->bCursorOn = 1;
    }
}

static void EraseCaret(HWND hWnd, PCALLEDITDATA p)
{
    if (p->bCursorOn)
    {
        HideCaret(hWnd);
        p->bCursorOn = 0;
    }
}

static void SetSoftCursorPos(PCALLEDITDATA p, HWND hWnd, HDC hDC)
{
    int x, y, pos;
//    SIZE size;
    RECT rcClient;
	char idxchar[1];
	SIZE charsize;
	RECT rcChar;
	int i=0, j=0, w, r, b;
	LPSTR ptr;
	int upcol;

	ptr = p->pEditText;
	idxchar[0]=0;
    GetClientRect(hWnd, &rcClient);	
	pos = p->wCursorPos;
	if ( pos == p->wLength)
	{
		x = rcClient.right -CURSOR_WIDTH - EXC;
		y=  rcClient.bottom- p->heightoffont - EDIT_SPACE;
	}
	else if( (pos < p->wLength) &&( pos >=0) )
	{
		i = p->wLength -1;
		for( j = 0; j< p->wWindowLines; j++)
		{
			w= CURSOR_WIDTH;
			r= rcClient.right;
			b= rcClient.bottom;
			upcol = 0;
			do 
			{	
				idxchar[0] = p->pEditText[i];
				GetTextExtentExPoint(hDC, (PCSTR)idxchar, 1, 0, NULL, NULL, &charsize);
				rcChar.bottom = b - EDIT_SPACE*(j+1) - p->heightoffont*j;
				rcChar.right = r - CURSOR_WIDTH;
				rcChar.top  = rcChar.bottom - p->heightoffont;
				rcChar.left = rcChar.right - charsize.cx;
//				memcpy(&rcpreChar, &rcChar, sizeof(RECT));
/*
				ptr += i;
				len = strlen(ptr);
				GetTextExtentExPoint(hDC, (PCSTR)ptr, len, p->wWindowWidth, &nfit, NULL, NULL);
*/
				w += charsize.cx;
				if (w >p->wWindowWidth)					
					break;
				r -=charsize.cx;
				if ( r < 0) 
					break ;
				if ( (i == pos && upcol+1 < p->wLineChars[j] && j<=p->wLineCount)
					  ||(i == pos && upcol+1 == p->wLineChars[j] && j == p->wLineCount))
				{
					x = rcChar.left- CURSOR_WIDTH - EXC;
					y = rcChar.top;
					goto setpos;
				}
				else if (i == pos && upcol+1 == p->wLineChars[j]&& j < p->wLineCount)
				{
					x =  rcClient.right - CURSOR_WIDTH - EXC;
					y = rcChar.top - EDIT_SPACE - p->heightoffont;
					goto setpos;
				}
				i--;
				upcol++;
				if (i<0) 
					return;
			} while (w < p->wWindowWidth);
			
/*
			if ( i == pos)
			{
				x =  r - CURSOR_WIDTH;
				y = rcChar.top+EDIT_SPACE+p->heightoffont;
				goto setpos;
			}
*/
		}
	}
setpos:
    if (p->bFocus)
        SetCaretPos(x, y);
	return;
}

/*
static int ComputeLineChars(PCALLEDITDATA p, HDC hDC, int linestart)
{
    int   count = 0;
    LPSTR ptr;
	int   nSize = 0, i;
	int   nlen1, nlen2;
	
	if (linestart >= p->wLength)
		return 0;
	
	ptr = (LPSTR)p->pEditText + linestart;
	
	if ((DWORD)ptr < (DWORD)(p->pEditText))
		ptr = (LPSTR)p->pEditText;	
	
	nlen1 = p->wLineChars;
		
	i = 0;
	while (i < nlen1)
	{
		if ((*ptr)== '\0')
			break;
		
		ptr ++;
		count ++;
		i ++;
	}
	
	if if ((*ptr)== '\0')
	{
		count ++;		
		goto RET;
	}		
RET : 
		return count;
}
*/

/*
static void DisplayText(HWND hWnd, PCALLEDITDATA p, HDC hDC)
{
	char * textinline;
	int i;
	RECT   rcinline;
	RECT rcClient;
	LPSTR ptr;
	int nlen;

	ptr = (LPSTR)p->pEditText;
	textinline = malloc(p->wLineChars + 1);
	if (NULL == textinline) 
		return;
    GetClientRect(hWnd, &rcClient);		
	if ((p->wCursorLine<=p->wWindowLines)&&(p->wCursorLine>0))
	{
		for( i=1; i<=p->wLineCount; i++)
		{
			rcinline.bottom = rcClient.bottom  - (i-1)*(p->heightoffont+EDIT_SPACE);
			rcinline.right = rcClient.right;
			rcinline.top = rcClient.top+(p->wWindowLines-i)*(p->heightoffont+EDIT_SPACE);
			rcinline.left = rcClient.left;
			memset(textinline, 0, p->wLineChars+1);
			if (i != p->wLineCount)
			{
				nlen = p->wLineChars;
				memcpy(textinline, ptr, nlen);
				textinline[p->wLineChars] = 0;
				ptr = ptr + p->wLineChars;
			}
			else
			{
				nlen = p->wLength - p->wLineChars * (i-1);
				if (nlen <= p->wLineChars) 
				{
					memcpy(textinline, ptr, nlen);
					textinline[nlen] = 0;
					ptr = ptr + nlen;
				}

			}
			DrawText(hDC, textinline, nlen+1, &rcinline, DT_RIGHT|DT_BOTTOM);
		}
	}
	free(textinline);
	return;
}*/


static void CallEdit_GetChar(PCALLEDITDATA p, HWND hWnd, WORD wCode)
{
	int pos, i;
    if (wCode == VK_BACK)
        return;
	pos = p->wCursorPos;

	if (pos == p->wLength)
	{
		p->pEditText[pos] = (char)LOWORD(wCode);
		p->wLength++;
		p->wCursorPos++;
		p->pEditText[p->wLength] = '\0';
		return;
	}
	
	for(i = p->wLength-1; i >=pos; i--)
		p->pEditText[i+1] =p->pEditText[i];
	p->pEditText[pos] = (char)LOWORD(wCode);
	p->wLength++;
	p->wCursorPos++;
	p->pEditText[p->wLength] = '\0';
	return;
}

static BOOL ProcessKey(HWND hWnd, PCALLEDITDATA p, WORD vkey, WPARAM wParam, LPARAM lParam)
{
    BOOL    bNotRedraw = FALSE, bMoveCursor = FALSE;
    HDC     hDC;
//    int     oldstart;

    switch (vkey)
    {
    default :
        return FALSE;

    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
        {
            switch (vkey)
            {
            case VK_UP:
                bMoveCursor = MoveCursorOneLine(p, 1);
				FlushInputMessages();
				if (p->wCursorPos  == p->wLength) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_HOME), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
					break;
				}
				if (p->wCursorPos  == 0) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
					break;
				}
				SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_MIDDLE), (LPARAM)hWnd);
				SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                break;
                
            case VK_DOWN:
                bMoveCursor = MoveCursorOneLine(p, 2);
				FlushInputMessages();
				if (p->wCursorPos  == p->wLength) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_HOME), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
					break;
				}
				if (p->wCursorPos  == 0) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
					break;
				}
				SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_MIDDLE), (LPARAM)hWnd);
				SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                break;
				
            case VK_LEFT:
                bMoveCursor = MoveCursorHorz(p, 1);
				if (p->wCursorPos  == p->wLength) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_HOME), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
					break;
				}
				if (p->wCursorPos  == 0) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
					break;
				}
				SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_MIDDLE), (LPARAM)hWnd);
				SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                break;
                
            case VK_RIGHT:
                bMoveCursor = MoveCursorHorz(p, 2);
				if (p->wCursorPos  == p->wLength) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_HOME), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
					break;
				}
				if (p->wCursorPos  == 0) 
				{
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
					SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
					break;
				}
				SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_MIDDLE), (LPARAM)hWnd);
	            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                break;
            }
            
            if (bMoveCursor)
            {
                hDC = GetDC(hWnd);
//				EraseCaret(hWnd, p);				
				DisplayCaret(hWnd, p, hDC);				
                ReleaseDC(hWnd, hDC);
            }
		}        
		break;
    
   case VK_BACK:
	   {
		   if (p->wCursorPos == 0)
		   {
			   SendMessage(p->hParentWnd, WM_COMMAND, 
				   MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
			   SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
			   break;
		   }
		   CallEdit_DeleteChar(p, hWnd);
		   if (p->wCursorPos  == 0) 
		   {
			   SendMessage(p->hParentWnd, WM_COMMAND, 
				   MAKELONG(p->wID, EN_END), (LPARAM)hWnd);
			   SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
			   InvalidateRect(hWnd, NULL, TRUE);
			   break;
		   }
		   InvalidateRect(hWnd, NULL, TRUE);
		   SendMessage(p->hParentWnd, WM_COMMAND, 
			   MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
	   }
	   break;

   case VK_F10:
	   if ((SendMessage(hWnd, EM_GETSEL, 0, 0) == 0) && (HIWORD(lParam) != 0))
	   {
		   // If the caret is at the beginning of the buffer and this key
		   // message is triggered by actual key-pressing action (NOT by
		   // REPEATING timer), this message will return to SOFT_RIGHT
		   // original process.
		   return SendMessage(p->hParentWnd, WM_KEYDOWN, wParam,
			   lParam);
	   }
	   SendMessage(hWnd, WM_KEYDOWN, VK_BACK, 0);
	   if (p->wLength == 0)
	   {
//		   SendMessage(p->hParentWnd, WM_CLOSE, 0, 0);//DestroyWindow(p->hParentWnd);
		   return SendMessage(p->hParentWnd, WM_KEYDOWN, wParam,
			   lParam);
//		   break;
	   }

	   return 0;
	   
	}
    return TRUE;
}

static void CallEdit_DeleteChar(PCALLEDITDATA p, HWND hWnd)
{
	int pos, i =0;
	int idx;
	pos = p->wCursorPos;
	if (pos == p->wLength) 
	{
		p->wLength--;
		p->pEditText[p->wLength]= 0;
		pos = p->wLength;
	}
    else if (pos == 0)
		return;
	else
	{
		idx = pos-1;
			for( i = idx; i < p->wLength-1; i++)
		p->pEditText[i] =p->pEditText[i+1];
		p->wLength--;
		p->pEditText[p->wLength]=0;
		pos = idx;
		
	}
	p->wCursorPos = pos;
	return;
}

#define LINE_UP   1
#define LINE_DOWN 2
static BOOL MoveCursorOneLine(PCALLEDITDATA p, int nFlag)
{
    int newpos = 0;
//    DWORD   t;
//	HDC     hdc;
    POINT   CaretPos;
	POINT   NewCaretPos;
	RECT   rcClient;

    if (!p->bMultiLine)
        return FALSE;
	GetClientRect( p->hWnd, &rcClient);

	GetCaretPos( &CaretPos);
    switch (nFlag)
    {
    case LINE_UP:
		if (CaretPos.y == p->WTextHeight)
		{
			NewCaretPos.x = CaretPos.x;
			NewCaretPos.y = rcClient.bottom - EDIT_SPACE -p->heightoffont;
		}
		else
		{
			NewCaretPos.x = CaretPos.x;
			NewCaretPos.y = CaretPos.y - EDIT_SPACE - p->heightoffont;
		}
		newpos = GetPosByXY(p, NewCaretPos.x, NewCaretPos.y);
        break;

    case LINE_DOWN:
		if (CaretPos.y == rcClient.bottom - EDIT_SPACE -p->heightoffont)
		{
			NewCaretPos.x = CaretPos.x;
			NewCaretPos.y = p->WTextHeight;
		}
		else
		{
			NewCaretPos.x = CaretPos.x;
			NewCaretPos.y = CaretPos.y + EDIT_SPACE + p->heightoffont;
		}
		newpos = GetPosByXY(p, NewCaretPos.x, NewCaretPos.y);
        break;
    }
    p->wCursorPos = newpos;
	
    return TRUE;
}

#define MOVE_LEFT   1
#define MOVE_RIGHT  2
static BOOL MoveCursorHorz(PCALLEDITDATA p, int nFlag)
{
	int pos, newpos;
	if (!p->bMultiLine)
        return FALSE;
	newpos = pos = p->wCursorPos;
	switch(nFlag)
	{
	case MOVE_LEFT:
		newpos -=1;
		if ( newpos < 0 ) 
			newpos = p->wLength;
		break;
	case MOVE_RIGHT:
		newpos +=1;
		if ( newpos > p->wLength)
			newpos =0;
		break;
	}
	p->wCursorPos = newpos;
	return TRUE;
}
static int GetPosByXY(PCALLEDITDATA p, LONG x, LONG y)
{
    int newpos;
	char idxchar[1];
	RECT rcChar;
	RECT rcClient;
	SIZE charsize;
	int i=0, j=0, w, r, b;
	HDC hDC;
	
	idxchar[0]=0;
	GetClientRect(p->hWnd, &rcClient);
	i = p->wLength -1;
	for( j = 0; j< p->wWindowLines; j++)
	{
		w= CURSOR_WIDTH;
		r= rcClient.right;
		b= rcClient.bottom;
		do 
		{	
			
			idxchar[0] = p->pEditText[i];
			idxchar[1] = 0;
			hDC = GetDC(p->hWnd);
			GetTextExtentExPoint(hDC, (PCSTR)idxchar, 1, 0, NULL, NULL, &charsize);
			ReleaseDC(p->hWnd, hDC);
			rcChar.bottom = b - EDIT_SPACE*(j+1) - p->heightoffont*j;
			rcChar.right = r - CURSOR_WIDTH;
			rcChar.top  = rcChar.bottom - p->heightoffont;
			rcChar.left = rcChar.right - charsize.cx;
			w += charsize.cx;
			if ( (y== rcChar.top) && x >= rcChar.right-1)//rcChar.left +CURSOR_WIDTH)
			{
				newpos = i+1;
				return newpos;
			}
			if (w >p->wWindowWidth) 
				break;
			r -=charsize.cx;
			if ( r < 0) 
				break ;
			i--;
			if (i<0) 
			{
				newpos = 0;
				return newpos;
			}
		} while (w < p->wWindowWidth);
	}

	newpos = i;
    return newpos;
}

static	BOOL	FIFOChar(LPSTR	szText, char lastch, int	pos)
{
	int	nLen, i;
//	PSTR	ptr;
	if (!szText) 
	{
		return FALSE;
	}
	nLen = strlen(szText);
	for(i=0; i< pos; i++)
	{
		szText[i] = szText[i+1];
	}
	szText[pos-1] = lastch;
	return TRUE;
}
