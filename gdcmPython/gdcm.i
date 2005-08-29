%module gdcm
#pragma SWIG nowarn=504
%{
#include <iostream>

#include "gdcmCommon.h"
#include "gdcmBase.h"
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
#include "gdcmFileHelper.h"
#include "gdcmGlobal.h"
#include "gdcmFile.h"
#include "gdcmSerieHelper.h"
#include "gdcmRLEFramesInfo.h"
#include "gdcmJPEGFragmentsInfo.h"
#include "gdcmSQItem.h"
#include "gdcmUtil.h"
#include "gdcmDocEntry.h"
#include "gdcmContentEntry.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictGroupName.h"

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
// Redefine all types used
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

////////////////////////////////////////////////
// Convert a DocEntry * to the real derived class
%typemap(out) gdcm::DocEntry * 
{
   PyObject *newEntry;

   if($1)
   {
      if(dynamic_cast<SeqEntry *>($1)) // SeqEntry *
         newEntry = SWIG_NewPointerObj($1,SWIGTYPE_p_gdcm__SeqEntry,0);
      else if(dynamic_cast<BinEntry *>($1)) // BinEntry *
         newEntry = SWIG_NewPointerObj($1,SWIGTYPE_p_gdcm__BinEntry,0);
      else // ValEntry *
         newEntry = SWIG_NewPointerObj($1,SWIGTYPE_p_gdcm__ValEntry,0);
   }
   else
   {
      newEntry = Py_BuildValue("");
   }
   $result = newEntry;
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
%typemap(python, in) (void(*method)(void *),void *arg,void(*argDelete)(void *))
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
%typemap(out) std::string
{
    $result = PyString_FromString(($1).c_str());
}

%typemap(out) string
{
    $result = PyString_FromString(($1).c_str());
}

%typemap(out) std::string const &
{
    $result = PyString_FromString(($1)->c_str());
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

////////////////////  gdcm.TagName versus Python str  //////////////////////
%typemap(out) gdcm::TagName, const gdcm::TagName &
{
    $result = PyString_FromString(($1)->c_str());
}

// Convertion of incoming Python str to STL string
%typemap(python, in) const gdcm::TagName, gdcm::TagName
{
  $1 = PyString_AsString($input);
}

// Same convertion as above but references (since swig converts C++
// refererences to pointers)
%typemap(python, in) gdcm::TagName const &
{
   $1 = new std::string( PyString_AsString( $input ) );
}

////////////////////////////////////////////////////////////////////////////
// Because overloading and %rename don't work together (see below Note 1)
// we need to ignore some methods (e.g. the overloaded default constructor).
// The gdcm::File class doesn't have any SetFilename method anyhow, and
// this constructor is only used internaly (not from the API) so this is
// not a big loss.
%ignore gdcm::binary_write(std::ostream &,uint32_t const &);
%ignore gdcm::binary_write(std::ostream &,uint16_t const &);

%ignore gdcm::DicomDir::SetStartMethod(DicomDir::Method *method,void *arg = NULL);
%ignore gdcm::DicomDir::SetProgressMethod(DicomDir::Method *method,void *arg = NULL);
%ignore gdcm::DicomDir::SetEndMethod(DicomDir::Method *method,void *arg = NULL);

// Ignore all placed in gdcmCommon.h
%ignore GDCM_UNKNOWN;
%ignore GDCM_UNFOUND;
%ignore GDCM_BINLOADED;
%ignore GDCM_NOTLOADED;
%ignore GDCM_UNREAD;

%constant const char *UNKNOWN   = "gdcm::Unknown";
%constant const char *UNFOUND   = "gdcm::Unfound";
%constant const char *BINLOADED = "gdcm::Binary data loaded";
%constant const char *NOTLOADED = "gdcm::NotLoaded";
%constant const char *UNREAD    = "gdcm::UnRead";

////////////////////////////////////////////////////////////////////////////
// Warning: Order matters !
%include "gdcmCommon.h"
%include "gdcmBase.h"
%include "gdcmDictEntry.h"
%include "gdcmDict.h"
%include "gdcmDictSet.h"
%include "gdcmDocEntrySet.h"
%include "gdcmElementSet.h"
%include "gdcmSQItem.h"
%include "gdcmDicomDirElement.h"
%include "gdcmDicomDirObject.h"
%include "gdcmDicomDirImage.h"
%include "gdcmDicomDirSerie.h"
%include "gdcmDicomDirStudy.h"
%include "gdcmDicomDirPatient.h"
%include "gdcmDicomDirMeta.h"
%include "gdcmDocument.h"
%include "gdcmFile.h"
%include "gdcmSerieHelper.h"
%include "gdcmFileHelper.h"
%include "gdcmUtil.h"
%include "gdcmGlobal.h"
%include "gdcmDicomDir.h"
%include "gdcmDocEntry.h"
%include "gdcmContentEntry.h"
%include "gdcmValEntry.h"
%include "gdcmBinEntry.h"
%include "gdcmSeqEntry.h"
%include "gdcmVR.h"
%include "gdcmTS.h"
%include "gdcmDictGroupName.h"

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
