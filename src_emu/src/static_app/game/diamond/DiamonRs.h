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

#ifdef	SCP_SMARTPHONE
#ifndef	_STR_PUBLIC_APP_
#define	_STR_PUBLIC_APP_
#include	"str_public.h"
#include	"str_hp.h"
#endif
#endif
/**********************************************
*
*	界面位置相关常量
*
***********************************************/

#if defined SCP_NOKEYBOARD
#define WND_X		0
#define WND_Y		0
#define WND_WIDTH	176
#define WND_HEIGHT	196
#define TOPSPACE	24
#define	BOTTOMSPACE	20

#define	X_COVERBG		0
#define	Y_COVERBG		0
#define	COVERBG_WIDTH	176
#define	COVERBG_HEIGHT	176
#define	X_LT_MENUITEM	59
#define	Y_LT_MENUITEM	107
#define	MENUITEM_WIDTH	76
#define	MENUITEM_HEIGHT	19
#define	X_LT_SELITEM	30
#define	Y_LT_SELITEM	108
#define	SELITEM_WIDTH	17
#define	SELITEM_HEIGHT	17
#define	Y_MENUITEM_INTERVAL		23
#define ALLMENU_WIDTH	76
#define	ALLMENUHEIGHT	43

#define	X_GAMEBACK		0
#define	Y_GAMEBACK		0
#define	GAMEBACK_WIDTH	176
#define	GAMEBACK_HEIGHT	176
#define DIAMOND_WIDTH		17
#define BAR_LEFT		35
#define BAR_TOP			159
#define	BAR_WIDTH		105
#define	BAR_HEIGHT		7
#define MAXPRICE		110

#define NUMBER_WIDTH	7
#define NUMBER_HEIGHT	11

#define	X_GAMELEVEL		49
#define	Y_GAMELEVEL		142
#define	X_TOTALMERGE	138
#define	Y_TOTALMERGE	142
#define	X_TEXTLEVEL		60
#define	Y_TEXTLEVEL		59

#define	X_DIAMOND_OFFSET		19
#define	Y_DIAMOND_OFFSET		2

#define	TIP_LEFT			55
#define	TIP_RIGHT			68
#define	TIP_UP				141
#define	TIP_DOWN			154
#else                             //S5_SMARTPHONE有键盘的
#define WND_X		0
#define WND_Y		0
#define WND_WIDTH	176
#define WND_HEIGHT	220
#define TOPSPACE	20          //LHBADD 04-02-19  21
#define	BOTTOMSPACE	21

#define	X_COVERBG		0
#define	Y_COVERBG		0
#define	COVERBG_WIDTH	176
#define	COVERBG_HEIGHT	199
#define	X_LT_MENUITEM	59
#define	Y_LT_MENUITEM	127
#define	MENUITEM_WIDTH	75
#define	MENUITEM_HEIGHT	20
#define	X_LT_SELITEM	35
#define	Y_LT_SELITEM	128
#define	SELITEM_WIDTH	15
#define	SELITEM_HEIGHT	15
#define	Y_MENUITEM_INTERVAL		24
#define ALLMENU_WIDTH	76
#define	ALLMENUHEIGHT	43

#define	X_GAMEBACK		0
#define	Y_GAMEBACK		0
#define	GAMEBACK_WIDTH	176
#define	GAMEBACK_HEIGHT	199
#define DIAMOND_WIDTH		17
#define BAR_LEFT		36
#define BAR_TOP			169 - 10     //LHBADD 04-02-27 169
#define	BAR_WIDTH		105
#define	BAR_HEIGHT		7 
#define MAXPRICE		110

#define NUMBER_WIDTH	7
#define NUMBER_HEIGHT	11

#define	X_GAMELEVEL		49
#define	Y_GAMELEVEL		153 - 10     //LHBADD 04-02-27   153
#define	X_TEXTLEVEL		70
#define	Y_TEXTLEVEL		60
#define	X_TOTALMERGE	138
#define	Y_TOTALMERGE	153 - 10     //LHBADD  04-02-27   153 

#define	X_DIAMOND_OFFSET		19
#define	Y_DIAMOND_OFFSET		1    //LHBADD 04-02-27  13

#define X_LBALL			55
#define Y_LBALL			151
#define WIDTH_LBALL		14
#define HEIGHT_LBALL	14

#define	TIP_LEFT			55
#define	TIP_RIGHT			68
#define	TIP_UP				152
#define	TIP_DOWN			165      

#define WAITTIMES			6
#endif

/**********************************************
*
*	字符串资源
*
***********************************************/


#define	IDS_DIAMOND			ML("Diamond")
#define	IDS_NEWGAME			ML("New")
#define	IDS_HELP			ML("Help")
#define	IDS_EXIT			ML("Exit")
#define	IDS_CONFIRM			ML("Ok")
#define	IDS_HELPTEXT		ML("Diamondhelp")
#define	IDS_YES				ML("Yes")
#define	IDS_NO				ML("No")
/*
#define	IDS_HELPTEXT		("Object: Try to reduce as many diamonds as possible till the end of the stage.\
Instruction: Switch the position of the neighboring diamonds to get a row that consists of three diamonds in\
the same color, either vertically or horizontally. Then the row of will disappear and the diamonds above will fall.\
New diamonds will also fill the vacancy at the top.  During the game, when clicking the blue button “hint”,\
hint will be given and the diamond hinted will rise.")
*/
#define	IDS_LOADRECORD		ML("Load record?")
#define	IDS_SAVERECORD		ML("Save record?")
#define	IDS_PROMPT			ML("Prompt")
#define	IDS_AGAIN			ML("Game Over,\nReplay?")
#define	IDS_CANCEL			ML("Cancel")
