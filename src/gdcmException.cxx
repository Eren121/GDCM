
#include "gdcmException.h"

#include <typeinfo>
#include <stdio.h>

gdcmException::gdcmException(const std::string &f, const std::string& msg) throw()
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
    std::cerr << "Fatal: exception received in " << from 
	 << " while handling exception." << std::endl;
    exit(-1);
  }
  catch(...) {
    try {
      std::cerr << "Fatal: exception received in Exception::fatal while handling exception."
	   << std::endl;
      exit(-1);
    }
    catch(...) {
      exit(-1);
    }
  }  
}


std::string gdcmException::getName() const throw() {
  try {
#ifdef __GNUC__   // GNU C++ compiler class name demangling
      unsigned int nested = 1, i, nb, offset;
      std::string one;

      std::string name;
      std::string iname = typeid(*this).name();
      if(iname[0] == 'Q') {
	nested = iname[1] - '0';
	iname = std::string(iname, 2, std::string::npos);
      }
      for(i = 0; i < nested; i++) {
	::sscanf(iname.c_str(), "%u%n", &nb, &offset);
	iname = std::string(iname, offset, std::string::npos);
	name += std::string(iname, 0, nb);
	if(i + 1 < nested) name += "::";
	iname = std::string(iname, nb, std::string::npos);
      }
      return name;
#else             // no class name demangling
      //name = typeid(*this).name();
      return "Exception";
#endif
  }
  catch(...) {
    fatal("Exception::getName(std::string &)");
    return "";
  }
}


gdcmException::operator const char *() const throw() {
  return getName().c_str();
}


std::ostream& operator<<(std::ostream &os, const gdcmException &e) {
  try {  
    os << "Exception " << e.getName() << " thrown: " << e.getError() << std::endl;
  }
  catch(...) {
    gdcmException::fatal("operator<<(std::ostream &, const gdcmException&)");
  }
  return os;
}

  
