/* Copyright (C) 1997 by Pattern Recognition and Human Language
   Technology Group, Technological Institute of Computer Science,
   Valencia University of Technology, Valencia (Spain).

   Permission to use, copy, modify, and distribute this software and its
   documentation for any purpose and without fee is hereby granted, provided
   that the above copyright notice appear in all copies and that both that
   copyright notice and this permission notice appear in supporting
   documentation.  This software is provided "as is" without express or
   implied warranty.
*/

/* pgm.h - header file for libpgm portable graymap library
*/
#ifndef _PGM_H_
#define _PGM_H_

#include "libpbm.h"

/* The following definition has nothing to do with the format of a PGM file */
typedef unsigned char gray;

#define PGM_MAXVAL 255


/* Magic constants. */

#define PGM_MAGIC1 'P'
#define PGM_MAGIC2 '2'
#define RPGM_MAGIC2 '5'
#define PGM_FORMAT (PGM_MAGIC1 * 256 + PGM_MAGIC2)
#define RPGM_FORMAT (PGM_MAGIC1 * 256 + RPGM_MAGIC2)
#define PGM_TYPE PGM_FORMAT
#define UNK_FORMAT -1

/* Macro for turning a format number into a type number. */

#define PGM_FORMAT_TYPE(f) ((f) == PGM_FORMAT || (f) == RPGM_FORMAT ? PGM_TYPE : UNK_FORMAT -1)


/* Declarations of routines. */

gray ** pgm_allocarray( int cols, int rows );

void pgm_freearray(gray ** const rowIndex, int const rows);

gray** pgm_readpgm ( FILE* file, int* colsP, int* rowsP, gray* maxvalP );

void pgm_writepgm ( FILE* file, gray** grays, int cols, int rows, gray maxval, int forceplain );

gray** pbm2pgm ( bit** bits, int cols, int rows );

gray** pgmcrop ( gray** grays, int cols, int rows, int* ncols, int* nrows );

#endif /*_PGM_H_*/
