/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : stub initialize
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "stub.h"

static StubTable *FuncList;
static int FuncListNum;

static int strcmp(char *p1,char *p2)
{
	int ret=0;
	while(*p1 && *p2 &&ret==0)
	{
		ret=*p1-*p2;
		p1++;
		p2++;
	}
	if( ret==0 &&(*p1|*p2)) //����p1,p2����һ��Ϊ��һ���Ӽ�������
		ret=*p1-*p2;

	return ret;
}
void stub_init(StubTable *pFuncList, int nFuncListNum)
{
	FuncList=pFuncList;
	FuncListNum=nFuncListNum;
}

//���ֲ��Һ�����,���غ������±�
int BinarySearch(char *strFuncName)
{
	
	int Low, Mid, High;
	Low = 0;
	High = FuncListNum - 1;
	
	while(Low <= High)
	{
		Mid = (Low + High) / 2; 
		if(strcmp(FuncList[Mid].strFuncName,strFuncName) < 0)
			Low = Mid + 1;
		else if(strcmp(FuncList[Mid].strFuncName,strFuncName)>0)
			High = Mid - 1;
		else
			return Mid;
	}
	return -1;
}
void *stub_find_func_addr(char *strFuncName)
{
	int index;
	void (*exit)(int);


	if( (index =BinarySearch(strFuncName) )!=-1 )
		return FuncList[index].FuncAddr;

	if( (index=BinarySearch("exit"))!=-1) 
	{
		//����ʧ��ʱ����exit�˳����Է�ֹ���ܳ��ֵ������쳣,
		//�˴�������Hopen3.0�ں���stub������
		exit=FuncList[index].FuncAddr;
		exit(1);
	}
	
	return 0;
}
