// gdcmObject.h
//-----------------------------------------------------------------------------
#ifndef GDCMOBJECT_H
#define GDCMOBJECT_H

#include <string>
#include <list>
#include "gdcmCommon.h"
#include "gdcmHeaderEntry.h"
#include "gdcmParser.h"

//-----------------------------------------------------------------------------
class gdcmObject;
typedef std::list<gdcmObject *> ListContent;

//-----------------------------------------------------------------------------
class GDCM_EXPORT gdcmObject 
{
public:
   gdcmObject(ListTag::iterator begin,ListTag::iterator end);
   ~gdcmObject(void);

   void SetPrintLevel(int level) { printLevel = level; };
   virtual void Print(std::ostream &os = std::cout);

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   TagHeaderEntryHT GetEntry(void);
   ListTag GetListEntry(void);
   ListTag::iterator debut(void) { return(beginObj);}
   ListTag::iterator fin  (void) { return(endObj);  }
   
protected:
   
   ListTag::iterator beginObj;
   ListTag::iterator endObj;
   int printLevel;

private:
};

//-----------------------------------------------------------------------------
#endif
