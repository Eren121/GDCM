/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllVM.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/21 14:42:12 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmUtil.h"
#include "gdcmCommon.h"
#include "gdcmDocEntry.h" 
#include "gdcmDocEntrySet.h"           
#include "gdcmDocument.h"          
#include "gdcmElementSet.h"        
#include "gdcmSeqEntry.h" 
#include "gdcmSQItem.h" 

//Generated file:
#include "gdcmDataImages.h"

int TestAllVM(int, char *[])
{
   int i = 0;

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = gdcmDataImages[i];

      gdcm::File file;
      //file.SetLoadMode( gdcm::LD_NOSEQ );
      file.SetFileName( filename );
      file.Load();

      gdcm::DocEntry *d = file.GetFirstEntry();
      while(d)
      {
         if ( gdcm::DataEntry *de = dynamic_cast<gdcm::DataEntry *>(d) )
         {
           if(! de->IsValueCountValid() )
             std::cerr << "Filename:" << filename << std::endl;
         }
         else
         {
          // We skip pb of SQ recursive exploration
         }

         d = file->GetNextEntry();
      }

      i++;
   }
   return 0;
}

