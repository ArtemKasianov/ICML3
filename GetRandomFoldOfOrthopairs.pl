use strict;


my $orthopairsFile = $ARGV[0];
my $foldNumber = $ARGV[1];
my $outDir = $ARGV[2];


my @arrOrthoPairs = ();

open(FTR,"<$orthopairsFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    push @arrOrthoPairs,$input;
    
}
close(FTR);

my %foldsData = ();

my $maxFoldCount = int(($#arrOrthoPairs+1)/$foldNumber)+1;

for(my $i = 0;$i<=$#arrOrthoPairs;$i++)
{
    my $currOrthoPair = $arrOrthoPairs[$i];
    my $index = int(rand($foldNumber));
    if (exists $foldsData{"$index"}) {
	my $ptrArr = $foldsData{"$index"};
	if (($#$ptrArr+1) >= $maxFoldCount) {
	    $i--;
	    next;
	}
	push @$ptrArr,$currOrthoPair;
    }
    else
    {
	my @arrTmp = ();
	push @arrTmp,$currOrthoPair;
	$foldsData{"$index"} = \@arrTmp;
    }
    
    
}


my @arrIndexes = sort {$a <=> $b} keys %foldsData;


for(my $i = 0;$i <= $#arrIndexes;$i++)
{
    my $currIndex = $arrIndexes[$i];
    
    open(FTW,">$outDir/fold_$currIndex.orthopairs") or die;
    
    my $ptrArr = $foldsData{"$currIndex"};
    
    for(my $j = 0;$j <= $#$ptrArr;$j++)
    {
	my $currOrthoPair = $ptrArr->[$j];
	print FTW "$currOrthoPair\n";
    }
    
    close(FTW);
    
}


