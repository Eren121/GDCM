/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestAllPrint.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/30 14:40:30 $
  Version:   $Revision: 1.3 $
                                                                                
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
#include "gdcmBinEntry.h"  
#include "gdcmDocEntry.h" 
#include "gdcmDocEntrySet.h"           
#include "gdcmDocument.h"          
#include "gdcmElementSet.h"        
#include "gdcmSeqEntry.h" 
#include "gdcmSQItem.h" 
#include "gdcmValEntry.h" 

#include <iostream>

//Generated file:
#include "gdcmDataImages.h"

int TestAllPrint(int, char *[])
{
   int i = 0;

   while( gdcmDataImages[i] != 0 )
   {
      std::string filename = gdcmDataImages[i];

      gdcm::File file;
      file.SetLoadMode( GDCM_LD_NOSEQ );
      file.SetFileName( filename );
      file.Load();
      //file.Print( std::cout ); //just for debug
      i++;
   }
   return 0;
}
