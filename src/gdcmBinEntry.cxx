/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmBinEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/14 17:36:23 $
  Version:   $Revision: 1.50 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmBinEntry.h"
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
BinEntry::BinEntry(DictEntry *e) : ValEntry(e)
{
   BinArea = 0;
   SelfArea = true;
}

/**
 * \brief   Constructor from a given BinEntry
 * @param   e Pointer to existing Doc entry
 */
BinEntry::BinEntry(DocEntry *e) : ValEntry(e->GetDictEntry())
{
   Copy(e);
/*   Length     = e->GetLength();
   ReadLength = e->GetReadLength();
   ImplicitVR = e->IsImplicitVR();
   Offset     = e->GetOffset();*/

   //FIXME
   //SQDepthLevel = e->GetDepthLevel();

   BinArea = 0; // let's be carefull !
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
// Print
/*
 * \brief   canonical Printer
 */
 
void BinEntry::Print(std::ostream &os)
{
   os << "B ";
   DocEntry::Print(os);
   std::ostringstream s;
   void* binArea = GetBinArea();
   if (binArea)
   {
      //s << " [" << GDCM_BINLOADED 
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
         //s << " [gdcm::Binary data NOT loaded]";
         s << " [" <<GetValue() << "]";
      }
         
   }
   os << s.str();
}

/*
 * \brief   canonical Writer
 * @param fp already open file pointer
 * @param filetype type of the file to be written
*/
void BinEntry::WriteContent(std::ofstream *fp, FileType filetype)
{ 
#define BUFFER_SIZE 4096
   DocEntry::WriteContent(fp, filetype);
   void* binArea = GetBinArea();
   int lgr = GetLength();
   if (binArea) // the binArea was *actually* loaded
   {

   // TODO FIME
   // Probabely, the same operation will have to be done when we want 
   // to write image with Big Endian Transfert Syntax, 
   //   and we are working onj Little Endian Processor

#ifdef GDCM_WORDS_BIGENDIAN
      // Be carefull with *any* 16 bits words 'binEntries !'
      // if ( GetVR() == "OW") // to be used later

      // TODO FIXME Right now, we only care of Pixels element

      // 8 Bits Pixels *are* OB, 16 Bits Pixels *are* OW
      // -value forced while Reading process-
      if (GetGroup() == 0x7fe0 && GetVR() == "OW")
      {     
         uint16_t *currPosition = (uint16_t *)binArea;

         // TODO FIXME : Maybe we should allocate somewhere a static buffer,
         // in order not to have to alloc/delete for almost every BinEntry ...
         uint16_t *buffer = new uint16_t[BUFFER_SIZE];

         // how many BUFFER_SIZE long pieces in binArea ?
         int nbPieces = lgr/BUFFER_SIZE/2; //(16 bits = 2 Bytes)
         for (int j=0;j<nbPieces;j++)
         {
            for (int i = 0; i < BUFFER_SIZE/2; i++)
            {
               buffer[i] =   (uint16_t *)binArea[i] >> 8 
                           | (uint16_t *)binArea[i] << 8;  
            }
            fp->write ( (char*)currPosition, BUFFER_SIZE );
            currPosition += BUFFER_SIZE/2;
         }
         int remainingSize = lgr%BUFFER_SIZE;
         if ( remainingSize != 0)
         {
            fp->write ( (char*)currPosition, remainingSize );   
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
//-----------------------------------------------------------------------------
// Public


/// \brief Sets the value (non string) of the current Dicom Header Entry
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
} // end namespace gdcm
