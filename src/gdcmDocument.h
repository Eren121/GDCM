/*=========================================================================
 
  Program:   gdcm
  Module:    $RCSfile: gdcmDocument.h,v $
  Language:  C++
  Date:      $Date: 2005/01/18 16:23:52 $
  Version:   $Revision: 1.90 $
 
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
 
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#ifndef GDCMDOCUMENT_H
#define GDCMDOCUMENT_H

#include "gdcmVR.h"
#include "gdcmDict.h"
#include "gdcmElementSet.h"

#include <map>
#include <list>
#include <fstream>

namespace gdcm 
{
class ValEntry;
class BinEntry;
class SeqEntry;
class Dict;
class RLEFramesInfo;
class JPEGFragmentsInfo;

//-----------------------------------------------------------------------------
/**
 * \brief Derived by both Header and DicomDir
 */
class GDCM_EXPORT Document : public ElementSet
{
public:
// Informations contained in the parser
   virtual bool IsReadable();
   FileType GetFileType();

   std::string GetTransferSyntax();
 
   RLEFramesInfo *GetRLEInfo() { return RLEInfo; }
   JPEGFragmentsInfo *GetJPEGInfo() { return JPEGInfo; }

// Dictionaries
   virtual void PrintPubDict (std::ostream &os = std::cout);
   virtual void PrintShaDict (std::ostream &os = std::cout);

   Dict* GetPubDict();
   Dict* GetShaDict();
   bool SetShaDict(Dict* dict);
   bool SetShaDict(DictKey const &dictName);

// Swap code
   /// 'Swap code' accessor (see \ref SwapCode )
   int GetSwapCode() { return SwapCode; }
   // System access (meaning endian related !?)
   uint16_t SwapShort(uint16_t);   // needed by File
   uint32_t SwapLong(uint32_t);    // needed by File
   uint16_t UnswapShort(uint16_t); // needed by File
   uint32_t UnswapLong(uint32_t);  // needed by File
   
// Ordering of Documents
   bool operator<(Document &document);

public:
// File I/O
   /// Accessor to \ref Filename
   const std::string &GetFileName() const { return Filename; }
   /// Accessor to \ref Filename
   void SetFileName(std::string const &fileName) { Filename = fileName; }

   std::ifstream *OpenFile();
   bool CloseFile();
   void WriteContent( std::ofstream *fp, FileType type );

// Content entries

   virtual bool SetEntry(std::string const &content,
                         uint16_t group, uint16_t element);
   virtual bool SetEntry(uint8_t *content, int lgth,
                         uint16_t group, uint16_t element);
   virtual bool SetEntry(std::string const &content, ValEntry *entry);
   virtual bool SetEntry(uint8_t *content, int lgth, BinEntry *entry);

   virtual void *GetEntryBinArea(uint16_t group, uint16_t elem);   

   virtual std::string GetEntry  (uint16_t group, uint16_t elem);
   virtual std::string GetEntryVR(uint16_t group, uint16_t elem);
   virtual int GetEntryLength(uint16_t group, uint16_t elem);

   DocEntry *GetDocEntry(uint16_t group, uint16_t element); 
   ValEntry *GetValEntry(uint16_t group, uint16_t element); 
   BinEntry *GetBinEntry(uint16_t group, uint16_t element); 

   ValEntry *ReplaceOrCreate(std::string const &value,
                             uint16_t group, uint16_t elem,
                             TagName const &vr = GDCM_UNKNOWN);
   BinEntry *ReplaceOrCreate(uint8_t* binArea, int lgth,
                             uint16_t group, uint16_t elem,
                             TagName const &vr = GDCM_UNKNOWN);
   SeqEntry *ReplaceOrCreate(uint16_t group, uint16_t elem);

   bool ReplaceIfExist(std::string const &value,
                       uint16_t group, uint16_t elem );

   bool CheckIfEntryExist(uint16_t group, uint16_t elem );
   
   virtual void LoadEntryBinArea(uint16_t group, uint16_t elem);
   virtual void LoadEntryBinArea(BinEntry *entry);

   void LoadDocEntrySafe(DocEntry *entry);
   /*TagDocEntryHT *BuildFlatHashTable();*/

   /// Return the Transfer Syntax as a string
   std::string GetTransferSyntaxName();

   bool IsDicomV3();

protected:
// Methods
   // Constructor and destructor are protected to forbid end user 
   // to instanciate from this class Document (only Header and
   // DicomDir are meaningfull).
   Document();
   Document( std::string const &filename );
   virtual ~Document();
   
   void ReadAndSkipEncapsulatedBasicOffsetTable();
   void ComputeRLEInfo();
   void ComputeJPEGFragmentInfo();
   // Entry

   int ComputeGroup0002Length( FileType filetype );

// Variables
   /// Refering underlying filename.
   std::string Filename;

   /// \brief Swap code gives an information on the byte order of a 
   ///  supposed to be an int32, as it's read on disc 
   /// (depending on the image Transfer Syntax *and* on the processor endianess)
   /// as opposed as it should in memory to be dealt as an int32.
   /// For instance :
   /// - a 'Little Endian' image, read with a little endian processor
   /// will have a SwapCode= 1234 (the order is OK; nothing to do)
   /// - a 'Little Endian' image, read with a big endian procesor
   /// will have a SwapCode= 4321 (the order is wrong; int32 an int16 must be
   /// swapped)
   /// note : values 2143, 4321, 3412 remain for the ACR-NEMA time, and
   /// the well known 'Bad Big Endian' and 'Bad Little Endian' codes
   int SwapCode;

   ///\brief whether we already parsed group 0002
   bool Group0002Parsed;

   ///\brief whether file has a DCM Preamble
   bool HasDCMPreamble;

   /// File Pointer, opened during Header parsing.
   std::ifstream *Fp;

   /// ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown
   FileType Filetype;  

   /// After opening the file, we read HEADER_LENGTH_TO_READ bytes.
   static const unsigned int HEADER_LENGTH_TO_READ; 

   /// \brief Elements whose value is longer than MAX_SIZE_LOAD_ELEMENT_VALUE
   /// are NOT loaded.
   static const unsigned int MAX_SIZE_LOAD_ELEMENT_VALUE;

   /// \brief Elements whose value is longer than  MAX_SIZE_PRINT_ELEMENT_VALUE
   /// are NOT printed.
   static const unsigned int MAX_SIZE_PRINT_ELEMENT_VALUE;

   /// Store the RLE frames info obtained during parsing of pixels.
   RLEFramesInfo *RLEInfo;

   /// Store the JPEG fragments info obtained during parsing of pixels.
   JPEGFragmentsInfo *JPEGInfo;

private:
// Methods
   // Read
   void ParseDES(DocEntrySet *set,long offset, long l_max, bool delim_mode);
   void ParseSQ (SeqEntry *seq,   long offset, long l_max, bool delim_mode);

   void LoadDocEntry         (DocEntry *e);
   void FindDocEntryLength   (DocEntry *e) throw ( FormatError );
   uint32_t FindDocEntryLengthOBOrOW() throw( FormatUnexpected );
   std::string FindDocEntryVR();
   bool CheckDocEntryVR      (VRKey k);

   std::string GetDocEntryValue  (DocEntry *entry);
   std::string GetDocEntryUnvalue(DocEntry *entry);


   void SkipDocEntry          (DocEntry *entry);
   void SkipToNextDocEntry    (DocEntry *entry);

   void FixDocEntryFoundLength(DocEntry *entry,uint32_t l);
   bool IsDocEntryAnInteger   (DocEntry *entry);

   uint16_t ReadInt16() throw ( FormatError );
   uint32_t ReadInt32() throw ( FormatError );
   void     SkipBytes(uint32_t);
   bool     ReadTag(uint16_t, uint16_t);
   uint32_t ReadTagLength(uint16_t, uint16_t);

   void Initialise();
   bool CheckSwap();
   void SwitchByteSwapCode();
   void SetMaxSizeLoadEntry(long);
   void SetMaxSizePrintEntry(long);

   // DocEntry related utilities
   DocEntry *ReadNextDocEntry();

   uint32_t GenerateFreeTagKeyInGroup(uint16_t group);
/*   void BuildFlatHashTableRecurse( TagDocEntryHT &builtHT,
                                   DocEntrySet* set );*/

   void HandleBrokenEndian(uint16_t &group, uint16_t &elem);
   void HandleOutOfGroup0002(uint16_t &group, uint16_t &elem);

// Variables
   /// Public dictionary used to parse this header
   Dict *RefPubDict;
   
   /// \brief Optional "shadow dictionary" (private elements) used to parse
   /// this header
   Dict *RefShaDict;

   /// \brief Size threshold above which an element value will NOT be loaded
   /// in memory (to avoid loading the image/volume itself). By default,
   /// this upper bound is fixed to 1024 bytes (which might look reasonable
   /// when one considers the definition of the various VR contents).
   uint32_t MaxSizeLoadEntry;
   
   /// \brief Size threshold above which an element value will NOT be *printed*
   /// in order no to polute the screen output. By default, this upper bound
   /// is fixed to 64 bytes.
   uint32_t MaxSizePrintEntry;   

private:
   friend class File;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
