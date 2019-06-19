
#include "jinclude.h"
#include "jpeglib.h"
#include "jpegint.h"
#include "jversion.h"
#include "jerror.h"

#include "djpeg.h"

#include <SETJMP.H>

METHODDEF(void) my_error_exit (j_common_ptr cinfo);
METHODDEF(void) output_message (j_common_ptr cinfo);
METHODDEF(void) emit_message (j_common_ptr cinfo, int msg_level);
METHODDEF(void) format_message (j_common_ptr cinfo, char * buffer);
METHODDEF(void) reset_error_mgr (j_common_ptr cinfo);


int JPEG_GetObjectSize( void )
{
int	count = 0;

	/* Base struct for deconpress */
	count += sizeof(struct jpeg_decompress_struct);
	count += sizeof(struct jpeg_error_mgr);
	count += sizeof(jmp_buf);//12/31

	return count;
}

/****************************************************************************
 *
 * Construct JPEG decoding object.
 *
 ****************************************************************************
 */

int JPEG_Construct( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;
struct jpeg_error_mgr * pErrMgr;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;

	/* Initialize error handle */
	pErrMgr = (struct jpeg_error_mgr *)(pJpegInfo + 1);

	pErrMgr->error_exit		 = my_error_exit;
	pErrMgr->emit_message	 = emit_message;
	pErrMgr->output_message	 = output_message;
	pErrMgr->format_message	 = format_message;
	pErrMgr->reset_error_mgr = reset_error_mgr;
	
	pErrMgr->trace_level	 = 0;
	pErrMgr->num_warnings	 = 0;
	pErrMgr->msg_code		 = 0;
	
	/* Initialize message table pointers */
	pErrMgr->jpeg_message_table	 = NULL;
	pErrMgr->last_jpeg_message	 = 0;
	
	pErrMgr->addon_message_table = NULL;
	pErrMgr->first_addon_message = 0;	/* for safety */
	pErrMgr->last_addon_message	 = 0;
	
	/* initialize the JPEG decompression object. */
	jpeg_create_decompress( pJpegInfo );
	/* Set error handle */
	pJpegInfo->err = pErrMgr;
	return 0;
}

/****************************************************************************
 *
 * Destruct process
 *
 ****************************************************************************
 */

int JPEG_Destruct( PJPEGOBJ pObject )
{
	/* Call abort process */
	jpeg_abort( pObject );
	jpeg_destroy( pObject );
	return 0;
}

/****************************************************************************
 *
 * Set user data
 *
 ****************************************************************************
 */

int JPEG_SetUserData ( PJPEGOBJ pObject, void * user_data)
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	pJpegInfo->client_data = user_data;
	return 1;
}

/****************************************************************************
 *
 * Get user data
 *
 ****************************************************************************
 */

void * JPEG_GetUserData ( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	return pJpegInfo->client_data;
}

/****************************************************************************
 *
 * Set pointer of memory log
 *
 ****************************************************************************
 */

int JPEG_SetMemLog ( PJPEGOBJ pObject, void * memlog)
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	pJpegInfo->mem_log = memlog;
	return 1;
}

/****************************************************************************
 *
 * Get pointer of memory log
 *
 ****************************************************************************
 */

void * JPEG_GetMemLog ( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	return pJpegInfo->mem_log;
}

/****************************************************************************
 *
 * Set data source
 *
 ****************************************************************************
 */

int JPEG_SetDataSource ( PJPEGOBJ pObject, PDSOBJ pDataSource)
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	if ( pDataSource->resync_to_restart == NULL )
		pDataSource->resync_to_restart =
		(int (*)(struct jpeg_decompress_struct *,int ))jpeg_resync_to_restart;
	pJpegInfo->src = (struct jpeg_source_mgr *)pDataSource;
	return 1;
}

/****************************************************************************
 *
 * Get data source
 *
 ****************************************************************************
 */

void * JPEG_GetDataSource ( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	return pJpegInfo->src;
}

/****************************************************************************
 *
 * Error Return
 *
 ****************************************************************************
 */

void JPEG_ErrorReturn ( PJPEGOBJ pObject, int error_number )
{
j_common_ptr pJpegInfo;

	pJpegInfo = (j_common_ptr)pObject;
	pJpegInfo->err->msg_code = error_number;
	pJpegInfo->err->error_exit( pJpegInfo );
}

int Jpeg_Reset( PJPEGOBJ pObject )
{
    j_decompress_ptr cinfo;
	cinfo = (j_decompress_ptr )pObject;
    cinfo->global_state = DSTATE_START;
	return 1;
}
/****************************************************************************
 *
 * Read jpeg header
 *
 ****************************************************************************
 */

int JPEG_ReadHeader( PJPEGOBJ pObject)
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_read_header( pObject, TRUE );
}

/****************************************************************************
 *
 * Set output color mode
 *
 ****************************************************************************
 */

int JPEG_SetColorMode( PJPEGOBJ pObject, int color_mode)
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	pJpegInfo->out_color_space = color_mode;
	return 0;
}

/****************************************************************************
 *
 * Get image dimension
 *
 ****************************************************************************
 */

int JPEG_GetImageDimensions( PJPEGOBJ pObject, struct jpeg_dimensions * pReturn)
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;

	pReturn->major_version	 = pJpegInfo->JFIF_major_version;
	pReturn->minor_version	 = pJpegInfo->JFIF_minor_version;
	pReturn->density_unit	 = pJpegInfo->density_unit;
	pReturn->X_density		 = pJpegInfo->X_density;
	pReturn->Y_density		 = pJpegInfo->Y_density;
	pReturn->image_width	 = pJpegInfo->image_width;
	pReturn->image_height	 = pJpegInfo->image_height;
	pReturn->color_mode		 = pJpegInfo->jpeg_color_space;
	pReturn->num_components	 = pJpegInfo->num_components;
	pReturn->adobe_transform = pJpegInfo->Adobe_transform;
	pReturn->flags			 = 0;

	if ( pJpegInfo->progressive_mode )
		pReturn->flags |= JPEG_PROGRESSIVE;
	if ( pJpegInfo->saw_Adobe_marker )
		pReturn->flags |= JPEG_ADOBEMARKER;
	if ( pJpegInfo->saw_JFIF_marker )
		pReturn->flags |= JPEG_JFIFMARKER;
	return 1;
}


/****************************************************************************
 *
 * Start decompress
 *
 ****************************************************************************
 */

int JPEG_StartDecompress( PJPEGOBJ pObject)
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */     
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_start_decompress( pObject );
}

/****************************************************************************
 *
 * Read in scan lines
 *
 ****************************************************************************
 */

int JPEG_ReadScanlines( PJPEGOBJ pObject, char ** pLineTable, int lines)
{
struct jpeg_error_mgr * pErrMgr;
int	   ret, count = 0;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return ( count > 0 ) ? count : -1;

	while ( lines > 0 )
	{
		ret = jpeg_read_scanlines(pObject, pLineTable, lines);
		if ( ret <= 0 )
			break;
		count += ret;
		pLineTable += ret;
		lines -= ret;
	}

	return count;
}

/****************************************************************************
 *
 * Get output scan line
 *
 ****************************************************************************
 */

int JPEG_GetOutputScanline( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	return pJpegInfo->output_scanline;
}


/****************************************************************************
 *
 * Start output
 *
 ****************************************************************************
 */

int JPEG_StartOutput ( PJPEGOBJ pObject, int scan_number )
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_start_output( pObject, scan_number );
}

/****************************************************************************
 *
 * Finish output
 *
 ****************************************************************************
 */

int JPEG_FinishOutput ( PJPEGOBJ pObject )
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_finish_output( pObject );
}

/****************************************************************************
 *
 * Check if input complete
 *
 ****************************************************************************
 */

int JPEG_InputComplete ( PJPEGOBJ pObject )
{
	return jpeg_input_complete( pObject );
}

/****************************************************************************
 *
 * Get input scan number
 *
 ****************************************************************************
 */

int JPEG_GetInputScanNumber ( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	return pJpegInfo->input_scan_number;
}

/****************************************************************************
 *
 * Consume input
 *
 ****************************************************************************
 */

int JPEG_ConsumeInput ( PJPEGOBJ pObject )
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_consume_input( pObject );
}

/****************************************************************************
 *
 * Set or clear buffered mode
 *
 ****************************************************************************
 */

int JPEG_SetBufferedMode ( PJPEGOBJ pObject, int mode )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;
	pJpegInfo->buffered_image = mode;
	return 1;
}

/****************************************************************************
 *
 * Finish decompress
 *
 ****************************************************************************
 */

int JPEG_FinishDecompress ( PJPEGOBJ pObject )
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = (*(j_common_ptr)pObject).err;
	/* If error, return with -1 */
	if ( setjmp( (void *)(pErrMgr + 1) ) )
		return -1;
	return jpeg_finish_decompress( pObject );
}

/****************************************************************************
 *
 * Abort decompress
 *
 ****************************************************************************
 */

void JPEG_AbortDecompress ( PJPEGOBJ pObject )
{
	jpeg_abort_decompress( pObject );
}

/****************************************************************************
 *
 * Get error code
 *
 ****************************************************************************
 */

int JPEG_GetLastErrorCode ( PJPEGOBJ pObject )
{
struct jpeg_decompress_struct * pJpegInfo;

	pJpegInfo = (struct jpeg_decompress_struct *)pObject;

	return ( pJpegInfo->err->msg_code );
}

/****************************************************************************
 *
 * Error handle programs.
 *
 ****************************************************************************
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
struct jpeg_error_mgr * pErrMgr;

	pErrMgr = cinfo->err;
	
	/* Return control to the setjmp point */
	longjmp((void *)(pErrMgr + 1), pErrMgr->msg_code);
}

METHODDEF(void)
output_message (j_common_ptr cinfo)
{
	return;
}

METHODDEF(void)
emit_message (j_common_ptr cinfo, int msg_level)
{
	return;
}

METHODDEF(void)
format_message (j_common_ptr cinfo, char * buffer)
{
	return;
}

METHODDEF(void)
reset_error_mgr (j_common_ptr cinfo)
{
	cinfo->err->num_warnings = 0;
	cinfo->err->msg_code = 0;
}

