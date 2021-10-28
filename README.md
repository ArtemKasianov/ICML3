# ICML
ICML - software for interspecies mapping of gene expression profiles.


Information about expression profiles for two species and a set of orthopairs are used as input data. Information about expression profiles should be presented in the form of a text file containing a tab - delimited table. In the first column of the table, the names of the genes should be indicated, in the second and subsequent columns, the expression values that make up the profile. Orthopairs should be given in the form of a text file containing a list of orthopairs. One orthopair per line, the names of the genes are separated by tabs. The first should always be the gene of the species whose expression profile is listed as the first in the list of parameters.
## Installation
```
git clone https://github.com/ArtemKasianov/ICML3
cd ICML3
chmod +x makemodel
chmod +x predictAllByPortion
```
## Analysis
The analysis using ICML is carried out in two stages:
#### 1) Creating of models
At the first stage, classifiers are trained on the basis of a positive sample based on orthopairs and a negative sample made up of a random set of gene pairs. Since the negative sample is formed randomly, conducting the learning process with one negative sample is clearly not enough and it is necessary to conduct training several times with a fixed positive sample and several different negative ones.

A script is used for this purpose **StartICMLTraining.pl** .
The command line and the parameters for running it:
```
perl StartICMLTraining.pl <firstSpExpressionFile> <secondSpExpressionFile> <orthopairsFile> <minIterations> <maxIterations > <sizeOfNegSet> < paramsFile >
```
- `<firstSpExpressionFile>` - a file with information about the expression profiles of genes of the first species. Must be a tab-delimited table. In the first column is the name of the gene, in the second and subsequent expression values that make up the profile;
- `<secondSpExpressionFile>` - a file with information about the expression profiles of genes of the second species;
- `<orthopairsFile>` - a file containing information about orthopairs. There is one orthopair in each line. The names of the genes should be separated by tabs. The gene of first species should always go first;
- `<minIterations>` - index of the first iteration;
- `<maxIterations>` - index of the last iteration;
- `<sizeOfNegSet>` - negative sample size; 
- `<paramsFile>` - a file containing a list of xgboost metaparameters. A set of default parameters and an example of such a file is attached to the software (paramsFile.txt );

As a result of running the script **StartICMLTraining.pl** a set of directories will be formed, the number of which is equal to the number of iterations in the directory from which it was launched.
The time spent for training is ~1-5 minutes per iteration for a single thread.
#### 2) Prediction of profile similarity based on trained models
Scripts for making predictions based on trained models must be run in the same directory in which the script **StartICMLTraining.pl** was run.
If you only need to get a prediction for the pairs included in the training sample, you need to use a script **PredictonsForPairsFromTrainingSets.pl**.

Command line and parameters for running the script **PredictonForPairsFromTrainingSets.pl**:
```
perl PredictonForPairsFromTrainingSets.pl <firstSpExpressionFile> < secondSpExpressionFile > <minIterations> <maxIterations> <numThreads> <outFile>
```
- `<firstSpExpressionFile>` - a file with information about the expression profiles of genes of the first species. Must be a tab-delimited table. In the first column is the name of the gene, in the second and subsequent expression values that make up the profile;
- `<secondSpExpressionFile>` - a file with information about the expression profiles of genes of the second species;
- `<minIterations>` - index of the first iteration;
- `<maxIterations>` - index of the last iteration;
- `<numThreads>` - maximum number of threads to be used; 
- `<outFile>` - the name of the file where you want to save the results of predictions. The results file is a text file on each line of which the result of assessing the similarity of the expression profiles of two genes is recorded. This file is a tab delimited table in the first two columns of which the names of genes are recorded, in the third the value of expression score. 

If you need to get predictions for an arbitrary set of pairs, you need to use a script **PredictonForPairsFromFile.pl**.

Command line and parameters for running the script **PredictonForPairsFromFile.pl**:
```
perl PredictonForPairsFromFile <firstSpExpressionFile> < secondSpExpressionFile > <minIterations> <maxIterations> <pairsToPredictFile> <numThreads> <outFile>
```
- `<firstSpExpressionFile>` - a file with information about the expression profiles of genes of the first species. Must be a tab-delimited table. In the first column is the name of the gene, in the second and subsequent expression values that make up the profile;
- `<secondSpExpressionFile>` - a file with information about the expression profiles of genes of the second species;
- `<minIterations>` - index of the first iteration;
- `<maxIterations>` - index of the last iteration;
- `<pairsToPredictFile>` - a file with a list of pairs for which you need to get predictions. The file format should be similar to the format of the file containing orthopairs for the script StartICMLTraining.pl
- `<numThreads>` - maximum number of threads to be used; 
- `<outFile>` - the name of the file where you want to save the results of predictions. The results file is a text file on each line of which the result of assessing the similarity of the expression profiles of two genes is recorded. This file is a tab delimited table in the first two columns of which the names of genes are recorded, in the third the value of expression score.
 
 Prediction for 10 thousand pairs takes ~10 seconds per iteration for a single thread

## Test set
To perform an ICML run on test example, you can use files located in the test directory

Test directory content:

- `test.1_species.expression_profile.txt` и `test.2_species.expression_profile.txt` – files with test expression profiles
- `test.orthopairs.list` – a file with a test set of orthopairs
- `test.pairsToPredict.list` – a file with a set of pairs for making a test prediction

To start training on a test example, you can use the commands: 
```
perl StartICMLTraining.pl test/test.1_species.expression_profile.txt test/test.2_species.expression_profile.txt test/test.orthopairs.list 0 9 1000 paramsFile.txt
perl PredictonsForPairsFromFile.pl test/test.1_species.expression_profile.txt test/test.2_species.expression_profile.txt 0 9 test/test.pairsToPredict.list 1 test.results.txt
```
