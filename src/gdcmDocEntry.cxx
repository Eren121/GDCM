/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/01 03:20:23 $
  Version:   $Revision: 1.15 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDocEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include <iomanip> // for std::ios::left, ...

// CLEAN ME
#define MAX_SIZE_PRINT_ELEMENT_VALUE 64

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntry
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmDocEntry::gdcmDocEntry(gdcmDictEntry* in)
{
   ImplicitVR = false;
   DictEntry = in;
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDocEntry
 * \brief   Prints the common part of gdcmValEntry, gdcmBinEntry, gdcmSeqEntry
 * @param   os ostream we want to print in
 */
void gdcmDocEntry::Print(std::ostream & os)
{
   PrintLevel = 2; // FIXME
   
   size_t o;
   unsigned short int g, e;
   char st[20];
   TSKey v;
   std::string d2, vr;
   std::ostringstream s;
   uint32_t lgth;
   char greltag[10];  //group element tag

   g  = GetGroup();
   e  = GetElement();
   o  = GetOffset();
   vr = GetVR();
   sprintf(greltag,"%04x|%04x ",g,e);           
   s << greltag ;
       
   if (PrintLevel >= 2)
   {
      s << "lg : ";
      lgth = GetReadLength(); // ReadLength, as opposed to UsableLength
      if (lgth == 0xffffffff)
      {
         sprintf(st,"x(ffff)");  // I said : "x(ffff)" !
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << "-1";      
      }
      else
      {
         sprintf(st,"x(%x)",lgth);
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << lgth; 
      }
      s << " Off.: ";
      sprintf(st,"x(%x)",o); 
      s << std::setw(10-strlen(st)) << " ";
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
 * \ingroup gdcmDocEntry
 * \brief   Writes the common part of any gdcmValEntry, gdcmBinEntry, gdcmSeqEntry
 */
void gdcmDocEntry::Write(FILE *fp, FileType filetype)
{
   uint32_t FFFF  = 0xffffffff;
   uint16_t group = GetGroup();
   gdcmVRKey vr   = GetVR();
   uint16_t el    = GetElement();
   uint32_t lgr   = GetReadLength();

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
   fwrite ( &group,(size_t)2 ,(size_t)1 ,fp);  //group
   fwrite ( &el,   (size_t)2 ,(size_t)1 ,fp);  //element
      
   if ( filetype == gdcmExplicitVR )
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

         int ff = 0xffffffff;
         fwrite (&ff,(size_t)4 ,(size_t)1 ,fp);
         return;
      }

      uint16_t z = 0;
      uint16_t shortLgr = lgr;

      if (vr == "unkn")
      {
         // Unknown was 'written'
         // deal with Little Endian            
         fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,fp);
         fwrite ( &z,  (size_t)2 ,(size_t)1 ,fp);
      }
      else
      {
         fwrite (vr.c_str(),(size_t)2 ,(size_t)1 ,fp); 
                  
         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") || (vr == "UN") )
         {
            fwrite ( &z,  (size_t)2 ,(size_t)1 ,fp);
            if (vr == "SQ")
            {
               // we set SQ length to ffffffff
               // and  we shall write a Sequence Delimitor Item 
               // at the end of the Sequence! 
               fwrite ( &FFFF,(size_t)4 ,(size_t)1 ,fp);
            }
            else
            {
               fwrite ( &lgr,(size_t)4 ,(size_t)1 ,fp);
            }
         }
         else
         {
            fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,fp);
         }
      }
   } 
   else // IMPLICIT VR 
   { 
      if (vr == "SQ")
      {
         fwrite ( &FFFF,(size_t)4 ,(size_t)1 ,fp);
      }
      else
      {
         fwrite ( &lgr,(size_t)4 ,(size_t)1 ,fp);
      }
   }
}

//-----------------------------------------------------------------------------
// Public

/**
 * \ingroup gdcmDocEntry
 * \brief   Gets the full length of the elementary DocEntry (not only value length)
 */
uint32_t gdcmDocEntry::GetFullLength()
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
 * \ingroup gdcmDocEntry
 * \brief   Copies all the attributes from an other DocEntry 
 */
void gdcmDocEntry::Copy (gdcmDocEntry* e)
{
   DictEntry    = e->DictEntry;
   UsableLength = e->UsableLength;
   ReadLength   = e->ReadLength;
   ImplicitVR   = e->ImplicitVR;
   Offset       = e->Offset;
   PrintLevel   = e->PrintLevel;
   // TODO : remove gdcmDocEntry SQDepth
}

/**
 * \ingroup gdcmDocEntry
 * \brief   tells us if entry is the last one of a 'no length' SequenceItem 
 *          (fffe,e00d) 
 */
bool gdcmDocEntry::IsItemDelimitor()
{
   return (GetGroup() == 0xfffe && GetElement() == 0xe00d);
}
/**
 * \ingroup gdcmDocEntry
 * \brief   tells us if entry is the last one of a 'no length' Sequence 
 *          (fffe,e0dd) 
 */
bool gdcmDocEntry::IsSequenceDelimitor()
{
   return (GetGroup() == 0xfffe && GetElement() == 0xe0dd);
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
