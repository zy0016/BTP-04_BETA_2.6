/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/sharelib.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/06/03 07:56:09 $
 * 
\**************************************************************************/

#ifndef _SHAREDLIB_H_
#define	_SHAREDLIB_H_

struct func_table{
	void* addr;
	char* name;
};

struct libinfo{
	int size;
	char name[16];
	unsigned short major;
	unsigned short minor;
	unsigned long date;
	void* start;
	const struct func_table * export;
	const char * info_string;
};

struct val_hlib{
				char* name;
				unsigned long version;
			};
struct val_libinfo{
				int handle;
				unsigned long addr;
				int size;
			};
struct val_linklib{
				int handle;
				int mode;
			};
			
struct val_funcinfo{
	int handle;
	char* name;
	void* addr;
	};

#include <sys/procenv.h>

#ifdef __cplusplus
extern "C" {
#endif

int LoadLibrary (char* name, unsigned long version, int mode);
int GetLibraryInfo (int handle, struct libinfo* libinfo, int size);
void* GetProcAddress (int handle, const char* name);
void SetProcEnvAddr (procenv* addr);
procenv* GetProcEnvAddr(void);

#ifdef __cplusplus
}
#endif

#endif	//_SHAREDLIB_H_
