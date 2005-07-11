/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg2000.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/11 14:37:53 $
  Version:   $Revision: 1.26 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"

#include <iostream>
#include <fstream>
#include <jasper/jasper.h>

namespace gdcm 
{
//-----------------------------------------------------------------------------
 /**
 * \brief   routine for JPEG decompression 
 * @param raw raw
 * @param inputdata inputdata
 * @param inputlength inputlength 
 * @return 1 on success, 0 on error
 */

bool gdcm_read_JPEG2000_file (void* raw, char *inputdata, size_t inputlength)
{
#if 0
  std::ofstream out("/tmp/jpeg2000.jpc");
  out.write((char*)inputdata,inputlength);
  out.close();
#endif
  jas_init(); //important...
  jas_stream_t *jasStream = 
    jas_stream_memopen((char *)inputdata, inputlength);
    
  int fmtid;
  if ((fmtid = jas_image_getfmt(jasStream)) < 0) 
    {
    gdcmErrorMacro("unknown image format");
    return false;
    }

  // Decode the image. 
  jas_image_t *jasImage = NULL;
  if (!(jasImage = jas_image_decode(jasStream, fmtid, 0))) 
    {
    gdcmErrorMacro("cannot decode image");
    return false;
    }

  // close the stream. 
  jas_stream_close(jasStream);
  int numcmpts = jas_image_numcmpts(jasImage);
  int width = jas_image_cmptwidth(jasImage, 0);
  int height = jas_image_cmptheight(jasImage, 0);
  int prec = jas_image_cmptprec(jasImage, 0);
  int i, j, k;

  // The following should serioulsy be rewritten I cannot believe we need to
  // do a per pixel decompression, there should be a way to read a full
  // scanline...
  if (prec == 8)
    {
    uint8_t *data8 = (uint8_t*)raw;
    for ( i = 0; i < height; i++)
      for ( j = 0; j < width; j++)
        for ( k= 0; k < numcmpts; k++)
          *data8++ = (uint8_t)(jas_image_readcmptsample(jasImage, k, j ,i ));
    }
  else if (prec <= 16)
    {
    uint16_t *data16 = (uint16_t*)raw;
    for ( i = 0; i < height; i++) 
      for ( j = 0; j < width; j++) 
        for ( k= 0; k < numcmpts; k++)
          *data16++ = (uint16_t)(jas_image_readcmptsample(jasImage, k, j ,i ));
    }
  else if (prec <= 32)
    {
    uint32_t *data32 = (uint32_t*)raw;
    for ( i = 0; i < height; i++) 
      for ( j = 0; j < width; j++) 
        for ( k= 0; k < numcmpts; k++)
          *data32++ = (uint32_t)(jas_image_readcmptsample(jasImage, k, j ,i ));
    }

  jas_image_destroy(jasImage);
  jas_image_clearfmts();

  //FIXME
  //delete the jpeg temp buffer
#if 0
  std::ofstream rawout("/tmp/jpeg2000.raw");
  rawout.write((char*)raw,height*width*numcmpts*((prec+4)/8));
  rawout.close();
#endif
  delete[] inputdata;

  return true;
}

//-----------------------------------------------------------------------------
} // end namespace gdcm

