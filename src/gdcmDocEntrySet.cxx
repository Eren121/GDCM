/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/24 16:00:47 $
  Version:   $Revision: 1.65 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDocEntrySet.h"

#include "gdcmDebug.h"
#include "gdcmCommon.h"
#include "gdcmDictSet.h"
#include "gdcmGlobal.h"
#include "gdcmDocEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmUtil.h"
#include "gdcmDataEntry.h"
#include "gdcmVR.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
DocEntrySet::DocEntrySet() 
{ 
   PreviousDocEntry = 0;
}
//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Get the "std::string representable" value of the Dicom entry
 * @param   group  Group number of the searched tag.
 * @param   elem Element number of the searched tag.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND otherwise.
 */
std::string DocEntrySet::GetEntryString(uint16_t group, uint16_t elem)
{
   DataEntry *entry = dynamic_cast<DataEntry *>(GetDocEntry(group,elem));
   if ( entry )
   {
      if( entry->IsNotLoaded() )
         return GDCM_NOTLOADED;
      if( entry->IsUnfound() )
         return GDCM_UNFOUND;
      if( entry->IsUnread() )
         return GDCM_UNREAD;
      return entry->GetString();
   }
   return GDCM_UNFOUND;
}

/**
 * \brief   Gets (from Header) a 'non string' element value 
 * @param group   group number of the Entry 
 * @param elem  element number of the Entry
 * @return Pointer to the 'non string' area
 */
void *DocEntrySet::GetEntryBinArea(uint16_t group, uint16_t elem) 
{
   DataEntry *entry = GetDataEntry(group, elem);
   if ( entry )
      return entry->GetBinArea();
   return 0;
}

/**
 * \brief   Return the value of the DataEntry if it's "std::string representable"
 * @param   group  Group number of the searched tag.
 * @param   elem Element number of the searched tag.
 * @return  Corresponding element value when it's "std::string representable"
 *          and the string GDCM_NOTASCII otherwise.
 */
std::string DocEntrySet::GetEntryForcedAsciiValue(uint16_t group, uint16_t elem)
{
   DocEntry *d = GetDocEntry(group,elem);
   if ( !d )
      return GDCM_UNFOUND;

   DataEntry *de = dynamic_cast<DataEntry *>(d);
   if ( de )
   {
      if( de->IsNotLoaded() )
         return GDCM_NOTLOADED;
      if( de->IsUnfound() )
         return GDCM_UNFOUND;
      if( de->IsUnread() )
         return GDCM_UNREAD;
   }

   if( Global::GetVR()->IsVROfStringRepresentable( de->GetVR() ) )
      return de->GetString();
   else
   {
      uint8_t *a = de->GetBinArea();
      if( Util::IsCleanArea(a, de->GetLength()) )
         return  Util::CreateCleanString(a, de->GetLength());
   }
   return GDCM_NOTASCII;
}

/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the value length of a given tag..
 * @param   group  Group number of the searched tag.
 * @param   elem Element number of the searched tag.
 * @return  Corresponding element length; -2 if not found
 */
int DocEntrySet::GetEntryLength(uint16_t group, uint16_t elem)
{
   DocEntry *entry = GetDocEntry(group, elem);
   if ( entry )
      return entry->GetLength();
   return -1;
}

/**
 * \brief  Same as \ref Document::GetDocEntry except it only
 *         returns a result when the corresponding entry is of type
 *         DataEntry.
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element  
 * @return When present, the corresponding DataEntry. 
 */
DataEntry *DocEntrySet::GetDataEntry(uint16_t group, uint16_t elem)
{
   DocEntry *currentEntry = GetDocEntry(group, elem);
   if ( !currentEntry )
      return NULL;

   return dynamic_cast<DataEntry*>(currentEntry);
}

/**
 * \brief  Same as \ref Document::GetDocEntry except it only
 *         returns a result when the corresponding entry is of type
 *         SeqEntry.
 * @param   group  Group number of the searched Dicom Element 
 * @param   elem Element number of the searched Dicom Element  
 * @return When present, the corresponding SeqEntry. 
 */
SeqEntry *DocEntrySet::GetSeqEntry(uint16_t group, uint16_t elem)
{
   DocEntry *currentEntry = GetDocEntry(group, elem);
   if ( !currentEntry )
   {
      gdcmWarningMacro( "No corresponding SeqEntry " << std::hex << group <<
                        "," << elem);
      return NULL;
   }

   return dynamic_cast<SeqEntry*>(currentEntry);
}

/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (string) to substitute with
 * @param   group  group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 */
bool DocEntrySet::SetEntryString(std::string const &content, 
                                 uint16_t group, uint16_t elem) 
{
   DataEntry *entry = GetDataEntry(group, elem);
   if (!entry )
   {
      gdcmWarningMacro( "No corresponding DataEntry " << std::hex << group <<
                         "," << elem << " element (try promotion first).");
      return false;
   }
   return SetEntryString(content,entry);
}

/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (void*  -> uint8_t*) to substitute with
 * @param   lgth new value length
 * @param   group  group number of the Dicom Element to modify
 * @param   elem element number of the Dicom Element to modify
 */
bool DocEntrySet::SetEntryBinArea(uint8_t *content, int lgth, 
                                  uint16_t group, uint16_t elem) 
{
   DataEntry *entry = GetDataEntry(group, elem);
   if (!entry )
   {
      gdcmWarningMacro( "No corresponding DataEntry " << std::hex << group <<
                        "," << elem << " element (try promotion first).");
      return false;
   }

   return SetEntryBinArea(content,lgth,entry);
} 

/**
 * \brief   Accesses an existing DocEntry (i.e. a Dicom Element)
 *          and modifies it's content with the given value.
 * @param  content new value (string) to substitute with
 * @param  entry Entry to be modified
 */
bool DocEntrySet::SetEntryString(std::string const &content, DataEntry *entry)
{
   if (entry)
   {
      entry->SetString(content);
      return true;
   }
   return false;
}

/**
 * \brief   Accesses an existing DataEntry (i.e. a Dicom Element)
 *          and modifies it's content with the given value.
 * @param   content new value (void*  -> uint8_t*) to substitute with
 * @param  entry Entry to be modified 
 * @param  lgth new value length
 */
bool DocEntrySet::SetEntryBinArea(uint8_t *content, int lgth, DataEntry *entry)
{
   if (entry)
   {
      entry->SetLength(lgth);
      entry->SetBinArea(content);  
      return true;
   }
   return false;
}

/**
 * \brief   Modifies the value of a given Doc Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * @param   value (string) Value to be set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * @param   vr  V(alue) R(epresentation) of the Entry -if private Entry-
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */ 
DataEntry *DocEntrySet::InsertEntryString(std::string const &value, 
                                             uint16_t group, uint16_t elem,
                                             VRKey const &vr )
{
   DataEntry *dataEntry = 0;
   DocEntry *currentEntry = GetDocEntry( group, elem );
   
   if (currentEntry)
   {
      dataEntry = dynamic_cast<DataEntry *>(currentEntry);

      // Verify the VR
      if ( dataEntry )
         if ( dataEntry->GetVR()!=vr )
            dataEntry = NULL;

      // if currentEntry doesn't correspond to the requested dataEntry
      if ( !dataEntry)
      {
         if ( !RemoveEntry(currentEntry) )
         {
            gdcmWarningMacro( "Removal of previous DocEntry failed.");
            return NULL;
         }
      }
   }

   // Create a new dataEntry if necessary
   if ( !dataEntry )
   {
      dataEntry = NewDataEntry( group, elem, vr );

      if ( !AddEntry(dataEntry) )
      {
         gdcmWarningMacro("AddEntry failed although this is a creation.");

         dataEntry->Delete();
         return NULL;
      }
   }

   // Set the dataEntry value
   SetEntryString(value, dataEntry); // The std::string value
   return dataEntry;
}

/**
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 *          A copy of the binArea is made to be kept in the Document.
 * @param   binArea (binary) value to be set
 * @param   lgth length of the Bin Area we want to set
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * @param   vr  V(alue) R(epresentation) of the Entry -if private Entry-
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */
DataEntry *DocEntrySet::InsertEntryBinArea(uint8_t *binArea, int lgth, 
                                              uint16_t group, uint16_t elem,
                                              VRKey const &vr )
{
   DataEntry *dataEntry = 0;
   DocEntry *currentEntry = GetDocEntry( group, elem );

   // Verify the currentEntry
   if (currentEntry)
   {
      dataEntry = dynamic_cast<DataEntry *>(currentEntry);

      // Verify the VR
      if ( dataEntry )
         if ( dataEntry->GetVR()!=vr )
            dataEntry = NULL;

      // if currentEntry doesn't correspond to the requested dataEntry
      if ( !dataEntry)
      {
         if ( !RemoveEntry(currentEntry) )
         {
            gdcmWarningMacro( "Removal of previous DocEntry failed.");
            return NULL;
         }
      }
   }

   // Create a new dataEntry if necessary
   if ( !dataEntry)
   {
      dataEntry = NewDataEntry(group, elem, vr);

      if ( !AddEntry(dataEntry) )
      {
         gdcmWarningMacro( "AddEntry failed although this is a creation.");

         dataEntry->Delete();
         return NULL;
      }
   }

   // Set the dataEntry value
   uint8_t *tmpArea;
   if ( lgth>0 && binArea )
   {
      tmpArea = new uint8_t[lgth];
      memcpy(tmpArea,binArea,lgth);
   }
   else
   {
      tmpArea = 0;
   }
   if ( !SetEntryBinArea(tmpArea,lgth,dataEntry) )
   {
      if ( tmpArea )
      {
         delete[] tmpArea;
      }
   }

   return dataEntry;
}  

/**
 * \brief   Modifies the value of a given Doc Entry (Dicom Element)
 *          when it exists. Creates it when unexistant.
 * @param   group   Group number of the Entry 
 * @param   elem  Element number of the Entry
 * \return  pointer to the modified/created SeqEntry (NULL when creation
 *          failed).
 */
SeqEntry *DocEntrySet::InsertSeqEntry(uint16_t group, uint16_t elem)
{
   SeqEntry *seqEntry = 0;
   DocEntry *currentEntry = GetDocEntry( group, elem );

   // Verify the currentEntry
   if ( currentEntry )
   {
      seqEntry = dynamic_cast<SeqEntry *>(currentEntry);

      // Verify the VR
      if ( seqEntry )
         seqEntry = NULL;

      // if currentEntry doesn't correspond to the requested seqEntry
      if ( !seqEntry )
      {
         if (!RemoveEntry(currentEntry))
         {
            gdcmWarningMacro( "Removal of previous DocEntry failed.");

            return NULL;
         }
      }
   }
   // Create a new seqEntry if necessary
   if ( !seqEntry )
   {
      seqEntry = NewSeqEntry(group, elem);

      if ( !AddEntry(seqEntry) )
      {
         gdcmWarningMacro( "AddEntry failed although this is a creation.");

         seqEntry->Delete();
         return NULL;
      }
   }

   // TODO : Find a trick to insert a SequenceDelimitationItem 
   //       in the SeqEntry, at the end.
   return seqEntry;
} 
 
/**
 * \brief   Checks if a given Dicom Element exists within the H table
 * @param   group   Group number of the searched Dicom Element 
 * @param   elem  Element number of the searched Dicom Element 
 * @return true is found
 */
bool DocEntrySet::CheckIfEntryExist(uint16_t group, uint16_t elem )
{
   return GetDocEntry(group,elem)!=NULL;
}

/**
 * \brief   Build a new Val Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group Group number   of the new Entry
 * @param   elem  Element number of the new Entry
 * @param   vr    V(alue) R(epresentation) of the new Entry 
 * \remarks The user of this method must destroy the DataEntry when unused
 */
DataEntry *DocEntrySet::NewDataEntry(uint16_t group,uint16_t elem,
                                     VRKey const &vr) 
{
   DictEntry *dictEntry = GetDictEntry(group, elem, vr);

   DataEntry *newEntry = DataEntry::New(dictEntry);
   dictEntry->Unregister(); // GetDictEntry register it
   if (!newEntry) 
   {
      gdcmWarningMacro( "Failed to allocate DataEntry");
      return 0;
   }
   return newEntry;
}

/**
 * \brief   Build a new Seq Entry from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   group Group   number of the new Entry
 * @param   elem  Element number of the new Entry
 * \remarks The user of this method must destroy the SeqEntry when unused
 */
SeqEntry* DocEntrySet::NewSeqEntry(uint16_t group, uint16_t elem) 
{
   DictEntry *dictEntry = GetDictEntry(group, elem, "SQ");

   SeqEntry *newEntry = SeqEntry::New( dictEntry );
   dictEntry->Unregister(); // GetDictEntry register it
   if (!newEntry)
   {
      gdcmWarningMacro( "Failed to allocate SeqEntry");
      return 0;
   }
   return newEntry;
}

//-----------------------------------------------------------------------------
// Protected
/**
 * \brief   Searches [both] the public [and the shadow dictionary (when they
 *          exist)] for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one(s), if any.
 * @param   group  Group number of the searched DictEntry
 * @param   elem Element number of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 * \remarks The returned DictEntry is registered when existing
 */
DictEntry *DocEntrySet::GetDictEntry(uint16_t group,uint16_t elem) 
{
   DictEntry *found = 0;
   Dict *pubDict = Global::GetDicts()->GetDefaultPubDict();
   if (!pubDict) 
   {
      gdcmWarningMacro( "We SHOULD have a default dictionary");
   }
   else
   {
      found = pubDict->GetEntry(group, elem);
      if( found )
         found->Register();
   }
   return found;
}

/**
 * \brief   Searches [both] the public [and the shadow dictionary (when they
 *          exist)] for the presence of the DictEntry with given
 *          group and element, and create a new virtual DictEntry if necessary
 * @param   group  group number of the searched DictEntry
 * @param   elem element number of the searched DictEntry
 * @param   vr V(alue) R(epresentation) to use, if necessary 
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 * \remarks The returned DictEntry is registered
 */
DictEntry *DocEntrySet::GetDictEntry(uint16_t group, uint16_t elem,
                                     VRKey const &vr)
{
   DictEntry *dictEntry = GetDictEntry(group,elem);
   DictEntry *goodEntry = dictEntry;
   VRKey goodVR = vr;

   if (elem == 0x0000) 
      goodVR="UL";

   if ( goodEntry )
   {
      if ( goodVR != goodEntry->GetVR()
        && goodVR != GDCM_VRUNKNOWN )
      {
         goodEntry = NULL;
      }
      dictEntry->Unregister();
   }

   // Create a new virtual DictEntry if necessary
   if (!goodEntry)
   {
      if (dictEntry)
      {
         goodEntry = DictEntry::New(group, elem, goodVR, "FIXME", 
                                    dictEntry->GetName() );
      }
      else
      {
         goodEntry = DictEntry::New(group, elem, goodVR);
      }
   }
   else
   {
      goodEntry->Register();
   }
   return goodEntry;
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
