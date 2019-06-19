/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for window system object management.
 *            
\**************************************************************************/

#ifndef __WSOBJ_H
#define __WSOBJ_H

/* Defines window object type constant  */

/* NULL Object  */

#define OBJ_NULL    0       /* NULL Object          */
#define OBJ_ANY     0xFFFFFFFF

/* GDI Object   */

#define OBJ_PEN     1       /* Pen Object           */
#define OBJ_BRUSH   2       /* Brush Object         */
#define OBJ_BITMAP  3       /* Bitmap Object        */
#define OBJ_FONT    4       /* Font Object          */
#define OBJ_PALETTE 5       /* Palette Object       */
#define OBJ_RGN     6       /* Region Object        */
#define OBJ_DC      7       /* DC Object            */

#define OBJ_XGDI_MIN    1   /* Min GDI Object need reference */
#define OBJ_XGDI_MAX    6   /* Min GDI Object need reference */

#define OBJ_GDI_MIN 1       /* Minimum GDI Object   */
#define OBJ_GDI_MAX 7       /* Maximum GDI Object   */

/* User Object  */

#define OBJ_WINDOW  8       /* Window Object        */

#define OBJ_CURSOR  9       /* Cursor Object        */
#define OBJ_ICON    10      /* Icon Object          */
#define OBJ_MENU    11      /* Menu Object          */

#define OBJ_TIMER   12      /* Timer Object         */
#define OBJ_MIN     1       /* Minimum object type  */
#define OBJ_MAX     12      /* Maxinum object type  */

#define HWND_ROOT   ((HWND)0x00800000)

typedef struct 
{
    HANDLE handle;
}OBJHDR, *POBJHDR;

/* Struct for window object */
typedef struct
{
    OBJHDR objhead;
} WSOBJ, *PWSOBJ, GDIOBJ, *PGDIOBJ;

typedef struct
{
    OBJHDR  objhead;
    WORD    refcount;       /* the used reference count */
    BYTE    refselect;      /* the selected reference count */
    BYTE    bDeleted;       /* the deleted flag */
}XGDIOBJ, *PXGDIOBJ, XGDIOBJHDR, *PXGDIOBJHDR;

#define ISDELETEDXGDIOBJ(p)     \
((((PXGDIOBJ)(p))->bDeleted      ) && \
 (((PXGDIOBJ)(p))->refcount == 0 ) && \
 (((PXGDIOBJ)(p))->refselect == 0))

/* Default bitmap for bitmap dc */
#define DEFAULT_BITMAP  -1

/* Functions to mange window object table */

void    WOT_Init(void);
void    WOT_Exit(void);

BOOL    WOT_RegisterObj(PWSOBJ pObj, UINT uType, BOOL bShared);
void    WOT_UnregisterObj(PWSOBJ pObj);

PWSOBJ  WOT_GetObj(HANDLE handle, UINT uType);
HANDLE  WOT_GetHandle(PWSOBJ pObj);

UINT    WOT_GetObjType(PWSOBJ pObj);

PXGDIOBJ    WOT_GetStockObj(int nIndex);
BOOL        WOT_IsStockObj(PXGDIOBJ pObj);
int         WOT_GetStockObjIndex(PXGDIOBJ pObj);

PWSOBJ      WOT_GetStockCursor(DWORD nIndex);

PXGDIOBJ    WOT_LockObj(HANDLE handle, UINT uType);
PXGDIOBJ    WOT_UnlockObj(PXGDIOBJ pObj);

void        WOT_SelectObj(PXGDIOBJ pObj);
PXGDIOBJ    WOT_UnselectObj(PXGDIOBJ pObj);
int         WOT_GetSelectCount(PXGDIOBJ pObj);

BOOL        WOT_DelectObj(PXGDIOBJ pObj);

#if (__MP_PLX_GUI)
BOOL        WOT_IsSharedObj(PXGDIOBJ pObj);
void        WOT_SetObjProcessId(PGDIOBJ pObj, DWORD dwProcessId);

#endif
/*
BOOL    WOT_AddObj(PWSOBJ pObj, UINT uType);
void    WOT_RemoveObj(PWSOBJ pObj);

PWSOBJ  WOT_GetObj(HANDLE handle, UINT uType);
PGDIOBJ WOT_GetGDIObj(HANDLE handle);

UINT    WOT_GetObjType(PWSOBJ pObj);

PGDIOBJ WOT_GetStockObj(int nIndex);
void*   WOT_GetStockPhysObj(PWSOBJ pObj);
BOOL    WOT_IsStockObj(PWSOBJ pObj);

int    WOT_GetStockObjIndex(PWSOBJ pObj);

PWSOBJ  WOT_GetStockCursor(DWORD nIndex);

PXGDIOBJ    WOT_LockObj(HANDLE handle, UINT uType);
int         WOT_UnlockObj(PXGDIOBJ pObj, UINT uType);
*/

#endif  /* __WSOBJ_H    */
