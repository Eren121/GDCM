/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: FlatHashTablePrint.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/14 11:28:28 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmHeader.h"
#include "gdcmDocEntry.h"

// Iterate on all the Dicom entries encountered in the gdcmFile (given
// as line argument) and print them. This is an illustration of the
// usage of \ref gdcmDocument::BuildFlatHashTable().

int main(int argc, char* argv[])
{
/*   if (argc < 2)
   {
      std::cerr << "Usage :" << std::endl << 
      argv[0] << " input_dicom " << std::endl;
      return 1;
   }

   gdcm::Header* header = new gdcm::Header( argv[1] );
   gdcm::TagDocEntryHT* Ht = header->BuildFlatHashTable();
   
   for (gdcm::TagDocEntryHT::iterator tag = Ht->begin(); tag != Ht->end(); ++tag)
   {
      tag->second->Print(); 
      std::cout << std::endl;
   }*/
   std::cout<<"Usefull example ???\n";

   return 0;
}
