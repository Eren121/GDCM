/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/22 14:37:04 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmValEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

// CLEAN ME
#define MAX_SIZE_PRINT_ELEMENT_VALUE 128

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor from a given gdcmDictEntry
 * @param   e Pointer to existing dictionary entry
 */
gdcmValEntry::gdcmValEntry(gdcmDictEntry* e) : gdcmDocEntry(e)
{
   voidArea = NULL; // will be in BinEntry ?
}

/**
 * \brief   Constructor from a given gdcmDocEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmValEntry::gdcmValEntry(gdcmDocEntry* e) : gdcmDocEntry(e->GetDictEntry())
{
   this->UsableLength = e->GetLength();
   this->ReadLength   = e->GetReadLength();
   this->ImplicitVR   = e->IsImplicitVR();
   this->Offset       = e->GetOffset();
   this->printLevel   = e->GetPrintLevel();
   this->SQDepthLevel = e->GetDepthLevel();

   this->voidArea = NULL; // will be in BinEntry ?
}


/**
 * \brief   Canonical destructor.
 */
gdcmValEntry::~gdcmValEntry (void)
{
   if (!voidArea)  // will be in BinEntry
      free(voidArea);
}


//-----------------------------------------------------------------------------
// Print
/**
 * \brief   canonical Printer
 */
void gdcmValEntry::Print(std::ostream & os)
{
   std::ostringstream s; 
   unsigned short int g, e;
   char st[20];
   TSKey v;
   std::string d2, vr;
     
   PrintCommonPart(os); 

   g  = GetGroup();
   
   if (g == 0xfffe) {
      s << std::endl;
      os << s.str(); 
      return;  // just to avoid identing all the remaining code     
   }
   
   e  = GetElement();
   vr = GetVR();
   gdcmTS * ts = gdcmGlobal::GetTS();
    
   v  = GetValue();  // not applicable for SQ ...     
   d2 = CreateCleanString(v);  // replace non printable characters by '.'            
   if( (GetLength()<=MAX_SIZE_PRINT_ELEMENT_VALUE) || 
       (printLevel>=3)  || (d2.find("gdcm::NotLoaded.") < d2.length()) )
   {
      s << " [" << d2 << "]";
   }
   else
   {
      s << " [gdcm::too long for print (" << GetLength() << ") ]";
   }
   
   // Display the UID value (instead of displaying only the rough code)  
   if (g == 0x0002) {  // Any more to be displayed ?
      if ( (e == 0x0010) || (e == 0x0002) )
         s << "  ==>\t[" << ts->GetValue(v) << "]";
   } else {
      if (g == 0x0008) {
         if ( (e == 0x0016) || (e == 0x1150)  )
            s << "  ==>\t[" << ts->GetValue(v) << "]";
      } else {
         if (g == 0x0004) {
           if ( (e == 0x1510) || (e == 0x1512)  )
              s << "  ==>\t[" << ts->GetValue(v) << "]";
         }     
      }
   }
   //if (e == 0x0000) {        // elem 0x0000 --> group length 
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") ) {
      if (v == "4294967295") // to avoid troubles in convertion 
         sprintf (st," x(ffffffff)");
      else {
         if ( GetLength() !=0 )        
            sprintf(st," x(%x)", atoi(v.c_str()));//FIXME
        else
            sprintf(st," "); 
      }
      s << st;
   }
        
   s << std::endl;
   os << s.str();   
}


/*
 * \brief   canonical Writer
 */
void gdcmValEntry::Write(FILE *fp) {
      std::string vr=GetVR();
      int lgr=GetLength();
      if (vr == "US" || vr == "SS") {
         // some 'Short integer' fields may be mulivaluated
         // each single value is separated from the next one by '\'
         // we split the string and write each value as a short int
         std::vector<std::string> tokens;
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize (GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) {
            guint16 val_uint16 = atoi(tokens[i].c_str());
            void *ptr = &val_uint16;
            fwrite ( ptr,(size_t)2 ,(size_t)1 ,fp);
         }
         tokens.clear();
         return;
      }
      if (vr == "UL" || vr == "SL") {
         // Some 'Integer' fields may be multivaluated (multiple instances 
         // of integers). But each single integer value is separated from the
         // next one by '\' (backslash character). Hence we split the string
         // along the '\' and write each value as an int:
         std::vector<std::string> tokens;
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize (GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++){
            guint32 val_uint32 = atoi(tokens[i].c_str());
            void *ptr = &val_uint32;
            fwrite ( ptr,(size_t)4 ,(size_t)1 ,fp);
         }
         tokens.clear();
         return;
      }           
      fwrite (GetValue().c_str(),
              (size_t)lgr ,(size_t)1, fp); // Elem value
} 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
