/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmContentEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/26 10:29:16 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmContentEntry.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <fstream>

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor for a given DictEntry
 * @param   e Pointer to existing dictionary entry
 */
ContentEntry::ContentEntry(DictEntry *e) : DocEntry(e)
{
   Value = GDCM_UNFOUND;
}

/**
 * \brief   Constructor for a given DocEntry
 * @param   e Pointer to existing Doc entry
 */
ContentEntry::ContentEntry(DocEntry *e)
             : DocEntry(e->GetDictEntry())
{
   Copy(e);
}

/**
 * \brief   Canonical destructor.
 */
ContentEntry::~ContentEntry ()
{
}

//-----------------------------------------------------------------------------
// Public

/**
 * \brief   Writes the 'std::string representable' value of a ContentEntry
 * @param fp already open ofstream pointer
 * @param filetype type of the file (ACR, ImplicitVR, ExplicitVR, ...)
 */
void ContentEntry::WriteContent(std::ofstream *fp, FileType filetype)
{
   DocEntry::WriteContent(fp, filetype);

   if ( GetGroup() == 0xfffe )
   {
      return; //delimitors have NO value
   }

   const VRKey &vr = GetVR();
   unsigned int lgr = GetLength();
   if (vr == "US" || vr == "SS")
   {
      // some 'Short integer' fields may be multivaluated
      // each single value is separated from the next one by '\'
      // we split the string and write each value as a short int
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Util::Tokenize (GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++)
      {
         uint16_t val_uint16 = atoi(tokens[i].c_str());
         binary_write( *fp, val_uint16);
      }
      tokens.clear();
      return;
   }
   if (vr == "UL" || vr == "SL")
   {
      // Some 'Integer' fields may be multivaluated (multiple instances 
      // of integer). But each single integer value is separated from the
      // next one by '\' (backslash character). Hence we split the string
      // along the '\' and write each value as an int:
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Util::Tokenize (GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++)
      {
         uint32_t val_uint32 = atoi(tokens[i].c_str());
         binary_write( *fp, val_uint32);
      }
      tokens.clear();
      return;
   } 

   gdcmAssertMacro( lgr == GetValue().length() );
   binary_write(*fp, GetValue());
} 

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
} // end namespace gdcm

