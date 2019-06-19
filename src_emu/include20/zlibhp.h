/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   : zlibhp.h
 *
 * Purpose : unzip header file
 *  
 * Author  : Li Chun
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Workfile::                                                      $
 * $Revision::    $     $Date::                                     $
 * 
\**************************************************************************/

typedef int write_callback(char* data, int len);


#define ZE_SUCCESS		0
#define ZE_PWDNEEDED	-1
#define ZE_PWDWRONG		-2
#define ZE_ERROR		-3
#define ZE_METHOD		-4
#define ZE_SIZE			-5

#define EOF             -1

int unzip(char* srcFile, write_callback func, char* password, unsigned long max_size);
int gzuncompress(Bytef *dest, uLongf *destLen,
				 const Bytef *source, uLong sourceLen);
/*   
   ����˵����
      srcdata:  �����������
      len      ���������ݵĳ���
      password ��������룬ΪNULLʱ��ʾ������
      codetype  �������ͣ�CT_DEFLATE, CT_GZIP
      outputdata  ���������ݡ����ڴ��ڸú����ڷ��䣬�ɵ������ͷ�
      outputlen   ���������ݵĳ���*/
   
//int DecodeContent(unsigned char *srcdata, long len, char *password, char codetype, unsigned char **outputdata, long *outputlen);
