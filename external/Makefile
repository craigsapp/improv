##
## Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
## Creation Date: Sun Mar  6 14:41:14 PST 2016
## Last Modified: Tue Mar  8 20:03:59 PST 2016
## Filename:      improv/external/Makefile
## Syntax:        GNU Makefile
##
## Description:   This Makefile prepares the external libraries.
##
## To run this makefile, type:
##     make
##

.PHONY: midifile

CURL = $(shell which curl)
WGET = $(shell which wget)


all: midifile


midifile: midifilecompile

	
midifiledownload:
ifeq ($(CURL),)
   ifeq ($(WGET),)
	@echo "Install either wget or curl on your computer to continue"
	exit 1
   else
	wget https://github.com/craigsapp/midifile/archive/master.zip -O midifile.zip
   endif
else
	curl -L https://github.com/craigsapp/midifile/archive/master.zip -o midifile.zip
endif


midifileunpack:
	unzip midifile.zip
	# unpacks to midifile-master, so move to midifile:
	-rm -rf midifile
	mv midifile-master midifile


midifilecompile:
ifeq ($(wildcard midifile),)
	$(MAKE) midifiledownload
	$(MAKE) midifileunpack
endif
	(cd midifile && $(MAKE) library)


clean:
	-rm -rf midifile
	-rm midifile.zip


