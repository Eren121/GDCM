// $Header: /cvs/public/gdcm/src/Attic/gdcmElValSet.h,v 1.5 2003/03/24 13:11:28 frog Exp $

#ifndef GDCMELVALSET_H
#define GDCMELVALSET_H

#include <stdio.h>    // FIXME For FILE on GCC only
#include <map>
#include "gdcmCommon.h"
#include "gdcmElValue.h"

////////////////////////////////////////////////////////////////////////////
// Container for a set of successfully parsed ElValues.
typedef map<TagKey, gdcmElValue*> TagElValueHT;
typedef map<string, gdcmElValue*> TagElValueNameHT;

class GDCM_EXPORT gdcmElValSet {
	TagElValueHT tagHt;             // Both accesses with a TagKey or with a
	TagElValueNameHT NameHt;        // the DictEntry.Name are required.
public:	
	void Add(gdcmElValue*);	
	void ReplaceOrCreate(gdcmElValue*);		
	void Print(ostream &);
	void PrintByName(ostream &);
	int  Write(FILE *fp);
	int  WriteAcr(FILE *fp);
	gdcmElValue* GetElementByNumber(guint32 group, guint32 element);
	gdcmElValue* GetElementByName  (string);
	string   GetElValueByNumber(guint32 group, guint32 element);
	string   GetElValueByName  (string);
	
	TagElValueHT & GetTagHt(void);	
	
	int SetElValueByNumber(string content, guint32 group, guint32 element);
	int SetElValueByName  (string content, string TagName);
	
	int SetElValueLengthByNumber(guint32 l, guint32 group, guint32 element);
	int SetElValueLengthByName  (guint32 l, string TagName);
   guint32 GenerateFreeTagKeyInGroup(guint32 group);

};

#endif
