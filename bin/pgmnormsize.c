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

/*******************************************************************
*  COMPILAR: gcc -o pgmnormsize pgmnormsize.c libpgm.c libpbm.c  libprep.c -O3 -lm
********************************************************************/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libpgm.h"
#include "libprep.h"
/*****************************************************************************/

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

/*****************************************************************************/
#define TAMANYO_MINIMO_PALABRA 50
#define LONGITUD_MEDIA_LETRA 40
#define ASCENDENTES 0.3
#define DESCENDENTES 0.15
#define TIPO_NORMALIZACION 5  //media 

int rows,cols;
gray imaxval;
float p_asc,p_des;
int m_normalizacion;
int verbosity=0;
int out_ascii=1;


int normalizar(gray **img_in, gray ** img_out,int inicio,int inicio_norm, int fin,int ubl, int lbl,int filas_centrales)
{
    float aux,fac_c,fac_f,frac_f0,frac_f1,frac_c0,frac_c1;
    int row,col,c0,c1,f0,f1,i,j;
    int contador;
    int ul,ll;
    int filas_ascendentes,filas_descendentes;
    int rows2 = rows*2;
    int cols_normalizadas;
    int nuevo_lbl;
    
    if (verbosity > 1)
	fprintf(stderr,"pgmhwtsize: UBL %d || LBL %d\n",ubl,lbl);
    
    if (ubl<0) ubl=0;
    if (lbl>rows2-2) lbl=rows2-2;
    
    nuevo_lbl=rows2;
//  filas_centrales=lbl-ubl+1; /* para que no se normalize la zona central */
    filas_ascendentes  = (int)(filas_centrales*p_asc+0.5);
    filas_descendentes = (int)(filas_centrales*p_des+0.5);
    cols_normalizadas  = (int) (filas_centrales*(fin-inicio+1)/(float)(lbl-ubl+1) + 0.5);

   
    if (verbosity > 1){
	fprintf(stderr,"pgmhwtsize: init. rows= %d norm. rows= %d\npgmhwtsize: init. cols= %d norm. cols= %d\n",lbl-ubl+1,filas_centrales,fin-inicio+1,cols_normalizadas);
	fprintf(stderr,"pgmhwtsize: ascenders=%d,centrals=%d,descenders%d\n",filas_ascendentes,\
		filas_centrales,filas_descendentes) ;
    }
    /* **************************** */
    /* Normalizacion de ascendentes */
    /* **************************** */
    

    /* calculamos la frontera superior del texto */
    for (row=0,contador=0;(contador==0)&&(row<ubl-1);row++)
	for (col=inicio;col<fin;col++)
	    if (img_in[row][col]<imaxval) contador++;
    
    ul=row-1;
    if (ul<0) ul=0;
    
    if (ubl-ul>filas_ascendentes)  { /* ascendentes */


	fac_f=(ubl-ul+1)/(float)(filas_ascendentes);
	fac_c=(fin-inicio+1)/(float)(cols_normalizadas);
	if (verbosity > 0)
	    fprintf(stderr,"pgmhwtsize: shrink ascenders %f %d %d %d\n",fac_f,ubl, ul,filas_ascendentes);
	for (row=0;row<filas_ascendentes;row++)
	    for (col=0;col<cols_normalizadas;col++){
		aux=0;
		f0=(int)(row*fac_f);
		f1=(int)((row+1)*fac_f);
		frac_f0=(float)(row*fac_f-f0);
		frac_f1=(float)((row+1)*fac_f-f1);

		c0=(int)(col*fac_c);
		c1=(int)((col+1)*fac_c);
		frac_c0=(float)(col*fac_c-c0);
		frac_c1=(float)((col+1)*fac_c-c1);
		if (c0 == c1){//Esto se hará cuando se esten ampliando el numero de columnas
		    frac_c1=fac_c;
		    frac_c0=1;//Se pone a uno para q al hacer al resta con 1 de 0
		}

		aux=img_in[f0+ul][inicio+c0]*(1-frac_f0)*(1-frac_c0) + img_in[f0+ul][inicio+c1]*(1-frac_f0)*frac_c1;
		
		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f0+ul][inicio+i]*(1-frac_f0);
		
		for(i=f0+1;i<f1;i++){
		    aux+=img_in[i+ul][inicio+c0]*(1-frac_c0) + img_in[i+ul][inicio+c1]*frac_c1;
		    for(j=c0+1;j<c1;j++)
			aux+=img_in[i+ul][inicio+j];
		}
		
		aux+=img_in[f1+ul][inicio+c0]*frac_f1*(1-frac_c0) + img_in[f1+ul][inicio+c1]*frac_f1*frac_c1;
		
		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f1+ul][inicio+i]*frac_f1;

		if(c0==c1) aux=aux/((f1-f0-frac_f0+frac_f1)*fac_c);
		else aux=aux/((f1-f0-frac_f0+frac_f1)*(c1-c0-frac_c0+frac_c1));
		
		img_out[nuevo_lbl-filas_centrales-filas_ascendentes+row][inicio_norm+col]=aux;
	    }

    }
    else { /* falsos ascendentes */
	filas_centrales+=(ubl-ul);
	ubl = ul; 


    }
    /* Fin normalizacion de ascendentes */
    
    /* **************************** */
    /* Normalizacion de descendentes */
    /* **************************** */
   
    for (row=rows2-1,contador=0;(contador==0)&&(row>lbl+1);row--)
	for (col=inicio;col<fin;col++)
	    if (img_in[row][col]<imaxval) contador++;
    
    ll=row+1;
    if (ll>rows2) ll=rows2;
    
    if (ll-lbl-1>filas_descendentes){

 

	fac_f=(ll-lbl+1)/(float)(filas_descendentes);
	fac_c=(fin-inicio+1)/(float)(cols_normalizadas);
	//fac=(ll-lbl-1)/(float)(filas_descendentes);
	if (verbosity > 0)
	    fprintf(stderr,"pgmhwtsize: shrink descenders %f\n",fac_f);

	for (row=0;row<filas_descendentes;row++)
	    for (col=0;col<cols_normalizadas;col++){
		aux=0;
		f0=(int)(row*fac_f);
		f1=(int)((row+1)*fac_f);
		frac_f0=(float)(row*fac_f-f0);
		frac_f1=(float)((row+1)*fac_f-f1);    

		c0=(int)(col*fac_c);
		c1=(int)((col+1)*fac_c);
		frac_c0=(float)(col*fac_c-c0);
		frac_c1=(float)((col+1)*fac_c-c1);
		if (c0==c1){ //Este caso ocurrirá cuando se este ampliando la imagen
		    frac_c1=fac_c;
		    frac_c0=1;
		}


		aux=img_in[lbl+f0][inicio+c0]*(1-frac_f0)*(1-frac_c0)+img_in[lbl+f0][inicio+c1]*(1-frac_f0)*frac_c1 ; 

		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f0+lbl][inicio+i]*(1-frac_f0);

		for (i=f0+1;i<f1;i++){
		    aux+=img_in[lbl+i][inicio+c0]*(1-frac_c0) + img_in[lbl+i][inicio+c1]*frac_c1;
		    for(j=c0+1;j<c1;j++)
			aux+=img_in[i+lbl][inicio+j];
		}
		
	
		aux+=img_in[lbl+f1][inicio+c0]*frac_f1*(1-frac_c0) + img_in[lbl+f1][inicio+c1]*frac_f1*frac_c1 ; 

		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f1+lbl][inicio+i]*frac_f1;


		if(c0==c1) aux=aux/((f1-f0-frac_f0+frac_f1)*fac_c);
		else aux=aux/((f1-f0-frac_f0+frac_f1)*(c1-c0-frac_c0+frac_c1));


		img_out[nuevo_lbl+row][inicio_norm+col]=aux;
	    }
	
	if (verbosity > 0)
	    fprintf(stderr,"pgmhwtsize: descenders have been shrunk\n");
    } else {
	filas_centrales+=ll-lbl;
	nuevo_lbl+=ll-lbl;
	lbl=ll;

    }
    
    /* Fin de la normalizacion de descendentes */
    
    /* ******************************** */
    /* Normalizacion de la zona central */
    /* ******************************** */
    
    //Teoricamente el valor de ambas proporciones deberia ser identico
    fac_f=(lbl-ubl+1)/(float)(filas_centrales);
    fac_c=(fin-inicio+1)/(float)(cols_normalizadas);
    
    if((lbl-ubl+1) < filas_centrales){ //ampliamos la imagen

	for (col=0;col<cols_normalizadas;col++)
	    for (row=0;row<filas_centrales;row++){
				
		f0=(int)(row*fac_f);
		f1=(int)((row+1)*fac_f);
		frac_f1=(float)((row+1)*fac_f-f1)/fac_f;
		if (frac_f1 > 1) frac_f1=1;
		
		c0=(int)(col*fac_c);
		c1=(int)((col+1)*fac_c);
		frac_c1=(float)((col+1)*fac_c-c1)/fac_c;
		if (frac_c1 > 1) frac_c1=1;
		
	
		aux=img_in[f0+ubl][inicio+c0]*(1-frac_f1) * (1-frac_c1) + img_in[f0+ubl][inicio+c1]*(1-frac_f1)*frac_c1 + img_in[f1+ubl][inicio+c0]*frac_f1*(1-frac_c1) + img_in[f1+ubl][inicio+c1]*frac_f1*frac_c1;
	
		img_out[nuevo_lbl-filas_centrales+row][inicio_norm+col]=aux;
	    }

    }
    else {//reducimos la imagen

	for (col=0;col<cols_normalizadas;col++)
	    for (row=0;row<filas_centrales;row++){
		f0=(int)(row*fac_f);
		f1=(int)((row+1)*fac_f);
		frac_f0=(float)(row*fac_f-f0);
		frac_f1=(float)((row+1)*fac_f-f1);
			
		c0=(int)(col*fac_c);
		c1=(int)((col+1)*fac_c);
		frac_c0=(float)(col*fac_c-c0);
		frac_c1=(float)((col+1)*fac_c-c1);
			
		aux=img_in[f0+ubl][inicio+c0]*(1-frac_f0)*(1-frac_c0) + img_in[f0+ubl][inicio+c1]*(1-frac_f0)*frac_c1;
		
		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f0+ubl][inicio+i]*(1-frac_f0);
		
		for(i=f0+1;i<f1;i++){
		    aux+=img_in[i+ubl][inicio+c0]*(1-frac_c0) + img_in[i+ubl][inicio+c1]*frac_c1;
		    for(j=c0+1;j<c1;j++)
			aux+=img_in[i+ubl][inicio+j];
		}
		
		aux+=img_in[f1+ubl][inicio+c0]*frac_f1*(1-frac_c0) + img_in[f1+ubl][inicio+c1]*frac_f1*frac_c1;
		
		for(i=c0+1;i<c1;i++)
		    aux+=img_in[f1+ubl][inicio+i]*frac_f1;
		
		aux=aux/((f1-f0-frac_f0+frac_f1)*(c1-c0-frac_c0+frac_c1));
		
		img_out[nuevo_lbl-filas_centrales+row][inicio_norm+col]=aux;
	    }
    }  
  
    return inicio_norm+cols_normalizadas;  
}

void calcular_cogx(int *cogxup, int * cogxlw, int tramo,int ntramos){
  int vecinos,auxup,auxlw,i;
  
  auxup=0;
  auxlw=0;
  vecinos=0;
  for (i=0;i<ntramos;i++)
      if (i != tramo && cogxup[i] > 0){
	  vecinos++;
	  auxup+=cogxup[i];
	  auxlw+=cogxlw[i];
      }
  // Si hay mas tramos devuelve la media de los otros tramos
  if (vecinos>0){
      cogxup[tramo]=auxup/vecinos;
      cogxlw[tramo]=auxlw/vecinos;
  }
  else { // Si no hay mas tramos lo hace como un porcentaje
      cogxup[tramo]=rows*0.25;
      cogxlw[tramo]=rows*0.75;
  } 
}

int line_detector(gray **image, int col_inicio, int col_final){
  int row,col;
  float sum,sumx,sumy,sumxx,sumxy,sumyy;
  float scatterxx,scatterxy,scatteryy;
  float lambda1,lambda2,lambda,aux,aux2;
  int pixel;
  int *histhor; 
  int j,totalhor=0,acumulado,rowmax,row_inicio,row_final;


  histhor=(int*)malloc(rows*sizeof(int));
  
  for (row=0;row<rows;row++){ 
      histhor[row]=0;
  }
  for (row=0;row<rows;row++){ 
      for (col=col_inicio;col<col_final;col++){
	  histhor[row]+=image[row][col];
      }
      totalhor+=histhor[row];
      
  }
      
  acumulado=0;
  for (j=0;acumulado<totalhor/2;j++)
      acumulado+=histhor[j];
  rowmax=j;
  
  row_inicio=rowmax-(int)(1+(col_final-col_inicio)*.06);
  row_final=rowmax+(int)(1+(col_final-col_inicio)*.06);

 
  if (row_inicio<0) row_inicio=0;
  if (row_final>rows-1) row_final=rows;  //en el for, la ultima linea no se tiene en cuenta.
    /*   if (verbosity > 0) */
/* 	fprintf(stderr,"pgmhwtsize: UBL Top=%d, Bottom=%d\n",topup,bottomup); */
  
 

  sum=sumx=sumy=sumxx=sumxy=sumyy=0.0;
  for (row=row_inicio;row<row_final;row++)
    for (col=col_inicio;col<col_final;col++){
      pixel=image[row][col];
      sum+=pixel;
      sumx+=pixel*row;
      sumy+=pixel*col;
      sumxx+=pixel*row*pixel*row;
      sumxy+=pixel*row*pixel*col;
      sumyy+=pixel*col*pixel*col;
    }

 

  scatterxx=sumxx-sumx*sumx/sum;
  scatterxy=sumxy-sumx*sumy/sum;
  scatteryy=sumyy-sumy*sumy/sum;

  aux=scatterxx+scatteryy;
  aux2=sqrt(aux*aux-4*(scatterxx*scatteryy-scatterxy*scatterxy));
  lambda1=(aux+aux2)/2.0;
  lambda2=(aux-aux2)/2.0;
  lambda=MAX(lambda1,lambda2);

  aux=(lambda-scatterxx-scatterxy)/(scatteryy+scatterxy-lambda);
  free(histhor);
  return sumx/sum;
  
}

int line_detector2(bit **image, int col_inicio, int col_final){
    unsigned int *histo;
    unsigned int maxVal,sum;
    int c,r,i,maxCol;
    
  
    //Obtencion de un histograma horizontal
    histo=(unsigned int*)malloc(sizeof(unsigned int)*rows);
    for (r=0;r<rows;r++)
	histo[r]=0;
    
    for (r=0;r<rows;r++)
	for (c=col_inicio;c<col_final;c++)
	    histo[r]+=image[r][c];

    maxCol=0;
    maxVal=0;
    for (r=0;r<rows;r++){
	sum=0;
	for (i=r-3;i<r+3;i++)
	    if (i>=0 && i<rows)
		sum+=histo[i];
	if (maxVal < sum){
	    maxVal=sum;
	    maxCol=r;
	}
    }
    free(histo);
    //Devuelve la fila q sumando lo q tiene a su alrededor (+/- 3 posiciones) tiene mas valor 
      
    return(maxCol);
}

void deteccionDeBordes(gray ** original, int* up,  int*lw, int n_tramos,int * inicio, int *fin){
    int cont,i,firstTime,c2,col,row,max_cont,row_fi;
    bit **image;
    gray **ubl,**lbl;
    FILE *tmp_fd;
    
    /* binarizamos la imagen */ 
    image=pbm_allocarray(cols,rows);
    for (row=0;row<rows;row++)
	for (col=0;col<cols;col++)
	    if (original[row][col] > imaxval/2) /*pixel blanco*///Se binariza poniendo a blanco un valor con nivel de luz mayor a 127'5 (si maxval=255)
		image[row][col]=PBM_WHITE;
	    else
		image[row][col]=PBM_BLACK;
    
    ubl=pbm_allocarray(cols,rows);
    lbl=pbm_allocarray(cols,rows);
    
    /* Ajuste de bordes de cada tramo */
    for (i=0;i<n_tramos;i++)     
	if (fin[i]-inicio[i]>TAMANYO_MINIMO_PALABRA){ //TAMANYO_MINIMO_PALABRA = 50
	    /* RLSA */
	    for (row=0;row<rows;row++){
		cont=0;
		firstTime=1;
		for (col=inicio[i];col<fin[i];col++)
		    if (image[row][col]==PBM_BLACK){
			if(firstTime)  firstTime=0;
			else
			    if (cont < LONGITUD_MEDIA_LETRA) //LONGITUD_MEDIA_LETRA = 40
				for (c2=col-cont; c2 <= col; c2++)
				    image[row][c2]=PBM_BLACK;
			cont=0;
		    } else 
			cont++;
	    }
	    /* obtencion de los bordes superior e inferior */
	    for (col=inicio[i];col<fin[i];col++){
		cont=0;
		max_cont=0;
		row_fi=0;
		for (row=0;row<rows;row++)
		    if (image[row][col]==PBM_BLACK) 
			cont++;
		    else{
			if (cont > max_cont){
			    max_cont=cont;
			    row_fi=row;
			}
			cont=0;
		    }
		if ((row_fi!=0) || (max_cont!=0)){
		    ubl[row_fi-max_cont][col]=PBM_BLACK; //Matriz con el borde superior
		    lbl[row_fi][col]=PBM_BLACK; //Matriz con el borde inferior
		}
	    }

	    up[i]=line_detector(ubl,inicio[i],fin[i]); 
 	    lw[i]=line_detector(lbl,inicio[i],fin[i]);

	    /*Si se cruzan los bordes se hace el mismo tratamiento que cuando la palabra es corta */
	    if (up[i]>=lw[i]){
		up[i]=-1;
		lw[i]=-1;
	    }
	    if (verbosity>=3){
		fprintf(stderr,"up %d :\n", up[i]);
		fprintf(stderr,"lw %d :\n", lw[i]);
	    }
	} 
	else{
	    /*calcular_cogx(image,&up[i],&lw[i]);*/
	    up[i]=-1;
	    lw[i]=-1;
	} /* fin de ajuste de los tramos */
   
    /*  tratamiento de los tramos pequeños (típicamente las y) y de los tramos donde se han cruzado los bordes */
    for (i=0;i<n_tramos;i++)
	if (up[i]<0) calcular_cogx(up,lw,i,n_tramos);
    
    if (verbosity>=3){
	tmp_fd=fopen("img_RLSA.pbm","w");
	pbm_writepbm(tmp_fd,image,cols,rows,out_ascii);
	fclose(tmp_fd);
	tmp_fd=fopen("img_upcont.pgm","w");
	pbm_writepbm(tmp_fd,ubl,cols,rows,out_ascii);
	fclose(tmp_fd);
	tmp_fd=fopen("img_lowcont.pgm","w");
	pbm_writepbm(tmp_fd,lbl,cols,rows,out_ascii);
	fclose(tmp_fd);
    }
    pbm_freearray(image,rows);
    pbm_freearray(ubl,rows); 
    pbm_freearray(lbl,rows);
}

int tamnyo_lineas_normalizado(int n_tramos,int *inicio, int *fin, int *up,int *lw,int mecanismo_norm){
    int longitud_total,n_filas_normalizado,n_filas_max;
    int i,r,maxRow;
    float n_filas_medio_ponderado, ponderado;
    unsigned int *histo;
    unsigned int maxVal,sum;


    switch (mecanismo_norm)
    {
	case 1:   
	    //Calculo del nº de lineas ponderando cada tramo con respecto a la longitud
	    longitud_total=0;
	    for(i=0;i<n_tramos;i++){
		longitud_total+=fin[i]-inicio[i];
	    }
	    
	    n_filas_medio_ponderado=0;
	    for(i=0;i<n_tramos;i++){
		ponderado=(float)(fin[i]-inicio[i])/(float)longitud_total;
		n_filas_medio_ponderado=n_filas_medio_ponderado+(lw[i]-up[i])*ponderado;
		fprintf(stderr,"word-clump height %d: %d \n",i,lw[i]-up[i]);
	    }
	    n_filas_normalizado=(int)(n_filas_medio_ponderado);
	    break;

	case 2:
	    //Calcular el tamanyo maximo del cuerpo del texto
	    n_filas_normalizado=0;
	    for (i=0;i<n_tramos;i++){
		if(lw[i]-up[i] > n_filas_normalizado)
		    n_filas_normalizado=lw[i]-up[i];
		fprintf(stderr,"altura tramo %d: %d \n",i,lw[i]-up[i]);

	    }
	    break;
	    
	case 3:
	    // Calcular segundo maximo del cuerpo de texto 
	    n_filas_max=0;
	    for (i=0;i<n_tramos;i++){
		if(lw[i]-up[i] > n_filas_max)
		    n_filas_max=lw[i]-up[i];
	    }
	    
	    n_filas_normalizado=0;
	    for (i=0;i<n_tramos;i++){
		if((lw[i]-up[i] > n_filas_normalizado)&&(lw[i]-up[i] < n_filas_max))
		    n_filas_normalizado=lw[i]-up[i];
	fprintf(stderr,"word-clump height %d: %d \n",i,lw[i]-up[i]);

	    }

	    if(n_filas_normalizado==0) n_filas_normalizado=n_filas_max;

	    break;

	case 4:
	    //Calcular el tamanyo medio del cuerpo del texto y sumarle 5
	    n_filas_normalizado=0; 
	    for (i=0;i<n_tramos;i++){ 
		n_filas_normalizado+=lw[i]-up[i]; 
	fprintf(stderr,"word-clump height %d: %d \n",i,lw[i]-up[i]);
	    } 
	    n_filas_normalizado/=n_tramos; 
	    n_filas_normalizado+=5;
	    break;

	case 5:
	    //Calcular el tamanyo medio del cuerpo del texto
	    n_filas_normalizado=0; 
	    for (i=0;i<n_tramos;i++){ 
		n_filas_normalizado+=lw[i]-up[i]; 
	fprintf(stderr,"word-clump height %d: %d \n",i,lw[i]-up[i]);
	    } 
	    n_filas_normalizado/=n_tramos; 
	    break;
	    
	case 6:
	    //Calcular la moda del cuerpo del texto considerando el contexto
	
	    histo=(unsigned int*)malloc(sizeof(unsigned int)*rows);
	    for (r=0;r<rows;r++)
		histo[r]=0;
	    
	    for (i=0;i<n_tramos;i++){
		histo[rows-(lw[i]-up[i])]+=1;
	fprintf(stderr,"word-clump height %d: %d \n",i,lw[i]-up[i]);
	    }
	    maxRow=0;
	    maxVal=0;
	    for (r=0;r<rows;r++){
		sum=0;
		for (i=r-5;i<r+5;i++) 
		    if (i>=0 && i<rows)
			sum+=histo[i];
		if (maxVal < sum){
		    maxVal=sum;
		    maxRow=r;
		}
	    }
	    free(histo);
	    n_filas_normalizado=rows-maxRow;
	 
	    
    }

     if (verbosity)
	fprintf(stderr,"pgmhwtsize: Normalization number of rows: %d \n",n_filas_normalizado);
    
    return n_filas_normalizado;

}
 
void usage(char * progName,char * cad){
    fprintf(stderr,"\nPerform handwritten text size normalization.\n\nUsage: %s %s\n",progName,cad);
    exit(1);
}

int main(int argc, char *argv[]){
    
    FILE *ifd=stdin, *ofd=stdout, *tmp_fd;
    int i,n_tramos;
    int nuevo_fin,*inicio,*fin;
    int row,col,c;
    int *cogxup,*cogxlw;
    gray ** original, **normalizada,** centrada, ** img_crop;
    
    int n_filas;
    int n_filas_normalizado;
    int option;
    
    char usagecad[]="[-i pgm_file][-o pgm_file][-a num][-d num][-v num] [b] [-c num] [-h] \n\
    Options are:\
       -i pgm_file     input file (default stdin)\n\
       -o pgm_file     output file (default stdout)\n\
       -a <num>        %% of central area assigned to ascenders  (default 0.3)\n\
       -d <num>        %% of central area assigned to descenders (default 0.15)\n\
       -v level        verbosity mode\n\
       -b              binary output mode\n\
       -c <num>        text size normalization method for the central area:\n\
                          1-Weighted Averageed word-clumps\n\
                          2-Maximum height\n\
                          3-Second maximum height\n\
                          4-Averaged word-clumps height plus 5\n\
                          5-Averaged word-clumps height\n\
                          6-Mode with context\n\
       -h              this help \n";
    m_normalizacion=TIPO_NORMALIZACION;
    p_asc=ASCENDENTES;
    p_des=DESCENDENTES;
    
    while ((option=getopt(argc,argv,"hbd:a:i:o:v:c:"))!=-1)
	switch (option)
	{
	    case 'i':
		ifd=fopen(optarg,"r");
		break;
	    case 'o':
		ofd=fopen(optarg,"w");
		break;
	    case 'a':
		p_asc=atof(optarg);
		break;
	    case 'd':
		p_des=atof(optarg); 
		break;
	    case 'v':
		verbosity=atoi(optarg);
		break;
	    case 'b':
		out_ascii=0;
		break;
	    case 'h':
		usage(argv[0],usagecad);
		break;
	    case 'c':
		m_normalizacion=atoi(optarg);
		break;
	    default:
		usage(argv[0],usagecad);
	}
    
    if (ifd==NULL  || ofd==NULL) {
	fprintf(stderr,"pgmhwtsize: Unable to open the input and/or output files\n");
    exit(-1);
    }
    
    original=pgm_readpgm(ifd,&cols,&rows,&imaxval);
    if (ifd!=stdin) fclose(ifd);

    /* inicializar matrices */
  
    normalizada=pgm_allocarray(cols*4,rows*3);

    /* Deteccion de los distintos tramos de la imagen */
    n_tramos=deteccionTramos(original,&inicio,&fin,rows,cols,imaxval);
    if (n_tramos == -1){
	fprintf(stderr,"pgmhwtsize: Any word-clump was detected.\n");
	exit(-1);
    }
    if (verbosity > 0)
	fprintf(stderr,"pgmhwtsize: Number of word-clumps detected: %d\n",n_tramos);
    
    cogxup=(int*)malloc((n_tramos)*sizeof(int));
    cogxlw=(int*)malloc((n_tramos)*sizeof(int));
   
    /*Deteccion de los bordes de los tramos*/
    deteccionDeBordes(original,cogxup,cogxlw,n_tramos,inicio,fin);
   
    /* centrar lineas base */
    centrada = centrarLinBase(original,n_tramos,inicio,fin,cogxlw,rows,cols);
    
  
    /*Calcular el numero de filas al que se va a normalizar la imagen */
    n_filas_normalizado=tamnyo_lineas_normalizado(n_tramos,inicio,fin,cogxup,cogxlw,m_normalizacion);
  
    /* Normalizacion de tamanyo */
     
    nuevo_fin=0;
    for (i=0;i<n_tramos;i++){

	n_filas=cogxlw[i]-cogxup[i];
	if (verbosity)
	    fprintf(stderr,"\npgmhwtsize: Word-clump=%d (froml %d to %d)\n",i,inicio[i],fin[i]);
	if (i>0)
	    nuevo_fin=normalizar(centrada,normalizada,inicio[i],inicio[i]+(nuevo_fin-fin[i-1]),fin[i],rows-n_filas-3,rows+3,n_filas_normalizado);
	else
	    nuevo_fin=normalizar(centrada,normalizada,inicio[i],inicio[i],fin[i],rows-n_filas-3,rows+3,n_filas_normalizado);
    }
     
    /* crop de la imagen normalizada */
    img_crop = crop(normalizada, cols*4, rows*3, &row, &col,imaxval);
    
    /* mostramos imagenes intermedias */
    if (verbosity>=3){
    /* pintamos lineas */
	for (i=0;i<cols;i++){
	    centrada[rows][i]=0;
            centrada[rows-n_filas_normalizado][i]=0;  
	}
	for (i=0;i<n_tramos;i++)
	    for (c=inicio[i];c<fin[i];c++)
		centrada[rows-(cogxlw[i]-cogxup[i])][c]=0;

	tmp_fd=fopen("img_centrada.pgm","w");
	pgm_writepgm(tmp_fd,centrada,cols,rows*2,255,out_ascii);
	fclose(tmp_fd);
    }	
    
    pgm_writepgm(ofd,img_crop,col,row,imaxval,out_ascii);
    //pgm_writepgm(ofd,normalizada,cols*4,rows*3,imaxval,out_ascii);
    if (ofd!=stdout) fclose(ofd);

    pgm_freearray(normalizada,rows*3);
    pgm_freearray(centrada,rows*2);
    pgm_freearray(img_crop,row);
    pgm_freearray(original,rows);
 
    
    free(inicio);
    free(fin);
    free(cogxup);
    free(cogxlw);
    
    return(0);
}

