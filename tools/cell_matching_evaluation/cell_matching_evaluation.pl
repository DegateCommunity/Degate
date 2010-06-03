#!/usr/bin/perl

use strict;
use XML::Simple;
use Data::Dumper;

#my $project_dir = shift || '/degate-data/degate-0.0.7_2010-01-14_1329_SC14421CVF_dect/subproject_1/';
my $project_dir = shift || '/degate-data/2010-01-13_0309_legic/subproject_1';

my $lmodel1 = parse($project_dir . '/lmodel.xml.orig', 'id');
my $lib1 = parse($project_dir . '/gate_library.xml.orig', 'type-id');
my $entries1 = 

my $lmodel2 = parse($project_dir . '/lmodel.xml', 'id');
my $lib2 = parse($project_dir . '/gate_library.xml', 'type-id');

my $dist_threshold = 5;

print "-----------------------------------------------------------------------\n";
my $matches = compare_placements($lmodel1, $lib1, $lmodel2, $lib2, $dist_threshold);
print "-----------------------------------------------------------------------\n";
my $reverse_matches = compare_placements($lmodel2, $lib2, $lmodel1, $lib1, $dist_threshold);
print "-----------------------------------------------------------------------\n";

print "Reference logic model has " . count($lmodel1) . " gates.\n";
print "Logic model 2 has " . count($lmodel2) . " gates.\n";

print "Found $matches gates from reference logic model in lmodel 2.\n";
print "That is a recognition rate of ", 100*$matches/count($lmodel1), " %.\n";

print "Found $reverse_matches gates from lmodel 2 in reference logic model.\n";
my $false_matches = count($lmodel2) - $reverse_matches;
print "Lmodel 2 has $false_matches gates that are not in the reference logic model.\n";
print "That is a false positive rate of ", 100*$false_matches/count($lmodel2), " %.\n";

#print "Score = $score\n";
#print "Possible number of mismatches = ", $max_mismatches, "\n";
#print "Recognition rate = " . (100 * $score / $max_mismatches) . "\n";

sub count {
    my $lmodel = shift;
    return scalar(keys %{$lmodel->{'gates'}->{gate}});
}

sub compare_placements {
    my ($lmodel1, $lib1, $lmodel2, $lib2, $dist_threshold) = @_;

    my $matches = 0;

    foreach my $gate_id1 (keys %{$lmodel1->{'gates'}->{gate}}) {

	my $gate1 = $lmodel1->{'gates'}->{gate}->{$gate_id1};
	my $x1 = $gate1->{"min-x"};
	my $y1 = $gate1->{"min-y"};
	my $name1 = $gate1->{"name"};
	my $cell_id1 = $gate1->{'type-id'};	
	my $cell1 = get_cell($cell_id1, $lib1);      
	my $type_name1 = $cell1->{name};
	my $orientation1 = $gate1->{'orientation'};
	
	print "$type_name1 id=$gate_id1 x=$x1 y=$y1 orientation=$orientation1\n";
	
	foreach my $gate_id2 (keys %{$lmodel2->{'gates'}->{gate}}) {
	    
	    my $gate2 = $lmodel2->{'gates'}->{gate}->{$gate_id2};
	    my $x2 = $gate2->{"min-x"};
	    my $y2 = $gate2->{"min-y"};
	    my $name2 = $gate2->{"name"};
	    my $cell_id2 = $gate2->{'type-id'};
	    my $cell2 = get_cell($cell_id2, $lib2);
	    my $type_name2 = $cell2->{name};
	    my $orientation2 = $gate1->{'orientation'};
	    
	    my $dx = $x2 - $x1;
	    my $dy = $y2 - $y1;

	    if((sqrt($dx*$dx + $dy*$dy) < $dist_threshold) &&
	       ($orientation1 eq $orientation2) &&
	       ($type_name1 eq $type_name2) ) {
		print "    found corresponding gate: $type_name2 id=$gate_id2 x=$x2 y=$y2 orientation=$orientation2\n";
		$matches++;
	    }
	}
    }

    return $matches;
}

sub get_cell {
    my $id = shift;
    my $lib = shift;
    foreach my $gate_id (keys %{$lib->{'gate-templates'}->{gate}}) {
	my $r = $lib->{'gate-templates'}->{gate}->{$gate_id};
	if($gate_id == $id) { return $r; }
    }
    return undef;
}

sub get_cell_type_for_id {
    my $id = shift;

}

sub parse {
    my $file = shift;
    my $key_attr = shift;
    my $response = XMLin($file, KeyAttr => [$key_attr]);
#    print Dumper($response);
    return $response;
}
