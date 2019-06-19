#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#ifndef ABS
#define ABS(a) (a>0?a:-a)
#endif

/* Data type definition */
#ifndef UCHAR
#define UCHAR	unsigned char
#endif

#ifndef CHAR
#define CHAR	char
#endif

#ifndef ULONG
#define ULONG	unsigned long
#endif

#ifndef LONG
#define LONG	long
#endif

#ifndef UINT
#define UINT	unsigned int
#endif

#ifndef INT32
#define	INT32	long
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif

#ifndef DWORD
#define DWORD	unsigned long
#endif

#ifndef BYTE
#define BYTE	unsigned char
#endif

#ifndef PBYTE
#define PBYTE   unsigned char *
#endif

#ifndef PDWORD
#define PDWORD  unsigned long *
#endif

#ifndef PVOID
#define PVOID	void *
#endif

#ifndef PCHAR
#define PCHAR   char *
#endif

#ifndef SYSTEM_LOCK
#define SYSTEM_LOCK		DWORD
#endif

#ifndef TIMER_ID
#define TIMER_ID	DWORD
#endif

#ifndef NULL
#define NULL	0
#endif
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#ifdef CONFIG_IAR
#define ULONGLONG double
#define LONGLONG double
#else
#ifndef CONFIG_MICROSOFT
#define ULONGLONG unsigned long long
#define LONGLONG long long
#else
#define ULONGLONG unsigned __int64
#define LONGLONG __int64
#define BOOL	int
#endif
#endif

#ifndef BOOL
#define BOOL	UCHAR
#endif


#ifdef CONFIG_CPU_BIT32
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef INT16
#define INT16 short
#endif
#define PTR_BITS 2
#else
#ifdef WORD
#undef WORD
#endif
#define WORD unsigned short
#ifndef INT16
#define INT16 short
#endif
#define PTR_BITS 2 
#endif

#ifndef LPCSTR
#define LPCSTR 	const char *
#endif

#ifndef LPSTR
#define LPSTR 	char *
#endif

#ifndef LPVOID
#define LPVOID	void *
#endif

#ifndef LPDWORD
#define LPDWORD unsigned long *
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE -1
#endif

#ifndef HANDLE
//#define HANDLE void *
typedef void* HANDLE; 
#endif

#ifndef UUID_DEFINED
#define UUID_DEFINED
typedef struct _UUID
{
   DWORD Data1;
   WORD  Data2;
   WORD  Data3;
   BYTE  Data[8];
} UUID;
#endif //UUID_DEFINED

#define BDLENGTH		6
#define LINKKEYLENGTH	16
#define MAXPINLENGTH	16
#define DEVCLSLENGTH	3
#define MAX_NAME_LEN	64

#define BT_SUCCESS		1
#define BT_FAIL			0
#define INVALID_STATUS	-1

#endif
