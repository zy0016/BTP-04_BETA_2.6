/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    store_dep.h
Abstract:
	This module defines the prototype of all the system depedent functions.
Author:
    Luo hongbo
Revision History:
	2002.1		created
---------------------------------------------------------------------------*/
#ifndef STORE_DEP_H
#define STORE_DEP_H

/*Changed from 50 to 256 because socket reported a case that when Outlook 2003
connects with Exchange Server, the luid's length is 150. (By WeiJunping on 2004-3-12 16:20:10)*/
#define MAX_IRMC_LUID		256   
#define MAX_IRMC_DID		20

#define ACCESS_RDONLY       0x01	/* open for reading only */
#define ACCESS_WRONLY       0x02	/* open for writing only */
#define ACCESS_APEND		0x04	/* open for apending */
#define ACCESS_CREAT        0x08	/* create and open file */

#define FMT_DEVINFO			0x10	/* devinfo.txt	*/
#define FMT_ILOG			0x20	/* info.log     */
#define FMT_CC				0x30	/* cc.log		*/
#define FMT_ENG_TABLE		0x40	/* engine table */

#define GET_FMT(a)			(a&0xf0)
#define GET_ACCESS(a)		(a&0x0f)

#define OBSTOREHDL DWORD			/* object store access handle */
#define INVALID_OBSTORE_HDL	0		/* invalid object store access handle */

#define OBS_FIND_HDL long			/* the object store enumerating handle */
#define INVALID_OBS_FIND_HDL -1		/* invalid enumerating handle */

/* luid : NULL create temporary object 
   else : opean an object according to the access */
OBSTOREHDL   OBSTORE_OpenObj(UCHAR db_type,const char* luid,UCHAR access);
DWORD        OBSTORE_ReadObj(OBSTOREHDL obs_hdl,UCHAR* buf,DWORD len,BOOL* isend);
DWORD	     OBSTORE_WriteObj(OBSTOREHDL obs_hdl,UCHAR* buf,DWORD len);	
DWORD	     OBSTORE_GetObjLen(OBSTOREHDL obs_hdl);
void	     OBSTORE_CloseObj(OBSTOREHDL obs_hdl);
int 	     OBSTORE_DelObj(UCHAR db_type,const char* luid);
UCHAR*		 OBSTORE_FindObj(UCHAR db_type,UCHAR* obj,DWORD len,WORD* count);
OBS_FIND_HDL OBSTORE_FindFirst(UCHAR db_type,UCHAR* luid);
int          OBSTORE_FindNext(OBS_FIND_HDL hdl,UCHAR* luid);
void		 OBSTORE_FindClose(OBS_FIND_HDL hdl);

int		     OBSTORE_SetRoot(const char* root);
void		 OBSTORE_GetDbPath(UCHAR type, const char* luid,/*[out]*/char* path);
void	     OBSTORE_Create(UCHAR db_type,UCHAR* did);
WORD         OBSTORE_GetObjLimit(UCHAR db_type);
WORD         OBSTORE_GetObjNum(UCHAR db_type);
void	     OBSTORE_SetObjPos(OBSTOREHDL obs_hdl,long off);
void		 OBSTORE_Rewind(OBSTOREHDL obs_hdl);

#ifdef CONFIG_SAVE_ENGINE
void		OBSTORE_CreatePartnerShip(UCHAR* bd,UCHAR db_type);
OBSTOREHDL	OBSTORE_OpenPartnerShip(UCHAR* bd,UCHAR db_type,UCHAR attrib);
void		OBSTORE_ClearPartnerShip(UCHAR* bd,UCHAR db_type,UCHAR fmt);
#endif

#endif
