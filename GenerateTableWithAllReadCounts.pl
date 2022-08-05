use strict;

my $listFile = $ARGV[0];
my $outFile = $ARGV[1];


open(FTW,">$outFile") or die;
open(FTR,"<$listFile") or die;

my %readCounts = ();

print FTW "gNam";

my $countIterations = 0;

while (my $input = <FTR>) {
    chomp($input);
    $countIterations++;
    my @arrOutInp = split(/\t/,$input);
    my $fileName = $arrOutInp[0];
	my $fileNameNegative = $arrOutInp[1];
	my $fileNameOthers = $arrOutInp[2];
    my $titleName = $arrOutInp[3];
    print FTW "\t$titleName";
    my $countLines = 0;
	
	my %negativeLists = ();
	my %otherLists = ();
	
	open(FTR_1,"<$fileNameNegative") or die;
	
	while(my $input_1 = <FTR_1>)
	{
		chomp($input_1);
		next if(substr($input_1,0,1) eq "[");
		my @arrInp = split(/\t/,$input_1);
		
		my $gNam1 = $arrInp[0];
		my $gNam2 = $arrInp[1];
		
		$negativeLists{"$gNam1\t$gNam2"} = 1;
		
	}
	
	
	close(FTR_1);
	
	open(FTR_1,"<$fileNameOthers") or die;
	
	while(my $input_1 = <FTR_1>)
	{
		chomp($input_1);
		next if(substr($input_1,0,1) eq "[");
		my @arrInp = split(/\t/,$input_1);
		
		my $gNam1 = $arrInp[0];
		my $gNam2 = $arrInp[1];
		
		$otherLists{"$gNam1\t$gNam2"} = 1;
		
	}
	
	
	close(FTR_1);
	
	
	
    open(FTR_1,"<$fileName") or die;
    
    while (my $input_1 = <FTR_1>) {
		chomp($input_1);
		next if(substr($input_1,0,1) eq "[");
		my @arrInp = split(/\t/,$input_1);
		
		my $gNam1 = $arrInp[0];
		my $gNam2 = $arrInp[1];
		if(exists $negativeLists{"$gNam1\t$gNam2"})
		{
			next if(not exists $otherLists{"$gNam1\t$gNam2"});
		}
		my $rCounts = $arrInp[2];
		
		if (exists $readCounts{"$gNam1\t$gNam2"}) {
			my $ptrArr = $readCounts{"$gNam1\t$gNam2"};
			#if(($#$ptrArr+1) => $countIterations)
			#{
			#	next;
			#}
			
			push @$ptrArr,$rCounts;
		}
		else
		{
			my @arrTmp = ();
			push @arrTmp,$rCounts;
			$readCounts{"$gNam1\t$gNam2"} = \@arrTmp;
		}
		$countLines++;
    }
    close(FTR_1);
	print "$countLines\n";
	my @arrReadCounts = keys %readCounts;
	
    
}


close(FTR);


print FTW "\n";
my @gNams = sort {$a cmp $b} keys %readCounts;
print "gNamsSize = $#gNams\n";
my $countLines = 0;
for(my $i = 0;$i <= $#gNams;$i++)
{
    my $currGNam = $gNams[$i];
    
    
    my $ptrArrTmp = $readCounts{"$currGNam"};
	if(($#$ptrArrTmp+1) != $countIterations)
	{
		print "$#$ptrArrTmp\n";
		next;
	}
	$countLines++;
    print FTW "$currGNam";
    for(my $j = 0;$j <= $#$ptrArrTmp;$j++)
    {
	my $currRCount = $ptrArrTmp->[$j];
	print FTW "\t$currRCount";
    }
    print FTW "\n";
    
    
    
}

print "$countLines - $countLines\n";

close(FTW);
