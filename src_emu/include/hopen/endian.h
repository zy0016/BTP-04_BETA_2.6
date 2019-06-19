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
 * $Source: /cvs/hopencvs/src/include/hopen/endian.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/08/19 06:39:28 $
 * 
\**************************************************************************/

#ifndef _HOPEN_ENDIAN_H_
#define	_HOPEN_ENDIAN_H_

#define __LITTLE_ENDIAN	0x1234
#define __BIG_ENDIAN	0x4321

#define __swab16(x) ((((x) & 0x00ffU) << 8) | (((x) & 0xff00U) >> 8))

#define __swab32(x) ((((x) & 0x000000ffUL) << 24) | (((x) & 0x0000ff00UL) << 8) | \
		(((x) & 0x00ff0000UL) >> 8) | (((x) & 0xff000000UL) >> 24))

#define	swab16(x)	__swab16(x)
#define	swab32(x)	__swab32(x)

#define	swab16p(x)	__swab16(*(x))
#define	swab32p(x)	__swab32(*(x))

#define	swab16s(x) do { *(x) = __swab16p((x)); } while (0)
#define	swab32s(x) do { *(x) = __swab32p((x)); } while (0)

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define constant_htonl(x)	__swab32((x))
#define constant_ntohl(x)	__swab32((x))
#define constant_htons(x)	__swab16((x))
#define constant_ntohs(x)	__swab16((x))
#define cpu_to_le32(x)		((unsigned long)(x))
#define le32_to_cpu(x)		((unsigned long)(x))
#define cpu_to_le16(x)		((unsigned short)(x))
#define le16_to_cpu(x)		((unsigned short)(x))
#define cpu_to_be32(x)		__swab32((x))
#define be32_to_cpu(x)		__swab32((x))
#define cpu_to_be16(x)		__swab16((x))
#define be16_to_cpu(x)		__swab16((x))
#define cpu_to_le32p(x)		(*(unsigned long*)(x))
#define le32_to_cpup(x)		(*(unsigned long*)(x))
#define cpu_to_le16p(x)		(*(unsigned short*)(x))
#define le16_to_cpup(x)		(*(unsigned short*)(x))
#define cpu_to_be32p(x)		__swab32p((x))
#define be32_to_cpup(x)		__swab32p((x))
#define cpu_to_be16p(x)		__swab16p((x))
#define be16_to_cpup(x)		__swab16p((x))
#define cpu_to_le32s(x)		do {} while (0)
#define le32_to_cpus(x)		do {} while (0)
#define cpu_to_le16s(x)		do {} while (0)
#define le16_to_cpus(x)		do {} while (0)
#define cpu_to_be32s(x)		__swab32s((x))
#define be32_to_cpus(x)		__swab32s((x))
#define cpu_to_be16s(x)		__swab16s((x))
#define be16_to_cpus(x)		__swab16s((x))

#endif		// __LITTLE_ENDIAN

#if __BYTE_ORDER == __BIG_ENDIAN

#define constant_htonl(x)	((unsigned long)(x))
#define constant_ntohl(x)	((unsigned long)(x))
#define constant_htons(x)	((unsigned short)(x))
#define constant_ntohs(x)	((unsigned short)(x))
#define cpu_to_le32(x)		__swab32((x))
#define le32_to_cpu(x)		__swab32((x))
#define cpu_to_le16(x)		__swab16((x))
#define le16_to_cpu(x)		__swab16((x))
#define cpu_to_be32(x)		((unsigned long)(x))
#define be32_to_cpu(x)		((unsigned long)(x))
#define cpu_to_be16(x)		((unsigned short)(x))
#define be16_to_cpu(x)		((unsigned short)(x))
#define cpu_to_le32p(x)		__swab32p((x))
#define le32_to_cpup(x)		__swab32p((x))
#define cpu_to_le16p(x)		__swab16p((x))
#define le16_to_cpup(x)		__swab16p((x))
#define cpu_to_be32p(x)		(*(unsigned long*)(x))
#define be32_to_cpup(x)		(*(unsigned long*)(x))
#define cpu_to_be16p(x)		(*(unsigned short*)(x))
#define be16_to_cpup(x)		(*(unsigned short*)(x))
#define cpu_to_le32s(x)		__swab32s((x))
#define le32_to_cpus(x)		__swab32s((x))
#define cpu_to_le16s(x)		__swab16s((x))
#define le16_to_cpus(x)		__swab16s((x))
#define cpu_to_be32s(x)		do {} while (0)
#define be32_to_cpus(x)		do {} while (0)
#define cpu_to_be16s(x)		do {} while (0)
#define be16_to_cpus(x)		do {} while (0)

#endif	// __BIG_ENDIAN

#undef ntohl
#undef ntohs
#undef htonl
#undef htons

/*
 * Do the prototypes. Somebody might want to take the
 * address or some such sick thing..
 */
/*
extern unsigned long int	ntohl(unsigned long int);
extern unsigned short int	ntohs(unsigned short int);
extern unsigned long int	htonl(unsigned long int);
extern unsigned short int	htons(unsigned short int);
*/
/*modified by guyintang 10:55 2004-5-18*/
extern unsigned long	ntohl(unsigned long port);
extern unsigned short	ntohs(unsigned short port);
extern unsigned long	htonl(unsigned long port);
extern unsigned short	htons(unsigned short port);

#define ___htonl(x)	cpu_to_be32(x)
#define ___htons(x)	cpu_to_be16(x)
#define ___ntohl(x)	be32_to_cpu(x)
#define ___ntohs(x)	be16_to_cpu(x)

#define htonl(x)	((unsigned long)___htonl(x))
#define htons(x)	___htons(x)
#define ntohl(x)	((unsigned long)___ntohl(x))
#define ntohs(x)	___ntohs(x)

#endif	// _ENDIAN_H_
