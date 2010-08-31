#!/usr/bin/perl -w

use strict;
use Frontier::RPC2;
use Data::Dumper;
use Data::Serializer;
use Tie::File;

# Parse channel
my $channel;
if($ENV{'QUERY_STRING'} =~ m!channel=([a-z]+)!) {
    $channel = $1;
}
else {
    http_error(500, "Internal Server Error") unless undef($channel);
}


my $filename = "/tmp/$channel.dat";
my $serializer = Data::Serializer->new(serializer => 'Storable');


process_cgi_call({'degate.push' => \&push,
		  'degate.pull' => \&pull});


sub push {
    print STDERR "push-request: " . Dumper(\@_);

    my @transactions;
    my $o = tie @transactions, 'Tie::File', $filename or 
	die "Can't open channel file '$filename': $!\n";
    $o->flock();

    my $tid = $#transactions + 1;
    $tid = 1 if($tid == 0);

    $transactions[$tid] = $serializer->serialize(\@_);
    untie @transactions;

    print STDERR "push-response: $tid\n";

    return $tid;
}

sub pull {
    my ($start_tid) = shift || 1;
    print STDERR "pull-request: " . Dumper(\@_);

    my @transactions;
    my $o = tie @transactions, 'Tie::File', $filename or 
	die "Can't open channel file '$filename': $!\n";
    $o->flock();

    my @results =  @transactions[$start_tid .. ($#transactions )];

    @results =  map { print STDERR "$_\n";
		      $_ = $serializer->deserialize($_) } @results;

    untie @transactions;

    print STDERR "pull-response: " . Dumper(\@results);

    return \@results;
}




#==========================================================================
#  CGI Support
#==========================================================================
#  Simple CGI support for Frontier::RPC2. You can copy this into your CGI
#  scripts verbatim, or you can package it into a library.
#  (Based on xmlrpc_cgi.c by Eric Kidd <http://xmlrpc-c.sourceforge.net/>.)

# Process a CGI call.
sub process_cgi_call ($) {
    my ($methods) = @_;
    
    # Get our CGI request information.
    my $method = $ENV{'REQUEST_METHOD'};
    my $type = $ENV{'CONTENT_TYPE'};
    my $length = $ENV{'CONTENT_LENGTH'};
 
    # Perform some sanity checks.
    http_error(405, "Method Not Allowed") unless $method eq "POST";
    http_error(400, "Bad Request") unless $type eq "text/xml";
    http_error(411, "Length Required") unless $length > 0;
    
    # Fetch our body.
    my $body;
    my $count = read STDIN, $body, $length;
    http_error(400, "Bad Request") unless $count == $length; 
    
    # Serve our request.
    my $coder = Frontier::RPC2->new;
    send_xml($coder->serve($body, $methods));
}

# Send an HTTP error and exit.
sub http_error ($$) {
    my ($code, $message) = @_;
    print <<"EOD";
    Status: $code $message
Content-type: text/html

<title>$code $message</title>
<h1>$code $message</h1>
<p>Unexpected error processing XML-RPC request.</p>
EOD
exit 0;
}

# Send an XML document (but don't exit).
sub send_xml ($) {
    my ($xml_string) = @_;
    my $length = length($xml_string);
    print <<"EOD";
    Status: 200 OK
Content-type: text/xml
Content-length: $length

EOD
    # We want precise control over whitespace here.
print $xml_string;
}
