#include "TextFile.h"

#define IDS_OPEN					ML("Open")
#define IDS_BACK					ML("Back")
#define IDS_NOTEBROWSERTITLE		ML("Note browser")
#define ICON_NOTE			"/rom/pim/notepad/note.bmp"

typedef struct tagNoteBrowserWndGlobalData
{
	HWND hFrameWnd;
	HWND hReturnWnd;
	UINT uiReturnMsg;
	HWND hNoteList;
	PSTR	szCaption;
	PTEXTINFONODE pTextNodeHead;
	PTEXTINFONODE pTextNodeEnd;
	PTEXTINFONODE pTextNodeCur;
	HWND hView;
}NOTEBROWSERWNDGLOBALDATA;

static HBITMAP hNotePadBmp;
static	char *pContent = NULL;
BOOL NoteBrowserRegisterClass(void);
DWORD BrowserNote(HWND hFrameWnd, HWND hReturnWnd, UINT uiReturnMsg, PSTR szCaption);
static LRESULT CALLBACK NoteBrowserWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static int InitList(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
static int MoveToFirstNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
static int MoveToNextNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
static PTEXTINFO GetCurNoteInfo(PTEXTINFONODE pNodeCur);
static int GetAllNotesInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
static int AddNodeInfo(PTEXTINFO pTxtInfo, NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
static void AddNoteToList(HWND hWnd,PTEXTINFO pInfo,  BOOL MoreLong);
static HWND ViewNote(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData, HWND hWnd);
static void ReleaseAllNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData);
