// gdcmDocEntrySet.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmDocEntrySet.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include <iomanip> // for std::ios::left, ...


//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntrySEt
 * \brief   Constructor from a given gdcmDocEntrySet
 */
gdcmDocEntrySet::gdcmDocEntrySet() {

}

//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntrySet
 * \brief   canonical Printer
 */


//-----------------------------------------------------------------------------
// Public


//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

/**
 * \brief   Parses an EntrySet (Document header or SQ Item )
 * \       and load element values (a voir !)
 * @return  false if file is not ACR-NEMA / PAPYRUS / DICOM 
 */
bool gdcmDocument::LoadDocEntrySet(bool exception_on_error) throw(gdcmFormatError) {
   (void)exception_on_error;
   rewind(fp);
   if (!CheckSwap())
      return false;

   guint16 g, n;      
   gdcmValEntry  *newValEntry = (gdcmValEntry *)0; 
   gdcmBinEntry  *newBinEntry = (gdcmBinEntry *)0; 
   gdcmSeqEntry  *newSeqEntry = (gdcmSeqEntry *)0;  
   gdcmDictEntry *NewTag      = (gdcmDictEntry *)0;
   char VR[3];
   long PositionOnEntry;
     
//   while ( (newHeaderEntry = ReadNextHeaderEntry()) ) {
   while (1) {
   
   // ----------------------- was ReadNextHeaderEntry -----------------
      g = ReadInt16();
      n = ReadInt16();   
      if (errno == 1)
      // We reached the EOF (or an error occured) therefore 
      // header parsing has to be considered as finished.
         break;
	 
   // Find out if the tag we encountered is in the dictionaries:
      DictEntry = GetDictEntryByNumber(Group, Elem);
      if (!DictEntry)
         DictEntry = NewVirtualDictEntry(Group, Elem);
	 if (!DictEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmDictEntry");
            return false;			   	 
         }			   
	 
                               // Right now, 	 
      vr = DictEntry->GetVR(); // Suppose we get it, suppose it's the same one
                               // that's in the Explicit VR part of the Header
                               // TODO
                               // Resoudre pb quand inadequation entre Explicit VR et Dict 
			       
      if (filetype != ExplicitVR) {} // jamais de risque de conflit
      // pour les autres cas,
      // refaire l'equiv de gdcmParser::FindHeaderEntryVR
      //                   qui contient CheckHeaderEntryVR 
      // le pb, c'est qu'on a besoin de la VR pour fabriquer l'Entry    
           			             
      if (vr == "SQ" ) {
       // --- SeqEntry      
      // TODO
      // SEQUENCE; appel 'récursif' de ??? pour charger la 'valeur'
      //           (ensemble d' ITEMs, en fait, 
      //            chaque ITEM etant chargé avec LoadDocEntrySet)
      
      
      } else  if (vr == "AE" || vr == "AS" || vr == "DA" || vr == "PN" || 
          vr == "UI" || vr == "TM" ) {	
      // --- ValEntry 
         NewValEntry = new gdcmValEntry(DictEntry);
	 if (!NewValEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmValEntry");
            return false;			   	 
         }
         FindHeaderEntryVR(NewEntry);
         FindHeaderEntryLength(NewEntry);	 	    
      }	
      
      
      else {
      // --- BinEntry
         NewBinEntry = new gdcmBinEntry(DictEntry);     
	 if (!NewValEntry) {
            dbg.Verbose(1, "gdcmDocEntrySet::LoadDocEntrySet",
                           "failed to allocate gdcmBinEntry");
            return false;			   	 
         }
    }



// ------------- end of former ReadNextHeaderEntry -----------------
             	      
     SkipHeaderEntry(newHeaderEntry);
     if ( (ignoreShadow==0) || (newHeaderEntry->GetGroup()%2) == 0) { 
        AddHeaderEntry(newHeaderEntry); 
     }     
   }   
   rewind(fp);
   // Be carefull : merging this two loops may cause troubles ...
   for (ListTag::iterator i = GetListEntry().begin();                           
        i != GetListEntry().end();                                                
        ++i)                                                                      
   {                                                                            
      LoadHeaderEntry(*i);                                                      
   }                                                                            
   rewind(fp);
 
   
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
   return true;
}



/**
 * \brief     Check the correspondance between the VR of the header entry
 *            and the taken VR. If they are different, the header entry is 
 *            updated with the new VR.
 * @param     Entry Header Entry to check
 * @param     vr    Dicom Value Representation
 * @return    false if the VR is incorrect of if the VR isn't referenced
 *            otherwise, it returns true
*/

// NE MARCHE PAS EN L'ETAT :
// On a besoin de VR pour 'fabriquer', au choix ValEntry, BinEntry, ou SeqEntry.
//

bool gdcmDocEntrySet::CheckEntryVR(gdcmHeaderEntry *Entry, VRKey vr)
{
   char msg[100]; // for sprintf
   bool RealExplicit = true;

   // Assume we are reading a falsely explicit VR file i.e. we reached
   // a tag where we expect reading a VR but are in fact we read the
   // first to bytes of the length. Then we will interogate (through find)
   // the dicom_vr dictionary with oddities like "\004\0" which crashes
   // both GCC and VC++ implementations of the STL map. Hence when the
   // expected VR read happens to be non-ascii characters we consider
   // we hit falsely explicit VR tag.

   if ( (!isalpha(vr[0])) && (!isalpha(vr[1])) )
      RealExplicit = false;

   // CLEANME searching the dicom_vr at each occurence is expensive.
   // PostPone this test in an optional integrity check at the end
   // of parsing or only in debug mode.
   if ( RealExplicit && !gdcmGlobal::GetVR()->Count(vr) )
      RealExplicit= false;

   if ( !RealExplicit ) 
   {
      // We thought this was explicit VR, but we end up with an
      // implicit VR tag. Let's backtrack.   
      sprintf(msg,"Falsely explicit vr file (%04x,%04x)\n", 
                   Entry->GetGroup(),Entry->GetElement());
      dbg.Verbose(1, "gdcmParser::FindVR: ",msg);
      if (Entry->GetGroup()%2 && Entry->GetElement() == 0x0000) { // Group length is UL !
         gdcmDictEntry* NewEntry = NewVirtualDictEntry(
                                   Entry->GetGroup(),Entry->GetElement(),
                                   "UL","FIXME","Group Length");
         Entry->SetDictEntry(NewEntry);     
      }
      return(false);
   }

   if ( Entry->IsVRUnknown() ) 
   {
      // When not a dictionary entry, we can safely overwrite the VR.
      if (Entry->GetElement() == 0x0000) { // Group length is UL !
         Entry->SetVR("UL");
      } else {
         Entry->SetVR(vr);
      }
   }
   else if ( Entry->GetVR() != vr ) 
   {
      // The VR present in the file and the dictionary disagree. We assume
      // the file writer knew best and use the VR of the file. Since it would
      // be unwise to overwrite the VR of a dictionary (since it would
      // compromise it's next user), we need to clone the actual DictEntry
      // and change the VR for the read one.
      gdcmDictEntry* NewEntry = NewVirtualDictEntry(
                                 Entry->GetGroup(),Entry->GetElement(),
                                 vr,"FIXME",Entry->GetName());
      Entry->SetDictEntry(NewEntry);
   }
   return(true); 
}




//-----------------------------------------------------------------------------
