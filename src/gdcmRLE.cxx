/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLE.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/06 22:31:31 $
  Version:   $Revision: 1.24 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <stdio.h>
#include "gdcmFile.h"
#include <ctype.h>     // For isprint()

#define str2num(str, typeNum) *((typeNum *)(str))

//-----------------------------------------------------------------------------
/**
 * \ingroup   gdcmFile
 * \brief     Reads a 'Run Length Encoded' Dicom encapsulated file
 * @param     fp already open File Pointer
 * @param     image_buffer destination Address (in caller's memory space) 
 *            at which the pixel data should be copied 
 * @return    Boolean 
 */
bool gdcmFile::gdcm_read_RLE_file (FILE* fp,void* image_buffer) {
   char * im = (char *)image_buffer;
   long uncompressedSegmentSize = Header->GetXSize() * Header->GetYSize();
   

   // Loop on the frame[s]
   for( gdcmRLEFramesInfo::RLEFrameList::iterator
        it  = Header->RLEInfo.Frames.begin();
        it != Header->RLEInfo.Frames.end();
      ++it )
   {
       std::cout << "...new frame...\n ";
      // Loop on the fragments
      for( unsigned int k = 1; k <= (*it)->NumberFragments; k++ )
      {
         fseek( fp, (*it)->Offset[k] ,SEEK_SET);  
         gdcm_read_RLE_fragment( &im, (*it)->Length[k],
                                 uncompressedSegmentSize, fp );
      }
   }

   if (Header->GetBitsAllocated()==16) { // try to deal with RLE 16 Bits
   
      im = (char *)image_buffer;
         //  need to make 16 Bits Pixels from Low Byte and Hight Byte 'Planes'

      int l = Header->GetXSize()*Header->GetYSize();
      int nbFrames = Header->GetZSize();

      char * newDest = new char[l*nbFrames*2];
      char *x  = newDest;
      char * a = (char *)image_buffer;
      char * b = a + l;

      for (int i=0;i<nbFrames;i++) {
         for (int j=0;j<l; j++) {
            *(x++) = *(a++);
            *(x++) = *(b++);
         }
      }
      memmove(image_buffer,newDest,ImageDataSize);
      delete[] newDest;
   }
      
   return true;
}


// ----------------------------------------------------------------------------
// RLE LossLess Fragment
int gdcmFile::gdcm_read_RLE_fragment(char** areaToRead, long lengthToDecode, 
                                     long uncompressedSegmentSize, FILE* fp) {
   (void)lengthToDecode; //FIXME
   int count;
   long numberOfOutputBytes=0;
   char n, car;

   while( numberOfOutputBytes < uncompressedSegmentSize )
   {
      fread(&n,sizeof(char),1,fp);
      count=n;
      if (count >= 0 && count <= 127) {
         fread(*areaToRead,(count+1)*sizeof(char),1,fp);
         *areaToRead+=count+1;
         numberOfOutputBytes+=count+1;
      } else {
         if (count <= -1 && count >= -127) {
            fread(&car,sizeof(char),1,fp);
            for(int i=0; i<-count+1; i++) {
               (*areaToRead)[i]=car;  
            }
            *areaToRead+=(-count+1);
            numberOfOutputBytes+=(-count+1); 
         }
      } 
      // if count = 128 output nothing (See : PS 3.5-2003 Page 86)
   } 
   return 1;
}

// ----------------------------------------------------------------------------
