// gdcmDictEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICTENTRY_H
#define GDCMDICTENTRY_H

#include "gdcmCommon.h"

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDictEntry
 * \brief
 * the gdcmDictEntry in an element contained by the gdcmDict.
 * It contains :
 *  - the key referenced by the DICOM norm or the constructor (for private keys)
 *  - the corresponding name in english (it's equivalent to a label)
 *  - the owner group
 *  - etc.
 */
class GDCM_EXPORT gdcmDictEntry 
{
public:
   gdcmDictEntry(guint16 group, 
                 guint16 element,
                 std::string vr     = "Unknown",
                 std::string fourth = "Unknown",
                 std::string name   = "Unknown");
	
   static TagKey TranslateToKey(guint16 group, guint16 element);

   void SetVR(std::string);

   /**
    * \ingroup     gdcmDictEntry
    * \brief       tells if the V(alue) R(epresentation) is known (?!)
    *              
    * @return 
    */
   inline bool IsVRUnknown() {return vr == "Unknown"; }

   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Group Number of the current gdcmDictEntry
    * return the Dicom Group Number
    */
   inline guint16 GetGroup(void) { return group; }
  
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Element Number of the current gdcmDictEntry
    * return the Dicom Element Number
    */
   inline guint16 GetElement(void) { return element; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Value Representation of the current gdcmDictEntry
    * return the Dicom Value Representation
    */
   inline std::string GetVR(void) { return vr; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   sets the key of the current gdcmDictEntry
    * @param k New key to be set.
    */
   inline void SetKey(std::string k)  { key = k; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Fourth field of the current gdcmDictEntry
    * \warning NOT part of the Dicom Standard
    * \        May be REMOVED an any time
    * \        NEVER use it
    * return the Fourth field
    */
   inline std::string GetFourth(void) { return fourth; } 

   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Name of the current gdcmDictEntry
    * \        e.g. "Patient Name" for Dicom Tag (0x0010, 0x0010) 
    * return the Dicom Name
    */
   inline std::string GetName(void) { return name; } 
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   Gets the key of the current gdcmDictEntry
    * @return the key .
    */
   inline std::string GetKey(void) { return key; }

private:
   // FIXME : were are the group and element used except from building up
   //         a TagKey. If the answer is nowhere then there is no need
   //         to store the group and element independently.
   guint16 group;   // e.g. 0x0010
   guint16 element; // e.g. 0x0103
   std::string vr; // Value Representation i.e. some clue about the nature
	                // of the data represented e.g. "FD" short for
	                // "Floating Point Double"
	// CLEANME: find the official dicom name for this field !
   std::string fourth; // Fourth field containing some semantics.
                       //(Group Name abbr.)
   std::string name; // e.g. "Patient_Name"
   TagKey  key;      // Redundant with (group, element) but we add it
                     // on efficiency purposes.

	// DCMTK has many fields for handling a DictEntry (see below). What are the
	// relevant ones for gdcmlib ?
	//      struct DBI_SimpleEntry {
	//         Uint16 upperGroup;
	//         Uint16 upperElement;
	//         DcmEVR evr;
	//         const char* tagName;
	//         int vmMin;
	//         int vmMax;
	//         const char* standardVersion;
	//         DcmDictRangeRestriction groupRestriction;
	//         DcmDictRangeRestriction elementRestriction;
	//       };
};

//-----------------------------------------------------------------------------
#endif
