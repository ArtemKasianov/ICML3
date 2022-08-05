#!/usr/bin/perl

package OrthologGraphList;

use strict;


sub new {
    
    my $class = shift;
    my $self = bless {} , $class;
    
    my %Edges = ();
    
    
    $self->{"Edges"} = \%Edges;
    $self->{"addedEdge"} = 0;
    $self->{"removeEdge"} = 0;
    $self->{"addedRemoveEdge"} = 0;
    $self->{"edgeStateStore"} = ();
    
    
    
    return $self
}

sub ResetEdgeStore
{
    my $self = shift;
    $self->{"addedEdge"} = 0;
    $self->{"removeEdge"} = 0;
    $self->{"addedRemoveEdge"} = 0;
    $self->{"edgeStateStore"} = ();
}

sub GetRemovedEdges
{
    my $self = shift;
    return $self->{"removeEdge"};
}
sub GetAddedEdges
{
    my $self = shift;
    return $self->{"addedEdge"};
}

sub GetAddedRemovedEdges
{
    my $self = shift;
    return $self->{"addedRemoveEdge"};
}


sub LogAddingEdge()
{
    my $self = shift;
    my $vertexFrom = shift;
    my $vertexTo = shift;
    
#    if (exists $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"}) {
#	
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == 1) {
#	    #die;
#	    #$self->{"addedEdge"} -= 1;
#	    #$self->{"addedRemoveEdge"} += 1;
#	    
#	}
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == -1) {
#	    $self->{"removeEdge"} -= 1;
#	    $self->{"addedRemoveEdge"} += 1;
#	    $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = 2;
#	    
#	}
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == 2) {
#	    $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = 1;
#	    $self->{"addedEdge"} += 1;
#	    
#	}
#	
#    }
#    else
#    {
#	$self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = 1;
#	$self->{"addedEdge"} += 1;
#    }
    
    
}

sub LogRemovingEdge()
{
    my $self = shift;
    my $vertexFrom = shift;
    my $vertexTo = shift;
#    if (exists $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"}) {
#	
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == 1) {
#	    $self->{"addedEdge"} -= 1;
#	    $self->{"addedRemoveEdge"} += 1;
#	    $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = 2;
#	    
#	}
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == -1) {
#	    die;
#	    #$self->{"removeEdge"} -= 1;
#	    #$self->{"addedRemoveEdge"} += 1;
#	    
#	}
#	if ($self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} == 2) {
#	    $self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = -1;
#	    $self->{"removeEdge"} += 1;
#	    
#	}
#	
#    }
#    else
#    {
#	$self->{"edgeStateStore"}->{"$vertexFrom\t$vertexTo"} = -1;
#	$self->{"removeEdge"} += 1;
#    }
    
    
}



sub GetVertexies {
    my $self = shift;
    
    my $ptrEdges = $self->{"Edges"};
    my @arrVertexies = keys %$ptrEdges;
    
    return \@arrVertexies;
    
}

#sub AddVertex {
#    my $self = shift;
#    my $vertex = shift;
#    
#    return if(exists $self->{"Edges"}->{"$vertex"});
#    
#    
#    my %hashForVertex = ();
#    my $ptrArrVertexies = $self->GetVertexies();
#    
#    for(my $i = 0;$i <= $#$ptrArrVertexies;$i++)
#    {
#	my $currVertex = $ptrArrVertexies->[$i];
#	$hashForVertex{"$currVertex"}=-1;
#	$self->{"Edges"}->{"$currVertex"}->{"$vertex"} = -1;
#    }
#    $self->{"Edges"}->{"$vertex"} = \%hashForVertex;
#}




sub AddEdge {
    my $self = shift;
    my $vertex1 = shift;
    my $vertex2 = shift;
    my $expressionWeight = shift;
    my $identWeight = shift;
    
    my @arrWeights = ();
    push @arrWeights,$expressionWeight;
    push @arrWeights,$identWeight;
    
    $self->LogAddingEdge($vertex1,$vertex2);
    $self->LogAddingEdge($vertex2,$vertex1);
    
    if (exists $self->{"Edges"}->{"$vertex1"}) {
	$self->{"Edges"}->{"$vertex1"}->{"$vertex2"} = \@arrWeights;
    }
    else
    {
	my %hashTmp = ();
	$hashTmp{"$vertex2"} = \@arrWeights;
	$self->{"Edges"}->{"$vertex1"} = \%hashTmp;
	
    }
    
    if (exists $self->{"Edges"}->{"$vertex2"}) {
	$self->{"Edges"}->{"$vertex2"}->{"$vertex1"} = \@arrWeights;
    }
    else
    {
	my %hashTmp = ();
	$hashTmp{"$vertex1"} = \@arrWeights;
	$self->{"Edges"}->{"$vertex2"} = \%hashTmp;
	
    }
    
    
}


sub GetEdgeWeight
{
    my $self = shift;
    my $vertex1 = shift;
    my $vertex2 = shift;
    
    my $weights = -1;
    
    if (exists $self->{"Edges"}->{"$vertex1"}->{"$vertex2"}) {
	$weights = $self->{"Edges"}->{"$vertex1"}->{"$vertex2"};
	
    }
    
    
    return $weights;
    
}




sub GetAllVertexesConnectedToVertex
{
    my $self = shift;
    my $vertex1 = shift;
    
    my $ptrVertexies = -1;
    
    
    
    if (exists $self->{"Edges"}->{"$vertex1"}) {
	my @arrToVertexies = ();
	my $ptrVertexiesTo = $self->{"Edges"}->{"$vertex1"};
	my @arrConnVertexies = keys %$ptrVertexiesTo;
	for(my $j = 0;$j <= $#arrConnVertexies;$j++)
	{
	    my $toVertex = $arrConnVertexies[$j];
	    if ($self->{"Edges"}->{"$vertex1"}->{"$toVertex"} == -1) {
		next;
	    }
	    
	    
	    push @arrToVertexies,$toVertex;
	}
	$ptrVertexies = \@arrToVertexies;
    }
    
    return $ptrVertexies;
    
}



sub PrintGraphByPairs
{
    my $self = shift;
    my $fileName = shift;
    
    open(FTW,">$fileName") or die;
    
    my $ptrArrVerexies = $self->GetVertexies();
    
    my %pairsPrinted = ();
    
    for(my $i = 0;$i <= $#$ptrArrVerexies;$i++)
    {
	my $currVertex1 = $ptrArrVerexies->[$i];
	my $ptrArrVertexies2 = $self->GetAllVertexesConnectedToVertex($currVertex1);
	next if($ptrArrVertexies2 == -1);
	
	for(my $j = 0;$j <= $#$ptrArrVertexies2;$j++)
	{
	    my $currVertex2 = $ptrArrVertexies2->[$j];
	    
	    die if(not exists $self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"});
	    die if(not exists $self->{"Edges"}->{"$currVertex2"}->{"$currVertex1"});
	    next if($self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"} == -1);
	    next if(exists $pairsPrinted{"$currVertex2\t$currVertex1"});
	    next if(exists $pairsPrinted{"$currVertex1\t$currVertex2"});
	    $pairsPrinted{"$currVertex1\t$currVertex2"} = 1;
	    
	    if (substr($currVertex1,0,2) eq "AT") {
		print FTW "$currVertex1\t$currVertex2\n";
	    }
	    else
	    {
		print FTW "$currVertex2\t$currVertex1\n";
	    }
	}
    }
    
    
    close(FTW);
}




sub RemoveEdgesWithWeightBelowTreshold
{
    my $self = shift;
    my $treshold = shift;
    my $isIdent = shift;
    
    my $ptrArrVerexies = $self->GetVertexies();
    for(my $i = 0;$i <= $#$ptrArrVerexies;$i++)
    {
	my $currVertex1 = $ptrArrVerexies->[$i];
	my $ptrArrVertexies2 = $self->GetAllVertexesConnectedToVertex($currVertex1);
	next if($ptrArrVertexies2 == -1);
	for(my $j = 0;$j <= $#$ptrArrVertexies2;$j++)
	{
	    my $currVertex2 = $ptrArrVertexies2->[$j];
	    
	    die if(not exists $self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"});
	    die if(not exists $self->{"Edges"}->{"$currVertex2"}->{"$currVertex1"});
	    next if($self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"} == -1);
	    my $ptrArr = $self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"};
	    my $weight = $ptrArr->[$isIdent];
	    if ($weight<$treshold) {
		$self->LogRemovingEdge($currVertex1,$currVertex2);
		$self->LogRemovingEdge($currVertex2,$currVertex1);
		$self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"} = -1;
		$self->{"Edges"}->{"$currVertex2"}->{"$currVertex1"} = -1;
	    }
	}
    }
}


sub DepthFirstSearch
{
    my $self = shift;
    my $vertex = shift;
    my $ptrVisited = shift;
    my $ptrArrVertexies = shift;
    
    
    for(my $i = 0;$i <= $#$ptrArrVertexies;$i++)
    {
	my $currVertex = $ptrArrVertexies->[$i];
	die if(not exists $self->{"Edges"}->{"$vertex"}->{"$currVertex"});
	die if(not exists $self->{"Edges"}->{"$currVertex"}->{"$vertex"});
	
	next if($self->{"Edges"}->{"$vertex"}->{"$currVertex"} == -1);
	next if(exists $ptrVisited->{"$currVertex"});
	
	$ptrVisited->{"$currVertex"} = 1;
	print FTW "\t$currVertex";
	my $ptrArrVertexies2 = $self->GetAllVertexesConnectedToVertex($currVertex);
	if($ptrArrVertexies2 == -1)
	{
	    print FTW "\n";
	    next;
	}
	
	$self->DepthFirstSearch($currVertex,$ptrVisited,$ptrArrVertexies2);
	
	
    }
    
    
}


sub PrintOrthogroups
{
    my $self = shift;
    my $fileName = shift;
    
    open(FTW,">$fileName") or die;
    
    my $ptrArrVerexies = $self->GetVertexies();
    
    my %vertexPrinted = ();
    
    for(my $i = 0;$i <= $#$ptrArrVerexies;$i++)
    {
	my $currVertex1 = $ptrArrVerexies->[$i];
	
	next if(exists $vertexPrinted{"$currVertex1"});
	$vertexPrinted{"$currVertex1"} = 1;
	print FTW "$currVertex1";
	
	my $ptrArrVertexies2 = $self->GetAllVertexesConnectedToVertex($currVertex1);
	if($ptrArrVertexies2 == -1)
	{
	    print FTW "\n";
	    next;
	}
	
	$self->DepthFirstSearch($currVertex1,\%vertexPrinted,$ptrArrVertexies2);
	print FTW "\n";
	
    }
    
    
    close(FTW);
}

sub GetNumberOfPairs
{
    my $self = shift;
    my $ptrArrVerexies = $self->GetVertexies();
    my %vertexPrinted = ();
    my $countNumberOfPairs = 0;
    for(my $i = 0;$i <= $#$ptrArrVerexies;$i++)
    {
	my $currVertex1 = $ptrArrVerexies->[$i];
	my $ptrArrVertexies2 = $self->GetAllVertexesConnectedToVertex($currVertex1);
	next if($ptrArrVertexies2 == -1);
	for(my $j = 0;$j <= $#$ptrArrVertexies2;$j++)
	{
	    my $currVertex2 = $ptrArrVertexies2->[$j];
	    
	    #die if(not exists $self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"});
	    #die if(not exists $self->{"Edges"}->{"$currVertex2"}->{"$currVertex1"});
	    next if($self->{"Edges"}->{"$currVertex1"}->{"$currVertex2"} == -1);
	    next if(exists $vertexPrinted{"$currVertex1\t$currVertex2"});
	    next if(exists $vertexPrinted{"$currVertex2\t$currVertex1"});
	    $vertexPrinted{"$currVertex1\t$currVertex2"} = 1;
	    $countNumberOfPairs++;
	}
    }
    return $countNumberOfPairs;
}


sub CheckNumberOfOrthoPairsConnected
{
    my $self = shift;
    my $ptrOrthoPairs = shift;
    
    my @arrOrthoPairs = keys %$ptrOrthoPairs;
    my $countOrthoPairsConnected = 0;
    
    for(my $i = 0;$i <= $#arrOrthoPairs;$i++)
    {
	my $currOrthoPair = $arrOrthoPairs[$i];
	my @arrPair = split(/\t/,$currOrthoPair);
	my $currAth = $arrPair[0];
	my $currFesc = $arrPair[1];
	
	my $weight = $self->GetEdgeWeight($currAth,$currFesc);
	
	if ($weight == -1) {
	    next;
	}
	$countOrthoPairsConnected++;
    }
    
    return $countOrthoPairsConnected;
}

sub GetArrOfWeightsInConnectedComponent
{
    my $self = shift;
    my $vertex = shift;
    my $ptrArrWeights = shift;
    my $isIdent = shift;
    my $ptrArrVisited = shift;
    my $ptrArrEdgesVisited = shift;
    
    my $ptrConToVertexList = $self->GetAllVertexesConnectedToVertex($vertex);
    
    for(my $i = 0;$i <= $#$ptrConToVertexList;$i++)
    {
	my $currVertex = $ptrConToVertexList->[$i];
	
	
	next if($self->{"Edges"}->{"$vertex"}->{"$currVertex"} == -1);
	#die("$vertex\t$currVertex\n") if(not exists $self->{"Edges"}->{"$vertex"}->{"$currVertex"});
	#die if(not exists $self->{"Edges"}->{"$currVertex"}->{"$vertex"});
	
	next if(exists $ptrArrEdgesVisited->{"$vertex\t$currVertex"});
	next if(exists $ptrArrEdgesVisited->{"$currVertex\t$vertex"});
	
	my $ptrArrCurrWeights = $self->GetEdgeWeight($vertex,$currVertex);
	
	my $weightVal = $ptrArrCurrWeights->[$isIdent];
	push @$ptrArrWeights,$weightVal;
	$ptrArrEdgesVisited->{"$vertex\t$currVertex"} = 1;
	$ptrArrEdgesVisited->{"$currVertex\t$vertex"} = 1;
	
	next if(exists $ptrArrVisited->{"$currVertex"});
	$ptrArrVisited->{"$currVertex"} = 1;
	
	$self->GetArrOfWeightsInConnectedComponent($currVertex,$ptrArrWeights,$isIdent,$ptrArrVisited,$ptrArrEdgesVisited);
	
	
	
    }
    
    
}

sub GetMedianOfWeightsInConnectedComponent
{
    my $self = shift;
    my $vertex = shift;
    my $isIdent = shift;
    my $ptrVisited = shift;
    
    #my %visited = ();
    my %edgesVisited = ();
    $ptrVisited->{"$vertex"}=1;
    
    my @arrWeights = ();
    
    $self->GetArrOfWeightsInConnectedComponent($vertex,\@arrWeights,$isIdent,$ptrVisited,\%edgesVisited);
    my $countTmp = $#arrWeights+1;
    my @sortArrWeights = sort {$a <=> $b} @arrWeights;
    
    my $sizeOfArrVal = $#sortArrWeights+1;
    my $medianVal = 0;
    if ($sizeOfArrVal%2 == 0) {
        my $middleIndex1 = $sizeOfArrVal/2;
        my $middleIndex2 = $middleIndex1-1;
        $medianVal = ($sortArrWeights[$middleIndex1] + $sortArrWeights[$middleIndex2])/2;
        
    }
    else
    {
        my $middleIndex = int($sizeOfArrVal/2);
        $medianVal = $sortArrWeights[$middleIndex];
    }
    
    return $medianVal;
}


sub GetMeanOfWeightsInConnectedComponent
{
    my $self = shift;
    my $vertex = shift;
    my $isIdent = shift;
    my $ptrVisited = shift;
    
    #my %visited = ();
    my %edgesVisited = ();
    $ptrVisited->{"$vertex"}=1;
    
    my @arrWeights = ();
    
    $self->GetArrOfWeightsInConnectedComponent($vertex,\@arrWeights,$isIdent,$ptrVisited,\%edgesVisited);
    my $countTmp = $#arrWeights+1;
    #my @sortArrWeights = sort {$a <=> $b} @arrWeights;
    
    my $meanVal = 0;
    
    for(my $i = 0;$i <= $countTmp;$i++)
    {
	$meanVal += $arrWeights[$i];
    }
    
    if ($countTmp != 0) {
	$meanVal = $meanVal/$countTmp;
    }
    else
    {
	$meanVal = 0;
    }
    
    
    
    return $meanVal;
}



sub GetListOfVertexiesInConnectedComponent
{
    my $self = shift;
    my $vertex = shift;
    my $ptrArrVisited = shift;
    my $ptrArrList = shift;
    
    my $ptrConToVertexList = $self->GetAllVertexesConnectedToVertex($vertex);
    print "$vertex\n";
    return if($#$ptrConToVertexList == -1);
    for(my $i = 0;$i <= $#$ptrConToVertexList;$i++)
    {
	my $currVertex = $ptrConToVertexList->[$i];
	
	next if(exists $ptrArrVisited->{"$currVertex"});
	next if($self->{"Edges"}->{"$vertex"}->{"$currVertex"} == -1);
	#die if(not exists $self->{"Edges"}->{"$vertex"}->{"$currVertex"});
	#die if(not exists $self->{"Edges"}->{"$currVertex"}->{"$vertex"});
	
	
	$ptrArrVisited->{"$currVertex"} = 1;
	push @$ptrArrList,$currVertex;
	
	$self->GetListOfVertexiesInConnectedComponent($currVertex,$ptrArrVisited,$ptrArrList);
	
	
	
    }
    
    
    
}


sub RemoveEdgeWithWeightsBelowTresholdInConnectedComponent
{
    my $self = shift;
    my $vertex = shift;
    my $weightTreshold = shift;
    my $isIdent = shift;
    my $ptrArrVisited = shift;
    my $ptrArrEdgesVisited = shift;
    
    my $ptrConToVertexList = $self->GetAllVertexesConnectedToVertex($vertex);
    
    for(my $i = 0;$i <= $#$ptrConToVertexList;$i++)
    {
	my $currVertex = $ptrConToVertexList->[$i];
	
	
	next if($self->{"Edges"}->{"$vertex"}->{"$currVertex"} == -1);
	#die if(not exists $self->{"Edges"}->{"$vertex"}->{"$currVertex"});
	#die if(not exists $self->{"Edges"}->{"$currVertex"}->{"$vertex"});
	next if(exists $ptrArrEdgesVisited->{"$vertex\t$currVertex"});
	next if(exists $ptrArrEdgesVisited->{"$currVertex\t$vertex"});
	
	$ptrArrEdgesVisited->{"$vertex\t$currVertex"} = 1;
	$ptrArrEdgesVisited->{"$currVertex\t$vertex"} = 1;
	
	my $ptrArrWeights = $self->GetEdgeWeight($vertex,$currVertex);
	
	my $weightVal = $ptrArrWeights->[$isIdent];
	
	
	if ($weightVal < $weightTreshold) {
	    $self->LogRemovingEdge($vertex,$currVertex);
	    $self->LogRemovingEdge($currVertex,$vertex);
	    $self->{"Edges"}->{"$vertex"}->{"$currVertex"} = -1;
	    $self->{"Edges"}->{"$currVertex"}->{"$vertex"} = -1;
	}
	
	next if(exists $ptrArrVisited->{"$currVertex"});
	$ptrArrVisited->{"$currVertex"} = 1;
	$self->RemoveEdgeWithWeightsBelowTresholdInConnectedComponent($vertex,$weightTreshold,$isIdent,$ptrArrVisited);
	
	
	
    }
    
    
}

sub PrintOrthogroupInFile
{
    my $self = shift;
    my $vertex = shift;
    my $ptrVisited = shift;
    my $isIdent = shift;
    my $ptrEdgesVisited = shift;
    my $outFile = shift;
    my $ptrTrivialNames = shift;
    #print "$outFile\n";
    
    
    my $ptrConToVertexList = $self->GetAllVertexesConnectedToVertex($vertex);
    return if($ptrConToVertexList == -1);
    for(my $i = 0;$i <= $#$ptrConToVertexList;$i++)
    {
	my $currVertex = $ptrConToVertexList->[$i];
	
	next if($self->{"Edges"}->{"$vertex"}->{"$currVertex"} == -1);
	
	next if(exists $ptrEdgesVisited->{"$vertex\t$currVertex"});
	next if(exists $ptrEdgesVisited->{"$currVertex\t$vertex"});
	
	my $ptrArrWeights = $self->GetEdgeWeight($vertex,$currVertex);
	
	
	my $weightValIdent = sprintf("%.2f",$ptrArrWeights->[1]);
	my $weightValPred = sprintf("%.2f",$ptrArrWeights->[0]);
	
	$ptrEdgesVisited->{"$vertex\t$currVertex"} = 1;
	$ptrEdgesVisited->{"$currVertex\t$vertex"} = 1;
    	open(FTW,">>$outFile") or die("$outFile");
	
	my $vertexToPrint = $vertex;
	my $currVertexToPrint = $currVertex;
	
	if (substr($vertexToPrint,0,2) eq "AT") {
	    my $trivNam = $vertexToPrint;
	    if (exists $ptrTrivialNames->{"$vertexToPrint"}) {
		$trivNam = $ptrTrivialNames->{"$vertexToPrint"};
	    }
	    $vertexToPrint = "$vertexToPrint\_$trivNam";
	}
	if (substr($currVertexToPrint,0,2) eq "AT") {
	    my $trivNam = $currVertexToPrint;
	    if (exists $ptrTrivialNames->{"$currVertexToPrint"}) {
		$trivNam = $ptrTrivialNames->{"$currVertexToPrint"};
	    }
	    $currVertexToPrint = "$currVertexToPrint\_$trivNam";
	}
	
	print FTW "$vertexToPrint -- $currVertexToPrint [label=\"$weightValPred;$weightValIdent\"];\n";
	close(FTW);
	next if(exists $ptrVisited->{"$currVertex"});
	$ptrVisited->{"$currVertex"} = 1;
	
	
	$self->PrintOrthogroupInFile($currVertex,$ptrVisited,$isIdent,$ptrEdgesVisited,$outFile,$ptrTrivialNames);
	
    }
    
    
}


sub IsAthGene
{
    my $self = shift;
    my $vertex = shift;
    
    if (substr($vertex,0,2) eq "AT") {
	return 1;
    }
    return 0;
    
    
}

sub IsFescGene
{
    my $self = shift;
    my $vertex = shift;
    
    if (substr($vertex,0,2) eq "AM") {
	return 1;
    }
    return 0;
    
    
}




1;
