/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: WriteDicomAsJPEG.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/19 17:56:57 $
  Version:   $Revision: 1.7 $
                                                                                
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

#include "gdcmJPEGFragment.h"
#include <setjmp.h>
#include <fstream>

#include "jdatasrc.cxx"
#include "jdatadst.cxx"

typedef std::pair<size_t, uint32_t> JpegPair; //offset, jpeg size
typedef std::vector<JpegPair> JpegVector;

// PS 3.5, page 66
void EncodeWithoutBasicOffsetTable(std::ostream *fp, int numFrag, JpegVector& v) //, uint32_t length)
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

  // back again...First fragment
  // Item tag:
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

void UpdateJpegFragmentSize(std::ostream *fp, JpegVector const &v)
{
  JpegVector::const_iterator i;
  for(i= v.begin(); i!=v.end(); ++i)
    {
    const JpegPair &jp = *i;
    fp->seekp( jp.first );
    gdcm::binary_write(*fp, jp.second );
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


// PS 3.5, page 67
void EncodeWithBasicOffsetTable(std::ostream *fp, int numFrag)
{
  (void)fp; (void)numFrag;
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
                     int image_width, int image_height, int sample_pixel, int quality, JpegVector &v)
{
  struct jpeg_compress_struct cinfo;
  int row_stride;            /* physical row width in image buffer */
  bool r = InitializeJpeg(fp, fragment_size, image_width, image_height, 
      sample_pixel, quality, cinfo, row_stride);
  assert( r );

  r = WriteScanlines(cinfo, input_buffer, row_stride);
  assert( r );

  r = FinalizeJpeg(cinfo);
  assert( r );

  JpegPair &jp = v[0];
  jp.second = 15328;

  return true;
}

// Open a dicom file and compress it as JPEG stream
int main(int argc, char *argv[])
{
  if( argc < 2)
    return 1;

  std::string filename = argv[1];

// Step 1 : Create the header of the image
   gdcm::File *f = new gdcm::File();
   f->SetLoadMode ( gdcm::LD_ALL ); // Load everything
   f->SetFileName( filename );
   f->Load();

   gdcm::FileHelper *tested = new gdcm::FileHelper( f );
   std::string PixelType = tested->GetFile()->GetPixelType();
   int xsize = f->GetXSize();
   int ysize = f->GetYSize();

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
   //EncodeWithoutBasicOffsetTable(of, 1, 15328);
   EncodeWithoutBasicOffsetTable(of, 1, JpegFragmentSize); //, 15328);
   CreateOneFrame(of, testedImageData, fragment_size, xsize, ysize, samplesPerPixel, 100, JpegFragmentSize);
   CloseJpeg(of, JpegFragmentSize);

   if( !f->IsReadable() )
   {
      std::cerr << "-------------------------------\n"
                << "Error while creating the file\n"
                << "This file is considered to be not readable\n";

      return 1;
   }
   std::streambuf* sb = of->rdbuf();
   (void)sb;






// Step 1 : Create the header of the image

   gdcm::File *fileToBuild = new gdcm::File();
   std::ostringstream str;

   // Set the image size
   str.str("");
   str << xsize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0011); // Columns
   str.str("");
   str << ysize;
   fileToBuild->InsertEntryString(str.str(),0x0028,0x0010); // Rows

   //if(img.sizeZ>1)
   //{
   //   str.str("");
   //   str << img.sizeZ;
   //   fileToBuild->InsertEntryString(str.str(),0x0028,0x0008); // Number of Frames
   //}

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
   size_t size = xsize * ysize * 1 /*Z*/ 
               * samplesPerPixel /* * img.componentSize / 8*/;

   uint8_t *imageData = new uint8_t[size];
   gdcm::FileHelper *fileH = new gdcm::FileHelper(fileToBuild);
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
   std::string fileName = "/tmp/bla.dcm";
   if( !fileH->Write(fileName) )
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
   delete f;
   delete tested;
   delete fileToBuild;
   delete fileH;

   return 0;
}

