// gdcmElValSet.cxx

#include "gdcm.h"
#include "gdcmUtil.h"

TagElValueHT & ElValSet::GetTagHt(void) {
	return tagHt;
}

void ElValSet::Add(ElValue * newElValue) {
	tagHt[newElValue->GetKey()]   = newElValue;
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
 

int ElValSet::Write(FILE * _fp) {

// ATTENTION : fonction non terminée (commitée a titre de precaution)

	guint16 gr, el;
	guint32 lgr;
	const char * val;
	
	// A FAIRE :
	// parcourir la table pour recalculer la longueur des 'elements 0x0000'
	// au cas ou un tag ai été ajouté par rapport à ce qui a été lu
	// dans l'image native
	//
	// cf : code IdDcmWriteFile
	
/*	
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){


	}
*/
	// resteront à tester les echecs en écriture
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
		
		fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp); 	//group
		fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp); 	//element
		
		//fwrite ( tag->second->GetVR(),(size_t)2 ,(size_t)1 ,_fp); 	//VR
		// voir pb lgr  + VR
		
		fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp); 			//lgr
		 
		// ATTENTION
		// voir pb des int16 et int32 : les identifier, les convertir, modifier la longueur
		// ou alors stocker la valeur 16 ou 32 bits, + un indicateur : char, int16, int32
		
		fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); //valeur Elem
	}
	return(1);
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
	return(1);		
}

string ElValSet::GetElValueByName(string TagName) {
	if ( ! NameHt.count(TagName))
		return "gdcm::Unfound";
	if (NameHt.count(TagName) > 1)
		dbg.Verbose(0, "ElValSet::GetElValue",
		            "multipe entries for this key (FIXME) !");
	return NameHt.find(TagName)->second->GetValue();
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
}


