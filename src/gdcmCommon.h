//gdcmCommon.h
//-----------------------------------------------------------------------------
#ifndef GDCMCOMMON_H
#define GDCMCOMMON_H

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
#include <stdint.h>
#define guint16 uint16_t
#define guint32 uint32_t
#define gint16  int16_t
#define gint32  int32_t
#else
typedef  unsigned short guint16;
typedef  unsigned int   guint32;
typedef  short 		gint16;
typedef  int   		gint32;
#define UINT32_MAX    (4294967295U)
#endif //HAVE_NO_STDINT_H
#endif

#ifdef _MSC_VER 
typedef  unsigned short guint16;
typedef  unsigned int   guint32;
typedef  short 		gint16;
typedef  int   		gint32;
#define UINT32_MAX    (4294967295U)
#endif

#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#define getcwd _getcwd
#else
#define GDCM_EXPORT
#endif

#include <string>

const std::string GDCM_UNFOUND = "gdcm::Unfound";

typedef std::string TagKey;
typedef std::string TagName;

enum FileType {
      Unknown = 0,
      ExplicitVR, // gdcmDicomDir are in this case
      ImplicitVR,
      ACR,
      ACR_LIBIDO
};

//For now gdcm is not willing cmake, try to be more quiet
//#cmakedefine GDCM_NO_ANSI_STRING_STREAM

//-----------------------------------------------------------------------------
#endif
