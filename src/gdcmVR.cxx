/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVR.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/11 15:22:19 $
  Version:   $Revision: 1.36 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmVR.h"
#include "gdcmUtil.h"
#include "gdcmDictSet.h"
#include "gdcmDebug.h"

#include <fstream>
#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/// \brief auto generated function, to fill up the 'Value Representation'
///        Dictionnary, if relevant file is not found on user's disk
void FillDefaultVRDict(VRHT &vr);

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief Constructor
 */
VR::VR() 
{
   std::string filename = DictSet::BuildDictPath() + DICT_VR;
   std::ifstream from(filename.c_str());
   if(!from)
   {
      gdcmWarningMacro("Can't open dictionary" << filename.c_str());
      FillDefaultVRDict(vr);
   }
   else
   {
      char buff[1024];
      VRKey key;
      VRAtr name;
   
      while (!from.eof()) 
      {
         from >> std::ws;
         from.getline(buff, 1024, ' ');
         key = buff;
         from >> std::ws;
         from.getline(buff, 1024, ';');
         name = buff;
   
         from >> std::ws;
         from.getline(buff, 1024, '\n');
   
         if(key != "")
         {
            vr[key] = name;
         }
      }
      from.close();
   }
}

/**
 * \brief Destructor
 */
VR::~VR()
{
   vr.clear();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Get the count for an element
 * @param   key key to count
 */
int VR::Count(VRKey const &key) 
{
   return vr.count(key);
}

/**
 * \brief   Simple predicate that checks whether the given argument
 *          corresponds to the Value Representation of a \ref BinEntry .
 * @param   tested value representation to check for.
 */
bool VR::IsVROfBinaryRepresentable(VRKey const &tested)
{
   if ( tested == GDCM_UNKNOWN)
      return true;

   if ( IsVROfStringRepresentable(tested) )
      return false;

   if ( IsVROfSequence(tested) )
      return false;

   return true;
}

/**
 * \brief   Simple predicate that checks whether the given argument
 *          corresponds to the Value Representation of a \ref ValEntry
 *          but NOT a \ref BinEntry.
 * @param   tested value representation to be checked.
 */
bool VR::IsVROfStringRepresentable(VRKey const &tested)
{
   return tested == "AE" ||
          tested == "AS" ||
          tested == "CS" ||
          tested == "DA" ||
          tested == "DS" ||
          tested == "IS" || 
          tested == "LO" ||
          tested == "LT" ||
          tested == "PN" ||
          tested == "SH" ||
          tested == "SL" ||
          tested == "SS" ||
          tested == "ST" ||
          tested == "TM" ||
          tested == "UI" ||
          tested == "UL" ||
          tested == "UN" ||
          tested == "US";
}

/**
 * \brief   Simple predicate that checks whether the given argument
 *          corresponds to the Value Representation of a \ref SeqEntry
 * @param   tested value representation to check for.
 */
bool VR::IsVROfSequence(VRKey const &tested)
{
   return tested == "SQ";
}

bool VR::IsValidVR(VRKey const &key)
{
   return vr.find(key) != vr.end();
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void VR::Print(std::ostream &os) 
{
   std::ostringstream s;

   for (VRHT::iterator it = vr.begin(); it != vr.end(); ++it)
   {
      s << "VR : " << it->first << " = " << it->second << std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
