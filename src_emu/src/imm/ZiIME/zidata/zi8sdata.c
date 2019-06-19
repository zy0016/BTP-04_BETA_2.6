/* zi8sdata.c
// This module contains a static language table that references
// The compiled version of data. This file is referenced from
// Zi8Initialize. 
//
// THIS FILE MUST BE MODIFIED TO PROVIDE THE DESIRED CONFIGURATION OF
// LANGUAGES. REFERENCES FROM THIS MODULE WILL CAUSE THE APPROPRIATE 
// DATASETS TO BE LINKED FROM THE LIBRARY. 
*/
#include "zi8api.h" 

extern ZI8UCHAR ZI8ROMDATA Zi8DatENPRC[];	/* English (PRC)   */
extern ZI8UCHAR ZI8ROMDATA Zi8DatZH[];		/* Chinese (PRC)   */

ZI8_LANGUAGE_ENTRY ZI8ROMDATA Zi8StaticLanguageTable[] =
{
    {ZI8_LANG_EN,	(void ZI8ROMPOINTER)&Zi8DatENPRC[0]},
    {ZI8_LANG_ZH,	(void ZI8ROMPOINTER)&Zi8DatZH[0]},
    {0,0}
};
