/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicomAsJPEG.cxx,v $
  Language:  C++
  Date:      $Date: 2006/07/04 08:00:43 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmUtil.h"
 
#include <iostream>
#include <sstream>

#include <stdio.h>
extern "C" {
#include "gdcmjpeg/8/jconfig.h"
#include "gdcmjpeg/8/jpeglib.h"
#include "gdcmjpeg/8/jinclude.h"
#include "gdcmjpeg/8/jerror.h"
}

#if defined(__BORLANDC__)
 #include <mem.h> // for memcpy
#endif

#include "gdcmJPEGFragment.h"
#include <setjmp.h>
#include <fstream>

#include "jdatasrc.cxx"
#include "jdatadst.cxx"

typedef std::pair<size_t, uint32_t> JpegPair; //offset, jpeg size
typedef std::vector<JpegPair> JpegVector;

void WriteDICOMItems(std::ostream *fp, JpegVector &v)
{
  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  gdcm::binary_write(*fp, group);
  gdcm::binary_write(*fp, elem);
  // Item Length
  uint32_t dummy = 0x12345678;
  size_t offset = fp->tellp();
  JpegPair jp;
  jp.first = offset;
  v.push_back(jp);
  gdcm::binary_write(*fp, dummy);
}

// PS 3.5, page 66
void EncodeWithoutBasicOffsetTable(std::ostream *fp, int numFrag)// JpegVector& v) //, uint32_t length)
{
  assert( numFrag == 1);

  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  gdcm::binary_write(*fp, group);
  gdcm::binary_write(*fp, elem);
  // Item Length
  uint32_t item_length = 0x0000;
  gdcm::binary_write(*fp, item_length);

}

// PS 3.5, page 67
void EncodeWithBasicOffsetTable(std::ostream *fp, int numFrag, size_t &start)
{
  // Item tag:
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  gdcm::binary_write(*fp, group);
  gdcm::binary_write(*fp, elem);
  // Item Length
  uint32_t item_length = numFrag*4; // sizeof(uint32_t)
  gdcm::binary_write(*fp, item_length);

  // Just prepare the space
  start = fp->tellp(); //to be able to rewind
  for(int i=0; i<numFrag;++i)
    {
    uint32_t dummy = 0x0000;
    gdcm::binary_write(*fp, dummy);
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
    gdcm::binary_write(*fp, jp.first - first.first);
    std::cerr << "Updating Table:" << jp.first - first.first << std::endl;
    }
}

void UpdateJpegFragmentSize(std::ostream *fp, JpegVector const &v)
{
  JpegVector::const_iterator i;
  for(i= v.begin(); i!=v.end(); ++i)
    {
    const JpegPair &jp = *i;
    fp->seekp( jp.first );
    gdcm::binary_write(*fp, jp.second );
    std::cerr << "Updating:" << jp.first << "," << jp.second << std::endl;
    }
}

void CloseJpeg(std::ostream *fp, JpegVector &v)
{
  // sequence terminator
  uint16_t group = 0xfffe;
  uint16_t elem  = 0xe000;
  gdcm::binary_write(*fp, group);
  gdcm::binary_write(*fp, elem);

  uint32_t length = 0x0;
  gdcm::binary_write(*fp, length);

  // Jpeg is done, now update the frag length
  UpdateJpegFragmentSize(fp, v);
}

bool InitializeJpeg(std::ostream *fp, int fragment_size, int image_width, int image_height, 
  int sample_pixel, int quality, struct jpeg_compress_struct &cinfo, int &row_stride)
{

  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  //struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  jpeg_stdio_dest(&cinfo, fp, fragment_size, 1);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width;/* image width and height, in pixels */
  cinfo.image_height = image_height;
  if ( sample_pixel == 3 )
    {
    cinfo.input_components = 3;     /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
    }
  else
    {
    cinfo.input_components = 1;     /* # of color components per pixel */
    cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
    }
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /*
   * http://www.koders.com/c/fid80DBBF1D49D004EF71CE7C493C34610C4F17D3D3.aspx
   * http://studio.imagemagick.org/pipermail/magick-users/2002-September/004685.html
   * You need to set -quality 101 or greater.  If quality is 100 or less you
   * get regular JPEG output.  This is not explained in the documentation, only
   * in the comments in coder/jpeg.c.  When you have configured libjpeg with
   * lossless support, then
   * 
   *    quality=predictor*100 + point_transform
   * 
   * If you don't know what these values should be, just use 101.
   * They only affect the compression ratio, not the image appearance,
   * which is lossless.
   */
  jpeg_simple_lossless (&cinfo, 1, 1);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  if (sample_pixel == 3)
    {
    assert( sample_pixel == 1 );
    row_stride = image_width * 3;/* JSAMPLEs per row in image_buffer */
    }
  else
    {
    row_stride = image_width * 1;/* JSAMPLEs per row in image_buffer */
    }

  /* everything was ok */
  return true;
}

bool FinalizeJpeg(struct jpeg_compress_struct &cinfo)
{
  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  
  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
  return true;
}

// If false then suspension return
bool WriteScanlines(struct jpeg_compress_struct &cinfo, void *input_buffer, int row_stride)
{
  JSAMPLE *image_buffer = (JSAMPLE*) input_buffer;
  JSAMPROW row_pointer[1];   /* pointer to JSAMPLE row[s] */
  row_pointer[0] = image_buffer;

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    //row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];

    if( jpeg_write_scanlines(&cinfo, row_pointer, 1) != 1)
      {
      //entering suspension mode, basically we wrote the whole jpeg fragment
      // technically we could enforce that by checkig the value of row_pointer to
      // actually be at the end of the image...TODO
      return false;
      }
    row_pointer[0] += row_stride;
  }

  // Well looks like we are done writting the scanlines
  return true;
}

// input_buffer is ONE image
// fragment_size is the size of this image (fragment)
bool CreateOneFrame (std::ostream *fp, void *input_buffer, int fragment_size,
                     int image_width, int image_height, int numZ, int sample_pixel, int quality, JpegVector &v)
{
  struct jpeg_compress_struct cinfo;
  int row_stride;            /* physical row width in image buffer */
  size_t beg = fp->tellp();
  bool r = InitializeJpeg(fp, fragment_size, image_width, image_height, 
      sample_pixel, quality, cinfo, row_stride);
  assert( r );
  (void)numZ;

  uint8_t *pbuffer = (uint8_t*)input_buffer;
  //int i;
  //for(i=0; i<numZ; ++i)
//    {
    r = WriteScanlines(cinfo, pbuffer, row_stride);
    assert( r );
//    pbuffer+=fragment_size; //shift to next image

    //Upodate frag size
//    size_t end = fp->tellp();
//    std::cerr << "DIFF: " << end-beg << std::endl;

//    JpegPair &jp = v[i];
//    jp.second = end-beg;
    //beg = end; //
 //   }

  r = FinalizeJpeg(cinfo);
  assert( r );
    size_t end = fp->tellp();
    static int i = 0;
    JpegPair &jp = v[i];
    jp.second = end-beg;
    
    if( ((end-beg) % 2) )
    {
       fp->put( '\0' );
       jp.second += 1;
    }
    assert( !(jp.second % 2) );
    std::cerr << "DIFF: " << i <<" -> " << jp.second << std::endl;    
       
    ++i;

  //JpegPair &jp = v[0];
  //jp.second = 15328;

  return true;
}

//bool CreateMultipleFrames (std::ostream *fp, void *input_buffer, int fragment_size,
//               int image_width, int image_height, int sample_pixel, int quality, JpegVector &v)
//{
//}

#define WITHOFFSETTABLE 1

// Open a dicom file and compress it as JPEG stream
int main(int argc, char *argv[])
{
  if( argc < 2)
    return 1;

   std::string filename = argv[1];
   std::string outfilename = "/tmp/bla.dcm";
   if( argc >= 3 )
     outfilename = argv[2];
   int quality = 100;
   if( argc >= 4 )
     quality = atoi(argv[2]);
   std::cerr << "Using quality: " << quality << std::endl;

// Step 1 : Create the header of the image
   gdcm::File *f = gdcm::File::New();
   f->SetLoadMode ( gdcm::LD_ALL ); // Load everything
   f->SetFileName( filename );
   f->Load();

   gdcm::FileHelper *tested = gdcm::FileHelper::New( f );
   std::string PixelType = tested->GetFile()->GetPixelType();
   int xsize = f->GetXSize();
   int ysize = f->GetYSize();
   int zsize = f->GetZSize();

   int samplesPerPixel = f->GetSamplesPerPixel();
   size_t testedDataSize    = tested->GetImageDataSize();
   std::cerr << "testedDataSize:" << testedDataSize << std::endl;
   uint8_t *testedImageData = tested->GetImageData();

   //std::ofstream *of = new std::ofstream("/tmp/jpeg.jpg");
   std::ostringstream *of = new std::ostringstream();
   std::cout << "X: " << xsize << std::endl;
   std::cout << "Y: " << ysize << std::endl;
   std::cout << "Sample: " << samplesPerPixel << std::endl;
   int fragment_size = xsize*ysize*samplesPerPixel;

   JpegVector JpegFragmentSize;
#if WITHOFFSETTABLE
   size_t bots; //basic offset table start
   EncodeWithBasicOffsetTable(of, zsize, bots);
#else
   EncodeWithoutBasicOffsetTable(of, 1);
#endif
   uint8_t *pImageData = testedImageData;
   for(int i=0; i<zsize;i++)
     {
     WriteDICOMItems(of, JpegFragmentSize);
     CreateOneFrame(of, pImageData, fragment_size, xsize, ysize, zsize, 
       samplesPerPixel, quality, JpegFragmentSize);
     assert( !(fragment_size % 2) );  
     pImageData += fragment_size;
     }
   CloseJpeg(of, JpegFragmentSize);
#if WITHOFFSETTABLE
   UpdateBasicOffsetTable(of, JpegFragmentSize, bots);
#endif

   if( !f->IsReadable() )
   {
      std::cerr << "-------------------------------\n"
                << "Error while creating the file\n"
                << "This file is considered to be not readable\n";

      return 1;
   }
   std::streambuf* sb = of->rdbuf();
   (void)sb;

   // Let save the file as jpeg standalone
     {
     std::ofstream *jof = new std::ofstream( "/tmp/test.jpg" );
     CreateOneFrame(jof, testedImageData, fragment_size, xsize, ysize, zsize, 
       samplesPerPixel, 70, JpegFragmentSize);
     jof->close();
     delete jof;
     }





// Step 1 : Create the header of the image

   gdcm::File *fileToBuild = gdcm::File::New();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << xsize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011); // Columns
   str.str("");
   str << ysize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010); // Rows

   if(zsize>1)
   {
      str.str("");
      str << zsize;
      fileToBuild->InsertEntryString(str.str(),0x0028,0x0008); // Number of Frames
   }

   // Set the pixel type
   str.str("");
   str << 8; //img.componentSize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0100); // Bits Allocated

   str.str("");
   str << 8; //img.componentUse;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0101); // Bits Stored

   str.str("");
   str << 7; //( img.componentSize - 1 );
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0102); // High Bit

   // Set the pixel representation
   str.str("");
   str << 0; //img.sign;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0103); // Pixel Representation

   // Set the samples per pixel
   str.str("");
   str << samplesPerPixel; //img.components;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0002); // Samples per Pixel

// Step 2 : Create the output image
//   std::cout << "2...";
//   if( img.componentSize%8 > 0 )
//   {
//      img.componentSize += 8-img.componentSize%8;
//   }
   size_t size = xsize * ysize * zsize
               * samplesPerPixel /* * img.componentSize / 8*/;

   uint8_t *imageData = new uint8_t[size];
   gdcm::FileHelper *fileH = gdcm::FileHelper::New(fileToBuild);
   //fileH->SetImageData(imageData,size);
   assert( size == testedDataSize );
   size = of->str().size();
   //size = sb->in_avail();
   std::cerr << "Size JPEG:" << size << std::endl;
   //fileH->SetImageData((uint8_t*)of->str().c_str(), size);
   memcpy(imageData, of->str().c_str(), size);
   fileH->SetImageData(imageData, size);
   //str::string *s = of->str();
   //fileH->SetWriteTypeToDcmExplVR();
   fileH->SetWriteTypeToJPEG(  );
   if( !fileH->Write(outfilename) )
     {
     std::cerr << "Badddd" << std::endl;
     }
   //of->close();
   std::ofstream out("/tmp/jpeg2.jpg");
   //out.write( of->str(), of
   //out << of->str(); //rdbuf is faster than going through str()
   //out.write( (char*)imageData, size);
   out.write( of->str().c_str(), size);
   //std::cerr << "JPEG marker is: " << imageData[6] << imageData[7] << 
   //  imageData[8] << imageData[9] << std::endl;
   //out.rdbuf( *sb );
   out.close();

   delete of;
   f->Delete();
   tested->Delete();
   fileToBuild->Delete();
   fileH->Delete();

   return 0;
}

