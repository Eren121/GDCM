// gdcmHeaderEntry.h
//-----------------------------------------------------------------------------
#ifndef GDCMHeaderEntry_H
#define GDCMHeaderEntry_H

#include <iostream>
#include <stdio.h>

#include "gdcmDictEntry.h"
class gdcmHeader;

//-----------------------------------------------------------------------------
/*
 * The dicom header of a Dicom file contains a set of such entries
 * (when successfuly parsed against a given Dicom dictionary)
 */
class GDCM_EXPORT gdcmHeaderEntry {
public:
   gdcmHeaderEntry(gdcmDictEntry*);
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the Dicom Group number of the current Dicom Header Entry
    * @return 
    */    
   inline guint16      GetGroup(void)     { return entry->GetGroup();  };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the Dicom Element number of the current Dicom Header Entry
    * @return 
    */ 
   inline guint16      GetElement(void)   { return entry->GetElement();};
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the 'key' of the current Dicom Header Entry
    * @return 
    */
   inline std::string  GetKey(void)       { return entry->GetKey();    };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the 'Name' '(e.g. "Patient's Name")
    *          found in the Dicom Dictionnary
    *          of the current Dicom Header Entry
    * @return
    */ 
   inline std::string  GetName(void)      { return entry->GetName();   };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the 'Value Representation' 
    *          (e.g. "PN" : Person Name, "SL" : Signed Long),
    *          found in the Dicom Header or in the Dicom Dictionnary,
    *          of the current Dicom Header Entry
    * @return
    */
   inline std::string  GetVR(void)        { return entry->GetVR();     };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the 'Value' (e.g. "Dupond Marcel")
    *          converted into a 'string', if it's stored as an integer
    *          in the Dicom Header
    *          of the current Dicom Header Entry
    * @return
    */ 
   inline std::string  GetValue(void)     { return value;              };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the area value of the current Dicom Header Entry
    *          when it's not string-translatable
    *          (e.g : a LUT table)         
    * @return
    */    
   inline void *       GetVoidArea(void)  { return voidArea;           };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns offset (since the beginning of the file,
    *          including the File Pramble, if any)
    *          of the value of the current Dicom Header Entry
    * \warning : offset of the *value*, not of the Dicom Header Entry
    *  
    * @return
    */    
   inline size_t       GetOffset(void)    { return Offset;             };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the actual value length of the current Dicom Header Entry
    * \warning this value is not *allways* the one stored in the Dicom Header
    *          in case on well knowned buggs
    * @return
    */        
   inline guint32      GetLength(void)    { return UsableLength;       };
    
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   returns the 'read length' of the current Dicom Header Entry
    * \warning this value is the one stored in the Dicom Header
    *          but not mandatoryly the one thats's used
    *          (in case on SQ, or delimiters, the usable length is set to zero)
    * @return
    */     
   inline guint32      GetReadLength(void) { return ReadLength;};

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the 'Value Representation' of the current Dicom Header Entry
    */   
   inline void         SetVR(std::string v)      { entry->SetVR(v);          };    
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets both 'Read Length' and 'Usable Length'
    *          of the current Dicom Header Entry
    */ 
   inline void         SetLength(guint32 l)      { ReadLength=UsableLength=l;};
      
   // The following 3 members, for internal use only ! 
   
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets only 'Read Length' (*not* 'Usable Length')
    *          of the current Dicom Header Entry
    */ 
   inline void         SetReadLength(guint32 l)  { ReadLength   = l; };
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets only 'Usable Length' (*not* 'Read Length')
    *          of the current Dicom Header Entry
    */      	
   inline void         SetUsableLength(guint32 l){ UsableLength = l; }; 
    	
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the value (string)
    *          of the current Dicom Header Entry
    */  	
   inline void         SetValue(std::string val) { value = val;      };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the value (non string)
    *          of the current Dicom Header Entry
    */ 
    inline void         SetVoidArea(void * area)  { voidArea = area;  };
   
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the offset of the Dicom Element
    * \warning : use with caution !
    * @param   of offset to be set
    */
   inline void gdcmHeaderEntry::SetOffset(size_t of) { Offset = of; };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets to TRUE the ImplicitVr flag of the current Dicom Element
    */
   inline void gdcmHeaderEntry::SetImplicitVR(void) { ImplicitVR = true; };
 
   /**
    * \ingroup gdcmHeaderEntry
    * \brief   tells us if the current Dicom Element was checked as ImplicitVr
    * @return true if the current Dicom Element was checked as ImplicitVr
    */ 
   inline bool  gdcmHeaderEntry::IsImplicitVR(void) { return ImplicitVR; };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   tells us if the VR of the current Dicom Element is Unkonwn
    * @return true if the VR is unkonwn
    */ 
   inline bool   gdcmHeaderEntry::IsVRUnknown(void) { return entry->IsVRUnknown(); };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the DicEntry of the current Dicom Element
    * @param   NewEntry pointer to the DictEntry
    */ 
   inline void gdcmHeaderEntry::SetDictEntry(gdcmDictEntry *NewEntry) { 
      entry = NewEntry;
   };

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Gets the DicEntry of the current Dicom Element
    * @return  the DicEntry of the current Dicom Element
    */
   gdcmDictEntry * gdcmHeaderEntry::GetDictEntry(void) { return entry; }; 

   /**
    * \ingroup gdcmHeaderEntry
    * \brief   Sets the print level for the Dicom Header Elements
    * \note 0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) { printLevel = level; };
   void Print (std::ostream & os = std::cout); 

private:
   // FIXME: In fact we should be more specific and use :
   // friend gdcmHeaderEntry * gdcmHeader::ReadNextElement(void);
   
   friend class gdcmHeader;

// Variables
   gdcmDictEntry *entry;
   /// Updated from ReadLength, by FixFoungLentgh()
   /// for fixing a bug in the header 
   /// or helping the parser going on    
   guint32 UsableLength; 
			  
   /// Length actually read on disk
   /// (before FixFoundLength)
   /// ReadLength will be updated only when
   /// FixFoundLength actually fixes a bug in the header,
   /// not when it performs a trick to help the Parser
   /// going on.
   /// *for internal* use only
   guint32 ReadLength;

   /// Even when reading explicit vr files, some
   /// elements happen to be implicit. Flag them here
   /// since we can't use the entry->vr without breaking
   /// the underlying dictionary.	
   bool ImplicitVR;
			  
   /// Header Entry value, stores as a std::string (VR will be used, later, to decode)
   std::string  value;

   /// unsecure memory area to hold 'non string' values 
   /// (ie : Lookup Tables, overlays)   
   void *voidArea;
   /// Offset from the begining of file for direct user access		     
   size_t Offset; 
   /// How many details are to be printed (value : 0,1,2)      
   int printLevel;
};

//-----------------------------------------------------------------------------
#endif
