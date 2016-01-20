# HPO - Hidden Markov Model Based Persian OCR 

A Hidden Markov Model Based Persian OCR . 
This was originally a the Project of [Artificial Intelligence Course](http://www.boute.ir/iust-ai-94/persian-ocr) at Iran University of Science and Technology . 

# Requirements 

## Abstract Understanding on Hidden Markov Models
While a wide variety of papers and resources could be found, i personally found the below two resources the best. 
+  [A tutorial on hidden Markov models and selected applications in speech recognition ]() 
+  [Matemathicalmonk Youtube playlist on Machine learning ( HMM Section )](https://www.youtube.com/user/mathematicalmonk/playlists)
+  [HTK Book](http://htk.eng.cam.ac.uk/docs/docs.shtml) . a complete tutorial on usage of HTK. recomended for users to read at least the first 4 chapters. 

It's recommended to read the first paper mentioned above ( or just take a short look ) and then continue the rest of the steps.


## Development Dependencies 
This project is entirely dependent on a variety of open source softwares listed briefly below. a link and a short reason of usage is provided.  
+ [HTK - Hidden Markov Model Toolkit](http://htk.eng.cam.ac.uk/) : used mainly to perform both training and recognition phases. it is indeed one of the bets and most adaptable HMM Implementations. while it's optimized for speech recognition task, it is not limited to it. 
+ [NETPBM Image Processing Libraries](http://netpbm.sourceforge.net/) : Used mainly to convert image files to binary format and prepare them for feature extraction. 
+ [SRILM Language Modeling Tool](http://www.speech.sri.com/projects/srilm/download.html) : used as a partner alongside HTK for language modeling  
+ [ImageMagick](http://www.imagemagick.org/script/index.php) / pango-view / Cario : used for generating image train data. 
+ Scripts used by HPO to extract text features are deprecaded from the core netpbm library . The original version of these scripts were maintained by [ICFHR](http://www.icfhr2014.org/) and are included in the `bin` directory. 

All of the named tools are available freely and have been tested and install under Linux and OSX.

# Usage Example 
After having all the dependencies installed, make sure to add all of the scripts to your PATH : 
```
export PATH=$PATH:[HPO_Source_Direcory]/HPO/src
```

the **test** folder under root directory contains some files that help you get started. 
inside `test/text_repo` a simple test file in included that contains a set of 1000 Persian word. 

to get started, run : 
```
cd test
HPO_Generate myFirstHPOTest text_repo/test.txt
```
The script will generate all the images and the corresponding text files in a folder named `myFirstHPOTest` 

Next Move to teh folder and execute : 
```
cd myFirstHPOTest
HPO_Eval 4 32 20 
```

The three parameters are : 
+ 4 : Iteration Count 
+ 32 : Gaussian Mixtures per state
+ 20 : text feature extraction Dimension 

**Note** `HPO_Eval` Embeds and automates the usage of a series of a lot of other programs. you can read the *Application Directory* Section and read the script itself to get more knowledge of how it works 

**Note2** As you might see by taking a look at the contents of `HPO_Eval` ( and more embedded scripts ) you will notice that the Initialization and evaluation step has A LOT more parameters. these three are included here just for convenience and the user might change them as he or her wants. 

The above script almost does all of the remaining job. generate Prototype HHMs, generate MLF ( Label files ) from the train data, generate random test scripts, train the HMMs and perform 10 random test. 

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

## HPO_CreateHMMsList 
parses all of the text data used for training and creates a list of all the used charachters. 
Parameters : 
+ <InputDir> : Path of stored text files ( data/text etc. ) 
+ <outDir> : Path to save the result 
Example : 

  
## HPO_CreateHMMsTopology
creates a sample prototype word HMMs . uses the standards of HTK for definition 
Parameters : 
+ <Vector-Size> : fixed size for observation vector ( at this stage, it should always be same as feature extraction dimention size times 3 ) 
+ <State-Number> : Number of states for each Model

  

## **HPO_CreateHTKDicNet**
## **HPO_CreateTestMLF**
## **HPO_CreateTrainMLF**
## **HPO_DBGenerator**
## **HPO_Eval**
## **HPO_Generate**
## **HPO_HTKFeatShow**
## **HPO_Preprocessing**
+ **HPO_TestMe**
+ **HPO_TestUs**
+ **HPO_TextTrim**
+ **HPO_TrainHMMs**
+ **HPO_TrainList**


# Notes 
