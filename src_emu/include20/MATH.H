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
 * $Source: /cvs/hopencvs/src/include/math.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:58 $
 *
\**************************************************************************/


/* math.h -- Definitions for the math floating point package.  */

#ifndef  _MATH_H_
#define  _MATH_H_

//2003-08-04 #include <machine/ieeefp.h>

#ifndef HUGE_VAL

/* Define HUGE_VAL as infinity, unless HUGE_VAL is already defined
   (which might have been done by something like math-68881.h).  */

union __dmath
{
	unsigned long i[2];
	double d;
};

extern const union __dmath __infinity;

#define HUGE_VAL (__infinity.d)

#endif /* ! defined (HUGE_VAL) */

#ifdef __cplusplus
extern "C" {
#endif

/* Reentrant ANSI C functions.  */

#ifndef __math_68881
double atan(double);
double cos(double);
double sin(double);
double tan(double);
double tanh(double);
double frexp(double, int *);
double modf(double, double *);
double ceil(double);
double fabs(double);
double floor(double);
#endif /* ! defined (__math_68881) */

/* Non reentrant ANSI C functions.  */

#ifndef _REENT_ONLY
#ifndef __math_6881
double acos(double);
double asin(double);
double atan2(double, double);
double cosh(double);
double sinh(double);
double exp(double);
double ldexp(double, int);
double log(double);
double log10(double);
double pow(double, double);
double sqrt(double);
double fmod(double, double);
#endif /* ! defined (__math_68881) */
#endif /* ! defined (_REENT_ONLY) */

#ifndef _STRICT_ANSI

/* Non ANSI double precision functions.  */

double infinity(void);
double nan(void);
int isnan(double);
int isinf(double);
int finite(double);
double copysign(double, double);
int ilogb(double);

double asinh(double);
double cbrt(double);
double nextafter(double, double);
double rint(double);
double scalbn(double, int);

#ifndef __math_68881
double log1p(double);
double expm1(double);
#endif /* ! defined (__math_68881) */

#ifndef _REENT_ONLY
double acosh(double);
double atanh(double);
double remainder(double, double);
double gamma(double);
double gamma_r(double, int *);
double lgamma(double);
double lgamma_r(double, int *);
double erf(double);
double erfc(double);
double y0(double);
double y1(double);
double yn(int, double);
double j0(double);
double j1(double);
double jn(int, double);
#define log2(x) (log (x) / M_LOG2_E)

#ifndef __math_68881
double hypot(double, double);
#endif

double cabs();
double drem(double, double);

#endif /* ! defined (_REENT_ONLY) */

/* Single precision versions of ANSI functions.  */

float atanf(float);
float cosf(float);
float sinf(float);
float tanf(float);
float tanhf(float);
float frexpf(float, int *);
float modff(float, float *);
float ceilf(float);
float fabsf(float);
float floorf(float);

#ifndef _REENT_ONLY
float acosf(float);
float asinf(float);
float atan2f(float, float);
float coshf(float);
float sinhf(float);
float expf(float);
float ldexpf(float, int);
float logf(float);
float log10f(float);
float powf(float, float);
float sqrtf(float);
float fmodf(float, float);
#endif /* ! defined (_REENT_ONLY) */

/* Other single precision functions.  */

float infinityf(void);
float nanf(void);
int isnanf(float);
int isinff(float);
int finitef(float);
float copysignf(float, float);
int ilogbf(float);

float asinhf(float);
float cbrtf(float);
float nextafterf(float, float);
float rintf(float);
float scalbnf(float, int);
float log1pf(float);
float expm1f(float);

#ifndef _REENT_ONLY
float acoshf(float);
float atanhf(float);
float remainderf(float, float);
float gammaf(float);
float gammaf_r(float, int *);
float lgammaf(float);
float lgammaf_r(float, int *);
float erff(float);
float erfcf(float);
float y0f(float);
float y1f(float);
float ynf(int, float);
float j0f(float);
float j1f(float);
float jnf(int, float);
#define log2f(x) (logf (x) / (float) M_LOG2_E)
float hypotf(float, float);

float cabsf();
float dremf(float, float);
#endif /* ! defined (_REENT_ONLY) */

/* The gamma functions use a global variable, signgam.  */

extern int signgam;

/* The exception structure passed to the matherr routine.  */

#ifdef __cplusplus
struct __exception
#else
struct exception
#endif
{
  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
  int err;
};

#ifdef __cplusplus
extern int matherr(struct __exception *e);
#else
extern int matherr(struct exception *e);
#endif

/* Values for the type field of struct exception.  */

#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

/* Useful constants.  */

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_TWOPI         (M_PI * 2.0)
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3   	1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        0.693147180559945309417
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

/* Global control over fdlibm error handling.  */

enum __fdlibm_version
{
  __fdlibm_ieee = -1,
  __fdlibm_svid,
  __fdlibm_xopen,
  __fdlibm_posix
};

#define _LIB_VERSION_TYPE enum __fdlibm_version
#define _LIB_VERSION __fdlib_version

extern _LIB_VERSION_TYPE _LIB_VERSION;

#define _IEEE_  __fdlibm_ieee
#define _SVID_  __fdlibm_svid
#define _XOPEN_ __fdlibm_xopen
#define _POSIX_ __fdlibm_posix

#endif /* ! defined (_STRICT_ANSI) */

#ifdef __cplusplus
}
#endif

#endif /* _MATH_H_ */
