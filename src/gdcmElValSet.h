// gdcmElValSet.h

#ifndef GDCMELVALSET_H
#define GDCMELVALSET_H

#include <stdio.h>    // FIXME For FILE on GCC only
#include <map>
#include "gdcmCommon.h"
#include "gdcmElValue.h"

////////////////////////////////////////////////////////////////////////////
// Container for a set of successfully parsed ElValues.
typedef map<TagKey, ElValue*> TagElValueHT;
typedef map<string, ElValue*> TagElValueNameHT;

class GDCM_EXPORT ElValSet {
	TagElValueHT tagHt;             // Both accesses with a TagKey or with a
	TagElValueNameHT NameHt;        // the DictEntry.Name are required.
public:	
	void Add(ElValue*);		
	void Print(ostream &);
	void PrintByName(ostream &);
	int  Write(FILE *fp);
	int  WriteAcr(FILE *fp);
	ElValue* GetElementByNumber(guint32 group, guint32 element);
	ElValue* GetElementByName  (string);
	string   GetElValueByNumber(guint32 group, guint32 element);
	string   GetElValueByName  (string);
	
	TagElValueHT & GetTagHt(void);	
	
	int SetElValueByNumber(string content, guint32 group, guint32 element);
	int SetElValueByName  (string content, string TagName);
	
	int SetElValueLengthByNumber(guint32 l, guint32 group, guint32 element);
	int SetElValueLengthByName  (guint32 l, string TagName);

};

#endif
