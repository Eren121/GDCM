/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg8.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/24 14:52:50 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFileHelper.h"
#include <stdio.h>

extern "C" {
#include "gdcmjpeg/8/jconfig.h"
#include "gdcmjpeg/8/jpeglib.h"
#include "gdcmjpeg/8/jinclude.h"
#include "gdcmjpeg/8/jerror.h"
}

#define gdcm_write_JPEG_file  gdcm_write_JPEG_file8
#define gdcm_read_JPEG_file   gdcm_read_JPEG_file8
#define gdcm_read_JPEG_memory gdcm_read_JPEG_memory8
//#define StateSuspension StateSuspension8
#define SampBuffer SampBuffer8
//#define pimage pimage8

#include "gdcmJpeg.cxx"

