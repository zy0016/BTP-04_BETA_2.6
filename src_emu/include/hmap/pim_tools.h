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

#ifndef _PIM_TOOLS_H_
#define _PIM_TOOLS_H_

//DB_STRUCT;

typedef int (* MEMUPDATEFUNC)(unsigned char*);
typedef int (* MEMNOINSFUNC)(unsigned long, unsigned short);
typedef int (* HALFCMPFUNC)(unsigned char*, unsigned short);
typedef int (* CMPFUNC)(unsigned char*, unsigned char*);
typedef int (* NAMECMPFUNC)(unsigned char *, unsigned char *, unsigned int);
typedef int (* CMPINDEXFUNC)(unsigned short, unsigned short);

// char coding
int PIM_Tools_GetNameYX(const unsigned char* inputname,unsigned char* outname);
int PIM_Tools_GetNamePY(const unsigned char* inputname,unsigned char* outname);
const char * PIM_Tools_GetWordPY(const unsigned char* pWord);  //add by yangyoubing

// deal with record foramt
int	PIM_Tools_FullBufInitNew(const DB_STRUCT* pFields, unsigned char* pRecBuf, int nMaxLen);
int	PIM_Tools_FullBufToPackBuf(const DB_STRUCT* pFields, unsigned char* pFullBuf, int nFullLen, unsigned char* pPackBuf, int nPackLen);
int	PIM_Tools_FullBufSetField(const DB_STRUCT* pFields, unsigned char* pFullBuf, unsigned char cFieldID, unsigned char* pValue);
int	PIM_Tools_FullBufGetField(const DB_STRUCT* pFields, unsigned char* pFullBuf, unsigned char cFieldID, unsigned char* pValue, int nMaxLen);
int	PIM_Tools_FullBufSetParam(const DB_STRUCT* pFields, unsigned char* pFullBuf, 
							  unsigned char cParamID,  const unsigned char* pValue);
int	PIM_Tools_FullBufGetParam(const DB_STRUCT* pFields, const unsigned char* pFullBuf, 
							  unsigned char cParamID,    unsigned char* pValue, int nMaxLen);


int	PIM_Tools_PackBufInitDel(unsigned short nId, unsigned char* pRecBuf, int nMaxLen);
int	PIM_Tools_PackBufToFullBuf(const DB_STRUCT* pFields, unsigned char* pPackBuf, unsigned char* pFullBuf, int nFullLen);
int	PIM_Tools_PackBufToTEBuf(const unsigned char* pPackBuf, unsigned char* pSimBuf, int nSimLen);
int	PIM_Tools_TEBufToPackBuf(const unsigned char* pSimBuf, unsigned char* pPackBuf, int nPackLen);
int PIM_Tools_PackBufToPackBuf(const DB_STRUCT* pFields, unsigned char* pPackBuf, unsigned char* pDestBuf, unsigned long nMask, int nDestLen);
int	PIM_Tools_PackBufGetField(const unsigned char* pPackBuf, unsigned char cFieldID, unsigned char* pValue, int nMaxLen);
int	PIM_Tools_PackBufGetParam(const unsigned char* pPackBuf, unsigned char cParamID, 
							  void* pValue,   int nMaxLen);
int	PIM_Tools_PackToSim(const unsigned char* pPackBuf, unsigned char* pSimBuf, int nSimLen);
int	PIM_Tools_SimToPack(const unsigned char* pPackBuf, unsigned char* pSimBuf, int nSimLen);

int	PIM_Tools_PackBufSetParam(unsigned char* pPackBuf,  unsigned char cParamID, 
							const  unsigned char* pValue,   int nMaxLen);

int PIM_Tools_GetFieldIndex(const DB_STRUCT* pFields, unsigned char cFieldID);
int PIM_Tools_FieldIdInMask(unsigned char nId, unsigned long nFieldMask);

int PIM_Tools_HeapSort(unsigned char* pArray, CMPFUNC cmpf,int, int nLen);
int PIM_Tools_HeapSortIndex(unsigned short* pArray, CMPINDEXFUNC cmpf, int nLen);
int PIM_Tools_FindInsert(unsigned char* pArray, HALFCMPFUNC halfcmpf, unsigned char* pData, int nItemLen, int nLen);

int PIM_Tools_GetFileSize(void* fp);
int	PIM_Tools_StrLen(unsigned char* pChar);
int	PIM_Tools_Strcpy(unsigned char* pDest, unsigned char* pSource);

void		   PIM_Tools_SetUShort(unsigned char* pChar, unsigned short nShort);
void		   PIM_Tools_SetULong(unsigned char* pChar, unsigned long nLong);
unsigned short PIM_Tools_GetUShort(const unsigned char* pChar);
unsigned long  PIM_Tools_GetULong(const unsigned char* pChar);

int PIM_Tools_ValidDate(int nYear, int nMonth, int nDay);
int PIM_Tools_ValidTime(int nHour, int nMinute, int nSecond);

unsigned long  PIM_Tools_GetDate(int nYear, int nMonth, int nDay);
unsigned long  PIM_Tools_GetTime(int nHour, int nMinute, int nSecond);

int PIM_Tools_DivDate(unsigned long nDate, int* nYear, int* nMonth, int* nDay);
int PIM_Tools_DivTime(unsigned long nTime, int* nHour, int* nMinute, int* nSecond);

short PIM_Tools_DateStrToStr(const char *DateStr, char *year, char *month, char *day);
short PIM_Tools_TimeStrToStr(const char *TimeStr, char *hour, char *minute, char *second);
short PIM_Tools_DateIsNum(char *year, char *month, char *day, int Date[]);

unsigned long  PIM_Tools_ParseFieldMask(const DB_STRUCT* pFields, const unsigned char* pFieldString);
unsigned char  PIM_Tools_ParseFieldName(const DB_STRUCT* pFields, const char* pFieldName);
int			   PIM_Tools_ParsePattern(const DB_STRUCT* pFields, const char* pPattern, unsigned char* cPattern, int nMaxlen);
int	           PIM_Tools_ParseNo(unsigned char* pNo, unsigned short nId, MEMNOINSFUNC pNoInsFunc);

unsigned long  PIM_Tools_PhoneStrToULong(unsigned char* pChar);
unsigned long  PIM_Tools_PhoneStrToAppend(unsigned char* pChar);

#endif //_PIM_TOOLS_H_