/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmParsePixels.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:48 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmCommon.h"
#include "gdcmFile.h"

#define str2num(str, typeNum) *((typeNum *)(str))

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmFile
 * \brief   Parse pixel data from disk and *prints* the result
 * \        For multi-fragment Jpeg/Rle files checking purpose *only*
 * \        Allows to 'see' if the file *does* conform
 * \       (some of them do not)
 * \        with Dicom Part 3, Annex A (PS 3.5-2003, page 58, page 85)
 *
 */
bool gdcmFile::ParsePixelData(void) {
// DO NOT remove the printf s.
// The ONLY purpose of this method is to PRINT the content
   FILE *fp;

   if ( !(fp=Header->OpenFile()))
      return false;
      
    if ( fseek(fp, Header->GetPixelOffset(), SEEK_SET) == -1 ) {
      Header->CloseFile();
      return false;
   } 
   
   if ( !Header->IsDicomV3()                             ||
        Header->IsImplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRLittleEndianTransferSyntax() ||
        Header->IsExplicitVRBigEndianTransferSyntax()    ||
        Header->IsDeflatedExplicitVRLittleEndianTransferSyntax() ) { 
        
        printf ("gdcmFile::ParsePixelData : non JPEG/RLE File\n");
        return false;       
   }        

   int nb;
   std::string str_nb=Header->GetEntryByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;
   }
   //int nBytes= nb/8;	//FIXME
      
   //int taille = Header->GetXSize() * Header->GetYSize() * Header->GetSamplesPerPixel(); 
         
   printf ("Checking the Dicom-encapsulated Jpeg/RLE Pixels\n");
      
   guint16 ItemTagGr,ItemTagEl; 
   int ln;
   long ftellRes;
   //char * destination = NULL;

  // -------------------- for Parsing : Position on begining of Jpeg/RLE Pixels 

   if( !Header->IsRLELossLessTransferSyntax()) {

      // JPEG Image
      ftellRes=ftell(fp);
      fread(&ItemTagGr,2,1,fp);  //Reading (fffe):Basic Offset Table Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  //Reading (e000):Basic Offset Table Item Tag El
      if(Header->GetSwapCode()) {
         ItemTagGr=Header->SwapShort(ItemTagGr); 
         ItemTagEl=Header->SwapShort(ItemTagEl);            
      }
      printf ("at %x : ItemTag (should be fffe,e000): %04x,%04x\n",
                (unsigned)ftellRes,ItemTagGr,ItemTagEl );
      ftellRes=ftell(fp);
      fread(&ln,4,1,fp); 
      if(Header->GetSwapCode()) 
         ln=Header->SwapLong(ln);    // Basic Offset Table Item Length
      printf("at %x : Basic Offset Table Item Length (\?\?) %d x(%08x)\n",
            (unsigned)ftellRes,ln,ln);
      if (ln != 0) {
         // What is it used for ??
         char * BasicOffsetTableItemValue= new char[ln+1];
         fread(BasicOffsetTableItemValue,ln,1,fp); 
         guint32 a;
         for (int i=0;i<ln;i+=4){
            a=str2num(&BasicOffsetTableItemValue[i],guint32);
            printf("      x(%08x)  %d\n",a,a);
         }              
      }
      
      ftellRes=ftell(fp);
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(Header->GetSwapCode()) {
         ItemTagGr=Header->SwapShort(ItemTagGr); 
         ItemTagEl=Header->SwapShort(ItemTagEl);            
      }  
      printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
            (unsigned)ftellRes,ItemTagGr,ItemTagEl );
      
      while ( ( ItemTagGr==0xfffe) && (ItemTagEl!=0xe0dd) ) { // Parse fragments
      
         ftellRes=ftell(fp);
         fread(&ln,4,1,fp); 
         if(Header->GetSwapCode()) 
            ln=Header->SwapLong(ln);    // length
         printf("      at %x : fragment length %d x(%08x)\n",
                (unsigned)ftellRes, ln,ln);

        // destination += taille * nBytes; // location in user's memory        
        //printf ("      Destination will be x(%x) = %d \n",
        //     destination,destination );

         // ------------------------                                     
         fseek(fp,ln,SEEK_CUR); // skipping (not reading) fragment pixels    
         // ------------------------              
     
         ftellRes=ftell(fp);
         fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
         fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
         if(Header->GetSwapCode()) {
            ItemTagGr=Header->SwapShort(ItemTagGr); 
            ItemTagEl=Header->SwapShort(ItemTagEl);            
         }
         printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
               (unsigned)ftellRes,ItemTagGr,ItemTagEl );
      } 

   } else {

      // RLE Image
      long RleSegmentLength[15],fragmentLength;
      guint32 nbRleSegments;
      guint32 RleSegmentOffsetTable[15];
      ftellRes=ftell(fp);
      // Basic Offset Table with Item Value
         // Item Tag
      fread(&ItemTagGr,2,1,fp);  //Reading (fffe):Basic Offset Table Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  //Reading (e000):Basic Offset Table Item Tag El
      if(Header->GetSwapCode()) {
         ItemTagGr=Header->SwapShort(ItemTagGr); 
         ItemTagEl=Header->SwapShort(ItemTagEl);            
      }
      printf ("at %x : ItemTag (should be fffe,e000): %04x,%04x\n",
                (unsigned)ftellRes,ItemTagGr,ItemTagEl );
         // Item Length
      ftellRes=ftell(fp);
      fread(&ln,4,1,fp); 
      if(Header->GetSwapCode()) 
         ln=Header->SwapLong(ln);    // Basic Offset Table Item Length
      printf("at %x : Basic Offset Table Item Length (\?\?) %d x(%08x)\n",
            (unsigned)ftellRes,ln,ln);
      if (ln != 0) {
         // What is it used for ??
         char * BasicOffsetTableItemValue= new char[ln+1];
         fread(BasicOffsetTableItemValue,ln,1,fp); 
         guint32 a;
         for (int i=0;i<ln;i+=4){
            a=str2num(&BasicOffsetTableItemValue[i],guint32);
            printf("      x(%08x)  %d\n",a,a);
         }              
      }

      ftellRes=ftell(fp);
      fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
      if(Header->GetSwapCode()) {
         ItemTagGr=Header->SwapShort(ItemTagGr); 
         ItemTagEl=Header->SwapShort(ItemTagEl);            
      }  
      printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
            (unsigned)ftellRes,ItemTagGr,ItemTagEl );

      // while 'Sequence Delimiter Item' (fffe,e0dd) not found
      while (  ( ItemTagGr == 0xfffe) && (ItemTagEl != 0xe0dd) ) { 
      // Parse fragments of the current Fragment (Frame)    
         ftellRes=ftell(fp);
         fread(&fragmentLength,4,1,fp); 
         if(Header->GetSwapCode()) 
            fragmentLength=Header->SwapLong(fragmentLength);    // length
         printf("      at %x : 'fragment' length %d x(%08x)\n",
                (unsigned)ftellRes, (unsigned)fragmentLength,(unsigned)fragmentLength);
                       
          //------------------ scanning (not reading) fragment pixels
 
         fread(&nbRleSegments,4,1,fp);  // Reading : Number of RLE Segments
         if(Header->GetSwapCode()) 
            nbRleSegments=Header->SwapLong(nbRleSegments);
            printf("   Nb of RLE Segments : %d\n",nbRleSegments);
 
         for(int k=1; k<=15; k++) { // Reading RLE Segments Offset Table
            ftellRes=ftell(fp);
            fread(&RleSegmentOffsetTable[k],4,1,fp);
            if(Header->GetSwapCode())
               RleSegmentOffsetTable[k]=Header->SwapLong(RleSegmentOffsetTable[k]);
            printf("        at : %x Offset Segment %d : %d (%x)\n",
                    (unsigned)ftellRes,k,RleSegmentOffsetTable[k],
                    RleSegmentOffsetTable[k]);
         }

          if (nbRleSegments>1) { // skipping (not reading) RLE Segments
             for(unsigned int k=1; k<=nbRleSegments-1; k++) { 
                RleSegmentLength[k]=   RleSegmentOffsetTable[k+1]
                                     - RleSegmentOffsetTable[k];
                ftellRes=ftell(fp);
                printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                           k,(unsigned)RleSegmentLength[k],(unsigned)RleSegmentLength[k], (unsigned)ftellRes);
                fseek(fp,RleSegmentLength[k],SEEK_CUR);    
             }
          }
          RleSegmentLength[nbRleSegments]= fragmentLength 
                                         - RleSegmentOffsetTable[nbRleSegments];
          ftellRes=ftell(fp);
          printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                           nbRleSegments,(unsigned)RleSegmentLength[nbRleSegments],
                           (unsigned)RleSegmentLength[nbRleSegments],(unsigned)ftellRes);

          fseek(fp,RleSegmentLength[nbRleSegments],SEEK_CUR); 
            
         // ------------------ end of scanning fragment pixels        
      
         ftellRes=ftell(fp);
         fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
         fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
         if(Header->GetSwapCode()) {
            ItemTagGr=Header->SwapShort(ItemTagGr); 
            ItemTagEl=Header->SwapShort(ItemTagEl);            
         }
         printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
               (unsigned)ftellRes,ItemTagGr,ItemTagEl );
      } 
   }
   return true;            
}

//-----------------------------------------------------------------------------
