// $Header: /cvs/public/gdcm/src/Attic/gdcmElValue.h,v 1.2 2003/03/12 21:33:20 frog Exp $

#ifndef GDCMELVALUE_H
#define GDCMELVALUE_H

#include "gdcmDictEntry.h"
class gdcmHeader;


///////////////////////////////////////////////////////////////////////////
// The dicom header of a Dicom file contains a set of such ELement VALUES
// (when successfuly parsed against a given Dicom dictionary)
class GDCM_EXPORT gdcmElValue {
private:
	gdcmDictEntry *entry;
	guint32 LgrElem;
	bool ImplicitVr;       // Even when reading explicit vr files, some
	                       // elements happen to be implicit. Flag them here
	                       // since we can't use the entry->vr without breaking
	                       // the underlying dictionary.
	void SetOffset(size_t of){ Offset = of; };
   // FIXME: In fact we should be more specific and use :
   //friend gdcmElValue * gdcmHeader::ReadNextElement(void);
   friend class gdcmHeader;
public:
	string  value;
	size_t Offset;     // Offset from the begining of file for direct user access
	
	gdcmElValue(gdcmDictEntry*);
	void SetDictEntry(gdcmDictEntry *NewEntry) { entry = NewEntry; };
	bool   IsVrUnknown(void) { return entry->IsVrUnknown(); };
	void SetImplicitVr(void) { ImplicitVr = true; };
	bool  IsImplicitVr(void) { return ImplicitVr; };
	
	guint16 GetGroup(void)   { return entry->GetGroup();  };
	guint16 GetElement(void) { return entry->GetElement();};
	string  GetKey(void)     { return entry->GetKey();    };
	string  GetName(void)    { return entry->GetName();   };
	string  GetVR(void)      { return entry->GetVR();     };
	void    SetVR(string v)  { entry->SetVR(v);           }; 
	void SetLength(guint32 l){ LgrElem = l;               };
	guint32 GetLength(void)  { return LgrElem;            };
	
	void SetValue(string val){ value = val; };
	string  GetValue(void)   { return value;};

	size_t  GetOffset(void)  { return Offset;};
};


#endif
