// gdcmElValue.h

#ifndef GDCMELVALUE_H
#define GDCMELVALUE_H

#include "gdcmDictEntry.h"


///////////////////////////////////////////////////////////////////////////
// The dicom header of a Dicom file contains a set of such ELement VALUES
// (when successfuly parsed against a given Dicom dictionary)
class GDCM_EXPORT ElValue {
private:
	gdcmDictEntry *entry;
	guint32 LgrElem;
	bool ImplicitVr;       // Even when reading explicit vr files, some
	                       // elements happen to be implicit. Flag them here
	                       // since we can't use the entry->vr without breaking
	                       // the underlying dictionary.
public:
	string  value;
	size_t Offset;     // Offset from the begining of file for direct user access
	
	ElValue(gdcmDictEntry*);
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
	
	// Question : SetLength est public 
	// (sinon, on ne pourrait pas l'appeler dans ElValSet)
	// alors que *personne* ne devrait s'en servir !
	// c'est *forcément* la lgr de la string 'value', non?

	void SetValue(string val){ value = val; };
	string  GetValue(void)   { return value;};

	void SetOffset(size_t of){ Offset = of; };
	size_t  GetOffset(void)  { return Offset;};
	// Question : SetOffset est public ...
	// Quel utilisateur serait ammené à modifier l'Offset ?
};

#endif
