.SECONDARY:

SWIG = swig
SWIGFLAGS= -python -c++

TARGET=_gdcm.so

GDCMDIR=../src
GDCMLIB=$(GDCMDIR)/.libs/libgdcm.a
GDCMINCLUDES=-I$(GDCMDIR)

PYTHON=python
PYTHON_PREFIX  =`$(PYTHON) -c "import sys; print sys.exec_prefix"`
PYTHON_VERSION =`$(PYTHON) -c "import sys; print sys.version[:3]"`
PYTHON_INCLUDES="-I$(PYTHON_PREFIX)/include/python$(PYTHON_VERSION)"

CXXFLAGS=$(GDCMINCLUDES)
CPPFLAGS=-g -Wall -Wunused-variable
LDFLAGS=-g

%_wrap.o : %_wrap.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(PYTHON_INCLUDES) $< -o $@
%_wrap.cxx : %.i
	$(SWIG) $(SWIGFLAGS) $(PYTHON_INCLUDES) $(GDCMINCLUDES) -o $@ $<
%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@


all: gdcm_wrap.o $(GDCMLIB)
	g++ -shared $^ -o $(TARGET) $(LDFLAGS)

test:
	$(PYTHON) -c"import _gdcm"

clean:
	rm -f *_wrap* *.so *.o *.pyc gdcm.py

$(GDCMLIB): FORCE
	cd $(GDCMDIR); make

FORCE:

#.SECONDARY: dcm_wrap.cxx
