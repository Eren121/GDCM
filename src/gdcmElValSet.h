// $Header: /cvs/public/gdcm/src/Attic/gdcmElValSet.h,v 1.21 2004/01/13 11:32:30 jpr Exp $

#ifndef GDCMELVALSET_H
#define GDCMELVALSET_H

#include "gdcmCommon.h"
#include "gdcmElValue.h"

#include <stdio.h>
#include <map>
#include <list>       // for linking together *all* the Dicom Elements

/*
 * Container for a set of successfully parsed ElValues (i.e. Dicom Elements).
 */

typedef std::multimap<TagKey, gdcmElValue*> TagElValueHT;
typedef std::pair<TagKey, gdcmElValue*> PairHT;
typedef std::pair<TagElValueHT::iterator,TagElValueHT::iterator> IterHT; 

typedef std::list<gdcmElValue*> ListTag; // for linking together the Elements

// TODO : to be removed after re-writting   gdcmElValSet::UpdateGroupLength
//          using the chained list instead of the H table
typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;


class GDCM_EXPORT gdcmElValSet {
   TagElValueHT tagHt; // H Table (multimap), to provide fast access
   ListTag listElem;   // chained list, to keep the 'spacial' ordering

public:	
   ~gdcmElValSet();
   void Add(gdcmElValue*);
			
   void Print(std::ostream &);
   bool Write(FILE *fp, FileType type);

   gdcmElValue* GetElementByNumber(guint16 group, guint16 element);
   //gdcmElValue* GetElementByName  (std::string); 
   // moved to gdcmHeader
   std::string  GetElValueByNumber(guint16 group, guint16 element);
	
   bool SetElValueByNumber(std::string content, guint16 group, guint16 element);
 // bool SetElValueByName  (std::string content, std::string TagName);
 // moved to gdcmHeader
	
   bool SetElValueLengthByNumber(guint32 l, guint16 group, guint16 element);
   
   bool SetVoidAreaByNumber(void *a, guint16 Group, guint16 Elem );

   guint32 GenerateFreeTagKeyInGroup(guint16 group);
   int CheckIfExistByNumber(guint16 Group, guint16 Elem );  // int !

 /**
 * \ingroup gdcmElValSet
 * \brief   returns a ref to the Dicom Header H table (multimap)
 * return the Dicom Header H table
 */
inline TagElValueHT & gdcmElValSet::GetTagHt(void)   {
    return tagHt; 
 };

 /**
 * \ingroup gdcmElValSet
 * \brief   returns a ref to the Dicom Header chained list
 * return the Dicom Header chained list
 */
 inline ListTag      & gdcmElValSet::GetListElem(void)  {
      return listElem;	
 };
 	
private:
   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteElements(FileType type, FILE *);
   

};

#endif
