/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2004/09/24 11:39:21 $
  Version:   $Revision: 1.51 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
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

   void   SetPixelDataSizeFromHeader();
   

/// \brief     Returns the size (in bytes) of required memory to hold
///            the pixel data represented in this file.
   size_t GetImageDataSize(){ return ImageDataSize; };

   
/// \brief     Returns the size (in bytes) of required memory to hold
///            the pixel data represented in this file, if user DOESN'T want 
///            to get RGB pixels image when it's stored as a PALETTE COLOR image
///            -the (vtk) user is supposed to know how to deal with LUTs-     
   size_t GetImageDataSizeRaw(){ return ImageDataSizeRaw; };

   uint8_t* GetImageData();
   size_t GetImageDataIntoVector(void* destination, size_t maxSize);
   uint8_t* GetImageDataRaw();
   size_t GetImageDataIntoVectorRaw(void* destination, size_t maxSize);

   // Allocates ExpectedSize bytes of memory at this->Data and copies the
   // pointed data to it. Copying the image might look useless but
   // the caller might destroy it's image (without knowing it: think
   // of a complicated interface where display is done with a library
   // e.g. VTK) before calling the Write
      
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
   void SwapZone(void* im, int swap, int lgr, int nb);

   bool ReadPixelData(void* destination);
   
   // For JPEG 8 Bits, body in file gdcmJpeg.cxx
   bool gdcm_read_JPEG_file     (FILE* fp, void* image_buffer); 
   bool gdcm_write_JPEG_file    (FILE* fp, void* image_buffer, 
                                 int image_width, int image_heigh,
                                 int quality);

   // For JPEG 12 Bits, body in file gdcmJpeg12.cxx
   bool gdcm_read_JPEG_file12   (FILE* fp, void* image_buffer);
   bool gdcm_write_JPEG_file12  (FILE* fp, void* image_buffer, 
                                 int image_width, int image_height,
                                 int quality);

   // For JPEG 2000, body in file gdcmJpeg2000.cxx
   bool gdcm_read_JPEG2000_file (FILE* fp, void* image_buffer);

   // For Run Length Encoding
   bool gdcm_read_RLE_file      (FILE* fp, void* image_buffer);
// FIXME : *sure* it's NOT static (C++)
// (would be static in C, or embedded in ADA)
// It's NOT a method, but a not user intended fonction.
// How do we write that in C++ ?)
   static int gdcm_read_RLE_fragment(char **areaToRead, long lengthToDecode, 
                                     long uncompressedSegmentSize, FILE* fp);

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
   
   /// \brief Area length to receive the Gray Level pixels
   size_t ImageDataSizeRaw;
   
   /// \brief Area length to receive the pixels making RGB
   ///        from Plane R, Plane G, Plane B 
   ///     or from Grey Plane + Palette Color
   ///     or from YBR Pixels (or from RGB Pixels, as well) 
   size_t ImageDataSize;
       
  /// \brief ==1  if GetImageDataRaw was used
  ///        ==0  if GetImageData    was used
  ///        ==-1 if ImageData never read                       
   int PixelRead;

  /// \brief length of the last allocated area devoided to receive Pixels
  ///        ( to allow us not to (free + new) if un necessary )     
   size_t LastAllocatedPixelDataLength; 

  // Initial values of some fields that can be modified during reading process
  // (in a future stage, they will be modified just before the writting process
  //  and restored just after)
  // if user asked to transform gray level + LUT image into RGB image
     
  /// \brief Samples Per Pixel           (0x0028,0x0002), as found on disk
   std::string InitialSpp;
  /// \brief Photometric Interpretation  (0x0028,0x0004), as found on disk
   std::string InitialPhotInt;
  /// \brief Planar Configuration        (0x0028,0x0006), as found on disk   
   std::string InitialPlanConfig;
    
  // Initial values of some fields that can be modified during reading process
  // (in a future stage, they will be modified just before the writting process
  //  and restored just after)
  // if the image was a 'strange' ACR-NEMA 
  // (Bits Allocated=12, High Bit not equal to Bits stored +1) 
  /// \brief Bits Allocated              (0x0028,0x0100), as found on disk
   std::string InitialBitsAllocated;
  /// \brief High Bit                    (0x0028,0x0102), as found on disk
   std::string InitialHighBit;
  
  // some DocEntry that can be moved out of the H table during reading process
  // (in a future stage, they will be modified just before the writting process
  //  and restored just after)
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
