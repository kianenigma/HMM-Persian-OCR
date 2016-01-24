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


NAME=${0##*/}
trap "rm -f tmp_samples tmp.arpa 2>/dev/null" EXIT
export LC_NUMERIC=C

if [ $# -ne 3 ]; then
  echo "Usage: $NAME <input-SamplesDB> <output-FileDic> <output-FileNet>" 1>&2
  exit 1
fi

iFILE=$1
oDIC=$2
oNET=$3
[ -e $iFILE ] || { echo "$iFILE doesn't exist ..."; exit 1; }


awk '{for (i=2;i<NF;i++) print $i}' $iFILE |
sort |
uniq -c |
awk 'BEGIN{prev="";N=0}
     {
       e=gensub(/@/,"","g",$2);
       if (e!=prev){
         for (i=1;i<=N;i++) {
	   printf("%s\t%f\t",prev,n[i]/sum);
	   l=length(p[i]);
	   for (j=1;j<=l;j++) printf("%c ",substr(p[i],j,1));
	   printf("\n");
	 }
	 sum=0;
	 N=0;
	 prev=e;
       }
       sum+=$1;
       N++;
       p[N]=$2;
       n[N]=$1
     }
     END{for (i=1;i<=N;i++) {
           printf("%s\t%f\t",prev,n[i]/sum);
	   l=length(p[i]);
	   for (j=1;j<=l;j++) printf("%c ",substr(p[i],j,1));
	   printf("\n")
	 }
     }' > $oDIC
#echo "@  []  @"  >> $oDIC    # For white space
echo "<s>  []"   >> $oDIC    # For initial symbol
echo "</s>  []"  >> $oDIC    # For ending symbol

sed -r "s/@//g;s/<s> //g;s/<\\\s>//g" $iFILE > tmp_samples
ngram-count -text tmp_samples -lm tmp.arpa -order 2 2>/dev/null
awk '{print $1}' Dictionary |sort -u > wordlist
HBuild -n tmp.arpa -s "<s>" "</s>" $oDIC $oNET
