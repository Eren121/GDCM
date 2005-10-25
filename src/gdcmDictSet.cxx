/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictSet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 14:52:34 $
  Version:   $Revision: 1.72 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDictSet.h"
#include "gdcmDebug.h"
#include <fstream>
#include <stdlib.h>  // For getenv
#include <stdio.h>   // For sprintf

namespace gdcm 
{

//-----------------------------------------------------------------------------
// Constructor / Destructor
/** 
 * \brief   The Dictionary Set obtained with this constructor simply
 *          contains the Default Public dictionary.
 */
DictSet::DictSet() 
{
   DictPath = BuildDictPath();
   std::string pubDictFile(DictPath);
   pubDictFile += PUB_DICT_FILENAME;
   Dicts[PUB_DICT_NAME] = Dict::New(pubDictFile);
}

/**
 * \brief  Destructor 
 */
DictSet::~DictSet() 
{
   // Remove dictionaries
   for (DictSetHT::iterator tag = Dicts.begin(); tag != Dicts.end(); ++tag) 
   {
      if ( tag->second )
         tag->second->Delete();
   }
   Dicts.clear();
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Loads a dictionary from a specified file, and add it
 *          to already the existing ones contained in this DictSet.
 * @param   filename Absolute or relative filename containing the
 *          dictionary to load.
 * @param   name Symbolic name that be used as identifier of the newly 
 *          created dictionary.
 */
Dict *DictSet::LoadDictFromFile(std::string const &filename, 
                                DictKey const &name) 
{
   Dict *newDict = Dict::New(filename);
   Dicts[name] = newDict;

   return newDict;
}

/**
 * \brief   Retrieve the specified dictionary (when existing) from this
 *          DictSet.
 * @param   dictName The symbolic name of the searched dictionary.
 * \result  The retrieved dictionary.
 */
Dict *DictSet::GetDict(DictKey const &dictName) 
{
   DictSetHT::iterator dict = Dicts.find(dictName);
   if ( dict != Dicts.end() )
   {
      return dict->second;
   }
   return NULL;
}

/**
 * \brief   Get the first dictionary while visiting the DictSet
 * \return  The first Dict if found, otherwhise NULL
 */
Dict *DictSet::GetFirstDict()
{
   ItDictHt = Dicts.begin();
   if ( ItDictHt != Dicts.end() )
      return ItDictHt->second;
   return NULL;
}

/**
 * \brief   Get the next dictionary while visiting the Hash table (DictSetHT)
 * \note : meaningfull only if GetFirstEntry already called
 * \return  The next Dict if found, otherwhise NULL
 */
Dict *DictSet::GetNextDict()
{
   gdcmAssertMacro (ItDictHt != Dicts.end());
  
   ++ItDictHt;
   if ( ItDictHt != Dicts.end() )
      return ItDictHt->second;
   return NULL;
}

/**
 * \brief   Obtain from the GDCM_DICT_PATH environnement variable the
 *          path to directory containing the dictionaries. When
 *          the environnement variable is absent the path is defaulted
 *          to "../Dicts/".
 * @return  path to directory containing the dictionaries
 */
std::string DictSet::BuildDictPath() 
{
   std::string resultPath;
   const char *envPath;
   envPath = getenv("GDCM_DICT_PATH");

   if (envPath && (strlen(envPath) != 0)) 
   {
      resultPath = envPath;
      gdcmWarningMacro( "Dictionary path set from environnement");
   } 
   else
   {
      resultPath = PUB_DICT_PATH;
   }
   if ( resultPath[resultPath.length()-1] != '/' )
   {
      resultPath += '/';
   }

   return resultPath;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Print, in an informal fashion, the list of all the dictionaries
 *          contained is this DictSet, along with their respective content.
 * @param   os Output stream used for printing.
 * @param indent Indentation string to be prepended during printing
 */
void DictSet::Print(std::ostream &os, std::string const & )
{
   for (DictSetHT::iterator dict = Dicts.begin(); dict != Dicts.end(); ++dict)
   {
      os << "Printing dictionary " << dict->first << std::endl;
      dict->second->Print(os);
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
