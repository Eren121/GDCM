// gdcmObject.h
//-----------------------------------------------------------------------------
#ifndef GDCMOBJECT_H
#define GDCMOBJECT_H

#include <string>
#include <list>
#include "gdcmCommon.h"
#include "gdcmHeaderEntry.h"
#include "gdcmParser.h"
#include "gdcmDicomDirElement.h"
//-----------------------------------------------------------------------------
class gdcmObject;
typedef std::list<gdcmObject *> ListContent;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmObject
 * \brief   Base object
 */
class GDCM_EXPORT gdcmObject 
{
public:
   /**
    * \ingroup gdcmParser
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
   virtual void Print(std::ostream &os = std::cout);

   std::string GetEntryByNumber(guint16 group, guint16 element);
   std::string GetEntryByName(TagName name);
   bool SetEntryByNumber(std::string val,guint16 group,guint16 element);
   
   TagHeaderEntryHT GetEntry(void);
   ListTag GetListEntry(void);
   
   void ResetBoundaries(int flag);
/**
 * \ingroup gdcmObject
 * \brief   returns an iterator on the first Header Entry (i.e Dicom Element),
 *          inside the DICOMDIR chained list,
 *          related to this 'Object' 
 * @return
 */
   ListTag::iterator debut(void) 
      { return(beginObj);}
   /**
    * \ingroup gdcmObject
    * \brief   returns an iterator on the last Header Entry (i.e Dicom Element),
    *          inside the DICOMDIR chained list,
    *          related to this 'Object' 
    * @return
    */
    ListTag::iterator fin  (void) 
       { return(endObj);  }

protected:
 // constructor and destructor are protected to avoid end user to instanciate this class.
   gdcmObject(ListTag::iterator begin,ListTag::iterator end,
              TagHeaderEntryHT *ptagHT, ListTag *plistEntries); 
   virtual ~gdcmObject(void);

   void FillObject(std::list<gdcmElement> elemList);

/**
* \brief iterator on the first Header Entry (i.e Dicom Element), 
*       inside the DICOMDIR chained list,
*       related to this 'Object'
*/   
   ListTag::iterator beginObj;
/**
* \brief iterator on the last Header Entry (i.e Dicom Element), 
*       inside the DICOMDIR chained list,
*       related to this 'Object'
*/
   ListTag::iterator endObj;
/**
* \brief pointer to the HTable of the gdcmParser,
*        (because we don't know it within any gdcmObject) 
*/
  TagHeaderEntryHT *ptagHT;
/**
* \brief pointer to the Chained List of the gdcmParser,
*        (because we don't know it within any gdcmObject)  
*/  
   ListTag *plistEntries;
/**
* \brief detail level to be printed
*/   
   int printLevel;
   
   /**
   * \brief used to pass variables to FillObject function
   *        Works as 'global' variable
   */
   std::list<gdcmHeaderEntry *>::iterator debInsertion, finInsertion, i,j;

private:

};

//-----------------------------------------------------------------------------
#endif
