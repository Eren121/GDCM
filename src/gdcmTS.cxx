/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/27 01:32:15 $
  Version:   $Revision: 1.27 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmTS.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmDictSet.h"

#include <fstream>
#include <string>
#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
TS::TS() 
{
   std::string filename = DictSet::BuildDictPath() + DICT_TS;
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "TS::TS: can't open dictionary", filename.c_str());

   TSKey key;
   TSAtr name;

   while (!from.eof())
   {
      from >> key;
      from >> std::ws;
      std::getline(from, name);

      if(key != "")
      {
         TsMap[key] = name;
      }
   }
   from.close();
}

//-----------------------------------------------------------------------------
TS::~TS() 
{
   TsMap.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup VR
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void TS::Print(std::ostream &os) 
{
   std::ostringstream s;

   for (TSHT::const_iterator it = TsMap.begin(); it != TsMap.end(); ++it)
   {
      s << "TS : " << it->first << " = " << it->second << std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
int TS::Count(TSKey const & key) 
{
   return TsMap.count(key);
}

TSAtr const & TS::GetValue(TSKey const & key) 
{
   TSHT::const_iterator it = TsMap.find(key);
   if (it == TsMap.end())
   {
      return GDCM_UNFOUND;
   }
   return it->second;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
