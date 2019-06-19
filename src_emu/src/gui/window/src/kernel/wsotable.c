/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Window system object table management module.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "string.h"

#include "wsobj.h"

#include  "wsodc.h"
/* Header files used to define stock object */
#include "wsopen.h"
#include "wsobrush.h"
#include "wsobmp.h"
#include "wsofont.h"

/* Header files used to define cursor object */
#include "wsocur.h"

/*  Header files for using g_pRootWin */
#include "wsownd.h"
#include "wsthread.h"

/**************************************************************************/
/*              Definition for some useful macro                          */ 
/**************************************************************************/

#define MASK_INDEX          0x0000FFFF  /* index mask in handle           */
#define MASK_SERIAL         0x000F0000  /* serial mask in handle          */
#define MASK_TYPE           0x00F00000  /* type mask in handle            */
#define MASK_HANDLE         0x00FFFFFF  /* handle mask in input handle    */ 

#define MAX_SERIAL          15          /* Maximum serial no              */

#define SERIAL_SHIFT_BITS   16          /* shift bits of serial in handle */
#define TYPE_SHIFT_BITS     20          /* shift bits of type in handle   */

/* Get index, serial & type from handle */
#define INDEXOFHANDLE(h)  ((DWORD)(h) & MASK_INDEX)
#define SERIALOFHANDLE(h) (((DWORD)(h) & MASK_SERIAL) >> SERIAL_SHIFT_BITS)
#define TYPEOFHANDLE(h)   (((DWORD)(h) & MASK_TYPE) >> TYPE_SHIFT_BITS)

/* Make handle using specified index, serial & type */
#define MAKEHANDLE(index, serial, type)     \
    (HANDLE)(index | ((long)serial << SERIAL_SHIFT_BITS) | \
    ((long)type << TYPE_SHIFT_BITS))

#define ISVALIDTYPE(type)   ((type) >= OBJ_MIN && (type) <= OBJ_MAX)
#define ISGDITYPE(type)     ((type) >= OBJ_GDI_MIN && (type) <= OBJ_GDI_MAX)
#define ISXGDITYPE(type)    ((type) >= OBJ_XGDI_MIN && (type) <= OBJ_XGDI_MAX)
#define ISSTOCKHANDLE(h)    (((DWORD)(h) & MASK_SERIAL) == 0)
#define ISTIMERTYPE(type)   ((type) == OBJ_TIMER)
#define STOCKOBJ_SERIAL      0
#define SHAREDOBJ_SERIAL     1
#define MIN_DYNAMAIC_SERIAL  2
#define MAX_DYNAMAIC_SERIAL  15

#if (!HANDLEISPTR)
/**************************************************************************/
/*              Definition for DynamicObj                                 */ 
/**************************************************************************/
typedef struct
{
    UINT    uSerial;
#if (__MP_PLX_GUI)
    DWORD   dwProcessId;
#endif
    PWSOBJ  pObj;
} DYNAMICOBJ;

/**************************************************************************/
/*              Definition for GDI Object Table                           */ 
/**************************************************************************/

#define MAX_GDI_MASKS   (MAX_GDI_OBJS / 32)

static DWORD       dwGDIBitMask[MAX_GDI_MASKS];
static DYNAMICOBJ  GDIObjTable[MAX_GDI_OBJS];

/**************************************************************************/
/*              Definition for User Object Table                          */ 
/**************************************************************************/

#define MAX_USER_MASKS   (MAX_USER_OBJS / 32)

static DWORD       dwUserBitMask[MAX_USER_MASKS];
static DYNAMICOBJ  UserObjTable[MAX_USER_OBJS];

#if(__MP_PLX_GUI)
/**************************************************************************/
/*              Definition for system shared GDI object table             */
/**************************************************************************/
#define MAX_SHARED_OBJS 32
#define MAX_SHARED_MASKS   (MAX_SHARED_OBJS / 32)

typedef struct
{
    PGDIOBJ pObj;       // gdi object pointer
} SHAREDOBJ;

static DWORD       dwSharedBitMask[MAX_SHARED_MASKS];
static SHAREDOBJ   SharedObjTable[MAX_SHARED_OBJS];
#endif

#endif //HANDLEISPTR

/**************************************************************************/
/*              Definition for stock GDI object table                     */ 
/**************************************************************************/

/* include file in which stock object define */
#include "stockobj.h"

typedef struct
{
    PGDIOBJ pObj;       // gdi object pointer
//    void*   pPhysObj;   // physical object of gdi object
} STATICOBJ;

#define MAX_STOCK_OBJS 16

static STATICOBJ StockObjTable[MAX_STOCK_OBJS] = 
{
    {(PGDIOBJ)&NullPen}, 
    {(PGDIOBJ)&BlackPen}, 
    {(PGDIOBJ)&DKGrayPen}, 
    {(PGDIOBJ)&GrayPen}, 
    {(PGDIOBJ)&LTGrayPen}, 
    {(PGDIOBJ)&WTGrayPen}, 
    {(PGDIOBJ)&WhitePen}, 
    {(PGDIOBJ)&NullBrush}, 
    {(PGDIOBJ)&BlackBrush}, 
    {(PGDIOBJ)&DKGrayBrush}, 
    {(PGDIOBJ)&GrayBrush}, 
    {(PGDIOBJ)&LTGrayBrush}, 
    {(PGDIOBJ)&WTGrayBrush}, 
    {(PGDIOBJ)&WhiteBrush}, 
    {(PGDIOBJ)&SystemFont}, 
    {(PGDIOBJ)&DefaultBitmap}
};
/**************************************************************************/
/*              Definition for predefine cursor object table              */ 
/**************************************************************************/

#if (!NOCURSOR)
#include "curobj.h"
#endif // NOCURSOR

/**************************************************************************/
/*              Definition for Object Management Debug                    */ 
/**************************************************************************/

#ifdef WINDEBUG

#include "windebug.h"

static WINDEBUGINFO ObjDebugInfo;

#endif // WINDEBUG

static void DeleteObj(PXGDIOBJ pObj);

#ifdef WINDEBUG
void WINDEBUG_AddObj(int uType);
void WINDEBUG_RemoveObj(int uType);
#endif
/*
**  Function : WOT_Init
**  Purpose  :
**      Initializes the window system object table.
*/
void WOT_Init(void)
{
    int i;

#if (!NOCURSOR)

    int nRet;
    PGRAPHDEV pGraphDev;

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    ASSERT(pGraphDev != NULL);

    // Initializes the stock cursor object
    for (i = 0; i < sizeof(CursorObjTable) / sizeof(CURSOROBJ); i++)
    {
        nRet = pGraphDev->drv.RealizeCursor(g_pDisplayDev, 
            &CursorObjTable[i].logcursor, NULL);
        ASSERT(nRet > 0);
        
        CursorObjTable[i].pPhysCursor = (PPHYSCURSOR)MemAlloc(nRet);
        ASSERT(CursorObjTable[i].pPhysCursor != NULL);
        
        nRet = pGraphDev->drv.RealizeCursor(g_pDisplayDev, 
            &CursorObjTable[i].logcursor, CursorObjTable[i].pPhysCursor);
        ASSERT(nRet > 0);
    }

#endif

#if (!HANDLEISPTR)
    // Initialize the GDI object table
    for (i = 0; i < MAX_GDI_OBJS; i++)
    {
        GDIObjTable[i].uSerial = MIN_DYNAMAIC_SERIAL;
        GDIObjTable[i].pObj = NULL;
#if (__MP_PLX_GUI)
        GDIObjTable[i].dwProcessId = INVALID_PROCESSID;
#endif
    }

    // Initialize the user object table
    for (i = 0; i < MAX_USER_OBJS; i++)
    {
        UserObjTable[i].uSerial = MIN_DYNAMAIC_SERIAL;
        UserObjTable[i].pObj = NULL;
#if (__MP_PLX_GUI)
        UserObjTable[i].dwProcessId = INVALID_PROCESSID;
#endif
    }
#if (__MP_PLX_GUI)
    for (i = 0; i < MAX_SHARED_OBJS; i++)
    {
        SharedObjTable[i].pObj = NULL;
    }
#endif
#endif //HANDLEISPTR
}

/*
**  Function : WOT_Exit
**  Purpose  :
**      Destroye the window object table.
*/
void WOT_Exit(void)
{
    int i;

#if (!NOCURSOR)
    PGRAPHDEV   pGraphDev;

    pGraphDev = GRAPHDEV_GetCursorScreen(0);
    ASSERT(pGraphDev != NULL);

    // Frees the stock cursor object
    for (i = 0; i < sizeof(CursorObjTable) / sizeof(CURSOROBJ); i++)
    {
        pGraphDev->drv.UnrealizeCursor(g_pDisplayDev, 
            CursorObjTable[i].pPhysCursor);
        MemFree(CursorObjTable[i].pPhysCursor);
    }

#endif // NOCURSOR

#if (!HANDLEISPTR)
    // Free all GDI object in GDI object table
    for (i = 0; i < MAX_GDI_OBJS; i++)
    {
        if (GDIObjTable[i].pObj)
        {
            MemFree(GDIObjTable[i].pObj);
            GDIObjTable[i].pObj = NULL;
        }
    }

    // Free all GDI object in user object table
    for (i = 0; i < MAX_USER_OBJS; i++)
    {
        if (UserObjTable[i].pObj)
        {
            MemFree(UserObjTable[i].pObj);
            UserObjTable[i].pObj = NULL;
        }
    }

#if (__MP_PLX_GUI)
    for (i = 0; i < MAX_SHARED_OBJS; i++)
    {
        if (SharedObjTable[i].pObj)
        {
            MemFree(SharedObjTable[i].pObj);
            SharedObjTable[i].pObj = NULL;
        }
    }
#endif

#endif //HANDLEISPTR
}

/*
**  Function : WOT_RegisterObj
**  Purpose  :
**      Adds a specified object into the object table.
**  Params   :
**      pObj : Pointer of the object to be added.
**      nType : Tag of the object to be added.
**  Returns  :
**      If the function succeeds, return handle of the object.
**      If the fucntion fails, return NULL.
*/
BOOL WOT_RegisterObj(PWSOBJ pObj, UINT uType, BOOL bShared)
{
#if (!HANDLEISPTR)
    int i, j;
    UINT uIndex;
#endif //HANDLEISPTR
    
    // Ensure the parameter is valid
    ASSERT(pObj != NULL);
    ASSERT(ISVALIDTYPE(uType));

#if (!HANDLEISPTR)
    if (bShared)
    {
#if (__MP_PLX_GUI)
        ASSERT(ISXGDITYPE(uType));

        // find a empty cell in GDI object table
        for (i = 0; i < MAX_SHARED_MASKS; i++)
        {
            if (dwSharedBitMask[i] != 0xFFFFFFFF)
                break;
        }
        
        // If no empty cell in the table, return 0
        if ( i >= MAX_SHARED_MASKS)
        {
            ASSERT(0);
            return FALSE;
        }
        
        for (j = 0; j < 32; j++)
        {
            if ((dwSharedBitMask[i] & (1L << j)) == 0)
            {
                dwSharedBitMask[i] |= (1L << j);
                break;
            }
        }
        
        uIndex = (i * 32) + j ;
        
        pObj->objhead.handle = MAKEHANDLE(uIndex, SHAREDOBJ_SERIAL, uType);
        SharedObjTable[uIndex].pObj = pObj;
#else  //__MP_PLX_GUI
        ASSERT(0);
#endif //__MP_PLX_GUI
    }
    else if (ISGDITYPE(uType))
    {
        // find a empty cell in GDI object table
        for (i = 0; i < MAX_GDI_MASKS; i++)
        {
            if (dwGDIBitMask[i] != 0xFFFFFFFF)
                break;
        }
        
        // If no empty cell in the table, return 0
        if ( i >= MAX_GDI_MASKS)
        {
            ASSERT(0);
            return FALSE;
        }
        
        for (j = 0; j < 32; j++)
        {
            if ((dwGDIBitMask[i] & (1L << j)) == 0)
            {
                dwGDIBitMask[i] |= (1L << j);
                break;
            }
        }
        
        uIndex = (i * 32) + j ;
        
        pObj->objhead.handle = MAKEHANDLE(uIndex, GDIObjTable[uIndex].uSerial, 
            uType);
        GDIObjTable[uIndex].pObj = pObj;
#if (__MP_PLX_GUI)
        GDIObjTable[uIndex].dwProcessId = WS_GetCurrentProcessId();
#endif
    }
    else
    {
        // find a empty cell in User object table
        for (i = 0; i < MAX_USER_MASKS; i++)
        {
            if (dwUserBitMask[i] != 0xFFFFFFFF)
                break;
        }
        
        // If no empty cell in the table, return 0
        if ( i >= MAX_USER_MASKS)
        {
            ASSERT(0);
            return FALSE;
        }
        
        for (j = 0; j < 32; j++)
        {
            if ((dwUserBitMask[i] & (1L << j)) == 0)
            {
                dwUserBitMask[i] |= (1L << j);
                break;
            }
        }
        
        uIndex = (i * 32) + j ;
        
        pObj->objhead.handle = 
            MAKEHANDLE(uIndex, UserObjTable[uIndex].uSerial, uType);
        UserObjTable[uIndex].pObj = pObj;
#if (__MP_PLX_GUI)
        UserObjTable[uIndex].dwProcessId = WS_GetCurrentProcessId();
#endif
    }
#else //HANDLEISPTR
#if (__MP_PLX_GUI)
    if (bShared)
        pObj->objhead.handle = MAKEHANDLE(0, SHAREDOBJ_SERIAL, uType);
    else
#endif
        pObj->objhead.handle = MAKEHANDLE(0, MIN_DYNAMAIC_SERIAL, uType);
#endif //HANDLEISPTR

    if (ISXGDITYPE(TYPEOFHANDLE(pObj->objhead.handle)))
    {
        ((PXGDIOBJ)(pObj))->bDeleted = FALSE;
        ((PXGDIOBJ)(pObj))->refcount = 0;
        ((PXGDIOBJ)(pObj))->refselect = 0;
    }

#ifdef WINDEBUG
    WINDEBUG_AddObj(uType);
#endif
    return TRUE;
}

/*
**  Function : WOT_UnregisterObj
**  Purpose  :
**      Removes a spcified object from the object table.
**  Params   :
**      handle : Handle of the object to be removed.
*/
void WOT_UnregisterObj(PWSOBJ pObj)
{
    UINT uIndex, uSerial, uType;
    HANDLE handle;

    ASSERT(pObj != NULL);

    handle = pObj->objhead.handle;
    if (handle == NULL)
        return;

    uIndex = INDEXOFHANDLE(handle);
    uSerial = SERIALOFHANDLE(handle);
    uType = TYPEOFHANDLE(handle);

    if (!ISVALIDTYPE(uType))
        return;

    if (uSerial == STOCKOBJ_SERIAL)
        return;

#if (!HANDLEISPTR)
    if (uSerial == SHAREDOBJ_SERIAL)
    {
#if (__MP_PLX_GUI)
        ASSERT(ISXGDITYPE(uType));
        
        if (SharedObjTable[uIndex].pObj == NULL)
            return;
        
        ASSERT(dwSharedBitMask[uIndex / 32] & (1L << (uIndex % 32)));
        
        if (handle != SharedObjTable[uIndex].pObj->objhead.handle)
            return;
        
        // 将要删除对象的handle设置为NULL，以表示该对象已被删除
        SharedObjTable[uIndex].pObj->objhead.handle = NULL;
        
        // 从对象表中删除该对象
        SharedObjTable[uIndex].pObj = NULL;
        dwSharedBitMask[uIndex / 32] &= ~(1L << (uIndex % 32));
#else  //__MP_PLX_GUI
        ASSERT(0);
#endif //__MP_PLX_GUI
    }
    else if (ISGDITYPE(uType))
    {
        // serial is not the same
        if (GDIObjTable[uIndex].uSerial != uSerial)
            return; 

        if (!GDIObjTable[uIndex].pObj)
            return;

#if (__MP_PLX_GUI)
        if (GDIObjTable[uIndex].dwProcessId != WS_GetCurrentProcessId())
        {
            SetLastError(1);
            return;
        }
#endif
        
        ASSERT(dwGDIBitMask[uIndex / 32] & (1L << (uIndex % 32)));

        if (handle != GDIObjTable[uIndex].pObj->objhead.handle)
            return;
        
        // 将要删除对象的handle设置为NULL，以表示该对象已被删除
        GDIObjTable[uIndex].pObj->objhead.handle = NULL;

        // 从对象表中删除该对象
        GDIObjTable[uIndex].pObj = NULL;
        dwGDIBitMask[uIndex / 32] &= ~(1L << (uIndex % 32));
        
        // 增加对象表中相应位置的序列号，使下次给位置对应的handle与刚被删
        // 除的对象使用的序列号不同
        if (GDIObjTable[uIndex].uSerial == MAX_SERIAL)
            GDIObjTable[uIndex].uSerial = MIN_DYNAMAIC_SERIAL;
        else 
            GDIObjTable[uIndex].uSerial++;
    }
    else
    {
        // serial is not the same
        if (UserObjTable[uIndex].uSerial != uSerial)
            return; 

        if (!UserObjTable[uIndex].pObj)
            return;

#if (__MP_PLX_GUI)
        if (UserObjTable[uIndex].dwProcessId != WS_GetCurrentProcessId())
        {
            SetLastError(1);
            return;
        }
#endif
        ASSERT(dwUserBitMask[uIndex / 32] & (1L << (uIndex % 32)));

        if (handle != UserObjTable[uIndex].pObj->objhead.handle)
            return;

        // 将要删除对象的handle设置为NULL，以表示该对象已被删除
        UserObjTable[uIndex].pObj->objhead.handle = NULL;

        // 从对象表中删除该对象
        UserObjTable[uIndex].pObj = NULL;
        dwUserBitMask[uIndex / 32] &= ~(1L << (uIndex % 32));

        // 增加对象表中相应位置的序列号，使下次给位置对应的handle与刚被删
        // 除的对象使用的序列号不同
        if (UserObjTable[uIndex].uSerial == MAX_SERIAL)
            UserObjTable[uIndex].uSerial = MIN_DYNAMAIC_SERIAL;
        else 
            UserObjTable[uIndex].uSerial++;
    }
#else //HANDLEISPTR
    pObj->objhead.handle = NULL;
#endif //HANDLEISPTR

#ifdef WINDEBUG
    WINDEBUG_RemoveObj(uType);
#endif
    return;
}

/*
**  Function : WOT_GetObj
**  Purpose  :
**      Retrieves a object using the specified handle and type.
**  Params   :
**      handle : Specifies the handle of the object to be located.
**      uType  : Specifies the tag of the object to be located.
**  Returns  :
**      If the function succeeds, return the object pointer.
**      If the function fails, return NULL.
*/
PWSOBJ WOT_GetObj(HANDLE handle, UINT uType)
{

#if (HANDLEISPTR)
    return (PWSOBJ)handle;
#else //HANDLEISPTR
    UINT uIndex, uSerial;

    // Process the desktop window(root window) specially
    if (handle == (HANDLE)HWND_ROOT && uType == OBJ_WINDOW)
        return (PWSOBJ)g_pRootWin;

    // If the handle is NULL, return NULL.
    if (handle == NULL)
        return NULL;

    // Get the real handle 
    handle = (HANDLE)((DWORD)handle & MASK_HANDLE);

    // Ensure the object type is valid
    if (uType == OBJ_ANY)
        uType = TYPEOFHANDLE(handle);
    else
    {
        if (uType != TYPEOFHANDLE(handle))
            return NULL;
    }

    if (!ISVALIDTYPE(uType))
        return NULL;

    // If the specified object type isn't equal to the type in the handle, 
    // return NULL

    // Get index from the specified handle.
    uIndex = INDEXOFHANDLE(handle);
    uSerial = SERIALOFHANDLE(handle);

    // If the serial no is 0, indicates the object is stock object, get 
    // the object from the stock object table
    if (uSerial == STOCKOBJ_SERIAL)
    {
#if (!NOCURSOR)

        // Gets the stock cursor object 
        if (uType == OBJ_CURSOR)
        {
            // Ensure the index is valid index
            if (uIndex >= sizeof(CursorObjTable) / sizeof(CURSOROBJ))
                return NULL;
            
            // Ensure the specified handle is equal to the stored handle
            if (handle != CursorObjTable[uIndex].objhead.handle)
                return NULL;
            
            return (PWSOBJ)&CursorObjTable[uIndex];
        }

#endif // NOCURSOR

        // Gets the GDI stock object

        // Ensure the index is valid index
        if (uIndex >= MAX_STOCK_OBJS)
            return NULL;
        
        // Ensure the specified handle is equal to the stored handle
        if (handle != StockObjTable[uIndex].pObj->objhead.handle)
            return NULL;
        
        return StockObjTable[uIndex].pObj;
    }
    else if (uSerial == SHAREDOBJ_SERIAL)
    {
#if (__MP_PLX_GUI)
        ASSERT(ISGDITYPE(uType));
        if (uIndex >= MAX_SHARED_OBJS)
            return NULL;

        if (handle != SharedObjTable[uIndex].pObj->objhead.handle)
            return NULL;

        return SharedObjTable[uIndex].pObj;
#else  //__MP_PLX_GUI
        // ASSERT(0);
		return NULL;
#endif //__MP_PLX_GUI
    }

    // If the object is GDI object, get the object from the GDI object
    // table.
    if (ISGDITYPE(uType))
    {
        // If the index is invalid, return NULL
        if (uIndex >= MAX_GDI_OBJS)
            return NULL;

        // If serial is not the same, return NULL
        if (GDIObjTable[uIndex].uSerial != uSerial)
            return NULL; 

        // If the stored object pointer is NULL, return NULL
        if (!GDIObjTable[uIndex].pObj)
            return NULL;

        // If the specified handle isn't equal to the stored handle, 
        // return NULL
        if (handle != GDIObjTable[uIndex].pObj->objhead.handle)
            return NULL;
        
#if (__MP_PLX_GUI)
        if (GDIObjTable[uIndex].dwProcessId != WS_GetCurrentProcessId())
        {
            SetLastError(1);
            return NULL;
        }
#endif
        return GDIObjTable[uIndex].pObj;
    }

    // Now get the object from user object table

    // If the index is invalid, return NULL
    if (uIndex >= MAX_USER_OBJS)
        return NULL;
    
    // If serial is not the same, return NULL
    if (UserObjTable[uIndex].uSerial != uSerial)
        return NULL; 
    
    // If the stored object pointer is NULL, return NULL
    if (!UserObjTable[uIndex].pObj)
        return NULL;
        
    // If the specified handle isn't equal to the stored handle, 
    // return NULL
    if (handle != UserObjTable[uIndex].pObj->objhead.handle)
        return NULL;
    
#if (__MP_PLX_GUI)
    /* The window object can be used by the various of processes. But other 
    ** objects be only used in a same process!
    */
    if (uType != OBJ_WINDOW && 
        UserObjTable[uIndex].dwProcessId != WS_GetCurrentProcessId())
    {
        SetLastError(1);
        return NULL;
    }
#endif

    return UserObjTable[uIndex].pObj;

#endif //HANDLEISPTR
}

HANDLE WOT_GetHandle(PWSOBJ pObj)
{
    if (pObj == NULL)
        return NULL;

#if (HANDLEISPTR)
    return (HANDLE)pObj;
#else
    return pObj->objhead.handle;
#endif
}

PXGDIOBJ WOT_LockObj(HANDLE handle, UINT uType)
{
    PXGDIOBJ pObj;

    if (uType != OBJ_ANY && !ISXGDITYPE(uType))
        return NULL;

    pObj = (PXGDIOBJ)WOT_GetObj(handle, uType);
    if (pObj == NULL)
    {
        return NULL;
    }

    if (uType == OBJ_ANY)
    {
        uType = TYPEOFHANDLE(pObj->objhead.handle);
        if (!ISXGDITYPE(uType))
            return NULL;
    }
    
    if (WOT_IsStockObj((PXGDIOBJ)pObj))
        return pObj;

    if (pObj->bDeleted == TRUE)
        return NULL;

    pObj->refcount ++;

    return pObj;
}

PXGDIOBJ WOT_UnlockObj(PXGDIOBJ pObj)
{
    UINT uType;

    ASSERT(pObj != NULL);
    uType = TYPEOFHANDLE(pObj->objhead.handle);
    if (uType < OBJ_XGDI_MIN || uType > OBJ_XGDI_MAX)
    {
        return NULL;
    }

    if (WOT_IsStockObj((PXGDIOBJ)pObj))
        return pObj;

    pObj->refcount --;

    if (!ISDELETEDXGDIOBJ(pObj))
        return pObj;

    DeleteObj(pObj);
    return NULL;
}

void WOT_SelectObj(PXGDIOBJ pObj)
{
    ASSERT(pObj != NULL);
    ASSERT(ISXGDITYPE(TYPEOFHANDLE(pObj->objhead.handle)));

    if (WOT_IsStockObj((PXGDIOBJ)pObj))
        return ;

    ASSERT (!pObj->bDeleted);

    pObj->refselect ++;
}

PXGDIOBJ WOT_UnselectObj(PXGDIOBJ pObj)
{
    UINT uType;

    ASSERT(pObj != NULL);
    uType = TYPEOFHANDLE(pObj->objhead.handle);
    if (uType < OBJ_XGDI_MIN || uType > OBJ_XGDI_MAX)
    {
        return NULL;
    }

    if (WOT_IsStockObj((PXGDIOBJ)pObj))
        return pObj;
    
    pObj->refselect --;
    
    if (!ISDELETEDXGDIOBJ(pObj))
        return pObj;

    DeleteObj(pObj);
    return NULL;
}

int WOT_GetSelectCount(PXGDIOBJ pObj)
{
    ASSERT(ISXGDITYPE(TYPEOFHANDLE(pObj->objhead.handle)));

    return pObj->refselect;
}

BOOL WOT_DelectObj(PXGDIOBJ pObj)
{
    ASSERT(pObj != NULL);
    
    if (WOT_IsStockObj((PXGDIOBJ)pObj))
        return FALSE;
    
    pObj->bDeleted = TRUE;

    if (!ISDELETEDXGDIOBJ(pObj))
        return FALSE;
    
    DeleteObj(pObj);
    return TRUE;
}

/*
**  Function : WOT_GetGDIObj
**  Purpose  :
**      Retrieves a object using the specified handle.
*/
/*
PGDIOBJ WOT_GetGDIObj(HANDLE handle)
{
#if (!HANDLEISPTR)
    UINT uType;

    // If the handle is NULL, return NULL.
    if (handle == NULL)
        return NULL;
    uType = TYPEOFHANDLE(handle);

    if (!ISGDITYPE(uType) || uType == OBJ_DC)
        return NULL;

    return (PGDIOBJ)WOT_GetObj(handle, uType);
#else //HANDLEISPTR
    return (PGDIOBJ)handle;
#endif //HANDLEISPTR
}
*/

/*
**  Function : WOT_GetObjType
**  Purpose  :
**      Gets the type of the specified object.
*/
UINT WOT_GetObjType(PWSOBJ pObj)
{
    UINT uType;

    ASSERT(pObj != NULL);
    uType = TYPEOFHANDLE(pObj->objhead.handle);

    if (uType < OBJ_MIN || uType > OBJ_MAX)
        uType = OBJ_NULL;

    return uType;
}

/*
**  Function : WOT_IsStockObj
**  Purpose  :
**      Retrieves whether the specified object is stock object.
*/
BOOL WOT_IsStockObj(PXGDIOBJ pObj)
{
    UINT uIndex, uSerial, uType;

    if (pObj == NULL)
        return FALSE;

    uIndex = INDEXOFHANDLE(pObj->objhead.handle);
    uSerial = SERIALOFHANDLE(pObj->objhead.handle);
    uType = TYPEOFHANDLE(pObj->objhead.handle);

    if (!ISVALIDTYPE(uType))
        return FALSE;

    if (uIndex >= MAX_STOCK_OBJS)
        return FALSE;

    return (uSerial == STOCKOBJ_SERIAL);
}

#if (__MP_PLX_GUI)
/*
**  Function : WOT_IsSharedObj
**  Purpose  :
**      Retrieves whether the specified object is shared object.
*/
BOOL WOT_IsSharedObj(PXGDIOBJ pObj)
{
    UINT uIndex, uSerial, uType;

    if (pObj == NULL)
        return FALSE;

    uIndex = INDEXOFHANDLE(pObj->objhead.handle);
    uSerial = SERIALOFHANDLE(pObj->objhead.handle);
    uType = TYPEOFHANDLE(pObj->objhead.handle);

    if (!ISVALIDTYPE(uType))
        return FALSE;

    if (uIndex >= MAX_SHARED_OBJS)
        return FALSE;

    return (uSerial == SHAREDOBJ_SERIAL);
}

/* 对于窗口内部使用的CacheDC，其进程号是动态改变的。 */
void WOT_SetObjProcessId(PGDIOBJ pObj, DWORD dwProcessId)
{
    UINT uIndex, uType, uSerial;
    
    uIndex = INDEXOFHANDLE(pObj->objhead.handle);
    uType = TYPEOFHANDLE(pObj->objhead.handle);
    uSerial = SERIALOFHANDLE(pObj->objhead.handle);
    
    if (uSerial == STOCKOBJ_SERIAL || uSerial == SHAREDOBJ_SERIAL)
        return;
    
    if (!ISGDITYPE(uType))
        return;
    
    GDIObjTable[uIndex].dwProcessId = dwProcessId;
    
    return;
}

#endif

/*
**  Function : WOT_GetStockObj
**  Purpose  :
**      Gets the stock object from index.
*/
PXGDIOBJ WOT_GetStockObj(int nIndex)
{
    // DEFAULT_BITMAP is a special index for default bitmap object in
    // memory DC, and the default bitmap object is the last object in
    // stock object table
    if (nIndex == DEFAULT_BITMAP)
        nIndex = MAX_STOCK_OBJS - 1;

    // Ensure the index is a valid index
    if (nIndex < 0 || nIndex >= MAX_STOCK_OBJS)
        return NULL;

    return (PXGDIOBJ)StockObjTable[nIndex].pObj;
}

/*
**  Function : WOT_GetStockObjIndex
**  Purpose  :
**      Gets the index of the specified object.
*/
int WOT_GetStockObjIndex(PXGDIOBJ pObj)
{
    if (!WOT_IsStockObj(pObj))
        return -1;

    return INDEXOFHANDLE(pObj->objhead.handle);
}

/*
**  Function : WOT_GetStockPhysObj
*/
/*
void* WOT_GetStockPhysObj(PWSOBJ pObj)
{
    UINT uIndex, uSerial;

    // Get index from the specified handle.
    uIndex = INDEXOFHANDLE(pObj->objhead.handle);
    uSerial = SERIALOFHANDLE(pObj->objhead.handle);

    // If the serial no isn't 0, indicates the object isn't stock object, 
    // return NULL
    if (uSerial != 0)
        return NULL;

    // Ensure the index is valid index
    if (uIndex >= MAX_STOCK_OBJS)
        return NULL;
    
    // Ensure the specified handle is equal to the stored handle
    if (pObj->objhead.handle != StockObjTable[uIndex].pObj->objhead.handle)
        return NULL;
    
    return StockObjTable[uIndex].pPhysObj;
}
*/

#if (!NOCURSOR)

/*
**  Function : WOT_GetStockCursor
**  Purpose  :
**      Gets the stock cursor from index.
*/
PWSOBJ WOT_GetStockCursor(DWORD nIndex)
{
    // Ensure the index is a valid index
    if (nIndex < (DWORD)IDC_ARROW || nIndex > (DWORD)IDC_HELP)
        return NULL;

    nIndex -= (DWORD)IDC_ARROW;

    return (PWSOBJ)&CursorObjTable[(int)nIndex];
}

#endif // NOCURSOR

static void DeleteObj(PXGDIOBJ pObj)
{
    UINT uType;

    ASSERT(pObj != NULL);
    ASSERT(ISDELETEDXGDIOBJ(pObj));

    uType = TYPEOFHANDLE(pObj->objhead.handle);
    ASSERT(ISXGDITYPE(uType));

    ASSERT(!WOT_IsStockObj(pObj));

    switch (uType)
    {
    case OBJ_PEN :
        
        PEN_Destroy((PPENOBJ)pObj);
        break;
        
    case OBJ_BRUSH :
        
        BRUSH_Destroy((PBRUSHOBJ)pObj);
        break;
        
    case OBJ_FONT :
        
        FONT_Destroy((PFONTOBJ)pObj);
        break;
        
    case OBJ_BITMAP :
        
        BMP_Destroy((PBMPOBJ)pObj);
        break;
        
    case OBJ_PALETTE :
        
        break;
        
#ifdef RGNSUPPORT
    case OBJ_RGN :
        
        RGN_Destroy((PRGNOBJ)pObj);
        break;
#endif
    }
}

#ifdef WINDEBUG
void WINDEBUG_RemoveObj(int uType)
{
    // Decrease the object count for debug version
    if (ISGDITYPE(uType))
        ObjDebugInfo.nGDIObjNum--;
    else if (ISTIMERTYPE(uType))
        ObjDebugInfo.nTimerNum--;
    else
        ObjDebugInfo.nUserObjNum--;
    
    switch (uType)
    {
    case OBJ_PEN :
        
        ObjDebugInfo.nPenNum--;
        break;
        
    case OBJ_BRUSH :
        
        ObjDebugInfo.nBrushNum--;
        break;
        
    case OBJ_BITMAP :
        
        ObjDebugInfo.nBitmapNum--;
        break;
        
    case OBJ_FONT :
        
        ObjDebugInfo.nFontNum--;
        break;
        
    case OBJ_PALETTE :
        
        ObjDebugInfo.nPaletteNum--;
        break;
        
    case OBJ_RGN :
        
        ObjDebugInfo.nRegionNum--;
        break;
        
    case OBJ_DC :
        
        ObjDebugInfo.nDCNum--;
        break;
        
    case OBJ_WINDOW :
        
        ObjDebugInfo.nWindowNum--;
        break;
        
    case OBJ_CURSOR :
        
        ObjDebugInfo.nCursorNum--;
        break;
        
    case OBJ_ICON :
        
        ObjDebugInfo.nIconNum--;
        break;
        
    case OBJ_MENU :
        
        ObjDebugInfo.nMenuNum--;
        break;

    case OBJ_TIMER:

        break;

    default :
        
        ASSERT(0);
        break;
    }
}

void WINDEBUG_AddObj(int uType)
{
    // Decrease the object count for debug version
    if (ISGDITYPE(uType))
    {
        ObjDebugInfo.nGDIObjNum++;
        if (ObjDebugInfo.nGDIObjNum > ObjDebugInfo.nGDIObjMax)
            ObjDebugInfo.nGDIObjMax = ObjDebugInfo.nGDIObjNum;
    }
    else if (ISTIMERTYPE(uType))
    {
        ObjDebugInfo.nTimerNum++;
        if (ObjDebugInfo.nTimerNum > ObjDebugInfo.nTimerObjMax)
            ObjDebugInfo.nTimerObjMax = ObjDebugInfo.nTimerNum;
    }
    else
    {
        ObjDebugInfo.nUserObjNum++;
        if (ObjDebugInfo.nUserObjNum > ObjDebugInfo.nUserObjMax)
            ObjDebugInfo.nUserObjMax = ObjDebugInfo.nUserObjNum;
    }
    
    switch (uType)
    {
    case OBJ_PEN :
        
        ObjDebugInfo.nPenNum++;
        break;
        
    case OBJ_BRUSH :
        
        ObjDebugInfo.nBrushNum++;
        break;
        
    case OBJ_BITMAP :
        
        ObjDebugInfo.nBitmapNum++;
        break;
        
    case OBJ_FONT :
        
        ObjDebugInfo.nFontNum++;
        break;
        
    case OBJ_PALETTE :
        
        ObjDebugInfo.nPaletteNum++;
        break;
        
    case OBJ_RGN :
        
        ObjDebugInfo.nRegionNum++;
        break;
        
    case OBJ_DC :
        
        ObjDebugInfo.nDCNum++;
        break;
        
    case OBJ_WINDOW :
        
        ObjDebugInfo.nWindowNum++;
        break;
        
    case OBJ_CURSOR :
        
        ObjDebugInfo.nCursorNum++;
        break;
        
    case OBJ_ICON :
        
        ObjDebugInfo.nIconNum++;
        break;
        
    case OBJ_MENU :
        
        ObjDebugInfo.nMenuNum++;
        break;
        
    case OBJ_TIMER:

        break;

    default :
        
        ASSERT(0);
        break;
    }
}
#endif

#ifdef WINDEBUG // For emulator only

/*
**  Function : GetWinDebugInfo
**  Purpose  : 
**      Gets the object info of window system.
*/
#ifdef _MSC_VER
_declspec(dllexport) 
#endif
void GetWinDebugInfo(PWINDEBUGINFO pWinDebugInfo)
{
    if (pWinDebugInfo)
        memcpy(pWinDebugInfo, &ObjDebugInfo, sizeof(WINDEBUGINFO));
}

#endif  // WINDEBUG
