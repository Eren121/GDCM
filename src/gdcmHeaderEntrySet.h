// gdcmHeaderEntrySet.h
//-----------------------------------------------------------------------------
#ifndef GDCMHeaderEntrySet_H
#define GDCMHeaderEntrySet_H

#include "gdcmCommon.h"
#include "gdcmHeaderEntry.h"

#include <stdio.h>
#include <map>
#include <list>       // for linking together *all* the Dicom Elements

//-----------------------------------------------------------------------------
typedef std::multimap<TagKey, gdcmHeaderEntry*> TagHeaderEntryHT;
typedef std::pair<TagKey, gdcmHeaderEntry*> PairHT;
typedef std::pair<TagHeaderEntryHT::iterator,TagHeaderEntryHT::iterator> IterHT; 

typedef std::list<gdcmHeaderEntry*> ListTag; // for linking together the Elements

// TODO : to be removed after re-writting   gdcmHeaderEntrySet::UpdateGroupLength
//          using the chained list instead of the H table
typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;

//-----------------------------------------------------------------------------
/*
 * Container for a set of successfully parsed HeaderEntrys (i.e. Dicom Elements).
 */
class GDCM_EXPORT gdcmHeaderEntrySet {
public:	
   ~gdcmHeaderEntrySet();

   void Print(std::ostream &);

   void Add(gdcmHeaderEntry*);
			
   gdcmHeaderEntry* GetHeaderEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByNumber(guint16 group, guint16 element);
	
   bool SetEntryByNumber(std::string content, guint16 group, guint16 element);
   bool SetEntryLengthByNumber(guint32 l, guint16 group, guint16 element);
   bool SetVoidAreaByNumber(void *a, guint16 Group, guint16 Elem );

   guint32 GenerateFreeTagKeyInGroup(guint16 group);
   int CheckIfExistByNumber(guint16 Group, guint16 Elem );  // int !

   /**
    * \ingroup gdcmHeaderEntrySet
    * \brief   returns a ref to the Dicom Header H table (multimap)
    * return the Dicom Header H table
    */
   inline TagHeaderEntryHT & gdcmHeaderEntrySet::GetTagHT(void) { return tagHT; };

   /**
    * \ingroup gdcmHeaderEntrySet
    * \brief   returns a ref to the Dicom Header chained list
    * return the Dicom Header chained list
    */
   inline ListTag      & gdcmHeaderEntrySet::GetListEntry(void) { return listEntries; };

   bool Write(FILE *fp, FileType type);

private:
   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteEntries(FileType type, FILE *);
   
// Variables
   TagHeaderEntryHT tagHT; // H Table (multimap), to provide fast access
   ListTag listEntries;   // chained list, to keep the 'spacial' ordering
};

//-----------------------------------------------------------------------------
#endif
