// gdcmDocEntrySet.cxx
//-----------------------------------------------------------------------------
//
#include <errno.h>
#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmGlobal.h"
#include "gdcmDocEntrySet.h"
#include "gdcmException.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntrySet
 * \brief   Constructor from a given gdcmDocEntrySet
 */
gdcmDocEntrySet::gdcmDocEntrySet() {
}
/**
 * \brief   Canonical destructor.
 */
gdcmDocEntrySet::~gdcmDocEntrySet(){
}
//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntrySet
 * \brief   canonical Printer
 */


//-----------------------------------------------------------------------------
// Public
//bool gdcmDocEntrySet::AddEntry(gdcmDocEntry *Entry){return true;}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

/**
 * \brief   Parses an EntrySet (Document header or SQ Item )
 * \       and load element values (a voir !)
 * @return  false anything wrong happens 
 */
 
 /* just to keep the code
 
bool gdcmDocEntrySet::LoadDocEntrySet(bool exception_on_error) 
                   throw(gdcmFormatError) {
   (void)exception_on_error;

   gdcmDocEntry  *newDocEntry = (gdcmDocEntry *)0;     
   gdcmValEntry  *newValEntry = (gdcmValEntry *)0; 
   gdcmBinEntry  *newBinEntry = (gdcmBinEntry *)0; 
   gdcmSeqEntry  *newSeqEntry = (gdcmSeqEntry *)0;  
   
   string vr;
   while (newDocEntry = ReadNextDocEntry()) { 
   // TODO (?) : liberation du DocEntry ainsi cree, 
   // apres copie dans un ValEntry, SeqEntry, BinEntry   
      vr = newDocEntry->GetVR();
         
      if (vr == "SQ" ) {
      // --- SeqEntry
      
         newSeqEntry = new gdcmSeqEntry((gdcmDictEntry*)NULL);
	 if (!newSeqEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmSeqEntry");
            return false;			   	 
         }	 
         newSeqEntry->Copy(newDocEntry);            
      // TODO
      // SEQUENCE; appel 'récursif' de ??? pour charger la 'valeur'
      //           (ensemble d' ITEMs, en fait, 
      //            chaque ITEM etant chargé avec LoadDocEntrySet)
            
         //SkipDocEntry(newSeqEntry); // voir ce qu'on fait pour une SeQuence
         AddEntry(newSeqEntry); // on appele la fonction generique, 
	                        // ou une fonction spécialisée ?
	 
      } else  if (vr == "AE" || vr == "AS" || vr == "DA" || vr == "PN" || 
                  vr == "UI" || vr == "TM" || vr == "SH" || vr == "LO" ||
		  vr == "CS" || vr == "IS" || vr == "LO" || vr == "LT" ||
		  vr == "SH" || vr == "ST" || 		  
		  vr == "SL" || vr == "SS" || vr == "UL" || vr == "US"
		                                                        ) {
      // --- ValEntry 		  
		  
         newValEntry = new gdcmValEntry((gdcmDictEntry*)NULL);
	 if (!newValEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmValEntry");
            return false;			   	 
         }	 
         newValEntry->Copy(newDocEntry);
         //SkipDocEntry(newValEntry); //le skip devrait etre fait dans le Read 
         AddEntry(newValEntry); // on appele la fonction generique, 
	                        // ou une fonction spécialisée ? 

        // Maybe the following VR do correspond to a BinEntry 
		
        //AT Attribute Tag;         // 2 16-bit unsigned short integers
        //FL Floating Point Single; // 32-bit IEEE 754:1985 float
        //FD Floating Point Double; // 64-bit IEEE 754:1985 double
        //UN Unknown;               // Any length of bytes
        //UT Unlimited Text;        // At most 2^32 -1 chars
	//OB Other Byte String;     // String of bytes (VR independant)
        //OW Other Word String;     // String of 16-bit words (VR dependant) 	 	 		  		  
      }	else { 
      // --- BinEntry
      
         newBinEntry = new gdcmBinEntry((gdcmDictEntry*)NULL);     
	 if (!newBinEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmBinEntry");
            return false;			   	 
         }      
         newBinEntry->Copy(newDocEntry);
        // SkipDocEntry(newBinEntry); //le skip devrait etre fait dans le Read 
         AddEntry(newBinEntry); // on appele la fonction generique, 
	                        // ou une fonction spécialisée ?	      
      }	                     
   }   



   // TODO : il n'y a plus de Chained List qui contient toutes les Entries 
   //        Le chargement des valeurs devra se faire à la volée  
   // Be carefull : merging this two loops may cause troubles ...
   
/* 
   rewind(fp);  
   for (ListTag::iterator i = GetListEntry().begin();                           
        i != GetListEntry().end();                                                
        ++i)                                                                      
   {                                                                            
      LoadDocEntry(*i);                                                      
   }                                                                            
   rewind(fp);
   
   */
 
 /* TO DO : deporter den fin de parsing du DOCUMENT (pas du EntrySet) 
  
   // --------------------------------------------------------------
   // Special Patch to allow gdcm to read ACR-LibIDO formated images
   //
   // if recognition code tells us we deal with a LibIDO image
   // we switch lineNumber and columnNumber
   //
   std::string RecCode;
   RecCode = GetEntryByNumber(0x0008, 0x0010); // recognition code
   if (RecCode == "ACRNEMA_LIBIDO_1.1" ||
       RecCode == "CANRME_AILIBOD1_1." )  // for brain-damaged softwares
                                          // with "little-endian strings"
   {
         filetype = ACR_LIBIDO; 
         std::string rows    = GetEntryByNumber(0x0028, 0x0010);
         std::string columns = GetEntryByNumber(0x0028, 0x0011);
         SetEntryByNumber(columns, 0x0028, 0x0010);
         SetEntryByNumber(rows   , 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ---------------- 
   */ 
   
   /* 
   return true;
}

*/



//-----------------------------------------------------------------------------
