
#ifndef  __LCD_H__
#define  __LCD_H__

#include <sys/ioctl.h> 


//lcd ioctl cmd
#define  LCD_IOC_MAGIC        		'L'
#define  LCD_IOC_ON       			_IO(LCD_IOC_MAGIC, 0)
#define  LCD_IOC_OFF          		_IO(LCD_IOC_MAGIC, 1)
#define  LCD_IOC_ONBKLIGHT			_IO(LCD_IOC_MAGIC, 2)
#define  LCD_IOC_OFFBKLIGHT			_IO(LCD_IOC_MAGIC, 3)	
#define  CUR_IOC_ENBLNK	  			_IO(LCD_IOC_MAGIC, 4)
#define  CUR_IOC_DISBLNK			_IO(LCD_IOC_MAGIC, 5)
#define  CUR_IOC_WIDTH				_IOW(LCD_IOC_MAGIC, 6, char)
#define  CUR_IOC_HIGHT				_IOW(LCD_IOC_MAGIC, 7, char)
#define  CUR_IOC_COLOR				_IOW(LCD_IOC_MAGIC, 8, long)
#define  CUR_IOC_XPOS				_IOW(LCD_IOC_MAGIC, 9, short)
#define  CUR_IOC_YPOS				_IOW(LCD_IOC_MAGIC, 10, short)

#define  LCD_IOC_GETADDR			_IO(LCD_IOC_MAGIC, 11)
#define  LCD_IOC_GETIOBASE			_IO(LCD_IOC_MAGIC, 12)
#define  LCD_IOC_GETSCREENINFO		_IO(LCD_IOC_MAGIC, 13)
#define  LCD_IOC_COLOR				_IOW(LCD_IOC_MAGIC, 14, long)
#define  LCD_IOC_SETREC				_IOW(LCD_IOC_MAGIC, 15, long)

typedef struct drawrec
{
	char xstart;
	char xend;
	short ystart;
	short yend;
} drawrec;


#endif //__LCD_H__
