/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sync_ui.h
Abstract:
	This module provides all the constants and user interface functions for 
	the upper layer application.
Author:
    Luo hongbo
Revision History:
	2002.1		created
	2002.6.25  Add two user interface AUTOSYNC_Start(),and AUTOSYNC_Stop().	
	2002.7.1   Delete two micros AUTO_SYNC and SYNC_CMD from the user interface.
version 2.0.5
	2002.10.30	add an member "svr_chnl" to the structure "SYNC_SvrInfo".
---------------------------------------------------------------------------*/
#ifndef SYNC_UI_H
#define SYNC_UI_H

#include "goep_ui.h"
#include "store_dep.h"

/* support sync type */
#define DB_VCARD_TYPE		0x00			/* VCF */
#define DB_VCAL_TYPE		0x01			/* VCS */
#define DB_VNOTE_TYPE		0x02			/* VNT */
#define DB_VMSG_TYPE	    0x03			/* VMG */

#define SUP_DB(type)		(1<<type)		/* supporting type */
/* conflict resolution method */
#define CONFLICT_NONE       0x00			/* leave unsolved  */
#define	CONFLICT_CLNT		(1<<0x04)		/* Replace the object on the client with the one on the server */
#define CONFLICT_SVR		(1<<0x05)		/* Replace the object on the server with the one on the client */

#define SYNC_ATTRIB_TYPE(a)		(a&0x0f)
#define SYNC_ATTRIB_CONFLICT(a)	(a&0x30)

/* three actions on the object store */
#define SYNC_ACT_ADD		0x10			/* Add operation    */
#define SYNC_ACT_DEL		0x20			/* Delete operation */
#define SYNC_ACT_MOD		0x30			/* Modify operation */

#define SYNC_MAX_NAME		128				/* the maximum length of the IrMC objects */
#define INVALID_SYNC_HDL	0				/* invalid sync handle */
#define IS_VALID_SYNC_HDL(hdl) (hdl!=INVALID_SYNC_HDL)

typedef UCHAR* SYNCHDL ;					/* sync handle type */
typedef UCHAR (*SyncCmdCbk)(UCHAR* bd,UCHAR attrib);	/* Sync command callback function   */
typedef UCHAR (*AutoSyncCbk)(UCHAR* bd);				/* Automatic sync callback function */

struct SYNC_ClntInfo{
	struct GOEPConnInfo	goep_info;			/* some GOEP information for the connection  */
	UCHAR			    conn_info;			/* support type and  conflict option */
};

struct SYNC_SvrInfo{
	DWORD           svc_hdl;
	UCHAR			sync_info;				/* support type				*/
	UCHAR			auth_attrib;			/* access and authentication */
	UCHAR			svr_chnl;				/* output parameter			*/
	UCHAR			pwd[MAX_PWD_LEN+1];		/* the password string		 */	
};

/* the following two structures are used in status callback function */
struct SYNC_Info{							/* used in put and get operation	*/
	UCHAR			type;					/* the object type			*/
	UCHAR			code;					/* the operation result code */
	UCHAR			name[SYNC_MAX_NAME];	/* the name of the object to be operated */
};

#ifdef CONFIG_IRMC_CLIENT
/* Client sync functions */
SYNCHDL SYNC_ClntStart(UCHAR* bd,UCHAR* sdp_attrib,UCHAR* loc_attrib);		/* Start the sync connection */
DWORD   SYNC_ClntStop(SYNCHDL hdl);											/* start the sync disconnection */
WORD    SYNC_StartSync(SYNCHDL hdl);										/* start the sync process */
WORD    SYNC_StopSync(SYNCHDL hdl);											/* stop the sync process */
/* Client sync command functions */
SYNCHDL SYNCCMD_ClntStart(SyncCmdCbk cmd_func,UCHAR attrib);				/* Stat the sync command service */
DWORD   SYNCCMD_ClntStop(SYNCHDL hdl);										/* Stop the sync command service */
#endif

#ifdef CONFIG_IRMC_SERVER
/* Server sync functions */
SYNCHDL SYNC_SvrStart(UCHAR* sync_attrib);									/* Start the sync service */
DWORD   SYNC_SvrStop(SYNCHDL hdl);											/* Stop the sync service  */
/* Server sync cmommand functions */
WORD    SYNCCMD_StartSync(UCHAR* bd,SYNCHDL hdl);							/* Star the sync command  */
#endif

/* Get the latest sync profile version */
const char* SYNC_GetVersion(void);							

/* Sync property setting and retrieving */
UCHAR   SYNC_SetAttrib(SYNCHDL hdl,UCHAR attrib);							/* Setting the sync attribute */
UCHAR   SYNC_GetAttrib(SYNCHDL hdl,UCHAR* attrib);							/* Retrieving the sync attribute */

/* Sync operation : add,modify or delete on the object store; and sync type */
UCHAR   SYNC_Act(SYNCHDL hdl,UCHAR flag,const char* luid);

/* Register event callback functions */
void SYNCAPP_RegCbk(UCHAR* hdl,UCHAR* ev_cbk);

/* Initialize the object store before starting synchronization */
WORD SYNC_InitStore(SYNCHDL hdl,UCHAR db_type);

WORD SYNC_Conn(SYNCHDL hdl);			/* Sync connection */
void SYNC_Disconn(SYNCHDL hdl);			/* Sync disconnection */
WORD SYNC_Sync(SYNCHDL hdl, UCHAR type);/* Sync only one object type */
WORD SYNC_SyncAll(SYNCHDL hdl);			/* Sync all object type after sync connection */

#ifdef CONFIG_AUTO_SYNC
WORD AUTOSYNC_Start(UCHAR* bd,UCHAR* pfunc,DWORD milliseconds);		/* start the auto sync procedure */
WORD AUTOSYNC_Stop(void);											/* stop the auto sync procedure */
#endif

#endif
