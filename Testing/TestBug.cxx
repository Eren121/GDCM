// The following crashes on Win32
// We have to be carefull when the code both use cout + printf VC++ gets
// confused, thus make sure we use only one kind of iostream

#include "gdcm.h"

int TestBug(int argc, char* argv[])
{  
  gdcm::Header* e1;

   if (argc > 1)
      e1 = new gdcm::Header( argv[1] );
   else {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/test.acr";
      e1 = new gdcm::Header( filename.c_str() );
   }
   //e1->PrintPubDict();
   //e1->GetPubDict()->GetEntriesByKey();
   e1->GetPubDict()->PrintByKey();
   delete e1;

   return 0;
}
