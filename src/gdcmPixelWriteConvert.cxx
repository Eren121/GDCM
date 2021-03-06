/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelWriteConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2008/04/10 12:15:36 $
  Version:   $Revision: 1.25 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDebug.h"
#include "gdcmPixelWriteConvert.h"
#include "gdcmFile.h"
#include "gdcmUtil.h"

#include <vector>
#include <stdlib.h> // abs

#define WITHOFFSETTABLE 1

namespace GDCM_NAME_SPACE
{
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

// Constructor / Destructor
/**
 * \brief Constructor
 */
PixelWriteConvert::PixelWriteConvert() 
{
   ReadData     = 0;
   ReadDataSize = 0;

   UserData     = 0;
   UserDataSize = 0;
   Compressed   = false;
}

/**
 * \brief Destructor
 */
PixelWriteConvert::~PixelWriteConvert() 
{
   gdcmDebugMacro("PixelWriteConvert::~PixelWriteConvert()" );
   if( Compressed )
     {
     delete[] UserData;
     }
}

/**
 * \brief   sets Read Data (and size)
 * @param   data data (uint8_t is for prototyping. if your data is not uint8_t
 *                     just cast the pointer for calling the method)
 * @param   size data size, in bytes
 */
void PixelWriteConvert::SetReadData(uint8_t *data, size_t size)
{
   ReadData = data;
   ReadDataSize = size;
}

/**
 * \brief   Sets the internal pointer to the caller's inData
 *          image representation, WITHOUT COPYING THE DATA.
 *          - 'image' Pixels are presented as C-like 2D arrays : line per line.
 *          - 'volume'Pixels are presented as C-like 3D arrays : plane per plane 
 * \warning Since the pixels are not copied, it is the caller's responsability
 *          not to deallocate its data before gdcm uses them (e.g. with
 *          the Write() method )
 * @param   data data (uint8_t is for prototyping. if your data is not uint8_t
 *                     just cast the pointer for calling the method)
 * @param   size size, in bytes.
 */
void PixelWriteConvert::SetUserData(uint8_t *data, size_t size)
{
   UserData = data;
   UserDataSize = size;
}

/**
 * \brief   Get Data (UserData or ReadData)
 * @return  data (uint8_t is for prototyping. if your data is *not* uint8_t
 *                just cast the returned pointer)
 */
uint8_t *PixelWriteConvert::GetData()
{
   if ( UserData )
   {
      return UserData;
   }
   else
   {
      return ReadData;
   }
}

/**
 * \brief   Get Data Size (UserData or ReadData)
 * @return  size, in bytes.
 */
size_t PixelWriteConvert::GetDataSize()
{
   if ( UserData )
   {
      return UserDataSize;
   }
   else
   {
      return ReadDataSize;
   }
}


typedef std::pair<size_t, uint32_t> JpegPair; //offset, jpeg size
typedef std::vector<JpegPair> JpegVector;

bool gdcm_write_JPEG2000_file (std::ostream *of, char *inputdata, size_t inputlength, 
  int image_width, int image_height, int numZ, int sample_pixel, int bitsallocated,
  int sign, int quality);


void WriteDICOMItems(std::ostream *fp, JpegVector &v)
{
  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  GDCM_NAME_SPACE::binary_write(*fp, group);
  GDCM_NAME_SPACE::binary_write(*fp, elem);
  // Item Length
  uint32_t dummy = 0x12345678;
  size_t offset = fp->tellp();
  JpegPair jp;
  jp.first = offset;
  v.push_back(jp);
  GDCM_NAME_SPACE::binary_write(*fp, dummy);
}

// PS 3.5, page 66
void EncodeWithoutBasicOffsetTable(std::ostream *fp, int numFrag)// JpegVector& v) //, uint32_t length)
{
  assert( numFrag == 1);

  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  GDCM_NAME_SPACE::binary_write(*fp, group);
  GDCM_NAME_SPACE::binary_write(*fp, elem);
  // Item Length
  uint32_t item_length = 0x0000;
  GDCM_NAME_SPACE::binary_write(*fp, item_length);

}

// PS 3.5, page 67
void EncodeWithBasicOffsetTable(std::ostream *fp, int numFrag, size_t &start)
{
  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  GDCM_NAME_SPACE::binary_write(*fp, group);
  GDCM_NAME_SPACE::binary_write(*fp, elem);
  // Item Length
  uint32_t item_length = numFrag*4; // sizeof(uint32_t)
  GDCM_NAME_SPACE::binary_write(*fp, item_length);

  // Just prepare the space
  start = fp->tellp(); //to be able to rewind
  for(int i=0; i<numFrag;++i)
    {
    uint32_t dummy = 0x0000;
    GDCM_NAME_SPACE::binary_write(*fp, dummy);
    }
}

void UpdateBasicOffsetTable(std::ostream *fp, JpegVector const &v, size_t pos)
{
  JpegVector::const_iterator i;
  fp->seekp( pos );
  const JpegPair &first = v[0];
  for(i=v.begin(); i!=v.end(); ++i)
    {
    const JpegPair &jp = *i;
    if(i == v.begin() ){ assert( jp.first - first.first == 0); }
    uint32_t offset = (uint32_t)(jp.first - first.first);
    GDCM_NAME_SPACE::binary_write(*fp, offset);
    //std::cerr << "Updating Table:" << jp.first - first.first << std::endl;
    }
}

void UpdateJpegFragmentSize(std::ostream *fp, JpegVector const &v)
{
  JpegVector::const_iterator i;
  for(i= v.begin(); i!=v.end(); ++i)
    {
    const JpegPair &jp = *i;
    fp->seekp( jp.first );
    uint32_t length = jp.second;
    GDCM_NAME_SPACE::binary_write(*fp, length);
    //std::cerr << "Updating:" << jp.first << "," << jp.second << std::endl;
    }
}

void CloseJpeg(std::ostream *fp, JpegVector &v)
{
  // sequence terminator
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe0dd;
  GDCM_NAME_SPACE::binary_write(*fp, group);
  GDCM_NAME_SPACE::binary_write(*fp, elem);

  uint32_t length = 0x0;
  GDCM_NAME_SPACE::binary_write(*fp, length);

  // Jpeg is done, now update the frag length
  UpdateJpegFragmentSize(fp, v);
}

// I need to pass the File*. I do not understand how PixelWriteConvert is supposed
// to access this information otherwise
// size can now be computed from File attributes (what an API...)
void PixelWriteConvert::SetCompressJPEG2000UserData(uint8_t *data, size_t size, File *image)
{
  Compressed = true;
  //char * userData = reinterpret_cast<char*>(UserData);

   std::ostringstream *of = new std::ostringstream();
    int xsize = image->GetXSize();
    int ysize = image->GetYSize();
    int zsize = image->GetZSize();
    int samplesPerPixel = image->GetSamplesPerPixel();
   //std::cout << "X: " << xsize << std::endl;
   //std::cout << "Y: " << ysize << std::endl;
   //std::cout << "Sample: " << samplesPerPixel << std::endl;
    int bitsallocated = image->GetBitsAllocated();
    int sign = image->IsSignedPixelData();
    unsigned int fragment_size = xsize*ysize*samplesPerPixel * (bitsallocated / 8);
    //assert( fragment_size*zsize == size );
    
    gdcmDebugMacro("fragment_size " << fragment_size << " zsize " << zsize << " size " << size);    
    assert( abs((long)(fragment_size*zsize-size)) <=1 );
    
   JpegVector JpegFragmentSize;
      gdcmDebugMacro("Call Encode..BasicOffsetTable " );
#if WITHOFFSETTABLE
   size_t bots; //basic offset table start
   EncodeWithBasicOffsetTable(of, zsize, bots);
#else
   EncodeWithoutBasicOffsetTable(of, 1);
#endif

   gdcmDebugMacro("Out of Encode..BasicOffsetTable " );
   
   uint8_t *pImageData = data;
   for(int i=0; i<zsize;i++)
     {
     gdcmDebugMacro("Write fragment no " << i );
     WriteDICOMItems(of, JpegFragmentSize);
     size_t beg = of->tellp();
     gdcm_write_JPEG2000_file(of, (char*)pImageData,size, 
       image->GetXSize(), image->GetYSize(), image->GetZSize(), image->GetSamplesPerPixel(),
       image->GetBitsAllocated(), sign, 100);
     //userData, UserDataSize);
     //     CreateOneFrame(of, pImageData, fragment_size, xsize, ysize, zsize, 
     //       samplesPerPixel, quality, JpegFragmentSize);
     //assert( !(fragment_size % 2) );
     // Update the JpegVector with offset
     size_t end = of->tellp();
     //static int i = 0;
     JpegPair &jp = JpegFragmentSize[i];
     jp.second = (uint32_t)(end-beg);
     if( ((end-beg) % 2) )
       {
       of->put( '\0' );
       jp.second += 1;
       }
     assert( !(jp.second % 2) );
     //std::cerr << "DIFF: " << i <<" -> " << jp.second << std::endl;
     //++i;
     pImageData += fragment_size;
     }
   CloseJpeg(of, JpegFragmentSize);
#if WITHOFFSETTABLE
   UpdateBasicOffsetTable(of, JpegFragmentSize, bots);
#endif


   size_t of_size = of->str().size();
   UserData = new uint8_t[of_size];
   memcpy(UserData, of->str().c_str(), of_size);
   UserDataSize = of_size;
   delete of;   
}

bool gdcm_write_JPEG_file8 (std::ostream *fp, char *inputdata, size_t inputlength,
                           int image_width, int image_height, int numZ,
                           int sample_pixel, int bitsallocated, int quality);
bool gdcm_write_JPEG_file12 (std::ostream *fp, char *inputdata, size_t inputlength,
                           int image_width, int image_height, int numZ,
                           int sample_pixel, int bitsallocated, int quality);
bool gdcm_write_JPEG_file16 (std::ostream *fp, char *inputdata, size_t inputlength,
                           int image_width, int image_height, int numZ,
                           int sample_pixel, int bitsallocated, int quality);

void PixelWriteConvert::SetCompressJPEGUserData(uint8_t *data, size_t size, File *image)
{
  (void)data;
  (void)size;
  (void)image;
  Compressed = true;
  //char * userData = reinterpret_cast<char*>(UserData);

   std::ostringstream *of = new std::ostringstream();
    int xsize = image->GetXSize();
    int ysize = image->GetYSize();
    int zsize = image->GetZSize();
    
    int samplesPerPixel = image->GetSamplesPerPixel();
    int bitsAllocated   = image->GetBitsAllocated();
    int bitsStored      = image->GetBitsStored();
        
   //std::cout << "X: " << xsize << std::endl;
   //std::cout << "Y: " << ysize << std::endl;
   //std::cout << "Sample: " << samplesPerPixel << std::endl;
   
    gdcmDebugMacro( "bitsAllocated " << bitsAllocated << " bitsStored " <<bitsStored <<
                    std::endl);
   

    unsigned int fragment_size = xsize*ysize*samplesPerPixel * (bitsAllocated / 8);
    gdcmDebugMacro("fragment_size " << fragment_size << " zsize " << zsize << " size " << size);
    assert( abs((long)(fragment_size*zsize-size)) <=1 );

   JpegVector JpegFragmentSize;
#if WITHOFFSETTABLE
   size_t bots; //basic offset table start
   EncodeWithBasicOffsetTable(of, zsize, bots);
#else
   EncodeWithoutBasicOffsetTable(of, 1);
#endif

      // to avoid major troubles when BitsStored == 8 && BitsAllocated==16 !

   if (bitsStored == 8 && bitsAllocated == 16)
      bitsStored = 16;

   uint8_t *pImageData = data;
   for(int i=0; i<zsize;i++)
     {
     gdcmDebugMacro("Write fragment no " << i );
     WriteDICOMItems(of, JpegFragmentSize);
     size_t beg = of->tellp();
     if( bitsStored == 8 )
       {
       gdcm_write_JPEG_file8(of, (char*)pImageData,size, 
         xsize, ysize, zsize, samplesPerPixel,
         bitsAllocated, 100 );
       }
     else if (bitsStored <= 12)
       {
       assert( bitsStored >= 8 );
       gdcm_write_JPEG_file12(of, (char*)pImageData, size,
         xsize, ysize, zsize, samplesPerPixel,
         bitsAllocated, 100);
       }
     else if (bitsStored <= 16)
       {
       assert( bitsStored >= 12 );
       gdcm_write_JPEG_file16(of, (char*)pImageData,size, 
         xsize, ysize, zsize, samplesPerPixel,
         bitsAllocated, 100);
       }
     else if (bitsStored <= 32)  // if we are lucky (?), it will compress as a 2 bytes stream
       {                            // (Actually it doesn't !)
       // Just to allow ctest not to abort on 32bits per pixel image RTDOSE.dcm
       assert( bitsStored >= 16 );
       gdcmDebugMacro( "Warning : bitsStored>16 not supported by JPEG !" );
       gdcm_write_JPEG_file16(of, (char*)pImageData, size, 
         xsize, ysize, zsize, samplesPerPixel,
         bitsAllocated, 100);
       }       
     else
       {
       std::cerr << "Major pb : bitsStored =" << bitsStored << std::endl;
       abort();
       }
    size_t end = of->tellp();
    //static int i = 0;
    JpegPair &jp = JpegFragmentSize[i];
    jp.second = (uint32_t)(end-beg);
    if( ((end-beg) % 2) )
      {
      of->put( '\0' );
      jp.second += 1;
      }
    assert( !(jp.second % 2) );
    //std::cerr << "DIFF: " << i <<" -> " << jp.second << std::endl;
    //++i;

  //JpegPair &jp = v[0];
  //jp.second = 15328;

     //userData, UserDataSize);
     //     CreateOneFrame(of, pImageData, fragment_size, xsize, ysize, zsize, 
     //       samplesPerPixel, quality, JpegFragmentSize);
     //assert( !(fragment_size % 2) );
     pImageData += fragment_size;
     }
   CloseJpeg(of, JpegFragmentSize);
#if WITHOFFSETTABLE
   UpdateBasicOffsetTable(of, JpegFragmentSize, bots);
#endif

   size_t of_size = of->str().size();
   UserData = new uint8_t[of_size];
   memcpy(UserData, of->str().c_str(), of_size);
   UserDataSize = of_size;
   delete of;
}


//-----------------------------------------------------------------------------
// Protected
//bool PixelWriteConvert::CompressJPEG2000(uint8_t *data, size_t size)
//{
//}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
