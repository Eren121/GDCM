/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVR.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:08 $
  Version:   $Revision: 1.17 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <fstream>

#include <iostream>

#include "gdcmVR.h"
#include "gdcmUtil.h"
#include "gdcmDictSet.h"
#include "gdcmDebug.h"

//-----------------------------------------------------------------------------
/**
 * \brief Constructor
 */
gdcmVR::gdcmVR(void) 
{
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_VR);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmVR::gdcmVR: can't open dictionary",filename.c_str());

   char buff[1024];
   std::string key;
   std::string name;

   while (!from.eof()) 
   {
      eatwhite(from);
      from.getline(buff, 1024, ' ');
      key = buff;
      eatwhite(from);
      from.getline(buff, 1024, ';');
      name = buff;

      eatwhite(from);
      from.getline(buff, 1024, '\n');

      if(key!="")
      {
         vr[key]=name;
      }
   }
   from.close();
}

/**
 * \brief Destructor
 */
gdcmVR::~gdcmVR() {
   vr.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void gdcmVR::Print(std::ostream &os) 
{
   std::ostringstream s;

   for (gdcmVRHT::iterator it = vr.begin(); it != vr.end(); ++it)
   {
      s << "VR : "<<it->first<<" = "<<it->second<<std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Get the count for an element
 * @param   key key to count
 */
int gdcmVR::Count(gdcmVRKey key) 
{
   return vr.count(key);
}

/**
 * \brief   Simple predicate that checks wether the given argument
 *          corresponds to the Value Representation of a \ref gdcmBinEntry .
 *          This predicate is the negation of
 *          \ref gdcmVR::IsVROfGdcmStringRepresentable .
 * @param   tested value representation to check for.
 */
bool gdcmVR::IsVROfGdcmBinaryRepresentable(gdcmVRKey tested)
{
   //std::cout << "gdcmVR::IsVROfGdcmBinaryRepresentable===================="
   //   << tested << std::endl;

   if ( tested == "unkn")
      return true;

   if ( ! Count(tested) )
   {
      dbg.Verbose(0, "gdcmVR::IsVROfGdcmBinaryRepresentable: tested not a VR!");
      return false;
   }

   if ( IsVROfGdcmStringRepresentable(tested) )
   {
      dbg.Verbose(0, "gdcmVR::IsVROfGdcmBinaryRepresentable: binary VR !");
      return false;
   }

   return true;
}

/**
 * \brief   Simple predicate that checks wether the given argument
 *          corresponds to the Value Representation of a \ref gdcmValEntry
 *          but NOT a \ref gdcmBinEntry.
 * @param   tested value representation to check for.
 */
bool gdcmVR::IsVROfGdcmStringRepresentable(gdcmVRKey tested)
{

   if ( ! Count(tested) )
   {
      dbg.Verbose(0, "gdcmVR::IsVROfGdcmStringRepresentable: tested not a VR!");
      return false;
   }

   if (tested == "AE" || tested == "AS" || tested == "DA" || tested == "PN" ||
       tested == "UI" || tested == "TM" || tested == "SH" || tested == "LO" ||
       tested == "CS" || tested == "IS" || tested == "LO" || tested == "LT" ||
       tested == "SH" || tested == "ST" || tested == "DS" || tested == "SL" ||
       tested == "SS" || tested == "UL" || tested == "US" || tested == "UN")
   {
      return true;
   }
   return false;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
