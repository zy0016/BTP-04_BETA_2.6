/* Access functions for GB2312 conversion.
   Copyright (C) 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _GB2312_H
#define _GB2312_H	1

#include <sys/types.h>

extern const char __gb2312_from_ucs4_tab1[][2];
extern const char __gb2312_from_ucs4_tab2[][2];
extern const char __gb2312_from_ucs4_tab3[][2];
extern const char __gb2312_from_ucs4_tab4[][2];
extern const char __gb2312_from_ucs4_tab5[][2];
extern const char __gb2312_from_ucs4_tab6[][2];
extern const char __gb2312_from_ucs4_tab7[][2];
extern const char __gb2312_from_ucs4_tab8[][2];
extern const char __gb2312_from_ucs4_tab9[][2];

unsigned int gb2312_to_ucs4 (const unsigned char **s, size_t avail, unsigned char offset);
size_t ucs4_to_gb2312 (unsigned short, unsigned char *, size_t);
int uni16_to_gb2312(unsigned char *gb, unsigned char *uni);
int gb2312_to_uni16(const char *gb, int len, char *uni, int *outlen);

#endif	/* gb2312.h */
