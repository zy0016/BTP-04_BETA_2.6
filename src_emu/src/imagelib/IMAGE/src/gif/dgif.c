
#include "dgif.h"
#include "gif_fsm.h"

//#include "memory.h"
#include "string.h"

static void GIF_OutputRGB( unsigned char * pOut, unsigned char * pIn,
					 int width, unsigned char * pColorTable )
{
unsigned char * ptr;

	while ( width -- )
	{
		ptr = pColorTable + *pIn * 3;
		pIn ++;
		*pOut ++ = *ptr ++;
		*pOut ++ = *ptr ++;
		*pOut ++ = *ptr ++;
	}
}

static void GIF_OutputRaw( unsigned char * pOut, unsigned char * pIn,
						  int width, unsigned char * pColorTable )
{
	memcpy( pOut, pIn, width );
}

static void GIF_FillBkgRGB( unsigned char * pOut, int bkg_color,
						  int width, unsigned char * pColorTable )
{
unsigned char * ptr;

	ptr = pColorTable + bkg_color * 3;
	while ( width -- )
	{
		*pOut ++ = *(ptr + 0);
		*pOut ++ = *(ptr + 1);
		*pOut ++ = *(ptr + 2);
	}
}

static void GIF_FillBkgRaw( unsigned char * pOut, int bkg_color,
						  int width, unsigned char * pColorTable )
{
	while ( width -- )
		*pOut ++ = (unsigned char)bkg_color;
}

int GIF_GetObjectSize( void )
{
int	retval;

	retval = sizeof(struct gif_decoder);	// Base size.
	retval += 2 * 256 * 3;	// Global color table and local color table.
//	retval += LWZ_MAX_OUTBYTE * 2;	// out buffer;
//	retval += (LWZ_MAX_CODES + 1) * sizeof(WORD);	// prefix;
//	retval += (LWZ_MAX_CODES + 1);					// suffix;
	return retval;
}


int GIF_Construct ( PGIFOBJ  pObject)
{
struct gif_decoder * pGif;
char * ptr;
int	i;

	pGif = (struct gif_decoder *)pObject;

	/* Clear to all zeros */
	ptr = (char *)pGif;
	for ( i = 0; i < sizeof(struct gif_decoder); i ++ )
		*ptr ++ = 0;

	ptr = (char *)(pGif + 1);

	pGif->p_global_color = ptr;
	ptr += 256 * 3;

	pGif->p_local_color = ptr;
	ptr += 256 * 3;

//	pGif->p_lwz_outbuf = ptr;
//	ptr += LWZ_MAX_OUTBYTE * 2;
//
//	pGif->p_lwz_prefix = (WORD *)ptr;
//	ptr += (LWZ_MAX_CODES + 1) * sizeof(WORD);
//
//	pGif->p_lwz_suffix = ptr;
//	ptr += LWZ_MAX_CODES + 1;
//
//	/* Set initial fields */
//	pGif->fsm_mainstate = S_HEADER;
//	pGif->color_mode = GCS_RAW;
//
//	pGif->out_function = GIF_OutputRaw;
//	pGif->fill_function = GIF_FillBkgRaw;

	return 1;
}

int GIF_Destruct ( PGIFOBJ  pObject)
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	if ( pGif->pDS != NULL )
		pGif->pDS->term_source( pGif );

	/* Free the decoding memory */
	if ( pGif->p_lwz_outbuf != NULL )
		gif_free_memory( pGif, pGif->p_lwz_outbuf, LWZ_MAX_OUTBYTE * 2 );

	if ( pGif->p_lwz_prefix != NULL )
		gif_free_memory( pGif, pGif->p_lwz_prefix,
							(LWZ_MAX_CODES + 1) * sizeof(WORD) * 2 );

	if ( pGif->p_lwz_suffix != NULL )
		gif_free_memory( pGif, pGif->p_lwz_suffix, (LWZ_MAX_CODES + 1));
	return 0;
}

int GIF_SetDataSource ( PGIFOBJ pObject, PGIFDS pDataSource)
{
	(*(struct gif_decoder *)pObject).pDS = pDataSource;
	pDataSource->init_source( pObject );
	return 0;
}


PGIFDS GIF_GetDataSource ( PGIFOBJ pObject )
{
	return (*(struct gif_decoder *)pObject).pDS;
}

int GIF_SetUserData( PGIFOBJ pObject, void * user_data )
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;
	pGif->user_data = user_data;
	return 0;
}

void * GIF_GetUserData( PGIFOBJ pObject )
{
	return (*(struct gif_decoder *)pObject).user_data;
}

int GIF_ReadHeader( PGIFOBJ pObject)
{
struct gif_decoder * pGif;
int	ret;

	pGif = (struct gif_decoder *)pObject;

	/* Advance fsm state to S_DATA */
	while ( pGif->fsm_mainstate < S_DATA )
	{
		if ( (ret = gif_fsm_func[pGif->fsm_mainstate]( pGif ) ) <= 0 )
			return ret;
	}
	return 1;
}

int GIF_SetColorMode( PGIFOBJ pObject, int color_mode)
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	pGif->color_mode = color_mode;

	/* Set output function */
	if ( color_mode == GCS_RAW )
	{
		pGif->out_function = GIF_OutputRaw;
		pGif->fill_function = GIF_FillBkgRaw;
	}
	else
	{
		pGif->out_function = GIF_OutputRGB;
		pGif->fill_function = GIF_FillBkgRGB;
	}

	return 0;
}

int GIF_GetLogicalScreen( PGIFOBJ pObject, PLSCREEN pReturn)
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	if ( pGif->fsm_mainstate >= S_DATA )
	{
		*pReturn = pGif->screen;
		return 1;
	}
	return -1;
}

int GIF_ReadFrameHeader( PGIFOBJ pObject, int flag)
{
struct gif_decoder * pGif;
unsigned char * psave, * ptr;
unsigned char blk_len;
int	i, ret = 1;

	pGif = (struct gif_decoder *)pObject;

	/* If user don't call GIF_ReadHeader, automatically call it */
	while ( pGif->fsm_mainstate < S_DATA )
	{
		ret = gif_fsm_func[pGif->fsm_mainstate]( pGif );
		if ( ret <= 0 )
			return ret;
	}

	/* May be change to next frame, skip all data of current frame */
	if ( pGif->fsm_mainstate == S_IMGDATA_1 )
	{	/* Advance state to S_IMGDATA_2 */
		ret = gif_fsm_func[pGif->fsm_mainstate]( pGif );
		if ( ret <= 0 )
			return ret;
	}

	/* skip this frame by read all data block until one with len = 0 */
	while ( pGif->fsm_mainstate == S_IMGDATA_2 )
	{
		if ( pGif->pDS->bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
		}
		blk_len = *pGif->pDS->next_input_byte ++;
		pGif->pDS->bytes_in_buffer --;

		if ( blk_len != 0 )
		{
			pGif->pDS->skip_input_data( pGif, blk_len );
			continue;
		}

		/* Clear initial state for next frame */
		pGif->fsm_mainstate = S_DATA;
		pGif->fsm_extension = 0;
		pGif->ending_flag = 0;
		/* Clear the frame structure */
		psave = pGif->frame.p_color_table;
		ptr = (unsigned char *)&pGif->frame;
		for ( i = 0; i < sizeof(pGif->frame); i ++ )
			*ptr ++ = 0;
		pGif->frame.p_color_table = psave;
		/* Clear the LWZ output counter */
		pGif->lwz.p_outptr = pGif->p_lwz_outbuf;
		pGif->read_offset = 0;
		break;
	}

	/* Advance fsm state to S_IMGDATA */
	while ( pGif->fsm_mainstate < S_IMGDATA_1 )
	{
		ret = gif_fsm_func[pGif->fsm_mainstate]( pGif );
		if ( ret <= 0 )
			return ret;
	}

	/* Allocate work memory for decode */
	if ( pGif->p_lwz_outbuf == NULL )
	{
		ptr = gif_alloc_memory( pGif, LWZ_MAX_OUTBYTE * 2 );
		if ( ptr == NULL )
			return -1;
		pGif->p_lwz_outbuf = ptr;
	}

	if ( pGif->p_lwz_prefix == NULL )
	{
		ptr = gif_alloc_memory( pGif, (LWZ_MAX_CODES + 1) * sizeof(WORD));
		if ( ptr == NULL )
			return -1;
		pGif->p_lwz_prefix = (WORD *)ptr;
	}

	if ( pGif->p_lwz_suffix == NULL )
	{
		ptr = gif_alloc_memory( pGif, (LWZ_MAX_CODES + 1));
		if ( ptr == NULL )
			return -1;
		pGif->p_lwz_suffix = ptr;
	}

	return (pGif->fsm_mainstate == S_IMGDATA_1) ? 1 : -1; 
}


int GIF_ConsumeInput( PGIFOBJ pObject )
{
struct gif_decoder * pGif;
unsigned char * psave, * ptr;
unsigned char blk_len;
int	i, ret = 1;

	pGif = (struct gif_decoder *)pObject;

next_frame:

	/* If less than IMGDATA_2, just do it */
	while ( pGif->fsm_mainstate < S_IMGDATA_2 )
	{
		ret = gif_fsm_func[pGif->fsm_mainstate]( pGif );
		if ( ret <= 0 )
			return ret;
	}

	/* skip this frame by read all data block until one with len = 0 */
	while ( pGif->fsm_mainstate == S_IMGDATA_2 )
	{
		if ( pGif->pDS->bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
		}
		blk_len = *pGif->pDS->next_input_byte ++;
		pGif->pDS->bytes_in_buffer --;

		if ( blk_len != 0 )
		{
			pGif->pDS->skip_input_data( pGif, blk_len );
			continue;
		}

		/* Clear initial state for next frame */
		pGif->fsm_mainstate = S_DATA;
		pGif->fsm_extension = 0;
		pGif->ending_flag = 0;
		/* Clear the frame structure */
		psave = pGif->frame.p_color_table;
		ptr = (unsigned char *)&pGif->frame;
		for ( i = 0; i < sizeof(pGif->frame); i ++ )
			*ptr ++ = 0;
		pGif->frame.p_color_table = psave;
		/* Clear the LWZ output counter */
		pGif->lwz.p_outptr = pGif->p_lwz_outbuf;
		pGif->read_offset = 0;
		break;
	}

	if ( pGif->fsm_mainstate < S_TRAILER )
		goto next_frame;
	
	return (pGif->fsm_mainstate < S_TRAILER) ? 1 : -1;
}



int GIF_GetFrameDimensions( PGIFOBJ pObject,  PGIFFRAME pReturn)
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	*pReturn = pGif->frame;
	return 0;
}

int GIF_ReadScanlines( PGIFOBJ pObject, unsigned char ** pLineTable, int lines)
{
struct gif_decoder * pGif;
BYTE  * out_ptr, * pColorTable;
int    count, img_width, out_lines = 0;
int	   ret = 0;

	pGif = (struct gif_decoder *)pObject;

	if ( pGif->fsm_mainstate == S_IMGDATA_1 )
	{
		if ( (ret = gif_fsm_func[S_IMGDATA_1]( pGif ) ) <= 0 )
			return ret;
	}

	if ( pGif->fsm_mainstate != S_IMGDATA_2 )
		return -1;

	/* put image width into register variable for faster access */
	img_width = pGif->frame.width;

	/* Locate color table */
	if ( pGif->frame.flags & GIF_COLORTBL )
		pColorTable = pGif->frame.p_color_table;
	else
		pColorTable = pGif->screen.p_color_table;

	while ( out_lines < lines )
	{	/* Check output lines */
		out_ptr = pGif->p_lwz_outbuf + pGif->read_offset;
		if ( (count = pGif->lwz.p_outptr - out_ptr) >= img_width )
		{
			if ( pLineTable != NULL )
			{
				pGif->out_function(*pLineTable, out_ptr, img_width, pColorTable);
				pLineTable ++;
			}
			out_lines ++;
			pGif->read_offset += img_width;
			continue;
		}

		/* If frame end, error return */
		if ( pGif->ending_flag )
		{	/* Fill output line with background color */
			if ( pLineTable != NULL )
			{
				if ( count > 0 )
					pGif->out_function(*pLineTable, out_ptr, count, pColorTable);
				pGif->fill_function(*pLineTable, pGif->screen.bkg_color,
						img_width - count, pColorTable );
				pLineTable ++;
			}
			pGif->read_offset += count;
			out_lines ++;
			continue;
		}

		/* Copy remained data into head of output buffer */
		if ( pGif->read_offset != 0 && count > 0 )
			memcpy( pGif->p_lwz_outbuf, out_ptr, count );

		/* Reset output pointer and decode new lines */
		pGif->lwz.p_outptr = pGif->p_lwz_outbuf + count;
		pGif->read_offset = 0;
    	if ( (ret = gif_fsm_func[S_IMGDATA_2]( pGif ) ) <= 0 )
			break;
	}

	/* Modify the output scanline counter */
	pGif->fsm_scanline += out_lines;

	return (ret < 0) ? ret : out_lines;
}

int GIF_EndFrame( PGIFOBJ pObject )
{
struct gif_decoder * pGif;
unsigned char * psave, * ptr;
unsigned char blk_len;
int	i, ret = 1;

	pGif = (struct gif_decoder *)pObject;
	if ( pGif->fsm_mainstate != S_IMGDATA_2 )
		return -1;

	/* skip this frame by read all data block until one with len = 0 */
	while ( pGif->fsm_mainstate == S_IMGDATA_2 )
	{
		if ( pGif->pDS->bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
		}
		blk_len = *pGif->pDS->next_input_byte ++;
		pGif->pDS->bytes_in_buffer --;

		if ( blk_len == 0 )
			break;
		pGif->pDS->skip_input_data( pGif, blk_len );
	}

	/* Clear initial state for next frame */
	pGif->fsm_mainstate = S_DATA;
	pGif->fsm_extension = 0;
	pGif->ending_flag = 0;
	/* Clear the frame structure */
	psave = pGif->frame.p_color_table;
	ptr = (unsigned char *)&pGif->frame;
	for ( i = 0; i < sizeof(pGif->frame); i ++ )
		*ptr ++ = 0;
	pGif->frame.p_color_table = psave;
	/* Clear the LWZ output counter */
	pGif->lwz.p_outptr = pGif->p_lwz_outbuf;
	pGif->read_offset = 0;

	/* Free the decoding memory */
	if ( pGif->p_lwz_outbuf != NULL )
		gif_free_memory( pGif, pGif->p_lwz_outbuf, LWZ_MAX_OUTBYTE * 2 );

	if ( pGif->p_lwz_prefix != NULL )
		gif_free_memory( pGif, pGif->p_lwz_prefix,
							(LWZ_MAX_CODES + 1) * sizeof(WORD) );

	if ( pGif->p_lwz_suffix != NULL )
		gif_free_memory( pGif, pGif->p_lwz_suffix, (LWZ_MAX_CODES + 1));

	pGif->p_lwz_outbuf = NULL;
	pGif->p_lwz_prefix = NULL;
	pGif->p_lwz_suffix = NULL;

	return 1;
}

int GIF_GetOutputScanline( PGIFOBJ pObject )
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	return pGif->fsm_scanline;
}


/* Check if input complete */

int GIF_InputComplete( PGIFOBJ pObject )
{
struct gif_decoder * pGif;

	pGif = (struct gif_decoder *)pObject;

	return (pGif->fsm_mainstate == S_TRAILER);
}

/* Reset to initial state */

int GIF_Reset( PGIFOBJ pObject )
{
struct gif_decoder * pGif;
//int	ret;

	pGif = (struct gif_decoder *)pObject;
	pGif->fsm_extension = 0;
	pGif->ending_flag = 0;

	/* Reset data source */
	pGif->pDS->init_source( pGif );
	pGif->fsm_mainstate = S_HEADER;

//	pGif->fsm_substate = 0;
//	pGif->bytes_read = 0;

//	/* Advance fsm state to S_DATA */
//	while ( pGif->fsm_mainstate < S_DATA )
//	{
//		if ( (ret = gif_fsm_func[pGif->fsm_mainstate]( pGif ) ) <= 0 )
//			return ret;
//	}
	return 1;
}
