
#ifndef  __FLASH_H__
#define  __FLASH_H__

#include <sys/ioctl.h>

#if  PLATFORM == DRAGONBALL
//flash command
#define  FLASH_IOC_MAGIC	'F'
#define  FLASH_IOC_ERASE	 _IOW(FLASH_IOC_MAGIC, 0, long)
#define  FLASH_IOC_ERASEALL      _IO(FLASH_IOC_MAGIC, 1)

#endif //DRAGONBALL

#endif //__FLASH_H__
