#!/bin/bash
#
# Shell script to compile fftgen.c
#
# Version 1   2021 Jun 21st   jost.brachert@gmx.de
#

# Change the option arguments to -DNUMBER_FORMAT and -DINDENT to your
# preferences

gcc -DNUMBER_FORMAT='"%21.14e"' -DINDENT='""' -Wall -Wextra -Wpedantic -Werror -Wundef -Wuninitialized -lm fftGen.c -o fftGen

