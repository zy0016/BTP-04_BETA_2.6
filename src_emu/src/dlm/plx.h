#ifndef PLX_H

#define MAX_FILENAME 32
#define IDSTRING_SIZE 32


#define ID_STRING "POLLEX Installation file V 0.01\0"

#define TRUNKSIZE 10240

typedef struct PLXInstallFile_
{
	unsigned char filename [MAX_FILENAME] ;
	unsigned long start_offset ; 		//from the beginning of this file)
	unsigned long compressed_size ;			//from the beginning of this file)
	unsigned long uncompressed_size ;
	unsigned long trunk_size ;
} PLXInstallFile, *PPLXInstallFile;

typedef struct _ProgmanInf
{
	unsigned int  thisLength ;
	unsigned int  plxVersion ;
	unsigned char Name[16] ;
	unsigned char Version[16] ;
	unsigned char Copyright[32] ;
	unsigned char unused[16] ;
} ProgmanInf, *PProgmanInf ;

#endif		// PLX_H

