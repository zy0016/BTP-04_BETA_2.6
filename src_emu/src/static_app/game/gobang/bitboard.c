

#include "super5.h"

void BB_Init(BITBOARD bb)
{
	int i;

	for(i=0;i<BOARD_SIZE;i++)
		bb[i]=0;
}

void BB_Set(BITBOARD bb, int x, int y)
{
	bb[y] |= (0x0001 << x);
}

int BB_Test(BITBOARD bb, int x, int y)
{
	return (bb[y] & (0x0001 << x));
}

void BB_Clear(BITBOARD bb, int x, int y)
{
	bb[y] &= ~(0x0001 << x);
}

int BB_IsEmpty(BITBOARD bb)
{
	int i;
	for(i=0;i<BOARD_SIZE;i++)
		if(bb[i]) return 0;

	return 1;
}

int BB_Match(BITBOARD bb1, BITBOARD bb2)
{
	register int i;
	for(i=0;i<BOARD_SIZE;i++)
		if(bb1[i] != bb2[i]) return 0;

	return 1;
}

void BB_And(BITBOARD r, BITBOARD bb1, BITBOARD bb2)
{
	register int i;
	for(i=0;i<BOARD_SIZE;i++)
		r[i] = bb1[i] & bb2[i];
}

void BB_Or(BITBOARD r, BITBOARD bb1, BITBOARD bb2)
{
	register int i;
	for(i=0;i<BOARD_SIZE;i++)
		r[i] = bb1[i] | bb2[i];
}

void BB_Not(BITBOARD r, BITBOARD bb)
{
	register int i;
	for(i=0;i<BOARD_SIZE;i++)
		r[i] = (~bb[i]) & 0x7FFF;
}

void BB_Stat(BITBOARD bb, int x, int y, int* stat)
{
	register int tx, ty;
	register unsigned short m;

//horizontal
	stat[0] = 1;
	m = 0x0001 << (x-1);
	for( tx = x-1; tx>=0; tx--, m >>= 1 )
		if( bb[y] & m )
			stat[0]++;
		else
			break;

	m = 0x0001 << (x+1);
	for( tx = x+1; tx<BOARD_SIZE; tx++, m <<= 1 )
		if( bb[y] & m )
			stat[0]++;
		else
			break;
//vertical
	stat[1] = 1;
	for( ty = y-1, m = 0x0001 << x; ty>=0; ty-- )
		if( bb[ty] & m )
			stat[1]++;
		else
			break;
	for( ty = y+1; ty<BOARD_SIZE; ty++ )
		if( bb[ty] & m )
			stat[1]++;
		else
			break;

// line \ //
	stat[2] = 1;
	m = 0x0001 << (x-1);
	for( tx = x-1, ty = y-1; tx>=0 && ty>=0; tx--, ty--, m >>= 1 )
		if( bb[ty] & m )
			stat[2]++;
		else
			break;
	m = 0x0001 << (x+1);
	for( tx = x+1, ty = y+1; tx<BOARD_SIZE && ty<BOARD_SIZE; tx++, ty++, m <<= 1 )
		if( bb[ty] & m )
			stat[2]++;
		else
			break;

// line / //
	stat[3] = 1;
	m = 0x0001 << (x-1);
	for( tx = x-1, ty = y+1; tx>=0 && ty<BOARD_SIZE; tx--, ty++, m >>= 1 )
		if( bb[ty] & m )
			stat[3]++;
		else
			break;
	m = 0x0001 << (x+1);
	for( tx = x+1, ty = y-1; tx<BOARD_SIZE && ty>=0; tx++, ty--, m <<= 1 )
		if( bb[ty] & m )
			stat[3]++;
		else
			break;

}

int BB_Fetch(BITBOARD bb, int *x, int *y)
{
	register int tx, ty;
	register unsigned short m;


	ty = *y;

	if( (bb[ty] & (0xFFFF << (*x+1)) ) == 0)
		ty ++;

	while(ty<BOARD_SIZE && bb[ty]==0)
	{
		ty++;
	}

	if(ty == BOARD_SIZE)
		return 0;

	if(ty==*y)
	{
		tx = *x + 1;
		m = 0x0001 << tx;
	}
	else
	{
		tx = 0;
		m = 0x0001;
	}

	while( (bb[ty] & m) == 0 )
	{
		m <<= 1;
		tx++;
	}

	*x = tx;
	*y = ty;

	return 1;
}
