/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : zlibhp.h
 *
 * Purpose  : unzip header file
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
   参数说明：
      srcdata:  待解码的数据
      len      待解码数据的长度
      password 解码的密码，为NULL时表示无密码
      codetype  编码类型：CT_DEFLATE, CT_GZIP
      outputdata  解码后的数据。该内存在该函数内分配，由调用者释放
      outputlen   解码后的数据的长度*/
   
//int DecodeContent(unsigned char *srcdata, long len, char *password, char codetype, unsigned char **outputdata, long *outputlen);

