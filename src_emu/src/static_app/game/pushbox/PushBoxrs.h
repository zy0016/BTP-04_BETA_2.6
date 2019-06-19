/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2005 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "pollex.h"


//#ifdef	SCP_SMARTPHONE
#ifndef	_STR_PUBLIC_APP_
#define	_STR_PUBLIC_APP_
#include	"str_public.h"
#include	"str_plx.h"
#endif
//#endif
#include	"mullang.h"



/**********************************************
*
*	界面位置相关常量
*
***********************************************/
//游戏界面
/*
#define WND_BORDER_X			0
#define WND_BORDER_Y			24//15	
#define WND_BORDER_LEN			176//120
#define WND_BORDER_HEIGHT		196//145
#define	BOTTOMSPACE				20

#define WND_X					0
#define WND_Y					0	
#define WND_WIDTH				240//176
#define WND_HEIGHT				240//176

#define RECT_W					18//14	//地图每一小块的长宽为14*/

#define WND_BORDER_X			0
#define WND_BORDER_Y			15	
#define WND_BORDER_LEN			120
#define WND_BORDER_HEIGHT		145
#define	BOTTOMSPACE				20

#define WND_X					0
#define WND_Y					0	
#define WND_WIDTH				176
#define WND_HEIGHT				182//176

#define RECT_W					14	//地图每一小块的长宽为14

//菜单区域
#define	X_FIRSTLU_MENUITEM		57
#define Y_FIRSTLU_MENUITEM		84
#define	X_FIRSTRD_MENUITEM		81
//#define Y_FIRSTRD_MENUITEM	105
#define Y_FIRSTRD_MENUITEM		105 + 50
#define X_MENUITEM_INTERVAL		60
#define	Y_MENUITEM_INTERVAL		21

#define COVER_WIDTH             176//240
#define COVER_HEIGHT            182//268

/*
#define	COVERWORDS_X			50//53//30
#define COVERWORDS_Y			80//109
#define	COVERWORDS_WIDTH		129//94//121
#define COVERWORDS_HEIGHT		125//86//44*/

#define	COVERWORDS_X			30//53//30
#define COVERWORDS_Y			30//109
#define	COVERWORDS_WIDTH		123//129//94//121
#define COVERWORDS_HEIGHT		125//86//44


// #define DIFFER_X				20   //天山与启明星界面位置差异
#define DIFFER_X				20
#define DIFFER_Y				50
// #define MENU_WORDSIZE           30
#define MENU_WORDSIZE           21
#define FIRSTMENU_X				COVERWORDS_X + DIFFER_X
#define FIRSTMENU_Y				COVERWORDS_Y + DIFFER_Y

/*
#define COVERFIRSTMAN_X         FIRSTMENU_X - 25
#define COVERFIRSTMAN_Y         FIRSTMENU_Y + 10*/
#define COVERFIRSTMAN_X         FIRSTMENU_X - 13
#define COVERFIRSTMAN_Y         FIRSTMENU_Y + 6


/*
#define	COVERMAN1_X				30//17
#define COVERMAN1_Y				85//110
#define	COVERMAN2_X				54//87
#define COVERMAN2_Y				106//110
#define	COVERMAN3_X				30//17
#define COVERMAN3_Y				127//138
#define	COVERMAN4_X				54//87
#define COVERMAN4_Y				148//138*/
#define	COVERMAN1_X				15
#define COVERMAN1_Y				30
#define	COVERMAN2_X				47//87
#define COVERMAN2_Y				59//110
#define	COVERMAN3_X				15//17
#define COVERMAN3_Y				88//138
#define	COVERMAN4_X				47//87
#define COVERMAN4_Y				116//138
/*
#define	COVERMAN_WIDTH			18//14
#define COVERMAN_HEIGHT			18//15*/
#define	COVERMAN_WIDTH			13//14
#define COVERMAN_HEIGHT			13//15


//选关界面
/*
#define LEVEL_TEXT_X			45//25
#define LEVEL_TEXT_Y			40//70
#define LEVEL_TEXT_LEN			90
#define LEVEL_TEXT_HEIGHT		20*/
#define LEVEL_TEXT_X			40//25
#define LEVEL_TEXT_Y			12//70
#define LEVEL_TEXT_LEN			90
#define LEVEL_TEXT_HEIGHT		20

/*
#define LEVEL_COMBO_X			45//76
#define LEVEL_COMBO_Y			65//68
#define LEVEL_COMBO_LEN			105//90//60
#define LEVEL_COMBO_HEIGHT		30//25*/
#define LEVEL_COMBO_X			25
#define LEVEL_COMBO_Y			30
#define LEVEL_COMBO_LEN			100
#define LEVEL_COMBO_HEIGHT		30


//过关界面
#define PASSWORDS_X				15
#define PASSWORDS_Y				30	
#define PASSWORDS_WIDTH			147
#define PASSWORDS_HEIGHT		56

//关底界面
#define ENDGAMEWORDS_X			8
#define ENDGAMEWORDS_Y			30
#define ENDGAMEWORDS_WIDTH		163
#define ENDGAMEWORDS_HEIGHT		84

/**********************************************
*
*	字符串定义
*
***********************************************/
#define IDS_GAMEOVER ML("Game over")
#define	IDS_NEXMISSON		ML("Congratulations! Next mission?")
#define	IDS_NEW				ML("New")
#define IDS_NEWGAME			ML("New game")
//#define	IDS_FAILREADMAP		ML("Failed to get map!")
#define IDS_CONFIRM			ML("Ok")
#define IDS_EXIT			ML("Exit")
#define	IDS_RETURN			ML("Back")
#define IDS_SELECT			ML("Select")
#define	IDS_GOONGAME		ML("Go on")
#define IDS_MENU_BACK		ML("Back")
#define IDS_MENU_REPLAY		ML("Replay")
#define IDS_LEVEL_SPEED		ML("Level select")
#define IDS_PUSHBOX			ML("PushBox")	//GetString(STR_PUSHBOX_PUSHBOX)

#define IDS_HELP			ML("Help")
#define IDS_HELPTEXT		ML("PushBoxHelp")
//#define IDS_HELPTEXT		("Object:\nPush all the boxes to\nthe destination.\nRegulation:\nPress a box without \
//any wall or other boxes around, the cargador beside it will move it for one step, in other conditions the box  \
//will not move. If\nyou want to replay the game, you can choose 'Replay' in the menu. Furthermore,the game has \
//different\nlevels, higher level, higher difficulty. You can choose any level as you like in the menu. If you don't\
// pass all the levels before exitting, next time, you can choose 'continue' to keep on playing. Every time you \
// pass the level, appears a dialog box to ask\nyou whether to enter\nthe higher level or not, if you pass the \
// highest level, the dialog box asks you whether to replay the game or not.")
#define IDS_GAME_PROMPT		ML("Are you sure?")
#define IDS_FILE_PROMPT		ML("Lake of memory!")
#define IDS_FILE_NOTICE		ML("Notice")

#define IDS_REPLAY          ML("Replay")
#define IDS_BACK            ML("Last")
