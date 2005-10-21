/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllVM.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/21 15:16:22 $
  Version:   $Revision: 1.5 $
                                                                                
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

int DoTheVMTest(std::string const &filename)
{
      gdcm::File file;
      // Do not test unknow VM ...
      file.SetLoadMode( gdcm::LD_NOSHADOW | gdcm::LD_NOSHADOWSEQ );
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
      }

      return 0;
}

int TestAllVM(int argc, char *argv[])
{
   int i = 0;
   if( argc >= 2 )
     {
     const char *filename = argv[1];
     if( DoTheVMTest( filename ) )
       return 1;
     return 0;
     }
   // else

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";
      filename += gdcmDataImages[i];

      if( DoTheVMTest( filename ) )
        return 1;
      i++;
      std::cerr << std::endl; // skip a line after each file
   }

   return 0;
}

