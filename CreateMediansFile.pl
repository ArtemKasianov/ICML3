use strict;





my $listFile = $ARGV[0];
my $iterMin = $ARGV[1];
my $iterMax = $ARGV[2];
my $outFile = $ARGV[3];


my %pairsExpVals = ();
my @filesList = ();


open(FTW,">$outFile") or die;


open(FTR,"<$listFile") or die;

while(my $input = <FTR>)
{
	chomp($input);
	push @filesList,$input;
}



close(FTR);


for(my $i = $iterMin;$i <= $iterMax;$i++)
{
	my %iterPairsExpVals = ();
	
	for(my $j = 0;$j <= $#filesList;$j++)
	{
		my $currFileName = $filesList[$j];
		
		open(FTR,"<iter_$i/results/predictions/$currFileName") or die;
		
		while(my $input = <FTR>)
		{
			chomp($input);
			
			my @arrInp = split(/\t/,$input);
			
			my $gNam1 = $arrInp[0];
			my $gNam2 = $arrInp[1];
			my $expVal = $arrInp[2];
			if(not exists $iterPairsExpVals{"$gNam1\t$gNam2"})
			{
				$iterPairsExpVals{"$gNam1\t$gNam2"} = $expVal;
			}
		}
		
		
		close(FTR);
	}
	
	my @arrPairs = keys %iterPairsExpVals;
	
	for(my $j = 0;$j <= $#arrPairs;$j++)
	{
		my $currPair = $arrPairs[$j];
		
		my $currExpVal = $iterPairsExpVals{"$currPair"};
		
		if(exists $pairsExpVals{"$currPair"})
		{
			my $ptrArrExpVals = $pairsExpVals{"$currPair"};
			push @$ptrArrExpVals,$currExpVal;
		}
		else
		{
			my @arrExpVals = ();
			push @arrExpVals,$currExpVal;
			$pairsExpVals{"$currPair"} = \@arrExpVals;
		}
		
	}
	
}


my @arrPairs = keys %pairsExpVals;

for(my $i = 0;$i <= $#arrPairs;$i++)
{
	my $currPair = $arrPairs[$i];
	my $ptrArrExpVals = $pairsExpVals{"$currPair"};
	
	print FTW "$currPair";
	
	my @arrSortedExpVals = sort {$a <=> $b} @$ptrArrExpVals;
	
	
	my $countVals = $#arrSortedExpVals + 1;
	
	if($countVals % 2 == 0)
	{
		my $middleIndex = $countVals/2;
		my $middleIndex1 = $middleIndex-1;
		
		my $medianVal = ($arrSortedExpVals[$middleIndex] + $arrSortedExpVals[$middleIndex-1])/2;
		print FTW "\t$medianVal\n";
	}
	else
	{
		my $middleIndex = $#arrSortedExpVals/2;
		my $medianVal = $arrSortedExpVals[$middleIndex];
		print FTW "\t$medianVal\n";
	}

}

close(FTW);

