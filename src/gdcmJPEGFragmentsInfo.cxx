/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragmentsInfo.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/17 03:05:55 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmJPEGFragmentsInfo.h"
#include <fstream>

namespace gdcm 
{

/**
 * \brief Default destructor
 */
JPEGFragmentsInfo::~JPEGFragmentsInfo()
{
   for(JPEGFragmentsList::iterator it  = Fragments.begin();
                                   it != Fragments.end();
                                 ++it )
   {
      delete *it;
   }
   Fragments.clear();
}

/**
 * \brief        Print self.
 * @param indent Indentation string to be prepended during printing.
 * @param os     Stream to print to.
 */
void JPEGFragmentsInfo::Print( std::ostream &os, std::string const & indent )
{
   os << indent
      << "----------------- JPEG fragments --------------------------------"
      << std::endl;
   os << indent
      << "Total number of fragments : " << Fragments.size()
      << std::endl;
   int fragmentNumber = 0;
   for(JPEGFragmentsList::iterator it  = Fragments.begin();
                                   it != Fragments.end();
                                 ++it)
   {
      os << indent
         << "   fragment number :" << fragmentNumber++;
      (*it)->Print( os, indent + "   ");
      os << std::endl;
   }
}

/**
 * \brief  Calculate sum of all fragments lenght and return total
 * @return Total size of JPEG fragments length
 */
size_t JPEGFragmentsInfo::GetFragmentsLength()
{
   // Loop on the fragment[s] to get total length
   size_t totalLength = 0;
   JPEGFragmentsList::const_iterator it;
   for( it  = Fragments.begin();
        it != Fragments.end();
        ++it )
   {
      totalLength += (*it)->Length;
   }
   return totalLength;
}

/**
 * \brief Read the all the JPEG Fragment into the input buffer
 */
void JPEGFragmentsInfo::ReadAllFragments(std::ifstream *fp, JOCTET *buffer )
{
   JOCTET *p = buffer;

   // Loop on the fragment[s]
   JPEGFragmentsList::const_iterator it;
   for( it  = Fragments.begin();
        it != Fragments.end();
        ++it )
   {
      fp->seekg( (*it)->Offset, std::ios::beg);
      size_t len = (*it)->Length;
      fp->read((char *)p,len);
      p += len;
   }

}

} // end namespace gdcm

