// gdcmElValue.h
//-----------------------------------------------------------------------------
#ifndef GDCMELVALUE_H
#define GDCMELVALUE_H

#include "gdcmDictEntry.h"
class gdcmHeader;

#include <stdio.h>

//-----------------------------------------------------------------------------
/*
 * The dicom header of a Dicom file contains a set of such ELement VALUES
 * (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmElValue {
public:
   gdcmElValue(gdcmDictEntry*);
   
   inline guint16      GetGroup(void)     { return entry->GetGroup();  };
   inline guint16      GetElement(void)   { return entry->GetElement();};
   inline std::string  GetKey(void)       { return entry->GetKey();    };
   inline std::string  GetName(void)      { return entry->GetName();   };
   inline std::string  GetVR(void)        { return entry->GetVR();     };
   inline std::string  GetValue(void)     { return value;              };
   inline void *       GetVoidArea(void)  { return voidArea;           };
   inline size_t       GetOffset(void)    { return Offset;             };   
   inline guint32      GetLength(void)    { return UsableLength;       };   
   inline void         SetVR(std::string v)      { entry->SetVR(v);          };    
   inline void         SetLength(guint32 l)      { ReadLength=UsableLength=l;};
      
   // The following 3 members, for internal use only ! 
   inline void         SetReadLength(guint32 l)  { ReadLength   = l; };  	
   inline void         SetUsableLength(guint32 l){ UsableLength = l; };  	
   inline guint32      GetReadLength(void)       { return ReadLength;};
 	
   inline void         SetValue(std::string val) { value = val;      };
   inline void         SetVoidArea(void * area)  { voidArea = area;  };
   
   /**
    * \ingroup gdcmElValue
    * \brief   Sets the offset of the Dicom Element
    * \warning : use with caution !
    * @param   of offset to be set
    */
   inline void gdcmElValue::SetOffset(size_t of) { Offset = of; };

   /**
    * \ingroup gdcmElValue
    * \brief   Sets the DicEntry of the current Dicom Element
    * @param   NewEntry pointer to the DictEntry
    */ 
   inline void gdcmElValue::SetDictEntry(gdcmDictEntry *NewEntry) { 
      entry = NewEntry; 
   };

   /**
    * \ingroup gdcmElValue
    * \brief   Sets to TRUE the ImplicitVr flag of the current Dicom Element
    */
   inline void gdcmElValue::SetImplicitVr(void) { 
      ImplicitVr = true; 
   };
 
   /**
    * \ingroup gdcmElValue
    * \brief   tells us if the current Dicom Element was checked as ImplicitVr
    * @return true if the current Dicom Element was checked as ImplicitVr
    */ 
   inline bool  gdcmElValue::IsImplicitVr(void) { 
       return ImplicitVr; 
    };

   /**
    * \ingroup gdcmElValue
    * \brief   Gets the DicEntry of the current Dicom Element
    * @return  the DicEntry of the current Dicom Element
    */
   gdcmDictEntry * gdcmElValue::GetDictEntry(void) { 
      return entry;    
   }; 

   /**
    * \ingroup gdcmElValue
    * \brief   tells us if the VR of the current Dicom Element is Unkonwn
    * @return true if the VR is unkonwn
    */ 
   inline bool   gdcmElValue::IsVRUnknown(void) { 
      return entry->IsVRUnknown(); 
   };

private:
   // FIXME: In fact we should be more specific and use :
   // friend gdcmElValue * gdcmHeader::ReadNextElement(void);
   friend class gdcmHeader;

// Variables
   gdcmDictEntry *entry;
   guint32 UsableLength;  // Updated from ReadLength, by FixFoungLentgh()
                          // for fixing a bug in the header or helping
                          // the parser going on 
			  
   guint32 ReadLength;    // Length actually read on disk
                          // (before FixFoundLength)
                          // ReadLength will be updated only when
                          // FixFoundLength actually fixes a bug in the header,
                          // not when it performs a trick to help the Parser
                          // going on.
                          // *for internal* use only
	
   bool ImplicitVr;       // Even when reading explicit vr files, some
	                  // elements happen to be implicit. Flag them here
	                  // since we can't use the entry->vr without breaking
	                  // the underlying dictionary.
			  

   std::string  value;
   void *voidArea;  // unsecure memory area to hold 'non string' values 
                     // (ie : Lookup Tables, overlays)
   size_t Offset;    // Offset from the begining of file for direct user access
};

//-----------------------------------------------------------------------------
#endif
