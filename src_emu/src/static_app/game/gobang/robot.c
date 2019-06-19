
#include "super5.h"
#include "stdlib.h"
#include "string.h"

extern GAME5 game5;

#define MAX_CAND (BOARD_SIZE * BOARD_SIZE)

static BITBOARD bbCand;
static int broken;

static BYTE cands[MAX_CAND];
static int cand_count;
static int max_score;

#define PAT_COUNT	24
static int pat[PAT_COUNT][2]=
{
	{-1, -1},
	{-1, 0},
	{-1, 1},
	{-1, -2},
	{-1, 2},

	{0, -1},
	{0, 1},
	{0, -2},
	{0, 2},

	{1, -1},
	{1, 0},
	{1, 1},
	{1, -2},
	{1, 2},

	{-2, -2},
	{-2, -1},
	{-2, 0},
	{-2, 1},
	{-2, -2},

	{2, -2},
	{2, -1},
	{2, 0},
	{2, 1},
	{2, -2},


};


static int dir_pat[8][2] = 
{
	{1, 0}, 
	{1, 1}, 
	{0, 1}, 
	{-1, 1}, 
	{-1, 0}, 
	{-1, -1}, 
	{0, -1}, 
	{1, -1}, 
};

typedef struct tagPattern
{
	int len;
	int jump[2];
	int free[2];
}Pattern;

#define VALID_XY(x,y) ((x)>=0 && (y)>=0 && (x)<BOARD_SIZE && (y)<BOARD_SIZE)

int BB_Radial(BITBOARD bb, int x, int y, int *nx, int *ny, int dir)
{
	register int tx, ty;
	register unsigned short m;
	int len;

	len = 0;
	m = 0x0001 << x;
	tx = x + dir_pat[dir][0];
	ty = y + dir_pat[dir][1]; 
	while(VALID_XY(tx,ty))
	{
		if(dir_pat[dir][0]>0)
			m <<= 1;
		else if(dir_pat[dir][0]<0)
			m >>= 1;

		if( bb[ty] & m )
			len++;
		else
			break;
		tx += dir_pat[dir][0];
		ty += dir_pat[dir][1];
	}
	
	*nx = tx;
	*ny = ty;
	return len;
}

enum 
{
	ps_5 = 0,
	ps_f4 = 1, 
	ps_d4 = 2, 
	ps_j4 = 3, 
	ps_r4 = 4, 
	ps_f3 = 5, 
	ps_r3 = 6,
	ps_f2 = 7,
	ps_r2 = 8,
	ps_1 = 9,
	ps_dead = 10 
};

int attack_scores[11]=
{1000000000,	10000000,	10000000, 100000, 100000, 100000, 1000, 1000, 100, 100, 0};


int Pat_Style(Pattern* pat)
{
	if(pat->len >= 5)
		return ps_5;
	
	if(pat->len == 4)
	{
		if(pat->free[0]>0 && pat->free[1]>0)
			return ps_f4;
		else if(pat->free[0]>0 || pat->free[1]>0)
			return ps_r4;
	}

	if( (pat->free[0] + pat->free[1] + pat->len + pat->jump[0] + pat->jump[1]) < 5)
		return ps_dead;

	if( (pat->len + pat->jump[0])>=4 &&  (pat->len + pat->jump[1])>=4 )
		return ps_d4;

	if( (pat->len + pat->jump[0])>=4 ||  (pat->len + pat->jump[1])>=4 )
		return ps_j4;

	if( pat->len == 3 )
	{
		if( pat->free[0]>0 && pat->free[1]>0 && (pat->free[0] + pat->free[1])>2 )
			return ps_f3;
		else if( pat->free[0]>0 || pat->free[1]>0 )
			return ps_r3;
	}

	if( (pat->len + pat->jump[0] == 3) )
	{
		if( pat->free[0]>1 && pat->free[1]>0)
			return ps_f3;
		else
			return ps_r3;
	}

	if( (pat->len + pat->jump[1] == 3) )
	{
		if( pat->free[0]>0 && pat->free[1]>1)
			return ps_f3;
		else
			return ps_r3;
	}

	if( pat->len == 2 )
	{
		if( pat->free[0]>0 && pat->free[1]>0 && (pat->free[0] + pat->free[1])>3 )
			return ps_f2;
		else
			return ps_r2;
	}

	if( (pat->len + pat->jump[0] == 2) || (pat->len + pat->jump[1] == 2))
	{
		if( pat->free[0]>1 && pat->free[1]>1)
			return ps_f2;
		else
			return ps_r2;
	}

//	ASSERT((pat->len + pat->jump[0] + pat->jump[1] )== 1 );

	return ps_1;

}

void ScanPat(BITBOARD me, BITBOARD enemy, int x, int y, Pattern* pat)
{
	int dir, nx, ny, len, nPat,nFree;
	BITBOARD free;
	
	for(dir=0; dir < BOARD_SIZE; dir++)
	{
		free[dir] = (~(me[dir] | enemy[dir])) & 0x7FFF;
	}

	for(nPat=0;nPat<4;nPat++)
	{
		pat[nPat].free[0] = pat[nPat].free[1] = 0;
		pat[nPat].jump[0] = pat[nPat].jump[1] = 0;
		pat[nPat].len = 1;
	}

	for(dir=0; dir < 8; dir++)
	{
		if(dir >=4 )
		{
			nPat = dir-4;
			nFree = 1;
		}
		else
		{
			nPat = dir;
			nFree = 0;
		}

		len = BB_Radial(me, x, y, &nx, &ny, dir);
		pat[nPat].len += len;
		if(!VALID_XY(nx, ny) || BB_Test(enemy, nx, ny))
			pat[nPat].free[nFree] = 0;
		else
		{
			pat[nPat].free[nFree] = 1;
			len = BB_Radial(me, nx, ny, &nx, &ny, dir);
			pat[nPat].jump[nFree] = len;
			if(VALID_XY(nx, ny) && !BB_Test(enemy, nx, ny))
			{
				pat[nPat].free[nFree] += BB_Radial(free, nx, ny, &nx, &ny, dir)+1;
			}
			
		}
	}

}

int Val(int x, int y)
{
	int attack, defend, i, ps, c4, c3;
	Pattern pat[4];

	ScanPat(game5.bb[game5.side],game5.bb[!game5.side], x, y, pat);

	attack = 0;
	c4=c3=0;
	for(i=0;i<4;i++)
	{
		ps = Pat_Style(&pat[i]);
		attack += attack_scores[ps] /*+ pat[i].free[0] + pat[i].free[1]*/;
		if(ps == ps_f4 || ps == ps_j4 || ps == ps_r4)
			c4++;
		if(ps == ps_f3)
			c3++;
	}
	if((c4 >=2 ) || (c4==1 && c3>0))
		attack += attack_scores[ps_f4];
	else if(c3>1)
		attack += attack_scores[ps_f3];

	defend = 0;
	c4=c3=0;
	ScanPat(game5.bb[!game5.side],game5.bb[game5.side], x, y, pat);
	for(i=0;i<4;i++)
	{
		ps = Pat_Style(&pat[i]);
		defend += attack_scores[ps]/10 /*- pat[i].free[0] - pat[i].free[1]*/;
		if(ps == ps_f4 || ps == ps_j4 || ps == ps_r4)
			c4++;
		if(ps == ps_f3)
			c3++;
	}
	if((c4 >=2 ) || (c4==1 && c3>0))
		defend += attack_scores[ps_f4]/10;
	else if(c3>1)
		defend += attack_scores[ps_f3]/10;

	
	return attack + defend;
}

void ScanCands()
{
	int x,y,i;
	BITBOARD b, b1;


	BB_Init(b);
	BB_Init(b1);
	BB_Init(bbCand);

	x = -1;
	y = 0;
	while(BB_Fetch(game5.bb[2], &x, &y))
	{
		for(i=0;i<PAT_COUNT;i++)
			if(x + pat[i][0]>=0 && y + pat[i][1]>=0 && x + pat[i][0]<BOARD_SIZE && y + pat[i][1]<BOARD_SIZE)
				BB_Set(b, x + pat[i][0], y + pat[i][1]);
	}

	BB_Not(b1, game5.bb[2]);
	BB_And(bbCand, b, b1);

	x = -1;
	y = 0;
	max_score = 0;
	cand_count = 0;
	while(BB_Fetch(bbCand, &x, &y))
	{
		int score;

		score = Val(x, y);
		if(score < max_score)
			continue;

		if(score > max_score)
		{
			cand_count = 0;
			max_score = score;
		}

		cands[cand_count++] = ((x<<4) | y);
	}

}


int Game5_RobotPlay(int *x, int *y)
{
	int t;
	int i, j;

	if(game5.side == black && game5.steps == 0)
	{
		*x = 7;
		*y = 7;
		return 1;
	}

	broken = 0;
	memset(cands, 0, sizeof(cands));
	i=j=0;
	ScanCands();

	while(0)
	{
		if(broken)
			return 0;
	}

	if(cand_count == 0)
		return 0;
	
	srand(GetTickCount());
	if(cand_count>1)
		t = rand() % cand_count;
	else
		t = 0;

	*x = (cands[t] & 0xF0) >> 4 ;
	*y = cands[t] & 0x0F;
	return 1;
}

void Game5_RobotBreak()
{
	broken = 1;
}
