use strict;



my $fullOrthoPairsFile = $ARGV[0];
my $countPairsFile = $ARGV[1];
my $firstSpListFile = $ARGV[2];
my $secondSpListFile = $ARGV[3];
my $outFile = $ARGV[4];


my %pairsUsed = ();
my @firstSpList = ();
my @secondSpList = ();

open(FTW,">$outFile") or die;
open(FTR,"<$firstSpListFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    my $currFirstSpNam = $arrInp[0];
	
    push @firstSpList,$currFirstSpNam;
}


close(FTR);


open(FTR,"<$secondSpListFile") or die;
<FTR>;
while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    my $currSecondSpNam = $arrInp[0];
    
    push @secondSpList,$currSecondSpNam;
}


close(FTR);



open(FTR,"<$fullOrthoPairsFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    
    my $firstSpNam = $arrInp[0];
    my $secondSpNam = $arrInp[1];
    
    $pairsUsed{"$firstSpNam\t$secondSpNam"}=1;
    
}


close(FTR);


for(my $i = 0;$i < $countPairsFile;$i++)
{
    my $countFirstSp = $#firstSpList+1;
    my $countSecondSp = $#secondSpList+1;
    
    my $firstSpIndex = int(rand($countFirstSp));
    my $secondSpIndex = int(rand($countSecondSp));
    
    my $currFirstSpNam = $firstSpList[$firstSpIndex];
    my $currSecondSpNam = $secondSpList[$secondSpIndex];
    print "$currFirstSpNam\n";
    if(exists $pairsUsed{"$currFirstSpNam\t$currSecondSpNam"})
    {
	$i--;
	next;
    }
    $pairsUsed{"$currFirstSpNam\t$currSecondSpNam"} = 1;
    print FTW "$currFirstSpNam\t$currSecondSpNam\n";
    
}



close(FTW);
