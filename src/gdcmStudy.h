// gdcmStudy.h
//-----------------------------------------------------------------------------
#ifndef GDCMSTUDY_H
#define GDCMSTUDY_H

#include "gdcmObject.h"
#include "gdcmSequence.h"

//-----------------------------------------------------------------------------

typedef std::list<gdcmSequence> lSequence;

//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmStudy : public gdcmObject {
public:

   gdcmStudy();
   ~gdcmStudy();

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);

   inline lSequence GetSequences() {return sequences;};

private:

   lSequence sequences;

};

//-----------------------------------------------------------------------------
#endif
