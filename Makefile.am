## Process this file with automake to produce Makefile.in

SUBDIRS = src vtk gdcmPython Test Dicts Doc

EXTRA_DIST = \
	AUTHORS	\
	ChangeLog \
	COPYING \
	INSTALL \
	NEWS \
	README \
	TODO \
	gdcm.spec \
	gdcm.dsw

release:
	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
