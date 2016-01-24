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
if [ $# -ne 2 ]; then
  echo "Usage: $NAME <input-Dir> <output-File> " 1>&2
  exit 1
fi

DORG=$1
FDEST=$2

echo "#!MLF!#" > $FDEST

for f in $DORG/*.txt
do
  F=$(basename $f | sed 's/\.txt/.lab/g')
  sed -r "s/ +/@/g" $f |
  awk -v n=$F 'BEGIN{print "\"*/"n"\""}{for (i=1;i<=NF;i++) for (j=1;j<=length($1); j++) print substr($i,j,1)}END{print "."}'
done >> $FDEST
