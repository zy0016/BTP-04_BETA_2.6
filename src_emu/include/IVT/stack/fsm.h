/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    buff.h
Abstract:
	This file includes the definition of finite state machine functions.
Author:
    Gang He
Revision History:
	2000.2
---------------------------------------------------------------------------*/
#ifndef FSM_H
#define FSM_H

#define MAX_INST_NAME_LEN		5
#define MAX_TIMER_NAME_LEN		7

struct FsmInst;

typedef void (FsmFunc) (struct FsmInst *, UCHAR, void *);
/*
 * Entry to fill the jump matrix for FSM
 */
struct FsmNode {		/* Fsm node*/
	UCHAR state, event;	/* state and event */
	FsmFunc * routine;
};

/*
 * Finite state machine
 */
struct Fsm {					/* Fsm entity */
#ifdef CONFIG_FUNC_LIST
	const struct FsmNode * jump_matrix;
#else
	FsmFunc ** jump_matrix;		/* Fsm jump matrix*/
#endif
#ifdef CONFIG_DEBUG
	char **str_event;			/* Description of each event;*/
	char **str_state;			/* Description of each state; */
#endif
	FsmFunc * default_func;		/* The entity of the default function*/
	UCHAR state_count; 			/* Total states in the Fsm*/
	UCHAR event_count;			/* Total Events int the Fsm*/
	UCHAR ref_count;			/* Number of Fsm instances that use this Fsm. */
};

#define BT_PRI_HIGHEST		1
#define BT_PRI_HIGH			0x10
#define BT_PRI_NORMAL		0x20
#define BT_PRI_LOW			0x30
#define BT_PRI_LOWEST		0x40
#define BT_PRI_USER			0x80

struct Event {				/* Event for the FSM*/
	struct FsmInst *fi; 	/* Correspond Fsm Instance*/
	void * arg;				/* Event Argument.*/
	UCHAR event;			/* Event ID.*/
	UCHAR pri;				/* Priority.*/
};

/*
 * Finite state machine instance
 */
struct FsmInst {				/* Fsm instance */
	void *user_data;			/* User specific data for the fsm*/
	struct Fsm *fsm;			/* The Fsm entity for this Fsm instance. */
	char state;					/* The Current state for the fsm. */
	UCHAR ref_count;			/* Number of event that is in processing */
	char fsm_inst_name[MAX_INST_NAME_LEN];		/* The name of the fsm instance */
};

/* 
 * FSM functions 
 */
struct Fsm * FsmNew (const struct FsmNode *fnlist, UCHAR fncount, UCHAR state_count, UCHAR event_count, FsmFunc * def_func);
struct Fsm * FsmDup (struct Fsm * fsm );
struct FsmInst * FsmInstNew( void * user_data,  const char * fsm_inst_name);
void FsmInstFree(struct FsmInst *fsminst);
char FsmEventEx(struct FsmInst *fi, UCHAR event, void *arg, UCHAR pri, UCHAR ref_inc);
void FsmChangeState(struct FsmInst *fi, char new_state);
FsmFunc * GetFsmFunc(struct FsmInst *fi, UCHAR ev);


#ifdef CONFIG_DEBUG
void SetDbgStr(struct Fsm * fsm, char **str_event, char **str_state);
#endif

#define FsmEvent(fi,event,arg) FsmEventEx(fi,event,arg,BT_PRI_NORMAL, 1)
#define FsmEventWithLock(fi,event,arg) FsmEventEx(fi,event,arg,BT_PRI_NORMAL, 1)
#define FsmEventWithoutRef(fi,event, arg) FsmEventEx(fi,event,arg,BT_PRI_NORMAL, 0)

/*
 * Timer structure 
 */
struct FsmTimer {			/* Timer in the fsm instance */
	DWORD	u_id; 			/*User defined id to index the timer*/
	DWORD millisec; 		/* Timer value */
	struct FsmInst *fi; 	/* Correspond Fsm Instance*/
	void *arg;				/* Argument for the event.*/
	TIMER_ID	timer_id;	/*System timer id*/
	UCHAR event;			/* Event when time out.*/
	char  name[MAX_TIMER_NAME_LEN];		/* Name for the timer */
}; 

/* 
 *	Timer functions
 */
typedef void (UserTimerFunc)(DWORD param);
struct FsmTimer * FsmAddTimer(struct FsmInst *fi, DWORD millisec, UCHAR event, void *arg, const char *name, DWORD u_id);
void FsmExpireTimer(struct FsmTimer *ft);
UCHAR * FsmDelTimer(struct FsmInst *fi, DWORD u_id);
UCHAR *  FsmDelTimer2(struct FsmTimer *ft);
void FsmDoneTimer(struct FsmInst *fi);
UCHAR *FsmTimerActive(struct FsmInst *fi, DWORD u_id);

/* 
*	Schedule functions
*/
#define SCH_NEXT	0
#define SCH_IDLE	1
#define SCH_DONE	2
void InitSchedule(void);
void ScheduleLoop(void);
#define DoneLoop() FsmEvent(NULL,0,NULL)

#endif
