/* Definitions of frame buffers						*/
#include <hopen/fs.h>

//?#define FB_MAJOR		29
#define FB_MAX			1 //? 32	/* sufficient for now */

/* ioctls
   0x46 is 'F'								*/
#define FBIOGET_VSCREENINFO	0x4600
#define FBIOPUT_VSCREENINFO	0x4601
#define FBIOGET_FSCREENINFO	0x4602
#define FBIOGETCMAP		0x4604
#define FBIOPUTCMAP		0x4605
#define FBIOPAN_DISPLAY		0x4606
/* 0x4607-0x460B are defined below */
/* #define FBIOGET_MONITORSPEC	0x460C */
/* #define FBIOPUT_MONITORSPEC	0x460D */
/* #define FBIOSWITCH_MONIBIT	0x460E */
#define FBIOGET_CON2FBMAP	0x460F
#define FBIOPUT_CON2FBMAP	0x4610
#define FBIOBLANK		0x4611		/* arg: 0 or vesa level + 1 */
#define FBIOGET_VBLANK		_IOR('F', 0x12, struct fb_vblank)
#define FBIO_ALLOC              0x4613
#define FBIO_FREE               0x4614
#define FBIOGET_GLYPH           0x4615
#define FBIOGET_HWCINFO         0x4616
#define FBIOPUT_MODEINFO        0x4617
#define FBIOGET_DISPINFO        0x4618

#define FB_TYPE_PACKED_PIXELS		0	/* Packed Pixels	*/
#define FB_TYPE_PLANES			1	/* Non interleaved planes */
#define FB_TYPE_INTERLEAVED_PLANES	2	/* Interleaved planes	*/
#define FB_TYPE_TEXT			3	/* Text/attributes	*/
#define FB_TYPE_VGA_PLANES		4	/* EGA/VGA planes	*/

#define FB_AUX_TEXT_MDA		0	/* Monochrome text */
#define FB_AUX_TEXT_CGA		1	/* CGA/EGA/VGA Color text */
#define FB_AUX_TEXT_S3_MMIO	2	/* S3 MMIO fasttext */
#define FB_AUX_TEXT_MGA_STEP16	3	/* MGA Millenium I: text, attr, 14 reserved bytes */
#define FB_AUX_TEXT_MGA_STEP8	4	/* other MGAs:      text, attr,  6 reserved bytes */

#define FB_AUX_VGA_PLANES_VGA4		0	/* 16 color planes (EGA/VGA) */
#define FB_AUX_VGA_PLANES_CFB4		1	/* CFB4 in planes (VGA) */
#define FB_AUX_VGA_PLANES_CFB8		2	/* CFB8 in planes (VGA) */

#define FB_VISUAL_MONO01		0	/* Monochr. 1=Black 0=White */
#define FB_VISUAL_MONO10		1	/* Monochr. 1=White 0=Black */
#define FB_VISUAL_TRUECOLOR		2	/* True color	*/
#define FB_VISUAL_PSEUDOCOLOR		3	/* Pseudo color (like atari) */
#define FB_VISUAL_DIRECTCOLOR		4	/* Direct color */
#define FB_VISUAL_STATIC_PSEUDOCOLOR	5	/* Pseudo color readonly */

#define FB_ACCEL_NONE		0	/* no hardware accelerator	*/
#define FB_ACCEL_ATARIBLITT	1	/* Atari Blitter		*/
#define FB_ACCEL_AMIGABLITT	2	/* Amiga Blitter                */
#define FB_ACCEL_S3_TRIO64	3	/* Cybervision64 (S3 Trio64)    */
#define FB_ACCEL_NCR_77C32BLT	4	/* RetinaZ3 (NCR 77C32BLT)      */
#define FB_ACCEL_S3_VIRGE	5	/* Cybervision64/3D (S3 ViRGE)	*/
#define FB_ACCEL_ATI_MACH64GX	6	/* ATI Mach 64GX family		*/
#define FB_ACCEL_DEC_TGA	7	/* DEC 21030 TGA		*/
#define FB_ACCEL_ATI_MACH64CT	8	/* ATI Mach 64CT family		*/
#define FB_ACCEL_ATI_MACH64VT	9	/* ATI Mach 64CT family VT class */
#define FB_ACCEL_ATI_MACH64GT	10	/* ATI Mach 64CT family GT class */
#define FB_ACCEL_SUN_CREATOR	11	/* Sun Creator/Creator3D	*/
#define FB_ACCEL_SUN_CGSIX	12	/* Sun cg6			*/
#define FB_ACCEL_SUN_LEO	13	/* Sun leo/zx			*/
#define FB_ACCEL_IMS_TWINTURBO	14	/* IMS Twin Turbo		*/
#define FB_ACCEL_3DLABS_PERMEDIA2 15	/* 3Dlabs Permedia 2		*/
#define FB_ACCEL_MATROX_MGA2064W 16	/* Matrox MGA2064W (Millenium)	*/
#define FB_ACCEL_MATROX_MGA1064SG 17	/* Matrox MGA1064SG (Mystique)	*/
#define FB_ACCEL_MATROX_MGA2164W 18	/* Matrox MGA2164W (Millenium II) */
#define FB_ACCEL_MATROX_MGA2164W_AGP 19	/* Matrox MGA2164W (Millenium II) */
#define FB_ACCEL_MATROX_MGAG100	20	/* Matrox G100 (Productiva G100) */
#define FB_ACCEL_MATROX_MGAG200	21	/* Matrox G200 (Myst, Mill, ...) */
#define FB_ACCEL_SUN_CG14	22	/* Sun cgfourteen		 */
#define FB_ACCEL_SUN_BWTWO	23	/* Sun bwtwo			*/
#define FB_ACCEL_SUN_CGTHREE	24	/* Sun cgthree			*/
#define FB_ACCEL_SUN_TCX	25	/* Sun tcx			*/
#define FB_ACCEL_MATROX_MGAG400	26	/* Matrox G400			*/
#define FB_ACCEL_NV3		27	/* nVidia RIVA 128              */
#define FB_ACCEL_NV4		28	/* nVidia RIVA TNT		*/
#define FB_ACCEL_NV5		29	/* nVidia RIVA TNT2		*/
#define FB_ACCEL_CT_6555x	30	/* C&T 6555x			*/
#define FB_ACCEL_3DFX_BANSHEE	31	/* 3Dfx Banshee			*/
#define FB_ACCEL_ATI_RAGE128	32	/* ATI Rage128 family		*/
#define FB_ACCEL_IGS_CYBER2000	33	/* CyberPro 2000		*/
#define FB_ACCEL_IGS_CYBER2010	34	/* CyberPro 2010		*/
#define FB_ACCEL_IGS_CYBER5000	35	/* CyberPro 5000		*/
#define FB_ACCEL_SIS_GLAMOUR    36	/* SiS 300/630/540              */

struct fb_fix_screeninfo {
	char id[16];			/* identification string eg "TT Builtin" */
	unsigned long smem_start;	/* Start of frame buffer mem */
					/* (physical address) */
	unsigned long smem_len;			/* Length of frame buffer mem */
	unsigned long type;			/* see FB_TYPE_*		*/
	unsigned long type_aux;			/* Interleave for interleaved Planes */
	unsigned long visual;			/* see FB_VISUAL_*		*/ 
	unsigned short xpanstep;			/* zero if no hardware panning  */
	unsigned short ypanstep;			/* zero if no hardware panning  */
	unsigned short ywrapstep;		/* zero if no hardware ywrap    */
	unsigned long line_length;		/* length of a line in bytes    */
	unsigned long mmio_start;	/* Start of Memory Mapped I/O   */
					/* (physical address) */
	unsigned long mmio_len;			/* Length of Memory Mapped I/O  */
	unsigned long accel;			/* Type of acceleration available */
	unsigned short reserved[3];		/* Reserved for future compatibility */
};

struct fb_var_screeninfo {
	unsigned long xres;			/* visible resolution		*/
	unsigned long yres;
	unsigned long xres_virtual;		/* virtual resolution		*/
	unsigned long yres_virtual;
	unsigned long xoffset;			/* offset from virtual to visible */
	unsigned long yoffset;			/* resolution			*/

	unsigned long bits_per_pixel;		/* guess what			*/
	unsigned long grayscale;		/* != 0 Graylevels instead of colors */

//?	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
//?	struct fb_bitfield green;	/* else only length is significant */
//?	struct fb_bitfield blue;
//?	struct fb_bitfield transp;	/* transparency			*/	

	unsigned long nonstd;			/* != 0 Non standard pixel format */

	unsigned long activate;			/* see FB_ACTIVATE_*		*/

	unsigned long height;			/* height of picture in mm    */
	unsigned long width;			/* width of picture in mm     */

	unsigned long accel_flags;		/* acceleration flags (hints)	*/

	/* Timing: All values in pixclocks, except pixclock (of course) */
	unsigned long pixclock;			/* pixel clock in ps (pico seconds) */
	unsigned long left_margin;		/* time from sync to picture	*/
	unsigned long right_margin;		/* time from picture to sync	*/
	unsigned long upper_margin;		/* time from sync to picture	*/
	unsigned long lower_margin;
	unsigned long hsync_len;		/* length of horizontal sync	*/
	unsigned long vsync_len;		/* length of vertical sync	*/
	unsigned long sync;			/* see FB_SYNC_*		*/
	unsigned long vmode;			/* see FB_VMODE_*		*/
	unsigned long reserved[6];		/* Reserved for future compatibility */
};

struct fb_cmap {
	unsigned long start;			/* First entry	*/
	unsigned long len;			/* Number of entries */
	unsigned short *red;			/* Red values	*/
	unsigned short *green;
	unsigned short *blue;
	unsigned short *transp;			/* transparency, can be NULL */
};

struct fb_monspecs {
	unsigned long hfmin;			/* hfreq lower limit (Hz) */
	unsigned long hfmax; 			/* hfreq upper limit (Hz) */
	unsigned short vfmin;			/* vfreq lower limit (Hz) */
	unsigned short vfmax;			/* vfreq upper limit (Hz) */
	unsigned dpms : 1;		/* supports DPMS */
};


struct fb_info;

    /*
     *  Frame buffer operations
     */

struct fb_ops {
    /* open/release and usage marking */
//?    struct module *owner;
    int (*fb_open)(struct fb_info *info, int user);
    int (*fb_release)(struct fb_info *info, int user);
    /* get non settable parameters */
    int (*fb_get_fix)(struct fb_fix_screeninfo *fix, int con,
		      struct fb_info *info); 
    /* get settable parameters */
    int (*fb_get_var)(struct fb_var_screeninfo *var, int con,
		      struct fb_info *info);		
    /* set settable parameters */
    int (*fb_set_var)(struct fb_var_screeninfo *var, int con,
		      struct fb_info *info);		
    /* get colormap */
    int (*fb_get_cmap)(struct fb_cmap *cmap, int kspc, int con,
		       struct fb_info *info);
    /* set colormap */
    int (*fb_set_cmap)(struct fb_cmap *cmap, int kspc, int con,
		       struct fb_info *info);
    /* pan display (optional) */
    int (*fb_pan_display)(struct fb_var_screeninfo *var, int con,
			  struct fb_info *info);
    /* perform fb specific ioctl (optional) */
    int (*fb_ioctl)(struct inode *inode, struct file *file, unsigned int cmd,
		    unsigned long arg, int con, struct fb_info *info);
    /* perform fb specific mmap */
    int (*fb_mmap)(struct fb_info *info, struct file *file, struct vm_area_struct *vma);
    /* switch to/from raster image mode */
    int (*fb_rasterimg)(struct fb_info *info, int start);
};


struct fb_info {
   char modename[40];			/* default video mode */
   kdev_t node;
   int flags;
   int open;                            /* Has this been open already ? */
#define FBINFO_FLAG_MODULE	1	/* Low-level driver is a module */
   struct fb_var_screeninfo var;        /* Current var */
   struct fb_fix_screeninfo fix;        /* Current fix */
   struct fb_monspecs monspecs;         /* Current Monitor specs */
   struct fb_cmap cmap;                 /* Current cmap */
   struct fb_ops *fbops;
   char *screen_base;                   /* Virtual address */
//?   struct display *disp;		/* initial display variable */
//?   struct vc_data *display_fg;		/* Console visible on this display */
   char fontname[40];			/* default font name */
//?   devfs_handle_t devfs_handle;         /* Devfs handle for new name         */
//?   devfs_handle_t devfs_lhandle;        /* Devfs handle for compat. symlink  */
//?   int (*changevar)(int);		/* tell console var has changed */
   int (*switch_con)(int, struct fb_info*);
					/* tell fb to switch consoles */
//?   int (*updatevar)(int, struct fb_info*);
					/* tell fb to update the vars */
//?   void (*blank)(int, struct fb_info*);	/* tell fb to (un)blank the screen */
					/* arg = 0: unblank */
					/* arg > 0: VESA level (arg-1) */
   void *pseudo_palette;                /* Fake palette of 16 colors and 
					   the cursor's color for non
                                           palette mode */
   /* From here on everything is device dependent */
   void *par;	
};

   /*
    *    Hardware Cursor
    */

#define FBIOGET_FCURSORINFO     0x4607
#define FBIOGET_VCURSORINFO     0x4608
#define FBIOPUT_VCURSORINFO     0x4609
#define FBIOGET_CURSORSTATE     0x460A
#define FBIOPUT_CURSORSTATE     0x460B


struct fb_fix_cursorinfo {
	unsigned short crsr_width;		/* width and height of the cursor in */
	unsigned short crsr_height;		/* pixels (zero if no cursor)	*/
	unsigned short crsr_xsize;		/* cursor size in display pixels */
	unsigned short crsr_ysize;
	unsigned short crsr_color1;		/* colormap entry for cursor color1 */
	unsigned short crsr_color2;		/* colormap entry for cursor color2 */
};

struct fb_var_cursorinfo {
	unsigned short width;
	unsigned short height;
	unsigned short xspot;
	unsigned short yspot;
	unsigned char data[1];			/* field with [height][width]        */
};

struct fb_cursorstate {
	short xoffset;
	short yoffset;
	unsigned short mode;
};

#define FB_CURSOR_OFF		0
#define FB_CURSOR_ON		1
#define FB_CURSOR_FLASH		2

int m68k4fb_init(void);
int register_framebuffer(struct fb_info *fb_info);

//lcd cmd
#define  LCD_IOC_MAGIC        	  'L'
#define  LCD_IOC_ON       	  _IO(LCD_IOC_MAGIC, 0)
#define  LCD_IOC_OFF          	  _IO(LCD_IOC_MAGIC, 1)
#define  LCD_IOC_ONBKLIGHT	  _IO(LCD_IOC_MAGIC, 2)
#define  LCD_IOC_OFFBKLIGHT	  _IO(LCD_IOC_MAGIC, 3)	
#define  CUR_IOC_ENBLNK	  	  _IO(LCD_IOC_MAGIC, 4)
#define  CUR_IOC_DISBLNK	  _IO(LCD_IOC_MAGIC, 5)
#define  CUR_IOC_WIDTH		  _IOW(LCD_IOC_MAGIC, 6, char)
#define  CUR_IOC_HIGHT		  _IOW(LCD_IOC_MAGIC, 7, char)
#define  CUR_IOC_COLOR		  _IOW(LCD_IOC_MAGIC, 8, long)
#define  CUR_IOC_XPOS		  _IOW(LCD_IOC_MAGIC, 9, short)
#define  CUR_IOC_YPOS		  _IOW(LCD_IOC_MAGIC, 10, short)

#define LCD_IOC_RESET         _IO(LCD_IOC_MAGIC, 11)
#define LCD_IOC_ONCONTRAST    _IO(LCD_IOC_MAGIC, 12)
#define LCD_IOC_OFFCONTRAST   _IO(LCD_IOC_MAGIC, 13)
#define LCD_IOC_SETADDR       _IOW(LCD_IOC_MAGIC, 14, long)
#define LCD_IOC_SETWIDTH      _IOW(LCD_IOC_MAGIC, 15, short)
#define LCD_IOC_SETHEIGHT     _IOW(LCD_IOC_MAGIC, 16, short)
#define LCD_IOC_REFRESHRATE   _IOW(LCD_IOC_MAGIC, 17, short)
#define LCD_IOC_SETCXP        _IOW(LCD_IOC_MAGIC, 18, short)
#define LCD_IOC_SETCYP        _IOW(LCD_IOC_MAGIC, 19, short)
#define LCD_IOC_SETCWCH       _IOW(LCD_IOC_MAGIC, 20, short)
#define LCD_IOC_SETBLK        _IOW(LCD_IOC_MAGIC, 21, short)
#define LCD_IOC_SETMODE       _IOW(LCD_IOC_MAGIC, 22, short)
#define LCD_IOC_SETCONTRAST   _IOW(LCD_IOC_MAGIC, 23, short)
