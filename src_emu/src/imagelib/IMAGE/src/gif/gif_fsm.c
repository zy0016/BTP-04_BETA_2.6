
#include "dgif.h"
#include "gif_fsm.h"

//#include "memory.h"
#include "string.h"

static const char gif_87a[8] = "gif87a";
static const char gif_89a[8] = "gif89a";

static int gif_get16( void * ptr )
{
unsigned char * p = (unsigned char *)ptr;

	return ((int)*(p+1) << 8) | (int)*p;
}

static int stricmp_6( char * pstr1, char * pstr2 )
{
int	i;

	for ( i = 0; i < 6; i ++ )
		if ( *pstr1 ++ != (*pstr2 ++ | 0x20) )
			break;

	return i - 6;
}

int GIF_FSM_Ext(struct gif_decoder * pGif);

int GIF_FSM_Header( struct gif_decoder * pGif )
{
int	bytes_to_read, count, ret;
unsigned char * next_input_byte;
int	bytes_in_buffer;

	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;
	bytes_to_read   = 13;

	/* Read in 6 + 7 = 13 bytes */
	while ( bytes_to_read > 0 )
	{
		if ( bytes_in_buffer <= 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
			next_input_byte = pGif->pDS->next_input_byte;
			bytes_in_buffer = pGif->pDS->bytes_in_buffer;
		}
		count = min( bytes_to_read, bytes_in_buffer );

		memcpy(	&pGif->ds_buff[13 - bytes_to_read],
						next_input_byte, count );
		next_input_byte += count;
		bytes_in_buffer -= count;
		bytes_to_read -= count;
	}

	/* We consume the heaser, set restart pointer */
	pGif->pDS->next_input_byte = next_input_byte;
	pGif->pDS->bytes_in_buffer = bytes_in_buffer;

	/* Check the flag */
	if ( stricmp_6 ( (char *)gif_89a, pGif->ds_buff ) != 0 &&
				stricmp_6 ( (char *)gif_87a, pGif->ds_buff ) != 0 )
	{
		pGif->fsm_mainstate = S_ERROR;
		return -1;
	}

	/* Set logical screen parameter */
	pGif->fsm_mainstate		 = S_DATA;
//	pGif->fsm_substate		 = 0;
	pGif->fsm_extension		 = 0;
	pGif->screen.palette_entries = 0;
	pGif->screen.flag		 = 0;
	pGif->screen.width		 = gif_get16( &pGif->ds_buff[6] );
	pGif->screen.height		 = gif_get16( &pGif->ds_buff[8] );
	pGif->screen.bkg_color	 = pGif->ds_buff[11];
	pGif->screen.aspect_ratio = pGif->ds_buff[12];
	if ( pGif->ds_buff[10] & 0x08 )
		pGif->screen.flag |= GIF_SORTFLAG;

	if ( pGif->ds_buff[10] & 0x80 )
	{
		pGif->screen.flag |= GIF_COLORTBL;
		pGif->screen.palette_entries = 1 << ((pGif->ds_buff[10] & 0x07) + 1);
		pGif->fsm_mainstate = S_GCOLOR;
		pGif->screen.p_color_table = pGif->p_global_color;
	}
	return 1;
}

int GIF_FSM_GColor( struct gif_decoder * pGif )
{
int	bytes_to_read, count, ret;
unsigned char * next_input_byte;
int	bytes_in_buffer;
char * ptr;

	bytes_to_read = pGif->screen.palette_entries * 3;
	ptr = pGif->screen.p_color_table;

	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;

	while ( bytes_to_read > 0 )
	{
		if ( bytes_in_buffer <= 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
			next_input_byte = pGif->pDS->next_input_byte;
			bytes_in_buffer = pGif->pDS->bytes_in_buffer;
		}
		count = min( bytes_to_read, bytes_in_buffer );

		memcpy(	ptr, next_input_byte, count );
		next_input_byte += count;
		bytes_in_buffer -= count;
		ptr += count;
		bytes_to_read -= count;
	}

	/* We consume the heaser, set restart pointer */
	pGif->pDS->next_input_byte = next_input_byte;
	pGif->pDS->bytes_in_buffer = bytes_in_buffer;

	/* Change state */
//	pGif->fsm_substate  = 0;
	pGif->fsm_extension = 0;
	pGif->fsm_mainstate = S_DATA;
	return 1;
}

int GIF_FSM_LColor( struct gif_decoder * pGif )
{
int	bytes_to_read, count, ret;
unsigned char * next_input_byte;
int	bytes_in_buffer;
char * ptr;

	bytes_to_read = pGif->frame.palette_entries * 3;
	ptr = pGif->frame.p_color_table;    //	ptr = pGif->screen.p_color_table;   Modify by sj

	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;

	while ( bytes_to_read > 0 )
	{
		if ( bytes_in_buffer <= 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
			next_input_byte = pGif->pDS->next_input_byte;
			bytes_in_buffer = pGif->pDS->bytes_in_buffer;
		}
		count = min( bytes_to_read, bytes_in_buffer );

		memcpy(	ptr, next_input_byte, count );
		next_input_byte += count;
		bytes_in_buffer -= count;
		ptr += count;
		bytes_to_read -= count;
	}

	/* We consume the heaser, set restart pointer */
	pGif->pDS->next_input_byte = next_input_byte;
	pGif->pDS->bytes_in_buffer = bytes_in_buffer;

//	pGif->fsm_substate  = 0;
	pGif->fsm_extension = 0;
	pGif->fsm_mainstate = S_IMGDATA_1;
	return 1;
}

int GIF_FSM_Data(struct gif_decoder * pGif)
{
unsigned char i_code;
int	ret;

	pGif->ending_flag = 0;
	while ( pGif->fsm_mainstate == S_DATA )
	{	/* Do sub state function */
//		while ( pGif->fsm_substate != 0 )
//			gif_fsm_subfunc[pGif->fsm_substate]( pGif );
		if ( pGif->fsm_extension )
		{	// Call extension process.
			ret = GIF_FSM_Ext( pGif );
			if ( ret <= 0 )
				return ret;
			pGif->fsm_extension = 0;
		}

		/* Read in extension labels */
		if ( pGif->pDS->bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
		}
		i_code = *pGif->pDS->next_input_byte ++;
		pGif->pDS->bytes_in_buffer --;

		/* Change state */
		switch ( i_code )
		{
		case 0x00:
			break;
		case 0x2C:
			pGif->fsm_mainstate = S_IMGDESC;
			break;
		case 0x3B:
			pGif->fsm_mainstate = S_TRAILER;
			break;
		case 0x21:
//			pGif->fsm_substate  = S_EXT;
			pGif->fsm_extension = 0x7f;
			break;
		default:
			pGif->fsm_mainstate = S_ERROR;
			return -1;
		}
	}
	return 1;	
}

int GIF_FSM_ImageDesc( struct gif_decoder * pGif )
{
int	bytes_to_read, count, flags, ret;
unsigned char * next_input_byte;
int	bytes_in_buffer;

	/* Image descriptor is 10 bytes long, byte 1 is read by FSM_DATA */
	bytes_to_read   = 9;
	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;

	while ( bytes_to_read > 0 )
	{
		if ( bytes_in_buffer <= 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
			next_input_byte = pGif->pDS->next_input_byte;
			bytes_in_buffer = pGif->pDS->bytes_in_buffer;
		}
		count = min( bytes_to_read, bytes_in_buffer );

		memcpy(	&pGif->ds_buff[9 - bytes_to_read],
						next_input_byte, count );
		next_input_byte += count;
		bytes_in_buffer -= count;
		bytes_to_read -= count;
	}

	/* We consume the heaser, set restart pointer */
	pGif->pDS->next_input_byte = next_input_byte;
	pGif->pDS->bytes_in_buffer = bytes_in_buffer;

	/* Set image dimension */
	pGif->frame.left   = gif_get16(&pGif->ds_buff[0]);
	pGif->frame.top    = gif_get16(&pGif->ds_buff[2]);
	pGif->frame.width  = gif_get16(&pGif->ds_buff[4]);
	pGif->frame.height = gif_get16(&pGif->ds_buff[6]);

	/* Assume if use global color table */
	pGif->frame.p_color_table = pGif->p_global_color;

	pGif->fsm_mainstate = S_IMGDATA_1;
//	pGif->fsm_substate = 0;

	pGif->fsm_scanline = 0;		/* Output scanline = 0 */

	flags = pGif->ds_buff[8];
	if ( flags & 0x40 )
		pGif->frame.flags |= GIF_INTERLACE;
	if ( flags & 0x20 )
		pGif->frame.flags |= GIF_SORTFLAG;
	if ( flags & 0x80 )
	{	/* Local color table */
		pGif->frame.flags |= GIF_COLORTBL;
		pGif->fsm_mainstate = S_LCOLOR;
		pGif->frame.palette_entries = 1 << ((pGif->ds_buff[8] & 0x07) + 1);     //add by sj
		pGif->frame.p_color_table = pGif->p_local_color;
	}
	return 1;
}

/* Process graphics control extension */

int GIF_GraphCtrlExt( struct gif_decoder * pGif )
{
	if ( pGif->block_size != 4 )
	{
		pGif->fsm_mainstate = S_ERROR;
		return -1;
	}

	pGif->frame.flags = 0;
	pGif->graphic_ext_flag = 1;
	if ( pGif->ds_buff[0] & 0x01 )
		pGif->frame.flags |= GIF_TRANSPARENT;
	pGif->frame.disposal_method = (pGif->ds_buff[0] >> 2) & 0x07;
	pGif->frame.delay_time = gif_get16( &pGif->ds_buff[1] );
	pGif->frame.transparent = pGif->ds_buff[3];
	return 1;
}

/* Process Plain Text extension */      //add by sj

int GIF_PlainTextExt( struct gif_decoder * pGif )
{
	if ( pGif->block_size != 12 )
	{
		pGif->fsm_mainstate = S_ERROR;
		return -1;
	}

	return 1;
}

int GIF_FSM_Trailer( struct gif_decoder * pGif )
{
	/* Just consume input data */
	pGif->pDS->next_input_byte += pGif->pDS->bytes_in_buffer;
	pGif->pDS->bytes_in_buffer = 0;
	return 1;
}

int GIF_FSM_Error( struct gif_decoder * pGif )
{
	/* Just consume input data */
	pGif->pDS->next_input_byte += pGif->pDS->bytes_in_buffer;
	pGif->pDS->bytes_in_buffer = 0;
	return -1;
}

int GIF_FSM_ImageData_1( struct gif_decoder * pGif );
int GIF_FSM_ImageData_2( struct gif_decoder * pGif );

int (*gif_fsm_func[])(struct gif_decoder * pGif) = {
	GIF_FSM_Header,
	GIF_FSM_GColor,
	GIF_FSM_Data,
	GIF_FSM_ImageDesc,
	GIF_FSM_LColor,
	GIF_FSM_ImageData_1,
	GIF_FSM_ImageData_2,
	GIF_FSM_Trailer,
	GIF_FSM_Error
};

int GIF_FSM_Ext(struct gif_decoder * pGif)
{
int	bytes_to_read, count, ret;
unsigned char * next_input_byte;
int	bytes_in_buffer;
unsigned char ext_label, blk_len;

	/* Read in extension code */
	if ( pGif->pDS->bytes_in_buffer == 0 )
	{
		ret = pGif->pDS->fill_input_buffer( pGif );
		if ( ret <= 0 )
			return ret;
	}

	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;

	ext_label = *next_input_byte ++;
	pGif->block_size = 0;
	bytes_in_buffer --;

	/* Read data in block by block until block length = 0 */
	for ( ; ; )
	{
		/* Get block length */
		if ( bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
			next_input_byte = pGif->pDS->next_input_byte;
			bytes_in_buffer = pGif->pDS->bytes_in_buffer;
		}

		blk_len = *next_input_byte ++;
		bytes_in_buffer --;
		if ( blk_len == 0 )
			break;

		/* Read data of this block into buffer */
		bytes_to_read = blk_len;
		pGif->block_size += blk_len;

		while ( bytes_to_read > 0 )
		{
			if ( bytes_in_buffer <= 0 )
			{
				ret = pGif->pDS->fill_input_buffer( pGif );
				if ( ret <= 0 )
					return ret;
				next_input_byte = pGif->pDS->next_input_byte;
				bytes_in_buffer = pGif->pDS->bytes_in_buffer;
			}
			count = min( bytes_to_read, bytes_in_buffer );

			memcpy(	&pGif->ds_buff[blk_len - bytes_to_read],
						next_input_byte, count );
			next_input_byte += count;
			bytes_in_buffer -= count;
			bytes_to_read -= count;
		}
	}

	/* We consume the heaser, set restart pointer */
	pGif->pDS->next_input_byte = next_input_byte;
	pGif->pDS->bytes_in_buffer = bytes_in_buffer;

//	pGif->fsm_substate  = 0;
	/* Process this extension block */
	if ( pGif->fsm_mainstate == S_DATA )
	{
		switch ( ext_label )
		{
		case 0xF9:
			ret = GIF_GraphCtrlExt( pGif );
			break;

        case 0x01:      //add by sj
			ret = GIF_PlainTextExt( pGif );
            break;

		default:
			ret = 1;
			break;
		}
	}
	return ret;
}


#if 0

int GIF_FSM_BlockLen( struct gif_decoder * pGif )
{
int	ret;
unsigned char i_code;

	/* Read size of block */
	if ( pGif->pDS->bytes_in_buffer == 0 )
	{
		ret = pGif->pDS->fill_input_buffer( pGif );
		if ( ret <= 0 )
			return ret;
	}
	i_code = *pGif->pDS->next_input_byte ++;
	pGif->pDS->bytes_in_buffer --;

	/* size of block read */
	if ( i_code == 0 )
	{	/* End of extension block */
		pGif->fsm_substate = S_BLOCK_TERM;
	}
	else
	{	/* change state to read block data */
		pGif->block_size = i_code;
		pGif->fsm_substate ++;
		pGif->bytes_read = 0;
	}
	return 1;
}

int GIF_FSM_BlockData( struct gif_decoder * pGif )
{
int	count, ret;

	while ( pGif->bytes_read < pGif->block_size )
	{	/* Read data into buffer */
		if ( pGif->pDS->bytes_in_buffer == 0 )
		{
			ret = pGif->pDS->fill_input_buffer( pGif );
			if ( ret <= 0 )
				return ret;
		}
		count = pGif->block_size - pGif->bytes_read;
		if ( count > pGif->pDS->bytes_in_buffer )
			count = pGif->pDS->bytes_in_buffer;

		memcpy( pGif->ds_buff + pGif->bytes_read, pGif->pDS->next_input_byte, count );
		pGif->bytes_read += count;
		pGif->pDS->bytes_in_buffer -= count;
		pGif->pDS->next_input_byte += count;
	}

	/* iF All data read, change data to read next block size */
	pGif->bytes_read = 0;
	pGif->fsm_substate --;
	
	/* Process the input data */
	if ( pGif->fsm_mainstate == S_DATA )
	{
		switch ( pGif->ext_label )
		{
		case 0xF9:
			ret = GIF_GraphCtrlExt( pGif );
			break;

        default:
			ret = 1;
			break;
		}
	}
	return ret;
}

int GIF_FSM_BlockTerm( struct gif_decoder * pGif )
{
	pGif->fsm_substate = 0;
	return 1;
}

int (*gif_fsm_subfunc[])(struct gif_decoder * pGif) = {
	NULL,
	GIF_FSM_Ext,
};

#endif

