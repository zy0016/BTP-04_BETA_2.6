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
 * $Source: /cvs/hopencvs/src/include/hopen/syscall.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2003/06/05 07:49:16 $
 * 
\**************************************************************************/

#ifndef _HOPEN_SYSCALL_H
#define _HOPEN_SYSCALL_H

#define SYS_GETSYSINFO			1
#define SYS_SETSYSINFO			2
#define SYS_GETTASKINFO			3
#define SYS_SETTASKINFO			4
#define SYS_GETPROCINFO			5
#define SYS_SETPROCINFO			6

#define SYS_PTRACE			7

#define SYS_CREATETASKEX		8
#define SYS_ENDTASK			9
#define SYS_DISABLESCHED		10
#define SYS_ENABLESCHED			11

#define SYS_SLEEP			12
#define SYS_SLEEPON			13
#define SYS_WAKEUP			14
#define SYS_SETITIMER			15

#define SYS_SIGACTION			16
#define SYS_SIGPROCMASK			17
#define SYS_RAISE			18
#define SYS_KILL			19
#define SYS_WAITPID			20
#define SYS_SIGRETURN			21
#define	SYS_SIGPENDING			22

#define SYS_BRK				23
#define SYS_GMEMALLOC			24
#define SYS_GMEMFREE			25
#define SYS_CREATEPROC			26
#define SYS_LOADIMAGE			27

/******************************************************************************
 *
 * File control commands.
 *
 ******************************************************************************
 */

#define SYS_PIPE			28
#define SYS_SYNC			29
#define SYS_MOUNT			30
#define SYS_UMOUNT			31

#define SYS_OPENDEV			32
#define SYS_CLOSE			33
#define SYS_POLL			34
#define SYS_DUP				35
#define SYS_DUP2			36
#define SYS_IOCTL			37
#define SYS_FCNTL			38
#define SYS_LSEEK			39
#define SYS_READ			40
#define SYS_WRITE			41
#define SYS_FSTAT			42
#define SYS_FSYNC			43

#define SYS_OPEN			44
#define SYS_GETDENTS		45
#define SYS_STAT			46
#define SYS_LSTAT			47
#define SYS_SYMLINK			48
#define SYS_LINK			49
#define SYS_UNLINK			50
#define SYS_MKNOD			51
#define SYS_MKDIR			52
#define SYS_RMDIR			53
#define SYS_RENAME			54
#define SYS_UTIMES			55
#define SYS_GETCWD			56
#define SYS_CHDIR			57
#define SYS_CHMOD			58
#define SYS_READLINK		59
#define SYS_FCHMOD			60

#define SYS_STATFS			61
#define SYS_FSTATFS			62
#define SYS_ACCESS			63

#define SYS_TRUNCATE		64
#define SYS_FTRUNCATE		65

#define SYS_MMAP			66
#define SYS_MUNMAP			67

 /******************************************************************************
 *
 * Network control commands.
 *
 ******************************************************************************
 */

#define SYS_SOCKET			96
#define	SYS_BIND			97
#define	SYS_LISTEN			98
#define	SYS_CONNECT			99
#define SYS_ACCEPT			100
#define	SYS_RECV			101
#define SYS_RECVFROM			102
#define	SYS_SEND			103
#define SYS_SENDTO			104
#define SYS_SETSOCKOPT			105
#define SYS_GETSOCKOPT			106
#define SYS_SOCKIOCTL			107
#define SYS_CLOSESOCKET			108
#define SYS_GETSOCKNAME			109
#define SYS_GETPEERNAME			110
#define SYS_SHUTDOWN			111

#endif
