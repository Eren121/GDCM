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
 
   gdcmHeader *e1;
   std::string fileName;   

   if (argc != 2) {
      std::cout << " Usage : " << argv[0] 
                << " filename." << std::endl;
   }

   if (argc > 1) {
      fileName=argv[1];
   } else {
      fileName += GDCM_DATA_ROOT;
      fileName += "/test.acr";
   }
   
   e1= new gdcmHeader 
      (fileName.c_str(),false, true);

   e1->SetPrintLevel(2);
   
   e1->Print();
      
   std::cout << "\n\n" << std::endl;        
   
   if(e1->IsReadable())
      std::cout <<std::endl<<fileName<<" is Readable"<<std::endl;
   else
      std::cout <<std::endl<<fileName<<" is NOT Readable"<<std::endl;
   std::cout<<std::flush;
   delete e1;

   return 0;
   
}
