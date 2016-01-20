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


/******************************************************************************
*  pgmskew: skew correction
*
*  COMPILE: gcc -o pgmskew pgmskew.c libpbm.c libpgm.c libprep.c -lm -O3
*  PRHTL Group
******************************************************************************/
#include <float.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>
#include <values.h>
#include <stdlib.h>
#include "libpgm.h"
#include "libprep.h"

/*****************************************************************************/

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

/*****************************************************************************/
#define TAMANYO_MINIMO_PALABRA 50
#define LONGITUD_MEDIA_LETRA 40
#define PI 3.14159

bit **image;
gray **original;
gray **rotada;
gray **final;
gray **salida;
int rows,cols;
int n_frames=0;
gray imaxval;
int *cogx;
int verbosity=0;
int out_ascii=1;

void rotar(int inicio, int fin, int row_central, int col_central, float angle)
{
  float original_row,original_col;
  float auxL,auxU,aux;
  int row,col,rowL,rowU,colL,colU;
  double c_angle,s_angle;
  
  c_angle=cos(angle);
  s_angle=sin(angle);

  for (col=inicio;col<fin;col++)
    for (row=0;row<rows;row++) {

      /* exact sampling location: */
      original_row=c_angle*(row-row_central)+s_angle*(col-col_central)+row_central; 
      original_col=-s_angle*(row-row_central)+c_angle*(col-col_central)+col_central;
      
      /* four nearest coefficients to the sampling location: */
      rowL=((int)original_row); 
      rowU=((int)original_row)+1;
      colL=((int)original_col);
      colU=((int)original_col)+1;
      
      /* Lagrangian interpolation: */
      if ((rowL>=0)&&(rowU<rows) && colL >= 0 && colU < cols)
	{
	  auxL=(rowU-original_row)*original[rowL][colL]*(255/imaxval)+(original_row-rowL)*original[rowU][colL]*(255/imaxval);
	  auxU=(rowU-original_row)*original[rowL][colU]*(255/imaxval)+(original_row-rowL)*original[rowU][colU]*(255/imaxval);
	  aux=(original_col-colL)*auxU+(colU-original_col)*auxL;
	  rotada[row][col]=aux;
	} 
    }   
}

int calcular_cogx(gray ** img, int c_ini, int c_fi,bit **ubl,bit**lbl){
 int r_up,r_down,c;
 unsigned int acum=0;

  for (r_up=0; r_up<rows; r_up++){
    acum=0;
    for (c=c_ini; c<c_fi; c++)
      acum+=img[r_up][c];
    if ((acum/(c_fi-c_ini)) < imaxval)
      break;
  }

  for (r_down=rows-1; r_down>=0; r_down--){
    acum=0;
    for (c=c_ini; c<c_fi; c++)
      acum+=img[r_down][c];
    if ((acum/(c_fi - c_ini)) < imaxval)
      break;
  }

 return(r_down + r_up)/2;
 
}

void ajuste_cont(bit **image,int col_ini,int col_fi, int *row_ini, int *row_fi){
  int *hist;
  int row,col,i,totMasa,acumulado;

  hist=(int*)malloc(rows*sizeof(int));

  for(row=0;row<rows;row++)
    hist[row]=0;

  totMasa=0;
  for(row=0;row<rows;row++){
    for(col=col_ini;col<col_fi;col++)
      hist[row]+=image[row][col]; 
    totMasa+=hist[row];
  }
  totMasa/=2;

  for (i=0,acumulado=0; i<rows && acumulado<totMasa; i++)
    acumulado+=hist[i];

  *row_ini = i - (int)(1+(col_fi-col_ini)*.06);
  *row_fi  = i + (int)(1+(col_fi-col_ini)*.06);

  if (*row_ini<0) *row_ini=0;
  if (*row_fi> rows) *row_fi=rows-1;
 
  free(hist);
}

void line_detector(bit **image,int row_inicio, int col_inicio, int row_final, int col_final,float *slope,int *cogx,int *cogy)
{
  int row,col;
  float sum,sumx,sumy,sumxx,sumxy,sumyy;
  float scatterxx,scatterxy,scatteryy;
  float lambda1,lambda2,lambda,aux,aux2;
  bit pixel;

  sum=sumx=sumy=sumxx=sumxy=sumyy=0.0;
  for (row=row_inicio;row<row_final;row++)
    for (col=col_inicio;col<col_final;col++) {
      pixel=image[row][col];
      sum+=pixel;
      sumx+=pixel*row;
      sumy+=pixel*col;
      sumxx+=pixel*row*pixel*row;
      sumxy+=pixel*row*pixel*col;
      sumyy+=pixel*col*pixel*col;
    }
  /* scatter matrix of the standardized points:
     
     | scatterxx scatterxy |   | sum(x-mx)*(x-mx)  sum(x-mx)*(y-my) |
     |                     | = |                                    |
     | scatterxy scatteryy |   | sum(x-mx)*(y-my)  sum(y-my)*(y-my) |
   
                              | sumxx-sumx*sumx/n  sumxy-sumx*sumy/n |
                            = |                                      |
                              | sumxy-sumx*sumy/n  sumyy-sumy*sumy/n | */

  scatterxx=sumxx-sumx*sumx/sum;
  scatterxy=sumxy-sumx*sumy/sum;
  scatteryy=sumyy-sumy*sumy/sum;

  /* maximum eigenvalue:
     
    | scatterxx-lambda         scatterxy |
    |                                    | = 0
    | scaterxy          scatteryy-lambda |

    ==> lambda*lambda - (scatterxx+scatteryy) * lambda +
        (scatterxx*scatteryy - scatterxy*scatterxy) = 0 */

  aux=scatterxx+scatteryy;
  aux2=sqrt(aux*aux-4*(scatterxx*scatteryy-scatterxy*scatterxy));
  lambda1=(aux+aux2)/2.0;
  lambda2=(aux-aux2)/2.0;
  lambda=MAX(lambda1,lambda2);

  /* slope:

     (scatterxx-lambda)*x +          scatterxy*y = 0 |
                                                     | ==> (adding both eqs)
              scatterxy*x + (scatteryy-lambda)*y = 0 |

               lambda - scatterxx - scatterxy
     ==>  y = ------------------------------- * x
               scatteryy + scatterxy - lambda        */

  aux=(lambda-scatterxx-scatterxy)/(scatteryy+scatterxy-lambda);
  if (aux==aux) /* number, including -Inf and Inf */
    (*slope)=MIN(MAX(aux,-FLT_MAX),FLT_MAX);
  else /* not a number (NAN) */
    (*slope)=0.0;
  
  (*cogx)=sumx/sum;
  (*cogy)=sumy/sum;
}

void deteccionDeBordes(gray ** original, bit*** publ,  bit***plbl, int n_tramos,int * inicio, int *fin){
  int cont,i,firstTime,c2,col,row,max_cont,row_fi;
  bit **image;
  FILE *tmp_fd;
  bit ** ubl, **lbl;
  
  ubl=pbm_allocarray(cols,rows);
  lbl=pbm_allocarray(cols,rows);
  /* binarizamos la imagen */ 
  image=pbm_allocarray(cols,rows);
  for (row=0;row<rows;row++)
    for (col=0;col<cols;col++)
      if (original[row][col] > imaxval/2) /*pixel blanco*/
	image[row][col]=PBM_WHITE;
      else
	image[row][col]=PBM_BLACK;
  
  
  /* Ajuste de bordes de cada tramo */
  for (i=0;i<n_tramos;i++) {
    /* RLSA */
    for (row=0;row<rows;row++){
      cont=0;
      firstTime=1;
      for (col=inicio[i];col<fin[i];col++)
	if (image[row][col]==PBM_BLACK){
	  if(firstTime)  firstTime=0;
	  else
	    if (cont < LONGITUD_MEDIA_LETRA)
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
      row_fi=0; //
      for (row=0;row<rows;row++){
	if (image[row][col]==PBM_BLACK) 
	  cont++;
	else{
	  if (cont > max_cont){
	    max_cont=cont;
	    row_fi=row;
	  }
	  cont=0;
	}
      }
      if((row_fi == 0) && (cont > 0)){ 
	  row_fi = rows-1; 
	  max_cont=cont-1;
	 
      }

      ubl[row_fi-max_cont][col]=PBM_BLACK;
      lbl[row_fi][col]=PBM_BLACK;
    }    
  } /* fin de ajuste de los tramos */
  
  
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
  *publ=ubl;
  *plbl=lbl;
}


void usage(char * progName,char * cad){
  fprintf(stderr,"\nPerform handwritten text skew correction.\n\nUsage: %s %s\n",progName,cad);
  exit(1);
}

/*************************************************************************/
/*                                                                       */
/* Programa principal                                                    */
/*                                                                       */
/*************************************************************************/

int main(int argc, char *argv[])
{
  
  char usagecar[]="[-i pgm_file][-o pgm_file] [-v num] [b] [-h] \n\
    Options are: \n\
       -i pgm_file     input file (default stdin) \n\
       -o pgm_file     output file (default stdout) \n\
       -v level        verbosity mode \n\
       -b              binary output mode \n\
       -h              this help \n";

  int option;
  FILE *ifd=stdin, *ofd=stdout;
  int cogx_medio,cogy,cogxup,cogyup,cogxlw,cogylw;
  float slopeup,slopelw,angulo;
  int i,j,n_tramos,n_tramos_tratados;
  int *inicio,*fin;
  int row,col,row_ini,row_fi;
  
  bit **ubl,**lbl;
  int frames;
  
  frames=1;
  opterr=0;
  
  while ((option=getopt(argc,argv,"hbi:o:v:"))!=-1)
    switch (option) {
    case 'i':
      ifd=fopen(optarg,"r");
      break;
    case 'o':
      ofd=fopen(optarg,"w");
      break;
    case 'h':
      usage(argv[0],usagecar);
    case 'b':
      out_ascii=0;
      break;
    case 'v':
      verbosity=atoi(optarg);
      break;
    default:
      usage(argv[0],usagecar);
    }

  if (ifd==NULL  || ofd==NULL) {
    fprintf(stderr,"pgmhwtsize: Unable to open input and/or output files !\n");
    exit(-1);
  }
  
  original=pgm_readpgm(ifd,&cols,&rows,&imaxval);
  if (ifd!=stdin) fclose(ifd);

  rotada=pgm_allocarray(cols,rows);
  image=pbm_allocarray(cols,rows);

  final=pgm_allocarray(cols,rows);
  /*  salida=pgm_allocarray(cols,7*rows);*/
  
  
  if (verbosity)
    fprintf(stderr,"pgmld: Rows: %d, Cols: %d\n",rows,cols);
  
  for (i=0;i<rows;i++)
    for (j=0;j<cols;j++) {	 
      image[i][j]=(int) 1-(int)(original[i][j]/(float)imaxval + 0.5);
      final[i][j]=255;
    }
  
  /* deteccion de tramos */  
  n_tramos=deteccionTramos(original,&inicio,&fin,rows,cols,imaxval);
  if (n_tramos == -1){
      fprintf(stderr,"pgmskew: Number of Words-clumps detected: %d\n",n_tramos);
      exit (-1);
  }
  if (verbosity)
    fprintf(stderr,"pgmld: Number of Words-clumps detected: %d\n",n_tramos);
  
  cogx=(int*)malloc(n_tramos*sizeof(int));
  cogx_medio=0;
  
  n_tramos_tratados=0;
  for (i=0;i<n_tramos;i++) /* Ajuste de cada tramo */
    if (fin[i]-inicio[i]>TAMANYO_MINIMO_PALABRA) {
      n_tramos_tratados++;
      /* Deteccion de lineas base */
      deteccionDeBordes(original,&ubl,&lbl,n_tramos,inicio,fin);

      /* Ajuste del tramo por eigenvector */
      ajuste_cont(ubl,inicio[i],fin[i], &row_ini, &row_fi);
      line_detector(ubl,row_ini,inicio[i],row_fi,fin[i],&slopeup,&cogxup,&cogyup);

      ajuste_cont(lbl,inicio[i],fin[i],&row_ini,&row_fi);
      line_detector(lbl,row_ini,inicio[i],row_fi,fin[i],&slopelw,&cogxlw,&cogylw);

      if (verbosity){
	fprintf(stderr,"pgmld: Word-clump %d: (%d,%d) Upper line angle %f, UpperLine-Y-interception(%d,%d) \n",i,inicio[i],fin[i],atan(-1.0/slopeup)*180/PI,cogxup,cogyup);  
	fprintf(stderr,"pgmld: Word-clump %d: Lower line angle %f, LowerLine-Y-Interception(%d,%d) \n",i,atan(-1.0/slopelw)*180/PI,cogxlw,cogylw);
      }

      angulo=(-atan(-1.0/slopeup)-atan(-1.0/slopelw))/2.0;
      cogx[i]=(cogxup+cogxlw)/2;
      cogy=(cogyup+cogylw)/2;
      cogx_medio+=cogx[i];
      
      /* Correccion del slope */   
      rotar(inicio[i],fin[i],cogx[i],cogy,angulo);
      
      if (verbosity)
	fprintf(stderr,"pgmld: Word-clump %d: final angle correction %f \n",i,angulo);
  
      pbm_freearray(ubl,rows); 
      pbm_freearray(lbl,rows);


    } else{ /* tramo demasiado pequeño */

      for(col=inicio[i];col <fin[i];col++)
	for (row=0;row<rows;row++)
	  rotada[row][col]=original[row][col];
      //cogx[i]=calcular_cogx(original,inicio[i],fin[i]);
      cogx[i]=calcular_cogx(original,inicio[i],fin[i],ubl,lbl);

      /* cogx_medio+=cogx[i];*/

    }
  
  cogx_medio/=n_tramos_tratados;
  for (i=0;i<n_tramos;i++) {
    if (verbosity)
      fprintf(stderr,"pgmld: Word-clump= %d Averaged CoordX-Line-Y-Interception=%d, CoordX-Line-Y-Interception=%d\n",i,cogx_medio,cogx[i]);
    trasladarpgm(rotada,final,inicio[i],fin[i],cogx[i]-cogx_medio,rows);
  }
  n_frames=1;
  
  /*pgm_writepgm(stdout,salida,cols,n_frames*rows,255,out_ascii);*/
  pgm_writepgm(ofd,final,cols,n_frames*rows,255,out_ascii);
  if (ofd!=stdout) fclose(ofd);

  pbm_freearray(image,rows);
  pgm_freearray(original,rows);
  pgm_freearray(rotada,rows);
  pgm_freearray(final,rows);

  free(inicio); free(fin); free(cogx);
  /*  pbm_freearray(salida,7*rows);*/

  return(0);

}

