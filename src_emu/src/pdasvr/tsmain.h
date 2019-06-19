/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TFTP Server Module
 *
 * Purpose  : TFTP Main Header File
 *            
\**************************************************************************/
#include "../../include/stdio.h"
#ifndef _TS_MAIN_H_
#define _TS_MAIN_H_

#ifdef __MODULE__
#undef __MODULE__
#endif

#define __MODULE__	"PDA_SYNC"

#define TFTP_CONNECT_USB	1		//	连接方式为usb
#define TFTP_CONNECT_BT		2		//  连接方式为蓝牙


#define USELESS_FILEPOINT	NULL

//Sys 0x01 - 0x09
#define		TS_OP_OPTION				0x01
#define		TS_OP_FLASHSPACE			0x02
#define		TS_OP_MMC_REQUEST			0x03
#define		TS_OP_UNKONW				0x04

//File & Folder 0x10 - 0x2F
#define		TS_OP_DIR					0x11
#define		TS_OP_GET					0x12
#define		TS_OP_GETDATA				0x13
#define		TS_OP_PUT					0x14
#define		TS_OP_PUTDATA				0x15
#define		TS_OP_DELETE				0x16
#define		TS_OP_CREATEDIR				0x17
#define		TS_OP_ATDEMO				0x18
#define		TS_OP_RENAME				0x19
#define		TS_OP_SENDSTOP				0x1A
#define		TS_OP_GETSTOP				0x1B
#define		TS_OP_DELDIR				0x20

//File Trans Notify	0x30 - 0x3F
#define		TS_PIC_MODIFY				0x30
#define		TS_BCKE						0x31
#define		TS_RESTORE					0x32

//Connect	0x40 - 0x4F
#define		TS_CONNECT_ADD				0x40
#define		TS_CONNECT_DEL				0x41
#define		TS_CONNECT_MODIFY			0x42
#define		TS_CONTECT_HUGE_BEGIN		0x43
#define		TS_CONTECT_HUGE_END			0x44

//Calecder  0x50 - 0x59
#define		TS_CALENDER_ADD				0x50
#define		TS_CALENDER_DEL				0x51
#define		TS_CALENDER_MODIFY			0x52

//Other		0xA0 - 0xBF


#define		E_NOERROR			0		//没有错误
#define		E_NOTFOUND			1		//文件未找到
#define		E_PATH				2		//路径不对
#define		E_ACCESS			3		//访问权限不够
#define		E_EXIST				4		//文件已存在
#define		E_BADPACKET			5		//BAD PACKET TYPE.
#define		E_BADFUNCTION		6		//OPCODE ISN'T SUPPORTED.
#define		E_CREATEERROR		7		//发生为知错误
#define		E_SEEKFILEERROR		9		//创建新文件错误
#define		E_UNKNOWNERROR		8		//文件指针移动错误
#define		E_WRITEFILEERROR	10		//写文件错误
#define		E_OPENFILEERROR		11		//打开文件错误
#define		E_READERROR			12		//读数据错误
#define     E_FILESIZE_ZERO     13		//文件大小为0
#define     E_PUTFILEERROR      14		//手机端接收文件错误
#define     E_MEMERROR          15		//手机端无法申请到足够的空间
#define     E_MEMWRITE          16		//手机端接收数据写内存的时候错误
#define     E_CREATEDIR         17		//创建目录错误
#define     E_DIRUNEMPTY        18		//目录不为空
#define     E_REMOVEDIR         19		//删除目录错误
#define     E_ID_ERROR          20		//用户验证错误
#define     E_NOENOUGH_MEM      21		//剩余的空间不足以存放要传输的文件
#define     E_CARD_NOT_READY    22		//外接卡没有准备好
#define     E_DEL_FILE          23		//删除文件错误
#define		E_SESSIONERROR		24		//C\S初始化认证失败
#define		E_SESSTIONPULSE		25		//传输请求暂时不能进行
#define		E_RECVBROKEN		26		//无法接收到手机的消息,传输中断
#define		E_CONNECTERROR		27		//手机与PC连接异常，请检查连接
#define		E_RENAEM			28		//重命名错误
#define		E_RMDIR				29		//删除文件夹错误
#define		E_PICUPDATA			30		//图片浏览器更新错误
#define		E_CALENDERADD		31		//添加日程错误
#define		E_CALENDERDEL		32		//删除日程错误
#define		E_CALCEDERMODIFY	33		//修改日程错误
#define		E_CONNECTADD		34		//添加联系人错误
#define		E_CONNECTMODIFY		35		//修改联系人错误
#define		E_CONNECTDEL		36		//删除联系人错误
#define		E_MMC				37		//MMC卡不可用	
#define		E_OUTRANGE			100		//偏移量或索引号超出范围
#define		E_TRANSFER			101		//CHECK SUM ERROR.
#define     E_STOPTIMER         102		//计时器不要工作
#define     E_SAVE_END          103		//PDA端内存节点已经转移到文件中去了

//#define TS_SERVER_VERSION	0x10
#define TS_RECVBUF_LENGTH	4096
#define TS_PACKBUF_LENGTH	(TS_RECVBUF_LENGTH/2)
//#define TSP_PATHLEN		64
#define TSP_PATHLEN		    200

//-----------------PacketData struct
typedef struct Tag_TS_Packetbuf{
	BYTE	PackHead;		// 0x00
	BYTE	Opcode;
	short	Length;
	//short	Serial;
	DWORD	Serial;
	short	Errcode;
	short	Reserve;
	BYTE	*pPacketData;	// pRecvData的内容根据Opcode的不同进行数据解析
	unsigned short	Checksum;
	//BYTE	PackTail[2];
}TS_PACKETBUF, *PTS_PACKETBUF;


//----------------Request Decode
typedef struct Tag_TSP_Dir{
	unsigned long	Index;
	char			Path[TSP_PATHLEN];
	BYTE			Reserve[2];
}TSP_DIR, *PTSP_DIR;

typedef struct Tag_TSR_Dir_File {
	long	size;
	unsigned long	modifyTime;
	char			FileName[TSP_PATHLEN];
}TSR_DIR_FILE, *PTSR_DIR_FILE;



typedef struct Tag_TSP_Get{
	char			Path[TSP_PATHLEN];
}TSP_GET, *PTSP_GET;

typedef struct Tag_TSP_GetData{
	unsigned long	trID;
	unsigned long	offset;
	unsigned long	length;
}TSP_GETDATA, *PTSP_GETDATA;

#define TC_PUTTYPE_NOEXIST			0
#define TC_PUTTYPE_COVER			1
#define TC_PUTTYPE_NOCONDITION		2

typedef struct Tag_TSP_Put{
	unsigned short	mode;
	BYTE			Reserve[2];
	unsigned long	size;
	char			Path[TSP_PATHLEN];
}TSP_PUT, *PTSP_PUT;

typedef struct Tag_TSP_PutData{
	unsigned long	trID;
	unsigned long	offset;
	BYTE			PutData[TS_PACKBUF_LENGTH];		// or set a point
}TSP_PUTDATA, *PTSP_PUTDATA;


#define TS_DELETE_DIR	0
#define TS_DELETE_FILE	1

typedef struct Tag_TSP_Delete{
	//unsigned short	Delmode;
	char			Path[TSP_PATHLEN];
}TSP_DELETE, *PTSP_DELETE;

typedef struct Tag_TSP_CreateDir{
	char			Path[TSP_PATHLEN];
}TSP_CREATEDIR, *PTSP_CREATEDIR;

//添加一个联系人
typedef struct Tag_TSP_ConnectAdd{	
	unsigned long		DataLen;
	BYTE				Data[TS_PACKBUF_LENGTH];
}TSP_ConnectAdd, *PTSP_ConnectAdd;

typedef struct Tag_TSR_ConnectAdd{	
	unsigned long		ConnectID;	
}TSR_ConnectAdd, *PTSR_ConnectAdd;


//修改一个联系人
typedef struct Tag_TSP_ConnectModify{
	unsigned long		ConnectID;
	unsigned long		DataLen;
	BYTE				Data[TS_PACKBUF_LENGTH];
}TSP_ConnectModify, *PTSP_ConnectModify;


//删除一个联系人
typedef struct Tag_TSP_ConnectDelete{
	unsigned long		ConnectID;
}TSP_ConnectDelete, *PTSP_ConnectDelete;


//添加一个日程
typedef struct Tag_TSP_CalenderAdd{	
	int					CaleEntry;
	BYTE				pRecord[TS_PACKBUF_LENGTH];
}TSP_CalenderAdd, *PTSP_CalenderAdd;

//删除一个日程
typedef struct Tag_TSP_CalenderDel{	
	int					CaleEntry;
	int					CaleID;
}TSP_CalenderDel, *PTSP_CalenderDel;

//修改一个日程
typedef struct Tag_TSP_CalenderModify{	
	int					CaleEntry;
	int					CaleID;
	BYTE				pRecord[TS_PACKBUF_LENGTH];
}TSP_CalenderModify, *PTSP_CalenderModify;



//-------------Control Cmd Reply Code-----------------

typedef struct Tag_TSP_Option{
	BYTE	Option;
	BYTE	Len;
	BYTE	OptData[2];
}TSR_OPTION, *PTSR_OPTION;

// define tag value
#define	RESTAG_Version		1
#define	RESTAG_Length		2


//-------------File Opcode Reply Struct--------------
#define TSR_DIRFILE_MAXCOUNT		10



typedef struct Tag_TSR_DirGroup{
	short	count;
	BYTE	reserve[2];
	TSR_DIR_FILE	Dir_File[TSR_DIRFILE_MAXCOUNT];
}TSR_DIRGROUP, *PTSR_DIRGROUP;


typedef struct Tag_TSR_Get {
	unsigned long	trID;
	unsigned long	filesize;
}TSR_GET, *PTSR_GET;

typedef struct Tag_TSR_Getdata {
	unsigned long	trID;
	BYTE 			Getdata[TS_PACKBUF_LENGTH];
}TSR_GETDATA, *PTSR_GETDATA;

typedef struct Tag_TSR_Put {
	unsigned long	trID;
}TSR_PUT, *PTSR_PUT;

typedef struct Tag_TSR_Putdata {
	unsigned long	nRecvDataLen;

}TSR_PUTDATA, *PTSR_PUTDATA;

typedef struct Tag_TSR_Rename {
	char	Old_Name[TSP_PATHLEN];
	char	New_Name[TSP_PATHLEN];	
}TSR_RENAME, *PTSR_RENAME;

typedef struct Tag_TSR_RmDir {
	char	m_Path[TSP_PATHLEN];	
}TSR_RMDIR, *PTSR_RMDIR;

typedef struct tag_TS_TransNode{
	DWORD	trID;
	FILE	*pFile;
	BYTE	szFileName[TSP_PATHLEN];
}TS_TRANSNODE, *PTS_TRANSNODE;


//---------------------Recvbuf block
typedef struct Tag_TS_Recvbuf{
	long	length;
	BYTE	*pRecvData;
}TS_RECVBUF, *PTS_RECVBUF;

typedef struct Tag_TS_Sendbuf{
	long	length;
	BYTE	*pSendData;
}TS_SENDBUF, *PTS_SENDBUF;

//-------------------ftpblock--------------------------
#define TS_ANALYSE_BEGIN		1
#define TS_ANALYSE_REPEAT		2

typedef int (*_PTS_GETPACKET)(void);

typedef struct Tag_TS_ftpblock{

	char	DevName[TSP_PATHLEN];
	unsigned long	DevRate;
	HANDLE	Devhandle;
	//FILE    *Devhandle;
	int		AnalyseStatus;
	short	Serial;

	TS_RECVBUF		TS_Recvbuf;
	TS_RECVBUF		TS_Procedbuf;
	TS_SENDBUF		TS_Sendbuf;

	TS_PACKETBUF	TS_PacketBuf;
	TS_PACKETBUF	TS_ReplyBuf;

	TS_TRANSNODE	TS_TransNode;
	
}TS_FTPBLOCK, *PTS_FTPBLOCK;


typedef struct Tag_TS_ApiRegister {
	HWND			hWnd;
	unsigned long	Msg;
}TS_APIREGISTER, *PTS_APIREGISTER;


//-----------------Fileapi sysdef

typedef UINT (*_CALLBACK)(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);
typedef struct UART_Control
{
	_CALLBACK	Dev_Event;
	void *		para;
	DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

#define		CPU_POWERPC		0	//CPU为POWERPC系列
#define		CPU_INTEL		1	//CPU为INTEL系列
// Define function return value

#define TS_RETVALUE_SUCCESS			 0
#define TS_RETVALUE_PARAMERROR		-1
#define TS_RETVALUE_DEVERROR		-2
#define TS_RETVALUE_NOMEMORY		-3
#define TS_SET_DEVEIOCONTROL		-4


int PCFTP_Init(char *DevName, int Length, long DevRate);
int PCFTP_Dest(void);
int PCFTP_Readdata(void);
int PCFTP_Register( HWND hWnd, unsigned long Msg, BYTE OpcType );


extern int TS_GetPacket( void );
extern int TS_ReplyPacket( BYTE Opcode, short ErrorCode, BYTE *Data, short Length );
extern int TS_Stream_AnalyseData(void);

extern UINT TS_DevCallBack(LONG Data, DWORD UartEvent, DWORD p1, LONG p2);

// Assistant Function
extern void		JudgeCPUType(void);
extern short	SwapShort(short wValue);
extern long		SwapLong(long dwValue);
extern unsigned short docrc (unsigned char *p, int len );

extern BOOL PdaSvr_Open(int connect_type);
extern void PdaSvr_Close(void);

// fapi
HANDLE	ts_CreateFile(PCSTR pFileName, DWORD dwMode, DWORD dwAttrib);
HANDLE	ts_OpenFile(PCSTR pFileName, const char *mode);	//add
BOOL	ts_CloseFile(HANDLE hFile);
DWORD	ts_ReadFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);
DWORD	ts_WriteFile(HANDLE hFile, PVOID pBuf, DWORD nBytes);
BOOL	ts_MoveFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL	ts_DeleteFile(PCSTR pFileName);
HANDLE	ts_FindFirstFile(PCSTR lpFileName, P_FIND_DATA pFindData);
BOOL	ts_FindNextFile(HANDLE hFindFile, P_FIND_DATA pFindData);
BOOL	ts_FindClose(HANDLE hFindFile);
DWORD	ts_GetFileSize(HANDLE hFile);
int		ts_SetFilePointer(HANDLE hFile, LONG lDistance, DWORD dwMode);



#endif //_TS_MAIN_H_
