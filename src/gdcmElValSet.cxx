// gdcmElValSet.cxx

#include <sstream>
#include "gdcmUtil.h"
#include "gdcmElValSet.h"

#define DEBUG 0


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

void gdcmElValSet::UpdateGroupLength(bool SkipSequence) {
	// On parcourt la table pour recalculer la longueur des 'elements 0x0000'
	// au cas ou un tag ai été ajouté par rapport à ce qui a été lu
	// dans l'image native
	//
	// cf : code IdDcmWriteFile dans libido/src/dcmwrite.c
				
	// On fait de l'implicit VR little Endian 
	// (pour moins se fairche sur processeur INTEL)
	// On force le TRANSFERT SYNTAX UID
   guint16 gr, el;
   string vr;
   guint32 lgrCalcGroupe=0;
   gdcmElValue *elem, *elemZ, *elemZPrec;
   guint16 grCourant = 0;
				
	TagElValueHT::iterator tag = tagHt.begin();
	
	elem = tag->second;
	gr   = elem->GetGroup();
	el   = elem->GetElement();
			
	if (el != 0x0000) {
		if(DEBUG)printf("ajout elem OOOO premiere fois\n");
		gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
		elemZPrec = new gdcmElValue(tagZ);	// on le cree
		elemZPrec->SetLength(4);
		Add(elemZPrec);				// On l'accroche à sa place
	} else {
		elemZPrec = elem;
		if(DEBUG)printf("Pas d'ajout elem OOOO premiere fois\n");
	}
	lgrCalcGroupe = 0;
	if(DEBUG)printf("init-1 lgr (%d) pour gr %04x\n",lgrCalcGroupe, gr);
	grCourant = gr;
	
	for (tag = ++tagHt.begin();
		  tag != tagHt.end();
		  ++tag){
		  
		elem = tag->second;
		gr = elem->GetGroup();
		el = elem->GetElement();
      vr = elem->GetVR();

      if (SkipSequence && vr == "SQ")
         // pas SEQUENCE en ACR-NEMA
         // WARNING : risque de pb 
         //           si on est descendu 'a l'interieur' des SQ 
         continue;
            //
            //
		if ( (gr != grCourant) /*&&	// On arrive sur un nv Groupe	  
		     (el != 0xfffe) */	) {
			    
			if (el != 0x0000) {
			 	gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
				elemZ = new gdcmElValue(tagZ); // on le cree
				elemZ->SetLength(4);
				Add(elemZ);	 		// On l'accroche à sa place 
				if(DEBUG)printf("ajout elem OOOO pour gr %04x\n",gr);
			} else { 
				elemZ=elem;
				if(DEBUG)printf("maj elemZ\n");
			}
			
			ostringstream f;
			f << lgrCalcGroupe; 
			//sprintf(str_lgrCalcGroupe,"%d",lgrCalcGroupe);
			elemZPrec->SetValue(f.str());
			if(DEBUG)printf("ecriture lgr (%d, %s) pour gr %04x\n",lgrCalcGroupe, f.str().c_str(), grCourant);
			if(DEBUG)printf ("%04x %04x [%s]\n",elemZPrec->GetGroup(), elemZPrec->GetElement(),
							elemZPrec->GetValue().c_str());
			if(DEBUG)cout << "Addresse elemZPrec " << elemZPrec<< endl;
			elemZPrec=elemZ;
			lgrCalcGroupe = 0;
			grCourant     = gr;	
			if(DEBUG)printf("init-2 lgr (%d) pour gr %04x\n",lgrCalcGroupe, gr);			
		} else {			// On n'EST PAS sur un nv Groupe
			lgrCalcGroupe += 2 + 2 + 4 + elem->GetLength();  // Gr + Num + Lgr + LgrElem
			if(DEBUG)printf("increment (%d) el %04x-->lgr (%d) pour gr %04x\n",
								elem->GetLength(), el, lgrCalcGroupe, gr);
		}		
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

   for (TagElValueHT::iterator tag2 = tagHt.begin();
        tag2 != tagHt.end();
        ++tag2){

      gr =  tag2->second->GetGroup();
      el =  tag2->second->GetElement();
      lgr = tag2->second->GetLength();
      val = tag2->second->GetValue().c_str();
      vr =  tag2->second->GetVR();
      if(DEBUG)printf ("%04x %04x [%s] : [%s]\n",gr, el, vr.c_str(), val);

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

      // Les pixels ne sont pas chargés dans l'element !
      if ((gr == 0x7fe0) && (el == 0x0010) ) break;

      fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
   }
}

// Sorry for the DEBUG's, but tomorow is gonna be hoter than today

int gdcmElValSet::Write(FILE * _fp) {

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_uint32;
	guint16 val_uint16;
	
	vector<string> tokens;
	
	void *ptr;
	
	string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
	SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
   //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
   //      values with a VR of UI shall be padded with a single trailing null
   //      Dans le cas suivant on doit pader manuellement avec un 0.
	SetElValueLengthByNumber(18, 0x0002, 0x0010); 
	
	// Question :
	// Comment pourrait-on tester si on est TrueDicom ou non ,
	// (FileType est un champ de gdcmHeader ...)
	//
   UpdateGroupLength();

	// restent à tester les echecs en écriture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag2 = tagHt.begin();
		  tag2 != tagHt.end();
		  ++tag2){
		
		gr =  tag2->second->GetGroup();
		el =  tag2->second->GetElement();
		lgr = tag2->second->GetLength();
		val = tag2->second->GetValue().c_str();
		vr =  tag2->second->GetVR();
		if(DEBUG)printf ("%04x %04x [%s] : [%s]\n",gr, el, vr.c_str(), val);
			
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
		
		// si on n'est pas en IMPLICIT VR voir pb (lgr  + VR)
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 	//lgr
		
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
		
		// Les pixels ne sont pas chargés dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}


int gdcmElValSet::WriteAcr(FILE * _fp) {
//
// ATTENTION : fusioner le code avec celui de gdcmElValSet::Write
//

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_uint32;
	guint16 val_uint16;
	
	vector<string> tokens;
	
	void *ptr;
	
	//string implicitVRTransfertSyntax = "1.2.840.10008.1.2"; // supprime par rapport à Write
   //CLEANME Utilisées pour le calcul Group Length
   //CLEANMEguint32 lgrCalcGroupe=0;
   //CLEANMEgdcmElValue *elem, *elemZ, *elemZPrec;
   //CLEANMEguint16 grCourant = 0;
  
   // Question :
   // Comment pourrait-on tester si on est TrueDicom ou non ,
   // (FileType est un champ de gdcmHeader ...)
   //
   UpdateGroupLength(true);

	// Si on fait de l'implicit VR little Endian 
	// (pour moins se fairche sur processeur INTEL)
	// penser a forcer le TRANSFERT SYNTAX UID
	
	// supprime par rapport à Write		
	//SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
	//SetElValueLengthByNumber(18, 0x0002, 0x0010);  // Le 0 de fin de chaine doit etre stocké, dans ce cas	
		
	// restent à tester les echecs en écriture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag2 = tagHt.begin();
		  tag2 != tagHt.end();
		  ++tag2){

		gr =  tag2->second->GetGroup();
						// saut des groupes speciaux DICOM V3
		if (gr < 0x0008) continue; 	// ajouté par rapport à Write
						// saut des groupes impairs
		if (gr %2) 	 continue; 	// ajouté par rapport à Write
		
		el =  tag2->second->GetElement();
		lgr = tag2->second->GetLength();
		val = tag2->second->GetValue().c_str();
		vr =  tag2->second->GetVR();
			
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
				
		// si on n'est pas en IMPLICIT VR voir pb (lgr  + VR)
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 		//lgr
		
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
		
		// Les pixels ne sont pas chargés dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}

// Sorry for the DEBUG's, but tomorow is gonna be hoter than today

int gdcmElValSet::WriteExplVR(FILE * _fp) {

//
// ATTENTION : fusioner le code avec celui de gdcmElValSet::Write
//

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_uint32;
	guint16 val_uint16;
	guint16 z=0, shortLgr;
	
	vector<string> tokens;
	
	void *ptr;

	string explicitVRTransfertSyntax = "1.2.840.10008.1.2.1";
	
	
	// Question :
	// Comment pourrait-on tester si on est TrueDicom ou non ,
	// (FileType est un champ de gdcmHeader ...)
	//

	// On fait de l'Explicit VR little Endian 
	
				
	SetElValueByNumber(explicitVRTransfertSyntax, 0x0002, 0x0010);	
	SetElValueLengthByNumber(20, 0x0002, 0x0010);  // Le 0 de fin de chaine doit etre stocké, dans ce cas  // ???	
			
   // Question :
   // Comment pourrait-on tester si on est TrueDicom ou non ,
   // (FileType est un champ de gdcmHeader ...)
   //
   UpdateGroupLength();

		
	// restent à tester les echecs en écriture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag2 = tagHt.begin();
		  tag2 != tagHt.end();
		  ++tag2){
		
		gr =  tag2->second->GetGroup();
		el =  tag2->second->GetElement();
		lgr = tag2->second->GetLength();
		val = tag2->second->GetValue().c_str();
		vr =  tag2->second->GetVR();
		if(DEBUG)printf ("%04x %04x [%s] : [%s]\n",gr, el, vr.c_str(), val);
			
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
				
		// On est en EXPLICIT VR
		if (gr == 0x0002) {
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
		
		// Les pixels ne sont pas chargés dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}



