/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/11 23:16:47 $
  Version:   $Revision: 1.36 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmTS.h"
#include "gdcmDebug.h"
#include "gdcmUtil.h"
#include "gdcmDictSet.h"

#include <fstream>
#include <string>
#include <iostream>

// TODO
// a lot of troubles expected with TS : 1.2.840.113619.5.2
// Implicit VR - Big Endian
// see : http://www.gemedicalsystemseurope.com/euen/it_solutions/pdf/lsqxi_rev2.pdf
// 

namespace gdcm 
{
//-----------------------------------------------------------------------------
static const char *SpecialStrings[] =  {
  // Implicit VR Little Endian
  "1.2.840.10008.1.2",
  // Implicit VR Big Endian DLX (G.E Private)
  "1.2.840.113619.5.2",
  // Explicit VR Little Endian
  "1.2.840.10008.1.2.1",
  // Deflated Explicit VR Little Endian
  "1.2.840.10008.1.2.1.99",
  // Explicit VR Big Endian
  "1.2.840.10008.1.2.2",
  // JPEG Baseline (Process 1)
  "1.2.840.10008.1.2.4.50",
  // JPEG Extended (Process 2 & 4)
  "1.2.840.10008.1.2.4.51",
  // JPEG Extended (Process 3 & 5)
  "1.2.840.10008.1.2.4.52",
  // JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8)
  "1.2.840.10008.1.2.4.53",
  // JPEG Full Progression, Non-Hierarchical (Process 10 & 12)
  "1.2.840.10008.1.2.4.55",
  // JPEG Lossless, Non-Hierarchical (Process 14)
  "1.2.840.10008.1.2.4.57",
  // JPEG Lossless, Hierarchical, First-Order Prediction (Process 14, [Selection Value 1])
  "1.2.840.10008.1.2.4.70",
  // JPEG 2000 Lossless
  "1.2.840.10008.1.2.4.90",
  // JPEG 2000
  "1.2.840.10008.1.2.4.91",
  // RLE Lossless
  "1.2.840.10008.1.2.5",
  // Unknown
  "Unknown Transfer Syntax"
};

void FillDefaultTSDict(TSHT &ts);
//-----------------------------------------------------------------------------
// Constructor / Destructor
TS::TS() 
{
   std::string filename = DictSet::BuildDictPath() + DICT_TS;
   std::ifstream from(filename.c_str());
   if( !from )
   {
      gdcmVerboseMacro("Can't open dictionary" << filename.c_str());
      FillDefaultTSDict( TsMap );
   }
   else
   {
      TSKey key;
      TSAtr name;

      while (!from.eof())
      {
         from >> key;
         from >> std::ws;
         std::getline(from, name);

         if(key != "")
         {
            TsMap[key] = name;
         }
      }
      from.close();
   }
}

//-----------------------------------------------------------------------------
TS::~TS() 
{
   TsMap.clear();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup VR
 * \brief   Print all 
 * @param   os The output stream to be written to.
 */
void TS::Print(std::ostream &os) 
{
   std::ostringstream s;

   for (TSHT::const_iterator it = TsMap.begin(); it != TsMap.end(); ++it)
   {
      s << "TS : " << it->first << " = " << it->second << std::endl;
   }
   os << s.str();
}

//-----------------------------------------------------------------------------
// Public
int TS::Count(TSKey const &key) 
{
   return TsMap.count(key);
}

TSAtr const & TS::GetValue(TSKey const &key) 
{
   // First thing clean up the string sometime the transfer syntax is padded with spaces
   std::string copy = key;
   while ( copy.size() && !isdigit((unsigned char)copy[copy.size()-1]) )
   {
      copy.erase(copy.size()-1, 1);
   }

   TSHT::const_iterator it = TsMap.find(copy);
   if (it == TsMap.end())
   {
      return GDCM_UNFOUND;
   }
   return it->second;
}

bool TS::IsTransferSyntax(TSKey const &key)
{
   TSHT::const_iterator it = TsMap.find(key);
   return it != TsMap.end();
}

bool TS::IsRLELossless(TSKey const &key)
{
   bool r = false;
   // First check this is an actual transfer syntax
   if( IsTransferSyntax(key) )
   {
      if ( key == SpecialStrings[RLELossless] )
      {
         r = true;
      }
   }
   return r;
}

bool TS::IsJPEGLossless(TSKey const &key)
{
   bool r = false;
   // First check this is an actual transfer syntax
   if( IsTransferSyntax(key) )
   {
      if ( key == SpecialStrings[JPEGFullProgressionProcess10_12]
        || key == SpecialStrings[JPEGLosslessProcess14]
        || key == SpecialStrings[JPEGLosslessProcess14_1] )
      {
         r = true;
      }
   }
   return r;
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEG2000 one
 * @return  True when JPEG2000 (Lossly or LossLess) found. False in all
 *          other cases.
 */
bool TS::IsJPEG2000(TSKey const &key)
{
   bool r = false;
   // First check this is an actual transfer syntax
   if( IsTransferSyntax(key) )
   {
      if ( key == SpecialStrings[JPEG2000Lossless]
        || key == SpecialStrings[JPEG2000] )
      {
         r = true;
      }
   }
   return r;
}

/**
 * \brief   Determines if the Transfer Syntax corresponds to any form
 *          of Jpeg encoded Pixel data.
 * @return  True when any form of JPEG found. False otherwise.
 */
bool TS::IsJPEG(TSKey const &key)
{
   bool r = false;
   // First check this is an actual transfer syntax
   if( IsTransferSyntax(key) )
   {
      if ( key == SpecialStrings[JPEGBaselineProcess1]
        || key == SpecialStrings[JPEGExtendedProcess2_4]
        || key == SpecialStrings[JPEGExtendedProcess3_5]
        || key == SpecialStrings[JPEGSpectralSelectionProcess6_8]
        || IsJPEGLossless( key ) 
        || IsJPEG2000( key ) )
      {
         r = true;
      }
   }
   return r;
}

/**
 * \brief   Determines if the Transfer Syntax corresponds to encapsulated
 *          of encoded Pixel Data (as opposed to native).
 * @return  True when encapsulated. False when native.
 */
bool TS::IsEncapsulate(TSKey const &key)
{
   bool r = false;
   // First check this is an actual transfer syntax
   if( IsTransferSyntax(key) )
   {
      if ( key == SpecialStrings[RLELossless]
        || IsJPEG(key) )
      {
         r = true;
      }
   }
   return r;
}

TS::SpecialType TS::GetSpecialTransferSyntax(TSKey const &key)
{
   for (int i = 0; SpecialStrings[i] != NULL; i++)
   {
      if ( SpecialStrings[i] == key )
      {
         return SpecialType(i);
      }
   }

   return UnknownTS;
}

const char* TS::GetSpecialTransferSyntax(SpecialType t)
{
   return SpecialStrings[t];
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------

} // end namespace gdcm
