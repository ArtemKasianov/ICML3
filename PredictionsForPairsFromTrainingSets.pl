use strict;

my $firstSpExpressionFile = $ARGV[0];
my $secondSpExpressionFile = $ARGV[1];

my $minIterations = $ARGV[2];
my $maxIterations = $ARGV[3];
my $numThreads = $ARGV[4];
my $outFile = $ARGV[5];


for(my $iter = $minIterations;$iter <=$maxIterations;$iter++ )
{

	for(my $fold_index = 0;$fold_index <= 9;$fold_index++)
	{
		system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile iter_$iter/results/data_for_learning/folds/fold_$fold_index.orthopairs $iter $iter 0 50 positive_train.$fold_index folds_$fold_index");
		system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile iter_$iter/results/data_for_learning/negative_folds/fold_$fold_index.orthopairs $iter $iter 0 50 negative_train.$fold_index folds_$fold_index");
	}
	

}
#
system("perl GetAllNegativePairs.pl $minIterations $maxIterations");
system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile all.negative.pairs.txt $minIterations $maxIterations 0 $numThreads negative folds_all");
system("perl CreateMediansFile.pl predictionsFileTrainingSet.list $minIterations $maxIterations $outFile");


