/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFileAccessors.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/03 09:51:53 $
  Version:   $Revision: 1.1 $
                                                                                
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
#include "gdcmBinEntry.h"  
#include "gdcmDocEntry.h" 
#include "gdcmDocEntrySet.h"           
#include "gdcmDocument.h"          
#include "gdcmElementSet.h"        
#include "gdcmSeqEntry.h" 
#include "gdcmSQItem.h" 
#include "gdcmValEntry.h" 

#include <iostream>

//Generated file:
#include "gdcmDataImages.h"

int TestFileAccessors(int, char *[])
{
   int i = 0;

   float iop[6];

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];

      gdcm::File *e1= new gdcm::File( filename );

// We don't check the returned values
// We just want to be sure no accessor seg faults on any image  ...
// And this will improve test coverage ;-)

      std::cout << "GetSwapCode()"      << e1->GetSwapCode()      << std::endl;
      std::cout << "GetImageNumber()"   << e1->GetImageNumber()   << std::endl;
      std::cout << "GetModality()"      << e1->GetModality()      << std::endl;
      std::cout << "GetXSize()"         << e1->GetXSize()         << std::endl;
      std::cout << "GetYSize()"         << e1->GetYSize()         << std::endl;
      std::cout << "GetZSize()"         << e1->GetZSize()         << std::endl;
      std::cout << "GetXSpacing()"      << e1->GetXSpacing()      << std::endl;
      std::cout << "GetYSpacing()"      << e1->GetYSpacing()      << std::endl;
      std::cout << "GetZSpacing()"      << e1->GetZSpacing()      << std::endl;
      std::cout << "GetXOrigin()"       << e1->GetXOrigin()       << std::endl;
      std::cout << "GetYOrigin()"       << e1->GetYOrigin()       << std::endl;
      std::cout << "GetZOrigin()"       << e1->GetZOrigin()       << std::endl;
      std::cout << "GetBitsStored()"    << e1->GetBitsStored()    << std::endl;
      std::cout << "GetBitsAllocated()" << e1->GetBitsAllocated() << std::endl;
      std::cout << "GetHighBitPosition()" << e1->GetHighBitPosition()  << std::endl;
      std::cout << "GetSamplesPerPixel()" << e1->GetSamplesPerPixel()  << std::endl;
      std::cout << "GetPlanarConfiguration()" << e1->GetPlanarConfiguration()    << std::endl;
      std::cout << "GetPixelSize()"        << e1->GetPixelSize()        << std::endl;
      std::cout << "GetPixelType()"        << e1->GetPixelType()        << std::endl;
      std::cout << "GetLUTNbits()"         << e1->GetLUTNbits()         << std::endl;
      std::cout << "GetRescaleIntercept()" << e1->GetRescaleIntercept() << std::endl;
      std::cout << "GetRescaleSlope()"     << e1->GetRescaleSlope()     << std::endl;
      std::cout << "GetGrPixel()"  << std::hex << e1->GetGrPixel()  << std::endl;
      std::cout << "GetNumPixel()" << std::hex << e1->GetNumPixel() << std::endl;
      std::cout << "GetPixelOffset()" << e1->GetPixelOffset() << std::endl;

      std::cout << "GetPixelAreaLength()"     << e1->GetPixelAreaLength()<< std::endl;
      std::cout << "GetNumberOfScalarComponents()"    << e1->GetNumberOfScalarComponents()<< std::endl;
      std::cout << "GetNumberOfScalarComponentsRaw()" << e1->GetNumberOfScalarComponentsRaw()<< std::endl;
      std::cout << "IsSignedPixelData()"      << e1->IsSignedPixelData()<< std::endl;
      std::cout << "IsMonochrome()"           << e1->IsMonochrome()     << std::endl;
      std::cout << "IsPaletteColor()"         << e1->IsPaletteColor()   << std::endl;
      std::cout << "IsYBRFull()"              << e1->IsYBRFull()        << std::endl;
      std::cout << "HasLut()   "              << e1->HasLUT()           << std::endl;
      std::cout << "GetTransferSyntax()"      << e1->GetTransferSyntax()      << std::endl;
      std::cout << "GetTransferSyntaxName()"  << e1->GetTransferSyntaxName()  << std::endl;
      std::cout << "GetFileType()"            << e1->GetFileType()            << std::endl;
      std::cout << "GetFileName()"            << e1->GetFileName()            << std::endl;

      e1->GetImageOrientationPatient( iop );
      for (int j=0; j<6; j++)
         std::cout << "iop[" << j << "] = " << iop[j] << std::endl;

      if( e1->IsReadable() )
      {
         std::cout <<filename   << " is Readable" 
                   << std::endl << std::endl;

         gdcm::FileHelper *fh1= new gdcm::FileHelper( e1 );   

         // TODO : Newbe user would appreciate any comment !
 
         std::cout << "GetImageDataSize()"    << fh1->GetImageDataSize()    << std::endl;
         std::cout << "GetImageDataRawSize()" << fh1->GetImageDataRawSize() << std::endl;
         // User Data
         std::cout << "GetRGBDataSize()"      << fh1->GetRGBDataSize()      << std::endl;
         std::cout << "GetRawDataSize()"      << fh1->GetRawDataSize()      << std::endl;
         std::cout << "GetUserDataSize()"     << fh1->GetUserDataSize()     << std::endl;

         std::cout << "GetWriteType()"        << fh1->GetWriteType()        << std::endl;
 
         delete fh1;
      }
      else
      {
         std::cout << filename << " is NOT Readable" 
                   << std::endl << std::endl;
         delete e1;
         return 1;
      }

      delete e1;
      i++;
   }
   return 0;
}
