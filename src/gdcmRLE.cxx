
#include <stdio.h>
#include "gdcmFile.h"
#include <ctype.h>		/* to declare isprint() */

#define str2num(str, typeNum) *((typeNum *)(str))

static int _gdcm_read_RLE_fragment (char ** image_buffer, 
                                    long lengthToDecode,
                                    long uncompressedSegmentSize, 
                                    FILE* fp);
// static because nothing but gdcm_read_RLE_file may call it

// ----------------------------------------------------------------------------
/**
 * \ingroup   gdcmFile
 * \brief     Reads a 'Run Length Encoded' Dicom encapsulated file
 * @param     image_buffer destination Address (in caller's memory space) 
 *            at which the pixel data should be copied
 *   
 * @return    int acts as a Boolean 
 */

// This is a debug version.
// Forget the printf as they will be removed
// as soon as the last Heuristics are checked

// pb with RLE 16 Bits : 


int
gdcmFile::gdcm_read_RLE_file (void * image_buffer) {
   long fragmentBegining; // for ftell, fseek
   char * im = (char *)image_buffer;

   long RleSegmentLength[15],fragmentLength,uncompressedSegmentSize;;
   long ftellRes, ln;
   guint32 nbRleSegments;
   guint32 RleSegmentOffsetTable[15];
   guint16 ItemTagGr,ItemTagEl;
   uncompressedSegmentSize=GetXSize()*GetYSize();
   ftellRes=ftell(fp);
   // Basic Offset Table with Item Value
      // Item Tag
   fread(&ItemTagGr,2,1,fp);  // Reading (fffe):Basic Offset Table Item Tag Gr
   fread(&ItemTagEl,2,1,fp);  // Reading (e000):Basic Offset Table Item Tag El
   if(GetSwapCode()) {
      ItemTagGr=SwapShort(ItemTagGr); 
      ItemTagEl=SwapShort(ItemTagEl);      
   }
      // Item Length
   ftellRes=ftell(fp);
   fread(&ln,4,1,fp); 
   if(GetSwapCode()) 
      ln=SwapLong(ln);    // Basic Offset Table Item Lentgh
   if (ln != 0) {
      // What is it used for ??
      char * BasicOffsetTableItemValue= (char *)malloc(ln+1);
      fread(BasicOffsetTableItemValue,ln,1,fp); 
      guint32 a;
      for (int i=0;i<ln;i+=4){
         a=str2num(&BasicOffsetTableItemValue[i],guint32);
      }        
   }

   ftellRes=ftell(fp);
   fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
   fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
   if(GetSwapCode()) {
      ItemTagGr=SwapShort(ItemTagGr); 
      ItemTagEl=SwapShort(ItemTagEl);      
   }  

   // while 'Sequence Delimiter Item' (fffe,e0dd) not found
   while (  ( ItemTagGr == 0xfffe) && (ItemTagEl != 0xe0dd) ) { 
   // Parse fragments of the current Fragment (Frame)    
      ftellRes=ftell(fp);
      fread(&fragmentLength,4,1,fp); 
      if(GetSwapCode()) 
         fragmentLength=SwapLong(fragmentLength);    // length

          //------------------ scanning (not reading) fragment pixels
 
      fread(&nbRleSegments,4,1,fp);  // Reading : Number of RLE Segments        
      if(GetSwapCode()) 
         nbRleSegments=SwapLong(nbRleSegments);
 
      for(int k=1; k<=15; k++) { // Reading RLE Segments Offset Table
         ftellRes=ftell(fp);
         fread(&RleSegmentOffsetTable[k],4,1,fp);
         if(GetSwapCode())
            RleSegmentOffsetTable[k]=SwapLong(RleSegmentOffsetTable[k]);
      }

      if (nbRleSegments>1) { 
         for(int k=1; k<=nbRleSegments-1; k++) { // reading RLE Segments
            RleSegmentLength[k]=RleSegmentOffsetTable[k+1]-RleSegmentOffsetTable[k];
            ftellRes=ftell(fp);
            fragmentBegining=ftell(fp);   
            _gdcm_read_RLE_fragment (&im, RleSegmentLength[k],uncompressedSegmentSize,fp);
            fseek(fp,fragmentBegining,SEEK_SET);  
            fseek(fp,RleSegmentLength[k],SEEK_CUR);        
         }
      }
      RleSegmentLength[nbRleSegments] = fragmentLength - RleSegmentOffsetTable[nbRleSegments];
      ftellRes=ftell(fp);
      fragmentBegining=ftell(fp);
      _gdcm_read_RLE_fragment (&im, RleSegmentLength[nbRleSegments],uncompressedSegmentSize, fp);
      fseek(fp,fragmentBegining,SEEK_SET);  
      fseek(fp,RleSegmentLength[nbRleSegments],SEEK_CUR);    
      
      // end of scanning fragment pixels       
   
      ftellRes=ftell(fp);
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);      
      }
   } 
   
   if (GetBitsAllocated()==16) { // try to deal with RLE 16 Bits
   
      im = (char *)image_buffer;
         //  need to make 16 Bits Pixels from Low Byte and Hight Byte 'Planes'

      int l = GetXSize()*GetYSize();
      int nbFrames = GetZSize();

      char * newDest = (char*) malloc(l*nbFrames*2);
      char *x  = newDest;
      char * a = (char *)image_buffer;
      char * b = a + l;

      for (int i=0;i<nbFrames;i++) {
         for (int j=0;j<l; j++) {
            *(x++) = *(a++);
            *(x++) = *(b++);
         }
      }
      memmove(image_buffer,newDest,lgrTotale);
      free(newDest);   
   }
      
   return (1);
}


 /* -------------------------------------------------------------------- */
 //
 // RLE LossLess Fragment
 //
 /* -------------------------------------------------------------------- */

   // static because nothing but gdcm_read_RLE_file can call it
   // DO NOT doxygen !

static int
_gdcm_read_RLE_fragment (char ** areaToRead, 
                         long lengthToDecode, 
                         long uncompressedSegmentSize, 
                         FILE* fp) {
   long ftellRes;
   int count;
   long numberOfOutputBytes=0;
   char n, car;
   ftellRes =ftell(fp);

   while(numberOfOutputBytes<uncompressedSegmentSize) {

      ftellRes =ftell(fp);
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
