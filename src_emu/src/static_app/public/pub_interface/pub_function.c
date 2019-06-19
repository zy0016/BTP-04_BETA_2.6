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
extern  BOOL    CallAutoCloseClewWindow(void);  //自动关机界面
extern  int     GetBright(void);                //获得背景灯亮度[1,6]
//extern  void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen);//获得当前来电的电话号码
//////////////////////////本地函数////////////////////////////////////////////////
//static  BOOL    CreateWindow_PlayMusic(void);//创建一个隐藏窗口用于播放声音,该函数是系统启动的时候创建
static  long    GetRingPRI(MUSICTYPE iRingType);
static  void    ResetCurrentRingType(void);//播放结束的时候,将正在播放的铃声的类别复位
static  LRESULT AppWndProc_PlayMusic ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  MUSICTYPE   GetCurrentRingType(void);//获得当前正在播放的铃声的类别
/********************************************************************
* Function   FindSIMState  
* Purpose     获得SIM卡的状态.
* Params     bShow:是否显示当前状态提示
* Return     当前状态编号
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
            AppMessageBox(NULL,ML("No SIM card"),ML("Clew"),WAITTIMEOUT);//未插入SIM卡
    }
    return (istate);
}
/********************************************************************
* Function   AddString  
* Purpose    将指针数组**p中的文字添加到控件中
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
* Purpose    加载**P中的位图
* Params     ** p位图文件名的指针数组
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
* Purpose    将相应的文字添加到列表框中
* Params     bAppend:图标是否是个数组,TRUE的话,hIconNormal指针需要移动,否则hIconNormal指针不动
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
* Purpose    提供给程序管理器,用于初始化设置各个参数
* Params     
* Return     
* Remarks    
**********************************************************************/
/*
BOOL    Setup_InitFunction(void)
{
    SCENEMODE sm;

//    SetCurrentLanguage(GetLanguage());  //设置当前语言
    CallAutoCloseClewWindow();          //创建自动关机界面
    SetUp_SetDialCfg();                 //创建拨号设置所需数据

    SetupHf_LcdBklight(GetBglight());   //开关LCD的背光灯 GetBglight()
    SetupHf_BgLight(GetBright());       //设置亮度部分 GetBright()

  //  CreateWindow_PlayMusic();           //初始化声音播放//xlzhu

    GetSM(&sm,GetCurSceneMode());       //获得情景模式信息
    if ((sm.iComeview == LIBRATE) || (sm.iComeview == RINGLIBRATE))//振铃模式(1:振动;2:振动铃声)
        BeginVibration(1);//震动1秒

    return (TRUE);
}
*/
//////////////////////////以下为声音播放部分begin//////////////////////////////////////////
static const char * pClassName = "PlayMusicWndClass";
static HWND         hWndApp;
static char         cRingName[RINGNAMEMAXLEN + 1] = ""; //铃声文件名
static int          iCallCount;                         //播放次数,0为循环播放,非零为播放次数

#define ID_TIMER_VIBRATION      9	//用于震动的计时器
#define ID_TIMER_MUSICPLAY      10	//用于音乐播放的计时器
#define WM_MMWRITE              WM_USER + 100
#define WM_MMSTOP               WM_USER + 110
#define WM_MMGETDATA            WM_USER + 115
#define IDC_PLAYMUSIC           WM_USER + 125	//用于一般声音播放的消息



static  BOOL                bCanPlayMusic_Key = TRUE;//是否可以播放按键音
static  BOOL                bCanPlayMusic     = TRUE;//当前是否可以播放声音
static  long                dCanPriority;//受限音乐级别
static  MUSICTYPE           CurrentRingType   = RING_WITHOUT;//当前正在播放什么类别的音乐
static  SetupHS_PlayMusic   setuphs_playmusic;
static  unsigned    char *  pBuffer_Play = NULL;//数据缓冲区位置
typedef struct 
{
    MUSICTYPE   iRingType;          //铃声类别
    long        lRingPRI;           //该类别铃声的优先级
    BOOL        bCallResetRingType; //停止播放音乐的时候,是否需要调用ResetCurrentRingType函数
}RINGPRI;
static const RINGPRI ringpri[] =//铃声类别优先级列表
{
    {RING_PHONE   ,WAVEOUT_PRIO_HIGH       ,TRUE}, //来电提示音
    {RING_ALARM   ,WAVEOUT_PRIO_BELOWHIGH  ,TRUE}, //闹钟
    {RING_CALENDAR,WAVEOUT_PRIO_BELOWHIGH  ,TRUE}, //日程
    {RING_POWNON  ,WAVEOUT_PRIO_LOW        ,FALSE},//开机音乐
    {RING_POWNOFF ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//关机音乐                                              
    {RING_SMS     ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//短信                                                  
    {RING_MMS     ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//彩信    
    {RING_WARNING ,WAVEOUT_PRIO_ABOVENORMAL,FALSE},//警告音
    {RING_CAMERA  ,WAVEOUT_PRIO_NORMAL     ,FALSE},//照相提示音
    {RING_KEY     ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//按键音                                                
    {RING_TOUCH   ,WAVEOUT_PRIO_NORMAL     ,FALSE},//接通提示音                                             
    {RING_SCREEN  ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//触屏音                                                 
    {RING_CLEW50  ,WAVEOUT_PRIO_BELOWNORMAL,FALSE},//50秒提示音                                             
    {RING_OTHER   ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //播放其它声音
    {RING_WAP     ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //浏览器中的声音文件或者数据
    {RING_MMSAPP  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //彩信应用中的文件或者数据
    {RING_PUBLIC  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //铃声预览中的文件
    {RING_VOLUME  ,WAVEOUT_PRIO_NORMAL     ,TRUE}, //测试音量用宏                                            
    {RING_WITHOUT ,0                       ,0}     //没有声音播放                                             
};//xlzhu
/********************************************************************
* Function   SetCanPlayMusic_Key  
* Purpose    设置当前是否可以播放按键音
* Params     bCanPlay:TRUE:可以播放声音,FALSE:不许播放声音
* Return     返回设置之前的状态
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
* Purpose    获得当前是否可以播放按键音
* Params     
* Return     TRUE:可以播放声音,FALSE:不许播放声音
* Remarks    
**********************************************************************/
static  BOOL    GetCanPlayMusic_Key(void)
{
    return bCanPlayMusic_Key;
}
/********************************************************************
* Function   SetCanPlayMusic  
* Purpose    设置当前是否可以播放声音
* Params     bCanPlay:TRUE:可以播放声音,FALSE:不许播放声音
            dPriority:bCanPlay为FALSE的时候,禁止播放的声音的优先级,比dPriority
            级别低的音乐将无法播放(包括级别等于dPriority的音乐),比dPriority
            级别高的音乐将不受限制
            bCanPlay为TRUE的时候,dPriority无意义
* Return     返回设置之前的状态
* Remarks    
**********************************************************************/
BOOL    SetCanPlayMusic(BOOL bCanPlay,DWORD dPriority)
{
    BOOL bPlayBak = bCanPlayMusic;
    MUSICTYPE iCurRingType;

    bCanPlayMusic = bCanPlay;
    if (bCanPlay)//当前可以播放声音
    {
        dCanPriority = 0;
    }
    else//当前禁止某些级别的音乐播放
    {
        dCanPriority = dPriority;
        iCurRingType = GetCurrentRingType();//获得当前正在播放的铃声的类别

        //printf("SetCanPlayMusic iCurRingType:%d\r\n",iCurRingType);

        if (iCurRingType != RING_WITHOUT)//当前有声音播放
        {
            if (dCanPriority >= GetRingPRI(iCurRingType))
            {
                //printf("SetCanPlayMusic GetRingPRI(iCurRingType):%d\r\n",GetRingPRI(iCurRingType));

                Setup_EndPlayMusic(iCurRingType);
                ResetCurrentRingType();//播放结束的时候,将正在播放的铃声的类别复位
            }
        }
    }
    return bPlayBak;
}
/********************************************************************
* Function   GetCanPlayMusic
* Purpose    获得当前是否可以播放声音
* Params     
* Return     TRUE:可以播放声音,FALSE:不许播放声音
* Remarks    
**********************************************************************/
static  BOOL    GetCanPlayMusic(MUSICTYPE uiringselect)
{
    if (bCanPlayMusic)//当前可以播放声音
        return bCanPlayMusic;
    //当前不许播放某个类别的声音
    return (GetRingPRI(uiringselect) > dCanPriority);//需要播放的声音的优先级大于当前禁止的优先级
}
/********************************************************************
* Function   GetRingPRI  
* Purpose    获得该类别铃声的优先级
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
* Purpose    获得该类别铃声在停止播放音乐的时候,是否需要调用ResetCurrentRingType函数
* Params     iRingType:铃声类别
* Return     是否需要调用ResetCurrentRingType函数
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
static  MUSICTYPE   GetCurrentRingType(void)//获得当前正在播放的铃声的类别
{
    return (CurrentRingType);
}
/********************************************************************
* Function   SetCurrentRingType  
* Purpose    注册当前正要播放的铃声的类别,只有注册成功才能播放
* Params     uiringselect:铃声的类别RING_SELECT,setting.h文件中定义
* Return     没有音乐播放返回0,注册成功返回1,失败返回-1
* Remarks    正要播放的铃声的优先级只有>当前铃声的优先级,注册才能成功
**********************************************************************/
static  int     SetCurrentRingType(MUSICTYPE uiringselect)
{
    MUSICTYPE iCurRingType;
    switch (uiringselect)
    {
    case RING_PHONE :   //来电提示音       
    case RING_ALARM:    //闹钟
    case RING_CALENDAR: //日程
    case RING_POWNON:   //开机铃声                                               
    case RING_POWNOFF:  //关机音乐                                               
    case RING_SMS:      //短信                                                  
    case RING_MMS:      //彩信      
    case RING_WARNING:  //警告音
    case RING_CAMERA:   //照相提示音
    case RING_KEY:      //按键音                                                
    case RING_TOUCH:    //接通提示音                                              
    case RING_SCREEN:   //触屏音                                                 
    case RING_CLEW50:   //50秒提示音                                                
    case RING_OTHER:    //播放其它声音
    case RING_WAP:      //浏览器中的声音文件或者数据
    case RING_MMSAPP:   //彩信应用中的文件或者数据
    case RING_PUBLIC:   //铃声预览中的文件
    case RING_VOLUME:   //测试音量用宏
        break;
    default://非法的铃声类型
        return -1;
    }
    iCurRingType = GetCurrentRingType();//获得当前音乐类别
    //printf("\n当前音乐类别:%d,需要播放的音乐类别:%d\n",iCurRingType,uiringselect);
    if (iCurRingType == RING_WITHOUT)//当前没有声音播放
    {
        CurrentRingType = uiringselect;//记录铃声类别
        return 0;
    }
    //当前有音乐正在播放,需要判断新来的音乐的优先级是否大于当前音乐的优先级
    if (GetRingPRI(uiringselect) > GetRingPRI(iCurRingType))
    {   //新来的音乐的优先级大于当前音乐的优先级
        Setup_EndPlayMusic(uiringselect);
        CurrentRingType = uiringselect;//记录铃声类别
#ifndef _EMULATE_
        usleep(400000L);
#endif
        return 1;
    }
    else//新来的音乐的优先级小于当前音乐的优先级
    {
        //printf("\n注册失败,当前正在播放的音乐类别:%d,需要播放的音乐类别:%d\n",iCurRingType,uiringselect);
        return -1;
    }
}
static  void    ResetCurrentRingType(void)//播放结束的时候,将正在播放的铃声的类别复位
{
    CurrentRingType = RING_WITHOUT;
    //printf("\n铃声类别复位\n");
}
/********************************************************************
* Function   GetMusicFile_DataType  
* Purpose    考查文件或者数据区是否是合法的声音播放类型
* Params     pFileName:需要检查的文件类型,pBufferData:需要检查的数据区
                ulBufferSize:数据区的长度
* Return     FILETYPE:文件类型或者声音类型
* Remarks    
**********************************************************************/
static  FILETYPE  GetMusicFile_DataType(const char * pFileName,
                                          const unsigned char * pBufferData,
                                          unsigned long ulBufferSize)
{
    int hf;
    unsigned char strAudioType[AUDIOTYPE] = "";
    unsigned long lDataSize;//保存文件长度或者数据区的长度
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
	
		if (lDataSize <= AUDIOTYPE)//文件长度太短
            return INVALID_TYPE;

        if (sizeof(strAudioType) != read(hf,strAudioType,sizeof(strAudioType)))//读取文件头
            return OPENFAIL;
    }
    else if ((pBufferData != NULL) && (ulBufferSize != 0))
    {
        lDataSize = ulBufferSize;   //保存数据区长度
        if (lDataSize <= AUDIOTYPE) //数据区长度太短
            return INVALID_TYPE;

        p = pBufferData;
        for (i = 0;i < AUDIOTYPE;i++)
            strAudioType[i] = * p++;
    }
    else
    {
        return INVALID_TYPE;
    }
    //开始检测文件头
    if ((strAudioType[0] == 0x23 ) && (strAudioType[1] == 0x21) && 
        (strAudioType[2] == cCh_A) && (strAudioType[3] == cCh_M) &&
        (strAudioType[4] == cCh_R) && (strAudioType[5] == 0x0a))
    {
        FileType = AMR_TYPE;
    }
    else if ((strAudioType[0] == cCh_R) && (strAudioType[1] == cCh_I) && 
             (strAudioType[2] == cCh_F) && (strAudioType[3] == cCh_F))
    {
        if (lDataSize <= sizeof(WAVEFORMATEX) + 0x16)//对于weve文件要靠查文件头是否合法
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
* Purpose    停止播放的回调函数
* Params     PlayEndType停止播放的的类型
* Return     
* Remarks    
**********************************************************************/
static  void	MusicCallBack(BYTE PlayEndType)
{
	switch (PlayEndType)
    {
    case 0://播完一遍
    case 1://全部播完
        PostMessage(setuphs_playmusic.hWnd,setuphs_playmusic.iWM_MMSTOP,PLAY_OVER,0);
        return;
    case 2://被其他播放中断
        PostMessage(setuphs_playmusic.hWnd,setuphs_playmusic.iWM_MMSTOP,PLAY_BREAK,0);
        return;
    }
}
/******************************************************************
* Function   SETUP_PlayMusic  
* Purpose    播放音乐文件
* Params     
             hWnd:      调用方的窗口句柄
             iWM_MMWRITE:调用方定义的消息,调用方必须在该消息中调用Setup_WriteMusicData函数
            推荐这样调用:Setup_WriteMusicData((LPWAVEHDR)lParam);
            iWM_MMSTOP: 调用方定义的消息,用于通知调用方铃声播放完毕,调用方必须在该消息中调用Setup_EndPlayMusic()函数
            当铃声播放完毕的时候,iWM_MMSTOP消息中的wParam=PLAY_OVER表示音乐播放完毕,而且是正常结束
            pBuffer:数据区域,
            ulBufferSize:数据区域的长度
* Return        见setting.h
* Remarks      
**********************************************************************/
PlayMusic_Result    SETUP_PlayMusic(SetupHS_PlayMusic * hs)
{
    SCENEMODE sm;//获得情景模式信息

    if (!GetCanPlayMusic(hs->uiringselect))//当前不可以播放该级别的声音
    {
        return (PLAYMUSIC_ERROR_PRI);//优先级不够
    }
    //如果当前音乐类别等于setuphs_playmusic.uiringselect则表示该音乐在Call_PlayMusic
    //函数中已经注册过了,不必再次注册,直接播放
    if (GetCurrentRingType() != hs->uiringselect)
    {
        switch (SetCurrentRingType(hs->uiringselect))
        {
        case 1://当前有音乐正在播放
        case 0://当前没有音乐播放
            break;
        case -1://注册失败,需要播放的音乐级别太低
            return (PLAYMUSIC_ERROR_PRI);//优先级不够
        }
    }

    GetSM(&sm,GetCurSceneMode());
    switch (hs->uiringselect)
    {
	case RING_TOUCH:
    case RING_PHONE:    //来电提示音
		hs->iVolume = sm.rIncomeCall.iRingVolume;
		strcpy(hs->pFileName , sm.rIncomeCall.cMusicFileName);
		break;
		
    case RING_ALARM:    //闹钟
    	hs->iVolume = sm.rAlarmClk.iRingVolume;
		strcpy(hs->pFileName, sm.rAlarmClk.cMusicFileName);
		break;

    case RING_CALENDAR: //日程
    	hs->iVolume = sm.rCalendar.iRingVolume;
		strcpy(hs->pFileName, sm.rCalendar.cMusicFileName);
		break;

	case RING_SMS:      //短信
    case RING_MMS:      //彩信
    	hs->iVolume = sm.rMessage.iRingVolume;
		strcpy(hs->pFileName,sm.rMessage.cMusicFileName);
		break;
     
    case RING_WARNING:  //警告音
    	hs->iVolume = sm.iNotification;
		break;
	case RING_KEY:      //按键音
		hs->iVolume = sm.iKeyVolume;
		break;

	case RING_POWNON:   //开机铃声
    case RING_POWNOFF:  //关机音乐
    case RING_CAMERA:   //照相提示音

        //接通提示音
    case RING_SCREEN:   //触屏音
    case RING_CLEW50:   //50秒提示音
    case RING_OTHER:    //播放其它声音
    case RING_WAP:      //浏览器中的声音文件或者数据
    case RING_MMSAPP:   //彩信应用中的文件或者数据
    case RING_PUBLIC:   //铃声预览中的文件
        //hs->iVolume = sm.iRingvolume;//设置铃声音量
		//hs->iVolume = sm.iMasterVol;
        break;
    case RING_VOLUME://测试音量用宏,音量由参数iVolume决定
        break;
    default:
        //printf("\nWrong sound type,unable to play,%ld\n",hs->uiringselect);
        ResetCurrentRingType();//播放失败,铃声的类别复位
        return (PLAYMUSIC_ERROR_RINGTYPE);//非法的铃声类别
    }

    pBuffer_Play = NULL;
    if ((hs->pBuffer != NULL) && (hs->ulBufferSize != 0))
    {   //根据数据缓冲区进行声音播放
        hs->iType = GetMusicFile_DataType(NULL,hs->pBuffer,hs->ulBufferSize);
        switch (hs->iType)
        {
        case INVALID_TYPE://不支持播放的声音文件类别
            ResetCurrentRingType();//播放失败,铃声的类别复位
            return PLAYMUSIC_ERROR_WAVEFORMAT;//非法的声音格式
        case OPENFAIL://文件打开失败
            ResetCurrentRingType();//播放失败,铃声的类别复位
            return PLAYMUSIC_ERROR_OPENFILE;//文件打开失败
        }

        if ((pBuffer_Play = 
            (unsigned char *)malloc(setuphs_playmusic.ulBufferSize + 1)) == NULL)
        {
            printf("\nmalloc failure,%ld\n",setuphs_playmusic.ulBufferSize);
            ResetCurrentRingType();//播放失败,铃声的类别复位
            return (PLAYMUSIC_ERROR_MALLOC);//malloc失败
        }
        memcpy(&setuphs_playmusic,hs,sizeof(SetupHS_PlayMusic));

        memset(pBuffer_Play,0x00,setuphs_playmusic.ulBufferSize + 1);
        memcpy(pBuffer_Play,hs->pBuffer,hs->ulBufferSize);
        //将数据拷贝到本地
        setuphs_playmusic.pBuffer   = pBuffer_Play;//指向本地数据区
        setuphs_playmusic.pFileName = NULL;

        if (SetupHf_PlayMusicFromData((BYTE)setuphs_playmusic.iType,1,pBuffer_Play,
            setuphs_playmusic.ulBufferSize,MusicCallBack))
            return PLAYMUSIC_SUCCESS;
        else
            return PLAYMUSIC_ERROR_PLAYFUNCTION;
    }
    else if (hs->pFileName != NULL)//按文件进行播放
    {
        hs->iType = GetMusicFile_DataType(hs->pFileName,NULL,0);
        switch (hs->iType)
        {
        case INVALID_TYPE://不支持播放的声音文件类别
            ResetCurrentRingType();//播放失败,铃声的类别复位
            return PLAYMUSIC_ERROR_WAVEFORMAT;//非法的声音格式
        case OPENFAIL://文件打开失败
            ResetCurrentRingType();//播放失败,铃声的类别复位
            return PLAYMUSIC_ERROR_OPENFILE;//文件打开失败
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
        printf("\n错误的声音参数,无法播放\n");
        ResetCurrentRingType();//播放失败,铃声的类别复位
        return (PLAYMUSIC_ERROR_PARAMETER);//非法的SetupHS_PlayMusic参数
    }
}
/********************************************************************
* Function   Setup_EndPlayMusic  
* Purpose    停止播放音乐
* Params     
* Return     
* Remarks      
**********************************************************************/
EndPlayMusic_Result Setup_EndPlayMusic(MUSICTYPE uiringselect)
{
    EndPlayMusic_Result res = STOPMUSIC_SUCCESS;
    MUSICTYPE iCurRingType;
	//stopSound(cRingName,0);
    iCurRingType = GetCurrentRingType();//获得当前正在播放的铃声的类别

    if (iCurRingType == RING_WITHOUT)//当前没有声音播放
    {
        return (STOPMUSIC_SUCCESS);//停止成功
    }
    else
    {
        //当前有音乐正在播放,需要判断停止音乐的应用的优先级是否大于当前音乐的优先级
        if (GetRingPRI(uiringselect) >= GetRingPRI(iCurRingType))
        {
            res = SetupHF_EndPlayMusic() ? STOPMUSIC_SUCCESS : STOPMUSIC_ERROR_UNKNOWN;
            if ((setuphs_playmusic.pBuffer != NULL) && (pBuffer_Play != NULL))
            {
                free(pBuffer_Play);
                pBuffer_Play = NULL;
                setuphs_playmusic.pBuffer = NULL;
            }
            //当前正在播放的声音就是需要停止的声音,则这是应用主动停止播放,不必发送消息
            
            if (bCallResetRingType(uiringselect))
            {
                //printf("Setup_EndPlayMusic ResetCurrentRingType\r\n");
                ResetCurrentRingType();//播放结束的时候,将正在播放的铃声的类别复位
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
static char         cRingName[RINGNAMEMAXLEN + 1] = ""; //铃声文件名
static int          iCallCount;                         //播放次数,0为循环播放,非零为播放次数

#define ID_TIMER_VIBRATION      9	//用于震动的计时器
#define ID_TIMER_MUSICPLAY      10	//用于音乐播放的计时器
#define WM_MMWRITE              WM_USER + 100
#define WM_MMSTOP               WM_USER + 110
#define WM_MMGETDATA            WM_USER + 115
#define IDC_PLAYMUSIC           WM_USER + 125	//用于一般声音播放的消息
  */
/********************************************************************
* Function   Call_PlayMusic  
* Purpose    播放声音接口
* Params     uiringselect:播放类别
            icount:播放次数,0为循环播放,非零为播放次数
* Return     
* Remarks     
**********************************************************************/
BOOL    Call_PlayMusic(MUSICTYPE uiringselect,unsigned int icount)
{

    SCENEMODE   sm;//获得情景模式信息
    ABINFO      pbNameIcon;
    char        cPhoneNum[PHONENUMMAXLEN + 1] = "";

    if (uiringselect == RING_KEY)
    {
        if (!GetCanPlayMusic_Key())
            return FALSE;
    }

    if (!GetCanPlayMusic(uiringselect))//当前不可以播放该级别的声音
        return FALSE;

    switch (SetCurrentRingType(uiringselect))
    {
    case 1://当前有音乐正在播放
    case 0://当前没有音乐播放
        break;
    case -1://注册失败,需要播放的音乐级别太低
        return FALSE;
    }

    GetSM(&sm,GetCurSceneMode());           //获得情景模式信息
    memset(cRingName,0,sizeof(cRingName));
//printf("\n Call_PlayMusic 123\n");
    switch (uiringselect)
    {

    case RING_PHONE://来电
#if 1      
		GetCurrentPhoneNum(cPhoneNum,sizeof(cPhoneNum));
		switch (sm.iVibraSwitch)
		{
		case SWITCH_OFF://铃声
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
		   
		case SWITCH_ON://振动铃声
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
		   ResetCurrentRingType();//不播放铃声,铃声的类别复位
		   return FALSE;
		}
#endif
		   
           break;
   
    case RING_SMS://短信提示音
        switch (sm.iVibraSwitch)
        {
        case SWITCH_OFF://铃声
            strcpy(cRingName,sm.rMessage.cMusicFileName);
            break;
		
        case SWITCH_ON://振动铃声
            strcpy(cRingName,sm.rMessage.cMusicFileName);
            //BeginVibration(1);//振动一秒
			SetupHf_Vibration(TRUE);
            break;
		
        default:
            ResetCurrentRingType();//不播放铃声,铃声的类别复位
            return FALSE;
        }
        break;

    case RING_POWNON://开机铃声
        GetOpenRing(cRingName);
        break;

    case RING_POWNOFF://关机音乐
        GetCloseRing(cRingName);
        break;
		
    case RING_MMS://彩信提示音
        strcpy(cRingName,sm.rMessage.cMusicFileName);
        break;
    case RING_WARNING://警告音
        strcpy(cRingName,"/rom/warning.wav");
        break;
    case RING_CAMERA://照相提示音
        strcpy(cRingName,"/rom/crack.wav");
        break;
    case RING_KEY://按键音提示音
        strcpy(cRingName,"/rom/key.wav");
        break;
    case RING_TOUCH://接通提示音
        strcpy(cRingName,"/rom/touch.wav");
        break;
//    case RING_SCREEN://触屏音
//        strcpy(cRingName,"/rom/key.wav");
//        break;
    case RING_CLEW50://50秒提示音
        strcpy(cRingName,"/rom/clew50.wav");
        break;
    default:
        //printf("\n错误的声音类型,无法播放,%ld\n",uiringselect);
        ResetCurrentRingType();//播放失败,铃声的类别复位
        return FALSE;
    }
    if (strlen(cRingName) == 0)//文件名为空的话,直接退出
    {
        ResetCurrentRingType();
        return FALSE;
    }
    iCallCount = abs(icount);
    //因为参数意义为0为循环播放,非零为播放次数,为了内部播放次数计数的方便
    //在内部将循环播放标志为-1,当播放一次的时候iCallCount--,当iCallCount减为零的时候
    //停止播放.
    if (iCallCount == 0)
        iCallCount = -1;
    else
        iCallCount--;//表示第一次播放

    //PostMessage(hWndApp,IDC_PLAYMUSIC,uiringselect,0);//将声音类别用wParam传入
	printf("now play music:\t%s\n", cRingName);
	//PlaySound(cRingName,0,icount<<28);
    return TRUE;
}
/********************************************************************
* Function   Call_EndPlayMusic  
* Purpose    声音停止接口函数,用户主动调用这个接口停止音乐
* Params     
* Return     
* Remarks     
**********************************************************************/
BOOL    Call_EndPlayMusic(MUSICTYPE uiringselect)
{
    EndPlayMusic_Result ires;

    ires = Setup_EndPlayMusic(uiringselect);
    ResetCurrentRingType();//播放结束的时候,将正在播放的铃声的类别复位
    SetupHf_Vibration(FALSE);//停止震动

    return (STOPMUSIC_SUCCESS == ires);
}

//////////////////////////以上为声音播放部分end//////////////////////////////////////////
void    BeginVibration(unsigned int iSecond)//实现振动iSecond秒
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
