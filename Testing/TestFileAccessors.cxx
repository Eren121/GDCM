/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestFileAccessors.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/08 13:39:57 $
  Version:   $Revision: 1.2 $
                                                                                
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
      
      gdcm::File *f= new gdcm::File( );
      f->SetFileName( filename );
      f->Load( );

// We don't check the returned values
// We just want to be sure no accessor seg faults on any image  ...
// And this will improve test coverage ;-)

      std::cout << "GetSwapCode()"      << f->GetSwapCode()      << std::endl;
      std::cout << "GetImageNumber()"   << f->GetImageNumber()   << std::endl;
      std::cout << "GetModality()"      << f->GetModality()      << std::endl;
      std::cout << "GetXSize()"         << f->GetXSize()         << std::endl;
      std::cout << "GetYSize()"         << f->GetYSize()         << std::endl;
      std::cout << "GetZSize()"         << f->GetZSize()         << std::endl;
      std::cout << "GetXSpacing()"      << f->GetXSpacing()      << std::endl;
      std::cout << "GetYSpacing()"      << f->GetYSpacing()      << std::endl;
      std::cout << "GetZSpacing()"      << f->GetZSpacing()      << std::endl;
      std::cout << "GetXOrigin()"       << f->GetXOrigin()       << std::endl;
      std::cout << "GetYOrigin()"       << f->GetYOrigin()       << std::endl;
      std::cout << "GetZOrigin()"       << f->GetZOrigin()       << std::endl;
      std::cout << "GetBitsStored()"    << f->GetBitsStored()    << std::endl;
      std::cout << "GetBitsAllocated()" << f->GetBitsAllocated() << std::endl;
      std::cout << "GetHighBitPosition()" << f->GetHighBitPosition()  << std::endl;
      std::cout << "GetSamplesPerPixel()" << f->GetSamplesPerPixel()  << std::endl;
      std::cout << "GetPlanarConfiguration()" << f->GetPlanarConfiguration()    << std::endl;
      std::cout << "GetPixelSize()"        << f->GetPixelSize()        << std::endl;
      std::cout << "GetPixelType()"        << f->GetPixelType()        << std::endl;
      std::cout << "GetLUTNbits()"         << f->GetLUTNbits()         << std::endl;
      std::cout << "GetRescaleIntercept()" << f->GetRescaleIntercept() << std::endl;
      std::cout << "GetRescaleSlope()"     << f->GetRescaleSlope()     << std::endl;
      std::cout << "GetGrPixel()"  << std::hex << f->GetGrPixel()  << std::endl;
      std::cout << "GetNumPixel()" << std::hex << f->GetNumPixel() << std::endl;
      std::cout << "GetPixelOffset()" << f->GetPixelOffset() << std::endl;

      std::cout << "GetPixelAreaLength()"     << f->GetPixelAreaLength()<< std::endl;
      std::cout << "GetNumberOfScalarComponents()"    << f->GetNumberOfScalarComponents()<< std::endl;
      std::cout << "GetNumberOfScalarComponentsRaw()" << f->GetNumberOfScalarComponentsRaw()<< std::endl;
      std::cout << "IsSignedPixelData()"      << f->IsSignedPixelData()<< std::endl;
      std::cout << "IsMonochrome()"           << f->IsMonochrome()     << std::endl;
      std::cout << "IsPaletteColor()"         << f->IsPaletteColor()   << std::endl;
      std::cout << "IsYBRFull()"              << f->IsYBRFull()        << std::endl;
      std::cout << "HasLut()   "              << f->HasLUT()           << std::endl;
      std::cout << "GetTransferSyntax()"      << f->GetTransferSyntax()      << std::endl;
      std::cout << "GetTransferSyntaxName()"  << f->GetTransferSyntaxName()  << std::endl;
      std::cout << "GetFileType()"            << f->GetFileType()            << std::endl;
      std::cout << "GetFileName()"            << f->GetFileName()            << std::endl;

      f->GetImageOrientationPatient( iop );
      for (int j=0; j<6; j++)
         std::cout << "iop[" << j << "] = " << iop[j] << std::endl;

      if( f->IsReadable() )
      {
         std::cout <<filename   << " is Readable" 
                   << std::endl << std::endl;

         gdcm::FileHelper *fh= new gdcm::FileHelper( f );   

         // TODO : Newbe user would appreciate any comment !
 
         std::cout << "GetImageDataSize()"    << fh->GetImageDataSize()    << std::endl;
         std::cout << "GetImageDataRawSize()" << fh->GetImageDataRawSize() << std::endl;
         // User Data
         std::cout << "GetRGBDataSize()"      << fh->GetRGBDataSize()      << std::endl;
         std::cout << "GetRawDataSize()"      << fh->GetRawDataSize()      << std::endl;
         std::cout << "GetUserDataSize()"     << fh->GetUserDataSize()     << std::endl;

         std::cout << "GetWriteType()"        << fh->GetWriteType()        << std::endl;
 
         delete fh;
      }
      else
      {
         std::cout << filename << " is NOT Readable" 
                   << std::endl << std::endl;
         delete f;
         return 1;
      }

      delete f;
      i++;
   }
   return 0;
}
