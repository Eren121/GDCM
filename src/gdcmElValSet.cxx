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

void ElValSet::PrintByName(ostream & os) {
	for (TagElValueNameHT::iterator tag = NameHt.begin();
		  tag != NameHt.end();
		  ++tag){
		os << tag->first << ": ";
		os << "[" << tag->second->GetValue() << "]";
		os << "[" << tag->second->GetKey()  << "]";
		os << "[" << tag->second->GetVR()    << "]" << endl;
	}
}

ElValue* ElValSet::GetElement(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return (ElValue*)0;
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "ElValSet::GetElValue",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second;
}

string ElValSet::GetElValue(guint32 group, guint32 element) {
	TagKey key = gdcmDictEntry::TranslateToKey(group, element);
	if ( ! tagHt.count(key))
		return "UNFOUND";
	if (tagHt.count(key) > 1)
		dbg.Verbose(0, "ElValSet::GetElValue",
		            "multiple entries for this key (FIXME) !");
	return tagHt.find(key)->second->GetValue();
}

string ElValSet::GetElValue(string TagName) {
	if ( ! NameHt.count(TagName))
		return "UNFOUND";
	if (NameHt.count(TagName) > 1)
		dbg.Verbose(0, "ElValSet::GetElValue",
		            "multipe entries for this key (FIXME) !");
	return NameHt.find(TagName)->second->GetValue();
}

