/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFileHelper.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/28 16:34:55 $
  Version:   $Revision: 1.21 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFileHelper.h"
#include "gdcmGlobal.h"
#include "gdcmTS.h"
#include "gdcmDocument.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmBinEntry.h"
#include "gdcmValEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmContentEntry.h"
#include "gdcmFile.h"
#include "gdcmPixelReadConvert.h"
#include "gdcmPixelWriteConvert.h"
#include "gdcmDocEntryArchive.h"
#include "gdcmDictSet.h"

#include <fstream>

namespace gdcm 
{
//-------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (gdcm::File only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        one sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 */
FileHelper::FileHelper( )
{
   FileInternal = new File( );
   SelfHeader = true;
   Initialize();
}

/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (File only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        user sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 * @param header already built Header
 */
FileHelper::FileHelper(File *header)
{
   FileInternal = header;
   SelfHeader = false;
   Initialize();
}

/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (gdcm::File only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        one sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 * @param filename file to be opened for parsing
 */
FileHelper::FileHelper(std::string const &filename )
{
   FileInternal = new File( filename );
   SelfHeader = true;
   Initialize();
}

/**
 * \brief canonical destructor
 * \note  If the header (gdcm::File) was created by the FileHelper constructor,
 *        it is destroyed by the FileHelper
 */
FileHelper::~FileHelper()
{ 
   if( PixelReadConverter )
   {
      delete PixelReadConverter;
   }
   if( PixelWriteConverter )
   {
      delete PixelWriteConverter;
   }
   if( Archive )
   {
      delete Archive;
   }

   if( SelfHeader )
   {
      delete FileInternal;
   }
   FileInternal = 0;
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (string) to substitute with
 * @param   group  group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 * \return  false if DocEntry not found
 */
bool FileHelper::SetValEntry(std::string const &content,
                             uint16_t group, uint16_t elem)
{ 
   return FileInternal->SetValEntry(content, group, elem);
}


/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (void*  -> uint8_t*) to substitute with
 * @param   lgth new value length
 * @param   group  group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 * \return  false if DocEntry not found
 */
bool FileHelper::SetBinEntry(uint8_t *content, int lgth,
                             uint16_t group, uint16_t elem)
{
   return FileInternal->SetBinEntry(content, lgth, group, elem);
}

/**
 * \brief   Modifies the value of a given DocEntry (Dicom entry)
 *          when it exists. Creates it with the given value when unexistant.
 * @param   content (string)value to be set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Dicom entry (NULL when creation
 *          failed).
 */ 
ValEntry *FileHelper::InsertValEntry(std::string const &content,
                                     uint16_t group, uint16_t elem)
{
   return FileInternal->InsertValEntry(content,group,elem);
}

/**
 * \brief   Modifies the value of a given DocEntry (Dicom entry)
 *          when it exists. Creates it with the given value when unexistant.
 *          A copy of the binArea is made to be kept in the Document.
 * @param   binArea (binary)value to be set
 * @param   lgth new value length
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Dicom entry (NULL when creation
 *          failed).
 */
BinEntry *FileHelper::InsertBinEntry(uint8_t *binArea, int lgth,
                                     uint16_t group, uint16_t elem)
{
   return FileInternal->InsertBinEntry(binArea, lgth, group, elem);
}

/**
 * \brief   Modifies the value of a given DocEntry (Dicom entry)
 *          when it exists. Creates it, empty (?!) when unexistant.
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Dicom entry (NULL when creation
 *          failed).
 */
SeqEntry *FileHelper::InsertSeqEntry(uint16_t group, uint16_t elem)
{
   return FileInternal->InsertSeqEntry(group, elem);
}

/**
 * \brief   Get the size of the image data
 *          If the image can be RGB (with a lut or by default), the size 
 *          corresponds to the RGB image
 *         (use GetImageDataRawSize if you want to be sure to get *only*
 *          the size of the pixels)
 * @return  The image size
 */
size_t FileHelper::GetImageDataSize()
{
   if ( PixelWriteConverter->GetUserData() )
   {
      return PixelWriteConverter->GetUserDataSize();
   }

   return PixelReadConverter->GetRGBSize();
}

/**
 * \brief   Get the size of the image data
 *          If the image could be converted to RGB using a LUT, 
 *          this transformation is not taken into account by GetImageDataRawSize
 *          (use GetImageDataSize if you wish)
 * @return  The raw image size
 */
size_t FileHelper::GetImageDataRawSize()
{
   if ( PixelWriteConverter->GetUserData() )
   {
      return PixelWriteConverter->GetUserDataSize();
   }

   return PixelReadConverter->GetRawSize();
}

/**
 * \brief   - Allocates necessary memory,
 *          - Reads the pixels from disk (uncompress if necessary),
 *          - Transforms YBR pixels, if any, into RGB pixels,
 *          - Transforms 3 planes R, G, B, if any, into a single RGB Plane
 *          - Transforms single Grey plane + 3 Palettes into a RGB Plane
 *          - Copies the pixel data (image[s]/volume[s]) to newly allocated zone.
 * @return  Pointer to newly allocated pixel data.
 *          NULL if alloc fails 
 */
uint8_t *FileHelper::GetImageData()
{
   if ( PixelWriteConverter->GetUserData() )
   {
      return PixelWriteConverter->GetUserData();
   }

   if ( ! GetRaw() )
   {
      // If the decompression failed nothing can be done.
      return 0;
   }

   if ( FileInternal->HasLUT() && PixelReadConverter->BuildRGBImage() )
   {
      return PixelReadConverter->GetRGB();
   }
   else
   {
      // When no LUT or LUT conversion fails, return the Raw
      return PixelReadConverter->GetRaw();
   }
}

/**
 * \brief   Allocates necessary memory, 
 *          Transforms YBR pixels (if any) into RGB pixels
 *          Transforms 3 planes R, G, B  (if any) into a single RGB Plane
 *          Copies the pixel data (image[s]/volume[s]) to newly allocated zone. 
 *          DOES NOT transform Grey plane + 3 Palettes into a RGB Plane
 * @return  Pointer to newly allocated pixel data.
 *          NULL if alloc fails 
 */
uint8_t *FileHelper::GetImageDataRaw ()
{
   return GetRaw();
}

/**
 * \brief
 *          Read the pixels from disk (uncompress if necessary),
 *          Transforms YBR pixels, if any, into RGB pixels
 *          Transforms 3 planes R, G, B, if any, into a single RGB Plane
 *          Transforms single Grey plane + 3 Palettes into a RGB Plane   
 *          Copies at most MaxSize bytes of pixel data to caller allocated
 *          memory space.
 * \warning This function allows people that want to build a volume
 *          from an image stack *not to* have, first to get the image pixels, 
 *          and then move them to the volume area.
 *          It's absolutely useless for any VTK user since vtk chooses 
 *          to invert the lines of an image, that is the last line comes first
 *          (for some axis related reasons?). Hence he will have 
 *          to load the image line by line, starting from the end.
 *          VTK users have to call GetImageData
 *     
 * @param   destination Address (in caller's memory space) at which the
 *          pixel data should be copied
 * @param   maxSize Maximum number of bytes to be copied. When MaxSize
 *          is not sufficient to hold the pixel data the copy is not
 *          executed (i.e. no partial copy).
 * @return  On success, the number of bytes actually copied. Zero on
 *          failure e.g. MaxSize is lower than necessary.
 */
size_t FileHelper::GetImageDataIntoVector (void *destination, size_t maxSize)
{
   if ( ! GetRaw() )
   {
      // If the decompression failed nothing can be done.
      return 0;
   }

   if ( FileInternal->HasLUT() && PixelReadConverter->BuildRGBImage() )
   {
      if ( PixelReadConverter->GetRGBSize() > maxSize )
      {
         gdcmWarningMacro( "Pixel data bigger than caller's expected MaxSize");
         return 0;
      }
      memcpy( destination,
              (void*)PixelReadConverter->GetRGB(),
              PixelReadConverter->GetRGBSize() );
      return PixelReadConverter->GetRGBSize();
   }

   // Either no LUT conversion necessary or LUT conversion failed
   if ( PixelReadConverter->GetRawSize() > maxSize )
   {
      gdcmWarningMacro( "Pixel data bigger than caller's expected MaxSize");
      return 0;
   }
   memcpy( destination,
           (void*)PixelReadConverter->GetRaw(),
           PixelReadConverter->GetRawSize() );
   return PixelReadConverter->GetRawSize();
}

/**
 * \brief   Set the image data defined by the user, BUT WITHOUT COPYING THE DATA.
 * \warning Since the pixels are not copied, it is the caller's responsability
 *          not to deallocate its data before gdcm uses them (e.g. with
 *          the Write() method )
 * @param inData user supplied pixel area (uint8_t* is just for the compiler.
 *               user is allowed to pass any kind of pixelsn since the size is
 *               given in bytes) 
 * @param expectedSize total image size, *in Bytes*
 */
void FileHelper::SetImageData(uint8_t *inData, size_t expectedSize)
{
   SetUserData(inData, expectedSize);
}

/**
 * \brief   Set the image data defined by the user, BUT WITHOUT COPYING THE DATA.
 * \warning When writting the file, this data are get as default data to write
 * \warning Since the pixels are not copied, it is the caller's responsability
 *          not to deallocate its data before gdcm uses them (e.g. with
 *          the Write() method )
 * @param inData user supplied pixel area (uint8_t* is just for the compiler.
 *               user is allowed to pass any kind of pixels since the size is
 *               given in bytes) 
 * @param expectedSize total image size, *in Bytes* 
 */
void FileHelper::SetUserData(uint8_t *inData, size_t expectedSize)
{
   PixelWriteConverter->SetUserData(inData, expectedSize);
}

/**
 * \brief   Get the image data defined by the user
 * \warning When writting the file, this data are get as default data to write
 */
uint8_t *FileHelper::GetUserData()
{
   return PixelWriteConverter->GetUserData();
}

/**
 * \brief   Get the image data size defined by the user
 * \warning When writting the file, this data are get as default data to write
 */
size_t FileHelper::GetUserDataSize()
{
   return PixelWriteConverter->GetUserDataSize();
}

/**
 * \brief   Get the image data from the file.
 *          If a LUT is found, the data are expanded to be RGB
 */
uint8_t *FileHelper::GetRGBData()
{
   return PixelReadConverter->GetRGB();
}

/**
 * \brief   Get the image data size from the file.
 *          If a LUT is found, the data are expanded to be RGB
 */
size_t FileHelper::GetRGBDataSize()
{
   return PixelReadConverter->GetRGBSize();
}

/**
 * \brief   Get the image data from the file.
 *          Even when a LUT is found, the data are not expanded to RGB!
 */
uint8_t *FileHelper::GetRawData()
{
   return PixelReadConverter->GetRaw();
}

/**
 * \brief   Get the image data size from the file.
 *          Even when a LUT is found, the data are not expanded to RGB!
 */
size_t FileHelper::GetRawDataSize()
{
   return PixelReadConverter->GetRawSize();
}

/**
 * \brief Access to the underlying \ref PixelReadConverter RGBA LUT
 */
uint8_t* FileHelper::GetLutRGBA()
{
   return PixelReadConverter->GetLutRGBA();
}

/**
 * \brief Writes on disk A SINGLE Dicom file
 *        NO test is performed on  processor "Endiannity".
 *        It's up to the user to call his Reader properly
 * @param fileName name of the file to be created
 *                 (any already existing file is over written)
 * @return false if write fails
 */
bool FileHelper::WriteRawData(std::string const &fileName)
{
   std::ofstream fp1(fileName.c_str(), std::ios::out | std::ios::binary );
   if (!fp1)
   {
      gdcmWarningMacro( "Fail to open (write) file:" << fileName.c_str());
      return false;
   }

   if( PixelWriteConverter->GetUserData() )
   {
      fp1.write( (char*)PixelWriteConverter->GetUserData(), 
                 PixelWriteConverter->GetUserDataSize() );
   }
   else if ( PixelReadConverter->GetRGB() )
   {
      fp1.write( (char*)PixelReadConverter->GetRGB(), 
                 PixelReadConverter->GetRGBSize());
   }
   else if ( PixelReadConverter->GetRaw() )
   {
      fp1.write( (char*)PixelReadConverter->GetRaw(), 
                 PixelReadConverter->GetRawSize());
   }
   else
   {
      gdcmErrorMacro( "Nothing written." );
   }

   fp1.close();

   return true;
}

/**
 * \brief Writes on disk A SINGLE Dicom file, 
 *        using the Implicit Value Representation convention
 *        NO test is performed on  processor "Endiannity".
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */

bool FileHelper::WriteDcmImplVR (std::string const &fileName)
{
   SetWriteTypeToDcmImplVR();
   return Write(fileName);
}

/**
* \brief Writes on disk A SINGLE Dicom file, 
 *        using the Explicit Value Representation convention
 *        NO test is performed on  processor "Endiannity". 
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */

bool FileHelper::WriteDcmExplVR (std::string const &fileName)
{
   SetWriteTypeToDcmExplVR();
   return Write(fileName);
}

/**
 * \brief Writes on disk A SINGLE Dicom file, 
 *        using the ACR-NEMA convention
 *        NO test is performed on  processor "Endiannity".
 *        (a l'attention des logiciels cliniques 
 *        qui ne prennent en entrée QUE des images ACR ...
 * \warning if a DICOM_V3 header is supplied,
 *         groups < 0x0008 and shadow groups are ignored
 * \warning NO TEST is performed on processor "Endiannity".
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */

bool FileHelper::WriteAcr (std::string const &fileName)
{
   SetWriteTypeToAcr();
   return Write(fileName);
}

/**
 * \brief Writes on disk A SINGLE Dicom file, 
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */
bool FileHelper::Write(std::string const &fileName)
{
   switch(WriteType)
   {
      case ImplicitVR:
         SetWriteFileTypeToImplicitVR();
         CheckMandatoryElements();
         break;
      case ExplicitVR:
         SetWriteFileTypeToExplicitVR();
         CheckMandatoryElements();
         break;
      case ACR:
      case ACR_LIBIDO:
         SetWriteFileTypeToACR();
         break;
      default:
         SetWriteFileTypeToExplicitVR();
         CheckMandatoryElements();
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   //
   // if recognition code tells us we dealt with a LibIDO image
   // we reproduce on disk the switch between lineNumber and columnNumber
   // just before writting ...
   /// \todo the best trick would be *change* the recognition code
   ///       but pb expected if user deals with, e.g. COMPLEX images
   if( WriteType == ACR_LIBIDO )
   {
      SetWriteToLibido();
   }
   else
   {
      SetWriteToNoLibido();
   }
   // ----------------- End of Special Patch ----------------
  
   switch(WriteMode)
   {
      case WMODE_RAW :
         SetWriteToRaw(); // modifies and pushes to the archive, when necessary
         break;
      case WMODE_RGB :
         SetWriteToRGB(); // modifies and pushes to the archive, when necessary
         break;
   }

   bool check = CheckWriteIntegrity(); // verifies length
   if(check)
   {
      check = FileInternal->Write(fileName,WriteType);
   }

   RestoreWrite();
   RestoreWriteFileType();

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the header to be Dicom Compliant again 
   // just after writting
   RestoreWriteOfLibido();
   // ----------------- End of Special Patch ----------------

   return check;
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief Checks the write integrity
 *
 * The tests made are :
 *  - verify the size of the image to write with the possible write
 *    when the user set an image data
 * @return true if check is successfull
 */
bool FileHelper::CheckWriteIntegrity()
{
   if(PixelWriteConverter->GetUserData())
   {
      int numberBitsAllocated = FileInternal->GetBitsAllocated();
      if ( numberBitsAllocated == 0 || numberBitsAllocated == 12 )
      {
         numberBitsAllocated = 16;
      }

      size_t decSize = FileInternal->GetXSize()
                    * FileInternal->GetYSize() 
                    * FileInternal->GetZSize()
                    * ( numberBitsAllocated / 8 )
                    * FileInternal->GetSamplesPerPixel();
      size_t rgbSize = decSize;
      if( FileInternal->HasLUT() )
         rgbSize = decSize * 3;

      switch(WriteMode)
      {
         case WMODE_RAW :
            if( decSize!=PixelWriteConverter->GetUserDataSize() )
            {
               gdcmWarningMacro( "Data size (Raw) is incorrect. Should be " 
                           << decSize << " / Found :" 
                           << PixelWriteConverter->GetUserDataSize() );
               return false;
            }
            break;
         case WMODE_RGB :
            if( rgbSize!=PixelWriteConverter->GetUserDataSize() )
            {
               gdcmWarningMacro( "Data size (RGB) is incorrect. Should be " 
                          << decSize << " / Found " 
                          << PixelWriteConverter->GetUserDataSize() );
               return false;
            }
            break;
      }
   }
   
   return true;
}

/**
 * \brief Updates the File to write RAW data (as opposed to RGB data)
 *       (modifies, when necessary, photochromatic interpretation, 
 *       bits allocated, Pixels element VR)
 */ 
void FileHelper::SetWriteToRaw()
{
   if( FileInternal->GetNumberOfScalarComponents() == 3 
    && !FileInternal->HasLUT())
   {
      SetWriteToRGB();
   } 
   else
   {
      ValEntry *photInt = CopyValEntry(0x0028,0x0004);
      if(FileInternal->HasLUT())
      {
         photInt->SetValue("PALETTE COLOR ");
      }
      else
      {
         photInt->SetValue("MONOCHROME2 ");
      }

      PixelWriteConverter->SetReadData(PixelReadConverter->GetRaw(),
                                       PixelReadConverter->GetRawSize());

      std::string vr = "OB";
      if( FileInternal->GetBitsAllocated()>8 )
         vr = "OW";
      if( FileInternal->GetBitsAllocated()==24 ) // For RGB ACR files 
         vr = "OB";
      BinEntry *pixel = 
         CopyBinEntry(GetFile()->GetGrPixel(),GetFile()->GetNumPixel(),vr);
      pixel->SetValue(GDCM_BINLOADED);
      pixel->SetBinArea(PixelWriteConverter->GetData(),false);
      pixel->SetLength(PixelWriteConverter->GetDataSize());

      Archive->Push(photInt);
      Archive->Push(pixel);
   }
}

/**
 * \brief Updates the File to write RGB data (as opposed to RAW data)
 *       (modifies, when necessary, photochromatic interpretation, 
 *       samples per pixel, Planar configuration, 
 *       bits allocated, bits stored, high bit -ACR 24 bits-
 *       Pixels element VR, pushes out the LUT, )
 */ 
void FileHelper::SetWriteToRGB()
{
   if(FileInternal->GetNumberOfScalarComponents()==3)
   {
      PixelReadConverter->BuildRGBImage();
      
      ValEntry *spp = CopyValEntry(0x0028,0x0002);
      spp->SetValue("3 ");

      ValEntry *planConfig = CopyValEntry(0x0028,0x0006);
      planConfig->SetValue("0 ");

      ValEntry *photInt = CopyValEntry(0x0028,0x0004);
      photInt->SetValue("RGB ");

      if(PixelReadConverter->GetRGB())
      {
         PixelWriteConverter->SetReadData(PixelReadConverter->GetRGB(),
                                          PixelReadConverter->GetRGBSize());
      }
      else // Raw data
      {
         PixelWriteConverter->SetReadData(PixelReadConverter->GetRaw(),
                                          PixelReadConverter->GetRawSize());
      }

      std::string vr = "OB";
      if( FileInternal->GetBitsAllocated()>8 )
         vr = "OW";
      if( FileInternal->GetBitsAllocated()==24 ) // For RGB ACR files 
         vr = "OB";
      BinEntry *pixel = 
         CopyBinEntry(GetFile()->GetGrPixel(),GetFile()->GetNumPixel(),vr);
      pixel->SetValue(GDCM_BINLOADED);
      pixel->SetBinArea(PixelWriteConverter->GetData(),false);
      pixel->SetLength(PixelWriteConverter->GetDataSize());

      Archive->Push(spp);
      Archive->Push(planConfig);
      Archive->Push(photInt);
      Archive->Push(pixel);

      // Remove any LUT
      Archive->Push(0x0028,0x1101);
      Archive->Push(0x0028,0x1102);
      Archive->Push(0x0028,0x1103);
      Archive->Push(0x0028,0x1201);
      Archive->Push(0x0028,0x1202);
      Archive->Push(0x0028,0x1203);

      // For old '24 Bits' ACR-NEMA
      // Thus, we have a RGB image and the bits allocated = 24 and 
      // samples per pixels = 1 (in the read file)
      if(FileInternal->GetBitsAllocated()==24) 
      {
         ValEntry *bitsAlloc = CopyValEntry(0x0028,0x0100);
         bitsAlloc->SetValue("8 ");

         ValEntry *bitsStored = CopyValEntry(0x0028,0x0101);
         bitsStored->SetValue("8 ");

         ValEntry *highBit = CopyValEntry(0x0028,0x0102);
         highBit->SetValue("7 ");

         Archive->Push(bitsAlloc);
         Archive->Push(bitsStored);
         Archive->Push(highBit);
      }
   }
   else
   {
      SetWriteToRaw();
   }
}

/**
 * \brief Restore the File write mode  
 */ 
void FileHelper::RestoreWrite()
{
   Archive->Restore(0x0028,0x0002);
   Archive->Restore(0x0028,0x0004);
   Archive->Restore(0x0028,0x0006);
   Archive->Restore(GetFile()->GetGrPixel(),GetFile()->GetNumPixel());

   // For old ACR-NEMA (24 bits problem)
   Archive->Restore(0x0028,0x0100);
   Archive->Restore(0x0028,0x0101);
   Archive->Restore(0x0028,0x0102);

   // For the LUT
   Archive->Restore(0x0028,0x1101);
   Archive->Restore(0x0028,0x1102);
   Archive->Restore(0x0028,0x1103);
   Archive->Restore(0x0028,0x1201);
   Archive->Restore(0x0028,0x1202);
   Archive->Restore(0x0028,0x1203);

   // group 0002 may be pushed out for ACR-NEMA writting purposes 
   Archive->Restore(0x0002,0x0000);
   Archive->Restore(0x0002,0x0001);
   Archive->Restore(0x0002,0x0002);
   Archive->Restore(0x0002,0x0003);
   Archive->Restore(0x0002,0x0010);
   Archive->Restore(0x0002,0x0012);
   Archive->Restore(0x0002,0x0013);
   Archive->Restore(0x0002,0x0016);
   Archive->Restore(0x0002,0x0100);
   Archive->Restore(0x0002,0x0102);
}

/**
 * \brief Pushes out the whole group 0002
 *        FIXME : better, set a flag to tell the writer not to write it ...
 *        FIXME : method should probably have an other name !
 *                SetWriteFileTypeToACR is NOT opposed to 
 *                SetWriteFileTypeToExplicitVR and SetWriteFileTypeToImplicitVR
 */ 
void FileHelper::SetWriteFileTypeToACR()
{
   Archive->Push(0x0002,0x0000);
   Archive->Push(0x0002,0x0001);
   Archive->Push(0x0002,0x0002);
   Archive->Push(0x0002,0x0003);
   Archive->Push(0x0002,0x0010);// Only TransferSyntax was pushed out !
   Archive->Push(0x0002,0x0012);
   Archive->Push(0x0002,0x0013);
   Archive->Push(0x0002,0x0016);
   Archive->Push(0x0002,0x0100);
   Archive->Push(0x0002,0x0102);
}

/**
 * \brief Sets in the File the TransferSyntax to 'Explicit VR Little Endian"   
 */ 
void FileHelper::SetWriteFileTypeToExplicitVR()
{
   std::string ts = Util::DicomString( 
      Global::GetTS()->GetSpecialTransferSyntax(TS::ExplicitVRLittleEndian) );

   ValEntry *tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);

   Archive->Push(tss);
}

/**
 * \brief Sets in the File the TransferSyntax to 'Implicit VR Little Endian"   
 */ 
void FileHelper::SetWriteFileTypeToImplicitVR()
{
   std::string ts = Util::DicomString(
      Global::GetTS()->GetSpecialTransferSyntax(TS::ImplicitVRLittleEndian) );

   ValEntry *tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);

   Archive->Push(tss);
}


/**
 * \brief Restore in the File the initial group 0002
 */ 
void FileHelper::RestoreWriteFileType()
{
   // group 0002 may be pushed out for ACR-NEMA writting purposes 
   Archive->Restore(0x0002,0x0000);
   Archive->Restore(0x0002,0x0001);
   Archive->Restore(0x0002,0x0002);
   Archive->Restore(0x0002,0x0003);
   Archive->Restore(0x0002,0x0010);//only TransferSyntax was pushed out/restored
   Archive->Restore(0x0002,0x0012);
   Archive->Restore(0x0002,0x0013);
   Archive->Restore(0x0002,0x0016);
   Archive->Restore(0x0002,0x0100);
   Archive->Restore(0x0002,0x0102);
}

/**
 * \brief Set the Write not to Libido format
 */ 
void FileHelper::SetWriteToLibido()
{
   ValEntry *oldRow = dynamic_cast<ValEntry *>
                (FileInternal->GetDocEntry(0x0028, 0x0010));
   ValEntry *oldCol = dynamic_cast<ValEntry *>
                (FileInternal->GetDocEntry(0x0028, 0x0011));
   
   if( oldRow && oldCol )
   {
      std::string rows, columns; 

      ValEntry *newRow=new ValEntry(oldRow->GetDictEntry());
      ValEntry *newCol=new ValEntry(oldCol->GetDictEntry());

      newRow->Copy(oldCol);
      newCol->Copy(oldRow);

      newRow->SetValue(oldCol->GetValue());
      newCol->SetValue(oldRow->GetValue());

      Archive->Push(newRow);
      Archive->Push(newCol);
   }

   ValEntry *libidoCode = CopyValEntry(0x0008,0x0010);
   libidoCode->SetValue("ACRNEMA_LIBIDO_1.1");
   Archive->Push(libidoCode);
}

/**
 * \brief Set the Write not to No Libido format
 */ 
void FileHelper::SetWriteToNoLibido()
{
   ValEntry *recCode = dynamic_cast<ValEntry *>
                (FileInternal->GetDocEntry(0x0008,0x0010));
   if( recCode )
   {
      if( recCode->GetValue() == "ACRNEMA_LIBIDO_1.1" )
      {
         ValEntry *libidoCode = CopyValEntry(0x0008,0x0010);
         libidoCode->SetValue("");
         Archive->Push(libidoCode);
      }
   }
}

/**
 * \brief Restore the Write format
 */ 
void FileHelper::RestoreWriteOfLibido()
{
   Archive->Restore(0x0028,0x0010);
   Archive->Restore(0x0028,0x0011);
   Archive->Restore(0x0008,0x0010);
}

/**
 * \brief Duplicates a ValEntry or creates it.
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the new Val Entry (NULL when creation failed).          
 */ 
ValEntry *FileHelper::CopyValEntry(uint16_t group, uint16_t elem)
{
   DocEntry *oldE = FileInternal->GetDocEntry(group, elem);
   ValEntry *newE;

   if( oldE )
   {
      newE = new ValEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetFile()->NewValEntry(group, elem);
   }

   return newE;
}

/**
 * \brief   Duplicates a BinEntry or creates it.
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * @param   vr  Value Representation of the Entry
 *          FIXME : what is it used for?
 * \return  pointer to the new Bin Entry (NULL when creation failed).
 */ 
BinEntry *FileHelper::CopyBinEntry(uint16_t group, uint16_t elem,
                                   const std::string &vr)
{
   DocEntry *oldE = FileInternal->GetDocEntry(group, elem);
   BinEntry *newE;

   if( oldE )
      if( oldE->GetVR()!=vr )
         oldE = NULL;

   if( oldE )
   {
      newE = new BinEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetFile()->NewBinEntry(group, elem, vr);
   }

   return newE;
}

/**
 * \brief   Checks the Mandatory Elements
 *          adds the mandatory Entries if not found
 *          (when user asks to write as a DICOM file, an ACR-NEMA file
 *           he read before)
 */ 
void FileHelper::CheckMandatoryElements()
{
   // just to remember : 'official' 0002 group

   //0002 0000 UL 1 Meta Group Length
   //0002 0001 OB 1 File Meta Information Version
   //0002 0002 UI 1 Media Stored SOP Class UID
   //0002 0003 UI 1 Media Stored SOP Instance UID
   //0002 0010 UI 1 Transfer Syntax UID
   //0002 0012 UI 1 Implementation Class UID
   //0002 0013 SH 1 Implementation Version Name
   //0002 0016 AE 1 Source Application Entity Title
   //0002 0100 UI 1 Private Information Creator
   //0002 0102 OB 1 Private Information


   // Create them if not found
   ValEntry *e0002_0000 = CopyValEntry(0x0002,0x0000);
   e0002_0000->SetValue("0"); // for the moment
   Archive->Push(e0002_0000);
  
/*   BinEntry *e0002_0001 = CopyBinEntry(0x0002,0x0001, "OB");
   e0002_0001->SetBinArea((uint8_t*)Util::GetFileMetaInformationVersion(),false);
   e0002_0001->SetLength(2);*/

/*   ValEntry *e0002_0002 = CopyValEntry(0x0002,0x0002);
   // [Secondary Capture Image Storage]
   e0002_0002->SetValue("1.2.840.10008.5.1.4.1.1.7"); 
   Archive->Push(e0002_0002);*/
 
   // 'Media Stored SOP Instance UID'   
/*   ValEntry *e0002_0003 = CopyValEntry(0x0002,0x0003);
   e0002_0003->SetValue(Util::CreateUniqueUID());
   Archive->Push(e0002_0003); */

/*   ValEntry *e0002_0010 = CopyValEntry(0x0002,0x0010);
   //[Explicit VR - Little Endian] 
   e0002_0010->SetValue("1.2.840.10008.1.2.1"); 
   Archive->Push(e0002_0010);*/
 
   // 'Implementation Class UID'
/*   ValEntry *e0002_0012 = CopyValEntry(0x0002,0x0012);
   e0002_0012->SetValue(Util::CreateUniqueUID());
   Archive->Push(e0002_0012); */

   // 'Implementation Version Name'
   ValEntry *e0002_0013 = CopyValEntry(0x0002,0x0013);
   e0002_0013->SetValue("GDCM 1.0");
   Archive->Push(e0002_0013);

   //'Source Application Entity Title' Not Mandatory
   //ValEntry *e0002_0016 = CopyValEntry(0x0002,0x0016);
   //   e0002_0016->SetValue("1.2.840.10008.5.1.4.1.1.7");
   //   Archive->Push(e0002_0016);

   // Push out 'LibIDO-special' entries, if any
   Archive->Push(0x0028,0x0015);
   Archive->Push(0x0028,0x0016);
   Archive->Push(0x0028,0x0017);
   Archive->Push(0x0028,0x00199);

   // --- Check UID-related Entries ---

   // If 'SOP Class UID' exists ('true DICOM' image)
   ValEntry *e0008_0016 = FileInternal->GetValEntry(0x0008, 0x0016);
   if ( e0008_0016 != 0 )
   {
/*      // Create 'Source Image Sequence' SeqEntry
      SeqEntry *s = new SeqEntry (
      Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x2112) );
      SQItem *sqi = new SQItem(1);
      // (we assume 'SOP Instance UID' exists too) 
      // create 'Referenced SOP Class UID'
      ValEntry *e0008_1150 = new ValEntry(
      Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x1150) );
      e0008_1150->SetValue( e0008_0016->GetValue());
      sqi->AddEntry(e0008_1150);
      
      // create 'Referenced SOP Instance UID'
      ValEntry *e0008_0018 = FileInternal->GetValEntry(0x0008, 0x0018);
      ValEntry *e0008_1155 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x1155) );
      e0008_1155->SetValue( e0008_0018->GetValue());
      sqi->AddEntry(e0008_1155);

      s->AddSQItem(sqi,1); 
      // temporarily replaces any previous 'Source Image Sequence' 
      Archive->Push(s);
 
      // 'Image Type'
      ValEntry *e0008_0008 = CopyValEntry(0x0008,0x0008);
      e0008_0008->SetValue("DERIVED\\PRIMARY");
      Archive->Push(e0008_0008);*/
   } 
   else
   {
   // SOP Class UID
/*      e0008_0016  =  new ValEntry( 
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0016) );
      // [Secondary Capture Image Storage]
      e0008_0016 ->SetValue("1.2.840.10008.5.1.4.1.1.7"); 
      Archive->Push(e0008_0016); */
   }

   // new value for 'SOP Instance UID'
   ValEntry *e0008_0018 = new ValEntry(
      Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0018) );
   e0008_0018->SetValue( Util::CreateUniqueUID() );
   Archive->Push(e0008_0018);

   // Instance Creation Date
   ValEntry *e0008_0012 = CopyValEntry(0x0008,0x0012);
   std::string date = Util::GetCurrentDate();
   e0008_0012->SetValue(date.c_str());
   Archive->Push(e0008_0012);
 
   // Instance Creation Time
   ValEntry *e0008_0013 = CopyValEntry(0x0008,0x0013);
   std::string time = Util::GetCurrentTime();
   e0008_0013->SetValue(time.c_str());
   Archive->Push(e0008_0013);

/*   // new value for 'Serie Instance UID'
   // TODO prevoir booleen pour figer la valeur d'un appel a l'autre
   //                           calculer nouvelle valeur a chaque fois

   ValEntry *e0020_000e = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0020, 0x000e) );
   e0020_000e->SetValue( Util::CreateUniqueUID() );
   Archive->Push(e0020_000e);

   // new value for 'Study Instance UID'
   // TODO prevoir flag pour figer la valeur d'un appel a l'autre
   //                        calculer nouvelle valeur a chaque fois
   //                        reutiliser la valeur image origine

   ValEntry *e0020_000d = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0020, 0x000d) );
   e0020_000d->SetValue( Util::CreateUniqueUID() );
   Archive->Push(e0020_000d);
*/

// ----- Add Mandatory Entries if missing ---

// Entries whose type is 1 are mandatory, with a mandatory value
// Entries whose type is 1c are ?
// Entries whose type is 2 are mandatory, with a optional value
// Entries whose type is 2c are ?
// Entries whose type is 3 are optional

//   ValEntry *e;

   // Modality
/*   ValEntry *e0008_0060 = FileInternal->GetValEntry(0x0008, 0x0060);
   if ( !e0008_0060 )
   {
      e0008_0060 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0060) );
      e0008_0060->SetValue("OT");
      Archive->Push(e0008_0060);
   } 

   // Manufacturer
   ValEntry *e0008_0070 = FileInternal->GetValEntry(0x0008, 0x0070);
   if ( !e0008_0070 )
   {
      e0008_0070 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0070) );
      e0008_0070->SetValue("GDCM Factory");
      Archive->Push(e0008_0070);
   }

   // Institution Name
   ValEntry *e0008_0080 = FileInternal->GetValEntry(0x0008, 0x0080);
   if ( !e0008_0080 )
   {
      e0008_0080 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0080) );
      e0008_0080->SetValue("GDCM Hospital");
      Archive->Push(e0008_0080);
   }

   // Patient's Name
   ValEntry *e0010_0010 = FileInternal->GetValEntry(0x0010, 0x0010);
   if ( !e0010_0010 )
   {
      e0010_0010 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0010, 0x0010) );
      e0010_0010->SetValue("GDCM_Patient");
      Archive->Push(e0010_0010);
   }

   // Patient's ID
   ValEntry *e_0010_0020 = FileInternal->GetValEntry(0x0010, 0x0020);
   if ( !e )
   {
      e_0010_0020 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0010, 0x0020) );
      e_0010_0020->SetValue("GDCM_Patient_ID");
      Archive->Push(e_0010_0020);
   }

   // Patient's Birth Date
   ValEntry *e_0010_0030 = FileInternal->GetValEntry(0x0010, 0x0030);
   if ( !e_0010_0030 )
   {
      e_0010_0030 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0010, 0x0030) );
      e_0010_0030->SetValue("");
      Archive->Push(e_0010_0030);
   }

   // Patient's Sex
   ValEntry *e_0010_0040 = FileInternal->GetValEntry(0x0010, 0x0040);
   if ( !e_0010_0040 )
   {
      e_0010_0040 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0010, 0x0040) );
      e_0010_0040->SetValue("");
      Archive->Push(e_0010_0040);
   }

   // Referring Physician's Name
   ValEntry *e_0008_0090 = FileInternal->GetValEntry(0x0008, 0x0090);
   if ( !e_0008_0090 )
   {
      e_0008_0090 = new ValEntry(
            Global::GetDicts()->GetDefaultPubDict()->GetEntry(0x0008, 0x0090) );
      e_0008_0090->SetValue("");
      Archive->Push(e_0008_0090);
   }*/
} 
 
//-----------------------------------------------------------------------------
// Private
/**
 * \brief Factorization for various forms of constructors.
 */
void FileHelper::Initialize()
{
   WriteMode = WMODE_RAW;
   WriteType = ExplicitVR;

   PixelReadConverter = new PixelReadConvert;
   PixelWriteConverter = new PixelWriteConvert;
   Archive = new DocEntryArchive( FileInternal );

   if ( FileInternal->IsReadable() )
   {
      PixelReadConverter->GrabInformationsFromFile( FileInternal );
   }
}

/**
 * \brief   
 */ 
uint8_t *FileHelper::GetRaw()
{
   uint8_t *raw = PixelReadConverter->GetRaw();
   if ( ! raw )
   {
      // The Raw image migth not be loaded yet:
      std::ifstream *fp = FileInternal->OpenFile();
      PixelReadConverter->ReadAndDecompressPixelData( fp );
      if(fp) 
         FileInternal->CloseFile();

      raw = PixelReadConverter->GetRaw();
      if ( ! raw )
      {
         gdcmWarningMacro( "Read/decompress of pixel data apparently went wrong.");
         return 0;
      }
   }

   return raw;
}

//-----------------------------------------------------------------------------
// Print
void FileHelper::Print(std::ostream &os, std::string const &)
{
   FileInternal->SetPrintLevel(PrintLevel);
   FileInternal->Print(os);

   PixelReadConverter->SetPrintLevel(PrintLevel);
   PixelReadConverter->Print(os);
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
