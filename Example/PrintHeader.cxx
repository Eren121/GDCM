#include <iostream>
#include "gdcm.h"

int main(int argc, char* argv[])
{ 
   std::string fileName;   

   if (argc == 1)
   {
      std::cout << argv[0] << " fileName" << std::endl;
   }

   if (argc > 1)
   {
      fileName=argv[1];
   }
   else
   {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }
   
   gdcmFile *e2 = new gdcmFile( fileName.c_str() );
   gdcmHeader *e1 = e2->GetHeader();  
        
   if (argc > 2) {
      int level = atoi(argv[2]);   
      e1->SetPrintLevel(level);
   }

  e1->Print();   
      
  std::cout << "\n\n" << std::endl; 
  
  if ( e1->GetEntryByNumber(0x0002,0x0010) == GDCM_NOTLOADED ) {
     std::cout << "Transfert Syntax not loaded. " << std::endl
               << "Better you increase MAX_SIZE_LOAD_ELEMENT_VALUE"
               << std::endl;
     return 0;
  }
      
  std::string transferSyntaxName = e1->GetTransfertSyntaxName();
  std::cout << " TransferSyntaxName= [" << transferSyntaxName << "]" << std::endl;
   
   if (  transferSyntaxName != "Implicit VR - Little Endian"
      && transferSyntaxName != "Explicit VR - Little Endian"     
      && transferSyntaxName != "Deflated Explicit VR - Little Endian"      
      && transferSyntaxName != "Explicit VR - Big Endian"
      && transferSyntaxName != "Uncompressed ACR-NEMA" )
  {
  std::cout << std::endl << "==========================================="
                  << std::endl; 
        e2->ParsePixelData();
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
