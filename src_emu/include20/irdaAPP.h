/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   : IrDA.h
*
* Purpose : IrDA.c �ⲿ�ӿ�ͷ�ļ�
*  
* Author  : �ż���
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    1.0   $Date:: 2003-10-12                          $
* 
\**************************************************************************/
#define    OBEX_NUM   2

//���⴫�����ݺ�������ϵ�ˡ��ճ̵��ã�
extern BOOL App_IrdaPutData(unsigned char *strName, unsigned char *pDataBuffer, int iDataLength);
//�������˳�ʼ�����������õ��ã�
extern BOOL App_IrdaServerStart(int ObexService[], int iServiceNum);
//�����������ݴ�����    
BOOL App_IrdaDataProcesse(int ObexService[], int iServiceNum);
