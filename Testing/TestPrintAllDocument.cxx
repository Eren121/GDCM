/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestPrintAllDocument.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/02 10:41:10 $
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

int TestPrintAllDocument(int, char *[])
{
   int i = 0;

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];

      gdcm::File *e1= new gdcm::File( filename );
      e1->SetPrintLevel(2);
      e1->Print();

      // just to be able to grep the display result, for some usefull info     
      std::cout << filename
                << " TransferSyntaxName= [" << e1->GetTransferSyntaxName() 
                << "] SwapCode = "          << e1->GetSwapCode() 
                << " PhotometricInterpretation=" 
                                            << e1->GetEntryValue(0x0028,0x0004)
                << " pixelType="            << e1->GetPixelType() 
                << " SamplesPerPixel="      << e1->GetSamplesPerPixel()
                << " PlanarConfiguration="  << e1->GetPlanarConfiguration();
      if ( e1->CheckIfEntryExist(0x0088,0x0200) )
           std::cout << " Icon Image Sequence";

       std::cout << std::endl;
   
      if( e1->IsReadable() )
      {
         std::cout <<filename << " is Readable" 
                   << std::endl << std::endl;
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
