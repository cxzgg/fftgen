#!/usr/bin/perl -w

my $version = 'Version 3';

################################################################################
#
# Source: gcov2html.pl
#
################################################################################
#
# Create documentation with
#  pod2html --infile=gcov2html.pl --outfile=gcov2html.pl.html --title "gcov2html.pl Manual"
#

################################################################################
# User Documentation

=head1 NAME

B<gcov2html.pl> - Convert the result of the GNU coverage testing tool to HTML
                  format


=head1 SYNOPSIS

B<gcov2html.pl> [I<Option>]... I<input-file>

Options:

[B<-v>] [B<-version>] [B<-h>] [B<--help>]


=head1 DESCRIPTION

gcov2html.pl converts the result of the GNU coverage testing tool (gcov) to
HTML format.  The result is written to STDOUT.


=head2 Arguments

=over 4

=item I<input-file>

Name of input file.

=back


=head2 Options

=over 4

=item B<-v>

=item B<-version>

Print version and exit..

=item B<-h>

=item B<--help>

Print usage information and terminate program.

=back


=head1 DIAGNOSTICS

Warnings and error messages are directed to STDERR.
There may be error messages from the Perl I/O system.


=head1 RETURN VALUE

Returns zero in case of normal successful completion.
In case of an error returns a value different from zero, normally the
value of errno (see PERL manual pages on "die").


=head1 BUGS

Not yet known.  If you encounter any error please report it to the author with
a brief description of the circumstances.


=head1 SEE ALSO

The documentation of the GNU coverage testing tool ("man 1 gcov" and
"man 1 gcc").


=head1 AUTHOR

jost.brachert@gmx.de


=head1 CHANGES

 Revision 4         Date: 15-Nov-2023           Author: jost
 '*' at end of flag for partly executed lines considered
 Legend added

 Revision 3         Date: 14-Feb-2018           Author: jost
 Documentation updated

 Revision 2         Date: 01-Oct-2017           Author: jost
 >, <, & will now be replaced by &gt;, &lt;, &amp;.

 Revision 1         Date: 14-Apr-2012           Author: jost
 Initial Revision


=head1 COPYING

gcov2html.pl is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the license, or (at your option) any later
version.

gcov2html.pl is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program, see file COPYING. If not, see L<https://www.gnu.org/licenses/>.

=cut

################################################################################


use strict;
use integer;

if (defined $ARGV[0]  and  substr($ARGV[0],0,2) eq '-v') {
    print __FILE__." $version\n";
    exit (0);
} elsif (not defined $ARGV[0]  or  substr($ARGV[0],0,1) eq '-') {
    print "Convert the result of gcov to html format\n".
          "usage: ".__FILE__." [options] <filename>\n".
          "            <filename>      Input filename.\n".
          "   options: -v  -version    Print version info and exit\n".
          "            -h  --help      Print usage information and exit\n";
    exit (0);
}

(my $title,) = $ARGV[0] =~ /^(.*)\.gcov$/;
print "<html><head><title>$title</title>\n";
print "<style type='text/css'><!--\n";
print "pre{font-size:x-small}\n";
print ".n{color:#4040FF}.e{color:#00D000}.p{color:#F08000}.u{color:#FF0000}.c{color:#808080}\n";
print "--></style>\n";
print "</head>\n";
print "<body><pre>\n";
while (my $inputline = <>) {
    chomp $inputline;
    my $match = (my $flag, my $lineno, my $line) = $inputline =~ /^([^:]+):\s*(\d+:+)(.*)$/;
    my $class = 'c';                            # Comment
    if    ($lineno =~ /^0:/ ) {$class = 'h';}   # Heading
    elsif ($flag =~ /#####/ ) {$class = 'u';}   # Unexecuted
    elsif ($flag =~ /\d$/   ) {$class = 'e';}   # Executed
    elsif ($flag =~ /\d\*?$/) {$class = 'p';}   # Partly executed

    $line =~ s/&/&amp;/g;
    $line =~ s/>/&gt;/g;
    $line =~ s/</&lt;/g;

    print "<span class='n'>$lineno</span> <span class='$class'>$line</span>\n";

    # Insert legend after "  -: 0:Runs:\d+"
    if ($inputline =~ /^\s+-:\s+0:Runs:\d+$/) {
        print "&nbsp;&nbsp;&nbsp;<span class='h'>Legend:</span>\n";
        print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class='c'>No code</span>\n";
        print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class='e'>Executed</span>\n";
        print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class='p'>Partly executed</span>\n";
        print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class='u'>Unexecuted</span>\n";
    }
}
print "</pre></body></html>\n";
