// $Header: /cvs/public/gdcm/src/Attic/gdcmElValue.h,v 1.10 2004/01/12 13:12:28 regrain Exp $

#ifndef GDCMELVALUE_H
#define GDCMELVALUE_H

#include "gdcmDictEntry.h"
class gdcmHeader;

#include <stdio.h>

/*
 * The dicom header of a Dicom file contains a set of such ELement VALUES
 * (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmElValue {
private:
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
	
   bool ImplicitVr;  // Even when reading explicit vr files, some
	                  // elements happen to be implicit. Flag them here
	                  // since we can't use the entry->vr without breaking
	                  // the underlying dictionary.
			  
   void SetOffset(size_t of){ Offset = of; };

   // FIXME: In fact we should be more specific and use :
   // friend gdcmElValue * gdcmHeader::ReadNextElement(void);
   friend class gdcmHeader;

public:
   std::string  value;
   void * voidArea;  // unsecure memory area to hold 'non string' values 
                     // (ie : Lookup Tables, overlays)
   size_t Offset;    // Offset from the begining of file for direct user access
	
   gdcmElValue(gdcmDictEntry*);
   void SetDictEntry(gdcmDictEntry *NewEntry) { entry = NewEntry; };
   bool   IsVrUnknown(void) { return entry->IsVrUnknown(); };
   void SetImplicitVr(void) { ImplicitVr = true; };
   bool  IsImplicitVr(void) { return ImplicitVr; };
	
   gdcmDictEntry * GetDictEntry(void) { return entry;    };
   
   guint16      GetGroup(void)     { return entry->GetGroup();  };
   guint16      GetElement(void)   { return entry->GetElement();};
   std::string  GetKey(void)       { return entry->GetKey();    };
   std::string  GetName(void)      { return entry->GetName();   };
   std::string  GetVR(void)        { return entry->GetVR();     };
   std::string  GetValue(void)     { return value;              };
   void *       GetVoidArea(void)  { return voidArea;           };
   size_t       GetOffset(void)    { return Offset;             };   
   guint32      GetLength(void)    { return UsableLength;       };
   // for internal use only!
   guint32      GetReadLength(void){ return ReadLength;         };
   
   void         SetVR(std::string v)      { entry->SetVR(v);          };    
   void         SetLength(guint32 l)      { ReadLength=UsableLength=l;};   
   // The following 2 members, for internal use only ! 
   void         SetReadLength(guint32 l)  { ReadLength = l;           };  	
   void         SetUsableLength(guint32 l){ UsableLength = l;         };  	
 	
   void         SetValue(std::string val) { value = val;       };
   void         SetVoidArea(void * area)  { voidArea = area;   };	
 
};

#endif
