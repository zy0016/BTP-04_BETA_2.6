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
//#include    "window.h"
//#include    "string.h"
//#include    "stdlib.h"
//#include    "stdio.h" 
//#include    "winpda.h"
//#include    "str_plx.h"
////#include    "fapi.h"
//#include	"sys/types.h"
//#include	"sys/stat.h"
//#include	"fcntl.h"
//#include	"unistd.h" 
//
//#include	"dirent.h"
//
//#include    "setting.h"
//#include    "str_public.h"
//#include    "plx_pdaex.h"
//#include    "hpimage.h"
//#include    "setup.h"
//#include    "MBPublic.h"
//#define     IDC_BUTTON_QUIT         3//�˳���ť
//#define     IDC_BUTTON_SET          4//ȷ����ť
//#define     IDC_BUTTON_START        5//����
//#define     IDC_RADIO1              6
//#define     IDC_RADIO2              7
//#define     IDC_RADIO3              8
//#define     IDC_RADIO4              9
//#define     IDC_LIST                10
//#define     IDC_INIT                WM_USER + 100
//#define     IDC_PREVIEW             11//ȫ��Ԥ����ť
//
//#define     TITLECAPTION            GetString(STR_SETUP_CARTOON_CAPTION)//��������
//#define     BUTTONOK                (LPARAM)GetString(STR_WINDOW_OK)    //ȷ��
//#define     BUTTONQUIT              (LPARAM)GetString(STR_WINDOW_EXIT)  //�˳�
//#define     BUTTONSTARTTXT          (LPARAM)GetString(STR_WINDOW_STARTUP)//����
//#define     ERROR1                  GetString(STR_SETUP_ERROR1)         //������ʾ
//#define     WINDOWICO               (LPARAM)""    //����ͼ��
//
//#define     RADIOTXT0               GetString(STR_SETUP_CARTOON_RADIOTXT0)//��
//#define     RADIOTXT1               GetString(STR_SETUP_CARTOON_RADIOTXT1)//����
//#define     RADIOTXT2               GetString(STR_SETUP_CARTOON_RADIOTXT2)//�ػ�
//#define     RADIOTXT3               GetString(STR_SETUP_CARTOON_RADIOTXT3)//����
//#define     RADIOTXT4               GetString(STR_SETUP_CARTOON_RADIOTXT4)//����
//#define     CARTOONTXT              GetString(STR_SETUP_CARTOON_CARTOONTXT)//����%d
//#define     SAVESMSUCC              GetString(STR_SETUP_SM_SETSUCC)     //���óɹ�
//#define     SAVESMERROR             GetString(STR_SETUP_SMSAVEFAIL)     //���ñ���ʧ��
//#define     BUTTON_PREVIEW          GetString(STR_SETUP_CARTOON_PREVIEW)//ȫ��Ԥ��
//#define     CLEW                    GetString(STR_SETUP_CARTOON_CLEW)//%s��������Ϊ��,�Ƿ����?
//#define     SIZEINVALID             GetString(STR_SETUP_CARTOON_SIZEINVALID)//"���ʵĶ�����СΪ240*120
//
//#define     SN_CARTOON              "cartoon"       //�������ýڵ�
//#define     KN_CARTOON_OPEN         "cartoon_open"  //�����ؼ�����
//#define     KN_CARTOON_CLOSE        "cartoon_close" //�ػ��ؼ�����
//#define     KN_CARTOON_SCREEN       "cartoon_screen"//�����ؼ�����
//#define     KN_CARTOON_CALL         "cartoon_call"  //����ؼ�����
//
//typedef struct 
//{
//    int x1;
//    int y1;
//    int x2;
//    int y2;
//    int nHeight;
//    int nWidth;
//}ROUNDRECT;
//
//#define     DEFGIF0                 "/rom/setup/personal/g1.gif"
//#define     DEFGIF1                 "/rom/setup/personal/g2.gif"
//#define     DEFGIF2                 "/rom/setup/personal/g3.gif"
//#define     DEFGIF3                 "/rom/setup/personal/g7.gif"
//
//#define     CSHOWNAMEMAXLEN         20
//typedef struct cartoon
//{
//    char                cFilename[CARTOONFILENAMELEN + 1];  //����ʵ���ļ���(����·��)
//    char                cShowname[CSHOWNAMEMAXLEN + 1];     //��������ʾ����
//    struct cartoon  *   next;
//};
//typedef struct cartoon CARTOONS;
