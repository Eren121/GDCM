// gdcmElValSet.cxx

#include "gdcmUtil.h"
#include "gdcmElValSet.h"


#include <vector>
static void Tokenize (const string& str, vector<string>& tokens, const string& delimiters = " ");


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

	// Question : m � j LgrElem ?
	tagHt[key]->SetLength(strlen(content.c_str()));	 

	// FIXME should we really update the element length ?
	tagHt[key]->SetLength(content.length());	 

	return(1);		
}

int ElValSet::SetElValueByName(string content, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	if (NameHt.count(TagName) > 1) {
		dbg.Verbose(0, "ElValSet::SetElValueByName",
		            "multipe entries for this key (FIXME) !");
		return 0;
	}
	NameHt.find(TagName)->second->SetValue(content);
	NameHt.find(TagName)->second->SetLength(strlen(content.c_str()));	 
	return(1);		
}


int ElValSet::SetElValueLengthByNumber(guint32 l, guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return 0;
	if (tagHt.count(key) > 1) {
		dbg.Verbose(0, "ElValSet::SetElValueLengthByNumber",
		            "multiple entries for this key (FIXME) !");
		return (0); 
	}		                       
	// m � j LgrElem 
	tagHt[key]->SetLength(l);	 
	return(1);		
}


int ElValSet::SetElValueLengthByName(guint32 l, string TagName) {
	if ( ! NameHt.count(TagName))
		return 0;
	if (NameHt.count(TagName) > 1) {
		dbg.Verbose(0, "ElValSet::SetElValueByName",
		            "multipe entries for this key (FIXME) !");
		return 0;
	}
	NameHt.find(TagName)->second->SetLength(l);	 
	return(1);		
}


int ElValSet::Write(FILE * _fp) {

// ATTENTION : fonction non termin�e (commit�e a titre de precaution)

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
	
	string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
	
	// Utilis�es pour le calcul Group Length
	int deja = 0;
	guint32 lgrCalcGroupe=0;
	ElValue *elem, *elemZ, *elemZPrec;
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
		
	if (0)  // Risque de pb dans le calcul des lgr de chaque groupe. On le saute pour le moment!
	
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){

		elem = tag->second;
		printf("gr %04x el %04x lgr %d\n",elem->GetGroup(), elem->GetElement(), elem->GetLength());
	
		if ( (elem->GetGroup() != grCourant) &&	  
			 (elem->GetGroup() != 0xfffe)	) { 	// On arrive sur un nv Groupe
			 
		printf("Nouv Groupegr %04x el %04x \n",elem->GetGroup(), elem->GetElement());

			elemZ = elem; 
			 
			if(elemZ->GetElement() != 0x0000) { 	// pas d'element 'Lgr groupe'
				// On cr�e
			 	gdcmDictEntry * tagZ = new gdcmDictEntry(grCourant, 0x0000, "UL");
				elemZ = new ElValue(tagZ); // on le cree
				elemZ->SetLength(4);
				Add(elemZ);	 				 // On l'accroche � sa place	
			}	
			
			if (deja) {
				//sprintf(str_lgrCalcGroupe,"%d",lgrCalcGroupe);
				elemZPrec->SetValue(str_lgrCalcGroupe);
				lgrCalcGroupe = 0;
			}
			deja = 1;
			
			lgrCalcGroupe =  12; //2 + 2 + 4 + 4; // Gr + Num + Lgr + LgrGroupe
			printf ("lgrCalcGroupe %d\n",lgrCalcGroupe);
			
			elemZPrec = elemZ;
	 		grCourant = elem->GetGroup();
							
		} else {		// On n'EST PAS sur un nv Groupe
		
			printf ("lgrCalcGroupe avant : %d LgrElem %d\n",lgrCalcGroupe,elem->GetLength());

			lgrCalcGroupe += 2 + 2 + 4 + elem->GetLength();  // Gr + Num + Lgr + LgrElem 
			
			printf ("lgrCalcGroupe apres %d\n",lgrCalcGroupe);
		}		
	}
	
	// Si on fait de l'implicit VR little Endian 
	// (pour moins se fairche sur processeur INTEL)
	// penser a forcer le TRANSFERT SYNTAX UID
				
	SetElValueByNumber(implicitVRTransfertSyntax, 0x0002, 0x0010);	
	SetElValueLengthByNumber(18, 0x0002, 0x0010);  // Le 0 de fin de chaine doit etre stock�, dans ce cas	
		
	// restent � tester les echecs en �criture (apres chaque fwrite)
	
	for (TagElValueHT::iterator tag2 = tagHt.begin();
		  tag2 != tagHt.end();
		  ++tag2){
		
		gr =  tag2->second->GetGroup();
		el =  tag2->second->GetElement();
		lgr = tag2->second->GetLength();
		val = tag2->second->GetValue().c_str();
		vr =  tag2->second->GetVR();
			
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
		
		//fwrite ( vr,(size_t)2 ,(size_t)1 ,_fp); 	//VR
		
		// si on n'est pas en IMPLICIT VR voir pb (lgr  + VR)
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 		//lgr
		
		tokens.erase(tokens.begin(),tokens.end());
		Tokenize (tag2->second->GetValue(), tokens, "\\");
		
		//printf ("%04x %04x [%s] : [%s]\n",gr, el, vr.c_str(), val);
		//if (tokens.size() > 1) { printf ("size : %d\n",tokens.size());}
		
		
		if (vr == "US" || vr == "SS") {
			/*
			val_int16 = atoi(val);
			ptr = &val_int16;
			fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);	
			continue;
			*/
			
			for (unsigned int i=0; i<tokens.size();i++) {
				val_uint16 = atoi(tokens[i].c_str());		
				ptr = &val_uint16;
				fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);
			}
			continue;
			
		}
		if (vr == "UL" || vr == "SL") {
			/*
			val_int32 = atoi(val);
			ptr = &val_int32;
			fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);	
			continue;
			*/
			
			
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




int ElValSet::WriteAcr(FILE * _fp) {


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
	ElValue *elem, *elemZ, *elemZPrec;
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
		
	if (1)  // Risque de pb dans le calcul des lgr de chaque groupe. On le saute pour le moment!
	
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){

		elem = tag->second;
		//printf("gr %04x el %04x lgr %d\n",elem->GetGroup(), elem->GetElement(), elem->GetLength());
	
		if ( (elem->GetGroup() != grCourant) &&	  
			 (elem->GetGroup() != 0xfffe)	) { 	// On arrive sur un nv Groupe
			 
		//printf("Nouv Groupegr %04x el %04x \n",elem->GetGroup(), elem->GetElement());

			elemZ = elem; 
			 
			if(elemZ->GetElement() != 0x0000) { 	// pas d'element 'Lgr groupe'
				// On cr�e
			 	gdcmDictEntry * tagZ = new gdcmDictEntry(grCourant, 0x0000, "UL");
				elemZ = new ElValue(tagZ); // on le cree
				elemZ->SetLength(4);
				Add(elemZ);	 				 // On l'accroche � sa place	
			}	
			
			if (deja) {
				//sprintf(str_lgrCalcGroupe,"%d",lgrCalcGroupe);
				elemZPrec->SetValue(str_lgrCalcGroupe);
				lgrCalcGroupe = 0;
			}
			deja = 1;
			
			lgrCalcGroupe =  12; //2 + 2 + 4 + 4; // Gr + Num + Lgr + LgrGroupe
			//printf ("lgrCalcGroupe %d\n",lgrCalcGroupe);
			
			elemZPrec = elemZ;
	 		grCourant = elem->GetGroup();
							
		} else {		// On n'EST PAS sur un nv Groupe
		
			//printf ("lgrCalcGroupe avant : %d LgrElem %d\n",lgrCalcGroupe,elem->GetLength());

			lgrCalcGroupe += 2 + 2 + 4 + elem->GetLength();  // Gr + Num + Lgr + LgrElem 
			
			//printf ("lgrCalcGroupe apres %d\n",lgrCalcGroupe);
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




// mettre �a dans une biblioth�que d'utilitaires ?
// ca peut servir

static void Tokenize (const string& str, vector<string>& tokens, const string& delimiters = " ") {
	string::size_type lastPos = str.find_first_not_of(delimiters,0);
	string::size_type pos     = str.find_first_of(delimiters,lastPos);
	while (string::npos != pos || string::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}
