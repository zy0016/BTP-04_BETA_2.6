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
#ifndef  CLOCK_SET_H

#define  CLOCK_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#define     ALARM_WEEKDAY       0
#define     ALARM_EVERYDAY      1
#define     ALARM_JUSTONE       2

#define   RING_NAME_LEN        127     // ring file name length

#define   RING_SHOWNAME_LEN    15     // ring show name length

#define     CLOCKNUM        3       // clock count

typedef	struct	tagRINGNAME             
{
	char name[RING_NAME_LEN + 1];     // ring file name
	char show[RING_SHOWNAME_LEN + 1]; // ring show name
}RINGNAME;

typedef struct tagCLOCKSET
{
	unsigned char   open;     // 0/1 on/off
	unsigned char   hour;     // hour
    unsigned char   minute;   // minute
	unsigned char   type;     // only once, every day, from monday to friday 
	unsigned char   interval; // invertal 1, 3, 5, 10
	unsigned char   times;
	RINGNAME   ring;      // ring file name
}CLOCKSET;


int Clock_Init();
int GetClockSet(CLOCKSET *pClockSet, int id);
int SaveClockSet(CLOCKSET *pClockSet, int id);


#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif  /* __clock_set_h */