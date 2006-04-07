/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFileAccessors.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/07 10:58:51 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

// TODO : check what's *actually* usefull

#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmUtil.h"
#include "gdcmCommon.h"
#include "gdcmDocEntry.h" 
#include "gdcmDocEntrySet.h"           
#include "gdcmDocument.h"          
#include "gdcmElementSet.h"        
#include "gdcmSeqEntry.h" 
#include "gdcmSQItem.h" 

#include <iostream>

//Generated file:
#include "gdcmDataImages.h"

#define TestMethodMacro(mode,obj,name)                \
   try                                                \
   {                                                  \
      std::cout <<"   "<< #name << "() : "            \
                << std::endl;                         \
      std::cout << "                     "            \
                << mode << obj->name() << std::endl;  \
   }                                                  \
   catch(...)                                         \
   {                                                  \
      std::cout << "   --> Can't access to the '"     \
                << #name << "' method !" << std::endl;\
      f->Delete();                                    \
      return 1;                                       \
   }

int TestFileAccessors(int argc, char *argv[])
{
   int i = 0;

   float iop[6];

  // gdcm::Debug::DebugOn();
   
   while( gdcmDataImages[i] != 0 )
   {
   
     // Keep the comment to be able to track a bug on a given image
     // for all the OS.
     
     // if (gdcmDataImages[i] == "00191113.dcm" )
     //    gdcm::Debug::DebugOn();
     // else
     //    gdcm::Debug::DebugOff();

      std::string filename;      
      if (argc ==2)
      {
         filename = argv[1];
      }
      else
      {
         filename = GDCM_DATA_ROOT;
         filename += "/";  //doh!      
         filename += gdcmDataImages[i];
      }
      
      std::cout << " ----------------------------------------------"
                << "Begin with " << filename << std::endl;

      gdcm::File *f= gdcm::File::New( );
      f->SetFileName( filename );
      f->Load( );

// We don't check the returned values
// We just want to be sure no accessor seg faults on any image  ...
// And this will improve test coverage ;-)

      TestMethodMacro(std::dec,f,GetSwapCode)
      TestMethodMacro(std::dec,f,GetImageNumber)
      TestMethodMacro(std::dec,f,GetModality)
      TestMethodMacro(std::dec,f,GetXSize)
      TestMethodMacro(std::dec,f,GetYSize)
      TestMethodMacro(std::dec,f,GetZSize)
      TestMethodMacro(std::dec,f,GetXSpacing)
      TestMethodMacro(std::dec,f,GetYSpacing)
      TestMethodMacro(std::dec,f,GetZSpacing)
      TestMethodMacro(std::dec,f,GetXOrigin)
      TestMethodMacro(std::dec,f,GetYOrigin)
      TestMethodMacro(std::dec,f,GetZOrigin)
      TestMethodMacro(std::dec,f,GetBitsStored)
      TestMethodMacro(std::dec,f,GetBitsAllocated)
      TestMethodMacro(std::dec,f,GetHighBitPosition)
      TestMethodMacro(std::dec,f,GetSamplesPerPixel)
      TestMethodMacro(std::dec,f,GetPlanarConfiguration)
      TestMethodMacro(std::dec,f,GetPixelSize)
      TestMethodMacro(std::dec,f,GetPixelType)
      TestMethodMacro(std::dec,f,GetLUTNbits)
      TestMethodMacro(std::dec,f,GetRescaleIntercept)
      TestMethodMacro(std::dec,f,GetRescaleSlope)
      TestMethodMacro(std::hex,f,GetGrPixel)
      TestMethodMacro(std::hex,f,GetNumPixel)
      TestMethodMacro(std::dec,f,GetPixelOffset)

      TestMethodMacro(std::dec,f,GetPixelAreaLength)
      TestMethodMacro(std::dec,f,GetNumberOfScalarComponents)
      TestMethodMacro(std::dec,f,GetNumberOfScalarComponentsRaw)
      TestMethodMacro(std::dec,f,IsSignedPixelData)
      TestMethodMacro(std::dec,f,IsMonochrome)
      TestMethodMacro(std::dec,f,IsPaletteColor)
      TestMethodMacro(std::dec,f,IsYBRFull)
      TestMethodMacro(std::dec,f,HasLUT)
      TestMethodMacro(std::dec,f,GetTransferSyntax)
      TestMethodMacro(std::dec,f,GetTransferSyntaxName)
      TestMethodMacro(std::dec,f,GetFileType)
      TestMethodMacro(std::dec,f,GetFileName)

      f->GetImageOrientationPatient( iop );
      std::cout << "   Orientation:" << std::endl;
      for (int j=0; j<6; j++)
         std::cout << "      iop[" << j << "] = " << iop[j] << std::endl;

      if( f->IsReadable() )
      {
         std::cout << "   " << filename << " is Readable" << std::endl;

         gdcm::FileHelper *fh= gdcm::FileHelper::New( f );   

         TestMethodMacro(std::dec,fh,GetImageDataSize)
         TestMethodMacro(std::dec,fh,GetImageDataRawSize)
         TestMethodMacro(std::dec,fh,GetRGBDataSize)
         TestMethodMacro(std::dec,fh,GetRawDataSize)
         TestMethodMacro(std::dec,fh,GetUserDataSize)
         TestMethodMacro(std::dec,fh,GetWriteType)
 
         fh->Delete();
      }
      else
      {
         std::cout << filename << " is NOT Readable" 
                   << std::endl << std::endl;
         f->Delete();
         return 1;
      }
      f->Delete();

      if (argc ==2)
         break; // user asked to check a single file.

      i++;
   }
   return 0;
}
