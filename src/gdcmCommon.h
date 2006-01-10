/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmCommon.h,v $
  Language:  C++
  Date:      $Date: 2006/01/10 15:54:03 $
  Version:   $Revision: 1.102 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMCOMMON_H
#define GDCMCOMMON_H

#include "gdcmConfigure.h"
#include "gdcmSystem.h"
#include "gdcmMacro.h"

#include <string>

//-----------------------------------------------------------------------------
#if defined(_WIN32) && defined(BUILD_SHARED_LIBS)
  #ifdef gdcm_EXPORTS
    #define GDCM_EXPORT __declspec( dllexport )
  #else
    #define GDCM_EXPORT __declspec( dllimport )
  #endif
#else
  #define GDCM_EXPORT
#endif

//-----------------------------------------------------------------------------
/// \brief namespace for Grass root DiCoM
namespace gdcm
{

// Centralize information about the gdcm dictionary in only one file:
#ifndef PUB_DICT_PATH
#  define PUB_DICT_PATH   "../Dicts/"
#endif
#define PUB_DICT_NAME     "DicomV3Dict"
#define PUB_DICT_FILENAME "gdcm.dic"
#define DICT_ELEM         "DicomDir.dic"
#define DICT_TS           "dicomTS.dic"
#define DICT_VR           "dicomVR.dic"
#define DICT_GROUP_NAME   "DictGroupName.dic"

GDCM_EXPORT extern const std::string GDCM_UNKNOWN;
GDCM_EXPORT extern const std::string GDCM_UNFOUND;
GDCM_EXPORT extern const std::string GDCM_BINLOADED;
GDCM_EXPORT extern const std::string GDCM_NOTLOADED;
GDCM_EXPORT extern const std::string GDCM_UNREAD;
GDCM_EXPORT extern const std::string GDCM_NOTASCII;
GDCM_EXPORT extern const std::string GDCM_PIXELDATA;

GDCM_EXPORT extern const std::string GDCM_VRUNKNOWN;

GDCM_EXPORT extern const char GDCM_FILESEPARATOR;

/// \brief TagKey is made to hold the standard Dicom Tag 
///               (Group number, Element number)
/// Instead of using the two '16 bits integers' as the Hask Table key, we
/// converted into a string (e.g. 0x0018,0x0050 converted into "0018|0050")
/// It appears to be a huge waste of time.
/// We'll fix the mess up -without any change in the API- as soon as the bench
/// marks are fully performed.

#if defined(_MSC_VER) && (_MSC_VER == 1200)
// Doing everything within gdcm namespace to avoid polluting 3d party software
inline std::ostream& operator<<(std::ostream& _O, std::string _val)
{
  return _O << _val.c_str();
}
#endif

/// \brief TagName is made to hold the 'non hexa" fields (VR, VM, Name) 
///        of Dicom Entries
typedef std::string TagName;

/// \brief various types of a DICOM file (for internal use only)
enum FileType {
   Unknown = 0,
   ExplicitVR, // DicomDir is in this case. Except when it's ImplicitVR !...
   ImplicitVR,
   ACR,
   ACR_LIBIDO,
   JPEG
};

/// \brief type of the elements composing a DICOMDIR (for internal use only)
enum DicomDirType {
   DD_UNKNOWN = 0,
   DD_META,
   DD_PATIENT,
   DD_STUDY,
   DD_SERIE,
   DD_IMAGE,
   DD_VISIT
};

/// \brief comparison operators (as used in SerieHelper::AddRestriction() )
enum CompOperators {
   GDCM_EQUAL = 0,
   GDCM_DIFFERENT,
   GDCM_GREATER,
   GDCM_GREATEROREQUAL,
   GDCM_LESS,
   GDCM_LESSOREQUAL
};

/// \brief Loading mode
enum LodModeType
{
   LD_ALL         = 0x00000000, // Load all
   LD_NOSEQ       = 0x00000001, // Don't load Sequences
   LD_NOSHADOW    = 0x00000002, // Don't load odd groups
   LD_NOSHADOWSEQ = 0x00000004  // Don't load Sequences if they belong 
                                //            to an odd group
                                // (*exclusive* from LD_NOSEQ and LD_NOSHADOW)
};

/**
 * \brief structure, for internal use only
 */  
struct DicomElement
{
   /// Dicom Group number
   unsigned short int Group;
   /// Dicom Element number
   unsigned short int Elem;
   /// value (coded as a std::string) of the Element
   std::string Value;
};

} //namespace gdcm
//-----------------------------------------------------------------------------
#endif
