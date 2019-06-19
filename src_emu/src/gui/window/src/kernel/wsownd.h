/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for window object management
 *            
\**************************************************************************/

#ifndef __WSOWND_H
#define __WSOWND_H

#ifndef __WSODC_H
#include "wsodc.h"
#endif // __WSODC_H

/* 窗口暴露区结构, 用于存储窗口的所有暴露区 */
typedef struct
{
    WORD    count;              // 暴露区所含矩形的个数
    WORD    clientblks;         // 暴露区所含客户区矩形的个数
    RECT    rect[1];            // 暴露区中矩形数组的首地址
} EXPOSETBL, *PEXPOSETBL;

/* 窗口的覆盖状态 */
#define T_UNDEFINED     0       // 暴露区无效, 需重新计算
#define T_OVERLAPPED    1       // 窗口被完全覆盖, 没有暴露区
#define T_EXPOSED       2       // 窗口的暴露区表有效

/* 窗口更新标记 */
#define PAINT_NO        0   //不需要重画
#define PAINT_WAIT      1   //需要重画，但需要等其父窗口绘制完后才能得到消息
#define PAINT_QUEUE     2   //已经排进了PAINT消息队列，等待重画
#define PAINT_DRAWING   3   //正在处理WM_PAINT消息的过程中

typedef struct
{
    OBJHDR objhead;
} WINOBJHDR, *PWINOBJHDR;

/* 定义窗口对象 */
typedef struct tagWINOBJ
{
//    HWND        handle;         // 窗口对象句柄
    WINOBJHDR   header;
    DWORD       dwThreadId;
    PWNDCLASS   pWndClass;      // 指向所属窗口类的指针
    WNDPROC     pfnWndProc;     // 窗口对应的窗口函数

    DWORD       dwStyle;        // 窗口基本风格
    DWORD       dwExStyle;      // 窗口扩展风格

    DWORD       id;             // 对于WS_CHILD窗口, 为子窗口ID; 
                                // 对于WS_POPUP窗口, 为窗口的菜单句柄

    HINSTANCE   hInst;          // 创建窗口的实例句柄
    PVOID       pParam;         // 创建窗口时最后一个参数指定的用户数据

    BYTE        byOwnWins;      // 拥有的窗口的个数
    BYTE        bMapped;        // 窗口是否真正映射(取决于窗口是否具有
                                // WS_VISIBLE风格以及窗口的祖先窗口是否
                                // 具有WS_VISIBLE风格)
    BYTE        bDestroying;    // 窗口正在删除标志
//    BYTE        bOverlapStat;   // 窗口的覆盖状态, 详见上面的说明

    BYTE        bErase;         // 窗口更新区域的背景是否要重画
    BYTE        bNCPaint;       // 标识是否处于响应WM_NCPAINT消息之中
//    BYTE        bUpdate;        // 窗口是否需要更新标记
    BYTE        flagUpdate;        // 窗口更新标记 0/1/2/3

    PEXPOSETBL  pExposeTbl;     // 指向窗口暴露区结构的指针    
    RECT        rcUpdate;       // 窗口当前需更新的区域, 屏幕坐标
    PDC         pDC;            // 用于窗口绘图操作的DC指针

    RECT        rcWindow;       // 未经过裁剪窗口矩形，屏幕坐标
    RECT        rcClient;       // 未经过裁剪窗口客户区矩形，屏幕坐标

    // 窗口的父窗口, 子窗口和兄弟窗口指针, 用以构成窗口树

    PWINOBJ     pParent;        // 指向窗口父窗口的指针
    PWINOBJ     pOwner;         // 指向窗口属主窗口的指针
    PWINOBJ     pChild;         // 指向窗口第一个子窗口的指针
    PWINOBJ     pNext;          // 指向窗口的下一个兄弟窗口的指针
    PWINOBJ     pPrev;          // 指向窗口的前一个兄弟窗口的指针

    void*       pDefWindowData; // 为DefWindowProc留出的数据指针
    BYTE        data[1];        // 窗口用户数据首地址
} WINOBJ;

// WINOBJ对象指针类型PWINOBJ已提前定义在wsodc.h中, 此处不再定义, 否则在某
// 些编译器商会产生编译错误

// 窗口系统根窗口指针, 用于窗口对象管理模块内部, 定义在winroot.c中
extern PWINOBJ g_pRootWin;

// 一些用于窗口管理实用宏
#define ISROOTWIN(pWin)      ((pWin) == g_pRootWin)
#define ISPOPUP(pWin)        ((pWin) && ((pWin)->pParent == g_pRootWin))
#define ISCHILD(pWin)        ((pWin)->dwStyle & WS_CHILD)
#define ISNCCHILD(pWin)      (((pWin)->dwStyle & WS_CHILD) && \
                              ((pWin)->dwStyle & WS_NCCHILD))
#define ISCLIENTCHILD(pWin)  (!((pWin)->dwStyle & WS_NCCHILD))
#define ISTOPMOST(pWin)      ((pWin)->dwStyle & WS_TOPMOST)
#define ISMAPPED(pWin)       ((pWin)->bMapped)
#define ISOVERLAPPED(pWin)   ((pWin)->bOverlapStat == T_OVERLAPPED)
#define ISEXPOSED(pWin)      ((pWin)->bOverlapStat == T_EXPOSED)
#define ISNOFOCUS(pWin)      ((pWin)->pWndClass->style & CS_NOFOCUS)
#define ISNOCLOSE(pWin)      ((pWin)->pWndClass->style & CS_NOCLOSE)
#define ISCLIPSIBLINGS(pWin) ((pWin)->dwStyle & WS_CLIPSIBLINGS) 
#define ISCLIPCHILDREN(pWin) ((pWin)->dwStyle & WS_CLIPCHILDREN)
#define ISDISABLED(pWin)     ((pWin)->dwStyle & WS_DISABLED)
#define ISOWNDC(pWin)        (((pWin)->pWndClass->style & CS_OWNDC) || \
                              ((pWin)->pWndClass->style & CS_CLASSDC))
#define ISCLASSDC(pWin)      (((pWin)->pWndClass->style & CS_CLASSDC))
#define ISPARENTDC(pWin)     (((pWin)->pWndClass->style & CS_PARENTDC))
#define ISDESTROYING(pWin)   ((pWin)->bDestroying)

/* 以下窗口对象操作函数 winobj.c */

// 窗口的创建和删除
PWINOBJ WND_Create(DWORD dwExStyle, PCSTR pszClassName, PCSTR pszWindowName, 
                   DWORD dwStyle, int x, int y, int nWidth, int nHeight, 
                   HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, 
                   PVOID pParam);
void    WND_Destroy(PWINOBJ pWin);
BOOL    WND_IsWindow(PWINOBJ pWin);

BOOL    WND_IsInSameThread(PWINOBJ pWin);
BOOL    WND_IsCanBeDestroyed(DWORD dwThreadId);
BOOL    WND_DestroyWindowsByThreadId(DWORD dwThreadId);

LONG    WND_SendMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                        LPARAM lParam);

BOOL    WND_SendNotifyMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                        LPARAM lParam);

void    WND_NCCalcSize(PWINOBJ pWin, PRECT prcClient);

// 窗口基本属性的存取
PWINOBJ WND_GetParent(PWINOBJ pWin);
//void*   WND_GetUserData(PWINOBJ pWin);
//void*   WND_GetUserDataBase(PWINOBJ pWin);
void*   WND_GetExtraDataPtr(PWINOBJ pWin);
void*   WND_GetExtraDataPtrBase(PWINOBJ pWin);


WORD    WND_SetWord(PWINOBJ pWin, int nIndex, WORD wNewValue);
WORD    WND_GetWord(PWINOBJ pWin, int nIndex);
LONG    WND_SetLong(PWINOBJ pWin, int nIndex, LONG lNewValue);
LONG    WND_GetLong(PWINOBJ pWin, int nIndex);

// 窗口的输入状态
BOOL    WND_IsEnabled(PWINOBJ pWin);
BOOL    WND_Enable(PWINOBJ pWin, BOOL bEnabled);

// 窗口的显示和隐藏
BOOL    WND_IsVisible(PWINOBJ pWin);
BOOL    WND_Show(PWINOBJ pWin, int nCmdShow);

// 与坐标有关的操作函数 wincoord.c

PWINOBJ WND_FromPoint(int x, int y);

BOOL    WND_PointInClient(PWINOBJ pWin, int x, int y);
void    WND_ScreenToClient(PWINOBJ pWin, int* px, int* py);
void    WND_WindowToScreen(PWINOBJ pWin, int* px, int* py);
void    WND_ClientToScreen(PWINOBJ pWin, int* pX, int* pY);

// winobj.c 
void    WND_GetRect(PWINOBJ pWin, PRECT pRect, int nPart, int nXYMode);

// 窗口的查询和枚举函数
int     WND_MakeHwndList(HWND *pHwnd, int nCount);
PWINOBJ WND_FindWindow(PCSTR pszClassName, PCSTR pszWindowName);
void    WND_EnumWindows(WNDENUMPROC pEnumFunc, LPARAM lParam);
void    WND_EnumChildWindows(PWINOBJ pParent, WNDENUMPROC pEnumFunc, 
                             LPARAM lParam);
PWINOBJ WND_GetWindow(PWINOBJ pWin, UINT uCmd);

// 下面函数供输入设备管理模块(devinput.c)调用
BOOL    WND_IsDescendant(PWINOBJ pWin, PWINOBJ pForefatherWin);
BOOL    WND_IsOwned(PWINOBJ pWin, PWINOBJ pOwnerWin);
BOOL    WND_IsDestroying(PWINOBJ pWin);

// 下面函数用于窗口的重画和滚动 winpaint.c

PDC     WND_GetClientDC(PWINOBJ pWin);
PDC     WND_GetWindowDC(PWINOBJ pWin);
BOOL    WND_ReleaseDC(PWINOBJ pWin, PDC pDC);

PDC     WND_BeginPaint(PWINOBJ pWin, PPAINTSTRUCT pPaintStruct);
void    WND_EndPaint(PWINOBJ pWin, const PAINTSTRUCT* pPaintStruct);

void    WND_InvalidateTree(PWINOBJ pWin, const RECT* pRect, BOOL bErase, 
                           BOOL bInvalidateAll);
void    WND_InvalidateRect(PWINOBJ pWin, const RECT* pRect, BOOL bErase);
void    WND_ValidateRect(PWINOBJ pWin, const RECT* pRect);

void    WND_Update(PWINOBJ pWin);
void    WND_UpdateTree(PWINOBJ pWin);
BOOL    WND_GetPaintMessage(PMSG pMsg);

void    WND_DestroyForPaint(PWINOBJ pWin);

void    WND_ScrollWindow(PWINOBJ pWin, int dx, int dy, 
                         const RECT* prcScroll, const RECT* prcClip);

// 定义窗口的当前绘图区域
#define PA_CLIENT           0       // 整个客户区
#define PA_WINDOW           1       // 整个窗口
#define PA_UPDATECLIENT     2       // 客户区需要更新的区域
#define PA_UPDATEWINDOW     3       // 窗口需要更新的区域

// 获得窗口的当前绘图区域和绘图原点 winpaint.c
void    WND_GetPaintRect(PWINOBJ pWin, int nPaintArea, PRECT pRect);
void    WND_GetPaintOrg(PWINOBJ pWin, int nPaintArea, PPOINT pptOrg);

// 下面三个函数用于存取指定窗口的窗口类的信息, 实现在winclass.c中
int     WND_GetClassName(PWINOBJ pWin, PSTR pszClassName, int nMaxCount);
WORD    WND_GetClassWord(PWINOBJ pWin, int nIndex);
WORD    WND_SetClassWord(PWINOBJ pWin, int nIndex, WORD wNewValue);
DWORD   WND_GetClassLong(PWINOBJ pWin, int nIndex);
DWORD   WND_SetClassLong(PWINOBJ pWin, int nIndex, DWORD dwNewValue);

// 下面三个函数实现在winpub.c中, 供窗口管理的各个模块调用
void    WND_Place(PWINOBJ pWin);

// Root window mangement functions, implemented in winroot.c
BOOL    WND_CreateRoot(void);

// Window map and unmap functions implemented in winmap.c
void    WND_Map(PWINOBJ pWin);
PWINOBJ WND_MapOwnedPopups(PWINOBJ pWin);
void    WND_Unmap(PWINOBJ pWin, BOOL bDestroy);
void    WND_UnmapOwnedPopups(PWINOBJ pWin);

// Window move and move to top fuction implemented in winpos.c
void    WND_Move(PWINOBJ pWin, int x, int y, int width, int height, 
                 BOOL bRepaint);
void    WND_MoveToTop(PWINOBJ pWin);
void    WND_ScrollChild(PWINOBJ pWin, int dx, int dy, const RECT* prcClip, 
                        BOOL bTop);

// 窗口暴露区管理函数, 实现在winexpos.c
PEXPOSETBL WND_GetExposeTbl(PWINOBJ pWin);
PEXPOSETBL WND_MakeExposeTbl(PWINOBJ pWin);
void WND_InvalidateExposeTbl(PWINOBJ pWin);
void WND_Expose(PWINOBJ pWin, const RECT* pRect, PWINOBJ pEventWin);
void WND_InvalidateExposeTblTree(PWINOBJ pWin, const RECT* prcExpose);
void WND_InvalidateExposeTblBelow(PWINOBJ pWin, const RECT* prcExpose);

#endif //__WSOWND_H
