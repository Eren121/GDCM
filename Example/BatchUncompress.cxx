/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: BatchUncompress.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/12 23:46:52 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
/*
 * See :
 * http://groups.google.com/group/comp.protocols.dicom/browse_thread/thread/2c70859b49122249/3c292c6acc5603ac
 * For the origin of this example. I think most people are looking for this kind of batch anyway.
 * Someone at some point could update it to have more option, like not copying the private tags,
 * sequence...
 */
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDocument.h"

int main(int argc, char *argv[])
{
  if (argc < 3) 
    {
    std::cerr << "Usage :" << std::endl << argv[0] << 
      " InputHeader OutputDicom" << std::endl;
    return 0;
    }

  const char *inputfilename = argv[1];
  const char *outputfilename = argv[2];

  gdcm::File *input = new gdcm::File( );
  input->SetFileName( inputfilename );
//  input->SetLoadMode(loadMode);
  input->Load();
  if ( input->IsReadable() )
    {
    gdcm::FileHelper *output = new gdcm::FileHelper( input );

    output->GetImageData(); //EXTREMELY IMPORTANT
    //Otherwise ReadPixel == -1 -> the dicom writing fails completely
    int dataSize       = output->GetImageDataSize();
    uint8_t *imageData = output->GetImageData();

    output->SetImageData( imageData, dataSize);
    output->WriteDcmExplVR( outputfilename );

    delete output;
    }
  delete input;

  return 0;
}
