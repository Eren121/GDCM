/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.h,v $
  Language:  C++
  Date:      $Date: 2004/06/26 04:09:33 $
  Version:   $Revision: 1.36 $
                                                                                
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
   gdcmFile(gdcmHeader *header);
   gdcmFile(std::string const & filename, 
            bool  exception_on_error = false, 
            bool  enable_sequences   = false,
            bool  skip_shadow        = false);
 
   virtual ~gdcmFile();

   /// \brief returns the gdcmHeader *Header
   inline gdcmHeader *GetHeader() { return Header; }

   // For promotion (performs a deepcopy of pointed header object)
   // TODO Swig gdcmFile(gdcmHeader* header);
   // TODO Swig ~gdcmFile();

   // On writing purposes. When instance was created through
   // gdcmFile(std::string filename) then the filename argument MUST be
        // different from the constructor's one (no overwriting allowed).
   // TODO Swig int SetFileName(std::string filename);

   void   SetPixelDataSizeFromHeader();
   size_t GetImageDataSize();
   size_t GetImageDataSizeRaw();

   void * GetImageData();
   size_t GetImageDataIntoVector(void* destination, size_t MaxSize);
   void * GetImageDataRaw();
   size_t GetImageDataIntoVectorRaw(void* destination, size_t MaxSize);

      // Allocates ExpectedSize bytes of memory at this->Data and copies the
      // pointed data to it. Copying the image might look useless but
      // the caller might destroy it's image (without knowing it: think
      // of a complicated interface where display is done with a library
      // e.g. VTK) before calling the Write
      
   // voir gdcmHeader::SetImageDataSize ?!?         
   bool SetImageData     (void * Data, size_t ExpectedSize);
      // When the caller is aware we simply point to the data:
      // TODO int SetImageDataNoCopy (void * Data, size_t ExpectedSize);

  // Push to disk.
  // TODO Swig int Write();

   // Write pixels of ONE image on hard drive
   // No test is made on processor "endianity"
   // The user must call his reader correctly
   bool WriteRawData  (std::string const & fileName);
   bool WriteDcmImplVR(std::string const & fileName);
   bool WriteDcmExplVR(std::string const & fileName);
   bool WriteAcr      (std::string const & fileName);

   // Don't look any longer for the code : 
   // It's in file gdcmParsePixels.cxx
   bool ParsePixelData();

   inline virtual bool SetEntryByNumber(std::string const & content,
                                        guint16 group, guint16 element)
   { 
      GetHeader()->SetEntryByNumber(content,group,element);
      return true;  //default behavior ?
   }
     
protected:
   bool WriteBase(std::string const & FileName, FileType type);

private:
   void SwapZone(void* im, int swap, int lgr, int nb);
   
   bool ReadPixelData(void * destination);
   
   // For JPEG 8 Bits, body in file gdcmJpeg.cxx
   bool gdcm_read_JPEG_file     (FILE *fp,void * image_buffer); 
   static int gdcm_read_RLE_fragment(char **areaToRead, long lengthToDecode, 
                                     long uncompressedSegmentSize,FILE *fp);
   // For JPEG 12 Bits, body in file gdcmJpeg12.cxx
   bool gdcm_read_JPEG_file12   (FILE *fp,void * image_buffer);
   // For JPEG 2000, body in file gdcmJpeg2000.cxx
   bool gdcm_read_JPEG2000_file (FILE *fp,void * image_buffer);

   // For Run Length Encoding (TOCHECK)
   bool gdcm_read_RLE_file      (FILE *fp,void * image_buffer); 

// Variables

   /// \brief Header to use to load the file
   gdcmHeader *Header;

   /// \brief Whether the underlying \ref gdcmHeader was loaded by
   ///  the constructor or passed to the constructor. When false
   ///  the destructor is in charge of deletion.
   bool SelfHeader;

   /// \brief to hold the Pixels (when read)
   void* PixelData;
   
   /// \brief Area length to receive the pixels
   size_t ImageDataSizeRaw;
   
   /// \brief Area length to receive the RGB pixels
   /// from Grey Plane + Palette Color  
   size_t ImageDataSize;
       
  /// \brief ==1  if GetImageDataRaw was used
  ///        ==0  if GetImageData    was used
  ///        ==-1 if ImageData never read                       
   int PixelRead;     

   /// wether already parsed 
   int Parsed;
};

//-----------------------------------------------------------------------------
#endif
