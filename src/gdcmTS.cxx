/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/07 22:03:30 $
  Version:   $Revision: 1.33 $
                                                                                
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

// TODO
// a lot of troubles expected with TS : 1.2.840.113619.5.2
// Implicit VR - Big Endian
// see : http://www.gemedicalsystemseurope.com/euen/it_solutions/pdf/lsqxi_rev2.pdf
// 

namespace gdcm 
{
void FillDefaultTSDict(TSHT &ts);
//-----------------------------------------------------------------------------
// Constructor / Destructor
TS::TS() 
{
   std::string filename = DictSet::BuildDictPath() + DICT_TS;
   std::ifstream from(filename.c_str());
   if( !from )
   {
      gdcmVerboseMacro("Can't open dictionary" << filename.c_str());
      FillDefaultTSDict( TsMap );
   }
   else
   {
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
int TS::Count(TSKey const &key) 
{
   return TsMap.count(key);
}

TSAtr const & TS::GetValue(TSKey const &key) 
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
