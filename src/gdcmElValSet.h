// gdcmElValSet.h
//-----------------------------------------------------------------------------
#ifndef GDCMELVALSET_H
#define GDCMELVALSET_H

#include "gdcmCommon.h"
#include "gdcmElValue.h"

#include <stdio.h>
#include <map>
#include <list>       // for linking together *all* the Dicom Elements

//-----------------------------------------------------------------------------
typedef std::multimap<TagKey, gdcmElValue*> TagElValueHT;
typedef std::pair<TagKey, gdcmElValue*> PairHT;
typedef std::pair<TagElValueHT::iterator,TagElValueHT::iterator> IterHT; 

typedef std::list<gdcmElValue*> ListTag; // for linking together the Elements

// TODO : to be removed after re-writting   gdcmElValSet::UpdateGroupLength
//          using the chained list instead of the H table
typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;

//-----------------------------------------------------------------------------
/*
 * Container for a set of successfully parsed ElValues (i.e. Dicom Elements).
 */
class GDCM_EXPORT gdcmElValSet {
public:	
   ~gdcmElValSet();

   void Print(std::ostream &);

   void Add(gdcmElValue*);
			
   gdcmElValue* GetElementByNumber(guint16 group, guint16 element);
   std::string  GetElValueByNumber(guint16 group, guint16 element);
	
   bool SetElValueByNumber(std::string content, guint16 group, guint16 element);
   bool SetElValueLengthByNumber(guint32 l, guint16 group, guint16 element);
   bool SetVoidAreaByNumber(void *a, guint16 Group, guint16 Elem );

   guint32 GenerateFreeTagKeyInGroup(guint16 group);
   int CheckIfExistByNumber(guint16 Group, guint16 Elem );  // int !

   /**
    * \ingroup gdcmElValSet
    * \brief   returns a ref to the Dicom Header H table (multimap)
    * return the Dicom Header H table
    */
   inline TagElValueHT & gdcmElValSet::GetTagHt(void) { return tagHt; };

   /**
    * \ingroup gdcmElValSet
    * \brief   returns a ref to the Dicom Header chained list
    * return the Dicom Header chained list
    */
   inline ListTag      & gdcmElValSet::GetListElem(void) { return listElem; };

   bool Write(FILE *fp, FileType type);

private:
   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteElements(FileType type, FILE *);
   
// Variables
   TagElValueHT tagHt; // H Table (multimap), to provide fast access
   ListTag listElem;   // chained list, to keep the 'spacial' ordering
};

//-----------------------------------------------------------------------------
#endif
