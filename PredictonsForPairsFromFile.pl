use strict;

my $firstSpExpressionFile = $ARGV[0];
my $secondSpExpressionFile = $ARGV[1];
my $minIterations = $ARGV[2];
my $maxIterations = $ARGV[3];
my $pairsToPredictFile = $ARGV[4];
my $numThreads = $ARGV[5];
my $outFile = $ARGV[6];


system("./predictAllByPortion $firstSpExpressionFile $secondSpExpressionFile $pairsToPredictFile $minIterations $maxIterations 0 $numThreads other folds_all");
system("perl CreateMediansFile.pl predictionsFileOther.list $minIterations $maxIterations tmpPredict.txt");

my %pairsToGet = ();

open(FTR,"<$pairsToPredictFile") or die;

while(my $input = <FTR>)
{
	chomp($input);
	my @arrInp = split(/\t/,$input);
	my $gNam1 = $arrInp[0];
	my $gNam2 = $arrInp[1];
	
	$pairsToGet{"$gNam1\t$gNam2"} = 1;
	
}


close(FTR);
open(FTW,">$outFile") or die;

open(FTR,"<tmpPredict.txt") or die;

while(my $input = <FTR>)
{
	chomp($input);
	my @arrInp = split(/\t/,$input);
	my $gNam1 = $arrInp[0];
	my $gNam2 = $arrInp[1];
	if(exists $pairsToGet{"$gNam1\t$gNam2"})
	{
		print FTW "$input\n";
	}
}

close(FTR);

system("rm tmpPredict.txt");
close(FTW);

