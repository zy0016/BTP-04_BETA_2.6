/*-----------------------------------------------------------------------------
Revision History:
1. (By WeiJunping on 2003-12-22 14:25:46)
  1.1 deleted function OBSOUT_Write
  1.2 deleted function OBSOUT_CheckName
2. (By WeiJunping on 2004-3-15 14:46:55)
  2.1 Deleted two declarations: EncodeObjToUTF7 and EncodeBufToUTF7
-----------------------------------------------------------------------------*/
#ifndef OUTINTER_H
#define OUTINTER_H

//Added (By WeiJunping on 2003-10-31 13:08:18)
extern void ShowBalloonText(UINT ids, int nDelayTime);

#ifdef __cplusplus
extern "C"{
#endif 

/* support sync type , according to SyncUI.h*/
#define DB_VCARD_TYPE		0x00			/* VCF */
#define DB_VCAL_TYPE		0x01			/* VCS */
#define DB_VNOTE_TYPE		0x02			/* VNT */
#define DB_VMSG_TYPE	    0x03			/* VMG */

typedef struct ItemStru{
	UCHAR*	item_hdl;		/* the item dispatch pointer */
	UCHAR*	file_hdl;		/* the item file handle		*/
	UCHAR	db_type;		/* the item type			*/
}*ITEMHDL;

typedef struct EnumItemStru{
	UCHAR*	items_hdl;		/* the item collection dispatch pointer */
	long	index;			/* the index of the current visited */
	long	count;			/* the total count of being enumerated */
	UCHAR	db_type;		/* the item type	*/
}*ENUMITEMHDL;

int		OBSOUT_Init();		/* Init the com library and outlook */
void	OBSOUT_Done();		/* Relase the com library and outlook */
BOOL	OBSOUT_MarshalInterface(void);	/* Marshal Com Proxy into a stream to across thread bondury */
BOOL	OBSOUT_GetInterface(void);		/* Retrive the Com Proxy from the global stream */
BOOL    OBSOUT_ReleaseInterface(void);	/* Free the global stream and mashalled data */
//added by Leng xiaoyu----------------------------------------------------------------
void ModifyCard(char *buf, int len);		/* Modify the buf to adapt to T39/T68 */
void DeleteAll();						

UCHAR* OBSOUT_IterateCmp(UCHAR db_type,UCHAR* obj,DWORD len,WORD* count,BOOL IsOpp);
//------------------------------------------------------------------------------------
ITEMHDL OBSOUT_OpenObj(UCHAR db_type,UCHAR* luid,UCHAR access);
ENUMITEMHDL OBSOUT_FindFirst(UCHAR db_type,UCHAR* luid);
int  OBSOUT_FindNext(ENUMITEMHDL hdl,UCHAR* luid);
void OBSOUT_FindClose(ENUMITEMHDL hdl);
int		OBSOUT_WriteObj(ITEMHDL obs_hdl,UCHAR* buf,int len);
int		OBSOUT_DelObj(UCHAR db_type,UCHAR* luid);
int		OBSOUT_GetItemCount(UCHAR db_type);
void 	OBSOUT_CloseObj(ITEMHDL obs_hdl);
void	OBSOUT_SaveAsFile(LPCTSTR path,LPDISPATCH disp,UCHAR db_type);		/* for	vCard:		db_type=0;
																					vCalendar:	db_type=1;
																				vMessage:	db_type=3;
																				vNote:		db_type=2;*/
//added by Leng Xiaoyu for using by Outlookaddin
void SaveCalendar(LPCTSTR path,LPDISPATCH disp);
BOOL SaveAsFile(LPCTSTR path,LPDISPATCH disp,UCHAR db_type);

void	OBSOUT_SetSyncHdl(UCHAR *hdl);
int		OBSOUT_GetNewObj(UCHAR db_type);
void	OBSOUT_SyncStartEvent();
void	OBSOUT_SyncEndEvent();
/*-----------------------------------------------------------------------------
Function: When the user deletes the last item in outlook, the corresponding sink
		  funtion is not called by outlook, this function deals with this garbage
		  problem.
Parameter: db_type - sync type, defind in sync_ui.h
Return value: none
Supervisor: Wei Junping
-----------------------------------------------------------------------------*/
void	OBSOUT_SyncDelItem(UCHAR db_type);

int	INBOX_Init();
void INBOX_Done();
BOOL INBOX_WriteItem(char* path);

#ifdef __cplusplus
}
#endif 
#endif
