/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2004/10/10 16:44:00 $
  Version:   $Revision: 1.59 $
                                                                                
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

//-----------------------------------------------------------------------------
/*
 * In addition to Dicom header exploration, this class is designed
 * for accessing the image/volume content. One can also use it to
 * write Dicom/ACR-NEMA/RAW files.
 */
class GDCM_EXPORT gdcmFile
{
public:
   gdcmFile( gdcmHeader *header );
   gdcmFile( std::string const& filename );
 
   virtual ~gdcmFile();

   /// Accessor to \ref Header
   gdcmHeader* GetHeader() { return Header; }

   int ComputeDecompressedPixelDataSizeFromHeader();

   void ConvertRGBPlanesToRGBPixels( uint8_t* source, uint8_t* destination );
   void ConvertYcBcRPlanesToRGBPixels( uint8_t* source, uint8_t* destination );
   
   /// Accessor to \ref ImageDataSize
   size_t GetImageDataSize(){ return ImageDataSize; };

   /// Accessor to \ref ImageDataSizeRaw
   size_t GetImageDataSizeRaw(){ return ImageDataSizeRaw; };

   uint8_t* GetImageData();
   size_t GetImageDataIntoVector(void* destination, size_t maxSize);
   uint8_t* GetImageDataRaw();
   size_t GetImageDataIntoVectorRaw(void* destination, size_t maxSize);

   // see also gdcmHeader::SetImageDataSize ?!?         
   bool SetImageData (uint8_t* data, size_t expectedSize);

   /// \todo When the caller is aware we simply point to the data:
   /// int SetImageDataNoCopy (void* Data, size_t ExpectedSize);

   // Write pixels of ONE image on hard drive
   // No test is made on processor "endianity"
   // The user must call his reader correctly
   bool WriteRawData  (std::string const& fileName);
   bool WriteDcmImplVR(std::string const& fileName);
   bool WriteDcmExplVR(std::string const& fileName);
   bool WriteAcr      (std::string const& fileName);

   // Don't look any longer for the code : 
   // It's in file gdcmParsePixels.cxx
   bool ParsePixelData();

   virtual bool SetEntryByNumber(std::string const& content,
                                 uint16_t group, uint16_t element)
   { 
      Header->SetEntryByNumber(content,group,element);
      return true;
   }
     
protected:
   bool WriteBase(std::string const& fileName, FileType type);

private:
   void Initialise();

   bool ReadPixelData(void* destination);
   
   // For JPEG 8 Bits, body in file gdcmJpeg.cxx
   bool gdcm_write_JPEG_file    (FILE* fp, void* image_buffer, 
                                 int image_width, int image_heigh,
                                 int quality);

   // For JPEG 12 Bits, body in file gdcmJpeg12.cxx
   bool gdcm_write_JPEG_file12  (FILE* fp, void* image_buffer, 
                                 int image_width, int image_height,
                                 int quality);

   void SaveInitialValues();    // will belong to the future gdcmPixelData class
   void RestoreInitialValues(); // will belong to the future gdcmPixelData class
   void DeleteInitialValues();  // will belong to the future gdcmPixelData class 

// members variables:

   /// \brief Header to use to load the file
   gdcmHeader *Header;

   /// \brief Whether the underlying \ref gdcmHeader was loaded by
   ///  the constructor or passed to the constructor. When false
   ///  the destructor is in charge of deletion.
   bool SelfHeader;
   
   /// wether already parsed 
   bool Parsed;
      
//
// --------------- Will be moved to a gdcmPixelData class
//

   /// \brief to hold the Pixels (when read)
   uint8_t* Pixel_Data;  // (was PixelData)
   
   /// \brief Size (in bytes) of required memory to hold the Gray Level pixels
   ///        represented in this file. This is used when the user DOESN'T want
   ///        the RGB pixels image when it's stored as a PALETTE COLOR image
   size_t ImageDataSizeRaw;
   
   /// \brief Size (in bytes) of requited memory to hold the the pixels
   ///        of this image in it's RGB convertion either from:
   ///        - Plane R, Plane G, Plane B 
   ///        - Grey Plane + Palette Color
   ///        - YBR Pixels (or from RGB Pixels, as well) 
   size_t ImageDataSize;
       
  /// \brief ==1  if GetImageDataRaw was used
  ///        ==0  if GetImageData    was used
  ///        ==-1 if ImageData never read                       
   int PixelRead;

  /// \brief length of the last allocated area devoided to receive Pixels
  ///        ( to allow us not to (free + new) if un necessary )     
   size_t LastAllocatedPixelDataLength; 

  // Initial values of some fields that can be modified during reading process
  // if user asked to transform gray level + LUT image into RGB image
     
  /// \brief Samples Per Pixel           (0x0028,0x0002), as found on disk
   std::string InitialSpp;
  /// \brief Photometric Interpretation  (0x0028,0x0004), as found on disk
   std::string InitialPhotInt;
  /// \brief Planar Configuration        (0x0028,0x0006), as found on disk   
   std::string InitialPlanConfig;
    
  // Initial values of some fields that can be modified during reading process
  // if the image was a 'strange' ACR-NEMA 
  // (Bits Allocated=12, High Bit not equal to Bits stored +1) 
  /// \brief Bits Allocated              (0x0028,0x0100), as found on disk
   std::string InitialBitsAllocated;
  /// \brief High Bit                    (0x0028,0x0102), as found on disk
   std::string InitialHighBit;
  
  // some DocEntry that can be moved out of the H table during reading process
  // if user asked to transform gray level + LUT image into RGB image
  // We keep a pointer on them for a future use.
     
  /// \brief Red Palette Color Lookup Table Descriptor   0028 1101 as read
  gdcmDocEntry* InitialRedLUTDescr;  
  /// \brief Green Palette Color Lookup Table Descriptor 0028 1102 as read
  gdcmDocEntry* InitialGreenLUTDescr;
  /// \brief Blue Palette Color Lookup Table Descriptor  0028 1103 as read
  gdcmDocEntry* InitialBlueLUTDescr;
  
  /// \brief Red Palette Color Lookup Table Data         0028 1201 as read
  gdcmDocEntry* InitialRedLUTData;  
  /// \brief Green Palette Color Lookup Table Data       0028 1202 as read
  gdcmDocEntry* InitialGreenLUTData;
  /// \brief Blue Palette Color Lookup Table Data        0028 1203 as read
  gdcmDocEntry* InitialBlueLUTData;
  
//
// --------------- end of future gdcmPixelData class
//  

};

//-----------------------------------------------------------------------------
#endif
