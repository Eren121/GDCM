## Process this file with automake to produce Makefile.in

SUBDIRS = src gdcmPython Test Dicts Doc vtk


EXTRA_DIST = \
	AUTHORS	\
	ChangeLog \
	COPYING \
	INSTALL \
	NEWS \
	README \
	TODO \
	gdcm.spec

release:
	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
