
###CC   = gcc
###LINK = gcc -shared -o ptinpoly.so
SWIG = swig
SWIGFLAGS= -python -c++

PYTHON=python
PYTHON_PREFIX  =`$(PYTHON) -c "import sys; print sys.exec_prefix"`
PYTHON_VERSION =`$(PYTHON) -c "import sys; print sys.version[:3]"`
PYTHON_INCLUDES="-I$(PYTHON_PREFIX)/include/python$(PYTHON_VERSION)"

CXXFLAGS=`glib-config --cflags`
CPPFLAGS=-g -Wall -Wunused-variable
LDFLAGS=`glib-config --libs` -g

%_wrap.o : %_wrap.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(PYTHON_INCLUDES) $< -o $@
%_wrap.cxx : %.i
	$(SWIG) $(SWIGFLAGS) $(PYTHON_INCLUDES) -o $@ $<
%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

all: gdcmlib.so

gdcmlib.so: gdcmUtil.o    \
	gdcmHeader.o    \
	gdcmElValue.o   \
	gdcmDictEntry.o \
	gdcmDict.o      \
	gdcmDictSet.o   \
	gdcmElValSet.o  \
	gdcmHeaderIdo.o \
	gdcmFile.o
	g++ -shared -o gdcmlib.so $^ $(LDFLAGS)

python: gdcmlib.so gdcm_wrap.o
	g++ -shared $^ -o _gdcm.so

clean:
	rm -f *_wrap* *.so *.o *.pyc gdcm.py

.SECONDARY: dcm_wrap.cxx
