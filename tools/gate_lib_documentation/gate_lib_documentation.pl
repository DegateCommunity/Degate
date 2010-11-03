#!/usr/bin/perl
#
# This is a simple perl script, that reads a gate library and prints parts
# of it into a HTML page.
#

use strict;
use XML::DOM;
use Data::Dumper;
use Getopt::Long;

#
# parse command line
#

my $project_dir;
my $out_dir = ".";
my $help = '';
my $resize = 200;

GetOptions ("project=s" => \$project_dir,
	    "out=s" => \$out_dir,
	    "resize=i" => \$resize,
	    "help" => \$help );


if($help or not defined($project_dir)) {
    print 
	"\n",
	"gate_lib_documentation.pl <options>\n",
	"\n",
	"  --help                  -- this screen\n",
	"  --project <project-dir> -- the project directory\n",
	"  --out <out-dir>         -- output directory (default: working dir)\n",
	"  --resize <percent>      -- resize images (default 100)\n",
	"\n\n";
    exit;
}

print "Project directory is $project_dir\n";
print "Will write HTML data to directory $out_dir\n";

#
# Prepare
#

if(! -d $out_dir) {
    print "Directory $out_dir does not exists. Create directory.\n";
    mkdir($out_dir) || die "Can't create directory $out_dir\n";
}

my $html = '';
my $gate_lib_file = $project_dir . '/gate_library.xml';

my $parser = new XML::DOM::Parser;
my $doc = $parser->parsefile($gate_lib_file);

if(not defined $doc) {
    print "Can't read gate library file $gate_lib_file\n";
}

#
# Generate HTML data
#


my @matching_gates = $doc->getElementsByTagName("gate");
foreach my $gate (@matching_gates) {

    $html .= 
	"<h3>" . $gate->getAttribute('name') .
	": " . $gate->getAttribute('description') . "</h3>\n";


    my %images;
    my @images = $gate->getElementsByTagName("image");
    foreach my $img (@images) {
	$images{$img->getAttribute('layer-type')} =  
	    $img->getAttribute('image');
    }

    my @avail_layers = 
	grep { exists($images{$_}) } qw(transistor logic metal);

    my @processed_images = map { 
	render_images($project_dir,
		      $out_dir,
		      $images{$_},
		      $resize,
		      $gate) } @avail_layers;

    foreach my $img (@processed_images) {
	$html .= "<img src=\"$img\"/>\n";
    }

    # merge images
    my @merged_images;

    if(exists($images{transistor}) &&
       exists($images{logic})) {
	push(@merged_images, merge_images($project_dir,
					  $out_dir,
					  $images{transistor}, 
					  $images{logic},
					  $resize,
					  $gate));
    }

    if(exists($images{logic}) &&
       exists($images{metal})) {
	push(@merged_images, merge_images($project_dir,
					  $out_dir,
					  $images{logic}, 
					  $images{metal},
					  $resize,
					  $gate));
    }
       
    $html .= "<br/>";
    foreach my $img (@merged_images) {
	$html .= "<img src=\"$img\"/>\n";
    }
    
}

#
# Write HTML data
#

my $out_file = $out_dir . '/index.html';
print "Write HTML file $out_file\n";
open(HTML_OUT, "> $out_file") || die "Can't write $out_file: $!\n";
print HTML_OUT render_html_header() . $html . render_html_footer();
close(HTML_OUT);

print "Rendering done. Just run\n\n",
    "  firefox $out_file\n\n";


#
# Helper functions
#



sub render_images {
    my $project_dir = shift;
    my $out_dir = shift;
    my $f1 = shift;
    my $resize = shift;
    my $node = shift;

    my $radius = 1;
    my @params = ('-resize', "${resize}%",
		  '-strokewidth', 1);


    my @ports = $node->getElementsByTagName("port");
    foreach my $port (@ports) {
	my $x = $port->getAttribute('x') * $resize / 100;
	my $y = $port->getAttribute('y') * $resize / 100;
	my $name = $port->getAttribute('name');
	push(@params, 
	     '-fill', 'red', '-stroke', 'red',
	     '-draw', sprintf('circle %d,%d %d,%d',
			      $x, $y, $x, $y+$radius),
	     '-fill', 'white', '-stroke', 'white',
	     '-draw', sprintf('text %d,%d "%s"', 
			      $x + $radius, $y + $radius, $name));
    }
    

    if(defined $f1) {
	my $out_file = $f1;
	$out_file =~ s!\.tif$!.png!;

	print "Convert image $f1.\n";
	if((system("convert",
		   $project_dir . '/' . $f1,
		   @params,
		   $out_dir . '/' . $out_file) >> 8)) {
	    die "system() failed: $!\n";
	}
	return $out_file;
    }

    return undef;
}


sub merge_images {
    my $project_dir = shift;
    my $out_dir = shift;
    my $f1 = shift;
    my $f2 = shift;
    my $resize = shift;
    my $node = shift;

    
    if(defined($f1) && defined($f2)) {
	
	my $out_file = 'merged_' . $f1;
	$out_file =~ s!\.tif$!.png!;

	my $radius = 1;
	my @params = ('-resize', "${resize}%",
		      '-alpha', 'Set',
		      '-compose', 'dissolve',
		      '-set', 'option:compose:args', '50,50',
		      '-composite',);

	my @ports = $node->getElementsByTagName("port");
	foreach my $port (@ports) {
	    my $x = $port->getAttribute('x') * $resize / 100;
	    my $y = $port->getAttribute('y') * $resize / 100;
	    my $name = $port->getAttribute('name');
	    push(@params, 
		 '-fill', 'red', '-stroke', 'red',
		 '-draw', sprintf('circle %d,%d %d,%d',
				  $x, $y, $x, $y+$radius),
		 '-fill', 'white', '-stroke', 'white',
		 '-draw', sprintf('text %d,%d "%s"', 
				  $x + $radius, $y + $radius, $name));
	}
	
	print "Merge images $f1 and $f2.\n";
	if((system("convert",
		   $project_dir . '/' . $f1,
		   $project_dir . '/' . $f2,
		   @params,		   
		   $out_dir . '/' . $out_file
	    ) >> 8)) {
	    die "system() failed: $!\n";
	}
	return $out_file;
    }

    return undef;
}

sub render_html_header {
    return "<html><body>\n";
}

sub render_html_footer {
    return "</body></html>\n";
}

