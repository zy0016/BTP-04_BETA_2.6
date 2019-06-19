#ifndef _MBCONTROL_H
#define _MBCONTROL_H
typedef	struct tagCALLWNDLISTNODE
{
	int		nIndex;
	HWND	hwnd;
	char	pClassName[50];			//class name 
	struct	tagCALLWNDLISTNODE* pNext;
	struct	tagCALLWNDLISTNODE* pPre;
}CALLWNDLISTNODE, * PCALLWNDLISTNODE;

typedef	struct	tagCALLWNDLISTBUF
{
	int			nDataNum;
	int			nCurrentIndex;

	PCALLWNDLISTNODE	pCurrentData;
	PCALLWNDLISTNODE	pDataHead;
}CALLWNDLISTBUF, *PCALLWNDLISTBUF;

PCALLWNDLISTBUF pGlobalCallListBuf;

void Set_CallWndOrder(HWND hpre, HWND hcur);
void	CALLWND_InitListBuf	(PCALLWNDLISTBUF pListBuf);
BOOL	CALLWND_AddData (PCALLWNDLISTBUF pListBuf, HWND	hwnd, const char* szClassName);
void	CALLWND_FreeListBuf (PCALLWNDLISTBUF pListBuf);
PCALLWNDLISTNODE	CALLWND_GetPDATA (PCALLWNDLISTBUF pListBuf, int nIndex );
BOOL	CALLWND_DelData (PCALLWNDLISTBUF pListBuf, int nIndex);
int		CALLWND_FindData (PCALLWNDLISTBUF pListBuf, HWND hWnd);
BOOL	CALLWND_InsertData (PCALLWNDLISTBUF pListBuf, int nIndex, PCSTR szClassName, HWND hwnd);
BOOL	CALLWND_InsertDataByHandle(PCALLWNDLISTBUF pListBuf, HWND hWnd);
BOOL	CALLWND_DeleteDataByHandle(PCALLWNDLISTBUF pListBuf, HWND hWnd);
extern void PMHideIdle(void); //plx_pdaex.h


#endif
