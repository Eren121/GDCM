// gdcmParse.cxx

//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'type' : forcing value to bool 'true' or 'false' (performance warning)
//#pragma warning ( disable : 4800 )
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
#endif //_MSC_VER

#include "gdcmFile.h"
#include "gdcmUtil.h"

#define str2num(str, typeNum) *((typeNum *)(str))

/////////////////////////////////////////////////////////////////
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

   if ( !OpenFile())
      return false;
      
    if ( fseek(fp, GetPixelOffset(), SEEK_SET) == -1 ) {
      CloseFile();
      return false;
   } 
   
   if ( !IsDicomV3()                             ||
        IsImplicitVRLittleEndianTransferSyntax() ||
        IsExplicitVRLittleEndianTransferSyntax() ||
        IsExplicitVRBigEndianTransferSyntax()    ||
        IsDeflatedExplicitVRLittleEndianTransferSyntax() ) { 
        
        printf ("gdcmFile::ParsePixelData : non JPEG/RLE File\n");
        return 0;       
   }        

   int nb;
   std::string str_nb=gdcmHeader::GetPubElValByNumber(0x0028,0x0100);
   if (str_nb == GDCM_UNFOUND ) {
      nb = 16;
   } else {
      nb = atoi(str_nb.c_str() );
      if (nb == 12) nb =16;
   }
   int nBytes= nb/8;
      
   int taille = GetXSize() *  GetYSize()  * GetSamplesPerPixel(); 
         
   printf ("Checking the Dicom-encapsulated Jpeg/RLE Pixels\n");
      
   guint16 ItemTagGr,ItemTagEl; 
   int ln;
   long ftellRes;
   char * destination = NULL;

  // -------------------- for Parsing : Position on begining of Jpeg/RLE Pixels 

   if( !IsRLELossLessTransferSyntax()) {

      // JPEG Image

      std::cout << "JPEG image" << std::endl;
      ftellRes=ftell(fp);
      fread(&ItemTagGr,2,1,fp);  //Reading (fffe):Basic Offset Table Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  //Reading (e000):Basic Offset Table Item Tag El
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }
      printf ("at %x : ItemTag (should be fffe,e000): %04x,%04x\n",
                ftellRes,ItemTagGr,ItemTagEl );
      ftellRes=ftell(fp);
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);    // Basic Offset Table Item Lentgh
      printf("at %x : Basic Offset Table Item Lentgh (??) %d x(%08x)\n",
            ftellRes,ln,ln);
      if (ln != 0) {
         // What is it used for ??
         char * BasicOffsetTableItemValue= (char *)malloc(ln+1);
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
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }  
      printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
            ftellRes,ItemTagGr,ItemTagEl );
      
      while ( ( ItemTagGr==0xfffe) && (ItemTagEl!=0xe0dd) ) { // Parse fragments
      
         ftellRes=ftell(fp);
         fread(&ln,4,1,fp); 
         if(GetSwapCode()) 
            ln=SwapLong(ln);    // length
         printf("      at %x : fragment length %d x(%08x)\n",
                ftellRes, ln,ln);

        // destination += taille * nBytes; // location in user's memory        
        //printf ("      Destination will be x(%x) = %d \n",
        //     destination,destination );

         // ------------------------                                     
         fseek(fp,ln,SEEK_CUR); // skipping (not reading) fragment pixels    
         // ------------------------              
     
         ftellRes=ftell(fp);
         fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
         fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
         if(GetSwapCode()) {
            ItemTagGr=SwapShort(ItemTagGr); 
            ItemTagEl=SwapShort(ItemTagEl);            
         }
         printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
               ftellRes,ItemTagGr,ItemTagEl );
      } 

   } else {

      // RLE Image

      std::cout << "RLE image" << std::endl;
      long RleSegmentLength[15],fragmentLength;
      guint32 nbRleSegments;
      guint32 RleSegmentOffsetTable[15];
      ftellRes=ftell(fp);
      // Basic Offset Table with Item Value
         // Item Tag
      fread(&ItemTagGr,2,1,fp);  //Reading (fffe):Basic Offset Table Item Tag Gr
      fread(&ItemTagEl,2,1,fp);  //Reading (e000):Basic Offset Table Item Tag El
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }
      printf ("at %x : ItemTag (should be fffe,e000): %04x,%04x\n",
                ftellRes,ItemTagGr,ItemTagEl );
         // Item Length
      ftellRes=ftell(fp);
      fread(&ln,4,1,fp); 
      if(GetSwapCode()) 
         ln=SwapLong(ln);    // Basic Offset Table Item Lentgh
      printf("at %x : Basic Offset Table Item Lentgh (??) %d x(%08x)\n",
            ftellRes,ln,ln);
      if (ln != 0) {
         // What is it used for ??
         char * BasicOffsetTableItemValue= (char *)malloc(ln+1);
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
      if(GetSwapCode()) {
         ItemTagGr=SwapShort(ItemTagGr); 
         ItemTagEl=SwapShort(ItemTagEl);            
      }  
      printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
            ftellRes,ItemTagGr,ItemTagEl );

      // while 'Sequence Delimiter Item' (fffe,e0dd) not found
      while (  ( ItemTagGr == 0xfffe) && (ItemTagEl != 0xe0dd) ) { 
      // Parse fragments of the current Fragment (Frame)    
         ftellRes=ftell(fp);
         fread(&fragmentLength,4,1,fp); 
         if(GetSwapCode()) 
            fragmentLength=SwapLong(fragmentLength);    // length
         printf("      at %x : 'fragment' length %d x(%08x)\n",
                ftellRes, fragmentLength,fragmentLength);
                       
          //------------------ scanning (not reading) fragment pixels
 
         fread(&nbRleSegments,4,1,fp);  // Reading : Number of RLE Segments
         if(GetSwapCode()) 
            nbRleSegments=SwapLong(nbRleSegments);
            printf("   Nb of RLE Segments : %d\n",nbRleSegments);
 
         for(int k=1; k<=15; k++) { // Reading RLE Segments Offset Table
            ftellRes=ftell(fp);
            fread(&RleSegmentOffsetTable[k],4,1,fp);
            if(GetSwapCode())
               RleSegmentOffsetTable[k]=SwapLong(RleSegmentOffsetTable[k]);
            printf("        at : %x Offset Segment %d : %d (%x)\n",
                    ftellRes,k,RleSegmentOffsetTable[k],
                    RleSegmentOffsetTable[k]);
         }

          if (nbRleSegments>1) { // skipping (not reading) RLE Segments
             for(int k=1; k<=nbRleSegments-1; k++) { 
                RleSegmentLength[k]=   RleSegmentOffsetTable[k+1]
                                     - RleSegmentOffsetTable[k];
                ftellRes=ftell(fp);
                printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                           k,RleSegmentLength[k],RleSegmentLength[k], ftellRes);
                fseek(fp,RleSegmentLength[k],SEEK_CUR);    
             }
          }
          RleSegmentLength[nbRleSegments]= fragmentLength 
                                         - RleSegmentOffsetTable[nbRleSegments];
          ftellRes=ftell(fp);
          printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                           nbRleSegments,RleSegmentLength[nbRleSegments],
                           RleSegmentLength[nbRleSegments],ftellRes);

          fseek(fp,RleSegmentLength[nbRleSegments],SEEK_CUR); 
            
         // ------------------ end of scanning fragment pixels        
      
         ftellRes=ftell(fp);
         fread(&ItemTagGr,2,1,fp);  // Reading (fffe) : Item Tag Gr
         fread(&ItemTagEl,2,1,fp);  // Reading (e000) : Item Tag El
         if(GetSwapCode()) {
            ItemTagGr=SwapShort(ItemTagGr); 
            ItemTagEl=SwapShort(ItemTagEl);            
         }
         printf ("at %x : ItemTag (should be fffe,e000 or e0dd): %04x,%04x\n",
               ftellRes,ItemTagGr,ItemTagEl );
      } 
   }
   return 1;            
}
