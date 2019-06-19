/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for region object
 *            
\**************************************************************************/

#ifndef __WSORGN_H
#define __WSORGN_H

typedef struct _RGNDATA { /* rgnd */
    DWORD dwSize; 
    DWORD iType; 
    DWORD nCount; 
	DWORD nTotalCount;
    DWORD nRgnSize; 
    RECT  rcBound; 
} RGNDATA, *PRGNDATA, *LPRGNDATA;

typedef struct tagRGNOBJ
{               
    HANDLE      handle;     /* handle of region object */
    WORD        refcount;   /* the used reference count */
    BYTE        bDeleted;   /* the deleted flag */
    RGNDATA     rgndata;    
    char  Buffer[1];
} RGNOBJ, *PRGNOBJ;

PRGNOBJ     RGN_Create( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect );
BOOL        RGN_Destroy(PRGNOBJ pRgn);
int RGN_Combine( PRGNOBJ pRgnDest, PRGNOBJ pRgnSrc1, PRGNOBJ pRgnSrc2, int fnCombineMode );
int RGN_Offset( PRGNOBJ pRgn, int nXOffset, int nYOffset );

#endif //__WSORGN_H
