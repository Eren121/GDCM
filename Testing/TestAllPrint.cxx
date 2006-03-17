/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllPrint.cxx,v $
  Language:  C++
  Date:      $Date: 2006/03/17 14:33:54 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

// TODO : check what's *actually* usefull

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

#include <iostream>

//Generated file:
#include "gdcmDataImages.h"

int TestAllPrint(int, char *[])
{
   int i = 0;

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = GDCM_DATA_ROOT;
      filename += "/";
      filename += gdcmDataImages[i];

      gdcm::File file;
      //file.SetLoadMode( gdcm::LD_NOSEQ | gdcm::LD_NOSHADOWSEQ );
      file.SetFileName( filename );
      if( !file.Load() )
        return 1;
      //file.Print( std::cout ); //just for debug
      i++;
   }
   return 0;
}
