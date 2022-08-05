use strict;

my $firstSpExpressionFile = $ARGV[0];
my $secondSpExpressionFile = $ARGV[1];

my $orthopairsFile = $ARGV[2];

my $minIterations = $ARGV[3];
my $maxIterations = $ARGV[4];
my $sizeOfNegSet = $ARGV[5];
my $paramsFile = $ARGV[6];



my $max_depth = 10;
my $eta = 0.3;
my $subsample = 1;
my $colsample_bytree = 1;
my $colsample_bylevel = 1;
my $min_child_weight = 1;
my $gamma = 0;
my $alpha = 4;
my $lambdaParam = 1;
my $numIterations = 100;
my $eval_metric = "auc";
my $scale_pos_weight = 1;
my $nthread = 1;



open(FTR,"<$paramsFile") or die;
my $input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$max_depth = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$eta = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$subsample = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$colsample_bytree = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$colsample_bylevel = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$min_child_weight = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$gamma = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$alpha = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$lambdaParam = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$numIterations = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$eval_metric = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$scale_pos_weight = $arrInp[1];
$input = <FTR>;
chomp($input);
my @arrInp = split(/\t/,$input);
$nthread = $arrInp[1];




for(my $iter = $minIterations;$iter <=$maxIterations;$iter++ )
{
	mkdir("iter_$iter");
	mkdir("iter_$iter/results");
	mkdir("iter_$iter/results/data_for_learning");
	mkdir("iter_$iter/negative_sample");
	mkdir("iter_$iter/results/predictions");
	
	system("perl GetNegativeRandomSet.pl $orthopairsFile $sizeOfNegSet $firstSpExpressionFile $secondSpExpressionFile iter_$iter/negative_sample/Negative_Sample.list");
	system("cp $firstSpExpressionFile iter_$iter/results/data_for_learning/expression.predict");
	system("cp $orthopairsFile iter_$iter/results/data_for_learning/orthopairs.list");
	mkdir("iter_$iter/results/data_for_learning/folds");
	mkdir("iter_$iter/results/data_for_learning/negative_folds");
	mkdir("iter_$iter/results/data_for_learning/svm");
	system("perl GetRandomFoldOfOrthopairs.pl iter_$iter/results/data_for_learning/orthopairs.list 10 iter_$iter/results/data_for_learning/folds");
	system("perl GetRandomFoldOfOrthopairs.pl iter_$iter/negative_sample/Negative_Sample.list 10 iter_$iter/results/data_for_learning/negative_folds");
	
	
	mkdir("iter_$iter/results/training");
	mkdir("iter_$iter/results/training/expression");
	for(my $i = 0;$i <= 9;$i++)
	{
		mkdir("iter_$iter/results/training/expression/folds_$i");
		mkdir("iter_$iter/results/training/expression/folds_$i/model");
	}
	mkdir("iter_$iter/results/training/expression/folds_all");
	mkdir("iter_$iter/results/training/expression/folds_all/model");

my $max_depth = 10;
my $eta = 0.3;
my $subsample = 1;
my $colsample_bytree = 1;
my $colsample_bylevel = 1;
my $min_child_weight = 1;
my $gamma = 0;
my $alpha = 4;
my $lambdaParam = 1;
my $eval_metric = "auc";
my $scale_pos_weight = 1;
my $nthread = 1;
	
	

    system("./makemodel $max_depth $eta $subsample $colsample_bytree $colsample_bylevel $min_child_weight $gamma $alpha $lambdaParam $numIterations $eval_metric $scale_pos_weight $nthread iter_$iter/results/training/expression $firstSpExpressionFile $secondSpExpressionFile iter_$iter/results/data_for_learning/folds iter_$iter/results/data_for_learning/negative_folds 0 0");
    
	for(my $fold_index = 0;$fold_index <= 9;$fold_index++)
	{
		system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile iter_$iter/results/data_for_learning/folds/fold_$fold_index.orthopairs $iter $iter 0 $nthread positive_train.$fold_index folds_$fold_index");
		system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile iter_$iter/results/data_for_learning/negative_folds/fold_$fold_index.orthopairs $iter $iter 0 $nthread negative_train.$fold_index folds_$fold_index");
	}
	

}
#
system("perl GetAllNegativePairs.pl $minIterations $maxIterations");
system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile all.negative.pairs.txt $minIterations $maxIterations 0 $nthread negative folds_all");

