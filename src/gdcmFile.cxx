/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 16:17:00 $
  Version:   $Revision: 1.194 $
                                                                                
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
#include "gdcmHeader.h"
#include "gdcmPixelReadConvert.h"
#include "gdcmPixelWriteConvert.h"
#include "gdcmDocEntryArchive.h"

#include <fstream>

namespace gdcm 
{
typedef std::pair<TagDocEntryHT::iterator,TagDocEntryHT::iterator> IterHT;

//-------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (Header only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 *        (see GetImageData, GetImageDataRaw)
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        one sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 */
FileHelper::FileHelper( )
{
   HeaderInternal = new Header( );
   SelfHeader = true;
   Initialise();
}

/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (Header only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 *        (see GetImageData, GetImageDataRaw)
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        user sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 * @param header already built Header
 */
FileHelper::FileHelper(Header *header)
{
   HeaderInternal = header;
   SelfHeader = false;
   Initialise();
}

/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (Header only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 *        (see GetImageData, GetImageDataRaw)
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        one sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 * @param filename file to be opened for parsing
 */
FileHelper::FileHelper(std::string const & filename )
{
   HeaderInternal = new Header( filename );
   SelfHeader = true;
   Initialise();
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
      delete HeaderInternal;
   }
   HeaderInternal = 0;
}

//-----------------------------------------------------------------------------
// Print
void FileHelper::Print(std::ostream &os, std::string const &)
{
   HeaderInternal->SetPrintLevel(PrintLevel);
   HeaderInternal->Print(os);

   PixelReadConverter->SetPrintLevel(PrintLevel);
   PixelReadConverter->Print(os);
}

//-----------------------------------------------------------------------------
// Public
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
 *          - Transforms YBR pixels, if any, into RGB pixels
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

   if ( HeaderInternal->HasLUT() && PixelReadConverter->BuildRGBImage() )
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
 * \        NULL if alloc fails 
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

   if ( HeaderInternal->HasLUT() && PixelReadConverter->BuildRGBImage() )
   {
      if ( PixelReadConverter->GetRGBSize() > maxSize )
      {
         gdcmVerboseMacro( "Pixel data bigger than caller's expected MaxSize");
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
      gdcmVerboseMacro( "Pixel data bigger than caller's expected MaxSize");
      return 0;
   }
   memcpy( destination,
           (void*)PixelReadConverter->GetRaw(),
           PixelReadConverter->GetRawSize() );
   return PixelReadConverter->GetRawSize();
}

/**
 * \brief   Points the internal pointer to the callers inData
 *          image representation, BUT WITHOUT COPYING THE DATA.
 *          'image' Pixels are presented as C-like 2D arrays : line per line.
 *          'volume'Pixels are presented as C-like 3D arrays : plane per plane 
 * \warning Since the pixels are not copied, it is the caller's responsability
 *          not to deallocate it's data before gdcm uses them (e.g. with
 *          the Write() method.
 * @param inData user supplied pixel area
 * @param expectedSize total image size, in Bytes
 *
 * @return boolean
 */
void FileHelper::SetImageData(uint8_t *inData, size_t expectedSize)
{
   SetUserData(inData,expectedSize);
}

/**
 * \brief   Set the image datas defined by the user
 * \warning When writting the file, this datas are get as default datas to write
 */
void FileHelper::SetUserData(uint8_t *data, size_t expectedSize)
{
   PixelWriteConverter->SetUserData(data,expectedSize);
}

/**
 * \brief   Get the image datas defined by the user
 * \warning When writting the file, this datas are get as default data to write
 */
uint8_t *FileHelper::GetUserData()
{
   return PixelWriteConverter->GetUserData();
}

/**
 * \brief   Get the image data size defined by the user
 * \warning When writting the file, this datas are get as default data to write
 */
size_t FileHelper::GetUserDataSize()
{
   return PixelWriteConverter->GetUserDataSize();
}

/**
 * \brief   Get the image datas from the file.
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
 * \brief   Get the image datas from the file.
 *          If a LUT is found, the datas are not expanded !
 */
uint8_t *FileHelper::GetRawData()
{
   return PixelReadConverter->GetRaw();
}

/**
 * \brief   Get the image data size from the file.
 *          If a LUT is found, the data are not expanded !
 */
size_t FileHelper::GetRawDataSize()
{
   return PixelReadConverter->GetRawSize();
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
      gdcmVerboseMacro( "Fail to open (write) file:" << fileName.c_str());
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
         break;
      case ExplicitVR:
         SetWriteFileTypeToExplicitVR();
         break;
      case ACR:
      case ACR_LIBIDO:
         SetWriteFileTypeToACR();
         break;
      default:
         SetWriteFileTypeToExplicitVR();
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
         SetWriteToRaw();
         break;
      case WMODE_RGB :
         SetWriteToRGB();
         break;
   }

   bool check = CheckWriteIntegrity();
   if(check)
   {
      check = HeaderInternal->Write(fileName,WriteType);
   }

   RestoreWrite();
   RestoreWriteFileType();

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting
   RestoreWriteOfLibido();
   // ----------------- End of Special Patch ----------------

   return check;
}

/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (string) to substitute with
 * @param   group     group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 */
bool FileHelper::SetEntry(std::string const &content,
                    uint16_t group, uint16_t elem)
{ 
   return HeaderInternal->SetEntry(content,group,elem);
}


/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (void*  -> uint8_t*) to substitute with
 * @param   lgth new value length
 * @param   group     group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 */
bool FileHelper::SetEntry(uint8_t *content, int lgth,
                    uint16_t group, uint16_t elem)
{
   return HeaderInternal->SetEntry(content,lgth,group,elem);
}

/**
 * \brief   Modifies the value of a given Doc Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * @param   content (string) Value to be set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */ 
bool FileHelper::ReplaceOrCreate(std::string const &content,
                           uint16_t group, uint16_t elem)
{
   return HeaderInternal->ReplaceOrCreate(content,group,elem) != NULL;
}

/*
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 *          A copy of the binArea is made to be kept in the Document.
 * @param   binArea (binary) value to be set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */
bool FileHelper::ReplaceOrCreate(uint8_t *binArea, int lgth,
                           uint16_t group, uint16_t elem)
{
   return HeaderInternal->ReplaceOrCreate(binArea,lgth,group,elem) != NULL;
}

/**
 * \brief Access to the underlying \ref PixelReadConverter RGBA LUT
 */
uint8_t* FileHelper::GetLutRGBA()
{
   return PixelReadConverter->GetLutRGBA();
}

//-----------------------------------------------------------------------------
// Protected

/**
 * \brief Check the write integrity
 *
 * The tests made are :
 *  - verify the size of the image to write with the possible write
 *    when the user set an image data
 * @return true if the check successfulls
 */
bool FileHelper::CheckWriteIntegrity()
{
   if(PixelWriteConverter->GetUserData())
   {
      int numberBitsAllocated = HeaderInternal->GetBitsAllocated();
      if ( numberBitsAllocated == 0 || numberBitsAllocated == 12 )
      {
         numberBitsAllocated = 16;
      }

      size_t decSize = HeaderInternal->GetXSize()
                    * HeaderInternal->GetYSize() 
                    * HeaderInternal->GetZSize()
                    * ( numberBitsAllocated / 8 )
                    * HeaderInternal->GetSamplesPerPixel();
      size_t rgbSize = decSize;
      if( HeaderInternal->HasLUT() )
         rgbSize = decSize * 3;

      switch(WriteMode)
      {
         case WMODE_RAW :
            if( decSize!=PixelWriteConverter->GetUserDataSize() )
            {
               gdcmVerboseMacro( "Data size is incorrect (Raw)" << decSize 
                    << " / " << PixelWriteConverter->GetUserDataSize() );
               return false;
            }
            break;
         case WMODE_RGB :
            if( rgbSize!=PixelWriteConverter->GetUserDataSize() )
            {
               gdcmVerboseMacro( "Data size is incorrect (RGB)" << decSize
                   << " / " << PixelWriteConverter->GetUserDataSize() );
               return false;
            }
            break;
      }
   }
   
   return true;
}

/**
 * \brief   
 */ 
void FileHelper::SetWriteToRaw()
{
   if( HeaderInternal->GetNumberOfScalarComponents() == 3 
    && !HeaderInternal->HasLUT())
   {
      SetWriteToRGB();
   } 
   else
   {
      ValEntry *photInt = CopyValEntry(0x0028,0x0004);
      if(HeaderInternal->HasLUT())
      {
         photInt->SetValue("PALETTE COLOR ");
      }
      else
      {
         photInt->SetValue("MONOCHROME1 ");
      }

      PixelWriteConverter->SetReadData(PixelReadConverter->GetRaw(),
                                       PixelReadConverter->GetRawSize());

      BinEntry *pixel = 
         CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
      pixel->SetValue(GDCM_BINLOADED);
      pixel->SetBinArea(PixelWriteConverter->GetData(),false);
      pixel->SetLength(PixelWriteConverter->GetDataSize());

      Archive->Push(photInt);
      Archive->Push(pixel);
   }
}


/**
 * \brief   
 */ 
void FileHelper::SetWriteToRGB()
{
   if(HeaderInternal->GetNumberOfScalarComponents()==3)
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

      BinEntry *pixel = 
         CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
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

      // For old ACR-NEMA
      // Thus, we have a RGB image and the bits allocated = 24 and 
      // samples per pixels = 1 (in the read file)
      if(HeaderInternal->GetBitsAllocated()==24) 
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
 * \brief   
 */ 
void FileHelper::RestoreWrite()
{
   Archive->Restore(0x0028,0x0002);
   Archive->Restore(0x0028,0x0004);
   Archive->Restore(0x0028,0x0006);
   Archive->Restore(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());

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
}

/**
 * \brief   
 */ 
void FileHelper::SetWriteFileTypeToACR()
{
   Archive->Push(0x0002,0x0010);
}

/**
 * \brief   
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
 * \brief   
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
 * \brief   
 */ 
void FileHelper::RestoreWriteFileType()
{
   Archive->Restore(0x0002,0x0010);
}

void FileHelper::SetWriteToLibido()
{
   ValEntry *oldRow = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntry(0x0028, 0x0010));
   ValEntry *oldCol = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntry(0x0028, 0x0011));
   
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
 * \brief   
 */ 
void FileHelper::SetWriteToNoLibido()
{
   ValEntry *recCode = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntry(0x0008,0x0010));
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
 * \brief   
 */ 
void FileHelper::RestoreWriteOfLibido()
{
   Archive->Restore(0x0028,0x0010);
   Archive->Restore(0x0028,0x0011);
   Archive->Restore(0x0008,0x0010);
}

ValEntry *FileHelper::CopyValEntry(uint16_t group,uint16_t elem)
{
   DocEntry *oldE = HeaderInternal->GetDocEntry(group, elem);
   ValEntry *newE;

   if(oldE)
   {
      newE = new ValEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetHeader()->NewValEntry(group,elem);
   }

   return newE;
}

/**
 * \brief   Modifies the value of a given Bin Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * @param   content (string) Value to be set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created Bin Entry (NULL when creation
 *          failed).
 */ 
BinEntry *FileHelper::CopyBinEntry(uint16_t group,uint16_t elem)
{
   DocEntry *oldE = HeaderInternal->GetDocEntry(group, elem);
   BinEntry *newE;

   if(oldE)
   {
      newE = new BinEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetHeader()->NewBinEntry(group,elem);
   }

   return newE;
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief Factorization for various forms of constructors.
 */
void FileHelper::Initialise()
{
   WriteMode = WMODE_RAW;
   WriteType = ExplicitVR;

   PixelReadConverter = new PixelReadConvert;
   PixelWriteConverter = new PixelWriteConvert;
   Archive = new DocEntryArchive( HeaderInternal );

   if ( HeaderInternal->IsReadable() )
   {
      PixelReadConverter->GrabInformationsFromHeader( HeaderInternal );
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
      std::ifstream *fp = HeaderInternal->OpenFile();
      PixelReadConverter->ReadAndDecompressPixelData( fp );
      if(fp) 
         HeaderInternal->CloseFile();

      raw = PixelReadConverter->GetRaw();
      if ( ! raw )
      {
         gdcmVerboseMacro( "Read/decompress of pixel data apparently went wrong.");
         return 0;
      }
   }

   return raw;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

