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
* 函数介绍：将DataBuf中的数据保存到文件中去
* 传入参数：Databuf： 要保存为文件的数据
nbuflen： 数据长度
ContType：数据要保存的文件类型，此参数可以用来知道文件保存的位置（目录）
szDefName：要保存的缺省文件名称
* 传出参数：无
* 返回    ：1：成功
0：失败
*****************************************************************/
int WLE_DownLoad(const char* DATABUF, long nbuflen, unsigned char ContType, const char *szMIMEName,
                 const char* SZDEFNAME,unsigned char xdrm_type);




#endif 
