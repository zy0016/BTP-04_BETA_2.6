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

//在此处用extern声明依赖项目的函数

//extern int MakeCall(const char* number, int numlen);

int WBDEP_PubMakeCall(const char* NUMBER, int numlen)
{
	int result = -1;
	
	//在此处添加依赖项目的makecall函数代码
	//result = MakeCall(NUMBER, numlen);	
	

	return result;
}

int WBDEP_PubSendDTMF   (const char* DTMF, int dtmflen)
{
	int result = -1;
	
	//((在此处添加项目相关得SendDTMF函数代码
	//result = SendDTMF(DTMF, dtmflen);
	

	return result;
}


int WBDEP_PubAddPBEntry (const char* NUMBER, int numlen, const char* NAME, int namelen)
{
	int result = -1;
	
	//((在此处添加项目相关得AddPBEntye函数代码
	//result = AddPBEntry(NUMBER, numlen,NAME,namelen);
	

	return result;
}





