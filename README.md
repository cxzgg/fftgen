# fftGen

Code generator to create code for a Fast Fourier Transform with the target to
speed up the transform by unrolling its internal loops together with additional
optimization measures.

Version 1 2021-Jun-21st jost


## Features

 * Generate C code to compute a Fast Fourier Transform (FFT) and an Inverse Fast
   Fourier Transform (IFFT).
 * Target of the generated code is to compute the algorithms in a very fast way.
   This is achieved by

   1. unrolling the loops,

   2. removing operations where sequence elements are multiplied by sine or
      cosine function values being zero or one,

   3. optionally removing operations which are obsolete in certain
      circumstances, see documentation for details.

   Note that optimization 1. comes at the expense of code size.


## <a id="Doc">Documentation</a>

A detailed user manual is provided as HTML file
[**doc/html/index.html**](doc/html/index.html). This file needs
`doc/html/doxy.css` for correct display.


## Code Size

Unrolling the loops increases the code size drastically. The total number of
code lines for a certain number `n` of FFT data points is

    n       Total LOC
    8       84
    16      228
    32      552
    64      1320
    128     3024
    256     6844
    512     15264
    1024    33696

Some of the optimizations reduce the number of code lines. For details on the
size of the generated source code see the [documentation](#Doc).


## <a id="Int">Integration</a>

The program generates the FFT code only, no function header and no function
ending. Also no definition or declaration of required variables is generated.
See the [documentation](#Doc) for details how to obtain according appropriate
code.

Advantage of this approach is that no assumption is required for the generated
code regarding the concrete type of the involved variables. The arrays and
variables can therefore be of any valid floating point type, e.g. `double` or
`float`.

The generated code must be copied manually into the source code of the target
program or included by an `#include` pre-processor statement. Details are
described in the documentation.


## <a id="Configuration">Configuration</a>

The number of FFT data points, the type of FFT (standard or inverse) and the
kind of optimization can be chosen using command line options when calling the
program, for details see the [documentation](#Doc).

But there are two additional options to configure the program. These have to be
configured when compiling the program on the compiler command line.

1.  The generated code is, as described in the [previous section](#Int),
    independent from the type of the involved variables. However, the generated
    literal constants do not have any type specifiers. They are thus of the
    default `double` type (usually 64-bit precision). According to the C
    standard are the arithmetic expressions in that case also to be realized in
    `double` arithmetic by the compiler.

    If the involved variables are defined as `float` (usually 32-bit precision)
    then to increase speed and save space some compilers might realize the
    arithmetic (as meant) also as `float` arithmetic because there is no other
    type involved beside the literal constants.

    However, the appearance of the literal constants can be changed to be on the
    save side in those cases. The format of those literal constants is defined
    internally in the source code `fftGen.c` by the pre-processor constant
    **NUMBER_FORMAT**. This constant has by default the value `"%21.14e"`,
    meaning double precision constants in scientific notation (`e`) with 14
    fractional digits and a total width of 21 characters.

    If the constants should e.g. be written as single precision ("`float`")
    constants with scientific notation, 7 fractional digits (usually sufficient
    for single precision) and a total width of 14 characters then the according
    format should look like `"%14.7ef"` (the trailing `f` denotes `float`). This
    will reduce the size of the source code as well as the size of the compiled
    program and increase the computation speed – compromising, however, the
    precision.

    To let this change be active the program must be recompiled with the new
    format specified on the compiler command line, usually using option `-D...`.

    For the described example the compiler command line should therefore
    additionally contain the argument `-DNUMBER_FORMAT='"%14.7ef"'`.

    If you don't use the GNU C compiler check your compiler's user manual on the
    exact way to specify pre-processor constants on the compiler command line.

2.  The generated code is by default not indented. However, indentation is
    sometimes desired, usually to increase readability. It is therefore possible
    to let `fftGen` indent the code.

    The indentation is internally in `fftGen`'s source code `fftGen.c` defined
    by pre-processor constant `INDENT`. This constant is by default empty, but
    it can also be changed on the compiler command line when `fftGen` is
    compiled.

    To let the generated code for example be indented by four spaces the
    compiler command line should additionally contain the argument
    `-DINDENT='"    "'` (in case of the GNU C compiler).

`fftGen` can be compiled using the provided [`Makefile`](Makefile) or the as
well provided shell script [`makefftgen.sh`](makefftgen.sh). To change the
configuration as described above edit that script and call it to recompile the
program.


## Deficiencies, Optional Improvements

It has been decided to precompute the numerous sine and cosine function values
required in the FFT and implement them as literal constants for speed.

But it would also be possible to store those precomputed constants in a
`static const` array. That could save a lot of space in the executable program
(but not necessarily – depending on the compiler).

However, the effect on the computation speed depends also on the compiler. The
indirect access could last longer than the immediate addressing, it could as
well also be equally fast (or even be a bit faster).

If the sine and cosine function values should be stored in a `const` array
instead being literal constants then the chosen design needs to be changed. It
is in that case no more possible to have only one type independent junk of
generated code as described in section [Integration](#Int).


## Building

The program is provided as single source code file [**`fftGen.c`**](fftGen.c).
A distribution of `fftGen` comes also with the file [**`fftGen`**](fftGen), a
compiled executable for an **x86_64** *Linux* system. Executables for most other
systems can be produced with a standard C compiler.

Building the executable and the documentation can most easily be done with the
[GNU Make Utility](https://www.gnu.org/software/make/). Open a command line
shell and change to the subdirectory `fftGen`. Type

    make help

to get an overview over the different available make targets.

### Compilation

You need a standard C99 C compiler to compile the program. The program has been
developed with the [GNU C compiler](https://gcc.gnu.org/).

The program is easiest compiled using the GNU Make Utility with:

    make fftGen

You can also call the provided shell script [`makefftgen.sh`](makefftgen.sh) to
compile the program. To adapt the format of the generated code edit that script
as described in section [Configuration](#Configuration).

### Creating the User Manual

The source of the user manual is also fftGen.c. To get the user manual
[Doxygen](http://www.stack.nl/~dimitri/doxygen/) and
[Perl](https://www.perl.org/) must be installed on your system.

To compile the documentation to an HTML user manual type

    make html

on the command line. The resulting user manual
[**doc/html/index.html**](doc/html/index.html) will be created in subdirectory
`doc/html`.

### Installation / Deinstallation

`fftGen` does not contain (and does not need) a particular installation
procedure. Just unpack the distribution archive to an arbitrary location on your
hard disk. Copy file `fftGen` (or the file created by your compiler if you
compiled `fftGen` new) to the location where you want to run it or to a
directory included in your `PATH` environment variable. This should also be
possible on Windows – you have to compile it for Windows though.

To deinstall `fftGen` just remove the files which have been extracted from the
distribution archive.


## <a id="Test">Test</a>

A regression test is provided. Start the test on the command line with

    make check

The test script needs the program [**test/fftTest.c**](test/fftTest.c). This
program runs the actual tests and will during the test be recompiled with
different options for every test case. Hint: `fftTest.c` might contain some
helpful constructions on the usage of `fftGen`.

If the tests finishes successfully it ends with message "test passed". Otherwise
an error message is displayed. The test results – `stdout.log` and `stderr.log`
– are created in subdirectory [**test**](test) as the messages logged from
`stdout` and `stderr`. References for both file can be found in
[**test/refs**](test/refs). The test script automatically compares the created
log files with those references after each test and reports any differences.

An additional result of the test is an HTML coverage report
[`test/fftGen.cov.html`](test/fftGen.cov.html). This file is a listing of the
source code of `fftGen`.

The test needs the GCC compiler with `gcov` to create the code instrumentation
and the coverage report. `Perl` is required to created the coverage report in
HTML and the GNU `diff` utility is required to compare the test result log files
with the references.


## License

GNU General Public License Version 3, see file [COPYING](COPYING) or
<https://www.gnu.org/licenses/>.
