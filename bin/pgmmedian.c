/*
 * M. Kopp, Efficient 3x3 Median Filter Computations, 1994
 */

/*

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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <float.h>
#include <sys/param.h>
#include <unistd.h>
#include "libpgm.h"



char *progname;

void usage(void) {
  fprintf(stderr, "\nApply a median filter on the image to reduce noise. \n\n");
  fprintf(stderr, "usage: %s [-h] [-e] [-f] [-o file] [-k n] file\n", progname);
  fprintf(stderr, "\t-e n\t\teven extension (default odd)\n");
  fprintf(stderr, "\t-f\t\tfast algorithm for 3x3 kernel\n");
  fprintf(stderr, "\t-h\t\tprint usage\n");
  fprintf(stderr, "\t-o file \toutput image file\n");
  fprintf(stderr, "\t-k n\t\tfiltering kernel size NxN (default 3)\n");
  exit(0);
}

#define SWAP_GRAY(a, b) { gray t; t = b; b = a; a = t; }
#define SWAP_SLICE(a, b) { gray *t; t = b; b = a; a = t; }

#ifndef MAX
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif

#ifndef MIN
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

#ifndef sqr
#define sqr(X) ((X) * (X))
#endif


inline void SORT_SLICE(gray a[]) {
  if (a[0] > a[1]) SWAP_GRAY(a[0], a[1]);
  if (a[1] > a[2]) SWAP_GRAY(a[1], a[2]);
  if (a[0] > a[1]) SWAP_GRAY(a[0], a[1]);
}

inline void MERGE_SLICE(gray a[], gray b[], gray bc[]) {
  int i, j, k;

  for (i = 0, j = 0, k = 0; i < 6; i++)
    if (j > 2)
      bc[i] = b[k++];
    else if (k > 2)
      bc[i] = a[j++];
    else
      bc[i] = (a[j] < b[k]) ? a[j++] : b[k++];
}

inline gray CALC_MEDIAN(gray x[], gray bc[]) {
  int i, j, k, m;

  for (i = 0, j = 0, k = 1; i < 4; i++)
    if (j > 2)
      m = bc[k++];
    else
      m = (x[j] < bc[k]) ? x[j++] : bc[k++];

  return m;
}

int compare_gray(const void *a, const void *b) {
  if (*(gray *)a < *(gray *)b)
    return -1;
  else if (*(gray *)a > *(gray *)b)
    return 1;
  else
    return 0;
}

void median(gray **output, gray **input, int cols, int rows, int kernel) {
  int col, row, n;
  gray *tmp;

  tmp = (gray*)malloc(sqr(kernel) * sizeof(gray));

  for (row = 0; row < rows; row++) {
    for (col = 0; col < cols; col++) {
      int i, j;

      n = 0;
      for (i = row; i < row + kernel; i++)
        for (j = col; j < col + kernel; j++)
          tmp[n++] = input[i][j];

      qsort(tmp, n, sizeof(gray), compare_gray);
      output[row][col] = tmp[n/2];
    }
  }

  free(tmp);
}

int main(int argc, char *argv[]) {
  gray maxval;
  gray **input, **output;

  int cols, rows;
  int col, row;

  int kernel = 3;
  int fast = 0;
  int extension_even = 0;

  FILE *in = stdin;
  FILE *out = stdout;

  int c;

  progname = argv[0];

  while ((c = getopt(argc, argv, "h?o:k:fe")) != EOF) {
  switch (c) {
     case 'h':
     case '?':
       usage();
       break;
     case 'f':
       fast = 1;
       break;
     case 'e':
       extension_even = 1;
       break;
     case 'k':
       kernel = atoi(optarg);
       if (kernel <= 0) {
         fprintf(stderr, "%s: kernel size %d out of range\n", progname, kernel);
         exit(1);
       }
       break;
     case 'o':
       if ((out = fopen(optarg, "wb")) == NULL) {
         fprintf(stderr, "%s: unable to open output file %s\n", progname, optarg);
         exit(1);
       }
       break;
   }
 }

  input=pgm_readpgm(in, &cols, &rows, &maxval);
  fclose(in);
  output = pgm_allocarray(cols, rows);

  // just copy border pixels to output
  // pose les vores a blanc
  for (col = 0; col < cols; col++) {
    output[0][col] = 255;
    output[rows - 1][col] = 255;
    //    output[0][col] = input[0][col];
    //output[rows - 1][col] = input[rows - 1][col];
  }

  for (row = 1; row < rows - 1; row++) {
    output[row][0] = 255;
    output[row][cols - 1] = 255;
    //    output[row][0] = input[row][0];
    //output[row][cols - 1] = input[row][cols - 1];
  }

  if (kernel == 3 && fast) {

  for (row = 1; row < rows - 1; row++) {
    gray _a[3], _b[3], _c[3], _d[3], bc[6];
    gray *a = _a, *b = _b, *c = _c, *d = _d;

    a[0] = input[row - 1][0];
    a[1] = input[row][0];
    a[2] = input[row + 1][0];
    SORT_SLICE(a);

    b[0] = input[row - 1][1];
    b[1] = input[row][1];
    b[2] = input[row + 1][1];
    SORT_SLICE(b);

    for (col = 1; col < cols - 1; col += 2) {

      c[0] = input[row - 1][col + 1];
      c[1] = input[row][col + 1];
      c[2] = input[row + 1][col + 1];

      d[0] = input[row - 1][col + 2];
      d[1] = input[row][col + 2];
      d[2] = input[row + 1][col + 2];

      // sort slice c and d
      // slice a and b are already sorted
      SORT_SLICE(c);
      SORT_SLICE(d);

      // merge slice b and c producing slice bc
      MERGE_SLICE(b, c, bc);

      // merge slice bc and a to calculate median 1
      output[row][col] = CALC_MEDIAN(a, bc);

      // merge slice bc and d to calculate median 2
      output[row][col + 1] = CALC_MEDIAN(d, bc);

      SWAP_SLICE(a, c);
      SWAP_SLICE(b, d);
    }
  }
  }
  else {
    int kernel2 = kernel / 2;
    int i, j;
    gray **ext_input = pgm_allocarray(cols + 2 * kernel2, rows + 2 * kernel2);

    for (row = 0; row < rows ; row++)
      memcpy(ext_input[row + kernel2] + kernel2, input[row], cols*sizeof(gray));

    if (extension_even) {
    // even symmetric extension
      for (i = kernel2; i < 2 * kernel2; i++) {
        for (j = kernel2 + 1; j < cols + kernel2; j++) {
          // top
          ext_input[2*kernel2 - 1 - i][j] = ext_input[i][j];
          // bottom
          ext_input[rows + i][j] = ext_input[2*kernel2 + rows - 1 - i][j];
        }
        for (j = 1; j < rows; j++) {
          // left
          ext_input[j][2*kernel2 - 1 - i] = ext_input[j][i];
          // right
          ext_input[j][cols + i] = ext_input[j][2*kernel2 + rows - 1 - i];
        }
      }
    }
    else {
    // odd symmetric extension
      for (i = kernel2; i < 2 * kernel2; i++) {
        for (j = kernel2 + 1; j < cols + kernel2; j++) {
          // top
          ext_input[2*kernel2 - 1 - i][j] = ext_input[1 + i][j];
          // bottom
          ext_input[rows + i][j] = ext_input[2*kernel2 + rows - 2 - i][j];
        }
        for (j = 1; j < rows; j++) {
          // left
          ext_input[j][2*kernel2 - 1 - i] = ext_input[j][1 + i];
          // right
          ext_input[j][cols + i] = ext_input[j][2*kernel2 + rows - 2 - i];
        }
      }
    }


    median(output, ext_input, cols, rows, kernel);
    pgm_freearray(ext_input, rows + 2 * kernel2);
  }

  pgm_writepgm(out, output,cols, rows, maxval, 1);

  pgm_freearray(input, rows);
  pgm_freearray(output, rows);

  exit(0);
}
