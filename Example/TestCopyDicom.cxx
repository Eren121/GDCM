#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDocument.h"
#include "gdcmValEntry.h"

#ifndef _WIN32
#include <unistd.h>
#endif

// return true if the file exists
bool FileExists(const char* filename)
{
#ifdef _MSC_VER
# define access _access
#endif
#ifndef R_OK
# define R_OK 04
#endif
  if ( access(filename, R_OK) != 0 )
    {
    return false;
    }
  else
    {
    return true;
    }
}

bool RemoveFile(const char* source)
{
#ifdef _MSC_VER
#define _unlink unlink
#endif
  return unlink(source) != 0 ? false : true;
}

// Here we load a gdcmFile and then try to create from scratch a copy of it,
// copying field by field the dicom image

int main(int argc, char* argv[])
{
   if (argc < 3)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom output_dicom" << std::endl;
      return 1;
   }

   if( FileExists( argv[2] ) )
   {
      std::cerr << "Don't try to cheat, I am removing the file anyway" << std::endl;
      if( !RemoveFile( argv[2] ) )
      {
         std::cerr << "Ouch, the file exist, but I cannot remove it" << std::endl;
         return 1;
      }
   }
   gdcmFile *original = new gdcmFile( argv[1] );
   
   std::cout << "--- Original ----------------------" << std::endl;
   //original->GetHeader()->Print();
   
   gdcmFile *copy = new gdcmFile( argv[2] );

   //First of all copy the header field by field
  
   // Warning :Accessor gdcmElementSet::GetEntry() should not exist 
   //It was commented out by Mathieu, that was a *good* idea
   // (the user does NOT have to know the way we implemented the Header !)
    
   TagDocEntryHT & Ht = original->GetHeader()->GetEntry();   
   
   for (TagDocEntryHT::iterator tag = Ht.begin(); tag != Ht.end(); ++tag)
   {
      if (tag->second->GetVR() == "SQ") //to skip pb of SQ recursive exploration
         continue;

      //According to JPR I should also skip those:
      if (tag->second->GetVR() == "unkn") //to skip pb of SQ recursive exploration
         continue;

      std::string value = ((gdcmValEntry*)(tag->second))->GetValue();
      if( value.find( "gdcm::NotLoaded" ) != 0 )
         continue;

// the following produce a seg fault at write time:
//      if( value.find( "gdcm::Loaded" ) != 0 )
//         continue;

      //std::cerr << "Reading: " << tag->second->GetVR() << std::endl;
      //tag->second->Print(); std::cout << std::endl;
    
      //std::cerr << "Reading: " << value  << std::endl;

      // Well ... Should have dynamic cast here 
      copy->GetHeader()->ReplaceOrCreateByNumber( 
                                 value,
                                 tag->second->GetGroup(), 
                                 tag->second->GetElement() );
   
     // todo : Setting Offset to 0 to avoid further missprint 
   }

   size_t dataSize = original->GetImageDataSize();
   void *imageData = original->GetImageData();

   copy->GetImageData();
   copy->SetImageData(imageData, dataSize);

   std::cout << "--- Copy ----------------------" << std::endl;
   std::cout <<std::endl << "DO NOT care about Offset"  <<std::endl<<std::endl;; 
   //copy->GetHeader()->Print();
   std::cout << "--- ---- ----------------------" << std::endl;
   
   copy->WriteDcmExplVR( argv[2] );

   return 0;
}



