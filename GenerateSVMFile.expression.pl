use strict;

my $sp1ExpressionFile = $ARGV[0];
my $sp2ExpressionFile = $ARGV[1];
my $orthologPairsFile = $ARGV[2];
my $typeOfSample = $ARGV[3];
my $isHeader = $ARGV[4];
my $outFile = $ARGV[5];


open(FTW,">$outFile") or die;


my %sp1Expression = ();
my %sp2Expression = ();



sub LoadDataFromTabularFile
{
    my $fileName = $_[0];
    my $ptrHash = $_[1];
    my $isHeader = $_[2];
    
    open(FTR,"<$fileName") or die;
    
    if ($isHeader == 1) {
	<FTR>;
    }
    
    
    while (my $input = <FTR>) {
	
	chomp($input);
	
	my @arrInp = split(/\t/,$input);
	
	my $gNam = $arrInp[0];
	if(substr($gNam,0,2) eq "AT")
	{
		my @arrGNam = split(/\./,$gNam);
		$gNam = $arrGNam[0];
	}
	my $expData = $arrInp[1];
	for(my $i = 2;$i <= $#arrInp;$i++)
	{
	    my $currExp = $arrInp[$i];
	    $expData = $expData."\t".$currExp;
	}

	$ptrHash->{"$gNam"} = $expData;
    }
    close(FTR);
    
    
}

sub LoadDataFromTabularFileWithPairs
{
    my $fileName = $_[0];
    my $ptrHash = $_[1];
    my $isHeader = $_[2];
    
    open(FTR,"<$fileName") or die;
    if ($isHeader == 1) {
	<FTR>;
    }
    while (my $input = <FTR>) {
	chomp($input);
	
	my @arrInp = split(/\t/,$input);
	
	my $gNam1 = $arrInp[0];
	my @arrGNam = split(/\./,$gNam1);
	$gNam1 = $arrGNam[0];
	my $gNam2 = $arrInp[1];
	@arrGNam = split(/\./,$gNam2);
	$gNam2 = $arrGNam[0];
	my $distVal = $arrInp[2];
	if (exists $ptrHash->{"$gNam1"}) {
	    $ptrHash->{"$gNam1"}->{"$gNam2"} = $distVal;
	}
	else
	{
	    my %hashTmp = ();
	    $hashTmp{"$gNam2"}=$distVal;
	    $ptrHash->{"$gNam1"} = \%hashTmp;
	}
	
    }
    close(FTR);
    
    
}

sub PrintValsFromString
{
    my $gNam = $_[0];
    my $ptrHash = $_[1];
    my $ptrIndex = $_[2];
    
    if (exists $ptrHash->{"$gNam"}) {
	
	my $strVal = $ptrHash->{"$gNam"};
	my @arrStr = split(/\t/,$strVal);
	
	for(my $i = 0;$i <= $#arrStr;$i++)
	{
	    my $currStrVal = $arrStr[$i];
	    $$ptrIndex = $$ptrIndex + 1;
	    print FTW "\t$$ptrIndex:$currStrVal";
	}
	#print FTW "\n";
    }
    else
    {
	die("$gNam\n");
    }
    
    
}


#sub PrintValsFromStringDomains
#{
#    my $gNam = $_[0];
#    my $ptrHash = $_[1];
#    my $ptrIndex = $_[2];
#    
#    if (exists $ptrHash->{"$gNam"}) {
#	
#	my $strVal = $ptrHash->{"$gNam"};
#	my @arrStr = split(/\t/,$strVal);
#	
#	for(my $i = 0;$i <= $#arrStr;$i++)
#	{
#	    my $currStrVal = $arrStr[$i];
#	    $$ptrIndex = $$ptrIndex + 1;
#	    print FTW "\t$$ptrIndex:$currStrVal";
#	}
#	#print FTW "\n";
#    }
#    else
#    {
#	for(my $i = 0;$i < $maxDomains;$i++ )
#	{
#	    print FTW "\t$$ptrIndex:0";
#	}
#    }
#    
#    
#}



sub PrintValsFromStringDomains
{
    my $gNam1 = $_[0];
    my $gNam2 = $_[1];
    my $ptrHash1 = $_[2];
    my $ptrHash2 = $_[3];
    my $ptrIndex = $_[4];
    my $gene1Number = 0;
    my $gene2Number = 0;
    my $distFlag = 0.5;
    if ((exists $ptrHash1->{"$gNam1"}) && (exists $ptrHash2->{"$gNam2"})) {
	my $strVal1 = $ptrHash1->{"$gNam1"};
	my @arrStr1 = split(/\t/,$strVal1);
	my $strVal2 = $ptrHash2->{"$gNam2"};
	my @arrStr2 = split(/\t/,$strVal2);
	$gene1Number = 0;
	$gene2Number = 0;
	for(my $i = 0;$i <= $#arrStr1;$i++)
	{
	    my $currStrVal1 = $arrStr1[$i];
	    my $currStrVal2 = $arrStr2[$i];
	    if ($currStrVal1 == 1) {
		$gene1Number++;
	    }
	    if ($currStrVal2 == 1) {
		$gene2Number++;
	    }
	    
	}
	
	my $dist2 = 0;
	for(my $i = 0;$i <= $#arrStr1;$i++)
	{
	    my $currStrVal1 = $arrStr1[$i];
	    my $currStrVal2 = $arrStr2[$i];
	    my $diff = ($currStrVal2 - $currStrVal1);
	    my $diff2 = $diff*$diff;
	    $dist2 = $dist2 + $diff2;
	}
	
	$distFlag = 1-($dist2/($gene1Number+$gene2Number));
	
	
    }
    
    if ((not exists $ptrHash1->{"$gNam1"}) && (exists $ptrHash2->{"$gNam2"})) {
	my $strVal2 = $ptrHash2->{"$gNam2"};
	my @arrStr2 = split(/\t/,$strVal2);
	$gene1Number = 0;
	$gene2Number = 0;
	for(my $i = 0;$i <= $#arrStr2;$i++)
	{
	    my $currStrVal2 = $arrStr2[$i];
	    if ($currStrVal2 == 1) {
		$gene2Number++;
	    }
	    
	}
	
	$distFlag = 0;
	
    }
    if ((exists $ptrHash1->{"$gNam1"}) && (not exists $ptrHash2->{"$gNam2"})) {
	my $strVal1 = $ptrHash1->{"$gNam1"};
	my @arrStr1 = split(/\t/,$strVal1);
	$gene1Number = 0;
	$gene2Number = 0;
	for(my $i = 0;$i <= $#arrStr1;$i++)
	{
	    my $currStrVal1 = $arrStr1[$i];
	    if ($currStrVal1 == 1) {
		$gene1Number++;
	    }
	    
	}
	$distFlag = 0;
    }
    
    if ((not exists $ptrHash1->{"$gNam1"}) && (not exists $ptrHash2->{"$gNam2"})) {
	$gene1Number = 0;
	$gene2Number = 0;
	$distFlag = 0.5;
    }
    
    $$ptrIndex = $$ptrIndex + 1;
    print FTW "\t$$ptrIndex:$gene1Number";
    $$ptrIndex = $$ptrIndex + 1;
    print FTW "\t$$ptrIndex:$gene2Number";
    $$ptrIndex = $$ptrIndex + 1;
    print FTW "\t$$ptrIndex:$distFlag";
}


sub PrintValsFromStringPairsIdentity
{
    my $gNam1 = $_[0];
    my $gNam2 = $_[1];
    my $ptrHash = $_[2];
    my $ptrIndex = $_[3];
    $$ptrIndex = $$ptrIndex + 1;
    if (exists $ptrHash->{"$gNam1"}->{"$gNam2"}) {
	
	
	my $distVal = $ptrHash->{"$gNam1"}->{"$gNam2"};
	print FTW "\t$$ptrIndex:$distVal";
    }
    else
    {
	print FTW "\t$$ptrIndex:0.0";
    }
    
    
}
sub PrintValsFromStringPairs
{
    my $gNam1 = $_[0];
    my $gNam2 = $_[1];
    my $ptrHash = $_[2];
    my $ptrIndex = $_[3];
    $$ptrIndex = $$ptrIndex + 1;
    if (exists $ptrHash->{"$gNam1"}->{"$gNam2"}) {
	
	my $distVal = $ptrHash->{"$gNam1"}->{"$gNam2"};
	print FTW "\t$$ptrIndex:$distVal";
    }
    else
    {
	die("$gNam1\t$gNam2\n");
    }
    
    
}

LoadDataFromTabularFile($sp1ExpressionFile,\%sp1Expression,1);
LoadDataFromTabularFile($sp2ExpressionFile,\%sp2Expression,1);
print "here\n";
#LoadDataFromTabularFile($sp1ExonLensFile,\%sp1ExonLens,0);
#LoadDataFromTabularFile($sp2ExonLensFile,\%sp2ExonLens,0);
#LoadDataFromTabularFile($sp1DomainsFile,\%sp1Domain,0);
#LoadDataFromTabularFile($sp2DomainsFile,\%sp2Domain,0);


#LoadDataFromTabularFileWithPairs($sp1sp2ExonVectorDistFile,\%sp1sp2ExonVectorDist,0);
#LoadDataFromTabularFileWithPairs($sp1sp2EuclidDistFile,\%sp1sp2EuclidDist,0);
#LoadDataFromTabularFileWithPairs($sp1sp2IdentFile,\%sp1sp2Ident,0);

open(FTR, "<$orthologPairsFile") or die("$orthologPairsFile\n");
if ($isHeader == 1) {
    <FTR>;
}
while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    my $sp1Nam = $arrInp[0];
    my $sp2Nam = $arrInp[1];
	
	#$sp2Nam = substr($sp2Nam,0,length($sp2Nam)-2);
	
    my $index = 0;
    #print "$sp1Nam\t$sp2Nam\n";
    next if(not exists $sp1Expression{"$sp1Nam"});
	#print "$sp2Nam\there 1\n";
    next if(not exists $sp2Expression{"$sp2Nam"});
    
	#next if(not exists $sp1ExonLens{"$sp1Nam"});
    #next if(not exists $sp2ExonLens{"$sp2Nam"});
    #next if(not exists $sp1Domain{"$sp1Nam"});
    #next if(not exists $sp2Domain{"$sp2Nam"});
    #next if(not exists $sp1sp2ExonVectorDist{"$sp1Nam"}->{"$sp2Nam"});
    #next if(not exists $sp1sp2EuclidDist{"$sp1Nam"}->{"$sp2Nam"});
    print FTW "$typeOfSample";
    
    PrintValsFromString($sp1Nam,\%sp1Expression,\$index);
    PrintValsFromString($sp2Nam,\%sp2Expression,\$index);
    #PrintValsFromString($sp1Nam,\%sp1ExonLens,\$index);
    #PrintValsFromString($sp2Nam,\%sp2ExonLens,\$index);
    #PrintValsFromStringPairsIdentity($sp1Nam,$sp2Nam,\%sp1sp2Ident,\$index);
    #PrintValsFromStringPairs($sp1Nam,$sp2Nam,\%sp1sp2ExonVectorDist,\$index);
    #PrintValsFromStringPairs($sp1Nam,$sp2Nam,\%sp1sp2EuclidDist,\$index);
    #PrintValsFromStringDomains($sp1Nam,$sp2Nam,\%sp1Domain,\%sp2Domain,\$index);
    print FTW "\n";
}




close(FTR);
close(FTW);
