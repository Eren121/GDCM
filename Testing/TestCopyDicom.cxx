#include "gdcmHeader.h"
#include "gdcmFile.h"

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

int TestCopyDicom(int argc, char* argv[])
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
   gdcmFile *copy = new gdcmFile( argv[2] );

   //First of all copy the header field by field
   TagNameHT & nameHt = original->GetHeader()->GetPubDict()->GetEntriesByName();
   for (TagNameHT::iterator tag = nameHt.begin(); tag != nameHt.end(); ++tag)
   {
      std::cerr << "Reading: " << tag->second->GetVR() << std::endl;

      copy->GetHeader()->ReplaceOrCreateByNumber( tag->second->GetVR(), 
            tag->second->GetGroup(), tag->second->GetElement() );
   }

   size_t dataSize = original->GetImageDataSize();
   void *imageData = original->GetImageData();

   copy->SetImageData(imageData, dataSize);
   //original->GetHeader()->SetImageDataSize(dataSize);

   //copy->GetHeader()->PrintEntry();

   copy->WriteDcmExplVR( argv[2] );

   return 0;
}



