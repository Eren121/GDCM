// $Header: /cvs/public/gdcm/src/Attic/gdcmElValSet.h,v 1.17 2003/10/02 11:26:15 malaterre Exp $

#ifndef GDCMELVALSET_H
#define GDCMELVALSET_H

#include "gdcmCommon.h"
#include "gdcmElValue.h"

#include <stdio.h>    // FIXME For FILE on GCC only
#include <map>

////////////////////////////////////////////////////////////////////////////
// Container for a set of successfully parsed ElValues.

typedef std::map<TagKey, gdcmElValue*> TagElValueHT;
typedef std::map<std::string, gdcmElValue*> TagElValueNameHT;

class GDCM_EXPORT gdcmElValSet {
   TagElValueHT tagHt;             // Both accesses with a TagKey or with a
   TagElValueNameHT NameHt;        // the DictEntry.Name are required.
   
   typedef std::string GroupKey;
   typedef std::map<GroupKey, int> GroupHT; 
public:	
   ~gdcmElValSet();
   void Add(gdcmElValue*);
			
   void Print(std::ostream &);
   void PrintByName(std::ostream &);
   int  Write(FILE *fp, FileType type);

   gdcmElValue* GetElementByNumber(guint16 group, guint16 element);
   gdcmElValue* GetElementByName  (std::string);
   std::string  GetElValueByNumber(guint16 group, guint16 element);
   std::string  GetElValueByName  (std::string);
	
   TagElValueHT & GetTagHt(void);	
	
   int SetElValueByNumber(std::string content, guint16 group, guint16 element);
   int SetElValueByName  (std::string content, std::string TagName);
	
   int SetElValueLengthByNumber(guint32 l, guint16 group, guint16 element);
   int SetElValueLengthByName  (guint32 l, std::string TagName);
   
   int SetVoidAreaByNumber(void *a, guint16 Group, guint16 Elem );

   guint32 GenerateFreeTagKeyInGroup(guint16 group);
   int CheckIfExistByNumber(guint16 Group, guint16 Elem );
	
private:
   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteElements(FileType type, FILE *);
};

#endif
