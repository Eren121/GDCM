// gdcmHeaderEntrySet.cxx
//-----------------------------------------------------------------------------
#include "gdcmUtil.h"
#include "gdcmHeaderEntrySet.h"
#include "gdcmTS.h"
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif

#include <iomanip> //la bibli qui va bien

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief  Destructor 
 */
gdcmHeaderEntrySet::~gdcmHeaderEntrySet() {
   for (TagHeaderEntryHT::iterator tag = tagHT.begin(); tag != tagHT.end(); ++tag) {
      gdcmHeaderEntry* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
      tag->second=NULL;
   }

   tagHT.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief prints the Dicom Elements of the gdcmHeader
 *        using both H table and Chained List
 * @param   os The output stream to be written to.  
 */
void gdcmHeaderEntrySet::Print(std::ostream & os) {

   size_t o;
   unsigned short int g, e;
   TSKey v;
   std::string d2;
   gdcmTS * ts = gdcmGlobal::GetTS();

   std::ostringstream s;
   
   // Tag HT
   s << "------------- using tagHT ---------------------" << std::endl; 
   for (TagHeaderEntryHT::iterator tag = tagHT.begin();
	   tag != tagHT.end();
	   ++tag){
      g = tag->second->GetGroup();
      e = tag->second->GetElement();
      v = tag->second->GetValue();
      o = tag->second->GetOffset();
      d2 = _CreateCleanString(v);  // replace non printable characters by '.'

      s << tag->first << ": ";
      s << " lgr : " <<tag->second->GetLength();
      s << ",\t Offset : " << o;
      s << " x(" << std::hex << o << std::dec << ") ";
      s << "\t[" << tag->second->GetVR()    << "]";
      s << "\t[" << tag->second->GetName()  << "]";       
      s << "\t[" << d2 << "]";

      // Display the UID value (instead of displaying the rough code)  
      if (g == 0x0002) {  // Some more to be displayed ?
         if ( (e == 0x0010) || (e == 0x0002) ) 	   
            s << "  ==>\t[" << ts->GetValue(v) << "]";   
      } else {
         if (g == 0x0008) {
            if ( (e == 0x0016) || (e == 0x1150)  ) 	   
               s << "  ==>\t[" << ts->GetValue(v) << "]"; 
         }
      }              
      s << std::endl;
   }

   // List element
   guint32 lgth;
   char greltag[10];  //group element tag
 
   s << "------------ using listEntries ----------------" << std::endl; 

   char st[11];
   for (ListTag::iterator i = listEntries.begin();  
	   i != listEntries.end();
	   ++i){
      g = (*i)->GetGroup();
      e = (*i)->GetElement();
      v = (*i)->GetValue();
      o = (*i)->GetOffset();
      sprintf(greltag,"%04x|%04x",g,e);           
      d2 = _CreateCleanString(v);  // replace non printable characters by '.'
      s << greltag << " lg : ";
      lgth = (*i)->GetReadLength(); 
      sprintf(st,"x(%x)",lgth);
      s.setf(std::ios::left);
      s << std::setw(10-strlen(st)) << " ";  
      s << st << " ";
      s.setf(std::ios::left);
      s << std::setw(8) << lgth; 
      s << " Off.: ";
      sprintf(st,"x(%x)",o); 
      s << std::setw(10-strlen(st)) << " ";       
      s << st << " ";
      s << std::setw(8) << o; 
      s << "[" << (*i)->GetVR()  << "] ";
      s.setf(std::ios::left);
      s << std::setw(66-(*i)->GetName().length()) << " ";
      s << "[" << (*i)->GetName()<< "] ";       
      s << "[" << d2 << "]";
       // Display the UID value (instead of displaying the rough code)  
      if (g == 0x0002) {  // Any more to be displayed ?
         if ( (e == 0x0010) || (e == 0x0002) ) 	   
            s << "  ==>\t[" << ts->GetValue(v) << "]";   
      } else {
         if (g == 0x0008) {
            if ( (e == 0x0016) || (e == 0x1150)  ) 	   
               s << "  ==>\t[" << ts->GetValue(v) << "]"; 
         }
      }                        
      s << std::endl;
   } 

   os<<s.str();
} 

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief  add a new Dicom Element pointer to 
 *         the H Table and to the chained List
 * @param   newHeaderEntry
 */
void gdcmHeaderEntrySet::Add(gdcmHeaderEntry * newHeaderEntry) {

//	tagHT [newHeaderEntry->GetKey()]  = newHeaderEntry;

   tagHT.insert( PairHT( newHeaderEntry->GetKey(),newHeaderEntry) );
	
// WARNING : push_bash in listEntries ONLY during ParseHeader
// TODO : something to allow further Elements addition 
// position to be taken care of !	
   listEntries.push_back(newHeaderEntry); 
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief  retrieves a Dicom Element (the first one) using (group, element)
 * \ warning (group, element) IS NOT an identifier inside the Dicom Header
 *           if you think it's NOT UNIQUE, check the count number
 *           and use iterators to retrieve ALL the Dicoms Elements within
 *           a given couple (group, element)
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
gdcmHeaderEntry* gdcmHeaderEntrySet::GetHeaderEntryByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return NULL;
   return tagHT.find(key)->second;
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief  Gets the value (string) of the target Dicom Element
 * @param   group Group  number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
std::string gdcmHeaderEntrySet::GetEntryByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return GDCM_UNFOUND;
   return tagHT.find(key)->second->GetValue();
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief  Sets the value (string) of the target Dicom Element
 * @param   content string value of the Dicom Element
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
bool gdcmHeaderEntrySet::SetEntryByNumber(std::string content,
                                          guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   int l = content.length();
   if(l%2) {  // Odd length are padded with a space (020H).
      l++;
      content = content + '\0';
   }
      
   //tagHT[key]->SetValue(content);   
   gdcmHeaderEntry * a;
   IterHT p;
   TagHeaderEntryHT::iterator p2;
   // DO NOT remove the following lines : they explain the stuff   
   //p= tagHT.equal_range(key); // get a pair of iterators first-last synonym
   //p2=p.first;                // iterator on the first synonym 
   //a=p2->second;              // H Table target column (2-nd col)
    
   // or, easier :
   a = ((tagHT.equal_range(key)).first)->second; 
       
   a-> SetValue(content); 
   
   //std::string vr = tagHT[key]->GetVR();
   std::string vr = a->GetVR();
   
   guint32 lgr;
   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;
   else
      lgr = l;	   
   //tagHT[key]->SetLength(lgr);
   a->SetLength(lgr);   
   return true;
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   Sets the value length of the Dicom Element
 * \warning : use with caution !
 * @param   length
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  boolean
 */
bool gdcmHeaderEntrySet::SetEntryLengthByNumber(guint32 length,
                                                guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   if (length%2) length++; // length must be even
   //tagHT[key]->SetLength(length);
   ( ((tagHT.equal_range(key)).first)->second )->SetLength(length);	 
	 
   return true ;		
}
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   Sets a 'non string' value to a given Dicom Element
 * @param   area
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
bool gdcmHeaderEntrySet::SetVoidAreaByNumber(void * area,
                                      guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   //tagHT[key]->SetVoidArea(area);
   ( ((tagHT.equal_range(key)).first)->second )->SetVoidArea(area);	 
   return true ;		
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   Generate a free TagKey i.e. a TagKey that is not present
 *          in the TagHt dictionary.
 * @param   group The generated tag must belong to this group.  
 * @return  The element of tag with given group which is fee.
 */
guint32 gdcmHeaderEntrySet::GenerateFreeTagKeyInGroup(guint16 group) {
   for (guint32 elem = 0; elem < UINT32_MAX; elem++) {
      TagKey key = gdcmDictEntry::TranslateToKey(group, elem);
      if (tagHT.count(key) == 0)
         return elem;
   }
   return UINT32_MAX;
}

/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   Checks if a given Dicom Element exists
 * \        within the H table
 * @param   group Group   number of the searched Dicom Element 
 * @param   element  Element number of the searched Dicom Element 
 * @return  number of occurences
 */
int gdcmHeaderEntrySet::CheckIfExistByNumber(guint16 group, guint16 element ) {
	std::string key = gdcmDictEntry::TranslateToKey(group, element );
	return (tagHT.count(key));
}

// ==============
// TODO to be re-written using the chained list instead of the H table
//      so we can remove the GroupHT from the gdcmHeader
// =============
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   
 * @param   _fp already open file pointer
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @return  always "True" ?!
 */
bool gdcmHeaderEntrySet::Write(FILE * _fp, FileType type) {

   	// Question :
	// Comment pourrait-on savoir si le DcmHeader vient d'un fichier DicomV3 ou non
	// (FileType est un champ de gdcmHeader ...)
	// WARNING : Si on veut ecrire du DICOM V3 a partir d'un DcmHeader ACR-NEMA
	// no way 
        // a moins de se livrer a un tres complique ajout des champs manquants.
        // faire un CheckAndCorrectHeader (?)

   if ( (type == ImplicitVR) || (type == ExplicitVR) )
      UpdateGroupLength(false,type);
   if ( type == ACR)
      UpdateGroupLength(true,ACR);

   WriteEntries(type, _fp);
   return(true);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmHeaderEntrySet
 * \brief   Re-computes the length of a ACR-NEMA/Dicom group from a DcmHeader
 * \warning : to be re-written using the chained list instead of the H table.
 * \todo : to be re-written using the chained list instead of the H table
 * @param   SkipSequence TRUE if we don't want to write Sequences (ACR-NEMA Files)
 * @param   type Type of the File (ExplicitVR,ImplicitVR, ACR, ...) 
 */
void gdcmHeaderEntrySet::UpdateGroupLength(bool SkipSequence, FileType type) {
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
 * \ingroup gdcmHeaderEntrySet
 * \brief   writes on disc according to the requested format
 * \        (ACR-NEMA, ExplicitVR, ImplicitVR) the image
 * \ warning does NOT add the missing elements in the header :
 * \         it's up to the user doing it !
 * \         (function CheckHeaderCoherence to be written)
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @param   _fp already open file pointer
 * @return  
 */
void gdcmHeaderEntrySet::WriteEntries(FileType type, FILE * _fp) {
   guint16 gr, el;
   guint32 lgr;
   const char * val;
   std::string vr;
   guint32 val_uint32;
   guint16 val_uint16;
   
   std::vector<std::string> tokens;
   
   // TODO : use listEntries to iterate, not TagHt!
   //        pb : gdcmHeaderEntrySet.Add does NOT update listEntries
   //        find a trick in STL to do it, at low cost !

   void *ptr;

   // Tout ceci ne marche QUE parce qu'on est sur un proc Little Endian 
   // restent a tester les echecs en ecriture (apres chaque fwrite)

   for (TagHeaderEntryHT::iterator tag2=tagHT.begin();
        tag2 != tagHT.end();
        ++tag2){

      gr =  tag2->second->GetGroup();
      el =  tag2->second->GetElement();
      lgr = tag2->second->GetLength();
      val = tag2->second->GetValue().c_str();
      vr =  tag2->second->GetVR();
      
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
         Tokenize (tag2->second->GetValue(), tokens, "\\");
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
         Tokenize (tag2->second->GetValue(), tokens, "\\");
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
