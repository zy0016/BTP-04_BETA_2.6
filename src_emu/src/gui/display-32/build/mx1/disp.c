/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : display
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

static unsigned long vram_io_base; //shall be obtained using IOCTRL
//#define SCREENWIDTH 240
//#define SCREENHEIGHT 320
//#define VRAM_SIZE	240*320 

#define SCREENWIDTH 176
#define SCREENHEIGHT 220
#define VRAM_SIZE	176*220      //by axli 


#define DNA_H	(vram_io_base + 0x00400000)
#define DNA_L	(vram_io_base)

#define EVT_BOARD
#if defined (EVT_BOARD)		
	#define LCD_BUS_WIDTH	volatile unsigned long
	#define LCD_BYTE_SHIFT	16
#else
	#define LCD_BUS_WIDTH	volatile unsigned short
	#define LCD_BYTE_SHIFT	0
#endif

static void inline writevram_16bits(unsigned short color)
{
	*(volatile unsigned short *)DNA_H = 0x27;
	*(volatile unsigned short *)DNA_L = color;
}

static void writevram_18bits(unsigned long color)
{
	int i;
	unsigned short dummy = 0;

	*(volatile unsigned short *)DNA_H = 0x27;
	*(volatile unsigned short *)DNA_H = dummy;
	for(i=0;i<8;i++)
		asm("nop");
	
	*(volatile unsigned short *)DNA_L = (color&0xFFFF);
	*(volatile unsigned short *)DNA_L = ((color>>16)&0x3);
	for(i=0;i<8;i++)
		asm("nop");
}

/*
void inline lcd_write_data16(unsigned short* ptr, int n)
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
*/
void inline lcd_write_data16(unsigned short* ptr, int n)
{
	int num;
	num = n / 8;
	
	*(LCD_BUS_WIDTH *)DNA_H = 0x27 << LCD_BYTE_SHIFT;
	while(num--)
	{
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
        *(LCD_BUS_WIDTH *)DNA_L = (*ptr++) << LCD_BYTE_SHIFT;
	}
    
	*(LCD_BUS_WIDTH *)DNA_H = 0x22 << LCD_BYTE_SHIFT;
	*(LCD_BUS_WIDTH *)DNA_L = *(LCD_BUS_WIDTH *)DNA_L;
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

#include <osver.h>
#include <assert.h>
#include <disppdd.h>

void inline lcd_writerc_data16(unsigned short* ptr, 
                        int x, int y, int width, int height)
{
	int num;
    int linebytes;
    unsigned short* p1;

    ASSERT (width > 0 && height > 0 && x + width < SCREENWIDTH 
        && y + height < SCREENHEIGHT);
    width = (width + 7) / 8 * 8;
    if ((x + width) > SCREENWIDTH)
        x = SCREENWIDTH - width;

    ptr += (y * SCREENWIDTH + x) * 2;
    linebytes = SCREENWIDTH * 2;

    num = width / 8;
	*(volatile unsigned short *)DNA_H = 0x27;
    while(height--)
    {
        p1 = ptr;
        while(num--)
        {
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
            *(volatile unsigned short *)DNA_L = (*p1++);
        }

        ptr += linebytes;
    }

	*(volatile unsigned short *)DNA_H = 0x22;
	*(volatile unsigned short *)DNA_L = *(volatile unsigned short *)DNA_L;
}


#undef TRACE
#define TRACE printf
/*********************************************************************\
* Function	   PDD_EnumDisplayMode
* Purpose      Enum the display modes of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
        pDisplayMode: (out)the display mode returned
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	  
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_EnumDisplayMode(int dev, int index, POS_DISPLAYMODE pDisplayMode)
{
    return TRUE;
}

/*********************************************************************\
* Function	   PDD_SetDisplayMode
* Purpose      Set the display mode of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_SetDisplayMode(int dev, int index)
{
    return TRUE;
}

/*********************************************************************\
* Function	   PDD_GetDisplayMode
* Purpose      Get the current mode of the given device.
* Params	   
        dev:    (in)the display device index
        pDisplayMode: (out)the display mode returned
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
#define DMA 1
#if (DMA)
static int fd_lcd;
BOOL PDD_GetDisplayMode(int dev, POS_DISPLAYMODE pDisplayMode)
{
    struct fb_var_screeninfo fvs;
    struct lcd_info lcdinfo;
    unsigned int framebuffer_addr;

	printf("PDD_GetDisplaymode\r\n");
    ASSERT(pDisplayMode != NULL);
    if (pDisplayMode == NULL)
        return FALSE;

	fd_lcd = open("/dev/lcd", O_RDWR);
    if (fd_lcd < 0) 
    {
        TRACE("LCD open failed!\r\n");
        TRACE("Error: errno=%d occured!!\r\n", errno);
        return FALSE;
    }
    
    ioctl(fd_lcd, LCD_IOC_GETSCREENINFO, &fvs);
    ioctl(fd_lcd, LCD_IOC_GETINFO, (char*)&lcdinfo);
    
    ioctl(fd_lcd,LCD_IOC_GETADDR,&framebuffer_addr);
	printf("=======framebuffer_addr=%x\r\n",framebuffer_addr);

    TRACE("LCD open successed! \r\n");
	TRACE("***** vramaddr = 0x%08lX \r\n", lcdinfo.addr);
	TRACE("***** iobaseaddr = 0x%08lX\r\n", vram_io_base);
	TRACE("***** xres = %ld,  yres = %ld\r\n", fvs.xres, fvs.yres);
	TRACE("***** bits_per_pixel = %ld\r\n", fvs.bits_per_pixel);
	TRACE("***** vramaddr = 0x%08lX\r\n", fvs.reserved[0]);

    pDisplayMode->dsp_mode = 0;
    pDisplayMode->dsp_width = 176; //fvs.xres;
    pDisplayMode->dsp_height = 220; //fvs.yres;
    pDisplayMode->dsp_planes = 1;
    pDisplayMode->dsp_scanline_bytes =176*4; // (fvs.xres * fvs.bits_per_pixel) / 8;
	if(dev!=-1)
	{
		//printf("dev != -1\r\n");
		//printf("=======framebuffer_addr=%x\r\n",framebuffer_addr);
		pDisplayMode->dsp_screen_buffer =framebuffer_addr;// 0xf4100100;//(void*)lcdinfo.addr;// 0x2F0000;
	}
	else
	{
		//printf("dev == -1\r\n");
		pDisplayMode->dsp_screen_buffer =framebuffer_addr+176*220*4; 
		//printf("=======framebuffer_addr+176*220*4=%x\r\n",framebuffer_addr+176*220*4);
	}

    pDisplayMode->dsp_updatescreen = LCD_NOLCDC;

    return TRUE;
}
#else
BOOL PDD_GetDisplayMode(int dev, POS_DISPLAYMODE pDisplayMode)
{
    int fd;
    struct fb_var_screeninfo fvs;
    struct lcd_info lcdinfo;

    ASSERT(pDisplayMode != NULL);
    if (pDisplayMode == NULL)
        return FALSE;

	fd = opendev(LCD_MAJOR, 0, 0);
    if (fd < 0) 
    {
        TRACE("LCD open failed!\r\n");
        TRACE("Error: errno=%d occured!!\r\n", errno);
        return FALSE;
    }
    
    ioctl(fd, LCD_IOC_GETSCREENINFO, &fvs);
    ioctl(fd_lcd, LCD_IOC_GETINFO, (char*)&lcdinfo);
    ioctl(fd, LCD_IOC_GETADDR, &vram_address);
	ioctl(fd, LCD_IOC_GETIOBASE, &vram_io_base);

    TRACE("LCD open successed! \r\n");
	TRACE("***** vramaddr = 0x%08XL \r\n", lcdinfo.addr);
	TRACE("***** iobaseaddr = 0x%08X\r\n", vram_io_base);
	TRACE("***** xres = %d,  yres = %d\r\n", fvs.xres, fvs.yres);
	TRACE("***** bits_per_pixel = %d\r\n", fvs.bits_per_pixel);
	TRACE("***** vramaddr = 0x%08X\r\n", fvs.reserved[0]);

    close(fd);

    pDisplayMode->dsp_mode = 0;
    pDisplayMode->dsp_width = fvs.xres;
    pDisplayMode->dsp_height = fvs.yres;
    pDisplayMode->dsp_planes = 1;
    pDisplayMode->dsp_scanline_bytes = (fvs.xres * fvs.bits_per_pixel) / 8;
    pDisplayMode->dsp_screen_buffer = (void*)lcdinfo.addr;// 0x2F0000;
    pDisplayMode->dsp_updatescreen = LCD_NOLCDC;

/*
    pDisplayMode->dsp_mode = 0;
    pDisplayMode->dsp_width = fvs.xres_virtual;
    pDisplayMode->dsp_height = fvs.yres_virtual;
    pDisplayMode->dsp_planes = 1;
    pDisplayMode->dsp_scanline_bytes = (fvs.xres * fvs.bits_per_pixel) / 8;
    pDisplayMode->dsp_screen_buffer = (void*)vram_address 
    + (GE_DispMode.dsp_scanline_bytes * fvs.yoffset 
       + (fvs.bits_per_pixel / 8) * fvs.xoffset) ;//(void*)fvs.reserved[0];// 0x2F0000;
    pDisplayMode->dsp_updatescreen = 0;
*/

    return TRUE;
}
#endif
/*********************************************************************\
* Function	   PDD_UpdateScreen
* Purpose      Update the given screen.
* Params	   
        dev:    (in)the display device index
        left,top,right,bottom: (in)the update rect
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
#if (DMA)
void PDD_UpdateScreen(int dev, int left, int top, int right, int bottom)
{
    int nr;

    nr = 0;
    /* 参数为显存序号，0 至 info.count - 1 */
    ioctl(fd_lcd, LCD_IOC_UPDATE, &nr);
    return;
}
#else
void PDD_UpdateScreen(int dev, int left, int top, int right, int bottom)
{
	if(color_mode == 16)
	{
//        lcd_writerc_data16((short*)vram_address, left, top, right - left, bottom - top);
        printf("##############PDD_UpdateScreen!!!\r\n");
		lcd_write_data16((short*)vram_address, VRAM_SIZE);
	}
	else 
	{
		if(color_mode == 18)
			lcd_write_data18((long*)vram_address, VRAM_SIZE);
	}
	
	return;
}
#endif

void OS_UpdateScreen(int left, int top, int right, int bottom)
{
    PDD_UpdateScreen(0, left, top, right, bottom);
	return;
}
