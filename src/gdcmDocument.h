/*=========================================================================
 
  Program:   gdcm
  Module:    $RCSfile: gdcmDocument.h,v $
  Language:  C++
  Date:      $Date: 2004/09/23 10:17:26 $
  Version:   $Revision: 1.41 $
 
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

   /// \brief Size threshold above which an element value will NOT be loaded
   /// in memory (to avoid loading the image/volume itself). By default,
   /// this upper bound is fixed to 1024 bytes (which might look reasonable
   /// when one considers the definition of the various VR contents).
   uint32_t MaxSizeLoadEntry;
   
   /// \brief Size threshold above which an element value will NOT be *printed*
   /// in order no to polute the screen output. By default, this upper bound
   /// is fixed to 64 bytes.
   uint32_t MaxSizePrintEntry;   

protected:
   /// Refering underlying filename.
   std::string Filename;

   /// \brief SWap code (e.g. Big Endian, Little Endian, Bad Big Endian,
   /// Bad Little Endian) according to the processor Endianity and
   /// what is written on disc.
   int SwapCode;

   /// File Pointer, opened during Header parsing.
   FILE *Fp;

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
   int EnableSequences;

   /// \brief Amount of printed details for each Header Entry (Dicom Element):
   /// 0 : stands for the least detail level.
   int PrintLevel;
   
public:
// the 2 following will be merged
   virtual void PrintPubDict (std::ostream &os = std::cout);
   virtual void PrintShaDict (std::ostream &os = std::cout);

// Dictionnaries
   gdcmDict *GetPubDict();
   gdcmDict *GetShaDict();
   bool SetShaDict(gdcmDict *dict);
   bool SetShaDict(DictKey const & dictName);

// Informations contained in the parser
   virtual bool IsReadable();
   bool IsGivenTransferSyntax(std::string const & syntaxToCheck);
   bool IsImplicitVRLittleEndianTransferSyntax();
   bool IsExplicitVRLittleEndianTransferSyntax();
   bool IsDeflatedExplicitVRLittleEndianTransferSyntax();
   bool IsExplicitVRBigEndianTransferSyntax();
   bool IsJPEGBaseLineProcess1TransferSyntax();
   bool IsJPEGExtendedProcess2_4TransferSyntax();
   bool IsJPEGExtendedProcess3_5TransferSyntax();
   bool IsJPEGSpectralSelectionProcess6_8TransferSyntax();
   bool IsRLELossLessTransferSyntax();
   bool IsJPEGLossless();
   bool IsJPEG2000();
   bool IsDicomV3();

   FileType GetFileType();

   FILE* OpenFile();
   bool CloseFile();

   void Write(FILE* fp, FileType type);

   gdcmValEntry* ReplaceOrCreateByNumber(std::string const & value,
                                         uint16_t group, uint16_t elem,
                                         std::string const & VR ="unkn");
   
   gdcmBinEntry* ReplaceOrCreateByNumber(uint8_t* binArea, int lgth,
                                         uint16_t group, uint16_t elem,
                                         std::string const & VR="unkn");

   gdcmSeqEntry* ReplaceOrCreateByNumber(uint16_t group, uint16_t elem);

   bool ReplaceIfExistByNumber ( std::string const & value,
                                 uint16_t group,
                                 uint16_t elem );
   
   virtual void* LoadEntryBinArea(uint16_t group, uint16_t elem);
   virtual void* LoadEntryBinArea(gdcmBinEntry* entry);
      
   // System access (meaning endian related !?)
   uint16_t SwapShort(uint16_t);   // needed by gdcmFile
   uint32_t SwapLong(uint32_t);    // needed by gdcmFile
   uint16_t UnswapShort(uint16_t); // needed by gdcmFile
   uint32_t UnswapLong(uint32_t);  // needed by gdcmFile

protected:
   // Constructor and destructor are protected to forbid end user 
   // to instanciate from this class gdcmDocument (only gdcmHeader and
   // gdcmDicomDir are meaningfull).
   gdcmDocument();
   gdcmDocument( std::string const & filename );
   virtual ~gdcmDocument();
   
   void Parse7FE0 ();   
   // Entry
   bool CheckIfEntryExistByNumber(uint16_t group, uint16_t elem );
public:
   virtual std::string GetEntryByName    (TagName const & tagName);
   virtual std::string GetEntryVRByName  (TagName const & tagName);
   virtual std::string GetEntryByNumber  (uint16_t group, uint16_t elem);
   virtual std::string GetEntryVRByNumber(uint16_t group, uint16_t elem);
   virtual int     GetEntryLengthByNumber(uint16_t group, uint16_t elem);
//protected:
   virtual bool SetEntryByName  (std::string const & content, 
                                 std::string const & tagName);
   virtual bool SetEntryByNumber(std::string const & content,
                                 uint16_t group, uint16_t element);
   virtual bool SetEntryByNumber(uint8_t* content, int lgth,
                                 uint16_t group, uint16_t element);
   virtual bool SetEntryLengthByNumber(uint32_t length,
                                       uint16_t group, uint16_t element);

   virtual size_t GetEntryOffsetByNumber (uint16_t group, uint16_t elem);
   virtual void* GetEntryBinAreaByNumber(uint16_t group, uint16_t elem);   
   virtual bool  SetEntryBinAreaByNumber(uint8_t* a, uint16_t group,
                                                   uint16_t elem);

   virtual void UpdateShaEntries();

   // Header entry
   gdcmDocEntry* GetDocEntryByNumber(uint16_t group, uint16_t element); 
   gdcmDocEntry* GetDocEntryByName  (std::string const & tagName);

   gdcmValEntry* GetValEntryByNumber(uint16_t group, uint16_t element); 
   gdcmBinEntry* GetBinEntryByNumber(uint16_t group, uint16_t element); 

   void LoadDocEntrySafe(gdcmDocEntry* entry);
   TagDocEntryHT* BuildFlatHashTable();

private:
   // Read
   long ParseDES(gdcmDocEntrySet *set,long offset, long l_max, bool delim_mode);
   long ParseSQ (gdcmSeqEntry *seq,   long offset, long l_max, bool delim_mode); 
   
   void LoadDocEntry      (gdcmDocEntry *);
   void FindDocEntryLength(gdcmDocEntry *) throw ( gdcmFormatError );
   void FindDocEntryVR    (gdcmDocEntry *);
   bool CheckDocEntryVR   (gdcmDocEntry *, gdcmVRKey);

   std::string GetDocEntryValue  (gdcmDocEntry *);
   std::string GetDocEntryUnvalue(gdcmDocEntry *);

   void SkipDocEntry          (gdcmDocEntry *);
   void SkipToNextDocEntry    (gdcmDocEntry *);

   void FixDocEntryFoundLength(gdcmDocEntry *, uint32_t);
   bool IsDocEntryAnInteger   (gdcmDocEntry *);

   uint32_t FindDocEntryLengthOB() throw( gdcmFormatUnexpected );

   uint16_t ReadInt16() throw ( gdcmFormatError );
   uint32_t ReadInt32() throw ( gdcmFormatError );
   void     SkipBytes(uint32_t);
   bool     ReadTag(uint16_t, uint16_t);
   uint32_t ReadTagLength(uint16_t, uint16_t);

   void Initialise();
   bool CheckSwap();
   void SwitchSwapToBigEndian();
   void SetMaxSizeLoadEntry(long);
   void SetMaxSizePrintEntry(long);

   // DocEntry related utilities
   gdcmDocEntry* ReadNextDocEntry();

   uint32_t GenerateFreeTagKeyInGroup(uint16_t group);
   void BuildFlatHashTableRecurse( TagDocEntryHT& builtHT,
                                   gdcmDocEntrySet* set );


public:
// Accessors:
   /// Accessor to \ref PrintLevel
   void SetPrintLevel(int level) { PrintLevel = level; }

   /// Accessor to \ref Filename
   const std::string &GetFileName() { return Filename; }

   /// Accessor to \ref Filename
   void SetFileName(std::string const & fileName) { Filename = fileName; }

   /// 'Swap code' accessor (see \ref SwapCode )
   int GetSwapCode() { return SwapCode; }
   
   /// File pointer
   FILE * GetFP() { return Fp; }

   bool operator<(gdcmDocument &document);

};

//-----------------------------------------------------------------------------
#endif
