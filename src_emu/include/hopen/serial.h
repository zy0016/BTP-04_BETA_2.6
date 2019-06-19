
#ifndef  __SERIAL_H__
#define  __SERIAL_H__

#include <sys/ioctl.h> 

#if CPU_ARCH == DRAGONBALL

//serial command 
#define  UART_IOC_MAGIC		 'U'
#define  UART_IOC_SETBAUD	 _IOW(UART_IOC_MAGIC, 0, long)

#endif //DRAGONBALL

#endif //__SERIAL_H__
