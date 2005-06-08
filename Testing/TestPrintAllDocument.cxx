/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestPrintAllDocument.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/08 04:01:41 $
  Version:   $Revision: 1.3 $
                                                                                
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

#include <fstream>
#include <iostream>
#include <iomanip> // for std::ios::left, ...

//Generated file:
#include "gdcmDataImages.h"

int TestPrintAllDocument(int, char *[])
{
   //std::ostringstream s;
   int i = 0;
   int swapC;
   std::string pixelType, photomInterp;
   int l;
   l = strlen("PALETTE COLOR ");
   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i];

      gdcm::File *e1= new gdcm::File( filename );
      e1->SetPrintLevel(2);
      e1->Print();
      // just to be able to grep the display result, for some usefull info
 
      //s.setf(std::ios::left);
      //s << std::setw(60-filename.length()) << " ";
      //std::cout << s.str() << gdcmDataImages[i];

      std::cout << gdcmDataImages[i];
      unsigned int j;
      for ( j=0; j<60-strlen(gdcmDataImages[i]); j++)
         std::cout << " ";    

      pixelType = e1->GetPixelType();
      std::cout << " pixelType="            << pixelType;
      if (pixelType == "8U" || pixelType == "8S" )
         std::cout << " ";
      std::cout << " Smpl.P.Pix.="          << e1->GetSamplesPerPixel()
                << " Plan.Config.="         << e1->GetPlanarConfiguration();
      photomInterp =  e1->GetEntryValue(0x0028,0x0004);
               
      std::cout << " Photom.Interp.="       << photomInterp;
      for (j=0; j<l-photomInterp.length(); j++)
         std::cout << " ";
 
      std::cout << " TransferSyntaxName= [" << e1->GetTransferSyntaxName() << "]" ;
      swapC = e1->GetSwapCode();
      if (swapC != 1234)
          std::cout << " SwapCode = "       << e1->GetSwapCode(); 
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
