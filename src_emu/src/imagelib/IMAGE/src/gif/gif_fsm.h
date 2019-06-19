
#ifndef	_GIF_FSM_H_
#define	_GIF_FSM_H_

#ifndef	NULL
#define NULL   (void *)0
#endif

#define LWZ_MAX_CODES		4095
#define	LWZ_MAX_OUTBYTE		4095

typedef	unsigned char  BYTE;
typedef unsigned short WORD;

enum fsm_state {
		S_HEADER, S_GCOLOR, S_DATA, S_IMGDESC, S_LCOLOR,
		S_IMGDATA_1, S_IMGDATA_2, S_TRAILER, S_ERROR
	};

//enum fsm_substate {
//		S_EXT = 1, S_BLOCK_LEN, S_BLOCK_DATA, S_BLOCK_TERM
//	};

extern int (*gif_fsm_func[])(struct gif_decoder * pGif);
extern int (*gif_fsm_subfunc[])(struct gif_decoder * pGif);

#define	min(a, b)	((a)<=(b)) ? (a) : (b)
#define	max(a, b)	((a)>=(b)) ? (a) : (b)

#endif
