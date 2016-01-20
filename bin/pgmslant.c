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


/* pgmslant -
 * 	
 *          Calculates the average slant of a text image
 *
 * Copyright (C) 2004 by Mois�s Pastor i Gadea (mpastorg@dsic.upv.es) 
 *                        PRHLT group
*****************************************************************************/

/* compilar: gcc -o pgmslant pgmslant.c ./libpgm.c ./libprep.c -lm -O3 */

#define VERBOSA 0
#define NO_VERBOSA 1
#define ProuNegre 170

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <unistd.h>
#include "libpgm.h"
#include "libprep.h"

int rows, cols, verbosity=0;
gray **frase,**rotada;
gray imaxval;
float threshold=1;
int out_ascii=1;

void shear(int inicio, int fin, float angle)
{
  int r,c,despl=0,rows_2;
  double shearfac=tan((double)angle);
  static int inicial_desp=0;
  float rest_desp=0;
  rows_2=rows/2;

  if ((inicio - fabs(rows_2*shearfac)) < 0)
    inicial_desp=(int)fabs(rows_2*shearfac);
    
  // comprovar si se'n va per l'esquerra
   for (r=0;r<=rows_2;r++){
     despl=(int)((rows_2 - r)*shearfac)- inicial_desp;
     rest_desp=((((rows_2 - r)*shearfac)- inicial_desp) -despl);
     for (c=inicio;c<fin;c++){
       if (rest_desp <0)
	 rotada[r][c-despl]=(gray)(frase[r][c]*(-rest_desp)+frase[r][c+1]*(1+rest_desp));
       else
	 rotada[r][c-despl]=(gray)(frase[r][c+1]*rest_desp+frase[r][c]*(1-rest_desp));
       //rotada[r][c-despl]=(gray)frase[r][c];
     }
   }
   for (r=rows_2+1;r<rows;r++){
     despl=(int)((r - rows_2)*shearfac) + inicial_desp;
     rest_desp=(((r - rows_2)*shearfac) + inicial_desp -despl);
     for (c=inicio;c<fin;c++){
       if (rest_desp <0)
	 rotada[r][c+despl]=(gray)(frase[r][c+1]*(-rest_desp)+frase[r][c]*(1+rest_desp));
       else
	 rotada[r][c+despl]=(gray)(frase[r][c]*rest_desp+frase[r][c+1]*(1-rest_desp));
       //rotada[r][c+despl]=(gray)frase[r][c];
     }
   }
}

inline float var_VProjection(int *VPr,int cols) {
  int i;
  float sum=0, sum2=0,var,mean;
  int *V_Proj=VPr;
  
  for (i=0;i<cols;VPr++,i++){
    sum+=(float)(*VPr);
    sum2+= (float)(*VPr)*(*VPr);
  }
  mean=sum/cols;
  var=(sum2 - 2 * mean * sum + sum2)/cols;

  if (verbosity == 3){
    fprintf(stderr,"#Vertical Projection\n");
    for (i=0;i<cols;i++) 
      fprintf(stderr,"%d %d\n",i, V_Proj[i]); 
  }

  return sqrt(var);
} 

int CalcLongPerfil(int *projection, int rows) {
  int    i;
  double l,lperf;
  lperf=0;
  for (i=1; i<rows; i++) {
    l=projection[i]-projection[i-1];
    lperf+=l*l;
  }
  return(lperf);
}

int ** get_projections(gray ** image, int inicio_tramo, int fin_tramo, int rows){
  int row,col,despl_inicial,dim_prj;
  int a,i;
  int desp[91];
  int ** VProj;
  gray * pcol;
  
  despl_inicial=rows - inicio_tramo;
  dim_prj=(fin_tramo-inicio_tramo)+2*rows ;

  /* Pedimos memoria para las proyecciones */
  VProj=(int **) malloc(sizeof(int*)*91);
  for (i=0;i<=90;i++)
    VProj[i]=(int *)malloc(sizeof(int)*dim_prj);

  /* Inicializamos las proyecciones */
  for (i=0;i<=90;i++)
    for (col=0; col < dim_prj; col++)
      VProj[i][col]=0;

  /* Calculamos las proyecciones */
  for (row=0;row<rows;row++){
    for (a=0;a<=90;a++)
      desp[a]=(int)((rows-row)*tan((float)(a-45)*M_PI/180.0)); /* todos los 
								 despl para 
								 cada fila */
    for (col=inicio_tramo,pcol=&image[row][inicio_tramo] ; col < fin_tramo;pcol++, col++) 
      if (*pcol < ProuNegre)         /* lo tenemos en cuenta */
	for (a=0;a<=90;a++)           /* si es suficientemente negro */
	  if ((despl_inicial+col+desp[a]) >= 0 && (despl_inicial+col+desp[a]) < dim_prj)
	    /* VProj[a+30][despl_inicial+col+desp[a+30]]+=image[row][col]; */
	    VProj[a][despl_inicial+col+desp[a]]++;
  }
  return VProj;
}
float Javi_slant(gray ** image, int inicio_tramo, int fin_tramo, int rows){
  int dim_prj; 
  int a,best_a,sum,max_sum,i;
  int ** VProj;
 
  VProj=get_projections(image, inicio_tramo,fin_tramo, rows);
  // dim_prj=(fin_tramo-inicio_tramo)*4;
  dim_prj=(fin_tramo-inicio_tramo)+2*rows ;
  max_sum=0;
  for (a=0;a<=90;a++) {
    sum=0;
    sum = CalcLongPerfil(VProj[a], dim_prj);
    if (sum > max_sum) {	
      max_sum=sum;
      best_a=a-45;
    }
  }
 
  /* Liberamos la memoria de las proyectiones */
  for (i=0;i<=90;i++)
    free(VProj[i]);

  free(VProj);
  
  return(-best_a);
}


float IDIAP_slant(gray ** image, int inicio_tramo, int fin_tramo, int rows){
  int row,col,c,dim_prj; 
  int a,best_a,sum,despl_inicial,max_sum,i,dist;
  int desp[91];
  int ** VProj,**MaxCol,**MinCol;
  gray * pcol;
  
  //despl_inicial=(fin_tramo-inicio_tramo)*1.5 - inicio_tramo;
  despl_inicial=rows - inicio_tramo;
  //dim_prj=(fin_tramo-inicio_tramo)*4;
  dim_prj=(fin_tramo-inicio_tramo)+2*rows ;
  /* Pedimos memoria para las proyecciones */
  VProj=(int **) malloc(sizeof(int*)*91);
  MaxCol=(int **) malloc(sizeof(int*)*91);
  MinCol=(int **) malloc(sizeof(int*)*91);

  for (i=0;i<=90;i++){
    VProj[i]=(int *)malloc(sizeof(int)*dim_prj);
    MaxCol[i]=(int *)malloc(sizeof(int)*dim_prj);
    MinCol[i]=(int *)malloc(sizeof(int)*dim_prj);
  }
/* Inicializamos las proyecciones */
  for (i=0;i<=90;i++)
    for (col=0; col < dim_prj; col++){
      VProj[i][col]=0;
      MaxCol[i][col]=-1;
      MinCol[i][col]=cols+1;
    }
  
  /* Calculamos las proyecciones */
  for (row=0;row<rows;row++){
    for (a=0;a<=90;a++)
      desp[a]=(int)((rows-row)*tan((float)(a-45)*M_PI/180.0)); /* todos los 
								 despl para 
								 cada fila */
    for (col=inicio_tramo,pcol=&image[row][inicio_tramo] ; col < fin_tramo;pcol++, col++) {
      if (*pcol < ProuNegre)         /* lo tenemos en cuenta */
	for (a=0;a<=90;a++)           /* si es suficientemente negro */
	  if ((despl_inicial+col+desp[a]) >= 0 && (despl_inicial+col+desp[a]) < dim_prj){
	    /* VProj[a+30][despl_inicial+col+desp[a+30]]+=image[row][col]; */
	    VProj[a][despl_inicial+col+desp[a]]++;
	    if (row > MaxCol[a][despl_inicial+col+desp[a]]) MaxCol[a][despl_inicial+col+desp[a]]= row;
	    if (row < MinCol[a][despl_inicial+col+desp[a]]) MinCol[a][despl_inicial+col+desp[a]]= row;
	  }
    }
  }

  /* verbosidad de las proyecciones */
  if (verbosity == 3){
    fprintf(stderr,"#Vertical Projection\n");
    fprintf(stderr,"# ");
    for (a=0;a<=90;a++) fprintf(stderr,"%d ",a-45);
    fprintf(stderr,"\n");
    for (c=0;c<dim_prj;c++) {
      fprintf(stderr,"%d ",c);
      for (a=0;a<=90;a++)
	fprintf(stderr,"%d ",VProj[a][c]);
      fprintf(stderr,"\n");
    }
  }
  
  max_sum=0;
  best_a=-10000;
  
  for (a=0;a<=90;a++) {
    //  fprintf(stderr,"%d ",a-45);
    sum=0;
    for (col=0; col < dim_prj; col++) {
      dist=MaxCol[a][col]-MinCol[a][col] + 1;
      //fprintf(stderr,"%f \n",VProj[a][col]/(float)dist);
      if (VProj[a][col] > 0 && dist > 0 && ((VProj[a][col]/(float)dist) >= threshold))
	sum+=VProj[a][col]*VProj[a][col];
      if (sum > max_sum) {	
	max_sum=sum;
	best_a=a-45;
      }
    }
    // fprintf(stderr,"%d\n",sum);
  }
  //  if (best_a==-10000) best_a=0;
  /* Liberamos la memoria de las proyectiones */
  for (i=0;i<=90;i++){
    free(VProj[i]);
    free(MaxCol[i]);
    free(MinCol[i]);
  }
  free(VProj);
  free(MaxCol);
  free(MinCol);

  return(-best_a);
}

float MVPV(gray ** image, int inicio_tramo, int fin_tramo, int rows){
  float var_max,var[91],sum,cont;
  int a, a_max;
  int ** VProj;
  int dim_prj;
  
  //dim_prj=(fin_tramo-inicio_tramo)*4;
  dim_prj=(fin_tramo-inicio_tramo)+2*rows ;
  VProj=get_projections(image,inicio_tramo, fin_tramo, rows);

  /* Calculamos las varianzas de las proyecciones*/
  /* y nos quedamos con el angulo que da el maximo */
  var_max=var_VProjection(VProj[0],dim_prj);
  var[0]=var_max;
  a_max=-45; /* ja veurem com ho arreglem */
  //  sum=0;
  for (a=-44;a<=45;a++){
    var[a+45]=var_VProjection(VProj[a+45],dim_prj);
    //    sum+=var[a+45]; 
    if (var_max < var[a+45]){
      var_max=var[a+45];
      a_max=a;
    }
  }
  if (verbosity == 2)
    for (a=-45;a<=45;a++)
      fprintf(stderr,"%d %f\n",a,var[a+45]);
  /* calculamos la media ponderada de todas las varianzas (centro de masas)
     que esten por debajo de un procentaje (threshold) del maximo */
  sum=0;
  cont=0;
  for (a=-45;a<=45;a++){
    if (var[a+45] >= threshold*var_max){
      sum+=var[a+45] * a;
      cont+=var[a+45];      
    }
  }  
  /* Liberamos la memoria de las proyectiones */
  for (a=0;a<=90;a++)
    free(VProj[a]);
  free(VProj);

  return(-sum/cont);
}

void usage(char * progName,char * cad){
  fprintf(stderr,"\nPerform handwritten text slant correction.\n\nUsage: %s %s\n",progName,cad);
  exit(1);
}



int main (int argc,char *argv[]) {
  
  FILE *ifd=stdin,*ofd=stdout;
  int *inicio_tramo,*fin_tramo, n_tramos, tramo;
  int row,col,rows_salida;
  float angulo;
  gray **salida;
  int version;
  int option;
  int N_pixels;
  char method='M';
  char usagecad[]=" [Options]\n \
          Options are:\n \
                 -i imput_image_file      by default stdin\n \
                 -o output_image_file     by default stdout\n \
                 -M method I|M|C          by defaul M(maxVar)\n \
                 -t threshold             by default 100%\n \
                 -b                       binary output mode\n \
                 -d                       demostration mode\n \
                 -h                       shows this information\n \
                 -v level                 verbosity mode\n \
          Methods: I -> Idiap, M -> maxVar, C -> Contourn length";

  version=NO_VERBOSA;
  while ((option=getopt(argc,argv,"hbdt:v:i:o:M:"))!=-1)
    switch (option)
      {
      case 'i':
	ifd=fopen(optarg,"r");
	break;
      case 'o':
	ofd=fopen(optarg,"w");
	break;
      case 't':
	threshold=atof(optarg)/100.0;
	break;
      case 'd':
	version=VERBOSA;
	break;
      case 'v':
	verbosity=atoi(optarg);
	break;
      case 'b':
	out_ascii=0;
	break;
      case 'M':
	method=optarg[0];
	break;
      case 'h':
	usage(argv[0],usagecad);
	break;
      default:
	usage(argv[0],usagecad);
      }

  if (ifd==NULL || ofd==NULL) {
    fprintf(stderr,"Unable to open the input and/or output files !\n");
    exit(-1);
  } 
      
  if (method != 'M' && method != 'I' && method != 'C'){
    usage(argv[0],usagecad);
    exit(-1);
  }
  frase=pgm_readpgm (ifd, &cols, &rows, &imaxval);
  if (ifd!=stdin) fclose(ifd);

  fprintf(stderr,"pgmslant: Method %c\n",method);

  /******************************************
   * Inicializacion de matrices auxiliares
   ******************************************/
  rotada=pgm_allocarray(cols+rows,rows);
  
  /***************************************
   * Deteccion de tramos 
   ***************************************/
   n_tramos=deteccionTramos(frase,&inicio_tramo,&fin_tramo,rows,cols,imaxval);
   if (n_tramos == -1){
       fprintf(stderr,"pgmslant: Number of word-clumps detected: %d\n",n_tramos);
       exit (-1);
   }
  /**************************************
   * Procesado de los tramos
   **************************************/ 
  if (verbosity==1)
    fprintf(stderr,"pgmslant: ------- %i word-clumps detected -------\n",n_tramos);
 
  for (tramo=0;tramo<n_tramos;tramo++) {
    if (fin_tramo[tramo]-inicio_tramo[tramo]>0) {
      if (verbosity)
	fprintf(stderr,"pgmslant: word-clump %d start: %d  end: %d\n",tramo,inicio_tramo[tramo],fin_tramo[tramo]);
      
      /*****************************************
       * Calculo del angulo por maxima varianza 
       * de la proyecci�n vertical
       *****************************************/
      if (method == 'I')
	angulo=IDIAP_slant(frase,inicio_tramo[tramo] ,fin_tramo[tramo],rows);
      else if(method == 'C')
	angulo=Javi_slant(frase,inicio_tramo[tramo] ,fin_tramo[tramo],rows);
      else
	angulo=MVPV(frase,inicio_tramo[tramo] ,fin_tramo[tramo],rows);

      shear(inicio_tramo[tramo],fin_tramo[tramo],(angulo)*M_PI/180.0);

      if (verbosity==1) 
	fprintf(stderr,"pgmslant: word-clump %d: averaged slant: %.0f\n",tramo,angulo+0.5);
    }
  }

  N_pixels=0;
  for (col=cols+rows-1;col >= 0;col--){
    for (row=0;row<rows;row++)
      if (rotada[row][col] < imaxval*0.5)
	N_pixels++;
    if (N_pixels > 5) 
      break;
  }

  col+=5;
  if (col>cols+rows) col=cols+rows;
  
  if (version==VERBOSA) {
      salida=pgm_allocarray(col,2*rows);
      copiar(frase,salida,cols,rows);
      copiar(rotada,salida,col,rows);
      //  copiar(frase,salida,col,rows,cols);
      //  copiar(rotada,salida,col,rows,col);
      rows_salida=2*rows;
      pgm_writepgm(ofd,salida,col,rows_salida,imaxval,out_ascii);
      pgm_freearray(salida,rows_salida);
  } else 
     pgm_writepgm(ofd,rotada,cols+rows,rows,imaxval,out_ascii);
  
  if (ofd!=stdout) fclose(ofd);
  free(inicio_tramo); free(fin_tramo);
  pgm_freearray(frase,rows);
  pgm_freearray(rotada,rows);
  return 0;
}
