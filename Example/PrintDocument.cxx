#include <iostream>

#include "gdcmException.h"
#include "gdcmCommon.h"

#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"
#include "gdcmHeader.h"
#include "gdcmUtil.h"
#include "gdcmBinEntry.h"  
#include "gdcmDocEntry.h" 
#include "gdcmDocEntrySet.h"           
#include "gdcmDocument.h"          
#include "gdcmElementSet.h"        
#include "gdcmSeqEntry.h" 
#include "gdcmSQItem.h" 
#include "gdcmValEntry.h" 

int main(int argc, char* argv[])
{
 
//   gdcmFile *e2;
   gdcmHeader *e1;
   bool dropPriv = false;
   bool showSeq  = true; 
   bool niou     = false;  
   std::string fileName;   

   if (argc == 1) {
      std::cout << argv[0] <<
      " fileName" << std::endl <<
      "    [nopriv]  if you don't want to print Shadow groups"  << std::endl <<
      "    [noseq]   if you don't want to 'go inside' the SQ's" << std::endl <<
      "    [new]     if you want a 'SeQuence indented' printing"<< std::endl;
   }	

   if (argc > 1) {
      fileName=argv[1];
   } else {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }
   
   for (int j=0;j<argc;j++) {
      if (strcmp(argv[j],"nopriv")==0)
         dropPriv=true;
      else if (strcmp(argv[j],"noseq")==0)
         showSeq=false;
      else if (strcmp(argv[j],"new")==0)
         niou = true;	 
   }
   
   //e2 = new gdcmFile(fileName.c_str(),false, showSeq, dropPriv);
   //e1 = e2->GetHeader();
    
   e1= new gdcmHeader 
   	(fileName.c_str(),
	false, showSeq,
	dropPriv);
	
//   if (argc > 2) {
//      int level = atoi(argv[2]);   
//      e1->SetPrintLevel(level);
//   }

e1->SetPrintLevel(2);

   //if (! niou) 
   //   e1->Print();   
   //else if (showSeq)
   //   e1->PrintNiceSQ();
   //else
   //   e1->PrintNoSQ();
   
  e1->Print();
      
  std::cout << "\n\n" << std::endl;      
  std::string transferSyntaxName = e1->GetTransfertSyntaxName();
  std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
   if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA"     )
  {
	std::cout << std::endl << "==========================================="
                  << std::endl; 
        //e2->ParsePixelData();
	std::cout << std::endl << "==========================================="
                  << std::endl; 
   }	      
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
   
}
