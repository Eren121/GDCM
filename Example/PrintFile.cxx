/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PrintFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/07 11:12:10 $
  Version:   $Revision: 1.38 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"

#include "gdcmArgMgr.h"

#include <iostream>

int main(int argc, char *argv[])
{

   START_USAGE(usage)
   " \n PrintFile : \n",
   " Display the header of a ACR-NEMA/PAPYRUS/DICOM File",
   " usage: PrintFile filein=fileName [level=n] [noshadow] [noseq] [debug] ",
   "        level = 0,1,2 : depending on the amount of details user wants to see",
   "        noshadow : user doesn't want to load Private groups (odd number)",
   "        noseq    : user doesn't want to load Sequences ",
   "        debug    : user wants to run the program in 'debug mode' ",
   FINISH_USAGE

   // Initialize Arguments Manager   
   gdcm::ArgMgr *am= new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1)
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   char *fileName = am->ArgMgrWantString("filein",usage);

   int loadMode;
   if ( am->ArgMgrDefined("noshadow") && am->ArgMgrDefined("noseq") )
       loadMode = NO_SEQ | NO_SHADOW;  
   else if ( am->ArgMgrDefined("noshadow") )
      loadMode = NO_SHADOW;
   else if ( am->ArgMgrDefined("noseq") )
      loadMode = NO_SEQ;
   else
      loadMode = 0;

   int level = am->ArgMgrGetInt("level", 2);

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();

   /* if unused Param we give up */
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   } 
 
   // gdcm::File::IsReadable() is no usable here, because we deal with
   // any kind of gdcm-Parsable *document* 
   // not only gdcm::File (as opposed to gdcm::DicomDir)

   gdcm::File *e1 = new gdcm::File();
   e1->SetLoadMode(loadMode);

   bool res = e1->Load( fileName );
   if ( !res )
   {
      delete e1;
      delete am;
      return 0;
   }

   gdcm::FileHelper *f1 = new gdcm::FileHelper(e1);
   f1->SetPrintLevel( level );

   f1->Print();   

   std::cout << "\n\n" << std::endl; 

   std::cout <<std::endl;
   std::cout <<" dataSize    " << f1->GetImageDataSize()    << std::endl;
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
   
   std::cout << " pixelType= ["            << pixelType 
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
   delete am;
   return 0;
   
}
