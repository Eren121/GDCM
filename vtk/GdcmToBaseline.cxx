/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: GdcmToBaseline.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/30 15:44:44 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "vtkGdcmReader.h"
#include "vtkImageViewer.h"
#include "vtkImageData.h"
#include "vtkPNGWriter.h"
#include "vtkWindowToImageFilter.h"




#include "gdcmFile.h"

#include <iostream>

//Generated file:
#include "gdcmDataImages.h"

int main(int argc, char *argv[])
{
   if( argc < 3 )
   {
      std::cerr << "Usage: " << argv[0] << " image.dcm image.png\n";
      return 0;
   }

   std::string filename = argv[1];
   std::cout << "Filename: " << filename << std::endl;

   //Extract name to find the png file matching:
   std::string pngfile = argv[2];
   std::cerr << "PNG file: " << pngfile << std::endl;
 
   // Ok for now still use the original image, 
   vtkGdcmReader *reader = vtkGdcmReader::New();
   reader->SetFileName( filename.c_str() );
   //reader->SetFileName( "../Testing/Temporary/TestWrite.dcm" );
   reader->Update();

   vtkImageViewer *viewer = vtkImageViewer::New();
   viewer->SetInput ( reader->GetOutput() );

   vtkFloatingPointType *range = reader->GetOutput()->GetScalarRange();
   viewer->SetColorWindow (range[1] - range[0]);
   viewer->SetColorLevel (0.5 * (range[1] + range[0]));

   int dim[3];
   reader->GetOutput()->GetDimensions( dim );
   viewer->SetSize(dim[0], dim[1]);
   viewer->OffScreenRenderingOn();
 
   if(dim[2] != 1)
   {
      //For multifame dicom, take a snapshot of the center slice (+/- 1)
      viewer->SetZSlice( dim[2] / 2 );
   }

   viewer->Render();

   // View to image
   vtkWindowToImageFilter *wif = vtkWindowToImageFilter::New();
   wif->SetInput(viewer->GetRenderWindow());

   // Write to image
   vtkPNGWriter *writer=vtkPNGWriter::New();
   writer->SetFileName(argv[2]);
   writer->SetInput(wif->GetOutput());
   writer->Write();

   // Clean up:
   reader->Delete();
   viewer->Delete();
   wif->Delete();
   writer->Delete();

   return 0;
}
