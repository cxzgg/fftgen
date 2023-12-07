//##############################################################################
// File: fftGen.c
//
// Version 1     2021 Jun 21st   jost.brachert@gmx.de
//
// Generate C code to compute a Fast Fourier Transform (FFT) and an Inverse Fast
// Fourier Transform (IFFT).
// Target of the generated code is to compute the algorithms in a very fast way.
// This is achieved by
// a) unrolling the loops,
// b) removing operations where sequence elements are multiplied by sine or
//    cosine function values being zero or one,
// c) optionally removing operations which are obsolete in certain
//    circumstances, see documentation for details.
// Note that optimization a) is at the expense of code size.
//
//------------------------------------------------------------------------------
// Copyright (C) 2021  Jost Brachert, jost.brachert@gmx.de
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the license, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program, see file COPYING. If not, see https://www.gnu.org/licenses/.
//

/*!
\mainpage fftGen

\section Name  NAME

fftGen - Generate code to compute an FFT or IFFT



\section Synopsis  SYNOPSIS

\b fftGen [option]...

\par Options:
[\c -n \e number] [\c \--points \e number]
[\c -i] [\c \--inverse]
[\c -r] [\c \--real-in-opt]
[\c -o] [\c \--real-out-opt]
[\c -m] [\c \--symm-in-opt]
[\c -s] [\c \--symm-out-opt]
[\c -l] [\c \--license]
[\c -v] [\c \--verbose]
[\c -V] [\c \--version]
[\c -h] [\c \--help]

Note that it is \e required to specify the number of data points with options
\c -n or \c \--points.



\section Description DESCRIPTION

The program generates code to compute a Fast Fourier Transform (FFT) or Inverse
Fast Fourier Transform (IFFT) for the C or C++ programming language. Target of
the generated code is to compute the algorithms in a very fast way.

The program needs to know the number of data points for the FFT. Either option
\c -n or \c \-\-points must therefore always be specified when the program is
called. The value of these options specifies the number of data points. It must
always be a power of two and it must not be zero. It may be equal one, however,
value one doesn't make much sense because in that case no code is produced.

If the program shall compute an inverse FFT then option \c -i or \c \-\-inverse
must be given as argument. Note that in this case the result is scaled by
\c n if \c n is the number of data points. To get the correct result of an
inverse FFT the results of the generated code must be divided by \c n.

The transform is conducted \"in place\", that means the resulting output
sequence overwrites the input sequence.


\subsection Optimizations Optimizations

The speed of computation is achieved by a number of optimizations of the
standard Cooley-Tukey algorithm:

1. Precomputing sine and cosine function values

   During an FFT a lot of sine and cosine function values are to be computed.
   Computation of these function values is time-consuming. However, they are
   fixed and known for any FFT of a certain number of data points. It is thus
   possible to compute those function values once before conducting the actual
   transform - at best already at compile time.

2. Unrolling loops

   The FFT is usually computed in several loops. However, executing the loop
   control statements needs some time. That time can be saved if the loops are
   unrolled, that means, instead of jumping from the end of a loop to its
   beginning the statements of the loop are copied in series.

   Drawback is certainly that the size of program increases.

3. Inserting precomputed sine and cosine function values immediately into the
   algorithm

   Both abovementioned optimizations offer this possibility. The precomputed
   function values can be inserted as literal constants directly into the source
   code of the unrolled loops. The access to those immediate constants is much
   faster than the indirect access to an array which would contain the
   precomputed values otherwise.

   However, this comes also at the downside of increased code size.

4. Removing multiplications by one

5. Removing summands in expressions which are the result of a multiplication by
   zero

6. Removing all operations assumed to be obsolete because the input sequence
   consists of real values only

   This optimization is optional because it will produce wrong results if the
   input sequence contains non-zero imaginary values.

   To activate this optimization use option \c -r.

   If this option is specified then it is not necessary to initialize the array
   for imaginary values because these values are ignored.

7. Removing all operations required to calculate points in the second half of
   the FFT result sequence

   If the input sequence consists of real values only then the values of the
   resulting sequence are symmetrical to the value with index n/2 (indices
   starting at 0) if n is the number of data points - according to the following
   expression:
   \code
     x[i] = x*[n-i]     for i=n/2+1...n-1
   \endcode
   x*[n-i] denoting the conjugate complex value to x[n-i]. The conjugate complex
   value of complex variable z has the same real value and the negative
   imaginary value of z.

   It is thus not necessary to calculate the points from n/2+1 on. They are
   usually not required and if needed they can anyway be calculated later in the
   way described above.

   This optimization is optional because it will produce wrong results if the
   input sequence contains non-zero imaginary values.

   To activate this optimization use option \c -s.

   Note that if this option is specified then the points from n/2+1 onwards
   contain useless arbitrary values.

8. Removing all operations assumed to be obsolete because the output sequence
   consists of real values only

   This optimization is optional because it will produce wrong results if the
   input sequence will produce non-zero imaginary values in the result sequence.

   To activate this optimization use option \c -o.

   The output sequence contains real only values in case of an IFFT with an
   input sequence being the result of an FFT with real only input.

   Note that a lot of manipulations of the result of the FFT that was used to
   generate the IFFT input sequence will spoil the property of generating real
   only output values in that IFFT. So use that option with care. It might be a
   good idea to first check the IFFT result before using this option.

   Note that if this option is specified then the imaginary values in the result
   sequence contain useless arbitrary values.

9. Removing all operations being obsolete because the input sequence is assumed
   to be symmetrical around the value with index n/2+1 (indices starting at 0)
   if n is the number of data points

   For the exact assumed symmetry relationship see abovementioned
   optimization 7. A sequence gets that kind of symmetry if it is the result of
   an FFT (or IFFT) with real only input data points.

   To activate this optimization use option \c -m.

   If this option is specified then it is not necessary to initialize the arrays
   for real and imaginary values from the value with index n/2+1 onwards
   (indices starting at 0) because these values are ignored.



\subsection Combinations Combinations of Optimizations

Optimizations 6. and 7. (options \c -r and \c -s) are both for real only
input sequences for normal FFT and are therefore usually specified together.

Optimizations 8. and 9. (options \c -o and \c -m) are both for symmetrical input
sequences for FFTs and IFFTs and are therefore usually specified together.

Optimization 8. (option \c -o) is usually for inverse FFTs only. It thus does
not make much sense to combine it with options \c -r or \c -s.



\section Size Size of the Generated Code

Unrolling the loops increases the code size drastically. If \c m is the power of
two exponent to compute the number of data points of the FFT then the generated
lines of code for the FFT internal binary inversion algorithm can be computed by
\code
LOC_B = 2**(m/2) - 2**((m-1)/2) * 6
\endcode
\*\* means exponentiation

The generated lines of code for the rest of the FFT transform can be calculated
by
\code
LOC_T = m*2**(m-1) * 6
\endcode
The total number of code lines for a certain number \c n of FFT data points is
\code
m   n       LOC_B   LOC_T   Total
3   8       12      72      84
4   16      36      192     228
5   32      72      480     552
6   64      168     1152    1320
7   128     336     2688    3024
8   256     720     6144    6844
9   512     1440    13824   15264
10  1024    2976    30720   33696
\endcode
Optimizations 6. to 9. will reduce the number of code lines a bit. For
<tt>m</tt>=5 (i.e. <tt>n</tt>=32) with optimizations 6. and 7. (options \c -r
and \c -s) the total number of code lines reduces to 365.



\section Integration INTEGRATION

The program generates the FFT code only, no function header and no function
ending. The generated code requires some defined variables:
- Two defined arrays for the complex sequence points, in particular
  - array <tt>xr[]</tt> for the real values and
  - array <tt>xi[]</tt>.
  .
  Both arrays must be of size \c n, where \c n is the number of data points of
  the FFT or IFFT.
- Two variables to store intermediate values:
  - <tt>tr</tt> and
  - <tt>ti</tt>.

Advantage of this approach is that no assumption is required for the generated
code regarding the concrete type of the involved variables. The arrays and
variables can therefore be of any valid floating point type, e.g. \c double or
\c float.

The generated code can easiest be integrated if it is piped to a file which is
then included via an \c \#include preprocessor statement into a function body
that provides the required definitions.

\b Example:

Assumed the generated code has been written to file \c fft.c and the FFT should
be computed in \c float arithmetic. Then the according code could look like
\code
void  fft (float *xr, float *xi) {
    float  tr, ti;
#include "fft.c"
}
\endcode

Function <tt>fft()</tt> can now be called to compute the FFT. The calling
program must
- define the two arrays and
- fill them with the input values for the FFT,
- then call function <tt>fft()</tt> with the pointers to those arrays as
  arguments,
- take the resulting transformed values from the arrays.
- In case of an inverse FFT the results must be divided by \c n the number of
  the data points.



\section Options  OPTIONS

\par \c -n, \c \-\-points \e number
Number of data points of the FFT. The number must be a power of two.\n
This option is not optional. It must be given to specify the required number of
data points.

\par \c -i, \c \-\-inverse
Generate code to calculate an inverse FFT.

\par \c -r, \c \-\-real-in-opt
Optimize the code assuming that the imaginary input values are all zero, see
\ref Optimizations.

\par \c -o, \c \-\-real-out-opt
Optimize the code assuming that in the result sequence all imaginary values are
zero. This is the case for an IFFT with an input sequence being the result of an
FFT with real only input. Use with care, see \ref Optimizations.

\par \c -s, \c \-\-symm-out-opt
Optimize the code assuming that the FFT result is symmetrical to element n/2 if
n is the number of data points passed with option \c -n (indices starting at 0).
The result of the FFT is symmetrical around n/2 if the imaginary input values
are all zero. For details see \ref Optimizations.

\par \c -m, \c \-\-symm-in-opt
Optimize the code assuming that the FFT input sequence is symmetrical to element
n/2 if n is the number of data points passed with option \c -n (indices starting
at 0), see \ref Optimizations.

\par \c -l, \c \-\-license
Write a short GPL 3 license note at the beginning of the generated code.

\par \c -v, \c \-\-verbose
Increase verbosity level, can be specified more than once. On level one writes
the chosen options to \c stderr.

\par \c -V, \c \-\-version
Print version information and exit.

\par \c -h, \c \-\-help
Print this info.



\section ExitStatus  EXIT STATUS

If no errors occurred in the course of the program the program terminates with
exit code \c 0. Otherwise it returns a system dependent error code different
from zero, see \c man \c stdlib.h -> \c EXIT_FAILURE.



\section Diagnostics  DIAGNOSTICS

Error messages are routed to \c stderr. They are sometimes accompanied by a
more detailed error description.

\par \"Unknown argument\"
\par \"Unknown option\"
\par \"Invalid option argument\"
Wrong command line argument or option. See \ref Synopsis or \ref Options.

\par \"Error allocating memory\"
Not enough heap memory available. It might help to close some applications.

\par \"No number of points specified\"
The number of points must be specified using option \c -n. It must be a power of
two. See \ref Synopsis or \ref Options.

\par \"Number of points is not a power of two\"
The number of data points specified with option \c -n must be a power of two.
See \ref Description or \ref Options.



\section KnownBugs  KNOWN BUGS

Not yet known. If you find one send an according note to the author.



\section Author  AUTHOR

jost.brachert@gmx.de



\section Copyright  COPYRIGHT

Copyright (C) 2021  Jost Brachert, jost.brachert@gmx.de

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the license, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program, see file COPYING. If not, see https://www.gnu.org/licenses/.

*/



#include  <stdio.h>
#include   <math.h>     // sin(),cos(),fabs()
#include <string.h>     // strlen(),strcat(),strncpy(),strcmp(),strerror()
#include <stdlib.h>     // malloc(),exit(),free(),EXIT_SUCCESS,EXIT_FAILURE,NULL
#include  <errno.h>     // errno

#define  LOGO       "fftGen"
#define  VERSION    "V1"

#define  OPTIMIZE_SINE_COSINE_VALUES

//------------------------------------------------------------------------------
// Options for formatting of the generated source code

#ifndef NUMBER_FORMAT
#define  NUMBER_FORMAT  "%21.14e"       // printf format for literal constants
#endif
#ifndef INDENT
#define  INDENT         ""              // Indentation of output code lines
#endif


//------------------------------------------------------------------------------
// Definitions and Declarations

static void  fftGen (int,int,int,int,int,int);  // Generating function


static char licenseText[] =
"// This program is free software: you can redistribute it and/or modify it under\n"
"// the terms of the GNU General Public License as published by the Free Software\n"
"// Foundation; either version 3 of the license, or (at your option) any later\n"
"// version.\n"
"//\n"
"// This program is distributed in the hope that it will be useful, but WITHOUT\n"
"// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
"// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more\n"
"// details.\n"
"//\n"
"// You should have received a copy of the GNU General Public License along with\n"
"// this program, see file COPYING. If not, see https://www.gnu.org/licenses/.\n"
;
static char header[] = "";          // Starting code of the generated function
static char footer[] = "";          // Ending of the generated function


//------------------------------------------------------------------------------
// Command line option descriptor

typedef
    struct Option {
            char  *shOpt;       // Name of short option
            char  *lgOpt;       // Name of long option (w/ one "-" before)
            char  *fmt;         // scanf format spec to read the value
            void  *val;         // Pointer to dest. value of the option
        }
            OPTION;

static int   checkOptions (int,const char*const*,int*,const OPTION*,int*);
static int   checkOption  (int,const char*const*,int*,const OPTION*);
static void  info (FILE*);



//==============================================================================
// main
//

int  main (
    const int    argc,
    const char  *argv[]
) {
    static int  n;       // Number of points for the FFT, must be a power of two
    static int  inv;     // Flag: !=0: Generate code for inverse FFT
    static int  realIn;  // Flag: !=0: Optimize for real only input
    static int  realOut; // Flag: !=0: Optimize for real only output
    static int  symmIn;  // Flag: !=0: Optimize for symmetry at input
    static int  symmOut; // Flag: !=0: Optimize for symmetry at output
    static int  verbose; // Level of verbosity
    static int  license; // Flag: !=0: Write a GPL 3 note at the beginning
#define  MAXOPT    50    // Must be greater than maximum length of short or
                         // long option strings (w/o parameter, including \0)
    static const OPTION  pOptions[] = {
        {"n", "-points"      , "%i", &n      },
        {"i", "-inverse"     , NULL, &inv    },
        {"r", "-real-in-opt" , NULL, &realIn },
        {"o", "-real-out-opt", NULL, &realOut},
        {"m", "-symm-in-opt" , NULL, &symmIn },
        {"s", "-symm-out-opt", NULL, &symmOut},
        {"l", "-license"     , NULL, &license},
        {"v", "-verbose"     , NULL, &verbose},
        {NULL, NULL          , NULL, NULL    }
    };

    //==========================================================================
    // Handle command line arguments
    int  i;
    for (i=1; i<argc; i++) {
        // If a non-stand-alone '-'
        if (argv[i][0]=='-' && argv[i][1]) {
            // Advance past '-' to
            //  - either check a long option (pOp->lgOpt contains an additional
            //    '-' as first character),
            //  - or check a short option. The while loop then advances to
            //    further concatenated short options.
            // The while loop terminates
            //  - either in its first cycle if the command line option is a
            //    long option,
            //  - or if a short option with a parameter has been encountered.
            while (*++argv[i]) {
                int  o = 0;
                int  ret = checkOptions (argc,argv,&i,pOptions,&o);
                if (ret == 1) {
                    // It was a param.option, or a long option w/o parameter, so
                    // leave short option loop
                    break;
                } else if (ret == -1) {
                    // Boolean short option (w/o parameter)
                    // If short option is longer than one character:
                    int  j = 0;
                    while (pOptions[o].shOpt[++j]) {
                        ++argv[i];          // Advance to next option in the
                    }                       // command line argument
                } else {
                    // ret==0: Option not found in the option list:
                    // Is it a "standard" option ?
                    if (    ! strcmp(argv[i],"-version")
                         || ! strncmp(argv[i],"V",1)) {
                        printf ("%s %s\n",LOGO,VERSION);
                        exit (EXIT_SUCCESS);
                    } else if (   ! strcmp(argv[i],"h")
                               || ! strcmp(argv[i],"-help")
                               || ! strncmp(argv[i],"?",1)) {
                        info (stdout);                  //  Print info and exit
                    }
                    // Option argument not found in option list
                    // Print error message, info, and terminate program
                    fprintf (stderr, "\n"LOGO": Unknown option -%s\n\n", argv[i]);
                    info (stderr);
                }
            }
        } else {
            // Print error message, info, and terminate program
            fprintf (stderr, "\n"LOGO": Unknown argument %s\n\n", argv[i]);
            info (stderr);
        }
    }

    if (verbose > 0) {
        fprintf (stderr, "Number of points %d\n", n);
        if (inv) {
            fprintf (stderr,"Generating code for inverse FFT\n");
        } else {
            fprintf (stderr,"Generating code for standard (not inverse) FFT\n");
        }
        if (realIn) {
            fprintf (stderr,"Optimize for real only input\n");
        }
        if (realOut) {
            fprintf (stderr,"Optimize for real only output\n");
        }
        if (symmIn) {
            fprintf (stderr,"Optimize for symmetry at input\n");
        }
        if (symmOut) {
            fprintf (stderr,"Optimize for symmetry at output\n");
        }
        if (license) {
            fprintf (stderr,"Include a GPL 3 note into the code\n");
        }
    }
    if (n == 0) {
        fprintf (stderr,"\n"LOGO": No number of points specified.\n");
        info (stderr);
    }
    if (n & (n-1)) {    // Check n is not a power of two
        fprintf (stderr,"\n"LOGO": Number of points %d is not a power of two.\n", n);
        info (stderr);
    }

    if (license)  fputs (licenseText, stdout);
    fputs (header, stdout);

    fftGen (n,inv,realIn, realOut, symmIn, symmOut);

    fputs (footer, stdout);

    return  EXIT_SUCCESS;
}



//==============================================================================
// Code generating function
//

static void  fftGen (
    const int  n,             // Number of points
    const int  inv,           // Flag: !=0: inverse FFT
    const int  realIn,        // Flag: !=0: Optimize for real only input
    const int  realOut,       // Flag: !=0: Optimize for real only output
    const int  symmIn,        // Flag: !=0: Optimize for symmetry at input
    const int  symmOut        // Flag: !=0: Optimize for symmetry at output
) {
#define  LINELEN   200
    static char  line[LINELEN];

    int     nm,mr,nn,m,k,istep,i,ii,jj;
    double  a,wr,wi;

    const double  eps = 0.5*sin(M_PI/(n/2));
    const double  epsOne  =  1.0 - 0.5*(1.0-cos(M_PI/(n/2)));
    const double  epsMOne = -1.0 + 0.5*(1.0-cos(M_PI/(n/2)));

    int  lastKCycle = 0;

    typedef
        struct SwapSt {
            int  m;         // m and mr will have to be swapped
            int  mr;
            int  m_new;     // m value to be used for the source of the
                            // assignment insted of m at symmIn
            int  mr_new;    // mr value to be used for the source of the
                            // assignment insted of mr at symmIn
            int  symmIn;    // Flag: Use the input symmetry relationship for
                            // this element
        } SWAP;
    SWAP  *swap;
    int  nSwap;             // Number of swap commands in array swap[]

    // To keep track of xi[i] being zero at realIn optimization.
    // If xi[i]!=0 then nz[i]==1.
    int  *nzi = (int*)malloc (sizeof(int)*n);
    if (nzi == NULL) {
        fprintf (stderr, "\n"LOGO": Error allocating memory: %s\n", strerror(errno));
        exit (EXIT_FAILURE);
    }

    if ( ! realIn) {
        for (i=0; i<n; ++i)  nzi[i] = 1;
    } else {
        for (i=0; i<n; ++i)  nzi[i] = 0;
    }

    nn = n-1;

    //==========================================================================
    // Implement the binary inversion algorithm

    // 1) Create the array swap[] to store the swapping commands to be conducted
    //    later.
    // 2) Fill it with the swapping commands of the binary inversion algorithm.
    // 3) In case of symmIn create new source indices mr_new according to the
    //    symmetry relationship x[m]=x*[mr_new=n-mr] (x* being the conjugate
    //    complex value) if mr > n/2.
    // 4) Check whether the new source index mr_new would have been overwritten
    //    already before the current swapping.
    //    - In that case insert the current swap command in the list before the
    //      one which would overwrite mr_new.
    // 5) Conduct the swapping commands in the order as now found in the array.

    swap = (SWAP*)malloc (sizeof(SWAP)*n);
    if (swap == NULL) {
        fprintf (stderr, "\n"LOGO": Error allocating memory: %s\n", strerror(errno));
        exit (EXIT_FAILURE);
    }
    for (i=0; i<n; ++i) {
        swap[i].m = 0;
        swap[i].mr = 0;
        swap[i].m_new = 0;
        swap[i].mr_new = 0;
    }

    //--------------------------------------------------------------------------
    // Fill swap[] with the binary inversion swap commands and order the
    // elements such that if the symmetry relationship is to be used the source
    // elements are not overwritten

    mr = 0;
    for (nSwap=0,m=1; m<=nn; ++m) {
        k = n;
        do {
            k /= 2;
        } while (mr+k > nn);
        mr = mr%k + k;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Prepare implementation of "if (mr > m)  SWAP(x[m],x[mr]);"

                    // nSwap is the index of the current swap command in swap[]
        if (mr > m) {
            if ( ! symmIn) {
                swap[nSwap].m = m;             // Swapping according to standard
                swap[nSwap].mr = mr;           //    binary inversion algorithm
                swap[nSwap].symmIn = 0;
            } else {
                // Optimize assuming input symmetry
                swap[nSwap].m = m;
                swap[nSwap].mr = mr;

                if (m<=n/2 && mr<=n/2) {
                    swap[nSwap].symmIn = 0;       // No symmetry subst. required
                } else {
                    int  i_m  = 0;
                    int  i_mr = 0;
                    int  m_new = m;
                    int  mr_new = mr;

                    if (m  > n/2)  m_new  = n - m ; // Use symmetry relationship
                    if (mr > n/2)  mr_new = n - mr; //  "

                    swap[nSwap].m_new = m_new;
                    swap[nSwap].mr_new = mr_new;
                    swap[nSwap].symmIn = 1;

                    // Check whether the array element at the new mr (mr_new)
                    // would have been assigned already

                    if (m > n/2) {
                        // Check whether m_new would have been overwritten by a
                        // writing to element with index m or mr
                        for (i_m=nSwap-1; i_m>0; --i_m) {
                            if (m_new == swap[i_m].m || m_new == swap[i_m].mr)  break;
                        }
                    }
                    if (mr > n/2) {
                        // Check whether mr_new would have been overwritten by a
                        // writing to element with index m or mr
                        for (i_mr=nSwap-1; i_mr>0; --i_mr) {
                            if (mr_new == swap[i_mr].m || mr_new == swap[i_mr].mr)  break;
                        }
                    }

                    // Use the lowest of both and assign it to i_mr
                    if (i_mr>0 && i_m>0) {
                        if (i_mr>i_m)  i_mr = i_m;
                    } else if (i_m>0) {
                        i_mr = i_m;
                    }

                    // If the search ended successfully (i.e. before reaching 0)
                    if (i_mr > 0) {
                        // The array element at mr_new would have been assigned
                        // already.
                        // Create space in array swap[] at index i_mr, firstly
                        // overwriting swap[nSwap]
                        for (ii=nSwap; ii>i_mr; --ii) {
                            swap[ii].m      = swap[ii-1].m ;
                            swap[ii].mr     = swap[ii-1].mr;
                            swap[ii].m_new  = swap[ii-1].m_new;
                            swap[ii].mr_new = swap[ii-1].mr_new;
                            swap[ii].symmIn = swap[ii-1].symmIn;
                        }
                        // Insert the new swap command at index ii, i.e.
                        // before the command that would overwrite mr
                        swap[i_mr].m = m;
                        swap[i_mr].mr = mr;
                        swap[i_mr].m_new = m_new;
                        swap[i_mr].mr_new = mr_new;
                        swap[i_mr].symmIn = 1;
                    }
                }
            }
            ++nSwap;
        }
    }
    if (symmIn) {
        // When optimizing assuming input symmetry then
        //   check whether there are indices not yet considered.
        // Therefore, check swap[] for not listed indices from n/2+1 onwards.
        // Consider the symmetry relationship for those now.
        // Note: This must be done before conducting the binary inversion
        // algorithm because that will overwrite the elements with according
        // indices.
        for (i=n/2+1; i<n; ++i) {
            for (ii=nSwap-1; ii>=0; --ii) {
                if (swap[ii].m==i || swap[ii].mr==i)  break;
            }
            if (ii < 0) {   // i not listed in swap[]
                printf (INDENT"xr[%d] =  xr[%d];\n", i, n - i);
                printf (INDENT"xi[%d] = -xi[%d];\n", i, n - i);
            }
        }
    }

    //--------------------------------------------------------------------------
    // Conduct swapping

    for (k=0; k<nSwap; ++k) {
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Implement  "if (mr > m)  SWAP(x[m],x[mr]);"
        if ( ! swap[k].symmIn) {
            // Swapping according to standard binary inversion algorithm
            printf (INDENT"tr = xr[%d];\n", swap[k].m);
            printf (INDENT"xr[%d] = xr[%d];\n", swap[k].m, swap[k].mr);
            printf (INDENT"xr[%d] = tr;\n", swap[k].mr);
            if ( ! realIn) {
                printf (INDENT"ti = xi[%d];\n", swap[k].m);
                printf (INDENT"xi[%d] = xi[%d];\n", swap[k].m, swap[k].mr);
                printf (INDENT"xi[%d] = ti;\n", swap[k].mr);
            }
        } else {
            // Use the conjugate complex value of (xr[n-mr],xi[n-mr]) but only
            // if the source index of the assignment would have been >n/2
            printf (INDENT"xr[%d] = xr[%d];\n", swap[k].mr, swap[k].m_new);
            printf (INDENT"xr[%d] = xr[%d];\n", swap[k].m, swap[k].mr_new);
            if ( ! realIn) {
                if (swap[k].m <= n/2) {
                    printf (INDENT"xi[%d] = xi[%d];\n", swap[k].mr, swap[k].m_new);
                } else {
                    // Negating xi for the conjugate complex value is required
                    printf (INDENT"xi[%d] = -xi[%d];\n", swap[k].mr, swap[k].m_new);
                }
                if (swap[k].mr <= n/2) {
                    printf (INDENT"xi[%d] = xi[%d];\n", swap[k].m, swap[k].mr_new);
                } else {
                    // Negating xi for the conjugate complex value is required
                    printf (INDENT"xi[%d] = -xi[%d];\n", swap[k].m, swap[k].mr_new);
                }
            }
        }
    }
    putchar ('\n');

    //==========================================================================
    // Do the transform

    for (k=1; k<n; k=istep) {
        istep = 2*k;
        if (istep==n)  lastKCycle = 1;

        for (m=0; m<k; ++m) {
            a  = M_PI*(-m)/k;
            wr = cos (a);
            wi = sin (a);
            if (inv)  wi = -wi;     // Prepare inverse FFT
            ii = m;
            nm = (nn-m)/istep + m;
            for (i=m; i<=nm; ++i) {
                size_t  len;
                               // Flag: 1st summand of
                               //   tr = wr*xr[jj] - wi*xi[jj];
                               // or
                               //   ti = wr*xi[jj] + wi*xr[jj];
                               // is zero
                int  firstOpZero;
                int  trz = 0;  // Flag: Expression tr=wr*xr[jj]-wi*xi[jj] == 0
                int  tiz = 0;  // Flag: Expression ti=wr*xi[jj]+wi*xr[jj] == 0

                jj = ii+k;

#ifndef OPTIMIZE_SINE_COSINE_VALUES
                printf (INDENT"tr = "NUMBER_FORMAT"*xr[%d] - "NUMBER_FORMAT"*xi[%d];\n", wr, jj, wi, jj);
                printf (INDENT"ti = "NUMBER_FORMAT"*xi[%d] + "NUMBER_FORMAT"*xr[%d];\n", wr, jj, wi, jj);
#else
                //--------------------------------------------------------------
                // Implement tr = wr*xr[jj] - wi*xi[jj];

                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Part wr*xr[jj]
                firstOpZero = 0;

                snprintf (line, LINELEN, INDENT"tr =");
                len = strlen (line);

                if (fabs(wr) > eps) {
                    // wr != 0
                    if (wr < epsOne) {
                        // wr != 1
                        if (wr > epsMOne) {
                            // wr != -1
                            snprintf (line+len,LINELEN-len," "NUMBER_FORMAT"*xr[%d]", wr, jj);
                        } else {
                            // wr == -1
                            snprintf (line+len,LINELEN-len," -xr[%d]", jj);
                        }
                    } else {
                        // wr == 1
                        snprintf (line+len,LINELEN-len," xr[%d]", jj);
                    }
                } else {
                    firstOpZero = 1;
                }
                len = strlen (line);

                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Part -wi*xi[jj]

                trz = 0;
                if (fabs(wi) > eps  &&  nzi[jj]) {
                    // wi != 0  and  xi[jj] non-zero
                    if (wi < epsOne) {
                        // wi != 1
                        if (wi > epsMOne) {
                            // wi != -1
                            if ( ! firstOpZero) {       // If wr*xr[jj] != 0
                                if (wi >= 0.0) {
                                    snprintf (line+len,LINELEN-len," - "NUMBER_FORMAT"*xi[%d]", wi, jj);
                                } else {
                                    snprintf (line+len,LINELEN-len," + "NUMBER_FORMAT"*xi[%d]", -wi, jj);
                                }
                            } else {
                                snprintf (line+len,LINELEN-len," "NUMBER_FORMAT"*xi[%d]", -wi, jj);
                            }
                        } else {
                            // wi == -1
                            if ( ! firstOpZero) {
                                snprintf (line+len,LINELEN-len," + xi[%d]", jj);
                            } else {
                                snprintf (line+len,LINELEN-len," xi[%d]", jj);
                            }
                        }
                    } else {
                        // wi == 1
                        snprintf (line+len,LINELEN-len," - xi[%d]", jj);
                    }
                    fputs (line, stdout);
                    fputs (";\n", stdout);
                } else {
                    // wr == 0  or  xi[jj] == 0
                    if ( ! firstOpZero) {
                        fputs (line, stdout);
                        fputs (";\n", stdout);
                    } else {
                        trz = 1;    // tr = wr*xr[jj]-wi*xi[jj] == 0
                        // Expression for tr is zero, so don't write anything
                    }
                }

                //--------------------------------------------------------------
                // Implement ti = wr*xi[jj] + wi*xr[jj];

                if ( ! (realOut && lastKCycle)) {

                    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                    // Part wr*xi[jj]

                    firstOpZero = 0;
                    snprintf (line, LINELEN, INDENT"ti =");
                    len = strlen (line);

                    if (fabs(wr) > eps  &&  nzi[jj]) {
                        // wr != 0  and  xi[jj] non-zero
                        if (wr < epsOne) {
                            // wr != 1
                            if (wr > epsMOne) {
                                // wr != -1
                                snprintf (line+len,LINELEN-len," "NUMBER_FORMAT"*xi[%d]", wr, jj);
                            } else {
                                // wr == -1
                                snprintf (line+len,LINELEN-len," -xi[%d]", jj);
                            }
                        } else {
                            // wr == 1
                            snprintf (line+len,LINELEN-len," xi[%d]", jj);
                        }
                    } else {
                        firstOpZero = 1;
                    }
                    len = strlen (line);

                    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                    // Part +wi*xr[jj]

                    tiz = 0;
                    if (fabs(wi) > eps) {
                        // wi != 0
                        if (wi < epsOne) {
                            // wi != 1
                            if (wi > epsMOne) {
                                // wi != -1
                                if ( ! firstOpZero) {       // If wr*xr[jj] != 0
                                    if (wi >= 0.0) {
                                        snprintf (line+len,LINELEN-len," + "NUMBER_FORMAT"*xr[%d]", wi, jj);
                                    } else {
                                        snprintf (line+len,LINELEN-len," - "NUMBER_FORMAT"*xr[%d]", -wi, jj);
                                    }
                                } else {
                                    snprintf (line+len,LINELEN-len," "NUMBER_FORMAT"*xr[%d]", wi, jj);
                                }
                            } else {
                                // wi == -1
                                snprintf (line+len,LINELEN-len," - xr[%d]", jj);
                            }
                        } else {
                            // wi == 1
                            snprintf (line+len,LINELEN-len," xr[%d]", jj);
                        }
                        fputs (line, stdout);
                        fputs (";\n", stdout);
                    } else {
                        // wi == 0
                        if ( ! firstOpZero) {      // If wr*xi[jj] != 0
                            fputs (line, stdout);
                            fputs (";\n", stdout);
                        } else {
                            tiz = 1;    // ti = wr*xi[jj]+wi*xr[jj] == 0
                            // Expression for tr is zero, so don't write anything
                        }
                    }
                }
#endif
                if ( ! (symmOut && lastKCycle && jj!=n/2)) {
                    //----------------------------------------------------------
                    // Implement xr[jj] = xr[ii] - tr;

                    if ( ! trz) {
                        printf (INDENT"xr[%d] = xr[%d] - tr;\n", jj, ii);
                    } else {
                        printf (INDENT"xr[%d] = xr[%d];\n", jj, ii);
                    }

                    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                    // Implement xi[jj] = xi[ii] - ti;

                    if ( ! (realOut && lastKCycle)) {
                        if ( ! tiz) {
                            if (nzi[ii]) {
                                printf (INDENT"xi[%d] = xi[%d] - ti;\n", jj, ii);
                            } else {
                                printf (INDENT"xi[%d] = - ti;\n", jj);
                            }
                            nzi[jj] = 1;
                        } else {
                            if (nzi[ii]) {
                                printf (INDENT"xi[%d] = xi[%d];\n", jj, ii);
                                nzi[jj] = 1;
                            } else if (realIn && lastKCycle) {
                                // In case of realIn this element has not yet
                                // been touched. So it must be set zero here
                                // because imaginary input values at realIn
                                // could be arbitrary but should contain valid
                                // values at output
                                printf (INDENT"xi[%d] = 0.0;\n", jj);
                            }
                        }
                    }
                }

                //--------------------------------------------------------------
                // Implement xr[ii] += tr;

                if ( ! trz) {
                    printf (INDENT"xr[%d] += tr;\n", ii);
                }

                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Implement xi[ii] += ti;

                if ( ! (realOut && lastKCycle)) {
                    if ( ! tiz) {
                        if (nzi[ii]) {
                            printf (INDENT"xi[%d] += ti;\n", ii);
                        } else {
                            printf (INDENT"xi[%d] = ti;\n", ii);
                            nzi[ii] = 1;
                        }
                    } else if (realIn && lastKCycle) {
                        // In case of realIn this element has not yet been
                        // touched. So it must be set zero here because
                        // imaginary input values at realIn could be arbitrary
                        // but should contain valid values at output
                        printf (INDENT"xi[%d] = 0.0;\n", ii);
                    }
                }

                ii += istep;
            }
        }
    }

    free (swap);
    free (nzi);
}



//==============================================================================
// checkOptions  V1.2
//
// Checks whether the command line option passed via argv[*pi] can be found in
// the option array passed as *pOp.
// The last entry of the option array must consist of zeroes to mark the end of
// the array.
//
// If the option passed as argv[*pi] is found in the array the according array
// index is returned in *po.
// In that case the function returns 1 as return value if the option was a long
// option with or without a parameter or -1 if the option was a short option
// without a parameter.
//
// If the passed option could not be fond in the option array then the function
// returns zero as return value.
//

static int  checkOptions (
    const int           argc,    // No. of command line entries (from main())
    const char *const   argv[],  // Command line entries  (from main())
          int  *const   pi,      // Ptr to current command line entry index
    const OPTION       *pOp,     // Ptr to option array
          int  *const   po       // Index into option array
) {
    int  ret = 0;

    for (*po=0; pOp->shOpt; ++pOp,++*po) {
        ret = checkOption (argc,argv,pi,pOp);
        if (ret != 0)  return ret;
    }
    return ret;
}



//==============================================================================
// checkOption  V1.2
//
// Routine checks whether option passed via argv[*pi] fits to the option in
// option list pointed to by pOp.
//
// If the currently checked option *pOp fits to the currently examined command
// line argument argv[*pi] and if it is an option with a parameter store the
// value in the option structure and return 1.
//
// If the currently checked option *pOp fits to the currently examined command
// line argument argv[*pi] and if it is an option without a parameter increment
// the option value in the option structure by one and return 0.
//
// If the currently checked option *pOp doesn't fit to the currently examined
// command line argument argv[*pi] return 0.

static int  checkOption (
    const int           argc,    // No. of command line entries (from main())
    const char   *const argv[],  // Command line entries  (from main())
          int    *const pi,      // Ptr to current command line entry index
    const OPTION *const pOp      // Ptr to current option from option list
) {
    if (pOp->fmt && pOp->fmt[1] == 's') {
        // Test whether one of the different possibilities for this option
        // fits to the just read command line option.
        // Check at first whether the first characters are equal to the short
        // or long option name.

        int  len;

        if (    ( len=strlen(pOp->shOpt), ! strncmp(argv[*pi],pOp->shOpt,len) )
             || ( len=strlen(pOp->lgOpt), ! strncmp(argv[*pi],pOp->lgOpt,len) )
           ) {
            // It is this short or long option: Now, is it of kind
            // "-Opt=XY" or "-OptXY" or "-Opt XY" (in that order)

            switch (argv[*pi][len]) {
                case '\0': if (++*pi==argc) {
                               fprintf (stderr, "\n"LOGO": Missing option argument\n");
                               info (stderr);
                           }
                           *(const char**)pOp->val = argv[*pi];
                           break;

                case '=' : ++len;
                           *(const char**)pOp->val = argv[*pi] + len;
                           break;

                default  : *(const char**)pOp->val = argv[*pi] + len;
                           break;
            }
            return 1;           // Signal: option found and parameter read
        }
    } else {
        char  fmtStr[MAXOPT+5];

        // Test whether one of the different possibilities for this option
        // fits to the just read command line option.
        // If the short option were "shOpt" and the long option were "loOpt"
        // and the option value were 10 then the possibilities checked here
        // would be "-shOpt10", "--loOpt10", "-shOpt=10" and "--loOpt=10".

        if (    pOp->fmt
             && (   // Assemble sscanf format string to e.g. "shopt=%f" etc.
                    sscanf( argv[*pi], strcat(strcat(strncpy(fmtStr,pOp->shOpt,MAXOPT),"="),pOp->fmt), pOp->val )==1
                 || sscanf( argv[*pi], strcat(       strncpy(fmtStr,pOp->shOpt,MAXOPT)     ,pOp->fmt), pOp->val )==1
                 || sscanf( argv[*pi], strcat(strcat(strncpy(fmtStr,pOp->lgOpt,MAXOPT),"="),pOp->fmt), pOp->val )==1
                 || sscanf( argv[*pi], strcat(       strncpy(fmtStr,pOp->lgOpt,MAXOPT)     ,pOp->fmt), pOp->val )==1
                )
           ) {

            return 1;           // Signal: option found and parameter read

        }
        // Check short option without parameter
        else if ( ! strncmp(argv[*pi],pOp->shOpt,strlen(pOp->shOpt)) && ! pOp->fmt) {
            // Command line argument is a short option without parameter

            ++*(int*)pOp->val;  // Set TRUE, or count up if used more than once

            return -1;          // Signal: boolean short option found

        }
        // Check short option with parameter or
        // long option with or without parameter
        else if (    ! strcmp(argv[*pi],pOp->shOpt)
                  || ! strcmp(argv[*pi],pOp->lgOpt)) {

            // Check whether the option is without parameter
            if ( ! pOp->fmt) {
                // Command line argument is a long option without parameter
                // (short option has already checked above)
                ++*(int*)pOp->val;  // Set TRUE, or count up if used more than once

                return 1;           // Signal: boolean long option found

            // We found a "-shOpt" or "--loOpt". Now we look for the number:
            } else if (++*pi==argc || sscanf(argv[*pi],pOp->fmt,pOp->val)!=1) {
                fprintf (stderr, "\n"LOGO": Invalid option argument %s\n\n", argv[*pi]);
                info (stderr);      // No number with the correct format found !
            } else {
                return 1;           // Signal: option found and parameter read
            }
        }
    }

    // The current test option was not the one recently read from command line
    return 0;
}



//==============================================================================
// info
//
// Print informative usage message to stream passed as argument
//

static void  info (
    FILE *const  file
) {
    if (file != stderr) {
        fputs (LOGO": Generate code to compute an FFT or IFFT\n"
               "Version: "VERSION"\n\n", file);
    }
    fputs (
        "Usage: fftGen [option...]\n"
        "Options:\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        " -n, --points NUMBER   Number of points, must be a power of 2.\n"
        " -i, --inverse         Generate code for inverse FFT.\n"
        " -r, --real-in-opt     Optimize for real only input.\n"
        " -o, --real-out-opt    Optimize for real only output.\n"
        " -m, --symm-in-opt     Optimize for symmetry at input sequence.\n"
        " -s, --symm-out-opt    Optimize for symmetry at output sequence.\n"
        " -l, --license         Write a GPL 3 note at the beginning of the code.\n"
        " -v, --verbose         Increase verbosity level.\n"
        "                       Verbose output is directed to stderr.\n"
        " -V, --version         Print version and exit.\n"
        " -h, --help            Print this info.\n"
        "Note that it is required to specify the number of data points by option -n\n"
        "or --points.\n"
        "Result is written to stdout\n", file);

    if (file == stderr)  exit (EXIT_FAILURE);
    else                 exit (EXIT_SUCCESS);
}
