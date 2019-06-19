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

/******************************************************************************\
 *
 *	Purpose   Cryptography for Secure Socket Layer(SSL Ver1.0/2.0/3.0)
 *
 *	This header file is used to launch the RSAEURO.H header file.
 *	Provided for compatiblity with RSAREF.
 *
\***************************************************************************/

#ifndef	_rsaeuro_h_
#define	_rsaeuro_h_

#include "string.h"

#ifdef _CA_WINDEBUG_
	#include <windows.h>
	#include <stdio.h>	
#else
	#include "window.h"
	#include "hpdef.h"	
#endif

#ifdef	__cplusplus
   extern	"C" {
#endif

/* PROTOTYPES should be set to one if and only if the compiler supports
 * function argument prototyping.

 * The following makes PROTOTYPES default to 1 if it has not already been
 * defined as 0 with C compiler flags. */

#if !defined( PROTOTYPES )
   #define PROTOTYPES           1
#endif

typedef unsigned char *			POINTER;             //a pointer type
typedef unsigned short int		UINT2;               //a two byte word
typedef unsigned long  int		UINT4;               //a four byte word
typedef unsigned char			BYTE;

// Types for Nature number and prime number
typedef unsigned long  int		NN_DIGIT;
typedef unsigned short int		NN_HALF_DIGIT;

#if PROTOTYPES
    #define PROTO_LIST(list)    list
#else
    #define PROTO_LIST(list)    ()
#endif

////Contansts////////////////////////////////////////////////////////////////

/* Message-digest algorithms. */

#define	DA_MD2				2
#define	DA_MD4				4
#define	DA_MD5				5
#define	DA_SHS				3

/* Encryption algorithms to be ored with digest algorithm in Seal and Open. */

#define	EA_DES_CBC			1
#define	EA_DES_EDE2_CBC		2
#define	EA_DES_EDE3_CBC		3
#define	EA_DESX_CBC			4

/* The SHA1 block size and message digest sizes, in bytes. */

#define	SHS_BLOCKSIZE                    64
#define	SHS_DIGESTSIZE                   20

/* The default and maximum (sane) number of RC5 rounds */

#define RC5_8_ROUNDS	8
#define RC5_12_ROUNDS	12
#define RC5_16_ROUNDS	16

/* RSA key lengths. */

#define	MIN_RSA_MODULUS_BITS             508

/* PGP(2.6.1): Now allows 2048-bit keys changing below will allow this.
 * It does lengthen key generation slightly if the value is increased. */

#define	MAX_RSA_MODULUS_BITS             1024
#define	MAX_RSA_MODULUS_LEN              ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define	MAX_RSA_PRIME_BITS               ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define	MAX_RSA_PRIME_LEN                ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Maximum length of Diffie-Hellman parameters. */

#define	DH_PRIME_LEN(bits)				 (((bits) + 7) / 8)

/* Maximum lengths of encoded and encrypted content, as a function of
 * content length len. Also, inverse functions. */

#define	ENCODED_CONTENT_LEN(len)          (4*(len)/3 + 3)
#define	ENCRYPTED_CONTENT_LEN(len)        ENCODED_CONTENT_LEN ((len)+8)
#define	DECODED_CONTENT_LEN(len)          (3*(len)/4 + 1)
#define	DECRYPTED_CONTENT_LEN(len)        (DECODED_CONTENT_LEN (len) - 1)


/* Maximum lengths of signatures, encrypted keys, encrypted
	 signatures, and message digests. */

#define	MAX_SIGNATURE_LEN MAX_RSA_MODULUS_LEN
#define	MAX_PEM_SIGNATURE_LEN ENCODED_CONTENT_LEN(MAX_SIGNATURE_LEN)
#define	MAX_ENCRYPTED_KEY_LEN MAX_RSA_MODULUS_LEN
#define	MAX_PEM_ENCRYPTED_KEY_LEN ENCODED_CONTENT_LEN(MAX_ENCRYPTED_KEY_LEN)
#define	MAX_PEM_ENCRYPTED_SIGNATURE_LEN ENCRYPTED_CONTENT_LEN(MAX_SIGNATURE_LEN)
#define	MAX_DIGEST_LEN 20


/* Error codes,  identical with RSA BSAFE. */
/* Error codes. */

#define	RE_CONTENT_ENCODING			0x0400
#define	RE_DATA						0x0401
#define	RE_DIGEST_ALGORITHM			0x0402
#define	RE_ENCODING					0x0403
#define	RE_KEY						0x0404
#define	RE_KEY_ENCODING				0x0405
#define	RE_LEN						0x0406
#define	RE_MODULUS_LEN				0x0407
#define	RE_NEED_RANDOM				0x0408
#define	RE_PRIVATE_KEY				0x0409
#define	RE_PUBLIC_KEY				0x040a
#define	RE_SIGNATURE				0x040b
#define	RE_SIGNATURE_ENCODING		0x040c
#define	RE_ENCRYPTION_ALGORITHM		0x040d
#define	RE_FILE						0x040e

/* Library details. */

#define	RSAEURO_VER_MAJ                   1
#define	RSAEURO_VER_MIN                   0
#define	RSAEURO_IDENT                     "Crypto-C"
#define	RSAEURO_DATE                      "2000/10/30"

/* ID_OK and ID_ERROR. */
#ifndef ID_OK
	#define	ID_OK                         0
	#define	ID_ERROR                      1
#endif

/* Internal defs. */
#ifndef TRUE
	#define	FALSE                         0
	#define	TRUE                 ( !FASLSE)
#endif

/* Block cipher modes */
typedef enum 
{									
	CRYPT_MODE_NONE,				/* No encryption mode */
	CRYPT_MODE_ECB,					/* ECB */
	CRYPT_MODE_CBC,					/* CBC */
	CRYPT_MODE_CFB,					/* CFB */
	CRYPT_MODE_OFB,					/* OFB */
	CRYPT_MODE_LAST,				/* Last possible crypt mode value */
}CRYPT_MODE;

////Data Types/////////////////////////////////////////////////////////////////

//Random structure.

typedef struct
{
    unsigned int  bytesNeeded;                       /* seed bytes required */
    unsigned char state[16];                             /* state of object */
    unsigned int  outputAvailable;                 /* number byte available */
    unsigned char output[16];                               /* output bytes */
}R_RANDOM_STRUCT;

// MD2 context

typedef struct
{
	unsigned char state[16];                                       /* state */
    unsigned char checksum[16];                                 /* checksum */
    unsigned int count;                       /* number of bytes, modulo 16 */
    unsigned char buffer[16];                               /* input buffer */
} MD2_CTX;

// MD4 context.

typedef struct
{
	UINT4 state[4];                                          /* state (ABCD) */
	UINT4 count[2];               /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                                /* input buffer */
} MD4_CTX;

// MD5 context.

typedef struct
{
	UINT4 state[4];                                         /* state (ABCD) */
	UINT4 count[2];              /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                               /* input buffer */
}MD5_CTX;

/* Secure Hash Algorithm(SHA) follows Secure Hash Standards(SHS) */

// The structure for storing SHS info

typedef struct
{
	UINT4 digest [5];                                    /* Message digest */
	UINT4 countLo, countHi;                            /* 64-bit bit count */
	UINT4 data [16];                                    /* SHS data buffer */
}SHS_CTX;

// The RC2_CBC_CTX structure

typedef struct
{
	UINT2 key[64];                                   /* rc2 expanding-key */
	UINT4 iv[2];                                   /* initializing vector */
    UINT4 originalIV[2];                    /* for restarting the context */
	int encrypt;                                          /* encrypt flag */
}RC2_CBC_CTX;

//===================== Advices ============================================
//*
//* To x and y, using int types instead of char types make the structure
//* larger but make the code faster.
//*
//* On most boxes I have tested -up to %20 faster.
//*
//* improved
//*
//===========================================================================

typedef struct RC4_KEY
{
	unsigned char x;                                          /* counter x */
    unsigned char y;                                          /* counter y */
    unsigned char state[256];                 /* an S-BOX with 256 numbers */
}RC4_KEY;

//DES_CBC structure

typedef struct
{
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES_CBC_CTX;

//DESX_CBC structure

typedef struct
{
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 inputWhitener[2];                                 /* input whitener */
  UINT4 outputWhitener[2];                               /* output whitener */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DESX_CBC_CTX;

//DES3_CBC structure

typedef struct
{
  UINT4 subkeys[3][32];                     /* subkeys for three operations */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES3_CBC_CTX;

//RSA prototype key

typedef struct
{
  unsigned int bits;                          /* length in bits of modulus */
  int useFermat4;                       /* public exponent (1 = F4, 0 = 3) */
} R_RSA_PROTO_KEY;

// Diffie-Hellman parameters.

typedef struct
{
  unsigned char *prime;                                           /* prime */
  unsigned int primeLen;                                /* length of prime */
  unsigned char *generator;                                   /* generator */
  unsigned int generatorLen;                        /* length of generator */
} R_DH_PARAMS;

//RSA public and private key.

typedef struct
{
  unsigned short int bits;                     /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];           /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct
{
  unsigned short int bits;                     /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN];     /* public exponent */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */
} R_RSA_PRIVATE_KEY;

/* digest algorithm context */

typedef struct
{
  int digestAlgorithm;                         /* digest type */
  union {                                      /* digest sub-context */
		MD2_CTX md2;
		MD4_CTX md4;
		MD5_CTX md5;
		SHS_CTX shs;
	} context;
} R_DIGEST_CTX;

/* signature context */

typedef struct
{
	R_DIGEST_CTX digestContext;
} R_SIGNATURE_CTX;

/* envelope context */

typedef struct
{
  int encryptionAlgorithm;                       /* encryption type */
  union {                                      /* encryption sub-context */
		DES_CBC_CTX des;
		DES3_CBC_CTX des3;
		DESX_CBC_CTX desx;
  } cipherContext;

  unsigned char buffer[8];                       /* data buffer */
  unsigned int bufferLen;                      /* buffer length */
} R_ENVELOPE_CTX;


#define CRYPT_MAX_KEYSIZE		256		/* The maximum user key size - 2048 bits */
#define CRYPT_MAX_IVSIZE		32		/* The maximum IV size - 256 bits */

/* General algorithm information */
typedef struct {		
	CRYPT_MODE mode;					/* Encryption mode being used */

	/* User keying information for.  The user key is the key as entered 
	 * by the user, the IV is the initial IV 
	 */
	BYTE userKey[ CRYPT_MAX_KEYSIZE ];	/* User encryption key */
	int userKeyLength;					/* User encryption key length in bytes */

	BYTE iv[ CRYPT_MAX_IVSIZE ];		/* Initial IV */
	int ivLength;						/* IV length in bytes */

	/* Conventional encryption keying information.  The key is the raw
	 * encryption key stored in whatever form is required by the algorithm,
	 * usually the key-scheduled user key.  The IV is the current working IV.
	 * The ivCount is the number of bytes of IV which have been used, and is
	 * used when a block cipher is used as a stream cipher 
	 */
	void *key;							/* Internal working key */
	BYTE currentIV[ CRYPT_MAX_IVSIZE ];	/* Internal working IV */
	int keyLength;						/* Internal key length in bytes */

}CRYPT_INFO;

//// Message-digest algorithms.////////////////////////////////////////////////

void MD2Init   PROTO_LIST ((MD2_CTX *));
void MD2Update PROTO_LIST ((MD2_CTX *, unsigned char *, unsigned int));
void MD2Final  PROTO_LIST ((unsigned char [16], MD2_CTX *));

void MD4Init   PROTO_LIST ((MD4_CTX *));
void MD4Update PROTO_LIST ((MD4_CTX *, unsigned char *, unsigned int));
void MD4Final  PROTO_LIST ((unsigned char [16], MD4_CTX *));

void MD5Init   PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST ((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final  PROTO_LIST ((unsigned char [16], MD5_CTX *));

void SHSInit   PROTO_LIST ((SHS_CTX *));
void SHSUpdate PROTO_LIST ((SHS_CTX *, unsigned char *, int ));
void SHSFinal  PROTO_LIST ((unsigned char [20], SHS_CTX *));

//================= SYMMETRY ALGORITHMS(RC2, RC4, DES, DESX, 3DES).================
//
//The following functions can be used to OFB, ECB and CBC, if necessary.
//
//void rc2_keyschedule (unsigned short xkey[64], const unsigned char *key,
//						 unsigned int length, int bits );
//void rc2_encryptblock( unsigned short xkey[64], unsigned char *block );
//void rc2_decryptblock( unsigned short xkey[64], unsigned char *block );
//
//=================================================================================

void RC2_CBCInit    PROTO_LIST(( RC2_CBC_CTX *context, const unsigned char *key,
	                             unsigned int len_of_key, unsigned char *iv,
								 int encrypt, int bits));

int  RC2_CBCUpdate  PROTO_LIST(( RC2_CBC_CTX *context, unsigned char *output,
	                             unsigned char *input, unsigned int len));
void RC2_CBCRestart PROTO_LIST(( RC2_CBC_CTX *context));

/////
void RC4keyInit PROTO_LIST ((RC4_KEY *rc4key, int len, unsigned char *userkey));
void RC4Decrypt PROTO_LIST ((RC4_KEY *rc4key, int len, unsigned char *indata,
							 unsigned char*outdata));
void RC4Encrypt PROTO_LIST ((RC4_KEY *rc4key, int len, unsigned char *indata,
							 unsigned char*outdata));
////
int rc5Init( CRYPT_INFO *cryptInfo );
int rc5InitKey( CRYPT_INFO *cryptInfo, const void *key, const int keyLength, 
			    unsigned char *iv, const int ivLength, const int rounds );
int rc5EncryptCBC( CRYPT_INFO *cryptInfo, BYTE *buffer, int noBytes );
int rc5DecryptCBC( CRYPT_INFO *cryptInfo, BYTE *buffer, int noBytes );
int rc5End( CRYPT_INFO *cryptInfo );

////
void DES_CBCInit     PROTO_LIST ((DES_CBC_CTX *, unsigned char *, unsigned char *,
								 int));
int  DES_CBCUpdate   PROTO_LIST ((DES_CBC_CTX *, unsigned char *, unsigned char *,
								 unsigned int));
void DES_CBCRestart  PROTO_LIST ((DES_CBC_CTX *));

////
void DESX_CBCInit    PROTO_LIST ((DESX_CBC_CTX *, unsigned char *, unsigned char *,
								 int));
int  DESX_CBCUpdate  PROTO_LIST ((DESX_CBC_CTX *, unsigned char *, unsigned char *,
								 unsigned int));
void DESX_CBCRestart PROTO_LIST ((DESX_CBC_CTX *));

////
void DES3_CBCInit    PROTO_LIST ((DES3_CBC_CTX *, unsigned char *, unsigned char *,
								 int));
int  DES3_CBCUpdate  PROTO_LIST ((DES3_CBC_CTX *, unsigned char *, unsigned char *,
								 unsigned int));
void DES3_CBCRestart PROTO_LIST ((DES3_CBC_CTX *));

//================ BASE64 PEM encoding and decoding generation. ======================

int R_EncodePEMBlock( unsigned char *encodedBlock, unsigned int *encodedBlockLen,
						  unsigned char *block, unsigned int blockLen );

int R_DecodePEMBlock ( unsigned char *outbuf, unsigned int *outlength,
						   unsigned char *inbuf, unsigned int inlength );

//===================Random Structures Routines.======================================

int  R_RandomInit    PROTO_LIST ((R_RANDOM_STRUCT *));
int  R_RandomUpdate  PROTO_LIST ((R_RANDOM_STRUCT *, unsigned char *, unsigned int));
void R_RandomFinal   PROTO_LIST ((R_RANDOM_STRUCT *));
void R_RandomCreate  PROTO_LIST ((R_RANDOM_STRUCT *));
void R_RandomMix     PROTO_LIST ((R_RANDOM_STRUCT *));
int  R_GetRandomBytesNeeded  PROTO_LIST ((unsigned int *, R_RANDOM_STRUCT *));
int  R_GenerateBytes PROTO_LIST ((unsigned char *block, unsigned int len,
					              R_RANDOM_STRUCT *random));

//===================Prime for key generate ==========================================

int  GeneratePrime   PROTO_LIST ( (NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, NN_DIGIT *,
							       unsigned int, R_RANDOM_STRUCT *));


//===================RSA Key-pair generation.=========================================

int RSA_GeneratePEMKeys PROTO_LIST ((R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *,
	                               R_RSA_PROTO_KEY *, R_RANDOM_STRUCT *));


//==================RSA encryption and decryption functions. =========================

int RSAPublicEncrypt PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *,
								  unsigned int, R_RSA_PUBLIC_KEY *, R_RANDOM_STRUCT *));
int RSAPrivateEncrypt PROTO_LIST((unsigned char *, unsigned int *, unsigned char *,
								  unsigned int, R_RSA_PRIVATE_KEY *));
int RSAPublicDecrypt PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *,
								  unsigned int, R_RSA_PUBLIC_KEY *));
int RSAPrivateDecrypt PROTO_LIST((unsigned char *, unsigned int *, unsigned char *,
								  unsigned int, R_RSA_PRIVATE_KEY *));

//=============Diffie-Hellman key exchange functions =================================

int DH_GenerateParams( R_DH_PARAMS	*params, unsigned int primeBits,
					    unsigned int subPrimeBits, R_RANDOM_STRUCT *randomStruct );

int DH_SetupAgreement( unsigned char *publicValue, unsigned char *privateValue,
					   unsigned int privateValueLen, R_DH_PARAMS *params,
						R_RANDOM_STRUCT *randomStruct );

int DH_ComputeAgreedKey( unsigned char *agreedKey, unsigned char *otherPublicValue,
						  unsigned char *privateValue, unsigned int privateValueLen,
						  R_DH_PARAMS * params );

//--------------------------------------------------------------------------------

/* Cryptographic procedures. */

int R_DigestInit PROTO_LIST ((R_DIGEST_CTX *, int));
int R_DigestUpdate PROTO_LIST ((R_DIGEST_CTX *, unsigned char *, unsigned int));
int R_DigestFinal PROTO_LIST ((R_DIGEST_CTX *, unsigned char *, unsigned int *));

//----
int R_SignInit PROTO_LIST ((R_SIGNATURE_CTX *, int));
int R_SignUpdate PROTO_LIST ((R_SIGNATURE_CTX *, unsigned char *, unsigned int));
int R_SignFinal PROTO_LIST ((R_SIGNATURE_CTX *, unsigned char *, unsigned int *, R_RSA_PRIVATE_KEY *));

//----
int R_VerifyInit PROTO_LIST ((R_SIGNATURE_CTX *, int));
int R_VerifyUpdate PROTO_LIST ((R_SIGNATURE_CTX *, unsigned char *, unsigned int));
int R_VerifyFinal PROTO_LIST ((R_SIGNATURE_CTX *, unsigned char *, unsigned int, R_RSA_PUBLIC_KEY *));

//----
int R_SealInit PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char **, unsigned int *, unsigned char [8],
							unsigned int, R_RSA_PUBLIC_KEY **, int, R_RANDOM_STRUCT *));
int R_SealUpdate PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,
							  unsigned int *, unsigned char *, unsigned int));
int R_SealFinal PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,	unsigned int *));

//----
int R_OpenInit PROTO_LIST ((R_ENVELOPE_CTX *, int, unsigned char *,
							unsigned int, unsigned char [8], R_RSA_PRIVATE_KEY *));
int R_OpenUpdate PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,
								unsigned int *, unsigned char *, unsigned int));
int R_OpenFinal PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,	unsigned int *));

//--------------------------------------------------------------------------------

/* Cryptographic enhancements. */

int R_SignPEMBlock PROTO_LIST (( unsigned char *,  unsigned int *,  unsigned char *, unsigned int *,
					unsigned char *,  unsigned int,  int,  int,  R_RSA_PRIVATE_KEY *));
int R_SignBlock PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int, int,
							R_RSA_PRIVATE_KEY *));
//-----
int R_VerifyPEMSignature PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
					unsigned char *, unsigned int, int, int, R_RSA_PUBLIC_KEY *));
int R_VerifyBlockSignature PROTO_LIST ((unsigned char *, unsigned int,
					unsigned char *, unsigned int, int, R_RSA_PUBLIC_KEY *));
//-----
int R_SealPEMBlock PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int *,
					unsigned char *, unsigned int *, unsigned char [8], unsigned char *,
					unsigned int, int, R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *,
					R_RANDOM_STRUCT *));
int R_OpenPEMBlock PROTO_LIST ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
					unsigned char *, unsigned int, unsigned char *, unsigned int,
					unsigned char [8], int, R_RSA_PRIVATE_KEY *, R_RSA_PUBLIC_KEY *));
//-----
int R_DigestBlock PROTO_LIST ((unsigned char *, unsigned int *,
					unsigned char *, unsigned int, int));

//-----
int R_DecryptOpenPEMBlock PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,
					unsigned int *, unsigned char *, unsigned int));
int R_EncryptOpenPEMBlock PROTO_LIST ((R_ENVELOPE_CTX *, unsigned char *,
					unsigned int *, unsigned char *, unsigned int));

//=============Standard library routines.=============================================

#ifdef USE_ANSI
    #define	R_memset(x, y, z)		memset(x, y, z)
    #define	R_memcpy(x, y, z)		memcpy(x, y, z)
    #define	R_memcmp(x, y, z)		memcmp(x, y, z)
#else
    void R_memset	PROTO_LIST ((POINTER, int, unsigned int));
    void R_memcpy	PROTO_LIST ((POINTER, POINTER, unsigned int));
    int  R_memcmp	PROTO_LIST ((POINTER, POINTER, unsigned int));
#endif

#ifdef __cplusplus
}
#endif

#endif  //End _rsaeuro_h
