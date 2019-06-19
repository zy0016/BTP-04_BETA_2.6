
#include "super5.h"

//GAME5 game5;

static int Check(int x, int y, int color)
{
	int i, c[4];

	BB_Stat(game5.bb[color], x, y, c);

	for(i=0;i<4;i++)
		if(c[i]>=5) return 1;

	return 0;
}

void Game5_New()
{
	game5.winner = empty;
	game5.side = black;
	game5.steps = 0;
	BB_Init(game5.bb[0]);
	BB_Init(game5.bb[1]);
	BB_Init(game5.bb[2]);
}

int Game5_Put(int x, int y)
{
	int c;

	if(BB_Test(game5.bb[2], x, y))
		return empty;
	c = game5.side;
	BB_Set(game5.bb[c], x, y);
	BB_Set(game5.bb[2], x, y);
	game5.histroy[game5.steps++] = (y<<4 | x);
	if(Check(x, y, c))
		game5.winner = c;

	game5.side = !c;

	return c;
}

int Game5_Color(int x, int y)
{
	if(!BB_Test(game5.bb[2], x, y))
		return empty;
	else if(BB_Test(game5.bb[black], x, y))
		return black;
	else
		return white;
}

int Game5_Won()
{
	return game5.winner;
}

int Game5_Side()
{
	return game5.side;
}

int Game5_Undo(int steps)
{
	int i, x, y, c;

	if(steps>game5.steps)
		steps = game5.steps;
	if(steps<=0)
		return 0;

	for(i=1, c = game5.side; i<=steps; i++)
	{
		x = game5.histroy[game5.steps-i] & 0x0F;
		y = (game5.histroy[game5.steps-i] & 0xF0) >> 4;
		c = !c;
		BB_Clear(game5.bb[c], x, y);
		BB_Clear(game5.bb[2], x, y);
	}

	game5.steps -= steps;
	game5.side = c;
	game5.winner = empty;
	return game5.steps;
}
