/****************************************************************************\
 *
 * Cert - X509 certificate management.
 *
 * Authentication:   Hopen&Linux CA System
 * Copyright(c)2001 by CASS Corporation & Software Engineering Center
 * All Rights Reserved
 *
 * modification history
 * --------------------
 * 28Aug 01,    by Chen Xurui,	added routines
 * 05June01,	by Chen Xurui,	added routines
 * 04June01,	by Chen xurui,	created.
 ***************************************************************************/

#ifndef _cert_header_
#define _cert_header_

#ifdef	__cplusplus
   extern	"C" {
#endif

#include "rsaeuroc.h"

/* X.509 and CRL certificate handle */
typedef int	X509HD;
typedef int	CRLHD;    

/* public functions */

int	X509_Init();
int	X509_CRLInit();

X509HD X509_ParseCertificate( void* pHigh, char* pdata, int len, int* result );
CRLHD  X509_ReadCRLCertInfo(void* pHigh, char* pdata, int len, int* result );

/* enum all Certificates */
const X509HD X509_FindCers( void* pHigh );
X509HD X509_EnumCers( int index );
X509HD X509_GetCersHD( void* pHigh );
int    X509_CloseHandle( int index);

/* display certificate Info */
int	X509_GetIssuer( X509HD index, char* pName, int len );
int	X509_GetSubject( X509HD index, char* pName, int len );
int	X509_GetValidBefore( X509HD index, char* pName, int len );
int	X509_GetValidAfter( X509HD index, char* pName, int len );

/* X.509 certificate Verification */
int	X509_GetDigestAlgorithm(X509HD index, char *buffer, int len );
int	X509_GetPublicKey( X509HD index, R_RSA_PUBLIC_KEY* pPublicKey );
int	X509_GetTBSCertInfo(X509HD index, char *buffer, int len);
int	X509_GetCertSignature(X509HD index, char *buffer, int len);
int X509_VerifyCertSignature( X509HD index );  /* only support self signature */

#ifdef __cplusplus
}
#endif

#endif /* end cert_header */
