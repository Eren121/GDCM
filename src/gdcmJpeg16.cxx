/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg16.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/08 20:05:52 $
  Version:   $Revision: 1.5 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"

extern "C" {
#include "gdcmjpeg/16/jconfig.h"
#include "gdcmjpeg/16/jpeglib.h"
#include "gdcmjpeg/16/jinclude.h"
#include "gdcmjpeg/16/jerror.h"
}

#define gdcm_write_JPEG_file gdcm_write_JPEG_file16
#define gdcm_read_JPEG_file  gdcm_read_JPEG_file16

#include "gdcmJpeg.cxx"

