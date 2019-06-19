
#include "window.h"
#include "hpimage.h"
#include "super5.h"

/*
#define CHESS_BMP_SIZE	11//13
#define BOARD_BMP_SIZE	165//240
#define	X_CHESS_OFFSET	0
#define	Y_CHESS_OFFSET	0
*/
//static HBITMAP hBmpBoard, hBmpChess;
//static int xBoard, yBoard;

void PaintInit(HWND hWnd, int x, int y)
{
//#if defined SCP_NOKEYBOARD
	{
		HDC hdc;
		COLORREF Color;
		BOOL	bTran;
		
		hdc = GetDC(hWnd);
		hBmpBoard = CreateBitmapFromImageFile(hdc,PATH_GOBANGBOARD,&Color,&bTran);
		hBmpChess = CreateBitmapFromImageFile(hdc,PATH_GOBANGCHESS,&Color,&bTran);
		ReleaseDC(hWnd,hdc);
	}
//#endif
	//hBmpBoard = CreateBitmap(BOARD_BMP_SIZE, BOARD_BMP_SIZE, 1, 2, boardBmp);
	//hBmpBoard = LoadImage(NULL,PATH_GOBANGBOARD, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//hBmpChess = CreateBitmap(CHESS_BMP_SIZE*2, CHESS_BMP_SIZE, 1, 2, chessBmp);
	//hBmpChess = LoadImage(NULL,PATH_GOBANGCHESS, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	xBoard = x;
	yBoard = y;
}

void PaintBoard(HDC hdc)
{
	int x, y, c;

	BitBlt(hdc, X_BACK, Y_BACK, BACK_BMP_WIDTH, BACK_BMP_HEIGHT, (HDC)hBmpBoard, 0, 0, SRCCOPY);
	for(x=0;x<BOARD_SIZE;x++)
		for(y=0;y<BOARD_SIZE;y++)
		{
			c = Game5_Color(x,y);
			if(c!=empty)
				PaintChess(hdc, x, y, c);
		}

}

void PaintChess(HDC hdc, int x, int y, int color)
{
	COLORREF OldColor;
	int		OldStyle;

	if (-1 == x || -1 == y)
		return ;

	OldColor = SetBkColor(hdc,RGB(0,0,255));
	OldStyle = SetBkMode(hdc,NEWTRANSPARENT);
	BitBlt(hdc,  x*(BOARD_BMP_SIZE/BOARD_SIZE)+ xBoard+ X_CHESS_OFFSET,  y*(BOARD_BMP_SIZE/BOARD_SIZE)+yBoard + Y_CHESS_OFFSET, CHESS_BMP_SIZE, CHESS_BMP_SIZE, (HDC)hBmpChess, color?0:CHESS_BMP_SIZE, 0, SRCCOPY);
	SetBkColor(hdc,OldColor);
	SetBkMode(hdc,OldStyle);

	return ;
}

void UnPaintChess(HDC hdc, int x, int y)
{
	if (-1 == x || -1 == y)
		return ;

	BitBlt(hdc,  
		x*(BOARD_BMP_SIZE/BOARD_SIZE) + xBoard + X_CHESS_OFFSET,  
		y*(BOARD_BMP_SIZE/BOARD_SIZE) + yBoard + Y_CHESS_OFFSET, 
		CHESS_BMP_SIZE, CHESS_BMP_SIZE, 
		(HDC)hBmpBoard, 
		x*(BOARD_BMP_SIZE/BOARD_SIZE) + xBoard + X_CHESS_OFFSET,  
		y*(BOARD_BMP_SIZE/BOARD_SIZE) + yBoard + Y_CHESS_OFFSET,
		SRCCOPY);

	return ;
}

void PaintDestroy()
{
	if (hBmpBoard)
	{
		DeleteObject(hBmpBoard);
		hBmpBoard = NULL;
	}
	
	if (hBmpChess)
	{
		DeleteObject(hBmpChess);
		hBmpChess = NULL;
	}

	return ;
}

int Board_HitTest(int x, int y, int* col, int* row)
{
	x-=xBoard;
	y-=yBoard;
	if(x<0 || y<0 || x>=BOARD_BMP_SIZE || y>=BOARD_BMP_SIZE)
		return 0;
	
	*col = (x-X_CHESS_OFFSET)/(BOARD_BMP_SIZE/BOARD_SIZE);
	*row = (y-Y_CHESS_OFFSET)/(BOARD_BMP_SIZE/BOARD_SIZE);

	return 1;
}
