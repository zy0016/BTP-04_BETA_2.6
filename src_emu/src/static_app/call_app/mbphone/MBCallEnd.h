#include "MBPublic.h"
#define DURATIONLEN  50
#define COSTLEN      50

typedef	struct tagCALLENDNODE
{
	char    dspNumber[PHONENUMMAXLEN + 1];
	char    dspName[PHONENUMMAXLEN + 1];
	char	dspTime[DURATIONLEN+1]; 
	char    dspCost[COSTLEN+1];
	struct	tagCALLENDNODE* pNext;
}CALLENDNODE, * PCALLENDNODE;

BOOL MBInsertMBEndNode(const char * pPhoneCode,const char * pName,const char * pDuration,const char * pCost);
BOOL  MBCallEndNodeWindow();
