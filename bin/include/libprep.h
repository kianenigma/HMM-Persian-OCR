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


#include "libpgm.h"
#define UMBRAL_VERTICAL 1
#define SEPARACION_MIN_LETRAS 25  /* en normtamanyo era 10 */

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

int * getV_Projection(gray **img,int rows,int cols, gray imaxval);
int tamanyo_medio_huecos(int * V_Projection, int cols);
int deteccionTramos(gray ** img, int ** inicio_tramo, int ** fin_tramo,int rows, int cols, gray imaxval);
void copiar(gray **origen, gray **destino,int cols, int rows) ;
void trasladarpgm(gray **origen, gray **destino, int col_inicial, int col_final, int incy, int rows);
gray ** centrarLinBase(gray ** img, int n_tramos, int *inicio, int *fin, int * cogxlw, int rows, int cols);
gray ** crop(gray ** img,int cols, int rows, int *rows_crop, int *cols_crop, gray imaxval);
gray ** row_crop(gray ** img,int cols, int rows, int *rows_crop, gray imaxval);
