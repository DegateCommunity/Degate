#!/usr/bin/perl

use strict;
use XML::Simple;
use Data::Dumper;

my $project_dir = shift || '/degate-data/2010-01-13_0309_legic/subproject_1';

my $lib = parse($project_dir . '/gate_library.xml');

print_html_header();

foreach my $gate_id (keys %{$lib->{'gate-templates'}->{gate}}) {
    my $tmpl = $lib->{'gate-templates'}->{gate}->{$gate_id};

#    print "Cell Name: ", $tmpl->{name}, "\n";
#    print "Cell Descr: ", $tmpl->{description}, "\n";
    # print Dumper($tmpl);
    print "<h3>$tmpl->{name}: $tmpl->{description}</h3>\n";

    my $images = $tmpl->{images};

    # print Dumper($images);
    my %images;
    foreach my $img (@$images) {
	# print Dumper($img);
	$images{$img->{image}->{'layer-type'}} = $img->{image}->{image};
    }
 #   print Dumper(\%images);
    render_images($project_dir . '/' . $images{logic});
}


print_html_footer();


sub render_images {
    my $f = shift;
    system("convert $f $f.png");
    print "<img src=\"$f.png\"/>\n";
}

sub print_html_header {
    print "<html><body>\n";
}

sub print_html_footer {
    print "</body></html>\n";
}

sub parse {
    my $file = shift;
    my $response = XMLin($file, 
			 KeyAttr => ['type-id', 'layer-type'], 
			 ForceArray => ['images']);    
    return $response;
}
