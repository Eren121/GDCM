/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmJPEGFragmentsInfo.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/26 16:28:58 $
  Version:   $Revision: 1.11 $
                                                                                
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

JPEGFragmentsInfo::JPEGFragmentsInfo()
{
   StateSuspension = 0;
}

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
 * @param os     Stream to print to.
 * @param indent Indentation string to be prepended during printing.
 */
void JPEGFragmentsInfo::Print( std::ostream &os, std::string const &indent )
{
   os << std::endl;
   os << indent
      << "----------------- JPEG fragments --------------------------------"
      << std::endl << std::endl;
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
   }
   os << std::endl;
}

/**
 * \brief  Calculate sum of all fragments length and return total
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
      totalLength += (*it)->GetLength();
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
      fp->seekg( (*it)->GetOffset(), std::ios::beg);
      size_t len = (*it)->GetLength();
      fp->read((char *)p,len);
      p += len;
   }

}

void JPEGFragmentsInfo::DecompressJPEGFramesFromFile(std::ifstream *fp, uint8_t *buffer, int nBits, int numBytes, int length)
{
   // Pointer to the Raw image
   uint8_t *localRaw = buffer;

  // Loop on the fragment[s]
   JPEGFragmentsList::const_iterator it;
   for( it  = Fragments.begin();
        it != Fragments.end();
        ++it )
   {
     //(*it)->pimage = localRaw;
     (*it)->DecompressJPEGFramesFromFile(fp, localRaw, nBits, StateSuspension);
     // update pointer to image after some scanlines read:
     localRaw = (*it)->GetImage();
      // Advance to next free location in Raw 
      // for next fragment decompression (if any)

      //localRaw += length * numBytes;
     //std::cerr << "Used to increment by: " << length * numBytes << std::endl;
   }
}

void JPEGFragmentsInfo::AddFragment(JPEGFragment *fragment)
{
   Fragments.push_back(fragment);
}

} // end namespace gdcm

