/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Physical display driver 
 *
 * Purpose  : Implement the interface for logical display driver
 *            
\**************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <hopen/devmajor.h>
#include <hopen/lcd.h>
#include <hopen/fb.h>
#include <osver.h>
#include <hp_diag.h>

#include "epsonlcd.h"

#define VRAM_SIZE	120*160 
#define MAX_ROWS	160
#define MAX_COLS	120

unsigned long vram_address; //shall be obtained using IOCTRL
unsigned long vram_io_base; //shall be obtained using IOCTRL

#define PTD_BASE_ADDR 	vram_io_base

#define PTD_DDIR                PTD_BASE_ADDR                
#define PTD_OCR1                (PTD_BASE_ADDR+0x04)                
#define PTD_OCR2                (PTD_BASE_ADDR+0x08)                
#define PTD_ICONFA1             (PTD_BASE_ADDR+0x0C)                
#define PTD_ICONFA2             (PTD_BASE_ADDR+0x10)                
#define PTD_ICONFB1             (PTD_BASE_ADDR+0x14)                
#define PTD_ICONFB2             (PTD_BASE_ADDR+0x18)                
#define PTD_DR                  (PTD_BASE_ADDR+0x1C)                
#define PTD_GIUS                (PTD_BASE_ADDR+0x20)                
#define PTD_SSR                 (PTD_BASE_ADDR+0x24)                
#define PTD_ICR1                (PTD_BASE_ADDR+0x28)                
#define PTD_ICR2                (PTD_BASE_ADDR+0x2C)                
#define PTD_IMR                 (PTD_BASE_ADDR+0x30)                
#define PTD_ISR                 (PTD_BASE_ADDR+0x34)                
#define PTD_GPR                 (PTD_BASE_ADDR+0x38)                
#define PTD_SWR                 (PTD_BASE_ADDR+0x3C)                
#define PTD_PUEN                (PTD_BASE_ADDR+0x40) 

static void write_command_to_lcd(unsigned long val)
{
	unsigned long data;
	
	data = (val << LCD_DATA_SHIFT);
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_DATA_MASK;
	
	*(volatile unsigned long *)PTD_DR |= data;
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_A0;
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_CS;
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_WR;
	
	*(volatile unsigned long *)PTD_DR |= LCD_WR;  //generate a rising edge;
	
	*(volatile unsigned long *)PTD_DR |= LCD_A0 | LCD_CS;
	
}

static void lcd_write_data(short* ptr,int n)
{
	unsigned long data;
	unsigned short pixel;
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_A0;	
	
	*(volatile unsigned long *)PTD_DR |= LCD_A0;
	
	*(volatile unsigned long *)PTD_DR &= ~LCD_CS;
	
	while(n--)
	{
		pixel = *ptr++;
		
		*(volatile unsigned long *)PTD_DR &= ~(LCD_WR);
		data = CHANGE_DATA(pixel);
		data <<= LCD_DATA_SHIFT;
		*(volatile unsigned long *)PTD_DR &= ~LCD_DATA_MASK;
		*(volatile unsigned long *)PTD_DR |= data;
		*(volatile unsigned long *)PTD_DR |= LCD_WR;  //generate a rising edge;
	}
	
	*(volatile unsigned long *)PTD_DR |= LCD_A0 | LCD_CS;

}

/*
void OS_UpdateScreen(int left, int top, int right, int bottom)
{
	return ;
}
*/
#include "display.h"

struct OS_DisplayMode GE_DispMode;
extern struct OS_DisplayMode * OS_GetDisplayMode(void);

/*
struct OS_DisplayMode * OS_GetDisplayMode(void)
{
    int fd;
    struct fb_var_screeninfo fvs;
    
    fd = opendev(LCD_MAJOR, 0, O_RDWR);
    if (fd < 0) 
    {
        TRACE("LCD open failed!\r\n");
        TRACEERROR();
        return NULL;
    }
    
    ioctl(fd, LCD_IOC_GETSCREENINFO, &fvs);
    ioctl(fd, LCD_IOC_GETADDR, &vram_address);
	ioctl(fd, LCD_IOC_GETIOBASE, &vram_io_base);

    close(fd);

    TRACELINE();
    GE_DispMode.dsp_mode = 0;
    GE_DispMode.dsp_width = fvs.xres;
    GE_DispMode.dsp_height = fvs.yres;
    GE_DispMode.dsp_planes = 1;
    GE_DispMode.dsp_scanline_bytes = (GE_DispMode.dsp_width * 2);//fvs.bits_per_pixel) / 8;
    GE_DispMode.dsp_screen_buffer = (void*)fvs.reserved[0];// 0x2F0000;

    TRACE("vram_address = 0x%8X \tvram_io_base=%08X\r\n", vram_address, vram_io_base);
    return &GE_DispMode;
}
*/
