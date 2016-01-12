#!/bin/bash

# Copyright (C) 1997 by Pattern Recognition and Human Language
# Technology Group, Technological Institute of Computer Science,
# Valencia University of Technology, Valencia (Spain).
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation.  This software is provided "as is" without express or
# implied warranty.

export PATH=$PATH:$HOME/bin:.

NAME=${0##*/}
if [ $# -ne 3 ]; then
  echo "Usage: $NAME <PBMSource-Dir> <featVect-Dir> <Dim>" 1>&2
  exit 1
fi

DORG=$1
DDEST=$2
DIM=$3

PTH=$(dirname $0)

[ -d $DORG ] || { echo "$DORG doesn't exist ..."; exit 1; }
[ -d $DDEST ] || mkdir $DDEST

for f in $DORG/*.pbm
do
  d=${f##*/}

  echo $f 1>&2

  pnmdepth 255 $f |	# Change to grey Level
  pgmmedian |		# Smooth the image
  pgmslope|		# Perform "Slope" correction
  pgmslant -M M |	# Perform "Slant" correction using Std. Desv.
  pgmnormsize -c 5 | 	# Perform size normalization
  pgmtextfea -c $DIM  > /tmp/tmp.fea 	# Compute feature extraction     
  pfl2htk /tmp/tmp.fea $DDEST/${d/pbm/fea} # Convert to HTK format
done
rm /tmp/tmp.fea
