%module gdcm
%{
#include "gdcmCommon.h"
#include "gdcmDict.h"
#include "gdcmDictEntry.h"
#include "gdcmDictSet.h"
#include "gdcmDicomDir.h"
#include "gdcmDicomDirElement.h"
#include "gdcmDicomDirImage.h"
#include "gdcmDicomDirMeta.h"
#include "gdcmDicomDirObject.h"
#include "gdcmDicomDirPatient.h"
#include "gdcmDicomDirStudy.h"
#include "gdcmDicomDirSerie.h"
#include "gdcmDocEntrySet.h"
#include "gdcmDocument.h"
#include "gdcmElementSet.h"
#include "gdcmFile.h"
#include "gdcmGlobal.h"
#include "gdcmHeader.h"
#include "gdcmSerieHeader.h"
#include "gdcmRLEFramesInfo.h"
#include "gdcmJPEGFragmentsInfo.h"
#include "gdcmSQItem.h"
#include "gdcmUtil.h"
#include "gdcmValEntry.h"

////////////////////////////////////////////////////////////////////////////
/// Refer (below) to the definition of multi-argument typemap
///   %typemap(python, in)
///      ( gdcm::DicomDir::Method*, void*, gdcm::DicomDir::Method*)
/// for detail on gdcmPythonVoidFunc() and gdcmPythonVoidFuncArgDelete().
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
         std::cerr << "Caught a Ctrl-C within python, exiting program.\n";
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

/// This is required in order to avoid %including all the gdcm include files.
using namespace gdcm;
%}


///////////////////////  typemap section  ////////////////////////////////////

////////////////////////////////////////////////
// Convert an STL list<> to a python native list
%typemap(out) std::list<std::string> * 
{
   PyObject* NewItem = (PyObject*)0;
   PyObject* NewList = PyList_New(0); // The result of this typemap

   for (std::list<std::string>::iterator NewString = ($1)->begin();
        NewString != ($1)->end();
        ++NewString)
   {
      NewItem = PyString_FromString(NewString->c_str());
      PyList_Append( NewList, NewItem);
   }
   $result = NewList;
}

//////////////////////////////////////////////////////////////////
// Convert an STL map<> (hash table) to a python native dictionary
%typemap(out) std::map<std::string, std::list<std::string> > * 
{
   PyObject* NewDict = PyDict_New(); // The result of this typemap
   PyObject* NewKey = (PyObject*)0;
   PyObject* NewVal = (PyObject*)0;

   for (std::map<std::string,
        std::list<std::string> >::iterator tag = ($1)->begin();
        tag != ($1)->end(); ++tag)
   {
      std::string first = tag->first;
      // Do not publish entries whose keys is made of spaces
      if (first.length() == 0)
         continue;
      NewKey = PyString_FromString(first.c_str());
      PyObject* NewList = PyList_New(0);
      for (std::list<std::string>::iterator Item = tag->second.begin();
           Item != tag->second.end();
           ++Item)
      {
         NewVal = PyString_FromString(Item->c_str());
         PyList_Append( NewList, NewVal);
      }
      PyDict_SetItem( NewDict, NewKey, NewList);
   }
   $result = NewDict;
}

/////////////////////////////////////////////////////////
// Convert a c++ hash table in a python native dictionary
%typemap(out) gdcm::TagDocEntryHT & 
{
   PyObject* NewDict = PyDict_New(); // The result of this typemap
   std::string RawName;              // Element name as gotten from gdcm
   PyObject* NewKey = (PyObject*)0;  // Associated name as python object
   std::string RawValue;             // Element value as gotten from gdcm
   PyObject* NewVal = (PyObject*)0;  // Associated value as python object

   for (gdcm::TagDocEntryHT::iterator tag = $1->begin(); tag != $1->end(); ++tag)
   {
      // The element name shall be the key:
      RawName = tag->second->GetName();
      // gdcm unrecognized (including not loaded because their size exceeds
      // the user specified treshold) elements are exported with their
      // TagKey as key.
      if (RawName == "Unknown")
         RawName = tag->second->GetKey();
      NewKey = PyString_FromString(RawName.c_str());

      // Element values are striped from leading/trailing spaces
      if (gdcm::ValEntry* ValEntryPtr =
                dynamic_cast< gdcm::ValEntry* >(tag->second) )
      {
         RawValue = ValEntryPtr->GetValue();
      }
      else
        continue; 
      NewVal = PyString_FromString(RawValue.c_str());
      PyDict_SetItem( NewDict, NewKey, NewVal);
   }
   $result = NewDict;
}

/////////////////////////////////////
%typemap(out) ListDicomDirPatient & 
{
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (std::list<gdcm::DicomDirPatient *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New)
   {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_DicomDirPatient,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirStudy & 
{
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (std::list<gdcm::DicomDirStudy *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New)
   {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_DicomDirStudy,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirSerie & 
{
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (std::list<gdcm::DicomDirSerie *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New)
   {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_DicomDirSerie,1);
		PyList_Append($result, NewItem);
	}
}

%typemap(out) ListDicomDirImage & 
{
	PyObject* NewItem = (PyObject*)0;
	$result = PyList_New(0); // The result of this typemap

	for (std::list<gdcm::DicomDirImage *>::iterator New = ($1)->begin();
	    New != ($1)->end(); ++New) 
   {
		NewItem = SWIG_NewPointerObj(*New,SWIGTYPE_p_DicomDirImage,1);
		PyList_Append($result, NewItem);
	}
}

////////////////////////////////////////////////////////////////////////////
// Multi-argument typemap designed for wrapping the progress related methods
// in order to control from an external application the computation of
// a DicomDir object (see DicomDir::SetStartMethod*,
// DicomDir::SetProgressMethod* and DicomDir::SetEndMethod*).
// Motivation: since DicomDir parsing can be quite long, a GUI application
//             needs to display the avancement and potentially offer a
//             cancel method to the user (when this one feels things are
//             longer than expected).
// Example of usage: refer to demo/DicomDirProgressMethod.py
// Note: Uses gdcmPythonVoidFunc and gdcmPythonVoidFuncArgDelete defined
//       in the Swig verbatim section of this gdcm.i i.e. in the above section
//       enclosed within the %{ ... %} scope operator ).
%typemap(python, in) ( gdcm::DicomDir::Method *, 
                       void * = NULL, 
                       gdcm::DicomDir::Method * = NULL )
{
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

////////////////////  STL string versus Python str  ////////////////////////
// Convertion returning a C++ string.
%typemap(out) string, std::string 
{
    $result = PyString_FromString(($1).c_str());
}

// Convertion of incoming Python str to STL string
%typemap(python, in) const std::string, std::string
{
  $1 = PyString_AsString($input);
}

// Same convertion as above but references (since swig converts C++
// refererences to pointers)
%typemap(python, in) std::string const &
{
   $1 = new std::string( PyString_AsString( $input ) );
}

////////////////////////////////////////////////////////////////////////////
// Because overloading and %rename don't work together (see below Note 1)
// we need to ignore some methods (e.g. the overloaded default constructor).
// The gdcm::Header class doesn't have any SetFilename method anyhow, and
// this constructor is only used internaly (not from the API) so this is
// not a big loss.
%ignore gdcm::binary_write(std::ostream &,uint32_t const &);
%ignore gdcm::binary_write(std::ostream &,uint16_t const &);

%ignore gdcm::Header::Header();
%ignore gdcm::DicomDir::DicomDir();

////////////////////////////////////////////////////////////////////////////
// Warning: Order matters !
%include "gdcmCommon.h"
%include "gdcmDictEntry.h"
%include "gdcmDict.h"
%include "gdcmDocEntrySet.h"
%include "gdcmElementSet.h"
%include "gdcmDictSet.h"
%include "gdcmSQItem.h"
%include "gdcmDicomDirElement.h"
%include "gdcmDicomDirObject.h"
%include "gdcmDicomDirImage.h"
%include "gdcmDicomDirSerie.h"
%include "gdcmDicomDirStudy.h"
%include "gdcmDicomDirPatient.h"
%include "gdcmDicomDirMeta.h"
%include "gdcmDocument.h"
%include "gdcmHeader.h"
%include "gdcmSerieHeader.h"
%include "gdcmFile.h"
%include "gdcmUtil.h"
%include "gdcmGlobal.h"
%include "gdcmDicomDir.h"

////////////////////////////////////////////////////////////////////////////
// Notes on swig and this file gdcm.i:
//
/////////////////////////////////////
// Note 1: swig collision of method overloading and %typemap
// Consider the following junk.i file:
//     %module junk
//     %{
//     #include <string>
//     #include <iostream>
//     void Junk(std::string const & bozo) { std::cout << bozo << std::endl; }
//     void Junk() { std::cout << "Renamed Junk()" << std::endl; }
//     %}
//   
//     %typemap(python, in) std::string const &
//     {
//     $1 = new std::string( PyString_AsString( $input ) );
//     }
//     void Junk();
//     void Junk(std::string const & bozo);
//
// that we compile on linux with:
//    swig -c++ -python junk.i
//    g++ -g -I/usr/include/python2.3/ -o junk_wrap.o -c junk_wrap.cxx
//    g++ junk_wrap.o -shared -g -o _junk.so -L/usr/lib/python2.3/config \
//        -lpython2.3
// and invoque with:
//    python -c 'from junk import *; Junk("aaa") '
// then we get the following unexpected (for novice) python TypeError:
//    TypeError: No matching function for overloaded 'Junk'
//
// This happens because the swig generated code (at least for python) does
// the following two stage process:
//   1/ first do a dynamic dispatch ON THE NUMBER OF ARGUMENTS of the overloaded
//      Junk function (the same happens with method of course). [Note that the
//      dispatch is NOT done on the type of the arguments].
//   2/ second apply the typemap.
// When the first dynamic dispatch is executed, the swig generated code
// has no knowledge of the typemap, and thus expects a pointer to a std::string
// type i.e. an argument to Junk of the form _p_std__int<address>. But this
// is not what python handles to Junk ! An invocation of the form 'Junk("aaa")'
// will make Python pass a PyString to swig (and this is precisely why we
// wrote the typemap). And this will fail....
/////////////////////////////////////
