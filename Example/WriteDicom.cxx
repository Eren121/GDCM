#include "gdcm.h"

// Writting of a DICOM file based on a correct dicom header
// and data pixel of another image

int main(int argc, char* argv[])
{
 
  if (argc < 3) 
    {
    std::cerr << "Usage :" << std::endl << argv[0] << 
      " HeaderFileName DataFileName" << std::endl;
    return 0;
    }

  const char *first = argv[1];
  gdcm::File *f1 = new gdcm::File( first );

  const char *second = argv[2];
  gdcm::File *f2 = new gdcm::File( second );

  // We assume that DICOM fields of second file actually exists :

  std::string nbFrames = f2->GetHeader()->GetEntryByNumber(0x0028, 0x0008);
  if(nbFrames != "gdcm::Unfound") {
      f1->GetHeader()->ReplaceOrCreateByNumber( nbFrames, 0x0028, 0x0008);
  }
         
  f1->GetHeader()->ReplaceOrCreateByNumber(
    f2->GetHeader()->GetEntryByNumber(0x0028, 0x0010), 0x0028, 0x0010); // nbLig
  f1->GetHeader()->ReplaceOrCreateByNumber( 
    f2->GetHeader()->GetEntryByNumber(0x0028, 0x0011), 0x0028, 0x0011); // nbCol

  // Some other tags should be updated:

  // TODO : add a default value
  // TODO : a function which take as input a list of tuple (gr, el)
  //        and that does the job

  int dataSize    = f2->GetImageDataSize();
  uint8_t* imageData = f2->GetImageData();

  std::cout << "dataSize :" << dataSize << std::endl;

  // TODO : Shouldn't we merge those two functions ?
  f1->SetImageData( imageData, dataSize);
  f1->GetHeader()->SetImageDataSize( dataSize );

  f1->GetHeader()->Print();

  std::string s0  = f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0000);
  std::string s10 = f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0010);

  std::cout << "lgr 7fe0, 0000 " << s0  << std::endl;
  std::cout << "lgr 7fe0, 0010 " << s10 << std::endl;

  std::cout << "WriteDCM" << std::endl;

  f1->WriteDcmExplVR("WriteDicom.dcm");

  return 0;
}
