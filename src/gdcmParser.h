// gdcmParser.h
//-----------------------------------------------------------------------------
#ifndef GDCMPARSER_H
#define GDCMPARSER_H

#include "gdcmCommon.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmHeaderEntry.h"

#include <map>
#include <list>       // for linking together *all* the Dicom Elements

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

typedef std::multimap<TagKey, gdcmHeaderEntry *> TagHeaderEntryHT;
typedef std::pair<TagKey, gdcmHeaderEntry *> PairHT;
typedef std::pair<TagHeaderEntryHT::iterator,TagHeaderEntryHT::iterator> IterHT; 

typedef std::list<gdcmHeaderEntry *> ListTag; // for linking together the Elements

typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmHeader
 * \brief
 */
class GDCM_EXPORT gdcmParser
{
public:
   gdcmParser(bool exception_on_error = false);
   gdcmParser(const char *filename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false);
   virtual ~gdcmParser(void);

// Print
   /**
    * \ingroup gdcmParser
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) { printLevel = level; };
   virtual void PrintEntry(std::ostream &os = std::cout);
   virtual void PrintPubDict (std::ostream &os = std::cout);
   virtual void PrintShaDict (std::ostream &os = std::cout);

// Standard values
   inline std::string GetFileName(void) {return filename;}

// Dictionnaries
   gdcmDict *GetPubDict(void);
   gdcmDict *GetShaDict(void);
   bool SetShaDict(gdcmDict *dict);
   bool SetShaDict(DictKey dictName);

// Informations contained in the parser
   bool IsReadable(void);
   bool IsImplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRLittleEndianTransferSyntax(void);
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRBigEndianTransferSyntax(void);
   FileType GetFileType(void);

// Entries
   /**
    * \ingroup gdcmHeader
    * \brief   returns a ref to the Dicom Header H table (multimap)
    * return the Dicom Header H table
    */
   inline TagHeaderEntryHT &GetEntry(void) { return tagHT; };

   /**
    * \ingroup gdcmHeader
    * \brief   returns a ref to the Dicom Header chained list
    * return the Dicom Header chained list
    */
   inline ListTag &GetListEntry(void) { return listEntries; };

// Read (used in gdcmFile)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);

// Write (used in gdcmFile)
   virtual bool Write(FILE *, FileType);

   bool ReplaceOrCreateByNumber(std::string Value, guint16 Group, guint16 Elem);
   bool ReplaceOrCreateByNumber(     char  *Value, guint16 Group, guint16 Elem);
   bool ReplaceIfExistByNumber (     char  *Value, guint16 Group, guint16 Elem);

// System access
   inline int GetSwapCode(void) { return sw; }
   guint16 SwapShort(guint16); // needed by gdcmFile
   guint32 SwapLong(guint32);  // needed by gdcmFile
   guint16 UnswapShort(guint16); // needed by gdcmFile
   guint32 UnswapLong(guint32);  // needed by gdcmFile

protected:
// Entry
   int CheckIfEntryExistByNumber(guint16 Group, guint16 Elem ); // int !
   virtual std::string GetEntryByName    (std::string tagName);
   virtual std::string GetEntryVRByName  (std::string tagName);
   virtual std::string GetEntryByNumber  (guint16 group, guint16 element);
   virtual std::string GetEntryVRByNumber(guint16 group, guint16 element);

   virtual bool SetEntryByName  (std::string content, std::string tagName);
   virtual bool SetEntryByNumber(std::string content, guint16 group, guint16 element);
   virtual bool SetEntryLengthByNumber(guint32 l, guint16 group, guint16 element);

   virtual size_t GetEntryOffsetByNumber  (guint16 Group, guint16 Elem);
   virtual void  *GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem);   
   virtual void  *LoadEntryVoidArea       (guint16 Group, guint16 Element);
   virtual bool   SetEntryVoidAreaByNumber(void *a, guint16 Group, guint16 Elem);

   virtual void UpdateShaEntries(void);

// Header entry
   gdcmHeaderEntry *GetHeaderEntryByName  (std::string Name);
   gdcmHeaderEntry *GetHeaderEntryByNumber(guint16 group, guint16 element); 

   void LoadHeaderEntrySafe(gdcmHeaderEntry *);

   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteEntries(FileType type, FILE *);

// Variables
   FILE *fp;
   FileType filetype; // ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown

   static const unsigned int HEADER_LENGTH_TO_READ; 
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;

private:
   // Read
   void Parse(bool exception_on_error = false) throw(gdcmFormatError);

   void LoadHeaderEntries    (void);
   void LoadHeaderEntry      (gdcmHeaderEntry *);
   void AddHeaderEntry       (gdcmHeaderEntry *);
   void FindHeaderEntryLength(gdcmHeaderEntry *);
   void FindHeaderEntryVR    (gdcmHeaderEntry *);
   bool CheckHeaderEntryVR   (gdcmHeaderEntry *, VRKey);

   std::string GetHeaderEntryValue  (gdcmHeaderEntry *);
   std::string GetHeaderEntryUnvalue(gdcmHeaderEntry *);

   void SkipHeaderEntry          (gdcmHeaderEntry *);
   void FixHeaderEntryFoundLength(gdcmHeaderEntry *, guint32);
   bool IsHeaderEntryAnInteger   (gdcmHeaderEntry *);

   guint32 FindHeaderEntryLengthOB(void);

   guint16 ReadInt16(void);
   guint32 ReadInt32(void);
   void    SkipBytes(guint32);

   void Initialise(void);
   void CheckSwap(void);
   void SwitchSwapToBigEndian(void);
   void SetMaxSizeLoadEntry(long);

   // Dict
   gdcmDictEntry *GetDictEntryByName  (std::string Name);
   gdcmDictEntry *GetDictEntryByNumber(guint16, guint16);

   // HeaderEntry related utilities
   gdcmHeaderEntry *ReadNextHeaderEntry   (void);
   gdcmHeaderEntry *NewHeaderEntryByNumber(guint16 group, guint16 element);
   gdcmHeaderEntry *NewHeaderEntryByName  (std::string Name);
   gdcmDictEntry *NewVirtualDictEntry(guint16 group, guint16 element,
                                      std::string vr = "Unknown",
                                      std::string fourth = "Unknown",
                                      std::string name   = "Unknown");
   gdcmDictEntry *NewVirtualDictEntry(gdcmHeaderEntry *);

   // Deprecated (Not used)
   gdcmHeaderEntry *NewManualHeaderEntryToPubDict(std::string NewTagName,
                                                  std::string VR);
   guint32 GenerateFreeTagKeyInGroup(guint16 group);

   // Refering underlying filename.
   std::string filename; 

   // Public dictionary used to parse this header
   gdcmDict *RefPubDict;
   // Optional "shadow dictionary" (private elements) used to parse this header
   gdcmDict *RefShaDict;

   TagHeaderEntryHT tagHT; // H Table (multimap), to provide fast access
   ListTag listEntries;    // chained list, to keep the 'spacial' ordering 
   int enableSequences;

   // true if a gdcmHeaderEntry was added post parsing 
   int wasUpdated;

   // Swap code e.g. little, big, bad-big, bad-little endian). Warning:
   // this code is not fixed during header parsing.
   int sw;

   // Size treshold above which an element value will NOT be loaded in 
   // memory (to avoid loading the image/volume itself). By default,
   // this upper bound is fixed to 1024 bytes (which might look reasonable
   // when one considers the definition of the various VR contents).
   guint32 MaxSizeLoadEntry;

   // for PrintHeader
   int printLevel;
};

//-----------------------------------------------------------------------------
#endif
