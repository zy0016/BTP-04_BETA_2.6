#ifndef IVT_BTAPP_GLOBAL_H
#define IVT_BTAPP_GLOBAL_H


#define BTA_FLAG_SYSTEM_RUN			0x0001
#define BTA_FLAG_BTHOST_INIT		0x0002
#define BTA_FLAG_SCHEDULE_READY		0x0004
#define BTA_FLAG_TIMER_READY		0x0008
#define BTA_FLAG_TL_RX_READY		0x0010


#define BTA_MSG_TYPE_MASK			0xff000000
#define BTA_MSG_EV_MASK				0x00ffffff

#define BTA_SET_MSG_ID(block, ev)	(((DWORD)block << 24) + (DWORD)ev)
#define BTA_CHK_BLOCK_TYPE(id)		((UCHAR)((id & BTA_MSG_TYPE_MASK) >> 24))
#define BTA_CHK_MSG_EV(id)			(id & BTA_MSG_EV_MASK)

struct BTA_MsgStru {
	DWORD	msg_id;
	DWORD	arg_len;
	UCHAR 	*msg_arg;
	DWORD	num_arg;
};

typedef void (BTA_ProcessAppTaskMsg)(struct BTA_MsgStru *msg);
typedef UCHAR (BTA_AppTaskInit)(void);
typedef UCHAR (BTA_AppTaskDone)(void);

struct BTA_AppTaskInfoStru {
	UCHAR				  task_id;
	WORD				  chk_flag;
	BTA_AppTaskInit		  *init_func;
	BTA_ProcessAppTaskMsg *msg_func;
	BTA_AppTaskDone		  *done_func;
};


void BTA_GlobalInit(void);
UCHAR BTA_BtInit(void);
void BTA_AppExit(void);

struct BTA_AppTaskInfoStru* BTA_AddAppTask(UCHAR *name, UCHAR task_id, WORD chk_flag, 
										   BTA_AppTaskInit *init_func,
										   BTA_ProcessAppTaskMsg *msg_func,
										   WORD pri, WORD stk_size, UCHAR *stk);
struct BTA_AppTaskInfoStru* BTA_AddAppTaskEx(UCHAR *name, UCHAR task_id, WORD chk_flag, 
										     BTA_AppTaskInit *init_func,
										     BTA_ProcessAppTaskMsg *msg_func,
										     BTA_AppTaskDone *done_func,
										     WORD pri, WORD stk_size, UCHAR *stk);

void BTA_AppTask(void *param);
void BTA_StackTask(void *param);

struct BTA_MsgStru* BTA_CreateNewMessageEx(DWORD block, DWORD event, DWORD arg_len, UCHAR *arg, 
										   DWORD num_arg);
struct BTA_MsgStru* BTA_CreateNewMessage(DWORD block, DWORD event, DWORD arg_len, UCHAR *arg);
struct BTA_MsgStru* BTA_CreateNoArgNewMessage(DWORD block, DWORD event);
void BTA_PostAppMessage(UCHAR task_id, struct BTA_MsgStru *msg);
void BTA_SetTaskReadyFlag(WORD flag, UCHAR is_ready);

UCHAR* BTA_Malloc(int size);
void BTA_Free(UCHAR *memblock);


#endif
