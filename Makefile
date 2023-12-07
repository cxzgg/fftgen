################################################################################
# Makefile for project fftGen
#
# Copyright (C) 2015-2023  Jost Brachert, jost.brachert@gmx.de
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

project = fftGen

version = 1

FOP = fop
MKDIR = mkdir -p


all: $(project) html


################################################################################
# Executable

CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wundef -Wuninitialized
LDFLAGS = -lm

$(project): $(project).c
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $<


################################################################################
# Documentation

HtmlOut = doc/html

.PHONY: html

html: $(HtmlOut)/index.html

$(HtmlOut)/index.html: $(project).c doxygen/doxy.css doxygen/manual.cfg
	$(MKDIR) $(HtmlOut)
	doxygen doxygen/manual.cfg > doxygen.log
	$(doxyclean)
	cp doxygen/doxy.css $(HtmlOut)
#	Insert style to highlight code bold
#	Remove reference to tabs.css, remove javascript, change doxygen.css to doxy.css
#	Note: Don't use $@
	perl -wpi~ -e "s|</head>|<style type='text/css'><!--code{font-weight:bold}--></style></head>|"  $(HtmlOut)/index.html
	perl -wpi~ -e "s|^.*tabs.css.*$$||;s|^<script.*</script>$$||;s|doxygen\.css|doxy.css|;"  $(HtmlOut)/index.html

define doxyclean
  -rm -f $(HtmlOut)/*.png $(HtmlOut)/*.gif $(HtmlOut)/doxygen.svg $(HtmlOut)/*.js
  -rm -f $(HtmlOut)/doxygen.css $(HtmlOut)/tabs.css
endef


#-------------------------------------------------------------------------------
# Test targets

.PHONY: check

# Create instrumented object file and executable
test/$(project).gcno: $(project).c
	cd test; gcc -fprofile-arcs -ftest-coverage $(CFLAGS) $(LDFLAGS) \
	-o $(basename $(notdir $@)) ../$<

# Run the tests
check\
test/$(project).gcda: maketest.sh test/fftTest.c
	maketest.sh


#-------------------------------------------------------------------------------
# Create distribution tar ball

.PHONY: dist clean distclean verify help

dist: all clean
	@if [ -e "../$(project)-$(version)" ] ; then\
	    echo "../$(project)-$(version) already exists. Please remove.";\
	    exit 1;\
	fi
	curdir=~+; ln -sni $$curdir "../$(project)-$(version)"
	tar --exclude=*.tar.gz --exclude=$(DocBookOut) --exclude=$(DoxyXmlDir) \
	    --exclude=dev --numeric-owner -chzvf \
	    "../$(project)-$(version).tar.gz" -C .. "$(project)-$(version)"
	rm "../$(project)-$(version)"

# Verify the xml structure of the intermediate DocBook xml documentation file
verify: $(DocBookOut)/$(project).dbxml
	xmllint --noout --nonet --xinclude --postvalid --noent $(DocBookOut)/$(project).dbxml

$(project)\
$(HtmlOut)/index.html \
test/$(project).gcda \
: Makefile

clean:
	-rm -vf $(HtmlOut)/index.html~ doxygen.log
	$(doxyclean)
	-rm -vf test/fftTest test/fft.c test/ffti.c
	-rm -vf test/$(project) test/$(project).gcda test/$(project).gcno
	-rm -vf test/stdout.log test/stderr.log
	-rm -vf test/scripts/pod*.tmp

distclean:
	-rm -vf $(project)
	-rm -vf $(HtmlOut)/index.html $(HtmlOut)/doxy.css
	-rm -vf test/$(project).c.gcov test/$(project).cov.html

help:
	@echo "Type 'make $(project)' to compile the program"
	@echo "Type 'make html' to get the html user manual"
	@echo "Type 'make all' compile the program and get the html user manual"
	@echo "Type 'make dist version=X.Y' to create the tar ball for distribution"
	@echo "Type 'make check' to run a test"
	@echo "Type 'make clean' to delete unnecessary temporary files"
	@echo "Type 'make distclean' to delete all maked files"
	@echo "Type 'make help' to get this info"
	@echo "See the Makefile for further make targets"
