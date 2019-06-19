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
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
//#include    "fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"sys/vfs.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include	"dirent.h"
#include    "setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "setup.h"
#include    "pmi.h"
#include    "malloc.h"

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť
#define     IDC_LIST                5

#define     TITLECAPTION            GetString(STR_SETUP_MEMORYSTAT_CAPTION)//�ռ�鿴
#define     BUTTONOK                (LPARAM)GetString(STR_WINDOW_OK)    //ȷ��
#define     BUTTONQUIT              (LPARAM)GetString(STR_WINDOW_EXIT)  //�˳�
#define     ERROR1                  GetString(STR_SETUP_ERROR1)         //������ʾ
#define     WINDOWICO               (LPARAM)""   //����ͼ��

#define     MEMORYTXT0              GetString(STR_SETUP_MEMORYSTAT_TXT1)//MMC��ʹ�����
#define     MEMORYTXT1              GetString(STR_SETUP_MEMORYSTAT_TXT0)//�ռ�ʹ�����"

#define     LISTTXT0                GetString(STR_SETUP_MEMORYSTAT_LISTTXT0)//"����"
#define     LISTTXT1                GetString(STR_SETUP_MEMORYSTAT_LISTTXT1)//"����"
#define     LISTTXT2                GetString(STR_SETUP_MEMORYSTAT_LISTTXT2)//"��ϵ��
#define     LISTTXT3                GetString(STR_SETUP_MEMORYSTAT_LISTTXT3)//"���±�
#define     LISTTXT4                GetString(STR_SETUP_MEMORYSTAT_LISTTXT4)//"�ټ�"
#define     LISTTXT5                GetString(STR_SETUP_MEMORYSTAT_LISTTXT5)//"�ճ�"
#define     LISTTXT6                GetString(STR_SETUP_MEMORYSTAT_LISTTXT6)//"ͼƬ"
#define     LISTTXT7                GetString(STR_SETUP_MEMORYSTAT_LISTTXT7)//"¼����
#define     LISTTXT8                GetString(STR_SETUP_MEMORYSTAT_LISTTXT8)//"��Ϸ"

typedef struct 
{
    DWORD iUsedSMS;                 //���ն���k��
    DWORD iTotalSMS;                //�������k��

    DWORD iUsedMMS;                 //���ղ���k��
    DWORD iTotalMMS;                //�������k��

    DWORD iUsedLinkman;             //��ϵ������k��
    DWORD iTotalLinkman;            //��ϵ�����k��

    DWORD iUsedNodepad;             //���±�
    DWORD iTotalNodepad;            //

    DWORD iUsedSteno;               //�ټ�
    DWORD iTotalSteno;              //

    DWORD iUsedCale;                //�ճ�
    DWORD iTotalCale;               //

    DWORD iUsedPic;                 //ͼƬ
    DWORD iTotalPic;                //

    DWORD iUsedRec;                 //¼����
    DWORD iTotalRec;                //

    DWORD iUsedGame;                //��Ϸ
    DWORD iTotalGame;               //
}MEMORYSTAT;

extern  DWORD   GetAllTextSize();       //���±�
extern  DWORD   GetLenthOfStenoFiles(); //�ټ�
extern  int     GetSMSSpace(void);      //����
extern  int     APP_GetPhoneBookFileSize(void);//�����ϵ���ļ��ߴ�,�ļ��ߴ�(KB). -1 = ʧ�ܡ�
extern  int     APP_GetMMSSize(void);   //����
extern  DWORD   GetRecorderFileSize();  //���¼���������ļ���ռ�ռ�
extern  DWORD   GetAllImageFileSize();  //���ͼƬ��ռ�ռ�
extern  DWORD   GetDatebookFileSize( void );//�ճ��ڴ�ռ�鿴
