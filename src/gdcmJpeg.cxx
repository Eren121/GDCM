// gdcmJpeg.cxx
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "gdcmFile.h"

#define BITS_IN_JSAMPLE 8

#ifdef GDCM_DEBUG
#define GDCM_jpr_DEBUG 0
#endif   //GDCM_DEBUG

/*
DICOM provides a mechanism for supporting the use of JPEG Image Compression 
through the Encapsulated Format (see PS 3.3 of the DICOM Standard). 
Annex A defines a number of Transfer Syntaxes which reference 
the JPEG Standard and provide a number of lossless (bit preserving) 
and lossy compression schemes.
In order to facilitate interoperability of implementations conforming 
to the DICOM Standard which elect to use one or more 
of the Transfer Syntaxes for JPEG Image Compression, the following policy is specified:

  Any implementation which conforms to the DICOM Standard and has elected 
  to support any one of the Transfer Syntaxes for lossless JPEG Image Compression, 
  shall support the following lossless compression: 
  The subset (first-order horizontal prediction [Selection Value 1) of JPEG Process 14 
  (DPCM, non-hierarchical with Huffman coding) (see Annex F of the DICOM Standard).

   Any implementation which conforms to the DICOM Standard and has elected 
   to support any one of the Transfer Syntaxes for 8-bit lossy JPEG Image Compression, 
   shall support the JPEG Baseline Compression (coding Process 1).

   Any implementation which conforms to the DICOM Standard and has elected 
   to support any one of the Transfer Syntaxes for 12-bit lossy JPEG Image Compression, 
   shall support the JPEG Compression Process 4.

Note: The DICOM conformance statement shall differentiate between implementations 
that can simply receive JPEG encoded images and those that can receive and process 
JPEG encoded images (see PS 3.2 of the DICOM Standard).

The use of the DICOM Encapsulated Format to support JPEG Compressed Pixel Data 
implies that the Data Elements which are related to the Native Format Pixel Data encoding
(e.g. Bits Allocated, Bits Stored, High Bit, Pixel Representation, Rows, Columns, etc.) 
shall contain values which are consistent with the characteristics 
of the uncompressed pixel data from which the compressed Data Stream was derived. 
The Pixel Data characteristics included in the JPEG Interchange Format 
shall be used to decode the compressed data stream.

Run Length Encoding Compression

DICOM provides a mechanism for supporting the use of Run Length Encoding (RLE) 
Compression which is a byte oriented lossless compression scheme through 
the encapsulated Format (see PS 3.3 of this Standard). 
Annex G of the DICOM Standard defines RLE Compression and its Transfer Syntax.

Note: The RLE Compression algorithm described in Annex G 
of the DICOM Standard is the compression used in 
the TIFF 6.0 specification known as the "PackBits" scheme.

The use of the DICOM Encapsulated Format to support RLE Compressed Pixel Data 
implies that the Data Elements which are related to the Native Format Pixel Data encoding (
e.g. Bits Allocated, Bits Stored, High Bit, Pixel Representation, Rows, Columns, etc.) 
shall contain values which are consistent with the characteristics 
of the uncompressed pixel data from which the compressed data is derived
*/

/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

/*
 * Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */

extern "C" {
#include "jpeglib.h"
#include <setjmp.h>
}

/******************** JPEG DECOMPRESSION SAMPLE INTERFACE *******************/

/* This half of the example shows how to read data from the JPEG decompressor.
 * It's a bit more refined than the above, in that we show:
 *   (a) how to modify the JPEG library's standard error-reporting behavior;
 *   (b) how to allocate workspace using the library's memory manager.
 *
 * Just to make this example a little different from the first one, we'll
 * assume that we do not intend to put the whole image into an in-memory
 * buffer, but to send it line-by-line someplace else.  We need a one-
 * scanline-high JSAMPLE array as a work buffer, and we will let the JPEG
 * memory manager allocate it for us.  This approach is actually quite useful
 * because we don't need to remember to deallocate the buffer separately: it
 * will go away automatically when the JPEG object is cleaned up.
 */

/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * Our example here shows how to override the "error_exit" method so that
 * control is returned to the library's caller when a fatal error occurs,
 * rather than calling exit() as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

//-----------------------------------------------------------------------------
struct my_error_mgr {
   struct jpeg_error_mgr pub; /* "public" fields */
   jmp_buf setjmp_buffer;     /* for return to caller */
};

//-----------------------------------------------------------------------------
typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */
METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
   /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
   my_error_ptr myerr = (my_error_ptr) cinfo->err;

   /* Always display the message. */
   /* We could postpone this until after returning, if we chose. */
   (*cinfo->err->output_message) (cinfo);

   /* Return control to the setjmp point */
   longjmp(myerr->setjmp_buffer, 1);
}

//-----------------------------------------------------------------------------
/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */
 
 /**
 * \ingroup gdcmFile
 * \brief   routine for JPEG decompression 
 * @param fp pointer to an already open file descriptor 
 *                      8 significant bits per pixel
 * @param image_buffer to receive uncompressed pixels
 * @return 1 on success, 0 on error
 */
 
bool gdcmFile::gdcm_read_JPEG_file (FILE *fp,void * image_buffer) {
   char *pimage;

   /* This struct contains the JPEG decompression parameters and pointers to
    * working space (which is allocated as needed by the JPEG library).
    */
   struct jpeg_decompress_struct cinfo;

   /* -------------- inside, we found :
    * JDIMENSION image_width;       // input image width 
    * JDIMENSION image_height;      // input image height 
    * int input_components;         // nb of color components in input image 
    * J_COLOR_SPACE in_color_space; // colorspace of input image 
    * double input_gamma;           // image gamma of input image 
    * -------------- */

   /* We use our private extension JPEG error handler.
    * Note that this struct must live as long as the main JPEG parameter
    * struct, to avoid dangling-pointer problems.
    */
   struct my_error_mgr jerr;
   /* More stuff */

   JSAMPARRAY buffer;/* Output row buffer */
  
   // rappel :
   // ------
   // typedef unsigned char JSAMPLE;
   // typedef JSAMPLE FAR *JSAMPROW;/* ptr to one image row of pixel samples. */
   // typedef JSAMPROW *JSAMPARRAY;/* ptr to some rows (a 2-D sample array) */
   // typedef JSAMPARRAY *JSAMPIMAGE;/* a 3-D sample array: top index is color */

   int row_stride;/* physical row width in output buffer */
  
#ifdef GDCM_JPG_DEBUG
   printf("entree dans gdcmFile::gdcm_read_JPEG_file12, depuis gdcmJpeg\n");
#endif //GDCM_JPG_DEBUG

   /* In this example we want to open the input file before doing anything else,
    * so that the setjmp() error recovery below can assume the file is open.
    * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
    * requires it in order to read binary files.
    */
    
  /* Step 1: allocate and initialize JPEG decompression object */  
#ifdef GDCM_JPG_DEBUG
  printf("Entree Step 1\n");
#endif //GDCM_JPG_DEBUG
  
  /* We set up the normal JPEG error routines, then override error_exit. */
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  
  /* Establish the setjmp return context for my_error_exit to use. */  
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

   /* Step 2: specify data source (eg, a file) */
#ifdef GDCM_JPG_DEBUG
  printf("Entree Step 2\n");
#endif //GDCM_JPG_DEBUG

   jpeg_stdio_src(&cinfo, fp);

   /* Step 3: read file parameters with jpeg_read_header() */
#ifdef GDCM_JPG_DEBUG
  printf("Entree Step 3\n");
#endif //GDCM_JPG_DEBUG

   (void) jpeg_read_header(&cinfo, TRUE);
   
   /* We can ignore the return value from jpeg_read_header since
    *   (a) suspension is not possible with the stdio data source, and
    *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
    * See libjpeg.doc for more info.
    */

#ifdef GDCM_JPG_DEBUG
      printf("--------------Header contents :----------------\n");
      printf("image_width %d image_height %d\n", 
              cinfo.image_width , cinfo.image_height);
      printf("bits of precision in image data  %d \n", 
              cinfo.output_components);
      printf("nb of color components returned  %d \n", 
              cinfo.data_precision);
#endif //GDCM_JPG_DEBUG


   /*
    * JDIMENSION image_width;       // input image width 
    * JDIMENSION image_height;      // input image height 
    * int output_components;        // # of color components returned 
    * J_COLOR_SPACE in_color_space; // colorspace of input image 
    * double input_gamma;           // image gamma of input image
    * int data_precision;           // bits of precision in image data 
    */

   /* Step 4: set parameters for decompression */
#ifdef GDCM_JPG_DEBUG
  printf("Entree Step 4\n");
#endif //GDCM_JPG_DEBUG
   /* In this example, we don't need to change any of the defaults set by
    * jpeg_read_header(), so we do nothing here.
    */

   /* Step 5: Start decompressor */
#ifdef GDCM_JPG_DEBUG
   printf("Entree Step 5\n");
#endif //GDCM_JPG_DEBUG

   (void) jpeg_start_decompress(&cinfo);
   /* We can ignore the return value since suspension is not possible
    * with the stdio data source.
    */

   /* We may need to do some setup of our own at this point before reading
    * the data.  After jpeg_start_decompress() we have the correct scaled
    * output image dimensions available, as well as the output colormap
    * if we asked for color quantization.
    * In this example, we need to make an output work buffer of the right size.
    */ 

   /* JSAMPLEs per row in output buffer */
   row_stride = cinfo.output_width * cinfo.output_components;
  
#ifdef GDCM_JPG_DEBUG
  printf ("cinfo.output_width %d cinfo.output_components %d  row_stride %d\n",
                      cinfo.output_width, cinfo.output_components,row_stride);
#endif //GDCM_JPG_DEBUG

   /* Make a one-row-high sample array that will go away when done with image */
   buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

   /* Step 6: while (scan lines remain to be read) */
#ifdef GDCM_JPG_DEBUG
    printf("Entree Step 6\n"); 
#endif //GDCM_JPG_DEBUG
   /*           jpeg_read_scanlines(...); */

   /* Here we use the library's state variable cinfo.output_scanline as the
    * loop counter, so that we don't have to keep track ourselves.
    */
#ifdef GDCM_JPG_DEBUG
      printf ("cinfo.output_height %d  cinfo.output_width %d\n",
               cinfo.output_height,cinfo.output_width);
#endif //GDCM_JPG_DEBUG
   pimage=(char *)image_buffer;
  
   while (cinfo.output_scanline < cinfo.output_height) {
      /* jpeg_read_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could ask for
       * more than one scanline at a time if that's more convenient.
       */
     
     // l'image est deja allouée (et passée en param)
     // on ecrit directement les pixels
     // (on DEVRAIT pouvoir)
    
    //(void) jpeg_read_scanlines(&cinfo, pimage, 1);
    
     (void) jpeg_read_scanlines(&cinfo, buffer, 1);
      
     if ( BITS_IN_JSAMPLE == 8) {
         memcpy( pimage, buffer[0],row_stride); 
         pimage+=row_stride;
     } else {
         memcpy( pimage, buffer[0],row_stride*2 ); // FIXME : *2  car 16 bits?!?
         pimage+=row_stride*2;                     // FIXME : *2 car 16 bits?!?     
     }
  }
 
  /* Step 7: Finish decompression */
#ifdef GDCM_JPG_DEBUG
   printf("Entree Step 7\n");
#endif //GDCM_JPG_DEBUG

   (void) jpeg_finish_decompress(&cinfo);
   
   /* We can ignore the return value since suspension is not possible
    * with the stdio data source.
    */

   /* Step 8: Release JPEG decompression object */

#ifdef GDCM_JPG_DEBUG
  printf("Entree Step 8\n");
#endif //GDCM_JPG_DEBUG

   /* This is an important step since it will release a good deal of memory. */

   jpeg_destroy_decompress(&cinfo);

   /* After finish_decompress, we can close the input file.
    * Here we postpone it until after no more JPEG errors are possible,
    * so as to simplify the setjmp error logic above.  (Actually, I don't
    * think that jpeg_destroy can do an error exit, but why assume anything...)
    */

   /* At this point you may want to check to see whether any corrupt-data
    * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
    */

   /* And we're done! */

   return(true);
}

/*
 * SOME FINE POINTS:
 *
 * In the above code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.doc for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.doc for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 */
 
//----------------------------------------------------------------------------
