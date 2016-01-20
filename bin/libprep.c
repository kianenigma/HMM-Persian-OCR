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


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "libprep.h"

int * getV_Projection(gray **img,int rows,int cols, gray imaxval){ 
    /* Calculamos la proyeccion vertical */
    int * histver;
    int i,j;
    histver=(int *)malloc(sizeof(int)*cols);
    for (j=0; j < cols; j++)
	histver[j]=0;
    
    for (j=0; j < cols; j++) {
	for (i=0; i < rows ; i++)   
	    if (img[i][j] < imaxval/1.5){ //Hace el histograma asumiendo negro a partir de 170 ( si suponemos q el maximo es 255)
		histver[j]++;
	    }
    }
    return histver;
}

int tamanyo_medio_huecos(int * V_Projection, int cols){ 
  int contador=0, suma_huecos=0, total_huecos=0, i;

  for (i=0;i<cols;i++) {
    contador=0;
    while ((i<cols) && (V_Projection[i]<UMBRAL_VERTICAL))
      {
        contador++;
        i++;
      }
    if (i<=cols && contador>0)
      {
        suma_huecos+=contador;
        total_huecos++;
      }
   }
  if (total_huecos) return(MAX(suma_huecos/total_huecos,SEPARACION_MIN_LETRAS));
  else return(INT_MAX);
 }


int deteccionTramos(gray ** img, int ** inicio_tramo, int ** fin_tramo, int rows, int cols, gray imaxval){
    int  n_tramos=0, i, separacion_tramos,hueco;
    int *histver;
    
    *inicio_tramo=(int*)malloc(cols*sizeof(int));
    *fin_tramo=(int*)malloc(cols*sizeof(int));
    
    /* Calculamos la proyeccion vertical */
    histver=getV_Projection(img,rows,cols,imaxval);
    
    /* Calculamos el tama�o medio de los huecos */
    separacion_tramos=tamanyo_medio_huecos(histver, cols);
    
    /* Dividimos la imagen en tramos contiguos*/
    n_tramos=0;
    /* quitamos posibles blancos iniciales */
    i=0;
    while (i < cols && histver[i] < UMBRAL_VERTICAL)  i++;
    if (i==cols) {
        free(histver);
        return (-1);
    }

    /*Dividimos en tramos */
    (*inicio_tramo)[n_tramos]=i; 
    while(1) {
	for (; i<cols && histver[i]>=UMBRAL_VERTICAL; i++);
	if (i==cols) { 
	    (*fin_tramo)[n_tramos]=cols-1; 
	    break;
	}
	(*fin_tramo)[n_tramos]=i; 
	for (hueco=0; i<cols && histver[i]<UMBRAL_VERTICAL; i++, hueco++);
	if (i==cols) break;
	if (hueco>=separacion_tramos) {
	    n_tramos++;
	    (*inicio_tramo)[n_tramos]=i;
	}
    }

    free(histver);

    return(n_tramos+1);
    
}

void copiar(gray **origen, gray **destino,int cols, int rows) 
{
  int i,j;
  static int n_frames=0; /* para copiar m�s de una frase por imagen */

  for (i=0;i<rows;i++)
    for (j=0;j<cols;j++)
      destino[i+n_frames*rows][j]=origen[i][j];
  
  n_frames++;
}

void trasladarpgm(gray **origen, gray **destino, int col_inicial, int col_final, int incy, int rows)
{
  int row,col;

  for (col=col_inicial;col<col_final;col++)
    if (incy<0)
      for (row=0;row<rows;row++)
	  if (row+incy>=0)
	    destino[row][col]=origen[row+incy][col];
	  else 
	    destino[row][col]=255;
    else
      for (row=rows-1;row>=0;row--)
	if (row+incy<rows)
	  destino[row][col]=origen[row+incy][col];
	else 
	  destino[row][col]=255;
}



gray ** centrarLinBase(gray ** img, int n_tramos, int *inicio, int *fin, int * cogxlw, int rows, int cols){
  int i,r,c,row_despl;
  
  gray ** centrada=pgm_allocarray(cols,rows*2);
  //Centrada es una matriz con el doble de filas, todos los tramos se ponen en el centro.
  for (i=0;i<n_tramos;i++){
      row_despl=rows - cogxlw[i];

      for (c=inicio[i];c<fin[i];c++)
	  for(r=0;r<rows;r++)
	      centrada[r+row_despl][c]=img[r][c];
  }
  return centrada;
}

gray ** crop(gray ** img,int cols, int rows, int *rows_crop, int *cols_crop, gray imaxval){
    int r_up,r_down,r,c,c_fin;
    unsigned int acum=0;
    gray ** img_crop=NULL;
    int N_pixels=0;

    
    for (r_up=0; r_up<rows; r_up++){
	for (c=0; c<cols; c++)
	    if(img[r_up][c]<imaxval*0.5)
		N_pixels++;
	if (N_pixels > 5) break;
    }
    
    if (r_up > rows) r_up=0;

    N_pixels=0;
    for (r_down=rows-1; r_down>=0; r_down--){
	for (c=0; c<cols; c++)
	    if(img[r_down][c]<imaxval*0.5)
		N_pixels++;
	if (N_pixels > 5) break;
    }
    if (r_down <0) r_down=rows-1;

    for (c_fin=cols-1; c_fin>=0;c_fin--){
	acum=0;
	for(r=0;r<rows;r++)
	    acum+=img[r][c_fin];
	if ((acum/rows) <imaxval)
	    break;
    }
					   

    * cols_crop= c_fin+1;
    * rows_crop= r_down - r_up +1;

    if (( *rows_crop > 0) && (*cols_crop > 0))
	img_crop=(gray**)pgm_allocarray(*cols_crop,*rows_crop);
    else{
	*rows_crop=0;
	*cols_crop=0;
    }
    
    for (r=0;r<*rows_crop;r++)
	for (c=0;c<*cols_crop;c++)
	    img_crop[r][c]=img[r+r_up][c];
    
    return img_crop;
}


gray ** row_crop(gray ** img,int cols, int rows, int *rows_crop, gray imaxval){
  int r_up,r_down,r,c;
  unsigned int acum=0;
  gray ** img_crop=NULL;
    
  for (r_up=0; r_up<rows; r_up++){
    acum=0;
    for (c=0; c<cols; c++)
      acum+=img[r_up][c];
    if ((acum/cols) < imaxval)
      break;
  }

  for (r_down=rows-1; r_down>=0; r_down--){
    acum=0;
    for (c=0; c<cols; c++)
      acum+=img[r_down][c];
    if ((acum/cols) < imaxval)
      break;
  }
  
  * rows_crop= r_down - r_up +1;
  if ( *rows_crop > 0)
    img_crop=(gray**)pgm_allocarray(cols,*rows_crop);
  else
    *rows_crop=0;
  
  for (r=0;r<*rows_crop;r++)
    for (c=0;c<cols;c++)
      img_crop[r][c]=img[r+r_up][c];

  return img_crop;
}
