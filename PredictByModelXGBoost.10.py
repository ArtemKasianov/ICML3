import xgboost as xgb
import sys
import os
import numpy as np


svmFile = sys.argv[1]
pairGenesFile = sys.argv[2]
modelNam = sys.argv[3]
treesCount = int(sys.argv[4])


pairsOfGenesArr = []

f = open(pairGenesFile,"r")

for line in f:
    line = line.strip('\n')
    pairsOfGenesArr.append(line)

dval = xgb.DMatrix(svmFile)
bst = xgb.Booster({'nthread':20}) #init model
bst.load_model(modelNam)



ypred = bst.predict(dval,ntree_limit=treesCount)
i = 0
for val in ypred:
    genes = pairsOfGenesArr[i]
    print(genes+'\t'+str(val))
    i=i+1


f.close()

