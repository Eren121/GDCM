  /*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmFile.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/10 16:44:00 $
  Version:   $Revision: 1.138 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmDebug.h"
#include "gdcmPixelConvert.h"

typedef std::pair<TagDocEntryHT::iterator,TagDocEntryHT::iterator> IterHT;

//-------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (gdcmHeader only deals with the ... header)
 *        Opens (in read only and when possible) an existing file and checks
 *        for DICOM compliance. Returns NULL on failure.
 *        It will be up to the user to load the pixels into memory
 *        (see GetImageData, GetImageDataRaw)
 * \note  the in-memory representation of all available tags found in
 *        the DICOM header is post-poned to first header information access.
 *        This avoid a double parsing of public part of the header when
 *        user sets an a posteriori shadow dictionary (efficiency can be
 *        seen as a side effect).   
 * @param header already built gdcmHeader
 */
gdcmFile::gdcmFile(gdcmHeader *header)
{
   Header     = header;
   SelfHeader = false;
   Initialise();
}

/**
 * \brief Constructor dedicated to deal with the *pixels* area of a ACR/DICOMV3
 *        file (gdcmHeader only deals with the ... header)
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
gdcmFile::gdcmFile(std::string const & filename )
{
   Header = new gdcmHeader( filename );
   SelfHeader = true;
   Initialise();
}

/**
 * \brief Factorization for various forms of constructors.
 */
void gdcmFile::Initialise()
{
   if ( Header->IsReadable() )
   {
      ImageDataSizeRaw = ComputeDecompressedPixelDataSizeFromHeader();
      if ( Header->HasLUT() )
      {
         ImageDataSize = 3 * ImageDataSizeRaw;
      }
      else
      {
         ImageDataSize = ImageDataSizeRaw;
      }
   }
   SaveInitialValues();
}

/**
 * \brief canonical destructor
 * \note  If the gdcmHeader was created by the gdcmFile constructor,
 *        it is destroyed by the gdcmFile
 */
gdcmFile::~gdcmFile()
{ 
   if( SelfHeader )
   {
      delete Header;
   }
   Header = 0;

   DeleteInitialValues();
}

/**
 * \brief Sets some initial values for the Constructor
 * \warning not end user intended
 */
void gdcmFile::SaveInitialValues()
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
                
   if ( Header->IsReadable() )
   {
      // the following values *may* be modified 
      // by gdcmFile::GetImageDataIntoVectorRaw
      // we save their initial value.
      InitialSpp           = Header->GetEntryByNumber(0x0028,0x0002);
      InitialPhotInt       = Header->GetEntryByNumber(0x0028,0x0004);
      InitialPlanConfig    = Header->GetEntryByNumber(0x0028,0x0006);
      
      InitialBitsAllocated = Header->GetEntryByNumber(0x0028,0x0100);
      InitialHighBit       = Header->GetEntryByNumber(0x0028,0x0102);

      // the following entries *may* be removed from the H table
      // (NOT deleted ...) by gdcmFile::GetImageDataIntoVectorRaw  
      // we keep a pointer on them.
      InitialRedLUTDescr   = Header->GetDocEntryByNumber(0x0028,0x1101);
      InitialGreenLUTDescr = Header->GetDocEntryByNumber(0x0028,0x1102);
      InitialBlueLUTDescr  = Header->GetDocEntryByNumber(0x0028,0x1103);

      InitialRedLUTData    = Header->GetDocEntryByNumber(0x0028,0x1201);
      InitialGreenLUTData  = Header->GetDocEntryByNumber(0x0028,0x1202);
      InitialBlueLUTData   = Header->GetDocEntryByNumber(0x0028,0x1203); 
   }
}

/**
 * \brief restores some initial values
 * \warning not end user intended
 */
void gdcmFile::RestoreInitialValues()
{   
   if ( Header->IsReadable() )
   {      
      // the following values *may* have been modified 
      // by gdcmFile::GetImageDataIntoVectorRaw
      // we restore their initial value.
      if ( InitialSpp != "")
         Header->SetEntryByNumber(InitialSpp,0x0028,0x0002);
      if ( InitialPhotInt != "")
         Header->SetEntryByNumber(InitialPhotInt,0x0028,0x0004);
      if ( InitialPlanConfig != "")

         Header->SetEntryByNumber(InitialPlanConfig,0x0028,0x0006);
      if ( InitialBitsAllocated != "")
          Header->SetEntryByNumber(InitialBitsAllocated,0x0028,0x0100);
      if ( InitialHighBit != "")
          Header->SetEntryByNumber(InitialHighBit,0x0028,0x0102);
               
      // the following entries *may* be have been removed from the H table
      // (NOT deleted ...) by gdcmFile::GetImageDataIntoVectorRaw  
      // we restore them.

      if (InitialRedLUTDescr)
         Header->AddEntry(InitialRedLUTDescr);
      if (InitialGreenLUTDescr)
         Header->AddEntry(InitialGreenLUTDescr);
      if (InitialBlueLUTDescr)
         Header->AddEntry(InitialBlueLUTDescr);

      if (InitialRedLUTData)
         Header->AddEntry(InitialBlueLUTDescr);
      if (InitialGreenLUTData)
         Header->AddEntry(InitialGreenLUTData);
      if (InitialBlueLUTData)
         Header->AddEntry(InitialBlueLUTData);
   }
}

/**
 * \brief delete initial values (il they were saved)
 *        of InitialLutDescriptors and InitialLutData
 */
void gdcmFile::DeleteInitialValues()
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
int gdcmFile::ComputeDecompressedPixelDataSizeFromHeader()
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

   // Number of "Bits Allocated"
   int numberBitsAllocated = Header->GetBitsAllocated();
   if ( ( numberBitsAllocated == 0 ) || ( numberBitsAllocated == 12 ) )
   {
      numberBitsAllocated = 16;
   } 

   int DecompressedSize = Header->GetXSize()
                        * Header->GetYSize() 
                        * Header->GetZSize()
                        * ( numberBitsAllocated / 8 )
                        * Header->GetSamplesPerPixel();
   
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
uint8_t* gdcmFile::GetImageData()
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
size_t gdcmFile::GetImageDataIntoVector (void* destination, size_t maxSize)
{
   GetImageDataIntoVectorRaw (destination, maxSize);
   PixelRead = 0 ; // =0 : no ImageDataRaw 
   if ( !Header->HasLUT() )
   {
      return ImageDataSize;
   }
                            
   // from Lut R + Lut G + Lut B
   uint8_t *newDest = new uint8_t[ImageDataSize];
   uint8_t *a       = (uint8_t *)destination;
   uint8_t *lutRGBA = Header->GetLUTRGBA();

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
   Header->SetEntryByNumber(spp,0x0028,0x0002);
   std::string rgb = "RGB ";     // Photometric Interpretation
   Header->SetEntryByNumber(rgb,0x0028,0x0004);
   std::string planConfig = "0"; // Planar Configuration
   Header->SetEntryByNumber(planConfig,0x0028,0x0006);

   }
   else  // GetLUTRGBA() failed
   { 
      // (gdcm-US-ALOKA-16.dcm), contains Segmented xxx Palette Color 
      // that are *more* than 65535 long ?!? 
      // No idea how to manage such an image !
      // Need to make RGB Pixels (?) from grey Pixels (?!) and Gray Lut  (!?!)
      // It seems that *no Dicom Viewer* has any idea :-(
        
      std::string photomInterp = "MONOCHROME1 ";  // Photometric Interpretation
      Header->SetEntryByNumber(photomInterp,0x0028,0x0004);
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
uint8_t* gdcmFile::GetImageDataRaw ()
{
   size_t imgDataSize;
   if ( Header->HasLUT() )
      /// \todo Let gdcmHeader user a chance to get the right value
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
size_t gdcmFile::GetImageDataIntoVectorRaw (void* destination, size_t maxSize)
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
      dbg.Verbose(0, "gdcmFile::GetImageDataIntoVector: pixel data bigger"
                     "than caller's expected MaxSize");
      return (size_t)0;
   }

   ReadPixelData( destination );

   // Number of Bits Allocated for storing a Pixel
   int numberBitsAllocated = Header->GetBitsAllocated();
   if ( numberBitsAllocated == 0 )
   {
      numberBitsAllocated = 16;
   }

   // Number of Bits actually used
   int numberBitsStored = Header->GetBitsStored();
   if ( numberBitsStored == 0 )
   {
      numberBitsStored = numberBitsAllocated;
   }

   // High Bit Position
   int highBitPosition = Header->GetHighBitPosition();
   if ( highBitPosition == 0 )
   {
      highBitPosition = numberBitsAllocated - 1;
   }

   bool signedPixel = Header->IsSignedPixelData();

   gdcmPixelConvert::ConvertReorderEndianity(
                         (uint8_t*) destination,
                         ImageDataSize,
                         numberBitsStored,
                         numberBitsAllocated,
                         Header->GetSwapCode(),
                         signedPixel );

   gdcmPixelConvert::ConvertReArrangeBits(
                         (uint8_t*) destination,
                         ImageDataSize,
                         numberBitsStored,
                         numberBitsAllocated,
                         highBitPosition );

#ifdef GDCM_DEBUG
   FILE*  DebugFile;
   DebugFile = fopen( "SpuriousFile.RAW", "wb" );
   fwrite( PixelConvertor.GetUncompressed(),
           PixelConvertor.GetUncompressedsSize(),
           1, DebugFile );
   fclose( DebugFile );
#endif //GDCM_DEBUG

// SPLIT ME
//////////////////////////////////
// Deal with the color
   
   // Monochrome pictures don't require color intervention
   if ( Header->IsMonochrome() )
   {
      return ImageDataSize; 
   }
      
   // Planar configuration = 0 : Pixels are already RGB
   // Planar configuration = 1 : 3 planes : R, G, B
   // Planar configuration = 2 : 1 gray Plane + 3 LUT

   // Well ... supposed to be !
   // See US-PAL-8-10x-echo.dcm: PlanarConfiguration=0,
   //                            PhotometricInterpretation=PALETTE COLOR
   // and heuristic has to be found :-( 

   int planConf = Header->GetPlanarConfiguration();

   // Planar configuration = 2 ==> 1 gray Plane + 3 LUT
   //   ...and...
   // whatever the Planar Configuration might be, "PALETTE COLOR "
   // implies that we deal with the palette. 
   if ( ( planConf == 2 ) || Header->IsPaletteColor() )
   {
      return ImageDataSize;
   }

   // When planConf is 0, pixels are allready in RGB

   if ( planConf == 1 )
   {
      uint8_t* newDest = new uint8_t[ImageDataSize];
      // Warning : YBR_FULL_422 acts as RGB
      if ( Header->IsYBRFull() )
      {
         ConvertYcBcRPlanesToRGBPixels((uint8_t*)destination, newDest);
      }
      else
      {
         ConvertRGBPlanesToRGBPixels((uint8_t*)destination, newDest);
      }
      memmove(destination, newDest, ImageDataSize);
      delete[] newDest;
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
     
   Header->SetEntryByNumber(spp,0x0028,0x0002);
   Header->SetEntryByNumber(photInt,0x0028,0x0004);
   Header->SetEntryByNumber(planConfig,0x0028,0x0006);
 
   return ImageDataSize; 
}

/**
 * \brief   Convert (Y plane, cB plane, cR plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void gdcmFile::ConvertYcBcRPlanesToRGBPixels(uint8_t* source,
                                             uint8_t* destination)
{
   // to see the tricks about YBR_FULL, YBR_FULL_422, 
   // YBR_PARTIAL_422, YBR_ICT, YBR_RCT have a look at :
   // ftp://medical.nema.org/medical/dicom/final/sup61_ft.pdf
   // and be *very* affraid
   //
   int l        = Header->GetXSize() * Header->GetYSize();
   int nbFrames = Header->GetZSize();

   uint8_t* a = source;
   uint8_t* b = source + l;
   uint8_t* c = source + l + l;
   double R, G, B;

   /// \todo : Replace by the 'well known' integer computation
   ///         counterpart. Refer to
   ///            http://lestourtereaux.free.fr/papers/data/yuvrgb.pdf
   ///         for code optimisation.
 
   for (int i = 0; i < nbFrames; i++)
   {
      for (int j = 0; j < l; j++)
      {
         R = 1.164 *(*a-16) + 1.596 *(*c -128) + 0.5;
         G = 1.164 *(*a-16) - 0.813 *(*c -128) - 0.392 *(*b -128) + 0.5;
         B = 1.164 *(*a-16) + 2.017 *(*b -128) + 0.5;

         if (R < 0.0)   R = 0.0;
         if (G < 0.0)   G = 0.0;
         if (B < 0.0)   B = 0.0;
         if (R > 255.0) R = 255.0;
         if (G > 255.0) G = 255.0;
         if (B > 255.0) B = 255.0;

         *(destination++) = (uint8_t)R;
         *(destination++) = (uint8_t)G;
         *(destination++) = (uint8_t)B;
         a++;
         b++;
         c++;  
      }
   }
}

/**
 * \brief   Convert (Red plane, Green plane, Blue plane) to RGB pixels
 * \warning Works on all the frames at a time
 */
void gdcmFile::ConvertRGBPlanesToRGBPixels(uint8_t* source,
                                           uint8_t* destination)
{
   int l = Header->GetXSize() * Header->GetYSize() * Header->GetZSize();

   uint8_t* a = source;
   uint8_t* b = source + l;
   uint8_t* c = source + l + l;

   for (int j = 0; j < l; j++)
   {
      *(destination++) = *(a++);
      *(destination++) = *(b++);
      *(destination++) = *(c++);
   }
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
bool gdcmFile::SetImageData(uint8_t* inData, size_t expectedSize)
{
   Header->SetImageDataSize( expectedSize );
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

bool gdcmFile::WriteRawData(std::string const & fileName)
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

bool gdcmFile::WriteDcmImplVR (std::string const & fileName)
{
   return WriteBase(fileName, gdcmImplicitVR);
}

/**
* \brief Writes on disk A SINGLE Dicom file, 
 *        using the Explicit Value Representation convention
 *        NO test is performed on  processor "Endiannity". * @param fileName name of the file to be created
 *                 (any already existing file is overwritten)
 * @return false if write fails
 */

bool gdcmFile::WriteDcmExplVR (std::string const & fileName)
{
   return WriteBase(fileName, gdcmExplicitVR);
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

bool gdcmFile::WriteAcr (std::string const & fileName)
{
   return WriteBase(fileName, gdcmACR);
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
bool gdcmFile::WriteBase (std::string const & fileName, FileType type)
{
   if ( PixelRead == -1 && type != gdcmExplicitVR)
   {
      return false;
   }

   FILE* fp1 = fopen(fileName.c_str(), "wb");
   if (fp1 == NULL)
   {
      printf("Failed to open (write) File [%s] \n", fileName.c_str());
      return false;
   }

   if ( type == gdcmImplicitVR || type == gdcmExplicitVR )
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
   if ( Header->GetFileType() == gdcmACR_LIBIDO)
   {
      rows    = Header->GetEntryByNumber(0x0028, 0x0010);
      columns = Header->GetEntryByNumber(0x0028, 0x0011);

      Header->SetEntryByNumber(columns,  0x0028, 0x0010);
      Header->SetEntryByNumber(rows   ,  0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
      
   uint16_t grPixel  = Header->GetGrPixel();
   uint16_t numPixel = Header->GetNumPixel();;
          
   gdcmDocEntry* PixelElement = 
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
 
   Header->Write(fp1, type);

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to re-write ACR-LibIDO formated images
   // 
   // ...and we restore the Header to be Dicom Compliant again 
   // just after writting

   if ( Header->GetFileType() == gdcmACR_LIBIDO )
   {
      Header->SetEntryByNumber(rows   , 0x0028, 0x0010);
      Header->SetEntryByNumber(columns, 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
   
   // fwrite(Pixel_Data, ImageDataSize, 1, fp1);  // should be useless, now
   fclose (fp1);

   return true;
}

//-----------------------------------------------------------------------------
// Private
/**
 * \brief   Read pixel data from disk (optionaly decompressing) into the
 *          caller specified memory location.
 * @param   destination where the pixel data should be stored.
 *
 */
bool gdcmFile::ReadPixelData(void* destination) 
{
   FILE* fp = Header->OpenFile();

   if ( !fp )
   {
      return false;
   }
   if ( fseek(fp, Header->GetPixelOffset(), SEEK_SET) == -1 )
   {
      Header->CloseFile();
      return false;
   }

   if ( Header->GetBitsAllocated() == 12 )
   {
      gdcmPixelConvert::ConvertDecompress12BitsTo16Bits(
                                       (uint8_t*)destination, 
                                       Header->GetXSize(),
                                       Header->GetYSize(),
                                       fp);
      Header->CloseFile();
      return true;
   }

   // ----------------------  Uncompressed File
   if ( !Header->IsDicomV3()                             ||
        Header->IsImplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRBigEndianTransferSyntax()    ||
        Header->IsDeflatedExplicitVRLittleEndianTransferSyntax() )
   {
      size_t ItemRead = fread(destination, Header->GetPixelAreaLength(), 1, fp);
      Header->CloseFile();
      if ( ItemRead != 1 )
      {
         return false;
      }
      else
      {
         return true;
      }
   }

   // ---------------------- Run Length Encoding
   if ( Header->IsRLELossLessTransferSyntax() )
   {
      bool res = gdcmPixelConvert::ReadAndDecompressRLEFile(
                                      destination,
                                      Header->GetXSize(),
                                      Header->GetYSize(),
                                      Header->GetZSize(),
                                      Header->GetBitsAllocated(),
                                      &(Header->RLEInfo),
                                      fp );
      Header->CloseFile();
      return res; 
   }  
    
   // --------------- SingleFrame/Multiframe JPEG Lossless/Lossy/2000 
   int numberBitsAllocated = Header->GetBitsAllocated();
   if ( ( numberBitsAllocated == 0 ) || ( numberBitsAllocated == 12 ) )
   {
      numberBitsAllocated = 16;
   }

   bool res = gdcmPixelConvert::ReadAndDecompressJPEGFile(
                                   (uint8_t*)destination,
                                   Header->GetXSize(),
                                   Header->GetYSize(),
                                   Header->GetBitsAllocated(),
                                   Header->GetBitsStored(),
                                   Header->GetSamplesPerPixel(),
                                   Header->GetPixelSize(),
                                   Header->IsJPEG2000(),
                                   Header->IsJPEGLossless(),
                                   &(Header->JPEGInfo),
                                   fp );
   Header->CloseFile();
   return res;
}

