 
//#include "stdio.h"

#include <window.h>
//#include <hpmm.h>
#include "winpda.h"

typedef int (*PROC)(int cmd, ...);


extern BOOL    WINAPI	SetColorScheme(PCOLORSCHEME pColScheme);
//extern BOOL WS_Init();
//int shell_main();

extern	BOOL InitPLXGUI( PROC );
extern  BOOL InitProgman();
extern  int  SetCurrentLanguage(int lang);
extern void	 printf(const char *, ...);

int	main( int argc, char **argv )
{
//	int i;
	if( InitPLXGUI(NULL) )
	{
		printf("\nPollex GUI init ok.");
	}
	else
	{
		printf("\nPollex GUI init error.");
		return -1;
	}

	printf("\n--Pollex Window Shell--\n");

	if( !InitProgman() )
	{
		printf("\nInit Progman error!\n");
		return -1;
	}

	printf("\n--Exit Window Shell-- ");

	return 0;
}
