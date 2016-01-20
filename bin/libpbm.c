/* libpbm2.c - pbm utility library part 2
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "libpbm.h"

int pbm_readmagicnumber(FILE * const ifP) {
  int ich1, ich2;
  
  ich1 = getc(ifP);
  ich2 = getc(ifP);
  if (ich1 == EOF || ich2 == EOF){
    fprintf(stderr, "Error reading magic number from Netpbm image stream.\n Most often, this means your input file is empty.\n" );
    exit(-1);
  }
  return ich1 * 256 + ich2;
}
unsigned int pbm_getuint(FILE * const ifP) {
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
            fprintf(stderr,"ASCII decimal integer in file is too \
            large to be processed.\n");
	    exit(-1);
	}
        i = i * 10 + digitVal;
        ch = fgetc(ifP);
    } while (ch >= '0' && ch <= '9');

    return i;
}


static bit getbit (FILE * const file) {
    char ch;

    do {
        ch = fgetc( file );
    } while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

    if ( ch != '0' && ch != '1' ){
        fprintf(stderr, "junk in file where bits should be\n" );
	exit(-1);
    }
    
    return ( ch == '1' ) ? 1 : 0;
}

unsigned char pbm_getrawbyte(FILE * const file) {
    int iby;

    iby = getc(file);
    if (iby == EOF){
      fprintf(stderr,"EOF / read error reading a one-byte sample\n");
      exit(-1);
    }
    return (unsigned char) iby;
}

bit ** pbm_allocarray(int cols,int rows){
  bit ** rowIndex;
  int row,col;

  rowIndex = (bit**) malloc( rows * sizeof(bit*) );
  if (rowIndex== NULL){
     fprintf(stderr,"out of memory allocating Row %u \
	     	       of an array\n", row);
     exit(-1);
    }
  for (row = 0; row < rows; row++) {
    rowIndex[row] = (bit*) malloc(cols * sizeof(bit));

    if (rowIndex[row] == NULL){
     fprintf(stderr,"out of memory allocating Row %u \
	       (%u columns, %u bytes per tuple) \
	       of an array\n", row, cols, sizeof(bit));
     exit(-1);
    }
  }

  for (row = 0; row < rows; row++)
    for (col=0; col < cols; col++)
      rowIndex[row][col]=PBM_WHITE;
  
  return rowIndex;
}
void pbm_freearray(bit ** const rowIndex, int const rows) {

  unsigned int row;
  for (row = 0; row < rows; row++)
    free(rowIndex[row]);
  
  free(rowIndex);
}

/*************************************************************/
/*                    PBM routines                           */
/*************************************************************/

void pbm_readpbminitrest( FILE * file, int * colsP, int * rowsP )
{
  /* Read size. */
  *colsP = (int)pbm_getuint( file );
  *rowsP = (int)pbm_getuint( file );
  
  /* *colsP and *rowsP really should be unsigned int, but they come
     from the time before unsigned ints (or at least from a person
     trained in that tradition), so they are int.  We could simply
     consider negative numbers to mean values > INT_MAX/2 and much
     code would just automatically work.  But some code would fail
     miserably.  So we consider values that won't fit in an int to
     be unprocessable.
  */
  if (*colsP < 0){
    fprintf(stderr,"Number of columns in header is too large.\n");
    exit(-1);
  }
  if (*rowsP < 0){
    fprintf(stderr,"Number of columns in header is too large.\n");
    exit(-1);
  }
}

void pbm_readpbminit( FILE * file, int * colsP, int * rowsP, int * formatP )
{
  /* Check magic number. */
  *formatP = pbm_readmagicnumber( file );
  switch ( PBM_FORMAT_TYPE(*formatP) )
    {
    case PBM_TYPE:
      pbm_readpbminitrest( file, colsP, rowsP );
      break;
      
    default:
      fprintf(stderr, "bad magic number - not a pbm file\n" );
      exit(-1);
    }
}

void pbm_readpbmrow( FILE * file, bit * bitrow, int  cols, int format )
{
  register int col, bitshift;
  register bit* bP;
  
  switch ( format )
    {
    case PBM_FORMAT:
      for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
        *bP = getbit( file );
      break;
      
    case RPBM_FORMAT: {
      register unsigned char item;
      bitshift = -1;  item = 0;  /* item's value is meaningless here */
      for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
	{
	  if ( bitshift == -1 )
	    {
	      item = pbm_getrawbyte( file );
	      bitshift = 7;
	    }
	  *bP = ( item >> bitshift ) & 1;
	  --bitshift;
	}
    }
      break;
      
    default:
      fprintf(stderr, "can't happen" );
      exit(-1);
    }
}




bit** pbm_readpbm( FILE * file, int * colsP, int *  rowsP )
{
  register bit** bits;
  int format, row;
  
  pbm_readpbminit( file, colsP, rowsP, &format );
  
  bits = pbm_allocarray( *colsP, *rowsP );
  
  for ( row = 0; row < *rowsP; ++row )
    pbm_readpbmrow( file, bits[row], *colsP, format );
  
  return bits;
}

/********************************************************************/
/*               writing routines                                   */
/********************************************************************/

void pbm_writepbminit(FILE * const fileP, 
		      int    const cols, 
		      int    const rows, 
		      int    const forceplain) {
  
  if (!forceplain) {
    fprintf(fileP, "%c%c\n%d %d\n", PBM_MAGIC1, RPBM_MAGIC2, cols, rows);
  } else
    fprintf(fileP, "%c%c\n%d %d\n", PBM_MAGIC1, PBM_MAGIC2, cols, rows);
}

inline void pbm_writepbmrowplain( FILE *file, bit * bitrow, int cols )
{
  register int col, charcount;
  register bit* bP;
  
  charcount = 0;
  for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
    {
      if ( charcount >= 70 )
	{
	  (void) putc( '\n', file );
	  charcount = 0;
	}
      (void) putc( *bP ? '1' : '0', file );
      ++charcount;
    }
  (void) putc( '\n', file );
}

void pbm_writepbmrowraw( FILE * file, bit * bitrow, int cols )
{
  register int col, bitshift;
  register unsigned char item;
  register bit* bP;
  
  bitshift = 7;
  item = 0;
  for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
    {
      if ( *bP )
	item += 1 << bitshift;
      --bitshift;
      if ( bitshift == -1 )
	{
	  (void) putc( item, file );
	  bitshift = 7;
	  item = 0;
	}
    }
  
  if ( bitshift != 7 )
    (void) putc( item, file );
}

void pbm_writepbmrow(FILE * const fileP, 
		     bit *  const bitrow, 
		     int    const cols, 
		     int    const forceplain) {

  if (!forceplain )
    pbm_writepbmrowraw(fileP, bitrow, cols);
  else
    pbm_writepbmrowplain(fileP, bitrow, cols);
}

void pbm_writepbm(FILE *file, bit ** bits, int cols, int rows, int forceplain)
{
  int row;
  
  pbm_writepbminit( file, cols, rows, forceplain );
  
  for ( row = 0; row < rows; ++row )
        pbm_writepbmrow( file, bits[row], cols, forceplain );
}
