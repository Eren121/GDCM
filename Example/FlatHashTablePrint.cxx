#include "gdcmHeader.h"

// Iterate on all the Dicom entries encountered in the gdcmFile (given
// as line argument) and print them. This is an illustration of the
// usage of \ref gdcmDocument::BuildFlatHashTable().

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom " << std::endl;
      return 1;
   }

   gdcm::Header* header = new gdcm::Header( argv[1] );
   gdcm::TagDocEntryHT* Ht = header->BuildFlatHashTable();
   
   for (gdcm::TagDocEntryHT::iterator tag = Ht->begin(); tag != Ht->end(); ++tag)
   {
      tag->second->Print(); 
      std::cout << std::endl;
   }

   return 0;
}
