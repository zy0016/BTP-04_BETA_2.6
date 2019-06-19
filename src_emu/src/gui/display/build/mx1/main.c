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
#include <hopen/devmajor.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/poll.h"

#include <errno.h>
#include <string.h>

#include <hopen/lcd.h>
#include <hopen/fb.h>

#include <fapi.h>

#include <string.h>

#include <ioctl.h>
#include <Sysctl.h>

unsigned long vram_address, xres, yres;

unsigned long vram_address; //shall be obtained using IOCTRL
unsigned long vram_io_base; //shall be obtained using IOCTRL
#define VRAM_SIZE	240*320 

int color_mode = 16;

#define DNA_H	(vram_io_base + 0x00400000)
#define DNA_L	(vram_io_base)


static void writevram_16bits(unsigned short color)
{
	int i;
	unsigned short dummy;

	*(volatile unsigned short *)DNA_H = 0x27;
	*(volatile unsigned short *)DNA_H = 0x27;
	for(i=0;i<8;i++)
		asm("nop");
	
	*(volatile unsigned short *)DNA_L = color;
	for(i=0;i<8;i++)
		asm("nop");
}

static void writevram_18bits(unsigned long color)
{
	int i;
	unsigned short dummy;

	*(volatile unsigned short *)DNA_H = 0x27;
	*(volatile unsigned short *)DNA_H = dummy;
	for(i=0;i<8;i++)
		asm("nop");
	
	*(volatile unsigned short *)DNA_L = (color&0xFFFF);
	*(volatile unsigned short *)DNA_L = ((color>>16)&0x3);
	for(i=0;i<8;i++)
		asm("nop");
}

void lcd_write_data16(unsigned short* ptr, int n)
{
	unsigned short pixel;
	int num;
	num = n;
	
	while(n--)
	{
		pixel = (*ptr++);
		writevram_16bits(pixel);
	}
}

void lcd_write_data18(unsigned long* ptr, int n)
{
	unsigned long pixel;
	int num;
	num = n;
	
	while(n--)
	{
		pixel = (*ptr++);
		writevram_18bits(pixel);
	}
}

int OS_UpdateScreen()
{
	if(color_mode == 16)
	{
		lcd_write_data16((short*)vram_address, VRAM_SIZE);
	}
	else 
	{
		if(color_mode == 18)
			lcd_write_data18((long*)vram_address, VRAM_SIZE);
	}
	
	return 0;
}

int main (int argc, char* argv[])
{
	int fd, ret, fd1, fd3;
	unsigned long * add;
	char buf[2000];
	int addr;
	char *ptr=buf;
	int i = 0;

	struct pollfd fds[3]=
	{
        {0, 0, POLLIN, 0},
        {0, 0, POLLIN, 0},
		{0, 0, POLLIN, 0}
	};

	fd = opendev(LCD_MAJOR, 0, 0);

	ret = ioctl(fd, LCD_IOC_GETADDR, &addr);

	printf("vramaddr=0x%08x\r\n",addr);
	
	ret = ioctl(fd, LCD_IOC_GETIOBASE, &vram_io_base);

	printf("iobaseaddr=0x%08x\r\n",vram_io_base);

	ret = ioctl(fd, LCD_IOC_GETSCREENINFO, buf);

	printf("xres=%d,yres=%d,bits_per_pixel=%d,vramaddr=0x%x\r\n",(*(struct fb_var_screeninfo *)buf).xres,
		(*(struct fb_var_screeninfo *)buf).yres, (*(struct fb_var_screeninfo *)buf).bits_per_pixel, 
		(*(struct fb_var_screeninfo *)buf).reserved[0]);

	vram_address = (*(struct fb_var_screeninfo *)buf).reserved[0];
	xres = (*(struct fb_var_screeninfo *)buf).xres;
	yres = (*(struct fb_var_screeninfo *)buf).yres;

	OS_UpdateScreen();

	close(fd);
	return 0;
}
