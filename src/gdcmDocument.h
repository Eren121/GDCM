/*=========================================================================
 
  Program:   gdcm
  Module:    $RCSfile: gdcmDocument.h,v $
  Language:  C++
  Date:      $Date: 2004/06/28 09:30:58 $
  Version:   $Revision: 1.17 $
 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
 
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#ifndef GDCMDOCUMENT_H
#define GDCMDOCUMENT_H

#include "gdcmCommon.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmException.h"
#include "gdcmDictSet.h"
#include "gdcmDocEntry.h"

class gdcmValEntry;
class gdcmBinEntry;
class gdcmSeqEntry;

#include "gdcmDocEntrySet.h"
#include "gdcmElementSet.h"

#include <map>
#include <list>

//-----------------------------------------------------------------------------
typedef std::string VRKey;
typedef std::string VRAtr;
typedef std::map<VRKey, VRAtr> VRHT;    // Value Representation Hash Table

//-----------------------------------------------------------------------------
/**
 * \brief Derived by both gdcmHeader and gdcmDicomDir
 */
class GDCM_EXPORT gdcmDocument : public gdcmElementSet
{
friend class gdcmFile;
private:
   /// Public dictionary used to parse this header
   gdcmDict *RefPubDict;
   
   /// \brief Optional "shadow dictionary" (private elements) used to parse
   /// this header
   gdcmDict *RefShaDict;
   
   /// \brief Equals =1 if user wants to skip shadow groups while parsing
   /// (to save space)
   int IgnoreShadow;

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
   std::string Filename;

   /// \brief SWap code (e.g. Big Endian, Little Endian, Bad Big Endian,
   /// Bad Little Endian) according to the processor Endianity and
   /// what is written on disc.
   int sw;

   /// File Pointer, opened during Header parsing.
   FILE *fp;

   /// ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   FileType Filetype;  

   /// After opening the file, we read HEADER_LENGTH_TO_READ bytes.
   static const unsigned int HEADER_LENGTH_TO_READ; 

   /// \brief Elements whose value is longer than MAX_SIZE_LOAD_ELEMENT_VALUE
   /// are NOT loaded.
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;

   /// \brief Elements whose value is longer than  MAX_SIZE_PRINT_ELEMENT_VALUE
   /// are NOT printed.
   /// \todo Currently not used since collides with #define in
   ///       \ref gdcmDocEntry.cxx. See also
   ///       \ref gdcmDocument::SetMaxSizePrintEntry()
   static const unsigned int MAX_SIZE_PRINT_ELEMENT_VALUE;

   /// Will be set 1 if user asks to 'go inside' the 'sequences' (VR = "SQ")
   int enableSequences;

   /// \brief Amount of printed details for each Header Entry (Dicom Element):
   /// 0 : stands for the least detail level.
   int printLevel;
   
public:
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
   bool IsGivenTransferSyntax(const std::string & SyntaxToCheck);
   bool IsImplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRLittleEndianTransferSyntax(void);
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax(void);
   bool IsExplicitVRBigEndianTransferSyntax(void);
   bool IsJPEGBaseLineProcess1TransferSyntax(void);
   bool IsJPEGExtendedProcess2_4TransferSyntax(void);
   bool IsJPEGExtendedProcess3_5TransferSyntax(void);
   bool IsJPEGSpectralSelectionProcess6_8TransferSyntax(void);
   bool IsRLELossLessTransferSyntax(void);
   bool IsJPEGLossless(void);
   bool IsJPEG2000(void);
   bool IsDicomV3(void);

   FileType GetFileType(void);

// Read (used in gdcmFile, gdcmDicomDir)
   FILE *OpenFile(bool exception_on_error = false) throw(gdcmFileError);
   bool CloseFile(void);

// Write (used in gdcmFile, gdcmDicomDir)

    void Write(FILE * fp,FileType type); // New stuff, with recursive exploration

   gdcmValEntry * ReplaceOrCreateByNumber(std::string Value,
                                             guint16 Group, guint16 Elem);

   gdcmBinEntry * ReplaceOrCreateByNumber(void *voidArea, int lgth,
                                             guint16 Group, guint16 Elem);
   bool ReplaceIfExistByNumber (char *Value, guint16 Group, guint16 Elem);
   
   virtual void  *LoadEntryVoidArea       (guint16 Group, guint16 Element);
   virtual void  *LoadEntryVoidArea       (gdcmBinEntry*);
      
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
   gdcmDocument(std::string const & inFilename, 
                bool  exception_on_error = false, 
                bool  enable_sequences   = false,
                bool  ignore_shadow      = false);
   virtual ~gdcmDocument(void);
   
   void gdcmDocument::Parse7FE0 (void);   
   // Entry
   int CheckIfEntryExistByNumber(guint16 Group, guint16 Elem ); // int !
public:
   virtual std::string GetEntryByName    (TagName tagName);
   virtual std::string GetEntryVRByName  (TagName tagName);
   virtual std::string GetEntryByNumber  (guint16 group, guint16 element);
   virtual std::string GetEntryVRByNumber(guint16 group, guint16 element);
   virtual int     GetEntryLengthByNumber(guint16 group, guint16 element);
protected:
   virtual bool SetEntryByName  (std::string content, std::string tagName);
   virtual bool SetEntryByNumber(std::string content,
                                 guint16 group, guint16 element);
   virtual bool SetEntryByNumber(void *content, int lgth,
                                 guint16 group, guint16 element);
   virtual bool SetEntryLengthByNumber(guint32 length,
                                 guint16 group, guint16 element);

   virtual size_t GetEntryOffsetByNumber  (guint16 Group, guint16 Elem);
   virtual void  *GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem);   
   virtual bool   SetEntryVoidAreaByNumber(void *a, guint16 Group, guint16 Elem);

   virtual void UpdateShaEntries();

   // Header entry
   gdcmDocEntry *GetDocEntryByNumber  (guint16 group, guint16 element); 
   gdcmDocEntry *GetDocEntryByName    (std::string Name);

   gdcmValEntry *GetValEntryByNumber  (guint16 group, guint16 element); 
   gdcmBinEntry *GetBinEntryByNumber  (guint16 group, guint16 element); 

   void LoadDocEntrySafe(gdcmDocEntry *);

private:
   // Read
   long ParseDES(gdcmDocEntrySet *set, long offset, long l_max,bool delim_mode);
   long ParseSQ(gdcmSeqEntry *seq, long offset, long l_max, bool delim_mode); 
   
   void LoadDocEntry      (gdcmDocEntry *);
   void FindDocEntryLength(gdcmDocEntry *);
   void FindDocEntryVR    (gdcmDocEntry *);
   bool CheckDocEntryVR   (gdcmDocEntry *, VRKey);

   std::string GetDocEntryValue  (gdcmDocEntry *);
   std::string GetDocEntryUnvalue(gdcmDocEntry *);

   void SkipDocEntry          (gdcmDocEntry *);
   void SkipToNextDocEntry    (gdcmDocEntry *);

   void FixDocEntryFoundLength(gdcmDocEntry *, guint32);
   bool IsDocEntryAnInteger   (gdcmDocEntry *);

   guint32 FindDocEntryLengthOB();

   guint16 ReadInt16();
   guint32 ReadInt32();
   void    SkipBytes(guint32);
   guint32 ReadTagLength(guint16, guint16);
   guint32 ReadItemTagLength();
   guint32 ReadSequenceDelimiterTagLength();

   void Initialise();
   bool CheckSwap();
   void SwitchSwapToBigEndian();
   void SetMaxSizeLoadEntry(long);
   void SetMaxSizePrintEntry(long);

   // DocEntry related utilities
   gdcmDocEntry *ReadNextDocEntry   ();


   guint32 GenerateFreeTagKeyInGroup(guint16 group);

public:
// Accessors:
   /// Accessor to \ref printLevel
   inline void SetPrintLevel(int level) { printLevel = level; }

   /// Accessor to \ref filename
   inline std::string &GetFileName() { return Filename; }

   /// Accessor to \ref filename
   inline void SetFileName(const char* fileName) { Filename = fileName; }

   /// Accessor to \ref gdcmElementSet::tagHT
   inline TagDocEntryHT &GetEntry() { return tagHT; };

   /// 'Swap code' accessor (see \ref sw )
   inline int GetSwapCode() { return sw; }
   
   /// File pointer
   inline FILE * GetFP() { return fp; }

   bool operator<(gdcmDocument &document);

};

//-----------------------------------------------------------------------------
#endif
