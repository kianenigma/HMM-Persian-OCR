HTR-Preprocessing.sh data/pbm feaDir 20
Create_HMMs-TOPOLOGY.sh 60 6 > proto
Create_HMMsList.sh data/text HMMsList
Create_Train-MLF.sh data/text/ samples.mlf
python trainList.py
Train-HMMs.sh train.lst hmm proto samples.mlf HMMsList
python DBGenerator.py
Create_HTK-DicNet.sh sampleDB.dat Dictionary Network
Create_Test-MLF.sh data/text samplesRef.mlf
./test.sh
