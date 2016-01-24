/* libpgm1.c - pgm utility library part 1

###########################################################################
# Copyright (C) 1997 by Pattern Recognition and Human Language
# Technology Group, Technological Institute of Computer Science,
# Valencia University of Technology, Valencia (Spain).
#
# Modification and Distribution by Kian Peymani, Artificial Intelligence Group
# Iran University of Science and Thechnology
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# with appearance of this license on top of each file.
# This software is provided "as is" without express or implied warranty.
# ##########################################################################

*/

/* See libpbm.c for the complicated explanation of this 32/64 bit file
   offset stuff.
*/
#define _FILE_OFFSET_BITS 64
#define _LARGE_FILES
#define _MAXGRAY 255

#include <string.h>
#include <stdio.h>
/*#include <errno.h>*/
#include <stdlib.h>
#include <limits.h>
#include "libpgm.h"

int readmagicnumber(FILE * const ifP) {
  int ich1, ich2;

  ich1 = getc(ifP);
  ich2 = getc(ifP);
  if (ich1 == EOF || ich2 == EOF){
    fprintf(stderr, "Error reading magic number from Netpbm image stream.\nMost often, this means your input file is empty.\n" );
    exit(-1);
  }
  return ich1 * 256 + ich2;
}

inline unsigned int pgm_getuint(FILE * const ifP) {
/*----------------------------------------------------------------------------
   Read an unsigned integer in ASCII decimal from the file stream
   represented by 'ifP' and return its value.

   If there is nothing at the current position in the file stream that
   can be interpreted as an unsigned integer, issue an error message
   to stderr and abort the program.

   If the number at the current position in the file stream is too
   great to be represented by an 'int' (Yes, I said 'int', not
   'unsigned int'), issue an error message to stderr and abort the
   program.
-----------------------------------------------------------------------------*/
    char ch;
    unsigned int i;

    do {
        ch = fgetc(ifP);
        } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9'){
        fprintf(stderr,"junk in file where an unsigned integer should be\n");
	exit(-1);
    }

    i = 0;
    do {
        unsigned int const digitVal = ch - '0';

        if (i > INT_MAX/10 - digitVal){
            fprintf(stderr,"ASCII decimal integer in file is too large to be processed.\n");
	    exit(-1);
	}
        i = i * 10 + digitVal;
        ch = fgetc(ifP);
    } while (ch >= '0' && ch <= '9');

    return i;
}


gray ** pgm_allocarray(int cols,int rows){
  gray ** rowIndex;
  int row,col;

  rowIndex = (gray**) malloc( rows * sizeof(gray*) );
   if (rowIndex== NULL){
     fprintf(stderr,"out of memory allocating Row %u of an array\n", rows);
     exit(-1);
   }
  for (row = 0; row < rows; row++) {
    rowIndex[row] = (gray*) malloc(cols * sizeof(gray));

    if (rowIndex[row] == NULL){
     fprintf(stderr,"out of memory allocating Row %u (%u columns, %u bytes per tuple) of an array\n", row, cols, sizeof(gray));
     exit(-1);
    }
  }

 for (row = 0; row < rows; row++)
   for (col=0; col < cols; col++)
     rowIndex[row][col]=PGM_MAXVAL;

  return rowIndex;
}

void pgm_freearray(gray ** const rowIndex, int const rows) {

  unsigned int row;
  for (row = 0; row < rows; ++row)
    free(rowIndex[row]);

  free(rowIndex);
}


/*************************************************************/
/*                    PGM routines                           */
/*************************************************************/


void pgm_readpgminitrest(FILE * const file,
                    int *  const colsP,
                    int *  const rowsP,
                    gray * const maxvalP) {

    gray maxval;

    /* Read size. */
    *colsP = (int)pgm_getuint(file);
    *rowsP = (int)pgm_getuint(file);

    /* Read maxval. */
    maxval = pgm_getuint(file);
    if (maxval == 0){
        fprintf(stderr,"maxval of input image is zero.");
	exit(-1);
    }
    *maxvalP = (gray) maxval;
}

void pgm_readpgminit(FILE * const file, int * const colsP,
                int * const rowsP, gray * const maxvalP, int * const formatP) {

    /* Check magic number. */
  *formatP = readmagicnumber( file );
  switch ( PGM_FORMAT_TYPE(*formatP) ) {
  case PGM_TYPE:
    pgm_readpgminitrest( file, colsP, rowsP, maxvalP );
    break;

  default:
    fprintf(stderr, "bad magic number - not a pgm file\n" );
    exit(-1);
  }
}

gray pgm_getrawsample(FILE * const file, gray const maxval) {
  int iby;

    iby = getc(file);
    if (iby == EOF){
        fprintf(stderr,"EOF / read error reading a one-byte sample\n");
	exit(-1);
    }
    return (unsigned char) iby;
}



void pgm_readpgmrow(FILE* const file, gray* const grayrow,
               int const cols, gray const maxval, int const format) {

  switch (format) {
  case PGM_FORMAT: {
    int col;
    for (col = 0; col < cols; ++col)
      grayrow[col] = pgm_getuint(file);
  }
    break;

  case RPGM_FORMAT: {
    int col;
    for (col = 0; col < cols; ++col)
      grayrow[col] = pgm_getrawsample( file, maxval );
  }
    break;

  default:
    fprintf(stderr,"can't happen\n");
  }
}


gray** pgm_readpgm(FILE * const file, int * const colsP, int * const rowsP,
             gray * const maxvalP) {

    gray** grays;
    int row;
    int format;

    pgm_readpgminit( file, colsP, rowsP, maxvalP, &format );

    grays = pgm_allocarray( *colsP, *rowsP );

    for ( row = 0; row < *rowsP; row++ )
      pgm_readpgmrow( file, grays[row], *colsP, *maxvalP, format );

    return grays;
}

/********************************************************************/
/*               writing routines                                   */
/********************************************************************/

void pgm_writepgminit(FILE * const fileP, int    const cols,  int    const rows,  gray   const maxval,  int    const forceplain)
{

    fprintf(fileP, "%c%c\n%d %d\n%d\n",  PGM_MAGIC1,
            forceplain ? PGM_MAGIC2 : RPGM_MAGIC2,
            cols, rows, maxval );
}

static void pgm_writepgmrowraw(FILE *file, gray *grayrow, int cols, gray maxval ) {
  int col,rc;

  for (col = 0; col < cols; col++,grayrow++) {
    rc = fputc(*grayrow, file);
    if (rc == EOF){
      fprintf(stderr,"Error writing single byte sample to file\n");
      exit(-1);
    }
  }
}

inline void pgm_writepgmrowplain(FILE *file, gray *grayrow, int cols, gray maxval ) {
  register int col, charcount;
  register gray* gP;

  charcount = 0;
  for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
    {
      if ( charcount >= 65 ){
	(void) fprintf(file, "\n");
	charcount = 0;
      } else if ( charcount > 0 )
	{
	  (void) fprintf(file," ");
	  charcount++;
	}
      fprintf(file, "%i ", (int) *gP);
      charcount += 3;

    }
  if ( charcount > 0 )
    (void) fprintf(file, "\n");
}

void pgm_writepgmrow( FILE* file, gray* grayrow, int cols, gray maxval, int forceplain )
{
  if (forceplain )
    pgm_writepgmrowplain( file, grayrow, cols, maxval );
  else
    pgm_writepgmrowraw( file, grayrow, cols, maxval );
}

void pgm_writepgm( FILE* file, gray** grays, int cols, int rows, gray maxval, int forceplain )
{
  int row;

  pgm_writepgminit( file, cols, rows, maxval, forceplain );

  for ( row = 0; row < rows; row++ )
    pgm_writepgmrow( file, grays[row], cols, maxval, forceplain );
}



/********************************************************************/
/*               Convert PBM to PGM				    */
/********************************************************************/

gray** pbm2pgm ( bit** bits, int cols, int rows )
{
  int row, col;
  gray** grays;

  grays = pgm_allocarray(cols,rows);

  for ( row = 0; row < rows; ++row )
    for ( col = 0; col < cols; ++col )
      if (bits[row][col]==PBM_WHITE) grays[row][col]=PGM_MAXVAL;
      else grays[row][col]=0;
  return grays;
}


/********************************************************************/
/*               PGMCROP 	                                    */
/********************************************************************/

gray** pgmcrop ( gray** grays, int cols, int rows, int* ncols, int* nrows )
{
  int row, col, irow=0, icol=0, frow=rows-1, fcol=cols-1;
  gray** fgrays=NULL;

  for ( col = 0; col < cols ; ++col ) {
    for ( row = 0; row < rows && grays[row][col]!=PGM_MAXVAL; ++row );
    if (row!=rows) { icol=col; break; }
  }
  if (col==cols) return fgrays;
  for ( col = cols-1; col >= 0 ; --col ) {
    for ( row = 0; row < rows && grays[row][col]==PGM_MAXVAL; ++row );
    if (row!=rows) { fcol=col; break; }
  }
  for ( row = 0; row < rows ; ++row ) {
    for ( col = icol; col <= fcol && grays[row][col]==PGM_MAXVAL; ++row );
    if (col!=fcol+1) { irow=row; break; }
  }
  for ( row = rows-1; row >= 0 ; --row ) {
    for ( col = icol; col <= fcol && grays[row][col]==PGM_MAXVAL; ++row );
    if (col!=fcol+1) { frow=row; break; }
  }

  fgrays = pgm_allocarray(fcol-icol+1,frow-irow+1);

  for ( row = irow; row <= frow; ++row )
    for ( col = icol; col <= fcol; ++col )
      fgrays[row-irow][col-icol]=grays[row][col];

  *ncols=fcol-icol+1;
  *nrows=frow-irow+1;
  return grays;
}
