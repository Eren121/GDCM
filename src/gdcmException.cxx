#ifdef _MSC_VER
//'identifier' : not all control paths return a value
//#pragma warning ( disable : 4715 )
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include "gdcmException.h"

#include <typeinfo>
#include <stdio.h>
using namespace std;


gdcmException::gdcmException(const string &f, const string& msg) throw()
#ifdef __GNUC__
  try
#endif
  : from(f), error(msg) {
  }
#ifdef __GNUC__
catch(...) {
  fatal("gdcmException::gdcmException(const std::string&, const std::string&, const std::string&)");
}
#endif



void gdcmException::fatal(const char *from) throw() {
  try {
    cerr << "Fatal: exception received in " << from 
	 << " while handling exception." << endl;
    exit(-1);
  }
  catch(...) {
    try {
      cerr << "Fatal: exception received in Exception::fatal while handling exception."
	   << endl;
      exit(-1);
    }
    catch(...) {
      exit(-1);
    }
  }  
}


string gdcmException::getName() const throw() {
  try {
#ifdef __GNUC__   // GNU C++ compiler class name demangling
      unsigned int nested = 1, i, nb, offset;
      string one;

      string name;
      string iname = typeid(*this).name();
      if(iname[0] == 'Q') {
	nested = iname[1] - '0';
	iname = string(iname, 2, std::string::npos);
      }
      for(i = 0; i < nested; i++) {
	::sscanf(iname.c_str(), "%u%n", &nb, &offset);
	iname = string(iname, offset, std::string::npos);
	name += string(iname, 0, nb);
	if(i + 1 < nested) name += "::";
	iname = string(iname, nb, std::string::npos);
      }
      return name;
#else             // no class name demangling
      //name = typeid(*this).name();
      return "Exception";
#endif
  }
  catch(...) {
    fatal("Exception::getName(string &)");
    return "";
  }
}


gdcmException::operator const char *() const throw() {
  return getName().c_str();
}


ostream& operator<<(ostream &os, const gdcmException &e) {
  try {  
    os << "Exception " << e.getName() << " thrown: " << e.getError() << endl;
  }
  catch(...) {
    gdcmException::fatal("operator<<(ostream &, const gdcmException&)");
  }
  return os;
}

  
