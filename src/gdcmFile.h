/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2004/12/03 20:16:58 $
  Version:   $Revision: 1.83 $
                                                                                
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

namespace gdcm 
{
class Header;
class ValEntry;
class BinEntry;
class PixelReadConvert;
class PixelWriteConvert;
class DocEntryArchive;
//-----------------------------------------------------------------------------
/*
 * In addition to Dicom header exploration, this class is designed
 * for accessing the image/volume content. One can also use it to
 * write Dicom/ACR-NEMA/RAW files.
 */
class GDCM_EXPORT File
{
public:
   enum WriteMode
   {
      WMODE_DECOMPRESSED,
      WMODE_RGB
   };
     
public:
   File( Header* header );
   File( std::string const& filename );
 
   virtual ~File();

   /// Accessor to \ref Header
   Header* GetHeader() { return HeaderInternal; }

   size_t GetImageDataSize();
   size_t GetImageDataRawSize();

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

   bool SetEntryByNumber(std::string const& content,
                                uint16_t group, uint16_t element);
   uint8_t* GetLutRGBA();

   // Write mode
   void SetWriteModeToDecompressed() { SetWriteMode(WMODE_DECOMPRESSED); };
   void SetWriteModeToRGB()          { SetWriteMode(WMODE_RGB); };
   void SetWriteMode(WriteMode mode) { WriteMode = mode; };
   WriteMode GetWriteMode()          { return WriteMode; };

   // Write format
   void SetWriteTypeToDcmImplVR()     { SetWriteType(ImplicitVR); };
   void SetWriteTypeToDcmExplVR()     { SetWriteType(ExplicitVR); };
   void SetWriteTypeToAcr()           { SetWriteType(ACR); };
   void SetWriteTypeToAcrLibido()     { SetWriteType(ACR_LIBIDO); };
   void SetWriteType(FileType format) { WriteType = format; };
   FileType GetWriteType()            { return WriteType; };

protected:
   bool WriteBase(std::string const& fileName);
   bool CheckWriteIntegrity();

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
   PixelReadConvert* PixelReadConverter;
   PixelWriteConvert* PixelWriteConverter;

   // Utility header archive
   DocEntryArchive *Archive;

   // Write variables
   WriteMode WriteMode;
   FileType WriteType;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
