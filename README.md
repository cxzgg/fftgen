# fftGen

Code generator to create code for a Fast Fourier Transform with the target to
speed up the transform by unrolling its internal loops together with additional
optimization measures.

Version 1 2021-Jun-21st jost.brachert@gmx.de


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


## <a id="Doc"/>Documentation

A detailed user manual is provided as HTML file **doc/html/index.html**. This
file needs `doc/html/doxygen.css` for correct display.

See [`makedoc.sh`](#mkdoc) on how to create the documentation.


## <a id="Test"/>Test

A regression test script for **bash** is provided as **maketest.sh**. The test
results are **test/stdout.log** and **test/stderr.log** and a test coverage
report as **test/fftGen.cov.html**.

The test script `maketest.sh` needs the program **test/fftTest.c**. This program
runs the actual tests and will during the test be recompiled with different
options for every test case. Hint: `fftTest.c` might contain some helpful
constructions on the usage of `fftGen`.

`test/stdout.log` and `test/stderr.log` contain the messages logged from
`stdout` and `stderr` during the test.

References for both files can be found in sub-directory **test/refs**.
`maketest.sh` automatically compares the current log files `test/stdout.log` and
`test/stderr.log` with those references after each test and reports any
differences.

Test coverage report `test/fftGen.cov.html` is a listing of the source code of
`fftGen`. In this listing are all

 - non-executable code lines highlighted *grey*,
 - executable code lines that were *executed* during the test highlighted
   *green*,
 - executable code lines which were *not executed* highlighted *red*.

Note that in version 1 of fftGen the still marked red code lines could not be
tested

 - because the according conditions cannot be provided, as e.g. malloc errors,
 - because the according option constellation is not used in the program, and
 - because the according expected code constellation does not occur in the
   Fourier Transform. The latter could, however, not generally be assumed.


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


## <a id="Int"/>Integration

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


## Configuration

The number of FFT data points, the type of FFT (standard or inverse) and the
kind of optimization can be chosen using command line options when calling the
program, for details see the [documentation](#Doc).

But there are two additional options to configure the program.

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

    Two let the generated code for example be indented by four spaces the
    compiler command line should additionally contain the argument
    `-DINDENT='"    "'` (in case of the GNU C compiler).

`fftGen` can be compiled using shell script `make.sh`. On how to use this script
see section [Utility Scripts](#mk).
`make.sh` calls the compiler. It must therefore be edited to change the compiler
command line in the above described way.


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


## <a id="Scr"/>Utility Scripts

-   <a id="mk"/>**`make.sh`**
    is a shell script to compile any C program consisting of only one source
    file and using only the math library. It expects the GNU C compiler
    (<https://gcc.gnu.org/>) be installed. It must be called with the filename
    of the program's source file without its extension. To compile `fftGen` use

        make.sh fftGen

-   <a id="mkdoc"/>**`makedoc.sh`**
    Shell script to create the documentation in `doc/html/index.html`. Source of
    the documentation is the program's source **fftGen.c**. `makedoc.sh` uses
    the files in sub-directory **doxygen**. It needs doxygen
    (<https://www.doxygen.nl/index.html>) be installed.

-   <a id="mktest"/>**`maketest.sh`**
    Shell script to perform a regression test of `fftGen`. Uses the files in
    sub-directory **test**, see section [Test](#Test).

-   **`makeclean.sh`**
    A number of intermediate temporary files are produced while creating the
    documentation by `makedoc.sh` and performing the test by `maketest.sh`.
    `makeclean.sh` removes those files again.

-   **`makedist.sh`**
    Shell script to create an archive of the total directory for distribution.


## Installation / Deinstallation

A distribution of `fftGen` comes with the file **`fftGen`**, a compiled
executable for an **x86_64** *Linux* system. Executables for most other
systems can be produced with a standard C compiler. Script [`make.sh`](#mk) can
be used as a hint how to call the compiler. There is only one C source file
`fftGen.c` to compile.

`fftGen` does not contain (and does not need) a particular installation
procedure. Just unpack the distribution archive to an arbitrary location on your
hard disk. Copy file `fftGen` (or the file created by your compiler if you
compiled `fftGen` new) to the location where you want to run it or to a
directory included in your `PATH` environment variable. This should also be
possible on Windows.

Note that on non-Unix/Linux/Posix systems the [shell scripts](#Scr) won't work.

To deinstall `fftGen` just remove the files which have been extracted from the
distribution archive.


## License

GNU General Public License Version 3, see file COPYING or
<https://www.gnu.org/licenses/>.
