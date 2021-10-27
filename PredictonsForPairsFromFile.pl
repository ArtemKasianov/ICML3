use strict;

my $firstSpExpressionFile = $ARGV[0];
my $secondSpExpressionFile = $ARGV[1];
my $minIterations = $ARGV[2];
my $maxIterations = $ARGV[3];
my $pairsToPredictFile = $ARGV[4];
my $numThreads = $ARGV[5];
my $outFile = $ARGV[6];


system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile $pairsToPredictFile $minIterations $maxIterations 0 $numThreads other folds_all");
system("perl CreateMediansFile.pl predictionsFileOther.list $minIterations $maxIterations $outFile");


