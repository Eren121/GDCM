CPPFLAGS=-g -Wall -Wunused-variable
LDFLAGS=-g

OBJECTS=gdcmUtil.o \
	gdcmHeader.o    \
	gdcmElValue.o   \
	gdcmDictEntry.o \
	gdcmDict.o      \
	gdcmDictSet.o   \
	gdcmElValSet.o  \
	gdcmHeaderIdo.o \
	gdcmFile.o

%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

all: libgdcm.so libgdcm.a

libgdcm.so: $(OBJECTS)
	g++ -shared -o $@ $^ $(LDFLAGS)

libgdcm.a: $(OBJECTS)
	ar cr $@ $^

clean:
	rm -f *.so *.a *.o
