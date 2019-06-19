#ifndef SWRAPUI_H
#define SWRAPUI_H

#ifndef CONFIG_OS_WINCE
	#define MAX_COM_NUM		60
#endif
#ifdef CONFIG_OS_WINCE
	#define MAX_COM_NUM		10
#endif
struct ComMgrStru
{
	WORD			uuid;				/* service type				*/
	UCHAR			vport;				/* used to bond with comm	*/
	UCHAR			usage_type;			/* bluelet or bluetester	*/
	UCHAR			modem_type;			/* modem type				*/
	UCHAR			port_status;		/* port status				*/
	UCHAR			opencmpltevent;		/* use bool value to check	*/
	HANDLE			reqsendevent;
	HANDLE			cansendevent;
	HANDLE			threadexitevent;	/* Profile_SendThread event	*/
	struct BtList	*recvdatalist;
};

extern struct ComMgrStru ComObject[MAX_COM_NUM];

#define COM_USED				0x0001	/* used or spare			*/
#define COM_SVRSIDE				0x0002	/* client or server			*/
#define COM_OPEN				0x0040	/* open or close			*/
#define COM_CANRCV				0x0080	/* can receive				*/

#define COM_OPENNOTCMPLT		0x00	/* open port not completed	*/
#define COM_OPENCMPLT			0x01	/* open port completed		*/

#define CLR_PORT_STATUS(idx)	(ComObject[idx].port_status = 0)
#define SET_COM_USED(idx)		(ComObject[idx].port_status |= COM_USED)
#define SET_COM_SPARE(idx)		(ComObject[idx].port_status &= ~COM_USED)
#define SET_COM_SVRSIDE(idx)	(ComObject[idx].port_status |= COM_SVRSIDE)
#define SET_COM_CLNTSIDE(idx)	(ComObject[idx].port_status &= ~COM_SVRSIDE)
#define CLR_COM_VPORT(idx) 		(ComObject[idx].vport = 0)
#define SET_COM_OPEN(idx)		(ComObject[idx].port_status |= COM_OPEN)
#define SET_COM_CLOSE(idx)		(ComObject[idx].port_status &= ~COM_OPEN)

#define IS_COM_USED(idx)		(ComObject[idx].port_status & COM_USED)
#define IS_COM_SVRSIDE(idx)		(ComObject[idx].port_status & COM_SVRSIDE)
#define IS_VALID_COM_VPORT(idx) (ComObject[idx].vport != 0)
#define IS_COM_OPEN(idx)		(ComObject[idx].port_status & COM_OPEN)

#define COM_OPENCMPLTEVENT(idx)	(ComObject[idx].opencmpltevent)
#define COM_REQSENDEVENT(idx)	(ComObject[idx].reqsendevent)
#define COM_CANSENDEVENT(idx)	(ComObject[idx].cansendevent)
#define COM_RECVDATALIST(idx)	(ComObject[idx].recvdatalist)
#define COM_VPORT(idx)			(ComObject[idx].vport)
#define COM_THREADEXITEVENT(idx)	(ComObject[idx].threadexitevent)

#define COM_TYPE_BLUELET		1
#define COM_TYPE_TESTER			2

#define MODEM_TYPE_NON			0
#define MODEM_TYPE_DUN			1
#define MODEM_TYPE_FAX			2
#define MODEM_TYPE_LAP			3
#define MODEM_TYPE_LAPCLT		4
#define MODEM_TYPE_LAPSVR		5

#include "swrapuifunc.h"

#ifdef CONFIG_OS_WIN32 
#include "btwin32/swrapwin32.h"
extern DWORD WindowsVer;
#endif

#endif
