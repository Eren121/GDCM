/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/12 04:35:48 $
  Version:   $Revision: 1.26 $
                                                                                
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
   std::string filename=DictSet::BuildDictPath() + std::string(DICT_TS);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "TS::TS: can't open dictionary",filename.c_str());

   std::string key;
   std::string name;

   while (!from.eof())
   {
      from >> key;

      from >> std::ws; // used to be eatwhite(from);
      std::getline(from, name);    /// MEMORY LEAK

      if(key!="")
      {
         ts[key]=name;
      }
   }
   from.close();
}

//-----------------------------------------------------------------------------
TS::~TS() 
{
   ts.clear();
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

   for (TSHT::iterator it = ts.begin(); it != ts.end(); ++it)
   {
      s << "TS : "<<it->first<<" = "<<it->second<<std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
int TS::Count(TSKey key) 
{
   return ts.count(key);
}

std::string TS::GetValue(TSKey key) 
{
   if (ts.count(key) == 0)
   {
      return GDCM_UNFOUND;
   }
   return ts[key];
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
