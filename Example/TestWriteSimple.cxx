#include "gdcm.h"

int main(int argc, char* argv[])
{

  if (argc < 3) 
    {
    std::cerr << "Usage :" << std::endl << argv[0] << 
      " InputHeader OutputDicom" << std::endl;
    return 0;
    }

  std::string header = argv[1];
  const char *output = argv[2];

  gdcmHeader *f1 = new gdcmHeader( header );
  gdcmFile   *f2 = new gdcmFile( f1 );

  // If the following call is important, then the API sucks. Why is it
  // required to allocate PixelData when we are not using it !?
  void* PixelData = f2->GetImageData(); //EXTREMELY IMPORTANT
  //Otherwise ReadPixel == -1 -> the dicom writing fails completely
  
  int dataSize    = f2->GetImageDataSize();
  // unsigned char cast is necessary to be able to delete the buffer
  // since deleting a void* is not allowed in c++
  char *imageData = (char*)f2->GetImageData();

  f2->SetImageData( imageData, dataSize);

  f2->WriteDcmExplVR( output );
  
  delete[] imageData;
  delete f1;
  delete f2;
  //delete PixelData; //Does GetImageData return the same pointer ?
  (void)PixelData;

  return 0;
}

