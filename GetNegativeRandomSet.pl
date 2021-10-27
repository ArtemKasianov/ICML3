use strict;



my $fullOrthoPairsFile = $ARGV[0];
my $countPairsFile = $ARGV[1];
my $athListFile = $ARGV[2];
my $fescListFile = $ARGV[3];
my $outFile = $ARGV[4];


my %pairsUsed = ();
my @athList = ();
my @fescList = ();

open(FTW,">$outFile") or die;
open(FTR,"<$athListFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
	my @arrInp = split(/\t/,$input);
    my $currAthNam = $arrInp[0];
	
    push @athList,$currAthNam;
}


close(FTR);


open(FTR,"<$fescListFile") or die;
<FTR>;
while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    my $currFescNam = $arrInp[0];
    
    push @fescList,$currFescNam;
}


close(FTR);



open(FTR,"<$fullOrthoPairsFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    
    my $athNam = $arrInp[0];
    my $fescNam = $arrInp[1];
    
    $pairsUsed{"$athNam\t$fescNam"}=1;
    
}


close(FTR);


for(my $i = 0;$i < $countPairsFile;$i++)
{
    my $countAth = $#athList+1;
    my $countFesc = $#fescList+1;
    
    my $athIndex = int(rand($countAth));
    my $fescIndex = int(rand($countFesc));
    
    my $currAthNam = $athList[$athIndex];
    my $currFescNam = $fescList[$fescIndex];
    print "$currAthNam\n";
    if(exists $pairsUsed{"$currAthNam\t$currFescNam"})
    {
	$i--;
	next;
    }
    $pairsUsed{"$currAthNam\t$currFescNam"} = 1;
    print FTW "$currAthNam\t$currFescNam\n";
    
}



close(FTW);
