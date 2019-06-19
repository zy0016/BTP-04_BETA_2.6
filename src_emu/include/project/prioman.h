/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : prioman.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PRIOMAN_H_
#define _PRIOMAN_H_

// 铃声播放优先级
#define PRIOMAN_PRIORITY_PHONE		100		// 来电
#define PRIOMAN_PRIORITY_ALARM		95		// 闹钟
#define PRIOMAN_PRIORITY_CALENDAR	94		// 日程
#define PRIOMAN_PRIORITY_POWERON	90		// 开机
#define PRIOMAN_PRIORITY_POWEROFF	89		// 关机
#define PRIOMAN_PRIORITY_BEEPSOUND	88		// Beep Sound
#define PRIOMAN_PRIORITY_SMSAPP		85		// 短消息
#define PRIOMAN_PRIORITY_MMSAPP		84		// 彩信
#define PRIOMAN_PRIORITY_EMAIL		83		// E-Mail
#define PRIOMAN_PRIORITY_BLUETOOTH	82		// 蓝牙
#define PRIOMAN_PRIORITY_PUSHINFO	81		// PUSH信息
#define PRIOMAN_PRIORITY_SIMTOOLKIT	75		// STK
#define PRIOMAN_PRIORITY_WAPAPP		70		// 浏览器
#define PRIOMAN_PRIORITY_MP3APP		65		// MP3播放
#define PRIOMAN_PRIORITY_MUSICMAN	60		// 音乐管理器
#define PRIOMAN_PRIORITY_CAMERA		55		// 照相机
#define PRIOMAN_PRIORITY_KJAVA		50		// KJAVA
#define PRIOMAN_PRIORITY_PUBLIC		45		// 公共预览
#define PRIOMAN_PRIORITY_SETTING	40		// 个性设置
#define PRIOMAN_PRIORITY_CONNECT	35		// 接通提示音
#define PRIOMAN_PRIORITY_50TIPS		34		// 50秒提示音
#define PRIOMAN_PRIORITY_WARNING	33		// 警告音
#define PRIOMAN_PRIORITY_KEYBOARD	32		// 按键音
#define PRIOMAN_PRIORITY_TSCREEN	31		// 触屏音
#define PRIOMAN_PRIORITY_ZERO		0		// 零优先级

// 铃声类型
#define PRIOMAN_RINGTYPE_WAVE		1		// Wave
#define PRIOMAN_RINGTYPE_MIDI		2		// Midi
#define PRIOMAN_RINGTYPE_MMF		3		// MMF
#define PRIOMAN_RINGTYPE_AMR		4		// AMR
#define PRIOMAN_RINGTYPE_MP3		5		// MP3
#define PRIOMAN_RINGTYPE_UNKNOWN	6		// Unknown

// 音量级别
#define PRIOMAN_VOLUME_DEFAULT		3		// 3级
#define PRIOMAN_VOLUME_MUTE			0		// 0级
#define PRIOMAN_VOLUME_LOWLEVEL		1		// 1级
#define PRIOMAN_VOLUME_HIGHLEVEL	5		// 5级

#define PRIOMAN_VOLUME_LEVEL0		0		// 0级
#define PRIOMAN_VOLUME_LEVEL1		1		// 1级
#define PRIOMAN_VOLUME_LEVEL2		2		// 2级
#define PRIOMAN_VOLUME_LEVEL3		3		// 3级
#define PRIOMAN_VOLUME_LEVEL4		4		// 4级
#define PRIOMAN_VOLUME_LEVEL5		5		// 5级

// 铃声文件
#define PRIOMAN_RINGFILE_POWERON	"/rom/prioman/startup.amr"
#define PRIOMAN_RINGFILE_POWEROFF	"/rom/prioman/shutdown.amr"
#define PRIOMAN_RINGFILE_BEEPSOUND	"/rom/prioman/beep.wav"
#define PRIOMAN_RINGFILE_WARNING	"/rom/prioman/ding.wav"
#define PRIOMAN_RINGFILE_KEYBOARD	"/rom/prioman/key.wav"
#define PRIOMAN_RINGFILE_TSCREEN	"/rom/prioman/touch.wav"

// 消息类型
#define PRIOMAN_MESSAGE				WM_USER+2881
#define PRIOMAN_MESSAGE_READDATA	PRIOMAN_MESSAGE+1		// 读数据
#define PRIOMAN_MESSAGE_WRITEDATA	PRIOMAN_MESSAGE+2		// 写数据
#define PRIOMAN_MESSAGE_PLAYOVER	PRIOMAN_MESSAGE+3		// 播完
#define PRIOMAN_MESSAGE_BREAKOFF	PRIOMAN_MESSAGE+4		// 中断

// 定时器类型
#define PRIOMAN_TIMER				927
#define PRIOMAN_TIMER_CALLMUSIC		PRIOMAN_TIMER+1			// 铃声播放
#define PRIOMAN_TIMER_ENDCALL		PRIOMAN_TIMER+2			// 结束播放
#define PRIOMAN_TIMER_VIBRATION		PRIOMAN_TIMER+3			// 振动

// 错误信息
#define PRIOMAN_ERROR_SUCCESS		1		// 成功
#define PRIOMAN_ERROR_PARAMETER		2		// 输入参数错误
#define PRIOMAN_ERROR_LOWPRIO		3		// 铃声播放优先级太低
#define PRIOMAN_ERROR_UNKNOWNPRIO	4		// 未知的铃声播放优先级
#define PRIOMAN_ERROR_RINGTYPE		5		// 铃声类型错误
#define PRIOMAN_ERROR_OPENFAIL		6		// 打开文件错误
#define PRIOMAN_ERROR_ALLOCMEM		7		// 分配内存错误
#define PRIOMAN_ERROR_DECORDER		8		// 创建解码器错误
#define PRIOMAN_ERROR_GETPLAYTIME	9		// 获取播放时间错误
#define PRIOMAN_ERROR_GETAUDIOINFO	10		// 获取声音信息错误
#define PRIOMAN_ERROR_GETDECDATA	11		// 获取解码数据错误
#define PRIOMAN_ERROR_WAVEFORMAT	12		// WAVE格式错误
#define PRIOMAN_ERROR_WAVEOPEN		13		// WAVE打开错误
#define PRIOMAN_ERROR_WAVECLOSE		14		// WAVE关闭错误
#define PRIOMAN_ERROR_SETVOLUME		15		// 设置音量错误
#define PRIOMAN_ERROR_PREPARE		16		// 准备播放错误
#define PRIOMAN_ERROR_WRITEOUT		17		// 写设备错误
#define PRIOMAN_ERROR_RESET			18		// RESET设备错误
#define PRIOMAN_ERROR_CREATERAM		19		// 创建RAM文件错误
#define PRIOMAN_ERROR_READRAM		20		// 读取RAM文件错误
#define PRIOMAN_ERROR_WRITERAM		21		// 写入RAM文件错误
#define PRIOMAN_ERROR_BREAKOFF		22		// 打断错误
#define PRIOMAN_ERROR_FINDWINDOW	23		// FINDWINDOW错误
#define PRIOMAN_ERROR_ISPLAYING		24		// 当前有进程正在播放
#define PRIOMAN_ERROR_VOLUME		25		// 音量不合适
#define PRIOMAN_ERROR_MUTEMODE		26		// 静音模式
#define PRIOMAN_ERROR_UNKNOWN		0		// 未知的错误

#define PRIOMAN_PLAYMUSIC			1		// 接口1
#define PRIOMAN_CALLMUSIC			2		// 接口2

// 铃声播放结构
typedef struct tagPM_PlayMusic
{
	HWND hCallWnd;
	int nPriority;
	int nRingType;
	int nVolume;
	int nRepeat;
	char* pMusicName;
	unsigned char* pDataBuf;
	int nDataLen;
}PM_PlayMusic, *PPM_PlayMusic;

// 接口1文件列表
typedef struct tagPM_MusicList {
	char* pMusicName;
	int nPriority;
	int nRingType;
	int nVolume;
}PM_MusicList, *PPM_MusicList;

// 接口2文件列表
typedef struct tagPM_CallList {
	int nPriority;
	int nRepeat;
	int nRingType;
	int nVolume;
}PM_CallList, *PPM_CallList;

// RAM数据结构
typedef struct tagPM_RAMFILE {
	HWND hCallWnd;
	int nPriority;
}PM_RAMFILE, *PPM_RAMFILE;

////////////////////////////////////////////////////////////////////////////
// 对程序管理器提供的接口
////////////////////////////////////////////////////////////////////////////

/*
接口说明：
功能 --创建优先级事件及信号灯参数。
参数 无。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_Initialize(void);

/*
接口说明：
功能 --释放优先级事件及信号灯参数。
参数 无。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_Uninitialize(void);

////////////////////////////////////////////////////////////////////////////
// 对外提供的接口1
////////////////////////////////////////////////////////////////////////////

/*
接口说明：
功能 --播放声音准备。
参数 pPM：传入的声音信息结构。请在数据结构一节查看详细信息。
返回值：如果成功，返回 PRIOMAN_ERROR_SUCCESS。请在宏定义一节查看详细信息。
*/
int  PrioMan_PlayMusic(PPM_PlayMusic pPM);

/*
接口说明：
功能 --读取声音数据。
参数 wParam：未使用，保留。
	 lParam：数据块。
返回值：无。
*/
void PrioMan_ReadData(WPARAM wParam, LPARAM lParam);

/*
接口说明：
功能 --写入声音数据。
参数 wParam：未使用，保留。
	 lParam：数据块。
返回值：无。
*/
void PrioMan_WriteData(WPARAM wParam, LPARAM lParam);

/*
接口说明：
功能 --结束声音播放。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：如果成功，返回 PRIOMAN_ERROR_SUCCESS。请在宏定义一节查看详细信息。
*/
int  PrioMan_EndPlayMusic(int nPrio);

/*
接口说明：
功能 --打断声音播放。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：如果成功，返回 PRIOMAN_ERROR_SUCCESS。请在宏定义一节查看详细信息。
*/
int  PrioMan_BreakOffMusic(int nPrio);

/*
接口说明：
功能 --暂停声音播放。
参数 无。
返回值：成功返回0。
*/
int  PrioMan_PauseMusic(void);

/*
接口说明：
功能 --继续声音播放。
参数 无。
返回值：成功返回0。
*/
int  PrioMan_ResumeMusic(void);

////////////////////////////////////////////////////////////////////////////
// 对外提供的接口2
////////////////////////////////////////////////////////////////////////////

/*
接口说明：
功能 --直接声音播放。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
     nRep：循环次数，0为无限循环。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_CallMusic(int nPrio, int nRep);

/*
接口说明：
功能 --直接声音播放扩展。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
     nTimeOut：超时时间，毫秒计。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_CallMusicEx(int nPrio, int nTimeOut);

/*
接口说明：
功能 --直接声音播放。
参数 pMusicName：声音文件。
     nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
     nRep：循环次数，0为无限循环。
     nVol：音量大小，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_CallMusicFile(char* pMusicName, int nPrio, int nRep, int nVol);

/*
接口说明：
功能 --结束直接播放。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
     bStop：是否完全停止播放，当为FALSE时只结束本次循环。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_EndCallMusic(int nPrio, BOOL bStop);

/*
接口说明：
功能 --打断直接播放。
参数 nPrio：App优先级，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：如果成功，返回TRUE；如果失败，返回FALSE。
*/
BOOL PrioMan_BreakOffCall(int nPrio);

////////////////////////////////////////////////////////////////////////////
// 对MP3应用提供的接口
////////////////////////////////////////////////////////////////////////////

/*
接口说明：
功能 --获取MP3总播放时间。
参数 无。
返回值：成功返回>0秒数，失败返回-1。
*/
int  PrioMan_MP3GetTotalTime(void);

/*
接口说明：
功能 --获取MP3当前时间。
参数 pSeconds：秒数。
返回值：成功返回0，失败返回-1。
*/
int  PrioMan_MP3CurrentTime(int* pSeconds);

/*
接口说明：
功能 --定位MP3播放进度。
参数 nSeconds：秒数。
返回值： 成功返回0，失败返回-1。
*/
int  PrioMan_MP3Seek(int nSeconds);

/*
接口说明：
功能 --设置MP3音量。
参数 nVolume：音量大小，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：成功返回0。
*/
int  PrioMan_MP3SetVolume(int nVolume);

////////////////////////////////////////////////////////////////////////////
// 其他
////////////////////////////////////////////////////////////////////////////

/*
接口说明：
功能 --调节音量。
参数 nVolume：音量大小，由prioman统一定义。请在宏定义一节查看详细信息。
返回值：成功返回0。
*/
int  PrioMan_VolumeAdjusting(int nVolume);

/*
接口说明：
功能 --手机振动。
参数 nSec：秒数，当为0时结束振动。
返回值：无。
*/
void PrioMan_Vibration(int nSec);

#endif // _PRIOMAN_H_
