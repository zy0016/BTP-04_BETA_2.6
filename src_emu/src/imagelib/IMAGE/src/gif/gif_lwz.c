
#include	"dgif.h"
#include	"gif_fsm.h"
//#include	"memory.h"
#include "string.h"

static const int code_mask[13] = {
     0,
     0x0001, 0x0003,
     0x0007, 0x000F,
     0x001F, 0x003F,
     0x007F, 0x00FF,
     0x01FF, 0x03FF,
     0x07FF, 0x0FFF
     };

static int GIF_ReadNextBlock( struct gif_decoder * pGif )
{
int	 count, bytes_to_read, ret;
BYTE * pTemp;
unsigned char * next_input_byte;
int	bytes_in_buffer;
unsigned char blk_len;
unsigned char * ptr;

	/* Copy currently unused bytes to start of buffer */
	if ( pGif->lwz.p_inptr != &pGif->ds_buff[0] )
	{
		count = (pGif->lwz.bit_count + pGif->lwz.bit_shift) >> 3;
		pTemp = &pGif->ds_buff[0];
		while ( count -- )
			* pTemp ++ = * pGif->lwz.p_inptr ++;
		pGif->lwz.p_inptr = &pGif->ds_buff[0];
	}

	if ( pGif->pDS->bytes_in_buffer <= 0 )
	{
		ret = pGif->pDS->fill_input_buffer( pGif );
		if ( ret <= 0 )
			return ret;
	}

	next_input_byte = pGif->pDS->next_input_byte;
	bytes_in_buffer = pGif->pDS->bytes_in_buffer;

	blk_len = *next_input_byte ++;
	bytes_in_buffer --;

	if ( blk_len == 0 )
	{	/* If end of data, change main state to S_DATA, I don't know
		 * if this is correct operation.
		 */
		pGif->fsm_mainstate = S_DATA;
		return 0;
	}

	bytes_to_read = blk_len;
	count = (pGif->lwz.bit_count + pGif->lwz.bit_shift) >> 3;
	ptr = &pGif->ds_buff[count];

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

	pGif->lwz.bit_count += (int)blk_len << 3;
	return 1;
}

/*********************************************************************
 *
 * Read a GIF code from data stream.
 *
 *********************************************************************
 */

static int GIF_GetNextCode( struct gif_decoder * pGif, int * pcode )
{
int	 ret, retcode;
int	 curr_size, bit_shift;
BYTE * pTemp;

	/* Put state variables into register for faster access */
	curr_size = pGif->lwz.curr_size;
	bit_shift = pGif->lwz.bit_shift;

	/* Normal case, total bit count great than current bit size */
	if ( pGif->lwz.bit_count < curr_size )
	{
		ret = GIF_ReadNextBlock( pGif );
		if ( ret <= 0 )
			return ret;
	}

	pTemp = pGif->lwz.p_inptr;
	/* Max is 3 bytes */
	retcode = *pTemp | ((int)(*(pTemp+1)) << 8) | ((int)(*(pTemp+2)) << 16);
	retcode >>= bit_shift;
	retcode &= pGif->lwz.code_mask;
	/* Modify counter */
	pGif->lwz.bit_count -= curr_size;
	bit_shift += curr_size;
	/* Modify byte pointer */
	pGif->lwz.p_inptr += (bit_shift >> 3);
	pGif->lwz.bit_shift = bit_shift & 7;
	/* return */
	*pcode = retcode;
	return 1;
}

/*********************************************************************
 *
 * Decode GIF input code stream. State 1, get initial code size and
 * do decode structure initialize.
 *
 *********************************************************************
 */

int GIF_FSM_ImageData_1( struct gif_decoder * pGif )
{
unsigned char i_size, * ptr;
int i, ret;

	/* Read in lwz start size */
	if ( pGif->pDS->bytes_in_buffer == 0 )
	{
		ret = pGif->pDS->fill_input_buffer( pGif );
		if ( ret <= 0 )
			return ret;
	}
	
	i_size = *pGif->pDS->next_input_byte ++;
	pGif->pDS->bytes_in_buffer --;

	/* size must between 1 and 12 */
	if ( i_size < 1 || i_size >= 12 )
	{
		pGif->fsm_mainstate = S_ERROR;
		return -1;
	}

	/* Clear structure to all zero */
	ptr = (char *)&pGif->lwz;
	for ( i = 0; i < sizeof(pGif->lwz); i ++ )
		*ptr ++ = 0;

	/* Initialize */
	pGif->lwz.size		= i_size;
	pGif->lwz.curr_size = i_size + 1;
	pGif->lwz.code_mask = code_mask[i_size + 1];
	pGif->lwz.top_slot	= 1 << pGif->lwz.curr_size;
	pGif->lwz.clear_code  = 1 << pGif->lwz.size;
	pGif->lwz.ending_code = pGif->lwz.clear_code + 1;
	pGif->lwz.slot = pGif->lwz.newcodes = pGif->lwz.ending_code + 1;

	/* Set up the stack pointer and decode buffer pointer */
	pGif->lwz.p_outptr	 = pGif->p_lwz_outbuf;
	pGif->lwz.p_inptr	 = &pGif->ds_buff[0];
	pGif->read_offset	 = 0;

//	pGif->fsm_substate = S_BLOCK_LEN;
	pGif->fsm_mainstate = S_IMGDATA_2;

	return S_IMGDATA_2;
}


/*********************************************************************
 *
 * Decode GIF input code stream.
 *
 *********************************************************************
 */
int GIF_FSM_ImageData_2( struct gif_decoder * pGif )
{
int  i_code, code, ret;
char * pstack, * outptr;
char stack[LWZ_MAX_CODES +1];

	/* Do lwz decode */
	outptr = pGif->lwz.p_outptr;

	while ( (int)outptr - (int)pGif->p_lwz_outbuf < (int)LWZ_MAX_CODES )
	{	/* Get next LWZ code */
		if ( (ret = GIF_GetNextCode(pGif, &i_code)) <= 0 )
			break;

		/* Check ending code */
		if ( i_code == pGif->lwz.ending_code )
		{
			pGif->ending_flag = 1;
			break;
		}

		/* If the code is a clear code, reinitialize all necessary items. */
		if ( i_code == pGif->lwz.clear_code )
		{
			pGif->lwz.state = 0;
			pGif->lwz.curr_size = pGif->lwz.size + 1;
			pGif->lwz.code_mask = code_mask[pGif->lwz.curr_size];
			pGif->lwz.slot = pGif->lwz.newcodes;
			pGif->lwz.top_slot = 1 << pGif->lwz.curr_size;
			continue;
		}
		
		/* Other codes */
		if ( pGif->lwz.state == 0 )
		{	/* 1st code after clear code, promote state */
			pGif->lwz.state ++;
			/* Finally, if the code is beyond the range of already set codes,
			* (This one had better NOT happen...  I have no idea what will
			* result from this, but I doubt it will look good...) then set it
			* to color zero.
			*/
			if ( i_code >= pGif->lwz.slot )
				i_code = 0;
				/* And let us not forget to put the char into the buffer... And
				* if, on the off chance, we were exactly one pixel from the end
				* of the line, we have to send the buffer to the out_line()
				* routine...
			*/
			pGif->lwz.oc = pGif->lwz.fc = i_code;
			*outptr ++ = (BYTE)i_code;
			continue;
		}
		
		/* In this case, it's not a clear code or an ending code, so
		* it must be a code code...  So we can now decode the code into
		* a stack of character codes. (Clear as mud, right?)
		*/
		
		/* Set stack pointer to initial value */
		pstack = &stack[0];

		/* Here we go again with one of those off chances...  If, on the
		* off chance, the code we got is beyond the range of those already
		* set up (Another thing which had better NOT happen...) we trick
		* the decoder into thinking it actually got the last code read.
		* (Hmmn... I'm not sure why this works...  But it does...)
		*/
		if ((code = i_code) >= pGif->lwz.slot)
		{
			if (code != pGif->lwz.slot)
			{
				++pGif->lwz.bad_code_count;
				i_code = code = 0;
			}
			else
			{
				code = pGif->lwz.oc;
				*pstack ++ = (BYTE)pGif->lwz.fc;
			}
		}

		/* Here we scan back along the linked list of prefixes, pushing
		 * helpless characters (ie. suffixes) onto the stack as we do so.
		 */
		while (code >= pGif->lwz.newcodes)
		{
			*pstack ++ = pGif->p_lwz_suffix[code];
			code = pGif->p_lwz_prefix[code];
		}
		
		/* Push the last character on the stack, and set up the new
		 * prefix and suffix, and if the required slot number is greater
		 * than that allowed by the current bit size, increase the bit
		 * size.  (NOTE - If we are all full, we *don't* save the new
		 * suffix and prefix...  I'm not certain if this is correct...
		 * it might be more proper to overwrite the last code...
		 */
		*pstack ++ = (BYTE)code;
		if (pGif->lwz.slot < pGif->lwz.top_slot)
		{
			pGif->lwz.fc = code;
			pGif->p_lwz_suffix[pGif->lwz.slot] = (BYTE)code;
			pGif->p_lwz_prefix[pGif->lwz.slot++] = pGif->lwz.oc;
			pGif->lwz.oc = i_code;
		}
		
		if (pGif->lwz.slot >= pGif->lwz.top_slot)
			if (pGif->lwz.curr_size < 12)
			{
				pGif->lwz.top_slot <<= 1;
				pGif->lwz.curr_size ++;
				pGif->lwz.code_mask = code_mask[pGif->lwz.curr_size];
			} 
			
		/* Now that we've pushed the decoded string (in reverse order)
		 * onto the stack, lets pop it off and put it into our decode
		 * buffer...  And when the decode buffer is full, write another
		 * line...
		 */
		while (pstack > &stack[0])
			* outptr ++ = *(--pstack);
	}

	if ( ret < 0 )
		return ret;

	if ( outptr == pGif->lwz.p_outptr )
		return 0;

	pGif->lwz.p_outptr = outptr;
	return 1;
}

