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
#ifndef	_BOMBRS_H
#define	_BOMBRS_H
                             
#include	"str_public.h"
#include	"str_plx.h"
#include	"mullang.h"

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

#define WND_X		0
#define WND_Y		0
#define WND_WIDTH	176
#define WND_HEIGHT	220
#define TOPSPACE	21
#define	BOTTOMSPACE	21

#define	X_COVERBG		0
#define	Y_COVERBG		0
#define	COVERBG_WIDTH	176
#define	COVERBG_HEIGHT	182//199
#define	X_LT_MENUITEM	60
#define	Y_LT_MENUITEM	108
#define	MENUITEM_WIDTH	70
#define	MENUITEM_HEIGHT	16
#define	X_LT_SELITEM	43//40
#define	Y_LT_SELITEM	92//112
#define	SELITEM_WIDTH	17
#define	SELITEM_HEIGHT	8
#define	Y_MENUITEM_INTERVAL		22//18

#define	X_MENUITEM		60
#define	Y_MENUITEM		106

#define	X_BMP_GAMEBACK	0
#define	Y_BMP_GAMEBACK	0
#define	GAMEBACK_WIDTH	176
#define	GAMEBACK_HEIGHT	181//199

#define	X_LT_SEASURFACE			0			//ˮ�����
#define	Y_LT_SEASURFACE			57//66			//ˮ��������
#define	SEASURFACE_WIDTH		176			//ˮ����ͼ���
#define	SEASURFACE_HEIGHT		4			//ˮ����ͼ�߶�

#define	X_WAVE_LEFT_MAX			-12			//�˵ļ���ֵ
#define	X_WAVE_INTERVAL			3			//��һ���x���򳤶�
#define	Y_WAVE_TOP				Y_LT_SEASURFACE	//��������
#define	WAVE_WIDTH				15			//һ���˵Ŀ��
#define	WAVE_HEIGHT				4			//�˵ĸ߶�

#define	NORMAL_GUNSHIP_WIDTH	24			//�ڴ����
#define	MIN_GUNSHIP_WIDTH		18			//�ڴ���С��Ŀ��
#define	NORMAL_GUNSHIP_HEIGHT	12			//�ڴ��߶�
#define	Y_GUNSHIP_TOP			45//54			//�ڴ�������
#define	X_GUNSHIP_MOVE_INTERVAL	5			//�ڴ��ƶ����

#define	SUBMARINE_WIDTH			30			//Ǳͧ���
#define	SUBMARINE_HEIGHT		20			//Ǳͧ�߶�
#define	Y_SUBMARINE				0			//Y������ʼ��
//����Ǳͧ
#define	X_SUBMARINE_TOR			0
//����Ǳͧ
#define	X_SUBMARINE_TOL			(X_SUBMARINE_TOR+SUBMARINE_WIDTH)

#define	SEA_WIDTH				176			//���Ŀ�
#define	SEA_HEIGHT				104//110			//������

#define	X_LTFIRSTSHIPLIVE		31//30			//��һ�����ĺ�����
#define	Y_LTFIRSTSHIPLIVE		167//184			//��һ������������
#define	SHIPLIVE_WIDTH			6			//���Ŀ�
#define	SHIPLIVE_HEIGHT			10			//���ĸ�
#define	X_SHIPLIVE_INTERVAL		0			//���ĺ�����

#define	X_LTFIRSTAVAILBOMB		73//64//72			//��һ������ը���ĺ�����
#define	Y_LTFIRSTAVAILBOMB		168//184			//��һ������ը����������
#define	AVAILBOMB_WIDTH			SHIPLIVE_WIDTH		//����ը���Ŀ�
#define	AVAILBOMB_HEIGHT		SHIPLIVE_HEIGHT		//����ը���ĸ�
#define	X_AVAILBOMB_INTERVAL	0			//����ը���ĺ�����

#define	X_SHIPLIFE_OFBMP		0
#define	X_AVAILBOMB_OFBMP		AVAILBOMB_WIDTH

#define	X_GAMESCORE				148			//�÷��ַ���������
#define	Y_GAMESCORE				165//180//184			//�÷��ַ���������

#define	X_GAMESCORETEXT			100//110
#define	Y_GAMESCORETEXT			178
#define	GAMESCORETEXT_WIDTH		40
#define	GAMESCORETEXT_HEIGHT	30

#define	TORPEDO_WIDTH			7			//���׵Ŀ�
#define	TORPEDO_HEIGHT			7			//���׵ĸ�
#define	X_TORPEDO				0			//������ʼ����
#define	X_MISSIL				(X_TORPEDO+TORPEDO_WIDTH)	//������ʼ����

#define	Y_MISSIL_VALID			96			//������Ч������

#define	BOMB_SHOT_WIDTH			10			//�����ը�����
#define	BOMB_SHOT_HEIGHT		5			//�����ը���߶�
#define	PRIZE_WIDTH				10			//�����Ŀ�
#define	PRIZE_HEIGHT			5			//�����ĸ�
#define	NOENEMY_WIDTH			5			//�޵б�־�Ŀ�
#define	NOENEMY_HEIGHT			5			//�޵б�־�ĸ�

#define	X_NOENEMY				0			//�޵б�־������
#define	X_BOMB_SHOT				(X_NOENEMY+NOENEMY_WIDTH)
//�ɻ�
#define	X_PLANE_START			(176-33)
#define	Y_PLANE_START			30
#define	PLANE_WIDTH				33
#define	PLANE_HEIGHT			10
#define	PLANE_MOVE_INTERVAL		15

#define	X_HERO_REC				30
#define	Y_HERO_REC				56
#define	HERO_REC_SPACE			17
//���ý���STRSPIN�ؼ�
#define	X_SPINMAX				45
#define	Y_SPINMAX				60
#define	SPINMAX_WIDTH			80
#define	SPINMAX_HEIGHT			20
//���ý������ը�����ַ���λ��
#define	X_MAX_NUM				50//45
#define	Y_MAX_NUM				40
//���ý���BUTTON�ؼ�
#define	X_BTN_PRIZE				45
#define	Y_BTN_PRIZE				100
#define	BTN_PRIZE_WIDTH			90//80
#define	BTN_PRIZE_HEIGHT		25

#define	X_GAMEOVER			28
#define	Y_GAMEOVER			62


/**********************************************
*
*	�ַ�����Դ
*lmx modify
***********************************************/
//#ifdef SCP_SMARTPHONE
#define	IDS_EXIT			ML("Exit")
#define	IDS_CONFIRM			ML("Ok")
#define	IDS_NEWGAME			ML("New")
#define	IDS_NEW				ML("New")
#define	IDS_SETUP			ML("Setup")
//#define	IDS_OPTION			ML("Setup")
#define	IDS_HEROBOARD		ML("Hero board")
#define	IDS_SCORES			ML("Scores")
#define	IDS_DEPTHBOMB		ML("DepthBomb")
#define	IDS_BACK			ML("Back")
#define	IDS_BEGIN			ML("Begin")
#define	IDS_STOP			ML("Stop")
#define	IDS_CONTINUE		ML("Continue")
#define	IDS_PAUSE			ML("Pause")
#define	SCORE_FORMAT		ML("%d")
#define	IDS_ANYMOUS			ML("Anymous")
#define	IDS_CLEAR			ML("Clear")
#define	IDS_MAX_BOMB_NUM	ML("Max bomb NR")
#define	IDS_SPECIEL_EFFECT	ML("Special")
#define	IDS_THREE			ML("Three")
#define	IDS_FOUR			ML("Four")
#define	IDS_FIVE			ML("Five")
#define	IDS_SETNAME			ML("Set name")
#define	IDS_CHEER			ML("Congratulation!")
#define	IDS_PLSETNAME		ML("Your name")
#define	IDS_GAME_PROMPT		ML("Are you sure?")
#define	IDS_HELP			ML("Help")
#define	IDS_GAMEOVER		ML("Game over")
//#define	IDS_HELPTEXT		("Game overview:\nBattle At Sea is an intense action game that should give you many hours of frustration, I mean fun. You are the commander of a battleship you must protect the country by destroying wave after wave of enemy submarines.\nInstructions:\nFive depth charges can be on screen at once time.Left and right\narrow keys move the\nship,down arrow project a bomb once.")
#define	IDS_HELPTEXT			ML("BombHelp")

#define	SEL_COLOR	RGB(255,0,0)

#endif
