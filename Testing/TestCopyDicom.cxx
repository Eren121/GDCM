#include "gdcmHeader.h"
#include "gdcmFile.h"
#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//Generated file:
#include "gdcmDataImages.h"

#ifndef _WIN32
#include <unistd.h> //for access, unlink
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
        // std::cerr << "Don't try to cheat, I am removing the file anyway" << std::endl;
         if( !RemoveFile( output.c_str() ) )
         {
            std::cerr << "Ouch, the file exist, but I cannot remove it" << std::endl;
            return 1;
         }
      }

      gdcmFile *original = new gdcmFile( filename );
      gdcmFile *copy = new gdcmFile( output );

      TagDocEntryHT & Ht = original->GetHeader()->GetEntry();

      //First of all copy the header field by field
  
      // Warning :Accessor gdcmElementSet::GetEntry() should not exist 
      // It was commented out by Mathieu, that was a *good* idea
      // (the user does NOT have to know the way we implemented the Header !)
      // Waiting for a 'clean' solution, I keep the method ...JPRx

      gdcmDocEntry* d;

      for (TagDocEntryHT::iterator tag = Ht.begin(); tag != Ht.end(); ++tag)
      {
         d = tag->second;
         if ( gdcmBinEntry* b = dynamic_cast<gdcmBinEntry*>(d) )
         {              
            copy->GetHeader()->ReplaceOrCreateByNumber( 
                                 b->GetVoidArea(),
                                 b->GetLength(),
                                 b->GetGroup(), 
                                 b->GetElement(),
                                 b->GetVR() );
         }
         else if ( gdcmValEntry* v = dynamic_cast<gdcmValEntry*>(d) )
         {   
             copy->GetHeader()->ReplaceOrCreateByNumber( 
                                 v->GetValue(),
                                 v->GetGroup(), 
                                 v->GetElement(),
                                 v->GetVR() ); 
         }
         else
         {
          // We skip pb of SQ recursive exploration
          //std::cout << "Skipped Sequence " 
          //          << "------------- " << d->GetVR() << " "<< std::hex
          //          << d->GetGroup() << " " << d->GetElement()
          //  << std::endl;    
         }
      }

      size_t dataSize = original->GetImageDataSize();
      void *imageData = original->GetImageData();

      copy->SetImageData(imageData, dataSize);
      original->GetHeader()->SetImageDataSize(dataSize);

      copy->WriteDcmExplVR( output );

      delete original;
      delete copy;

      copy = new gdcmFile( output );

      //Is the file written still gdcm parsable ?
      if ( !copy->GetHeader()->IsReadable() )
      { 
         retVal +=1;
         std::cout << output << " Failed" << std::endl;
      }
      i++;
   }
   return retVal;
}

