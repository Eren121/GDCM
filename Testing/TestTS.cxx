#include "gdcmTS.h"

int TestTS(int , char *[])
{
   gdcm::TS ts;
   // There should be 150 entries
   ts.Print( std::cout );

   return ts.GetValue( "" ) != gdcm::GDCM_UNFOUND;
}
