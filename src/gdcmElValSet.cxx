// gdcmElValSet.cxx

#include "gdcmUtil.h"
#include "gdcmElValSet.h"
#include "gdcmTS.h"
#include <sstream>

gdcmElValSet::~gdcmElValSet() {
   for (TagElValueHT::iterator tag = tagHt.begin(); tag != tagHt.end(); ++tag) {
      gdcmElValue* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   tagHt.clear();
   // Since Add() adds symetrical in both tagHt and NameHt we can
   // assume all the pointed gdcmElValues are already cleaned-up when
   // we cleaned tagHt.
   NameHt.clear();
}

TagElValueHT & gdcmElValSet::GetTagHt(void) {
	return tagHt;
}


ListTag & gdcmElValSet::GetListElem(void) {
	return listElem;
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   newElValue
 * @return  
 */
void gdcmElValSet::Add(gdcmElValue * newElValue) {
	tagHt [newElValue->GetKey()]  = newElValue;
	NameHt[newElValue->GetName()] = newElValue;
	
// WARNING : push_bash in listElem ONLY during ParseHeader
// TODO : something to allow further Elements addition 
// position to be taken care of !	
	listElem.push_back(newElValue); 
}

/**
 * \ingroup gdcmElValSet
 * \brief   Checks if a given Dicom element exists
 * \        within a ElValSet
 * @param   Group
 * @param   Elem
 * @return  
 */
int gdcmElValSet::CheckIfExistByNumber(guint16 Group, guint16 Elem ) {
	std::string key = TranslateToKey(Group, Elem );
	return (tagHt.count(key));
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 */
void gdcmElValSet::Print(std::ostream & os) {

   size_t o;
   short int g, e;
   TSKey v;
   std::string d2;
   gdcmTS * ts = gdcmGlobal::GetTS();
   
   std::cout << "------------- using tagHt ----------------------------" << std::endl;
   
   for (TagElValueHT::iterator tag = tagHt.begin();
	   tag != tagHt.end();
	   ++tag){
      g = tag->second->GetGroup();
      e = tag->second->GetElement();
      v = tag->second->GetValue();
      o = tag->second->GetOffset();
      d2 = _CreateCleanString(v);  // replace non printable characters by '.'
		 
      os << tag->first << ": ";
      os << " lgr : " << tag->second->GetLength();
      os << ", Offset : " << o;
      os << " x(" << std::hex << o << std::dec << ") ";
      os << "\t[" << tag->second->GetVR()    << "]";
      os << "\t[" << tag->second->GetName()  << "]";       
      os << "\t[" << d2 << "]";
      
      // Display the UID value (instead of displaying the rough code)  
      if (g == 0x0002) {  // Some more to be displayed ?
         if ( (e == 0x0010) || (e == 0x0002) ) 	   
            os << "  ==>\t[" << ts->GetValue(v) << "]";   
      } else {
         if (g == 0x0008) {
            if ( (e == 0x0016) || (e == 0x1150)  ) 	   
               os << "  ==>\t[" << ts->GetValue(v) << "]"; 
         }
      }              
      os << std::endl;
   }
   
   std::cout << "------------ using listElem -----------------" << std::endl;
   
   //for (ListTag::iterator i = listElem.begin();
   
  char greltag[9];  //group element tag
   
  for (std::list<gdcmElValue*>::iterator i = listElem.begin();  
	   i != listElem.end();
	   ++i){
      sprintf(greltag,"%04x|%04x",(*i)->GetGroup(),(*i)->GetElement());
      g = (*i)->GetGroup();
      e = (*i)->GetElement();
      v = (*i)->GetValue();
      o = (*i)->GetOffset();
      d2 = _CreateCleanString(v);  // replace non printable characters by '.'
      //os << std::hex <<g << "|" << e << std::dec << ": ";
      os << greltag << " : ";
      os << " lgr : " << (*i)->GetReadLength();
      os << ", Offset : " << o;
      os << " x(" << std::hex << o << std::dec << ") ";
      os << "\t[" << (*i)->GetVR()    << "]";
      os << "\t[" << (*i)->GetName()  << "]";       
      os << "\t[" << d2 << "]";
      
      // Display the UID value (instead of displaying the rough code)  
      if (g == 0x0002) {  // Any more to be displayed ?
         if ( (e == 0x0010) || (e == 0x0002) ) 	   
            os << "  ==>\t[" << ts->GetValue(v) << "]";   
      } else {
         if (g == 0x0008) {
            if ( (e == 0x0016) || (e == 0x1150)  ) 	   
               os << "  ==>\t[" << ts->GetValue(v) << "]"; 
         }
      }              
      os << std::endl;
   }	   
} 

/**
 * \ingroup gdcmElValSet
 * \brief   
 */
void gdcmElValSet::PrintByName(std::ostream & os) {
   for (TagElValueNameHT::iterator tag = NameHt.begin();
          tag != NameHt.end();
          ++tag){
      os << tag->first << ": ";
      os << "[" << tag->second->GetValue() << "]";
      os << "[" << tag->second->GetKey()   << "]";
      os << "[" << tag->second->GetVR()    << "]" << std::endl;
   }
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   group 
 * @param   element 
 * @return  
 */
gdcmElValue* gdcmElValSet::GetElementByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHt.count(key))
      return (gdcmElValue*)0;
   return tagHt.find(key)->second;
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @return  
 */
gdcmElValue* gdcmElValSet::GetElementByName(std::string TagName) {
   if ( ! NameHt.count(TagName))
      return (gdcmElValue*)0;
   return NameHt.find(TagName)->second;
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   group 
 * @param   element 
 * @return  
 */
std::string gdcmElValSet::GetElValueByNumber(guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHt.count(key))
      return GDCM_UNFOUND;
   return tagHt.find(key)->second->GetValue();
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @return  
 */
std::string gdcmElValSet::GetElValueByName(std::string TagName) {
   if ( ! NameHt.count(TagName))
      return GDCM_UNFOUND;
   return NameHt.find(TagName)->second->GetValue();
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   content
 * @param   group 
 * @param   element 
 * @return  
 */
int gdcmElValSet::SetElValueByNumber(std::string content,
                                     guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHt.count(key))
      return 0;
   int l = content.length();
   if(l%2) {  // Odd length are padded with a space (020H).
      l++;
      content = content + '\0';
   }
   tagHt[key]->SetValue(content);

   std::string vr = tagHt[key]->GetVR();
   guint32 lgr;

   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;
   else
      lgr = l;	   
   tagHt[key]->SetLength(lgr); 
   return 1;
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   content
 * @param   TagName
 * @return  
 */
int gdcmElValSet::SetElValueByName(std::string content, std::string TagName) {
   if ( ! NameHt.count(TagName))
      return 0;
   int l = content.length();
   if(l%2) {  // Odd length are padded with a space (020H).
      l++;
      // Well. I know that '/0' is NOT a space
      // but it doesn't work with a space. 
      // Use hexedit and see 0002|0010 value (Transfer Syntax UID)
      content = content + '\0';
   }
   NameHt[TagName]->SetValue(content);

   std::string vr = NameHt[TagName]->GetVR();
   guint32 lgr;

   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;	   
   else 
      lgr = content.length();
	   
// TODO : WARNING: le cas de l'element des pixels (7fe0,0010) n'est pas traite
// par SetElValueByName
// il faudra utiliser SetElValueByNumber
	   
   NameHt[TagName]->SetLength(lgr);
   return 1;		
}

/**
 * \ingroup gdcmElValSet
 * \brief   Generate a free TagKey i.e. a TagKey that is not present
 *          in the TagHt dictionary.
 * @param   group The generated tag must belong to this group.  
 * @return  The element of tag with given group which is fee.
 */
guint32 gdcmElValSet::GenerateFreeTagKeyInGroup(guint16 group) {
   for (guint32 elem = 0; elem < UINT32_MAX; elem++) {
      TagKey key = gdcmDictEntry::TranslateToKey(group, elem);
      if (tagHt.count(key) == 0)
         return elem;
   }
   return UINT32_MAX;
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   area
 * @param   group 
 * @param   element 
 * @return  
 */
int gdcmElValSet::SetVoidAreaByNumber(void * area,
                                      guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHt.count(key))
      return 0;
   tagHt[key]->SetVoidArea(area);	 
   return 1 ;		
}

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   length
 * @param   group 
 * @param   element 
 * @return  int acts as a boolean
 */
int gdcmElValSet::SetElValueLengthByNumber(guint32 length,
                                           guint16 group, guint16 element) {
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHt.count(key))
      return 0;
   if (length%2) length++; // length must be even
   tagHt[key]->SetLength(length);	 
   return 1 ;		
}
/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   length
 * @param   TagName
 * @return  
 */
int gdcmElValSet::SetElValueLengthByName(guint32 length, std::string TagName) {
   if ( ! NameHt.count(TagName))
      return 0;
   if (length%2) length++; // length must be even
   NameHt.find(TagName)->second->SetLength(length);	 
   return 1 ;		
}

/**
 * \ingroup gdcmElValSet
 * \brief   Re-computes the length of a ACR-NEMA/Dicom group from a DcmHeader
 * @param   SkipSequence TRUE if we don't want to write Sequences (ACR-NEMA Files)
 * @param   type
 */
void gdcmElValSet::UpdateGroupLength(bool SkipSequence, FileType type) {
   guint16 gr, el;
   std::string vr;
   
   gdcmElValue *elem;
   char trash[10];
   std::string str_trash;
   
   GroupKey key;
   GroupHT groupHt;  // to hold the length of each group
   TagKey tk;
   // remember :
   // typedef std::map<GroupKey, int> GroupHT;
   
   gdcmElValue *elemZ;
  
   // for each Tag in the DCM Header
   
   for (TagElValueHT::iterator tag2 = tagHt.begin(); 
        tag2 != tagHt.end();
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
                     
      if ( tagHt.count(tk) == 0) { 		// if element 0x0000 not found
         gdcmDictEntry * tagZ = new gdcmDictEntry(gr_bid, 0x0000, "UL");       
         elemZ = new gdcmElValue(tagZ);
         elemZ->SetLength(4);
         Add(elemZ);				// create it
      } else {
         elemZ=GetElementByNumber(gr_bid, 0x0000);
      }     
      sprintf(trash ,"%d",g->second);
      str_trash=trash;
      elemZ->SetValue(str_trash);
   }   
}

/**
 * \ingroup gdcmElValSet
 * \brief   writes on disc according to the requested format
 * \        (ACR-NEMA, DICOM, RAW) the image
 * \ warning does NOT add the missing elements in the header :
 * \         it's up to the user doing it !
 * \         (function CheckHeaderCoherence to be written)
 * @param   type type of the File to be written 
 *          (ACR-NEMA, DICOM, RAW)
 * @param   _fp already open file pointer
 * @return  
 */
void gdcmElValSet::WriteElements(FileType type, FILE * _fp) {
   guint16 gr, el;
   guint32 lgr;
   const char * val;
   std::string vr;
   guint32 val_uint32;
   guint16 val_uint16;
   
   std::vector<std::string> tokens;
   
   // TODO : use listElem to iterate, not TagHt!
   //        pb : gdcmElValSet.Add does NOT update listElem
   //        find a trick in STL to do it, at low cost !

   void *ptr;

   // Tout ceci ne marche QUE parce qu'on est sur un proc Little Endian 
   // restent a tester les echecs en ecriture (apres chaque fwrite)

   for (TagElValueHT::iterator tag2=tagHt.begin();
        tag2 != tagHt.end();
        ++tag2){

      gr =  tag2->second->GetGroup();
      el =  tag2->second->GetElement();
      lgr = tag2->second->GetLength();
      val = tag2->second->GetValue().c_str();
      vr =  tag2->second->GetVR();
      
     // std::cout << "Tag "<< std::hex << gr << " " << el << std::endl;

      if ( type == ACR ) { 
         if (gr < 0x0008)   continue; // ignore pure DICOM V3 groups
         if (gr %2)         continue; // ignore shadow groups
         if (vr == "SQ" )   continue; // ignore Sequences
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

/**
 * \ingroup gdcmElValSet
 * \brief   
 * @param   _fp
 * @param   type
 * @return  
 */
int gdcmElValSet::Write(FILE * _fp, FileType type) {

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

   WriteElements(type, _fp);
   return(1);
}
