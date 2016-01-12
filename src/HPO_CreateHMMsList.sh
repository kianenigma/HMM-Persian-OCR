#!/bin/bash


NAME=${0##*/}
if [ $# -ne 2 ]; then
  echo "Usage: $NAME <input-Dir> <output-File>" 1>&2
  exit 1
fi

DORG=$1
FDEST=$2
[ -d $DORG ] || { echo "$DORG doesn't exist ..."; exit 1; }


cat $DORG/* |
sed -r "s/[ ]/@/g" | 
sed -r "s/./& /g; s/[ ]/\n/g" | sed "/^[ ]*$/d" | sort -u > $FDEST

