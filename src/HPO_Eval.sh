#!/bin/bash

if [ $# -ne 4 ]; then
  echo "Usage: $NAME  <iter-count> <g-state> <Dim> <FilterType>"
  exit 1
fi

echo "Initializing ... $(date)" > info.config
echo "HPO_Preprocessing.sh data/img feaDir $3 $4" >> info.config
HPO_Preprocessing.sh data/img feaDir $3 $4 
HPO_CreateHMMsTopology.sh $((3*$3)) 6 > proto
echo "HPO_CreateHMMsTopology.sh $((3*$3)) 6 > proto" >> info.config
HPO_CreateHMMsList.sh data/text HMMsList
HPO_CreateTrainMLF.sh data/text/ samples.mlf
HPO_TrainList.py
HPO_DBGenerator.py
HPO_CreateHTKDicNet.sh sampleDB.dat Dictionary Network
HPO_CreateTestMLF.sh data/text samplesRef.mlf
HPO_TrainHMMs.sh train.lst hmm proto samples.mlf HMMsList $1 $2
echo "HPO_TrainHMMs.sh train.lst hmm proto samples.mlf HMMsList $1 $2" >> info.config
echo "Done Training $(date)" >> info.config

#HVite -A -T 1 -o ST -p -17 -s 50 -S train.lst -H hmm/hmm_$2/Macros_hmm -l "*" -i res_all.mlf -w Network Dictionary HMMsList
for j in `seq 0 9`;
	do 
        HVite -A -T 1 -o ST -p -17 -s 50 -S test_sh_$j.lst -H hmm/hmm_$2/Macros_hmm -l "*" -i res$j.mlf -w Network Dictionary HMMsList
        HResults -I samplesRef.mlf HMMsList res$j.mlf >> result_test_$j.res
    done
