  /*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2004/12/16 10:51:49 $
  Version:   $Revision: 1.177 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
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
File::File( )
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
File::File(Header *header)
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
File::File(std::string const & filename )
{
   HeaderInternal = new Header( filename );
   SelfHeader = true;
   Initialise();
}

/**
 * \brief canonical destructor
 * \note  If the Header was created by the File constructor,
 *        it is destroyed by the File
 */
File::~File()
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

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Get the size of the image data
 * 
 *          If the image can be RGB (with a lut or by default), the size 
 *          corresponds to the RGB image
 * @return  The image size
 */
size_t File::GetImageDataSize()
{
   if ( PixelWriteConverter->GetUserData() )
   {
      return PixelWriteConverter->GetUserDataSize();
   }

   return PixelReadConverter->GetRGBSize();
}

/**
 * \brief   Get the size of the image data
 * 
 *          If the image can be RGB by transformation in a LUT, this
 *          transformation isn't considered
 * @return  The raw image size
 */
size_t File::GetImageDataRawSize()
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
uint8_t* File::GetImageData()
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
uint8_t* File::GetImageDataRaw ()
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
size_t File::GetImageDataIntoVector (void* destination, size_t maxSize)
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
         dbg.Verbose(0, "File::GetImageDataIntoVector: pixel data bigger"
                        "than caller's expected MaxSize");
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
      dbg.Verbose(0, "File::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
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
void File::SetImageData(uint8_t* inData, size_t expectedSize)
{
   SetUserData(inData,expectedSize);
}

/**
 * \brief   Set the image datas defined by the user
 * \warning When writting the file, this datas are get as default datas to write
 */
void File::SetUserData(uint8_t* data, size_t expectedSize)
{
   PixelWriteConverter->SetUserData(data,expectedSize);
}

/**
 * \brief   Get the image datas defined by the user
 * \warning When writting the file, this datas are get as default datas to write
 */
uint8_t* File::GetUserData()
{
   return PixelWriteConverter->GetUserData();
}

/**
 * \brief   Get the image data size defined by the user
 * \warning When writting the file, this datas are get as default datas to write
 */
size_t File::GetUserDataSize()
{
   return PixelWriteConverter->GetUserDataSize();
}

/**
 * \brief   Get the image datas from the file.
 *          If a LUT is found, the datas are expanded to be RGB
 */
uint8_t* File::GetRGBData()
{
   return PixelReadConverter->GetRGB();
}

/**
 * \brief   Get the image data size from the file.
 *          If a LUT is found, the datas are expanded to be RGB
 */
size_t File::GetRGBDataSize()
{
   return PixelReadConverter->GetRGBSize();
}

/**
 * \brief   Get the image datas from the file.
 *          If a LUT is found, the datas are not expanded !
 */
uint8_t* File::GetRawData()
{
   return PixelReadConverter->GetRaw();
}

/**
 * \brief   Get the image data size from the file.
 *          If a LUT is found, the datas are not expanded !
 */
size_t File::GetRawDataSize()
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

bool File::WriteRawData(std::string const & fileName)
{
  std::ofstream fp1(fileName.c_str(), std::ios::out | std::ios::binary );
   if (!fp1)
   {
      dbg.Verbose(2, "Fail to open (write) file:", fileName.c_str());
      return false;
   }

   if(PixelWriteConverter->GetUserData())
      fp1.write((char*)PixelWriteConverter->GetUserData(), PixelWriteConverter->GetUserDataSize());
   else if(PixelReadConverter->GetRGB())
      fp1.write((char*)PixelReadConverter->GetRGB(), PixelReadConverter->GetRGBSize());
   else if(PixelReadConverter->GetRaw())
      fp1.write((char*)PixelReadConverter->GetRaw(), PixelReadConverter->GetRawSize());

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

bool File::WriteDcmImplVR (std::string const & fileName)
{
   SetWriteTypeToDcmImplVR();
   return Write(fileName);
}

/**
* \brief Writes on disk A SINGLE Dicom file, 
 *        using the Explicit Value Representation convention
 *        NO test is performed on  processor "Endiannity". * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */

bool File::WriteDcmExplVR (std::string const & fileName)
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

bool File::WriteAcr (std::string const & fileName)
{
   SetWriteTypeToAcr();
   return Write(fileName);
}

bool File::Write(std::string const& fileName)
{
   return WriteBase(fileName);
}

bool File::SetEntryByNumber(std::string const& content,
                            uint16_t group, uint16_t element)
{ 
   return HeaderInternal->SetEntryByNumber(content,group,element);
}

bool File::SetEntryByNumber(uint8_t* content, int lgth,
                            uint16_t group, uint16_t element)
{
   return HeaderInternal->SetEntryByNumber(content,lgth,group,element);
}

bool File::ReplaceOrCreateByNumber(std::string const& content,
                                   uint16_t group, uint16_t element)
{
   return HeaderInternal->ReplaceOrCreateByNumber(content,group,element) != NULL;
}

bool File::ReplaceOrCreateByNumber(uint8_t* binArea, int lgth,
                                   uint16_t group, uint16_t element)
{
   return HeaderInternal->ReplaceOrCreateByNumber(binArea,lgth,group,element) != NULL;
}

/**
 * \brief Access to the underlying \ref PixelReadConverter RGBA LUT
 */
uint8_t* File::GetLutRGBA()
{
   return PixelReadConverter->GetLutRGBA();
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief NOT a end user inteded function
 *        (used by WriteDcmExplVR, WriteDcmImplVR, WriteAcr, etc)
 * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @param  type file type (ExplicitVR, ImplicitVR, ...)
 * @return false if write fails
 */
bool File::WriteBase (std::string const & fileName)
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
 * \brief Check the write integrity
 *
 * The tests made are :
 *  - verify the size of the image to write with the possible write
 *    when the user set an image data
 * @return true if the check successfulls
 */
bool File::CheckWriteIntegrity()
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
               dbg.Verbose(0, "File::CheckWriteIntegrity: Data size is incorrect (Raw)");
               //std::cerr << "File::CheckWriteIntegrity: Data size is incorrect (Raw)\n"
               //          << decSize << " / " << PixelWriteConverter->GetUserDataSize() << "\n";
               return false;
            }
            break;
         case WMODE_RGB :
            if( rgbSize!=PixelWriteConverter->GetUserDataSize() )
            {
               dbg.Verbose(0, "File::CheckWriteIntegrity: Data size is incorrect (RGB)");
               //std::cerr << "File::CheckWriteIntegrity: Data size is incorrect (RGB)\n"
               //          << decSize << " / " << PixelWriteConverter->GetUserDataSize() << "\n";
               return false;
            }
            break;
      }
   }
   
   return true;
}

void File::SetWriteToRaw()
{
   if(HeaderInternal->GetNumberOfScalarComponents()==3 && !HeaderInternal->HasLUT())
   {
      SetWriteToRGB();
   } 
   else
   {
      ValEntry* photInt = CopyValEntry(0x0028,0x0004);
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

      BinEntry* pixel = CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
      pixel->SetValue(GDCM_BINLOADED);
      pixel->SetBinArea(PixelWriteConverter->GetData(),false);
      pixel->SetLength(PixelWriteConverter->GetDataSize());

      Archive->Push(photInt);
      Archive->Push(pixel);
   }
}

void File::SetWriteToRGB()
{
   if(HeaderInternal->GetNumberOfScalarComponents()==3)
   {
      PixelReadConverter->BuildRGBImage();
      
      ValEntry* spp = CopyValEntry(0x0028,0x0002);
      spp->SetValue("3 ");

      ValEntry* planConfig = CopyValEntry(0x0028,0x0006);
      planConfig->SetValue("0 ");

      ValEntry* photInt = CopyValEntry(0x0028,0x0004);
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

      BinEntry* pixel = CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
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
         ValEntry* bitsAlloc = CopyValEntry(0x0028,0x0100);
         bitsAlloc->SetValue("8 ");

         ValEntry* bitsStored = CopyValEntry(0x0028,0x0101);
         bitsStored->SetValue("8 ");

         ValEntry* highBit = CopyValEntry(0x0028,0x0102);
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

void File::RestoreWrite()
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

void File::SetWriteFileTypeToACR()
{
   Archive->Push(0x0002,0x0010);
}

void File::SetWriteFileTypeToExplicitVR()
{
   std::string ts = Util::DicomString( 
      Document::GetTransferSyntaxValue(ExplicitVRLittleEndian).c_str() );

   ValEntry* tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);

   Archive->Push(tss);
}

void File::SetWriteFileTypeToImplicitVR()
{
   std::string ts = Util::DicomString(
      Document::GetTransferSyntaxValue(ImplicitVRLittleEndian).c_str() );

   ValEntry* tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);

   Archive->Push(tss);
}

void File::RestoreWriteFileType()
{
   Archive->Restore(0x0002,0x0010);
}

void File::SetWriteToLibido()
{
   ValEntry *oldRow = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntryByNumber(0x0028, 0x0010));
   ValEntry *oldCol = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntryByNumber(0x0028, 0x0011));
   
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

void File::SetWriteToNoLibido()
{
   ValEntry *recCode = dynamic_cast<ValEntry *>
                (HeaderInternal->GetDocEntryByNumber(0x0008,0x0010));
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

void File::RestoreWriteOfLibido()
{
   Archive->Restore(0x0028,0x0010);
   Archive->Restore(0x0028,0x0011);
   Archive->Restore(0x0008,0x0010);
}

ValEntry* File::CopyValEntry(uint16_t group,uint16_t element)
{
   DocEntry* oldE = HeaderInternal->GetDocEntryByNumber(group, element);
   ValEntry* newE;

   if(oldE)
   {
      newE = new ValEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetHeader()->NewValEntryByNumber(group,element);
   }

   return(newE);
}

BinEntry* File::CopyBinEntry(uint16_t group,uint16_t element)
{
   DocEntry* oldE = HeaderInternal->GetDocEntryByNumber(group, element);
   BinEntry* newE;

   if(oldE)
   {
      newE = new BinEntry(oldE->GetDictEntry());
      newE->Copy(oldE);
   }
   else
   {
      newE = GetHeader()->NewBinEntryByNumber(group,element);
   }

   return(newE);
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief Factorization for various forms of constructors.
 */
void File::Initialise()
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

uint8_t* File::GetRaw()
{
   uint8_t* raw = PixelReadConverter->GetRaw();
   if ( ! raw )
   {
      // The Raw image migth not be loaded yet:
      std::ifstream* fp = HeaderInternal->OpenFile();
      PixelReadConverter->ReadAndDecompressPixelData( fp );
      if(fp) 
         HeaderInternal->CloseFile();

      raw = PixelReadConverter->GetRaw();
      if ( ! raw )
      {
         dbg.Verbose(0, "File::GetRaw: read/decompress of "
                        "pixel data apparently went wrong.");
         return 0;
      }
   }

   return raw;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

