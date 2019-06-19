/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#include <FCNTL.H>

#include <stdio.h>
#include <stdlib.h>
#include <MALLOC.H>
#include <string.h>
#include <window.h>
#include "elf.h"


#ifndef bool
#define bool int
#define true 0xffffffff
#define false 0
#endif

#define ELF_DESTINATION_ADDRESS	0x1900000
#define ELF_DESTINATION_SIZE	0x800000

void ErrorMessage (unsigned char* msg)
{
	printf ("\r\n") ;
	printf ((const char*)msg) ;
}

bool dlm_GetELFHeader (int elfFile, Elf32_Ehdr *elf_header)
{
	lseek (elfFile, 0, SEEK_SET) ;
	if (read (elfFile, elf_header, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr) )
	{
		ErrorMessage ("Read file header error!") ;
		return false ;
	}
		
	if (elf_header->e_ident[0] != 0x7F ||
		elf_header->e_ident[1] != 'E' ||
		elf_header->e_ident[2] != 'L' ||
		elf_header->e_ident[3] != 'F' ||
		elf_header->e_ident[4] != ELFCLASS32 ||
	    elf_header->e_ident[5] != ELFDATA2LSB ||
	    elf_header->e_ident[6] != EV_CURRENT ||
	    elf_header->e_machine != EM_BENEFON ||
	    elf_header->e_phnum > 1 
	    )
	{
		ErrorMessage ("Header Format error!") ;
		return false ;
	}
	if (elf_header->e_type != ET_EXEC && elf_header->e_type != ET_REL)
	{
		ErrorMessage ("Header Format not support error!") ;
		return false ;
	}
	return true ;	
}
bool dlm_GetProgramHeader (int elfFile, int offset, Elf32_Phdr *program_header)
{
	lseek (elfFile, offset, SEEK_SET) ;
	if (read(elfFile, program_header, sizeof(Elf32_Phdr))!=sizeof(Elf32_Phdr))
	{
		ErrorMessage ("Read program header error!") ;
		return false ;
	}
	return true ;
}

bool dlm_CheckElfFile (Elf32_Ehdr *elf_header, Elf32_Phdr *program_header)
{
	if (program_header->p_vaddr != ELF_DESTINATION_ADDRESS ||
		program_header->p_paddr != ELF_DESTINATION_ADDRESS ||
		program_header->p_memsz > ELF_DESTINATION_SIZE
		)
	{
		ErrorMessage ("Program header format error!") ;
		return false ;
	}
	return true ;
}

bool dlm_CreateImage (int elfFile, Elf32_Ehdr *elf_header, Elf32_Phdr *program_header)
{
	unsigned char *buf = 0 ;
	lseek(elfFile, program_header->p_offset, SEEK_SET) ;
	
	if (read(elfFile, (void *)program_header->p_paddr, program_header->p_filesz) 
		!= (int)program_header->p_filesz)
/*
	buf = malloc (program_header->p_filesz) ;
	if (buf==0)
	{
		ErrorMessage ("Alloc memory error!") ;
		return false ;
	}
	if (read(elfFile, (void *)buf, program_header->p_filesz) 
			!= (int)program_header->p_filesz)
*/	
	{
		ErrorMessage ("Create image error!") ;
		printf ("\r\np_addr: %x, p_filesz, %x",(int)program_header->p_paddr, (int)program_header->p_filesz) ;
		return false ;	
	}
	printf ("\r\np_addr: %x, p_filesz, %x",(int)program_header->p_paddr, (int)program_header->p_filesz) ;
//	free (buf) ;
	return true ;
}

int fd_mem = 0 ;
void * dlm_LoadFile (unsigned char* FileName)
{
	int fd = 0 ;

	Elf32_Ehdr elf_header ;
	Elf32_Phdr program_header ;
	bool ret ;
	
	fd_mem = open("/mnt/flash/info", O_RDWR) ;
	if (fd_mem<=0)
	{
		ErrorMessage ("error open memory!") ;
		return NULL ;
	}
	
	if (!FileName) {
		close (fd_mem) ;
		return NULL ;
	}

	fd = open(FileName, O_RDONLY,0) ;
	if (fd<=0)
	{
		ErrorMessage ("error open file!") ;
		close (fd_mem) ;
		return NULL ;
	}
	ret = dlm_GetELFHeader (fd, &elf_header) ;
	
	if (!ret)
	{
		ErrorMessage ("error header!") ;
		close (fd_mem) ;
		close (fd) ;
		return NULL ;
	}

	if (elf_header.e_phnum==1) 
	{
		ret = dlm_GetProgramHeader(fd, elf_header.e_phoff, &program_header) ;
		if (!ret)
		{
			ErrorMessage ("error pheader!") ;
			close (fd_mem) ;
			close (fd) ;
			return NULL ;
		}
		ret = dlm_CheckElfFile (&elf_header, &program_header) ;
		if (!ret)
		{
			ErrorMessage ("error check file!") ;
			close (fd_mem) ;
			close (fd) ;
			return NULL ;
		}
		if (ioctl(fd_mem, 1, program_header.p_filesz))
		{
			ErrorMessage ("mem size!") ;
			close (fd_mem) ;
			close (fd) ;
			return NULL ;

		}
		ret = dlm_CreateImage (fd, &elf_header, &program_header) ;
		if (!ret)
		{
			ioctl(fd_mem, 2, 0);
			close (fd_mem) ;
			close (fd) ;
			return NULL ;
		}
	}
	else
	{
		close (fd_mem) ;
		close (fd) ;
		return NULL ;
	}
	
	close (fd) ;
	printf ("\r\nload OK!") ;
	return  (void *)elf_header.e_entry;
}
typedef void (*ENTRY_POINT)(int arg1, int arg2) ;

struct tagStubTable{
	char *strFuncName;
	void *FuncAddr;
} st_table[] = {{"printf", printf}} ;

int main ()
{
	ENTRY_POINT entry_point ;
	entry_point = (ENTRY_POINT)dlm_LoadFile ("test.elf") ;
	if (entry_point)
	{
		printf("entry_point, %x", entry_point) ;
		entry_point ((int)st_table, sizeof(st_table)/sizeof(st_table[0])) ;
	}
	ioctl (fd_mem, 2, 0) ;
	close (fd_mem) ;
	return 0 ;
}


