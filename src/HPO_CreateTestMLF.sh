#!/bin/bash

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
  awk -v n=$F 'BEGIN{print "\"*/"n"\""}{for (i=1;i<=NF;i++) print $i}END{print "."}' $f
done >> $FDEST
