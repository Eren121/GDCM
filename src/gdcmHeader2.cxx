// gdcmHeader2.cxx
//-----------------------------------------------------------------------------
#include "gdcmUtil.h"
#include "gdcmHeader.h"
#include "gdcmTS.h"
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#include <iomanip> // for std::ios::left, ...

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmHeader
 * \brief prints the Dicom Elements of the gdcmHeader
 *        using both H table and Chained List
 * @param   os The output stream to be written to.  
 */
void gdcmHeader::Print(std::ostream & os) {

   size_t o;
   unsigned short int g, e;
   TSKey v;
   std::string d2;
   gdcmTS * ts = gdcmGlobal::GetTS();
   std::ostringstream s;   
	   
   guint32 lgth;
   char greltag[10];  //group element tag
 
   s << "------------ using listEntries ----------------" << std::endl; 

   char st[20];
   for (ListTag::iterator i = listEntries.begin();  
	   i != listEntries.end();
	   ++i){
	(*i)->SetPrintLevel(printLevel);
	(*i)->Print(os);   
   } 
   os<<s.str();
} 

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmHeader
 * \brief  add a new Dicom Element pointer to 
 *         the H Table and to the chained List
 * \warning  push_bash in listEntries ONLY during ParseHeader
 * \todo  something to allow further Elements addition,
 * \      when position to be taken care of     
 * @param   newHeaderEntry
 */
void gdcmHeader::Add(gdcmHeaderEntry * newHeaderEntry) {

// tagHT [newHeaderEntry->GetKey()]  = newHeaderEntry;
   tagHT.insert( PairHT( newHeaderEntry->GetKey(),newHeaderEntry) );
   listEntries.push_back(newHeaderEntry); 
   wasUpdated = 1;
}


/**
 * \ingroup gdcmHeader
 * \brief   Sets a 'non string' value to a given Dicom Element
 * @param   area
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
bool gdcmHeader::SetVoidAreaByNumber(void * area,
                                      guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   //tagHT[key]->SetVoidArea(area);
   ( ((tagHT.equal_range(key)).first)->second )->SetVoidArea(area);	 
   return true ;		
}

/**
 * \ingroup gdcmHeader
 * \brief   Generate a free TagKey i.e. a TagKey that is not present
 *          in the TagHt dictionary.
 * @param   group The generated tag must belong to this group.  
 * @return  The element of tag with given group which is fee.
 */
guint32 gdcmHeader::GenerateFreeTagKeyInGroup(guint16 group) {
   for (guint32 elem = 0; elem < UINT32_MAX; elem++) {
      TagKey key = gdcmDictEntry::TranslateToKey(group, elem);
      if (tagHT.count(key) == 0)
         return elem;
   }
   return UINT32_MAX;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmHeader
 * \brief   Re-computes the length of a ACR-NEMA/Dicom group from a DcmHeader
 * \warning : to be re-written using the chained list instead of the H table.
 * \warning : DO NOT use (doesn't work any longer because of the multimap)
 * \todo : to be re-written using the chained list instead of the H table
 * @param   SkipSequence TRUE if we don't want to write Sequences (ACR-NEMA Files)
 * @param   type Type of the File (ExplicitVR,ImplicitVR, ACR, ...) 
 */
void gdcmHeader::UpdateGroupLength(bool SkipSequence, FileType type) {
   guint16 gr, el;
   std::string vr;
   
   gdcmHeaderEntry *elem;
   char trash[10];
   std::string str_trash;
   
   GroupKey key;
   GroupHT groupHt;  // to hold the length of each group
   TagKey tk;
   // remember :
   // typedef std::map<GroupKey, int> GroupHT;
   
   gdcmHeaderEntry *elemZ;
  
   // for each Tag in the DCM Header
   
   for (TagHeaderEntryHT::iterator tag2 = tagHT.begin(); 
        tag2 != tagHT.end();
        ++tag2){

      elem  = tag2->second;
      gr = elem->GetGroup();
      el = elem->GetElement();
      vr = elem->GetVR(); 
                 
      sprintf(trash, "%04x", gr);
      key = trash;		// generate 'group tag'
      
      // if the caller decided not to take SEQUENCEs into account 
      // e.g : he wants to write an ACR-NEMA File 
                
      if (SkipSequence && vr == "SQ") continue;
      
         // Still unsolved problem :
         // we cannot find the 'Sequence Delimitation Item'
         // since it's at the end of the Hash Table
         // (fffe,e0dd) 
             
         // pas SEQUENCE en ACR-NEMA
         // WARNING : 
         // --> la descente a l'interieur' des SQ 
         // devrait etre faite avec une liste chainee, pas avec une HTable...
            
      if ( groupHt.count(key) == 0) { // we just read the first elem of a given group
         if (el == 0x0000) {	      // the first elem is 0x0000
            groupHt[key] = 0;	      // initialize group length 
         } else {
            groupHt[key] = 2 + 2 + 4 + elem->GetLength(); // non 0x0000 first group elem
         } 
      } else {   // any elem but the first    
         if (type == ExplicitVR) {
            if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {
               groupHt[key] +=  4; // explicit VR AND OB, OW, SQ : 4 more bytes
            }
         }
         groupHt[key] += 2 + 2 + 4 + elem->GetLength(); 
      } 
   }

   unsigned short int gr_bid;
  
   for (GroupHT::iterator g = groupHt.begin(); // for each group we found
        g != groupHt.end();
        ++g){ 
      // FIXME: g++ -Wall -Wstrict-prototypes reports on following line:
      //        warning: unsigned int format, different type arg
      sscanf(g->first.c_str(),"%x",&gr_bid);
      tk = g->first + "|0000";			// generate the element full tag
                     
      if ( tagHT.count(tk) == 0) { 		// if element 0x0000 not found
         gdcmDictEntry * tagZ = new gdcmDictEntry(gr_bid, 0x0000, "UL");       
         elemZ = new gdcmHeaderEntry(tagZ);
         elemZ->SetLength(4);
         Add(elemZ);				// create it
      } else {
         elemZ=GetHeaderEntryByNumber(gr_bid, 0x0000);
      }     
      sprintf(trash ,"%d",g->second);
      str_trash=trash;
      elemZ->SetValue(str_trash);
   }   
}

/**
 * \ingroup gdcmHeader
 * \brief   writes on disc according to the requested format
 * \        (ACR-NEMA, ExplicitVR, ImplicitVR) the image
 * \ warning does NOT add the missing elements in the header :
 * \         it's up to the user doing it !
 * \         (function CheckHeaderCoherence to be written)
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @param   _fp already open file pointer
 */
void gdcmHeader::WriteEntries(FileType type, FILE * _fp) {
   guint16 gr, el;
   guint32 lgr;
   const char * val;
   std::string vr;
   guint32 val_uint32;
   guint16 val_uint16;
   
   std::vector<std::string> tokens;
   
   //  uses now listEntries to iterate, not TagHt!
   //
   //        pb : gdcmHeader.Add does NOT update listEntries
   //       TODO : find a trick (in STL?) to do it, at low cost !

   void *ptr;

   // TODO (?) tester les echecs en ecriture (apres chaque fwrite)

   for (ListTag::iterator tag2=listEntries.begin();
        tag2 != listEntries.end();
        ++tag2){

      gr =  (*tag2)->GetGroup();
      el =  (*tag2)->GetElement();
      lgr = (*tag2)->GetLength();
      val = (*tag2)->GetValue().c_str();
      vr =  (*tag2)->GetVR();
      
      if ( type == ACR ) { 
         if (gr < 0x0008)   continue; // ignore pure DICOM V3 groups
         if (gr %2)         continue; // ignore shadow groups
         if (vr == "SQ" )   continue; // ignore Sequences
	           // TODO : find a trick to *skip* the SeQuences !
		   // Not only ignore the SQ element
         if (gr == 0xfffe ) continue; // ignore delimiters
      } 

      fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp);  //group
      fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp);  //element

      if ( (type == ExplicitVR) && (gr <= 0x0002) ) {
         // EXPLICIT VR
         guint16 z=0, shortLgr;
         fwrite (vr.c_str(),(size_t)2 ,(size_t)1 ,_fp);

         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {
            fwrite ( &z,  (size_t)2 ,(size_t)1 ,_fp);
            fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);

         } else {
            shortLgr=lgr;
            fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,_fp);
         }
      } else { // IMPLICIT VR
         fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);
      }

      if (vr == "US" || vr == "SS") {
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize ((*tag2)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) {
            val_uint16 = atoi(tokens[i].c_str());
            ptr = &val_uint16;
            fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);
         }
         tokens.clear();
         continue;
      }
      if (vr == "UL" || vr == "SL") {
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize ((*tag2)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) {
            val_uint32 = atoi(tokens[i].c_str());
            ptr = &val_uint32;
            fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);
         }
         tokens.clear();
         continue;
      }     
      // Pixels are never loaded in the element !
      if ((gr == 0x7fe0) && (el == 0x0010) ) break;

      fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); // Elem value
   }
}

//-----------------------------------------------------------------------------
