#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDocument.h"
#include "gdcmValEntry.h"

//Generated file:
#include "gdcmDataImages.h"

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

int TestCopyDicom(int , char* [])
{
   int i =0;
   int retVal = 0;  //by default this is an error
   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];
      std::cerr << "Filename: " << filename << std::endl;

      std::string output = "../Testing/Temporary/output.dcm";

      if( FileExists( output.c_str() ) )
      {
         std::cerr << "Don't try to cheat, I am removing the file anyway" << std::endl;
         if( !RemoveFile( output.c_str() ) )
         {
            std::cerr << "Ouch, the file exist, but I cannot remove it" << std::endl;
            return 1;
         }
      }

      gdcmFile *original = new gdcmFile( filename );
      gdcmFile *copy = new gdcmFile( output );

      //First of all copy the header field by field
      TagNameHT & nameHt = original->GetHeader()->GetPubDict()->GetEntriesByName();
      for (TagNameHT::iterator tag = nameHt.begin(); tag != nameHt.end(); ++tag)
      {
         if (tag->second->GetVR() == "SQ") //to skip pb of SQ recursive exploration
            continue;

         //According to JPR I should also skip those:
//         if (tag->second->GetVR() == "unkn") //to skip pb of SQ recursive exploration
//            continue;

         //no clue what to do with this one
         //std::cerr << "Reading: " << tag->second->GetVR() << std::endl;
         std::string value = ((gdcmValEntry*)(tag->second))->GetValue();
//         if( value.find( "gdcm::NotLoaded" ) == 0 )
//            continue;

         //no clue what to do with this one
         if( value.find( "gdcm::Loaded" ) == 0 )
            continue;

         copy->GetHeader()->ReplaceOrCreateByNumber( 
               value, 
               tag->second->GetGroup(), 
               tag->second->GetElement() );
      }

      size_t dataSize = original->GetImageDataSize();
      void *imageData = original->GetImageData();

      copy->SetImageData(imageData, dataSize);
      //original->GetHeader()->SetImageDataSize(dataSize);

      //copy->GetHeader()->PrintEntry();

      //copy->WriteDcmExplVR( output );
      
      //Is the file written still gdcm parsable ?
      gdcmFile check( output );
      retVal += !check.GetHeader()->IsReadable();
   }

   return retVal;
}



