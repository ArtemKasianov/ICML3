import xgboost as xgb
import sys

trainFile = sys.argv[1]
testFile = sys.argv[2]
max_depth = sys.argv[3]
eta = sys.argv[4]
subsample = sys.argv[5]
colsample_bytree = sys.argv[6]
colsample_bylevel = sys.argv[7]
min_child_weight = sys.argv[8]
gamma = sys.argv[9]
alpha = sys.argv[10]
lambdaParam = sys.argv[11]
numIterations = sys.argv[12]
evalMetric = sys.argv[13]
scale_pos_weight = sys.argv[14]
modelNam = sys.argv[15]

print(trainFile)
print(testFile)
print(max_depth)
print(eta)
print(subsample)
print(colsample_bytree)
print(colsample_bylevel)
print(min_child_weight)
print(gamma)
print(alpha)
print(lambdaParam)
print(numIterations)
print(evalMetric)
print(scale_pos_weight)
print(modelNam)

dtrain = xgb.DMatrix(trainFile)
dtest = xgb.DMatrix(testFile)

print(dtrain)
print(dtest)

param = {'scale_pos_weight':scale_pos_weight,'alpha':alpha,'lambda':lambdaParam,'gamma':gamma,'min_child_weight':min_child_weight,'colsample_bylevel':colsample_bylevel,'colsample_bytree':colsample_bytree,'subsample':subsample,'bst:max_depth':max_depth, 'bst:eta':eta, 'silent':1, 'objective':'binary:logistic' }
param['nthread'] = 10
param['eval_metric'] = evalMetric
evallist  = [(dtest,'eval'), (dtrain,'train')]

plst = param.items()
num_round = 30
print("training");
bst = xgb.train( plst, dtrain, 80, evallist )
bst.save_model(modelNam)
print(bst)
#evalHistory = xgb.cv( plst, dtrain,int(numIterations),100,['auc','error'])
#print(evalHistory);
