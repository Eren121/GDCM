#include "gdcmVR.h"

int TestVR(int , char *[])
{
   gdcm::VR vr;
   // There should be 16 entries
   vr.Print( std::cout );
   vr.IsVROfGdcmStringRepresentable( "" );
   vr.IsVROfGdcmBinaryRepresentable( "" );

   return 0;
}
