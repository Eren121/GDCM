/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeader.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.74 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include "gdcmCommon.h"
#include "gdcmDocument.h"
//-----------------------------------------------------------------------------
/**
 * \brief
 * The purpose of an instance of gdcmHeader is to act as a container of
 * all the DICOM elements and their corresponding values (and
 * additionaly the corresponding DICOM dictionary entry) of the header
 * of a DICOM file.
 *
 * The typical usage of instances of class gdcmHeader is to classify a set of
 * dicom files according to header information e.g. to create a file hierarchy
 * reflecting the Patient/Study/Serie informations, or extracting a given
 * SerieId. Accessing the content (image[s] or volume[s]) is beyond the
 * functionality of this class and belongs to gdmcFile.
 * \note  The various entries of the explicit value representation (VR) shall
 *        be managed within a dictionary which is shared by all gdcmHeader
 *        instances.
 * \note  The gdcmHeader::Set*Tag* family members cannot be defined as
 *        protected due to Swig limitations for as Has_a dependency between
 *        gdcmFile and gdcmHeader.
 */
class GDCM_EXPORT gdcmHeader : public gdcmDocument
{
protected:
   /// \brief In some cases (e.g. for some ACR-NEMA images) the Header Entry Element
   /// Number of the 'Pixel Element' is *not* found at 0x0010. In order to
   /// make things easier the parser shall store the proper value in
   /// NumPixel to provide a unique access facility. See also the constructor
   /// \ref gdcmHeader::gdcmHeader
   guint16 NumPixel;
   /// \brief In some cases (e.g. for some ACR-NEMA images) the header entry for
   /// the group of pixels is *not* found at 0x7fe0. In order to
   /// make things easier the parser shall store the proper value in
   /// GrPixel to provide a unique access facility. See also the constructor
   /// \ref gdcmHeader::gdcmHeader
   guint16 GrPixel;

public:
   gdcmHeader(bool exception_on_error = false);
   gdcmHeader(const char *filename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false,
              bool  skip_shadow        = false);
 
   virtual ~gdcmHeader();

   // Standard values and informations contained in the header
   virtual bool IsReadable(void);

   // Some heuristic based accessors, end user intended 
   // (to be moved to gdcmHeaderHelper?) 
   int GetXSize(void);
   int GetYSize(void);
   int GetZSize(void);
   int GetBitsStored(void);
   int GetBitsAllocated(void);
   int GetSamplesPerPixel(void);   
   int GetPlanarConfiguration(void);

   int GetPixelSize(void);   
   std::string GetPixelType(void);  
   size_t GetPixelOffset(void);
   size_t GetPixelAreaLength(void);

   bool   HasLUT(void);
   int    GetLUTNbits(void);
   unsigned char * GetLUTRGBA(void);

   std::string GetTransfertSyntaxName(void);

   /// Accessor to \ref gdcmHeader::GrPixel
   guint16 GetGrPixel(void)  {return GrPixel;}
   
   /// Accessor to \ref gdcmHeader::NumPixel
   guint16 GetNumPixel(void) {return NumPixel;}

   /// Read (used in gdcmFile)
   void SetImageDataSize(size_t ExpectedSize);

protected:
   bool anonymizeHeader(void);
private:

};

//-----------------------------------------------------------------------------
#endif
