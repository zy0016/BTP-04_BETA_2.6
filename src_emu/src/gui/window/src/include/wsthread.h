/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Contain the infomation about the window thread
 *            
\**************************************************************************/

#ifndef __WSTHREAD_H
#define __WSTHREAD_H

#include <string.h>
#include "osal.h"

#ifndef __WSOWND_H
#include "../kernel/wsownd.h"
#endif

#define INVALID_PROCESSID   0xFFFFFFFF
#define INVALID_THREADID    0xFFFFFFFF

#if (__MP_PLX_GUI)
/* In the MultiProcess GUI, all the mutual object use the semaphore. */
typedef SEMAPHOREOBJ MSGQUEOBJ;
typedef SEMAPHOREOBJ MONITOROBJ;
typedef SEMAPHOREOBJ BLOCKOBJ;
typedef SEMAPHOREOBJ COMMONMUTUALOBJ;

#ifndef _EMULATE_
#define INITMONITOR             SEMAPHORE_CREATE(sem_monitor)
#define EXITMONITOR             SEMAPHORE_DESTROY(sem_monitor)
#define ENTERMONITOR            SEMAPHORE_WAIT(sem_monitor)
#define LEAVEMONITOR            SEMAPHORE_RELEASE(sem_monitor)

#define INITBLOCKOBJ(sem)       SEMAPHORE_CREATE(sem)
#define DESTROYBLOCKOBJ(sem)    SEMAPHORE_DESTROY(sem)
#define BLOCK(pThreadInfo)      SEMAPHORE_WAIT((pThreadInfo)->sem_message)
#define WAKEUP(pThreadInfo)     SEMAPHORE_RELEASE((pThreadInfo)->sem_message)

#define CREATEOBJ(sem, name)    SEMAPHORE_CREATE(sem)
#define DESTROYOBJ(sem, name)   SEMAPHORE_DESTROY(sem)
#define WAITOBJ(sem, name)      SEMAPHORE_WAIT(sem)
#define RELEASEOBJ(sem, name)   SEMAPHORE_RELEASE(sem)

#define INITMSGQUEOBJ           INITMONITOR
#define DESTROYMSGQUEOBJ        EXITMONITOR
#define WAITMSGQUEOBJ           ENTERMONITOR
#define RELEASEMSGQUEOBJ        LEAVEMONITOR

#else  //_EMULATE_
#define SEMNAME_MONITOR     "$$PLXGUI_MONITOR$$"
#define SEMNAME_THREAD      "$$PLXGUI_THREAD$$_"

#define INITMONITOR             SEMAPHORE_CREATE(sem_monitor, SEMNAME_MONITOR)
#define EXITMONITOR             SEMAPHORE_DESTROY(sem_monitor)
#define ENTERMONITOR            \
{   \
    HANDLE handle;  \
    SEMAPHORE_OPEN(handle, SEMNAME_MONITOR);   \
    ASSERT(handle != NULL); \
    SEMAPHORE_WAIT(handle); \
    SEMAPHORE_DESTROY(handle);  \
}
#define LEAVEMONITOR            \
{   \
    HANDLE handle;  \
    SEMAPHORE_OPEN(handle, SEMNAME_MONITOR);   \
    ASSERT(handle != NULL); \
    SEMAPHORE_RELEASE(handle); \
    SEMAPHORE_DESTROY(handle);  \
}

#define INITBLOCKOBJ(sem)       \
{   \
    char tmp[64];   \
    sprintf(tmp, "%s0x%08X", SEMNAME_THREAD, WS_GetCurrentThreadId()); \
    SEMAPHORE_CREATE(sem, tmp); \
}
#define DESTROYBLOCKOBJ(sem)    SEMAPHORE_DESTROY(sem)
#define BLOCK(pThreadInfo)      \
{   \
    char tmp[64];   \
    HANDLE handle;  \
    sprintf(tmp, "%s0x%08X", SEMNAME_THREAD, (pThreadInfo)->dwThreadId);  \
    SEMAPHORE_OPEN(handle, tmp);    \
    ASSERT(handle != NULL); \
    SEMAPHORE_WAIT(handle); \
    SEMAPHORE_DESTROY(handle);  \
}
#define WAKEUP(pThreadInfo)     \
{   \
    char tmp[64];   \
    HANDLE handle;  \
    sprintf(tmp, "%s0x%08X", SEMNAME_THREAD, (pThreadInfo)->dwThreadId);  \
    SEMAPHORE_OPEN(handle, tmp);    \
    ASSERT(handle != NULL); \
    SEMAPHORE_RELEASE(handle);   \
    SEMAPHORE_DESTROY(handle);  \
}

#define CREATEOBJ(sem, name)       SEMAPHORE_CREATE(sem, name)
#define DESTROYOBJ(sem, name)      SEMAPHORE_DESTROY(sem)
#define WAITOBJ(sem, name)  \
{   \
    HANDLE handle;  \
    SEMAPHORE_OPEN(handle, name);    \
    ASSERT(handle != NULL); \
    SEMAPHORE_WAIT(handle); \
    SEMAPHORE_DESTROY(handle);  \
}
#define RELEASEOBJ(sem, name)   \
{   \
    HANDLE handle;  \
    SEMAPHORE_OPEN(handle, name);    \
    ASSERT(handle != NULL); \
    SEMAPHORE_RELEASE(handle); \
    SEMAPHORE_DESTROY(handle);  \
}
#endif //_EMULATE_

#elif (__MT_PLX_GUI)

/* In the MultiThread GUI, all the mutual object use the mutex. */
typedef MUTEXOBJ MONITOROBJ;
typedef MUTEXOBJ MSGQUEOBJ;
typedef SEMAPHOREOBJ BLOCKOBJ;
typedef MUTEXOBJ COMMONMUTUALOBJ;

#define INITMONITOR             MUTEX_CREATE(sem_monitor)
#define EXITMONITOR             MUTEX_DESTROY(sem_monitor)
#define ENTERMONITOR            MUTEX_WAIT(sem_monitor)
#define LEAVEMONITOR            MUTEX_RELEASE(sem_monitor)

#define INITBLOCKOBJ(sem)       SEMAPHORE_CREATE(sem)
#define DESTROYBLOCKOBJ(sem)    SEMAPHORE_DESTROY(sem)
#define BLOCK(pThreadInfo)      SEMAPHORE_WAIT((pThreadInfo)->sem_message)
#define WAKEUP(pThreadInfo)     SEMAPHORE_RELEASE((pThreadInfo)->sem_message)

#define CREATEOBJ(sem)          MUTEX_CREATE(sem)
#define DESTROYOBJ(sem)         MUTEX_DESTROY(sem)
#define WAITOBJ(sem)            MUTEX_WAIT(sem)
#define RELEASEOBJ(sem)         MUTEX_RELEASE(sem)

#define INITMSGQUEOBJ           INITMONITOR
#define DESTROYMSGQUEOBJ        EXITMONITOR
#define WAITMSGQUEOBJ           ENTERMONITOR
#define RELEASEMSGQUEOBJ        LEAVEMONITOR

#elif (__XMT_PLX_GUI)
/* In the No rap MultiThread GUI, the monitor object is needn't. */
typedef MUTEXOBJ MSGQUEOBJ;
typedef MUTEXOBJ BLOCKOBJ;
typedef MUTEXOBJ COMMONMUTUALOBJ;

#define INITMONITOR     
#define EXITMONITOR    
#define ENTERMONITOR    
#define LEAVEMONITOR    

#define INITBLOCKOBJ(sem)       SEMAPHORE_CREATE(sem)
#define DESTROYBLOCKOBJ(sem)    SEMAPHORE_DESTROY(sem)
#define BLOCK(pThreadInfo)      SEMAPHORE_WAIT((pThreadInfo)->sem_message)
#define WAKEUP(pThreadInfo)     SEMAPHORE_RELEASE((pThreadInfo)->sem_message)

#define CREATEOBJ(sem)          MUTEX_CREATE(sem)
#define DESTROYOBJ(sem)         MUTEX_DESTROY(sem)
#define WAITOBJ(sem)            MUTEX_WAIT(sem)
#define RELEASEOBJ(sem)         MUTEX_RELEASE(sem)

#define INITMSGQUEOBJ           MUTEX_CREATE(sem_msgque)
#define DESTROYMSGQUEOBJ        MUTEX_DESTROY(sem_msgque)
#define WAITMSGQUEOBJ           MUTEX_WAIT(sem_msgque)
#define RELEASEMSGQUEOBJ        MUTEX_RELEASE(sem_msgque)
#else // (__ST_PLX_GUI)
typedef MUTEXOBJ MSGQUEOBJ;
typedef MUTEXOBJ COMMONMUTUALOBJ;

/* In the single thread GUI, only the block object is used. */
#define INITMONITOR     
#define EXITMONITOR    
#define ENTERMONITOR    
#define LEAVEMONITOR    

#if ((_HOPEN_VER >= 300) || defined(_LINUX_OS_))
typedef MUTEXOBJ BLOCKOBJ;
#define INITBLOCKOBJ(sem)       MUTEX_CREATE(sem)
#define DESTROYBLOCKOBJ(sem)    MUTEX_DESTROY(sem)
#define BLOCK(pThreadInfo)      MUTEX_WAIT((pThreadInfo)->sem_message)
#define WAKEUP(pThreadInfo)     MUTEX_RELEASE((pThreadInfo)->sem_message)
#else 
typedef SEMAPHOREOBJ BLOCKOBJ;
#define INITBLOCKOBJ(sem)       SEMAPHORE_CREATE(sem)
#define DESTROYBLOCKOBJ(sem)    SEMAPHORE_DESTROY(sem)
#define BLOCK(pThreadInfo)      SEMAPHORE_WAIT((pThreadInfo)->sem_message)
#define WAKEUP(pThreadInfo)     SEMAPHORE_RELEASE((pThreadInfo)->sem_message)
#endif

#define CREATEOBJ(sem)          MUTEX_CREATE(sem)  
#define DESTROYOBJ(sem)         MUTEX_DESTROY(sem)  
#define WAITOBJ(sem)            MUTEX_WAIT(sem)  
#define RELEASEOBJ(sem)         MUTEX_RELEASE(sem)
         
#define INITMSGQUEOBJ           MUTEX_CREATE(sem_msgque)
#define DESTROYMSGQUEOBJ        MUTEX_DESTROY(sem_msgque)
#define WAITMSGQUEOBJ           MUTEX_WAIT(sem_msgque)
#define RELEASEMSGQUEOBJ        MUTEX_RELEASE(sem_msgque)

#endif

#if (__MP_PLX_GUI || __MT_PLX_GUI)
extern MONITOROBJ sem_monitor;
#elif (__ST_PLX_GUI || __XMT_PLX_GUI)
extern MSGQUEOBJ sem_msgque;
#endif

/*
** Queue status flags for GetQueueStatus() and MsgWaitForMultipleObjects()
*/
#define QS_KEY              0x0001
#define QS_MOUSEMOVE        0x0002
#define QS_MOUSEBUTTON      0x0004
#define QS_POSTMESSAGE      0x0008
#define QS_TIMER            0x0010
#define QS_PAINT            0x0020
#define QS_SENDMESSAGE      0x0040
#define QS_HOTKEY           0x0080
#define QS_ALLPOSTMESSAGE   0x0100
#define QS_REPLYMESSAGE     0x0200
#define QS_QUIT             0x8000

#define QS_MOUSE           (QS_MOUSEMOVE     | \
                            QS_MOUSEBUTTON)

#define QS_INPUT           (QS_MOUSE         | \
                            QS_KEY)

#define QS_ALLEVENTS       (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY)

#define QS_ALLINPUT        (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY        | \
                            QS_SENDMESSAGE)


/* 
** Send message type 
*/
#define SENDMSG_NOTIFY      0
#define SENDMSG_SEND        1
#define SENDMSG_REPLY       2

typedef struct tagSENDMSG
{
    MSG     msg;                    /* The MSG struct */
    BYTE    type;                   /* The sendmsg type as above define */
    DWORD   dwThreadId;             /* The thread id of the send thread */
    long    retval;                 /* The return value of send message */
    struct tagSENDMSG *pNext;       /* Pointer to the next in send queue */
    struct tagSENDMSG *pReplyNext;  /* Pointer to the next in reply queue */
}SENDMSG, *PSENDMSG, REPLYMSG, *PREPLYMSG;

typedef struct tagReplyMsgStack
{
    REPLYMSG *pTop;            /* Pointer to the top reply message */
}REPLYMSGSTACK, *PREPLYMSGSTACK;

typedef struct tagSendMsgQue
{
    SENDMSG *pFirst;            /* Pointer to the first send message */
    SENDMSG *pLast;             /* Pointer to the first send message */
    REPLYMSGSTACK ReplyMsgStack;/* The reply stack is the stack of messages 
                                ** that have been sent out.
                                */
}SENDMSGQUE, *PSENDMSGQUE;

/*
** Post message
*/
typedef struct tagPostMsg
{
    MSG     msg;                /* The MSG struct */
    DWORD   dwThreadId;         /* The thread id of the post thread */
    struct tagPostMsg *pNext;   /* Pointer to the next in the post queue */
}POSTMSG, *PPOSTMSG;

typedef struct tagPostMsgQue
{
    POSTMSG *pFirst;            /* Pointer to the first post message */
    POSTMSG *pLast;             /* Pointer to the first post message */
}POSTMSGQUE, *PPOSTMSGQUE;

/* 
** The input event queue
*/
/* Keyboard evevt struct */
typedef struct tagKBDEV 
{
    WORD	vkCode;         /* Virtual key code */
    WORD	scanCode;       /* Scan code */
    DWORD	flags;          /* Record the key state */
    DWORD	time;           /* The time when the event occured */
    DWORD	dwExtraInfo;    /* The extra infomation */
} KBDEV, *PKBDEV, FAR *LPKBDEV;

/* Mouse event struct */
typedef struct tagMSEV
{
    POINT	pt;             /* Coordinate */
    DWORD	mouseData;      /* */
    DWORD	flags;          /* Record the button state */
    DWORD	time;           /* The time when the event occured */
    DWORD	dwExtraInfo;    /* The extra infomation */
} MSEV, *PMSEV, FAR *LPMSEV;

/* Device input event struct */
typedef struct tagDEVEVENT
{
    int type;
    union
    {
        MSEV    mouse;
        KBDEV   kbd;
    } event;
} DEVEVENT, *PDEVEVENT, FAR *LDEVEVENT;

#define MAX_KBD_EVENTS  48
#define DEV_QUEUE_SIZE  (MAX_KBD_EVENTS + 16)   /*Size of dev event queue*/
#define USER_QUEUE_SIZE 16                      /*Size of user event queue*/

typedef struct tagDEVEVENTQUEUE
{
    int head;                       /* The position of first event */
    int tail;                       /* The next position of the tail event*/
    DEVEVENT events[DEV_QUEUE_SIZE];/* The array of device event */
} DEVEVENTQUEUE, *PDEVEVENTQUEUE;

typedef struct tagUSEREVENTQUEUE
{
    int head;                       /* The position of first event */
    int tail;                       /* The next position of the tail event*/
    DEVEVENT events[USER_QUEUE_SIZE];/* The array of user event */
} USEREVENTQUEUE, *PUSEREVENTQUEUE;

/* Values for type field in DEVEVENT */
#define DE_MOUSE        0x01
#define DE_KEYBOARD     0x02

/* Flag mask for event from device */
#define EVENTF_FROMDEVICE 0x80000000

typedef struct tagINPUTEVENTQUEUE
{
    DEVEVENTQUEUE    DevEventQueue;
    USEREVENTQUEUE   UserEventQueue;
//    MSEV       OldMouseEvent;
}INPUTEVENTQUEUE, *PINPUTEVENTQUEUE;

/*
** WM_PAINT message 
*/
typedef struct tagPaintMsg
{
    MSG msg;                    /* MSG struct */
    DWORD   dwThreadId;         /* the thread id */
    struct tagPaintMsg *pNext;  /* Pointer to the next paint message */
}PAINTMSG, *PPAINTMSG;

typedef struct tagPaintMsgQue
{
    PAINTMSG *pFirst;            /* Pointer to the first paint message */
    PAINTMSG *pLast;             /* Pointer to the last paint message */
}PAINTMSGQUE, *PPAINTMSGQUE;

/*
** timer message 
*/
/* Max timer num in the system */
#define MAX_TIMERS  64
/* Max timer num in a thread */
#if (__ST_PLX_GUI || __XMT_PLX_GUI)
#define MAX_THREAD_TIMERS     MAX_TIMERS
#else
#define MAX_THREAD_TIMERS     (MAX_TIMERS / 8)
#endif

/* Type of timer */
#define GENERAL_TIMER   1       // General timer
#define SYSTEM_TIMER    2       // System timer

typedef struct tagTIMERINFO
{
    UINT       TimerId;     /* timer identifier */
    BYTE       type;        /* timer type: 1:general, 2:system */
    HWND       hWnd;        /* timer owner */
    UINT       uEventId;    /* timer id */
    TIMERPROC  pTimerProc;  /* timer proc */
}TIMERINFO, *PTIMERINFO;

typedef struct tagTimerQue{
    TIMERINFO   TimerInfo[MAX_THREAD_TIMERS];
    DWORD       dwSlotMask[(MAX_THREAD_TIMERS + 31) / 32];     /* The slot flag the usage of timer */
    DWORD       dwActiveMask[(MAX_THREAD_TIMERS + 31) / 32];   /* The mask flag the active of timer */
    UINT        uPreStart; /* The start id when get timer message. Used to */
}TIMERMSGQUE, *PTIMERMSGQUE;

/*
** Hook message
*/
typedef struct tagHOOKOBJ
{
    struct tagHOOKOBJ *pNext;   /* Pointer to the next hook */
    HANDLE hHook;               /* The handle of the hook */
    HOOKPROC pfnHookProc;       /* hook call back function */
    HINSTANCE hMod;             /* module handle */
    DWORD dwThreadId;           /* Thread id of the hook */
}HOOKOBJ, *PHOOKOBJ;

typedef struct tagHookChain
{
    PHOOKOBJ pHead;             /* Chain of hook */
}HOOKCHAIN, *PHOOKCHAIN;

/*
** The threadinfo struct define.
*/

/* The type of thread block */
#define BLOCK_NO              0
#define BLOCK_INGETMESSAGE    1
#define BLOCK_INSENDMESSAGE   2

typedef struct tagThreadInfo
{
    struct tagThreadInfo *pNext; /*Pointer to the next ThreadInfo */
    DWORD           dwThreadId;  /* Thread id */
    BLOCKOBJ        sem_message; /* The block object of the window thread */
    BYTE            bBlock;      /* block state, defined above */
    int             nExitCode;   /* The Exitcode of the thread, 
                                 ** used in PostQuitMessage
                                 */

    DWORD           dwState;     /* message queue states */
    SENDMSGQUE      SendMsgQue;     /**/
    POSTMSGQUE      PostMsgQue;     /**/
    INPUTEVENTQUEUE InputEventQue;  /* input event queue */
    PAINTMSGQUE     PaintMsgQue;    /**/
    TIMERMSGQUE     TimerMsgQue;    /**/
    
    HOOKCHAIN       HookChains[WH_MAX + 1];

    /* 局部状态变量 */
//    PWINOBJ         pFirstTopWin;
//    PCARETINFO      pCaretInfo;
//    WORD            wPaintNum;
//    PWINOBJ         pPaintWin;      // 当有且仅有一个窗口需要重画时, 标识这个
                                    // 需要重画的窗口; 否则为NULL
//    CURSOR          cursor;   //cursor info of this thread
    PWINOBJ         pModalWin;
    PWINOBJ         pFocusWin;
    PWINOBJ         pKeyGrabWin;
    PWINOBJ         pActiveWin;
    PWINOBJ         pCaptureWin;
    int             nCaptureMode;
    WORD            wKeyState;
    WORD            wKeyAfterSpecKey;
    WORD            wBtnState;
}THREADINFO, *PTHREADINFO;

/* Judge the block state of a thread */
#define ISBLOCKINGET(pThreadInfo)   ((pThreadInfo)->bBlock == BLOCK_INGETMESSAGE)
#define ISBLOCKINSEND(pThreadInfo)  ((pThreadInfo)->bBlock == BLOCK_INSENDMESSAGE)
#define ISBLOCK(pThreadInfo)   ((pThreadInfo)->bBlock != BLOCK_NO)

/* Judge the existence of a thread */
#define THREAD_ISDESTROYED(dwThreadId)  (FALSE)

DWORD           WS_GetCurrentThreadId(void);
DWORD           WS_GetCurrentProcessId(void);
DWORD           WS_GetProcessIdFromThreadId(DWORD dwThreadId);
PTHREADINFO     WS_GetThreadInfo(DWORD dwThreadId);
PTHREADINFO     WS_GetCurrentThreadInfo(void);

BOOL            THREADINFO_Init(void);
void            THREADINFO_Exit(void);
PTHREADINFO     THREADINFO_Create(void);
BOOL            THREADINFO_Destroy(PTHREADINFO pThreadInfo);

BOOL            WS_ThreadInit(void);
BOOL            WS_ThreadExit(DWORD dwThreadId);

#endif //__WSTHREAD_H
