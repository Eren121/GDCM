// gdcmElValSet.cxx

#include <sstream>
#include "gdcmUtil.h"
#include "gdcmElValSet.h"


TagElValueHT & gdcmElValSet::GetTagHt(void) {
	return tagHt;
}

void gdcmElValSet::Add(gdcmElValue * newElValue) {
   cout << "#### gdcmElValSet::Add" << newElValue->GetKey() << newElValue->GetName() << endl;
	tagHt [newElValue->GetKey()]  = newElValue;
	NameHt[newElValue->GetName()] = newElValue;
}

// TODO : faire un gdcmElValSet::ReplaceOrCreate qui remplace si �a existe, qui cree sinon

void gdcmElValSet::ReplaceOrCreate(gdcmElValue * newElValue) {

	TagKey key = newElValue->GetKey();

	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "gdcmElValSet::GetElValueByNumber",
		            "multiple entries for this key (FIXME) !");
		            
	if (tagHt.count(key)) {
		tagHt.erase(key);
		tagHt.erase(newElValue->GetName());
	}

	tagHt [key]  		      = newElValue;
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

gdcmElValue* gdcmElValSet::GetElementByNumber(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return (gdcmElValue*)0;
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "gdcmElValSet::GetElementByNumber",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second;
}

gdcmElValue* gdcmElValSet::GetElementByName(string TagName) {
   if ( ! NameHt.count(TagName))
      return (gdcmElValue*)0;
   if (NameHt.count(TagName) > 1)
      dbg.Verbose(0, "gdcmElValSet::GetElement",
                  "multipe entries for this key (FIXME) !");
   return NameHt.find(TagName)->second;
}

string gdcmElValSet::GetElValueByNumber(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return "gdcm::Unfound";
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "gdcmElValSet::GetElValueByNumber",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second->GetValue();
}

string gdcmElValSet::GetElValueByName(string TagName) {
	if ( ! NameHt.count(TagName))
		return "gdcm::Unfound";
	if (NameHt.count(TagName) > 1)
		dbg.Verbose(0, "gdcmElValSet::GetElValue",
		            "multipe entries for this key (FIXME) !");
	return NameHt.find(TagName)->second->GetValue();
}

int gdcmElValSet::SetElValueByNumber(string content,
                                     guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	if (tagHt.count(key) > 1) {
		dbg.Verbose(0, "gdcmElValSet::SetElValueByNumber",
		            "multiple entries for this key (FIXME) !");
		return (0); 
	}		                       
	tagHt[key]->SetValue(content);

	// Question : m � j LgrElem ?
	tagHt[key]->SetLength(strlen(content.c_str()));	 

	// FIXME should we really update the element length ?
	tagHt[key]->SetLength(content.length());	 

	return(1);		
}

int gdcmElValSet::SetElValueByName(string content, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	if (NameHt.count(TagName) > 1) {
		dbg.Verbose(0, "gdcmElValSet::SetElValueByName",
		            "multipe entries for this key (FIXME) !");
		return 0;
	}
	NameHt.find(TagName)->second->SetValue(content);
	NameHt.find(TagName)->second->SetLength(strlen(content.c_str()));	 
	return(1);		
}

/**
 * \ingroup  gdcmElValSet
 * \brief    Generate a free TagKey i.e. a TagKey that is not present
 *           in the TagHt dictionary. One of the potential usage is
 *           to add  gdcm generated additional informartion to the ElValSet
 *           (see gdcmHeader::AddAndDefaultElements).
 * @param group The generated tag must belong to this group.  
 * @return   The element of tag with given group which is fee.
 */
guint32 gdcmElValSet::GenerateFreeTagKeyInGroup(guint32 group) {
   for (guint32 elem = 0; elem < UINT32_MAX; elem++) {
      TagKey key = gdcmDictEntry::TranslateToKey(group, elem);
      if (tagHt.count(key) == 0)
         return elem;
   }
   return UINT32_MAX;
}

int gdcmElValSet::SetElValueLengthByNumber(guint32 l,
                                           guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	if (tagHt.count(key) > 1) {
		dbg.Verbose(0, "gdcmElValSet::SetElValueLengthByNumber",
		            "multiple entries for this key (FIXME) !");
		return (0); 
	}		                       
	tagHt[key]->SetLength(l);	 
	return(1);		
}


int gdcmElValSet::SetElValueLengthByName(guint32 l, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	if (NameHt.count(TagName) > 1) {
		dbg.Verbose(0, "gdcmElValSet::SetElValueByName",
		            "multipe entries for this key (FIXME) !");
		return 0;
	}
	NameHt.find(TagName)->second->SetLength(l);	 
	return(1);		
}

// Sorry for the DEBUG's, but tomorow is gonna be hoter than today
#define DEBUG 0

int gdcmElValSet::Write(FILE * _fp) {

// ATTENTION : fonction non termin�e (commit�e a titre de precaution)

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_uint32;
	gint32  val_int32;
	guint16 val_uint16;
	gint16  val_int16;;
	
	vector<string> tokens;
	
	void *ptr;
	char str_lgrCalcGroupe[10];

	
	string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
	
	// Utilis�es pour le calcul Group Length
	int deja = 0, prem=0;
	guint32 lgrCalcGroupe=0;
	gdcmElValue *elem, *elemZ, *elemZPrec;
	guint16 grCourant = 0;
	
	// Question :
	// Comment pourrait-on tester si on est TrueDicom ou non ,
	// (FileType est un champ de gdcmHeader ...)
	//

	// On parcourt la table pour recalculer la longueur des 'elements 0x0000'
	// au cas ou un tag ai �t� ajout� par rapport � ce qui a �t� lu
	// dans l'image native
	//
	// cf : code IdDcmWriteFile dans libido/src/dcmwrite.c
		
if (1) {  // Risque de pb dans le calcul des lgr de chaque groupe. On le saute pour le moment!
		
	// On fait de l'implicit VR little Endian 
	// (pour moins se fairche sur processeur INTEL)
	// On force le TRANSFERT SYNTAX UID
				
	SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
	SetElValueLengthByNumber(18, 0x0002, 0x0010);  // Le 0 de fin de chaine doit etre stock�, dans ce cas	
			
	TagElValueHT::iterator tag = tagHt.begin();
	
	elem = tag->second;
	gr   = elem->GetGroup();
	el   = elem->GetElement();
			
	if (el != 0x0000) {
		if(DEBUG)printf("ajout elem OOOO premiere fois\n");
		gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
		elemZPrec = new gdcmElValue(tagZ);	// on le cree
		elemZPrec->SetLength(4);
		Add(elemZPrec);				// On l'accroche � sa place
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

		if ( (gr != grCourant) /*&&	// On arrive sur un nv Groupe	  
		     (el != 0xfffe) */	) {
			    
			if (el != 0x0000) {
			 	gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
				elemZ = new gdcmElValue(tagZ); // on le cree
				elemZ->SetLength(4);
				Add(elemZ);	 		// On l'accroche � sa place 
				if(DEBUG)printf("ajout elem OOOO pour gr %04x\n",gr);
			} else { 
				elemZ=elem;
				if(DEBUG)printf("maj elmeZ\n");
			}
			
			ostringstream fock;
			fock << lgrCalcGroupe; 
			//sprintf(str_lgrCalcGroupe,"%d",lgrCalcGroupe);
			elemZPrec->SetValue(fock.str());
			if(DEBUG)printf("ecriture lgr (%d, %s) pour gr %04x\n",lgrCalcGroupe, fock.str().c_str(), grCourant);
			if(DEBUG)printf ("%04x %04x [%s]\n",elemZPrec->GetGroup(), elemZPrec->GetElement(),elemZPrec->GetValue().c_str());
			if(DEBUG)cout << "Addresse elemZPrec " << elemZPrec<< endl;
			elemZPrec=elemZ;
			lgrCalcGroupe = 0;
			grCourant     = gr;	
			if(DEBUG)printf("init-2 lgr (%d) pour gr %04x\n",lgrCalcGroupe, gr);			
		} else {			// On n'EST PAS sur un nv Groupe
			lgrCalcGroupe += 2 + 2 + 4 + elem->GetLength();  // Gr + Num + Lgr + LgrElem
			if(DEBUG)printf("increment (%d) el %04x-->lgr (%d) pour gr %04x\n",elem->GetLength(), el, lgrCalcGroupe, gr);
		}		
	}
	
} // fin if (1)

	
	// restent � tester les echecs en �criture (apres chaque fwrite)
	
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
		
		//fwrite ( vr,(size_t)2 ,(size_t)1 ,_fp); 	//VR
		
		// si on n'est pas en IMPLICIT VR voir pb (lgr  + VR)
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 	//lgr
		
		tokens.erase(tokens.begin(),tokens.end());
		Tokenize (tag2->second->GetValue(), tokens, "\\");
		
		//if (tokens.size() > 1) { printf ("size : %d\n",tokens.size());}
		
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
		
		// Les pixels ne sont pas charg�s dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}




int gdcmElValSet::WriteAcr(FILE * _fp) {


// ATTENTION : fonction non termin�e (commit�e a titre de precaution)
// ATTENTION : fusioner le code avec celui de lValSet::Write


	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_uint32;
	gint32  val_int32;
	guint16 val_uint16;
	gint16  val_int16;
	
	vector<string> tokens;
	
	void *ptr;
	char str_lgrCalcGroupe[10];
	
	//string implicitVRTransfertSyntax = "1.2.840.10008.1.2"; // supprime par rapport � Write
	
	// Utilis�es pour le calcul Group Length
	int deja = 0;
	guint32 lgrCalcGroupe=0;
	gdcmElValue *elem, *elemZ, *elemZPrec;
	guint16 grCourant = 0;
	
	// Question :
	// Comment pourrait-on tester si on est TrueDicom ou non ,
	// (FileType est un champ de gdcmHeader ...)
	//

	// On parcourt la table pour recalculer la longueur des 'elements 0x0000'
	// au cas ou un tag ai �t� ajout� par rapport � ce qui a �t� lu
	// dans l'image native
	//
	// cf : code IdDcmWriteFile dans libido/src/dcmwrite.c
		

			
	TagElValueHT::iterator tag = tagHt.begin();
	
	elem = tag->second;
	gr   = elem->GetGroup();
	el   = elem->GetElement();
			
	if (el != 0x0000) {
		if(DEBUG)printf("ajout elem OOOO premiere fois\n");
		gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
		elemZPrec = new gdcmElValue(tagZ);	// on le cree
		elemZPrec->SetLength(4);
		Add(elemZPrec);				// On l'accroche � sa place
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

		if ( (gr != grCourant) /*&&	// On arrive sur un nv Groupe	  
		     (el != 0xfffe) */	) {
			    
			if (el != 0x0000) {
			 	gdcmDictEntry * tagZ = new gdcmDictEntry(gr, 0x0000, "UL");
				elemZ = new gdcmElValue(tagZ); // on le cree
				elemZ->SetLength(4);
				Add(elemZ);	 		// On l'accroche � sa place 
				if(DEBUG)printf("ajout elem OOOO pour gr %04x\n",gr);
			} else { 
				elemZ=elem;
				if(DEBUG)printf("maj elmeZ\n");
			}
			
			ostringstream fock;
			fock << lgrCalcGroupe; 
			//sprintf(str_lgrCalcGroupe,"%d",lgrCalcGroupe);
			elemZPrec->SetValue(fock.str());
			if(DEBUG)printf("ecriture lgr (%d, %s) pour gr %04x\n",lgrCalcGroupe, fock.str().c_str(), grCourant);
			if(DEBUG)printf ("%04x %04x [%s]\n",elemZPrec->GetGroup(), elemZPrec->GetElement(),elemZPrec->GetValue().c_str());
			if(DEBUG)cout << "Addresse elemZPrec " << elemZPrec<< endl;
			elemZPrec=elemZ;
			lgrCalcGroupe = 0;
			grCourant     = gr;	
			if(DEBUG)printf("init-2 lgr (%d) pour gr %04x\n",lgrCalcGroupe, gr);			
		} else {			// On n'EST PAS sur un nv Groupe
			lgrCalcGroupe += 2 + 2 + 4 + elem->GetLength();  // Gr + Num + Lgr + LgrElem
			if(DEBUG)printf("increment (%d) el %04x-->lgr (%d) pour gr %04x\n",elem->GetLength(), el, lgrCalcGroupe, gr);
		}		
	}
	
	// Si on fait de l'implicit VR little Endian 
	// (pour moins se fairche sur processeur INTEL)
	// penser a forcer le TRANSFERT SYNTAX UID
	
	// supprime par rapport � Write		
	//SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
	//SetElValueLengthByNumber(18, 0x0002, 0x0010);  // Le 0 de fin de chaine doit etre stock�, dans ce cas	
		
	// restent � tester les echecs en �criture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag2 = tagHt.begin();
		  tag2 != tagHt.end();
		  ++tag2){

		gr =  tag2->second->GetGroup();
						// saut des groupes speciaux DICOM V3
		if (gr < 0x0008) continue; 	// ajout� par rapport � Write
						// saut des groupes impairs
		if (gr %2) 	 continue; 	// ajout� par rapport � Write
		
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
		
		// Les pixels ne sont pas charg�s dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}
