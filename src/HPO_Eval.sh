#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $NAME  <iter-count> <g-state> "
  exit 1
fi

HPO_Preprocessing.sh data/img feaDir 20
HPO_CreateHMMsTopology.sh 60 6 > proto
HPO_CreateHMMsList.sh data/text HMMsList
HPO_CreateTrainMLF.sh data/text/ samples.mlf
HPO_TrainList.py
HPO_DBGenerator.py
HPO_CreateHTKDicNet.sh sampleDB.dat Dictionary Network
HPO_CreateTestMLF.sh data/text samplesRef.mlf
HPO_TrainHMMs.sh train.lst hmm proto samples.mlf HMMsList $1 $2

for j in `seq 0 9`;
	do 
        HVite -A -T 1 -o ST -p -17 -s 50 -S test_sh_$j.lst -H hmm/hmm_$2/Macros_hmm -l "*" -i res$j.mlf -w Network Dictionary HMMsList
        HResults -I samplesRef.mlf HMMsList res$j.mlf >> result_test_$j.res
    done