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
 * $Source: /cvs/hopencvs/src/include/hopen/ndisapi.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/12/13 07:41:19 $
 * 
\**************************************************************************/

#ifndef _HOPEN_NDISAPI_H_
#define _HOPEN_NDISAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* return structure of listing every MAC module*/

struct macinfo {
	int		MACID;  
	char	MACname[16];    
	char	MACtype[16];
};

int NDIS_EnumMAC (int index, struct macinfo * pret);


struct mac_physaddr {
	char	MACAddr[16];
};

/* index = 0 unicast address,  index = others multicast address */
int NDIS_GetPhysAddr (int MACID, int index, struct mac_physaddr * pret);
int NDIS_SetPhysAddr (int MACID, int index, struct mac_physaddr * pdata);

/* get or set the protocol address of the device */

struct mac_protaddr {
	unsigned short unused;
	unsigned short addrlen;
	unsigned char addr[16];
	unsigned char mask[16];
};

int NDIS_GetProtAddr (int MACID, int index, struct mac_protaddr * pret);
int NDIS_SetProtAddr (int MACID, int index, struct mac_protaddr * pdata);

/* The input structure of starting a MAC device.*/

struct start_mac {
	int		filter;
};

int NDIS_StartMAC (int MACID, struct start_mac * pparm);

int NDIS_BindAndStart (int unused, void * pdata);

/**************************************************************
 *
 * interface of all the functions above. Implement later
 *
 **************************************************************
 */

int NDIS_Control (int code, void * param, int paramlen);

/**************************************************************
 *
 * get or set the work state of the network
 *
 * item include two parts: 
 *      high word is main project ID;
 *      low word is minor projict ID;
 *
 *   main project ID is used to select the program, minor project ID
 * is paramenter of the program
 *
 **************************************************************
 */

int NDIS_Get ( int item, int index, void * pret, int retlen );

int NDIS_Set ( int item, int index, void * pdata, int datalen );


#define	NDIS_CMD_BINDANDSTART	0x0001
#define	NDIS_CMD_STARTMAC		0x0002

#define	NDIS_CMD_GETPHYSADDR	0x0010
#define	NDIS_CMD_SETPHYSADDR	0x0011

#define	NDIS_CMD_GETPROTADDR	0x0012
#define	NDIS_CMD_SETPROTADDR	0x0013

#ifdef __cplusplus
}
#endif

#endif // _HOPEN_NDISAPI_H_
