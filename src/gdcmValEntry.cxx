/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmValEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2004/09/10 18:54:39 $
  Version:   $Revision: 1.24 $
                                                                                
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
   VoidArea = NULL; // will be in BinEntry ?
}

/**
 * \brief   Constructor from a given gdcmDocEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmValEntry::gdcmValEntry(gdcmDocEntry* e) : gdcmDocEntry(e->GetDictEntry())
{
   UsableLength = e->GetLength();
   ReadLength   = e->GetReadLength();
   ImplicitVR   = e->IsImplicitVR();
   Offset       = e->GetOffset();
   PrintLevel   = e->GetPrintLevel();
   SQDepthLevel = e->GetDepthLevel();

   VoidArea = NULL; // will be in BinEntry ?
}


/**
 * \brief   Canonical destructor.
 */
gdcmValEntry::~gdcmValEntry ()
{
   if (!VoidArea)  // will be in BinEntry
   {
      free(VoidArea);
      VoidArea = NULL; // let's be carefull !
   }
}

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   canonical Printer
 */
void gdcmValEntry::Print(std::ostream & os)
{
   uint16_t g = GetGroup();
   uint16_t e = GetElement();
   std::string vr = GetVR();
   std::ostringstream s; 
   char st[20];
   TSKey v;
   std::string d2;
     
   gdcmDocEntry::Print(os); 

   if (g == 0xfffe)
   {
      // just to avoid identing all the remaining code     
      return;
   }
   
   gdcmTS * ts = gdcmGlobal::GetTS();
    
   v  = GetValue();  // not applicable for SQ ...     
   d2 = CreateCleanString(v);  // replace non printable characters by '.'            
   if( (GetLength()<=MAX_SIZE_PRINT_ELEMENT_VALUE) || 
       //(PrintLevel>=3)  || (d2.find("gdcm::NotLoaded.") < d2.length()) )
       (PrintLevel>=3)  || (d2.find(GDCM_NOTLOADED) < d2.length()) )
   {
      s << " [" << d2 << "]";
   }
   else
   {
      s << " [gdcm::too long for print (" << GetLength() << ") ]";
   }
   
   // Display the UID value (instead of displaying only the rough code)
   // First 'clean' trailing character (space or zero) 
   if (g == 0x0002)
   {
      // Any more to be displayed ?
      if ( (e == 0x0010) || (e == 0x0002) )
      {
         if ( v.length() != 0 )  // for brain damaged headers
         {
            if ( ! isdigit(v[v.length()-1]) )
            {
               v.erase(v.length()-1, 1);
            }
         }
         s << "  ==>\t[" << ts->GetValue(v) << "]";
      }
   }
   else
   {
      if (g == 0x0008)
      {
         if ( e == 0x0016 || e == 0x1150 )
         {
            if ( v.length() != 0 )  // for brain damaged headers
            {
               if ( ! isdigit(v[v.length()-1]) )
               {
                  v.erase(v.length()-1, 1);
               }
            }
            s << "  ==>\t[" << ts->GetValue(v) << "]";
         }
      }
      else
      {
         if (g == 0x0004)
         {
            if ( (e == 0x1510) || (e == 0x1512)  )
            {
               if ( v.length() != 0 )  // for brain damaged headers  
               {
                  if ( ! isdigit(v[v.length()-1]) )
                  {
                     v.erase(v.length()-1, 1);  
                  }
               }
              s << "  ==>\t[" << ts->GetValue(v) << "]";
            }
         }     
      }
   }
   //if (e == 0x0000) {        // elem 0x0000 --> group length 
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
   {
      if (v == "4294967295") // to avoid troubles in convertion 
      {
         sprintf (st," x(ffffffff)");
      }
      else
      {
         if ( GetLength() !=0 )
         {
            sprintf(st," x(%x)", atoi(v.c_str()));//FIXME
         }
         else
         {
            sprintf(st," ");
         }
      }
      s << st;
   }
   os << s.str();
}

/*
 * \brief   canonical Writer
 */
void gdcmValEntry::Write(FILE *fp, FileType filetype)
{
   gdcmDocEntry::Write(fp, filetype);

   //std::cout << "=====================================" << GetVR() << std::endl;
      
   if ( GetGroup() == 0xfffe ) 
   {
      return; //delimitors have NO value
   }
      
   std::string vr = GetVR();
   int lgr = GetReadLength();
   if (vr == "US" || vr == "SS")
   {
      // some 'Short integer' fields may be mulivaluated
      // each single value is separated from the next one by '\'
      // we split the string and write each value as a short int
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Tokenize (GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++)
      {
         uint16_t val_uint16 = atoi(tokens[i].c_str());
         void *ptr = &val_uint16;
         fwrite ( ptr,(size_t)2 ,(size_t)1 ,fp);
      }
      tokens.clear();
      return;
   }
   if (vr == "UL" || vr == "SL")
   {
      // Some 'Integer' fields may be multivaluated (multiple instances 
      // of integers). But each single integer value is separated from the
      // next one by '\' (backslash character). Hence we split the string
      // along the '\' and write each value as an int:
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Tokenize (GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++)
      {
         uint32_t val_uint32 = atoi(tokens[i].c_str());
         void *ptr = &val_uint32;
         fwrite ( ptr,(size_t)4 ,(size_t)1 ,fp);
      }
      tokens.clear();
      return;
   } 
          
   fwrite (GetValue().c_str(), (size_t)lgr ,(size_t)1, fp); // Elem value
} 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
