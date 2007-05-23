/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmMpeg.cxx,v $
  Language:  C++
  Date:      $Date: 2007/05/23 14:18:10 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDebug.h"

#include <fstream>


typedef struct
{
  std::ifstream *InFd;
} istream;
extern "C" {
#define GLOBAL
#include "config.h"
#include "global.h"
}


off_t my_seek(istream *infile, off_t offset, int whence)
{
  //return fseek(infile->InFd,offset, whence);
  switch(whence)
    {
    case SEEK_SET:
      infile->InFd->seekg(offset, std::ios::beg);
      break;
    case SEEK_END:
      infile->InFd->seekg(offset, std::ios::end);
      break;
    case SEEK_CUR:
      infile->InFd->seekg(offset, std::ios::cur);
      break;
    }
  return infile->InFd->tellg();
}
ssize_t my_read(istream *infile, void *buf, size_t count)
{
  //return fread(buf,1,count, infile->InFd);
  infile->InFd->read((char*)buf, count);
  return infile->InFd->gcount();
}

int my_close(istream *infile)
{
  //return fclose(infile->InFd);
  infile->InFd->close();
  return 0;
}

namespace GDCM_NAME_SPACE 
{

static int Headers();
static void DeInitialize_Decoder()
{
  free(Clip-384); /* I love magic number */
}
static void Deinitialize_Sequence()
{
  int i;

  /* First cleanup the static buffer in store.c */
  FreeStaticBuffer();

  /* clear flags */
  base.MPEG2_Flag=0;

  for(i=0;i<3;i++)
  {
    free(backward_reference_frame[i]);
    free(forward_reference_frame[i]);
    free(auxframe[i]);

    if (base.scalable_mode==SC_SPAT)
    {
     free(llframe0[i]);
     free(llframe1[i]);
    }
  }

  if (base.scalable_mode==SC_SPAT)
    free(lltmp);

#ifdef DISPLAY
  if (Output_Type==T_X11) 
    Terminate_Display_Process();
#endif
}

/* mostly IMPLEMENTAION specific routines */
static void Initialize_Sequence()
{
  int cc, size;
  static int Table_6_20[3] = {6,8,12};

  /* check scalability mode of enhancement layer */
  if (Two_Streams && (enhan.scalable_mode!=SC_SNR) && (base.scalable_mode!=SC_DP))
    Error("unsupported scalability mode\n");

  /* force MPEG-1 parameters for proper decoder behavior */
  /* see ISO/IEC 13818-2 section D.9.14 */
  if (!base.MPEG2_Flag)
  {
    progressive_sequence = 1;
    progressive_frame = 1;
    picture_structure = FRAME_PICTURE;
    frame_pred_frame_dct = 1;
    chroma_format = CHROMA420;
    matrix_coefficients = 5;
  }

  /* round to nearest multiple of coded macroblocks */
  /* ISO/IEC 13818-2 section 6.3.3 sequence_header() */
  mb_width = (horizontal_size+15)/16;
  mb_height = (base.MPEG2_Flag && !progressive_sequence) ? 2*((vertical_size+31)/32)
                                        : (vertical_size+15)/16;

  Coded_Picture_Width = 16*mb_width;
  Coded_Picture_Height = 16*mb_height;

  /* ISO/IEC 13818-2 sections 6.1.1.8, 6.1.1.9, and 6.1.1.10 */
  Chroma_Width = (chroma_format==CHROMA444) ? Coded_Picture_Width
                                           : Coded_Picture_Width>>1;
  Chroma_Height = (chroma_format!=CHROMA420) ? Coded_Picture_Height
                                            : Coded_Picture_Height>>1;
  
  /* derived based on Table 6-20 in ISO/IEC 13818-2 section 6.3.17 */
  block_count = Table_6_20[chroma_format-1];

  for (cc=0; cc<3; cc++)
  {
    if (cc==0)
      size = Coded_Picture_Width*Coded_Picture_Height;
    else
      size = Chroma_Width*Chroma_Height;

    if (!(backward_reference_frame[cc] = (unsigned char *)malloc(size)))
      Error("backward_reference_frame[] malloc failed\n");

    if (!(forward_reference_frame[cc] = (unsigned char *)malloc(size)))
      Error("forward_reference_frame[] malloc failed\n");

    if (!(auxframe[cc] = (unsigned char *)malloc(size)))
      Error("auxframe[] malloc failed\n");

    if(Ersatz_Flag)
      if (!(substitute_frame[cc] = (unsigned char *)malloc(size)))
        Error("substitute_frame[] malloc failed\n");


    if (base.scalable_mode==SC_SPAT)
    {
      /* this assumes lower layer is 4:2:0 */
      if (!(llframe0[cc] = (unsigned char *)malloc((lower_layer_prediction_horizontal_size*lower_layer_prediction_vertical_size)/(cc?4:1))))
        Error("llframe0 malloc failed\n");
      if (!(llframe1[cc] = (unsigned char *)malloc((lower_layer_prediction_horizontal_size*lower_layer_prediction_vertical_size)/(cc?4:1))))
        Error("llframe1 malloc failed\n");
    }
  }

  /* SCALABILITY: Spatial */
  if (base.scalable_mode==SC_SPAT)
  {
    if (!(lltmp = (short *)malloc(lower_layer_prediction_horizontal_size*((lower_layer_prediction_vertical_size*vertical_subsampling_factor_n)/vertical_subsampling_factor_m)*sizeof(short))))
      Error("lltmp malloc failed\n");
  }

#ifdef DISPLAY
  if (Output_Type==T_X11)
  {
    Initialize_Display_Process("");
    Initialize_Dither_Matrix();
  }
#endif /* DISPLAY */

}
static int video_sequence(int *Bitstream_Framenumber)
//int *Bitstream_Framenumber;
{
  int Bitstream_Framenum;
  int Sequence_Framenum;
  int Return_Value;

  Bitstream_Framenum = *Bitstream_Framenumber;
  Sequence_Framenum=0;

  Initialize_Sequence();

  /* decode picture whose header has already been parsed in 
     Decode_Bitstream() */


  Decode_Picture(Bitstream_Framenum, Sequence_Framenum);

  /* update picture numbers */
  if (!Second_Field)
  {
    Bitstream_Framenum++;
    Sequence_Framenum++;
  }

  /* loop through the rest of the pictures in the sequence */
  while ((Return_Value=Headers()))
  {
    Decode_Picture(Bitstream_Framenum, Sequence_Framenum);

    if (!Second_Field)
    {
      Bitstream_Framenum++;
      Sequence_Framenum++;
    }
  }

  /* put last frame */
  if (Sequence_Framenum!=0)
  {
    Output_Last_Frame_of_Sequence(Bitstream_Framenum);
  }

  Deinitialize_Sequence();

#ifdef VERIFY
    Clear_Verify_Headers();
#endif /* VERIFY */

  *Bitstream_Framenumber = Bitstream_Framenum;
  return(Return_Value);
}

static int Headers()
{
  int ret;

  ld = &base;
  

  /* return when end of sequence (0) or picture
     header has been parsed (1) */

  ret = Get_Hdr();


  if (Two_Streams)
  {
    ld = &enhan;
    if (Get_Hdr()!=ret && !Quiet_Flag)
      fprintf(stderr,"streams out of sync\n");
    ld = &base;
  }

  return ret;
}
static int Decode_Bitstream()
{
  int ret;
  int Bitstream_Framenum;

  Bitstream_Framenum = 0;

  for(;;)
  {

#ifdef VERIFY
    Clear_Verify_Headers();
#endif /* VERIFY */

    ret = Headers();
    
    if(ret==1)
    {
      ret = video_sequence(&Bitstream_Framenum);
    }
    else
      return(ret);
  }

}


/* IMPLEMENTATION specific routines */
static void Initialize_Decoder()
{
  int i;

  /* Clip table */
  if (!(Clip=(unsigned char *)malloc(1024)))
    Error("Clip[] malloc failed\n");

  Clip += 384;

  for (i=-384; i<640; i++)
    Clip[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  /* IDCT */
  if (Reference_IDCT_Flag)
    Initialize_Reference_IDCT();
  else
    Initialize_Fast_IDCT();

}

/**
 * \brief   routine for MPEG decompression 
 * @param fp pointer to an already open file descriptor 
 *                      8 significant bits per pixel
 * @param image_buffer to receive uncompressed pixels
 * @param length length
 * @return 1 on success, 0 on error
 */
bool ReadMPEGFile (std::ifstream *fp, char *image_buffer, size_t length)
{
  std::streampos mpeg_start = fp->tellg(); // the MPEG stream in our case does not start at 0
#if 0
  fp->read((char*)image_buffer, length);
  std::ofstream out("/tmp/etiam.mpeg");
  out.write((char*)image_buffer, length);
  out.close();
#else
  (void)length;
#endif

  int ret, code;
  istream bos,eos;
  /*base.open_stream = my_open;*/
  base.seek_stream = my_seek;
  base.read_stream = my_read;
  base.close_stream = my_close;

  Verbose_Flag                         = 0;
  Output_Type                          = T_MEM; //store in mem
  Output_Picture_Filename              = "/tmp/rec%d";
  hiQdither                            = 0;
  Frame_Store_Flag                     = 1;
  Spatial_Flag                         = 0;
  Lower_Layer_Picture_Filename         = "";
  Reference_IDCT_Flag                  = 1;
  Trace_Flag                           = 0;
  Quiet_Flag                           = 0;
  Ersatz_Flag                          = 0;
  Substitute_Picture_Filename          = "";
  Two_Streams                          = 0;
  Enhancement_Layer_Bitstream_Filename = "";
  Big_Picture_Flag                     = 0;
  Main_Bitstream_Flag                  = 1;
  Main_Bitstream_Filename              = "/tmp/etiam.mpeg";
  Main_Bitstream_Filename              = "/tmp/ts.mpg";
  Verify_Flag                          = 0;
  Stats_Flag                           = 0;
  User_Data_Flag                       = 0;
  OUTMEM                               = (char*)image_buffer;


  ld = &base; /* select base layer context */

  /* open MPEG base layer bitstream file(s) */
  /* NOTE: this is either a base layer stream or a spatial enhancement stream */
/*  if ((base.Infile=open(Main_Bitstream_Filename,O_RDONLY|O_BINARY))<0) */
  /*base.Infile = ld->open_stream(Main_Bitstream_Filename);*/
  base.Infile = &bos;
#ifdef FILESTAR
  //base.Infile->InFd = fopen(Main_Bitstream_Filename, "rb");
  //base.Infile->InFd.open(Main_Bitstream_Filename, std::ios::binary | std::ios::in);
  base.Infile->InFd = fp;
#else
  base.Infile->InFd = open(Main_Bitstream_Filename,O_RDONLY|O_BINARY );
#endif
  if( !base.Infile->InFd)
  {
    fprintf(stderr,"Base layer input file %s not found\n", Main_Bitstream_Filename);
    exit(1);
  }


  if(base.Infile != 0)
  {
    Initialize_Buffer();

    if(Show_Bits(8)==0x47)
    {
      sprintf(Error_Text,"Decoder currently does not parse transport streams\n");
      Error(Error_Text);
    }

    next_start_code();
    code = Show_Bits(32);

    switch(code)
    {
    case SEQUENCE_HEADER_CODE:
      break;
    case PACK_START_CODE:
      System_Stream_Flag = 1;
    case VIDEO_ELEMENTARY_STREAM:
      System_Stream_Flag = 1;
      break;
    default:
      sprintf(Error_Text,"Unable to recognize stream type\n");
      Error(Error_Text);
      break;
    }

    /*lseek(base.Infile, 0l, SEEK_SET);*/
    ld->seek_stream(base.Infile,mpeg_start,SEEK_SET);
    Initialize_Buffer(); 
  }

  if(base.Infile!=0)
  {
    /*lseek(base.Infile, 0l, SEEK_SET);*/
    ld->seek_stream(base.Infile,mpeg_start,SEEK_SET);
  }

  Initialize_Buffer(); 

  if(Two_Streams)
  {
    ld = &enhan; /* select enhancement layer context */

    /*if ((enhan.Infile = open(Enhancement_Layer_Bitstream_Filename,O_RDONLY|O_BINARY))<0)*/
    /*enhan.Infile = ld->open_stream(Enhancement_Layer_Bitstream_Filename);*/
    enhan.Infile = &eos;
#ifdef FILESTAR
    //enhan.Infile->InFd = fopen(Main_Bitstream_Filename, "rb");
    enhan.Infile->InFd->open(Main_Bitstream_Filename, std::ios::binary|std::ios::in);
    abort();
#else
    enhan.Infile->InFd = open(Enhancement_Layer_Bitstream_Filename,O_RDONLY|O_BINARY);
#endif
    if (enhan.Infile->InFd)
    {
      sprintf(Error_Text,"enhancment layer bitstream file %s not found\n",
        Enhancement_Layer_Bitstream_Filename);

      Error(Error_Text);
    }

    Initialize_Buffer();
    ld = &base;
  }

  Initialize_Decoder();

  ret = Decode_Bitstream();

  /*close(base.Infile);*/
  ld->close_stream(base.Infile);

  if (Two_Streams)
    /*close(enhan.Infile);*/
    ld->close_stream(enhan.Infile);

  DeInitialize_Decoder();

#if 0
  std::ofstream out2("/tmp/etiam.raw");
  out2.write((char*)image_buffer, 352*240*3*10);
  out2.close();
#endif

  return ret;
}

} // end namespace gdcm
