/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRLEFrame.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/26 11:42:02 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
                                                                                
#include "gdcmRLEFrame.h"
#include "gdcmDebug.h"
                                                                                
namespace gdcm
{

/**
 * \brief        Print self.
 * @param indent Indentation string to be prepended during printing.
 * @param os     Stream to print to.
 */
void RLEFrame::Print( std::ostream &os, std::string indent )
{
   os << indent
      << "--- fragments"
      << std::endl;
   for ( unsigned int i = 0; i < NumberFragments; i++ )
   {
      os << indent
         << "   offset : " <<  Offset[i]
         << "   length : " <<  Length[i]
         << std::endl;
   }
}

void RLEFrame::SetOffset(unsigned int id,long offset)
{
   gdcmAssertMacro(id<15);
   Offset[id] = offset;
}

long RLEFrame::GetOffset(unsigned int id)
{
   gdcmAssertMacro(id<15);
   return Offset[id];
}

void RLEFrame::SetLength(unsigned int id,long length)
{
   gdcmAssertMacro(id<15);
   Length[id] = length;
}

long RLEFrame::GetLength(unsigned int id)
{
   gdcmAssertMacro(id<15);
   return Length[id];
}

} // end namespace gdcm

