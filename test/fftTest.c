//##############################################################################
// File: fftTest.c
//
// Program to test the result of fftGen
//
// The program is very flexibly configurable using define options ("-D...") on
// the compiler command line.
//
// Version 1     2021 Jun 21st   jost.brachert@gmx.de
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

#include  <stdio.h>
#include   <math.h>     // sin(),cos(),sqrt(),fabs(),atan()
#include <stdlib.h>     // rand(),RAND_MAX

#define  LOGO   "fftTest"

// Commented out defines come from compiler command line

//#define  M   5
#define  N  (1<<M)

//#define  REAL_IN_OPTIMIZED
//#define  REAL_OUT_OPTIMIZED
//#define  SYMM_IN_OPTIMIZED
//#define  SYMM_OUT_OPTIMIZED
//#define  TEST_OUTPUT
//#define  NON_ZERO_IMAG_INPUT
#ifndef FFT_TYPE
#define  FFT_TYPE       double
#endif

#ifndef EPS
#define  EPS   1.e-8                // Tolerance for comparison to reference
#endif

typedef  struct Complex {double  r;
                         double  i;}  COMPLEX;

FFT_TYPE  xr[N];
FFT_TYPE  xi[N];
COMPLEX  xRef[N];
COMPLEX  xOri[N];


void  fftRef (COMPLEX*, int);
void  fft (FFT_TYPE*,FFT_TYPE*);
void  ffti (FFT_TYPE*,FFT_TYPE*);
void  conv (COMPLEX, double*, double*);



int  main ()
{
    int  failed = 0;                // Flag: !=0: Test failed
    int  i;

    //==========================================================================
    // Create input vector

    double  ts = 1./N;              // Sampling time [s]

    for (i=0; i<N; ++i) {
               // Ampl                         Phase
        xr[i] =   0.1*cos ((2.*M_PI)*1.*i*ts + 3.0)     // f=1/N Hz
                + 0.2*cos ((2.*M_PI)*2.*i*ts + 2.0)     // f=2/N Hz
                + 0.3*cos ((2.*M_PI)*3.*i*ts + 1.0)     // f=3/N Hz
#if M > 3
                + 0.4*cos ((2.*M_PI)*4.*i*ts + 0.0)     // f=4/N Hz
                + 0.5*cos ((2.*M_PI)*5.*i*ts - 1.0)     // f=5/N Hz
                + 0.6*cos ((2.*M_PI)*6.*i*ts - 2.0)     // f=6/N Hz
                + 0.7*cos ((2.*M_PI)*7.*i*ts - 3.0)     // f=7/N Hz
#endif
#if M > 4
                + 0.15*cos ((2.*M_PI)* 8.*i*ts + 2.5 )  // f=8/N Hz
                + 0.25*cos ((2.*M_PI)* 9.*i*ts + 1.5 )  // f=9/N Hz
                + 0.35*cos ((2.*M_PI)*10.*i*ts + 0.5 )  // f=10/N Hz
                + 0.45*cos ((2.*M_PI)*11.*i*ts + 0.25)  // f=11/N Hz
                + 0.55*cos ((2.*M_PI)*12.*i*ts - 0.25)  // f=12/N Hz
                + 0.65*cos ((2.*M_PI)*13.*i*ts - 0.5 )  // f=13/N Hz
                + 0.75*cos ((2.*M_PI)*14.*i*ts - 1.5 )  // f=14/N Hz
                + 0.85*cos ((2.*M_PI)*15.*i*ts - 2.5 )  // f=15/N Hz
#endif
                + 0.49;
        // Prevent from point N/2 being zero (here in the loop for readability
        // and to make the code at the end of this loop simpler)
        // NOTE that this will spoil the equality between magnitude/phase of
        // input harmonics and r/phi at test output.
        xr[N-1] = 0.13;
                                            //
#if M > 5
        {                                   // Create arbitrary test data for
            int  j;                         //   higher order FFTs
            for (j=(1<<4); j<N/2; ++j) {
                xr[i] += (double)rand()/((double)N*RAND_MAX)
                        * cos ((2.*M_PI)*j*i*ts + (double)rand()/RAND_MAX);
            }
        }
#endif

#ifndef NON_ZERO_IMAG_INPUT
        xi[i] = 0.;
#else
        xi[i] = i*2.0/N;
#endif
        xOri[i].r = xRef[i].r = xr[i];
        xOri[i].i = xRef[i].i = xi[i];
    }

    fftRef (xRef,N);

    //==========================================================================
    fputs (LOGO": Standard FFT Test\n", stderr);

#ifdef REAL_IN_OPTIMIZED
    for (i=0; i<N; ++i) {               // Create arbitrary input data, should
        xi[i] = rand();                 //   be ignored in the test object
    }
#endif

    fft (xr,xi);

#ifdef SYMM_OUT_OPTIMIZED
    for (i=N/2+1; i<N; ++i) {           // Reconstruct the omitted values
        xr[i] =  xr[N-i];               //   for the IFFT later
        xi[i] = -xi[N-i];
    }
#endif

    //--------------------------------------------------------------------------
    // Compare result
    for (i=0; i<N; ++i) {
        if (fabs(xr[i]-xRef[i].r) > EPS) {
            fprintf (stderr,
                     LOGO": at idx %d real: res: %13.6e <> ref: %13.6e\n",
                     i, xr[i], xRef[i].r);
            failed = 1;
        }
        if (fabs(xi[i]-xRef[i].i) > EPS) {
            fprintf (stderr,
                     LOGO": at idx %d imag: res: %13.6e <> ref: %13.6e\n",
                     i, xi[i], xRef[i].i);
            failed = 1;
        }
    }
#ifdef TEST_OUTPUT
    //--------------------------------------------------------------------------
    // Write test output to stdout
    puts ("Test Result                                  |Reference Result");
    for (i=0; i<N; ++i) {
        COMPLEX  x = {xr[i], xi[i]};
        double  r;
        double  phi;

        conv (x, &r, &phi);
        printf ("%4.1f: re%7.3f im%7.3f r%7.4f phi%7.4f",
                  i/(ts*N), xr[i], xi[i], r, phi);

        conv (xRef[i], &r, &phi);
        printf ("|%7.3f%7.3f %7.4f %7.4f\n", xRef[i].r, xRef[i].i, r, phi);
    }
    putchar ('\n');
#endif

    //==========================================================================
    fputs (LOGO": Inverse FFT Test\n", stderr);

#ifdef SYMM_IN_OPTIMIZED
    for (i=N/2+1; i<N; ++i) {           // Create arbitrary input data, should
        xr[i] = rand();                 //   be ignored in the test object
        xi[i] = rand();
    }
#endif

    ffti (xr,xi);

    for (i=0; i<N; ++i) {
        xr[i] *= 1./N;
        xi[i] *= 1./N;      // This can be omitted, if original sequence didn't
    }                       // contain imaginary values different from zero

    //--------------------------------------------------------------------------
    // Compare result
    for (i=0; i<N; ++i) {
        if (fabs(xr[i]-xOri[i].r) > EPS) {
            fprintf (stderr,
                     LOGO": at idx %d real: res: %13.6e <> ref: %13.6e\n",
                     i, xr[i], xOri[i].r);
            failed = 1;
        }
#ifndef REAL_OUT_OPTIMIZED
        if (fabs(xi[i]-xOri[i].i) > EPS) {
            fprintf (stderr,
                     LOGO": at idx %d imag: res: %13.6e <> ref: %13.6e\n",
                     i, xi[i], xOri[i].i);
            failed = 1;
        }
#endif
    }
#ifdef TEST_OUTPUT
    //--------------------------------------------------------------------------
    // Write test output to stdout
    puts ("Inverse FFT");
    puts ("Test Result                                    |Original Input Sequence");
    for (i=0; i<N; ++i) {
        COMPLEX  x = {xr[i], xi[i]};
        double  r;
        double  phi;

        conv (x, &r, &phi);
        printf ("%4.1f: re%8.4f im%7.4f r%7.4f phi%7.4f | reOri%8.4f imOri%7.4f\n",
                  i/(ts*N), xr[i], xi[i], r*(N/2), phi, xOri[i].r, xOri[i].i);
    }
#endif

    if (failed)  return 1;
    return 0;
}



//==============================================================================
// Reference FFT Algorithm
//

#define  SWAP(a,b)     do {COMPLEX t=(a);(a)=(b);(b)=t;} while (0)


void  fftRef (
    COMPLEX  x[],
    int      n
) {
    int     nm,mr,nn,m,k,istep,i,ii,jj;
    double  tr,ti,a,wr,wi;

    mr = 0;
    nn = n-1;

    for (m=1; m<=nn; ++m) {
        k = n;
        do {
            k /= 2;
        } while (mr+k > nn);
        mr = mr%k + k;
        if (mr > m)  SWAP(x[m],x[mr]);
    }
    for (k=1; k<n; k=istep) {
        istep = 2*k;
        for (m=0; m<k; ++m) {
            a  = M_PI*(-m)/k;
            wr = cos (a);
            wi = sin (a);
            ii = m;
            nm = (nn-m)/istep + m;
            for (i=m; i<=nm; ++i) {
                jj  = ii+k;
                tr  = wr*x[jj].r-wi*x[jj].i;
                ti  = wr*x[jj].i+wi*x[jj].r;
                x[jj].r = x[ii].r-tr;
                x[jj].i = x[ii].i-ti;
                x[ii].r += tr;
                x[ii].i += ti;
                ii += istep;
            }
        }
    }
}



//==============================================================================
// FFT Test Object
//

void  fft (
    FFT_TYPE  *xr,
    FFT_TYPE  *xi
) {
    FFT_TYPE  tr, ti;
#include "fft.c"
}



//==============================================================================
// IFFT Test Object
//

void  ffti (
    FFT_TYPE  *xr,
    FFT_TYPE  *xi
) {
    FFT_TYPE  tr, ti;
#include "ffti.c"
}



//==============================================================================
// Convert from cartesian coordinates to polar coordinates
//

void  conv (
    COMPLEX   x,
    double   *r,
    double   *phi
) {
    *r = sqrt (x.r*x.r+x.i*x.i)*(2./N);
    if (fabs(x.i) < fabs(x.r)) {
        if (x.r==0.) {
            *phi = 0.;
        } else {
            *phi = atan (x.i/x.r);
            if (x.r < 0.)  *phi += x.i>0 ? M_PI : -M_PI;
        }
    } else {
        if (x.i==0.) {
            *phi = 0.;
        } else {
            *phi = (x.i>=0. ? M_PI/2. : -M_PI/2.) - atan (x.r/x.i);
        }
    }
}
