/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg12.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/22 03:05:42 $
  Version:   $Revision: 1.23 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"

extern "C" {
#include "src/jpeg/libijg12/jconfig.h"
#include "src/jpeg/libijg12/jpeglib.h"
#include "src/jpeg/libijg12/jinclude.h"
#include "src/jpeg/libijg12/jerror.h"
}


#define gdcm_write_JPEG_file gdcm_write_JPEG_file12
#define gdcm_read_JPEG_file  gdcm_read_JPEG_file12

#include "gdcmJpeg.cxx"

