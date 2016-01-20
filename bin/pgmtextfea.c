
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

/* gcc -Wall -O2 -o pgmtextfea pgmtextfea.c libpgm.c -lm 2>error */

#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libpgm.h"

#ifndef PI
#define PI M_PI
#endif

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )

/*****************************************************************************/
#define Samples_Per_pixel 5
#define Sing_Value1 6       /* Local slope for blank regions */
#define Sing_Value2 9       /* Local slope for homogenius regions */
#define level_black 0.1     /* Coef. for detection of blank regions */
#define homog_grade 0.1     /* Coef. for detection of homogeneus regions */
#define num_neighbour 3     /* Min.num. of neighbour for the thining Loc.Slp.*/
/*****************************************************************************/

typedef struct datapgm {
  gray **image;
  int COLS;
  int ROWS;
  gray MAXVAL;
} pgm_image;

typedef struct datauser {
  int frec;
  int cells;
  float of_frec;
  float of_cells;
  char filter;
  int grey;
  int hder;
  int vder;
  int slope;
}input_user;

int verbose=0;

/*****************************************************************************/

int RoundUp(float x) { return (ceil(x)); }

int Round(float x) { return (x+0.5); }


/*****************************************************************************/

float *Create_vector(int dim)
{
  float *ARRAY;
  if ((ARRAY=(float *)malloc(dim*sizeof(float)))==NULL) {
    fprintf(stderr,"pgmtextfea: Insufficient memory space for the assignment of %i bytes\n",dim);
    exit(EXIT_FAILURE); 
  }
  return ARRAY;
}

/*****************************************************************************/

float **Create_matrix(int rows,int cols) 
{
  int i;
  float **ARRAY;
  if ((ARRAY=(float **)malloc(rows*sizeof(float *)))==NULL) {
    fprintf(stderr,"pgmtextfea: Insufficient memory space for the assignment of %i rows\n",rows);
    exit(EXIT_FAILURE); }
  for (i=0;i<rows;i++)
    if ((ARRAY[i]=(float *)malloc(cols*sizeof(float)))==NULL) {
      fprintf(stderr,"pgmtextfea: Insufficient memory space for the assignment of %i cols\n",cols);
      exit(EXIT_FAILURE); }
  return ARRAY;
}

/*****************************************************************************/

void Free_matrix(float **ARRAY,int rows) 
{
  int i;
  for (i=0;i<rows;i++) free(ARRAY[i]);
  free(ARRAY);
}

/*****************************************************************************/

float Func_filter(float x, float u, float s, char Type)
{
  float result;
 switch (Type) 
    {
    case 'g': 
      s=s/4;
      result=exp(-0.5*pow(((x-u)/s),2));
      break;
    case 'h':
      result=0.54+0.46*cos(PI*(x-u)/s);
      break; 
    default:
      result=1.0;
    }
  return(result);
}

/*****************************************************************************/

float sum_grey_level(float **Cell, float *yf, float *xf, int rows, int cols)
{
  int i,j;
  float result=0.0;
  for (i=0;i<rows;i++)
    for (j=0;j<cols;j++) result+=Cell[i][j]*yf[i]*xf[j];
  return result;
}

/*****************************************************************************/

float Derivate(float **Cell, int rows, int cols, float *x,\
               float *w, int maxval, char Type)
{
  float A, B, sumA, sumAX, sumB, sumBX, sumW;
  float *y=NULL, slope=0.0, aux;
  int i, j, n=0;

  switch (Type)
    {
    case 'h':
      y=Create_vector(cols);
      for (j=0;j<cols;j++) {
	aux=0.0;
	for (i=0;i<rows;i++) aux+=Cell[i][j];
	y[j]=aux/(rows*maxval);
      }
      n=cols;
      break; 
    case 'v':
      y=Create_vector(rows);
      for (i=0;i<rows;i++) {
	aux=0.0;
	for (j=0;j<cols;j++) aux+=Cell[i][j];
	y[i]=aux/(cols*maxval);
      }   
      n=rows;
      break;
    }

  sumA=sumAX=sumB=sumBX=sumW=0.0;
  
  for (i=0;i<n;i++) {
    A=w[i]*x[i];
    B=w[i]*y[i];
    sumA+=A;
    sumB+=B;
    sumW+=w[i];
    sumAX+=A*x[i];
    sumBX+=B*x[i];
  }

  slope=(sumB*sumA-sumW*sumBX)/(sumA*sumA-sumW*sumAX);

  free(y);
  return(atan(slope));
}

/*****************************************************************************/

float LocalSlope(float **Cell, int rows, int cols, float yc[], float xc[],\
		 float yf[], float xf[], int maxval)
{
  int i,j;
  float gray01,sumg01,sumx,sumy,sumxx,sumxy,sumyy;
  float scatterxx,scatterxy,scatteryy;
  float lambda1,lambda2,lambda,aux,aux2,slope;
  float m,s;

  sumx=sumxx=0.0;
  for (i=0;i<rows;i++)
    for (j=0;j<cols;j++) {
      sumx+=Cell[i][j];
      sumxx+=Cell[i][j]*Cell[i][j];
    } 
  m=sumx/(rows*cols);
  s=sqrt(sumxx/(rows*cols)-m*m);

  /* if (verbose) fprintf(stderr,"media=%f desv.=%f\n",m,s);*/

 
  if ((m/maxval)<level_black) slope=Sing_Value1;
  else if (s<homog_grade) slope=Sing_Value2;
  else {
    sumg01=sumx=sumy=sumxx=sumxy=sumyy=0.0;
    for (i=0;i<rows;i++)
      for (j=0;j<cols;j++) {
	gray01=(Cell[i][j]*yf[i]*xf[j])/maxval;
	sumg01+=gray01;
	sumx+=gray01*xc[j];
	sumy+=gray01*yc[i];
	sumxx+=gray01*xc[j]*gray01*xc[j];
	sumxy+=gray01*xc[j]*gray01*yc[i];
	sumyy+=gray01*yc[i]*gray01*yc[i];
      }

    scatterxx=sumxx-sumx*sumx/sumg01;
    scatterxy=sumxy-sumx*sumy/sumg01;
    scatteryy=sumyy-sumy*sumy/sumg01;
    
    aux=scatterxx+scatteryy;
    aux2=sqrt(aux*aux-4*(scatterxx*scatteryy-scatterxy*scatterxy));
    lambda1=(aux+aux2)/2.0;
    lambda2=(aux-aux2)/2.0;
    lambda=MAX(lambda1,lambda2);
    
    aux=(lambda-scatterxx-scatterxy)/(scatteryy+scatterxy-lambda);
    if (aux==aux) /* number, including -Inf and Inf */
      slope=atan(MIN(MAX(aux,-FLT_MAX),FLT_MAX));
    else /* not a number (NAN) */
      slope=Sing_Value2;
  }

  return(slope);
}

/*****************************************************************************/

void Thining_for_LocalSlope(float **Cell, int rows, int cols)
{
  int i, j, n, flag=1;
  float sum;

  while (flag) {
    flag=0;
    for (i=0;i<rows;i++)
      for (j=0;j<cols;j++) 
	if (Cell[i][j]==Sing_Value2) {
	  sum=0.0; n=0;	  
	  if (i!=0) { 
	    if ((j!=0) && (Cell[i-1][j-1]!=Sing_Value2)\
		&& (Cell[i-1][j-1]!=Sing_Value1)) {sum+=Cell[i-1][j-1]; n++;}
	    if ((Cell[i-1][j]!=Sing_Value2) && (Cell[i-1][j]!=Sing_Value1))
	      {sum+=Cell[i-1][j]; n++;}
	    if ((j!=cols-1) && (Cell[i-1][j+1]!=Sing_Value2)\
		&& (Cell[i-1][j+1]!=Sing_Value1)) {sum+=Cell[i-1][j+1]; n++;} }
	  if ((j!=0) && (Cell[i][j-1]!=Sing_Value2)\
	      && (Cell[i][j-1]!=Sing_Value1)) {sum+=Cell[i][j-1]; n++;}
	  if ((j!=cols-1) && (Cell[i][j+1]!=Sing_Value2)\
	      && (Cell[i][j+1]!=Sing_Value1)) {sum+=Cell[i][j+1]; n++;}
	  if (i!=rows-1) {
	    if ((j!=0) && (Cell[i+1][j-1]!=Sing_Value2)\
		&& (Cell[i+1][j-1]!=Sing_Value1)) {sum+=Cell[i+1][j-1]; n++;}
	    if ((Cell[i+1][j]!=Sing_Value2) && (Cell[i+1][j]!=Sing_Value1))
	      {sum+=Cell[i+1][j]; n++;}
	    if ((j!=cols-1) && (Cell[i+1][j+1]!=Sing_Value2)\
		&& (Cell[i+1][j+1]!=Sing_Value1)) {sum+=Cell[i+1][j+1]; n++;} }
	  if (n>=num_neighbour) {Cell[i][j]=sum/n; flag=1;}
	}
  }
}

/*****************************************************************************/

void Draw_Grey(float **Cell, int rows, int cols, int maxval)
{
  int i, j;
  fprintf(stderr,"****************************************************\n");
  fprintf(stderr,"P2\n%d %d\n%d\n",cols,rows,maxval);
  fprintf(stderr,"#GREY LEVEL DATAS:\n");
  fprintf(stderr,"#(Use pgm format)\n");  
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) 
      fprintf(stderr,"%d ",(int)(maxval-Cell[i][j]*maxval/100));
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"****************************************************\n\n");
}

/*****************************************************************************/

void Draw_Der(float **Cell, int rows, int cols, int maxval, char TYPE)
{
  int i, j;
  fprintf(stderr,"****************************************************\n");
  fprintf(stderr,"P2\n%d %d\n%d\n",cols,rows,maxval);
  fprintf(stderr,"#Derivate %c:\n",TYPE);
  fprintf(stderr,"#(Use pgm format)\n");  
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) 
      fprintf(stderr,"%d ",(int)(((Cell[i][j]+PI/2)/PI)*maxval));
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"****************************************************\n\n");
}

/*****************************************************************************/
void Draw_Local_slope(float **Cell, int rows, int cols, float size_y,\
		      float size_x, float pos_y, float pos_x)
{
  int i, j;
  float x, y;

  fprintf(stderr,"****************************************************\n");
  fprintf(stderr,"#COORDENATES POINTS OF LOCAL SLOPE IN EACH CELL:\n");
  fprintf(stderr,"#(For GNUPLOT Application)\n");
  for (j=0;j<cols;j++)
    for (i=0;i<rows;i++) {
      fprintf(stderr,"%f %f\n\n",pos_x*(1+2*j),-pos_y*(1+2*i));
      if ((Cell[i][j]!=Sing_Value1) && (Cell[i][j]!=Sing_Value2)) {
	if (fabs(tan(Cell[i][j]))<1) {
	  x=-size_x/2+pos_x*(1+2*j);
	  y=tan(Cell[i][j])*(-size_x/2)+pos_y*(1+2*i);
	  fprintf(stderr,"%f %f\n",x,-y);
	  x=size_x/2+pos_x*(1+2*j);
	  y=tan(Cell[i][j])*(size_x/2)+pos_y*(1+2*i);
	  fprintf(stderr,"%f %f\n\n",x,-y);
	}
	else {
	  x=(1/tan(Cell[i][j]))*(-size_y/2)+pos_x*(1+2*j);
	  y=-size_y/2+pos_y*(1+2*i);
	  fprintf(stderr,"%f %f\n",x,-y);
	  x=(1/tan(Cell[i][j]))*(size_y/2)+pos_x*(1+2*j);
	  y=size_y/2+pos_y*(1+2*i);
	  fprintf(stderr,"%f %f\n\n",x,-y);
	}
      }
    }
  fprintf(stderr,"****************************************************\n");
}

/*****************************************************************************/

float **Process_Image(input_user *iuser, pgm_image *idata)
{
  float size_x_cell, size_y_cell, pos_x_cell, pos_y_cell;
  float x_cell, y_cell, Normalization_Value=0.0;
  float **getfeatures=NULL, **sample_cell=NULL;
  float *X_coord=NULL, *X_filt=NULL, *Y_coord=NULL, *Y_filt=NULL;
  int i, j, h, k, sub_frec_x, sub_frec_y;
  int pos_hder, pos_vder, pos_slope, num_features;
   
  size_x_cell=((float)idata->COLS/(float)iuser->frec)*(1+2*iuser->of_frec);
  pos_x_cell=0.5*((float)idata->COLS/(float)iuser->frec);
  sub_frec_x=RoundUp(size_x_cell*Samples_Per_pixel);

  size_y_cell=((float)idata->ROWS/(float)iuser->cells)*(1+2*iuser->of_cells);
  pos_y_cell=0.5*((float)idata->ROWS/(float)iuser->cells);
  sub_frec_y=RoundUp(size_y_cell*Samples_Per_pixel);


  if (verbose) fprintf(stderr,"SUB-SAMPLE DATAS:\n\
Sub-sample frec.X=%d\n\
Sub-sample frec.Y=%d\n\
Size X cell(in Pixels)=%.2f\n\
Inic. Pos. X cell=%.2f\n\
Size Y cell(in Pixels)=%.2f\n\
Inic. Pos. Y cell=%.2f\n\n\n",sub_frec_x,sub_frec_y,size_x_cell,pos_x_cell,\
		       size_y_cell,pos_y_cell);
  
  pos_hder=pos_vder=pos_slope=num_features=0;
  pos_hder=iuser->grey;
  pos_vder=pos_hder+iuser->hder;
  pos_slope=pos_vder+iuser->vder;
  num_features=pos_slope+iuser->slope;

  getfeatures=Create_matrix(num_features*iuser->cells,iuser->frec);
  X_coord=Create_vector(sub_frec_x);
  X_filt=Create_vector(sub_frec_x);
  Y_coord=Create_vector(sub_frec_y);
  Y_filt=Create_vector(sub_frec_y);
  sample_cell=Create_matrix(sub_frec_y,sub_frec_x);
  

  for (k=0;k<sub_frec_x;k++) {
    X_coord[k]=size_x_cell*(((float)k/sub_frec_x)+(0.5/sub_frec_x)-0.5);
    X_filt[k]=Func_filter(X_coord[k],0,size_x_cell,iuser->filter);
  }
  for (h=0;h<sub_frec_y;h++) {
    Y_coord[h]=size_y_cell*(((float)h/sub_frec_y)+(0.5/sub_frec_y)-0.5);
    Y_filt[h]=Func_filter(Y_coord[h],0,size_y_cell,iuser->filter);
  }

  for (k=0;k<sub_frec_x;k++) 
    for (h=0;h<sub_frec_y;h++) Normalization_Value+=Y_filt[h]*X_filt[k];
  Normalization_Value*=idata->MAXVAL;
 
  
  for (j=0;j<iuser->frec;j++)
    for (i=0;i<iuser->cells;i++) {
      for (k=0;k<sub_frec_x;k++) {
	x_cell=X_coord[k]+pos_x_cell*(1+2*j);
	for (h=0;h<sub_frec_y;h++) {
	  y_cell=Y_coord[h]+pos_y_cell*(1+2*i);
	  if (x_cell<0 || x_cell>=idata->COLS || y_cell<0 ||\
	      y_cell>=idata->ROWS) sample_cell[h][k]=0;
	  else 
	    sample_cell[h][k]=(idata->MAXVAL-\
			       idata->image[(int)y_cell][(int)x_cell]);
          /*printf("%d %d %f %f %f\n",h,k,y_cell,x_cell,sample_cell[h][k]);*/ 
	}
      }     
      if (iuser->grey)
	getfeatures[i][j]=100*\
	  sum_grey_level(sample_cell,Y_filt,X_filt,sub_frec_y,sub_frec_x)/\
	  Normalization_Value;
      
      if (iuser->hder) 
	getfeatures[i+pos_hder*iuser->cells][j]=\
	  Derivate(sample_cell,sub_frec_y,sub_frec_x,X_coord,X_filt,\
		   idata->MAXVAL,'h');

      if (iuser->vder)
	getfeatures[i+pos_vder*iuser->cells][j]=\
	  Derivate(sample_cell,sub_frec_y,sub_frec_x,Y_coord,Y_filt,\
		   idata->MAXVAL,'v');

      if (iuser->slope)
	getfeatures[i+pos_slope*iuser->cells][j]=\
	  LocalSlope(sample_cell,sub_frec_y,sub_frec_x,Y_coord,X_coord,\
		     Y_filt,X_filt,idata->MAXVAL);
    }
  
  /*Derive1(getfeatures,iuser->cells,iuser->frec);*/


  if (iuser->slope)
    Thining_for_LocalSlope(&(getfeatures[pos_slope*iuser->cells]),\
			   iuser->cells,iuser->frec);
  

  if (verbose) {
    if (iuser->grey)
      Draw_Grey(getfeatures,iuser->cells,iuser->frec,idata->MAXVAL);
    if (iuser->hder) 
      Draw_Der(&(getfeatures[pos_hder*iuser->cells]),iuser->cells,\
	       iuser->frec,idata->MAXVAL,'X');
    if (iuser->vder)
      Draw_Der(&(getfeatures[pos_vder*iuser->cells]),\
	       iuser->cells,iuser->frec,idata->MAXVAL,'Y');    
    if (iuser->slope)
      Draw_Local_slope(&(getfeatures[pos_slope*iuser->cells]),iuser->cells,\
		       iuser->frec,size_y_cell,size_x_cell,size_y_cell/2,\
		       size_x_cell/2);
  }

  Free_matrix(sample_cell,sub_frec_y); 
  free(X_coord); free(Y_coord);
  free(X_filt); free(Y_filt);
  
  return(getfeatures);
}

void usage(char * progName,char * cad){
  fprintf(stderr,"Usage: %s %s\n",progName,cad);
  exit(1);
}

/*****************************************************************************/

int main(int argc, char *argv[])
{
  char* usagecar = "[<options below>] [<filepgm>]\n\n\
  -i pgmfile      by default stdin\n\
  -c <num:int>    number of cells per frame (def. 20)\n\
  -f <u|h|g>      type of filter--> u:Uniform h:Haming g:Gaussian (def. g)\n\
  -V <num:float>  overlapping factor of cells (def. 2)\n\
  -O <num:float>  overlapping horizontal factor of frames (def. 2)\n\
  -F <num:float>  scaling factor of frecuency (def. 1)\n\
  -s              include slope (excluded)\n\
  -h              this message\n\
  -v              verbose mode\n\n";

  int i, j, number_feature,option;
  float asp_rat, frec_factor=1.0, **Attributes=NULL;

  pgm_image idata;
  input_user udata={1,20,2,2,'g',1,1,1,0};
/*
typedef struct datauser {
  int frec;
+  int cells;
+  float of_frec;
+  float of_cells;
+  char filter;
+  int grey;
+  int hder;
+  int vder;
+  int slope;
}input_user;

*/
  FILE *ifd=stdin, *ofd=stdout;

 while ((option=getopt(argc,argv,"shvc:i:o:f:V:O:F:"))!=-1)
    switch (option)
      {
      case 'i':
	 ifd = fopen(optarg,"r");
	 break;
      case 'o':
          ofd = fopen(optarg,"w");
	  break;
      case 'c': 
	udata.cells=atoi(optarg);
	break;
      case 'f': 
	udata.filter=optarg[0];
	break;
      case 'V':
	udata.of_cells=atof(optarg);
	break;
      case 'O':
	udata.of_frec=atof(optarg);
	break;
      case 'F':
	frec_factor=atof(optarg);
	break;
      case 's':
	udata.slope=1;
	break;
      case 'v':
	verbose=1;
	break;
      case 'h':
      default:
	usage(argv[0], usagecar );
      }
 
  if (ifd==NULL ) {
    fprintf(stderr,"Unable to open the given input file !\n");
    exit(-1);
  }

  idata.image=pgm_readpgm(ifd,&idata.COLS,&idata.ROWS,&idata.MAXVAL);
  if (ifd!=stdin) fclose(ifd);

  asp_rat=(float)idata.COLS/(float)idata.ROWS;
  udata.frec=Round(frec_factor*asp_rat*udata.cells);

  /*if (udata.frec>idata.COLS || udata.cells>idata.ROWS) pm_error("ERROR: frecuency|cells is bigger than the number of columns|rows\nof the original image.");*/
  
  
  if (verbose) fprintf(stderr,\
	  "DATA USER:           DATA IMAGE:\n\
Cells =%2d            Rows =%4d\n\
Frec.=%2d             Cols =%4d\n\
Ov.Frec.= %2.2f       Asp.Rat.=%2.2f\n\
Ov.Cells = %2.2f      Max_Grey =%2d\n\
Scal.Frec.Fact.= %2.2f\n\
Filter = %c\n\
Flag of greys = %d\n\
Flag of horinzal derivates = %d\n\
Flag of vertical derivates = %d\n\
Flag of local slopes = %d\n\n\n",udata.cells,idata.ROWS,udata.frec,\
		       idata.COLS,udata.of_frec,asp_rat,udata.of_cells,\
		       idata.MAXVAL,frec_factor,udata.filter,udata.grey,\
		       udata.hder,udata.vder,udata.slope);

  number_feature=udata.grey+udata.hder+udata.vder+udata.slope;
  Attributes=Process_Image(&udata,&idata);


  for (j=0;j<udata.frec;j++) {
    for (i=0;i<number_feature*udata.cells;i++)
      fprintf(ofd,"%e ",Attributes[i][j]);
    fprintf(ofd,"\n");
  }
  if (ofd!=stdout) fclose(ofd);

  Free_matrix(Attributes,number_feature*udata.cells);
  pgm_freearray(idata.image,idata.ROWS);
  return(0);
}
