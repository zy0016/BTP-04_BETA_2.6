
#ifndef	_DGIF_H_
#define	_DGIF_H_

typedef	void * PGIFOBJ;
typedef struct gif_datasource * PGIFDS;

struct gif_datasource
{
	unsigned char * next_input_byte;
	int		bytes_in_buffer;
    unsigned char * first_input_byte;
    int     nbytes;
	void	(*init_source)(PGIFOBJ pGifObj);
	int		(*fill_input_buffer)( PGIFOBJ pGifObj );
	void	(*skip_input_data)( PGIFOBJ pGifObj, int num_bytes );
	int		(*resync_to_restart)( PGIFOBJ pGifObj, int desired );
	void	(*term_source)( PGIFOBJ pGifObj );
};

/*
 * Struct of logical screen.
 */

struct gif_logscreen
{
		unsigned short	width;
		unsigned short	height;
		unsigned char	bkg_color;
		unsigned char	aspect_ratio;
		unsigned char	flag;
		unsigned char	unused_1;
		unsigned short	palette_entries;
		unsigned short	unused_2;
		void *			p_color_table;
};

/*
 * Struct of graphics frame.
 */

typedef struct gif_frame
{
		unsigned short	left;
		unsigned short	top;
		unsigned short	width;
		unsigned short	height;
		unsigned short	delay_time;
		unsigned short	palette_entries;
		unsigned char	disposal_method;
		unsigned char	transparent;
		unsigned char	flags;
		unsigned char	unused;
		void *			p_color_table;
}gif_frame, *pGifFrame;
 

struct lwz_block
{
	int		state;			/* LWZ state */
	int		size;			/* LWZ deecoder size */
	int		fc;				/* Old charactor */
	int		oc;				/* Old charactor */
	int		curr_size;		/* The current code size */
	int		clear_code;		/* Value for a clear code */
	int		ending_code;	/* Value for a ending code */
	int		newcodes;		/* First available code */
	int		top_slot;		/* Highest code for current size */
	int		slot;			/* Last read code */
	int		code_mask;		/* Mask of code */
	int		bad_code_count;
	int		bit_count;		/* # bits left in current block */
	int		bit_shift;		/* Start bit in 1st byte */

	unsigned char *	p_outptr;		/* Gif decoder output pointer */
	unsigned char *	p_inptr;		/* Pointer to next byte in block */
};

struct gif_decoder
{
	int		fsm_mainstate;		/* Main FSM state */
	int		fsm_extension;		/* In extension process */
	int		fsm_scanline;		/* Output scan line number */
//	int		fsm_substate;		/* State of decoder */
	void *  user_data;			/* User data */
	unsigned char	ending_flag;		/* Ending flag hit */
	unsigned char	graphic_ext_flag;	/* Graphics control ext flag hit */
//	BYTE	ext_label;			/* Extension block label */
//	BYTE	ds_inited;			/* If data source initialized */
	int		frame_n;			/* Current frame number */
	int		read_offset;		/* Image read pointer */
	int		block_size;			/* # bytes of one data block */
//	int		bytes_read;			/* # bytes read in one data block */
	int		frame_start;		/* Start position of 1st frame */
	int		color_mode;			/* Output color mode */
	void  (*out_function)(unsigned char * pOut, unsigned char * pIn,
					int width, unsigned char * pColorTable );
	void  (*fill_function)(unsigned char * pOut, int bkg_color,
					int width, unsigned char * pColorTable );
	struct gif_datasource  *pDS;	/* Pointer to data source */
    struct gif_datasource  DataSrc;

	unsigned char *  p_global_color;		/* Global color table */
	unsigned char *	p_local_color;		/* Local color table */
	unsigned char *	p_lwz_suffix;		/* Suffix table */
	unsigned short *	p_lwz_prefix;		/* Prefix linked list */
	unsigned char *	p_lwz_outbuf;		/* Output buffer */

	struct gif_logscreen screen;	/* Logical screen */
	struct gif_frame frame;		/* Frame attribute */
	struct lwz_block  lwz;		/* LWZ state block */
	unsigned char	ds_buff[260];		/* Data source buffer */
};

#define	GIF_COLORTBL			0x01
#define	GIF_SORTFLAG			0x02
#define	GIF_INTERLACE			0x10
#define	GIF_TRANSPARENT			0x20
#define	GIF_USERINPUT			0x80

#define	GCS_RGB			0
#define	GCS_RAW			1

typedef	struct gif_logscreen * PLSCREEN;
typedef struct gif_frame * PGIFFRAME;

#ifdef __cplusplus
extern "C" {
#endif

int GIF_GetObjectSize( void );
int GIF_Construct ( PGIFOBJ pGif );
void GIF_InitSource( PGIFOBJ pGif, struct gif_datasource *m_datasrc);
int GIF_Destruct ( PGIFOBJ pGif );
int GIF_SetUserData( PGIFOBJ pGif, void * user_data );
void * GIF_GetUserData( PGIFOBJ pGif );
int GIF_SetDataSource ( PGIFOBJ pGif, PGIFDS pDataSource);
int GIF_SetDataSrc( PGIFOBJ pGif, PGIFDS pDataSource );
PGIFDS GIF_GetDataSource ( PGIFOBJ pGif );
int GIF_ReadHeader( PGIFOBJ pGif );
int GIF_SetColorMode( PGIFOBJ pGif, int color_mode );
int GIF_GetLogicalScreen( PGIFOBJ pGif, PLSCREEN pReturn );
int GIF_ReadFrameHeader( PGIFOBJ pGif, int flag );
int GIF_GetFrameDimensions( PGIFOBJ pGif,  PGIFFRAME pReturn );
int GIF_ReadScanlines( PGIFOBJ pGif, unsigned char ** pLineTable, int lines );
int GIF_ConsumeInput( PGIFOBJ pObject );
int GIF_EndFrame( PGIFOBJ pObject );
int GIF_GetOutputScanline( PGIFOBJ pObject );
int GIF_InputComplete( PGIFOBJ pGif );
int GIF_Reset( PGIFOBJ pGif);

/* Memory allocation */
void * gif_alloc_memory( PGIFOBJ pGif, int sizeofobject );
void gif_free_memory( PGIFOBJ pObject, void * memaddr, int sizeofobject);

#ifdef __cplusplus
}
#endif

#endif
