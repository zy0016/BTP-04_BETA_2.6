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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

//#include "PWBE.h"

/*****************************************************************
* �������ܣ���DataBuf�е����ݱ��浽�ļ���ȥ
* ���������Databuf�� Ҫ����Ϊ�ļ�������
nbuflen�� ���ݳ���
ContType������Ҫ������ļ����ͣ��˲�����������֪���ļ������λ�ã�Ŀ¼��
szDefName��Ҫ�����ȱʡ�ļ�����
* ������������
* ����    ��1���ɹ�
0��ʧ��
*****************************************************************/
int WLE_DownLoad(const char* DATABUF, long nbuflen, unsigned char ContType, const char *szMIMEName,
                 const char* SZDEFNAME,unsigned char xdrm_type);




#endif 
