## Process this file with automake to produce Makefile.in
LIBS_VTK=-L/usr/lib/vtk \
         -lvtkCommon -lvtkIO -lvtkFiltering -lvtkRendering -lvtkGraphics \
         -lvtkImaging -lvtkpng -lvtkzlib -lvtkjpeg -lvtkexpat -lvtktiff \
         -lvtkftgl -lvtkfreetype
LIBS_VTK_PYTHON= $(LIBS_VTK) \
         -lvtkCommonPython -lvtkIOPython -lvtkFilteringPython \
         -lvtkRenderingPython -lvtkGraphicsPython -lvtkImagingPython
INCLUDE_VTK=-I/usr/include/vtk

LIBS_X11= -L/usr/X11R6/lib -lX11 -lXt -lSM -lICE -lXext -lGL
#LIBS_X11= $(x_ldflags) $(x_libs)      

INCLUDES =             \
   -I.                 \
   -I$(top_srcdir)/src \
   $(PYTHON_INCLUDES)  \
   $(INCLUDE_VTK)

LDADD =              \
   $(top_builddir)/src/libgdcm.la \
   $(LIBS_VTK) \
   $(LIBS_X11)

if BUILD_VTK
noinst_PROGRAMS = vtkgdcmdemo vtkgdcmViewer
vtkgdcmdemo_SOURCES = testvtkGdcmReader.cxx  vtkGdcmReader.cxx 
vtkgdcmViewer_SOURCES = vtkgdcmViewer.cxx  vtkGdcmReader.cxx 
vtkgdcmdemo_LDADD = $(LDADD)
lib_LTLIBRARIES = libvtkgdcm.la
libvtkgdcm_la_SOURCES = vtkGdcmReader.cxx
libvtkgdcm_la_LIBADD = $(LDADD)
libvtkgdcmincludedir = $(includedir)
libvtkgdcminclude_HEADERS = vtkGdcmReader.h

else
all:
endif
