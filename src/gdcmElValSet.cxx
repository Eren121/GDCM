#include "gdcmlib.h"

int ElValSet::Add(ElValue * newElValue) {
	tagHt[newElValue->GetKey()]   = newElValue;
	NameHt[newElValue->GetName()] = newElValue;
}

int ElValSet::Print(ostream & os) {
	for (TagElValueHT::iterator tag = tagHt.begin();
		  tag != tagHt.end();
		  ++tag){
		os << tag->first << ": [";
		os << tag->second->GetValue() << "] [";
		os << tag->second->GetName() << "]" << endl;
	}
}
