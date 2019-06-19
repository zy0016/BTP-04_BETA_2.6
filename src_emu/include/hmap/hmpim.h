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


#ifndef _PIM_API_H_
#define _PIM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PB_NAME	"phonebook"
#define CA_NAME	"calender"

//sort mode
// asc or desc ( order )
#define PIM_SORTORD_ASC		0x00
#define PIM_SORTORD_DESC	0x01
// chinese compare mode
#define PIM_SORTCMP_PIN		0x00  // 0000 ping yin  
#define PIM_SORTCMP_YMU		0x04  // 0100 yun mu    
#define PIM_SORTCMP_NMA     0x08  // 1000 nei ma
// segmentation mode
#define PIM_SORTSEG_CHN		0x00  // chinese first
#define PIM_SORTSEG_ENG		0x10  // englist first
#define PIM_SORTSEG_OTH		0x20  // other first
#define PIM_SORTSEG_MIX     0x30  // mix 
// get lonely parameter
#define PIM_SORTORD(mode)       (mode & 0x03)
#define PIM_SORTCMP(mode)       (mode & 0x0c)
#define PIM_SORTSEG(mode)       (mode & 0x30)

#define	PIM_NOTSORT			((unsigned)-1)

#define PIM_UPDATE_MODE_CACHE	1
#define PIM_UPDATE_MODE_FLUSH	2

#define PIM_SEEK_SET		1
#define PIM_SEEK_CUR		2
#define PIM_SEEK_END		3

#define PIM_OPENMODE_READONLY	0
#define PIM_OPENMODE_READWRITE	1
#define PIM_OPENMODE_SYNC       3
#define PIM_O_RDONLY	    0
#define PIM_O_RDWR      	1
#define PIM_O_SYNC          3

#define PIM_CN_CODESTART		128
#define PIM_FIELDNAME_LEN		12
#define PIM_MAX_FIELDS			15

/***** record oper type for sync ****************/
#define PIM_OPERTYPE_NEW          1
#define PIM_OPERTYPE_MODIFY       2
#define PIM_OPERTYPE_DEL          3

/***** sync type ********************************/
#define	    PIM_SYNC_START		1
#define	    PIM_SYNC_ANCHOR		2
#define     ANCHOR_SIZE		    16
/*********************************************
**********************************************/
#define     PIM_FIELD_RECID      "RECORD_ID"
#define     PIM_FIELD_OPERTYPE   "OPER_TYPE"

/***** error number **********/
#define	    PIM_E_SIMFULL      -100
#define	    PIM_E_FLASHFULL	   -101  
#define	    PIM_E_NOSIM		   -102  // no SIM
#define	    PIM_E_MEMLOW	   -103  // not enough memory
#define     PIM_E_BUFFER       -104  // not enough buffer
#define     PIM_E_UNSUPPORT    -105  // not support
#define     PIM_E_FILE         -106  // file error
#define     PIM_E_NOTFOUND     -107  // file,folder,record not fount
#define     PIM_E_INVALID      -108  // invalid
#define     PIM_E_PARAM        -109  // param invalid
#define     PIM_E_REG          -110  // registry error
#define     PIM_E_CONFLICT     -111  // 
#define     PIM_E_RWSIM        -112  // read or write SIM error
#define     PIM_E_DELFAIL      -113  // sync, prevent multiple delete
#define     PIM_E_RDONLY       -114  // read only


struct pim_status
{
    unsigned short nMaxRecNum;  // max record amount in flash
	unsigned short nTotalNum;   // current valid record amount
    unsigned short nInvalidNum; // current invalid record amount
    unsigned short nSyncNum;    // need sync amount 
    unsigned short nSimNum;     // record amount int SIM
    unsigned short nSimMaxNum;  // max record amount int SIM
    long nFileSize;  // file size
};

typedef struct tagDB_STRUCT
{
	int		fieldnum;
	int		display_len;
	struct {
		unsigned char		id;
		unsigned char		type;
		unsigned char		name[PIM_FIELDNAME_LEN+1];
		unsigned short		len;
		unsigned short		offset;
	}field[PIM_MAX_FIELDS];
}DB_STRUCT;

typedef struct tagPIM PIM;

int PIM_Init(const char* pName);

// get a hamdle of PIM
PIM* PIM_Open(const char* pName, int nMode);

// free the handle and the resource which occupied by it
void  PIM_Close(PIM* pPIM);

// change the handle mode: read or readwrite
int   PIM_ChangeMode(PIM* pPIM, int nMode);

// set notify information when database changed
void  PIM_SetNotify(PIM* pPIM, void* hWnd, unsigned uMsg);

// get status of database
int   PIM_GetStatus(PIM* pPIM, struct pim_status* pStatus);

// save the data in buffer to file
int   PIM_Save(PIM* pPIM);

// clean up database file
int   PIM_Pack(PIM* pPIM);

// get record id only in select set
int   PIM_Select(PIM* pPIM, const char* pFieldList, const char* pPattern, const char* pSortKey, unsigned nSortMode);

// reselect in select set, only get record id
int   PIM_Reselect(PIM* pPIM, const char* pPattern);

// do when select set is invalid, free the latest select set
void  PIM_Unselect(PIM* pPIM);

// sort the record in select set
int   PIM_Sort(PIM* pPIM, const char* pSort_key, int nMode);

// get record from the server to client and set the current record
int   PIM_Seek(PIM* pPIM, int nOffset, int nMode);

// get record offset in select set ( every record )
int   PIM_Find(PIM* pPIM, void* pData, int nDatalen);

// add record , initialize an empty record for adding
int   PIM_Add(PIM* pPIM);

// notify server update current operate
int   PIM_Update(PIM* pPIM, int nMode);

// notify delete current record ( current record specified by PIM_Seek ) 
int   PIM_Delete(PIM* pPIM, int nMode);

// get an integrity record to buffer for operating
int	  PIM_GetFieldAll(PIM* pPIM);

// get field data
int   PIM_GetField(PIM* pPIM, const char* pName, void* pData, int nMaxlen);

// set field data
int   PIM_SetField(PIM* pPIM, const char* pName, void* pData);

// get SIMState
int PIM_GetSIMState();

// locate record in the selected set
int   PIM_Locate(PIM* pPIM, const char * pPattern);

int PIM_GetAnchor(PIM * pPIM, char *anchor);
int PIM_UpdateAnchor(PIM *pPIM, const char *anchor);
int PIM_ResetAnchor(PIM *pPIM);

#ifdef __cplusplus
}
#endif

#endif//_PIM_API_H_
