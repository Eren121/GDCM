// gdcmDocument.h
//-----------------------------------------------------------------------------
#ifndef GDCMDOCUMENT_H
#define GDCMDOCUMENT_H

#include "gdcmCommon.h"

#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmDocEntry.h"

#include <map>
#include <list>



//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

typedef std::multimap<TagKey, gdcmDocEntry *> TagDocEntryHT;
typedef std::pair<TagKey, gdcmDocEntry *> PairHT;
typedef std::pair<TagDocEntryHT::iterator,TagDocEntryHT::iterator> IterHT; 
/// for linking together the Elements
typedef std::list<gdcmDocEntry *> ListTag;

typedef std::string GroupKey;
typedef std::map<GroupKey, int> GroupHT;

//-----------------------------------------------------------------------------
/**
 * \brief used by both gdcmHeader and gdcmDicomDir
 */
class GDCM_EXPORT gdcmDocument : public gdcmElementSet
{
private:
   /// Public dictionary used to parse this header
   gdcmDict *RefPubDict;
   
   /// \brief Optional "shadow dictionary" (private elements) used to parse
   /// this header
   gdcmDict *RefShaDict;

   /// Equals 1 if a gdcmDocEntry was added post parsing 
   int wasUpdated;
   
   /// \brief Equals =1 if user wants to skip shadow groups while parsing
   /// (to save space)
   int ignoreShadow;

   /// \brief Size threshold above which an element value will NOT be loaded
   /// in memory (to avoid loading the image/volume itself). By default,
   /// this upper bound is fixed to 1024 bytes (which might look reasonable
   /// when one considers the definition of the various VR contents).
   guint32 MaxSizeLoadEntry;
   
   /// \brief Size threshold above which an element value will NOT be *printed*
   /// in order no to polute the screen output. By default, this upper bound
   /// is fixed to 64 bytes.
   guint32 MaxSizePrintEntry;

protected:
   /// Refering underlying filename.
   std::string filename; 

   /// \brief SWap code (e.g. Big Endian, Little Endian, Bad Big Endian,
   /// Bad Little Endian) according to the processor Endianity and
   /// what is written on disc.
   int sw;

   /// File Pointer, opened during Header parsing.
   FILE *fp;

   /// ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   FileType filetype;  

   /// After opening the file, we read HEADER_LENGTH_TO_READ bytes.
   static const unsigned int HEADER_LENGTH_TO_READ; 

   /// \brief Elements whose value is longer than MAX_SIZE_LOAD_ELEMENT_VALUE
   /// are NOT loaded.
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;

   /// \brief Elements whose value is longer than  MAX_SIZE_PRINT_ELEMENT_VALUE
   /// are NOT printed.
   static const unsigned int MAX_SIZE_PRINT_ELEMENT_VALUE;

   /// Hash Table (multimap), to provide fast access
   TagDocEntryHT tagHT; 

   /// Chained list, to keep the 'spacial' ordering
   ListTag listEntries; 

   /// Will be set 1 if user asks to 'go inside' the 'sequences' (VR = "SQ")
   int enableSequences;

   /// \brief Amount of printed details for each Header Entry (Dicom Element):
   /// 0 : stands for the least detail level.
   int printLevel;
   
public:
   

// Print
   /// Canonical Printing method (see also gdcmDocument::SetPrintLevel)
   virtual void Print        (std::ostream &os = std::cout) 
      {PrintEntry(os);};
   virtual void PrintEntry      (std::ostream &os = std::cout);

   // the 2 following will be merged
   virtual void PrintPubDict (std::ostream &os = std::cout);
   virtual void PrintShaDict (std::ostream &os = std::cout);

// Dictionnaries
   gdcmDict *GetPubDict(void);
   gdcmDict *GetShaDict(void);
   bool SetShaDict(gdcmDict *dict);
   bool SetShaDict(DictKey dictName);

// Informations contained in the parser
   virtual bool IsReadable(void);
   bool IsImplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRLittleEndianTransferSyntax(void);
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRBigEndianTransferSyntax(void);
   FileType GetFileType(void);

// Read (used in gdcmFile, gdcmDicomDir)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);

// Write (used in gdcmFile, gdcmDicomDir)
   virtual bool Write(FILE *, FileType);
   virtual void WriteEntryTagVRLength(gdcmDocEntry *tag,
                                       FILE *_fp, FileType type);
   virtual void WriteEntryValue(gdcmDocEntry *tag,FILE *_fp,FileType type);
   virtual bool WriteEntry(gdcmDocEntry *tag,FILE *_fp,FileType type);
   virtual bool WriteEntries(FILE *_fp,FileType type);
   void WriteEntriesDeprecated(FILE *_fp,FileType type); // JPR

   gdcmDocEntry * ReplaceOrCreateByNumber(std::string Value,
                                             guint16 Group, guint16 Elem);
   bool ReplaceIfExistByNumber (char *Value, guint16 Group, guint16 Elem);

// System access
   guint16 SwapShort(guint16);   // needed by gdcmFile
   guint32 SwapLong(guint32);    // needed by gdcmFile
   guint16 UnswapShort(guint16); // needed by gdcmFile
   guint32 UnswapLong(guint32);  // needed by gdcmFile

protected:
   // Constructor and destructor are protected to forbid end user 
   // to instanciate from this class gdcmDocument (only gdcmHeader and
   // gdcmDicomDir are meaningfull).
   gdcmDocument(bool exception_on_error  = false);
   gdcmDocument(const char *inFilename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false,
	      bool  ignore_shadow      = false);
   virtual ~gdcmDocument(void);
// Entry
   int CheckIfEntryExistByNumber(guint16 Group, guint16 Elem ); // int !
   virtual std::string GetEntryByName    (std::string tagName);
   virtual std::string GetEntryVRByName  (std::string tagName);
   virtual std::string GetEntryByNumber  (guint16 group, guint16 element);
   virtual std::string GetEntryVRByNumber(guint16 group, guint16 element);
   virtual int     GetEntryLengthByNumber(guint16 group, guint16 element);

   virtual bool SetEntryByName  (std::string content, std::string tagName);
   virtual bool SetEntryByNumber(std::string content,
                                 guint16 group, guint16 element);
   virtual bool SetEntryLengthByNumber(guint32 length,
                                 guint16 group, guint16 element);

   virtual size_t GetEntryOffsetByNumber  (guint16 Group, guint16 Elem);
   virtual void  *GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem);   
   virtual void  *LoadEntryVoidArea       (guint16 Group, guint16 Element);
   virtual bool   SetEntryVoidAreaByNumber(void *a, guint16 Group, guint16 Elem);

   virtual void UpdateShaEntries(void);

// Header entry
   gdcmDocEntry *GetDocEntryByNumber  (guint16 group, guint16 element); 
   gdcmDocEntry *GetDocEntryByName    (std::string Name);
   IterHT        GetDocEntrySameNumber(guint16 group, guint16 element); 
// IterHT        GetDocEntrySameName  (std::string Name); 

   void LoadDocEntrySafe(gdcmDocEntry *);

   void UpdateGroupLength(bool SkipSequence = false,
                          FileType type = ImplicitVR);

   void AddDocEntry       (gdcmDocEntry *);
   
      
private:
   // Read
   bool LoadDocEntries(bool exception_on_error = false) throw(gdcmFormatError);

   void LoadDocEntry      (gdcmDocEntry *);
   void FindDocEntryLength(gdcmDocEntry *);
   void FindDocEntryVR    (gdcmDocEntry *);
   bool CheckDocEntryVR   (gdcmDocEntry *, VRKey);

   std::string GetDocEntryValue  (gdcmDocEntry *);
   std::string GetDocEntryUnvalue(gdcmDocEntry *);

   void SkipDocEntry          (gdcmDocEntry *);
   void FixDocEntryFoundLength(gdcmDocEntry *, guint32);
   bool IsDocEntryAnInteger   (gdcmDocEntry *);

   guint32 FindDocEntryLengthOB(void);

   guint16 ReadInt16(void);
   guint32 ReadInt32(void);
   void    SkipBytes(guint32);

   void Initialise(void);
   bool CheckSwap(void);
   void SwitchSwapToBigEndian(void);
   void SetMaxSizeLoadEntry(long);
   void SetMaxSizePrintEntry(long);

   // DictEntry  related utilities
   gdcmDictEntry *GetDictEntryByName  (std::string Name);
   gdcmDictEntry *GetDictEntryByNumber(guint16, guint16);
   gdcmDictEntry *NewVirtualDictEntry(guint16 group, 
                                      guint16 element,
                                      std::string vr     = "unkn",
                                      std::string fourth = "unkn",
                                      std::string name   = "unkn");
   //gdcmDictEntry *NewVirtualDictEntry(gdcmDocEntry *); // never defined
   
   // DocEntry related utilities
   
   gdcmDocEntry *ReadNextDocEntry   (void);
   gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                     guint16 element);
   gdcmDocEntry *NewDocEntryByName  (std::string Name);
   
   // Deprecated (Not used) --> commented out
   //gdcmDocEntry *NewManualDocEntryToPubDict(std::string NewTagName,
   //                                         std::string VR);
   
   guint32 GenerateFreeTagKeyInGroup(guint16 group);

public:
// Accessors:
   /// Accessor to \ref printLevel
   void SetPrintLevel(int level) { printLevel = level; };

   /// Accessor to \ref filename
   inline std::string GetFileName(void) {return filename;}

   /// Accessor to \ref filename
   inline void SetFileName(char* fileName) {filename = fileName;}

   /// Accessor to \ref gdcmDocument::tagHT
   inline TagDocEntryHT &GetEntry(void) { return tagHT; };

   /// Accessor to \ref gdcmDocument::listEntries
   inline ListTag &GetListEntry(void) { return listEntries; };

   /// 'Swap code' accessor (see \ref sw )
   inline int GetSwapCode(void) { return sw; }
};

//-----------------------------------------------------------------------------
#endif
