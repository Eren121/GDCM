## Process this file with automake to produce Makefile.in
LIBS_VTK=-L/usr/lib/vtk \
         -lvtkCommon -lvtkIO -lvtkFiltering -lvtkRendering -lvtkGraphics \
         -lvtkImaging -lvtkpng -lvtkzlib -lvtkjpeg -lvtkexpat -lvtktiff \
         -lvtkftgl -lvtkfreetype
INCLUDE_VTK=-I/usr/include/vtk

LIBS_X11= -L/usr/X11R6/lib -lX11 -lXt -lSM -lICE -lXext -lGL
##   $(x_ldflags)    
##   $(x_libs)      

INCLUDES =         \
   -I.          \
   -I$(top_srcdir)/src \
   $(INCLUDE_VTK)

LDADD =              \
   $(top_builddir)/src/libgdcm.la \
   $(LIBS_VTK) \
   $(LIBS_X11)

if BUILD_VTK
noinst_PROGRAMS = vtkgdcmdemo
vtkgdcmdemo_SOURCES = testvtkGdcmReader.cxx  vtkGdcmReader.cxx 

else
all:
endif


