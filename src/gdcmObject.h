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
class gdcmObject {
public:
   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   ListTag::iterator beginObj;
   ListTag::iterator endObj;
   
protected:
};

//-----------------------------------------------------------------------------
#endif
