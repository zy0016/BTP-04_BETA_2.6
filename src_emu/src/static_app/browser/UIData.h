#include "pwbe.h"

typedef struct UIData
{
	HWND hwnd;
	HDC  hdc;
	WB_RECT rc;
}UIDATA, *PUIDATA;


int  WB_BeginDisplay(void* pUIData, WB_RECT *lprc);
int  WB_EndDisplay(void* pUIData);

int     WB_DrawLine    (void* pUIData, int x1, int y1, int x2, int y2, int style, 
                        unsigned long color);
int		WB_Rectangle(void* pUIData, WB_RECT *lprc, unsigned long color, int bFill);
int		WB_Polygon  (void* pUIData, const WB_POINT *lpPoints, int nCount, 
				     unsigned long color, int penStlye, int bFill);

int		WB_Ellipse(void* pUIData, WB_RECT *lprc, unsigned long color, int bFill);

//int WB_SetTextStyle( void* pUIData, int option, void* pnewStyle, void* poldStyle);
int WB_SetTextStyle( void* pUIData, WB_TEXTSTYLEOPTION option, void* pnewStyle, void* poldStyle);
int				WB_DrawText(void* pDev, WB_RECT *lprc, 
								char* pText, int len, unsigned long fgcolor, unsigned long bkcolor);
//int           WB_GetTextInfo(void* pParent, WB_FONT * pWBFont, char* pStr, int len, int* w, int* h);
int           WB_GetTextInfo(void* pUIData, WB_FONT * pWBFont, char* pStr, int len, int* w, int* h);


//int WB_DrawImageEx(void* pDev, 
int	WB_DrawImageEx(void* pUIData, 				   
				   int x, int y, int width, int height, 
				   int src_x, int src_y, int src_width, int src_height,
				   int imgType, 
				   char _WBHUGE_ *pData, long len, 
				   void** hBitMap, 
				   void* Param);
int WB_EndImageEx(void* hBitMap, int imgType, void* param);
int WB_GetImageInfo(int imagetype, char _WBHUGE_ *pData, int len, int* w, int* h);
int WB_InputMethod(WB_EDITTYPE iEditType , char *pBuffer, int iMaxCharLen, int iBufferLen, char *InputFormat, ECHO_FINISHEDITTEXT pFinishEditProc);
int WB_EndImageEx(void* hBitMap, int imgType, void* param);
int WB_GetImageInfo(int imagetype, char _WBHUGE_ *pData, int len, int* w, int* h);

int WIE_WapSelectOption(int type, WB_OPTIONDATA seloptions[], int iItemNum, ECHO_FINISHSELECT FinishSelect);



