/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestWriteSimple.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 16:16:58 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include "gdcmFile.h"

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

  gdcm::Header       *f1 = new gdcm::Header( header );
  gdcm::FileHelper   *f2 = new gdcm::FileHelper( f1 );

  // If the following call is important, then the API sucks. Why is it
  // required to allocate PixelData when we are not using it !?
  uint8_t* PixelData = f2->GetImageData(); //EXTREMELY IMPORTANT
  //Otherwise ReadPixel == -1 -> the dicom writing fails completely
  
  int dataSize    = f2->GetImageDataSize();
  // unsigned char cast is necessary to be able to delete the buffer
  // since deleting a void* is not allowed in c++
  uint8_t *imageData = (uint8_t *)f2->GetImageData();

  f2->SetImageData( imageData, dataSize );

  f2->WriteDcmExplVR( output );
  
  delete f1;
  delete f2;
  //delete PixelData; //Does GetImageData return the same pointer ?
  (void)PixelData;

  return 0;
}

