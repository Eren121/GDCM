// gdcmDocEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMDocEntry_H
#define GDCMDocEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmDictEntry.h"
class gdcmHeader;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmDocEntry
 * \brief   The dicom header of a Dicom file contains a set of such entries
 *          (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmDocEntry {
public:
   gdcmDocEntry(gdcmDictEntry*);

   /// Returns the Dicom Group number of the current Dicom Header Entry
   inline guint16      GetGroup(void)     { return entry->GetGroup();  };

   /// Returns the Dicom Element number of the current Dicom Header Entry
   inline guint16      GetElement(void)   { return entry->GetElement();};

   /// Returns the 'key' of the current Dicom Header Entry
   inline std::string  GetKey(void)       { return entry->GetKey();    };

   /// \brief Returns the 'Name' '(e.g. "Patient's Name") found in the Dicom
   /// Dictionnary of the current Dicom Header Entry
   inline std::string  GetName(void)      { return entry->GetName();   };

   /// \brief Returns the 'Value Representation' (e.g. "PN" : Person Name,
   /// "SL" : Signed Long), found in the Dicom Header or in the Dicom
   /// Dictionnary, of the current Dicom Header Entry
   inline std::string  GetVR(void)        { return entry->GetVR();     };

   /// \brief Returns the 'Value' (e.g. "Dupond Marcel") converted into a
   /// 'string', if it's stored as an integer in the Dicom Header of the
   /// current Dicom Header Entry
   inline std::string  GetValue(void)     { return value;              };

   /// \brief Returns the area value of the current Dicom Header Entry
   ///  when it's not string-translatable (e.g : a LUT table)         
   inline void *       GetVoidArea(void)  { return voidArea;           };

   /// \brief Returns offset (since the beginning of the file, including
   /// the File Pramble, if any) of the value of the current Dicom HeaderEntry
   /// \warning offset of the *value*, not of the Dicom Header Entry
   inline size_t       GetOffset(void)    { return Offset;             };

   /// \brief Returns the actual value length of the current Dicom Header Entry
   /// \warning this value is not *allways* the one stored in the Dicom Header
   ///          in case of well knowned bugs
   inline guint32 GetLength(void) { return UsableLength; };
    
   /// \brief Returns the 'read length' of the current Dicom Header Entry
   /// \warning this value is the one stored in the Dicom Header but not
   ///          mandatoryly the one thats's used (in case on SQ, or delimiters,
   ///          the usable length is set to zero)
   inline guint32 GetReadLength(void) { return ReadLength; };

   /// Sets the 'Value Representation' of the current Dicom Header Entry
   inline void SetVR(std::string v) { entry->SetVR(v); };    

   /// \brief Sets both 'Read Length' and 'Usable Length' of the current
   /// Dicom Header Entry
   inline void SetLength(guint32 l) { ReadLength=UsableLength=l;};
      
   // The following 3 members, for internal use only ! 
   
   /// \brief Sets only 'Read Length' (*not* 'Usable Length') of the current
   /// Dicom Header Entry
   inline void SetReadLength(guint32 l) { ReadLength   = l; };

   /// \brief Sets only 'Usable Length' (*not* 'Read Length') of the current
   /// Dicom Header Entry
   inline void SetUsableLength(guint32 l) { UsableLength = l; }; 
    
   /// Sets the value (string) of the current Dicom Header Entry
   inline void SetValue(std::string val) {  value = val; };

   /// Sets the value (non string) of the current Dicom Header Entry
   inline void SetVoidArea(void * area)  { voidArea = area;  };
   
   /// \brief   Sets the offset of the Dicom Element
   /// \warning use with caution !
   /// @param   of offset to be set
   inline void gdcmDocEntry::SetOffset(size_t of) { Offset = of; };

   /// Sets to TRUE the ImplicitVr flag of the current Dicom Element
   inline void gdcmDocEntry::SetImplicitVR(void) { ImplicitVR = true; };
 
   /// \brief Tells us if the current Dicom Element was checked as ImplicitVr
   /// @return true if the current Dicom Element was checked as ImplicitVr
   inline bool gdcmDocEntry::IsImplicitVR(void) { return ImplicitVR; };

   /// \brief Tells us if the VR of the current Dicom Element is Unkonwn
   /// @return true if the VR is unkonwn
   inline bool gdcmDocEntry::IsVRUnknown(void)
               { return entry->IsVRUnknown(); };

   /// \brief   Sets the DicEntry of the current Dicom Element
   /// @param   NewEntry pointer to the DictEntry
   inline void gdcmDocEntry::SetDictEntry(gdcmDictEntry *NewEntry)
               { entry = NewEntry; };

   /// \brief  Gets the DicEntry of the current Dicom Element
   /// @return The DicEntry of the current Dicom Element
   gdcmDictEntry * gdcmDocEntry::GetDictEntry(void) { return entry; }; 

   /// \brief Sets the print level for the Dicom Header Elements
   /// \note 0 for Light Print; 1 for 'medium' Print, 2 for Heavy
   void SetPrintLevel(int level) { printLevel = level; };

   void Print (std::ostream & os = std::cout); 
    
   /// Gets the depth level of a Dicom Header Entry embedded in a SeQuence
   inline int GetSQDepthLevel(void) { return (SQDepthLevel); };
         
   guint32 GetFullLength(void);
   
   void Copy(gdcmDocEntry *doc);
     
private:
   // FIXME: In fact we should be more specific and use :
   // friend gdcmDocEntry * gdcmHeader::ReadNextElement(void);
   friend class gdcmHeader;

   /// Sets the depth level of a Dicom Header Entry embedded in a SeQuence 
   inline void SetSQDepthLevel(int depthLevel) { SQDepthLevel = depthLevel; };
      
// Variables
   gdcmDictEntry *entry;

   /// \brief Updated from ReadLength, by FixFoungLentgh() for fixing a bug
   /// in the header or helping the parser going on    
   guint32 UsableLength; 
  
   /// \brief Length actually read on disk (before FixFoundLength). ReadLength
   /// will be updated only when FixFoundLength actually fixes a bug in the
   /// header, not when it performs a trick to help the Parser going on.
   guint32 ReadLength;

   /// \brief Even when reading explicit vr files, some elements happen to
   /// be implicit. Flag them here since we can't use the entry->vr without
   /// breaking the underlying dictionary.
   bool ImplicitVR;

   /// Offset from the begining of file for direct user access
   size_t Offset; 

   /// How many details are to be printed (value : 0,1,2)      
   int printLevel;
   
   /// Gives the depth level of elements inside SeQuences   
   int SQDepthLevel;
};

//-----------------------------------------------------------------------------
#endif
