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
#ifdef	SCP_SMARTPHONE
#ifndef	_STR_PUBLIC_APP_
#define	_STR_PUBLIC_APP_
#include	"str_public.h"
#include	"str_hp.h"
#endif
#endif
	
/**********************************************
*
*	Ӧ�ó���ͼ��
*
***********************************************/
static const BYTE AppIconData[] =	//����ͼ��
{
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0x40,0x00,0x00,0x00,0x00,0x01,0xFF,
	0xFF,0x3A,0xAA,0xAA,0xAA,0xAA,0xA8,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x00,0x00,0x00,0x00,0x54,0xFF,
	0xFF,0x25,0x2A,0xAA,0xAA,0xAB,0x54,0xFF,
	0xFF,0x25,0x2A,0xAA,0xAA,0xAB,0x54,0xFF,
	0xFF,0x25,0x2A,0xAA,0xAA,0xAB,0x54,0xFF,
	0xFF,0x25,0x3F,0xFF,0xFF,0xFF,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0xC7,0x1C,0x71,0xC7,0x14,0xFF,
	0xFF,0x25,0x04,0x10,0x41,0x04,0x14,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0xC7,0x1C,0x71,0xC7,0x14,0xFF,
	0xFF,0x25,0x04,0x10,0x41,0x04,0x14,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0xC7,0x1C,0x71,0xFF,0x14,0xFF,
	0xFF,0x25,0x04,0x10,0x41,0x00,0x14,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x25,0x55,0x55,0x55,0x55,0x54,0xFF,
	0xFF,0x40,0x00,0x00,0x00,0x00,0x01,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};


/**********************************************
*
*	����λ����س���
*
***********************************************/
//��Ϸ����
#define WND_X					0
#define WND_Y					24//0
#define WND_WIDTH				176
#define WND_HEIGHT				206//216//220
#define TOPSPACE				14
#define	BOTTOMSPACE				20//16

//�˵�����
#define	X_COVERBG				0
#define	Y_COVERBG				0
#define	COVERBG_WIDTH			176//120
#define	COVERBG_HEIGHT			181//198//199
#define	X_FIRSTLU_MENUITEM		21
#define Y_FIRSTLU_MENUITEM		110//121
#define	X_FIRSTRD_MENUITEM		88//87
#define Y_FIRSTRD_MENUITEM		130//134
#define X_MENUITEM_INTERVAL		66
#define	Y_MENUITEM_INTERVAL		21//19

//�α����
#define SEL_WIDTH				14
#define SEL_HEIGHT				10
#define MENU_ITEM_NUM			4
#define	X_LEFT_ITEM				16
#define X_RIGHT_ITEM			93
#define	Y_UP_ITEM				114
#define Y_DOWN_ITEM				137
#define	SEL_COLOR				RGB(255,0,0)

//���ý���
#define	X_SETPRESSBTN			35//55//35
#define	Y_SETPRESSBTN			70//10
#define	SETPRESSBTN_WIDTH		120//50
#define	SETPRESSBTN_HEIGHT		25

/*#define	X_SETMOVEBTN			25//55//35
#define	Y_SETMOVEBTN			90//40
#define	SETMOVEBTN_WIDTH		120
#define	SETMOVEBTN_HEIGHT		25*/

//�����������ʹ�С
#define	X_DARKBACK				8
#define	Y_DARKBACK				20
#define	DARKBACK_WIDTH			118//112
#define	DARKBACK_HEIGHT			121//134

//��Ϸ������ʹ�С
#define	X_GAMEBACK				8//18//9			
#define	Y_GAMEBACK				20//27//6
#define	GAMEBACK_WIDTH			118//112//84//101
#define	GAMEBACK_HEIGHT			154//172//110//102

//ƫ������0Ϊ����
#define	RADIUS_HEIGHT_GUN		11//7
#define RADIUS_WIDTH_GUN		19									//ǹ��뾶
#define	RADIUS_BALL				7//6//��뾶	
#define	GUN_LENTH				RADIUS_BALL								//ǹ�ڳ�
#define	DRAGEN_WIDTH			23										//��������
#define	DRAGEN_HEIGHT			19										//��������
#define	MIN_X_LEFT_GUN			(X_GAMEBACK+GUN_LENTH)					//ǹ�����Сֵ
#define	MAX_X_GUN_OFFSET		(GAMEBACK_WIDTH-2*(RADIUS_WIDTH_GUN+GUN_LENTH))	//ǹƫ�������ֵ
#define	Y_GUN					(Y_GAMEBACK+GAMEBACK_HEIGHT-2*RADIUS_HEIGHT_GUN+1)//ǹ�϶�ֵ
#define	Y_DRAGEN				(Y_GAMEBACK+GAMEBACK_HEIGHT-DRAGEN_HEIGHT)//������������
#define	MAX_X_BALL_OFFSET		(GAMEBACK_WIDTH-2*RADIUS_BALL+1)		//��xƫ�������ֵ
#define	MAX_Y_BALL_OFFSET		(GAMEBACK_HEIGHT-2*RADIUS_BALL-(RADIUS_HEIGHT_GUN-RADIUS_BALL))		//��yƫ�������ֵ
#define	X_BALL_INTERVAL			(RADIUS_BALL*2-1)							//x����
#define	Y_BALL_INTERVAL			12//8//9									//y����
#define	GUN_MOVE_INTERVAL		3										//ǹ�ƶ����
#define	X_GAME_PAO				5
#define	Y_GAME_PAO				4
#define	X_GAME_SCORE			50
#define	Y_GAME_SCORE			4
#define	X_LU_SCORENUM			134//137//80
#define	Y_LU_SCORENUM			45//4
#define	X_RD_SCORENUM			168//164
#define	Y_RD_SCORENUM			58	

//��ʾ����
#define	Pao_SCORE_X				130
#define	Pao_SCORE_Y				31
#define Pao_SCORE_WIDTH			36
#define	Pao_SCORE_HEIGHT		13

//Ӣ�۰����
#define	X_HERO_REC				30//30//8
#define	Y_HERO_REC				56//30//20
#define	HERO_REC_SPACE			17//16

//��ʾ��Ϣ����
#define OVER_TEXT_X				50//30
#define OVER_TEXT_Y				40//30
#define SCORE_TEXT_X			28//60//40
#define SCORE_TEXT_Y			63//90//80
#define SCORE_NUM_X				100//120//80
#define SCORE_NUM_Y				63//90//80

//��Ϸ������ÿ�ع�����ʾ����
#define	X_ROUNDBACK				14//11
#define	Y_ROUNDBACK				48//55
#define	ROUNDBACK_WIDTH			106
#define	ROUNDBACK_HEIGHT		46

//��Ϸ��������
#define	X_GAMEOVERBACK			0			
#define	Y_GAMEOVERBACK			0

//��Ϸ������ÿ�ع�����������
#define	X_ROUNDNUM_FIRST		55//50
#define	X_ROUNDNUM_SECOND		68
#define	Y_ROUNDNUM				74//81
#define	ROUNDNUM_WIDTH			13
#define	ROUNDNUM_HEIGHT			12

//��Ϸ����ʱ��ʾ��GAMEOVER����
#define	X_GAMEOVER				16//5
#define	Y_GAMEOVER				48//46



/**********************************************
*
*	�ַ�����Դ
*
***********************************************/

#define	IDS_PAO				ML("Paopao")
#define	IDS_EXIT			ML("Exit")
#define	IDS_CONFIRM			ML("OK")
#define	IDS_OK				ML("Ok")
#define	IDS_NEWGAME			ML("New")
#define	IDS_SETUP			ML("Setup")
#define	IDS_HEROBOARD		ML("Hero board")
#define	IDS_GAMESTART		ML("Start")
#define	IDS_SCORE			ML("Scores")
#define	IDS_ANYMOUS			ML("Anymous")
#define	IDS_CLEAR			ML("Clear")
#define	IDS_GAME			ML("Game")
#define	IDS_PRESS			ML("Press")
#define	IDS_MOVE			ML("Move")//"�ƶ���̨"
#define	IDS_ENDGAME			ML("Over")
#define IDS_GAMEOVER		ML("Game over")
#define	IDS_PROMPT			ML("Prompt")//"��ʾ"
#define	IDS_WRITE_ERROR		ML("Failed to save\ntry it later!")//"д�ļ�ʧ�ܣ����Ժ�����"
#define	IDS_READ_SETUP_ERROR		ML("Unable to\nget setting")//"�޷���ȡ���ã����Ժ�����"
#define	IDS_READ_HERO_ERROR			ML("Unable to\nget Hero board")//"�޷���ȡӢ�۰����Ժ�����"
#define	IDS_SETNAME			ML("Set name")//"Ӣ������"
#define	IDS_CHEER			ML("Congratulation!")//"��ϲ!��ϲ!"
#define	IDS_PLSETNAME		ML("Your name")//"��Ӣ�����´���!"
#define	IDS_BACK			ML("Back")
#define IDS_GAMESHOW		ML("Help") 

#define	IDS_HELPTEXT		ML("Paohelp")
//#define IDS_HELPTEXT		("Object:\nThe goal is to clear all the bubbles from the board, scoring as many points as possible. Finish all the stages, be Hero of the Bubble Shooter and be recorded.\nHint:\nWhen three or more bubbles of the same color come together, they all explode. In the setting options, one can choose timing shooting and a moving barbette to increase the difficulty of the game.\nChart:\nthe top five scores saved are hero records.")
#define IDS_GAME_PROMPT		ML("Are you sure?")
#define IDS_CONTINUE		ML("Play")//ML("Continue")
#define IDS_PAUSE			ML("Pause")
#define IDS_FILE_PROMPT		ML("Lake of memory,\nplease release the redundant file!")
#define IDS_FILE_NOTICE		ML("Notice")
#define IDS_INPUT_NOTICE	ML("Input Notice")
