/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2004/11/30 14:17:52 $
  Version:   $Revision: 1.79 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMFILE_H
#define GDCMFILE_H

#include "gdcmCommon.h"
#include "gdcmHeader.h"
#include "gdcmPixelConvert.h"
#include "gdcmDocEntryArchive.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
/*
 * In addition to Dicom header exploration, this class is designed
 * for accessing the image/volume content. One can also use it to
 * write Dicom/ACR-NEMA/RAW files.
 */
class GDCM_EXPORT File
{
public:
   enum TWriteMode
   {
      WMODE_NATIVE,
      WMODE_DECOMPRESSED,
      WMODE_RGB
   };
     
   enum TWriteType
   {
      WTYPE_IMPL_VR,
      WTYPE_EXPL_VR,
      WTYPE_ACR
   };
     
public:
   File( Header* header );
   File( std::string const& filename );
 
   virtual ~File();

   /// Accessor to \ref Header
   Header* GetHeader() { return HeaderInternal; }

   size_t GetImageDataSize();
   size_t GetImageDataRawSize();

   /// Accessor to \ref PixelConverter
   PixelConvert* GetPixelConverter() { return PixelConverter; };

   uint8_t* GetImageData();
   uint8_t* GetImageDataRaw();
   size_t GetImageDataIntoVector(void* destination, size_t maxSize);

   bool SetImageData (uint8_t* data, size_t expectedSize);

   // Write pixels of ONE image on hard drive
   // No test is made on processor "endianity"
   // The user must call his reader correctly
   bool WriteRawData  (std::string const& fileName);
   bool WriteDcmImplVR(std::string const& fileName);
   bool WriteDcmExplVR(std::string const& fileName);
   bool WriteAcr      (std::string const& fileName);
   bool Write(std::string const& fileName);

   virtual bool SetEntryByNumber(std::string const& content,
                                 uint16_t group, uint16_t element)
   { 
      HeaderInternal->SetEntryByNumber(content,group,element);
      return true;
   }
   uint8_t* GetLutRGBA();

   // Write mode
   void SetWriteModeToNative()          { SetWriteMode(WMODE_NATIVE); };
   void SetWriteModeToDecompressed()    { SetWriteMode(WMODE_DECOMPRESSED); };
   void SetWriteModeToRGB()             { SetWriteMode(WMODE_RGB); };
   void SetWriteMode(unsigned int mode) { WriteMode = mode; };
   unsigned int GetWriteMode()          { return WriteMode; };

   // Write format
   void SetWriteTypeToDcmImplVR()         { SetWriteType(WTYPE_EXPL_VR); };
   void SetWriteTypeToDcmExplVR()         { SetWriteType(WTYPE_EXPL_VR); };
   void SetWriteTypeToAcr()               { SetWriteType(WTYPE_ACR); };
   void SetWriteType(unsigned int format) { WriteType = format; };
   unsigned int GetWriteType()            { return WriteType; };

protected:
   bool WriteBase(std::string const& fileName, FileType type);
   bool CheckWriteIntegrity();

   void SetWriteToNative();
   void SetWriteToDecompressed();
   void SetWriteToRGB();
   void RestoreWrite();

   void SetWriteFileTypeToACR();
   void SetWriteFileTypeToExplicitVR();
   void SetWriteFileTypeToImplicitVR();
   void RestoreWriteFileType();

   void SetWriteToLibido();
   void SetWriteToNoLibido();
   void RestoreWriteOfLibido();

   ValEntry* CopyValEntry(uint16_t group,uint16_t element);
   BinEntry* CopyBinEntry(uint16_t group,uint16_t element);

private:
   void Initialise();

   uint8_t* GetDecompressed();
   int ComputeDecompressedPixelDataSizeFromHeader();

private:
// members variables:

   /// Header to use to load the file
   Header *HeaderInternal;

   /// \brief Whether the underlying \ref Header was loaded by
   ///  the constructor or passed to the constructor. When false
   ///  the destructor is in charge of deletion.
   bool SelfHeader;
   
   /// Wether already parsed or not
   bool Parsed;

   /// Utility pixel converter
   PixelConvert* PixelConverter;

   // Utility header archive
   DocEntryArchive *Archive;

   // Write variables
   unsigned int WriteMode;
   unsigned int WriteType;

/// FIXME
// --------------- Will be moved to a PixelData class
//

   /// \brief to hold the Pixels (when read)
   uint8_t* Pixel_Data;  // (was PixelData)
   
   /// \brief Size (in bytes) of requited memory to hold the the pixels
   ///        of this image in it's RGB convertion either from:
   ///        - Plane R, Plane G, Plane B 
   ///        - Grey Plane + Palette Color
   ///        - YBR Pixels (or from RGB Pixels, as well) 
   size_t ImageDataSize;
       
//
// --------------- end of future PixelData class
// 

};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
