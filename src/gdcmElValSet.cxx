// gdcmElValSet.cxx

#include "gdcm.h"
#include "gdcmUtil.h"

TagElValueHT & ElValSet::GetTagHt(void) {
	return tagHt;
}

void ElValSet::Add(ElValue * newElValue) {
	tagHt [newElValue->GetKey()]  = newElValue;
	NameHt[newElValue->GetName()] = newElValue;
}

void ElValSet::Print(ostream & os) {
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){
		os << tag->first << ": ";
		os << "[" << tag->second->GetValue() << "]";
		os << "[" << tag->second->GetName()  << "]";
		os << "[" << tag->second->GetVR()    << "]" << endl;
	}
} 

void ElValSet::PrintByName(ostream & os) {
	for (TagElValueNameHT::iterator tag = NameHt.begin();
		  tag != NameHt.end();
		  ++tag){
		os << tag->first << ": ";
		os << "[" << tag->second->GetValue() << "]";
		os << "[" << tag->second->GetKey()   << "]";
		os << "[" << tag->second->GetVR()    << "]" << endl;
	}
}

ElValue* ElValSet::GetElementByNumber(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return (ElValue*)0;
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "ElValSet::GetElementByNumber",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second;
}

ElValue* ElValSet::GetElementByName(string TagName) {
   if ( ! NameHt.count(TagName))
      return (ElValue*)0;
   if (NameHt.count(TagName) > 1)
      dbg.Verbose(0, "ElValSet::GetElement",
                  "multipe entries for this key (FIXME) !");
   return NameHt.find(TagName)->second;
}

string ElValSet::GetElValueByNumber(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return "gdcm::Unfound";
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "ElValSet::GetElValueByNumber",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second->GetValue();
}

string ElValSet::GetElValueByName(string TagName) {
	if ( ! NameHt.count(TagName))
		return "gdcm::Unfound";
	if (NameHt.count(TagName) > 1)
		dbg.Verbose(0, "ElValSet::GetElValue",
		            "multipe entries for this key (FIXME) !");
	return NameHt.find(TagName)->second->GetValue();
}

int ElValSet::SetElValueByNumber(string content, guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	if (tagHt.count(key) > 1) {
		dbg.Verbose(0, "ElValSet::SetElValueByNumber",
		            "multiple entries for this key (FIXME) !");
		return (0); 
	}		                       
	tagHt[key]->SetValue(content);
	// Question : m à j LgrElem ?
	tagHt[key]->SetLength(strlen(content.c_str()));	 
				// Ou trouver les fonctions d'une classe donnée?
				// lgr d'une string, p.ex 
	return(1);		
}


int ElValSet::SetElValueByName(string content, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	if (NameHt.count(TagName) > 1) {
		dbg.Verbose(0, "ElValSet::SetElValue",
		            "multipe entries for this key (FIXME) !");
		return 0;
	}
	NameHt.find(TagName)->second->SetValue(content);
	NameHt.find(TagName)->second->SetLength(strlen(content.c_str()));	 
	return(1);		
}



int ElValSet::Write(FILE * _fp) {

// ATTENTION : fonction non terminée (commitée a titre de precaution)

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	string vr;
	guint32 val_int32;
	guint16 val_int16;
	void *ptr;
	
	string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
	
/*	// Utilisées pour le calcul Group Length
	int deja = 0;
	guint32 lgrCalcGroupe;
	ElValue * elemZ, *elemZPrec;
	guint16 grCourant = 0;
	
*/

	// Question :
	// Comment pourrait-on tester si on est TRueDicom ou non ,
	// (FileType est un champ de gdcmHeader ...)
	//

	// On parcourt la table pour recalculer la longueur des 'elements 0x0000'
	// au cas ou un tag ai été ajouté par rapport à ce qui a été lu
	// dans l'image native
	//
	// cf : code IdDcmWriteFile

/*
		// Pas le temps de finir 
		// voir libido/src/dcmwrite.c
		//
		// mais avant ... voir si le 'group length', lorsqu'il est present
		// sert encore a qq chose
		// patcher une image DICOM, mettre une lgr erronnée
		// et voir si e-film la reconnait ...
		
		
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){
		  
		elemZ = tag->second;
	
		if ( (elemZ->GetGroup() != grCourant) &&	  
			 (elemZ->GetGroup() != 0xfffe)	) { 	// On arrive sur un nv Groupe
			 
			if(elemZ->GetNum != 0x0000) { 	// pas d'element 'Lgr groupe'
				// On le crée
			 	gdcmDictEntry * tagZ = IsInDicts(tag->second->GetGroup(), 0);
				elemZ = new (ElValue(tagZ)); // on le cree
				elemZ.SetLength(4);
				Add(elemZ);	 				 // On l'accroche à sa place	
			}	
			
			if (deja) {
			
			// A FINIR
			}
			deja = 1;
			elemZPrec = elemZ;
	 		grCourant = elemZ->GetGroup();
			lgrCalcGroupe =  12; //2 + 2 + 4 + 4; // lgr (Gr + Num + LgrElem + LgrGroupe)
							
		} else {		// On n'EST PAS sur un nv Groupe
		
			lgrCalcGroupe += 2 + 2;  // lgr (Gr + Num )
		
			if (IsVrUnknowkn()) {
			
			// A FINIR
			
			}
			
			// A FINIR	  
	}

*/
	
	// Si on fait de l'implicit VR littele Endian 
	// (pour moins se fairche sur processeur INTEL)
	// penser a forcer le SYNTAX TRANSFERT UID
				
	SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
		
	// restent à tester les echecs en écriture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){

		// Question :
		// peut-on se passer des affectations?
		// - passer l'adresse du resultat d'une fonction (???)
		// - acceder au champ sans passer par un accesseur ?
		
		gr =  tag->second->GetGroup();
		el =  tag->second->GetElement();
		lgr = tag->second->GetLength();
		val = tag->second->GetValue().c_str();
		vr =  tag->second->GetVR();
			
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
		
		//fwrite ( vr,(size_t)2 ,(size_t)1 ,_fp); 	//VR
		
		// si on n'est pas en IMPLICIT VR voir pb (lgr  + VR)
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 			//lgr
		
		if (vr == "US" || vr == "SS") {
			val_int16 = atoi(val);
			ptr = &val_int16;
			fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);	
			continue;
		}
		if (vr == "UL" || vr == "SL") {
			val_int32 = atoi(val);
			ptr = &val_int32;
			fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);	
			continue;
		}	
		
		// Les pixels ne sont pas chargés dans l'element !
		if ((gr == 0x7fe0) && (el == 0x0010) ) break;

		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
		
	return(1);
}
