# HPO - Hidden Markov Model Based Persian OCR 


A Hidden Markov Model Based Persian OCR. 
This was originally a the Project of [Artificial Intelligence Course](http://www.boute.ir/iust-ai-94/persian-ocr) at Iran University of Science and Technology . 


# Requirements 


## Abstract Understanding on Hidden Markov Models
While a wide variety of papers and resources could be found, i personally found the below two resources the best. 
+  [A tutorial on hidden Markov models and selected applications in speech recognition ]() 
+  [Matemathicalmonk Youtube playlist on Machine learning ( HMM Section )](https://www.youtube.com/user/mathematicalmonk/playlists)
+  [HTK Book](http://htk.eng.cam.ac.uk/docs/docs.shtml) . a complete tutorial on usage of HTK. Recomended for users to read at least the first 4 chapters. 


It's recommended to read the first paper mentioned above ( or just take a short look ) and then continue the rest of the steps.




## Development Dependencies 
This project is entirely dependent on a variety of open source software listed briefly below. a link and a short reason of usage is provided.  
+ [HTK - Hidden Markov Model Toolkit](http://htk.eng.cam.ac.uk/) : used mainly to perform both training and recognition phases. it is indeed one of the bets and most adaptable HMM Implementations. while it's optimized for speech recognition task, it is not limited to it. 
+ [NETPBM Image Processing Libraries](http://netpbm.sourceforge.net/) : Used mainly to convert image files to binary format and prepare them for feature extraction. 
+ [SRILM Language Modeling Tool](http://www.speech.sri.com/projects/srilm/download.html) : used as a partner alongside HTK for language modeling  
+ [ImageMagick](http://www.imagemagick.org/script/index.php) / pango-view / Cario : used for generating image train data. 
+ Scripts used by HPO to extract text features are deprecaded from the core netpbm library . The original version of these scripts were maintained by [ICFHR](http://www.icfhr2014.org/) and are included in the `bin` directory. they should also be compiled and the binary files added to PATH 


All of the named tools are available freely and have been tested and installed under Linux and OSX.


# Usage Example 
After having all the dependencies installed, make sure to add all of the scripts to your PATH : 
```
export PATH=$PATH:[HPO_Source_Direcory]/HPO/src
```


The **test** folder under root directory contains some files that help you get started. 
Inside `test/text_repo` a simple test file in included that contains a set of 1000 Persian word. 


to get started, run : 
```
cd test
HPO_Generate myFirstHPOTest text_repo/test.txt
```
The script will generate all the images and the corresponding text files in a folder named `myFirstHPOTest` 


Next Move to the folder and execute : 
```
cd myFirstHPOTest
HPO_Eval 4 32 20 g
```


The three parameters are : 
+ 4 : Iteration Count 
+ 32 : Gaussian Mixtures per state
+ 20 : text feature extraction Dimension 
+ g : indicates the filter type used in `HPO_PreProcessing` for feature extraction.


**Note** `HPO_Eval` Embeds and automates the usage of a series of a lot of other programs. You can read the *Application Directory* Section and read the script itself to get more knowledge of how it works 


**Note2** As you might see by taking a look at the contents of `HPO_Eval` ( and more embedded scripts ) you will notice that the Initialization and evaluation step has A LOT more parameters. These three are included here just for convenience and the user might change them as he or her wants. 


The above script almost does all of the remaining job. Generate Prototype HHMs, generate MLF ( Label files ) from the train data, generate random test scripts, train the HMMs and perform 10 random test. 


in the final folder structure, HMM definitions are stored under `hmm` folder, feature extracted images under `feaDir`, and the final test results are stored in `result_test[i].res` 
```
cat result_test_0.res 
====================== HTK Results Analysis =======================
  Date: Tue Jan 19 21:13:25 2016
  Ref : samplesRef.mlf
  Rec : res0.mlf
------------------------ Overall Results --------------------------
SENT: %Correct=93.00 [H=93, S=7, N=100]
WORD: %Corr=97.97, Acc=97.76 [H=482, D=4, S=6, I=1, N=492]
===================================================================
```
  
# Application Directory 


the contents of `src` directory of HPO is described here : 


#### HPO_Eval
Alongside `HPO_Generate` , the 2 main scripts running and managing everything else in HPO 
this script runs a sequence of other task and scripts as mentioned above . Most of the scripts mentioned below are embedded in HPO_Eval.
By now in accepts these parameters : 


+ iter-count : Iteration Count 
+ gaussian-mix-per-state : Gaussian Mixtures per state
+ feature-extr-dim : text feature extraction Dimension 
+ feature-extr-filter-type : indicates the filter type used in `HPO_PreProcessing` for feature extraction.


Example :
```
HPO_Eval 4 32 20 g
```


#### HPO_Generate
Main script for generating a test folder with needed data with given name, from a give text source file


+ test-name : name of the folder created for this test ( not really important, u know ? :D ) 
+ text-source : path to source text file 


Example : 
```
HPO_Generate myFirstHPOTest text_repo/test.txt
```






#### HPO_CreateHMMsList 
Parses all of the text data used for 
+ Dictionary-out : path to output Dictionary file 
+ Network-out : path to output Network file 


Example : 
```
HPO_CreateHMMsList.sh data/text HMMsList
```


#### HPO_CreateTestMLF 
Creates a Master Label file in HTK format as a reference for later testing
+ text-data-dir : Path to Text Data Directory


Example : 
```
HPO_CreateTestMLF.sh data/text samplesRef.mlf
```


#### HPO_CreateTrainMLF
Creates a Master Label file in HTK format as a reference for later training
+ text-data-dir : Path to Text Data Directory


Example : 
```
HPO_CreateTrainMLF.sh data/text/ samples.mlf
```


#### HPO_DBGenerator


When no 3rd-party text context source is available, HPO uses the entire training data as language model reference . This script generates file name sampleDB.dat whih contains db formatted text stored in path `data/text`


Example : 
```
HPO_DBGenerator.py
```


#### HPO_HTKFeatShow
show the content of feature extracted image in the give path. Note that this only works with provided feature extractor script ( pgmtextfea ) 
+ featureVec-dir : Path to a feature vector file 


Example : 
```
HPO_HTkFeatShow feaDir/train_100.fea
```


#### HPO_Preprocessing




#### HPO_TestMe
Tests a given string with the trained models. Script should be run inside a test directory . It generates a new image from given text, extract feature vector from it and finds the ML ( Maximum like hood ) . 
Note that it uses normal feature extraction ( 20 Dimensions with Gaussian filter - should be fixed ) 


+ text : a to be tested 
+ maximum-gaussian-mixture : defines with which model, with how many Gaussian mixtures, the test should be performed. 


Example : 
``` 
HPO_TestMe کیان 64
```




#### HPO_TrainHMMs
Trains a set of HMMs according to a given HMM Prototype and a list of train data


+ Train-list : a list of paths to a set of `.fea` feature vector files. Generated by `HPO_TrainList`
+ hmm-dir : path to create hmms
+ proto : a prototype of hmms created by `HPO_CreateHMMsTopology`
+ sample-labels : Label file defining the corresponding text with each train data ( generated by `HPO_CreateTrainMLF` ) 
+ HMMsList : path HMMsLis file generated by `HPO_CreateHMMsList ` 
+ iter-cout : count of iterations in training 
+ max-gaussian-mix-per-state : count of maximum Gaussian mixtures per state 


Example : 
```
HPO_TrainHMMs.sh train.lst hmm proto samples.mlf HMMsList 4 32
```


#### HPO_TrainList
generates 10 random list of feature vector paths for testing. also generates a `train.lst` file which contains all of the feature vectors, for training phase 
Note that this script takes no Parameter and read the default `feaDir` Directory ( should be fixed ) 


Example : 
```
HPO_TrainList
```




### HPO_CreateHMMsTopology


#### HPO_TestUs
NOT TESTED AND FULLY IMPLEMENTED YET
#### HPO_TextTrim
NOT TESTED AND FULLY IMPLEMENTED YET


# Notes 


## User Defined Feature Vector
HTK is Flexible !(Hopefully) . in order to use an entire separate system for feature extraction than what HPO Provides, 


1. Don't use HPO_Preprocessign anymore 
2. Take a good look at the result of `pgmtextfea` command. its just a vector separated by space in each line :) generate this vector in any way that you wish 
3. Use the same `plf2htk` command provided in HPO to generate your own vector, to HTK Format 




## Parallel Train on MultiCore CPU 
as mentioned in HTKBOOK, in order to enable parallel processing for `HEREST` ( main application responsible for training phase, which takes a fair amount of you time and CPU usage) the parameter `-p N` can be added to its command. Where N is the number of CPU Cores allowing to work. 
Note that this is option is not a user inputted parameter to HPO at this stage. the user shall modify `HPO_TrainHMMs` script in order to make this change
