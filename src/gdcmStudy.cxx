// gdcmStudy.cxx
//-----------------------------------------------------------------------------
#include "gdcmStudy.h"

gdcmStudy::gdcmStudy() {

}


gdcmStudy::~gdcmStudy() {
   lSerie::iterator cc = GetSeries().begin();
   while  (cc != GetSeries().end() ) {
      delete *cc;
      ++cc;
   }
}
