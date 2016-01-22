#!/bin/bash

export PATH=$PATH:$HOME/bin:$HOME/scripts:.

NAME=${0##*/}
if [ $# -ne 7 ]; then
  echo "Usage: $NAME <train.lst> <HMMs-Dir> <proto> <samplesLabels.mlf> <HMMsList>"
  exit
fi

LSTTRA=$1
DIRHMM=$2
PROTO=$3
LABELS=$4
L_HMMS=$5

S=$(grep -i "<Numstates>" $PROTO | cut -d " " -f 2)
D=$(grep -i "<VecSize>" proto | cut -d " " -f 3)
NUM_ITER=$6
#NUM_ITER=4
#NUM_GAUSS=32
NUM_GAUSS=$7


FLAGSHEREST="-A -T 1 -v 0.01 -m 3 "

g=1
while [ $g -le $NUM_GAUSS ]; do
  [ -d $DIRHMM/hmm_$g ] && { g=$[g*2]; continue; }
  mkdir -p $DIRHMM/hmm_$g || { echo "ERROR: Could not create HMM dirs"; exit 1; }
  g=$[g*2]
done
[ -d $DIRHMM/hmm_0 ] || mkdir -p $DIRHMM/hmm_0 || { echo "ERROR: Could not create HMM dirs"; exit 1; }

echo "Initialization..." 1>&2
HCompV -A -T 1 -f 0.01 -m -S $LSTTRA -M $DIRHMM/hmm_0 $PROTO
head -3 $DIRHMM/hmm_0/$PROTO > $DIRHMM/hmm_1/Macros_hmm
cat $DIRHMM/hmm_0/vFloors >> $DIRHMM/hmm_1/Macros_hmm
for i in `cat ${L_HMMS}`; do 
  tail -n +4 $DIRHMM/hmm_0/$PROTO | sed "s/$PROTO/$i/g" >> $DIRHMM/hmm_1/Macros_hmm
done


echo "First iteration" 1>&2
k=1
while [ $k -le $NUM_ITER ]; do
  echo "Re-estimation $k of hmm_1 with 1 gaussian ..."  1>&2
  HERest $FLAGSHEREST -S $LSTTRA -I $LABELS -H $DIRHMM/hmm_1/Macros_hmm $L_HMMS
  k=$[k+1]
done

g=2
while [ $g -le $NUM_GAUSS ]; do
  echo "Creating hmm with $g gaussians ..." 1>&2
  echo "MU $g {*.state[2-$[S-1]].mix}" > mult_gauss_$g
  echo "Creating $k th train data Model" > /home/kian/Desktop/htkocr/work/Persian/News/log 
  HHEd -A -H $DIRHMM/hmm_$[g/2]/Macros_hmm -M $DIRHMM/hmm_$g mult_gauss_$g $L_HMMS
  rm mult_gauss_$g
  echo "Re-estimation of hmm_$g with $g gaussians ..."  1>&2
  k=1
  while [ $k -le $NUM_ITER ]; do
    echo "Re-estimation $k of hmm_$g with $g gaussians ..."  1>&2
    HERest $FLAGSHEREST -S $LSTTRA -I $LABELS -H $DIRHMM/hmm_$g/Macros_hmm $L_HMMS
    k=$[k+1]
  done
  g=$[g*2]
done

