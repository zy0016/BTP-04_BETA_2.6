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

#ifndef __WINPDA_H
#define __WINPDA_H

#ifndef __WINDOW_H
#include "window.h"
#endif

// LOWORD(wParam) : ID of the listbox
// HIWORD(wParam) : unused
// LOWORD(lPARAM) : width of the list box
// HIWORD(lParam) : unused
#define PWM_CREATECAPTIONLISTBOX    0x0381      // For Legend860, Radiant

// wParam : 0 - hide, 1 - show
// lParam : unused. 
#define PWM_SHOWCAPTIONLISTBOX      0x0382      // For Legend860 only

// LOWORD(wParam) : ID of the listbox
// HIWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Specifies the button text
#define PWM_CREATECAPTIONBUTTON     0x0383      // For Radiant only

// lParam : handle of app icon bitmap
// return the old app icon bitmap
#define PWM_SETAPPICON              0x0384      // For Radiant only

#define PWM_SETSCROLLSTATE          0x0385      // For Mobile Phone
#define PWM_GETSCROLLSTATE          0x0389      // For Mobile Phone

// HIWORD(wParam) : unused
// LOWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Specifies new button text
#define PWM_SETBUTTONTEXT           0x0386      // For Mobile Phone, changed the text on 
                                                // the button
// HIWORD(wParam) : unused
// LOWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Pointer to the buffer that will receive the text on the button.
#define PWM_GETBUTTONTEXT           0x0387      // For Mobile Phone, get the text on 
                                                // the button

//wParam           : CAPTIONCOLOR
//lParam           : STATICBARCOLOR
#define PWM_SETDEFWINDOWCOLOR       0x0388      // For Mobile Phone, modify the color of
												// the default window's caption.

#define PWM_LBSELCHANGE			0x038A

// wParam : unused
// lParam : pointer to the buffer that will receive the rect of current submenu
#define PWM_GETCURRSUBMENURECT 0x38B

#define PWM_SETSIGNAL          0x38C

// Msg:    WM_IMESWITCHED
// wParam: Image type IMAGE_BITMAP or IMAGE_ICON
// lParam: Path of icon file or handle to bitmap
#define WM_IMESWITCHED         0x038E

#define PWM_SHOWWINDOW          0x0390
#define PWM_CLOSEWINDOW         0x0391
#define PWM_ACTIVATE            0x0392
#define PWM_CLOSEWINDOWEX       0x0393

#define PWS_STATICBAR               0x0001     //For Mobile Phone, Specifies showing the
                                               // staticbar or not.
#define PWS_NOTSHOWPI               0x0002
// Set Scroll State
#define SCROLLUP        0x0001
#define SCROLLDOWN      0x0002
#define SCROLLLEFT      0x0004
#define SCROLLRIGHT     0x0008
#define SCROLLMIDDLE    0x0010

#define MASKUP          0x0001
#define MASKDOWN        0x0002
#define MASKLEFT        0x0004
#define MASKRIGHT       0x0008
#define MASKMIDDLE      0x0010
#define MASKALL         0x001F

#define CAP_COLOR       0x0001
#define BAR_COLOR       0x0002
#define BTN_COLOR       0X0003
#define FOCUS_COLOR     0x0004
#define NORMAL_COLOR    0x0005



#define NORBMPFILE  "ROM:ss_1btn.bmp"
#define FOCBMPFILE  "ROM:ss_fbtn.bmp"
#define PUSBMPFILE  "ROM:ss_3btn.bmp"

/**************************************************************************/
/*              Default PDA main window proc                              */
/**************************************************************************/
LRESULT WINAPI PDADefWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                LPARAM lParam);

/**************************************************************************/
/*              Menu Functions                                            */
/**************************************************************************/

#if (!NOMENUS)
#ifndef NOMENUAPI

HMENU   WINAPI PDAGetMenu(HWND hWnd);
BOOL    WINAPI PDASetMenu(HWND hWnd, HMENU hMenu);
BOOL    WINAPI PDADrawMenuBar(HWND hWnd);
BOOL    WINAPI GetSubMenuRect(HMENU hMenu, RECT *pRect);
BOOL    WINAPI GetMainMenuRect(HMENU hMenu, RECT *pRect);
#endif // NOMENUAPI
#endif // NOMENUS

COLORREF GetWindowColor(int index);
void SetWindowColor(int index, COLORREF color);
void SetCaptionBmp(PCSTR pFileName);
void SetBottomBmp(PCSTR pFileName);

/**************************************************************************/
/*              Scroll bar support                                        */
/**************************************************************************/

#if (!NOSCROLL)

int     WINAPI PDASetScrollInfo(HWND hWnd, int nBar, 
                                PCSCROLLINFO pScrollInfo, BOOL bRedraw);
BOOL    WINAPI PDAGetScrollInfo(HWND hWnd, int nBar, 
                                PSCROLLINFO pScrollInfo);
BOOL    WINAPI PDAEnableScrollBar(HWND hWnd, int nSBFlags, int nArrow);

#endif // NOSCROLL

HWND    WINAPI  CreateFrameWindow(DWORD dwstyle);

/**************************************************************************/
/*              color Scheme support                                        */
/**************************************************************************/
#define MAXFILENAME		64//32
typedef struct _COLORSCHEME
{
	char		CapBmpName[MAXFILENAME];			//background bitmap in caption;
	COLORREF	CapColor1;							//color of the first line in caption
	COLORREF	CapColor2;							//color of the second line in caption
	COLORREF	CapColor3;							//color of the third line in caption
	COLORREF	CapColor4;							//color of the fourth line in caption
    char        BarBtnDownBmpName[MAXFILENAME];     // button's pushdown bitmap in staticbar
    char		BarBtnUpBmpName[MAXFILENAME];	    // button's normao bitmap in staticbar
    char        BarMenuDownBmpName[MAXFILENAME];    // menu's pushdown bitmap in staticbar
    char		BarMenuUpBmpName[MAXFILENAME];		//menu's normal bitmap in staticbar
    char        BarBmpName[MAXFILENAME];           // bracground bitmap in staticbar
    char        BarCopsBmpName[MAXFILENAME];       // 
	COLORREF	WinBkColor;							//color ofwindow's background  
	char		SoftKeyBmp[MAXFILENAME];			//softkey bitmap
	char		SofyKeyDownBmp[MAXFILENAME];		//softkey pushdown bitmap
	char		UpIconName[MAXFILENAME];			//up arrow
	char		DownIconName[MAXFILENAME];			//down arrow
	char		LeftIconName[MAXFILENAME];			//left arrow
	char		RightIconName[MAXFILENAME];			//right arrow
	char		MiddleIconName[MAXFILENAME];		//middle arrow	
	COLORREF	BorderColor;						//color of border(normao)
	COLORREF	FocusBorColor1;						//color of border(focus)
	COLORREF	FocusBorColor2;						//color of border(focus)
	COLORREF	FocusBorColor3;						//color of border(focus)
	COLORREF	RealFocusColor;						//color of Real focus
	COLORREF	FocusColor;							//color of focus
	char		SpinLeftArrow[MAXFILENAME];			//left arrow in strspin or numspin
	char		SpinRightArrow[MAXFILENAME];		//right arrow in strspin or numspin
	char		SpinDisLeftArrow[MAXFILENAME];		//disabled left arrow in numspin
	char		SpinDisRightArrow[MAXFILENAME];		//disabled right arrow in numspin
	char		ComboDownArrow[MAXFILENAME];		//down arrow in combobox
	char		CheckboxChecked[MAXFILENAME];		//checked bitmap in checkbox
	char		CheckboxUnchecked[MAXFILENAME];		//unchecked bitmap in checkbox
	char		RadioChecked[MAXFILENAME];			//checked bitmap in radiobutton
	char		RadioUnchecked[MAXFILENAME];		//unchecked bitmap in radiobutton
	char		NormalBmp[MAXFILENAME];				//normal bitmap in button
	char		FocusBmp[MAXFILENAME];				//focus bitmap in button
	char		PenDownBmp[MAXFILENAME];			//pushdown bitmap in button
	char		DisableBmp[MAXFILENAME];			//disabled bitmap in button
	char		ScrollUp[MAXFILENAME];				//up arrow in scroll bar
	char		ScrollDown[MAXFILENAME];			//down arrow in scroll bar
	char		ScrollLeft[MAXFILENAME];			//left arrow in scroll bar
	char		ScrollRight[MAXFILENAME];			//right arrow in scroll bar
	char		ScrollDisUp[MAXFILENAME];			//disabled up arrow in scroll bar
	char		ScrollDisDown[MAXFILENAME];			//disabled down arrow in scroll bar
	char		ScrollDisLeft[MAXFILENAME];			//disabled left arrow in scroll bar
	char		ScrollDisRight[MAXFILENAME];		//disabled right arrow in scroll bar
	COLORREF	ScrollBorderColor;					//color of border in scroll bar
    COLORREF    ScrollBgColor;                      //color of background in scroll bar
	COLORREF	ScrollThumbColor;					//color of thumb in scroll bar
	COLORREF	ScrollThuBorColor1;					//color of thumb's border in scroll bar
	COLORREF	ScrollThuBorColor2;					//color of thumb's border in scroll bar
	COLORREF	MenuBorderColor;					//color of border in menu
	COLORREF	MenuBKColor;						//color of background in menu
	COLORREF	MenuFocusColor;						//hight light color in menu
}COLORSCHEME, *PCOLORSCHEME;
BOOL			WINAPI	SetColorScheme(PCOLORSCHEME pColScheme);
PCOLORSCHEME	WINAPI  GetColorScheme(void);

#define LB_SECONDIMAGE  0x0010
#define LBN_POSOFCLICK  8
#define MENU_SUBPOP     0x8000
#define MENU_MAINPOP    0x4000

//#define LBN_CLICKED     7        

#define CLICKINFIRICON	1
#define CLICKINSECICON	2
#define CLICKINTEXT		3

#define LEFTICON		0
#define RIGHTICON		1
#define CALLICON        2
#define ENDCALLICON     3
#define SIGNALICON      4
#define POWERICON       5

#endif // __WINPDA_H
