#ifndef _MENU_H
#define _MENU_H

#define DEFAULT_TIMEOUT  20

void    Menu_DeskGetRect(RECT *rcRect, RECT *rcText, int iCurr);
void    Menu_DeskGetGifRect(RECT *rcRect, int iCurr);
BOOL	Menu_GetDisplayName(char* pDisplayName, char *str, unsigned int len);
void	SetPmhandleTableItem(PCSTR pDirName);
BOOL    MenuManagerCtrl(HWND hParentWnd);
int	    Menu_PmGetDivideNum(int dividend, int divisor);

extern PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDirLink);
extern BOOL     AppRunStaticFile( PAPPADM  pAdm, PCSTR pPathName , short fileID, char bShow );
extern PAPPADM	 GetAppMain(void);
extern PKEYEVENTDATA  GetKeyTypeByCode(long vkCode);
extern PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDirLink);
extern  int		 Desk_ShowAppList(int nFatherID);
extern PDIRSLINK GetDirFromId(struct appMain * pAdm, short dirId);


#endif
