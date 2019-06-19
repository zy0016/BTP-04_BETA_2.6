#ifndef __DISPMODE_H
#define __DISPMODE_H

#define MAX_SCREEN 3

#define	DDM_MONO		0x00000001
#define DDM_GRAY4       0x00000002
#define DDM_GRAY16      0x00000004
#define DDM_COLOR16     0x00000104
#define DDM_PAL256      0x00010008

#define DDM_RGB16_565   0x00020010
#define DDM_RGB16_555   0x0002010F
#define	DDM_RGB12_4440	0x0002020C

#define	DDM_RGB24_RGB   0x00030016
#define	DDM_RGB24_BGR   0x00030116
#define	DDM_RGB18_RGB   0x00030212
#define	DDM_RGB18_BGR   0x00030312
#define	DDM_YUV24		0x00030416

#define	DDM_RGB32_0RGB  0x00040020
#define	DDM_RGB32_RGB0  0x00040120
#define	DDM_RGB32_0BGR  0x00040220
#define	DDM_RGB32_BGR0  0x00040320
#define	DDM_RGB18_0RGB  0x00040412
#define	DDM_RGB18_0BGR  0x00040512
#define	DDM_RGB18_RGB0  0x00040612
#define	DDM_RGB18_BGR0  0x00040712
#define	DDM_YUV32		0x00040820

#define	DDM_VGA			DDM_COLOR16
#define	DDM_VGA256		DDM_PAL256
#define	DDM_VGA32K		DDM_RGB16_555
#define	DDM_VGA64K		DDM_RGB16_565

#define	DDM_VERTICAL    0x01000000
#define DDM_HORIZONTAL  0x02000000
#define	DDM_OTHER	    0xFFFFFFFF  /* the screen is not supported */

#define GETPIXBYTES(mode)   (((mode) & 0x000F0000) >> 16)
#define GETPIXBITS(mode)   ((((mode) & 0x000F0000) >> 16) * 8)
#define GETPIXCOLORBITS(mode)   (((mode) & 0xFF))

// Define constant for attrib field
#define DDA_PALETTE         0x00000001  /* 有调色板     */
#define DDA_DIRECT_DRAW     0x00000002  /* 支持直接写屏 */
#define DDA_ENERGY_SAVE     0x00000004  /* 具有节电功能 */
#define DDA_ANIMATE_CURSOR  0x00000008  /* 支持动画光标 */
#define DDA_HARDWARE_CURSOR 0x00000010  /* 支持硬件光标 */
#define DDA_ROP3_SUPPORT    0x00000020  /* 支持ROP3操作 */
#define DDA_REGION_CLIP     0x00000040  /* 支持复杂裁剪 */
#define DDA_VESA            0x00040000  /* 该方式是一种 VESA方式 */
#define DDA_TV              0x00080000  /* 该方式是一种 TV 方式 */
#define DDA_LCD             0x00100000  /* 该方式是一种 LCD 方式  */
#define DDA_VGAOUT          0x00200000  /* 该方式可以用 VGA 输出 */
#define DDA_AVOUT           0x00400000  /* 该方式可以用 AV  输出 */
#define DDA_SVIDEO          0x00800000  /* 该方式可以用 S 端子输出 */
#define DDA_RGB             0x01000000  /* 该方式可以用RGB分量输出 */

typedef struct LDD_ModeInfo
{
    DWORD dwModeFlag;   /* the mode flag defined upon */
    BYTE pix_bits;      /* the pixel bits of this mode */
    BYTE bRotate;       /* rotate screen flag */
    BYTE bReserved1;    /* reserved */
    BYTE bReserved2;    /* reserved */
}LDD_MODEINFO, *PLDD_MODEINFO;

/*********************************************************************\
* Function	   LDD_GetDisplayDriverInfo
* Purpose      Get the logical display driver info.
* Params	   
        nDev:   (in)the number of screens
        pDisplayDrvInfo: (out)the display driver info should be returned.
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	   
**********************************************************************/
#ifdef _MSC_VER
_declspec(dllexport) 
#endif
BOOL LDD_GetDisplayDriverInfo(int nDev, PLDD_MODEINFO pDisplayModeInfo);

typedef struct OS_DisplayMode
{
    int    dsp_mode;            //Display mode.
    int    dsp_width;           //srceen width in pix.
    int    dsp_height;          //srceen height in pix.
    int    dsp_planes;          //number of planes.
    int    dsp_scanline_bytes;  //bytes per scan line.
    void*  dsp_screen_buffer;   //Address of emulate screen buffer.
    int    dsp_updatescreen;    //forced update screen flag.
}OS_DISPLAYMODE, *POS_DISPLAYMODE;

#define SCREEN_MAIN     0
#define SCREEN_FIRST    SCREEN_MAIN
#define SCREEN_SECOND   1
#define SCREEN_THIRD    2

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
BOOL PDD_EnumDisplayMode(int dev, int index, POS_DISPLAYMODE pDisplayMode);

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
BOOL PDD_SetDisplayMode(int dev, int index);

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
BOOL PDD_GetDisplayMode(int dev, POS_DISPLAYMODE pDisplayMode);

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
void PDD_UpdateScreen(int dev, int left, int top, int right, int bottom);

#endif //__DISPMODE_H


