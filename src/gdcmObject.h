// gdcmObject.h
//-----------------------------------------------------------------------------
#ifndef GDCMOBJECT_H
#define GDCMOBJECT_H

#include <string>
#include <list>
#include "gdcmCommon.h"


//-----------------------------------------------------------------------------

typedef std::list<std::string> lstring;

//-----------------------------------------------------------------------------

class gdcmObject {
public:

   virtual std::string GetEntryByNumber(guint16 group, guint16 element)=0;
   virtual std::string GetEntryByName(TagName name)=0;

protected:

   //to modify (I don't know the list type) : both iterators (beginning and end)
   lstring::iterator beginIter;
   lstring::iterator endIter;

   //pointer to the data list
   lstring *objectData;

};

//-----------------------------------------------------------------------------
#endif
