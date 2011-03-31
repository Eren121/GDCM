/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exUnMosaic.cxx,v $
  Language:  C++
  Date:      $Date: 2011/03/31 21:45:08 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <stdlib.h> // for exit
#include <math.h> // for sqrt, ceil
#include "gdcmFile.h"
#include "gdcmFileHelper.h"

bool reorganize_mosaic(const unsigned short *input, const unsigned int *inputdims, unsigned int square,
  const unsigned int *outputdims, unsigned short *output );
  
  
  
bool reorganize_mosaic(const unsigned short *input, const unsigned int *inputdims, unsigned int nbImagesPerRow,
  const unsigned int *outputdims, unsigned short *output )
{
  for(unsigned x = 0; x < outputdims[0]; ++x)
    {
    for(unsigned y = 0; y < outputdims[1]; ++y)
      {
      for(unsigned z = 0; z < outputdims[2]; ++z)
        {
        output[ x + y*outputdims[0] + z*outputdims[0]*outputdims[1] ] =
          input[ (x + z * outputdims[0]) + (y + (z/nbImagesPerRow)*outputdims[0])*inputdims[0] ];
        }
      }
    }
  return true;
}


int main(int argc, char *argv[])
{   

   std::string fileName       = argv[1];
   std::string outputFileName = argv[2];
   unsigned int nbImagesPerRow = 8;
   unsigned int numberOfImagesInMosaic = 52;
   
   unsigned int inputdims[2];
   inputdims[0] = 1024;
   inputdims[1] = 1024;
   
   unsigned int outputdims[3];
     
// ============================================================
//   Read the input image.
// ============================================================

   std::cout << argv[1] << std::endl;

   GDCM_NAME_SPACE::File *f = GDCM_NAME_SPACE::File::New();
   f->SetLoadMode( GDCM_NAME_SPACE::LD_ALL);
   f->SetFileName( fileName );
   bool res = f->Load();        

   if (!res) {
       std::cerr << "Sorry, " << fileName <<"  not a gdcm-readable "
                 << "DICOM / ACR File"
                 <<std::endl;
       f->Delete();
       return 0;
   }
   std::cout << " ... is readable " << std::endl;
   
   
// ============================================================
//   Read the Pixels
//
// ============================================================
 
   // Pixel Reading must be done here, to be sure 
   // to load the Palettes Color (if any)

   // First, create a GDCM_NAME_SPACE::FileHelper
   GDCM_NAME_SPACE::FileHelper *fh = GDCM_NAME_SPACE::FileHelper::New(f);

   // Load the pixels, DO NOT transform LUT (if any) into RGB Pixels 
   uint8_t *imageDataRaw = fh->GetImageDataRaw();
   // Get the image data size
   size_t dataRawSize    = fh->GetImageDataRawSize();
   
// ============================================================
//   Create a new GDCM_NAME_SPACE::Filehelper, to hold new image.
// ============================================================

   GDCM_NAME_SPACE::FileHelper *copy = GDCM_NAME_SPACE::FileHelper::New( );
   copy->SetFileName( outputFileName );
  // copy->Load(); 

  unsigned int div = (unsigned int)ceil(sqrt( (double)numberOfImagesInMosaic ) );
  outputdims[0] = inputdims[0]/ div;
  outputdims[1] = inputdims[1] / div;
  outputdims[2] = numberOfImagesInMosaic;
  
/*   
  if (outputdims[0] *  outputdims[1] * outputdims[2] * 2 != dataRawSize)
  {
     std::cout << "outputdims[0] : " << outputdims[0] << " outputdims[1] : " <<  outputdims[1] << " numberOfImagesInMosaic : " << numberOfImagesInMosaic
               << " ( = " << outputdims[0] *  outputdims[1] * outputdims[2] * 2 << " ) doesn't match with dataRawSize : " << dataRawSize << std::endl;
     //exit(0);
  }
*/ 

   
  unsigned short *input = (unsigned short *)imageDataRaw;
  
  unsigned short *output = (unsigned short *) malloc(outputdims[0] * outputdims[1] * numberOfImagesInMosaic * 2);
  
  reorganize_mosaic(input, inputdims,
                    nbImagesPerRow,
                    outputdims, output);

  FILE *fPixels;
  fPixels = fopen(outputFileName.c_str(), "w");
  fwrite(output,2, outputdims[0] * outputdims[1] * numberOfImagesInMosaic, fPixels);
} 
   
