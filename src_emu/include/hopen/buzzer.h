
#ifndef  __BUZZER_H__
#define  __BUZZER_H__

#include <sys/ioctl.h>

#if  CPU_ARCH == DRAGONBALL
//buzzer command
#define  BUZZER_IOC_MAGIC	'B'
#define  BUZZER_IOC_SOUND	_IO(BUZZER_IOC_MAGIC, 0)
#define  BUZZER_IOC_DUMMY	_IO(BUZZER_IOC_MAGIC, 1)

#endif //DRAGONBALL

#endif //__BUZZER_H__
