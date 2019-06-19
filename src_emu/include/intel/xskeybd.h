#ifndef __xskeybdH
#define __xskeybdH

#include "../sys/ioctl.h" 


#define RGB_OFF			0
#define RGB_RED			1
#define RGB_GREEN		2
#define RGB_BLUE		3

#define VIB_ON			1
#define VIB_OFF			0


/********************************************
 * IOCTL commands for /dev/cmr
 */

#define LED_RGB_CTL		_IOW('S', 0, long)
#define MOTOR_VIB_CTL	_IOW('S', 1, long)

#define GET_TICKCOUNT	_IOR('S', 2, long)


#define INIT_USB_HOST   0x1111
#define DISABLE_USB_HOST	0x2222
//#define ENABLE_WATCHDOG		0x3333
//#define WATCHDOG_TIMEOUT_VALUE	0x4444

void usb_mount_add_key(void);
void  add_sd_insert_key();
void  add_sd_eject_key();
void add_pen_virtul_key(unsigned long value);
void add_wakeup_virtul_key(void);
void usb_insert_int_hndlr(int irq, void *dev_id, struct pt_regs *regs);



#endif

