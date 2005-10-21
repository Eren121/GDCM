/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllVM.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/21 15:01:28 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmFile.h"
#include "gdcmDataEntry.h"

//Generated file:
#include "gdcmDataImages.h"

int TestAllVM(int, char *[])
{
   int i = 0;

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";
      filename += gdcmDataImages[i];

      gdcm::File file;
      file.SetLoadMode( gdcm::LD_NOSHADOW );
      file.SetFileName( filename );
      if( !file.Load() ) //would be really bad...
        return 1;

      gdcm::DocEntry *d = file.GetFirstEntry();
      std::cerr << "Testing file : " << filename << std::endl;
      while(d)
      {
         if ( gdcm::DataEntry *de = dynamic_cast<gdcm::DataEntry *>(d) )
         {
           if( !de->IsValueCountValid() )
             {
             std::cerr << "Element: " << de->GetKey() <<
               " (" << de->GetName() << ") " <<
               "Contains a wrong VM: " << de->GetValueCount() 
               << " should be: " << de->GetVM() << std::endl;;
             }
         }
         else
         {
          // We skip pb of SQ recursive exploration
         }

         d = file.GetNextEntry();
         std::cerr << std::endl; // skip a line after each file
      }

      i++;
   }
   return 0;
}

