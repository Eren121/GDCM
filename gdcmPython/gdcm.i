%module gdcm
%{
#include "gdcmCommon.h"
#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"
#include "gdcmParser.h"
#include "gdcmHeaderEntry.h"
#include "gdcmHeader.h"
#include "gdcmHeaderHelper.h"
#include "gdcmFile.h"
#include "gdcmUtil.h"
#include "gdcmGlobal.h"
#include "gdcmObject.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirElement.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDicomDirImage.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// Utility functions on strings for removing leading and trailing spaces
void EatLeadingAndTrailingSpaces(string & s) {
	while ( s.length() && (s[0] == ' ') )
		s.erase(0,1);
	while ( s.length() && (s[s.length()-1] == ' ') )
		s.erase(s.length()-1, 1);
}

void gdcmPythonVoidFunc(void *arg)
{
  PyObject *arglist, *result;
  PyObject *func = (PyObject *)arg;

  arglist = Py_BuildValue("()");

  result = PyEval_CallObject(func, arglist);
  Py_DECREF(arglist);

  if (result)
    {
    Py_XDECREF(result);
    }
  else
    {
    if (PyErr_ExceptionMatches(PyExc_KeyboardInterrupt))
      {
      cerr << "Caught a Ctrl-C within python, exiting program.\n";
      Py_Exit(1);
      }
    PyErr_Print();
    }
}

void gdcmPythonVoidFuncArgDelete(void *arg)
{
  PyObject *func = (PyObject *)arg;
  if (func)
    {
    Py_DECREF(func);
    }
}

%}
typedef  unsigned short guint16;
typedef  unsigned int guint32;

////////////////////////////////////////////////////////////////////////////
// Global variables get exported to cvar in Python
%immutable;
extern gdcmGlobal gdcmGlob;
%mutable;

////////////////////////////////////////////////////////////////////////////
%typemap(out) std::list<std::string> * {
	PyObject* NewItem = (PyObject*)0;
	PyObject* NewList = PyList_New(0); // The result of this typemap
	for (list<string>::iterator NewString = ($1)->begin();
	     NewString != ($1)->end(); ++NewString) {
		NewItem = PyString_FromString(NewString->c_str());
		PyList_Append( NewList, NewItem);
	}
	$result = NewList;
}

////////////////////////////////////////////////////////////////////////////
// Convert a c++ hash table in a python native dictionary
%typemap(out) std::map<std::string, std::list<std::string> > * {
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

////////////////////////////////////////////////////////////////////////////
// Convert a c++ hash table in a python native dictionary
%typemap(out) TagHeaderEntryHT & {
	PyObject* NewDict = PyDict_New(); // The result of this typemap
	string RawName;                   // Element name as gotten from gdcm
	PyObject* NewKey = (PyObject*)0;  // Associated name as python object
	string RawValue;                  // Element value as gotten from gdcm
	PyObject* NewVal = (PyObject*)0;  // Associated value as python object

	for (TagHeaderEntryHT::iterator tag = $1->begin(); tag != $1->end(); ++tag) {

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

%typemap(out) TagHeaderEntryHT {
	PyObject* NewDict = PyDict_New(); // The result of this typemap
	string RawName;                   // Element name as gotten from gdcm
	PyObject* NewKey = (PyObject*)0;  // Associated name as python object
	string RawValue;                  // Element value as gotten from gdcm
	PyObject* NewVal = (PyObject*)0;  // Associated value as python object

	for (TagHeaderEntryHT::iterator tag = $1.begin(); tag != $1.end(); ++tag) {

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

////////////////////////////////////////////////////////////////////////////
%typemap(out) ListDicomDirPatient & {
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (list<gdcmDicomDirPatient *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New) {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_gdcmDicomDirPatient,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirStudy & {
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (list<gdcmDicomDirStudy *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New) {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_gdcmDicomDirStudy,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirSerie & {
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (list<gdcmDicomDirSerie *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New) {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_gdcmDicomDirSerie,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirImage & {
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (list<gdcmDicomDirImage *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New) {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_gdcmDicomDirImage,1);
		PyList_Append($result, NewItem);
	}
}

////////////////////////////////////////////////////////////////////////////
// Deals with function returning a C++ string.
%typemap(python, in) (gdcmMethod *,void * =NULL,gdcmMethod * =NULL) {
	if($input!=Py_None)
	{
		Py_INCREF($input);
		$1=gdcmPythonVoidFunc;
		$2=$input;
		$3=gdcmPythonVoidFuncArgDelete;
	}
	else
	{
		$1=NULL;
		$2=NULL;
		$3=NULL;
	}
}


////////////////////////////////////////////////////////////////////////////
// Deals with function returning a C++ string.
%typemap(out) string, std::string  {
    $result = PyString_FromString(($1).c_str());
}

%typemap(python, in) const std::string, std::string
{
  $1 = PyString_AsString($input);
}

////////////////////////////////////////////////////////////////////////////
%include "gdcmCommon.h"
%include "gdcmDictEntry.h"
%include "gdcmDict.h"
%include "gdcmDictSet.h"
%include "gdcmParser.h"
%include "gdcmHeaderEntry.h"
%include "gdcmHeader.h"
%include "gdcmHeaderHelper.h"
%include "gdcmFile.h"
%include "gdcmUtil.h"
%include "gdcmGlobal.h"
%include "gdcmObject.h"
%include "gdcmDicomDir.h"
%include "gdcmDicomDirElement.h"
%include "gdcmDicomDirMeta.h"
%include "gdcmDicomDirPatient.h"
%include "gdcmDicomDirStudy.h"
%include "gdcmDicomDirSerie.h"
%include "gdcmDicomDirImage.h"
