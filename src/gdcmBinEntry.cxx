/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/10 14:23:18 $
  Version:   $Revision: 1.68 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmBinEntry.h"
#include "gdcmContentEntry.h"

#include "gdcmDebug.h"

#include <fstream>
#include <sstream>
#include <iostream> // for std::ios_base, since <ios> does not exist on gcc/Solaris

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor from a given BinEntry
 */
BinEntry::BinEntry(DictEntry *e) 
         :ContentEntry(e)
{
   BinArea = 0;
   SelfArea = true;
}

/**
 * \brief   Constructor from a given BinEntry
 * @param   e Pointer to existing Doc entry
 */
BinEntry::BinEntry(DocEntry *e) 
        : ContentEntry(e->GetDictEntry())
{
   Copy(e);

   BinArea = 0;
   SelfArea = true;
}

/**
 * \brief   Canonical destructor.
 */
BinEntry::~BinEntry()
{
   if (BinArea && SelfArea)
   {
      delete[] BinArea;
      BinArea = 0; // let's be carefull !
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   canonical Writer
 * @param fp already open file pointer
 * @param filetype type of the file (ACR, ImplicitVR, ExplicitVR, ...)
*/
void BinEntry::WriteContent(std::ofstream *fp, FileType filetype)
{ 
   DocEntry::WriteContent(fp, filetype);
   void* binArea = GetBinArea();
   int lgr = GetLength();
   if (binArea) // the binArea was *actually* loaded
   {

   // TODO FIME
   // Probabely, the same operation will have to be done when we want 
   // to write image with Big Endian Transfert Syntax, 
   //   and we are working on Little Endian Processor

#ifdef GDCM_WORDS_BIGENDIAN
      const int BUFFER_SIZE = 4096;
      // TODO FIXME Right now, we only care of Pixels element

      // 8 Bits Pixels *are* OB, 16 Bits Pixels *are* OW
      // -value forced while Reading process-
      if (GetGroup() == 0x7fe0 && GetVR() == "OW")
      {     
         uint16_t *buffer = new uint16_t[BUFFER_SIZE/2];

         // how many BUFFER_SIZE long pieces in binArea ?
         int nbPieces = lgr/BUFFER_SIZE; //(16 bits = 2 Bytes)
         int remainingSize = lgr%BUFFER_SIZE;

         uint16_t *binArea16 = (uint16_t*)binArea;
         for (int j=0;j<nbPieces;j++)
         {
            for (int i = 0; i < BUFFER_SIZE/2; i++)
            {
               //buffer[i] =  (binArea16[i] >> 8) | (binArea16[i] << 8);
               uint16_t val = binArea16[i];
               buffer[i] = ((( val << 8 ) & 0xff00 ) | (( val >> 8 ) & 0x00ff ) );
            }
            fp->write ( (char*)buffer, BUFFER_SIZE );
            binArea16 += BUFFER_SIZE/2;
         }
         if ( remainingSize > 0)
         {
            for (int i = 0; i < remainingSize/2; i++)
            {
               //buffer[i] =  (binArea16[i] >> 8) | (binArea16[i] << 8);
               uint16_t val = binArea16[i];
               buffer[i] = ((( val << 8 ) & 0xff00 ) | (( val >> 8 ) & 0x00ff ) );
            }
            fp->write ( (char*)buffer, remainingSize );
         } 
         delete[] buffer;
      }
      else
      { 
         // For any other VR, BinEntry is re-written as-is
         fp->write ( (char*)binArea, lgr );
      }
#else
      fp->write ( (char*)binArea, lgr ); // Elem value
#endif //GDCM_WORDS_BIGENDIAN

   }
   else
   {
      // nothing was loaded, but we need to skip space on disc
      fp->seekp(lgr, std::ios::cur);
   }
}

/**
 * \brief Sets the value (non string) of the current Dicom Header Entry
 */
void BinEntry::SetBinArea( uint8_t *area, bool self )  
{ 
   if (BinArea && SelfArea)
      delete[] BinArea;

   BinArea = area;
   SelfArea=self;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints a BinEntry (Dicom entry)
 * @param   os ostream we want to print in
 * @param indent Indentation string to be prepended during printing
 */
void BinEntry::Print(std::ostream &os, std::string const & )
{
   os << "B ";
   DocEntry::Print(os);
   std::ostringstream s;
   void* binArea = GetBinArea();
   if (binArea)
   {
      s << " [" << GetValue()
        << "; length = " << GetLength() << "]";
   }
   else
   {
      if ( GetLength() == 0 )
      {
         s << " []";
      }
      else 
      {
         s << " [" <<GetValue() << "]";
      }         
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
