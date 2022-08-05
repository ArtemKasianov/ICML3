#!/usr/bin/perl

package OrthologousGroups;

use strict;


sub new {
    
    my $class = shift;
    my $self = bless {} , $class;
    
    my @arrTmpAthFesc = ();
    my @arrTmpAth = ();
    my @arrTmpFesc = ();
    
    $self->{athArr} = \@arrTmpAth;
    $self->{fescArr} = \@arrTmpFesc;
    $self->{athFescArr} = \@arrTmpAthFesc;
    
    
    return $self
}


sub GetAthArr {
    my $self = shift;
    
    return $self->{athArr};
}

sub GetAthNumberGenes {
    my $self = shift;
    my $ptrArr = $self->{athArr};
    
    my $numberGenes = $#$ptrArr+1;
    return $numberGenes;
    
}

sub GetFescArr {
    my $self = shift;
    
    
    
    return $self->{fescArr};
}


sub GetFescNumberGenes {
    my $self = shift;
    my $ptrArr = $self->{fescArr};
    
    my $numberGenes = $#$ptrArr+1;
    return $numberGenes;
    
}



sub GetAthFescArr {
    my $self = shift;
    
    return $self->{athFescArr};
}

sub AddGene
{
    my $self = shift;
    my $geneNam = shift;
    #print "$geneNam\n";
    if (substr($geneNam,0,2) ne "AT") {
		my $ptrAthArr = $self->{athArr};
		push @$ptrAthArr,$geneNam;
    }
    else
    {
	    my $ptrFescArr = $self->{fescArr};
	    push @$ptrFescArr,$geneNam;
    }
    
}

sub PrintToFile
{
    my $self = shift;
    my $fh = shift;
    my $ptrAthArr = $self->{athArr};
    my $ptrFescArr = $self->{fescArr};
    
    if($#$ptrAthArr > -1)
    {
	my $currAth = $ptrAthArr->[0];
	print $fh "$currAth";
	
	for(my $i = 1;$i <= $#$ptrAthArr;$i++)
	{
	    $currAth = $ptrAthArr->[$i];
	    print $fh "\t$currAth";
	}
	for(my $i = 0;$i <= $#$ptrFescArr;$i++)
	{
	    my $currFesc = $ptrFescArr->[$i];
	    print $fh "\t$currFesc";
	}
    }
    else
    {
	if($#$ptrFescArr > -1)
	{
	    my $currFesc = $ptrFescArr->[0];
	    print $fh "$currFesc";
	    for(my $i = 1;$i <= $#$ptrFescArr;$i++)
	    {
		my $currFesc = $ptrFescArr->[$i];
		print $fh "\t$currFesc";
	    }
	}
    }
    print $fh "\n";
    
    
}


1;
