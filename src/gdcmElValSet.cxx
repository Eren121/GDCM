// gdcmElValSet.cxx

#include <sstream>
#include "gdcmUtil.h"
#include "gdcmElValSet.h"

gdcmElValSet::~gdcmElValSet() {
   for (TagElValueHT::iterator tag = tagHt.begin(); tag != tagHt.end(); ++tag) {
      gdcmElValue* EntryToDelete = tag->second;
      if ( EntryToDelete )
         delete EntryToDelete;
   }
   tagHt.clear();
   // Since Add() adds symetrical in both tagHt and NameHt we can
   // assume all the pointed gdcmElValues are allready cleaned-up when
   // we cleaned tagHt.
   NameHt.clear();
}

TagElValueHT & gdcmElValSet::GetTagHt(void) {
	return tagHt;
}

void gdcmElValSet::Add(gdcmElValue * newElValue) {
	tagHt [newElValue->GetKey()]  = newElValue;
	NameHt[newElValue->GetName()] = newElValue;
}

void gdcmElValSet::Print(ostream & os) {
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){
		os << tag->first << ": ";
		os << "[" << tag->second->GetValue() << "]";
		os << "[" << tag->second->GetName()  << "]";
		os << "[" << tag->second->GetVR()    << "]"; 
		os << " lgr : " << tag->second->GetLength();
		os << endl;
	}
} 

void gdcmElValSet::PrintByName(ostream & os) {
	for (TagElValueNameHT::iterator tag = NameHt.begin();
		  tag != NameHt.end();
		  ++tag){
		os << tag->first << ": ";
		os << "[" << tag->second->GetValue() << "]";
		os << "[" << tag->second->GetKey()   << "]";
		os << "[" << tag->second->GetVR()    << "]" << endl;
	}
}

gdcmElValue* gdcmElValSet::GetElementByNumber(guint16 group, guint16 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return (gdcmElValue*)0;
	return tagHt.find(key)->second;
}

gdcmElValue* gdcmElValSet::GetElementByName(string TagName) {
   if ( ! NameHt.count(TagName))
      return (gdcmElValue*)0;
   return NameHt.find(TagName)->second;
}

string gdcmElValSet::GetElValueByNumber(guint16 group, guint16 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return "gdcm::Unfound";
	return tagHt.find(key)->second->GetValue();
}

string gdcmElValSet::GetElValueByName(string TagName) {
	if ( ! NameHt.count(TagName))
		return "gdcm::Unfound";
	return NameHt.find(TagName)->second->GetValue();
}


int gdcmElValSet::SetElValueByNumber(string content,
                                     guint16 group, guint16 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	tagHt[key]->SetValue(content);
	tagHt[key]->SetLength(content.length());	 
	return 1;
}

int gdcmElValSet::SetElValueByName(string content, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	NameHt[TagName]->SetValue(content);
	NameHt[TagName]->SetLength(content.length());
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

int gdcmElValSet::SetElValueLengthByNumber(guint32 length,
                                           guint16 group, guint16 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	tagHt[key]->SetLength(length);	 
	return 1 ;		
}


int gdcmElValSet::SetElValueLengthByName(guint32 length, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	NameHt.find(TagName)->second->SetLength(length);	 
	return 1 ;		
}



void gdcmElValSet::UpdateGroupLength(bool SkipSequence, FileType type) {
   guint16 gr, el;
   string vr;
   
   gdcmElValue *elem;
   char trash[10];
   string str_trash;
   GroupKey key;
   GroupHT groupHt;
   TagKey tk;
   gdcmElValue *elemZ;
   
   for (TagElValueHT::iterator tag2 = tagHt.begin();
        tag2 != tagHt.end();
        ++tag2){

      elem  = tag2->second;
      gr = elem->GetGroup();
      el = elem->GetElement();
      vr = elem->GetVR(); 
                 
      sprintf(trash, "%04x", gr);
      key = trash;
                  
      if (SkipSequence && vr == "SQ") continue;
         // pas SEQUENCE en ACR-NEMA
         // WARNING : pb CERTAIN
         //           si on est descendu 'a l'interieur' des SQ 
         //
         // --> la descente a l'interieur' des SQ 
         // devra etre faite avec une liste chainee, pas avec une HTable...
             
      if ( groupHt.count(key) == 0) { 
         if (el ==0x0000) {
            groupHt[key] = 0;
         } else {
            groupHt[key] =2 + 2 + 4 + elem->GetLength();
         } 
      } else {       
         if (type = ExplicitVR) {
            if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {
               groupHt[key] +=  4;
            }
         }
         groupHt[key] += 2 + 2 + 4 + elem->GetLength(); 
      } 
   }
  
     if(0)
     for (GroupHT::iterator g = groupHt.begin();
        g != groupHt.end();
        ++g){        
        printf("groupKey %s : %d\n",g->first.c_str(),g->second);
     }
       
  
   unsigned short int gr_bid;
  
   for (GroupHT::iterator g = groupHt.begin();
        g != groupHt.end();
        ++g){ 
  
      sscanf(g->first.c_str(),"%x",&gr_bid);
      tk = g->first + "|0000";
                     
      if ( tagHt.count(tk) == 0) { 
         gdcmDictEntry * tagZ = new gdcmDictEntry(gr_bid, 0x0000, "UL");       
         elemZ = new gdcmElValue(tagZ);
         elemZ->SetLength(4);
         Add(elemZ);
      } else {
         elemZ=GetElementByNumber(gr_bid, 0x0000);
      }     
      sprintf(trash ,"%d",g->second);
      str_trash=trash;
      elemZ->SetValue(str_trash);
   }   
}

void gdcmElValSet::WriteElements(FileType type, FILE * _fp) {
   guint16 gr, el;
   guint32 lgr;
   const char * val;
   string vr;
   guint32 val_uint32;
   guint16 val_uint16;
   
   vector<string> tokens;

   void *ptr;

   // Tout ceci ne marche QUE parce qu'on est sur un proc Little Endian 
   // restent à tester les echecs en écriture (apres chaque fwrite)

   for (TagElValueHT::iterator tag2=tagHt.begin();
        tag2 != tagHt.end();
        ++tag2){

      gr =  tag2->second->GetGroup();
      el =  tag2->second->GetElement();
      lgr = tag2->second->GetLength();
      val = tag2->second->GetValue().c_str();
      vr =  tag2->second->GetVR();

      if ( type == ACR ) { 
         if (gr < 0x0008) continue;
         if (gr %2)   continue;
         if (vr == "SQ" ) continue;
      } 

      fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp);  //group
      fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp);  //element

      if ( (type == ExplicitVR) && (gr <= 0x0002) ) {
         // On est en EXPLICIT VR
         guint16 z=0, shortLgr;
         fwrite (vr.c_str(),(size_t)2 ,(size_t)1 ,_fp);

         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) {
            fwrite ( &z,  (size_t)2 ,(size_t)1 ,_fp);
            fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);

         } else {
            shortLgr=lgr;
            fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,_fp);
         }
      } else {
         fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);
      }

      tokens.erase(tokens.begin(),tokens.end());
      Tokenize (tag2->second->GetValue(), tokens, "\\");

      if (vr == "US" || vr == "SS") {
         for (unsigned int i=0; i<tokens.size();i++) {
            val_uint16 = atoi(tokens[i].c_str());
            ptr = &val_uint16;
            fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);
         }
         continue;
      }
      if (vr == "UL" || vr == "SL") {
         for (unsigned int i=0; i<tokens.size();i++) {
            val_uint32 = atoi(tokens[i].c_str());
            ptr = &val_uint32;
            fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);
         }
         continue;
      }
      tokens.clear();

      // Les pixels ne sont pas chargés dans l'element !
      if ((gr == 0x7fe0) && (el == 0x0010) ) break;

      fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
   }
}

int gdcmElValSet::Write(FILE * _fp, FileType type) {

   if (type == ImplicitVR) {
      string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
      SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);
      
      //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
      //      values with a VR of UI shall be padded with a single trailing null
      //      Dans le cas suivant on doit pader manuellement avec un 0
      
      SetElValueLengthByNumber(18, 0x0002, 0x0010);
   }
   
   	// Question :
	// Comment pourrait-on savoir si le DcmHeader vient d'un fichier DicomV3 ou non ,
	// (FileType est un champ de gdcmHeader ...)
	// WARNING : Si on veut ecrire du DICOM V3 a partir d'un DcmHeader ACR-NEMA
	// no way
	
	
   if (type == ExplicitVR) {
      string explicitVRTransfertSyntax = "1.2.840.10008.1.2.1";
      SetElValueByNumber(explicitVRTransfertSyntax, 0x0002, 0x0010);
      // See above comment 
      SetElValueLengthByNumber(20, 0x0002, 0x0010);
   }

   if ( (type == ImplicitVR) || (type == ExplicitVR) )
      UpdateGroupLength(false,type);
   if ( type == ACR)
      UpdateGroupLength(true,ACR);

   WriteElements(type, _fp);

   return(1);
}
