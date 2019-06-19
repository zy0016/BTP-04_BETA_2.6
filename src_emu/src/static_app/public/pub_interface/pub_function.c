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
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "str_plx.h"
#include "str_public.h"
#include "plx_pdaex.h"
#include "pubapp.h"
#include "setup/setting.h"
//#include "fapi.h"
#include "imesys.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h" 
#include "winpda.h"
#include "hpimage.h"
#include "setup/setup.h"
#include "ab/phonebookext.h"
#include "callapp/MBPublic.h"

static const char cCh_A = 'A';
static const char cCh_M = 'M';
static const char cCh_R = 'R';
static const char cCh_I = 'I';
static const char cCh_F = 'F';
static const char cCh_D = 'D';
static const char cCh_T = 'T';
static const char cCh_h = 'h';
static const char cCh_d = 'd';
static const char * pListBox    = "LISTBOX";
static const char * pComboBox   = "COMBOBOX";
static const char * pStrSpinBox = "STRSPINBOX";

extern  BOOL    SetupHf_PlayMusicFromFile(const char * pFilename,BYTE byPlayType,
                                  BYTE byPlayCount,void(*callbackproc)(BYTE));
extern  BOOL    SetupHf_PlayMusicFromData(BYTE byRingType,BYTE byPlayTime,BYTE *pBuf,
                                  unsigned long DataSize, void(*callbackproc)(BYTE));
extern  int     GetSIMState();
extern  BOOL    CallAutoCloseClewWindow(void);  //�Զ��ػ�����
extern  int     GetBright(void);                //��ñ���������[1,6]
//extern  void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen);//��õ�ǰ����ĵ绰����
//////////////////////////���غ���////////////////////////////////////////////////
//static  BOOL    CreateWindow_PlayMusic(void);//����һ�����ش������ڲ�������,�ú�����ϵͳ������ʱ�򴴽�
static  long    GetRingPRI(MUSICTYPE iRingType);
static  void    ResetCurrentRingType(void);//���Ž�����ʱ��,�����ڲ��ŵ����������λ
static  LRESULT AppWndProc_PlayMusic ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  MUSICTYPE   GetCurrentRingType(void);//��õ�ǰ���ڲ��ŵ����������
/********************************************************************
* Function   FindSIMState  
* Purpose     ���SIM����״̬.
* Params     bShow:�Ƿ���ʾ��ǰ״̬��ʾ
* Return     ��ǰ״̬���
* Remarks      
**********************************************************************/
BOOL    FindSIMState(BOOL bShow)
{
    int istate = 1;
#ifndef _EMULATE_
    istate = GetSIMState();
#endif
    if (bShow)
    {
        if(0 == istate)
            AppMessageBox(NULL,ML("No SIM card"),ML("Clew"),WAITTIMEOUT);//δ����SIM��
    }
    return (istate);
}
/********************************************************************
* Function   AddString  
* Purpose    ��ָ������**p�е�������ӵ��ؼ���
* Params     
* Return     
* Remarks    
**********************************************************************/
BOOL    AddString(HWND hWnd,char ** p)
{
    char cClassName[15] = "";
    UINT wMsgCmd,uSelCur;
    if (!GetClassName(hWnd,cClassName,sizeof(cClassName)))
        return FALSE;
    if (strcmp(cClassName,pListBox) == 0)
    {
        wMsgCmd = LB_ADDSTRING;
        uSelCur = LB_SETCURSEL;
    }
    else if (strcmp(cClassName,pComboBox) == 0)
    {
        wMsgCmd = CB_ADDSTRING;
        uSelCur = CB_SETCURSEL;
    }
    else if (strcmp(cClassName,pStrSpinBox) == 0)
    {
        wMsgCmd = SSBM_ADDSTRING;
        uSelCur = SSBM_SETCURSEL;
    }
    else
    {
        return FALSE;
    }
    while (** p)
        SendMessage(hWnd,wMsgCmd,0,(LPARAM)* p++);

    SendMessage(hWnd,uSelCur,0,0);
    return TRUE;
}
/********************************************************************
* Function   LoadHbitmaps_Gif  
* Purpose    ����**P�е�λͼ
* Params     ** pλͼ�ļ�����ָ������
* Return     
* Remarks    
**********************************************************************/
void    LoadHbitmaps_Gif(HWND hWnd,HBITMAP * hbitmap,char ** p)
{
    COLORREF    Color;
    BOOL        bGif;
    HDC         hdc;

    hdc = GetDC(hWnd);
    while (** p)
        * hbitmap++ = CreateBitmapFromImageFile(hdc,* p++,&Color,&bGif);

    ReleaseDC(hWnd,hdc);
}
/********************************************************************
* Function   List_AddString  
* Purpose    ����Ӧ��������ӵ��б����
* Params     bAppend:ͼ���Ƿ��Ǹ�����,TRUE�Ļ�,hIconNormalָ����Ҫ�ƶ�,����hIconNormalָ�벻��
* Return     
* Remarks    
**********************************************************************/
void    List_AddString(HWND hList,char ** p,HBITMAP * hIconNormal,BOOL bAppend)
{
    DWORD   dWord;
    int     i = 0;

    while (** p)
    {
        SendMessage(hList,LB_ADDSTRING,0,(LPARAM)* p++);

        if (hIconNormal != NULL)
        {
            dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i++);
            SendMessage(hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)* hIconNormal);
            if (bAppend)
                hIconNormal++;
        }
    }
    SendMessage(hList,LB_SETCURSEL,0,0);
}
/********************************************************************
* Function   Setup_Init  
* Purpose    �ṩ�����������,���ڳ�ʼ�����ø�������
* Params     
* Return     
* Remarks    
**********************************************************************/
/*
BOOL    Setup_InitFunction(void)
{
    SCENEMODE sm;

//    SetCurrentLanguage(GetLanguage());  //���õ�ǰ����
    CallAutoCloseClewWindow();          //�����Զ��ػ�����
    SetUp_SetDialCfg();                 //��������������������

    SetupHf_LcdBklight(GetBglight());   //����LCD�ı���� GetBglight()
    SetupHf_BgLight(GetBright());       //�������Ȳ��� GetBright()

  //  CreateWindow_PlayMusic();           //��ʼ����������//xlzhu

    GetSM(&sm,GetCurSceneMode());       //����龰ģʽ��Ϣ
    if ((sm.iComeview == LIBRATE) || (sm.iComeview == RINGLIBRATE))//����ģʽ(1:��;2:������)
        BeginVibration(1);//��1��

    return (TRUE);
}
*/
//////////////////////////����Ϊ�������Ų���begin//////////////////////////////////////////
static const char * pClassName = "PlayMusicWndClass";
static HWND         hWndApp;
static char         cRingName[RINGNAMEMAXLEN + 1] = ""; //�����ļ���
static int          iCallCount;                         //���Ŵ���,0Ϊѭ������,����Ϊ���Ŵ���

#define ID_TIMER_VIBRATION      9	//�����𶯵ļ�ʱ��
#define ID_TIMER_MUSICPLAY      10	//�������ֲ��ŵļ�ʱ��
#define WM_MMWRITE              WM_USER + 100
#define WM_MMSTOP               WM_USER + 110
#define WM_MMGETDATA            WM_USER + 115
#define IDC_PLAYMUSIC           WM_USER + 125	//����һ���������ŵ���Ϣ



static  BOOL                bCanPlayMusic_Key = TRUE;//�Ƿ���Բ��Ű�����
static  BOOL                bCanPlayMusic     = TRUE;//��ǰ�Ƿ���Բ�������
static  long                dCanPriority;//�������ּ���
static  MUSICTYPE           CurrentRingType   = RING_WITHOUT;//��ǰ���ڲ���ʲô��������
static  SetupHS_PlayMusic   setuphs_playmusic;
static  unsigned    char *  pBuffer_Play = NULL;//���ݻ�����λ��
typedef struct 
{
    MUSICTYPE   iRingType;          //�������
    long        lRingPRI;           //��������������ȼ�
    BOOL        bCallResetRingType; //ֹͣ�������ֵ�ʱ��,�Ƿ���Ҫ����ResetCurrentRingType����
}RINGPRI;
static const RINGPRI ringpri[] =//����������ȼ��б�
{
    {RING_PHONE   ,WAVEOUT_PRIO_HIGH       ,TRUE}, //������ʾ��
    {RING_ALARM   ,WAVEOUT_PRIO_BELOWHIGH  ,TRUE}, //����
    {RING_CALENDAR,WAVEOUT_PRIO_BELOWHIGH  ,TRUE}, //�ճ�
    {RING_POWNON  ,WAVEOUT_PRIO_LOW        ,FALSE},//��������
    {RING_POWNOFF ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//�ػ�����                                              
    {RING_SMS     ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//����                                                  
    {RING_MMS     ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//����    
    {RING_WARNING ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//������
    {RING_CAMERA  ,WAVEOUT_PRIO_NORMAL     ,FALSE},//������ʾ��
    {RING_KEY     ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//������                                                
    {RING_TOUCH   ,WAVEOUT_PRIO_NORMAL     ,FALSE},//��ͨ��ʾ��                                             
    {RING_SCREEN  ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//������                                                 
    {RING_CLEW50  ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//50����ʾ��                                             
    {RING_OTHER   ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //������������
    {RING_WAP     ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //������е������ļ���������
    {RING_MMSAPP  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //����Ӧ���е��ļ���������
    {RING_PUBLIC  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //����Ԥ���е��ļ�
    {RING_VOLUME  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //���������ú�                                            
    {RING_WITHOUT ,0                       ,0}     //û����������                                             
};//xlzhu
/********************************************************************
* Function   SetCanPlayMusic_Key  
* Purpose    ���õ�ǰ�Ƿ���Բ��Ű�����
* Params     bCanPlay:TRUE:���Բ�������,FALSE:����������
* Return     ��������֮ǰ��״̬
* Remarks    
**********************************************************************/
BOOL    SetCanPlayMusic_Key(BOOL bCanPlay)
{
    BOOL bPlayBak = bCanPlayMusic_Key;

    bCanPlayMusic_Key = bCanPlay;

    return bPlayBak;
}
/********************************************************************
* Function   GetCanPlayMusic_Key  
* Purpose    ��õ�ǰ�Ƿ���Բ��Ű�����
* Params     
* Return     TRUE:���Բ�������,FALSE:����������
* Remarks    
**********************************************************************/
static  BOOL    GetCanPlayMusic_Key(void)
{
    return bCanPlayMusic_Key;
}
/********************************************************************
* Function   SetCanPlayMusic  
* Purpose    ���õ�ǰ�Ƿ���Բ�������
* Params     bCanPlay:TRUE:���Բ�������,FALSE:����������
            dPriority:bCanPlayΪFALSE��ʱ��,��ֹ���ŵ����������ȼ�,��dPriority
            ����͵����ֽ��޷�����(�����������dPriority������),��dPriority
            ����ߵ����ֽ���������
            bCanPlayΪTRUE��ʱ��,dPriority������
* Return     ��������֮ǰ��״̬
* Remarks    
**********************************************************************/
BOOL    SetCanPlayMusic(BOOL bCanPlay,DWORD dPriority)
{
    BOOL bPlayBak = bCanPlayMusic;
    MUSICTYPE iCurRingType;

    bCanPlayMusic = bCanPlay;
    if (bCanPlay)//��ǰ���Բ�������
    {
        dCanPriority = 0;
    }
    else//��ǰ��ֹĳЩ��������ֲ���
    {
        dCanPriority = dPriority;
        iCurRingType = GetCurrentRingType();//��õ�ǰ���ڲ��ŵ����������

        //printf("SetCanPlayMusic iCurRingType:%d\r\n",iCurRingType);

        if (iCurRingType != RING_WITHOUT)//��ǰ����������
        {
            if (dCanPriority >= GetRingPRI(iCurRingType))
            {
                //printf("SetCanPlayMusic GetRingPRI(iCurRingType):%d\r\n",GetRingPRI(iCurRingType));

                Setup_EndPlayMusic(iCurRingType);
                ResetCurrentRingType();//���Ž�����ʱ��,�����ڲ��ŵ����������λ
            }
        }
    }
    return bPlayBak;
}
/********************************************************************
* Function   GetCanPlayMusic
* Purpose    ��õ�ǰ�Ƿ���Բ�������
* Params     
* Return     TRUE:���Բ�������,FALSE:����������
* Remarks    
**********************************************************************/
static  BOOL    GetCanPlayMusic(MUSICTYPE uiringselect)
{
    if (bCanPlayMusic)//��ǰ���Բ�������
        return bCanPlayMusic;
    //��ǰ������ĳ����������
    return (GetRingPRI(uiringselect) > dCanPriority);//��Ҫ���ŵ����������ȼ����ڵ�ǰ��ֹ�����ȼ�
}
/********************************************************************
* Function   GetRingPRI  
* Purpose    ��ø�������������ȼ�
* Params     
* Return     
* Remarks    
**********************************************************************/
static  long    GetRingPRI(MUSICTYPE iRingType)
{
    int i = 0;
    while ((ringpri[i].lRingPRI != 0) && (ringpri[i].iRingType != RING_WITHOUT))
    {
        if (ringpri[i].iRingType == iRingType)
            return (ringpri[i].lRingPRI);

        i++;
    }
    return (RING_WITHOUT);
}
/********************************************************************
* Function   bCallResetRingType  
* Purpose    ��ø����������ֹͣ�������ֵ�ʱ��,�Ƿ���Ҫ����ResetCurrentRingType����
* Params     iRingType:�������
* Return     �Ƿ���Ҫ����ResetCurrentRingType����
* Remarks    
**********************************************************************/
static  BOOL    bCallResetRingType(MUSICTYPE iRingType)
{
    int i = 0;
    while ((ringpri[i].lRingPRI != 0) && (ringpri[i].iRingType != RING_WITHOUT))
    {
        if (ringpri[i].iRingType == iRingType)
            return (ringpri[i].bCallResetRingType);

        i++;
    }
    return (FALSE);
}
static  MUSICTYPE   GetCurrentRingType(void)//��õ�ǰ���ڲ��ŵ����������
{
    return (CurrentRingType);
}
/********************************************************************
* Function   SetCurrentRingType  
* Purpose    ע�ᵱǰ��Ҫ���ŵ����������,ֻ��ע��ɹ����ܲ���
* Params     uiringselect:���������RING_SELECT,setting.h�ļ��ж���
* Return     û�����ֲ��ŷ���0,ע��ɹ�����1,ʧ�ܷ���-1
* Remarks    ��Ҫ���ŵ����������ȼ�ֻ��>��ǰ���������ȼ�,ע����ܳɹ�
**********************************************************************/
static  int     SetCurrentRingType(MUSICTYPE uiringselect)
{
    MUSICTYPE iCurRingType;
    switch (uiringselect)
    {
    case RING_PHONE :   //������ʾ��       
    case RING_ALARM:    //����
    case RING_CALENDAR: //�ճ�
    case RING_POWNON:   //��������                                               
    case RING_POWNOFF:  //�ػ�����                                               
    case RING_SMS:      //����                                                  
    case RING_MMS:      //����      
    case RING_WARNING:  //������
    case RING_CAMERA:   //������ʾ��
    case RING_KEY:      //������                                                
    case RING_TOUCH:    //��ͨ��ʾ��                                              
    case RING_SCREEN:   //������                                                 
    case RING_CLEW50:   //50����ʾ��                                                
    case RING_OTHER:    //������������
    case RING_WAP:      //������е������ļ���������
    case RING_MMSAPP:   //����Ӧ���е��ļ���������
    case RING_PUBLIC:   //����Ԥ���е��ļ�
    case RING_VOLUME:   //���������ú�
        break;
    default://�Ƿ�����������
        return -1;
    }
    iCurRingType = GetCurrentRingType();//��õ�ǰ�������
    //printf("\n��ǰ�������:%d,��Ҫ���ŵ��������:%d\n",iCurRingType,uiringselect);
    if (iCurRingType == RING_WITHOUT)//��ǰû����������
    {
        CurrentRingType = uiringselect;//��¼�������
        return 0;
    }
    //��ǰ���������ڲ���,��Ҫ�ж����������ֵ����ȼ��Ƿ���ڵ�ǰ���ֵ����ȼ�
    if (GetRingPRI(uiringselect) > GetRingPRI(iCurRingType))
    {   //���������ֵ����ȼ����ڵ�ǰ���ֵ����ȼ�
        Setup_EndPlayMusic(uiringselect);
        CurrentRingType = uiringselect;//��¼�������
#ifndef _EMULATE_
        usleep(400000L);
#endif
        return 1;
    }
    else//���������ֵ����ȼ�С�ڵ�ǰ���ֵ����ȼ�
    {
        //printf("\nע��ʧ��,��ǰ���ڲ��ŵ��������:%d,��Ҫ���ŵ��������:%d\n",iCurRingType,uiringselect);
        return -1;
    }
}
static  void    ResetCurrentRingType(void)//���Ž�����ʱ��,�����ڲ��ŵ����������λ
{
    CurrentRingType = RING_WITHOUT;
    //printf("\n�������λ\n");
}
/********************************************************************
* Function   GetMusicFile_DataType  
* Purpose    �����ļ������������Ƿ��ǺϷ���������������
* Params     pFileName:��Ҫ�����ļ�����,pBufferData:��Ҫ����������
                ulBufferSize:�������ĳ���
* Return     FILETYPE:�ļ����ͻ�����������
* Remarks    
**********************************************************************/
static  FILETYPE  GetMusicFile_DataType(const char * pFileName,
                                          const unsigned char * pBufferData,
                                          unsigned long ulBufferSize)
{
    int hf;
    unsigned char strAudioType[AUDIOTYPE] = "";
    unsigned long lDataSize;//�����ļ����Ȼ����������ĳ���
    const unsigned char * p = NULL;
    FILETYPE FileType;
    int i;
	struct stat *buf = NULL;

    //WAVEFORMATEX WaveFormatex;

    if (pFileName != NULL)
    {
        hf = open(pFileName,O_RDONLY);
        if (-1 == hf)
            return OPENFAIL;
		buf = malloc(sizeof(struct stat));

		if(buf == NULL)
		{
			close(hf);
			return OPENFAIL;
		}
		
		memset(buf, 0, sizeof(struct stat));
		stat(pFileName, buf);
		
		lDataSize = buf->st_size;
		
		free(buf);
		buf = NULL;
	
		if (lDataSize <= AUDIOTYPE)//�ļ�����̫��
            return INVALID_TYPE;

        if (sizeof(strAudioType) != read(hf,strAudioType,sizeof(strAudioType)))//��ȡ�ļ�ͷ
            return OPENFAIL;
    }
    else if ((pBufferData != NULL) && (ulBufferSize != 0))
    {
        lDataSize = ulBufferSize;   //��������������
        if (lDataSize <= AUDIOTYPE) //����������̫��
            return INVALID_TYPE;

        p = pBufferData;
        for (i = 0;i < AUDIOTYPE;i++)
            strAudioType[i] = * p++;
    }
    else
    {
        return INVALID_TYPE;
    }
    //��ʼ����ļ�ͷ
    if ((strAudioType[0] == 0x23 ) && (strAudioType[1] == 0x21) && 
        (strAudioType[2] == cCh_A) && (strAudioType[3] == cCh_M) &&
        (strAudioType[4] == cCh_R) && (strAudioType[5] == 0x0a))
    {
        FileType = AMR_TYPE;
    }
    else if ((strAudioType[0] == cCh_R) && (strAudioType[1] == cCh_I) && 
             (strAudioType[2] == cCh_F) && (strAudioType[3] == cCh_F))
    {
        if (lDataSize <= sizeof(WAVEFORMATEX) + 0x16)//����weve�ļ�Ҫ�����ļ�ͷ�Ƿ�Ϸ�
            return INVALID_TYPE;

        FileType =  WAVE_TYPE;
    }
    else if ((strAudioType[0] == cCh_M) && (strAudioType[1] == cCh_T) && 
             (strAudioType[2] == cCh_h) && (strAudioType[3] == cCh_d))
    {
        FileType =  MIDI_TYPE;
    }
    else if ((strAudioType[0] == cCh_M) && (strAudioType[1] == cCh_M) &&
             (strAudioType[2] == cCh_M) && (strAudioType[3] == cCh_D))
    {
        FileType =  MMF_TYPE;
    }
    else
    {
        FileType =  INVALID_TYPE;
    }
    if (pFileName != NULL)
        close(hf);

    return FileType;
}
/********************************************************************
* Function   MusicCallBack  
* Purpose    ֹͣ���ŵĻص�����
* Params     PlayEndTypeֹͣ���ŵĵ�����
* Return     
* Remarks    
**********************************************************************/
static  void	MusicCallBack(BYTE PlayEndType)
{
	switch (PlayEndType)
    {
    case 0://����һ��
    case 1://ȫ������
        PostMessage(setuphs_playmusic.hWnd,setuphs_playmusic.iWM_MMSTOP,PLAY_OVER,0);
        return;
    case 2://�����������ж�
        PostMessage(setuphs_playmusic.hWnd,setuphs_playmusic.iWM_MMSTOP,PLAY_BREAK,0);
        return;
    }
}
/******************************************************************
* Function   SETUP_PlayMusic  
* Purpose    ���������ļ�
* Params     
             hWnd:      ���÷��Ĵ��ھ��
             iWM_MMWRITE:���÷��������Ϣ,���÷������ڸ���Ϣ�е���Setup_WriteMusicData����
            �Ƽ���������:Setup_WriteMusicData((LPWAVEHDR)lParam);
            iWM_MMSTOP: ���÷��������Ϣ,����֪ͨ���÷������������,���÷������ڸ���Ϣ�е���Setup_EndPlayMusic()����
            ������������ϵ�ʱ��,iWM_MMSTOP��Ϣ�е�wParam=PLAY_OVER��ʾ���ֲ������,��������������
            pBuffer:��������,
            ulBufferSize:��������ĳ���
* Return        ��setting.h
* Remarks      
**********************************************************************/
PlayMusic_Result    SETUP_PlayMusic(SetupHS_PlayMusic * hs)
{
    SCENEMODE sm;//����龰ģʽ��Ϣ

    if (!GetCanPlayMusic(hs->uiringselect))//��ǰ�����Բ��Ÿü��������
    {
        return (PLAYMUSIC_ERROR_PRI);//���ȼ�����
    }
    //�����ǰ����������setuphs_playmusic.uiringselect���ʾ��������Call_PlayMusic
    //�������Ѿ�ע�����,�����ٴ�ע��,ֱ�Ӳ���
    if (GetCurrentRingType() != hs->uiringselect)
    {
        switch (SetCurrentRingType(hs->uiringselect))
        {
        case 1://��ǰ���������ڲ���
        case 0://��ǰû�����ֲ���
            break;
        case -1://ע��ʧ��,��Ҫ���ŵ����ּ���̫��
            return (PLAYMUSIC_ERROR_PRI);//���ȼ�����
        }
    }

    GetSM(&sm,GetCurSceneMode());
    switch (hs->uiringselect)
    {
	case RING_TOUCH:
    case RING_PHONE:    //������ʾ��
		hs->iVolume = sm.rIncomeCall.iRingVolume;
		strcpy(hs->pFileName , sm.rIncomeCall.cMusicFileName);
		break;
		
    case RING_ALARM:    //����
    	hs->iVolume = sm.rAlarmClk.iRingVolume;
		strcpy(hs->pFileName, sm.rAlarmClk.cMusicFileName);
		break;

    case RING_CALENDAR: //�ճ�
    	hs->iVolume = sm.rCalendar.iRingVolume;
		strcpy(hs->pFileName, sm.rCalendar.cMusicFileName);
		break;

	case RING_SMS:      //����
    case RING_MMS:      //����
    	hs->iVolume = sm.rMessage.iRingVolume;
		strcpy(hs->pFileName,sm.rMessage.cMusicFileName);
		break;
     
    case RING_WARNING:  //������
    	hs->iVolume = sm.iNotification;
		break;
	case RING_KEY:      //������
		hs->iVolume = sm.iKeyVolume;
		break;

	case RING_POWNON:   //��������
    case RING_POWNOFF:  //�ػ�����
    case RING_CAMERA:   //������ʾ��

        //��ͨ��ʾ��
    case RING_SCREEN:   //������
    case RING_CLEW50:   //50����ʾ��
    case RING_OTHER:    //������������
    case RING_WAP:      //������е������ļ���������
    case RING_MMSAPP:   //����Ӧ���е��ļ���������
    case RING_PUBLIC:   //����Ԥ���е��ļ�
        //hs->iVolume = sm.iRingvolume;//������������
		//hs->iVolume = sm.iMasterVol;
        break;
    case RING_VOLUME://���������ú�,�����ɲ���iVolume����
        break;
    default:
        //printf("\nWrong sound type,unable to play,%ld\n",hs->uiringselect);
        ResetCurrentRingType();//����ʧ��,���������λ
        return (PLAYMUSIC_ERROR_RINGTYPE);//�Ƿ����������
    }

    pBuffer_Play = NULL;
    if ((hs->pBuffer != NULL) && (hs->ulBufferSize != 0))
    {   //�������ݻ�����������������
        hs->iType = GetMusicFile_DataType(NULL,hs->pBuffer,hs->ulBufferSize);
        switch (hs->iType)
        {
        case INVALID_TYPE://��֧�ֲ��ŵ������ļ����
            ResetCurrentRingType();//����ʧ��,���������λ
            return PLAYMUSIC_ERROR_WAVEFORMAT;//�Ƿ���������ʽ
        case OPENFAIL://�ļ���ʧ��
            ResetCurrentRingType();//����ʧ��,���������λ
            return PLAYMUSIC_ERROR_OPENFILE;//�ļ���ʧ��
        }

        if ((pBuffer_Play = 
            (unsigned char *)malloc(setuphs_playmusic.ulBufferSize + 1)) == NULL)
        {
            printf("\nmalloc failure,%ld\n",setuphs_playmusic.ulBufferSize);
            ResetCurrentRingType();//����ʧ��,���������λ
            return (PLAYMUSIC_ERROR_MALLOC);//mallocʧ��
        }
        memcpy(&setuphs_playmusic,hs,sizeof(SetupHS_PlayMusic));

        memset(pBuffer_Play,0x00,setuphs_playmusic.ulBufferSize + 1);
        memcpy(pBuffer_Play,hs->pBuffer,hs->ulBufferSize);
        //�����ݿ���������
        setuphs_playmusic.pBuffer   = pBuffer_Play;//ָ�򱾵�������
        setuphs_playmusic.pFileName = NULL;

        if (SetupHf_PlayMusicFromData((BYTE)setuphs_playmusic.iType,1,pBuffer_Play,
            setuphs_playmusic.ulBufferSize,MusicCallBack))
            return PLAYMUSIC_SUCCESS;
        else
            return PLAYMUSIC_ERROR_PLAYFUNCTION;
    }
    else if (hs->pFileName != NULL)//���ļ����в���
    {
        hs->iType = GetMusicFile_DataType(hs->pFileName,NULL,0);
        switch (hs->iType)
        {
        case INVALID_TYPE://��֧�ֲ��ŵ������ļ����
            ResetCurrentRingType();//����ʧ��,���������λ
            return PLAYMUSIC_ERROR_WAVEFORMAT;//�Ƿ���������ʽ
        case OPENFAIL://�ļ���ʧ��
            ResetCurrentRingType();//����ʧ��,���������λ
            return PLAYMUSIC_ERROR_OPENFILE;//�ļ���ʧ��
        }
        memcpy(&setuphs_playmusic,hs,sizeof(SetupHS_PlayMusic));

        setuphs_playmusic.pBuffer = NULL;
        if (SetupHf_PlayMusicFromFile(setuphs_playmusic.pFileName,
            (BYTE)setuphs_playmusic.iType,1,MusicCallBack))
            return PLAYMUSIC_SUCCESS;
        else
            return PLAYMUSIC_ERROR_PLAYFUNCTION;
    }
    else
    {
        printf("\n�������������,�޷�����\n");
        ResetCurrentRingType();//����ʧ��,���������λ
        return (PLAYMUSIC_ERROR_PARAMETER);//�Ƿ���SetupHS_PlayMusic����
    }
}
/********************************************************************
* Function   Setup_EndPlayMusic  
* Purpose    ֹͣ��������
* Params     
* Return     
* Remarks      
**********************************************************************/
EndPlayMusic_Result Setup_EndPlayMusic(MUSICTYPE uiringselect)
{
    EndPlayMusic_Result res = STOPMUSIC_SUCCESS;
    MUSICTYPE iCurRingType;
	//stopSound(cRingName,0);
    iCurRingType = GetCurrentRingType();//��õ�ǰ���ڲ��ŵ����������

    if (iCurRingType == RING_WITHOUT)//��ǰû����������
    {
        return (STOPMUSIC_SUCCESS);//ֹͣ�ɹ�
    }
    else
    {
        //��ǰ���������ڲ���,��Ҫ�ж�ֹͣ���ֵ�Ӧ�õ����ȼ��Ƿ���ڵ�ǰ���ֵ����ȼ�
        if (GetRingPRI(uiringselect) >= GetRingPRI(iCurRingType))
        {
            res = SetupHF_EndPlayMusic() ? STOPMUSIC_SUCCESS : STOPMUSIC_ERROR_UNKNOWN;
            if ((setuphs_playmusic.pBuffer != NULL) && (pBuffer_Play != NULL))
            {
                free(pBuffer_Play);
                pBuffer_Play = NULL;
                setuphs_playmusic.pBuffer = NULL;
            }
            //��ǰ���ڲ��ŵ�����������Ҫֹͣ������,������Ӧ������ֹͣ����,���ط�����Ϣ
            
            if (bCallResetRingType(uiringselect))
            {
                //printf("Setup_EndPlayMusic ResetCurrentRingType\r\n");
                ResetCurrentRingType();//���Ž�����ʱ��,�����ڲ��ŵ����������λ
            }
        }
        else
        {
            //printf("Setup_EndPlayMusic iCurRingType:%d,uiringselect:%d\r\n",iCurRingType,uiringselect);
            return (STOPMUSIC_ERROR_PRI);
        }
    }
    return (res);
}
/*
static const char * pClassName = "PlayMusicWndClass";
static HWND         hWndApp;
static char         cRingName[RINGNAMEMAXLEN + 1] = ""; //�����ļ���
static int          iCallCount;                         //���Ŵ���,0Ϊѭ������,����Ϊ���Ŵ���

#define ID_TIMER_VIBRATION      9	//�����𶯵ļ�ʱ��
#define ID_TIMER_MUSICPLAY      10	//�������ֲ��ŵļ�ʱ��
#define WM_MMWRITE              WM_USER + 100
#define WM_MMSTOP               WM_USER + 110
#define WM_MMGETDATA            WM_USER + 115
#define IDC_PLAYMUSIC           WM_USER + 125	//����һ���������ŵ���Ϣ
  */
/********************************************************************
* Function   Call_PlayMusic  
* Purpose    ���������ӿ�
* Params     uiringselect:�������
            icount:���Ŵ���,0Ϊѭ������,����Ϊ���Ŵ���
* Return     
* Remarks     
**********************************************************************/
BOOL    Call_PlayMusic(MUSICTYPE uiringselect,unsigned int icount)
{

    SCENEMODE   sm;//����龰ģʽ��Ϣ
    ABINFO      pbNameIcon;
    char        cPhoneNum[PHONENUMMAXLEN + 1] = "";

    if (uiringselect == RING_KEY)
    {
        if (!GetCanPlayMusic_Key())
            return FALSE;
    }

    if (!GetCanPlayMusic(uiringselect))//��ǰ�����Բ��Ÿü��������
        return FALSE;

    switch (SetCurrentRingType(uiringselect))
    {
    case 1://��ǰ���������ڲ���
    case 0://��ǰû�����ֲ���
        break;
    case -1://ע��ʧ��,��Ҫ���ŵ����ּ���̫��
        return FALSE;
    }

    GetSM(&sm,GetCurSceneMode());           //����龰ģʽ��Ϣ
    memset(cRingName,0,sizeof(cRingName));
//printf("\n Call_PlayMusic 123\n");
    switch (uiringselect)
    {

    case RING_PHONE://����
#if 1      
		GetCurrentPhoneNum(cPhoneNum,sizeof(cPhoneNum));
		switch (sm.iVibraSwitch)
		{
		case SWITCH_OFF://����
		   if (MB_GetInfoFromPhonebook(cPhoneNum,&pbNameIcon))
		   {
			   if (!FileIfExist(pbNameIcon.szRing))
				   strcpy(cRingName,sm.rIncomeCall.cMusicFileName);
			   else
			   {
				   strcpy(cRingName,pbNameIcon.szRing);
				   //strcpy(cRingName,"/mnt/flash/phone.wav");
			   }
		   }
		   else
		   {
			   strcpy(cRingName,sm.rIncomeCall.cMusicFileName);
		   }
		   //strcpy(cRingName,"/mnt/flash/phone.wav");
		   break;
		   
		case SWITCH_ON://������
		   SetupHf_Vibration(TRUE);
		   if (MB_GetInfoFromPhonebook(cPhoneNum,&pbNameIcon))
		   {
			   if (!FileIfExist(pbNameIcon.szRing))
				   strcpy(cRingName,sm.rIncomeCall.cMusicFileName);
			   else
				   strcpy(cRingName,pbNameIcon.szRing);
		   }
		   else
		   {
			   strcpy(cRingName,sm.rIncomeCall.cMusicFileName);
		   }
		   break;
		   
		default:
		   ResetCurrentRingType();//����������,���������λ
		   return FALSE;
		}
#endif
		   
           break;
   
    case RING_SMS://������ʾ��
        switch (sm.iVibraSwitch)
        {
        case SWITCH_OFF://����
            strcpy(cRingName,sm.rMessage.cMusicFileName);
            break;
		
        case SWITCH_ON://������
            strcpy(cRingName,sm.rMessage.cMusicFileName);
            //BeginVibration(1);//��һ��
			SetupHf_Vibration(TRUE);
            break;
		
        default:
            ResetCurrentRingType();//����������,���������λ
            return FALSE;
        }
        break;

    case RING_POWNON://��������
        GetOpenRing(cRingName);
        break;

    case RING_POWNOFF://�ػ�����
        GetCloseRing(cRingName);
        break;
		
    case RING_MMS://������ʾ��
        strcpy(cRingName,sm.rMessage.cMusicFileName);
        break;
    case RING_WARNING://������
        strcpy(cRingName,"/rom/warning.wav");
        break;
    case RING_CAMERA://������ʾ��
        strcpy(cRingName,"/rom/crack.wav");
        break;
    case RING_KEY://��������ʾ��
        strcpy(cRingName,"/rom/key.wav");
        break;
    case RING_TOUCH://��ͨ��ʾ��
        strcpy(cRingName,"/rom/touch.wav");
        break;
//    case RING_SCREEN://������
//        strcpy(cRingName,"/rom/key.wav");
//        break;
    case RING_CLEW50://50����ʾ��
        strcpy(cRingName,"/rom/clew50.wav");
        break;
    default:
        //printf("\n�������������,�޷�����,%ld\n",uiringselect);
        ResetCurrentRingType();//����ʧ��,���������λ
        return FALSE;
    }
    if (strlen(cRingName) == 0)//�ļ���Ϊ�յĻ�,ֱ���˳�
    {
        ResetCurrentRingType();
        return FALSE;
    }
    iCallCount = abs(icount);
    //��Ϊ��������Ϊ0Ϊѭ������,����Ϊ���Ŵ���,Ϊ���ڲ����Ŵ��������ķ���
    //���ڲ���ѭ�����ű�־Ϊ-1,������һ�ε�ʱ��iCallCount--,��iCallCount��Ϊ���ʱ��
    //ֹͣ����.
    if (iCallCount == 0)
        iCallCount = -1;
    else
        iCallCount--;//��ʾ��һ�β���

    //PostMessage(hWndApp,IDC_PLAYMUSIC,uiringselect,0);//�����������wParam����
	printf("now play music:\t%s\n", cRingName);
	//PlaySound(cRingName,0,icount<<28);
    return TRUE;
}
/********************************************************************
* Function   Call_EndPlayMusic  
* Purpose    ����ֹͣ�ӿں���,�û�������������ӿ�ֹͣ����
* Params     
* Return     
* Remarks     
**********************************************************************/
BOOL    Call_EndPlayMusic(MUSICTYPE uiringselect)
{
    EndPlayMusic_Result ires;

    ires = Setup_EndPlayMusic(uiringselect);
    ResetCurrentRingType();//���Ž�����ʱ��,�����ڲ��ŵ����������λ
    SetupHf_Vibration(FALSE);//ֹͣ��

    return (STOPMUSIC_SUCCESS == ires);
}

//////////////////////////����Ϊ�������Ų���end//////////////////////////////////////////
void    BeginVibration(unsigned int iSecond)//ʵ����iSecond��
{
	/*
    KillTimer(hWndApp,ID_TIMER_VIBRATION);
    if (iSecond != 0)
        SetTimer(hWndApp,ID_TIMER_VIBRATION,iSecond * 1000,NULL);
	*/
    SetupHf_Vibration((BOOL)iSecond);
}
void	InitImePara(HWND hWnd,IMEEDIT * imeedit)
{
	static	char ImeName[10] = "";
	strcpy(ImeName,GetString(STR_IMEASP_PYID));

	imeedit->hwndNotify		= hWnd;
	imeedit->dwAttrib		&= ~IME_ATTRIB_GENERAL;
	imeedit->uMsgSetText	= 0;
	imeedit->dwUniTextMax	= 0;
	imeedit->dwAscTextMax	= 0;
	imeedit->pszImeName		= ImeName;
    imeedit->pszTitle		= "";
}
