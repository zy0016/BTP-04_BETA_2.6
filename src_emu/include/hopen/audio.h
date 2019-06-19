#ifndef  __AUDIO_H__
#define  __AUDIO_H__

#include <sys/ioctl.h>

#if  CPU_ARCH == DRAGONBALL
//audio command 
#define  SOUND_IOC_MAGIC	'S'
//#define  SOUND_IOC_PLAY	_IO(SOUND_IOC_MAGIC, 0)
//#define  SOUND_IOC_STOP	_IO(SOUND_IOC_MAGIC, 1)
#define  SOUND_IOC_CHANNEL	_IO(SOUND_IOC_MAGIC, 2)
#define  SOUND_IOC_SAMPLE	_IO(SOUND_IOC_MAGIC, 3)
#define  SOUND_IOC_NONBLOCK _IO(SOUND_IOC_MAGIC, 4)
#define  SOUND_IOC_BLOCK 	_IO(SOUND_IOC_MAGIC, 5)
#define  SOUND_IOC_RESET 	_IO(SOUND_IOC_MAGIC, 6)

#endif //DRAGONBALL

#endif //__AUDIO_H__
