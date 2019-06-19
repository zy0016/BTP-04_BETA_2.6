  // MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _APPFRAME_H_
#define _APPFRAME_H_

#define WND_X			0			//window X coordinate
#define WND_Y			24			//window Y coordinate
#define     WINDOW_WIDTH    176//240
#define     WINDOW_HEIGHT   196//296

#define WM_ACTIVEIF         (WM_USER+10)
#define WM_DESTROYIF        (WM_USER+11)
#define WM_HIDEIF           (WM_USER+12)
#define WM_CHECKMENU        (WM_USER+13)
#define WM_CMDLINE          (WM_USER+14)
#define WM_MOVETOTOP        (WM_USER+15)
#define WM_PAINTFRAME       (WM_USER+16)
#define WM_SETLBTNACTION    (WM_USER+20)
#define WM_SETRBTNACTION    (WM_USER+21)
#define WM_SETLBTNTEXT      (WM_USER+22)
#define WM_SETRBTNTEXT      (WM_USER+503)  //plx_pdaex.h
#define WM_SETSTRINGTABLE   (WM_USER+24)
#define WM_GETSTRINGTABLE   (WM_USER+30)

enum btnaction { post_cmd = 1, popup_menu };

typedef struct app_menu {
    unsigned short mode;
    unsigned short stat;
    unsigned long value;
    const char * pText;
} APPMENU;

typedef struct string_tbl {
    int     language;           /* Language ID */
    int     count;              /* Number of items of this string */
    const char ** strlist;      /* Chinese and english string table */
} STRINGTBL;

#define MAX_STRINGTBLS      10

#define LANG_CHINESE        0
#define LANG_ENGLISH        1

#ifdef __cplusplus
extern "C" {
#endif

LRESULT CALLBACK AppWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
HWND CreateAppFrame (LPCTSTR lpWindowName, WNDPROC wndProc);
HWND CreateControl (LPCTSTR lpClassName, LPCTSTR lpWindowName,
                    DWORD dwStyle,
					int x, int y, int nWidth, int nHeight,
					HWND hWndParent, int nChildID,
					HINSTANCE hInstance, LPVOID lParam);
HWND FindAppWindow (LPCTSTR lpClassName, LPCTSTR lpWindowName);
const char * GetStringFromID (HWND hWnd, unsigned long strID);
BOOL    SetCaptionText(HWND hWnd, PCSTR pszText);
int     GetCaptionText(HWND hWnd, PSTR pszText, int nMaxCount);
int     GetCaptionTextLength(HWND hWnd);
//int AppExec (const char * appName, const char * cmdLine, int mode);

int displayLanguage;

#ifdef __cplusplus
}
#endif

/* Define virtual keys */
#define VK_LEFTBTN      0x0D//VK_RETURN
#define VK_RIGHTBTN     0x79//0x76//VK_F7
#define VK_SELECTBTN    0x75//VK_F6
#define VK_CALL         0x70//VK_F1
#define VK_HANGUP       0x71//VK_F2


#endif // _APPFRAME_H_
