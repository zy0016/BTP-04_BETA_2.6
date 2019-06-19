
#include	"malloc.h"
//#include	"memory.h"
#include    "string.h"
#include	"dgif.h"


void * gif_alloc_memory( PGIFOBJ pGif, int sizeofobject )
{
void * retval;

	retval = (void *)malloc(sizeofobject);
	return retval;
}

void gif_free_memory (PGIFOBJ pObject, void * memaddr, int sizeofobject)
{
	free ( memaddr );
}
