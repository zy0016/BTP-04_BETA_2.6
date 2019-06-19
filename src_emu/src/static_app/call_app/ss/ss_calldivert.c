 #include   "winpda.h"
#include  "plx_pdaex.h"
#include  "mullang.h"
#include  "MBPublic.h"
#include  "prebrowhead.h"
#include   "MB_control.h"

#define     IDC_OK                  5
#define     IDS_OK                  ML("Ok")
#define     CAP_CALLDIVERT1         "Divert active"
#define     IDS_LAB_TYPE            ML("Type:")
#define     IDS_LAB_SERVICE         ML("Service:")
#define     IDS_LAB_NUMBER          ML("Number:")
#define     IDS_LAB_DELAY           ML("Delay:")
#define     CAP_NETWORKSERVICE      ML("Network service")

static HINSTANCE        hInstance;
static HWND             hCFWnd;
static HWND             hCFFormView;
static HWND             hFocus;
static int              iBsCounter;
static int              idxBS;
static const char * pClassName      = "CFWndClass";
static const char * pLeft = "/rom/public/arrow_left.ico";
static const char * pRight = "/rom/public/arrow_right.ico";

static CFINFO * pcfinfo = NULL;
static LRESULT  CFAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
BOOL ShowCFInfo(HWND hPWnd, CFINFO * pcf, int len)
{
    WNDCLASS    wc;
	RECT        rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CFAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	if (pcf != NULL)
	{
		pcfinfo = malloc(len);
		if (pcfinfo == NULL)
			return FALSE;
		memcpy(pcfinfo, pcf, len);
		iBsCounter = len /sizeof(CFINFO);
	}
	else
		return FALSE;
	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hCFWnd = CreateWindow(pClassName, CAP_CALLDIVERT1,
			PWS_STATICBAR, 
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hCFWnd = CreateWindow(pClassName, CAP_CALLDIVERT1,
			PWS_STATICBAR|WS_CAPTION|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}
    if (NULL == hCFWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCFWnd);
    ShowWindow(hCFWnd, SW_SHOW);
    UpdateWindow(hCFWnd); 
/*
	if (hCFFormView != NULL)
		SetFocus(hCFFormView);
	else
		SetFocus(hCFWnd);
*/
    return TRUE;
}

static LRESULT CFAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
//    static HANDLE hLeftBmp = NULL;
//	static HANDLE hRightBmp = NULL;
	static SIZE lsize,rsize;
    static int iPage = 0;//当前页
    static int iCountPage;//总页数

    HDC hdc;
    LRESULT     lResult;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			int i;
			GetClientRect(hWnd, &rClient);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			PDASetMenu(hWnd, NULL);

            iPage = 0;
            iCountPage = 0;
            for (i = 0;i < iBsCounter;i++)
            {
                if (pcfinfo[i].Active)
                    iCountPage++;
            }
//            if (iCountPage > 1)
//            {
//                GetImageDimensionFromFile(pLeft, &lsize);
//                GetImageDimensionFromFile(pRight, &rsize);
//                if ( hLeftBmp == NULL )
//                    hLeftBmp = LoadImage(NULL, pLeft, IMAGE_BITMAP,lsize.cx,lsize.cy, LR_LOADFROMFILE);
//                if ( hRightBmp == NULL )		
//                    hRightBmp = LoadImage(NULL, pRight, IMAGE_BITMAP,rsize.cx,rsize.cy, LR_LOADFROMFILE);
//            }

			idxBS = 0;
			i= idxBS;
			while ((!pcfinfo[i].Active) && ( i < iBsCounter)) 
			{
				i++;
				if ( (i == iBsCounter -1) &&(!pcfinfo[i].Active))
				{
					//PostMessage(hWnd, WM_CLOSE, 0, 0);
					//break;
				}
			}
			if (i == iBsCounter)
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Divert not active"), CAP_NETWORKSERVICE, Notify_Info, ML("Ok"), NULL, -1);
				return -1;
			}
			else
			{
                int j = 0;
                PREBROW_InitListBuf (&Listtmp);
				while (j < iBsCounter)
                {
                    idxBS = j++;		
                    if (!pcfinfo[idxBS].Active)
                        continue;
                    switch (pcfinfo[idxBS].Reason)
                    {
                    case CFU:
                        PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All calls"));
                        break;
                    case CFB:
                        PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("If busy"));
                        break;
                    case CFNRy:
                        PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if unanswered"));				
                        break;
                    case CFNRc:
                        PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if not reachable"));
                        break;
                    }
                    switch (pcfinfo[idxBS].BS)
                    {
                    case BSVOICE:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Voice"));
                        break;
                    case BSDATA:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("All data"));
                        break;
                    case BSFAX:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX"));
                        break;
                    case BSSMS:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("SMS"));
                        break;
                    case BSDCS:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circuit sync"));
                        break;
                    case BSDCA:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circurt async"));
                        break;
                    case BSDPAC:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated packted access"));
                        break;
                    case BSDPAD:
                        PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated PAD access"));
                        break;
                    }
                    if (pcfinfo[idxBS].DN != NULL)
                        PREBROW_AddData(&Listtmp, IDS_LAB_NUMBER, pcfinfo[idxBS].DN);
                    if ((pcfinfo[idxBS].Reason == CFNRy) && (pcfinfo[idxBS].Time> 0))
                        switch (pcfinfo[idxBS].Time)
                    {
                    case 5:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("5 senconds"));
                        break;
                    case 10:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("10 seconds"));
                        break;
                    case 15:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("15 seconds"));
                        break;
                    case 20:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("20 seconds"));
                        break;
                    case 25:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("25 seconds"));
                        break;
                    case 30:
                        PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("30 seconds"));
                        break;
                    }
                    break;
                }
				FormView_Register();
				hCFFormView = CreateWindow(WC_FORMVIEW,
					NULL,
					WS_VISIBLE  |WS_CHILD |WS_VSCROLL,
					rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
					hWnd, NULL, NULL, (PVOID)&Listtmp);
				if (hCFFormView == NULL)
					return FALSE;
				hFocus = hCFFormView;
			}
		}
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
			SetFocus(hFocus);
        }
		else
        {
            PostMessage(hWnd, WM_CLOSE, 0,0);
            hFocus = GetFocus();
        }
        break;		

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;
		
    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCFWnd);
		if (pcfinfo != NULL)
		{
			free(pcfinfo);
			pcfinfo = NULL;
		}
//        if (hRightBmp) 
//		{
//			DeleteObject(hRightBmp);
//			hRightBmp = NULL;
//		}
//		if (hLeftBmp)
//		{
//			DeleteObject(hLeftBmp);
//			hLeftBmp = NULL;
//		}
		hCFFormView = NULL;
        hCFWnd = NULL;
		hFocus = NULL;
        UnregisterClass(pClassName,NULL);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        if ( iPage == 0 )
        {
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)hRightBmp);
            SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
            if (iCountPage == 1)
                SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
            else
                SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)pRight);
        }
        else if (iPage == iCountPage - 1)
        {
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hLeftBmp);
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
            if (iCountPage == 1)
                SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
            else
                SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)pLeft);

            SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
        }
        else
        {
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hLeftBmp);
            //SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)hRightBmp);
            SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)pLeft);
            SendMessage(hWnd,PWM_SETAPPICON,MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)pRight);
        }
        EndPaint(hWnd, NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RIGHT:
			{
				LISTBUF	Listtmp;
				RECT rClient;
				int i;

                if ((idxBS < 0) || (idxBS >= iBsCounter - 1))
                    break;

				i= idxBS+1;
				if ( i == iBsCounter)
					break;
				while ((!pcfinfo[i].Active) && ( i < iBsCounter)) 
				{
					i++;
					if ( (i == iBsCounter -1) &&(!pcfinfo[i].Active)) 
						break;
				}
                if (!pcfinfo[i].Active)
                    break;
                if (i == iBsCounter)
                    break;

				idxBS = i;
                iPage++;
				GetClientRect(hWnd, &rClient);
				DestroyWindow(hCFFormView);

				PREBROW_InitListBuf (&Listtmp);
				switch (pcfinfo[idxBS].Reason)
				{
				case CFU:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All calls"));
					break;
				case CFB:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("If busy"));
					break;
				case CFNRy:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if unanswered"));				
					break;
				case CFNRc:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if not reachable"));
					break;
				}
				switch (pcfinfo[idxBS].BS)
				{
				case BSVOICE:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Voice"));
					break;
				case BSDATA:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("All data"));
					break;
				case BSFAX:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX"));
					break;
				case BSSMS:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("SMS"));
					break;
				case BSDCS:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circuit sync"));
					break;
				case BSDCA:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circurt async"));
					break;
				case BSDPAC:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated packted access"));
					break;
				case BSDPAD:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated PAD access"));
					break;
				}
				if (pcfinfo[idxBS].DN != NULL)
					PREBROW_AddData(&Listtmp, IDS_LAB_NUMBER, pcfinfo[idxBS].DN);
				if ((pcfinfo[idxBS].Reason == CFNRy) && (pcfinfo[idxBS].Time> 0))
				{
					switch (pcfinfo[idxBS].Time)
					{
					case 5:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("5 senconds"));
						break;
					case 10:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("10 seconds"));
						break;
					case 15:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("15 seconds"));
						break;
					case 20:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("20 seconds"));
						break;
					case 25:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("25 seconds"));
						break;
					case 30:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("30 seconds"));
						break;
					}
				}
				FormView_Register();
				hCFFormView = CreateWindow(WC_FORMVIEW,
					NULL,
					WS_VISIBLE  |WS_CHILD|WS_VSCROLL,
					rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
					hWnd, NULL, NULL, (PVOID)&Listtmp);
				if (hCFFormView == NULL)
					return FALSE;
				UpdateWindow(hCFWnd); 
                hFocus = hCFFormView;
                SetFocus(hFocus);
			}
            break;
        case VK_LEFT:
			{
				RECT rClient;
				LISTBUF	Listtmp;
				int i;

                if ((idxBS <= 0) || (idxBS > iBsCounter))
                    break;

				i= idxBS-1;
				if ( i == iBsCounter)
					break;
				while ((!pcfinfo[i].Active) && ( i >= 0)) 
				{
					i--;
					if ( (i == 0) &&(!pcfinfo[i].Active)) 
						break;
				}
                if (!pcfinfo[i].Active)
                    break;
                if (i < 0)
                    break;
				idxBS = i;
                iPage--;
				GetClientRect(hWnd, &rClient);
				DestroyWindow(hCFFormView);				
				PREBROW_InitListBuf (&Listtmp);
				switch (pcfinfo[idxBS].Reason)
				{
				case CFU:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("All calls"));
					break;
				case CFB:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("If busy"));
					break;
				case CFNRy:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if unanswered"));				
					break;
				case CFNRc:
					PREBROW_AddData(&Listtmp, IDS_LAB_TYPE, ML("if not reachable"));
					break;
				}
				switch (pcfinfo[idxBS].BS)
				{
				case BSVOICE:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Voice"));
					break;
				case BSDATA:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("All data"));
					break;
				case BSFAX:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("FAX"));
					break;
				case BSSMS:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("SMS"));
					break;
				case BSDCS:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circuit sync"));
					break;
				case BSDCA:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Data circurt async"));
					break;
				case BSDPAC:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated packted access"));
					break;
				case BSDPAD:
					PREBROW_AddData(&Listtmp, IDS_LAB_SERVICE, ML("Dedicated PAD access"));
					break;
				}
				if (pcfinfo[idxBS].DN != NULL)
					PREBROW_AddData(&Listtmp, IDS_LAB_NUMBER, pcfinfo[idxBS].DN);
				if ((pcfinfo[idxBS].Reason == CFNRy) && (pcfinfo[idxBS].Time> 0))
				{
					switch (pcfinfo[idxBS].Time)
					{
					case 5:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("5 senconds"));
						break;
					case 10:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("10 seconds"));
						break;
					case 15:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("15 seconds"));
						break;
					case 20:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("20 seconds"));
						break;
					case 25:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("25 seconds"));
						break;
					case 30:
						PREBROW_AddData(&Listtmp, IDS_LAB_DELAY, ML("30 seconds"));
						break;
					}
				}
				FormView_Register();
				hCFFormView = CreateWindow(WC_FORMVIEW,
					NULL,
					WS_VISIBLE  |WS_CHILD|WS_VSCROLL,
					rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
					hWnd, NULL, NULL, (PVOID)&Listtmp);
				if (hCFFormView == NULL)
					return FALSE;
				UpdateWindow(hCFWnd); 
                hFocus = hCFFormView;
                SetFocus(hFocus);
			}
            break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OK:
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
