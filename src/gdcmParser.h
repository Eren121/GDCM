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
 * \defgroup gdcmParser
 * \brief used by both gdcmHeader and gdcmDicomDir
 */
class GDCM_EXPORT gdcmParser
{
public:

// Print
   /**
    * \ingroup gdcmParser
    * \brief   Sets the print level for the Dicom Header 
    * \note    0 for Light Print; 1 for 'medium' Print, 2 for Heavy
    */
   void SetPrintLevel(int level) 
      { printLevel = level; };
   /**
    * \ingroup gdcmParser
    * \brief   canonical Printer 
    * \sa    SetPrintLevel
    */   
   virtual void Print        (std::ostream &os = std::cout) 
      {PrintEntry(os);};
   virtual void PrintEntry   (std::ostream &os = std::cout);
   virtual void PrintPubDict (std::ostream &os = std::cout);
   virtual void PrintShaDict (std::ostream &os = std::cout);

// Standard values
   /**
    * \ingroup gdcmParser
    * \brief   Gets the external File Name 
    */
   inline std::string GetFileName(void) 
      {return filename;}

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

// Read (used in gdcmFile, gdcmDicomDir)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);

// Write (used in gdcmFile, gdcmDicomDir)
   virtual bool Write(FILE *, FileType);

   gdcmHeaderEntry * ReplaceOrCreateByNumber(std::string Value, guint16 Group, guint16 Elem);
   gdcmHeaderEntry * ReplaceOrCreateByNumber(     char  *Value, guint16 Group, guint16 Elem);
   bool ReplaceIfExistByNumber (     char  *Value, guint16 Group, guint16 Elem);

// System access
   /**
    * \ingroup gdcmHeader
    * \brief   returns the 'swap code' 
    *          (Big Endian, Little Endian, 
    *          Bad Big Endian, Bad Little Endian)
    *          according to the processor Endianity and what's written on disc
    * return 
    */
   inline int GetSwapCode(void) { return sw; }
   
   guint16 SwapShort(guint16);   // needed by gdcmFile
   guint32 SwapLong(guint32);    // needed by gdcmFile
   guint16 UnswapShort(guint16); // needed by gdcmFile
   guint32 UnswapLong(guint32);  // needed by gdcmFile

protected:
// constructor and destructor are protected to forbid end user to instanciate
// class gdcmParser (only gdcmHeader and gdcmDicomDir are meaningfull)
   gdcmParser(bool exception_on_error  = false);
   gdcmParser(const char *inFilename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false,
	      bool  ignore_shadow      = false);
   virtual ~gdcmParser(void);
// Entry
   int CheckIfEntryExistByNumber(guint16 Group, guint16 Elem ); // int !
   virtual std::string GetEntryByName    (std::string tagName);
   virtual std::string GetEntryVRByName  (std::string tagName);
   virtual std::string GetEntryByNumber  (guint16 group, guint16 element);
   virtual std::string GetEntryVRByNumber(guint16 group, guint16 element);
   virtual int     GetEntryLengthByNumber(guint16 group, guint16 element);

   virtual bool SetEntryByName  (std::string content, std::string tagName);
   virtual bool SetEntryByNumber(std::string content,  guint16 group, guint16 element);
   virtual bool SetEntryLengthByNumber(guint32 length, guint16 group, guint16 element);

   virtual size_t GetEntryOffsetByNumber  (guint16 Group, guint16 Elem);
   virtual void  *GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem);   
   virtual void  *LoadEntryVoidArea       (guint16 Group, guint16 Element);
   virtual bool   SetEntryVoidAreaByNumber(void *a, guint16 Group, guint16 Elem);

   virtual void UpdateShaEntries(void);

// Header entry
   gdcmHeaderEntry *GetHeaderEntryByNumber  (guint16 group, guint16 element); 
   gdcmHeaderEntry *GetHeaderEntryByName    (std::string Name);
   IterHT           GetHeaderEntrySameNumber(guint16 group, guint16 element); 
// IterHT           GetHeaderEntrySameName  (std::string Name); 

   void LoadHeaderEntrySafe(gdcmHeaderEntry *);

   void UpdateGroupLength(bool SkipSequence = false, FileType type = ImplicitVR);
   void WriteEntry(gdcmHeaderEntry *tag,FILE *_fp,FileType type);
   void WriteEntries(FILE *_fp,FileType type);
   void WriteEntriesDeprecated(FILE *_fp,FileType type); // JPR

   void AddHeaderEntry       (gdcmHeaderEntry *);
   
   /**
    * \ingroup gdcmParser
    * \brief   Set the external File Name 
    */
   inline void SetFileName(char* fileName) 
      {filename = fileName;}
      
// Variables
   /**
   * \brief File Pointer, open during Header parsing
   */
   FILE *fp;
   /**
   * \brief ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   */
   FileType filetype;  

/// after opening the file, we read HEADER_LENGTH_TO_READ bytes.
   static const unsigned int HEADER_LENGTH_TO_READ; 
/// Elements whose value is longer than MAX_SIZE_LOAD_ELEMENT_VALUE are NOT loaded
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;
/// Elements whose value is longer than  MAX_SIZE_PRINT_ELEMENT_VALUE are NOT printed  
   static const unsigned int MAX_SIZE_PRINT_ELEMENT_VALUE;

protected:
   /**
   * \brief H Table (multimap), to provide fast access
   */
   TagHeaderEntryHT tagHT; 
   /**
   * \brief chained list, to keep the 'spacial' ordering
   */
   ListTag listEntries; 
   /**
   * \brief will be set 1 if user asks to 'go inside' the 'sequences' (VR = "SQ")
   */    
   int enableSequences;
   /**
   * \brief amount of printed details for each Header Entry (Dicom Element)
   *  0 : the least 
   */    
   int printLevel;
   
   /** 
   * \brief For some ACR-NEMA images, it's *not* 7fe0 ... 
   */   
   guint16 GrPixel;
   
   /** 
   * \brief For some ACR-NEMA images, it's *not* 0010 ... 
   */    
   guint16 NumPixel;
   /**
   * \brief some files may contain icons; GrPixel,NumPixel appears several times
   * Let's remember how many times!
   */
   int countGrPixel;
   /**
   * \brief = true when the 'pixel Element' is reached during writting process
   */   
   bool itsTimeToWritePixels;
      
private:
   // Read
   bool ParseHeader(bool exception_on_error = false) throw(gdcmFormatError);

   void LoadHeaderEntries    (void);
   void LoadHeaderEntry      (gdcmHeaderEntry *);
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
   //gdcmDictEntry *NewVirtualDictEntry(gdcmHeaderEntry *); // never defined
   
   // HeaderEntry related utilities
   
   gdcmHeaderEntry *ReadNextHeaderEntry   (void);
   gdcmHeaderEntry *NewHeaderEntryByNumber(guint16 group, 
                                           guint16 element);
   gdcmHeaderEntry *NewHeaderEntryByName  (std::string Name);
   
   // Deprecated (Not used) --> commented out
   //gdcmHeaderEntry *NewManualHeaderEntryToPubDict(std::string NewTagName,
   //                                               std::string VR);
   
   guint32 GenerateFreeTagKeyInGroup(guint16 group);

   /**
   * \brief Refering underlying filename.
   */
   std::string filename; 

   /**
   * \brief Public dictionary used to parse this header
   */
   gdcmDict *RefPubDict;
   
   /**
   * \brief Optional "shadow dictionary" (private elements) used to parse this header
   */
   gdcmDict *RefShaDict;

   /**
   * \brief = 1 if a gdcmHeaderEntry was added post parsing 
   */   
   int wasUpdated;
   
   /**
   * \brief =1 if user wants to skip shadow groups while parsing (to save space)
   */
   int ignoreShadow;
   
   /**
   * \brief Swap code e.g. little, big, bad-big, bad-little endian). 
   * \warning : this code is not fixed during header parsing.      
   */
   int sw;
   /**
   * \brief Size threshold above which an element value will NOT be loaded in 
   *       memory (to avoid loading the image/volume itself). By default,
   *       this upper bound is fixed to 1024 bytes (which might look reasonable
   *       when one considers the definition of the various VR contents).
   */
   guint32 MaxSizeLoadEntry;
   
   /**
   * \brief Size threshold above which an element value will NOT be *printed* in
   *        order no to polute the screen output. 
   *        By default, this upper bound is fixed to 64 bytes.
   */   
   guint32 MaxSizePrintEntry;
   
};

//-----------------------------------------------------------------------------
#endif
