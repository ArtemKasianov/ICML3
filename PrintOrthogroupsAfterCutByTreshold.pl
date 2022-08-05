use strict;
use OrthologGraphList;
use OrthologousGroups;

my $identFile = $ARGV[0];
my $predictFile = $ARGV[1];
my $treshold = $ARGV[2];
my $outOrthogroupsFile = $ARGV[3];




my %predictPairs = ();
my %identPairs = ();

my %allSp1Hash = ();
my %allSp2Hash = ();






open(FTR,"<$predictFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    
    my $sp1Nam = $arrInp[0];
    my $sp2Nam = $arrInp[1];
    my $predictVal = $arrInp[2];
    
    $allSp1Hash{"$sp1Nam"} = 1;
    $allSp2Hash{"$sp2Nam"} = 1;
    
    
    $predictPairs{"$sp1Nam\t$sp2Nam"}=$predictVal;
    
    
}


close(FTR);


my @allSp1 = keys %allSp1Hash;
my @allSp2 = keys %allSp2Hash;

open(FTR,"<$identFile") or die;

while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    
    my $sp1Nam = $arrInp[0];
    my $sp2Nam = $arrInp[1];
    my $identVal = $arrInp[2];
    if (exists $predictPairs{"$sp1Nam\t$sp2Nam"}) {
	$identPairs{"$sp1Nam\t$sp2Nam"}=$identVal;
    }
    
    
}


close(FTR);




my $orthologGraph = OrthologGraphList->new();
for(my $i = 0;$i <= $#allSp1;$i++)
{
    my $currSp1 = $allSp1[$i];
    for(my $j = 0;$j <= $#allSp2;$j++)
    {
	my $currSp2 = $allSp2[$j];
	
	next if(not exists $predictPairs{"$currSp1\t$currSp2"});
	next if(not exists $identPairs{"$currSp1\t$currSp2"});
	
	my $predictVal = $predictPairs{"$currSp1\t$currSp2"};
	my $identVal = $identPairs{"$currSp1\t$currSp2"};
	
	$orthologGraph->AddEdge($currSp1,$currSp2,$predictVal,$identVal);
	
    }
}


$orthologGraph->RemoveEdgesWithWeightBelowTreshold($treshold,0);
$orthologGraph->PrintOrthogroups($outOrthogroupsFile);

