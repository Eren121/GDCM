/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJpeg2000.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/04 15:20:13 $
  Version:   $Revision: 1.33 $
                                                                                
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

extern "C" {
  #include <openjpeg.h>
}

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
   j2k_image_t img;
   j2k_cp_t cp;
 
   // default blindly copied
   cp.layer=0;
   cp.reduce=0;
   cp.decod_format=-1;
   cp.cod_format=-1;
 
   cp.cod_format=J2K_CFMT;
   cp.decod_format = PGX_DFMT;
   int len = inputlength;
   unsigned char *src = (unsigned char*)inputdata;
 
   // Decompression
   if (!j2k_decode(src, len, &img, &cp))
   {
      gdcmErrorMacro( "ERROR -> j2k_to_image: failed to decode image!" );
      return false;
   }
 
   // Copy buffer
   for (int compno = 0; compno < img.numcomps; compno++)
   {
      j2k_comp_t *comp = &img.comps[compno];
  
      int w = img.comps[compno].w;
      int wr = int_ceildivpow2(img.comps[compno].w, img.comps[compno].factor);
  
      //int h = img.comps[compno].h;
      int hr = int_ceildivpow2(img.comps[compno].h, img.comps[compno].factor);
  
      if (comp->prec <= 8)
      {
         uint8_t *data8 = (uint8_t*)raw;
         for (int i = 0; i < wr * hr; i++) 
         {
            int v = img.comps[compno].data[i / wr * w + i % wr];
            *data8++ = (uint8_t)v;
         }
      }
      else if (comp->prec <= 16)
      {
         uint16_t *data16 = (uint16_t*)raw;
         for (int i = 0; i < wr * hr; i++) 
         {
            int v = img.comps[compno].data[i / wr * w + i % wr];
            *data16++ = (uint16_t)v;
         }
      }
      else
      {
         uint32_t *data32 = (uint32_t*)raw;
         for (int i = 0; i < wr * hr; i++) 
         {
            int v = img.comps[compno].data[i / wr * w + i % wr];
            *data32++ = (uint32_t)v;
         }
      }
      free(img.comps[compno].data);
   }
 
   // Free remaining structures
   j2k_dec_release();
   // FIXME
   delete[] inputdata;
 
   return true;
}

#if 0
bool gdcm_read_JASPER_file (void* raw, char *inputdata, size_t inputlength)
{
#if 0
  std::cerr << "Inputlenght=" << inputlength << std::endl;
  std::ofstream out("/tmp/jpeg2000.jpc", std::ios::binary);
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
  jas_image_t *jasImage /* = NULL*/; // Useless assignation
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
#endif

//-----------------------------------------------------------------------------
} // end namespace gdcm

