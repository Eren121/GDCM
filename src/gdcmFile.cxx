  /*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/13 14:56:07 $
  Version:   $Revision: 1.143 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmDebug.h"

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
   if ( HeaderInternal->IsReadable() )
   {
      ImageDataSizeRaw = ComputeDecompressedPixelDataSizeFromHeader();
      if ( HeaderInternal->HasLUT() )
      {
         ImageDataSize = 3 * ImageDataSizeRaw;
      }
      else
      {
         ImageDataSize = ImageDataSizeRaw;
      }



                                                                                
      // Just in case some access to a Header element requires disk access.
      // Note: gdcmDocument::Fp is leaved open after OpenFile. 
      (void)HeaderInternal->OpenFile();
      // Number of Bits Allocated for storing a Pixel is defaulted to 16
      // when absent from the header.
      int numberBitsAllocated = HeaderInternal->GetBitsAllocated();
      if ( numberBitsAllocated == 0 )
      {
         numberBitsAllocated = 16;
      }
      PixelConverter.SetBitsAllocated( numberBitsAllocated );
                                                                                
      // Number of "Bits Stored" defaulted to number of "Bits Allocated"
      // when absent from the header.
      int numberBitsStored = HeaderInternal->GetBitsStored();
      if ( numberBitsStored == 0 )
      {
         numberBitsStored = numberBitsAllocated;
      }
      PixelConverter.SetBitsStored( numberBitsStored );
                                                                                
      // High Bit Position
      int highBitPosition = HeaderInternal->GetHighBitPosition();
      if ( highBitPosition == 0 )
      {
         highBitPosition = numberBitsAllocated - 1;
      }
      PixelConverter.SetHighBitPosition( highBitPosition );
                                                                                
                                                                                
      PixelConverter.SetXSize( HeaderInternal->GetXSize() );
      PixelConverter.SetYSize( HeaderInternal->GetYSize() );
      PixelConverter.SetZSize( HeaderInternal->GetZSize() );
      PixelConverter.SetSamplesPerPixel( HeaderInternal->GetSamplesPerPixel() );
      PixelConverter.SetPixelSize( HeaderInternal->GetPixelSize() );
      PixelConverter.SetPixelSign( HeaderInternal->IsSignedPixelData() );
      PixelConverter.SetSwapCode( HeaderInternal->GetSwapCode() );
      PixelConverter.SetIsUncompressed(
         ! HeaderInternal->IsDicomV3()
        || HeaderInternal->IsImplicitVRLittleEndianTransferSyntax()
        || HeaderInternal->IsExplicitVRLittleEndianTransferSyntax()
        || HeaderInternal->IsExplicitVRBigEndianTransferSyntax()
        || HeaderInternal->IsDeflatedExplicitVRLittleEndianTransferSyntax() );
      PixelConverter.SetIsJPEG2000( HeaderInternal->IsJPEG2000() );
      PixelConverter.SetIsJPEGLossless( HeaderInternal->IsJPEGLossless() );
      PixelConverter.SetIsRLELossless(
                        HeaderInternal->IsRLELossLessTransferSyntax() );
      PixelConverter.SetPixelOffset( HeaderInternal->GetPixelOffset() );
      PixelConverter.SetPixelDataLength( HeaderInternal->GetPixelAreaLength() );
      PixelConverter.SetRLEInfo( &(HeaderInternal->RLEInfo) );
      PixelConverter.SetJPEGInfo( &(HeaderInternal->JPEGInfo) );
      PixelConverter.SetDecompressedSize( ImageDataSize );

      PixelConverter.SetPlanarConfiguration(
          HeaderInternal->GetPlanarConfiguration() );
      PixelConverter.SetIsMonochrome( HeaderInternal->IsMonochrome() );
      PixelConverter.SetIsPaletteColor( HeaderInternal->IsPaletteColor() );
      PixelConverter.SetIsYBRFull( HeaderInternal->IsYBRFull() );
                                                                                
      HeaderInternal->CloseFile();

   }
   SaveInitialValues();
}

/**
 * \brief canonical destructor
 * \note  If the Header was created by the File constructor,
 *        it is destroyed by the File
 */
File::~File()
{ 
   if( SelfHeader )
   {
      delete HeaderInternal;
   }
   HeaderInternal = 0;

   DeleteInitialValues();
}

/**
 * \brief Sets some initial values for the Constructor
 * \warning not end user intended
 */
void File::SaveInitialValues()
{ 

   PixelRead  = -1; // no ImageData read yet.
   LastAllocatedPixelDataLength = 0;
   Pixel_Data = 0;

   InitialSpp = "";     
   InitialPhotInt = "";
   InitialPlanConfig = "";
   InitialBitsAllocated = "";
   InitialHighBit = "";
  
   InitialRedLUTDescr   = 0;
   InitialGreenLUTDescr = 0;
   InitialBlueLUTDescr  = 0;
   InitialRedLUTData    = 0;
   InitialGreenLUTData  = 0;
   InitialBlueLUTData   = 0; 
                
   if ( HeaderInternal->IsReadable() )
   {
      // the following values *may* be modified 
      // by File::GetImageDataIntoVectorRaw
      // we save their initial value.
      InitialSpp           = HeaderInternal->GetEntryByNumber(0x0028,0x0002);
      InitialPhotInt       = HeaderInternal->GetEntryByNumber(0x0028,0x0004);
      InitialPlanConfig    = HeaderInternal->GetEntryByNumber(0x0028,0x0006);
      
      InitialBitsAllocated = HeaderInternal->GetEntryByNumber(0x0028,0x0100);
      InitialHighBit       = HeaderInternal->GetEntryByNumber(0x0028,0x0102);

      // the following entries *may* be removed from the H table
      // (NOT deleted ...) by File::GetImageDataIntoVectorRaw  
      // we keep a pointer on them.
      InitialRedLUTDescr   = HeaderInternal->GetDocEntryByNumber(0x0028,0x1101);
      InitialGreenLUTDescr = HeaderInternal->GetDocEntryByNumber(0x0028,0x1102);
      InitialBlueLUTDescr  = HeaderInternal->GetDocEntryByNumber(0x0028,0x1103);

      InitialRedLUTData    = HeaderInternal->GetDocEntryByNumber(0x0028,0x1201);
      InitialGreenLUTData  = HeaderInternal->GetDocEntryByNumber(0x0028,0x1202);
      InitialBlueLUTData   = HeaderInternal->GetDocEntryByNumber(0x0028,0x1203); 
   }
}

/**
 * \brief restores some initial values
 * \warning not end user intended
 */
void File::RestoreInitialValues()
{   
   if ( HeaderInternal->IsReadable() )
   {      
      // the following values *may* have been modified 
      // by File::GetImageDataIntoVectorRaw
      // we restore their initial value.
      if ( InitialSpp != "")
         HeaderInternal->SetEntryByNumber(InitialSpp,0x0028,0x0002);
      if ( InitialPhotInt != "")
         HeaderInternal->SetEntryByNumber(InitialPhotInt,0x0028,0x0004);
      if ( InitialPlanConfig != "")

         HeaderInternal->SetEntryByNumber(InitialPlanConfig,0x0028,0x0006);
      if ( InitialBitsAllocated != "")
          HeaderInternal->SetEntryByNumber(InitialBitsAllocated,0x0028,0x0100);
      if ( InitialHighBit != "")
          HeaderInternal->SetEntryByNumber(InitialHighBit,0x0028,0x0102);
               
      // the following entries *may* be have been removed from the H table
      // (NOT deleted ...) by File::GetImageDataIntoVectorRaw  
      // we restore them.

      if (InitialRedLUTDescr)
         HeaderInternal->AddEntry(InitialRedLUTDescr);
      if (InitialGreenLUTDescr)
         HeaderInternal->AddEntry(InitialGreenLUTDescr);
      if (InitialBlueLUTDescr)
         HeaderInternal->AddEntry(InitialBlueLUTDescr);

      if (InitialRedLUTData)
         HeaderInternal->AddEntry(InitialBlueLUTDescr);
      if (InitialGreenLUTData)
         HeaderInternal->AddEntry(InitialGreenLUTData);
      if (InitialBlueLUTData)
         HeaderInternal->AddEntry(InitialBlueLUTData);
   }
}

/**
 * \brief delete initial values (il they were saved)
 *        of InitialLutDescriptors and InitialLutData
 */
void File::DeleteInitialValues()
{ 

// InitialLutDescriptors and InitialLutData
// will have to be deleted if the don't belong any longer
// to the Header H table when the header is deleted...

   if ( InitialRedLUTDescr )           
      delete InitialRedLUTDescr;
  
   if ( InitialGreenLUTDescr )
      delete InitialGreenLUTDescr;
      
   if ( InitialBlueLUTDescr )      
      delete InitialBlueLUTDescr; 
       
   if ( InitialRedLUTData )      
      delete InitialRedLUTData;
   
   if ( InitialGreenLUTData != NULL)
      delete InitialGreenLUTData;
      
   if ( InitialBlueLUTData != NULL)      
      delete InitialBlueLUTData;      
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public

/**
 * \brief     computes the length (in bytes) we must ALLOCATE to receive the
 *            image(s) pixels (multiframes taken into account) 
 * \warning : it is NOT the group 7FE0 length
 *          (no interest for compressed images).
 */
int File::ComputeDecompressedPixelDataSizeFromHeader()
{
   // see PS 3.3-2003 : C.7.6.3.2.1  
   // 
   //   MONOCHROME1
   //   MONOCHROME2
   //   PALETTE COLOR
   //   RGB
   //   HSV  (Retired)
   //   ARGB (Retired)
   //   CMYK (Retired)
   //   YBR_FULL
   //   YBR_FULL_422 (no LUT, no Palette)
   //   YBR_PARTIAL_422
   //   YBR_ICT
   //   YBR_RCT

   // LUT's
   // ex : gdcm-US-ALOKA-16.dcm
   // 0028|1221 [OW]   [Segmented Red Palette Color Lookup Table Data]
   // 0028|1222 [OW]   [Segmented Green Palette Color Lookup Table Data]  
   // 0028|1223 [OW]   [Segmented Blue Palette Color Lookup Table Data]

   // ex  : OT-PAL-8-face.dcm
   // 0028|1201 [US]   [Red Palette Color Lookup Table Data]
   // 0028|1202 [US]   [Green Palette Color Lookup Table Data]
   // 0028|1203 [US]   [Blue Palette Color Lookup Table Data]

   int numberBitsAllocated = HeaderInternal->GetBitsAllocated();
   // Number of "Bits Allocated" is fixed to 16 when:
   //  - it is not defined (i.e. it's value is 0)
   //  - it's 12, since we will expand the image to 16 bits (see
   //    PixelConvert::ConvertDecompress12BitsTo16Bits() )
   if ( ( numberBitsAllocated == 0 ) || ( numberBitsAllocated == 12 ) )
   {
      numberBitsAllocated = 16;
   } 

   int DecompressedSize = HeaderInternal->GetXSize()
                        * HeaderInternal->GetYSize() 
                        * HeaderInternal->GetZSize()
                        * ( numberBitsAllocated / 8 )
                        * HeaderInternal->GetSamplesPerPixel();
   
   return DecompressedSize;
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
   // FIXME (Mathieu)
   // I need to deallocate Pixel_Data before doing any allocation:
   
   if ( Pixel_Data )
     if ( LastAllocatedPixelDataLength != ImageDataSize ) 
        free(Pixel_Data);
   if ( !Pixel_Data )
      Pixel_Data = new uint8_t[ImageDataSize];
    
   if ( Pixel_Data )
   {
      LastAllocatedPixelDataLength = ImageDataSize;

      // we load the pixels (and transform grey level + LUT into RGB)
      GetImageDataIntoVector(Pixel_Data, ImageDataSize);

      // We say the value *is* loaded.
      GetHeader()->SetEntryByNumber( GDCM_BINLOADED,
         GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel());

      // Will be 7fe0, 0010 in standard case
      GetHeader()->SetEntryBinAreaByNumber( Pixel_Data, 
         GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel()); 
   }      
   PixelRead = 0; // no PixelRaw

   return Pixel_Data;
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
   GetImageDataIntoVectorRaw (destination, maxSize);
   PixelRead = 0 ; // =0 : no ImageDataRaw 
   if ( !HeaderInternal->HasLUT() )
   {
      return ImageDataSize;
   }
                            
   // from Lut R + Lut G + Lut B
   uint8_t *newDest = new uint8_t[ImageDataSize];
   uint8_t *a       = (uint8_t *)destination;
   uint8_t *lutRGBA = HeaderInternal->GetLUTRGBA();

   if ( lutRGBA )
   {
      int j;
      // move Gray pixels to temp area  
      memmove(newDest, destination, ImageDataSizeRaw);
      for (size_t i=0; i<ImageDataSizeRaw; ++i) 
      {
         // Build RGB Pixels
         j    = newDest[i]*4;
         *a++ = lutRGBA[j];
         *a++ = lutRGBA[j+1];
         *a++ = lutRGBA[j+2];
      }
      delete[] newDest;
    
   // now, it's an RGB image
   // Lets's write it in the Header

   // FIXME : Better use CreateOrReplaceIfExist ?

   std::string spp = "3";        // Samples Per Pixel
   HeaderInternal->SetEntryByNumber(spp,0x0028,0x0002);
   std::string rgb = "RGB ";     // Photometric Interpretation
   HeaderInternal->SetEntryByNumber(rgb,0x0028,0x0004);
   std::string planConfig = "0"; // Planar Configuration
   HeaderInternal->SetEntryByNumber(planConfig,0x0028,0x0006);

   }
   else  // GetLUTRGBA() failed
   { 
      // (gdcm-US-ALOKA-16.dcm), contains Segmented xxx Palette Color 
      // that are *more* than 65535 long ?!? 
      // No idea how to manage such an image !
      // Need to make RGB Pixels (?) from grey Pixels (?!) and Gray Lut  (!?!)
      // It seems that *no Dicom Viewer* has any idea :-(
        
      std::string photomInterp = "MONOCHROME1 ";  // Photometric Interpretation
      HeaderInternal->SetEntryByNumber(photomInterp,0x0028,0x0004);
   } 

   /// \todo Drop Palette Color out of the Header?
   return ImageDataSize; 
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
   size_t imgDataSize;
   if ( HeaderInternal->HasLUT() )
      /// \todo Let Header user a chance to get the right value
      imgDataSize = ImageDataSizeRaw;
   else 
      imgDataSize = ImageDataSize;
    
   // FIXME (Mathieu)
   // I need to deallocate Pixel_Data before doing any allocation:
   
   if ( Pixel_Data )
      if ( LastAllocatedPixelDataLength != imgDataSize )
         free(Pixel_Data);
   if ( !Pixel_Data ) 
      Pixel_Data = new uint8_t[imgDataSize];

   if ( Pixel_Data )
   {
      LastAllocatedPixelDataLength = imgDataSize;
      
      // we load the pixels ( grey level or RGB, but NO transformation)
       GetImageDataIntoVectorRaw(Pixel_Data, imgDataSize);

      // We say the value *is* loaded.
      GetHeader()->SetEntryByNumber( GDCM_BINLOADED,
         GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel());
 
      // will be 7fe0, 0010 in standard cases
      GetHeader()->SetEntryBinAreaByNumber(Pixel_Data, 
         GetHeader()->GetGrPixel(), GetHeader()->GetNumPixel());
   } 
   PixelRead = 1; // PixelRaw

   return Pixel_Data;
}

/**
 * \brief   Copies at most MaxSize bytes of pixel data to caller's
 *          memory space.
 * \warning This function was designed to avoid people that want to build
 *          a volume from an image stack to need first to get the image pixels 
 *          and then move them to the volume area.
 *          It's absolutely useless for any VTK user since vtk chooses 
 *          to invert the lines of an image, that is the last line comes first
 *          (for some axis related reasons?). Hence he will have 
 *          to load the image line by line, starting from the end.
 *          VTK users hace to call GetImageData
 * \warning DOES NOT transform the Grey Plane + Palette Color (if any) 
 *                   into a single RGB Pixels Plane
 *          the (VTK) user will manage the palettes
 *     
 * @param   destination Address (in caller's memory space) at which the
 *          pixel data should be copied
 * @param   maxSize Maximum number of bytes to be copied. When MaxSize
 *          is not sufficient to hold the pixel data the copy is not
 *          executed (i.e. no partial copy).
 * @return  On success, the number of bytes actually copied. Zero on
 *          failure e.g. MaxSize is lower than necessary.
 */
void File::GetImageDataIntoVectorRaw (void* destination, size_t maxSize)
{
  // we save the initial values of the following
  // in order to be able to restore the header in a disk-consistent state
  // (if user asks twice to get the pixels from disk)

   if ( PixelRead != -1 ) // File was "read" before
   {  
      RestoreInitialValues(); 
   }
   
   PixelRead = 1 ; // PixelRaw
    
   if ( ImageDataSize > maxSize )
   {
      dbg.Verbose(0, "File::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return;
   }

   FILE* fp = HeaderInternal->OpenFile();
   PixelConverter.ReadAndDecompressPixelData( destination, fp );
   HeaderInternal->CloseFile();
                                                                                
   if ( ! PixelConverter.HandleColor( (uint8_t*)destination ) )
   {
      return;
   }

///////////////////////////////////////////////////
   // now, it's an RGB image
   // Lets's write it in the Header
 
   // Droping Palette Color out of the Header
   // has been moved to the Write process.

   // TODO : move 'values' modification to the write process
   //      : save also (in order to be able to restore)
   //      : 'high bit' -when not equal to 'bits stored' + 1
   //      : 'bits allocated', when it's equal to 12 ?!

   std::string spp = "3";            // Samples Per Pixel
   std::string photInt = "RGB ";     // Photometric Interpretation
   std::string planConfig = "0";     // Planar Configuration
     
   HeaderInternal->SetEntryByNumber(spp,0x0028,0x0002);
   HeaderInternal->SetEntryByNumber(photInt,0x0028,0x0004);
   HeaderInternal->SetEntryByNumber(planConfig,0x0028,0x0006);
 
   return; 
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
   HeaderInternal->SetImageDataSize( expectedSize );
// FIXME : if already allocated, memory leak !
   Pixel_Data     = inData;
   ImageDataSize = ImageDataSizeRaw = expectedSize;
   PixelRead     = 1;
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
   FILE* fp1 = fopen(fileName.c_str(), "wb");
   if (fp1 == NULL)
   {
      printf("Fail to open (write) file [%s] \n", fileName.c_str());
      return false;
   }
   fwrite (Pixel_Data, ImageDataSize, 1, fp1);
   fclose (fp1);

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
   return WriteBase(fileName, ImplicitVR);
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
   return WriteBase(fileName, ExplicitVR);
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
   return WriteBase(fileName, ACR);
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
   if ( PixelRead == -1 && type != ExplicitVR)
   {
      return false;
   }

   FILE* fp1 = fopen(fileName.c_str(), "wb");
   if (fp1 == NULL)
   {
      printf("Failed to open (write) File [%s] \n", fileName.c_str());
      return false;
   }

   if ( type == ImplicitVR || type == ExplicitVR )
   {
      // writing Dicom File Preamble
      uint8_t* filePreamble = new uint8_t[128];
      memset(filePreamble, 0, 128);
      fwrite(filePreamble, 128, 1, fp1);
      fwrite("DICM", 4, 1, fp1);

      delete[] filePreamble;
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   //
   // if recognition code tells us we dealt with a LibIDO image
   // we reproduce on disk the switch between lineNumber and columnNumber
   // just before writting ...
   
   /// \todo the best trick would be *change* the recognition code
   ///       but pb expected if user deals with, e.g. COMPLEX images

   std::string rows, columns; 
   if ( HeaderInternal->GetFileType() == ACR_LIBIDO)
   {
      rows    = HeaderInternal->GetEntryByNumber(0x0028, 0x0010);
      columns = HeaderInternal->GetEntryByNumber(0x0028, 0x0011);

      HeaderInternal->SetEntryByNumber(columns,  0x0028, 0x0010);
      HeaderInternal->SetEntryByNumber(rows   ,  0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
      
   uint16_t grPixel  = HeaderInternal->GetGrPixel();
   uint16_t numPixel = HeaderInternal->GetNumPixel();;
          
   DocEntry* PixelElement = 
      GetHeader()->GetDocEntryByNumber(grPixel, numPixel);  
 
   if ( PixelRead == 1 )
   {
      // we read pixel 'as is' (no tranformation LUT -> RGB)
      PixelElement->SetLength( ImageDataSizeRaw );
   }
   else if ( PixelRead == 0 )
   {
      // we tranformed GrayLevel pixels + LUT into RGB Pixel
      PixelElement->SetLength( ImageDataSize );
   }
 
   HeaderInternal->Write(fp1, type);

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting

   if ( HeaderInternal->GetFileType() == ACR_LIBIDO )
   {
      HeaderInternal->SetEntryByNumber(rows   , 0x0028, 0x0010);
      HeaderInternal->SetEntryByNumber(columns, 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
   
   // fwrite(Pixel_Data, ImageDataSize, 1, fp1);  // should be useless, now
   fclose (fp1);

   return true;
}

} // end namespace gdcm

