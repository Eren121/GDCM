#include "gdcmHeader.h"
#include "gdcmFile.h"
#include <string>

// Writting of a DICOM file, using a correct gdcmHeader.
// and pixels of an other image


int TestChangeHeader(int argc, char* argv[])
{
   if (argc < 3)
   {
      std::cerr << "usage :" << std::endl << 
      argv[0] << " FileForHeader FileForPixels" << std::endl;
      return 1;
   }

   std::string premier = argv[1];
   gdcmFile  *f1 = new gdcmFile(premier);

   std::string deuxieme = argv[2];
   gdcmFile  *f2 = new gdcmFile(deuxieme);

   //f1->PrintPubElVal();

   // We suppose the DICOM Entries of the second file *do* exist !

   std::string nbFrames = f2->GetHeader()->GetEntryByNumber(0x0028, 0x0008);
   if(nbFrames != "gdcm::Unfound")
   {
      f1->GetHeader()->ReplaceOrCreateByNumber( nbFrames, 0x0028, 0x0008);
   }
         
   f1->GetHeader()->ReplaceOrCreateByNumber(
      f2->GetHeader()->GetEntryByNumber(0x0028, 0x0010), 0x0028, 0x0010);// nbLig
   f1->GetHeader()->ReplaceOrCreateByNumber( 
      f2->GetHeader()->GetEntryByNumber(0x0028, 0x0011), 0x0028, 0x0011);// nbCol

// Probabely some more to update (?)

// TODO : add a default value
// TODO : add a method that receives a list of pairs  (gr,el), 
//                        and that does the work.

   int dataSize = f2->GetImageDataSize();
   printf ("dataSize %d\n",dataSize);
   void* imageData= f2->GetImageData();

// TODO : Why don't we merge theese 2 functions ?

   f1->SetImageData(imageData,dataSize);
   f1->GetHeader()->SetImageDataSize(dataSize);

   f1->GetHeader()->Print();

   std::string s0 =f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0000);
   std::string s10=f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0010);
   printf("lgr 7fe0, 0000 %s\n",s0.c_str());
   printf("lgr 7fe0, 0010 %s\n",s10.c_str());

   char resultat[512];

   sprintf(resultat, "%s.vol", deuxieme.c_str());
   printf ("WriteDCM\n");
  //f1->WriteDcmExplVR("itk-gdcm-ex.dcm");
  //f1->WriteDcmImplVR(resultat);
   f1->WriteAcr(resultat);

   return 0;
}
