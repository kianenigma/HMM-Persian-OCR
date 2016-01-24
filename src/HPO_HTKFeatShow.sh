#!/bin/bash

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

export LC_NUMERIC=C

PROG=${0##*/}
if [ $# -gt 1 ]; then echo "Usage: $PROG [<features-file>]"; exit; fi

TMP=./$$.aux
trap "rm $TMP* 2>/dev/null" EXIT

#cat $1 > $TMP
#sed -r 0,/^[Dd]ata$/d $1 > $TMP
HList -r -h  $1|awk 'BEGIN{getline;getline;getline}{print}' > $TMP
maxval=255
Wide=$(awk 'END{print NR}' $TMP)
Hight=$(awk 'END{print int(NF/3)}' $TMP)

echo $Wide $Hight $maxval

awk -v h=$Hight -v w=$Wide -v mx=$maxval '
BEGIN{printf("P2\n%d %d\n%d\n",w,h,mx)}
{for (i=1;i<=h;i++) C[i,NR]=$i}
END{for (i=1;i<=h;i++) {for (j=1;j<=w;j++)
printf("%d ",int(mx*((100-C[i,j])/100)));
printf("\n")}}' $TMP > tmp1.pgm

awk -v h=$Hight -v w=$Wide -v mx=$maxval -v pi=3.141516 '
BEGIN{min=255;max=0;printf("P2\n%d %d\n%d\n",w,h,mx)}
{
  for (i=1;i<=h;i++){
    C[i,NR]=$(i+h)
    if (C[i,NR] < min){
         min=C[i,NR];
      }
      if (C[i,NR] > max){
         max=C[i,NR];
      }
  }
}
END{for (i=1;i<=h;i++) {for (j=1;j<=w;j++)
printf("%d ",mx*(C[i,j]-min)/(max-min));
#printf("%d ",int(mx*((pi/2+C[i,j])/pi)));
printf("\n")}}' $TMP > tmp2.pgm

awk -v h=$Hight -v w=$Wide -v mx=$maxval -v pi=3.141516 '
BEGIN{min=255;max=0;printf("P2\n%d %d\n%d\n",w,h,mx)}
{
  for (i=1;i<=h;i++){
      C[i,NR]=$(i+2*h)
      if (C[i,NR] < min){
         min=C[i,NR];
      }
      if (C[i,NR] > max){
         max=C[i,NR];
      }
  }
}
END{for (i=1;i<=h;i++) {for (j=1;j<=w;j++)
printf("%d ",mx*(C[i,j]-min)/(max-min));
printf("\n")}}' $TMP > tmp3.pgm

pnmcat -tb tmp1.pgm tmp2.pgm tmp3.pgm | display &
