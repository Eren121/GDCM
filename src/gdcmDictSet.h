/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictSet.h,v $
  Language:  C++
  Date:      $Date: 2004/08/01 00:59:21 $
  Version:   $Revision: 1.22 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICTSET_H
#define GDCMDICTSET_H

#include "gdcmDict.h"
#include <map>
#include <list>

typedef std::string DictKey;
typedef std::map<DictKey, gdcmDict*> DictSetHT;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDictSet
 * \brief  Container for managing a set of loaded dictionaries.
 * \note   Hopefully, sharing dictionaries should avoid
 * \par    reloading an already loaded dictionary (saving time)
 * \par    having many in memory representations of the same dictionary
 *        (saving memory).
 */
class GDCM_EXPORT gdcmDictSet
{
public:
   // TODO Swig int LoadDictFromFile(std::string filename);
   // QUESTION: the following function might not be thread safe !? Maybe
   //           we need some mutex here, to avoid concurent creation of
   //           the same dictionary !?!?!
   // TODO Swig int LoadDictFromName(std::string filename);
   // TODO Swig int LoadAllDictFromDirectory(std::string DirectoryName);
   // TODO Swig std::string* GetAllDictNames();
   gdcmDictSet();
   ~gdcmDictSet();

   void Print(std::ostream& os);

   std::list<std::string> *GetPubDictEntryNames();
   std::map<std::string, std::list<std::string> > *
       GetPubDictEntryNamesByCategory();

   gdcmDict *LoadDictFromFile(std::string FileName, DictKey Name);

   gdcmDict *GetDict(DictKey DictName);
   gdcmDict *GetDefaultPubDict();

   gdcmDictEntry *NewVirtualDictEntry(uint16_t group, uint16_t element,
                                      std::string vr     = "Unknown",
                                      std::string fourth = "Unknown",
                                      std::string name   = "Unknown");

   static std::string BuildDictPath();

protected:
   bool AppendDict(gdcmDict *NewDict,DictKey Name);

private:
   /// Hash table of all dictionaries contained in this gdcmDictSet
   DictSetHT Dicts;
   /// Directory path to dictionaries
   std::string DictPath;
   /// H table for the on the fly created gdcmDictEntries  
   std::map<std::string,gdcmDictEntry *> virtualEntry;
};

//-----------------------------------------------------------------------------
#endif
