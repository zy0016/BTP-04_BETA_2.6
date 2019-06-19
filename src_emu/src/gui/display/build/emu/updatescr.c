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


int OS_UpdateScreen()
{
    TRACE("vram_address = 0x%8X\r\n", vram_address);
	write_command_to_lcd(CHANGE_DATA(RAMWR));
	lcd_write_data((short*)vram_address, VRAM_SIZE);
	
	return 0;
}
