

#ifndef _HOPENPM_IOCTL_H
#define _HOPENPM_IOCTL_H

#define PS_BATTERY_FAILURE		0x00000001	//battery fail
#define PS_AC_LINE				0x00000002	//AC  in
#define PS_BATTERY_CHARGING		0x00000004	//charge

int		battery_level;						//(0~100)

/*
 * Powermanager Device ioctl
 */

#define PM_IOC_GETPOWERSTATUS		_IOR('P', 0x01, sizeof(long))
#define PM_IOC_GETBATTERYPERCENT	_IOR('P', 0x02, sizeof(long))
#define PM_IOC_SUSPEND				_IOW('P', 0x03, sizeof(long))
#define PM_IOC_POWEROFF				_IOW('P', 0x04, sizeof(long))

#endif

