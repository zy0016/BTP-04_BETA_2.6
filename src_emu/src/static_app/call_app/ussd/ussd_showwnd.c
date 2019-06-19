#include     "window.h"
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "stdlib.h"
#include    "mullang.h"
#include    "pubapp.h"
#include    "MBPublic.h"
#include   "MB_control.h"

#define     IDC_OK                  5
#define     IDC_REPLY               6
#define     IDC_CANCEL              7

#define     IDS_OK                  ML("Ok")
#define     IDS_REPLY               ML("Reply")
#define     IDS_CANCEL              ML("Cancel")
#define     CAP_NETWORKSERVICE      ML("Network service")
#define     MAX_LEN     256

#define ISNEWLINE(p)  \
		(*p == '\r' || *p == '\n' )

#define ISENTERNEWLINE(p) \
        (*p == '\r' && *(p+1) && *(p+1) == '\n')


typedef struct tagUSSDView_Layout
{
    char* pszFragment;
    int   nLen;
    struct tagUSSDView_Layout *pNext;
}USSDVIEW_LAYOUT, *PUSSDVIEW_LAYOUT;


static HWND  hNotifyWnd;
static UINT  wNotifyMsg;
static HINSTANCE        hInstance;
static HWND             hUSSDWnd;
static HWND             hFocus;
static PVOID            pLayouthead;
static const char * pClassName      = "USSDWndClass";
static char ussdshow[MAX_LEN];
static BOOL IsR;
static RECT rc[5];
static LRESULT USSDAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static BOOL USSDView_Layout(HWND hWnd, char * psz, int szlen);
static void USSDView_InitVScroll(HWND hWnd);
static int USSDView_GetLine(HWND hWnd);
static void USSDView_OnPaint(HWND hWnd);
static void USSDView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static HWND hussdview;

BOOL ShowUSSDInfo(HWND hPWnd, USSD_INFO * ussd_info, BOOL IsResponse)
{
    WNDCLASS    wc;
	RECT        rClient;
	USSD_INFO   uinfo;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = USSDAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	if (ussd_info!= NULL)
	{
		memcpy(&uinfo, ussd_info, sizeof(USSD_INFO));
		memset(ussdshow,0x00,MAX_LEN);		
		if (CBS_ALPHA_UCS2 == uinfo.Code)
		{
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)uinfo.Data,
				uinfo.DataLen/2, (LPSTR)ussdshow, MAX_LEN, 0, 0);			
		}
		else
		{
			memcpy(ussdshow, uinfo.Data,uinfo.DataLen);
		}		
	}
	else
		return FALSE;
	IsR = IsResponse;
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hUSSDWnd = CreateWindow(pClassName, CAP_NETWORKSERVICE,
			PWS_STATICBAR|WS_CAPTION,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hUSSDWnd = CreateWindow(pClassName, CAP_NETWORKSERVICE,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hUSSDWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
//	hFocus = hUSSDWnd;
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hUSSDWnd);
    ShowWindow(hUSSDWnd, SW_SHOW);
    UpdateWindow(hUSSDWnd); 
//	SetFocus(hUSSDWnd);
    return TRUE;
}

static LRESULT USSDAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int nWidth, nHeigth, nY, i;
			i = 0;
			
			GetClientRect(hWnd, &rClient);
			if (IsR)
			{
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANCEL);
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_REPLY, 1), (LPARAM)IDS_REPLY);
			}
			else
			{
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 0), (LPARAM)IDS_OK);
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			}
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);
			GetClientRect(hWnd,&rClient);
			
			nWidth = rClient.right - rClient.left;			
			nHeigth = (rClient.bottom - rClient.top) / 5;			
			nY = 0;			
			for(i = 0 ; i < 5 ; i++)
			{
				SetRect(&rc[i],0,nY,nWidth,nY+nHeigth);
				nY += nHeigth;
			}
			if (strlen(ussdshow)!= 0) 
			{
				hussdview = PlxTextView(NULL, hWnd,ussdshow, strlen(ussdshow), FALSE, NULL, NULL, 0);
				hFocus = hussdview;
/*
				USSDView_Layout(hWnd, ussdshow, strlen(ussdshow));
			    USSDView_InitVScroll(hWnd);
*/
			}
			else
				hFocus = hWnd;
		}
        break;

		
	case  WM_PAINT:
		USSDView_OnPaint(hWnd);
		break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
//			if (hFocus == hUSSDWnd)
//				ShowWindow(hFocus, SW_SHOW);
//			else
				SetFocus(hFocus);
        }
		else
			hFocus = GetFocus();
        break;		

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;
		
    case WM_DESTROY :
		{			
//			USSDVIEW_LAYOUT *pLayout,*pTmp;
			DestroyWindow(hussdview);
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hUSSDWnd);
/*
			pLayout = pLayouthead;			
			while(pLayout)
			{
				pTmp = pLayout->pNext;
				free(pLayout);
				pLayout = pTmp;
			}	*/
		
			hUSSDWnd = NULL;
			hFocus = NULL;
			UnregisterClass(pClassName,NULL);
		}
        break;

/*
    case WM_KEYDOWN:
        USSDView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
*/

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OK:
			PostMessage(hNotifyWnd, wNotifyMsg, 0, 0);
			PostMessage(hWnd, WM_CLOSE, 0,0);
			break;
		case IDC_REPLY:
			PostMessage(hNotifyWnd, wNotifyMsg, 1, 0);
//			PostMessage(hWnd, WM_CLOSE, 0,0);
			break;
		case IDC_CANCEL:
			PostMessage(hNotifyWnd, wNotifyMsg, 0, 0);
			PostMessage(hWnd, WM_CLOSE, 0,0);
			break;
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}


/*********************************************************************\
* Function     SMSView_Layout
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL USSDView_Layout(HWND hWnd, char * psz, int szlen)
{
    char *pTxt = NULL,*p = NULL,*pString = NULL;
    int  len = 0,nFit = 0,nClientWidth = 0,nClientHeight = 0,i = 0;
    HDC  hdc;
    RECT rcClient;
    USSDVIEW_LAYOUT *pLayout,*pTmpLayout;
    BOOL bBreak = FALSE;
    HFONT hFontBig = NULL,hFontOld = NULL;
    
    len = szlen;
    pTxt = ussdshow;

    if (len <= 0 || pTxt == NULL)
        return FALSE;

#if 0
    pTxt = "1234567890 \r\nI am a good person,if there is no bad wether ,did you?\r1\n2\r\r3\n\n4\r\n5\n\r6abcdefghijklmnopqrstuvwsyz12345678900987654321 error\r\n";
#endif

    GetClientRect(hWnd,&rcClient);
    nClientHeight = rcClient.bottom - rcClient.top;
    nClientWidth = rcClient.right - rcClient.left;
    p = pTxt;

    pLayout = (USSDVIEW_LAYOUT*)malloc(sizeof(USSDVIEW_LAYOUT));
    if(pLayout == NULL)// need to free memory
        return FALSE;
    memset(pLayout,0,sizeof(USSDVIEW_LAYOUT));
	pLayouthead = pLayout;
    pLayout->pszFragment = pTxt;
    
    while(*p)
    {
        if(ISENTERNEWLINE(p))
        {
            pTmpLayout = pLayout;
            if(pTmpLayout->nLen == 0)
                pTmpLayout->pszFragment = NULL;

            pTmpLayout->pNext = (USSDVIEW_LAYOUT*)malloc(sizeof(USSDVIEW_LAYOUT));
            if(pTmpLayout->pNext == NULL)// need to free memory
                return FALSE;
            memset(pTmpLayout->pNext,0,sizeof(USSDVIEW_LAYOUT));
            pLayout = pTmpLayout->pNext;
            pLayout->pszFragment = p+2;
            p += 2;
        }
        else if(ISNEWLINE(p))
        {
            pTmpLayout = pLayout;
            if(pTmpLayout->nLen == 0)
                pTmpLayout->pszFragment = NULL;

            pTmpLayout->pNext = (USSDVIEW_LAYOUT*)malloc(sizeof(USSDVIEW_LAYOUT));
            if(pTmpLayout->pNext == NULL)// need to free memory
                return FALSE;
            memset(pTmpLayout->pNext,0,sizeof(USSDVIEW_LAYOUT));
            pLayout = pTmpLayout->pNext;
            pLayout->pszFragment = p+1;
            p++;
        }
        else 
        {
            pLayout->nLen++;
            p++;
        }
    }


    
    pLayout = pLayouthead;
    hdc = GetDC(hWnd);
    
    GetFontHandle(&hFontBig,1);
    hFontOld = SelectObject(hdc,hFontBig);

    while(pLayout != NULL)
    {
        pString = pLayout->pszFragment;
        len = pLayout->nLen;
        while(len > 0)
        {
            GetTextExtentExPoint(hdc, pString, len, nClientWidth, &nFit, NULL, NULL);
            if(nFit >= len)
                break;
            
            pTmpLayout = (USSDVIEW_LAYOUT*)malloc(sizeof(USSDVIEW_LAYOUT));
            if(pTmpLayout == NULL)// need to free memory
                return FALSE;
            memset(pTmpLayout,0,sizeof(USSDVIEW_LAYOUT));

            
            for(i=nFit-1 ; i >0 ; i--)
            {
                bBreak = FALSE;
                if(pString[i] == ' ')
                {
                    len -= (i+1);
                    pString += (i+1);
                    
                    pLayout->nLen = (i+1);
                    pTmpLayout->pNext = pLayout->pNext; 
                    pLayout->pNext = pTmpLayout; 
                    pTmpLayout->pszFragment = pString;
                    pTmpLayout->nLen = len;
                    pLayout = pTmpLayout;
                    bBreak = TRUE;
                    break;
                }
            }
            if(bBreak == FALSE)
            {
                len -= nFit;
                pString += nFit;
                pLayout->nLen = nFit;
                pTmpLayout->pNext = pLayout->pNext; 
                pLayout->pNext = pTmpLayout; 
                pTmpLayout->pszFragment = pString;
                pTmpLayout->nLen = len;
                pLayout = pTmpLayout;
                continue;
            }
        }        
        pLayout = pLayout->pNext;
    }
    
    SelectObject(hdc,hFontOld);
    ReleaseDC(hWnd,hdc);

    return TRUE;
}
/*********************************************************************\
* Function     USSDView_GetLine
* Purpose      
* Params      
* Return       
**********************************************************************/
static int USSDView_GetLine(HWND hWnd)
{
    USSDVIEW_LAYOUT *pLayout;
    int nLine = 0;

    pLayout = pLayouthead;

    while(pLayout != NULL)
    {
        nLine++;
        pLayout = pLayout->pNext;
    }
    return nLine;
}
/*********************************************************************\
* Function	   USSDView_InitVScroll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void USSDView_InitVScroll(HWND hWnd)
{
    SCROLLINFO      vsi;
    
    memset(&vsi, 0, sizeof(SCROLLINFO));
   
    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 5;
    vsi.nMax   = USSDView_GetLine(hWnd)-1;
    vsi.nPos   = 0;
    
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

	return;
}

/*********************************************************************\
* Function	USSDView_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void USSDView_OnPaint(HWND hWnd)
{
    USSDVIEW_LAYOUT *pLayout;
    int  nLine,i;
    SCROLLINFO vsi;
	int OldBK;
    HFONT hFontBig = NULL,hFontOld = NULL;
	HDC hdc = BeginPaint(hWnd, NULL);
    
    GetFontHandle(&hFontBig,1);
    hFontOld = SelectObject(hdc,hFontBig);
    memset(&vsi, 0, sizeof(SCROLLINFO));
    vsi.fMask  = SIF_ALL ;
    GetScrollInfo(hWnd, SB_VERT, &vsi);
    pLayout = pLayouthead;

    nLine = 0;
    i = 0;
    while(pLayout)
    {
        if(vsi.nPos > nLine)
        {
            pLayout = pLayout->pNext;
            nLine++;
        }
        else if(vsi.nPos <= nLine && (int)(vsi.nPos+vsi.nPage) > nLine)
        {
            if(pLayout->nLen && pLayout->pszFragment)
			{
				OldBK = SetBkMode(hdc, BM_TRANSPARENT);
                DrawText(hdc,pLayout->pszFragment,pLayout->nLen,&rc[i],DT_VCENTER);
				SetBkMode(hdc, OldBK);
            }
            pLayout = pLayout->pNext;
            nLine++;
            i++;
        }
        else
            break;
    }

    SelectObject(hdc,hFontOld);
	EndPaint(hWnd, NULL);

	return;
}


/*********************************************************************\
* Function	USSDView_OnKey
* Purpose   WM_KEYDOWN message handler of the main window
* Params
*			hWnd: Handle of the window
*			vk:	Virtual key code
*			fDown: Is key donw
*			cRepeat: Key repeat rate
*			flags:	flag of key down
* Return	None
* Remarks
**********************************************************************/
static void USSDView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SCROLLINFO vsi;
    int nY;
    RECT rcClient;

	switch (vk)
	{
    case VK_UP:
        
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
        
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/5;
        
        if(vsi.nPos > 0)
        {
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            UpdateWindow(hWnd);
            break;
        }
        break;

    case VK_DOWN:
        
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
        
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/5;
        
        if((int)(vsi.nPos+vsi.nPage) <= vsi.nMax )
        {
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            UpdateWindow(hWnd);
            break;
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

void SetUSSDNotify(HWND hWnd, UINT wMsg)
{
	hNotifyWnd = hWnd;
	wNotifyMsg = wMsg;

}

void CloseUSSDShowWnd()
{
	PostMessage(hUSSDWnd, WM_CLOSE, 0,0);
}
