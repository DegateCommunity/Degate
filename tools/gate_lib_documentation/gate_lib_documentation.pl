#!/usr/bin/perl
#
# This is a simple perl script, that reads a gate library and prints parts
# of it into a HTML page.
#

use strict;
use XML::Simple;
use Data::Dumper;
use Getopt::Long;

#
# parse command line
#

my $project_dir;
my $out_dir = ".";
my $help = '';
my $resize = 100;

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
my $lib = parse($gate_lib_file);
if(not defined $lib) {
    print "Can't read gate library file $gate_lib_file\n";
}

#
# Generate HTML data
#

foreach my $gate_id (keys %{$lib->{'gate-templates'}->{gate}}) {
    my $tmpl = $lib->{'gate-templates'}->{gate}->{$gate_id};

    $html .= "<h3>$tmpl->{name}: $tmpl->{description}</h3>\n";

    my $images = $tmpl->{images};

    my %images;
    foreach my $img (@$images) {
	$images{$img->{image}->{'layer-type'}} = $img->{image}->{image};
    }

    $html .= render_images($project_dir,
			   $out_dir,
			   $images{transistor},
			   $images{logic},
			   $images{metal},
			   $resize);
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
    my $f2 = shift;
    my $f3 = shift;
    my $resize = shift;

    my $ret = '';

    my @params = ('-resize', "${resize}%");

    if(defined $f1) {
	print "Convert image $f1.\n";
	if((system("convert", 
		   $project_dir . '/' . $f1,  
		   @params,
		   $out_dir . '/' . $f1 .".png") >> 8)) {
	    die "system() failed\n";
	}
	$ret .= "<img src=\"$f1.png\"/>\n";
    }

    if(defined $f2) {
	print "Convert image $f2.\n";
	if((system("convert", 
		   $project_dir . '/' . $f2,  
		   @params,
		   $out_dir . '/' . $f2 .".png") >> 8)) {
	    die "system() failed\n";
	}
	$ret .= "<img src=\"$f2.png\"/>\n";
	
    }

    if(defined $f3) {
	print "Convert image $f3.\n";
	if((system("convert", 
		   $project_dir . '/' . $f3,  
		   @params,
		   $out_dir . '/' . $f3 .".png") >> 8)) {
	    die "system() failed\n";
	}
	$ret .= "<img src=\"$f3.png\"/>\n";	
    }

    return $ret;
}

sub render_html_header {
    return "<html><body>\n";
}

sub render_html_footer {
    return "</body></html>\n";
}

sub parse {
    my $file = shift;
    print "Read gate library from file $file\n";
    my $response = XMLin($file, 
			 KeyAttr => ['type-id', 'layer-type'], 
			 ForceArray => ['images']);    
    return $response;
}
