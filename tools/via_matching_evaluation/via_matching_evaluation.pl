#!/usr/bin/perl

use strict;
use XML::Simple;
use Data::Dumper;

my $project_dir1 = shift || '/home/martin/degate/xcorrtest1-orig';
my $project_dir2 = shift || '/home/martin/degate/xcorrtest1';


my $lmodel1 = parse($project_dir1 . '/lmodel.xml', 'id');
my $lmodel2 = parse($project_dir2 . '/lmodel.xml', 'id');

my $dist_threshold = 5;

print "-----------------------------------------------------------------------\n";
my $matches = compare_placements($lmodel1, $lmodel2, $dist_threshold);
print "-----------------------------------------------------------------------\n";
my $reverse_matches = compare_placements($lmodel2, $lmodel1, $dist_threshold);
print "-----------------------------------------------------------------------\n";
print "\n";

print "Reference logic model has " . count($lmodel1) . " vias.\n";
print "Logic model 2 has " . count($lmodel2) . " vias.\n";
print "\n";

print "Found $matches vias from reference logic model in lmodel 2.\n";
print "That is a recognition rate of ", 100*$matches/count($lmodel1), " %.\n";
print "\n";

print "Found $reverse_matches vias from lmodel 2 in reference logic model.\n";
my $false_matches = count($lmodel2) - $reverse_matches;
print "Lmodel 2 has $false_matches vias that are not in the reference logic model.\n";

print "That is a false positive rate of ", 100*$false_matches/count($lmodel2), " %.\n";
print "\n";


sub count {
    my $lmodel = shift;
    return scalar(keys %{$lmodel->{'vias'}->{'via'}});
}

sub compare_placements {
    my ($lmodel1, $lmodel2, $dist_threshold) = @_;

    my $matches = 0;

    foreach my $v_id1 (keys %{$lmodel1->{'vias'}->{via}}) {

	my $v1 = $lmodel1->{'vias'}->{via}->{$v_id1};
	my $x1 = $v1->{"x"};
	my $y1 = $v1->{"y"};
	
	print "id=$v_id1 x=$x1 y=$y1\n";
	

	foreach my $v_id2 (keys %{$lmodel2->{'vias'}->{via}}) {

	    my $v2 = $lmodel1->{'vias'}->{via}->{$v_id2};
	    my $x2 = $v2->{"x"};
	    my $y2 = $v2->{"y"};
	
	    print "id=$v_id2 x=$x2 y=$y2\n";
	    
	    my $dx = $x2 - $x1;
	    my $dy = $y2 - $y1;

	    if((sqrt($dx*$dx + $dy*$dy) < $dist_threshold) ) {
		print "    found corresponding via: id=$v_id2 x=$x2 y=$y2\n";
		$matches++;
	    }
	}
    }

    return $matches;
}


sub parse {
    my $file = shift;
    my $key_attr = shift;
    my $response = XMLin($file, KeyAttr => [$key_attr]);
#    print Dumper($response);
    return $response;
}
