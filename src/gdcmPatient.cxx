// gdcmPatient.cxx
//-----------------------------------------------------------------------------
#include "gdcmPatient.h"

gdcmPatient::gdcmPatient() {

}


gdcmPatient::~gdcmPatient() {
   lStudy::iterator cc = GetStudies().begin();
   while  (cc != GetStudies().end() ) {
      delete *cc;
      ++cc;
   }
}

