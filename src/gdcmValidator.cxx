/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValidator.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/28 15:52:15 $
  Version:   $Revision: 1.6 $
                                                                                
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

#include <sstream>

namespace gdcm 
{

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
  std::string::size_type n = s.find("\\");
  if ( n == s.npos ) // none found
  {
    n = 0;
  }
  n++; // number of '\' + 1 == Value Multiplicity

  unsigned int m;
  std::istringstream os;
  os.str( entry->GetVM());
  os >> m;

  return n == m;
}

void Validator::SetInput(ElementSet *input)
{
  // berk for now SetInput do two things at the same time
  DocEntry *d=input->GetFirstEntry();
  while(d)
  {
    if ( DataEntry *v = dynamic_cast<DataEntry *>(d) )
    {   
      if ( !CheckVM(v) )
      {
        std::cout << "Rah this DICOM contains one wrong tag:" << 
        v->GetString() << " " <<
        v->GetGroup() << "," << v->GetElement() << "," <<
        v->GetVR() << " " << v->GetVM() << " " << v->GetName() << std::endl;
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
