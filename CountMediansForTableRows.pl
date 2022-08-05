use strict;

my $tableFile = $ARGV[0];
my $outFile = $ARGV[1];


open(FTW,">$outFile") or die;
open(FTR,"<$tableFile") or die;
<FTR>;



while (my $input = <FTR>) {
    chomp($input);
    
    my @arrInp = split(/\t/,$input);
    my $geneName1 = $arrInp[0];
    my $geneName2 = $arrInp[1];
    
    my @arrValues = ();
    for(my $i = 2;$i <= $#arrInp;$i++)
    {
        my $currVal = $arrInp[$i];
        push @arrValues,$currVal;
        
    }
    my @arrValuesSorted = sort {$a <=> $b} @arrValues;
    my $median = $arrValuesSorted[($#arrInp-1)/2];
    if(($#arrInp-1) % 2 != 0)
    {
        $median = ($arrValuesSorted[($#arrInp-1)/2] + $arrValuesSorted[($#arrInp-1)/2 + 1])/2;     
    }
    
    
    print FTW "$geneName1\t$geneName2\t$median\n";
    
    
}


close(FTR);



close(FTW);
