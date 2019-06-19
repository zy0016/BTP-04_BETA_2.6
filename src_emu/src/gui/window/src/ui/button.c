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
#include "hpfile.h"
#include "control.h"
#include "string.h"
#include "plx_pdaex.h"
#include "winpda.h"

#define STBBGCOLOR      RGB(251, 251, 210)
#define GROUPLEFTWIDTH	7

#define MINIHEIGHT		8
#define MINIWIDTH		8

#define TEXT_GAP        12
#define FOCUSRECTWIDTH  4
#define CX_ICON_LEFTMARGIN 5

#if (INTERFACE_MONO)

#define CHKBMPWIDTH		17  // Width of the checkbox bitmap
#define CHKBMPHEIGHT    16  // Height of the checkbox bitmap
#define RADIOBMPWIDTH   17  // Width of the radiobutton bitmap
#define RADIOBMPHEIGHT  16  // Height of the radiobutton bitmap

#define MAXBMPWIDTH     17  // Max width of checkbox and radio bitmap

// The data of unchecked checkbox bitmap
#if (COLUMN_BITMAP)
const static BYTE UncheckedBmpData[] = 
{
    0xFF, 0xF0, 0x00, 0x00,    // 11111111111100000000000000000000
    0xFF, 0xF0, 0x00, 0x00,    // 11111111111100000000000000000000
    0xC0, 0x00, 0x00, 0x00,    // 11000000000000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xC0, 0x00, 0x00, 0x00     // 11000000000000000000000000000000
};
#else
/*
const static BYTE UncheckedBmpData[] = 
{
    0xFF, 0xF0, 0x00, 0x00,    // 11111111111100000000000000000000
    0xFF, 0xF0, 0x00, 0x00,    // 11111111111100000000000000000000
    0x00, 0x30, 0x00, 0x00,    // 00000000001100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x00, 0x30, 0x00, 0x00     // 00000000001100000000000000000000
};
*/
const static BYTE UncheckedBmpData[] = 
{
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
    0xC0,0x30,0x00,0x00,      // 11000000001100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xC0,0x30,0x00,0x00,      // 11000000001100000000000000000000
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
};
#endif

// The data of checked checkbox bitmap
#if (COLUMN_BITMAP)
const static BYTE CheckedBmpData[] = 
{
    0x9F, 0xF0, 0x00, 0x00,    // 10011111111100000000000000000000
    0x8F, 0xF0, 0x00, 0x00,    // 10001111111100000000000000000000
    0xC4, 0x00, 0x00, 0x00,    // 11000100000000000000000000000000
    0xE3, 0xE0, 0x00, 0x00,    // 11100011111000000000000000000000
    0xD1, 0xE0, 0x00, 0x00,    // 11010001111000000000000000000000
    0xD8, 0xE0, 0x00, 0x00,    // 11011000111000000000000000000000
    0xDC, 0x60, 0x00, 0x00,    // 11011100011000000000000000000000
    0xDC, 0x60, 0x00, 0x00,    // 11011100011000000000000000000000
    0xD8, 0xE0, 0x00, 0x00,    // 11011000111000000000000000000000
    0xD9, 0xE0, 0x00, 0x00,    // 11011001111000000000000000000000
    0xDF, 0xE0, 0x00, 0x00,    // 11011111111000000000000000000000
    0xC0, 0x00, 0x00, 0x00     // 11000000000000000000000000000000
};
#else
/*
const static BYTE CheckedBmpData[] = 
{
    0xFF, 0xF0, 0x00, 0x00,    // 11111111111100000000000000000000
    0xFF, 0xC0, 0x00, 0x00,    // 11111111110000000000000000000000
    0x00, 0x80, 0x00, 0x00,    // 00000000100000000000000000000000
    0x7F, 0x10, 0x00, 0x00,    // 01111111000100000000000000000000
    0x7E, 0x30, 0x00, 0x00,    // 01111110001100000000000000000000
    0x4C, 0x70, 0x00, 0x00,    // 01001100011100000000000000000000
    0x40, 0xB0, 0x00, 0x00,    // 01000000101100000000000000000000
    0x61, 0xB0, 0x00, 0x00,    // 01100001101100000000000000000000
    0x73, 0xB0, 0x00, 0x00,    // 01110011101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x7F, 0xB0, 0x00, 0x00,    // 01111111101100000000000000000000
    0x00, 0x30, 0x00, 0x00     // 00000000001100000000000000000000
};*/
const static BYTE CheckedBmpData[] = 
{
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
    0xFF,0xE0,0x00,0x00,      // 11111111111000000000000000000000
    0xC0,0x00,0x00,0x00,      // 11000000000000000000000000000000
    0xDF,0x90,0x00,0x00,      // 11011111100100000000000000000000
    0xDF,0x30,0x00,0x00,      // 11011111001100000000000000000000
    0xD2,0x30,0x00,0x00,      // 11010010001100000000000000000000
    0xD8,0xB0,0x00,0x00,      // 11011000101100000000000000000000
    0xDD,0xB0,0x00,0x00,      // 11011101101100000000000000000000
    0xDF,0xB0,0x00,0x00,      // 11011111101100000000000000000000
    0xC0,0x30,0x00,0x00,      // 11000000001100000000000000000000
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
    0xFF,0xF0,0x00,0x00,      // 11111111111100000000000000000000
};
#endif

// The data of unchecked radio button bitmap
const static BYTE UncheckedRadioBmpData[] = 
{
    0xF1, 0xE0, 0x00, 0x00,    // 11110001111000000000000000000000
    0xCE, 0x60, 0x00, 0x00,    // 11001110011000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0x7F, 0xC0, 0x00, 0x00,    // 01111111110000000000000000000000
    0x7F, 0xC0, 0x00, 0x00,    // 01111111110000000000000000000000
    0x7F, 0xC0, 0x00, 0x00,    // 01111111110000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0xCE, 0x60, 0x00, 0x00,    // 11001110011000000000000000000000
    0xF1, 0xE0, 0x00, 0x00     // 11110001111000000000000000000000
};

// The data of checked radio button bitmap
const static BYTE CheckedRadioBmpData[] = 
{
    0xF1, 0xE0, 0x00, 0x00,    // 11110001111000000000000000000000
    0xCE, 0x60, 0x00, 0x00,    // 11001110011000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0xB1, 0xA0, 0x00, 0x00,    // 10110001101000000000000000000000
    0x60, 0xC0, 0x00, 0x00,    // 01100000110000000000000000000000
    0x60, 0xC0, 0x00, 0x00,    // 01100000110000000000000000000000
    0x60, 0xC0, 0x00, 0x00,    // 01100000110000000000000000000000
    0xB1, 0xA0, 0x00, 0x00,    // 10110001101000000000000000000000
    0xBF, 0xA0, 0x00, 0x00,    // 10111111101000000000000000000000
    0xCE, 0x60, 0x00, 0x00,    // 11001110011000000000000000000000
    0xF1, 0xE0, 0x00, 0x00     // 11110001111000000000000000000000
};

#else

#define CHKBMPHEIGHT    9
#define CHKBMPWIDTH     9

#define STBCHKBMPHEIGHT 12
#define STBCHKBMPWIDTH  12

#define RMONOBMPHEIGHT  13
#define RMONOBMPWIDTH   13
#define R3DBMPHEIGHT    12
#define R3DBMPWIDTH     12
#define RSTBBMPHEIGHT   16
#define RSTBBMPWIDTH    16
#define RSTBDOTHEIGHT   8
#define RSTBDOTWIDTH    8
#define RMONODOTHEIGHT  7
#define RMONODOTWIDTH   7
#define R3DDOTHEIGHT    4
#define R3DDOTWIDTH     4

#define MAXBMPWIDTH     16  // Max width of checkbox and radio bitmap

const static BYTE cCheckedBmpData[] = 
{
    0x7F,0x00,0x00,0x00,     // 01111111000000000000000000000000
    0xBE,0x80,0x00,0x00,     // 10111110100000000000000000000000
    0xDD,0x80,0x00,0x00,     // 11011101100000000000000000000000
    0xEB,0x80,0x00,0x00,     // 11101011100000000000000000000000
    0xF7,0x80,0x00,0x00,     // 11110111100000000000000000000000
    0xEB,0x80,0x00,0x00,     // 11101011100000000000000000000000
    0xDD,0x80,0x00,0x00,     // 11011101100000000000000000000000
    0xBE,0x80,0x00,0x00,     // 10111110100000000000000000000000
    0x7F,0x00,0x00,0x00      // 01111111000000000000000000000000
};

const static BYTE cSTBCheckedBmpData[] = 
{
    0xFF,0xF0,0x00,0x00,     // 11111111111100000000000000000000
    0xFF,0xF0,0x00,0x00,     // 11111111111100000000000000000000
    0xFF,0xB0,0x00,0x00,     // 11111111101100000000000000000000
    0xFF,0x30,0x00,0x00,     // 11111111001100000000000000000000
    0xFE,0x30,0x00,0x00,     // 11111110001100000000000000000000
    0xDC,0x70,0x00,0x00,     // 11011100011100000000000000000000
    0xC8,0xF0,0x00,0x00,     // 11001000111100000000000000000000
    0xC1,0xF0,0x00,0x00,     // 11000001111100000000000000000000
    0xE3,0xF0,0x00,0x00,     // 11100011111100000000000000000000
    0xF7,0xF0,0x00,0x00,     // 11110111111100000000000000000000
    0xFF,0xF0,0x00,0x00,     // 11111111111100000000000000000000
    0xFF,0xF0,0x00,0x00      // 11111111111100000000000000000000
};

const static BYTE rMonoRadioBmpData[] = 
{
    0xF8,0xF8,0x00,0x00,     // 11111000111110000000000000000000
    0xE7,0x38,0x00,0x00,     // 11100111001110000000000000000000
    0xDF,0xD8,0x00,0x00,     // 11011111110110000000000000000000
    0xBF,0xE8,0x00,0x00,     // 10111111111010000000000000000000
    0xBF,0xE8,0x00,0x00,     // 10111111111010000000000000000000
    0x7F,0xF0,0x00,0x00,     // 01111111111100000000000000000000
    0x7F,0xF0,0x00,0x00,     // 01111111111100000000000000000000
    0x7F,0xF0,0x00,0x00,     // 01111111111100000000000000000000
    0xBF,0xE8,0x00,0x00,     // 10111111111010000000000000000000
    0xBF,0xE8,0x00,0x00,     // 10111111111010000000000000000000
    0xDF,0xD8,0x00,0x00,     // 11011111110110000000000000000000
    0xE7,0x38,0x00,0x00,     // 11100111001110000000000000000000
    0xF8,0xF8,0x00,0x00      // 11111000111110000000000000000000
};

const static BYTE rSTBRadioBmpData[] = 
{
    0xFC,0x3F,0x00,0x00,     // 11111100001111110000000000000000
    0xF0,0x0F,0x00,0x00,     // 11110000000011110000000000000000
    0xE3,0xC7,0x00,0x00,     // 11100011110001110000000000000000
    0xCF,0xF3,0x00,0x00,     // 11001111111100110000000000000000
    0x9F,0xF9,0x00,0x00,     // 10011111111110010000000000000000
    0x9F,0xF9,0x00,0x00,     // 10011111111110010000000000000000
    0x3F,0xFC,0x00,0x00,     // 00111111111111000000000000000000
    0x3F,0xFC,0x00,0x00,     // 00111111111111000000000000000000
    0x3F,0xFC,0x00,0x00,     // 00111111111111000000000000000000
    0x3F,0xFC,0x00,0x00,     // 00111111111111000000000000000000
    0x9F,0xF9,0x00,0x00,     // 10011111111110010000000000000000
    0x9F,0xF9,0x00,0x00,     // 10011111111110010000000000000000
    0xCF,0xF3,0x00,0x00,     // 11001111111100110000000000000000
    0xE3,0xC7,0x00,0x00,     // 11100011110001110000000000000000
    0xF0,0x0F,0x00,0x00,     // 11110000000011110000000000000000
    0xFC,0x3F,0x00,0x00      // 11111100001111110000000000000000
};

const static BYTE rSTBXorBmpData[] = 
{
    0xF0,0x3C,0x00,0x00,     // 11110000001111000000000000000000
    0xC0,0x0C,0x00,0x00,     // 11000000000011000000000000000000
    0x87,0x84,0x00,0x00,     // 10000111100001000000000000000000
    0x9F,0xF4,0x00,0x00,     // 10011111111101000000000000000000
    0x1F,0xF0,0x00,0x00,     // 00011111111100000000000000000000
    0x3C,0xF8,0x00,0x00,     // 00111100111110000000000000000000
    0x38,0x78,0x00,0x00,     // 00111000011110000000000000000000
    0x38,0x78,0x00,0x00,     // 00111000011110000000000000000000
    0x3C,0xF8,0x00,0x00,     // 00111100111110000000000000000000
    0x3F,0xF0,0x00,0x00,     // 00111111111100000000000000000000
    0xBF,0xF4,0x00,0x00,     // 10111111111101000000000000000000
    0x9F,0xE4,0x00,0x00,     // 10011111111001000000000000000000
    0xC7,0x8C,0x00,0x00,     // 11000111100011000000000000000000
    0xF0,0x3C,0x00,0x00      // 11110000001111000000000000000000
};

const static BYTE rAndBmpData[] = 
{
    0xF0,0xF0,0x00,0x00,     // 11110000111100000000000000000000
    0xC0,0x30,0x00,0x00,     // 11000000001100000000000000000000
    0x80,0x10,0x00,0x00,     // 10000000000100000000000000000000
    0x80,0x10,0x00,0x00,     // 10000000000100000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x80,0x10,0x00,0x00,     // 10000000000100000000000000000000
    0x80,0x10,0x00,0x00,     // 10000000000100000000000000000000
    0xC0,0x30,0x00,0x00,     // 11000000001100000000000000000000
    0xF0,0xF0,0x00,0x00      // 11110000111100000000000000000000
};

const static BYTE rSTBAndBmpData[] = 
{
    0xF0,0x3C,0x00,0x00,     // 11110000001111000000000000000000
    0xC0,0x0C,0x00,0x00,     // 11000000000011000000000000000000
    0x80,0x04,0x00,0x00,     // 10000000000001000000000000000000
    0x80,0x04,0x00,0x00,     // 10000000000001000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x80,0x04,0x00,0x00,     // 10000000000001000000000000000000
    0x80,0x04,0x00,0x00,     // 10000000000001000000000000000000
    0xC0,0x0C,0x00,0x00,     // 11000000000011000000000000000000
    0xF0,0x3C,0x00,0x00      // 11110000001111000000000000000000
};

const static BYTE rSTBDotBmpData[] = 
{
    0xC3,0x00,0x00,0x00,     // 11000011000000000000000000000000
    0x81,0x00,0x00,0x00,     // 10000001000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x81,0x00,0x00,0x00,     // 10000001000000000000000000000000
    0xC3,0x00,0x00,0x00      // 11000011000000000000000000000000
};

const static BYTE rMonoDotBmpData[] = 
{
    0xC6,0x00,0x00,0x00,     // 11000110000000000000000000000000
    0x82,0x00,0x00,0x00,     // 10000010000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x82,0x00,0x00,0x00,     // 10000010000000000000000000000000
    0xC6,0x00,0x00,0x00      // 11000110000000000000000000000000
};

const static BYTE r3DDotBmpData[] = 
{
    0x90,0x00,0x00,0x00,     // 10010000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x00,0x00,0x00,0x00,     // 00000000000000000000000000000000
    0x90,0x00,0x00,0x00      // 10010000000000000000000000000000
};

#endif

typedef struct
{
    BYTE  bPenDown;			// if pen down
	BYTE  bState;			// if push button is highlight.
	BYTE  bFocus;
    BYTE  bLeaveWindow;
	BYTE  bPenMove;
	BYTE  byStyle;			// style
    BOOL  bLeftText;
	WORD  wCheckState;		// Current checked state.
    WORD  wID;
    HBITMAP hImageNormal;
    HBITMAP hImagePushDown;
    HBITMAP hImageFocus;
    HBITMAP hImageDisable;
} BUTTONDATA, *PBUTTONDATA;

// Internal function prototypes
static LRESULT CALLBACK BUTTON_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam);
static void PUSHBUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                             PSTR pszText, BOOL bDown, int nBitsPixel, 
                             RECT* pRect);
static void GROUP_Paint(HDC hdc, int nBitsPixel, PSTR pszText, RECT* pRect);
static void DrawGroupRect(HDC hdc, int nBitsPixel, int iTextWidth, RECT* pRect);
static void RADIOBUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                              PSTR pszText, int nBitsPixel, RECT* pRect);
//static void Paint3DRIcon(HDC hdc, RECT* pRect);
static void RDDot_Paint(HWND hWnd, HDC hdc, RECT* pRect, int nBitsPixel, 
                        PBUTTONDATA pButtonData);
static void CHECKBOX_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                           PSTR pszText, int nBitsPixel, RECT* pRect);
static void CHKIcon_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                          int nBitsPixel, RECT* pRect);
#if (!INTERFACE_MONO)
static void DrawCHKIconFrame(HDC hdc, RECT* pRect, int nBitsPixel);
static void EraseRect(HWND hWnd, HDC hdc, RECT* pRect);
static void Draw3DRectFrame(HWND hWnd, HDC hdc, PRECT pRect, BOOL bDown, 
                            int nBitsPixel, PBUTTONDATA pButtonData);
#endif

static void DrawFocus(HDC hdc, RECT* pRect);

static void BUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                         PSTR pszText, BOOL bDown, int nBitsPixel, RECT* pRect);
static void BITMAP_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                         RECT* pRect, BOOL bDown);
static void PaintTheRightPart(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                              RECT* pRect, PSTR pszText);
static void GetTextAndPaintButton(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData,
                               BOOL bDown, int nBitsPixel, RECT* pRect);
static void GetTextAndDrawFocus(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                                RECT* pRect);
static HBRUSH SetCtlColorButton(HWND hWnd, HDC hdc);
static HBITMAP CreateEdgedTextBitmap( HDC hdc, char* szText, COLORREF clrText, 
     COLORREF clrEdge, COLORREF clrTransparent, DWORD dwDrawFlag, int width, int height);

//static HBITMAP hbmpXor, hbmpAnd, hbmpSTBXor, hbmpSTBAnd, hBmpChecked, hSTBBmpChecked;

static HBITMAP  hFocBmp, hNorBmp, hPusBmp, hDisBmp, hbmpFocus = NULL;

BOOL BUTTON_RegisterClass(void)
{
    WNDCLASS wc;
    int  nPathLen;
    char PathName[MAX_PATH];

    wc.style            = CS_OWNDC | CS_DEFWIN;//CS_PARENTDC;
    wc.lpfnWndProc      = BUTTON_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(BUTTONDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "BUTTON";

    if (!RegisterClass(&wc))
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);

#if (INTERFACE_MONO)
#else
    hBmpChecked = CreateBitmap(CHKBMPWIDTH, CHKBMPHEIGHT, 1, 1, cCheckedBmpData);
    hSTBBmpChecked = CreateBitmap(STBCHKBMPWIDTH, STBCHKBMPHEIGHT, 1, 1, 
        cSTBCheckedBmpData);
#endif
    strcat(PathName, "ss_1btn.bmp");
    hNorBmp = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "ss_fbtn.bmp");
    hFocBmp = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "ss_3btn.bmp");
    hPusBmp = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "ss_dbtn.bmp");
    hDisBmp = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "listfocus.bmp");
    hbmpFocus = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //PathName[nPathLen] = '\0';
    //strcat(PathName, "radio.bmp");
    //hbmpXor = (HBITMAP)LoadImage(NULL, PathName, IMAGE_BITMAP, 
        //0, 0, LR_LOADFROMFILE);
    //hbmpAnd = CreateBitmap(R3DBMPWIDTH, R3DBMPHEIGHT, 1, 1, rAndBmpData);
   
    //hbmpSTBXor = CreateBitmap(RSTBBMPWIDTH, RSTBBMPHEIGHT, 1, 1, rSTBXorBmpData);
    //PathName[nPathLen] = '\0';
    //strcat(PathName, "stbradio.bmp");
    //hbmpSTBXor = (HBITMAP)LoadImage(NULL, PathName, IMAGE_BITMAP, 
        //0, 0, LR_LOADFROMFILE);
    //hbmpSTBAnd = CreateBitmap(RSTBBMPWIDTH, RSTBBMPHEIGHT, 1, 1, rSTBAndBmpData);
   
    return TRUE;
}

static LRESULT CALLBACK BUTTON_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    PBUTTONDATA     pButtonData;
    PCREATESTRUCT   pCreateStruct;
    HDC             hdc;
    int             x, y;
    RECT            rect;
	int             nBitsPixel;
	WORD            vkey;
    HANDLE	        hImage;
	WORD	        fImageType;
    HWND            hWndNext, hWndParent, hWndFocus, hwndNewFocus;
	DWORD	        dwStyle, wStyle, wStyleOld;
    char	        achClassName[8];
    //int           nCXMin, nCYMin;
    HBRUSH          hBrush;
    BOOL            fRedraw;
    HFONT           hFont;

    pButtonData = (PBUTTONDATA)GetUserData(hWnd);
	lResult = 0;

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
	
    switch (wMsgCmd)
    {
    case WM_CREATE :

        pCreateStruct = (PCREATESTRUCT)lParam;
        memset(pButtonData, 0, sizeof(BUTTONDATA));

        // Initializes the internal data
		//pButtonData->byStyle = (BYTE)LOWORD(pCreateStruct->style);
        pButtonData->byStyle = (BYTE)(LOWORD(pCreateStruct->style) & 0x000F);
        pButtonData->bLeftText = pCreateStruct->style & BS_LEFT;
		pButtonData->wID = (WORD)(DWORD)pCreateStruct->hMenu;
        pButtonData->wCheckState = BST_UNCHECKED;
        
        if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
            (pButtonData->byStyle == BS_DEFPUSHBUTTON) ||
            (pButtonData->byStyle == BS_CHECKBOX) || 
            (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
            (pButtonData->byStyle == BS_RADIOBUTTON) ||
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))
            //(pButtonData->byStyle == BS_BITMAP))
		{
			pButtonData->hImageNormal = (HANDLE)(pCreateStruct->lpCreateParams);
#if 0
            if (pButtonData->hImageNormal)
            {
                GetObject((HGDIOBJ)pButtonData->hImageNormal, sizeof(BITMAP), &bitmap);
                
                if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
                    (pButtonData->byStyle == BS_DEFPUSHBUTTON))// ||
                    //(pButtonData->byStyle == BS_BITMAP))
                {
#if (INTERFACE_MONO)
#else // INTERFACE_MONO
                    if (nBitsPixel != 1) //not mono
                    {
                        pCreateStruct->cx = bitmap.bmWidth + 4;
                    }
#endif // INTERFACE_MONO
                }
                else
                    pCreateStruct->cx = bitmap.bmWidth + MAXBMPWIDTH + TEXT_GAP;

                pCreateStruct->cy = bitmap.bmHeight;
            }
#endif
		}

        //nCXMin = GetSystemMetrics(SM_CXMIN);
        //nCYMin = GetSystemMetrics(SM_CYMIN);
/*
        if (pCreateStruct->cx < MINIWIDTH) 
            pCreateStruct->cx = MINIWIDTH;

        if (pCreateStruct->cy < MINIHEIGHT) 
            pCreateStruct->cy = MINIHEIGHT;
*/
        // Creating succeed.
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (((pButtonData->byStyle == BS_PUSHBUTTON) ||
            (pButtonData->byStyle == BS_DEFPUSHBUTTON)) && (dwStyle & WS_BITMAP))
        {            
            pButtonData->hImageNormal = hNorBmp;
            pButtonData->hImageFocus = hFocBmp;
            pButtonData->hImagePushDown = hPusBmp;
            pButtonData->hImageDisable = hDisBmp;
        }
        lResult = (LRESULT)TRUE;
                                     
        break;

    case WM_SIZE :
        
        break;

    case WM_NCDESTROY :
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
		//DeleteObject((HDC)pButtonData->hImageDisable);

        break;
    case WM_NCPAINT:
        if (pButtonData->hImageFocus == NULL)
            lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;


    case WM_PAINT :

        hdc = BeginPaint(hWnd, NULL);

        GetClientRect(hWnd, &rect);
        GetTextAndPaintButton(hWnd, hdc, pButtonData, pButtonData->bPenDown, 
            nBitsPixel, &rect);

        EndPaint(hWnd, NULL);

        break;

    case WM_PENMOVE :

        if (!IsWindowEnabled(hWnd) || !pButtonData->bPenDown)
            break;
        
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        GetClientRect(hWnd, &rect);
        if (PtInRectXY(&rect, x, y) ^ pButtonData->bLeaveWindow)
            break;

        if (!PtInRectXY(&rect, x, y))
        {
            pButtonData->bLeaveWindow = TRUE;
            pButtonData->bState = FALSE;
        }
        else
        {
            pButtonData->bLeaveWindow = FALSE;
            pButtonData->bState = TRUE;
        }
        
        pButtonData->bPenMove = TRUE;
        
    //   WM_PENMOVE时,要有LEAVEWINDOW 和 ENTERWINDOW效果.
    case WM_PENDOWN :

//#ifndef _EMULATE_
//		if (IsWindowEnabled(hWnd))
//			
//#endif //_EMULATE_

        if (!pButtonData->bPenMove)
        {
            if (!IsWindowEnabled(hWnd))
                break;
            
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            
            GetClientRect(hWnd, &rect);

            if (PtInRectXY(&rect, x, y))
            {
                SetFocus(hWnd);
                SetCapture(hWnd);
                
                if (GetFocus() != hWnd)
                {
                    ReleaseCapture();
                    break;
                }

                pButtonData->bPenDown = TRUE;
                pButtonData->bState = TRUE;
            }
        }
        
        hdc = GetDC(hWnd);

#if (INTERFACE_MONO)
        if (pButtonData->byStyle == BS_PUSHBUTTON || 
            pButtonData->byStyle == BS_DEFPUSHBUTTON)
        {
            GetTextAndPaintButton(hWnd, hdc, pButtonData, 
				!pButtonData->bLeaveWindow, nBitsPixel, &rect);
        }

#else // INTERFACE_MONO
        if (nBitsPixel == 1) //not mono
		{
            if (pButtonData->byStyle == BS_PUSHBUTTON || 
                pButtonData->byStyle == BS_DEFPUSHBUTTON)
				InvertRect(hdc, &rect);
        }
        else
        {
			if (pButtonData->byStyle == BS_PUSHBUTTON || 
                pButtonData->byStyle == BS_DEFPUSHBUTTON)
			{
                SetCtlColorButton(hWnd, hdc);
                GetTextAndPaintButton(hWnd, hdc, pButtonData, 
                    !pButtonData->bLeaveWindow, nBitsPixel, &rect);
			}
		}
#endif // INTERFACE_MONO

		ReleaseDC(hWnd, hdc);

        pButtonData->bPenMove = FALSE;

        break;

    case WM_PENUP :

        if (!IsWindowEnabled(hWnd) || !pButtonData->bPenDown)
            break;

        ReleaseCapture();

        pButtonData->bPenDown = FALSE;
        pButtonData->bState = FALSE;

        x = LOWORD(lParam);
        y = HIWORD(lParam);

        GetClientRect(hWnd, &rect);

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        
        if (dwStyle & WS_CHILD)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
        
        if ((pButtonData->byStyle == BS_AUTOCHECKBOX) || 
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))
        {
            if (PtInRectXY(&rect, x, y))
            {
                hdc = GetDC(hWnd);

                if (pButtonData->byStyle == BS_AUTOCHECKBOX)	
                {
                    if (pButtonData->wCheckState == BST_UNCHECKED)
                    {
                        pButtonData->wCheckState = BST_CHECKED;
                    }
                    else if (pButtonData->wCheckState == BST_CHECKED)
                    {
                        pButtonData->wCheckState = BST_UNCHECKED;
                    }
                    CHKIcon_Paint(hWnd, hdc, pButtonData, nBitsPixel, &rect);
                }
                else //BS_AUTORADIOBUTTON
                {
                    if (pButtonData->wCheckState == BST_UNCHECKED)
                    {
                        pButtonData->wCheckState = BST_CHECKED;
                    
#if (!INTERFACE_MONO)
                        if (nBitsPixel == 2) 
                            SetCtlColorButton(hWnd, hdc);
#endif // INTERFACE_MONO

                        RDDot_Paint(hWnd, hdc, &rect, nBitsPixel, pButtonData);

                        if (hWndParent)
                        {
                            hWndNext = hWnd;
                            while ((hWndNext = GetNextDlgGroupItem(hWndParent, hWndNext, 
                                TRUE)) != hWnd)
                            {
                                wStyle = GetWindowLong(hWndNext, GWL_STYLE);
                                
                                GetClassName(hWndNext, achClassName, 10);
                                
                                if (((wStyle & BS_RADIOBUTTON) || 
                                    (wStyle & BS_AUTORADIOBUTTON)) &&
                                    !(stricmp(achClassName, "BUTTON")))
                                {
                                    SendMessage(hWndNext, BM_SETCHECK, 
                                        (WPARAM)BST_UNCHECKED, 0);
                                }
                            }
                            //According MS Windows, when a radiobutton is hided, 
                            //GetNextDlgGroupItem func will not find it.
                            //And when radiobutton already is selected, 
                            //it also send notify message.
                        }
                    }
                }

                ReleaseDC(hWnd, hdc);

                SendMessage(hWndParent, WM_COMMAND, 
                    MAKELONG(pButtonData->wID, BN_CLICKED), (LPARAM)hWnd);
            }
        }
        else if (pButtonData->byStyle == BS_PUSHBUTTON || 
                 pButtonData->byStyle == BS_DEFPUSHBUTTON) 
        {
            if (!pButtonData->bLeaveWindow)
            {
                hdc = GetDC(hWnd);

#if (INTERFACE_MONO)
            GetTextAndPaintButton(hWnd, hdc, pButtonData, 
                FALSE, nBitsPixel, &rect);
#else // INTERFACE_MONO
                if (nBitsPixel == 1) //mono.
                    InvertRect(hdc, &rect);
                else
                {
                    SetCtlColorButton(hWnd, hdc);
                    GetTextAndPaintButton(hWnd, hdc, pButtonData, 
                        pButtonData->bPenDown, nBitsPixel, &rect);
                }
#endif // INTERFACE_MONO

                ReleaseDC(hWnd, hdc);
            }
            else
                pButtonData->bLeaveWindow = FALSE;

            //在画图时把RECT改变了，所以重新获得。
            GetClientRect(hWnd, &rect);

            if (PtInRectXY(&rect, x, y))
            {
                SendMessage(hWndParent, WM_COMMAND, 
                    MAKELONG(pButtonData->wID, BN_CLICKED), (LPARAM)hWnd);
            }
        }

        break;

//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN :
        
        vkey = LOWORD(wParam);
        
        switch (vkey)
        {
        //Don't process VK_RETURN, dialog send BN_COMMAND message to default button,
        //even if hasn't create this button, only define BUTTON ID.
        case VK_SPACE :
        //case VK_RETURN:
            
            SendMessage(hWnd, WM_PENDOWN, NULL, (LPARAM)MAKELPARAM(5, 5));
            SendMessage(hWnd, WM_PENUP, NULL, (LPARAM)MAKELPARAM(5, 5));
            break;
        case VK_RETURN:
            SendMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
            break;

        case VK_UP:
            hWndParent = GetParent(hWnd);
            if (hWndParent)
            {
                DWORD dwStyle;
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                if (dwStyle & CS_NOSYSCTRL)
                {
                    SendMessage(hWndParent, wMsgCmd, wParam, lParam);
                    return 0;
                }
                hWndFocus = GetNextDlgTabItem(hWndParent, hWnd, TRUE);
				SetFocus(hWndFocus);
                return 0;
            }
            break;
//#endif //SCP_NOKEYBOARD
            
        case VK_DOWN:
            hWndParent = GetParent(hWnd);
            if (hWndParent)
            {
                DWORD dwStyle;
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                if (dwStyle & CS_NOSYSCTRL)
                {
                    SendMessage(hWndParent, wMsgCmd, wParam, lParam);
                    return 0;
                }
                hWndFocus = GetNextDlgTabItem(hWndParent, hWnd, FALSE);
				SetFocus(hWndFocus);
                return 0;
            }
            break;
            
        case VK_F5:
//            SendMessage(hWnd, WM_PENDOWN, NULL, (LPARAM)MAKELPARAM(5, 5));
//
//            SendMessage(hWnd, WM_PENUP, NULL, (LPARAM)MAKELPARAM(5, 5));
//
//            break;

        case VK_F2://挂断键退出
        case VK_F10:
        //case VK_RETURN:
            hWndParent = GetParent(hWnd);
            SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);
            return 0;

        default :
            break;
        }
        break;

    case WM_KEYUP:
        if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
        {
            SendMessage(GetParent(hWnd), WM_KEYUP, wParam, lParam);
        }
        break;

	case BM_GETCHECK :

		return pButtonData->wCheckState;

	case BM_SETCHECK :

        if ((pButtonData->byStyle == BS_CHECKBOX) ||
            (pButtonData->byStyle == BS_AUTOCHECKBOX) ||
            (pButtonData->byStyle == BS_RADIOBUTTON) ||
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
        {
            if (pButtonData->wCheckState != wParam && 
                pButtonData->wCheckState != BST_INDETERMINATE)
            {
                //According MS Windows, not remove the selected state of the other 
                //RadioButton in its group.
                pButtonData->wCheckState = (BOOL)wParam;
                
                hdc = GetDC(hWnd);

                GetClientRect(hWnd, &rect);
                
                if ((pButtonData->byStyle == BS_CHECKBOX) ||
                    (pButtonData->byStyle == BS_AUTOCHECKBOX))
                {
                    CHKIcon_Paint(hWnd, hdc, pButtonData, nBitsPixel, &rect);
                }
                else
                {
#if (!INTERFACE_MONO)
                    if (nBitsPixel == 2) 
                        SetCtlColorButton(hWnd, hdc);
#endif
                    
                    RDDot_Paint(hWnd, hdc, &rect, nBitsPixel, pButtonData);
                }
                
                ReleaseDC(hWnd, hdc);
            }
        }

        //This message always returns zero. 

		break;

    case WM_ERASEBKGND:
        
        hdc = (HDC)wParam;

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        
        if (dwStyle & WS_CHILD)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
        
#if (!NOSENDCTLCOLOR)
        if (!hWndParent)
        {
#endif
            SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
            SetTextColor(hdc, RGB(0, 0, 0));
            
            hBrush = (HBRUSH)(COLOR_BTNFACE + 1);
#if (!NOSENDCTLCOLOR)
        }
        else
        {
            hBrush = (HBRUSH)SendMessage(hWndParent, (WORD)WM_CTLCOLORBTN, 
                (WPARAM)hdc, (LPARAM)hWnd);
        }
#endif
    
        dwStyle = GetWindowLong(hWndParent, GWL_STYLE);

        //GroupBox must be transparent mode. RadioButton and check box also use 
        //transparent mode, though this is diffrent from WINDOW.
        if (pButtonData->byStyle == BS_PUSHBUTTON || 
            pButtonData->byStyle == BS_DEFPUSHBUTTON || 
            ((dwStyle & WS_CLIPCHILDREN) && (pButtonData->byStyle != BS_GROUPBOX)))
        {
            GetClientRect(hWnd, &rect);
    
            FillRect(hdc, &rect, hBrush);

            //An application should return nonzero if it erases the background; 
            //otherwise, it should return zero. 
            lResult = (LRESULT)TRUE;
        }
            
        break;

    case WM_NCHITTEST:

        if (pButtonData->byStyle == BS_GROUPBOX)
            lResult = (LRESULT)HTTRANSPARENT;
        else
            lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;

    case WM_GETDLGCODE:
        
        switch (pButtonData->byStyle)
        {
        case BS_AUTOCHECKBOX: 
        
            lResult = (LRESULT)DLGC_WANTCHARS | DLGC_BUTTON;
            break;
            
        case BS_AUTORADIOBUTTON:
            
            lResult = (LRESULT)DLGC_RADIOBUTTON; 
            break;
        
        case BS_CHECKBOX:
        
            lResult = (LRESULT)DLGC_WANTCHARS | DLGC_BUTTON;
            break;
        
        case BS_DEFPUSHBUTTON: 
            
            lResult = (LRESULT)DLGC_DEFPUSHBUTTON; 
            break;
            
        case BS_GROUPBOX: 
            
            lResult = (LRESULT)DLGC_STATIC; 
            break;
            
        case BS_PUSHBUTTON:
            
            lResult = (LRESULT)DLGC_UNDEFPUSHBUTTON; 
            break;
            
        case BS_RADIOBUTTON: 
            
            lResult = (LRESULT)DLGC_RADIOBUTTON; 
            break;
        }
        break;

    case BM_SETSTYLE:
        
        //Except for BS_LEFTTEXT.
        wStyle =  LOWORD(wParam) & 0x000F; // style 
        fRedraw =  LOWORD(lParam);   // redraw flag 
        
        /*if (((DWORD)pButtonData->byStyle & 0x0F) == wStyle)
            break;

        wStyleOld = pButtonData->byStyle; 
        pButtonData->byStyle &= ~0x0F;
        pButtonData->byStyle |= wStyle;*/
        if ((DWORD)pButtonData->byStyle == wStyle)
            break;

        wStyleOld = pButtonData->byStyle; 
        pButtonData->byStyle = wStyle;

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);

        ClearRect(hdc, &rect, COLOR_WHITE);

        SetCtlColorButton(hWnd, hdc);
        GetTextAndPaintButton(hWnd, hdc, pButtonData, pButtonData->bPenDown, 
            nBitsPixel, &rect);

        ReleaseDC(hWnd, hdc);

        //SetWindowLong(hWnd, GWL_STYLE, ((pButtonData->byStyle & 0xFF) | 
            //(GetWindowLong(hWnd, GWL_STYLE) & 0xFFFF0000)));
        SetWindowLong(hWnd, GWL_STYLE, (pButtonData->byStyle | 
            (GetWindowLong(hWnd, GWL_STYLE) & 0xFFFFFFF0)));
        //This message always returns zero. 
        break;

    case BM_GETSTATE :

        if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
            (pButtonData->byStyle == BS_DEFPUSHBUTTON))
        {
            if (pButtonData->bState)
                lResult = BST_PUSHED;
        }
        else if ((pButtonData->byStyle == BS_CHECKBOX) ||
            (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
            (pButtonData->byStyle == BS_RADIOBUTTON) ||
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))
        {
            lResult = pButtonData->wCheckState;
        }

        if (pButtonData->bFocus)
            lResult |= BST_FOCUS;

		break;

	case BM_SETSTATE :
        
        //Highlighting only affects the appearance of a button. 
        //It has no effect on the check state of a radio button or check box. 
        if ((pButtonData->byStyle != BS_PUSHBUTTON) &&
            (pButtonData->byStyle != BS_DEFPUSHBUTTON))
            break;

        if (pButtonData->bState == wParam)
            break;

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);

        //wParam specifies whether the button is to be highlighted. 
        //A value of TRUE highlights the button. A value of FALSE removes any highlighting.
        pButtonData->bState = (BYTE)wParam;
        GetTextAndPaintButton(hWnd, hdc, pButtonData, wParam, nBitsPixel, 
            &rect);

        ReleaseDC(hWnd, hdc);

        //This message always returns zero. 
		break;

	case WM_ENABLE :

		//bDisabled = (BOOL) wParam;
        //重画Button的Enable状态.

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);
        
        SetCtlColorButton(hWnd, hdc);
        GetTextAndPaintButton(hWnd, hdc, pButtonData, pButtonData->bPenDown, 
            nBitsPixel, &rect);
        
        ReleaseDC(hWnd, hdc);

		break;

    case WM_SETFOCUS :
        
        hWndParent = GetParent(hWnd);
        hwndNewFocus = GetNextDlgTabItem(hWndParent, hWnd, TRUE);
        if (hwndNewFocus == hWnd)
            SendMessage(hWndParent, PWM_SETSCROLLSTATE, 
                SCROLLMIDDLE, MASKALL);
        else
		    SendMessage(hWndParent, PWM_SETSCROLLSTATE, 
                SCROLLUP | SCROLLDOWN | SCROLLMIDDLE, MASKALL);

        if (pButtonData->hImageFocus == NULL)
            DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        
#if (!NOKEYBOARD)
        pButtonData->bFocus = TRUE;

        SendMessage(GetParent(hWnd), WM_COMMAND, 
            MAKELONG(pButtonData->wID, BN_SETFOCUS), (LPARAM)hWnd);

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);

        if ((pButtonData->byStyle == BS_CHECKBOX) ||
            (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
            (pButtonData->byStyle == BS_RADIOBUTTON) ||
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
        {
            GetTextAndDrawFocus(hWnd, hdc, pButtonData, &rect);
        }

        if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
            (pButtonData->byStyle == BS_DEFPUSHBUTTON))
        {
            //Pushbutton 3D effect.
            InflateRect(&rect, -4, -4);

            //Except default pushbutton black frame.

            //DrawFocus(hdc, &rect);
        }
        
        ReleaseDC(hWnd, hdc);
#endif // NOKEYBOARD
        InvalidateRect(hWnd, NULL, TRUE);

		break;

    case WM_KILLFOCUS :
        
        if (pButtonData->hImageFocus == NULL)
            DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        
#if (!NOKEYBOARD)
        pButtonData->bFocus = FALSE;

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);

        if ((pButtonData->byStyle == BS_CHECKBOX) ||
            (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
            (pButtonData->byStyle == BS_RADIOBUTTON) ||
            (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
        {
            GetTextAndDrawFocus(hWnd, hdc, pButtonData, &rect);
        }

        if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
            (pButtonData->byStyle == BS_DEFPUSHBUTTON))
        {
            if (!pButtonData->bLeaveWindow && pButtonData->bPenDown)
            {
                pButtonData->bLeaveWindow = pButtonData->bPenDown = FALSE;
                ReleaseCapture();

                SetCtlColorButton(hWnd, hdc);
                GetTextAndPaintButton(hWnd, hdc, pButtonData, 
                    FALSE, nBitsPixel, &rect);
            }
            else
            {
                //Pushbutton 3D effect.
                InflateRect(&rect, -4, -4);
                
                //Except default pushbutton black frame.
             
                //DrawFocus(hdc, &rect);
            }
        }
        
        ReleaseDC(hWnd, hdc);
#endif // NOKEYBOARD
        SendMessage(GetParent(hWnd), WM_COMMAND, 
            MAKELONG(pButtonData->wID, BN_KILLFOCUS), (LPARAM)hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        
        break;

    case WM_SETFONT:
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        hdc = GetDC(hWnd);
        SelectObject(hdc, hFont);
        ReleaseDC(hWnd, hdc);

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

	case WM_SETTEXT :

        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        hdc = GetDC(hWnd);

        GetClientRect(hWnd, &rect);
        SetCtlColorButton(hWnd, hdc);
        BUTTON_Paint(hWnd, hdc, pButtonData, (PSTR)lParam, pButtonData->bPenDown, 
            nBitsPixel, &rect);

        ReleaseDC(hWnd, hdc);

		break;

	case BM_GETIMAGE :
		
		fImageType = LOWORD(wParam);       // image-type flag 
		
		if (((fImageType == IMAGE_BITMAP) || (fImageType == IMAGE_ICON)) && 
            ((pButtonData->byStyle == BS_PUSHBUTTON) || 
             (pButtonData->byStyle == BS_DEFPUSHBUTTON) || 
             (pButtonData->byStyle == BS_CHECKBOX) || 
             (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
             (pButtonData->byStyle == BS_RADIOBUTTON) ||
             (pButtonData->byStyle == BS_AUTORADIOBUTTON)))// ||
             //(pButtonData->byStyle == BS_BITMAP)))
		{
            if (wParam & IMAGE_DISABLE)
            {
                lResult = (LRESULT)pButtonData->hImageDisable;
            }
            else if (wParam & IMAGE_PUSHDOWN)
            {
                lResult = (LRESULT)pButtonData->hImagePushDown;
            }
            else if (wParam & IMAGE_FOCUS)
            {
                lResult = (LRESULT)pButtonData->hImageFocus;
            }
            else
            {
                lResult = (LRESULT)pButtonData->hImageNormal;
            }
		}

		break;
        
	case BM_SETIMAGE :
		
		fImageType = LOWORD(wParam); // image-type flag 
		hImage = (HANDLE) lParam;   // handle of the image 
 		
		if (((fImageType == IMAGE_BITMAP) || (fImageType == IMAGE_ICON)) &&
            ((pButtonData->byStyle == BS_PUSHBUTTON) ||
             (pButtonData->byStyle == BS_DEFPUSHBUTTON) || 
             (pButtonData->byStyle == BS_CHECKBOX) || 
             (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
             (pButtonData->byStyle == BS_RADIOBUTTON) ||
             (pButtonData->byStyle == BS_AUTORADIOBUTTON)))// ||
             //(pButtonData->byStyle == BS_BITMAP)))
		{
            if (wParam & IMAGE_DISABLE)
            {
                lResult = (LRESULT)pButtonData->hImageDisable;
                pButtonData->hImageDisable = hImage;
            }
            else if (wParam & IMAGE_PUSHDOWN)
            {
                lResult = (LRESULT)pButtonData->hImagePushDown;
                pButtonData->hImagePushDown = hImage;
            }
            else if (wParam & IMAGE_FOCUS)
            {
                lResult = (LRESULT)pButtonData->hImageFocus;
                pButtonData->hImageFocus = hImage;
            }
            else
            {
                lResult = (LRESULT)pButtonData->hImageNormal;
                pButtonData->hImageNormal = hImage;
            }
            
#if 0
            if (hImage)
            {
                GetObject((HGDIOBJ)hImage, sizeof(BITMAP), &bitmap);
                
                if ((pButtonData->byStyle == BS_PUSHBUTTON) ||
                    (pButtonData->byStyle == BS_DEFPUSHBUTTON))// ||
                    //(pButtonData->byStyle == BS_BITMAP))
                {
#if (!INTERFACE_MONO)
                    if (nBitsPixel == 2)
                    {
                        bitmap.bmWidth = bitmap.bmWidth + 6;
                        bitmap.bmHeight = bitmap.bmHeight + 6;
                    }
                    else if (nBitsPixel != 1)
                    {
                        bitmap.bmWidth = bitmap.bmWidth + 12;
                        bitmap.bmHeight = bitmap.bmHeight + 12;
                    }
#endif // INTERFACE_MONO
                }
                else
                {
                    bitmap.bmWidth = bitmap.bmWidth + MAXBMPWIDTH + TEXT_GAP;
                    bitmap.bmHeight = bitmap.bmHeight;
                }

                GetWindowRectEx(hWnd, &rect, W_WINDOW, XY_SCREEN);
                
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                    
                if ((dwStyle & WS_NCCHILD) || !(dwStyle & WS_CHILD))
                {
                    // WS_NCCHILD, WS_POPUP, WS_POPUPWINDOW. (WS_TOPMOST等于
                    // WS_NCCHILD)
                    MoveWindow(hWnd, rect.left, rect.top, bitmap.bmWidth + FOCUSRECTWIDTH, 
                        bitmap.bmHeight + FOCUSRECTWIDTH, TRUE);
                }
                else
                {
                    hWndParent = GetParent(hWnd);
                    
                    GetWindowRectEx(hWndParent, &rcParent, W_CLIENT, 
                        XY_SCREEN);
                    MoveWindow(hWnd, rect.left - rcParent.left, 
                        rect.top - rcParent.top, bitmap.bmWidth + FOCUSRECTWIDTH, 
                        bitmap.bmHeight + FOCUSRECTWIDTH, TRUE);
                }
            }
#endif
		}

		break;

    default :

        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

static void PUSHBUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                             PSTR pszText, BOOL bDown, int nBitsPixel, 
                             RECT* pRect)
{
#if (!INTERFACE_MONO)
	HPEN hPenOld;
	HBRUSH hBrushOld;
    COLORREF clrOldText;
#endif // INTERFACE_MONO
    //COLORREF clrOldBK;
    //int     nOldMode;
    HBRUSH hBrush = NULL;
    DWORD   dwStyle;
   // HBITMAP  hBmp;

    if ((pButtonData->hImageNormal) || (pButtonData->hImagePushDown && bDown) ||
        (pButtonData->hImageDisable && !IsWindowEnabled(hWnd)) ||
        (pButtonData->hImageFocus && pButtonData->bFocus == TRUE))
    {
#if (INTERFACE_MONO)
        
        BITMAP_Paint(hWnd, hdc, pButtonData, pRect, bDown);
        SendMessage(GetParent(hWnd), (WORD)WM_CTLCOLORBTN, 
            (WPARAM)hdc, (LPARAM)hWnd);
        if (bDown && pButtonData->hImagePushDown == NULL)
            SetROP2(hdc, ROP_NSRC);
        pRect->top +=  FOCUSRECTWIDTH / 2;
        pRect->left +=  FOCUSRECTWIDTH / 2;
        pRect->right -=  FOCUSRECTWIDTH / 2;
        pRect->bottom -=  FOCUSRECTWIDTH / 2;
        //DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);

/*
        if (pButtonData->bLeftText)
        {
            hBmp = CreateEdgedTextBitmap(hdc, pszText, RGB(0, 0, 0), RGB(255, 255, 255), 
                RGB(255, 255, 0), DT_LEFT | DT_VCENTER, pRect->right - pRect->left,
                pRect->bottom - pRect->top);
        }
        else
        {
            hBmp = CreateEdgedTextBitmap(hdc, pszText, RGB(0, 0, 0), RGB(255, 255, 255),
                RGB(255, 255, 0), DT_HCENTER | DT_VCENTER, pRect->right - pRect->left,
                pRect->bottom - pRect->top);
        }
        nOldMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
        clrOldBK = SetBkColor(hdc, RGB(255, 255, 0));
        BitBlt(hdc, pRect->left, pRect->top, pRect->right - pRect->left,
                pRect->bottom - pRect->top, (HDC)hBmp, 0, 0, SRCCOPY);
        SetBkColor(hdc, clrOldBK);
        SetBkMode(hdc, nOldMode);
        DeleteObject((HGDIOBJ)hBmp);*/

        SetBkMode(hdc, BM_TRANSPARENT);
        pRect->left -= 2;
        pRect->top -= 2;
        if (pButtonData->bLeftText)
            DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER);
        else
            DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);
        pRect->left += 2;
        pRect->top += 2;
        SetTextColor(hdc, RGB(255, 255, 255));
        if (pButtonData->bLeftText)
            DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER);
        else
            DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);
        SetBkMode(hdc, BM_OPAQUE);
        if (bDown && pButtonData->hImagePushDown == NULL)
            SetROP2(hdc, ROP_SRC);

#else // INTERFACE_MONO
        EraseRect(hWnd, hdc, pRect);
        if (nBitsPixel == 1)
            BITMAP_Paint(hWnd, hdc, pButtonData, pRect, bDown);
        else
        {
            if (bDown)
            {
                pRect->top +=  2;
                pRect->left += 2;
            }

            BITMAP_Paint(hWnd, hdc, pButtonData, pRect, bDown);

            if (bDown)
            {
                pRect->top -=  2;
                pRect->left -= 2;
            }

            Draw3DRectFrame(hWnd, hdc, pRect, bDown, nBitsPixel, pButtonData);

            if (pButtonData->bFocus)
            {
                InflateRect(pRect, -3, -3);
                if (pButtonData->byStyle == BS_PUSHBUTTON)
                    InflateRect(pRect, -1, -1);
                
                
                
                if ((pButtonData->byStyle == BS_CHECKBOX) ||
                    
                    (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
                    
                    (pButtonData->byStyle == BS_RADIOBUTTON) ||
                    
                    (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
                    DrawFocus(hdc, pRect);
            }
        }
#endif // INTERFACE_MONO
        
        return;
    }

#if (INTERFACE_MONO)

    ClearRect(hdc, pRect, COLOR_WHITE);

    //if (bDown)
        //SelectObject(hdc, GetStockObject(BLACK_BRUSH));
    hBrush = (HBRUSH)SendMessage(GetParent(hWnd), (WORD)WM_CTLCOLORBTN, 
            (WPARAM)hdc, (LPARAM)hWnd);
    SelectObject(hdc, hBrush);

    if (bDown)
        SetROP2(hdc, ROP_NSRC);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (dwStyle & WS_BORDER)
        RoundRect(hdc, pRect->left, pRect->top, pRect->right, pRect->bottom, 
            0, 0);

/*
    if (pButtonData->byStyle == BS_DEFPUSHBUTTON)
    {
        RoundRect(hdc, pRect->left, pRect->top + 1, pRect->right, 
            pRect->bottom - 1, 0, 0);
        RoundRect(hdc, pRect->left + 1, pRect->top + 1, pRect->right - 1, 
            pRect->bottom - 1, 0, 0);
    }
*/
    /*if (bDown)
    {
        SetBkColor(hdc, COLOR_BLACK);
        SetTextColor(hdc, COLOR_WHITE);
    }*/
    

    // Leave space for round rectangle corners
    InflateRect(pRect, -2, -2);

    //DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);
    if (pButtonData->bLeftText)
        DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER);
    else
        DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);

    // Gray the output text
    if (!IsWindowEnabled(hWnd))
    {
        int y;
        HPEN hPenold;
        SIZE szChar;

        GetTextExtentPoint(hdc, "T", 1, &szChar);
        hPenold = SelectObject(hdc, GetStockObject(WHITE_PEN));

        y = pRect->top;
        while (y < pRect->bottom)
        {
            DrawLine(hdc, pRect->left, y, pRect->right, y);
            y += 2;
        }
        SelectObject(hdc, hPenold);
    }
    if (bDown)
        SetROP2(hdc, ROP_SRC);
    
#else // INTERFACE_MONO

    if (nBitsPixel == 1) //mono.
	{
        if (bDown)
		{
			clrOldBK = SetBkColor(hdc, COLOR_BLACK);
			clrOldText = SetTextColor(hdc, COLOR_WHITE);
		}
		else
		{
			clrOldBK = SetBkColor(hdc, COLOR_WHITE);
			clrOldText = SetTextColor(hdc, COLOR_BLACK);
		}
	}
    else
	{
        if (bDown)
		{
            pRect->top += 2;
            pRect->left += 2;
        }
		else if (!IsWindowEnabled(hWnd))
        {
            pRect->left += 2;
            //clrOldBK = SetBkColor(hdc, GetSysColor(COLOR_BTNHIGHLIGHT));
            clrOldText = SetTextColor(hdc, COLOR_WHITE);//first white, then dark gray.
        }
    }

    //DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER | DT_CLEAR);
    if (pButtonData->bLeftText)
        DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER | DT_CLEAR);
    else
        DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER | DT_CLEAR);

    if (nBitsPixel == 1) //not mono.
    {
        SetBkColor(hdc, clrOldBK);
        SetTextColor(hdc, clrOldText);

        //if color is mono, draw a border.
		hPenOld = SelectObject(hdc, GetStockObject(BLACK_PEN));
		hBrushOld = SelectObject(hdc, GetStockObject(NULL_BRUSH));

		DrawRect(hdc, pRect);
		SelectObject(hdc, hPenOld);
		SelectObject(hdc, hBrushOld);
	}
    else
    {
        if (bDown)
        {
            pRect->top -= 2;
            pRect->left -= 2;
        }

        if (!IsWindowEnabled(hWnd)) //not  disable.
        {
            int iBkModeOld;

            pRect->top -= 2;
            pRect->left -= 2;
            
            iBkModeOld = SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, GetSysColor(COLOR_BTNSHADOW));
            
            //DrawText(hdc, pszText, -1, pRect,
            //  DT_HCENTER | DT_VCENTER);
            if (pButtonData->bLeftText)
                DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER);
            else
                DrawText(hdc, pszText, -1, pRect, DT_HCENTER | DT_VCENTER);
            
            SetBkMode(hdc, iBkModeOld);
            
            SetTextColor(hdc, clrOldText);
            pRect->top += 2;
        }

        Draw3DRectFrame(hWnd, hdc, pRect, bDown, nBitsPixel, pButtonData);
        //if is DEFPUSHBUTTON, has inflate -1.

        if (pButtonData->bFocus)
        {
            InflateRect(pRect, -3, -3);
            if (pButtonData->byStyle == BS_PUSHBUTTON)
                InflateRect(pRect, -1, -1);
            
            
            if ((pButtonData->byStyle == BS_CHECKBOX) ||
                
                (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
                
                (pButtonData->byStyle == BS_RADIOBUTTON) ||
                
                (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
                DrawFocus(hdc, pRect);
        }
    }
#endif // INTERFACE_MONO
}

static void DrawGroupRect(HDC hdc, int nBitsPixel, int iTextWidth, RECT* pRect)
{
    HPEN hOldPen, hNewPen;

    if (!pRect)
        return;

#if (INTERFACE_MONO)
    hNewPen = GetStockObject(BLACK_PEN);
#else // INTERFACE_MONO
    if (nBitsPixel == 1)
        hNewPen = GetStockObject(BLACK_PEN);
    else
        hNewPen = GetStockObject(GRAY_PEN);
#endif // INTERFACE_MONO
    
    hOldPen = SelectObject(hdc, hNewPen);   

    // Draw top darkgray line
    if (iTextWidth != 0)
    {
        DrawLine(hdc, pRect->left, pRect->top, GROUPLEFTWIDTH, pRect->top);
        DrawLine(hdc, GROUPLEFTWIDTH + iTextWidth + 5, pRect->top, 
            pRect->right - 1, pRect->top);
    }
    else
    {
        DrawLine(hdc, pRect->left, pRect->top, pRect->right - 1, 
            pRect->top);
    }

    // Draw left right bottom darkgray line
    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);
    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2, 
        pRect->bottom - 1);
    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 2, 
        pRect->bottom - 2);

#if (!INTERFACE_MONO)
    if (nBitsPixel != 1)
    {
        SelectObject(hdc, GetStockObject(WHITE_PEN));
        
        // Draw top white line
        if (iTextWidth != 0)
        {
            DrawLine(hdc, pRect->left + 1, pRect->top + 1, GROUPLEFTWIDTH, 
                pRect->top + 1);
            DrawLine(hdc, GROUPLEFTWIDTH + iTextWidth + 5, pRect->top + 1, 
                pRect->right - 2, pRect->top + 1);
        }
        else
        {
            DrawLine(hdc, pRect->left + 1, pRect->top + 1, 
                pRect->right - 2, pRect->top + 1);
        }
        
        // Draw left right bottom white line
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1, pRect->bottom - 2);
        DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1, pRect->bottom - 1);
        DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right, pRect->bottom - 1);
    }
#endif

    // Restore the old pen.
    SelectObject(hdc, hOldPen);
}

static void GROUP_Paint(HDC hdc, int nBitsPixel, PSTR pszText, RECT* pRect)
{
    SIZE size;

    if (pszText)
        GetTextExtentPoint32(hdc, pszText, strlen(pszText), &size);
    else
    {
    }

    pRect->top = size.cy / 2;
    
    DrawGroupRect(hdc, nBitsPixel, size.cx, pRect);

    if (pszText)
    {
        int nOldBkMode;

        pRect->top = 0;
        pRect->bottom = size.cy;
        pRect->left += GROUPLEFTWIDTH + 3;
        pRect->right = pRect->left + size.cx;
        
        nOldBkMode = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pszText, -1, pRect, DT_LEFT | DT_VCENTER);
        SetBkMode(hdc, nOldBkMode);
    }
}

static void PaintTheRightPart(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                              RECT* pRect, PSTR pszText)
{
    RECT rect1;
    BITMAP bitmap;
    int iBkModeOld;
    SIZE size;
    COLORREF crOld = 0;
    
    rect1.left = pRect->left + MAXBMPWIDTH + TEXT_GAP;
    rect1.right = pRect->right;
    rect1.top = pRect->top;
    rect1.bottom = pRect->bottom;
    
    if (pButtonData->hImageNormal)
    {
        GetObject((HGDIOBJ)pButtonData->hImageNormal, sizeof(BITMAP), &bitmap);
        rect1.top += (pRect->bottom - bitmap.bmHeight) / 2;

        //The last parameter is bDown for pushButton to decide which bitmap to draw.
        //Here is FALSE for always using the normal bitmap.
        BITMAP_Paint(hWnd, hdc, pButtonData, &rect1, FALSE);
        //BkColor and FgColor is set by WM_ERASEBKGND.

        //For the focus.
        rect1.top = pRect->top;
        rect1.right = pRect->right;
        rect1.bottom = pRect->bottom;
    }
    else
    {
        if (pButtonData->bFocus)
        {
            crOld = SetTextColor(hdc, COLOR_WHITE);
        }
        iBkModeOld = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pszText, -1, &rect1, DT_LEFT | DT_VCENTER);
        SetBkMode(hdc, iBkModeOld);
        if (pButtonData->bFocus)
        {
            SetTextColor(hdc, crOld);
        }

        GetTextExtentPoint32(hdc, pszText, strlen(pszText), &size);

        rect1.top = (pRect->bottom - pRect->top - size.cy) / 2;
        rect1.right = rect1.left + size.cx;
        rect1.bottom = rect1.top + size.cy;
        
        InflateRect(&rect1, 1, 1);

        if (rect1.left < pRect->left)
            rect1.left = pRect->left;

        if (rect1.top < pRect->top)
            rect1.top = pRect->top;

        if (rect1.right > pRect->right)
            rect1.right = pRect->right;

        if (rect1.bottom > pRect->bottom)
            rect1.bottom = pRect->bottom;
    }
    
//    if (pButtonData->bFocus)
//    {
//        if ((pButtonData->byStyle == BS_CHECKBOX) ||
//            
//            (pButtonData->byStyle == BS_AUTOCHECKBOX) || 
//            
//            (pButtonData->byStyle == BS_RADIOBUTTON) ||
//            
//            (pButtonData->byStyle == BS_AUTORADIOBUTTON))	
//            
//            DrawFocus(hdc, &rect1);
//    }
}

static void RDDot_Paint(HWND hWnd, HDC hdc, RECT* pRect, int nBitsPixel, 
                        PBUTTONDATA pButtonData)
{
#if (INTERFACE_MONO)

    char PathName[MAX_PATH];

    switch (pButtonData->wCheckState)
    {
    case BST_CHECKED :
        
        /*DrawBitmapFromData(hdc, pRect->left, pRect->top + 
            (pRect->bottom - pRect->top - RADIOBMPHEIGHT + 1) / 2,
            RADIOBMPWIDTH, RADIOBMPHEIGHT, 1, 1, CheckedRadioBmpData, 
            ROP_SRC);*/
        PLXOS_GetResourceDirectory(PathName);
        strcat(PathName, "checkradio.bmp");
        DrawBitmapFromFile(hdc, pRect->left + CX_ICON_LEFTMARGIN, 
            pRect->top + (pRect->bottom - pRect->top - RADIOBMPHEIGHT + 1) / 2, 
			PathName, ROP_SRC);
        break;
        
    default :

        /*DrawBitmapFromData(hdc, pRect->left, pRect->top + 
            (pRect->bottom - pRect->top - RADIOBMPHEIGHT + 1) / 2,
            RADIOBMPWIDTH, RADIOBMPHEIGHT, 1, 1, UncheckedRadioBmpData, 
            ROP_SRC);*/
        PLXOS_GetResourceDirectory(PathName);
        strcat(PathName, "unchkradio.bmp");
        DrawBitmapFromFile(hdc, pRect->left + CX_ICON_LEFTMARGIN, 
            pRect->top + (pRect->bottom - pRect->top - RADIOBMPHEIGHT + 1) / 2, 
			PathName, ROP_SRC);
        break;
    }

#else // INTERFACE_MONO
    int x, y, width, height;
    HBITMAP hBitmap;
    RECT rect;
    COLORREF crOldBk, crOldText;

    if (nBitsPixel == 1 || nBitsPixel == 2)//is mono.
    {
        height = RMONOBMPHEIGHT;
        width = RMONOBMPWIDTH;
    }
    else
    {
        height = RSTBBMPHEIGHT;
        width = RSTBBMPWIDTH;
    }

    rect.top = pRect->top + (pRect->bottom - pRect->top - height) / 2;
    rect.left = pRect->left;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    //Draw unchecked radio button icon for application.
    if (nBitsPixel == 1 || nBitsPixel == 2)//is mono.
    {
        hBitmap = CreateBitmap(width, height, 1, 1, rMonoRadioBmpData);
        BitBlt(hdc, rect.left, rect.top, width, height, (HDC)hBitmap, 0, 0,
            ROP_SRC);
        DeleteObject(hBitmap);
    }
    else
    {
        hBitmap = CreateBitmap(width, height, 1, 1, rSTBRadioBmpData);

        crOldBk = SetBkColor(hdc, STBBGCOLOR);
        BitBlt(hdc, rect.left, rect.top, width, height, (HDC)hBitmap, 0, 0, ROP_SRC);
        SetBkColor(hdc, crOldBk);
        
        DeleteObject(hBitmap);
        //Paint3DRIconFrame(hdc, &rect);
    }

    if (nBitsPixel == 1 || nBitsPixel == 2)//is mono.
    {
        height = RMONODOTHEIGHT;
        width = RMONODOTWIDTH;
        y = pRect->top + (pRect->bottom - pRect->top - RMONOBMPHEIGHT) / 2;
    }
    else
    {
        height = RSTBDOTHEIGHT;
        width = RSTBDOTWIDTH;
        y = pRect->top + (pRect->bottom - pRect->top - RSTBBMPHEIGHT) / 2;
    }

    x = pRect->left;

    //dot position offset frame.
    if (nBitsPixel == 1 || nBitsPixel == 2)
    {
        y += 3;
        x += 3;
    }
    else
    {
        y += 4;
        x += 4;
    }

    // Draw radio button dot icon for application
    switch (pButtonData->wCheckState)
    {
    case BST_CHECKED :

        if (nBitsPixel == 1 || nBitsPixel == 2)
            hBitmap = CreateBitmap(width, height, 1, 1, rMonoDotBmpData);
        else
            hBitmap = CreateBitmap(width, height, 1, 1, rSTBDotBmpData);

        if (nBitsPixel == 1)
            crOldBk = SetBkColor(hdc, COLOR_WHITE);
        else if (nBitsPixel != 2)
            crOldBk = SetBkColor(hdc, STBBGCOLOR);

        crOldText = SetTextColor(hdc, COLOR_BLACK);

        BitBlt(hdc, x, y, width, height, (HDC)hBitmap, 0, 0, ROP_SRC);

        if (nBitsPixel != 2)
            SetBkColor(hdc, crOldBk);
        
        SetTextColor(hdc, crOldText);

        DeleteObject(hBitmap);
        
        break;
        
    default :

        SetRect(&rect, x, y, x + width, y + height);

        if (nBitsPixel == 1)
            ClearRect(hdc, &rect, COLOR_WHITE);
        else if (nBitsPixel == 2)
            EraseRect(hWnd, hdc, &rect);
        else
            ClearRect(hdc, &rect, STBBGCOLOR);

        break;
    }
#endif // INTERFACE_MONO
}

//static void Paint3DRIconFrame(HDC hdc, RECT* pRect)
//{
//#if (!INTERFACE_MONO)
//    int crOldText, crOldBk;
//
//    BitBlt(hdc, pRect->left, pRect->top, RSTBBMPWIDTH, RSTBBMPHEIGHT, 
//        (HDC)hbmpSTBXor, 0, 0, ROP_SRC_XOR_DST);
//
//    crOldText = SetTextColor(hdc, COLOR_BLACK);
//    crOldBk = SetBkColor(hdc, COLOR_WHITE);
//
//    BitBlt(hdc, pRect->left, pRect->top, RSTBBMPWIDTH, RSTBBMPHEIGHT, 
//        (HDC)hbmpSTBAnd, 0, 0, ROP_SRC_AND_DST);
//    
//    SetTextColor(hdc, crOldText);
//    SetBkColor(hdc, crOldBk);
//
//    BitBlt(hdc, pRect->left, pRect->top, RSTBBMPWIDTH, RSTBBMPHEIGHT, 
//        (HDC)hbmpSTBXor, 0, 0, ROP_SRC_XOR_DST);
//#endif // INTERFACE_MONO
//}

static void RADIOBUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                              PSTR pszText, int nBitsPixel, RECT* pRect)
{
    if (pButtonData->bFocus)
    {
        DrawFocus(hdc, pRect);
    }
    else
    {
        ClearRect(hdc, pRect, COLOR_TRANSBK);
    }
    RDDot_Paint(hWnd, hdc, pRect, nBitsPixel, pButtonData);
    PaintTheRightPart(hWnd, hdc, pButtonData, pRect, pszText);
}


#if (!INTERFACE_MONO)

static void Draw3DRectFrame(HWND hWnd, HDC hdc, RECT* pRect, BOOL bDown, 
                            int nBitsPixel, PBUTTONDATA pButtonData)
{
    HPEN hOldPen, hNewPen = NULL;
    HBRUSH hOldBrush;

    if (!pRect)
        return;

    // Draw default button black line.
    if (pButtonData->byStyle == BS_DEFPUSHBUTTON && nBitsPixel != 2)
    {
        hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
        hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        DrawRect(hdc, pRect);

        InflateRect(pRect, -1, -1);
        
        if (bDown)
        {
            hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));
            DrawRect(hdc, pRect);

            InflateRect(pRect, -1, -1);
            hNewPen = CreatePen(PS_SOLID, 1, GetBkColor(hdc));
            SelectObject(hdc, hNewPen);   
            DrawRect(hdc, pRect);
        }

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);

        if (bDown)
        {
            DeleteObject(hNewPen);
            return;
        }
    }

    // Draw left and top black line
    if (!bDown)
    {
        if (nBitsPixel == 2)
        {
            hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));
        }
        else
            hOldPen = SelectObject(hdc, GetStockObject(WHITE_PEN));
    }
    else
    {
        hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
    }

    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    // Draw left and top white line or darkgray line

    if (!bDown)
    {
        SelectObject(hdc, GetStockObject(LTGRAY_PEN));
    }
    else
    { 
        SelectObject(hdc, GetStockObject(GRAY_PEN));
    }

    DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
        pRect->bottom - 2);
    
    DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
        pRect->top + 1);

    // Draw right and bottom gray line

    if (!bDown)
    {
        hNewPen = GetStockObject(GRAY_PEN);
    }
    else
    {
        hNewPen = CreatePen(PS_SOLID, 1, GetBkColor(hdc));
    }

    SelectObject(hdc, hNewPen);

    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
        pRect->bottom - 2);
    
    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
        pRect->bottom - 2);

    if (!bDown)
    {
        SelectObject(hdc, GetStockObject(BLACK_PEN));
    }
    else
    {
        SelectObject(hdc, GetStockObject(WHITE_PEN));
    }

    if (bDown)
        DeleteObject(hNewPen);

    DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,
        pRect->bottom - 1);

    DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,
        pRect->bottom - 1);

    // Restore the old pen.
    SelectObject(hdc, hOldPen);
}


static void EraseRect(HWND hWnd, HDC hdc, RECT* pRect)
{
    HBRUSH  hBrush;
    
    hBrush = SetCtlColorButton(hWnd, hdc);
    FillRect(hdc, pRect, hBrush);
}

static void DrawCHKIconFrame(HDC hdc, RECT* pRect, int nBitsPixel)
{
    HPEN hOldPen;

    if (!pRect)
        return;

    // Draw left and top black line

    hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));

    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    // Draw left and top white line or darkgray line

    SelectObject(hdc, GetStockObject(BLACK_PEN));

    DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
        pRect->bottom - 2);

    DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
        pRect->top + 1);

    // Draw right and bottom gray line

    if (nBitsPixel == 2)
        SelectObject(hdc, GetStockObject(LTGRAY_PEN));
    else
        SelectObject(hdc, GetStockObject(WTGRAY_PEN));

    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
        pRect->bottom - 2);

    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
        pRect->bottom - 2);

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,
        pRect->bottom - 1);

    DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,
        pRect->bottom - 1);

    // Restore the old pen.
    SelectObject(hdc, hOldPen);
}
#endif

static void CHKIcon_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                          int nBitsPixel, RECT* pRect)
{
#if (INTERFACE_MONO)
    char PathName[MAX_PATH];

    switch (pButtonData->wCheckState)
    {
    case BST_CHECKED :
        
        /*DrawBitmapFromData(hdc, pRect->left, 
            pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2, 
            CHKBMPWIDTH, CHKBMPHEIGHT, 1, 1, CheckedBmpData, ROP_SRC);*/
        PLXOS_GetResourceDirectory(PathName);
//        strcat(PathName, "checkbmp.bmp");
        strcat(PathName, "select.bmp");
        DrawBitmapFromFile(hdc, pRect->left + CX_ICON_LEFTMARGIN, 
            pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2, 
			PathName, ROP_SRC);
        break;
        
    default :

        /*DrawBitmapFromData(hdc, pRect->left, 
            pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2, 
            CHKBMPWIDTH, CHKBMPHEIGHT, 1, 1, UncheckedBmpData, ROP_SRC);*/
        PLXOS_GetResourceDirectory(PathName);
//        strcat(PathName, "unchkbmp.bmp");
        strcat(PathName, "unselect.bmp");
        DrawBitmapFromFile(hdc, pRect->left + CX_ICON_LEFTMARGIN, 
            pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2, 
			PathName, ROP_SRC);
        break;
    }

#else // INTERFACE_MONO

    int x, y;
    RECT rect;
    COLORREF crBkOld, crTextOld;

	x = pRect->left;

    if (nBitsPixel == 1)
    {
        x += 1;
        y = pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2;
    }
    else if (nBitsPixel == 2)
    {
        x += 2;
        y = pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2;
    }
    else
    {
        x += 2;
        y = pRect->top + (pRect->bottom - pRect->top - STBCHKBMPHEIGHT) / 2;
    }

    switch (pButtonData->wCheckState)
    {
    case BST_CHECKED :
        // Draw icon for application
        if (nBitsPixel == 1 || nBitsPixel == 2)
        {
            crBkOld = SetBkColor(hdc, COLOR_WHITE);
            crTextOld = SetTextColor(hdc, COLOR_BLACK);
            BitBlt(hdc, x, y, CHKBMPWIDTH, CHKBMPHEIGHT, 
                (HDC)hBmpChecked, 0, 0, ROP_SRC);
        }
        else
        {
            crBkOld = SetBkColor(hdc, STBBGCOLOR);
            crTextOld = SetTextColor(hdc, COLOR_RED);
            BitBlt(hdc, x, y, STBCHKBMPWIDTH, STBCHKBMPHEIGHT, 
                (HDC)hSTBBmpChecked, 0, 0, ROP_SRC);
        }

        SetBkColor(hdc, crBkOld);
        SetTextColor(hdc, crTextOld);
        
        break;
        
    default :

        if (nBitsPixel == 1 || nBitsPixel == 2)
        {
            SetRect(&rect, x, y, x + CHKBMPWIDTH, y + CHKBMPHEIGHT);
            ClearRect(hdc, &rect, COLOR_WHITE);
        }
        else
        {
            SetRect(&rect, x, y, x + STBCHKBMPWIDTH, y + STBCHKBMPHEIGHT);
            ClearRect(hdc, &rect, STBBGCOLOR);
        }

        break;
    }
#endif // INTERFACE_MONO
}
	
static void CHECKBOX_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                           PSTR pszText, int nBitsPixel, RECT* pRect)
{
    RECT rect;

#if (INTERFACE_MONO)

    rect.left = pRect->left;
    rect.top = pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2;
    rect.right = rect.left + CHKBMPWIDTH;
    rect.bottom = rect.top + CHKBMPHEIGHT;

    if (pButtonData->bFocus)
    {
        DrawFocus(hdc, pRect);
    }
    else
    {
        ClearRect(hdc, pRect, COLOR_TRANSBK);
    }

#else // INTERFACE_MONO

    HPEN hOldPen;

    rect.left = pRect->left;
    if (nBitsPixel == 1 || nBitsPixel == 2)
    {
        rect.top = pRect->top + (pRect->bottom - pRect->top - CHKBMPHEIGHT) / 2;
        rect.right = rect.left + CHKBMPWIDTH;
        rect.bottom = rect.top + CHKBMPHEIGHT;
    }
    else
    {
        rect.top = pRect->top + (pRect->bottom - pRect->top - STBCHKBMPHEIGHT) / 2;
        rect.right = rect.left + STBCHKBMPWIDTH;
        rect.bottom = rect.top + STBCHKBMPHEIGHT;
    }

    if (nBitsPixel == 1)//is mono.
    {
        hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
        InflateRect(&rect, 0, 1);
        rect.right += 2;
        DrawRect(hdc, &rect);
        SelectObject(hdc, hOldPen);
    }
    else
    {
        InflateRect(&rect, 0, 2);
        rect.right += 4;
        DrawCHKIconFrame(hdc, &rect, nBitsPixel);
    }
#endif // INTERFACE_MONO

    CHKIcon_Paint(hWnd, hdc, pButtonData, nBitsPixel, pRect);
    PaintTheRightPart(hWnd, hdc, pButtonData, pRect, pszText);
}

static void DrawFocus(HDC hdc, RECT* pRect)
{
//#if (!NOFOCUSRECT)
#if (INTERFACE_MONO)
    BITMAP bmp;

    GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
    StretchBlt(hdc, pRect->left, pRect->top, pRect->right - pRect->left,
        pRect->bottom - pRect->top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
        bmp.bmHeight, ROP_SRC);

#else
    HBRUSH  hBrush;
    int     fnOldMode;
    COLORREF crBkOld;

    crBkOld = SetBkColor(hdc, COLOR_BLACK);

    fnOldMode = SetROP2(hdc, ROP_SRC_XOR_DST);
    
    hBrush = CreateHatchBrush(HS_SIEVE, COLOR_WHITE);//GetBkColor(hdc));
    FrameRect(hdc, pRect, hBrush);

    DeleteObject(hBrush);
    
    SetROP2(hdc, fnOldMode);
    SetBkColor(hdc, crBkOld);
#endif
//#endif // NOFOCUSRECT
}

static void BUTTON_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                         PSTR pszText, BOOL bDown, int nBitsPixel, RECT* pRect)
{
    switch (pButtonData->byStyle)
    {
        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
        
            CHECKBOX_Paint(hWnd, hdc, pButtonData, pszText, nBitsPixel, pRect);
            break;
        
        case BS_PUSHBUTTON:
        case BS_DEFPUSHBUTTON:
        
            PUSHBUTTON_Paint(hWnd, hdc, pButtonData, pszText, bDown, nBitsPixel, pRect);
            break;
        
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
        
            RADIOBUTTON_Paint(hWnd, hdc, pButtonData, pszText, nBitsPixel, pRect);
            break;
        
        case BS_GROUPBOX:
        
            GROUP_Paint(hdc, nBitsPixel, pszText, pRect);
        
        default :
            break;
    }
}

static void BITMAP_Paint(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                         RECT* pRect, BOOL bDown)
{
    HBITMAP hImage = NULL;
    BITMAP bitmap;
    RECT   rcOld;

    if (!IsWindowEnabled(hWnd) && pButtonData->hImageDisable)
        hImage = pButtonData->hImageDisable;
    if (pButtonData->bFocus && pButtonData->hImageFocus)
        hImage = pButtonData->hImageFocus;
    if (bDown && pButtonData->hImagePushDown)
        hImage = pButtonData->hImagePushDown;    

    if (!hImage)
        hImage = pButtonData->hImageNormal;

    GetObject((HBITMAP)hImage, sizeof(BITMAP), &bitmap);

    if (pButtonData->byStyle == BS_PUSHBUTTON || 
        pButtonData->byStyle == BS_DEFPUSHBUTTON)
    {
        SetRect(&rcOld, pRect->left, pRect->top, pRect->right, pRect->bottom);
#if 0

        pRect->top += (pRect->bottom - pRect->top - bitmap.bmHeight) / 2;
        pRect->left += (pRect->right - pRect->left - bitmap.bmWidth) / 2;
        pRect->bottom = pRect->top + bitmap.bmHeight;
        pRect->right = pRect->left + bitmap.bmWidth;
#endif
        pRect->top += FOCUSRECTWIDTH / 2;
        pRect->left += FOCUSRECTWIDTH / 2;
        pRect->bottom -= FOCUSRECTWIDTH / 2;
        pRect->right -= FOCUSRECTWIDTH / 2;

    }

#if (INTERFACE_MONO)

    
    // 对于PushButton，在没有指定PushDown位图时，通过反显表示button按下状态
    if ((pButtonData->byStyle == BS_PUSHBUTTON || 
        pButtonData->byStyle == BS_DEFPUSHBUTTON) && 
        bDown && hImage != pButtonData->hImagePushDown)
    {
        /*BitBlt(hdc, pRect->left, pRect->top,
            pRect->right - pRect->left, 
            pRect->bottom - pRect->top, (HDC)hImage, 0, 0, ROP_NSRC);*/
        StretchBlt(hdc, pRect->left, pRect->top,
            pRect->right - pRect->left, 
            pRect->bottom - pRect->top, (HDC)hImage, 0, 0, 
            bitmap.bmWidth, bitmap.bmHeight, ROP_NSRC);
    }
    else
    {
        /*BitBlt(hdc, pRect->left, pRect->top,
            pRect->right - pRect->left, 
            pRect->bottom - pRect->top, (HDC)hImage, 0, 0, ROP_SRC);*/
        StretchBlt(hdc, pRect->left, pRect->top,
            pRect->right - pRect->left, 
            pRect->bottom - pRect->top, (HDC)hImage, 0, 0, 
            bitmap.bmWidth, bitmap.bmHeight, ROP_SRC);
    }

#else // INTERFACE_MONO

    BitBlt(hdc, pRect->left, pRect->top,
        pRect->right - pRect->left, 
        pRect->bottom - pRect->top, (HDC)hImage, 0, 0, ROP_SRC);

#endif // INTERFACE_MONO

    if (pButtonData->byStyle == BS_PUSHBUTTON || 
        pButtonData->byStyle == BS_DEFPUSHBUTTON)
    {
        SetRect(pRect, rcOld.left, rcOld.top, rcOld.right, rcOld.bottom);
    }
}

static void GetTextAndDrawFocus(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData, 
                                RECT* pRect)
{
    RECT    rect1;
    BITMAP  bitmap;

    return;
    rect1.left = pRect->left + MAXBMPWIDTH + TEXT_GAP;
    
    if (!pButtonData->hImageNormal)
    {
        char    str[SINGLE_DEFLENGTH];
        int     nCount;
        SIZE    size;

        nCount = DefWindowProc(hWnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
        
        if (nCount > SINGLE_DEFLENGTH - 1)
            nCount = SINGLE_DEFLENGTH - 1;

        DefWindowProc(hWnd, WM_GETTEXT, (WPARAM)(nCount + 1),
            (LPARAM)str);

        if (str)
            GetTextExtentPoint32(hdc, str, nCount, &size);

        rect1.top = (pRect->bottom - pRect->top - size.cy) / 2;
        rect1.right = rect1.left + size.cx;
        rect1.bottom = rect1.top + size.cy;
        
        InflateRect(&rect1, 1, 1);

        if (rect1.left < pRect->left)
            rect1.left = pRect->left;

        if (rect1.top < pRect->top)
            rect1.top = pRect->top;

        if (rect1.right > pRect->right)
            rect1.right = pRect->right;

        if (rect1.bottom > pRect->bottom)
            rect1.bottom = pRect->bottom;
    }
    else
    {
        GetObject((HGDIOBJ)pButtonData->hImageNormal, sizeof(BITMAP), &bitmap);
        rect1.top = pRect->top;
        rect1.right = pRect->right;
        rect1.bottom = pRect->bottom;
    }
    
    DrawFocus(hdc, &rect1);
}

static void GetTextAndPaintButton(HWND hWnd, HDC hdc, PBUTTONDATA pButtonData,
                               BOOL bDown, int nBitsPixel, RECT* pRect)
{
    char    str[SINGLE_DEFLENGTH];
	int		nCount;

    //if (!pButtonData->hImageNormal)
    {
        nCount = DefWindowProc(hWnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);

        if (nCount > SINGLE_DEFLENGTH - 1)
            nCount = SINGLE_DEFLENGTH;

        DefWindowProc(hWnd, WM_GETTEXT, (WPARAM)(nCount + 1),
            (LPARAM)str);
    }
    //else
      //  str[0] = 0;
    
	BUTTON_Paint(hWnd, hdc, pButtonData, str, bDown, nBitsPixel, pRect);
}    

//Only nBitsPixel isn't 1, can call Draw3DRectFrame.

static HBRUSH SetCtlColorButton(HWND hWnd, HDC hdc)
{
    HBRUSH  hBrush;

#if (!NOSENDCTLCOLOR)
    HWND    hWndParent;
    DWORD   dwStyle;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hWndParent = GetParent(hWnd);
    else
        hWndParent = GetWindow(hWnd, GW_OWNER);
    
    if (!hWndParent)
    {
#endif
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SetTextColor(hdc, RGB(0, 0, 0));
        
        hBrush = (HBRUSH)(COLOR_BTNFACE + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(hWndParent, (WORD)WM_CTLCOLORBTN, 
            (WPARAM)hdc, (LPARAM)hWnd);
    }
#endif

    return hBrush;
}

/*************************************************************************
Function   :CreateEdgedTextBitmap
--------------------------------------------------------------------------
Description: Create bitmap with edged text
--------------------------------------------------------------------------
Input      : hdc             should be a 32bit device context
             szText          text
             clrText         text color
             clrEdge         Edge color
             clrTransparent  Transparent color
             dwDrawFlag      flag for DrawText
             width           drawing region width
             height          drawing region height
--------------------------------------------------------------------------
Return     : HBITMAP        generated edged-text bitmap handle.
--------------------------------------------------------------------------
IMPORTANT NOTES:
    Only support 32bit bitmap
*************************************************************************/
static HBITMAP CreateEdgedTextBitmap( HDC hdc, char* szText, COLORREF clrText, COLORREF clrEdge, COLORREF clrTransparent, DWORD dwDrawFlag, int width, int height)
{
    HBITMAP nbmp=NULL;
    HBITMAP bmp;
    HBITMAP hbmp;
    WORD *  p, *p1, *p2 = NULL;
//    WORD *  pp;
//    WORD *  pl;
//    HBRUSH  brush;
    BITMAP  bm;
    RECT    rect;
    HDC     dc;
    int     i, j, k, d[8];
    int     size;
    int     pitch;
    WORD    wClrText, wClrEdge;

    if ((width % 2) == 1)
        width ++;
    SetRect(&rect,0,0,width,height);

    dc = CreateCompatibleDC(hdc);
    if(dc == NULL)
        goto exit2;

    bmp = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
    if(bmp == NULL)
        goto exit3;

    hbmp = (HBITMAP)SelectObject(dc,bmp);
    //brush = CreateSolidBrush(clrTransparent);
    //if(brush == NULL)
        //goto exit4;

    //FillRect(dc, &rect, brush);
    ClearRect(dc, &rect, clrTransparent);
    SetTextColor(dc, clrText);
    SetBkMode(dc, TRANSPARENT);    
    DrawText(dc, szText, -1, &rect, dwDrawFlag);
    SelectObject(dc, hbmp);

    GetObject(bmp, sizeof(bm), &bm);

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Genghis[11/10/2003 0:48] Only support 32bit bitmap
    if(bm.bmBitsPixel!=16)
        goto exit5;

    pitch = bm.bmWidth;
    size  = pitch*bm.bmHeight*2;

    p = (WORD *)LocalAlloc(LMEM_FIXED, size);
    if(p == NULL)
        goto exit5;

    memcpy(p,bm.bmBits,size);

#define FIXCOLOR(x, y) y=(((GetRValue(x)>>3)<<11) | ((GetGValue(x)>>2)<<5) | GetBValue(x)>>3)
    wClrText = FIXCOLOR(clrText, wClrText);
    wClrEdge = FIXCOLOR(clrEdge, wClrEdge);

//    for(i=1, pl=p+pitch; i<bm.bmHeight-1; i++, pl+=pitch)
//    {
//        for(j=1, pp=pl+1; j<bm.bmWidth-1; j++, pp++)
//        {
//            if(pp[0]==wClrText)
//                continue;
//
//            if(pp[1]==wClrText || pp[-1]==wClrText || pp[-pitch]==wClrText || pp[-pitch-1]==wClrText || pp[-pitch+1]==wClrText || pp[pitch-1]==wClrText || pp[pitch]==wClrText || pp[pitch+1]==wClrText)
//                pp[0]=wClrEdge;
//        }
//    }
//    
//    for(j=1, pp = p + 1; j < bm.bmWidth - 1; j++, pp++)
//    {
//        if(pp[0]==wClrText)
//            continue;
//
//        if(pp[1]==wClrText || pp[-1]==wClrText || pp[pitch-1]==wClrText || pp[pitch]==wClrText || pp[pitch+1]==wClrText)
//            pp[0]=wClrEdge;
//    }
//
//    for(j=1, pp=p+(bm.bmHeight-1)*pitch+1; j<bm.bmWidth-1; j++, pp++)
//    {
//        if(pp[0]==wClrText)
//            continue;
//
//        if(pp[1]==wClrText || pp[-1]==wClrText || pp[-pitch]==wClrText || pp[-pitch-1]==wClrText || pp[-pitch+1]==wClrText )
//            pp[0]=wClrEdge;
//    }
//
//    for(i=1, pp=p+pitch; i<bm.bmHeight-1; i++, pp+=pitch)
//    {
//        if(pp[0]==wClrText)
//            continue;
//
//        if(pp[1]==wClrText || pp[-pitch]==wClrText || pp[-pitch+1]==wClrText || pp[pitch]==wClrText || pp[pitch+1]==wClrText)
//            pp[0]=wClrEdge;
//    }
//
//    for(i=1, pp=p+pitch+(bm.bmWidth - 1); i<bm.bmHeight-1; i++, pp+=pitch)
//    {
//        if(pp[0]==wClrText)
//            continue;
//
//        if(pp[-1]==wClrText || pp[-pitch]==wClrText || pp[-pitch-1]==wClrText || pp[pitch-1]==wClrText || pp[pitch]==wClrText )
//            pp[0]=wClrEdge;
//    }
//
//    pp=p;
//    if(pp[1]==wClrText || pp[pitch]==wClrText || pp[pitch+1]==wClrText)
//        pp[0]=wClrEdge;
//
//    pp=p+(bm.bmWidth-1);
//    if(pp[-1]==wClrText || pp[pitch-1]==wClrText || pp[pitch]==wClrText)
//        pp[0]=wClrEdge;
//
//    pp=p+(bm.bmHeight-1)*pitch;
//    if(pp[1]==wClrText || pp[-pitch]==wClrText || pp[-pitch+1]==wClrText)
//        pp[0]=wClrEdge;
//
//    pp=p+(bm.bmHeight-1)*pitch+(bm.bmWidth-1);
//    if(pp[-1]==wClrText || pp[-pitch]==wClrText || pp[-pitch-1]==wClrText)
//        pp[0]=wClrEdge;

//      0   1   2
//      3  p1   4
//      5   6   7
    for (i = 0; i < bm.bmHeight; i++)
    {
        for (j = 0; j < pitch; j++)
        {
            for (k = 0; k < 8; k++)
                d[k] = 1;
            
            p1 = p + (i * pitch) + j;
            if (*p1 == clrText)
                continue;
            if (j == 0)
                d[0] = d[3] = d[5] = 0;
            if (j == bm.bmWidth - 1)
                d[2] = d[4] = d[7] = 0;
            if (i == 0)
                d[0] = d[1] = d[2] = 0;
            if (i== bm.bmHeight - 1)
                d[5] = d[6] = d[7] = 0;
            for (k = 0; k < 8; k++)
            {
                if (d[k])
                {
                    switch(k)
                    {
                    case 0:
                        p2 = p1 - pitch - 1;
                        break;
                    case 1:
                        p2 = p1 - pitch;
                        break;
                    case 2:
                        p2 = p1 -pitch + 1;
                        break;
                    case 3:
                        p2 = p1 - 1;
                        break;
                    case 4:
                        p2 = p1 + 1;
                        break;
                    case 5:
                        p2 = p1 + pitch - 1;
                        break;
                    case 6:
                        p2 = p1 + pitch;
                        break;
                    case 7:
                        p2 = p1 + pitch + 1;
                        break;
                    }
                    if (*p2 == clrText)
                    {
                        *p1 = clrEdge;
                        continue;
                    }
                }
                
            }//for(k=0~8)
        }//for(j=0~pitch)
    }//for(i=0~bm.bmHeight)

    nbmp = CreateBitmap(bm.bmWidth,bm.bmHeight,bm.bmPlanes,bm.bmBitsPixel,p);

    LocalFree((HANDLE)p);
exit5:
    //DeleteObject(brush);
//exit4:
    DeleteObject(bmp);
exit3:
    DeleteDC(dc);
exit2:

    return nbmp;
}
