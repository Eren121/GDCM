// gdcmSequence.cxx
//-----------------------------------------------------------------------------
#include "gdcmSerie.h"

gdcmSerie::gdcmSerie() {

}


gdcmSerie::~gdcmSerie() {
   lImage::iterator cc = GetImages().begin();
   while  (cc != GetImages().end() ) {
      delete *cc;
      ++cc;
   }
}

