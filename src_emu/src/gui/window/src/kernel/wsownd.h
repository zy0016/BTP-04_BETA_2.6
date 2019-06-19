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

/* ���ڱ�¶���ṹ, ���ڴ洢���ڵ����б�¶�� */
typedef struct
{
    WORD    count;              // ��¶���������εĸ���
    WORD    clientblks;         // ��¶�������ͻ������εĸ���
    RECT    rect[1];            // ��¶���о���������׵�ַ
} EXPOSETBL, *PEXPOSETBL;

/* ���ڵĸ���״̬ */
#define T_UNDEFINED     0       // ��¶����Ч, �����¼���
#define T_OVERLAPPED    1       // ���ڱ���ȫ����, û�б�¶��
#define T_EXPOSED       2       // ���ڵı�¶������Ч

/* ���ڸ��±�� */
#define PAINT_NO        0   //����Ҫ�ػ�
#define PAINT_WAIT      1   //��Ҫ�ػ�������Ҫ���丸���ڻ��������ܵõ���Ϣ
#define PAINT_QUEUE     2   //�Ѿ��Ž���PAINT��Ϣ���У��ȴ��ػ�
#define PAINT_DRAWING   3   //���ڴ���WM_PAINT��Ϣ�Ĺ�����

typedef struct
{
    OBJHDR objhead;
} WINOBJHDR, *PWINOBJHDR;

/* ���崰�ڶ��� */
typedef struct tagWINOBJ
{
//    HWND        handle;         // ���ڶ�����
    WINOBJHDR   header;
    DWORD       dwThreadId;
    PWNDCLASS   pWndClass;      // ָ�������������ָ��
    WNDPROC     pfnWndProc;     // ���ڶ�Ӧ�Ĵ��ں���

    DWORD       dwStyle;        // ���ڻ������
    DWORD       dwExStyle;      // ������չ���

    DWORD       id;             // ����WS_CHILD����, Ϊ�Ӵ���ID; 
                                // ����WS_POPUP����, Ϊ���ڵĲ˵����

    HINSTANCE   hInst;          // �������ڵ�ʵ�����
    PVOID       pParam;         // ��������ʱ���һ������ָ�����û�����

    BYTE        byOwnWins;      // ӵ�еĴ��ڵĸ���
    BYTE        bMapped;        // �����Ƿ�����ӳ��(ȡ���ڴ����Ƿ����
                                // WS_VISIBLE����Լ����ڵ����ȴ����Ƿ�
                                // ����WS_VISIBLE���)
    BYTE        bDestroying;    // ��������ɾ����־
//    BYTE        bOverlapStat;   // ���ڵĸ���״̬, ��������˵��

    BYTE        bErase;         // ���ڸ�������ı����Ƿ�Ҫ�ػ�
    BYTE        bNCPaint;       // ��ʶ�Ƿ�����ӦWM_NCPAINT��Ϣ֮��
//    BYTE        bUpdate;        // �����Ƿ���Ҫ���±��
    BYTE        flagUpdate;        // ���ڸ��±�� 0/1/2/3

    PEXPOSETBL  pExposeTbl;     // ָ�򴰿ڱ�¶���ṹ��ָ��    
    RECT        rcUpdate;       // ���ڵ�ǰ����µ�����, ��Ļ����
    PDC         pDC;            // ���ڴ��ڻ�ͼ������DCָ��

    RECT        rcWindow;       // δ�����ü����ھ��Σ���Ļ����
    RECT        rcClient;       // δ�����ü����ڿͻ������Σ���Ļ����

    // ���ڵĸ�����, �Ӵ��ں��ֵܴ���ָ��, ���Թ��ɴ�����

    PWINOBJ     pParent;        // ָ�򴰿ڸ����ڵ�ָ��
    PWINOBJ     pOwner;         // ָ�򴰿��������ڵ�ָ��
    PWINOBJ     pChild;         // ָ�򴰿ڵ�һ���Ӵ��ڵ�ָ��
    PWINOBJ     pNext;          // ָ�򴰿ڵ���һ���ֵܴ��ڵ�ָ��
    PWINOBJ     pPrev;          // ָ�򴰿ڵ�ǰһ���ֵܴ��ڵ�ָ��

    void*       pDefWindowData; // ΪDefWindowProc����������ָ��
    BYTE        data[1];        // �����û������׵�ַ
} WINOBJ;

// WINOBJ����ָ������PWINOBJ����ǰ������wsodc.h��, �˴����ٶ���, ������ĳ
// Щ�������̻�����������

// ����ϵͳ������ָ��, ���ڴ��ڶ������ģ���ڲ�, ������winroot.c��
extern PWINOBJ g_pRootWin;

// һЩ���ڴ��ڹ���ʵ�ú�
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

/* ���´��ڶ���������� winobj.c */

// ���ڵĴ�����ɾ��
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

// ���ڻ������ԵĴ�ȡ
PWINOBJ WND_GetParent(PWINOBJ pWin);
//void*   WND_GetUserData(PWINOBJ pWin);
//void*   WND_GetUserDataBase(PWINOBJ pWin);
void*   WND_GetExtraDataPtr(PWINOBJ pWin);
void*   WND_GetExtraDataPtrBase(PWINOBJ pWin);


WORD    WND_SetWord(PWINOBJ pWin, int nIndex, WORD wNewValue);
WORD    WND_GetWord(PWINOBJ pWin, int nIndex);
LONG    WND_SetLong(PWINOBJ pWin, int nIndex, LONG lNewValue);
LONG    WND_GetLong(PWINOBJ pWin, int nIndex);

// ���ڵ�����״̬
BOOL    WND_IsEnabled(PWINOBJ pWin);
BOOL    WND_Enable(PWINOBJ pWin, BOOL bEnabled);

// ���ڵ���ʾ������
BOOL    WND_IsVisible(PWINOBJ pWin);
BOOL    WND_Show(PWINOBJ pWin, int nCmdShow);

// �������йصĲ������� wincoord.c

PWINOBJ WND_FromPoint(int x, int y);

BOOL    WND_PointInClient(PWINOBJ pWin, int x, int y);
void    WND_ScreenToClient(PWINOBJ pWin, int* px, int* py);
void    WND_WindowToScreen(PWINOBJ pWin, int* px, int* py);
void    WND_ClientToScreen(PWINOBJ pWin, int* pX, int* pY);

// winobj.c 
void    WND_GetRect(PWINOBJ pWin, PRECT pRect, int nPart, int nXYMode);

// ���ڵĲ�ѯ��ö�ٺ���
int     WND_MakeHwndList(HWND *pHwnd, int nCount);
PWINOBJ WND_FindWindow(PCSTR pszClassName, PCSTR pszWindowName);
void    WND_EnumWindows(WNDENUMPROC pEnumFunc, LPARAM lParam);
void    WND_EnumChildWindows(PWINOBJ pParent, WNDENUMPROC pEnumFunc, 
                             LPARAM lParam);
PWINOBJ WND_GetWindow(PWINOBJ pWin, UINT uCmd);

// ���溯���������豸����ģ��(devinput.c)����
BOOL    WND_IsDescendant(PWINOBJ pWin, PWINOBJ pForefatherWin);
BOOL    WND_IsOwned(PWINOBJ pWin, PWINOBJ pOwnerWin);
BOOL    WND_IsDestroying(PWINOBJ pWin);

// ���溯�����ڴ��ڵ��ػ��͹��� winpaint.c

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

// ���崰�ڵĵ�ǰ��ͼ����
#define PA_CLIENT           0       // �����ͻ���
#define PA_WINDOW           1       // ��������
#define PA_UPDATECLIENT     2       // �ͻ�����Ҫ���µ�����
#define PA_UPDATEWINDOW     3       // ������Ҫ���µ�����

// ��ô��ڵĵ�ǰ��ͼ����ͻ�ͼԭ�� winpaint.c
void    WND_GetPaintRect(PWINOBJ pWin, int nPaintArea, PRECT pRect);
void    WND_GetPaintOrg(PWINOBJ pWin, int nPaintArea, PPOINT pptOrg);

// ���������������ڴ�ȡָ�����ڵĴ��������Ϣ, ʵ����winclass.c��
int     WND_GetClassName(PWINOBJ pWin, PSTR pszClassName, int nMaxCount);
WORD    WND_GetClassWord(PWINOBJ pWin, int nIndex);
WORD    WND_SetClassWord(PWINOBJ pWin, int nIndex, WORD wNewValue);
DWORD   WND_GetClassLong(PWINOBJ pWin, int nIndex);
DWORD   WND_SetClassLong(PWINOBJ pWin, int nIndex, DWORD dwNewValue);

// ������������ʵ����winpub.c��, �����ڹ���ĸ���ģ�����
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

// ���ڱ�¶��������, ʵ����winexpos.c
PEXPOSETBL WND_GetExposeTbl(PWINOBJ pWin);
PEXPOSETBL WND_MakeExposeTbl(PWINOBJ pWin);
void WND_InvalidateExposeTbl(PWINOBJ pWin);
void WND_Expose(PWINOBJ pWin, const RECT* pRect, PWINOBJ pEventWin);
void WND_InvalidateExposeTblTree(PWINOBJ pWin, const RECT* prcExpose);
void WND_InvalidateExposeTblBelow(PWINOBJ pWin, const RECT* prcExpose);

#endif //__WSOWND_H
