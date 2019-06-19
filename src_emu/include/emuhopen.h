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

#ifdef __cplusplus
extern "C" {
#endif


#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	13
#define _IOC_DIRBITS	3

#define _IOC_NRMASK	((1L << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK	((1L << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK	((1L << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK	((1L << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

/*
 * Direction bits _IOC_NONE could be 0, but OSF/1 gives it a bit.
 * And this turns out useful to catch old ioctl numbers in header
 * files for us.
 */

#define _IOC_NONE	1L
#define _IOC_READ	2L
#define _IOC_WRITE	4L

#define _IOC(dir,type,nr,size) \
	(((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) | \
	 ((nr) << _IOC_NRSHIFT)  | ((size) << _IOC_SIZESHIFT))

/* used to create numbers */
#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0L)
#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(long)sizeof(size))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(long)sizeof(size))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(long)sizeof(size))

/* used to decode them.. */
#define _IOC_DIR(nr)		(((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)		(((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)		(((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)		(((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* various drivers, such as the pcmcia stuff, need these... */
#define IOC_IN			(_IOC_WRITE << _IOC_DIRSHIFT)
#define IOC_OUT			(_IOC_READ << _IOC_DIRSHIFT)
#define IOC_INOUT		((_IOC_WRITE|_IOC_READ) << _IOC_DIRSHIFT)
#define IOCSIZE_MASK		(_IOC_SIZEMASK << _IOC_SIZESHIFT)
#define IOCSIZE_SHIFT		(_IOC_SIZESHIFT)

#define	ROMDEV	    'R'

#define	ROM_GETBASEADDR	    _IO(ROMDEV, 1)
#define ROM_GETSIZE	    _IO(ROMDEV, 2)

#define ROM_SETBASEADDR	    _IO(ROMDEV, 3)
#define ROM_SETSIZE	    _IO(ROMDEV, 4)

#define	FATDEV	    'F'

#define	FAT_GETBASEADDR	    _IO(FATDEV, 1)
#define FAT_GETSIZE	    _IO(FATDEV, 2)

#define FAT_SETBASEADDR	    _IO(FATDEV, 3)
#define FAT_SETSIZE	    _IO(FATDEV, 4)

#define FAT_SETBLKSIZE	_IO(FATDEV, 5)
#define FAT_GETBLKSIZE	_IO(FATDEV, 6)

#define MINORBITS		16
#define MINORMASK		((1L << MINORBITS) - 1)

#define MAJOR(dev)		((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)		((unsigned int) ((dev) & MINORMASK))
#define HASHDEV(dev)		((unsigned int) (dev))
#define NODEV			0
#define MKDEV(ma,mi)		(((ma) << MINORBITS) | (mi))

int hopen_syscall (int cmd, ...);

_declspec (dllimport) int (*emu_fputs) (void * f, char * str, int count);
_declspec (dllimport) int (*emu_loadapp) (const char * name, void * pentry);
_declspec (dllimport) int (*emu_unloadapp) (const char * name);
_declspec (dllimport) struct vnet_operations ** vnet_table;

int emu_init_kernel (void * kmem_start, unsigned long kmem_size,
		int (**dev_init_table)(void), int (**fs_init_table)(void));
int emu_exit_kernel(void);

int _KERN_AddUserMemory (void * address, unsigned long size);
int do_mount (unsigned long dev, const char * dir_name,
		const char * fstype_name, int flags, void *data);

int KAPI_opendev (int major, int mirror, int flags);
int KAPI_open (const char * filename, int flags, int mode);
int KAPI_dup (int fd);
int KAPI_close (int fd);
int KAPI_read (int fd, char * buf, int count);
int KAPI_write (int fd, const char * buf, int count);
int KAPI_ioctl (int fd, unsigned long cmd, unsigned long arg);
int KAPI_getsysinfo (int item, unsigned long value);
int KAPI_setsysinfo (int item, unsigned long value);

int KAPI_loadimage (int mode, char * filename, void * envaddr, long envsize);

int NET_Init (void);
int TCPIP_Init (void);
int KAPI_sockioctl (int fd, int cmd, unsigned long * arg);

#ifdef __cplusplus
}
#endif
