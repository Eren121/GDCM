
###CC   = gcc
###LINK = gcc -shared -o ptinpoly.so
SWIG = ~/Local/bin/swig
SWIGFLAGS= -python -c++

PYTHON=python
PYTHON_PREFIX  =`$(PYTHON) -c "import sys; print sys.exec_prefix"`
PYTHON_VERSION =`$(PYTHON) -c "import sys; print sys.version[:3]"`
PYTHON_INCLUDES="-I$(PYTHON_PREFIX)/include/python$(PYTHON_VERSION)"

CXXFLAGS=$(PYTHON_INCLUDES)

%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
%_wrap.cxx : %.i
	$(SWIG) $(SWIGFLAGS) $(PYTHON_INCLUDES) -o $@ $<

all: dcm_wrap.o

clean:
	rm -f *_wrap* *.so *.o *.pyc

.SECONDARY: dcm_wrap.cxx
	
