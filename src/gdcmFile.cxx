  /*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2004/11/25 16:35:17 $
  Version:   $Revision: 1.165 $
                                                                                
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
 * \brief Factorization for various forms of constructors.
 */
void File::Initialise()
{
   WriteMode = WMODE_DECOMPRESSED;
   WriteType = WTYPE_IMPL_VR;

   PixelConverter = new PixelConvert;
   Archive = new DocEntryArchive( HeaderInternal );

   if ( HeaderInternal->IsReadable() )
   {
      PixelConverter->GrabInformationsFromHeader( HeaderInternal );
   }

   Pixel_Data = 0;
   ImageDataSize = 0;
}

/**
 * \brief canonical destructor
 * \note  If the Header was created by the File constructor,
 *        it is destroyed by the File
 */
File::~File()
{ 
   if( PixelConverter )
   {
      delete PixelConverter;
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
   return PixelConverter->GetRGBSize();
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
   return PixelConverter->GetDecompressedSize();
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
   if ( ! GetDecompressed() )
   {
      // If the decompression failed nothing can be done.
      return 0;
   }

   if ( HeaderInternal->HasLUT() && PixelConverter->BuildRGBImage() )
   {
      return PixelConverter->GetRGB();
   }
   else
   {
      // When no LUT or LUT conversion fails, return the decompressed
      return PixelConverter->GetDecompressed();
   }
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
   if ( ! GetDecompressed() )
   {
      // If the decompression failed nothing can be done.
      return 0;
   }

   if ( HeaderInternal->HasLUT() && PixelConverter->BuildRGBImage() )
   {
      if ( PixelConverter->GetRGBSize() > maxSize )
      {
         dbg.Verbose(0, "File::GetImageDataIntoVector: pixel data bigger"
                        "than caller's expected MaxSize");
         return 0;
      }
      memcpy( destination,
              (void*)PixelConverter->GetRGB(),
              PixelConverter->GetRGBSize() );
      return PixelConverter->GetRGBSize();
   }

   // Either no LUT conversion necessary or LUT conversion failed
   if ( PixelConverter->GetDecompressedSize() > maxSize )
   {
      dbg.Verbose(0, "File::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return 0;
   }
   memcpy( destination,
           (void*)PixelConverter->GetDecompressed(),
           PixelConverter->GetDecompressedSize() );
   return PixelConverter->GetDecompressedSize();
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
   return GetDecompressed();
}

uint8_t* File::GetDecompressed()
{
   uint8_t* decompressed = PixelConverter->GetDecompressed();
   if ( ! decompressed )
   {
      // The decompressed image migth not be loaded yet:
      std::ifstream* fp = HeaderInternal->OpenFile();
      PixelConverter->ReadAndDecompressPixelData( fp );
      if(fp) 
         HeaderInternal->CloseFile();

      decompressed = PixelConverter->GetDecompressed();
      if ( ! decompressed )
      {
         dbg.Verbose(0, "File::GetDecompressed: read/decompress of "
                        "pixel data apparently went wrong.");
         return 0;
      }
   }

   return decompressed;
}

/**
 * \brief   Points the internal Pixel_Data pointer to the callers inData
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
bool File::SetImageData(uint8_t* inData, size_t expectedSize)
{
// FIXME : if already allocated, memory leak !
   Pixel_Data     = inData;
   ImageDataSize = expectedSize;
// FIXME : 7fe0, 0010 IS NOT set ...
   return true;
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
   fp1.write((char*)Pixel_Data, ImageDataSize);
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
   switch(WriteType)
   {
      case WTYPE_IMPL_VR:
         return WriteBase(fileName,ImplicitVR);
      case WTYPE_EXPL_VR:
         return WriteBase(fileName,ExplicitVR);
      case WTYPE_ACR:
         return WriteBase(fileName,ACR);
   }
   return(false);
}

/**
 * \brief Access to the underlying \ref PixelConverter RGBA LUT
 */
uint8_t* File::GetLutRGBA()
{
   return PixelConverter->GetLutRGBA();
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
bool File::WriteBase (std::string const & fileName, FileType type)
{
   switch(type)
   {
      case ImplicitVR:
         SetWriteFileTypeToImplicitVR();
         break;
      case ExplicitVR:
         SetWriteFileTypeToExplicitVR();
         break;
      case ACR:
         SetWriteFileTypeToACR();
         break;
      case ACR_LIBIDO:
         SetWriteFileTypeToACRLibido();
         break;
      default:
         SetWriteFileTypeToExplicitVR();
   }
  
   switch(WriteMode)
   {
      case WMODE_NATIVE :
         SetWriteToNative();
         break;
      case WMODE_DECOMPRESSED :
         SetWriteToDecompressed();
         break;
      case WMODE_RGB :
         SetWriteToRGB();
         break;
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   //
   // if recognition code tells us we dealt with a LibIDO image
   // we reproduce on disk the switch between lineNumber and columnNumber
   // just before writting ...
   /// \todo the best trick would be *change* the recognition code
   ///       but pb expected if user deals with, e.g. COMPLEX images
/*   if ( HeaderInternal->GetFileType() == ACR_LIBIDO)
   {
      SetWriteToLibido();
   }*/
   // ----------------- End of Special Patch ----------------

   bool check = CheckWriteIntegrity();
   if(check)
   {
      check = HeaderInternal->Write(fileName,type);
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting
/*   if ( HeaderInternal->GetFileType() == ACR_LIBIDO )
   {
      RestoreWriteFromLibido();
   }*/
   // ----------------- End of Special Patch ----------------

   RestoreWrite();
   RestoreWriteFileType();

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
   if(Pixel_Data)
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
         case WMODE_NATIVE :
            break;
         case WMODE_DECOMPRESSED :
            if( decSize!=ImageDataSize )
            {
               dbg.Verbose(0, "File::CheckWriteIntegrity: Data size is incorrect");
               //std::cerr<<"Dec : "<<decSize<<" | "<<ImageDataSize<<std::endl;
               return false;
            }
            break;
         case WMODE_RGB :
            if( rgbSize!=ImageDataSize )
            {
               dbg.Verbose(0, "File::CheckWriteIntegrity: Data size is incorrect");
               //std::cerr<<"RGB : "<<decSize<<" | "<<ImageDataSize<<std::endl;
               return false;
            }
            break;
      }
   }
   
   return true;
}

void File::SetWriteToNative()
{
   if(Pixel_Data)
   {
      BinEntry* pixel = CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
      pixel->SetValue(GDCM_BINLOADED);
      pixel->SetBinArea(Pixel_Data,false);
      pixel->SetLength(ImageDataSize);

      Archive->Push(pixel);
   }
}

void File::SetWriteToDecompressed()
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
         photInt->SetLength(14);
      }
      else
      {
         photInt->SetValue("MONOCHROME1 ");
         photInt->SetLength(12);
      }

      BinEntry* pixel = CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
      pixel->SetValue(GDCM_BINLOADED);
      if(Pixel_Data)
      {
         pixel->SetBinArea(Pixel_Data,false);
         pixel->SetLength(ImageDataSize);
      }
      else
      {
         pixel->SetBinArea(PixelConverter->GetDecompressed(),false);
         pixel->SetLength(PixelConverter->GetDecompressedSize());
      }

      Archive->Push(photInt);
      Archive->Push(pixel);
   }
}

void File::SetWriteToRGB()
{
   if(HeaderInternal->GetNumberOfScalarComponents()==3)
   {
      PixelConverter->BuildRGBImage();
      
      ValEntry* spp = CopyValEntry(0x0028,0x0002);
      spp->SetValue("3 ");
      spp->SetLength(2);

      ValEntry* planConfig = CopyValEntry(0x0028,0x0006);
      planConfig->SetValue("0 ");
      planConfig->SetLength(2);

      ValEntry* photInt = CopyValEntry(0x0028,0x0004);
      photInt->SetValue("RGB ");
      photInt->SetLength(4);

      BinEntry* pixel = CopyBinEntry(GetHeader()->GetGrPixel(),GetHeader()->GetNumPixel());
      pixel->SetValue(GDCM_BINLOADED);
      if(Pixel_Data)
      {
         pixel->SetBinArea(Pixel_Data,false);
         pixel->SetLength(ImageDataSize);
      }
      else if(PixelConverter->GetRGB())
      {
         pixel->SetBinArea(PixelConverter->GetRGB(),false);
         pixel->SetLength(PixelConverter->GetRGBSize());
      }
      else // Decompressed data
      {
         pixel->SetBinArea(PixelConverter->GetDecompressed(),false);
         pixel->SetLength(PixelConverter->GetDecompressedSize());
      }

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
         bitsAlloc->SetLength(2);

         ValEntry* bitsStored = CopyValEntry(0x0028,0x0101);
         bitsStored->SetValue("8 ");
         bitsStored->SetLength(2);

         ValEntry* highBit = CopyValEntry(0x0028,0x0102);
         highBit->SetValue("7 ");
         highBit->SetLength(2);

         Archive->Push(bitsAlloc);
         Archive->Push(bitsStored);
         Archive->Push(highBit);
      }
   }
   else
   {
      SetWriteToDecompressed();
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

void File::SetWriteFileTypeToACRLibido()
{
   SetWriteFileTypeToACR();
}

void File::SetWriteFileTypeToExplicitVR()
{
   std::string ts = Util::DicomString( 
      Document::GetTransferSyntaxValue(ExplicitVRLittleEndian).c_str() );

   ValEntry* tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);
   tss->SetLength(ts.length());

   Archive->Push(tss);
}

void File::SetWriteFileTypeToImplicitVR()
{
   std::string ts = Util::DicomString(
      Document::GetTransferSyntaxValue(ImplicitVRLittleEndian).c_str() );

   ValEntry* tss = CopyValEntry(0x0002,0x0010);
   tss->SetValue(ts);
   tss->SetLength(ts.length());
}

void File::RestoreWriteFileType()
{
   Archive->Restore(0x0002,0x0010);
}

void File::SetWriteToLibido()
{
   ValEntry *oldRow = dynamic_cast<ValEntry *>(HeaderInternal->GetDocEntryByNumber(0x0028, 0x0010));
   ValEntry *oldCol = dynamic_cast<ValEntry *>(HeaderInternal->GetDocEntryByNumber(0x0028, 0x0011));
   
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
}

void File::RestoreWriteFromLibido()
{
   Archive->Restore(0x0028,0x0010);
   Archive->Restore(0x0028,0x0011);
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
// Private
/**
 * \brief Set the pixel datas in the good entry of the Header
 */
void File::SetPixelData(uint8_t* data)
{
   GetHeader()->SetEntryByNumber( GDCM_BINLOADED,
      GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel());

   // Will be 7fe0, 0010 in standard case
   DocEntry* currentEntry = GetHeader()->GetDocEntryByNumber(GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel());
   if ( currentEntry )
   {
      if ( BinEntry* binEntry = dynamic_cast<BinEntry *>(currentEntry) )
         // Flag is to false because datas are kept in the gdcmPixelConvert
         binEntry->SetBinArea( data, false );
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm

