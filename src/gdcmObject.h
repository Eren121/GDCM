// gdcmObject.h
//-----------------------------------------------------------------------------
#ifndef GDCMOBJECT_H
#define GDCMOBJECT_H

#include <string>
#include <list>
#include "gdcmCommon.h"
#include "gdcmDocEntry.h"
//#include "gdcmParser.h"
#include "gdcmDocument.h"
#include "gdcmDicomDirElement.h"
#include "gdcmSQItem.h"

//-----------------------------------------------------------------------------
class gdcmObject;
typedef std::list<gdcmObject *> ListContent;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmObject
 * \brief   Base object
 */
class GDCM_EXPORT gdcmObject : public gdcmSQItem
{
public:

   /**
    * \ingroup gdcmParser
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
   
   TagDocEntryHT GetEntry(void);
   void FillObject(std::list<gdcmElement> elemList);

protected:

 // constructor and destructor are protected to avoid end user to instanciate this class.
   gdcmObject(TagDocEntryHT *ptagHT, int depth = 0);
   virtual ~gdcmObject(void);

// variables :

///\brief detail level to be printed 
   int printLevel;

private:
};

//-----------------------------------------------------------------------------
#endif
