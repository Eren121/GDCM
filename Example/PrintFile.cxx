/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/26 16:21:55 $
  Version:   $Revision: 1.32 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "gdcmFileHelper.h"

#include <iostream>

int main(int argc, char *argv[])
{
   gdcm::File *e1;
   gdcm::FileHelper   *f1;
   std::string fileName;
  
   if (argc == 1) 
   {
      std::cout << " usage : PrintFile fileName printLevel debug "
          << "short (=NOSEQ + NOSHADOW)" 
                << std::endl;
    return 0;
   }

   e1 = new gdcm::File();

   if (argc > 1) 
   {
      fileName=argv[1];
   } 

   if (argc > 3)
      gdcm::Debug::DebugOn();

   if (argc > 4)
      e1->SetLoadMode(NO_SEQ | NO_SHADOW);   
   e1->Load( fileName.c_str() );

   if ( !e1->IsReadable() )
   {
      delete e1;
      return 0;
   }

   f1 = new gdcm::FileHelper(e1);

   if (argc > 2)  // keep it here (f1 needs to be constructed !)
   {
      int level = atoi(argv[2]);   
      f1->SetPrintLevel(level);
   }

   f1->Print();   

   std::cout << "\n\n" << std::endl; 

   std::cout <<std::endl;
   std::cout <<" dataSize " << f1->GetImageDataSize() << std::endl;
   std::cout <<" dataSizeRaw " << f1->GetImageDataRawSize() << std::endl;

   int nX,nY,nZ,sPP,planarConfig;
   std::string pixelType;
   nX=e1->GetXSize();
   nY=e1->GetYSize();
   nZ=e1->GetZSize();
   std::cout << " DIMX=" << nX << " DIMY=" << nY << " DIMZ=" << nZ << std::endl;

   pixelType    = e1->GetPixelType();
   sPP          = e1->GetSamplesPerPixel();
   planarConfig = e1->GetPlanarConfiguration();
   
   std::cout << " pixelType= ["           << pixelType 
             << "] SamplesPerPixel= ["     << sPP
             << "] PlanarConfiguration= [" << planarConfig 
             << "] "<< std::endl 
             << " PhotometricInterpretation= [" 
                                << e1->GetEntryValue(0x0028,0x0004)
             << "] "<< std::endl;

   int numberOfScalarComponents=e1->GetNumberOfScalarComponents();
   std::cout << " NumberOfScalarComponents = " << numberOfScalarComponents <<std::endl
             << " LUT = " << (e1->HasLUT() ? "TRUE" : "FALSE")
             << std::endl;

  
   if ( e1->GetEntryValue(0x0002,0x0010) == gdcm::GDCM_NOTLOADED ) 
   {
      std::cout << "Transfer Syntax not loaded. " << std::endl
                << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
                << std::endl;
      return 0;
   }
  
   std::string transferSyntaxName = e1->GetTransferSyntaxName();
   std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   std::cout << " SwapCode= " << e1->GetSwapCode() << std::endl;
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;
   delete f1;

   return 0;
   
}
