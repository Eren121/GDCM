## Process this file with automake to produce Makefile.in

## $(top_builddir) is required for idtypes.h
GDCM_INC   = -I$(top_srcdir)/src  -I$(top_builddir)
SWIG_INC   = $(GLIB_CFLAGS) $(PYTHON_INCLUDES) $(GDCM_INC) 
INCLUDES   = $(GDCM_INC) $(PYTHON_INCLUDES)

LIBADDS     = $(top_builddir)/src/libgdcm.la

### Non default rules

pygdcm_la_LDFLAGS = -module -avoid-version -L$(top_builddir)/src/.libs

if BUILD_PYTHON
pythondir = $(prefix)/lib/python${PYTHON_VERSION}/site-packages/gdcm
python_LTLIBRARIES = pygdcm.la
pygdcm_la_SOURCES = gdcm_wrap.cxx
pygdcm_la_LIBADD = $(LIBADDS)
gdcm_wrap.cxx: $(srcdir)/gdcm.i
	$(SWIG)  -python -c++ $(SWIG_INC) -o $@ $<

MOSTLYCLEANFILES = $(pygdcm_la_SOURCES)
BUILT_SOURCES = gdcm_wrap.cxx
CLEANFILES = gdcm_wrap.cxx

dist-hook:
	rm -f $(distdir)/gdcm_wrap.cxx

install-data-local:
	@INSTALL@ -m 0644 gdcm.py $(DESTDIR)$(pythondir)
	@INSTALL@ -m 0644 __init__.py $(DESTDIR)$(pythondir)
	cp -a $(DESTDIR)$(pythondir)/pygdcm.so $(DESTDIR)$(pythondir)/_gdcm.so
else
all:
endif

EXTRA_DIST =  gdcm.i __init__.py testSuite.py
