/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmGlobal.cxx,v $
  Language:  C++
  Date:      $Date: 2006/07/06 15:08:10 $
  Version:   $Revision: 1.33 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmGlobal.h"

#include "gdcmDebug.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmDictGroupName.h"
#include "gdcmDictSet.h"
#include "gdcmDicomDirElement.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
/// \brief Those global string that are returned by reference everywhere in 
/// gdcm code used to be in gdcmCommon.h but due to a 'bug' in gcc/MacOSX
/// you cannot have static initialization in a multithreaded environment
/// since there is a lazy construction everything got skrew up somehow
/// Therefore the actual initialization is done in a cxx file (avoid
/// duplicated symbol), and an extern is used in gdcmCommon.h

const std::string GDCM_UNKNOWN   = "gdcm::Unknown";
const std::string GDCM_UNFOUND   = "gdcm::Unfound";
const std::string GDCM_BINLOADED = "gdcm::Binary data loaded";
const std::string GDCM_NOTLOADED = "gdcm::NotLoaded";
const std::string GDCM_UNREAD    = "gdcm::UnRead";
const std::string GDCM_NOTASCII  = "gdcm::NotAscii";
const std::string GDCM_PIXELDATA = "gdcm::Pixel Data to be loaded";

const char GDCM_VRUNKNOWN[2] = {' ',' '}; // avoid useless std::string stuff

#ifdef _WIN32
const char GDCM_FILESEPARATOR = '\\';
#else
const char GDCM_FILESEPARATOR = '/';
#endif

//-----------------------------------------------------------------------------
DictSet         *Global::Dicts     = (DictSet *)0;
VR              *Global::ValRes    = (VR *)0;
TS              *Global::TranSyn   = (TS *)0;
DictGroupName   *Global::GroupName = (DictGroupName *)0;
DicomDirElement *Global::ddElem    = (DicomDirElement *)0;

Dict            *Global::DefaultPubDict = (Dict *)0;

//-----------------------------------------------------------------------------
/**
 * \brief   Global container
 */
Global Glob;

//-------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   constructor : populates the various H Tables
 */
Global::Global()
{
   if (ValRes || TranSyn || Dicts || ddElem || GroupName )
   {
      gdcmStaticWarningMacro( "VR or TS or Dicts or ... already allocated");
      return;
   }
   Dicts     = DictSet::New();
   ValRes    = VR::New();
   TranSyn   = TS::New();
   GroupName = DictGroupName::New();
   ddElem    = DicomDirElement::New();
}

/**
 * \brief   canonical destructor 
 */
Global::~Global()
{
   Dicts->Delete();
   ValRes->Delete();
   TranSyn->Delete();
   GroupName->Delete();
   ddElem->Delete();
}

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
