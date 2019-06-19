
#ifndef	_DJPEG_H_
#define	_DJPEG_H_

typedef struct jpeg_dimensions
{
	unsigned char major_version;
	unsigned char minor_version;
	short		flags;
	short		density_unit;
	short		X_density;
	short		Y_density;
	short		image_width;
	short		image_height;
	short		color_mode;
	short		num_components;
	short		adobe_transform;
} JPEG_DIMENSIONS;

#define	JPEG_PROGRESSIVE	1
#define	JPEG_ADOBEMARKER	2
#define	JPEG_JFIFMARKER		4

struct jpeg_outputmode
{
	short		out_width;
	short		out_height;
	short		out_color_components;
	short		out_components;
	short		image_height;
	short		color_map;
	short		actual_number_of_colors;
};

typedef void * PJPEGOBJ;

typedef struct jpeg_datasource
{
	unsigned char * next_input_byte;
	int		bytes_in_buffer;
	void	(* init_source) ( PJPEGOBJ pJpeg );
	int		(* fill_input_buffer) ( PJPEGOBJ pJpeg );
	void	(* skip_input_data) ( PJPEGOBJ pJpeg, int num_bytes );
	int		(* resync_to_restart) ( PJPEGOBJ pJpeg, int desired );
	void	(* term_source) ( PJPEGOBJ pJpeg );
}DSOBJ, * PDSOBJ;

typedef struct backing_store_struct {
	void (*read_backing_store) ( PJPEGOBJ pJpeg,
			struct backing_store_struct * pBackStore, void * buffer_address,
			long file_offset, long byte_count);
	void (*write_backing_store) (PJPEGOBJ pJpeg,
			struct backing_store_struct * pBackStore, void * buffer_address,
			long file_offset, long byte_count);
	void (*close_backing_store) (PJPEGOBJ pJpeg,
			struct backing_store_struct * pBackStore);

	void * file_handle;
	char temp_name[64];
} backing_store_info;

#define JPEG_SUSPENDED		0 /* Suspended due to lack of input data */
#define JPEG_REACHED_SOS	1 /* Reached start of new scan */
#define JPEG_REACHED_EOI	2 /* Reached end of image */
#define JPEG_ROW_COMPLETED	3 /* Completed one iMCU row */
#define JPEG_SCAN_COMPLETED	4 /* Completed last iMCU row of a scan */

#ifdef __cplusplus
extern "C" {
#endif

int JPEG_GetObjectSize ( void );
int JPEG_Construct ( PJPEGOBJ  pObject);
int JPEG_Destruct ( PJPEGOBJ  pObject);
int JPEG_SetUserData ( PJPEGOBJ pObject, void * user_data);
void * JPEG_GetUserData ( PJPEGOBJ pObject );
int JPEG_SetMemLog ( PJPEGOBJ pObject, void * memlog);
void * JPEG_GetMemLog ( PJPEGOBJ pObject );
int JPEG_SetDataSource ( PJPEGOBJ pObject, PDSOBJ pDataSource);
void * JPEG_GetDataSource ( PJPEGOBJ pObject );
void JPEG_ErrorReturn ( PJPEGOBJ pObject, int error_number );
int JPEG_ReadHeader ( PJPEGOBJ pObject );
int JPEG_StartDecompress ( PJPEGOBJ pObject );
int JPEG_SetColorMode( PJPEGOBJ pObject, int color_mode);
int JPEG_GetImageDimensions( PJPEGOBJ pObject,  struct jpeg_dimensions * pReturn);
int JPEG_ReadScanlines( PJPEGOBJ pObject, char ** pLineTable, int lines);
int JPEG_GetOutputScanline( PJPEGOBJ pObject );
int JPEG_StartOutput ( PJPEGOBJ pObject, int scan_number );
int JPEG_FinishOutput ( PJPEGOBJ pObject );
int JPEG_InputComplete ( PJPEGOBJ pObject );
int JPEG_GetInputScanNumber ( PJPEGOBJ pObject );
int JPEG_ConsumeInput ( PJPEGOBJ pObject );
int JPEG_SetBufferedMode ( PJPEGOBJ pObject, int mode );
int JPEG_FinishDecompress ( PJPEGOBJ pObject );
void JPEG_AbortDecompress ( PJPEGOBJ pObject );
int JPEG_GetLastErrorCode ( PJPEGOBJ pObject );

/* memory allocation functions */
long   jpeg_mem_init ( PJPEGOBJ  pObject );
void   jpeg_mem_term ( PJPEGOBJ  pObject );
void * jpeg_get_small ( PJPEGOBJ pObject, int sizeofobject );
void   jpeg_free_small ( PJPEGOBJ pObject, void * memaddr, int sizeofobject );
void * jpeg_get_large ( PJPEGOBJ pObject, int sizeofobject );
void   jpeg_free_large ( PJPEGOBJ pObject, void * memaddr, int sizeofobject );
long   jpeg_mem_available ( PJPEGOBJ pObject, long min_bytes_needed,
							long max_bytes_needed, long already_allocated );
void   jpeg_open_backing_store (PJPEGOBJ pObject,
							struct backing_store_struct * pBackstore,
							long total_bytes_needed);

#ifdef __cplusplus
};
#endif

#endif
