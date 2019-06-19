
#ifndef  __TOUCHPEN_H__
#define  __TOUCHPEN_H__

#include <sys/ioctl.h> 

#define  TOUCHPEN_IOC_MAGIC		 'T'

/*
	arg = pen_calibrate *
*/
#define  TOUCHPEN_IOC_CALIBRATE	 _IOW(TOUCHPEN_IOC_MAGIC, 0, long)

/*
	arg = 0, driver return real sample data
	arg = 1, driver return logic data
*/
#define  TOUCHPEN_IOC_SETMODE	 _IOW(TOUCHPEN_IOC_MAGIC, 1, long)

#ifndef __GNUC__
#define __attribute__(x)
#endif

//big endian
typedef struct touch_pen
{
	unsigned char  m_stat;
	unsigned char  m_unused;
	unsigned short m_xpos;
	unsigned short m_ypos;
} __attribute__ ((packed)) touch_pen;

typedef struct coordinate
{
	unsigned short xpos;
	unsigned short ypos;
} coordinate;

typedef struct pen_calibrate
{
	int point_num;
	coordinate *phy;
	coordinate *log;
} pen_calibrate;

#define TS_EV_MOVE	0
#define TS_EV_UP	1
#define TS_EV_DOWN	2

#endif //__TOUCHPEN_H__

