/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TFTP Server Module
 *
 * Purpose  : TFTP server api layer
 *            
**************************************************************************/
#include "../../include/stdio.h"
#include "hpimage.h"
#include "winpda.h"
#include "di_uart.h"
#include "window.h"    
#include "device.h"  
#include "fapi.h"
#include "malloc.h"
#include "pubapp.h"
#include "string.h"
#include "plx_pdaex.h" 
#include "tsmain.h"
#include "errno.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dirent.h"
#include "calendar/Calendar.h"
#include "ab/PhonebookExt.h"

extern BOOL RealtimeUpdatePicture(void);
extern BOOL MMC_CheckCardStatus(void);

extern BOOL Tftp_IsConnected;
static void TS_Decode_PutClose(void);
void       TS_Decode_GetStop(void);
extern     TS_FTPBLOCK TS_ftpblock;
static int TS_Decode_Option( BYTE * Data );
static int TS_Decode_Dir( BYTE * Data );
static int TS_Decode_Get( BYTE * Data );
static void TS_Decode_Unknow(void);
static void TS_Decode_HugeBegin();
static void TS_Decode_HugeEnd();
static int TS_Decode_GetData( BYTE * Data );
static int TS_Decode_Put( BYTE * Data );
static int TS_Decode_PutData( BYTE * Data );
static int TS_Decode_Delete( BYTE * Data );
static int TS_Decode_Rename(BYTE * Data);
static int TS_Decode_RmDir(BYTE * Data);
static int TS_Decode_FlashFreeSpace(BYTE * Data);
static int TS_Decode_CalenderAdd(BYTE * Data);
static int TS_Decode_CalenderDel(BYTE *Data);
static int TS_Decode_CalenderModify(BYTE *Data);
static int TS_Decode_ConnectAdd(BYTE *Data);
static int TS_Decode_ConnectModify(BYTE *Data);
static int TS_Decode_ConnectDel(BYTE *Data);
	  void TS_Decode_MMCIsWork(void);
	  void TS_Decode_PicNotify(void);
	  void TS_AtDemo(BYTE * Data);
extern int TS_GetPacket( void );

static int TS_Decode_CreateDir(BYTE * Data);
extern int TS_ReplyPacket( BYTE Opcode, short ErrorCode, BYTE *Data, short Length );
short	   TS_Data_Transparentfilter( BYTE * DestBuf, short Destlen, BYTE * SrcBuf, short Srclen );
static int TS_Dir_GetFile( PTSR_DIR_FILE pDirFile, char *szPath, unsigned short index);




//-----------------Recv Packet--------------------

extern int TS_GetPacket( void )
{
	PTS_PACKETBUF pPacketBuf;
	PTS_PACKETBUF pReplyBuf;
	
	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);
	pReplyBuf = &(TS_ftpblock.TS_ReplyBuf);

	// fill the answer packet
	pReplyBuf->PackHead = 0x00;
	pReplyBuf->Opcode = pPacketBuf->Opcode;
	pReplyBuf->Serial = SwapLong(pPacketBuf->Serial);

	switch( pPacketBuf->Opcode )
	{
	case TS_OP_OPTION:
		TS_Decode_Option( pPacketBuf->pPacketData );
		break;
	case TS_OP_DIR:
		TS_Decode_Dir( pPacketBuf->pPacketData );
		break;
	case TS_OP_GET:
		TS_Decode_Get( pPacketBuf->pPacketData );
		break;
	case TS_OP_GETDATA:
		TS_Decode_GetData( pPacketBuf->pPacketData );
		break;
	case TS_OP_PUT:
		TS_Decode_Put( pPacketBuf->pPacketData );
		break;
	case TS_OP_PUTDATA:
		TS_Decode_PutData( pPacketBuf->pPacketData );
		break;
	case TS_OP_DELETE:
		TS_Decode_Delete( pPacketBuf->pPacketData );
		break;
	case TS_OP_CREATEDIR:
		TS_Decode_CreateDir( pPacketBuf->pPacketData );
		break;
	case TS_OP_ATDEMO:
		TS_AtDemo(pPacketBuf->pPacketData);
		break;
	case TS_OP_RENAME:
		TS_Decode_Rename(pPacketBuf->pPacketData);
		break;
	case TS_OP_DELDIR: 
		TS_Decode_RmDir(pPacketBuf->pPacketData);
		break;
	case TS_OP_FLASHSPACE:
		TS_Decode_FlashFreeSpace(pPacketBuf->pPacketData);
		break;
	case TS_PIC_MODIFY:
		TS_Decode_PicNotify();
		break;
	case TS_CALENDER_ADD:
		TS_Decode_CalenderAdd(pPacketBuf->pPacketData);
		break; 
	case TS_CALENDER_DEL:
		TS_Decode_CalenderDel(pPacketBuf->pPacketData);
		break;
	case TS_CALENDER_MODIFY:
		TS_Decode_CalenderModify(pPacketBuf->pPacketData);
		break;
	case TS_CONNECT_ADD:
		TS_Decode_ConnectAdd(pPacketBuf->pPacketData);
		break;
	case TS_CONNECT_MODIFY:
		TS_Decode_ConnectModify(pPacketBuf->pPacketData);
		break;
	case TS_CONNECT_DEL:
		TS_Decode_ConnectDel(pPacketBuf->pPacketData);
		break;
	case TS_OP_MMC_REQUEST:
		TS_Decode_MMCIsWork();
		break;
	case TS_OP_SENDSTOP:
		TS_Decode_PutClose();
		break;
	case TS_OP_GETSTOP: 
		TS_Decode_GetStop();
		break;
	case TS_CONTECT_HUGE_BEGIN: 
		TS_Decode_HugeBegin();
		break;
	case TS_CONTECT_HUGE_END: 
		TS_Decode_HugeEnd();
		break;
	default:
		TS_Decode_Unknow();
		break;
	}

	return 0;
}

//----------------Option----------------------

static int TS_Decode_Option( BYTE * Data )
{
	short	Version = 0x10;
	BYTE	TSR_OptionStr[32];

	TSR_OptionStr[0]= RESTAG_Version;
	TSR_OptionStr[1] = 2;
	TSR_OptionStr[2] = (unsigned char)(Version >> 8);
	TSR_OptionStr[3] = (unsigned char)(Version & 0xFF);

	TSR_OptionStr[4]= RESTAG_Length;
	TSR_OptionStr[5] = 2;
	TSR_OptionStr[6] = TS_PACKBUF_LENGTH >> 8;
	TSR_OptionStr[7] = TS_PACKBUF_LENGTH & 0xFF;

	TS_ReplyPacket(TS_OP_OPTION, E_NOERROR, TSR_OptionStr, 8);


	Tftp_IsConnected = TRUE;	// have connected with pc
	return 0;
}

void TS_AtDemo(BYTE * Data)
{

}


//--------------------Dir------------------------
static int TS_Decode_Dir( BYTE * Data )
{
	TSP_DIR TSP_Dir;
	PTSR_DIR_FILE	pDirFile;
	TSR_DIRGROUP	TSR_DirGroup;
	short	x;
	BYTE	ret;

	PTS_PACKETBUF pPacketBuf = &(TS_ftpblock.TS_PacketBuf);

	memcpy( &TSP_Dir, Data, sizeof(TSP_DIR) );
	TSP_Dir.Index = SwapLong(TSP_Dir.Index);		
	TSP_Dir.Path[pPacketBuf->Length-2] = 0;

	x = 0;
	while( x< TSR_DIRFILE_MAXCOUNT )
	{
		pDirFile = &(TSR_DirGroup.Dir_File[x]);

		ret = (BYTE)TS_Dir_GetFile(pDirFile, TSP_Dir.Path, (unsigned short)TSP_Dir.Index++);

		if( ret == E_NOERROR )
		{
			//pDirFile->size = SwapLong(pDirFile->size);
			x++;
			continue;
		}

		if( ret == E_PATH )
		{
			TS_ReplyPacket( TS_OP_DIR, ret, NULL, 0 );
		}
		break;

	}

	TSR_DirGroup.count = x;

	TS_ReplyPacket( TS_OP_DIR, ret, (BYTE*)&TSR_DirGroup, (short)(4+sizeof(TSR_DIR_FILE)*x) );
	return 0;
}


static int	TS_Dir_GetFile( PTSR_DIR_FILE pDirFile, char *szPath, unsigned short index)
{
	static HANDLE	hFile;
	DIR				*pdir;
	struct dirent   *pdirent;
	int				i = 0;
	char			path[TSP_PATHLEN];
	struct stat		filestat;
	
	if (strlen(szPath) == 0)
		return E_PATH;

	//打开目录
	pdir = opendir(szPath);

	if (NULL == pdir)
		return E_PATH;

	for(;; i++)
	{
		pdirent = readdir(pdir);
		if (pdirent == NULL)	//即已经读完
		{
			closedir(pdir);	//关闭目录
			return E_OUTRANGE;
		}

		if (index != i)			//如果不是需要的文件则跳过
			continue;

		{
			//填写文件名
			strcpy(pDirFile->FileName, pdirent->d_name);

			//为查询文件状态准备全文件名
			strcpy(path, szPath);
			strcat(path, "/");
			strcat(path, pDirFile->FileName);

			stat(path, &filestat);

			//填写文件最后修改日期
			pDirFile->modifyTime = filestat.st_mtime;
			
			pDirFile->size = (unsigned long)-2;

			if (S_ISREG(filestat.st_mode)) 
				pDirFile->size = filestat.st_size;
			else if(S_ISDIR(filestat.st_mode)) 
				pDirFile->size = (unsigned long)-1;
			
			closedir(pdir);	//关闭目录
			return E_NOERROR;
		}
	
	}

	closedir(pdir);	//关闭目录
	return E_NOERROR;
	
}


//------------------Get---------------------------
static int TS_Decode_Get( BYTE * Data )
{
	TSP_GET TSP_Get;
	TSR_GET TSR_GetReply;
	struct stat	filestat;
	int			iRet;
	FILE		*pFile;
	
	PTS_TRANSNODE	pTS_TransNode = &(TS_ftpblock.TS_TransNode);
	PTS_PACKETBUF	pPacketBuf = &(TS_ftpblock.TS_PacketBuf);
	
	if((HANDLE)USELESS_FILEPOINT != pTS_TransNode->pFile)
	{
		fclose(pTS_TransNode->pFile);
		pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
	}
	
	memcpy(&TSP_Get, Data, sizeof(TSP_GET));
	TSP_Get.Path[pPacketBuf->Length] = 0;
	
	//Get File Info
	iRet = stat(TSP_Get.Path, &filestat);
	
	if(-1 == iRet)
	{
		TS_ReplyPacket( TS_OP_GET, E_NOTFOUND, NULL, 0);
		return 0;
	}
	
	pFile = fopen(TSP_Get.Path, "rb");
	
	if (NULL == pFile) 
	{
		TS_ReplyPacket( TS_OP_GET, E_NOTFOUND, NULL, 0);
		return 0;
	}
	else
		pTS_TransNode->pFile = pFile;

	// renew trID
	pTS_TransNode->trID ++;
	strcpy(pTS_TransNode->szFileName, TSP_Get.Path);
	pTS_TransNode->pFile = pFile;
	
	TSR_GetReply.trID = pTS_TransNode->trID;
	TSR_GetReply.filesize = filestat.st_size;
	
	// Response
	TS_ReplyPacket(TS_OP_GET, E_NOERROR, (BYTE*)&TSR_GetReply, sizeof(TSR_GET));
	return 0;
}

//---------------GetData----------------------------

static int TS_Decode_GetData( BYTE * Data )
{
	TSP_GETDATA		TSP_GetData;
	TSR_GETDATA		TSR_GetDataReply;
	PTS_TRANSNODE	pTS_TransNode;
    FILE			*pFile;
	int				nBytesRead;
	
	pTS_TransNode = &(TS_ftpblock.TS_TransNode);
	
	memcpy( &TSP_GetData, Data, sizeof(TSP_GETDATA) );
	TSP_GetData.trID = SwapLong(TSP_GetData.trID);
	TSP_GetData.offset = SwapLong(TSP_GetData.offset);
	TSP_GetData.length = SwapLong(TSP_GetData.length);

	// trID
	if( TSP_GetData.trID != pTS_TransNode->trID )
	{
		TS_ReplyPacket( TS_OP_GETDATA, E_NOTFOUND, NULL, 0);
		return 0;
	}
	
	pFile = pTS_TransNode->pFile;

	if ((nBytesRead = fread(TSR_GetDataReply.Getdata, sizeof(BYTE), TSP_GetData.length, pFile)) == 0)
	{
		if (0 != feof(pFile))
			TS_ReplyPacket( TS_OP_GETDATA, E_OUTRANGE, NULL, 0);
		else if (0 != ferror(pFile)) 
		{
			printf("Read File Error !");
			TS_ReplyPacket( TS_OP_GETDATA, E_READERROR, NULL, 0);
		}
		
		if ((HANDLE)USELESS_FILEPOINT != pTS_TransNode->pFile) 
		{
			fclose(pFile);
			pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
		}
		
		return 0;
	}
	
	TSR_GetDataReply.trID = TSP_GetData.trID;
	
	printf("Send Data !");
	TS_ReplyPacket( TS_OP_GET, E_NOERROR, (BYTE*)&TSR_GetDataReply, (short)(nBytesRead+4));
	return 0;
}


//-------------------Put---------------------------
static int TS_Decode_Put( BYTE * Data )
{
	TSP_PUT	TSP_Put;
	TSR_PUT	TSR_PutReply;
	struct  stat	filestat;
	int				iRet;
	FILE			*pFile;
	
	PTS_TRANSNODE	pTS_TransNode = &(TS_ftpblock.TS_TransNode);
	PTS_PACKETBUF pPacketBuf = &(TS_ftpblock.TS_PacketBuf);
	
	if((HANDLE)USELESS_FILEPOINT != pTS_TransNode->pFile)
	{
		fclose(pTS_TransNode->pFile);
		pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
	}
	
	memcpy( &TSP_Put, Data, sizeof(TSP_PUT));
	TSP_Put.mode = SwapShort(TSP_Put.mode);
	TSP_Put.size = SwapLong(TSP_Put.size);
	TSP_Put.Path[pPacketBuf->Length-8] = 0;
	
	
	// If File Exist
	iRet = stat(TSP_Put.Path, &filestat);
	
	if(-1 == iRet)	//Not Found
	{
		if( TSP_Put.mode == TC_PUTTYPE_COVER )
		{
			TS_ReplyPacket( TS_OP_PUT, E_NOTFOUND, NULL, 0);
			return 0;
		}
		if(NULL == (pFile = fopen(TSP_Put.Path, "w+b")))
		{
			TS_ReplyPacket(TS_OP_PUT, E_CREATEERROR, NULL, 0);
			return 0;
		}
		pTS_TransNode->pFile = pFile;

	}
	
	if(-1 != iRet )	//Found the File
	{
		if(TSP_Put.mode == TC_PUTTYPE_NOEXIST)
		{
			TS_ReplyPacket( TS_OP_PUT, E_EXIST, NULL, 0);
			return 0;
		}

		if(NULL == (pFile = fopen(TSP_Put.Path, "w+b")))
		{
			TS_ReplyPacket(TS_OP_PUT, E_CREATEERROR, NULL, 0);
			return 0;
		}
		pTS_TransNode->pFile = pFile;
	}
	
	// renew trID
	pTS_TransNode->trID ++;
		
	//TSR_PutReply.trID = SwapLong(pTS_TransNode->trID);
	TSR_PutReply.trID =	pTS_TransNode->trID;
	TS_ReplyPacket( TS_OP_PUT, E_NOERROR, (BYTE*)&TSR_PutReply, sizeof(TSR_PUT));
	return 0;
}


static int TS_Decode_PutData( BYTE * Data )
{
	TSP_PUTDATA TSP_PutData;
	TSR_PUTDATA TSR_PutDataReply;
	long			nBytesWrite;
	FILE			*pFile;
	PTS_TRANSNODE	pTS_TransNode;
	
	pTS_TransNode = &(TS_ftpblock.TS_TransNode);
	pFile = pTS_TransNode->pFile;
	
	memcpy( &TSP_PutData, Data, sizeof(TSP_PUTDATA) );
	TSP_PutData.trID = SwapLong(TSP_PutData.trID);
	TSP_PutData.offset = SwapLong(TSP_PutData.offset);
	
	// trID
	if( TSP_PutData.trID != pTS_TransNode->trID )
	{
		TS_ReplyPacket( TS_OP_PUTDATA, E_NOTFOUND, NULL, 0);
		return 0;
	}

	if (pFile == NULL) 
	{
		TS_ReplyPacket( TS_OP_PUTDATA, E_WRITEFILEERROR, NULL, 0);
		return 0;
	}
	
	if ((nBytesWrite = fwrite(TSP_PutData.PutData, sizeof(BYTE), TS_ftpblock.TS_PacketBuf.Length-8, pFile)) <0)
	{
		if ((HANDLE)USELESS_FILEPOINT != pFile) 
		{
			fclose(pFile);
			pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
		}
		
		printf("Write File Error");
		TS_ReplyPacket( TS_OP_PUTDATA, E_WRITEFILEERROR, NULL, 0);
		return 0;
	}
	
	TSR_PutDataReply.nRecvDataLen = nBytesWrite;
	TS_ReplyPacket( TS_OP_GET, E_NOERROR, (BYTE*)&TSR_PutDataReply, sizeof(TSR_PUTDATA));
	return 0;
}

static int TS_Decode_Delete( BYTE * Data )
{
	TSP_DELETE TSP_Delete;

	memcpy(&TSP_Delete.Path, Data, sizeof(TSP_DELETE) );
	
	if(-1 == remove(TSP_Delete.Path))
		TS_ReplyPacket( TS_OP_DELETE, E_NOTFOUND, NULL, 0);
	else
		TS_ReplyPacket( TS_OP_DELETE, E_NOERROR, NULL, 0);
	
	return 0;
}

static int TS_Decode_CreateDir( BYTE * Data )
{
	TSP_CREATEDIR TSP_CreateDir;
	PTS_PACKETBUF pPacketBuf = &(TS_ftpblock.TS_PacketBuf);

	memcpy( &TSP_CreateDir, Data, sizeof(TSP_CREATEDIR) );
	TSP_CreateDir.Path[pPacketBuf->Length] = 0;

	if(-1 == mkdir(TSP_CreateDir.Path, 0))
	{
		TS_ReplyPacket( TS_OP_CREATEDIR, E_CREATEDIR, NULL, 0);
		return 0;
	}

	TS_ReplyPacket( TS_OP_CREATEDIR, E_NOERROR, NULL, 0);
	return 0;
}


//-------------Reply Packet--------------
extern int TS_ReplyPacket( BYTE Opcode, short ErrorCode, BYTE *Data, short Length )
{
	//short len;
	int				len;
	unsigned short	cCheckCrc;
	PTS_PACKETBUF	pReplyBuf;
	PTS_SENDBUF		pTS_Sendbuf;
	BYTE *			pTemp;

	pReplyBuf = &(TS_ftpblock.TS_ReplyBuf);
	pTS_Sendbuf = &(TS_ftpblock.TS_Sendbuf);
 
	pTemp = pReplyBuf->pPacketData;

	pReplyBuf->Length  = SwapShort(Length);
	pReplyBuf->Errcode = SwapShort(ErrorCode);

	//包头
	*pTemp = 0xFE;
	memcpy( pTemp+1, (BYTE*)pReplyBuf, 12);
	len = 13;

	if( Data != NULL && Length != 0 )	//数据段
	{
		memcpy(pTemp+len, Data, Length );
		len += Length;
	}

	cCheckCrc = docrc( pTemp+13,  Length);

	memcpy( pTemp+len, &cCheckCrc, 2 );
	*(pTemp+len) = (BYTE)(cCheckCrc >> 8);
	*(pTemp+len+1) = (BYTE)(cCheckCrc & 0xFF);
	len += 2;

	pTS_Sendbuf->length = TS_Data_Transparentfilter( pTS_Sendbuf->pSendData, TS_RECVBUF_LENGTH, pTemp, (short)len );

	//包尾
	*(pTS_Sendbuf->pSendData + 0 + pTS_Sendbuf->length) = 0xF0;
	*(pTS_Sendbuf->pSendData + 1 + pTS_Sendbuf->length) = 0xFE;
	*(pTS_Sendbuf->pSendData + 2 + pTS_Sendbuf->length) = 0xF0;
		
	ts_WriteFile( TS_ftpblock.Devhandle, pTS_Sendbuf->pSendData, pTS_Sendbuf->length +3);
	

	return 0;
}

//如果数据段中有0xFE将扩充为两个
short TS_Data_Transparentfilter( BYTE * DestBuf, short Destlen, BYTE * SrcBuf, short Srclen )
{
	short i,j;

	// Packet Head 0xFE
	*DestBuf = *SrcBuf;

	for( i=1,j=1; i<Destlen,j<Srclen; i++,j++ )
	{
		*(DestBuf+i) = *(SrcBuf+j);
		if( *(DestBuf+i) == 0xFE )
		{
			i++;
			*(DestBuf+i) = 0xFE;
		}
	}

	return i;
}

static int TS_Decode_Rename(BYTE * Data)
{
	TSR_RENAME	TSR_Rename;
	memcpy(&TSR_Rename, Data, sizeof(TSR_RENAME));
	
	if (-1 == rename(TSR_Rename.Old_Name, TSR_Rename.New_Name)) 
		TS_ReplyPacket(TS_OP_RENAME, E_RENAEM, NULL, 0);
	else 
		TS_ReplyPacket(TS_OP_RENAME, E_NOERROR, NULL, 0);

	return 0;
}

static int TS_Decode_RmDir(BYTE * Data)
{
	TSR_RMDIR	TSR_RmDir;
	memcpy(&TSR_RmDir, Data, sizeof(TSR_RMDIR));
	
	if (-1 == rmdir(TSR_RmDir.m_Path)) 
		TS_ReplyPacket(TS_OP_DELDIR, E_RMDIR, NULL, 0);
	else 
		TS_ReplyPacket(TS_OP_DELDIR, E_NOERROR, NULL, 0);
	
	return 0;
}

static int TS_Decode_FlashFreeSpace(BYTE * Data)
{
	unsigned long space;
	short type;

	memcpy(&type, Data, sizeof(short));
	type = SwapShort(type);

	switch(type)
	{
	case 0:
		space =GetAvailFlashSpace();
		break;
	case 1:
		space =GetAvailMMCSpace();
		break;
	default:
		break;
	}

	TS_ReplyPacket(TS_OP_FLASHSPACE, E_NOERROR, (BYTE *)&space, sizeof(unsigned long));
	return 0;
}	

void TS_Decode_PicNotify()
{	
	if (RealtimeUpdatePicture()) 
		TS_ReplyPacket(TS_PIC_MODIFY, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_PIC_MODIFY, E_PICUPDATA, NULL, 0);	
}

void TS_Decode_HugeBegin()
{	
	TS_ReplyPacket(TS_CONTECT_HUGE_BEGIN, 0, NULL, 0);	
}

void TS_Decode_HugeEnd()
{	
	TS_ReplyPacket(TS_CONTECT_HUGE_END, 0, NULL, 0);
}

static int TS_Decode_CalenderAdd(BYTE *Data)
{
	int	ret;
	TSP_CalenderAdd	TSP_CalenAdd;
	memcpy(&TSP_CalenAdd, Data, sizeof(TSP_CalenderAdd));

	TSP_CalenAdd.CaleEntry = SwapLong(TSP_CalenAdd.CaleEntry);
	printf("%d", TSP_CalenAdd.CaleEntry);	//for test
	
	ret = CALE_ExternAddApi(TSP_CalenAdd.CaleEntry, TSP_CalenAdd.pRecord);
	
	if (-1 != ret) 
		TS_ReplyPacket(TS_CALENDER_ADD, E_NOERROR, (BYTE *)&ret, sizeof(int));
	else
		TS_ReplyPacket(TS_CALENDER_ADD, E_CALENDERADD, NULL, 0);

	return 0;
}

static int TS_Decode_CalenderDel(BYTE *Data)
{
	TSP_CalenderDel	TSP_CalenDel;
	memcpy(&TSP_CalenDel, Data, sizeof(TSP_CalenderDel));
	
	TSP_CalenDel.CaleEntry = SwapLong(TSP_CalenDel.CaleEntry);
	TSP_CalenDel.CaleID = SwapLong(TSP_CalenDel.CaleID);

	if (CALE_ExternDelApi(TSP_CalenDel.CaleEntry, TSP_CalenDel.CaleID)) 
		TS_ReplyPacket(TS_CALENDER_DEL, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_CALENDER_DEL, E_CALENDERDEL, NULL, 0);
	
	return 0;
}

static int TS_Decode_CalenderModify(BYTE *Data)
{
	int	size;
	TSP_CalenderModify	TSP_CModify;

	memcpy(&TSP_CModify, Data, sizeof(TSP_CalenderModify));
	
	TSP_CModify.CaleEntry = SwapLong(TSP_CModify.CaleEntry);
	TSP_CModify.CaleID = SwapLong(TSP_CModify.CaleID);

	switch(TSP_CModify.CaleEntry) 
	{
	case 1:		//CALE_MEETING
		size = sizeof(CaleMeetingNode);
		break;
	case 2:		//CALE_EVENT
		size = sizeof(CaleEventNode);
		break;
	case 3:		//CALE_ANNI
		size = sizeof(CaleAnniNode);
		break;
	default:
		TS_ReplyPacket(TS_CALENDER_MODIFY, E_CALCEDERMODIFY, NULL, 0);
		return 0;
	}

	if (CALE_ExternModApi(TSP_CModify.CaleEntry, TSP_CModify.CaleID, TSP_CModify.pRecord, size)) 
		TS_ReplyPacket(TS_CALENDER_MODIFY, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_CALENDER_MODIFY, E_CALCEDERMODIFY, NULL, 0);
	
	return 0;
}

static int TS_Decode_ConnectAdd(BYTE *Data)
{
	TSP_ConnectAdd	TSP_Connect;
	DWORD			PID;

	memcpy(&TSP_Connect, Data, sizeof(TSP_ConnectAdd));
	
	TSP_Connect.DataLen = SwapLong(TSP_Connect.DataLen);

	if (APP_AddApi(&PID, TSP_Connect.Data, TSP_Connect.DataLen)) 
		TS_ReplyPacket(TS_CONNECT_ADD, E_NOERROR, (unsigned char *)&PID, sizeof(DWORD));
	else
		TS_ReplyPacket(TS_CONNECT_ADD, E_CONNECTADD, NULL, 0);
	
	return 0;
}

static int TS_Decode_ConnectModify(BYTE *Data)
{
	TSP_ConnectModify	TSP_ConnectMod;

	memcpy(&TSP_ConnectMod, Data, sizeof(TSP_ConnectModify));
	
	//Process 
	TSP_ConnectMod.ConnectID = SwapLong(TSP_ConnectMod.ConnectID);
	TSP_ConnectMod.DataLen = SwapLong(TSP_ConnectMod.DataLen);

	printf("connect_modift id = %d   len = %d",  TSP_ConnectMod.ConnectID, TSP_ConnectMod.DataLen);

	//printf(TSP_ConnectMod.Data);
	if (APP_ModifyApi(TSP_ConnectMod.ConnectID, TSP_ConnectMod.Data, TSP_ConnectMod.DataLen)) 
		TS_ReplyPacket(TS_CONNECT_MODIFY, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_CONNECT_MODIFY, E_CONNECTMODIFY, NULL, 0);
	
	return 0;
}

static int TS_Decode_ConnectDel(BYTE *Data)
{
	TSP_ConnectDelete	TSP_ConnectDel;
	
	memcpy(&TSP_ConnectDel, Data, sizeof(TSP_ConnectDelete));
	
	TSP_ConnectDel.ConnectID = SwapLong(TSP_ConnectDel.ConnectID);
	printf("Connect_del id = %d", TSP_ConnectDel.ConnectID);

	if (APP_DeleteApi(TSP_ConnectDel.ConnectID)) 
		TS_ReplyPacket(TS_CONNECT_DEL, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_CONNECT_DEL, E_CONNECTDEL, NULL, 0);
	
	return 0;
}

void TS_Decode_MMCIsWork(void)
{
	if (MMC_CheckCardStatus()) 
		TS_ReplyPacket(TS_OP_MMC_REQUEST, E_NOERROR, NULL, 0);
	else
		TS_ReplyPacket(TS_OP_MMC_REQUEST, E_MMC, NULL, 0);	
}

static void TS_Decode_PutClose()
{
	PTS_TRANSNODE	pTS_TransNode;
	pTS_TransNode = &(TS_ftpblock.TS_TransNode);	
	
	if ((HANDLE)USELESS_FILEPOINT != pTS_TransNode->pFile)
	{
		fclose(pTS_TransNode->pFile);
		pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
	}

	TS_ReplyPacket(TS_OP_SENDSTOP, E_NOERROR, NULL, 0);
	
}

void TS_Decode_GetStop(void)
{
	PTS_TRANSNODE	pTS_TransNode;
	pTS_TransNode = &(TS_ftpblock.TS_TransNode);
	
	if ((HANDLE)USELESS_FILEPOINT != pTS_TransNode->pFile) 
	{
		fclose(pTS_TransNode->pFile);
		pTS_TransNode->pFile = (HANDLE)USELESS_FILEPOINT;
	}
	
	TS_ReplyPacket(TS_OP_GETSTOP, E_NOERROR, NULL, 0);
}

void TS_Decode_Unknow()
{
	TS_ReplyPacket(TS_OP_UNKONW, E_NOERROR, NULL, 0);
}

// End of progra

