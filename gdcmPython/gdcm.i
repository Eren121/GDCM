%module gdcm
%{
#include "gdcm.h"

// Utility functions on strings for removing leading and trailing spaces
void EatLeadingAndTrailingSpaces(string & s) {
	while ( s.length() && (s[0] == ' ') )
		s.erase(0,1);
	while ( s.length() && (s[s.length()-1] == ' ') )
		s.erase(s.length()-1, 1);
}
%}
typedef  unsigned short guint16;
typedef  unsigned int guint32;

%typemap(out) list<string> * {
	PyObject* NewItem = (PyObject*)0;
	PyObject* NewList = PyList_New(0); // The result of this typemap
	for (list<string>::iterator NewString = ($1)->begin();
	     NewString != ($1)->end(); ++NewString) {
		NewItem = PyString_FromString(NewString->c_str());
		PyList_Append( NewList, NewItem);
	}
	$result = NewList;
}

// Convert a c++ hash table in a python native dictionary
%typemap(out) map<string, list<string> > * {
	PyObject* NewDict = PyDict_New(); // The result of this typemap
	PyObject* NewKey = (PyObject*)0;
	PyObject* NewVal = (PyObject*)0;

	for (map<string, list<string> >::iterator tag = ($1)->begin();
	     tag != ($1)->end(); ++tag) {
      string first = tag->first;
      // Do not publish entries whose keys is made of spaces
      if (first.length() == 0)
         continue;
		NewKey = PyString_FromString(first.c_str());
		PyObject* NewList = PyList_New(0);
		for (list<string>::iterator Item = tag->second.begin();
		     Item != tag->second.end(); ++Item) {
			NewVal = PyString_FromString(Item->c_str());
			PyList_Append( NewList, NewVal);
		}
		PyDict_SetItem( NewDict, NewKey, NewList);
	}
	$result = NewDict;
}

// Convert a c++ hash table in a python native dictionary
%typemap(out) TagElValueHT & {
	PyObject* NewDict = PyDict_New(); // The result of this typemap
	string RawName;                   // Element name as gotten from gdcm
	PyObject* NewKey = (PyObject*)0;  // Associated name as python object
	string RawValue;                  // Element value as gotten from gdcm
	PyObject* NewVal = (PyObject*)0;  // Associated value as python object

	for (TagElValueHT::iterator tag = $1->begin(); tag != $1->end(); ++tag) {

		// The element name shall be the key:
		RawName = tag->second->GetName();
		// gdcm unrecognized (including not loaded because their size exceeds
		// the user specified treshold) elements are exported with their
		// TagKey as key.
		if (RawName == "Unknown")
			RawName = tag->second->GetKey();
		NewKey = PyString_FromString(RawName.c_str());

		// Element values are striped from leading/trailing spaces
		RawValue = tag->second->GetValue();
		EatLeadingAndTrailingSpaces(RawValue);
		NewVal = PyString_FromString(RawValue.c_str());

		PyDict_SetItem( NewDict, NewKey, NewVal);
    }
	$result = NewDict;
}

%include gdcm.h
