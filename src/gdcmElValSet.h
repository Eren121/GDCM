// $Header: /cvs/public/gdcm/src/Attic/gdcmElValSet.h,v 1.8 2003/04/08 15:03:35 frog Exp $

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
//FIXME This is redundant with gdcmHeader::FileType enum. That sux !
public:	
	void Add(gdcmElValue*);	
	// TODO
	//void ReplaceOrCreate(gdcmElValue*);		
	void Print(ostream &);
	void PrintByName(ostream &);
	int  Write(FILE *fp, FileType type);

	gdcmElValue* GetElementByNumber(guint16 group, guint16 element);
	gdcmElValue* GetElementByName  (string);
	string   GetElValueByNumber(guint16 group, guint16 element);
	string   GetElValueByName  (string);
	
	TagElValueHT & GetTagHt(void);	
	
	int SetElValueByNumber(string content, guint16 group, guint16 element);
	int SetElValueByName  (string content, string TagName);
	
	int SetElValueLengthByNumber(guint32 l, guint16 group, guint16 element);
	int SetElValueLengthByName  (guint32 l, string TagName);

   guint32 GenerateFreeTagKeyInGroup(guint16 group);
	
private:
   void UpdateGroupLength(bool SkipSequence = false);
   void WriteElements(FileType type, FILE *);

};

#endif
