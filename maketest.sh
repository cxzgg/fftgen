#!/bin/bash
#
# Test script to test fftGen
#
# Version 1   2021 Jun 21st   jost.brachert@gmx.de
#
#-------------------------------------------------------------------------------
# Copyright (C) 2021  Jost Brachert, jost.brachert@gmx.de
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the license, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program, see file COPYING. If not, see https://www.gnu.org/licenses/.
#

project="fftGen"

CFLAGS="-Wall"
CFLAGS="-lm"
source="../$project.c"

testscrdir="scripts"

sep="\n====\n"

cd test || exit

rm -v $project.gcda

#-------------------------------------------------------------------------------
# Create instrumented code
# => test/$project.gcno, test/$project
gcc -fprofile-arcs -ftest-coverage $CFLAGS $LDFLAGS -o $project $source

#-------------------------------------------------------------------------------
# Run tests
# => test/$project.gcda

date | tee stdout.log stderr.log

echo -e "${sep}Test help info output by short option"|\
    tee -a stderr.log >>stdout.log
./$project -h >>stdout.log 2>>stderr.log

echo -e "${sep}Test help info output by long option"|\
    tee -a stderr.log >>stdout.log
./$project --help >>stdout.log 2>>stderr.log

echo -e "${sep}Test version output by short option"|\
    tee -a stderr.log >>stdout.log
./$project -V >>stdout.log 2>>stderr.log

echo -e "${sep}Test version output by long option"|\
    tee -a stderr.log >>stdout.log
./$project --version >>stdout.log 2>>stderr.log

echo -e "${sep}Test no argument"|\
    tee -a stderr.log >>stdout.log
echo -e "    Expecting error message" >>stderr.log
./$project >>stdout.log 2>>stderr.log

echo -e "${sep}Test invalid option"|\
    tee -a stderr.log >>stdout.log
echo -e "    Expecting error message" >>stderr.log
./$project -y >>stdout.log 2>>stderr.log

echo -e "${sep}Test invalid argument"|\
    tee -a stderr.log >>stdout.log
echo -e "    Expecting error message" >>stderr.log
./$project y >>stdout.log 2>>stderr.log

echo -e "${sep}Test verbosity\nTest option assignment by =
Test option argument to -n is not power of 2"|\
    tee -a stderr.log >>stdout.log
echo -e "    Expecting error message" >>stderr.log
./$project -vn=7 >>stdout.log 2>>stderr.log

echo -e "${sep}Test invalid option argument"|\
    tee -a stderr.log >>stdout.log
echo -e "    Expecting error message" >>stderr.log
./$project -vn x >>stdout.log 2>>stderr.log

echo -e "${sep}Test 2-point FFT\nTest short option license output"|\
    tee -a stderr.log >>stdout.log
./$project -ln2 2>>stderr.log | tee fft.c >>stdout.log
./$project -in2 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=1 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 4-point FFT\nTest long option license output
Test verbosity regarding license"|\
    tee -a stderr.log >>stdout.log
./$project -vln4 2>>stderr.log | tee fft.c >>stdout.log
./$project -in4 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=2 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 8-point FFT\nTest no space before option argument"|\
    tee -a stderr.log >>stdout.log
./$project -n8  > fft.c  2>>stderr.log
./$project -in8 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=3 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 16-point FFT\nTest space before option argument
Test -n and -i long option"|\
    tee -a stderr.log >>stdout.log
./$project -n 16  > fft.c  2>>stderr.log
./$project --inverse --points 16 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=4 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 32-point FFT\nTest = before option argument
Test separate short options"|\
    tee -a stderr.log >>stdout.log
./$project -n=32 > fft.c  2>>stderr.log
./$project -i -n=32 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=5 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 32-point FFT\nTest option -r"|\
    tee -a stderr.log >>stdout.log
./$project -rn32 > fft.c  2>>stderr.log
./$project -in32 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=5 -DREAL_IN_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 64-point FFT\nTest option -s"|\
    tee -a stderr.log >>stdout.log
./$project -sn64 > fft.c  2>>stderr.log
./$project -in64 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=6 -DSYMM_OUT_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 64-point FFT\nTest options -r and -s
Test -r and -s long option\nTest verbosity long option
Test verbosity regarding -r and -s"|\
    tee -a stderr.log >>stdout.log
./$project --verbose --real-in-opt --symm-out-opt -n64 > fft.c  2>>stderr.log
./$project -in64 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=6 -DREAL_IN_OPTIMIZED -DSYMM_OUT_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 128-point FFT\nTest option -m"|\
    tee -a stderr.log >>stdout.log
./$project -n128 > fft.c  2>>stderr.log
./$project -imn128 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=7 -DSYMM_IN_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 256-point FFT\nTest option -o"|\
    tee -a stderr.log >>stdout.log
./$project -n256 > fft.c  2>>stderr.log
./$project -ion256 > ffti.c 2>>stderr.log
gcc $CFLAGS -DM=8 -DREAL_OUT_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 512-point FFT\nTest options -m and -o
Test -m and -o long option\nTest verbosity regarding -m and -o"|\
    tee -a stderr.log >>stdout.log
./$project -n512 > fft.c  2>>stderr.log
./$project -vi --symm-in-opt --real-out-opt -n512 > ffti.c 2>>stderr.log
gcc $CFLAGS -DEPS=1.e-7 -DM=9\
 -DSYMM_IN_OPTIMIZED -DREAL_OUT_OPTIMIZED -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test 1024-point FFT\nTest options -r, -s, -m, -o\n"|\
    tee -a stderr.log >>stdout.log
./$project -rsn1024 > fft.c  2>>stderr.log
./$project -imon1024 > ffti.c 2>>stderr.log
gcc $CFLAGS -DEPS=1.e-7 -DM=10\
 -DREAL_IN_OPTIMIZED -DSYMM_OUT_OPTIMIZED -DSYMM_IN_OPTIMIZED -DREAL_OUT_OPTIMIZED\
 -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "${sep}Test usability for type float\n"|\
    tee -a stderr.log >>stdout.log
./$project -n32 > fft.c  2>>stderr.log
./$project -in32 > ffti.c 2>>stderr.log
gcc $CFLAGS -DFFT_TYPE=float -DEPS=1.e-5 -DM=5\
 -DNON_ZERO_IMAG_INPUT -o fftTest fftTest.c 2>>stderr.log
if ! fftTest 2>>stderr.log ; then
    echo -e "\nTest failed, see stderr.log\n"
fi

echo -e "test script finished\n"

#-------------------------------------------------------------------------------
# Create coverage report in ASCII format
# => test/fftGen.c.gcov
gcov -o $project.gcno ../$project.c

# Create coverage report in HTML format
# => test/fftGen.cov.html
$testscrdir/gcov2html.pl $project.c.gcov > $project.cov.html

# Verify the test results
if ! diff -b -I "\([0-9][0-9]:\)\{2\}[0-9][0-9].*[0-9]\{4\}" stdout.log refs/stdout.log ; then
    echo -e "test failed: stdout.log differs from reference"
    exit 1
fi
if ! diff -b -I "\([0-9][0-9]:\)\{2\}[0-9][0-9].*[0-9]\{4\}" stderr.log refs/stderr.log ; then
    echo -e "test failed: stderr.log differs from reference"
    exit 1
fi

echo -e "test passed"
