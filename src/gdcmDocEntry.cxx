/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/18 08:01:41 $
  Version:   $Revision: 1.42 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDocEntry.h"
#include "gdcmTS.h"
#include "gdcmVR.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <iomanip> // for std::ios::left, ...
#include <fstream>

namespace gdcm 
{

// CLEAN ME
#define MAX_SIZE_PRINT_ELEMENT_VALUE 64

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor from a given DictEntry
 * @param   in Pointer to existing dictionary entry
 */
DocEntry::DocEntry(DictEntry *in)
{
   ImplicitVR = false;
   DicomDict  = in;
   SetKey( in->GetKey( ) );
   Offset     = 0 ; // To avoid further missprinting

   // init some variables
   ReadLength = 0;
   Length = 0;
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints the common part of ValEntry, BinEntry, SeqEntry
 * @param   os ostream we want to print in
 */
void DocEntry::Print(std::ostream &os, std::string const & )
{
   size_t o;
   std::string st;
   TSKey v;
   std::string d2, vr;
   std::ostringstream s;
   uint32_t lgth;

   o  = GetOffset();
   vr = GetVR();
   if(vr==GDCM_UNKNOWN)
      vr="  ";

   s << DictEntry::TranslateToKey(GetGroup(),GetElement()); 

   if (PrintLevel >= 2)
   {
      s << " lg : ";
      lgth = GetReadLength(); // ReadLength, as opposed to Length
      if (lgth == 0xffffffff)
      {
         st = Util::Format("x(ffff)");  // I said : "x(ffff)" !
         s.setf(std::ios::left);
         s << std::setw(10-st.size()) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << "-1"; 
      }
      else
      {
         st = Util::Format("x(%x)",lgth);
         s.setf(std::ios::left);
         s << std::setw(10-st.size()) << " ";
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << lgth; 
      }
      s << " Off.: ";
      st = Util::Format("x(%x)",o); 
      s << std::setw(10-st.size()) << " ";
      s << st << " ";
      s << std::setw(8) << o; 
   }

   s << "[" << vr  << "] ";

   if (PrintLevel >= 1)
   {
      s.setf(std::ios::left);
      s << std::setw(66-GetName().length()) << " ";
   }
    
   s << "[" << GetName()<< "]";
   os << s.str();      
}

/**
 * \brief   Writes the common part of any ValEntry, BinEntry, SeqEntry
 * @param fp already open file pointer
 * @param filetype type of the file to be written
 */
void DocEntry::WriteContent(std::ofstream *fp, FileType filetype)
{
   uint32_t ffff  = 0xffffffff;
   uint16_t group = GetGroup();
   VRKey vr   = GetVR();
   uint16_t el    = GetElement();
   uint32_t lgr   = GetLength();

   if ( group == 0xfffe && el == 0x0000 )
   {
     // Fix in order to make some MR PHILIPS images e-film readable
     // see gdcmData/gdcm-MR-PHILIPS-16-Multi-Seq.dcm:
     // we just *always* ignore spurious fffe|0000 tag !   
      return;
   }

   //
   // ----------- Writes the common part
   //
   binary_write( *fp, group);  //group
   binary_write( *fp, el);  //element

   if ( filetype == ExplicitVR )
   {
      // Special case of delimiters:
      if (group == 0xfffe)
      {
         // Delimiters have NO Value Representation
         // Hence we skip writing the VR.
         // In order to avoid further troubles, we choose to write them
         // as 'no-length' Item Delimitors (we pad by writing 0xffffffff)
         // The end of a given Item will be found when  :
         //  - a new Item Delimitor Item is encountered (the Seq goes on)
         //  - a Sequence Delimitor Item is encountered (the Seq just ended)

         // TODO : verify if the Sequence Delimitor Item was forced during Parsing 

         uint32_t ff = 0xffffffff;
         binary_write(*fp, ff);
         return;
      }

      uint16_t z = 0;
      uint16_t shortLgr = lgr;

      if (vr == GDCM_UNKNOWN)
      {
         // Unknown was 'written'
         // deal with Little Endian            
         binary_write(*fp, shortLgr);
         binary_write(*fp, z);
      }
      else
      {
         binary_write(*fp, vr);
         gdcmAssertMacro( vr.size() == 2 );
                  
         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") || (vr == "UN") )
         {
            binary_write(*fp, z);
            if (vr == "SQ")
            {
               // we set SQ length to ffffffff
               // and  we shall write a Sequence Delimitor Item 
               // at the end of the Sequence! 
               binary_write(*fp, ffff);
            }
            else
            {
               binary_write(*fp, lgr);
            }
         }
         else
         {
            binary_write(*fp, shortLgr);
         }
      }
   } 
   else // IMPLICIT VR 
   { 
      if (vr == "SQ")
      {
         binary_write(*fp, ffff);
      }
      else
      {
         binary_write(*fp, lgr);
      }
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \brief   Gets the full length of the elementary DocEntry (not only value
 *          length) depending on the VR.
 */
uint32_t DocEntry::GetFullLength()
{
   uint32_t l = GetReadLength();
   if ( IsImplicitVR() )
   {
      l = l + 8;  // 2 (gr) + 2 (el) + 4 (lgth) 
   }
   else
   {
      if ( GetVR()=="OB" || GetVR()=="OW" || GetVR()=="SQ" )
      {
         l = l + 12; // 2 (gr) + 2 (el) + 2 (vr) + 2 (unused) + 4 (lgth)
      }
      else
      {
         l = l + 8;  // 2 (gr) + 2 (el) + 2 (vr) + 2 (lgth)
      }
   }
   return l;
}

/**
 * \brief   Copies all the attributes from an other DocEntry 
 * @param e entry to copy from
 */
void DocEntry::Copy (DocEntry *e)
{
   Length     = e->Length;
   ReadLength = e->ReadLength;
   ImplicitVR = e->ImplicitVR;
   Offset     = e->Offset;
}

/**
 * \brief   tells us if entry is the last one of a 'no length' SequenceItem 
 *          (fffe,e00d) 
 */
bool DocEntry::IsItemDelimitor()
{
   return (GetGroup() == 0xfffe && GetElement() == 0xe00d);
}
/**
 * \brief   tells us if entry is the last one of a 'no length' Sequence 
 *          (fffe,e0dd) 
 */
bool DocEntry::IsSequenceDelimitor()
{
   return (GetGroup() == 0xfffe && GetElement() == 0xe0dd);
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
