
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 16384

char *prog;
FILE *arch_dest;

/***************************************************************************/
void write_long(long data) {
	fwrite( ((char*) &data) + 3, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data) + 2, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data) + 1, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data)    , sizeof(char), 1, arch_dest);
}

/***************************************************************************/
void write_int(int data) {
	fwrite( ((char*) &data) + 1, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data)    , sizeof(char), 1, arch_dest);
}

/***************************************************************************/
void write_float(float data) {
	fwrite( ((char*) &data) + 3, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data) + 2, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data) + 1, sizeof(char), 1, arch_dest);
	fwrite( ((char*) &data)    , sizeof(char), 1, arch_dest);
}

/***************************************************************************/
void usage(void)
{
  fprintf(stderr,"\nusage: %s <DATA-File> <HTK-File>\n\
  Header:  nSamples=number_of_vectors\n\
           sampPeriod=50 (arbitrary)\n\
           sSize=4*PUNTOS (float, vector dimension)\n\
           parmKind=9 (USER)\n\n",prog);
  exit(1);
}

int main (int argc, char *argv[]) 
{
	int i, j, cont, puntos;
	char linea[MAX_LINE_LENGTH], *np, *endp;
	FILE *arch_orig=NULL;

	// Cabecera
	long nSamples, sampPeriod;
	int sampSize, parmKind;
		
	prog=argv[0];
	if ((argc<2) || (argc>3)) {
		usage();
		exit(1);
	}
		
	if (strncmp(argv[1],"-help",2)==0) usage();
    	// else if (strncmp(argv[1],"-",1)==0) arch_orig=stdin;
	arch_orig=fopen(argv[1],"r");
	if (arch_orig==NULL) {
		printf("\nERROR: can't open input file\n\n");
		exit(1);
	}
	arch_dest=fopen(argv[2],"w");
	if (arch_dest==NULL) {
		printf("\nERROR: can't open output file\n");
		exit(1);
	}
	
	cont=0;
	while (fgets(linea,MAX_LINE_LENGTH,arch_orig)!=NULL) cont++;
	rewind(arch_orig);

	for (i=0,strtod(np=linea,&endp);np!=endp;i++,strtod(np=endp,&endp));
	puntos=i;
	fprintf(stderr,"num. Vect=%d  num. Comp=%d\n",cont,puntos);

	nSamples=cont;
	write_long(nSamples);
	//sampPeriod=50;
	sampPeriod=1;
	write_long(sampPeriod);
	sampSize=4*puntos;
	write_int(sampSize);
	parmKind=9;
	//parmKind=6;
	write_int(parmKind);
	for (i=0;i<cont;i++) {
	    fgets(linea,MAX_LINE_LENGTH,arch_orig);
	    np=linea;
	    for (j=0;j<puntos;j++) {
		write_float(strtod(np,&endp));
		np=endp;
	    }
	}
	if (fclose(arch_orig)!=0) {
		printf("\nERROR: Problema al cerrar el archivo origen.\n\n");
		exit(1);
		}
	if (fclose(arch_dest)!=0) {
		printf("\nERROR: Problema al cerrar el archivo destino.\n\n");
		exit(1);
		}
        exit(0);
}
