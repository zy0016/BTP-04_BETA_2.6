/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include <window.h>
#include <version.h>
#include <mullang.h>
#define ID_EDIT_SAVEBACK	200
#define ID_SELECT_BACK		201
#define ID_NEW_SAVEBACK		202
#define ID_DELEOK_BACK		203
#define ID_ASBASE_BACK		204
#define ID_EditValue_Back	205
#define ID_CURR_RATE_BACK	206


#define VALUE_MAX_LENGTH		12
#define RATE_MAX_LENGTH			12
#ifdef LANGUAGE_CHN
#define CURRENCY_MAX_LENGTH		8
#else
#define CURRENCY_MAX_LENGTH		3
#endif

#define DECIMALPART				2

#define EXCH_FLASH_PATH		"/mnt/flash/exch"
#define EXCH_RATE_FILE			"/mnt/flash/exch/exch_rate.dat" 
#define EXCH_VALUE_FILE			"/mnt/flash/exch/exch_value.dat"

#define Default_Currecy1		ML("EUR")
#define Default_Currecy2		ML("USD")
#define Default_BaseCurrency	ML("EUR")

//#define RADIO_ON			"/rom/pim/exch/radioon.bmp"
//#define RADIO_OFF			"/rom/pim/exch/radiooff.bmp"

#define RADIO_OFF		"/rom/ime/radiooff.bmp"
#define RADIO_ON		"/rom/ime/radioon.bmp"

typedef struct tagExchangeMoney
{
	char	name[CURRENCY_MAX_LENGTH +1];	
	double	rate;
	BOOL	bBase;
	int		nDefault;
}EXCHMONEY;

typedef struct tagExchMoneyInfoNode{
	EXCHMONEY ExchMoneyInfo;
	struct tagExchMoneyInfoNode * pPreInfo;
	struct tagExchMoneyInfoNode * pNextInfo;
}EXCHMONEYINFONODE;



#define ExchMoneySize sizeof(EXCHMONEY)
#define ExchNodeSize sizeof(EXCHMONEYINFONODE)



int Exch_InitRateFile(void);
int Exch_GetAllNodeFromFile(void);
int Exch_AddNode(EXCHMONEY *pMoney);
int Exch_DeleteNode(EXCHMONEYINFONODE *pNode);
EXCHMONEYINFONODE *Exch_FindNode(char *pName);
int Exch_EditNode(EXCHMONEY *pWillEdit,  EXCHMONEY *pHasEdit);
void Exch_LocatePnter(int CurIndex);
BOOL Exch_SaveAllNodeToFile(void);
BOOL Exch_DeleteAllNodeToFile();
int Exch_InitValueFile();
int Exch_GetValueFromFile();
BOOL Exch_SaveTwoValueToFile();

double Exch_StrToNumber(const char *buff);
void Exch_NumberToStr(char* buf,double op, int iMaxlength);
DWORD Exch_SelectWnd(HWND hWnd,int nCurrID);
DWORD Exch_CurrAndRateWnd(HWND hWnd);

HWND hWndEditValueMain;
HWND Exch_hWndApp;
HWND hExchFrameWnd;
