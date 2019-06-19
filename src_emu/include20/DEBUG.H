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

#ifndef _DEBUG_H_
#define	_DEBUG_H_

typedef struct tag_emuenv
{
	char * pRam;
	char * pRom;
	char * pFlash;
	char * unused;
	int	total_ramsize;
	int	total_romsize;
	int	total_flashsize;
	int	sys_ramsize;
	int	maxsysobjs;
	int	maxtasks;
	int	ramfile_start;		// Start of RAM file system.
	int	ramfile_size;		// Size of RAM file system.
	int	commmode;
} EMUENV, * PEMUENV;

typedef struct tag_tskstat {
	int		index;			// Start index.
	char	name[12];		// Name of this task.
	int		taskid;			// ID of this task.
	int		flags;
	int		priority;
	int		state;
	int		waitobj_id;
	int		waitobj_type;
} TASKSTAT, * PTASKSTAT;

typedef struct mem_state
{
	int	start_address;
	int	total_size;
	int	free_size;
	int	max_blocksize;
	int	total_handles;
	int	used_handles;
} MEMSTATE, * PMEMSTATE;

typedef struct object_state
{
	int	total;
	int	used;
	int	freed;
	int	garbage;
} OBJSTATE, * POBJSTATE;


#define M_UDPPORT       0
#define	M_SERIALPORT	1
#define M_MODEM         9
#define M_TCPSERVER     2
#define M_TCPCLIENT     3


#define	APP_INIT_DONE	101

#ifndef __cplusplus

extern void MsgOut( char * format, ... );
extern char * GetDebugInput( char * prompt );

extern int  Emu_GetCommMode( void );
extern char * Emu_GetDisplayMode( void );

extern void ErrorPrompt( char * prompt, ... );
extern void ErrorExit( char * prompt );

extern void Emu_sleep( int time );
extern void Emu_SendMessage( int msg );

extern int Emu_GetEmuEnviromemt( struct tag_emuenv * pReturn );

#endif

#endif
