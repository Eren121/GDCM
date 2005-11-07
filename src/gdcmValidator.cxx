/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValidator.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/07 11:42:25 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmValidator.h"
#include "gdcmElementSet.h"
#include "gdcmDataEntry.h"
#include "gdcmUtil.h"
#include <map>

#include <sstream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
typedef std::map<uint16_t, int> GroupHT;    //  Hash Table
//-----------------------------------------------------------------------------

Validator::Validator()
{
}

Validator::~Validator()
{
}

// Function to compare the VM found while parsing d->GetString()
// compare to the one from the dictionary
bool CheckVM(DataEntry *entry)
{
  // Don't waste time checking tags where VM is OB and OW, since we know
  // it's allways 1, whatever the actual length (found on disc)
  
  if ( entry->GetVR() == "OB" ||  entry->GetVR() == "OW" )
     return true;
     
  const std::string &s = entry->GetString();

  unsigned int n = Util::CountSubstring( s , "\\");
  
  n++; // number of '\' + 1 == Value Multiplicity

  std::string vmFromDict = entry->GetVM();
  if ( vmFromDict == "1-n" || vmFromDict == "2-n" || vmFromDict == "3-n" )
     return true;
     
  unsigned int m;
  std::istringstream is;
  is.str( vmFromDict );
  is >> m;

  return n == m;
}

void Validator::SetInput(ElementSet *input)
{
/*
// First stage to check group length
  GroupHT grHT;
  DocEntry *d=input->GetFirstEntry();
  while(d)
  {
    grHT[d->GetGroup()] = 0;
    d=input->GetNextEntry();
  }
  for (GroupHT::iterator it = grHT.begin(); it != grHT.end(); ++it)  
  {
      std::cout << std::hex << it->first << std::endl; 
  } 
*/

  // berk for now SetInput do two things at the same time
  d=input->GetFirstEntry();
  if (!d)
  {
     std::cout << "No Entry found" << std::endl;
     return;
  }
  while(d)
  { 
    if ( DataEntry *v = dynamic_cast<DataEntry *>(d) )
    { 
      if ( v->GetVM() != gdcm::GDCM_UNKNOWN )
         if ( !CheckVM(v) )
         {
           std::cout << "Tag (" <<  v->GetKey() 
                     << ")-> [" << v->GetName() << "] contains an illegal VM. "
                     << "value [" << v->GetString() << "] VR :"
                     << v->GetVR() << ", Expected VM :" << v->GetVM() << " " 
                     << std::endl;
         }
      
      if ( v->GetReadLength() % 2 )
      {
        std::cout << "Tag (" <<  v->GetKey() 
                  << ")-> [" << v->GetName() << "] has an uneven length :"
                  << v->GetReadLength()
                  << " [" << v->GetString() << "] " 
                  << std::endl;         
      }
    }
    else
    {
      // We skip pb of SQ recursive exploration
    }
    d=input->GetNextEntry();
  }
}

} // end namespace gdcm
