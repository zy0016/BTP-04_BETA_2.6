/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "hpwin.h"
#include "control.h"
#include "string.h"
//#include "str_public.h"
//#include "str_hp.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "hpfile.h"

#if 1
#define CLEAR_TEXT  ML("Clear")
#define EXIT_TEXT   ((p->szRightKey[0] != '\0') ? p->szRightKey : ML("Cancel"))
#else
#define CLEAR_TEXT  "Clear"
#define EXIT_TEXT   "Exit"
#endif

#define ABS(a)      (((a) < 0) ? -(a) : (a))
#define EDIT_SPACE  5
#define MAX_PASSWORD    50//Windows的password最多能输入49个字符.

#define CTRL_HEIGHT     8
#define CTRL_WIDTH      8

#define CURSOR_WIDTH    2

#define FILL_CHAR       0x7f

#define MOVE_RIGHT      1
#define MOVE_LEFT       2
#define MOVE_NO         0

//#define RB_DELETE       "清除"
//#define RB_EXIT         "退出"

#define EDIT_TIMER_EVENT_ID   5
#define ISCONTROLCHAR(c)   (((c) == '\0') || ((c) == '\r') || ((c) == '\n'))

#define IDT_TOMASK            1
#define ET_TOMASK             600

#define CX_LEFTMARGIN         2
#define CX_RIGHTMARGIN        3
#define CY_TOPMARGIN          1
#define CY_BTMMARGIN          1
#define CX_TITLE_LEFTMARGIN   2
#define CX_TITLE_RIGHTMARGIN  0
#define CY_TITLE_TOPMARGIN    18
#define CY_TITLE_BTMMARGIN    1

//Define data struct for edit control
typedef struct
{
    BYTE    bFocus;                 //是聚焦窗口
    BYTE    bCursorOn;              //光标处于显示状态, 在屏幕范围以内
    BYTE    bSelectOn;              //选择区域有效
    BYTE    bReplace;               //是否用新字符代替选中字符
    BYTE    bVScrollBar;            //有垂直滚动条
    BYTE    bHScrollBar;            //有水平滚动条
    BYTE    bRedraw;                //需要完全重绘制窗口
    BYTE    bMultiLine;             //该窗口是多行编辑窗口
    BYTE    bModify;                //编辑框的内容是否被修改
    BYTE    bReadOnly;              //编辑框的内容是否只读
    BYTE    bPenDown;
    BYTE    bRegionOn;              //是否有选中字符
    BYTE    bReplaceSel;            //在只读属性时是否使用命令EM_REPLACESEL来取代字符 
    UINT    nPasswordChar;          //编辑框的属性为ES_PASSWORD时的显示字符
    BYTE    bUnderline;
    BYTE    bScrollScreen;
    BYTE    bLimitText;				//采用了EM_LIMITTEXT消息
    BYTE    bMoveCursorUp;			//判断在只读情况下光标的移动情况
    BYTE    bRealFocus;             //是真焦点窗口
    BYTE    bIsSingle;              //是否只有这一个控件
    BYTE    bEnable;
	BYTE	CharSet;				//当前edit中的字体
    UINT    uTimerID;
    WORD    wSingleLineMove;
    IWORD   wSingleLineXPos;
    WORD    *pLineIndex;            //对于只读Edit,记录每一行开始字符的索引值
    WORD    wIndexCount;            //对于只读Edit,指已记录的索引值的个数
    WORD    wLineCountOrg;
    WORD    wID;                    //窗口的控制 ID 号
    WORD    wMaxLength;             //最大允许正文长度
    WORD    wMaxEditLength;         //最大允许用户输入正文长度
    WORD    wLength;                //当前窗口中正文的长度
	WORD	w3DBorder;				//窗口立体框所占宽度。
    WORD    wWindowWidth;           //窗口的宽度
    WORD    wWindowHeight;          //窗口的高度
    IWORD   wLineChars;             //窗口横向可容纳的字符数
    IWORD   wScrollLineChars;       //滚动时横向可容纳的字符数
    IWORD   wHScrollChars;          //横向滚动的字符数
    IWORD   wMaxLineChars;          //横向已有的最大字符数
    IWORD   wWindowLines;           //窗口纵向可容纳的行数
    IWORD   wLineCount;
    WORD    wCursorPos;             //光标在缓冲区中的的位置
    WORD    wCursorLine;            //光标在显示窗口中的行数
    IWORD   wCursorCol;             //光标在显示窗口中的列数
    WORD    wWindowStart;           //显示窗口的第一个字在缓冲区中的位置
    WORD    wOffsetLine;            //显示窗口的第一个字在整个缓冲区中的行号

    WORD    wPosStart;              //选择区域的起始字符
    WORD    wPosEnd;                //选择区域的结束字符
    WORD    wPosEndOld;             //选择区域的临时字符
    LPSTR   pEditText;              //指向缓冲区的指针
    HWND    hWnd;
    HWND    hParentWnd;
	LONG	widthoffont;
	LONG	heightoffont;
    HFONT   hFont;
    DWORD   wState;                 //style
	DWORD   wReturnNum;				//控制字符数目
    PSTR    pszTitle;
    WORD    wTitleLen;
    BOOL    bToMask;
    WORD    wCharToMask;
    char    szRightKey[16];
} EDITDATA, *PEDITDATA;

static HBITMAP hbmpFocus = NULL; // hbmpUpArrow, hbmpDownArrow;

//Edit control window proc prototypes
LRESULT CALLBACK EDIT_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);

// static BOOL IsDBCSLeadChar(BYTE c);
//static BOOL IsDBCSSecondChar(BYTE c);
//static BOOL IsControlChar(BYTE c);
static int  InHalfDBCSChar(PEDITDATA p, LPSTR pstr, int offset);
static int  AdjustDisplayWindow(HWND hWnd, PEDITDATA p);//调整光标、滚动条以及起始字符位置
static void FormatText(PEDITDATA p, PSTR ptr, int len);
static void SwapPos(WORD *pwPosStart, WORD *pwPosEnd);

static int  PaintText(PEDITDATA p, HWND hWnd, HDC hDC, BOOL bReversed, 
                            WORD wPosStart, WORD wPosEnd);
static int  ComputeLineChars(PEDITDATA p, HDC hDC, int linestart);

static int   FindLineHead(PEDITDATA p, int pos);
static DWORD UpOneLine(PEDITDATA p, WORD oldstart);
static DWORD DownOneLine(PEDITDATA p, WORD oldstart);
static DWORD UpOnePage(PEDITDATA p, WORD oldstart);
static DWORD DownOnePage(PEDITDATA p, WORD oldstart);

static void  SetSoftCursorPos(PEDITDATA p, HWND hWnd, HDC hDC);
static BOOL  MoveCursorOneLine(PEDITDATA p, int nFlag);
static BOOL  MoveCursorHome(PEDITDATA p);
static BOOL  MoveCursorEnd(PEDITDATA p);
static BOOL  MoveCursorHorz(PEDITDATA p, int nFlag);

static int   GetPosByXY(PEDITDATA p, int line, int col);
static DWORD GetXYByPos(PEDITDATA p, int pos);

static int  ReplaceSelect(HWND hWnd, PEDITDATA p, BYTE * ptr, int len);

static void DoVScroll(PEDITDATA p, HWND hWnd, int code, int LineCount);
static void DoHScroll(PEDITDATA p, HWND hWnd, int code, int LineCount);
static int  SingleLineMoveRight(PEDITDATA p);
static int  SingleLineMoveLeft(PEDITDATA p);
static IWORD GetHScrollChars(PEDITDATA p, IWORD offset);

static BOOL ProcessKey(HWND hWnd, PEDITDATA p, WORD vkey, WPARAM wParam, LPARAM lParam);
static void ProcessPen(HWND hWnd, PEDITDATA p, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void EDIT_Paint(HWND hWnd, PEDITDATA p, HDC hDC);
static void Edit_GetChar(PEDITDATA p, HWND hWnd, WORD wCode, LPARAM lParam);
static BOOL Edit_Init(HWND hWnd, PEDITDATA p, LPARAM lParam);
static void Edit_Size(PEDITDATA p, HWND hWnd, LPARAM lParam);
static void Edit_SetSel(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam);
static BOOL Edit_LineScroll(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam);
static void Edit_PenMove(HWND hWnd, PEDITDATA p, LPARAM lParam);
static void Edit_SetTitle(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam);

static void DrawUnderLine(HWND hWnd, HDC hDC, PEDITDATA p, RECT* pRect);
#if (!INTERFACE_MONO)
static void Draw3DEditRect_W95(HDC hdc, RECT* pRect, BOOL bHScroll, BOOL bVScroll);
#endif
static void GetEditRect(PEDITDATA p, HWND hWnd, RECT* pRect);
static void ConvertToEditRect(PEDITDATA p, RECT* pRect);
static void ConvertXY(PEDITDATA p, int *pX, int *pY);
static void ClearEdit(HWND hWnd, HDC hDC, PEDITDATA p, HBRUSH hBrush, int charHeight, 
                      RECT* pRectClient, int nStartHeight);
static void EraseCaret(HWND hWnd, PEDITDATA p);
static void DisplayCaret(HWND hWnd, PEDITDATA p, HDC hDC);
static void DisplayNewChars(HWND hWnd, PEDITDATA p, BYTE * ptr, int len);
static HBRUSH SetCtlColorEdit(PEDITDATA p, HWND hWnd, HDC hdc);
static int SearchIndex(PEDITDATA p);
static int ReMemAlloc(PEDITDATA , int );
static void MoveBuffer(PEDITDATA p, int pos, int delta, BOOL bReturn);
static void DrawHighlight(HWND hWnd, HDC hdc, PEDITDATA pEditData,
                          PRECT prcEdit, PRECT prcPaint);
static void DrawTitle(HWND hWnd, HDC hdc, PEDITDATA p);
static BOOL GetTitleRect(HWND hWnd, PRECT pRect);
static int GetPasswordExtent(HDC hdc, PEDITDATA p, int nLen, PSIZE pSize);
static void CALLBACK EditTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

BOOL EDIT_RegisterClass(void)
{
    WNDCLASS wc;
    char szPathName[MAX_PATH] = "";

    wc.style            = CS_OWNDC | CS_DEFWIN;//CS_PARENTDC;
    wc.lpfnWndProc      = EDIT_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(EDITDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "EDIT";

    if (!RegisterClass(&wc))
        return FALSE;

    PLXOS_GetResourceDirectory(szPathName);
    strcat(szPathName, "listfocus.bmp");
    hbmpFocus = LoadImage(NULL, szPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    return TRUE;
}

LRESULT CALLBACK EDIT_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT         lResult;
    PEDITDATA       p;
    int             len, line, pos = 0, oldstart, nBitsPixel;
    int             ich, cchMax, nTextMax;
    WORD            wCode, nPos, wPosStart, wPosEnd;
    DWORD           t;
    IWORD           col;
    HDC             hDC;
    HANDLE          hClipboard;
    PSTR            pszText, pch, pSrc, pDest; 
    PCSTR           pszch, pszReplace;
    LPDWORD         pdwStart, pdwEnd;
    SIZE			size;
    BOOL            fRedraw, bSwap = FALSE;
    HFONT           hFont = 0;
    RECT            rcClient, rcUnderLine, rcEdit;
    //char            achClassName[32];
    PAINTSTRUCT     ps;
    int             nOldTop;
    HBRUSH          hBrush;
    // add up&down arrow
    int             oldCursorline;
	POINT pt;
    
    lResult = 0;

    // Gets the user data pointer of the specified window, if the result 
    // pointer is NULL, indicats that the window handle is a invalid 
    // handle(maybe the window has been destroyed), just return.

    p = (PEDITDATA)GetUserData(hWnd);
    if (!p)
        return 0;

    switch (wMsgCmd)
    {
    case WM_CREATE :

        lResult = (LRESULT)Edit_Init(hWnd, p, lParam);
    
        break;

    case WM_DESTROY:

        //释放掉 pEditText 内存
		if (p->pEditText && !(p->bReadOnly && (p->wState & ES_USERSTRING)))
			LocalFree((HANDLE)p->pEditText);

		if (p->pLineIndex)
			LocalFree((HANDLE)p->pLineIndex);

        if (p->pszTitle != NULL)
            LocalFree((HANDLE)p->pszTitle);
        break;

#if (!NOKEYBOARD)
//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN :

        if (p->bPenDown)
            break;

        switch ((WORD)wParam)
        {
        case VK_UP:
        case VK_DOWN:
        case VK_PAGEDOWN:
        case VK_PAGEUP:
        case VK_RETURN:
        case VK_ESCAPE:
        case VK_TAB:
        case VK_F10:
        case VK_F5:
        case VK_F1:
        case VK_F2:
            if((wParam == VK_F10 || wParam == VK_RETURN || wParam == VK_F1
                || wParam == VK_F2 || wParam == VK_F5 && p->hParentWnd))
            {
                SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
                return lResult;
            }
            if ((p->wState & ES_COMBOEDIT) || (wParam == VK_ESCAPE) || 
                (wParam == VK_TAB) || ((wParam == VK_RETURN) && 
                !(((p->wState & ES_WANTRETURN) || (lParam & CK_CONTROL)) && 
                p->bMultiLine)))
            {
                PostMessage(p->hParentWnd, wMsgCmd, wParam, lParam);//Must use PostMessage.
                PostMessage(p->hParentWnd, WM_KEYUP, wParam, lParam);
                return lResult;
            }
            break;
        
        default:
            break;
        }
        ProcessKey(hWnd, p, (WORD)wParam, wParam, lParam);

        break;

    case WM_KEYUP:
        if ((p->hParentWnd) && (p->wState & CS_NOSYSCTRL))
        {
            SendMessage(p->hParentWnd, WM_KEYUP, wParam, lParam);
        }
        break;
        
//#endif //SCP_NOKEYBOARD
#endif // NOKEYBOARD

    case WM_PAINT :

        hDC = BeginPaint(hWnd, &ps);
        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

        GetClientRect(hWnd, &rcClient);
        CopyRect(&rcEdit, &rcClient);
#if (!INTERFACE_MONO)
        if ((ps.rcPaint.left <= rcEdit.left) ||
            (ps.rcPaint.top <= rcEdit.top) || 
            (ps.rcPaint.right >= rcEdit.right) ||
            (ps.rcPaint.bottom >= rcEdit.bottom))
        {
            if (nBitsPixel != 1 && nBitsPixel != 2 && !(p->wState & ES_COMBOEDIT))
            {
                Draw3DEditRect_W95(hDC, &rcClient, (p->wState & WS_HSCROLL), 
                    p->wState & WS_VSCROLL);
            }
        }
#endif
		ConvertToEditRect(p, &rcClient);

        nOldTop = ps.rcPaint.top;
        ConvertXY(p, NULL, (int *)&ps.rcPaint.top);
        
        //GetTextExtentPoint32(hDC, "T", 1, &size);
        
        SetRect(&rcUnderLine, rcClient.left,  
            ps.rcPaint.top + p->w3DBorder,
            rcClient.right, ps.rcPaint.bottom + p->w3DBorder);
        
        ps.rcPaint.top = nOldTop;
        line = ps.rcPaint.top / (p->heightoffont + EDIT_SPACE + 1);
        wPosStart = GetPosByXY(p, line, 0);
        line = ps.rcPaint.bottom / (p->heightoffont + EDIT_SPACE + 1) + 1;
        wPosEnd = GetPosByXY(p, line, 0);
        
        DrawHighlight(hWnd, hDC, p, &rcEdit, &ps.rcPaint);
        DrawTitle(hWnd, hDC, p);

        if (p->bSelectOn)
        {
            BOOL    bSwapPos = FALSE;
            
            if (p->wPosEnd < p->wPosStart)
            {
                SwapPos(&p->wPosStart, &p->wPosEnd);  
                bSwapPos = TRUE;
            }
            
            if (wPosStart >= p->wPosStart)
            {
                if (wPosEnd >= p->wPosEnd)
                {
                    PaintText(p, hWnd, hDC, 1, wPosStart, p->wPosEnd);
                    PaintText(p, hWnd, hDC, 0, p->wPosEnd, wPosEnd);
                }
                else
                {
                    PaintText(p, hWnd, hDC, 1, wPosStart, wPosEnd);
                }
            }
            else
            {
                if (wPosEnd > p->wPosStart)
                {
                    PaintText(p, hWnd, hDC, 0, wPosStart, p->wPosStart);
                    
                    if (wPosEnd >= p->wPosEnd)
                    {
                        PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
                        PaintText(p, hWnd, hDC, 0, p->wPosEnd, wPosEnd);
                    }
                    else
                        PaintText(p, hWnd, hDC, 1, p->wPosStart, wPosEnd);
                }
                else//wPosEnd <= p->wPosStart
                    PaintText(p, hWnd, hDC, 0, wPosStart, wPosEnd);
            }
            
            if (bSwapPos)//还原。
            {
                SwapPos(&p->wPosStart, &p->wPosEnd);  
            }
        }
        else if (wPosStart != 0 || wPosEnd != 0) 
            PaintText(p, hWnd, hDC, 0, wPosStart, wPosEnd);
        DrawUnderLine(hWnd, hDC, p, &rcUnderLine);
        DisplayCaret(hWnd, p, hDC);
        OS_UpdateScreen(0, 0, 0, 0);

        EndPaint(hWnd, &ps);

        break;

    case WM_CHAR :
        if (wParam == VK_RETURN || p->bReadOnly || wParam == VK_BACK)
            return 0;
		wCode = (WORD)wParam;
        
		//达到用户所限制的字符数
		if (p->wLength >= p->wMaxEditLength && p->wPosEnd == p->wPosStart)
        {
            SendMessage(GetParent(hWnd), WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
            break;
        }
        //无垂直滚动时的最后一行不允许输入回车
		else if ((p->wCursorLine + 1) == p->wWindowLines && !p->bVScrollBar && wCode == '\r')
        {
            SendMessage(GetParent(hWnd), WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
            break;
        }
        oldCursorline = p->wCursorLine;
//        if (p->wState & ES_GSM)
//        {
//            if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
//            {
//                SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
//                if (p->wLength == 0)
//                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
//                    SCROLLLEFT, MASKRIGHT | MASKLEFT);
//                else
//                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
//                    SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
//            }
//
//        }
//        else
//        {
//            // If the character to send is a visible character, the message
//            // will be sent. The condition of a visible character is: In
//            // character set of CHARSET_1252, all characters except NULL and
//            // '\n' are visible; Otherwise, the characters '\r' and the
//            // characters equal to SPACE and greater than it are visible.
//            if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0
//                && ((p->CharSet != CHARSET_1252 && (wParam > 0x20
//                || wParam == '\r' || wParam == ' '))
//                || (p->CharSet == CHARSET_1252 && (wParam > 0 && wParam != '\n')))
//                && p->bFocus)
//            {
//                SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
//                if (p->wLength == 0)
//                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
//                    SCROLLLEFT, MASKRIGHT | MASKLEFT);
//                else
//                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
//                    SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
//            }
//        }
        wCode = (WORD)wParam;
        //达到用户所限制的字符数

        if (p->wLength >= p->wMaxEditLength && p->wPosEnd == p->wPosStart)
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
            break;
        }
        //无垂直滚动时的最后一行不允许输入回车
		else if ((p->wCursorLine + 1) == p->wWindowLines && !p->bVScrollBar && wCode == '\r')
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
            break;
        }
        Edit_GetChar(p, hWnd, wCode, lParam);
        //add up&down arrow
        if ((oldCursorline != p->wCursorLine) && p->bMultiLine && p->bIsSingle)
        {
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                    NULL, MASKUP | MASKDOWN);
			else if (p->wCursorLine == 0 && p->wLineCount == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(GetParent(hWnd), PWM_SETSCROLLSTATE, 
                    SCROLLDOWN, MASKUP | MASKDOWN); 
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLDOWN | SCROLLUP, MASKUP | MASKDOWN); 
        }
        break;

    case WM_PENDOWN :
    case WM_PENMOVE :
    case WM_PENUP :

        ProcessPen(hWnd, p, wMsgCmd, wParam, lParam);
        break;

    case WM_SYSTIMER :

        if (wParam != EDIT_TIMER_EVENT_ID)
        {
            DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }

        if (p->wSingleLineMove == MOVE_RIGHT)
            pos = SingleLineMoveRight(p);
        else if (p->wSingleLineMove == MOVE_LEFT)
            pos = SingleLineMoveLeft(p);

        if (pos == -1)
            break;

        p->wCursorPos = pos;
        t = GetXYByPos(p, p->wCursorPos);
        p->wCursorLine = HIWORD(t);
        p->wCursorCol = LOWORD(t);

        p->wPosEndOld = p->wPosEnd;
        p->wPosEnd = pos;

        p->bSelectOn = 1;
        
        hDC = GetDC(hWnd);

        //GetTextExtentPoint32(hDC, "T", 1, &size);

        SetCtlColorEdit(p, hWnd, hDC);
        EDIT_Paint(hWnd, p, hDC);

        ReleaseDC(hWnd, hDC);
        break;
/*
    case EM_CANUNDO:
        break;
*/
    case EM_GETFIRSTVISIBLELINE:

        if (p->bMultiLine)
        {
            WORD    startline, startcol, len;

            oldstart = p->wWindowStart;
            p->wWindowStart = 0;
            
            t = GetXYByPos(p, oldstart);
            startline = HIWORD(t);
            startcol = LOWORD(t);
			hDC = GetDC(hWnd);
            len = ComputeLineChars(p, hDC, oldstart - startcol);
			ReleaseDC(hWnd, hDC);
            
            if (len == startcol &&
                (len == p->wScrollLineChars || len == p->wScrollLineChars - 1))
            {
                startline ++;
            }
            
            p->wWindowStart = oldstart;
            lResult = (LRESULT)startline;
        }
        else
            lResult = (LRESULT)p->wWindowStart;

        break;

    case EM_GETHANDLE:

        if (p->bMultiLine)
            lResult = (LRESULT)p->pEditText;
                       
        break;

    case EM_GETCARETSTATE:
        if (p->bCursorOn)
            return TRUE;
        else
            return FALSE;

    case EM_GETLIMITTEXT:
        lResult = (LRESULT)p->wMaxEditLength;
        break;

    case EM_GETLINE:
        line = (WORD)wParam;//line number to retrieve 
        pszch = (PCSTR)lParam;

        if (pszch == NULL || line < 0)//if address is invalid.
            break;

        cchMax = *(WORD *)pszch;
        if (cchMax <= 0)
            break;

        if (p->pEditText)
        {
            if (!p->bMultiLine)
            {           
                pos = 0;
                len = p->wLength;
            }
            else
            {
                long lineCount;
                int oldStart;

                lineCount = SendMessage(hWnd, (WORD)EM_GETLINECOUNT, 0, 0);
                
                if (line > lineCount)
                {
                    len = 0;
                    break;
                }
                
                oldStart = p->wWindowStart;
                p->wWindowStart = 0;
                pos = GetPosByXY(p, line, 0);
				hDC = GetDC(hWnd);
                len = ComputeLineChars(p, hDC, pos);
				ReleaseDC(hWnd, hDC);
                p->wWindowStart = oldStart;
				len -= p->wReturnNum;                
                if (len < 0)
                    len = 0;
            }

            if (len > cchMax)
                len = cchMax;
            //ASSERT(len > 0);

            //The copied line does not contain a terminating null character.
            memcpy((PSTR)pszch, p->pEditText + pos, len);     

            lResult = len;
        }
        break;

    case EM_GETLINECOUNT:
        if (p->pEditText && p->bMultiLine && p->wLength > 0)
        {
            oldstart = p->wWindowStart;
            p->wWindowStart = 0;
            t = GetXYByPos(p, p->wLength);
            p->wWindowStart = oldstart;
            line = HIWORD(t);
            line += 1;
            lResult = line;
        }
        else
            lResult = 1;

        break;

    case EM_GETMODIFY:
        lResult = (LRESULT)p->bModify;
        break;

	case EM_GETSELTEXT:
		pszText = (PSTR)lParam;
		
		if (!pszText)
			return 0;
		if (p->wPosStart > p->wPosEnd)
		{
			bSwap = TRUE;
			SwapPos(&p->wPosStart, &p->wPosEnd);
		}
		
		if (p->pEditText)
		{
            len = p->wPosEnd - p->wPosStart;
            if (wParam == 0)
            {
                strncpy(pszText, p->pEditText + p->wPosStart, len);
                pszText[len] = 0;
                lResult = len;
            }
            else // Patch for recipient control
            {
                if (wParam == 1)
                {
                    strncpy(pszText, p->pEditText + p->wPosStart, 1);
                    pszText[1] = '\0';
                }
                else
                {
                    if (p->wPosStart > 0)
                    {
                        strncpy(pszText, p->pEditText + p->wPosStart - 1, 1);
                        pszText[1] = '\0';
                    }
                    else
                    {
                        pszText[0] = '\0';
                    }
                }
                lResult = 1;
            }
        }
		else
		{
			pszText[0] = 0;
			lResult = 0;
		}
		
		if (bSwap)
			SwapPos(&p->wPosStart, &p->wPosEnd);
		break;
    
	case EM_GETSEL:
        
		pdwStart = (LPDWORD)wParam;//receives starting position 
        pdwEnd = (LPDWORD)lParam;
        
		if (p->wPosStart > p->wPosEnd)
		{
			SwapPos(&p->wPosStart, &p->wPosEnd);
			bSwap = TRUE;
		}
        if (pdwStart)
            *pdwStart = p->wPosStart;

        if (pdwEnd)
            *pdwEnd = p->wPosEnd;

        //wPosEnd, wPosStart are defined as WORD, so this providing can't be TRUE.
        if (p->wPosEnd >= 65535 || p->wPosStart >= 65535)
        {
            lResult = -1;
            break;
        }

        lResult = ((p->wPosEnd)<<16) + (p->wPosStart);
		if (bSwap)
			SwapPos(&p->wPosStart, &p->wPosEnd);

        break;

    case EM_LIMITTEXT:

        cchMax = (int)wParam;//text length can be input by user, in characters 
        
        if (p->pEditText)
        {
            if (!p->bMultiLine && p->bHScrollBar)
            {
                if (cchMax <= 0 || cchMax > SINGLE_MAXLENGTH)
                {
                    cchMax = SINGLE_MAXLENGTH;
                }
            }
            else if (p->bMultiLine && p->bVScrollBar)
            {
                if (cchMax <= 0 || cchMax > MULTI_MAXLENGTH)
                {
                    cchMax = MULTI_MAXLENGTH;
                }
            }
			p->bLimitText  = 1;
            if (cchMax <= p->wMaxLength)
                p->wMaxEditLength = cchMax;
            else
            {
                LPSTR pTmp;

				if ((!p->bMultiLine && !p->bHScrollBar)
					||(p->bMultiLine && !p->bVScrollBar))
                {
                    p->bLimitText = 0;
                    break;
                }
                
				if (p->bReadOnly && p->wState & ES_USERSTRING)
                {
                    p->wMaxLength = cchMax;
                    p->wMaxEditLength = cchMax;
					break;
				}

                //Allocate memory for text buffer
				pTmp = (PSTR)LocalAlloc(LMEM_FIXED, cchMax + 2);

                if (pTmp)
                {
                    p->wMaxLength = cchMax;
                    p->wMaxEditLength = cchMax;
                    //memset(pTmp, 0, p->wMaxLength + 2);
                    memcpy(pTmp, p->pEditText, p->wLength);

                    //释放掉 pEditText 内存
                    LocalFree((HANDLE)p->pEditText);
                    p->pEditText = pTmp;
					*(p->pEditText + p->wLength) = 0;
                }
                else
                {
                    SendMessage(p->hParentWnd, WM_COMMAND, 
                        MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
                }
            }
        }

        //This message does not return a value. 
        break;

    case EM_LINELENGTH:
        ich = (int) wParam;//character index 

        if (p->pEditText)
        {
            if (!p->bMultiLine)
            {
                len = p->wLength;
                if (p->bSelectOn && ich == -1)
                    len -= p->wPosEnd - p->wPosStart;
            }
            else
            {
                oldstart = p->wWindowStart;
                p->wWindowStart = 0;
				hDC = GetDC(hWnd);
                if (p->bSelectOn && ich == -1)
                {
                    IWORD lenStart, lenEnd;

                    t = GetXYByPos(p, p->wPosStart);
                    lenStart = LOWORD(t);
                    t = GetXYByPos(p, p->wPosEnd);
                    col = LOWORD(t);
                    len = ComputeLineChars(p, hDC, p->wPosEnd - col);
					len -= p->wReturnNum;//changed here for return
					lenEnd = len - col;
                    len = lenStart + lenEnd;
                }
                else    
                {
                    //According to MS Windows
                    if (ich > p->wLength) 
                        ich = p->wLength;
                    else if (ich == -1)
                        ich = p->wCursorPos;
                    t = GetXYByPos(p, ich);
                    col = LOWORD(t);
                    len = ComputeLineChars(p, hDC, ich-col);
					len -= p->wReturnNum;
				  }
				ReleaseDC(hWnd, hDC);

                p->wWindowStart = oldstart;
            }
            lResult = len;
        }
        break;

    case EM_LINESCROLL:

        lResult = Edit_LineScroll(hWnd, p, wParam, lParam);

        //If the message is sent to a multiline edit control, the return value is TRUE; 
        //if the message is sent to a single-line edit control, the return value is FALSE. 
        break;

    case EM_REPLACESEL:

        //flag that specifies whether replacement can be undone
        //fCanUndo = (BOOL) wParam ;

        //According MS Windows, ES_READONLY also process this message.
        pszReplace = (LPCSTR) lParam;//pointer to replacement text string 

        if (!pszReplace || (p->bReadOnly && p->wState & ES_USERSTRING))
            break;
        if (p->bReadOnly && p->bMultiLine)
            p->bReplaceSel = 1;

        len = strlen(pszReplace);
        DisplayNewChars(hWnd, p, (BYTE*)pszReplace, len);
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus
            && !p->bReadOnly)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
        }
        break;

    case EM_SETPASSWORDCHAR:
        
        if (wParam == 0)
            p->wState &= ~ES_PASSWORD;
        else if (!p->bMultiLine)
        {
            //According MS Windows.
            p->wState |= ES_PASSWORD;

            if (wParam <= 255)
                p->nPasswordChar =  (UINT) wParam;//character to display 
            else
                p->nPasswordChar =  (UINT)'?';
        }
        break;

    case EM_SETMODIFY:
        
        p->bModify = (BOOL)wParam;
        break;

    case EM_SETSEL:
        
        Edit_SetSel(hWnd, p, wParam, lParam);
        break;

    case EM_UNDO:
        break;

    case EM_SETTITLE:
        Edit_SetTitle(hWnd, p, wParam, lParam);
        break;
    
    case WM_CLEAR:

        if (p->bReadOnly)
            break;

        if (!p->bSelectOn)
        {
            //删除光标处的字符
            p->wPosStart = p->wCursorPos;

            if (p->wPosStart < p->wLength)
            {
				if (p->CharSet == CHARSET_GB18030 && 
					!IsGB18030LeadChar(*(p->pEditText + p->wPosStart)))
                    p->wPosEnd = p->wPosStart + 1;
				else if (p->CharSet == CHARSET_GB18030 && 
					IsGB18030Code2(*(p->pEditText + p->wPosStart + 1)))
                    p->wPosEnd = p->wPosStart + 2;
				else if (p->CharSet == CHARSET_GB18030 && 
					IsGB18030Code4(*(p->pEditText + p->wPosStart + 1)))
                    p->wPosEnd = p->wPosStart + 4;
				else if (p->CharSet == CHARSET_GB2312 && 
					IsGB2312Code(*(p->pEditText + p->wPosStart), 
					*(p->pEditText + p->wPosStart + 1)))
                    p->wPosEnd = p->wPosStart + 2;
				else if (p->CharSet == CHARSET_GBK && 
					IsGBKCode(*(p->pEditText + p->wPosStart), 
					*(p->pEditText + p->wPosStart + 1)))
                    p->wPosEnd = p->wPosStart + 2;
				else if (p->CharSet == CHARSET_BIG5 && 
					IsBIG5Code(*(p->pEditText + p->wPosStart), 
					*(p->pEditText + p->wPosStart + 1)))
                    p->wPosEnd = p->wPosStart + 2;				
				else
					p->wPosEnd = p->wPosStart + 1;
            }
            p->bSelectOn = 1;
        }
        
        if (p->wPosEnd > p->wLength)
            p->wPosEnd = p->wLength;

        if (p->wPosStart > p->wPosEnd)
            SwapPos(&p->wPosStart, &p->wPosEnd);  

        if (p->wPosStart == p->wPosEnd)
            p->bSelectOn = 0;
        else
        {
           DisplayNewChars(hWnd, p, NULL, 0);
        }

        break;
    
    case WM_COPY :

        if (!p->bMultiLine && (p->wState & ES_PASSWORD))
            break;

        if (p->wPosStart > p->wPosEnd)
            SwapPos(&p->wPosStart, &p->wPosEnd);  

        len = p->wPosEnd - p->wPosStart;
        if (len == 0)
            break;
        
        OpenClipboard(hWnd);
        EmptyClipboard();

        hClipboard = LocalAlloc(LMEM_MOVEABLE, len + 1);
        if (!hClipboard)
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
            break;
        }

        pszText = LocalLock(hClipboard);

        if (pszText)
        {
            memcpy(pszText, p->pEditText + p->wPosStart, len);     
            *(pszText +len) = 0;
        }
        
        LocalUnlock(hClipboard);
        SetClipboardData(CF_TEXT, hClipboard);
        CloseClipboard();

        break;
    
    case WM_CUT :

        if ((!p->bMultiLine && (p->wState & ES_PASSWORD)) || p->bReadOnly)
            break;

        if (p->wPosStart > p->wPosEnd)
            SwapPos(&p->wPosStart, &p->wPosEnd);  

        len = p->wPosEnd - p->wPosStart;
        if (len == 0)
            break;

        OpenClipboard(hWnd);
        EmptyClipboard();

        hClipboard = LocalAlloc(LMEM_MOVEABLE, len + 1);
        if (!hClipboard)
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
            break;
        }

        pszText = LocalLock(hClipboard);

        if (pszText)
        {
            memcpy(pszText, p->pEditText + p->wPosStart, len);     
            *(pszText +len) = 0;
        }
        
        LocalUnlock(hClipboard);
        SetClipboardData(CF_TEXT, hClipboard);
        CloseClipboard();

        DisplayNewChars(hWnd, p, NULL, 0);
        break;
    
    case WM_PASTE :
        if (p->bReadOnly)
            break;
        
        OpenClipboard(hWnd);

        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            hClipboard = GetClipboardData(CF_TEXT);
            pszText = LocalLock(hClipboard);

            if (!p->bMultiLine)
            {
                len = 0;
                while (*(pszText + len))
                {
                    if (*(pszText + len) == '\r' || *(pszText + len) == '\n')
                    {
                        *(pszText + len)= '\0';
                        break;
                    }
                    else if (p->wState & ES_NUMBER && ((*(pszText + len) < 0x30) ||
                        (*(pszText + len) > 0x39)))
                    {
                        *(pszText + len)= '\0';
                        break;
                    }
                    len ++;
                }
            }
            else
            {
                len = 0;
                while (*(pszText + len))
                {
                    if (p->wState & ES_NUMBER && ((*(pszText + len) < 0x30) ||
                        (*(pszText + len) > 0x39)) && *(pszText + len) != '\r' &&
                        *(pszText + len) != '\n')
                    {
                        *(pszText + len)= '\0';
                        break;
                    }
                    len ++;
                }
            }
            
            DisplayNewChars(hWnd, p, (BYTE*)pszText, len);
        }

        CloseClipboard();
        break;

    
    case WM_SETFOCUS :

        if (p->bIsSingle == (BYTE)-1)
        {
            HWND hwndNewFocus;

            hwndNewFocus = GetNextDlgTabItem(p->hParentWnd, hWnd, FALSE);
            if (hwndNewFocus == hWnd)
                p->bIsSingle = 1;
            else
                p->bIsSingle = 0;
        }
        if (strlen((const char*)p->szRightKey) == 0)
        {
            SendMessage(p->hParentWnd, PWM_GETBUTTONTEXT, 0,
                (LPARAM)p->szRightKey);
        }
        if (p->bReadOnly)
        {
            if (p->bIsSingle && !(p->wState & CS_NOSYSCTRL)
                && !(p->bMultiLine))
                p->bRealFocus = TRUE;
            else
                p->bRealFocus = FALSE;
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
        }
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                    MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                    MASKRIGHT | MASKLEFT);
        }
        else
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
        if (p->hParentWnd)
        {
            SendMessage(p->hParentWnd, WM_SETLBTNTEXT,
                MAKEWPARAM(p->wID, (p->wLength == 0)), 0);
        }
        // add up&down arrow
        if (!p->bIsSingle)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        else if (p->bMultiLine)
        {
            if (p->wLineCount == 0)
            {
                if (p->bReadOnly)
                    p->wLineCount = p->wIndexCount + 1;
                else
                    p->wLineCount = (WORD)SendMessage(hWnd, (WORD)EM_GETLINECOUNT, 0, 0);
            }
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
			else if (p->wCursorLine == 0 && p->wLineCount == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLDOWN, MASKUP | MASKDOWN);
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        }
        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            NULL, MASKMIDDLE);
        if (p->wState & ES_COMBOEDIT)
            DefWindowProc(p->hParentWnd, wMsgCmd, wParam, lParam);
        else
            DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        p->bFocus = 1;
        
        hDC = GetDC(hWnd);

        if (p->heightoffont > p->wWindowHeight)
            CreateCaret(hWnd, (HBITMAP)1, CURSOR_WIDTH, p->wWindowHeight + EDIT_SPACE);
        else
            CreateCaret(hWnd, (HBITMAP)1, CURSOR_WIDTH, p->heightoffont + EDIT_SPACE);
        
        /*if (!p->bMultiLine && (p->wState & ES_PASSWORD) && 
            !(p->wState & ES_NOHIDESEL))//According MS Windows.
        {
            SetCtlColorEdit(p, hWnd, hDC);
            EDIT_Paint(hWnd, p, hDC);
        }
        //对话框中用TAB键等方法转移焦点后，EDIT的全部反显是由对话框所设，
        //这里只负责恢复以前的状态。According MS Windows.
        else if (!p->bPenDown && p->bSelectOn)*/
        if (!p->bPenDown && p->bSelectOn)
        {
            SetCtlColorEdit(p, hWnd, hDC);
            PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
        }

        if ((p->wState & ES_TITLE) || !p->bMultiLine)
        {
            SetCaretColor(COLOR_WHITE);
        }
        else
        {
            SetCaretColor(COLOR_BLACK);
        }
        SetSoftCursorPos(p, hWnd, hDC);
        if (p->wCursorPos >= p->wWindowStart && p->wCursorLine < p->wWindowLines)
        {
            ShowCaret(hWnd);
            p->bCursorOn = 1;
        }

        ReleaseDC(hWnd, hDC);

        SendMessage(p->hParentWnd, WM_COMMAND, 
            MAKELONG(p->wID, EN_SETFOCUS), (LPARAM)hWnd);

        if (p->wState & ES_TITLE)
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KILLFOCUS :

        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        p->bFocus = 0;
        DestroyCaret();

        //Remove highlight(Hilite).
        if (p->bSelectOn && !(p->wState & ES_NOHIDESEL))
        {
            if (!(p->wState & ES_TITLE))
            {
                hDC = GetDC(hWnd);

                SetCtlColorEdit(p, hWnd, hDC);
                PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wPosEnd);

                ReleaseDC(hWnd, hDC);
            }
            p->wPosStart = p->wPosEnd = 0;
            p->bSelectOn = 0;
        }

        //GetClassName(p->hParentWnd, achClassName, 32);
        //achClassName[31] = 0;
        
        //if (stricmp(achClassName, "COMBOBOX"))
		if (!(p->wState & ES_COMBOEDIT))
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_KILLFOCUS), (LPARAM)hWnd);
        }
        else
        {
            //Here(only in ComboBox) lParam is handle of window receiving focus.
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_KILLFOCUS), (LPARAM)wParam);
            SendMessage(p->hParentWnd, WM_KILLFOCUS, 
                wParam, lParam);
        }
        if (p->hParentWnd)
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, 
                (LPARAM)EXIT_TEXT);

        if (p->wState & ES_TITLE)
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_GETTEXT :
        
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
		pos = len;

        if (p->pEditText)
        {
            //Zhf fix bug for 单行编辑框的最后一个字符若为半个双字节时，显示为填充字符。
			if (p->wLength >= nTextMax)
			{
				pSrc = p->pEditText;
				pDest = pszText;
				while(pos)
				{
					if (p->CharSet == CHARSET_GB18030 && !IsGB18030LeadChar(*pSrc))
					{
						*pDest++ = *pSrc++;
						pos --;
					}
					else if (p->CharSet == CHARSET_GB18030 && 
						IsGB18030Code2(*(pSrc + 1)))
					{
						if (pos < 2)
							break;
						pos -= 2;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;						
					}
					else if (p->CharSet == CHARSET_GB18030 && 
						IsGB18030Code4(*(pSrc + 1)))
					{
						if (pos < 4)
							break;
						pos -= 4;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;						
					}
					else if (p->CharSet == CHARSET_GB2312 && 
						IsGB2312Code(*pSrc, *(pSrc + 1)))
					{
						if (pos < 2)
							break;
						pos -= 2;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;						
					}
					else if (p->CharSet == CHARSET_GBK && 
						IsGBKCode(*pSrc, *(pSrc + 1)))
					{
						if (pos < 2)
							break;
						pos -= 2;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;						
					}
					else if (p->CharSet == CHARSET_BIG5 && 
						IsBIG5Code(*pSrc, *(pSrc + 1)))
					{
						if (pos < 2)
							break;
						pos -= 2;
						*pDest++ = *pSrc++;
						*pDest++ = *pSrc++;						
					}
					else
					{
						*pDest++ = *pSrc++;
						pos --;
					}
				}
				if (pos)
					len -= pos;
			}
			else
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
        
        //The return value is the number of characters copied. 
        break;

    case WM_SETTEXT :
        pch = pszText = (PSTR)lParam;
		if (!pszText)
            break;
		p->wLength = 0;
		
		if ((p->bReadOnly && (p->wState & ES_USERSTRING)))
		{
			len = strlen(pszText);
			
			if (len > SINGLE_MAXLENGTH && !p->bMultiLine)
				p->wLength = p->wMaxLength = SINGLE_MAXLENGTH;
			else if (len > MULTI_MAXLENGTH && p->bMultiLine)
				p->wLength = p->wMaxLength = MULTI_MAXLENGTH;
			
			p->pEditText = pch;
		}
		else
		{
			if (!p->bMultiLine)
            {
                len = strlen(pszText);
				
				if (len > p->wMaxLength && p->bHScrollBar 
					&& p->wMaxLength != SINGLE_MAXLENGTH)
				{
					p->wMaxLength = SINGLE_MAXLENGTH;
					pch = (char *)LocalAlloc(LMEM_FIXED, p->wMaxLength + 1);
					
					if (pch == NULL)
						break;
					if (p->pEditText)
						LocalFree((HANDLE)p->pEditText);
					p->pEditText = pch;
				}
				
				if (len > p->wMaxLength)
				{
					len = p->wMaxLength;
					SendMessage(p->hParentWnd, WM_COMMAND, 
						MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
                }
                
                strncpy(p->pEditText, pszText, len);
                *(p->pEditText + len) = '\0';
				p->wLength = len;
				
				//according to windows, only send for single line
				SendMessage(p->hParentWnd, WM_COMMAND, 
					MAKELONG(p->wID, EN_UPDATE), (LPARAM)hWnd);
			}
			else
            {
                len = strlen(pszText);
				
				if (!p->bVScrollBar)// && len > p->wMaxLength
				{
					int wLineCount = 0, wLineLength = 0, wReturn = 0;
                    while(*pch)
                    {
                        if (*(pch) == '\r' && *(pch+1) == '\n')
                        {
                            wLineCount ++;
                            *(p->pEditText + p->wLength) = *(pch++);
                            p->wLength ++;
                            wLineLength = 0;
                            wReturn = 2;
                        }
                        else if (*(pch) == '\r' || *(pch) == '\n')
                        {
                            wLineCount ++;
                            wLineLength = 0;
                            wReturn = 1;
                        }
                        else if (wLineLength == p->wScrollLineChars)
                        {
                            wLineCount ++;
                            wLineLength = 0;
                            wReturn = 0;
                        }
                        else
                        {
                            wLineLength ++;
                        }
                        
                        *(p->pEditText + p->wLength) = *(pch++);
                        p->wLength ++;
                        
                        //if (wLineCount == p->wWindowLines)
                        
                        if (p->wLength > p->wMaxLength)
                        {
                            SendMessage(p->hParentWnd, WM_COMMAND, 
                                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
                            p->wLength = p->wMaxLength;
                            break;
                        }
                        if (wLineCount == p->wWindowLines)
                        {
                            p->wLength -= wReturn;
                            break;
                        }
                    }
                        
                        
                    *(p->pEditText + p->wLength) = '\0';
				}
				else if (len > p->wMaxLength)
				{
					if(p->wMaxLength != MULTI_MAXLENGTH)
					{
						p->wMaxLength = MULTI_MAXLENGTH;
						
						pch = (PSTR)LocalAlloc(LMEM_FIXED, p->wMaxLength + 2);
						if (pch)
						{
							if (len > p->wMaxLength)
							{
								p->wLength = p->wMaxLength;
								SendMessage(p->hParentWnd, WM_COMMAND, 
									MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);
							}
				            else
                                p->wLength = len;

							LocalFree((HANDLE)p->pEditText);
							p->pEditText = pch;
						}
					}
					else
						p->wLength = p->wMaxLength;
					
					strncpy(p->pEditText, pszText, len);
					*(p->pEditText + len) = '\0';
				}
				else
				{
					strncpy(p->pEditText, pszText, len);
					*(p->pEditText + len) = '\0';
					p->wLength = len;
				}
				
				if (p->pLineIndex)
				{
					LocalFree((HANDLE)p->pLineIndex);
					p->pLineIndex = NULL;
					p->wIndexCount = 0;
				}
			}			
		}

		if (p->bReadOnly && p->bMultiLine)
		{
			p->wLineCountOrg = (len + p->wScrollLineChars) / p->wScrollLineChars;
			
            if (p->wLineCountOrg <= 2)
				p->wLineCountOrg = 40;
		
            p->pLineIndex = (WORD *)LocalAlloc(LMEM_FIXED, p->wLineCountOrg * 3 / 2 * sizeof(WORD));
			
            if (!p->pLineIndex)
				return FALSE;
			
            *(p->pLineIndex) = 0;
			FormatText(p, (PSTR)p->pEditText, p->wLength);
		}
		
		/*if (p->wLength > 0  
			&& (*(p->pEditText + p->wLength - 1) == FILL_CHAR
			|| IsInHalfDBCSChar(p->pEditText, p->wLength)))
		{
			p->wLength --;
		}*/
		
		if (!p->bReplaceSel)
			p->wCursorPos = 0;
		
		p->wWindowStart = 0;
		p->wOffsetLine  = 0;
		
		p->bSelectOn    = 0;
		p->wPosEnd = p->wPosStart = 0;
		
		AdjustDisplayWindow(hWnd, p);//This function also set p->wCursorCol.
        InvalidateRect(hWnd, NULL, TRUE);
		p->bModify = 1;
		//according to windows
		if (!p->bMultiLine)
		{
			SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
        }
        if (p->bFocus)
        {
            if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0)
            {
                SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
                if (p->wCursorPos == p->wLength)
                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                    MASKRIGHT | MASKLEFT);
                else
                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                    MASKRIGHT | MASKLEFT);
            }
            else
            {
                SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                if (p->wCursorPos == p->wLength)
                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLLEFT, MASKRIGHT | MASKLEFT);
                else
                    SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
            }
        }
        // add up&down arrow
        if (p->bMultiLine && p->bIsSingle)
        {
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLDOWN, MASKUP | MASKDOWN);
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        }

		lResult = TRUE;
		break;
		
    case WM_GETTEXTLENGTH :

        lResult = p->wLength;
        break;

    case WM_HSCROLL:
        if ((p->wState & ES_RIGHT) || (p->wState & ES_CENTER))
        {
            lResult = FALSE;
            break;
        }
        wCode = (WORD)wParam;
        nPos = HIWORD(wParam);//scroll box position 
        DoHScroll(p, hWnd, wCode, nPos);

        lResult = TRUE;
        break;

    case WM_VSCROLL:
        wCode = LOWORD(wParam);//scroll bar value 
        nPos = HIWORD(wParam);//scroll box position 

        if (p->bMultiLine && p->bVScrollBar)
            DoVScroll(p, hWnd, wCode, nPos);
        lResult = TRUE;
        break;

    case WM_ENABLE:
        p->bEnable = (BYTE) wParam;//enabled/disabled flag 
        EraseCaret(hWnd, p);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;

    case WM_SIZE:
        
        Edit_Size(p, hWnd, lParam);
        break;

	case WM_CANCELMODE :
		if (p->bPenDown == 0)
            break;
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		SendMessage(hWnd, WM_PENUP, 0, MAKELPARAM(pt.x, pt.y));
		break;

    case WM_ERASEBKGND :
        
        hDC = (HDC)wParam;
        GetClientRect(hWnd, &rcClient);
        if ( p->bEnable)
            hBrush = SetCtlColorEdit(p, hWnd, hDC);
        else
            hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
//        FillRect(hDC, &rcClient, hBrush);
        lResult = (LRESULT)TRUE;
        break;
        
    case WM_GETDLGCODE:

        lResult = (LRESULT)DLGC_WANTCHARS | DLGC_HASSETSEL | 
            DLGC_WANTARROWS | DLGC_WANTALLKEYS;

        if ((p->bMultiLine && (p->wState & ES_WANTRETURN)) || 
            (p->wState & ES_COMBOEDIT))
        {
            lResult |= DLGC_WANTRETURN;
        }
        
        //If the user presses ALT+BACKSPACE(undo), it also returns DLGC_WANTMESSAGE.
        break;

    case WM_UNDO:
        
        break;

    case WM_SETFONT:
        
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        
        if (hFont == 0)
            break;
        
        p->hFont = hFont;

        hDC = GetDC(hWnd);
        SelectObject(hDC, hFont);
        GetTextExtentPoint32(hDC, "W", 1, &size);

        if (p->wWindowWidth < size.cx)
            p->wLineChars   = 1;
        else
        {
            p->wLineChars   = (IWORD)(p->wWindowWidth / p->widthoffont);
        }

        if (!p->bHScrollBar)//(p->wState & WS_HSCROLL))
        {
            p->wScrollLineChars = p->wLineChars;
        }

        if (p->wState & ES_MULTILINE)
        {
            p->wWindowLines = p->wWindowHeight / (p->heightoffont + EDIT_SPACE + 1);
        }
        
        AdjustDisplayWindow(hWnd, p);//This function also set p->wCursorCol.
        
        ReleaseDC(hWnd, hDC);

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    default :

        return DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

#if (!NOKEYBOARD)
//#ifndef SCP_NOKEYBOARD
static BOOL ProcessKey(HWND hWnd, PEDITDATA p, WORD vkey, WPARAM wParam, LPARAM lParam)
{
    BOOL    bNotRedraw = FALSE, bMoveCursor = FALSE;
    HDC     hDC;
    int     oldstart;
    IWORD   oldpos = p->wHScrollChars;
    int     oldCursorLine;

    oldCursorLine = p->wCursorLine;
    switch (vkey)
    {
    default :
        return FALSE;

    case VK_UP:
    case VK_DOWN:
    case VK_HOME:
    case VK_END:
    case VK_LEFT:
    case VK_RIGHT:
        {
            switch (vkey)
            {
            case VK_UP:
                if (p->hParentWnd)
                {
                    if (p->wState & CS_NOSYSCTRL)
                    {
                        if (!p->bMultiLine)
                        {
                            SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
                            return 0;
                        }
                    }
                    if (!p->bIsSingle && !(p->bReadOnly && p->bRealFocus)
                        && ((p->bMultiLine && (p->wOffsetLine + p->wCursorLine
                        <= 0)) || !p->bMultiLine))
                    {
                        if (p->wState & CS_NOSYSCTRL)
                        {
                            SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
                            return 0;
                        }
                        else
                        {
                            SetFocus(GetNextDlgTabItem(p->hParentWnd, hWnd, TRUE));
                            return TRUE;
                        }
                    }
                }
                bMoveCursor = MoveCursorOneLine(p, 1);
				p->bMoveCursorUp = 1;
				FlushInputMessages();
                break;
                
            case VK_DOWN:
                if (p->hParentWnd)
                {
                    if (p->wState & CS_NOSYSCTRL)
                    {
                        if (!p->bMultiLine)
                        {
                            SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
                            return 0;
                        }
                    }
                    if ((!p->bIsSingle && !(p->bReadOnly && p->bRealFocus))
                        && ((p->bMultiLine && (p->wOffsetLine + p->wCursorLine >=
                        p->wLineCount - 1)) || !p->bMultiLine))
                    {
                        if (p->wState & CS_NOSYSCTRL)
                        {
                            SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
                            return 0;
                        }
                        else
                        {
                            SetFocus(GetNextDlgTabItem(p->hParentWnd, hWnd, FALSE));
                            return TRUE;
                        }
                    }
                }
                bMoveCursor = MoveCursorOneLine(p, 2);
				p->bMoveCursorUp = 0;
				FlushInputMessages();
                break;
                
            case VK_HOME:
                if (lParam & CK_CONTROL)
                {
                    if (p->wCursorPos != 0)
                    {
                        p->wCursorPos = 0;
                        bMoveCursor = TRUE;
                    }
                }
                else
                    bMoveCursor = MoveCursorHome(p);
                
                break;
                
            case VK_END:
                if (lParam & CK_CONTROL)
                {
                    if (p->wCursorPos != p->wLength)
                    {
                        p->wCursorPos = p->wLength;
                        bMoveCursor = TRUE;
                    }
                }
                else
                    bMoveCursor = MoveCursorEnd(p);
                
                break;
                
            case VK_LEFT:
                bMoveCursor = MoveCursorHorz(p, 1);
                if ((p->hParentWnd != NULL) && (p->wState & CS_NOSYSCTRL))
                {
                    SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
//                    return 0;
                }
                break;
                
            case VK_RIGHT:
                bMoveCursor = MoveCursorHorz(p, 2);
                if ((p->hParentWnd != NULL) && (p->wState & CS_NOSYSCTRL))
                {
                    SendMessage(p->hParentWnd, WM_KEYDOWN, wParam, lParam);
//                    return 0;
                }
                break;
            }
            
            bNotRedraw = TRUE;
            p->wPosEndOld = p->wPosEnd;
            
            if (lParam & CK_SHIFT)
            {
                p->bSelectOn = 1;
                p->wPosEnd = p->wCursorPos;
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
            {
                if (p->bSelectOn)
                {
                    bMoveCursor = TRUE;
                    bNotRedraw = FALSE;
                    
                    //取消 Select 标记
                    p->bSelectOn = 0;
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            
            if (bMoveCursor)
            {
                HBRUSH hBrush;

                hDC = GetDC(hWnd);
                oldstart = p->wWindowStart;
                AdjustDisplayWindow(hWnd, p);
                
                if (oldstart != p->wWindowStart || oldpos != p->wHScrollChars)
                    p->bScrollScreen = 1;
                
                hBrush = SetCtlColorEdit(p, hWnd, hDC);
                if (p->bScrollScreen)
                {
                    if (p->bSelectOn)
                    {
                        RECT rcClient;
                        //SIZE size;
                        
                        //GetTextExtentPoint32(hDC, "T", 1, &size);
                        GetEditRect(p, hWnd, &rcClient);

                        ClearEdit(hWnd, hDC, p, hBrush, p->heightoffont + EDIT_SPACE, &rcClient, rcClient.top);
                    }

                    EDIT_Paint(hWnd, p, hDC);
                }
                else
                {
                    //关掉软件光标
                    EraseCaret(hWnd, p);
                    
                    if (p->bSelectOn)
                    {
                        //选择区域在同方向减小。
                        if ((p->wPosEnd >= p->wPosStart && p->wPosEndOld > p->wPosEnd) ||
                            (p->wPosEnd <= p->wPosStart && p->wPosEndOld < p->wPosEnd))
                        {
                            PaintText(p, hWnd, hDC, 0, p->wPosEnd, p->wPosEndOld);
                        }
                        //选择区域在同方向扩大。
                        else if ((p->wPosEnd > p->wPosStart && 
                            p->wPosEndOld >= p->wPosStart) ||
                            (p->wPosEnd < p->wPosStart && p->wPosEndOld <= p->wPosStart))
                        {
                            if (!((vkey == VK_HOME) && (lParam & CK_SHIFT)))
                                PaintText(p, hWnd, hDC, 1, p->wPosEnd, p->wPosEndOld);
                        }
                        //选择区域变为相反方向。
                        //I don't know these condition
                        /*
                        else if (p->wPosEnd >= p->wPosStart && 
                            p->wPosEndOld < p->wPosEnd ||
                            p->wPosEnd <= p->wPosStart && p->wPosEndOld > p->wPosEnd)
                        {
                            PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wPosEndOld);
                            PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
                        }
                        */
                    }
                    else if (!bNotRedraw)
                        PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wPosEnd);
                    
                    DisplayCaret(hWnd, p, hDC);
                }
                ReleaseDC(hWnd, hDC);
            }
            
            //Prepare for SHIFT+arrow to select text.
            if (!p->bSelectOn)
                p->wPosStart = p->wPosEnd = p->wPosEndOld = p->wCursorPos;
            if (p->hParentWnd && (vkey == VK_UP || vkey == VK_DOWN ||
                vkey == VK_LEFT || vkey == VK_RIGHT || vkey == VK_HOME
                || vkey == VK_END))
            {
                SendMessage(p->hParentWnd, WM_CHECKCURSOR, wParam, lParam);
                if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
                {
                    if (!p->bReadOnly)
                        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
                    if (p->wCursorPos == p->wLength)
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                            MASKRIGHT | MASKLEFT);
                    else
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                            MASKRIGHT | MASKLEFT);
                }
                else if (p->bFocus)
                {
                    if (!p->bReadOnly)
                        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
                    if (p->wCursorPos == p->wLength)
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                        SCROLLLEFT, MASKRIGHT | MASKLEFT);
                    else
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                        SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
                }
                // add up&down arrow
                if (p->bMultiLine && oldCursorLine != p->wCursorLine
                    && p->bIsSingle)
                {
                    if (p->wCursorLine == 0 && p->wLineCount == 1)
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                        NULL, MASKUP | MASKDOWN);
					else if (p->wCursorLine == 0 && p->wLineCount == 0)
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                        NULL, MASKUP | MASKDOWN);
                    else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLDOWN, MASKUP | MASKDOWN);
                    else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLUP, MASKUP | MASKDOWN);
                    else
                        SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
                }
            }

            break;
        }

    case VK_PAGEUP:
        SendMessage(hWnd, WM_VSCROLL, SB_PAGEUP, 0);
        break;

    case VK_PAGEDOWN:
        SendMessage(hWnd, WM_VSCROLL, SB_PAGEDOWN, 0);
        break;
    
    case VK_BACK:
		if (p->bReadOnly)
			break;
        if (!p->bSelectOn)
        {
            //Is at top
            if (p->wCursorPos == 0)
                break;
            MoveCursorHorz(p, 1);
        }
        //Fall through, no break.
    case VK_DELETE:
		if (p->bReadOnly)
			break;
        SendMessage(hWnd, (WORD)WM_CLEAR, 0, 0);
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
            if (p->wLength == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                    MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                    MASKRIGHT | MASKLEFT);
        }
        else if (p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
        // add up&down arrow
        if (p->bMultiLine && oldCursorLine != p->wCursorLine 
            && p->bIsSingle)
        {
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
			else if (p->wCursorLine == 0 && p->wLineCount == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLDOWN, MASKUP | MASKDOWN);
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        }
        break;

    case VK_CLEAR:
        if (p->bReadOnly)
			break;
        p->wPosStart = 0;
        p->wPosEnd = p->wLength;
        p->bSelectOn = 1;
        p->bRegionOn = 1;
        p->wCursorPos = 0;
        p->wCursorLine = 0;
        p->wCursorCol = 0;
        DisplayNewChars(hWnd, p, NULL, 0);
		SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, MASKLEFT | MASKRIGHT);
        if (p->bRealFocus)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKALL);
        else if (!p->bIsSingle)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        else
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                NULL, MASKUP | MASKDOWN);
        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
        break;

    case VK_C:
        if (lParam & CK_CONTROL)
            SendMessage(hWnd, (WORD)WM_COPY, 0, 0);
        break;

    case VK_V:
        if (lParam & CK_CONTROL)
            SendMessage(hWnd, (WORD)WM_PASTE, 0, 0);
        break;

    case VK_X:
        if (lParam & CK_CONTROL)
            SendMessage(hWnd, (WORD)WM_CUT, 0, 0);
        break;
    }

    return TRUE;
}
//#endif //SCP_NOKEYBOARD
#endif // NOKEYBOARD

//将起始显示位置向上移动一行，返回新的起始显示位置。
static DWORD UpOneLine(PEDITDATA p, WORD oldstart)
{
    int len, newstart, offsetLine = 0;
	HDC hdc;

    if (oldstart <= 0)
        return 0;
    
    //向前找到第一个回车符,由于换行符可以是俩个，故判断
	if (oldstart > 1 && *(p->pEditText + oldstart - 1) == '\n' 
		&& *(p->pEditText + oldstart - 2) == '\r')
		newstart = FindLineHead(p, oldstart - 2);
	else
		newstart = FindLineHead(p, oldstart - 1);

    //找到包含 oldstart 字符的行
	hdc = GetDC(p->hWnd);
    for (; ;)
    {
        len = ComputeLineChars(p, hdc, newstart);
        if (newstart + len >= oldstart - 1)
            break;
        newstart += len;
    }
	ReleaseDC(p->hWnd, hdc);
    if (newstart != oldstart)
        offsetLine = 1;

    if (newstart == 0)
        offsetLine = p->wOffsetLine;

    return (offsetLine << 16) + newstart;
}

//将起始显示位置向下移动一行，返回新的起始显示位置。
static DWORD DownOneLine(PEDITDATA p, WORD oldstart)
{
    int lentmp, len, EndLine, wPosPageEnd, offsetLine = 0;
	HDC hdc;    

	wPosPageEnd = p->wWindowStart;
	hdc = GetDC(p->hWnd);
	len = ComputeLineChars(p, hdc, oldstart);
	for (EndLine = 0; EndLine < p->wWindowLines; EndLine ++)
	{
		lentmp = ComputeLineChars(p, hdc, wPosPageEnd);
		wPosPageEnd += lentmp;
	}
	ReleaseDC(p->hWnd, hdc);
	
    if (wPosPageEnd < p->wLength)
	{
		EndLine = p->wWindowLines;
	}
	else
	{
        wPosPageEnd = p->wLength;
		EndLine = HIWORD(GetXYByPos(p, wPosPageEnd));
	}

    // - 2 看最后一行是否充满.
    //EndLine = HIWORD(GetXYByPos(p, p->wLength - 2));
    //CurLine = HIWORD(GetXYByPos(p, oldstart));
	//changed here
    if ((oldstart + len - p->wReturnNum >= p->wLength) || 
        (EndLine < p->wWindowLines))
    {
        return oldstart;
    }
    else
    {
        //if(len == p->wScrollLineChars && p->pEditText[oldstart + len] == '\n')
            //len ++;
        
        if (len > 0)
            offsetLine = 1;

        return (offsetLine << 16) + (oldstart + len);
    }
}


//将起始显示位置向上移动一页，返回新的起始显示位置。
static DWORD UpOnePage(PEDITDATA p, WORD oldstart)
{
    int i, newstart, offsetLine = 0;
	newstart = oldstart;
	//反复调用 UpOneLine
	for (i = 0; i < p->wWindowLines; i ++)
	{
		newstart = LOWORD(UpOneLine(p, oldstart));
		//如果到头，退出循环
		if (newstart == oldstart)
			break;
		
		offsetLine ++;
		oldstart = newstart;
	}
    return (offsetLine << 16) + newstart;
} 

//将起始显示位置向下移动一页，返回新的起始显示位置。
static DWORD DownOnePage(PEDITDATA p, WORD oldstart)
{
    int i, newstart, offsetLine = 0;
    int EndLine, count, wPosPageEnd;

    wPosPageEnd = p->wWindowStart + p->wWindowLines * p->wLineChars * 2;
	if (wPosPageEnd < p->wLength)
	{
		EndLine = p->wWindowLines * 2;
	}
	else
	{
        wPosPageEnd = p->wLength;
        EndLine = HIWORD(GetXYByPos(p, wPosPageEnd));
    }

    if (EndLine < p->wWindowLines)
        return oldstart;

    if (EndLine + 1 >= p->wWindowLines * 2)
    {
        count = p->wWindowLines;
    }
    else
    {
        count = EndLine - p->wWindowLines + 1;
    }

    newstart = oldstart;

    //反复调用 DownOneLine
    for (i = 0; i < count; i ++)
    {
        newstart = LOWORD(DownOneLine(p, oldstart));
        //如果到头，退出循环
        if (newstart == oldstart)
            break;

        offsetLine ++;
        oldstart = newstart;
    }

    return (offsetLine << 16) + newstart;
}

//将起始显示位置向左移动一个字符，返回新的起始显示位置。
static int LeftOneLine(PEDITDATA p, int oldstart)
{
    int     newstart;

    if (oldstart <= 0)
        return 0;
    
    newstart = oldstart - 1;
    if (newstart == '\n')
        return oldstart;

    return newstart;
}

//将起始显示位置向右移动一个字符，返回新的起始显示位置。
static int RightOneLine(PEDITDATA p, int oldstart)
{
    int newstart;

    if (oldstart == '\r' || oldstart == '\n')//change here
        return oldstart;
        
    newstart = oldstart + 1;

    if (newstart >= p->wLength)
        return oldstart;
    else
        return newstart;
}


//将起始显示位置向左移动一页，返回新的起始显示位置。
static int LeftOnePage(PEDITDATA p, int oldstart)
{
    int i;
    int newstart;

    newstart = oldstart;
    //反复调用 LeftOneLine
    for (i = 0; i < p->wLineChars; i ++)
    {
        newstart = LeftOneLine(p, oldstart);
        //如果到头，退出循环
        if (newstart == oldstart)
            break;
        oldstart = newstart;
    }
    return newstart;
} 

//将起始显示位置向右移动一页，返回新的起始显示位置。
static int RightOnePage(PEDITDATA p, int oldstart)
{
    int i;
    int newstart;

    newstart = oldstart;
    //反复调用 RightOneLine
    for (i = 0; i < p->wLineChars; i ++)
    {
        newstart = RightOneLine(p, oldstart);
        //如果到头，退出循环
        if (newstart == oldstart)
            break;
        oldstart = newstart;
    }
    return newstart;
}

//计算指定行应显示的字符数
static int ComputeLineChars(PEDITDATA p, HDC hDC, int linestart)
{
    int   count = 0;
    LPSTR ptr;
	int   nSize = 0, i;
	int   nlen1, nlen2;
	
	p->wReturnNum = 0;

//#ifndef _EMULATE_
//	KickDog();
//#endif //_EMULATE_
	
	if (linestart >= p->wLength)
		return 0;
	
	ptr = (LPSTR)p->pEditText + linestart;
	
	if ((DWORD)ptr < (DWORD)(p->pEditText))
		ptr = (LPSTR)p->pEditText;
	
	if (!p->bHScrollBar)
	{
        nlen1 = p->wWindowWidth / p->widthoffont;
        nlen2 = nlen1 * 2;
        
        
        i = 0;
        /*while (i < nlen1)
        {
            if (ISCONTROLCHAR(*ptr))
                break;
            
            ptr ++;
            count ++;
            i ++;
        }
        
        if (ISCONTROLCHAR(*ptr))
        {
            if(*ptr == '\r'&& *(ptr+1) == '\n')
            {
                p->wReturnNum = 2;
                count = count + 2;
            }
            else 
            {
                p->wReturnNum = 1;
                count ++;
            }
            
            goto RET;
        }*/
        
        ptr = (LPSTR)p->pEditText + linestart;
        
        GetTextExtentExPoint(hDC, ptr, -1, p->wWindowWidth, &nSize, NULL, NULL);
        
        if (nSize < count)
        {
            count = nSize;
            p->wReturnNum = 0;
            
            goto RET;
        }
        else
        {
            ptr += count;
			
			i = count;
			while (i < nSize)
			{
				if (ISCONTROLCHAR(*ptr))
					break;
				
				ptr ++;
				count ++;
				i ++;
			}
            
            if (ISCONTROLCHAR(*ptr))
			{
				if(*ptr == '\r'&& *(ptr+1) == '\n')
				{
					p->wReturnNum = 2;
					count = count + 2;
				}
				else 
				{
					p->wReturnNum = 1;
					count ++;
				}
				
				goto RET;
			}
        }


    }
	else
	{
		i = 0;
		while (i < p->wScrollLineChars)
        {
            if (ISCONTROLCHAR(*ptr))
                break;
            
            ptr ++;
            count ++;
            i ++;
        }
        
        if (ISCONTROLCHAR(*ptr))
        {
            if(*ptr == '\r'&& *(ptr+1) == '\n')
            {
                p->wReturnNum = 2;
                count = count + 2;
            }
            else 
            {
                p->wReturnNum = 1;
                count ++;
            }
            
            goto RET;
        }		 
	}

RET : 

//#ifndef _EMULATE_
//	KickDog();
//#endif //_EMULATE_

	return count;
}

//根据光标的X,Y数值获得对应字符在缓冲区中的位置
static int GetPosByXY(PEDITDATA p, int line, int col)
{
    int pos;
    int i, len;
	int nOffset;
	HDC hdc;

    pos = p->wWindowStart;
	hdc = GetDC(p->hWnd);
    for (i = 0; i < line; i ++)
    {
        len = ComputeLineChars(p, hdc, pos);
        pos += len;
    }
    len = ComputeLineChars(p, hdc, pos);
	len -= p->wReturnNum;
	ReleaseDC(p->hWnd, hdc);
    if (col > len)
        col = len;

    //判断是否处于半个汉字上
	nOffset = InHalfDBCSChar(p, p->pEditText + pos, col);
    if (col > 0 && nOffset)
	{
		col += nOffset;
	}

    //如果超过了末尾，返回末尾的位置
    if (pos + col > p->wLength)
        return p->wLength;

    return pos + col;
}

//根据字符在缓冲区中的位置获得光标的位置。返回值的高16位是行数，低16位是列数。
//如果 pos 不在窗口起始显示位置之后则返回 -1.
static DWORD GetXYByPos(PEDITDATA p, int pos)
{
    int line, col, t, len, nOffset;
	HDC hdc;

    //ASSERT(pos >= p->wWindowStart);

    if (pos > p->wLength)
        pos = p->wLength;
    
    if (!p->bMultiLine)
    {
        //单行编辑框
        col = pos - p->wWindowStart;
        return col;
    }

    //多行编辑框
    line = col = 0;
    t = p->wWindowStart;

    len = 0;
    //从窗口的第一行开始累加，直到 pos 所在的行
	hdc = GetDC(p->hWnd);
    while (t < p->wLength)
    {
        len = ComputeLineChars(p, hdc, t);
        if (t + len > pos || len == 0)
            break;

        t = t + len;
        line ++;
    }
	ReleaseDC(p->hWnd, hdc);

    col = pos - t;

    nOffset = InHalfDBCSChar(p, p->pEditText + t, col);
	if (col > 0 && nOffset)
        col -= nOffset;
    else if (col < 0)
        col = 0;

    if (pos > 0 && len != 0)
    {
        if (t + len == pos && (p->pEditText[pos - 1] == '\n' || p->pEditText[pos - 1] == '\r'))//For right align.
        {
            line ++;
            col = 0;
        }
    }
    return (line << 16) + col;
}

//bReversed为TRUE，则把Select部分反白显示，否则重画字符。
static int PaintText(PEDITDATA p, HWND hWnd, HDC hDC, BOOL bReversed, 
                            WORD wPosStart, WORD wPosEnd)
{
    DWORD       t;
    RECT        rc;
    int         line, col, len, count, line1 = 0, i, oldCount, nSize = 0, nOffset,
                nMode = 0;
    COLORREF    clrBKOld = RGB(0, 0, 0), clrTextOld = RGB(0, 0, 0);
    //HPEN        hpenDot, hpenOld;
    SIZE        size;
    char        arcPassWord[MAX_PASSWORD];
    RECT        rcClient;
    HBRUSH      hBrush;
    HFONT       hFontOld;

    if (bReversed && ABS(wPosEnd - wPosStart) < 1)
        return 0;
    
    if (wPosEnd < wPosStart)
        SwapPos(&wPosStart, &wPosEnd);  
    if (wPosEnd < p->wWindowStart)
        return 0;

    if (wPosStart < p->wWindowStart)
        wPosStart = p->wWindowStart;

    //必须保证 pos 不在半个汉字位置上
    nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, 
		wPosStart - p->wWindowStart);
    wPosStart -= nOffset;

    nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, 
		wPosEnd - p->wWindowStart);
    wPosEnd -= nOffset;

#if (!NOSENDCTLCOLOR)
    if (!p->hParentWnd)
    {
#endif // NOSENDCTLCOLOR
        SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
        SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(p->hParentWnd, (WORD)WM_CTLCOLOREDIT, 
            (WPARAM)hDC, (LPARAM)hWnd);
    }
#endif // NOSENDCTLCOLOR

    if (bReversed)
    {
        clrBKOld = SetBkColor(hDC, GetWindowColor(FOCUS_COLOR));
        clrTextOld = SetTextColor(hDC, COLOR_WHITE);
        hBrush = GetStockObject(NULL_BRUSH);
        p->bRegionOn = 1;
    }
    else if ((p->bFocus) && ((p->wState & ES_TITLE) || !p->bMultiLine))
    {
        nMode = SetBkMode(hDC, TRANSPARENT);
        clrTextOld = SetTextColor(hDC, COLOR_WHITE);
        hBrush = GetStockObject(NULL_BRUSH);
    }

    if (!p->bEnable)
    {
        clrBKOld = SetBkColor(hDC, COLOR_LIGHTGRAY);
        hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    }
    if (p->hFont)
        hFontOld = SelectObject(hDC, p->hFont);

//	GetTextExtentPoint32(hDC, "T", 1, &size);
    GetEditRect(p, hWnd, &rcClient);

    if (!p->bMultiLine)//单行
    {
        col = wPosStart - p->wWindowStart;
        rc.top = rcClient.top;
        rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;// + EDIT_SPACE;
        
        if (rc.bottom > rcClient.bottom)
            rc.bottom = rcClient.bottom;
        
//        GetTextExtentExPoint(hDC, p->pEditText + p->wWindowStart, 
//            p->wLength, p->wWindowWidth, &nSize, NULL, NULL);
        if (!(p->wState & ES_PASSWORD))
        {
            GetTextExtentExPoint(hDC, p->pEditText + p->wWindowStart, p->wLength,
                p->wWindowWidth, &nSize, NULL, NULL);
        }
        else
        {
            GetTextExtentPoint32(hDC, (PCSTR)&p->nPasswordChar, 1, &size);
            nSize = p->wWindowWidth / size.cx;
            if (nSize > p->wLength)
            {
                nSize = p->wLength;
            }
            GetPasswordExtent(hDC, p, p->wLength, &size);
        }
        if (bReversed || wPosEnd != p->wLength)
        {
            //计算该行的字符数，wPosStart - col 是该行的头部。
            count = wPosEnd - wPosStart;
            
            if (wPosEnd >= p->wLength)
                count = p->wLength - wPosStart;
            
            if (col + count	> nSize)
                count = nSize - col;
        }
        else
        {
            count = nSize - col;
            if (count < 0)
                return 0;
            
            if (wPosStart + count > p->wLength)
                count = p->wLength - wPosStart;
        }
        
        nOffset = InHalfDBCSChar(p, p->pEditText + (wPosStart - col), 
            col + count);
        count -= nOffset;
        
        if (p->wState & ES_RIGHT)//!p->bHScrollBar && 
        {
            if (p->wWindowStart + nSize > p->wLength)
                len = p->wLength - p->wWindowStart;
            else
                len = nSize;
            
            nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, len);
            len -= nOffset;
            
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, len - col, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart + col,
                    len - col, &size);
            }
            rc.left = p->wWindowWidth - size.cx - CURSOR_WIDTH + rcClient.left;
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, count, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart, count, &size);
            }
            if (bReversed || wPosEnd != p->wLength)
                rc.right = size.cx + rc.left;
            else
                rc.right = p->wWindowWidth + rcClient.left;
        }
        else if (p->wState & ES_CENTER)
        {
            if (p->wWindowStart + nSize > p->wLength)
                len = p->wLength - p->wWindowStart;
            else
                len = nSize;
            
            nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, len);
            len -= nOffset;
            
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, len, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart, len, &size);
            }
            rc.left = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, col, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart, col,
                    &size);
            }
            rc.left += size.cx;
            
            if (bReversed || wPosEnd != p->wLength)
            {
                if (p->wState & ES_PASSWORD)
                {
                    GetPasswordExtent(hDC, p, count, &size);
                }
                else
                {
                    GetTextExtentPoint32(hDC,
                        p->pEditText + p->wWindowStart + col, count, &size);
                }
                rc.right = size.cx + rc.left;
            }
            else
            {
                if (p->wState & ES_PASSWORD)
                {
                    GetPasswordExtent(hDC, p, len - col, &size);
                }
                else
                {
                    GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart
                        + col, len - col, &size);
                }
                rc.right = size.cx + rc.left;
            }
        }
        else
        {
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, col, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart, col,
                    &size);
            }
            rc.left = size.cx + rcClient.left;
            
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, count + col, &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + p->wWindowStart,
                    count + col, &size);
            }
            if (bReversed || wPosEnd < p->wLength)
                rc.right = size.cx + rcClient.left;
            else
                rc.right = p->wWindowWidth + rcClient.left;
        }
        
        if ((wPosStart + count - 1) > 0)
        {
            nOffset = InHalfDBCSChar(p, p->pEditText + wPosStart, count);
            if (ISCONTROLCHAR(*(p->pEditText + (wPosStart + count - 1))) ||
                nOffset)
                len = count - nOffset;
            else
                len = count;
        }
        else
            len = count;
        
        nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, col + len);
        if ((wPosStart == p->wWindowStart) || (wPosEnd == p->wLength) || 
            (len + col - nOffset== nSize))
        {
            int oldleft = rc.left;
            int oldright = rc.right;
            
            if ((wPosEnd == p->wLength) || (len + col - nOffset== nSize))
            {
                rc.right = p->wWindowWidth + rcClient.left;
            }
            
            if (wPosStart == p->wWindowStart)
                rc.left = rcClient.left;
            
            FillRect(hDC, &rc, hBrush);

            rc.left = oldleft;
            rc.right = oldright;
        }
        
        if (!count && !bReversed)
            len = 0;
        
        if (len != 0)
        {
            if (p->wState & ES_PASSWORD)
            {
                for (i = 0; i < len; i++)
                {
                    if (p->bToMask)
                    {
                        arcPassWord[i] = (i != p->wCharToMask - wPosStart)
                            ? p->nPasswordChar : p->pEditText[p->wCharToMask];
                    }
                    else
                    {
                        arcPassWord[i] = p->nPasswordChar;
                    }
                }
                arcPassWord[len] = '\0';
            }
            
            if (p->wState & ES_RIGHT)//!p->bHScrollBar && ()
            {
                if (p->wState & ES_PASSWORD)
                {
                    DrawText(hDC, arcPassWord, len,
                        &rc, DT_RIGHT | DT_VCENTER);//DT_VCENTER
                }
                else
                {
                    DrawText(hDC, p->pEditText + wPosStart, len,
                        &rc, DT_RIGHT | DT_VCENTER);//DT_VCENTER
                }
                
                if (len * size.cx < (rc.right - rc.left))
                {
                    RECT    rcFill;
                    
                    SetRect(&rcFill, rc.left, rc.top, 
                        rc.right - len * size.cx, rc.bottom);
                    FillRect(hDC, &rcFill, hBrush);
                }
            }
            else if (p->wState & ES_CENTER)//!p->bHScrollBar && ()
            {
                if (p->wState & ES_PASSWORD)
                {
                    DrawText(hDC, arcPassWord, len,
                        &rc, DT_CENTER | DT_VCENTER);//DT_VCENTER
                }
                else
                {
                    DrawText(hDC, p->pEditText + wPosStart, len,
                        &rc, DT_CENTER | DT_VCENTER);//DT_VCENTER
                }
            }
            else 
            {
                if (p->wState & ES_PASSWORD)
                {
                    DrawText(hDC, arcPassWord, len,
                        &rc, DT_LEFT | DT_VCENTER);//DT_VCENTER
                }
                else
                {
                    DrawText(hDC, p->pEditText + wPosStart, len,
                        &rc, DT_LEFT |DT_VCENTER);
                }
                
                if (len * size.cx < (rc.right - rc.left))
                {
                    RECT    rcFill;
                    
                    SetRect(&rcFill, rc.left + len * size.cx, 
                        rc.top, rc.right, rc.bottom);
                    FillRect(hDC, &rcFill, hBrush);
                }
            }                
        }
        else if (!bReversed)
            FillRect(hDC, &rc, hBrush);
    }
    else//多行
    {
        t = GetXYByPos(p, wPosStart);
        line = HIWORD(t);
        col = LOWORD(t);
        len = ComputeLineChars(p, hDC, wPosStart - col);//For right align.
        
		nOffset = InHalfDBCSChar(p, p->pEditText + wPosStart, len - col);
        if (len == col && p->bHScrollBar && len != 0 &&
           (len -nOffset == p->wScrollLineChars))//For right align.
        {
            rc.top = line * (p->heightoffont + EDIT_SPACE) + rcClient.top;
            rc.bottom = rc.top + p->heightoffont;
            if (p->wState & ES_RIGHT)//
            {
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                rc.left = rcClient.left;
                rc.right = p->wWindowWidth - size.cx - 
                    CURSOR_WIDTH + rcClient.left;
            }
            else if (p->wState & ES_CENTER)
            {
                RECT rc1;

                rc.left = rcClient.left;
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                rc.right = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
				SetRect(&rc1, rc.right + size.cx, rc.top,
					p->wWindowWidth + rcClient.left, rc.bottom);
                FillRect(hDC, &rc1, hBrush);
            }
            else
            {
                int NumCol;

                if (len > p->wHScrollChars)
                    NumCol = len - p->wHScrollChars;
                else
                   NumCol = 0;

                GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, NumCol, &size);
				rc.left = rcClient.left + size.cx;
                rc.right = p->wWindowWidth + rcClient.left;
            }
            if(rc.left > rc.right)
                rc.left = rc.right;
			//changed at 6/11 
            FillRect(hDC, &rc, hBrush);
            line ++;
            col = 0;
            
            if ((*(p->pEditText + wPosStart) == '\r') && (*(p->pEditText + wPosStart) == '\n'))
                wPosStart += 2;
            else if (*(p->pEditText + wPosStart) == '\n'
                || *(p->pEditText + wPosStart) == '\r')
                wPosStart ++;
        }
		else if (len == col && !p->bHScrollBar && len != 0)
		{
            if (p->wCursorPos == wPosStart)
            {
                rc.top = line * (p->heightoffont + EDIT_SPACE + 1) + rcClient.top;
                rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;
                
                if (rc.bottom > rcClient.bottom)
                    rc.bottom = rcClient.bottom;
                
                if (p->wWindowLines == 1 && p->bMultiLine )//一行时的多行
                {
                    rc.top = rcClient.top;
                    rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;
                }
                if (p->wState & ES_RIGHT)//
                {
                    GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                    rc.left = rcClient.left;
                    rc.right = p->wWindowWidth - size.cx - 
                        CURSOR_WIDTH + rcClient.left;
                }
                else if (p->wState & ES_CENTER)
                {
                    RECT rc1;
                    
                    rc.left = rcClient.left;
                    GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                    rc.right = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
                    SetRect(&rc1, rc.right + size.cx, rc.top,
                        p->wWindowWidth + rcClient.left, rc.bottom);
                    FillRect(hDC, &rc1, hBrush);
                }
                else
                {
                    int NumCol;
                    
                    if (len > p->wHScrollChars)
                        NumCol = len - p->wHScrollChars;
                    else
                        NumCol = 0;
                    
                    GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, NumCol, &size);
                    rc.left = rcClient.left + size.cx;
                    rc.right = p->wWindowWidth + rcClient.left;
                }
                
                if(rc.left > rc.right)
                    rc.left = rc.right;
                //changed at 6/11 
                FillRect(hDC, &rc, hBrush);
            }

			line ++;
			col = 0;
            if ((*(p->pEditText + wPosStart) == '\r') && (*(p->pEditText + wPosStart) == '\n'))
                wPosStart += 2;
            else if (*(p->pEditText + wPosStart) == '\n'
                || *(p->pEditText + wPosStart) == '\r')
                wPosStart ++;
        }
        //change here
        if (!bReversed && line >= p->wWindowLines)// && p->wWindowLines != 1)
            return 0;

        if (bReversed || wPosEnd != p->wLength)
        {
            t = GetXYByPos(p, wPosEnd);
            line1 = HIWORD(t);
            len = ComputeLineChars(p, hDC, wPosEnd - LOWORD(t));//For right align.
            
			nOffset = InHalfDBCSChar(p, p->pEditText + wPosEnd - LOWORD(t), len - col);
			if (len == LOWORD(t) && p->bHScrollBar &&
				(len -nOffset == p->wScrollLineChars))
			{
				line1 ++;
			}
			else if (len == LOWORD(t) && !p->bHScrollBar)
            {
                line1 ++;
            }
        }

        //设置rc的坐标为line * EMU_LINEHEIGHT, col * EMU_CHARWIDTH
        rc.top = line * (p->heightoffont + EDIT_SPACE + 1) + rcClient.top;
        rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;

        if (rc.bottom > rcClient.bottom)
            rc.bottom = rcClient.bottom;
        
        if (p->wWindowLines == 1 && p->bMultiLine )//一行时的多行
        {
            rc.top = rcClient.top;
            rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;
        }
        
        for (; ;)
        {
            len = ComputeLineChars(p, hDC, wPosStart - col);

            if (line >= line1 && (bReversed || wPosEnd != p->wLength))//最后一行
                count = wPosEnd - wPosStart;
            else

                count = len - col;

            if ((bReversed || wPosEnd != p->wLength) && (line >= p->wWindowLines || 
                wPosEnd <= p->wWindowStart || (wPosEnd != p->wLength && count == 0)))
            {
                break;
            }
			len -= p->wReturnNum;
            nOffset = InHalfDBCSChar(p, p->pEditText + wPosStart - col, len);
            len -= nOffset;

            //clean the old characters
            if (p->wState & ES_RIGHT)//
            {
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                rc.left = rcClient.left;
                rc.right = p->wWindowWidth - size.cx - 
                    CURSOR_WIDTH + rcClient.left;
            }
            else if (p->wState & ES_CENTER)
            {
                RECT rc1;

                rc.left = rcClient.left;
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, len, &size);
                rc.right = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
				SetRect(&rc1, rc.right + size.cx, rc.top,
					p->wWindowWidth + rcClient.left, rc.bottom);
                FillRect(hDC, &rc1, hBrush);
            }
            else
            {
                int NumCol;

                if (len > p->wHScrollChars)
                    NumCol = len - p->wHScrollChars;
                else
                   NumCol = 0;

                GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, NumCol, &size);
				rc.left = rcClient.left + size.cx;
                rc.right = p->wWindowWidth + rcClient.left;
            }

            if(rc.left > rc.right)
                rc.left = rc.right;
			//changed at 6/11 
            FillRect(hDC, &rc, hBrush);

            if (wPosStart + count > p->wLength)
                count = p->wLength - wPosStart;

            oldCount = count;
            nOffset = InHalfDBCSChar(p, p->pEditText + wPosStart, count);
            if ((count > 0) && ((*(p->pEditText +(wPosStart + count - 1)) == '\n')
                && (*(p->pEditText +(wPosStart + count - 2)) == '\r')))
            {
                count -= 2;
            }
            else if ((count > 0) && (ISCONTROLCHAR(*(p->pEditText +(wPosStart + count - 1)))
                || nOffset))
            {
                count -= nOffset;
            }
			GetTextExtentPoint(hDC, "T", -1, &size);
            if (p->wState & ES_RIGHT)//For right align.
            {
                if (wPosStart == p->wWindowStart && !bReversed && wPosEnd == p->wLength)
                {
                    ClearEdit(hWnd, hDC, p, hBrush, size.cy + EDIT_SPACE, &rcClient, rcClient.top);
                }
                
                //For right align.
                if ((len - col) < count || len <= col || 
                    (!bReversed && wPosEnd == p->wLength))
                {
                    rc.right = p->wWindowWidth -  CURSOR_WIDTH + rcClient.left;
                }
                else
				{
					GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
						len - count - col, &size);
                    rc.right = p->wWindowWidth - size.cx - 
                        CURSOR_WIDTH + rcClient.left;//For right align.
				}

                GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
					len - col, &size);
				rc.left = p->wWindowWidth - size.cx - 
                    CURSOR_WIDTH + rcClient.left;//For right align.
            }
            else if (p->wState & ES_CENTER)//!p->bHScrollBar && ()//For center align.
            {
                if (wPosStart == p->wWindowStart && !bReversed && wPosEnd == p->wLength)
                {
                    ClearEdit(hWnd, hDC, p, hBrush, size.cy + EDIT_SPACE, &rcClient, rcClient.top);
                }
                
                GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
					len, &size);
				rc.left = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
				if ((len - col) < count || len <= col || 
                    (!bReversed && wPosEnd == p->wLength))
				{
					rc.right = size.cx;
				}
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, 
					col, &size);
				rc.left += size.cx;
                rc.right += rc.left;

                if ((len - col) < count || len <= col || 
                    (!bReversed && wPosEnd == p->wLength))
                {
                    rc.right -= size.cx;
                }
                else
				{
					GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, 
					count, &size);
                    rc.right = size.cx + rc.left;
				}
            }
            else
            {
				GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, 
					col, &size);
                rc.left = size.cx + rcClient.left;

                if (bReversed || wPosEnd != p->wLength)
				{
					GetTextExtentPoint32(hDC, p->pEditText + wPosStart - col, 
					col + count, &size);
                    rc.right = size.cx + rcClient.left;
				}
                else
                    rc.right = p->wWindowWidth + rcClient.left;
            }

            if (oldCount > 0)
                len = count;
            else
            {
                if (bReversed || wPosEnd != p->wLength)
                    break;
                else
                    len = 0;
            }

            count = oldCount;

            if (len != 0)
            {
                if (p->wState & ES_RIGHT)//!p->bHScrollBar && ()
                {
                    DrawText(hDC, p->pEditText + wPosStart, len,
                        &rc, DT_RIGHT | DT_VCENTER);//DT_VCENTER
					GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
					len, &size);

                    if (size.cx < (rc.right - rc.left))
                    {
                        RECT    rcFill;
                        
                        SetRect(&rcFill, rc.left, rc.top, 
                            rc.right - size.cx, rc.bottom);
                        FillRect(hDC, &rcFill, hBrush);
                    }
                }
                else if (p->wState & ES_CENTER)//!p->bHScrollBar && ()
                {
                    DrawText(hDC, p->pEditText + wPosStart, len,
                        &rc, DT_CENTER | DT_VCENTER);//DT_VCENTER
                }
                else 
                {
                    if (!p->bHScrollBar)
                    {
                        DrawText(hDC, p->pEditText + wPosStart, len,
                            &rc, DT_LEFT | DT_VCENTER);//DT_VCENTER

						GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
							len, &size);
                        if (size.cx < (rc.right - rc.left))
                        {
                            RECT    rcFill;
                            
                            SetRect(&rcFill, rc.left + size.cx, 
                                rc.top, rc.right, rc.bottom);
                            FillRect(hDC, &rcFill, hBrush);
                        }
                    }
                    else
                    {
                        int wStartNew = wPosStart, lenNew;
                        int firstPos;

                        firstPos = GetPosByXY(p, line, 0);

                        if (col < p->wHScrollChars)
                        {
                            lenNew = len - (p->wHScrollChars - col);
                            wStartNew = wPosStart + p->wHScrollChars - col;
                            GetTextExtentExPoint(hDC, p->pEditText + wStartNew, -1,
								p->wWindowWidth, &nSize, NULL, NULL);
                            rc.left = rcClient.left;

                            if (lenNew > 0)
                            {
                                if (lenNew > nSize)
                                    lenNew = nSize;
                    
                                nOffset = InHalfDBCSChar(p, p->pEditText + firstPos, 
									p->wHScrollChars + lenNew);
                                if (((*(p->pEditText +(wStartNew + lenNew - 1)) == '\n')
                                    && (*(p->pEditText +(wStartNew + lenNew - 2)) == '\r')))
                                {
                                    lenNew -= 2;
                                    rc.right = p->wWindowWidth 
                                        - (p->wWindowWidth - p->wLineChars * size.cy) + rcClient.left;
                                }
                                else if (ISCONTROLCHAR(*(p->pEditText +(wStartNew + lenNew - 1)))
                                    || nOffset)
                                {
                                    lenNew -= nOffset;
                                    rc.right = p->wWindowWidth 
                                        - (p->wWindowWidth - p->wLineChars * size.cy) + rcClient.left;
                                }
                                else
                                {
                                    if (bReversed || wPosEnd != p->wLength)
									{
										GetTextExtentPoint32(hDC, p->pEditText + wStartNew,
											lenNew, &size);
                                        rc.right = rc.left + size.cx;
									}
                                }
                            }
                            else
                            {
                                if(len < p->wHScrollChars)
                                    rc.right = p->wWindowWidth 
                                        - (p->wWindowWidth - p->wLineChars * size.cx) + rcClient.left;
                                else
                                    rc.right = rc.left;
                            }
                        }
                        else
                        {
                            lenNew = len;
                            wStartNew = wPosStart;
							GetTextExtentExPoint(hDC, p->pEditText + wStartNew, -1, 
									p->wWindowWidth, &nSize, NULL, NULL);
                            
                            if ((lenNew + col - p->wHScrollChars > nSize)
                                && (nSize < (int)strlen(p->pEditText + wStartNew)))
                                lenNew = nSize - (col - p->wHScrollChars);

                            if(lenNew > 0)
                            {
                                GetTextExtentPoint32(hDC, p->pEditText + wStartNew, p->wHScrollChars,
									&size);
								rc.left -= size.cx;
                            
								nOffset = InHalfDBCSChar(p, p->pEditText + firstPos, 
									lenNew + col);
                                if (((*(p->pEditText +(wStartNew + lenNew - 1)) == '\n')
                                    && (*(p->pEditText +(wStartNew + lenNew - 2)) == '\r')))
                                {
                                    lenNew -= 2;
                                    rc.right = p->wWindowWidth 
                                        - (p->wWindowWidth - p->wLineChars * size.cy) + rcClient.left;
                                }
                                else if (ISCONTROLCHAR(*(p->pEditText +(wStartNew + lenNew - 1)))
                                    || nOffset)
                                {
                                    lenNew -= nOffset;
                                    rc.right = p->wWindowWidth 
                                        - (p->wWindowWidth - p->wLineChars * size.cx) + rcClient.left;
                                }
                                else
                                {
                                    if (bReversed || wPosEnd != p->wLength)
									{
										GetTextExtentPoint32(hDC, p->pEditText + wStartNew, lenNew,
											&size);
                                        rc.right = rc.left + size.cx;
									}
                                }
                            }
                            else
                            {
                                rc.left = rc.right = rcClient.left;
                            }

                        }

                        if (lenNew > 0)
                        {
							nOffset = InHalfDBCSChar(p, p->pEditText + firstPos, 
                                p->wHScrollChars);
                            if (wStartNew == firstPos + p->wHScrollChars && nOffset)
                            {
								RECT    rc1;
								
								SetRect(&rc1, rc.left, rc.top, 
									rc.left + size.cy, rc.bottom);
								FillRect(hDC, &rc1, hBrush);

                                if (nOffset)
									wStartNew += nOffset;
                                lenNew -= nOffset;
                                rc.left += size.cy;
                            }

                            if (lenNew)
                            {
                                DrawText(hDC, p->pEditText + wStartNew, lenNew,//wPosStart
                                  &rc, DT_LEFT | DT_VCENTER);

								GetTextExtentPoint32(hDC, p->pEditText + wStartNew, 
									lenNew, &size);

                                if (size.cx < (rc.right - rc.left))//Client
                                {
                                    RECT    rcFill;
                                    SetRect(&rcFill, rc.left + size.cx, 
                                        rc.top, rc.right, rc.bottom);
                                    FillRect(hDC, &rcFill, hBrush);
                                }
                            }
                            else
                            {
                                //clean the old characters
                                FillRect(hDC, &rc, hBrush);
                            }
                        }
                        else
                        {
                            FillRect(hDC, &rc, hBrush);
                        }
                    }
                }
            }
            else if (!bReversed && wPosEnd == p->wLength && len)
            {
                if (p->bHScrollBar)
                {
                    GetTextExtentPoint32(hDC, p->pEditText + wPosStart, 
						p->wHScrollChars, &size);
					rc.left -= size.cx;
                    if (rc.left < 0)
                        rc.left = rcClient.left;
                }
                FillRect(hDC, &rc, hBrush);
            }

            line ++;
            wPosStart = wPosStart + count;
            if (!bReversed && (line >= p->wWindowLines || count == 0))
                break;
            col = 0;
            //修改 rc 的X1, Y1, Y2参数
            rc.left = rcClient.left;
            rc.top = rc.bottom + 1;
            rc.bottom = rc.top + p->heightoffont + EDIT_SPACE;

            if (!bReversed && wPosEnd == p->wLength)
                rc.right = p->wWindowWidth + rcClient.left;
        }

        if (!bReversed && wPosEnd == p->wLength)//在删除情况下用于清除选中字符
            ClearEdit(hWnd, hDC, p, hBrush, p->heightoffont + EDIT_SPACE, &rcClient, rc.bottom + 1);
		/*
        if (p->bUnderline && bReversed)
        {
            SelectObject(hDC, hpenOld);
            DeleteObject(hpenDot);
        }*/
    }

    if ((p->bFocus) && ((p->wState & ES_TITLE) || !p->bMultiLine))
    {
        SetBkMode(hDC, nMode);
//        DeleteObject(hBrush);
    }

    if (bReversed || !p->bEnable)
    {
        SetBkColor(hDC, clrBKOld);
        SetTextColor(hDC, clrTextOld);
    }
    if (p->hFont)
        hFontOld = SelectObject(hDC, p->hFont);
    p->bRedraw = 0;
    return 0;
}

static void SwapPos(WORD *pwPosStart, WORD *pwPosEnd)
{
    WORD temp;
    
    //if (*pwPosStart > *pwPosEnd)
    {   
        temp =*pwPosStart;
        *pwPosStart = *pwPosEnd;
        *pwPosEnd = temp;
    }
}

//在Select位置插入一个字符串
static void MoveBuffer(PEDITDATA p, int pos, int delta, BOOL bReturn)
{
    LPSTR   pSrc, pDest;
    int     count;

    count = p->wLength - pos + 1;
    pSrc = p->pEditText + pos;
    if (bReturn && *pSrc == '\n')
    {
        pSrc ++;
        count --;
    }
    pDest = pSrc + delta;

    if (delta < 0)
    {
        //从后向前移动
        while (count > 0)
        {
            *pDest ++ = *pSrc ++;
            count --;
        }
    }
    else if (delta > 0)
    {
        //从前向后移动
        pSrc += count - 1;
        pDest += count - 1;
        while (count > 0)
        {
            *pDest -- = *pSrc --;
            count --;
        }
    }
}

static int ReplaceSelect(HWND hWnd, PEDITDATA p, BYTE * ptr, int len)
{
    int     delta, nOffset;
    BOOL    bReturn = 0, bEmpty = FALSE;
    
	if (p->bSelectOn)
        p->bReplace = 1;
    
	if (!p->bSelectOn)
        p->wPosStart = p->wPosEnd = p->wCursorPos;
    else if (p->wPosStart > p->wPosEnd)
        SwapPos(&p->wPosStart, &p->wPosEnd);    
    
    if (ptr == NULL)
        len = 0;

    bEmpty = (p->wLength == 0);

    delta = len - (p->wPosEnd - p->wPosStart);//缓冲区应后移的字符个数。

    //delta 大于零为插入，小于零为删除,字符长度不得超过长度限制
    if (p->wLength + delta > p->wMaxEditLength)//For LIMITTEXT.changed here//p->wMaxLength
    {
        //超过长度限制时，COPY到最大长度为止。
        delta = p->wMaxEditLength - p->wLength;//For LIMITTEXT.

        if (delta < 0)
        {
            delta = -(p->wPosEnd - p->wPosStart);
            len = 0;
        }
        else
            len = delta + (p->wPosEnd - p->wPosStart);
        
        if (ptr != NULL)//When just deleting, not send notify message.
        {
            PostMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_MAXTEXT), (LPARAM)hWnd);

        }
		nOffset = InHalfDBCSChar(p, (char *)ptr, len);
		if (len > 0 && nOffset)
		{
			len -= nOffset;
			delta -= nOffset;
		}
    }
    if (delta == -1 && !p->bRegionOn
        && ((*(p->pEditText + p->wCursorPos) == '\r' &&
        *(p->pEditText + p->wCursorPos + 1) == '\n')   
        || ((*(p->pEditText + p->wCursorPos) == '\n' &&
        *(p->pEditText + p->wCursorPos - 1) == '\r'))))
	{
        delta --;
        bReturn = 1;
    }
    //将 SelectEnd 位置之后的字移动 delta
    MoveBuffer(p, p->wPosEnd, delta, bReturn);
    p->wLength += delta;
    
    //将ptr字符串复制到 SelectStart 开始的位置
    if (len > 0 && !p->bReplaceSel)//For LIMITTEXT.
    {
        memcpy(p->pEditText + p->wPosStart, ptr, len);
        FormatText(p, (PSTR)p->pEditText + p->wPosStart, len);
    }

    //修改长度
	/*nOffset = InHalfDBCSChar(p, p->pEditText, p->wLength);	
    if (p->wLength > 0 && nOffset)
    {
        p->wLength -= nOffset;
        *(p->pEditText + p->wLength) = '\0';
    }*/

    if (!p->bMultiLine && delta < 0 && p->wWindowStart + delta >= 0 &&
        (p->wState & ES_CENTER || p->wState & ES_RIGHT))
    {
        p->wWindowStart += delta;
		nOffset = InHalfDBCSChar(p, p->pEditText, p->wWindowStart);
		if (nOffset)
			p->wWindowStart += nOffset;
    }

    if (p->wLength <= 0)
    {
        p->wWindowStart = p->wCursorLine = p->wCursorCol = 0;
    }

    if (((p->wLength == 0) && !bEmpty) || ((p->wLength > 0) && bEmpty))
    {
        SendMessage(p->hParentWnd, WM_SETLBTNTEXT,
            MAKEWPARAM(p->wID, (p->wLength == 0)), 0);
    }

    if(p->bReplaceSel && len > 0)
    {
        int lenold = p->wCursorPos;
        memcpy(p->pEditText + lenold, ptr, len);//p->wCursorPos
		p->wCursorPos += len;
        SendMessage( hWnd, WM_SETTEXT, 0, (LPARAM)p->pEditText);
        p->bReplaceSel = 0;
        return 1;
    }

    //设置光标位置为 SelectStart + len
    p->wCursorPos = p->wPosStart + len;

    //取消 Select 标记
    p->bSelectOn = 0;
    p->wPosEnd = p->wPosStart;
    p->bRegionOn = 0;
    //如果是只读属性，则需要重新制作索引表

    if (delta == 0)
        return -1;
    
    return 0;
}

static void SetSoftCursorPos(PEDITDATA p, HWND hWnd, HDC hDC)
{
    int x, y, len, pos;
    SIZE size;
    RECT rcClient;

    GetEditRect(p, hWnd, &rcClient);
	
	pos = p->wCursorPos;
	len = ComputeLineChars(p, hDC, pos - p->wCursorCol);
	if(len)
		len -= p->wReturnNum;      

    if ((p->wState & ES_RIGHT))// && (p->wLength == len)) //For right align.
    {
        if (p->wState & ES_PASSWORD)
        {
            GetPasswordExtent(hDC, p, (int)(p->wCursorCol), &size);
        }
        else
        {
		    GetTextExtentPoint32(hDC, p->pEditText + pos - p->wCursorCol,
                len - p->wCursorCol, &size);
        }
		
		x = p->wWindowWidth - size.cx - CURSOR_WIDTH + rcClient.left;
    }
    else if ((p->wState & ES_CENTER))// && (p->wLength == len))
    {
		if (p->wState & ES_PASSWORD)
        {
            GetPasswordExtent(hDC, p, len, &size);
        }
        else
        {
		    GetTextExtentPoint32(hDC, p->pEditText + pos - p->wCursorCol,
                len, &size);
        }
		x = (p->wWindowWidth - size.cx) / 2 + rcClient.left;
		
        if (p->wState & ES_PASSWORD)
        {
            GetPasswordExtent(hDC, p, (int)p->wCursorCol, &size);
        }
        else
        {
		    GetTextExtentPoint32(hDC, p->pEditText + pos - p->wCursorCol,
                p->wCursorCol, &size);
        }
		x += size.cx;
    }
    else
    {
        if (!p->bHScrollBar)
        {
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, (int)(p->wCursorCol), &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + pos - p->wCursorCol,
                    p->wCursorCol, &size);
            }
            x = size.cx + rcClient.left;
		}
        else
		{
            if (p->wState & ES_PASSWORD)
            {
                GetPasswordExtent(hDC, p, (int)(p->wCursorCol - p->wHScrollChars),
                    &size);
            }
            else
            {
                GetTextExtentPoint32(hDC, p->pEditText + pos - p->wCursorCol
                    + p->wHScrollChars, p->wCursorCol - p->wHScrollChars, &size);
            }
            x = size.cx + rcClient.left;
		}
    }

	y = (p->wCursorLine) * (p->heightoffont + EDIT_SPACE + 1) + rcClient.top + 1;

    if (p->bFocus)
        SetCaretPos(x, y);
}

static BOOL MoveCursorOneLine(PEDITDATA p, int nFlag)
{
    int newpos = 0, start, oldWindowStart, line, col, oldCol, len, lineWindow;
    DWORD   t;
	HDC     hdc;

    if (!p->bMultiLine)
        return FALSE;

    oldWindowStart = p->wWindowStart;
    p->wWindowStart = 0;

    oldCol = p->wCursorCol;

    t = GetXYByPos(p, p->wCursorPos);
    line = HIWORD(t);
    col = LOWORD(t);
	hdc = GetDC(p->hWnd);
    len = ComputeLineChars(p, hdc, p->wCursorPos - col);
	ReleaseDC(p->hWnd, hdc);
	
	lineWindow = HIWORD(GetXYByPos(p, oldWindowStart));
    
	if (!p->bHScrollBar)
	{
		if (oldCol == 0 && len == col && len + p->wCursorPos - col < p->wLength)
		{
			line ++;
			col = 0;
		}
	}
	else 
	{
		if (oldCol == 0 && len == col)
		{
			line ++;
			col = 0;
		}
	}
    switch (nFlag)
    {
    case 1:
        
        if (line == 0 && lineWindow == 0)
        {
            p->wWindowStart = oldWindowStart;
            return FALSE;
//            if (p->wCursorPos == 0)
//                return FALSE;
//            else
//            {
//                p->wCursorPos = 0;
//                return TRUE;
//            }
        }
		else if (line == 0)//当光标在第一行而文本不位于第一屏时
			line = lineWindow;
        
		line -= 1;
        
        newpos = GetPosByXY(p, line, col);
        p->wWindowStart = oldWindowStart;
        
        if (newpos > p->wWindowStart)
            start = p->wWindowStart;
        else
            start = 0;
        break;
    case 2:
        
        //if ((p->wCursorCol + len - col) >= p->wLength)//changed for only one line
        //{
         //   p->wWindowStart = oldWindowStart;
         //   return FALSE;
       // }
        if (line == p->wLineCount - 1)
        {
            p->wWindowStart = oldWindowStart;
            return FALSE;
        }
//		else if (line == p->wLineCount - 1)//当光标在第一行而文本不位于第一屏时
//			line = lineWindow;

        line += 1;
        
        newpos = GetPosByXY(p, line, col);
        p->wWindowStart = oldWindowStart;
        
        if (newpos > p->wLength)
            newpos = p->wLength;
        
        start = p->wWindowStart;
        break;
    }

//    nOffset = InHalfDBCSChar(p, p->pEditText + start, newpos - start);
//	if (newpos >= start && nOffset)
  //      newpos -= nOffset;


    p->wCursorPos = newpos;
    return TRUE;
}
//光标移至行首
static BOOL MoveCursorHome(PEDITDATA p)
{
    if (p->wCursorPos == 0)
        return FALSE;

    if (!p->bMultiLine)
    {
        p->wCursorPos = 0;
    }
    else
    {
        p->wCursorPos = GetPosByXY(p, p->wCursorLine, 0);
        p->wCursorCol = 0;
    }
    return TRUE;
}

//光标移至行尾
static BOOL MoveCursorEnd(PEDITDATA p)
{
    DWORD   t;
    int     col, len;
	HDC      hdc;

    if (p->wCursorPos >= p->wLength)
        return FALSE;

    if (!p->bMultiLine)
    {
        p->wCursorPos = p->wLength;
    }
    else
    {
        t = GetXYByPos(p, p->wCursorPos);
        col = LOWORD(t);
		hdc = GetDC(p->hWnd);
        len = ComputeLineChars(p, hdc, p->wCursorPos - col);
		ReleaseDC(p->hWnd, hdc);;

        p->wCursorCol = len;
        p->wCursorPos = GetPosByXY(p, p->wCursorLine, p->wCursorCol);// + len - col
    }

    return TRUE;
}

//光标左移一个字符位置
static BOOL MoveCursorHorz(PEDITDATA p, int nFlag)
{
    int newpos = 0, start, nOffset;
    DWORD   t;
    switch(nFlag)
    {
    case 1:
        if (p->wCursorPos == 0)
            return FALSE;

        newpos = p->wCursorPos - 1;
        if (newpos > p->wWindowStart)
            start = p->wWindowStart;
        else
            start = 0;
        //changed here
		nOffset = InHalfDBCSChar(p, p->pEditText + start, newpos - start);
        newpos -= nOffset;
        if (p->wCursorPos > 1)
        {
            if ((*(p->pEditText + p->wCursorPos - 1) == '\n') 
            && (*(p->pEditText + p->wCursorPos - 2) == '\r'))
                newpos --;
        }
        break;
    case 2:
        if (p->wCursorPos >= p->wLength)
            return FALSE;
        
		if (p->CharSet == CHARSET_GB18030)
		{
			if (!IsGB18030LeadChar((*(p->pEditText + p->wCursorPos))))
				newpos = p->wCursorPos + 1;
			else if (IsGB18030Code2((*(p->pEditText + p->wCursorPos + 1))))
				newpos = p->wCursorPos + 2;
			else if (IsGB18030Code4((*(p->pEditText + p->wCursorPos + 1))))
				newpos = p->wCursorPos + 4;
			else
				newpos = p->wCursorPos + 1;
			
		}
		else if ((p->CharSet == CHARSET_ASCII) || (p->CharSet == CHARSET_1252))
			newpos = p->wCursorPos + 1;
		else if (p->CharSet == CHARSET_GB2312)
		{
			if (IsGB2312Code((*(p->pEditText + p->wCursorPos)), 
				(*(p->pEditText + p->wCursorPos + 1))))
				newpos = p->wCursorPos + 2;
			else
				newpos = p->wCursorPos + 1;
		}
		else if (p->CharSet == CHARSET_GBK)
		{
			if (IsGBKCode((*(p->pEditText + p->wCursorPos)), 
				(*(p->pEditText + p->wCursorPos + 1))))
				newpos = p->wCursorPos + 2;
			else
				newpos = p->wCursorPos + 1;
		}
		else if (p->CharSet == CHARSET_BIG5)
		{
			if (IsBIG5Code((*(p->pEditText + p->wCursorPos)), 
				(*(p->pEditText + p->wCursorPos + 1))))
				newpos = p->wCursorPos + 2;
			else
				newpos = p->wCursorPos + 1;
		}

        //changed here
		if (newpos == p->wCursorPos + 1)
		{
			if(*(p->pEditText + p->wCursorPos) =='\r' 
				&& *(p->pEditText + p->wCursorPos + 1) =='\n')
				newpos = p->wCursorPos + 2;
		}
        //避免万一最后一个字符是半个汉字。
        if (newpos > p->wLength)
            newpos = p->wLength;
        break;
    }
    p->wCursorPos = newpos;
    t = GetXYByPos(p, p->wCursorPos);
    p->wCursorLine = HIWORD(t);
    p->wCursorCol = LOWORD(t);
    return TRUE;
}

//调整光标到显示范围之内以及滚动条设置。
static int AdjustDisplayWindow(HWND hWnd, PEDITDATA p)
{
    int         newstart, len, lines, chars, col;
    int         oldstart, oldCol;
    int         curLines, offsetLines, nOffset, nSize = 0, nSizeFull = 0;
    DWORD       t;
    SCROLLINFO  ScrollInfo, OldScrollInfo;
	HDC			hdc;    
    
    hdc = GetDC(hWnd);
    if (!p->bMultiLine)//处理单行编辑框
    {
        if (!p->bSelectOn &&
            (p->wState & ES_CENTER || p->wState & ES_RIGHT))      
        {
            p->bRedraw = 1;
        }

        if (p->wCursorPos - 1 < p->wWindowStart)//光标位置在显示窗口之前
        {
            newstart = p->wCursorPos - p->wLineChars / 3;
            if (newstart < 0)
                newstart = 0;
            nOffset = InHalfDBCSChar(p, p->pEditText, newstart);
            newstart -= nOffset;
            p->wWindowStart = newstart;
            p->bRedraw = 1;
        }
        else
        {
            int *pnDx = NULL;
            SIZE size;

            chars = p->wCursorPos - p->wWindowStart;
            pnDx = (int*)calloc(sizeof(int), chars);
            if (!(p->wState & ES_PASSWORD))
            {
                GetTextExtentExPoint(hdc, p->pEditText + p->wWindowStart, chars,
				    p->wWindowWidth, &nSize, pnDx, &size);
            }
            else
            {
                GetTextExtentPoint32(hdc, (PCSTR)&p->nPasswordChar, 1, &size);
                nSize = p->wWindowWidth / size.cx;
                if (nSize > chars)
                {
                    nSize = chars;
                }
                GetPasswordExtent(hdc, p, chars, &size);
            }
			//CHANGED HERE
			if (chars > nSize)
			{
				newstart = p->wCursorPos - nSize;
                if (!(p->wState & ES_PASSWORD))
                {
                    if (pnDx[newstart - p->wWindowStart - 1] == 0)
                    {
                        GetTextExtentExPoint(hdc, p->pEditText + p->wWindowStart,
                            chars, size.cx, &nSizeFull, pnDx, &size);
                    }
                    while (size.cx - pnDx[newstart - p->wWindowStart - 1]
                        >= p->wWindowWidth)
                    {
                        nSize--;
                        newstart = p->wCursorPos - nSize;
                        if (pnDx[newstart - p->wWindowStart - 1] == 0)
                        {
                            GetTextExtentExPoint(hdc, p->pEditText + p->wWindowStart,
                                chars, size.cx, &nSizeFull, pnDx, &size);
                        }
                    }
                }
                nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart,
                    newstart - p->wWindowStart);
				if (nOffset)
					newstart += nOffset;
                p->wWindowStart = newstart;
                p->bRedraw = 1;
			}
            free((void*)pnDx);
        }
    }
    else//处理多行编辑框
    {
		oldCol = p->wCursorCol;
		//光标位置在显示窗口之前
		if ((!p->bPenDown) && (p->wCursorPos < p->wWindowStart || 
			(p->wCursorPos == p->wWindowStart && 
			(oldCol == p->wScrollLineChars))))
		{
			if (p->bReadOnly)
			{
				int nPos;
				nPos = SearchIndex(p);
				p->wOffsetLine = nPos;
				
				if (nPos == -1)
					p->wWindowStart = 0;
				else
					p->wWindowStart = *(p->pLineIndex + nPos);
			}
			else
			{
				lines = 0;
				newstart = 0;
				//从第一行开始计算，直到该行包含光标位置
				
				for (; ;)
				{
					len = ComputeLineChars(p, hdc, newstart);
					if (newstart + len > p->wCursorPos || 
						(newstart + len == p->wCursorPos 
                        && (p->wCursorPos == p->wWindowStart || 
						p->wCursorPos == newstart + oldCol ||
                        p->wCursorPos == newstart )))
					{
						break;
					}
					//changed here for only one line 
					if (newstart + len >= p->wCursorPos && p->wWindowLines == 1)
						break;
					
					newstart += len; 
					lines ++;
				}
				p->wWindowStart = newstart;
				p->wOffsetLine = lines;
			}
			p->bRedraw = 1;
		}
		
		t = GetXYByPos(p, p->wCursorPos);
		
		lines = HIWORD(t);
		col = LOWORD(t);
		len = ComputeLineChars(p, hdc, p->wCursorPos - col);

		if (oldCol == 0 && len == col && len == p->wScrollLineChars)
		{
			lines ++;
			col = 0;
		}
		//光标位置在显示窗口之后
		if ((!p->bPenDown) && lines >= p->wWindowLines)
		{
			if (p->bReadOnly)
			{
				int nPos;
				
				nPos = SearchIndex(p);
				p->wOffsetLine = nPos;
				
				if (nPos == -1)
					p->wWindowStart = 0;
				else
					p->wWindowStart = *(p->pLineIndex + nPos);
			}
			else
			{
				newstart = p->wWindowStart;
				while (lines >= p->wWindowLines)
				{
					len = ComputeLineChars(p, hdc, newstart);
					newstart += len;
					lines --;
					p->wOffsetLine ++;
				}
				p->wWindowStart = newstart;
			}
			
			p->bRedraw = 1;
		}
		
		if (p->bReadOnly)
			p->wLineCount = p->wIndexCount + 1;
		else
			p->wLineCount = (WORD)SendMessage(hWnd, (WORD)EM_GETLINECOUNT, 0, 0);

        offsetLines = p->wWindowLines - p->wLineCount;//剩余行数
        
        oldstart = p->wWindowStart;
        
        //After delete many lines, the current lines less than one page, 
        //let scroll bar disappear.
        if (offsetLines >= 0)//全文不满一屏。
        {
            p->wWindowStart = 0;
            p->wOffsetLine = 0;

            if (oldstart != 0)
                p->bRedraw = 1;
        }
        else
        {
            int     wPosPageEnd, EndLine;

			wPosPageEnd = p->wWindowStart;
            for (EndLine = 0; EndLine < p->wWindowLines; EndLine ++)
			{
				len = ComputeLineChars(p, hdc, wPosPageEnd);
				wPosPageEnd += len;
			}
            if (wPosPageEnd < p->wLength)
            {
                EndLine = p->wWindowLines;
            }
            else
            {
                wPosPageEnd = p->wLength;
                EndLine = HIWORD(GetXYByPos(p, wPosPageEnd));
            }

            offsetLines = p->wWindowLines - 1 - EndLine;

            curLines = p->wOffsetLine;

			//最后一屏不满,让最后一个字符处于最底行。
            if ((offsetLines > 0) && (curLines != 0))
            {
                DWORD   dwRet;

             	if (p->bReadOnly)
				{
					p->wOffsetLine -= offsetLines;
					if (p->wOffsetLine < 0)
						p->wOffsetLine = 0;

					p->wWindowStart = *(p->pLineIndex + p->wOffsetLine);
				}
				else
				{
                    int i, newstart = oldstart = p->wWindowStart, offsetLine = 0;
                    for (i = 0; i < offsetLines; i ++)
                    {
                        newstart = LOWORD(UpOneLine(p, (WORD)oldstart));

                        //如果到头，退出循环
                        if (newstart == oldstart)
                            break;
                        
                        offsetLine ++;
                        oldstart = newstart;
                    }

                    dwRet = (offsetLine << 16) + newstart;
					p->wWindowStart = LOWORD(dwRet);
					offsetLines = HIWORD(dwRet);

					p->wOffsetLine = curLines - offsetLines;
				}

				p->bRedraw = 1;
            }
            else
                p->wWindowStart = oldstart;
        }
        ScrollInfo.cbSize = sizeof(SCROLLINFO);
        ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
        ScrollInfo.nMin = 0;

        if (p->wState & WS_VSCROLL && p->bMultiLine)
        {
            ScrollInfo.nPage = p->wWindowLines;
            ScrollInfo.nPos = p->wOffsetLine;
            ScrollInfo.nMax = p->wLineCount;

            OldScrollInfo.cbSize = sizeof(SCROLLINFO);
            OldScrollInfo.fMask = SIF_ALL;
            GetScrollInfo(hWnd, SB_VERT, &OldScrollInfo);
            OldScrollInfo.nMax ++;
            if ((OldScrollInfo.nMax != ScrollInfo.nMax ||
                 OldScrollInfo.nMin != ScrollInfo.nMin ||
                 OldScrollInfo.nPos != ScrollInfo.nPos ||
                 OldScrollInfo.nPage != ScrollInfo.nPage) &&
                 (!(((UINT)OldScrollInfo.nMax <= OldScrollInfo.nPage) &&
                    ((UINT)ScrollInfo.nMax <= ScrollInfo.nPage))))
            {
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
            }
        }

        oldCol = p->wCursorCol;

        t = GetXYByPos(p, p->wCursorPos);
        p->wCursorLine = HIWORD(t);
        p->wCursorCol = LOWORD(t);

        len = ComputeLineChars(p, hdc, p->wCursorPos - p->wCursorCol);
		
        if (oldCol == 0 && len == p->wCursorCol)
        {
			//changed here 只有一行时不能修正，否则不能显示光标
			if (p->wWindowLines != 1)
			{
				p->wCursorLine ++;
                p->wCursorCol = 0;
			}
        }

        if (p->bHScrollBar)
        {
            int firstPos;

            firstPos = GetPosByXY(p, p->wCursorLine, 0);

            ScrollInfo.nPage = p->wLineChars;

			if (!p->bReadOnly)
			{
				//从第一行开始计算
				newstart = 0;
				p->wMaxLineChars = 0;
				
				for (; ;)
				{
					//Compute the maxlinechars
					int count;
					
                    len = ComputeLineChars(p, hdc, newstart);
					count = len;
					
					nOffset = InHalfDBCSChar(p, p->pEditText + newstart, len);
					count -= nOffset;
					count -= p->wReturnNum;
					if (count > p->wMaxLineChars)
					{
						//p->bRedraw = 1;
						p->wMaxLineChars = count;
					}
					
					if (newstart + len >= p->wLength)
						break;
					newstart += len;
				}
			}

            len = ComputeLineChars(p, hdc, firstPos);

            nOffset = InHalfDBCSChar(p, p->pEditText + firstPos, len);
			len -= nOffset;
			len -= p->wReturnNum;
            GetTextExtentExPoint(hdc, p->pEditText + firstPos, -1, p->wWindowWidth,
				&nSize, NULL, NULL);
			if (len <= nSize && p->wHScrollChars)
            {
                p->wHScrollChars = 0;
                p->bRedraw = 1;
            }
            else
            {
                int t = 0, line = 0, col = 0, nLineWndStart = 0,
                    nFit = 0, nLineStart = 0;
                // Scroll horizontally begin:
                t = GetXYByPos(p, p->wCursorPos);
                line = HIWORD(t);
                col = LOWORD(t);
                nLineStart = GetPosByXY(p, line, 0);

                nLineWndStart = nLineStart + p->wHScrollChars;
                GetTextExtentExPoint(hdc, p->pEditText + nLineWndStart,
                    p->wCursorPos - nLineWndStart, p->wWindowWidth, &nFit, NULL, NULL);
//                if (p->wCursorCol > nSize + p->wHScrollChars)
                if (nFit < p->wCursorPos - nLineWndStart)
                {
                    //need move right
                    while(p->wCursorCol > nFit + p->wHScrollChars)
                    {
                        int newpos;
                
						newpos = GetHScrollChars(p, (IWORD)(nSize / 3));
                        if (newpos == p->wHScrollChars)
                            break;
                        p->wHScrollChars = newpos;
                        p->bRedraw = 1;
                    }
                    
                }
                else
                {
                    //move left
                    while(p->wCursorCol < p->wHScrollChars + 1)
                    {
                        int newpos;
                        newpos = GetHScrollChars(p, (IWORD)(-nSize / 3));
                        if (newpos == p->wHScrollChars)
                            break;
                        p->wHScrollChars = newpos;
                        p->bRedraw = 1;
                    }
                    
                }
                // Scroll horizontally end.
            }

            if (p->wState & WS_HSCROLL)
            {
                ScrollInfo.nPos = p->wHScrollChars;
                ScrollInfo.nMax = p->wMaxLineChars;
                OldScrollInfo.cbSize = sizeof(SCROLLINFO);
                OldScrollInfo.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_HORZ, &OldScrollInfo);
                OldScrollInfo.nMax ++;
                if ((OldScrollInfo.nMax != ScrollInfo.nMax ||
                    OldScrollInfo.nMin != ScrollInfo.nMin ||
                    OldScrollInfo.nPos != ScrollInfo.nPos ||
                    OldScrollInfo.nPage != ScrollInfo.nPage) &&
                    (!(((UINT)OldScrollInfo.nMax <= ScrollInfo.nPage) &&
                    ((UINT)ScrollInfo.nMax <= ScrollInfo.nPage))))
                {
                    if (ScrollInfo.nMax != 0)
                        ScrollInfo.nMax --;
                    SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                }
            }
        }
    }
    
    oldCol = p->wCursorCol;

    t = GetXYByPos(p, p->wCursorPos);
    p->wCursorLine = HIWORD(t);
    p->wCursorCol = LOWORD(t);

    len = ComputeLineChars(p, hdc, p->wCursorPos - p->wCursorCol);
    
	if (p->bMultiLine && oldCol == 0 && len == p->wCursorCol &&
        len == p->wScrollLineChars)
    {
        p->wCursorLine ++;
        p->wCursorCol = 0;
    }
	ReleaseDC(hWnd, hdc);
    return 0;
}

//static BOOL IsDBCSLeadChar(BYTE c)
//{
//
//    return (c >= 0x81) && (c <= 0xfe);
//}

/*static BOOL IsDBCSSecondChar(BYTE c)
{
#ifndef PDA_LEGEND
    return (c >= 0xa1) && (c <= 0xfe);
#else
    return (c >= 0x40) && (c <= 0xfe);
#endif
}*/

//static BOOL IsControlChar(BYTE c)
//{
//    return (c == 0 || c == '\n' || c == '\r');
//}

//判断是否处于半个汉字
static int InHalfDBCSChar(PEDITDATA p, LPSTR ptr, int offset)
{
	if (p->CharSet == CHARSET_GB18030)
	{
		while (offset >0)
		{
#ifndef _EMULATE_
			KickDog();
#endif //_EMULATE_
			if (!IsGB18030LeadChar((*ptr)))
			{
				offset --;
				ptr ++;
			}
			else if (IsGB18030Code2((*(ptr + 1))))
			{
				offset -= 2;
				ptr += 2;
			}
			else if (IsGB18030Code4((*(ptr + 1))))
			{
				offset -= 4;
				ptr += 4;
			}
			else
			{
				offset --;
				ptr ++;
			}
		}
	}
	else if ((p->CharSet == CHARSET_ASCII) || (p->CharSet == CHARSET_1252))
		return 0;
	else if (p->CharSet == CHARSET_GB2312)
	{
		while (offset > 0)
		{
#ifndef _EMULATE_
			KickDog();
#endif //_EMULATE_
			if (IsGB2312Code((*ptr), (*(ptr + 1))))
			{
				offset -= 2;
				ptr += 2;
			}
			else
			{
				offset --;
				ptr ++;
			}
		}
	}
	else if (p->CharSet == CHARSET_GBK)
	{
		while (offset > 0)
		{
#ifndef _EMULATE_
			KickDog();
#endif //_EMULATE_
			if (IsGBKCode((*ptr), (*(ptr + 1))))
			{
				offset -= 2;
				ptr += 2;
			}
			else
			{
				offset --;
				ptr ++;
			}
		}
	}
	else if (p->CharSet == CHARSET_BIG5)
	{
		while (offset > 0)
		{
#ifndef _EMULATE_
			KickDog();
#endif //_EMULATE_
			if (IsBIG5Code((*ptr), (*(ptr + 1))))
			{
				offset -= 2;
				ptr += 2;
			}
			else
			{
				offset --;
				ptr ++;
			}
		}
	}
	return (0 - offset);
}
static void FormatText(PEDITDATA p, PSTR ptr, int len)
{
	int		pos = 0;	//buffer char index.
	int		count = 0;	//current count of one line.
    int     nCountMax = p->wLineCountOrg;
	int		nSize = 0, nOffset, lentmp = len;
	HDC		hdc;
	PSTR	pTmp = ptr; 
    BOOL    bReturn = FALSE;
    
    hdc = GetDC(p->hWnd);
	while(len > 0)
    {
        
		if (!p->bHScrollBar)
			GetTextExtentExPoint(hdc, ptr, -1, p->wWindowWidth, &nSize, NULL, NULL);
		while(lentmp > 0)
		{
			if (*ptr == '\r' && *(ptr + 1) == '\n')
				break;
			else if (ISCONTROLCHAR(*ptr))
				break;
			else if (count > nSize && !p->bHScrollBar)
				break;
			ptr ++;
			count ++;
			lentmp --;
		}
		if (!p->bHScrollBar && count > nSize)
			count = nSize;
		else if (p->bHScrollBar && count > p->wScrollLineChars)
		{
			nOffset = InHalfDBCSChar(p, pTmp, p->wScrollLineChars);
			count = p->wScrollLineChars - nOffset;
		}
		ptr = pTmp + count;
        bReturn = FALSE;
		if (*ptr == '\r' && *(ptr + 1) == '\n')
        {
			count += 2;
            bReturn = TRUE;
        }
		else if (ISCONTROLCHAR(*ptr))
			count ++;
		pTmp += count;
		ptr = pTmp;
		len -= count;
		lentmp = len;
		pos += count;
		
		if (p->wIndexCount + 1 == nCountMax)
		{
			nCountMax = ReMemAlloc(p, nCountMax);
		}
		if (p->pLineIndex)
		{
			*(p->pLineIndex + p->wIndexCount + 1) = pos;
			p->wIndexCount ++;
		}
		count = 0;
#ifndef _EMULATE_
		KickDog();
#endif //_EMULATE_
		
    }
	if (!bReturn)
        p->wIndexCount --;
	ReleaseDC(p->hWnd, hdc);
}

static int FindLineHead(PEDITDATA p, int pos)
{
    BYTE * ptr;

    ptr = (PBYTE)p->pEditText + pos;
    while (ptr > (PBYTE)p->pEditText)
    {
		/*
        if (*(ptr - 1) == '\n' && *(ptr - 2) == '\r')
		{
			ptr --;
            break;
		}
		*/
		if (*(ptr - 1) == '\n' || *(ptr - 1) == '\r')
			break;
        ptr --;
    }
    return ptr - (PBYTE)p->pEditText;
}

static void DoHScroll(PEDITDATA p, HWND hWnd, int code, int LineCount)
{
    int     newstart = 0;
    HDC     hDC;

    switch (code)
    {
    case SB_LINELEFT:
        if (!(p->bMultiLine || p->bHScrollBar))
            newstart = LeftOneLine(p, p->wWindowStart);
        else if (p->bHScrollBar)
            newstart = GetHScrollChars(p, -1);
        break;

    case SB_LINERIGHT:
        if (!(p->bMultiLine || p->bHScrollBar))
            newstart = RightOneLine(p, p->wWindowStart);
        else if (p->bHScrollBar)
            newstart = GetHScrollChars(p, 1);
        break;

    case SB_PAGELEFT:
        if (!(p->bMultiLine || p->bHScrollBar))
            newstart = LeftOnePage(p, p->wWindowStart);
        else if (p->bHScrollBar)
            newstart = GetHScrollChars(p, (IWORD)(-p->wLineChars));
        break;

    case SB_PAGERIGHT:
        if (!(p->bMultiLine || p->bHScrollBar))
            newstart = RightOnePage(p, p->wWindowStart);
        else if (p->bHScrollBar)
            newstart = GetHScrollChars(p, p->wLineChars);
        break;

#if (!NOKEYBOARD)
    case SB_THUMBTRACK:
#endif // NOKEYBOARD
    case SB_THUMBPOSITION:

        newstart = LineCount;
        if (newstart > p->wMaxLineChars - p->wLineChars)
            newstart = p->wMaxLineChars - p->wLineChars;
        if (newstart < 0)
            newstart = 0;
        break;

    default:
        return;
    }

    hDC = GetDC(hWnd);
    if (!(p->bMultiLine || p->bHScrollBar))
    {
        if (newstart != p->wWindowStart)
        {
            p->wWindowStart = newstart;
            SetCtlColorEdit(p, hWnd, hDC);
            EDIT_Paint(hWnd, p, hDC);
        }
    }
    else if (p->bHScrollBar && newstart != p->wHScrollChars)
    {
        p->wHScrollChars = newstart;
        SetCtlColorEdit(p, hWnd, hDC);
        EDIT_Paint(hWnd, p, hDC);

        if (p->wState & WS_HSCROLL)
        {
            SCROLLINFO  ScrollInfo;
            
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_POS | SIF_DISABLENOSCROLL;
            ScrollInfo.nPos = p->wHScrollChars;
            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
        }
    }
    ReleaseDC(hWnd, hDC);
}

static void DoVScroll(PEDITDATA p, HWND hWnd, int code, int LineCount)
{
    int     newstart = 0, oldstart, offsetLine = 0;
    DWORD   dwRet;
    HDC     hDC;

    oldstart = p->wWindowStart;

    switch (code)
    {
    case SB_LINEUP:
		if (p->bReadOnly)
		{
			if (oldstart > 0)
			{
				offsetLine = -1;
				newstart = *(p->pLineIndex + p->wOffsetLine - 1);
			}
		}
		else
		{
			dwRet = UpOneLine(p, p->wWindowStart);
			offsetLine = - HIWORD(dwRet);
			newstart = LOWORD(dwRet);
		}
			
		p->wOffsetLine += offsetLine;

        break;

    case SB_LINEDOWN:
		
		if (p->bReadOnly)
		{
			if (p->wOffsetLine + p->wWindowLines <= p->wIndexCount)
			{
				offsetLine = 1;
				newstart = *(p->pLineIndex + p->wOffsetLine + 1);
			}
			else
				newstart = oldstart;
		}
		else
		{
			dwRet = DownOneLine(p, p->wWindowStart);
			offsetLine = HIWORD(dwRet);
			newstart = LOWORD(dwRet);
		}
			
		p->wOffsetLine += offsetLine;
        break;

    case SB_PAGEUP:
   
		if (p->bReadOnly)
		{
			if (p->wOffsetLine >= p->wWindowLines)
			{
				offsetLine = 1 - p->wWindowLines;
				newstart = *(p->pLineIndex + p->wOffsetLine + offsetLine);
			}
			else
			{
				newstart = 0;
				offsetLine = -p->wOffsetLine;
			}
		}
		else
		{
			dwRet = UpOnePage(p, p->wWindowStart);
			offsetLine = - HIWORD(dwRet);
			newstart = LOWORD(dwRet);
		}
			
		p->wOffsetLine += offsetLine;
        break;

    case SB_PAGEDOWN:
        
        if (p->bReadOnly)
        {
            if ((p->wIndexCount - p->wOffsetLine - p->wWindowLines) / p->wWindowLines >= 1 )			
            {
                offsetLine = p->wWindowLines - 1;
            } 
            else if (p->wIndexCount >= p->wWindowLines)
            {
                offsetLine = p->wIndexCount - p->wOffsetLine - p->wWindowLines + 1;
            }

            newstart = *(p->pLineIndex + p->wOffsetLine + offsetLine);
            p->wOffsetLine += offsetLine;
        }
		else
		{
			dwRet = DownOnePage(p, p->wWindowStart);
			offsetLine = HIWORD(dwRet);
			newstart = LOWORD(dwRet);
		    p->wOffsetLine += offsetLine;
        }

        break;

    case SB_THUMBPOSITION:
#if (!NOKEYBOARD)
    case SB_THUMBTRACK:
#endif
        if (LineCount == 0)
            newstart = 0;
        else
        {
			if (p->bReadOnly)
			{
				if (LineCount + p->wWindowLines <= p->wIndexCount + 1)
					newstart = *(p->pLineIndex + LineCount);
				else
					newstart = *(p->pLineIndex + p->wIndexCount - p->wWindowLines + 1);
			}
			else
			{
				p->wWindowStart = 0;
                newstart = GetPosByXY(p, LineCount, 0);
				p->wWindowStart = oldstart;
            }
        }

        offsetLine = LineCount - p->wOffsetLine;
        p->wOffsetLine = LineCount;

        break;

    default:
        return;
    }
 
    if (newstart != p->wWindowStart)
    {
        SCROLLINFO  ScrollInfo;

        p->wWindowStart = newstart;
        p->wCursorLine -= offsetLine;

        hDC = GetDC(hWnd);
        SetSoftCursorPos(p, hWnd, hDC);

        if (!p->bScrollScreen)
        {
            RECT    rcClient;
            int     nDistance;

        
            nDistance = - offsetLine * (p->heightoffont + EDIT_SPACE + 1);

            GetEditRect(p, hWnd, &rcClient);
            rcClient.bottom -= (rcClient.bottom - rcClient.top) % (p->heightoffont + EDIT_SPACE + 1);
            //change here for only one line's multiline
            if(rcClient.bottom <= rcClient.top)
                rcClient.bottom = rcClient.top + p->heightoffont + EDIT_SPACE;

            ScrollWindow(hWnd, 0, nDistance, NULL, &rcClient);

            EraseCaret(hWnd, p);
            DisplayCaret(hWnd, p, hDC);
        }

        ReleaseDC(hWnd, hDC);
        p->bScrollScreen = 0;

        ScrollInfo.cbSize = sizeof(SCROLLINFO);
        ScrollInfo.fMask = SIF_POS | SIF_DISABLENOSCROLL;
        ScrollInfo.nPos = p->wOffsetLine;
        
        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
        //changed here
        //InvalidateRect(hWnd, NULL, TRUE);           
    }

}

static void EDIT_Paint(HWND hWnd, PEDITDATA p, HDC hDC)
{
    EraseCaret(hWnd, p);

//    if (p->bSelectOn)
//    {
//        //完全重画。
//        PaintText(p, hWnd, hDC, 0, p->wWindowStart, 
//            (WORD)min(p->wPosEnd, p->wPosStart));
//        PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
//        PaintText(p, hWnd, hDC, 0, (WORD)max(p->wPosEnd, p->wPosStart), 
//            p->wLength);
//    }
//    else
//        PaintText(p, hWnd, hDC, 0, p->wWindowStart, p->wLength);
    InvalidateRect(hWnd, NULL, TRUE);

    DisplayCaret(hWnd, p, hDC);
}

static void EraseCaret(HWND hWnd, PEDITDATA p)
{
    if (p->bCursorOn)
    {
        HideCaret(hWnd);
        p->bCursorOn = 0;
    }
}

static void DisplayCaret(HWND hWnd, PEDITDATA p, HDC hDC)
{
    if (!p->bEnable)
        return;
    SetSoftCursorPos(p, hWnd, hDC);
    
    if (p->wCursorPos >= p->wWindowStart && p->wCursorLine < p->wWindowLines)
    {
		ShowCaret(hWnd);
		p->bCursorOn = 1;
    }
    
    p->bScrollScreen = 0;
}

static int SingleLineMoveRight(PEDITDATA p)
{
    int pos, nOffset;

    if ((p->wWindowStart + p->wLineChars) < p->wLength)
    {
        p->wWindowStart ++;//= p->wLineChars / 2;
        nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart - 1, 1);
		if (nOffset)
			p->wWindowStart += nOffset;
        pos = p->wWindowStart + p->wLineChars;
        if (pos > p->wLength)
            pos = p->wLength;
    }
    else
    {
        if (p->wPosEndOld >= p->wLength)
        {
            return -1;
        }

        pos = p->wLength;// - 1;
    }
    p->bScrollScreen = 1;

    nOffset = InHalfDBCSChar(p, p->pEditText + p->wWindowStart, pos - p->wWindowStart);
    pos -= nOffset;

    return pos;
}

static int SingleLineMoveLeft(PEDITDATA p)
{
    int pos, nOffset;
    
    if (p->wWindowStart - p->wLineChars > 0)
    {
        p->wWindowStart --;//= p->wLineChars / 2;
        nOffset = InHalfDBCSChar(p, p->pEditText, p->wWindowStart);
        p->wWindowStart -= nOffset;
    }
    else
    {
        if (p->wPosEndOld == 0)
        {
            return -1;
        }

        p->wWindowStart = 0;
    }
    pos = p->wWindowStart;
    p->bScrollScreen = 1;

    return pos;
}

static IWORD GetHScrollChars(PEDITDATA p, IWORD offset)
{
    int newpos;

    newpos = p->wHScrollChars + offset;
    if (newpos > p->wMaxLineChars - p->wLineChars)
        newpos = p->wMaxLineChars - p->wLineChars;
    if (newpos < 0)
        newpos = 0;

    return newpos;
}

static void ProcessPen(HWND hWnd, PEDITDATA p, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    RECT        rcClient;
    int         line, x, y, pos, nSize = 0;
    IWORD       col;
    DWORD       t;  
    HDC         hDC;
    SIZE        size;

    switch (wMsgCmd)
    {
    case WM_PENDOWN :
		
//#ifndef _EMULATE_
//		if (IsWindowEnabled(hWnd) && !p->bReadOnly)
//			
//#endif //_EMULATE_
		
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        GetEditRect(p, hWnd, &rcClient);

        if (!PtInRectXY(&rcClient, x, y))
            break;
        ConvertXY(p, &x, &y);
        
        hDC = GetDC(hWnd);
        p->bPenDown = 1;
        SetCapture(hWnd);
        
        line = y / (p->heightoffont + EDIT_SPACE + 1);
        
        if ((p->wState & ES_RIGHT) && (p->wLength <= p->wLineChars))
        {
            int pos1, len;
            
            pos1 = GetPosByXY(p, line, 0);
            len = ComputeLineChars(p, hDC, pos1);
            
			len -= p->wReturnNum;            

            //col number according as from right to left.
            //col = (IWORD)((p->wWindowWidth - x) / size.cx);
			GetTextExtentExPoint(hDC, p->pEditText + pos1, len, p->wWindowWidth - x,
				&nSize, NULL, NULL);
			col = nSize;
            
            //when pen position bigger than half char width, 
            //let caret stay at next char pos.
            //if ((((p->wWindowWidth - x) % size.cx) > size.cx / 2) && (col < p->wLineChars))
              //  col ++;

			if (col > len)
				col = 0;
			else
				col = len - col;
        }
        else if ((p->wState & ES_CENTER) && (p->wLength <= p->wLineChars))
        {
            int pos1, len;
            
            pos1 = GetPosByXY(p, line, 0);
            len = ComputeLineChars(p, hDC, pos1);
            
            //nOffset = InHalfDBCSChar(p, p->pEditText + pos1, len);
			//len -= nOffset;
			len -= p->wReturnNum;            
            
			GetTextExtentPoint32(hDC, p->pEditText + pos1, len, &size);            
			if (x < (p->wWindowWidth - size.cx) / 2)
                col = 0;
            else
			{
				GetTextExtentExPoint(hDC, p->pEditText + pos1, len, 
					x - (p->wWindowWidth - size.cx) / 2, &nSize, NULL, NULL);
                col = nSize;
			}
            
            //if ((((x - (p->wWindowWidth - len * size.cx) / 2) % size.cx) > size.cx / 2) 
              //  && (col < p->wLineChars))
                //col ++;
        }
        else
        {
			int pos1, len;
            
            pos1 = GetPosByXY(p, line, 0);
            len = ComputeLineChars(p, hDC, pos1);
            len -= p->wReturnNum;            
            GetTextExtentExPoint(hDC, p->pEditText + pos1, len, x,
				&nSize, NULL, NULL);
			col = nSize;
        }
        
        //if (col > p->wLineChars)
            //col = p->wLineChars;
        
        if (p->bHScrollBar)////get the real col number in a line
            col += p->wHScrollChars;
        
        if (p->bMultiLine)//点击下半部分滚动
        {
            int line1;
            line1 = (y + 5) / (p->heightoffont + EDIT_SPACE + 1);
            pos = GetPosByXY(p, line1, 0);
			
            if (line1 >= p->wWindowLines && pos <= p->wLength)
                DoVScroll(p, hWnd, SB_LINEDOWN, 0);
		}
        
		if (line >= p->wWindowLines)
            line = p->wWindowLines - 1;
		
        pos = GetPosByXY(p, line, col);
		
        if (col != 0)
            col = LOWORD(GetXYByPos(p, pos));

        if (p->bHScrollBar)
        {
            int pos1;
            IWORD oldpos, newpos = p->wHScrollChars;
            
            oldpos = newpos;
            pos1 = GetPosByXY(p, line, 0);
            
            while (col >= 0 && pos == pos1 && pos == p->wLength &&
                line != 0 && (*(p->pEditText + pos - 1) != '\n' &&
				*(p->pEditText + pos - 1) != '\r'))
            {
                //get a valid position
                line --;
                pos = GetPosByXY(p, line, col);
                pos1 = GetPosByXY(p, line, 0);
            }

            while (pos - pos1 < newpos)
            {
                DoHScroll(p, hWnd, SB_PAGELEFT, 0);
                newpos = p->wHScrollChars;
            }
        }

		p->wCursorPos = pos;
		t = GetXYByPos(p, p->wCursorPos);
		p->wCursorLine = HIWORD(t);
		p->wCursorCol = LOWORD(t);
		
		if (p->bMultiLine && p->wCursorLine < line && pos != p->wLength &&
			p->wCursorCol == p->wScrollLineChars)//For right align.
		{
			p->wCursorLine ++;
			p->wCursorCol = 0;
		} 

        EraseCaret(hWnd, p);
        
		if (p->bSelectOn)
        {
            p->bSelectOn = 0;
            SetCtlColorEdit(p, hWnd, hDC);
            PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wPosEnd);
        }

        p->wPosStart = p->wPosEnd = p->wPosEndOld = pos;       
        
        DisplayCaret(hWnd, p, hDC);
        
        ReleaseDC(hWnd, hDC);
        
        break;
        
    case WM_PENMOVE :
        
       Edit_PenMove(hWnd, p, lParam);
        
        break;
        
    case WM_PENUP :
        
        if (!p->bPenDown)         
            break;

        if (p->uTimerID)
        {
            KillSystemTimer(hWnd, p->uTimerID);
            p->uTimerID = 0;
            p->wSingleLineMove = MOVE_NO;
        }
        
        if (p->wPosEnd < p->wPosStart)
        {
            SwapPos(&p->wPosStart, &p->wPosEnd);    
        }
        
        if (p->wPosEnd == p->wPosStart)
            p->bSelectOn = 0;
        
        p->bPenDown = 0;
        ReleaseCapture();

        if (p->bReadOnly && (p->wState & ES_MULTILINE) && !p->bRealFocus)
            p->bRealFocus = TRUE;
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                MASKRIGHT | MASKLEFT);
        }
        else if (p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
        // add up&down arrow
        if (p->bMultiLine && p->bIsSingle)
        {
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLDOWN, MASKUP | MASKDOWN);
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        }
        if (p->bIsSingle && (p->wState & ES_MULTILINE))
            SendMessage(p->hParentWnd, WM_CHECKCURSOR, wParam, lParam);
        SetFocus(hWnd);
        break;
    }
}

static BOOL Edit_Init(HWND hWnd, PEDITDATA p, LPARAM lParam)
{
    RECT            rcClient;
    PCREATESTRUCT   pCreateData;
    SIZE            size;
    HDC             hDC;
	TEXTMETRIC		TextMetric;
    //structure with creation data 
    pCreateData = (PCREATESTRUCT)lParam;
    hDC = GetDC(hWnd);
	GetTextMetrics(hDC, &TextMetric);
    GetTextExtentPoint32(hDC, "W", 1, &size);
    ReleaseDC(hWnd, hDC);
    
    pCreateData->style |= WS_CLIPCHILDREN;
    
    //Initializes the internal data of edit control
    memset(p, 0, sizeof(EDITDATA));
    GetEditRect(p, hWnd, &rcClient);
    p->wWindowWidth     = (IWORD)(rcClient.right - rcClient.left);
    p->wWindowHeight    = (IWORD)(rcClient.bottom - rcClient.top);
	p->hWnd = hWnd;
	p->CharSet = TextMetric.tmCharSet;
	p->widthoffont = size.cx;
	p->heightoffont = size.cy;
    
    if (p->wWindowWidth < 0)
        p->wWindowWidth = 0;

    if (p->wWindowHeight < 0)
        p->wWindowHeight = 0;

    p->wID = (WORD)(DWORD)pCreateData->hMenu;
    p->hParentWnd = pCreateData->hwndParent;

    p->wState = pCreateData->style;
    p->bEnable = TRUE;
    p->bIsSingle = (BYTE)-1;
        
    p->bUnderline = (p->wState & ES_UNDERLINE) ? 1 : 0;
    p->bReadOnly = (p->wState & ES_READONLY) ? 1 : 0;

    if (p->wState & WS_VSCROLL || p->wState & ES_AUTOVSCROLL)
        p->bVScrollBar = 1;

    if ((p->wState & WS_HSCROLL || p->wState & ES_AUTOHSCROLL))
        p->bHScrollBar = 1;
    if (!p->wState & ES_MULTILINE)
        p->bVScrollBar = 0;

    if ((p->bReadOnly && (p->wState & ES_MULTILINE)) 
        || ((p->wState & ES_RIGHT) && (p->wState & WS_HSCROLL))
        || ((p->wState & ES_CENTER) && (p->wState & WS_HSCROLL)))
        p->bHScrollBar = 0;
    
    if (p->wWindowWidth < p->widthoffont)
        p->wLineChars   = 1;
    else
        p->wLineChars   = (IWORD)(p->wWindowWidth / p->widthoffont);

    if (p->bHScrollBar)
        p->wScrollLineChars = SINGLE_DEFLENGTH;
    else
        p->wScrollLineChars = p->wLineChars;

    if (p->wState & ES_MULTILINE)
    {
        p->bMultiLine = 1;
        p->wWindowLines = p->wWindowHeight / (p->heightoffont + EDIT_SPACE);
        p->wMaxLength = p->wMaxEditLength = MULTI_DEFLENGTH;
		if (!p->bVScrollBar)
			p->wMaxLength = p->wMaxEditLength = p->wWindowLines * p->wScrollLineChars;
    }
    else
    {
        p->wWindowLines = 1;
        
        if (p->wState & ES_PASSWORD)
        {
            p->nPasswordChar = (WORD)'*';//0x2A
            p->wMaxLength = p->wMaxEditLength = MAX_PASSWORD;
        }
        else
            p->wMaxLength = p->wMaxEditLength = p->wScrollLineChars;
    }
    if (pCreateData->lpszName)
       p->wLength = strlen(pCreateData->lpszName);
    
	if (p->wLength > p->wMaxLength)
    {
        p->wLength = p->wMaxLength;
        //nOffset = InHalfDBCSChar(p, (LPSTR)pCreateData->lpszName, p->wLength);
        //p->wLength -= nOffset;
    }

    if (p->bReadOnly && (p->wState & ES_USERSTRING))
		p->pEditText = (PSTR)pCreateData->lpszName;
    else
    {
        p->pEditText = (PSTR)LocalAlloc(LMEM_FIXED, p->wMaxLength + 2);
        if (!p->pEditText)
        {
            SendMessage(p->hParentWnd, WM_COMMAND, 
                MAKELONG(p->wID, EN_ERRSPACE), (LPARAM)hWnd);
            return FALSE;
        }
		p->pEditText[0] = 0;
    }

    if (p->wLength)
    {
        if (!(p->bReadOnly && (p->wState & ES_USERSTRING)))
            strncpy(p->pEditText, pCreateData->lpszName, p->wLength);
		p->pEditText[p->wLength] = 0;
        
        if (p->bReadOnly && p->bMultiLine && !p->pLineIndex)
        {
            p->wLineCountOrg = (p->wLength + p->wScrollLineChars - 1) / p->wScrollLineChars;
			
			if (p->wLineCountOrg <= 2)
				p->wLineCountOrg = 40;
            
			p->pLineIndex = (WORD *)LocalAlloc(LMEM_FIXED, p->wLineCountOrg * 3 / 2 * sizeof(WORD));
            
			if (!p->pLineIndex)
                return FALSE;
			*(p->pLineIndex) = 0;
            //memset(p->pLineIndex, 0, p->wLineCountOrg * 3 / 2 * sizeof(WORD));
        }
        FormatText(p, (PSTR)p->pEditText, p->wLength);
    }

    //Creating succeed.
    return TRUE;
}
    
static void  DrawUnderLine(HWND hWnd, HDC hDC, PEDITDATA p, RECT* pRect)
{
    int nBottom, line = 0;
    HPEN hpenDot, hpenOld;

    if (p->bUnderline)
    {
        nBottom = ((pRect->top) / (p->heightoffont + EDIT_SPACE + 2))
			* (p->heightoffont + EDIT_SPACE + 1) + p->w3DBorder;
        line = nBottom / (p->heightoffont + EDIT_SPACE + 1);
        hpenDot = CreatePen(PS_DOT, 1, COLOR_BLACK);
        hpenOld = SelectObject(hDC, hpenDot);

        while (line < p->wWindowLines && nBottom <= (pRect->bottom - p->heightoffont - EDIT_SPACE))
        {
            nBottom += p->heightoffont + EDIT_SPACE;
            //Draw a dot line under text
            DrawLine(hDC, pRect->left, nBottom, 
                pRect->right, nBottom);
            
            line++;
            nBottom += 1;
        }
        SelectObject(hDC, hpenOld);
        DeleteObject(hpenDot);
    }
}
        
static HBRUSH SetCtlColorEdit(PEDITDATA p, HWND hWnd, HDC hdc)
{
    HBRUSH  hBrush;

#if (!NOSENDCTLCOLOR)
    if (!p->hParentWnd)
    {
#endif
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(p->hParentWnd, (WORD)WM_CTLCOLOREDIT, 
            (WPARAM)hdc, (LPARAM)hWnd);
    }
#endif

    return hBrush;
}

//For edit 3D effect. and leave 1 pixel space for each side.
static void GetEditRect(PEDITDATA p, HWND hWnd, RECT* pRect)
{
    GetClientRect(hWnd, pRect);
    ConvertToEditRect(p, pRect);
}

static void ConvertToEditRect(PEDITDATA p, RECT* pRect)
{
#if (INTERFACE_MONO)

    pRect->left += CX_LEFTMARGIN;
    pRect->right -= CX_RIGHTMARGIN;

    if (p->bMultiLine)
    {
        pRect->top += CY_TOPMARGIN;
        pRect->bottom -= CY_BTMMARGIN;
    }
    
    if (p->wState & ES_TITLE)
    {
        pRect->left += CX_TITLE_LEFTMARGIN;
        pRect->right -= CX_TITLE_RIGHTMARGIN;
        pRect->top += CY_TITLE_TOPMARGIN;
        pRect->bottom -= CY_TITLE_BTMMARGIN;
    }

	p->w3DBorder = 1;
#else // INTERFACE_MONO

    pRect->left += 1;
    pRect->top += 1;
    pRect->bottom -= 1;
    pRect->right -= 1;
	p->w3DBorder = 1;

    if (GetDeviceCaps(NULL, BITSPIXEL) > 2 && !(p->wState & ES_COMBOEDIT))
    {
        pRect->left += 2;
        pRect->top += 2;

        if (!(p->wState & WS_HSCROLL))
            pRect->bottom -= 2;   

        if (!(p->wState & WS_VSCROLL))
            pRect->right -= 2;
		p->w3DBorder = 3;
    }
#endif // INTERFACE_MONO
}
//For edit 3D effect. and leave 1 pixel space for each side.
static void ConvertXY(PEDITDATA p, int *pX, int *pY)
{
    int nBitsPixel;

    if (!pX && !pY)
        return;

    if (pX && *pX > 0)
        *pX -= 1;

    if (pY && *pY > 0)
        *pY -= 1;

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    if (nBitsPixel != 1 && nBitsPixel != 2 && !(p->wState & ES_COMBOEDIT))
    {
        if (pX && *pX > 1)
            *pX -= (p->w3DBorder - 1);

        if (pY && *pY > 1)
            *pY -= (p->w3DBorder - 1);
    }
}

#if (!INTERFACE_MONO)
static void Draw3DEditRect_W95(HDC hdc, RECT* pRect, BOOL bHScroll, BOOL bVScroll)
{
    HPEN hOldPen;

    if (!pRect)
        return;

    //Draw left and top darkgray line
    hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));

    if (bHScroll)
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom);
    else
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    if (bVScroll)
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right, pRect->top);
    else
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    //Draw left and top black line
    SelectObject(hdc, GetStockObject(BLACK_PEN));

    if (bHScroll)
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
            pRect->bottom);
    else
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
            pRect->bottom - 2);

    if (bVScroll)
        DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right, 
            pRect->top + 1);
    else
        DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
            pRect->top + 1);

    //Draw right and bottom light gray line
    SelectObject(hdc, GetStockObject(WTGRAY_PEN));

    if (!bVScroll)
        DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
            pRect->bottom - 1);
    else
        pRect->right += 1;

    if (!bHScroll)
        DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
            pRect->bottom - 2);

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    if (!bVScroll)
        DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,
            pRect->bottom);
    else
        pRect->right += 1;

    if (!bHScroll)
        DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right - 1,
            pRect->bottom - 1);

    //Restore the old brush
    SelectObject(hdc, hOldPen);
}
#endif
static void ClearEdit(HWND hWnd, HDC hDC, PEDITDATA p, HBRUSH hBrush, int charHeight, 
          RECT* pRectClient, int nStartHeight)
{
    if (!p->bMultiLine || !p->bUnderline)
        nStartHeight--;
	pRectClient->top = nStartHeight; 
	FillRect(hDC, pRectClient, hBrush);
	DrawUnderLine(hWnd, hDC, p, pRectClient);
}

static void Edit_Size(PEDITDATA p, HWND hWnd, LPARAM lParam)
{
    PSTR pTmp;
    
    p->wWindowWidth = LOWORD(lParam);  
    p->wWindowHeight = HIWORD(lParam);
    
//    if ((p->wState & ES_CENTER) || (p->wState & ES_RIGHT))
  //      p->wWindowWidth = p->wLineChars * size.cx + 2;
#if (INTERFACE_MONO)

    p->wWindowWidth -= CX_LEFTMARGIN + CX_RIGHTMARGIN;//Leave a space pixel on each side.

    if (p->bMultiLine)
        p->wWindowHeight -= CY_TOPMARGIN + CY_BTMMARGIN;

    if (p->wState & ES_TITLE)
    {
        p->wWindowWidth -= CX_TITLE_LEFTMARGIN + CX_TITLE_RIGHTMARGIN;
        p->wWindowHeight -= CY_TITLE_TOPMARGIN + CY_TITLE_BTMMARGIN;
    }

#else // INTERFACE_MONO
    
    p->wWindowWidth -= 2;//Leave a space pixel on each side.
    p->wWindowHeight -= 2;
    
    if (GetDeviceCaps(NULL, BITSPIXEL) > 2 && !(p->wState & ES_COMBOEDIT))
    {
        p->wWindowWidth -= 2;
        p->wWindowHeight -= 2;
        
        if (!(p->wState & WS_HSCROLL))
            p->wWindowHeight -= 2;
        
        if (!(p->wState & WS_VSCROLL))
            p->wWindowWidth -= 2;
    }
    
    if (p->wWindowWidth < 0)
        p->wWindowWidth = 0;
    
    if (p->wWindowHeight < 0)
        p->wWindowHeight = 0;

#endif // INTERFACE_MONO
    
    
 
    if (p->wWindowWidth < p->widthoffont)
        p->wLineChars   = 1;
    else
        p->wLineChars   = (IWORD)(p->wWindowWidth / p->widthoffont);

    if (!p->bHScrollBar)
        p->wScrollLineChars = p->wLineChars;

    if (p->bMultiLine)
    {
        p->wWindowLines = p->wWindowHeight / (p->heightoffont + EDIT_SPACE + 1);

        if (p->wWindowLines <= 0)
            p->wWindowLines = 1;
		
		if (!p->bVScrollBar && ((p->wMaxEditLength < p->wWindowLines * p->wScrollLineChars
			&& !p->bLimitText) ||
			(p->wMaxEditLength > p->wWindowLines * p->wScrollLineChars)))
		{
			p->wMaxEditLength = p->wWindowLines *  p->wScrollLineChars;
		}
		
		if ((!p->bVScrollBar) && (p->wMaxLength != p->wWindowLines * p->wScrollLineChars))
		{
			p->wMaxLength = p->wWindowLines * p->wScrollLineChars;
			pTmp = (char *)LocalAlloc(LMEM_FIXED, p->wMaxLength + 2);
			memset(pTmp, 0, p->wMaxLength + 2);

			if (p->pEditText)
			{
				memcpy(pTmp, p->pEditText, min(p->wLength, p->wMaxLength));
				LocalFree((HANDLE)p->pEditText);
			}
			p->pEditText = pTmp;
            if (p->wLength > p->wMaxLength)
			    SendMessage(hWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)p->pEditText);
			return;
		}
    }
	else 
	{
		if ((p->wMaxEditLength < p->wScrollLineChars && !p->bLimitText)
			|| (p->wMaxEditLength > p->wScrollLineChars))
		{
			p->wMaxEditLength = p->wScrollLineChars;
		}
		
		if (p->wMaxLength != p->wScrollLineChars)
		{
			p->wMaxLength = p->wScrollLineChars;
			pTmp = (char *)LocalAlloc(LMEM_FIXED, p->wMaxLength + 2);
			memset(pTmp, 0, p->wMaxLength + 2);

			if (p->pEditText)
			{
				memcpy(pTmp, p->pEditText, min(p->wLength, p->wMaxLength));
				LocalFree((HANDLE)p->pEditText);
			}
			p->pEditText = pTmp;
            if (p->wLength > p->wMaxLength)
			    SendMessage(hWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)p->pEditText);
			return;
		}
	}
	if (p->wLength >= 0)
	{
          AdjustDisplayWindow(hWnd, p);//This function also set p->wCursorCol.
	}
    InvalidateRect(hWnd, NULL, TRUE);
}

static void Edit_GetChar(PEDITDATA p, HWND hWnd, WORD wCode, LPARAM lParam)
{
    int         len = 0;
    BYTE        code[5];

    //如果是控制字符，只接受回车符。
    if (p->wState & ES_GSM)
    {
        if ( p->bReadOnly || p->bPenDown)
            return;
    }
    else
    {
        if ( p->bReadOnly || p->bPenDown || ((wCode < 0x20 )
            && (wCode != '\r') && (p->CharSet != CHARSET_1252)))
            return;
    }

    if (wCode == VK_BACK)
        return;
    
    if (p->wState & ES_NUMBER)
    {
        if ((wCode != '\r') && (wCode < 0x30 || wCode > 0x39)) 
            return;
    }

	if (p->CharSet == CHARSET_GB18030)
	{
		if (wCode == 0x0A0D)
		{
			code[0] = 0x0D;
			code[1] = 0x0A;
			code[2] = 0;
			len = 2;
		}
		else if (!IsGB18030LeadChar(wCode >> 8))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = 0;
			len = 1;
		}
		else if (IsGB18030Code2(wCode & 0xff))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = (BYTE)(wCode >> 8);
			code[2] = 0;
			len = 2;
		}
		else if (IsGB18030Code4(wCode & 0xff))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = (BYTE)(wCode >> 8);
			code[2] = (BYTE)(wCode >> 16);
			code[3] = (BYTE)(wCode >> 24);
			code[4] = 0;
			len = 4;
		}
		else
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = 0;
			len = 1;
		}
		
	}
	else if ((p->CharSet == CHARSET_ASCII) || (p->CharSet == CHARSET_1252))
	{
        if (wCode == 0x0A0D)
        {
            code[0] = 0x0D;
            code[1] = 0x0A;
            code[2] = '\0';
            len = 2;
        }
        else
        {
            code[0] = (BYTE)(wCode & 0xff);
            code[1] = 0;
            len = 1;
        }
	}
	else if (p->CharSet == CHARSET_GB2312)
	{
        if (wCode == 0x0A0D)
		{
			code[0] = 0x0D;
			code[1] = 0x0A;
			code[2] = 0;
			len = 2;
		}
        else if (wCode == '\r')
        {
            code[0] = (BYTE)(wCode & 0xff);
            code[1] = (BYTE)('\n' & 0xff);
            code[2] = 0;
            len = 2;
        }
		else if (IsGB2312Code((wCode >> 8), (wCode & 0xff)))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = (BYTE)(wCode >> 8);
			code[2] = 0;
			len = 2;
		}
		else
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = 0;
			len = 1;
		}
	}
	else if (p->CharSet == CHARSET_GBK)
	{
		if (IsGBKCode((wCode >> 8), (wCode & 0xff)))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = (BYTE)(wCode >> 8);
			code[2] = 0;
			len = 2;
		}
		else
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = 0;
			len = 1;
		}
	}
	else if (p->CharSet == CHARSET_BIG5)
	{
		if (IsBIG5Code((wCode >> 8), (wCode & 0xff)))
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = (BYTE)(wCode >> 8);
			code[2] = 0;
			len = 2;
		}
		else
		{
			code[0] = (BYTE)(wCode & 0xff);
			code[1] = 0;
			len = 1;
		}
	}
	
    /*if (wCode & 0xff00)//是汉字
    {
        code[0] = (BYTE)(wCode & 0xff);
        code[1] = (BYTE)(wCode >> 8);
        code[2] = 0;
        len = 2;
    }
    else if (wCode == '\r')
    {
        code[0] = (BYTE)(wCode & 0xff);
        code[1] = (BYTE)('\n' & 0xff);
        code[2] = 0;
        len = 2;
    }
    else//是英文
    {
        code[0] = (BYTE)(wCode & 0xff);
        code[1] = 0;
        len = 1;
    }*/
    
    if ((p->wState & ES_PASSWORD) && !(p->wState & ES_NUMBER))
    {
        p->bToMask = TRUE;
    }

    if (p->wState & ES_GSM)
    {
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wLength == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
    }
    else
    {
        // If the character to send is a visible character, the message
        // will be sent. The condition of a visible character is: In
        // character set of CHARSET_1252, all characters except NULL and
        // '\n' are visible; Otherwise, the characters '\r' and the
        // characters equal to SPACE and greater than it are visible.
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0
            && ((p->CharSet != CHARSET_1252 && (wCode > 0x20
            || wCode == '\r' || wCode == ' '))
            || (p->CharSet == CHARSET_1252 && (wCode > 0 && wCode != '\n')))
            && p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wLength == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
    }

    DisplayNewChars(hWnd, p, code, len);
}

static void DisplayNewChars(HWND hWnd, PEDITDATA p, BYTE * ptr, int len)
{
    int         nRetVal, linelen, oldlinelen;
    IWORD       col;
    DWORD       t;  
    HDC         hDC;
    BOOL        haveEnter = FALSE;
    int         i;
    BYTE        *pch = ptr;
    RECT        rc;
    
    if (pch)
    {
        for (i = 0; i < len; i++, pch++)
        {
            if (*pch == '\r')
            {
                haveEnter = TRUE;
                break;
            }
        }
    }

    if (p->bToMask)
    {
        if (KillTimer(hWnd, IDT_TOMASK))
        {
            InvalidateRect(hWnd, NULL, TRUE);
        }
        SetTimer(hWnd, IDT_TOMASK, ET_TOMASK, EditTimerProc);
        p->wCharToMask = p->wCursorPos;
    }

    t = GetXYByPos(p, p->wPosStart);
    col = LOWORD(t);

    hDC = GetDC(hWnd);
	oldlinelen = ComputeLineChars(p, hDC, p->wPosStart - col);//为输入前的个数。
	oldlinelen -= p->wReturnNum;    
	
    nRetVal = ReplaceSelect(hWnd, p, ptr, len);
    if (nRetVal == 1)
	{
		ReleaseDC(hWnd, hDC);
        return;
	}
    AdjustDisplayWindow(hWnd, p);
	//用于WM_PASTE和EM_REPLACE消息粘贴字符超过窗口可显示的范围时不显示
	if (p->wLineCount > p->wWindowLines && !p->bVScrollBar)
	{
		SendMessage(hWnd, EM_SETSEL, p->wPosStart, p->wCursorPos);
		SendMessage(hWnd, WM_CLEAR, 0, 0);
		ReleaseDC(hWnd, hDC);
		return;
	}
    SendMessage(p->hParentWnd, WM_COMMAND, 
        MAKELONG(p->wID, EN_UPDATE), (LPARAM)hWnd);

    //关掉软件光标
    EraseCaret(hWnd, p);
    
    t = GetXYByPos(p, p->wPosStart);
    col = LOWORD(t);
    linelen = ComputeLineChars(p, hDC, p->wPosStart - col);//为当前输入后的个数。
    
	linelen -= p->wReturnNum;
    
    if (ptr == NULL)
        len = 0;

    //缓冲区应后移的字符个数。
    len -= p->wPosEnd - p->wPosStart;

    SetCtlColorEdit(p, hWnd, hDC);

//    if (p->bRedraw)
//        PaintText(p, hWnd, hDC, 0, p->wWindowStart, p->wLength);
//    //If it isn't ES_LEFT mode, must redraw the whole line.
//    
//    else if (!p->bHScrollBar && p->bMultiLine && 
//        (p->wState & ES_CENTER || p->wState & ES_RIGHT))
//    {
//        if ((oldlinelen + len > oldlinelen) && (oldlinelen + len <= p->wLineChars) && 
//            ptr && !haveEnter && !p->bReplace)
//        {
//            PaintText(p, hWnd, hDC, 0, (WORD)(p->wPosStart - col), 
//                (WORD)(p->wPosStart - col + linelen));
//        }
//        else
//        {
//            PaintText(p, hWnd, hDC, 0, (WORD)(p->wPosStart - col), 
//                p->wLength);
//        }
//    }
//    else
//    {
//        if ((oldlinelen + len > oldlinelen) && (oldlinelen + len <= p->wLineChars) && 
//            ptr && !haveEnter && !p->bReplace)//! 
//        {
//            PaintText(p, hWnd, hDC, 0, (WORD)(p->wPosStart- col), // 
//                (WORD)(p->wPosStart - col + linelen));//(WORD)
//        }
//        else
//            PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wLength);
//    }
    GetEditRect(p, hWnd, &rc);
    InvalidateRect(hWnd, (const RECT*)&rc, TRUE);

    //p->wCursorPos在ReplaceSelect时，已置为输入字符后的位置，wPosStart and wPosEnd还没有。
    p->wPosEnd = p->wPosStart = p->wCursorPos;

    DisplayCaret(hWnd, p, hDC);
    
    ReleaseDC(hWnd, hDC);
    p->bModify = 1;
    p->bReplace = 0;
    SendMessage(p->hParentWnd, WM_COMMAND, 
        MAKELONG(p->wID, EN_CHANGE), (LPARAM)hWnd);
}

static void Edit_SetSel(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam)
{
    int         oldstart, wStart, wEnd, wWindowStart, nOffset;
    DWORD       t;  
    HDC         hDC;

    p->wPosEndOld = p->wPosEnd;
    oldstart = p->wPosStart;
    
    p->wPosStart = wStart = (int)wParam;//starting position 
    p->wPosEnd = wEnd = (int)lParam;//ending position 
    
    //According to MS Windows
    if (wEnd < 0 || wEnd > p->wLength)
        p->wPosEnd = p->wLength;
    
    //According to MS Windows
    if (wStart < 0 || wStart > p->wLength)
        p->wPosStart = p->wLength;
    
    if (p->wPosEnd < p->wPosStart)
    {
        SwapPos(&p->wPosStart, &p->wPosEnd);    
    }
    //If the nStart parameter is 0 and the nEnd parameter is -1, 
    //all the text in the edit control is selected. 
    if ((wStart == 0) && (wEnd == -1))
    {
        p->wPosStart = wStart;
        p->wPosEnd = p->wLength;
    }
    
    //If nStart is -1, any current selection is removed. 
    if (wStart == -1)
    {
        p->wPosStart = p->wPosEnd;
    }
    
    //The caret is placed at the end of the selection indicated by 
    //the greater of the two values nEnd and nStart.
    //p->wCursorPos = p->wPosEnd;
    p->wCursorPos = p->wPosStart;
    if (p->wPosStart > p->wWindowStart)
        wWindowStart = p->wWindowStart;
    else
        wWindowStart = 0;

    nOffset = InHalfDBCSChar(p, p->pEditText + wWindowStart, 
        p->wPosStart - wWindowStart);
    p->wPosStart -= nOffset;

    if (p->wCursorPos > p->wWindowStart)
        wWindowStart = p->wWindowStart;
    else
        wWindowStart = 0;

    nOffset = InHalfDBCSChar(p, p->pEditText + wWindowStart, 
        p->wCursorPos - wWindowStart);
    p->wCursorPos -= nOffset;
    p->wPosEnd -= nOffset;
    
    if ((p->wPosStart == oldstart) && (p->wPosEndOld == p->wPosEnd))
        return;
    
    t = GetXYByPos(p, p->wCursorPos);
    p->wCursorLine = HIWORD(t);
    p->wCursorCol = LOWORD(t);
    
    hDC = GetDC(hWnd);
    wWindowStart = p->wWindowStart;
    AdjustDisplayWindow(hWnd, p);
    
    if (!IsWindowVisible(hWnd))
    {
        if (wStart == wEnd)
            p->bSelectOn = 0;
        else
            p->bSelectOn = 1;
        ReleaseDC(hWnd, hDC);
        return;
    }
    
    SetCtlColorEdit(p, hWnd, hDC);
    if (p->wWindowStart != wWindowStart)
    {
        if (wStart == wEnd)
            p->bSelectOn = 0;
        else
            p->bSelectOn = 1;
        EDIT_Paint(hWnd, p, hDC);
        ReleaseDC(hWnd, hDC);
        if (p->bReadOnly)
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
        if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
                MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
                MASKRIGHT | MASKLEFT);
        }
        else if (p->bFocus)
        {
            SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
            if (p->wCursorPos == p->wLength)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKRIGHT | MASKLEFT);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
        }
        // add up&down arrow
        if (p->bMultiLine && p->bIsSingle)
        {
            if (p->wCursorLine == 0 && p->wLineCount == 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLDOWN, MASKUP | MASKDOWN);
			else if (p->wCursorLine == 0 && p->wLineCount == 0)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                NULL, MASKUP | MASKDOWN);
            else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLDOWN, MASKUP | MASKDOWN);
            else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLUP, MASKUP | MASKDOWN);
            else
                SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
        }
        return;
    }

    EraseCaret(hWnd, p);
    SetSoftCursorPos(p, hWnd, hDC);
    
    //According MS Windows, paint with GetDC.
    if (p->bSelectOn)
    {
        //old selection and new selection don't intersect.
        if (p->wPosEnd < oldstart || p->wPosEndOld < p->wPosStart)
        {
            PaintText(p, hWnd, hDC, 0, (WORD)oldstart, p->wPosEndOld);
            PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
        }
        //old selection include new selection completely.
        else if (p->wPosEnd <= p->wPosEndOld && p->wPosStart >= oldstart)
        {
//            PaintText(p, hWnd, hDC, 0, (WORD)oldstart, p->wPosStart);
//            PaintText(p, hWnd, hDC, 0, p->wPosEnd, p->wPosEndOld);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        //new selection include old selection completely.
        else if (p->wPosEnd >= p->wPosEndOld && p->wPosStart <= oldstart)
        {
            PaintText(p, hWnd, hDC, 1, p->wPosStart, (WORD)oldstart);
            PaintText(p, hWnd, hDC, 1, p->wPosEndOld, p->wPosEnd);
        }
        //old selection and new selection intersect.
        else if (p->wPosEnd >= p->wPosEndOld && p->wPosStart >= oldstart)
        {
            PaintText(p, hWnd, hDC, 0, (WORD)oldstart, p->wPosStart);
            PaintText(p, hWnd, hDC, 1, p->wPosEndOld, p->wPosEnd);
        }
        //old selection and new selection intersect.
        else if (p->wPosEnd <= p->wPosEndOld && p->wPosStart <= oldstart)
        {
            PaintText(p, hWnd, hDC, 1, p->wPosStart, (WORD)oldstart);
            PaintText(p, hWnd, hDC, 0, p->wPosEnd, p->wPosEndOld);
        }
    }
    else
        PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
    
    DisplayCaret(hWnd, p, hDC);
    ReleaseDC(hWnd, hDC);
    
    if (wStart == wEnd)
        p->bSelectOn = 0;
    else
        p->bSelectOn = 1;

    if (p->bReadOnly)
        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
    if (p->hParentWnd && p->wPosEnd == 0 && p->wPosStart == 0 && p->bFocus)
    {
        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)EXIT_TEXT);
        if (p->wCursorPos == p->wLength)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, NULL, 
            MASKRIGHT | MASKLEFT);
        else
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, 
            MASKRIGHT | MASKLEFT);
    }
    else if (p->bFocus)
    {
        SendMessage(p->hParentWnd, WM_SETRBTNTEXT, NULL, (LPARAM)CLEAR_TEXT);
        if (p->wCursorPos == p->wLength)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLLEFT, MASKRIGHT | MASKLEFT);
        else
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLRIGHT | SCROLLLEFT, MASKRIGHT | MASKLEFT);
    }
    // add up&down arrow
    if (p->bMultiLine && p->bIsSingle)
    {
        if (p->wCursorLine == 0 && p->wLineCount == 1)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            SCROLLDOWN, MASKUP | MASKDOWN);
		else if (p->wCursorLine == 0 && p->wLineCount == 0)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            NULL, MASKUP | MASKDOWN);
        else if ((p->wCursorLine+ p->wOffsetLine) == 0 && p->wLineCount > 1)
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            SCROLLDOWN, MASKUP | MASKDOWN);
        else if ((p->wCursorLine + p->wOffsetLine)== (p->wLineCount - 1))
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            SCROLLUP, MASKUP | MASKDOWN);
        else
            SendMessage(p->hParentWnd, PWM_SETSCROLLSTATE,
            SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN);
    }
}

static BOOL Edit_LineScroll(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam)
{
    int         cyScroll, cxScroll;
    int         i;//, oldstart;
    BOOL        lReturn = FALSE;

    cxScroll = wParam;//characters to scroll horizontally 
    cyScroll = lParam;//lines to scroll vertically

    if (p->bMultiLine)
    {
        if (cyScroll < 0) 
        {
            cyScroll = - cyScroll;
            for (i = 0; i < cyScroll; i++)
                DoVScroll(p, hWnd, SB_LINEUP, 0);//SB_LINEUP
        }
        else
        {
            for (i = 0; i < cyScroll; i++)
                DoVScroll(p, hWnd, SB_LINEDOWN, 0);//SB_LINEUP
        } 
        if (IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
 
        if (cxScroll < 0) 
        {
            cxScroll = - cxScroll;
            for (i = 0; i < cxScroll; i++)
                DoHScroll(p, hWnd, SB_LINELEFT, 0);//SB_LINEUP
        }
        else
        {
            for (i = 0; i < cxScroll; i++)
                DoHScroll(p, hWnd, SB_LINERIGHT, 0);//SB_LINEUP
        } 
        lReturn = TRUE;
    }
    return lReturn;
}

static void Edit_PenMove(HWND hWnd, PEDITDATA p, LPARAM lParam)
{
    RECT        rcClient;
    int         line, x, y, pos, nSize = 0;
    IWORD       col;
    DWORD       t;  
    HDC         hDC;
    SIZE        size;
    SCROLLINFO  ScrollInfo;
    BOOL        bXisNegative = FALSE;
    int         oldstart;
    IWORD       oldx;

    if (!p->bPenDown)
        return;
    
    x = LOWORD(lParam);
    y = HIWORD(lParam);
    
    ConvertXY(p, &x, &y);
    
    GetEditRect(p, hWnd, &rcClient);
    
    oldx = (IWORD)x;
    
	if (x & 0x8000)
    {
        x = 0;
        bXisNegative = 1;
    }
    
    if (((x < p->wSingleLineXPos) && 
        (p->wSingleLineMove == MOVE_RIGHT)) ||
        ((x > p->wSingleLineXPos) && 
        (p->wSingleLineMove == MOVE_LEFT)))
    {
        if (p->uTimerID)
        {
            KillSystemTimer(hWnd, p->uTimerID);
            p->uTimerID = 0;
            p->wSingleLineMove = MOVE_NO;
            return;
        }
    }
    
    p->wSingleLineXPos = x;
    
    if (!p->bMultiLine && x > p->wWindowWidth)//单行编辑框水平向右滚动
    {
        if (!p->uTimerID)
        {
            p->uTimerID = SetSystemTimer(hWnd, EDIT_TIMER_EVENT_ID, 200, NULL);
        }
        p->wSingleLineMove = MOVE_RIGHT;
        return;
    }
    
    if (!p->bMultiLine && bXisNegative)//单行编辑框水平向左滚动
    {
        if (!p->uTimerID)
        {
            p->uTimerID = SetSystemTimer(hWnd, EDIT_TIMER_EVENT_ID, 200, NULL);
        }
        p->wSingleLineMove = MOVE_LEFT;
        return;
    }
    hDC = GetDC(hWnd);

    
    if (y & 0x8000)
        line = 0;
    else
        line = y / (p->heightoffont + EDIT_SPACE + 1);

    if (p->bMultiLine && line >= p->wWindowLines)
		line = p->wWindowLines - 1;

    if ((p->wState & ES_RIGHT)/* && (p->wLength <= p->wLineChars)*/)//col number when right alignment.
    {
		int pos1, len;
        
        pos1 = GetPosByXY(p, line, 0);
        len = ComputeLineChars(p, hDC, pos1);
		len -= p->wReturnNum;
        GetTextExtentExPoint(hDC, p->pEditText + pos1, len, 
			p->wWindowWidth - x, &nSize, NULL, NULL);
		col = nSize;
		//col = (IWORD)((p->wWindowWidth - x) / size.cx);
        
        //if exceed half char width.
        //if ((((p->wWindowWidth - x) % size.cx) > size.cx / 2) && (col < p->wLineChars))
          //  col ++;
    }
    else if ((p->wState & ES_CENTER)/* && (p->wLength <= p->wLineChars)*/)//!p->bHScrollBar && (//For center align.
    {
        int pos1, len;
        
        pos1 = GetPosByXY(p, line, 0);
        len = ComputeLineChars(p, hDC, pos1);
        
        len -= p->wReturnNum;
        
		GetTextExtentPoint32(hDC, p->pEditText + pos1, len, &size);
		
		if (x < (p->wWindowWidth - size.cx) / 2)
			col = 0;
		else
		{
			GetTextExtentExPoint(hDC, p->pEditText + pos1, len, 
				x - (p->wWindowWidth - size.cx) / 2, &nSize, NULL, NULL);
			col = nSize;
		}
        
        //if exceed half char width.
        //if ((((x - (p->wWindowWidth - len * size.cx) / 2) % size.cx) > size.cx / 2) 
          //  && (col < p->wLineChars))
            //col ++;
    }
    else
    {
		int pos1;
		
		pos1 = GetPosByXY(p, line, 0);   
		GetTextExtentExPoint(hDC, p->pEditText + pos1, -1, x,
			&nSize, NULL, NULL);
		col = nSize;
        
    }
    
    if (p->bHScrollBar)
    {
        col += p->wHScrollChars;
        if (col < 0)
            col = 0;
    }
    
    if (p->bMultiLine && (y & 0x8000))
    {
        oldstart = p->wWindowStart;
        
        //由于在WM_VSCROLL处,不做SetScrollInfo,所以此处用SendMessage,而没用
        //PostMessage,以获得p->wWindowStart,从而SetScrollInfo.
        DoVScroll(p, hWnd, SB_LINEUP, 0);
        
        if (oldstart != p->wWindowStart)
            p->bScrollScreen = 1;
    }
    
    if (!(y & 0x8000) && p->bMultiLine) 
    {
        int line1;
        
        GetTextExtentPoint32(hDC, "T", 1, &size);
        line1 = (y + 5) / (size.cy + EDIT_SPACE + 1);
        pos = GetPosByXY(p, line1, 0);

        if (line1 >= p->wWindowLines && pos <= p->wLength)
        {
            oldstart = p->wWindowStart;
            DoVScroll(p, hWnd, SB_LINEDOWN, 0);
            
            if (oldstart != p->wWindowStart)
                p->bScrollScreen = 1;
        }
    }

	if (line >= p->wWindowLines)
        line = p->wWindowLines - 1;
    
    if ((p->wState & ES_RIGHT) && (p->wLength <= p->wLineChars))//For right align.
    {
        int len, pos1;
        
        pos1 = GetPosByXY(p, line, 0);
        len = ComputeLineChars(p, hDC, pos1);
        
		len -= p->wReturnNum;
        
        if (col > len)
            col = 0;
        else
            col = len - col;
    }
    
    pos = GetPosByXY(p, line, col);

    if (col != 0)
        col = LOWORD(GetXYByPos(p, pos));
    
    //Left or Right move limited
    if (p->bHScrollBar && col - p->wHScrollChars > p->wLineChars)
    {
        if (col - p->wHScrollChars - p->wLineChars > p->wLineChars / 3)
            col = p->wHScrollChars + p->wLineChars + p->wLineChars / 3;
    }
    else if (p->bHScrollBar && col < p->wHScrollChars)
    {
        if (p->wHScrollChars - col > p->wLineChars / 3)
            col = p->wHScrollChars - p->wLineChars / 3;
    }
    
    if (p->bHScrollBar)
    {
        IWORD newpos = p->wHScrollChars;
        int pos1, oldLine;
        
        oldLine = HIWORD(GetXYByPos(p, p->wCursorPos));
        pos1 = GetPosByXY(p, line, 0);
        while(col >= 0 && pos == pos1 && pos == p->wLength && line != 0 
			&& *(p->pEditText + pos - 1) != '\n' 
			&& *(p->pEditText + pos - 1) != '\r')
        {
            //get a valid position
            line --;
            pos = GetPosByXY(p, line, col);
            pos1 = GetPosByXY(p, line, 0);
        }
        
        if (oldLine != line)//Move up or down
        {
            int oldpos = p->wHScrollChars;
            while (pos - pos1 <= newpos)
            {
                newpos = GetHScrollChars(p, (IWORD)(-p->wLineChars));
                if (newpos == p->wHScrollChars)
                    break;
                p->wHScrollChars = newpos;
            }
            p->wHScrollChars = oldpos;
        }
        else//move in one line
        {
            int offset = 0;
            //move right
            if (pos - pos1 - p->wHScrollChars + p->wLineChars / 3 > p->wLineChars)
            {
                if (x > p->wWindowWidth)
                    offset = pos - pos1 - p->wHScrollChars - p->wLineChars + p->wLineChars / 3;
            }
            //move left
            if (pos - pos1 - p->wLineChars / 3 < p->wHScrollChars)
            {
                if (bXisNegative)
                    offset = pos - pos1 - p->wHScrollChars - p->wLineChars / 3;
            }
            newpos = GetHScrollChars(p, (IWORD)offset);
        }
        
        if (p->wHScrollChars != newpos)
        {
            p->wHScrollChars = newpos;
            p->bScrollScreen = 1;
            
            if (p->wState & WS_HSCROLL)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS | SIF_DISABLENOSCROLL;
                ScrollInfo.nPos = p->wHScrollChars;
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            }
        }
    }
    
    p->wPosEndOld = p->wPosEnd;
    p->wPosEnd = pos;
    
    p->bSelectOn = 1;
    

    if (p->wCursorPos != pos || p->bScrollScreen)
    {
        HBRUSH hBrush;

        p->wCursorPos = pos;
        t = GetXYByPos(p, p->wCursorPos);
        p->wCursorLine = HIWORD(t);//必须重新算一下，使光标不显示在没字的地方。
        p->wCursorCol = LOWORD(t);
        
		if (p->bMultiLine && p->wCursorLine < line && pos != p->wLength &&
            p->wCursorCol == p->wScrollLineChars )//For right align.
        {
            p->wCursorLine ++;
            p->wCursorCol = 0;
        }
        
        hBrush = SetCtlColorEdit(p, hWnd, hDC);
        if (p->bScrollScreen)
        {
            if (p->bSelectOn)
            {
                ClearEdit(hWnd, hDC, p, hBrush, p->heightoffont + EDIT_SPACE, &rcClient, rcClient.top);
            }
            EDIT_Paint(hWnd, p, hDC);
        }
        else
        {
            EraseCaret(hWnd, p);
            
            //选择区域在同方向减小。
            if ((p->wPosEnd >= p->wPosStart && p->wPosEndOld > p->wPosEnd) ||
                (p->wPosEnd <= p->wPosStart && p->wPosEndOld < p->wPosEnd))
            {
                PaintText(p, hWnd, hDC, 0, p->wPosEnd, p->wPosEndOld);
            }
            //选择区域在同方向扩大。
            else if ((p->wPosEnd > p->wPosStart && p->wPosEndOld >= p->wPosStart) ||
                (p->wPosEnd < p->wPosStart && p->wPosEndOld <= p->wPosStart))
            {
                PaintText(p, hWnd, hDC, 1, p->wPosEnd, p->wPosEndOld);
            }
            //选择区域变为相反方向。
            else if ((p->wPosEnd >= p->wPosStart && p->wPosEndOld < p->wPosEnd) ||
                (p->wPosEnd <= p->wPosStart && p->wPosEndOld > p->wPosEnd))
            {
                PaintText(p, hWnd, hDC, 0, p->wPosStart, p->wPosEndOld);
                PaintText(p, hWnd, hDC, 1, p->wPosStart, p->wPosEnd);
            }
            DisplayCaret(hWnd, p, hDC);
        }
    }
    ReleaseDC(hWnd, hDC);
}

static int SearchIndex(PEDITDATA p)
{
	int nStart = 0, nEnd = p->wIndexCount, nMid, nTemp;

	if ((*(p->pLineIndex) <= p->wCursorPos) &&
		(*(p->pLineIndex + 1) > p->wCursorPos))
	{
		return nStart;
	}

	if (*(p->pLineIndex + nEnd) <= p->wCursorPos)
		return nEnd;
	
	for (; ;)
	{
		if (nEnd < nStart)
			return -1;

		nTemp = (nEnd - nStart) / 2;
		if (nTemp > 0)
			nMid = nStart + nTemp;
		else
			nMid = nStart + 1;

		if ((*(p->pLineIndex + nMid) <= p->wCursorPos) &&
			(*(p->pLineIndex + nMid + 1) > p->wCursorPos))
		{
			if (!p->bMoveCursorUp)
				nMid --;

			break;
		}
		
		if (*(p->pLineIndex + nMid) < p->wCursorPos)
			nStart = nMid;
		else
			nEnd = nMid;
	}
	return nMid;
}

/**********************************************************************
 * Function     DrawHighlight
 * Purpose      
 * Params       hWnd, hdc, pRect
 * Return       
 * Remarks      
 **********************************************************************/

static void DrawHighlight(HWND hWnd, HDC hdc, PEDITDATA pEditData,
                          PRECT prcEdit, PRECT prcPaint)
{
    BITMAP bmp;

    if ((pEditData->bFocus) && ((pEditData->wState & ES_TITLE)
        || !pEditData->bMultiLine))
    {
        GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
        StretchBlt(hdc, prcEdit->left, prcEdit->top, prcEdit->right
            - prcEdit->left, prcEdit->bottom - prcEdit->top, (HDC)hbmpFocus,
            0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
    }
    else
    {
        ClearRect(hdc, prcPaint, COLOR_TRANSBK);
    }
}

/**********************************************************************
 * Function     DrawTitle
 * Purpose      
 * Params       hWnd, hdc, pEditData
 * Return       
 * Remarks      
 **********************************************************************/

static void DrawTitle(HWND hWnd, HDC hdc, PEDITDATA pEditData)
{
    int   nLength = 0, nMode = 0;
    RECT  rcTitle;
    COLORREF cr;
    HFONT hFont = NULL, hfontOld = NULL;

    if (pEditData->wState & ES_TITLE)
    {
        if (pEditData->pszTitle == NULL)
            return;

        nLength = strlen(pEditData->pszTitle);

        GetTitleRect(hWnd, &rcTitle);

        GetFontHandle(&hFont, SMALL_FONT);
        hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);

        nMode = SetBkMode(hdc, TRANSPARENT);
        if (pEditData->bFocus)
            cr = SetTextColor(hdc, COLOR_WHITE);

        DrawText(hdc, pEditData->pszTitle, nLength, &rcTitle,
            DT_LEFT | DT_VCENTER);

        if (pEditData->bFocus)
            SetTextColor(hdc, cr);
        
        SetBkMode(hdc, nMode);
        SelectObject(hdc, (HGDIOBJ)hfontOld);
    }
}

/**********************************************************************
 * Function     Edit_SetTitle
 * Purpose      
 * Params       hWnd, p, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void Edit_SetTitle(HWND hWnd, PEDITDATA p, WPARAM wParam, LPARAM lParam)
{
    int   nNewLen = 0;
    PSTR  pszNewTitle = NULL;
    RECT  rcTitle;

    if (!(p->wState & ES_TITLE))
        return;

    pszNewTitle = (PSTR)lParam;
    ASSERT(pszNewTitle != NULL);

    nNewLen = strlen(pszNewTitle);

    if (p->pszTitle == NULL)
    {
        p->pszTitle = LocalAlloc(LMEM_FIXED, nNewLen + 1);
    }
    else if (p->wTitleLen < nNewLen)
    {
        p->pszTitle = LocalRealloc(p->pszTitle, nNewLen + 1);
    }

    strcpy((char*)p->pszTitle, (const char*)pszNewTitle);
    p->wTitleLen = nNewLen;

    GetTitleRect(hWnd, &rcTitle);
    InvalidateRect(hWnd, &rcTitle, TRUE);
}

/**********************************************************************
 * Function     GetTitleRect
 * Purpose      
 * Params       hWnd, pRect
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL GetTitleRect(HWND hWnd, PRECT pRect)
{
    RECT rcClient;
    HDC hdc = NULL;
    HFONT hFont = NULL, hfontOld = NULL;
    SIZE size;

    GetClientRect(hWnd, &rcClient);

    hdc = GetDC(hWnd);
    GetFontHandle(&hFont, SMALL_FONT);
    hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    SelectObject(hdc, (HGDIOBJ)hfontOld);
    ReleaseDC(hWnd, hdc);
    
    pRect->left   = rcClient.left + 4;
    pRect->top    = rcClient.top + 4;
    pRect->right  = rcClient.right - 3;
    pRect->bottom = pRect->top + size.cy;

    return TRUE;
}

/**********************************************************************
 * Function     GetPasswordExtent
 * Purpose      
 * Params       hdc, nLen, pSize
 * Return       
 * Remarks      
 **********************************************************************/

static int GetPasswordExtent(HDC hdc, PEDITDATA p, int nLen, PSIZE pSize)
{
    SIZE sizeCaret;

    GetTextExtentPoint32(hdc, (PCSTR)&p->nPasswordChar, 1, pSize);

    if (p->bToMask)
    {
        if (nLen == 0)
        {
            pSize->cx = 0;
        }
        else
        {
            pSize->cx *= nLen - 1;
            GetTextExtentPoint32(hdc, p->pEditText + p->wCursorPos - 1, 1, &sizeCaret);
            pSize->cx += sizeCaret.cx;
        }
    }
    else
    {
        pSize->cx *= nLen;
    }

    return pSize->cx;
}

int ReMemAlloc(PEDITDATA p, int nCount)
{
    WORD* pTmp;
    pTmp = (WORD *)LocalAlloc(LMEM_FIXED, (nCount + p->wLineCountOrg/2) * sizeof(WORD));
    if (!pTmp)
        return 0;
    //memset(pTmp, 0, (nCount + p->wLineCountOrg/2) * sizeof(WORD));
	memcpy(pTmp, p->pLineIndex, nCount * sizeof(WORD));
	LocalFree((HANDLE)p->pLineIndex);
    p->pLineIndex = pTmp;
    return nCount + p->wLineCountOrg/2;
}

/**********************************************************************
 * Function     EditTimerProc
 * Purpose      
 * Params       hWnd, uMsg, idEvent, dwTime
 * Return       
 * Remarks      
 **********************************************************************/

static void CALLBACK EditTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                   DWORD dwTime)
{
    PEDITDATA p = NULL;

    p = (PEDITDATA)GetUserData(hWnd);
    if (p == NULL)
        return;

    KillTimer(hWnd, IDT_TOMASK);
    p->bToMask = FALSE;
    InvalidateRect(hWnd, NULL, TRUE);
}
