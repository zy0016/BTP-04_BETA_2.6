 
#ifndef _RC_COMMON_H_
#define _RC_COMMON_H_

#include "winpda.h"
#include "stdio.h"
#include "stdlib.h"



#include "MBPublic.h"

#ifndef MMS_CALLEDIT_MOBIL
#define MMS_CALLEDIT_MOBIL 8 
#endif

typedef enum
{
    UNANSWER_REC = 1,   
    ANSWERED_REC = 2,   
    DIALED_REC   = 4    
}MBRECORDTYPE;

#define MAX_PHONE_RECORD    30//20              
typedef struct
{
    SYSTEMTIME  Time;                       
    SYSTEMTIME  TimeUsed;                   
    char        PhoneNo[PHONENUMMAXLEN + 1];
}CallRecord;

typedef struct
{
    int nLastHour;
    int nLastMin;
    int nLastSecond;
    int nOutHour;
    int nOutMin;
    int nOutSecond;
    int nInHour;
    int nInMin;
    int nInSecond;
}MBRecordCostTime;
BOOL    InitMBRecordData(void);
int     CallMBLogListWindow(HWND hwndCall,int itype);
BOOL    CallMBRecordCostWindow(HWND hwndCall);
int     CallMBDeleteWindow(HWND hwndCall);
BOOL    InitMBPostMsg(void);
BOOL    CallMBRecordFare(HWND hwndCall);
BOOL    CallValidPin2(HWND hwndCall);
BOOL    CallSetCost(HWND hwndCall);
BOOL    CallChargingLimitWindow(HWND hwndCall);
BOOL    CallChargingTypeWindow(HWND hwndCall);
void    GetValidPin2(char * pPin2);
void    SetPin2Change(void);
void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type);

BOOL    SaveMBRecordByPhoneNum(MBRECORDTYPE CallRecType,const char * pPhoneNum,SYSTEMTIME * pCurTime);
BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime);
BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index);
int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex);
int     GetRecorderNumber(MBRECORDTYPE CallRecType);

BOOL    ReadRecord_Cost(MBRecordCostTime * pCost);
BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost);

extern int  APP_EditSMS(HWND hWnd, const char* pszReciever, const char* pszContent);
void    CallMMSEditWnd(char *fileName, HWND hwnd, UINT msg, UINT nKind, PSTR name);
BOOL SaveMBRecordLog(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                     SYSTEMTIME * pInOutTime,SYSTEMTIME * pUsedTime);
void RegisterInfoWnd(HWND hWnd, UINT wMsg);
void GetRecentMissedCall(int * pcount);
void MessageUpdateCost(void);
BOOL UpdateCallCounters(void);
BOOL UpdateGPRSCounters();
#endif 
