/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntryArchive.h,v $
  Language:  C++
  Date:      $Date: 2004/11/19 18:49:39 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDOCENTRYARCHIVE_H
#define GDCMDOCENTRYARCHIVE_H

#include "gdcmCommon.h"
#include "gdcmHeader.h"

namespace gdcm 
{

//-----------------------------------------------------------------------------
/*
 * /brief Container 
 *
 * It's goal is to change the Header correctly. At this time, the change is 
 * only made for the first level of the Document. In the future, it might 
 * consider sequences.
 * The change is made by replacing a DocEntry by an other that is created
 * outside the class. The old value is kept. When we restore the header
 * status, the added DocEntry is deleted and replaced by the old value.
 */
class GDCM_EXPORT DocEntryArchive 
{
public:
   DocEntryArchive(Header *header);
   ~DocEntryArchive();

   void Print(std::ostream &os = std::cout);

   bool Push(DocEntry *newEntry);
   bool Restore(uint16_t group,uint16_t element);

   void ClearArchive(void);

private:
   TagDocEntryHT &HeaderHT;
   TagDocEntryHT Archive;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
