## Process this file with automake to produce Makefile.in

SUBDIRS = src Test Dicts

release:
	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
