/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/21 04:43:02 $
  Version:   $Revision: 1.19 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <fstream>
#include <string>
#include <iostream>

#include "gdcmTS.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmDictSet.h"



//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmTS::gdcmTS(void) 
{
   std::string filename=gdcmDictSet::BuildDictPath() + std::string(DICT_TS);
   std::ifstream from(filename.c_str());
   dbg.Error(!from, "gdcmTS::gdcmTS: can't open dictionary",filename.c_str());

   //char buff[1024];  //not used
   std::string key;
   std::string name;

   while (!from.eof()) {
      from >> key;
      eatwhite(from);
      getline(from, name);    /// MEMORY LEAK

      if(key!="") 
      {
         ts[key]=name;
      }
   }
   from.close();
}

gdcmTS::~gdcmTS() 
{
   ts.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmVR
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void gdcmTS::Print(std::ostream &os) 
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
int gdcmTS::Count(TSKey key) 
{
   return ts.count(key);
}

std::string gdcmTS::GetValue(TSKey key) 
{
   if (ts.count(key) == 0) 
      return (GDCM_UNFOUND);
   return ts[key];
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
