use strict;


my $minIterations = $ARGV[0];
my $maxIterations = $ARGV[1];

my $catString = "cat";

for(my $iter = $minIterations;$iter <=$maxIterations;$iter++ )
{
	
	$catString = $catString." iter_$iter/negative_sample/Negative_Sample.list";
	
}

$catString = $catString." >all.negative.pairs.txt";
system("$catString");
