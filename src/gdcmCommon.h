/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmCommon.h,v $
  Language:  C++
  Date:      $Date: 2004/07/02 13:55:27 $
  Version:   $Revision: 1.28 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMCOMMON_H
#define GDCMCOMMON_H

#include "gdcmConfigure.h"

//-----------------------------------------------------------------------------
//This is needed when compiling in debug mode
#ifdef _MSC_VER
// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )
// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )
//'identifier' : decorated name length exceeded, name was truncated
#pragma warning ( disable : 4503 )
// C++ exception specification ignored except to indicate a 
// function is not __declspec(nothrow)
#pragma warning ( disable : 4290 )
// signed/unsigned mismatch
#pragma warning ( disable : 4018 )
// return type for 'identifier' is '' (ie; not a UDT or reference to UDT. Will
// produce errors if applied using infix notation
#pragma warning ( disable : 4284 )
// 'type' : forcing value to bool 'true' or 'false' (performance warning)
// //#pragma warning ( disable : 4800 )
#endif //_MSC_VER

//-----------------------------------------------------------------------------
#ifdef __GNUC__
#ifndef HAVE_NO_STDINT_H
#include <stdint.h>   // For uint16_t and uint32_t
#else
typedef  unsigned short uint16_t;
typedef  unsigned int   uint32_t;
#define UINT32_MAX    (4294967295U)
#endif //HAVE_NO_STDINT_H
#endif

#ifdef _MSC_VER 
typedef  unsigned short uint16_t;
typedef  unsigned int   uint32_t;
#define UINT32_MAX    (4294967295U)
#endif

#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#define getcwd _getcwd
#else
#define GDCM_EXPORT
#endif

// ifdef for old gcc / broken compiler
#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif


// Centralize information about the gdcm dictionary in only one file:
#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH   "../Dicts/"
#endif
#define PUB_DICT_NAME     "DicomV3Dict"
#define PUB_DICT_FILENAME "dicomV3.dic"
#define DICT_ELEM         "DicomDir.dic"
#define DICT_TS           "dicomTS.dic"
#define DICT_VR           "dicomVR.dic"

#include <string>

const std::string GDCM_UNFOUND = "gdcm::Unfound";   /// MEMORY LEAK

typedef std::string gdcmTagKey;
typedef std::string TagName;

enum FileType {
      gdcmUnknown = 0,
      gdcmExplicitVR, // gdcmDicomDir is in this case
      gdcmImplicitVR,
      gdcmACR,
      gdcmACR_LIBIDO
};

//-----------------------------------------------------------------------------
#endif
