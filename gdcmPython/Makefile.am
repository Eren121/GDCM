## Process this file with automake to produce Makefile.in

### VTK related
VTK_INC = -I/usr/include/vtk
LIBS_VTK=-L/usr/lib/vtk \
         -lvtkCommon -lvtkIO -lvtkFiltering -lvtkRendering -lvtkGraphics \
         -lvtkImaging -lvtkpng -lvtkzlib -lvtkjpeg -lvtkexpat -lvtktiff \
         -lvtkftgl -lvtkfreetype
LIBS_VTK_PYTHON= $(LIBS_VTK) \
         -lvtkCommonPython -lvtkIOPython -lvtkFilteringPython \
         -lvtkRenderingPython -lvtkGraphicsPython -lvtkImagingPython
### Gdcm itself:
GDCM_INC    = -I$(top_srcdir)/src  -I$(top_builddir)
### Vtk wrappers of gdcm relate:
VTKGDCM_DIR = $(top_srcdir)/vtk/
VTKGDCM_INC = -I$(VTKGDCM_DIR)
### Swig related (for wrapping Gdcm):
SWIG_INC    = $(PYTHON_INCLUDES) $(GDCM_INC) 

### The resulting include path:
INCLUDES    = $(GDCM_INC) $(PYTHON_INCLUDES) $(VTK_INC) $(VTKGDCM_INC)

### Since maude_LDFLAGS cannot be defined conditionaly:
pygdcm_la_LDFLAGS = -module -avoid-version -L$(top_builddir)/src/.libs
vtkgdcmPython_la_LDFLAGS = -module -avoid-version

############################################################
if BUILD_PYTHON
pythondir = $(prefix)/lib/python${PYTHON_VERSION}/site-packages/gdcm
if BUILD_VTK
GDCMVTKLIB = vtkgdcmPython.la
endif
python_LTLIBRARIES = pygdcm.la $(GDCMVTKLIB)

############ Gdcm python wrappers:
pygdcm_la_SOURCES = gdcm_wrap.cxx
pygdcm_la_LIBADD = $(top_builddir)/src/libgdcm.la -lstdc++
gdcm_wrap.cxx: $(srcdir)/gdcm.i
	$(SWIG)  -python -c++ $(SWIG_INC) -o $@ $<

############ Vtk wrappers of Gdcm wrapped for python
if BUILD_VTK
PYTHON_LIB_BASENAME=vtkgdcmPython

# Python wrappers
PYTHON_WRAPPER = vtkWrapPython
PYTHON_WRAPPER_INIT=./vtkWrapPythonInit
PWFLAGS=hints

vtkgdcmPython_la_SOURCES =  \
	vtkGdcmReaderPython.cxx     \
	vtkGdcmInit.cxx
vtkgdcmPython_la_LIBADD = $(top_builddir)/vtk/libvtkgdcm.la \
                             $(LIBS_VTK_PYTHON)

vtkGdcmReaderPython.cxx: $(VTKGDCM_DIR)/vtkGdcmReader.h
	$(PYTHON_WRAPPER) $< $(PWFLAGS) $@

vtkGdcmInit.cxx : $(VTKGDCM_DIR)/vtkGdcmReader.h $(PYTHON_WRAPPER_INIT)
	$(PYTHON_WRAPPER_INIT) $(PYTHON_LIB_BASENAME) \
	vtkGdcmReader.h \
	> $@

$(PYTHON_WRAPPER_INIT) : vtkWrapPythonInit.c
	gcc -o $@ $<
endif### BUILD_PYTHON #######################################

############ Automake general usage classics:
MOSTLYCLEANFILES = $(pygdcm_la_SOURCES) $(vtkgdcmPython_la_SOURCES)
BUILT_SOURCES =    $(pygdcm_la_SOURCES) $(vtkgdcmPython_la_SOURCES)
CLEANFILES = gdcm_wrap.cxx gdcm.py gdcm.pyc vtkWrapPythonInit

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
