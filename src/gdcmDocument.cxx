/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocument.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/23 09:30:22 $
  Version:   $Revision: 1.26 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDocument.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"

#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <errno.h>
#include <vector>

// For nthos:
#ifdef _MSC_VER
   #include <winsock.h>
#else
   #include <netinet/in.h>
#endif

#  include <iomanip>

// Implicit VR Little Endian
#define UI1_2_840_10008_1_2      "1.2.840.10008.1.2"
// Explicit VR Little Endian
#define UI1_2_840_10008_1_2_1    "1.2.840.10008.1.2.1"
// Deflated Explicit VR Little Endian
#define UI1_2_840_10008_1_2_1_99 "1.2.840.10008.1.2.1.99"
// Explicit VR Big Endian
#define UI1_2_840_10008_1_2_2    "1.2.840.10008.1.2.2"
// JPEG Baseline (Process 1)
#define UI1_2_840_10008_1_2_4_50 "1.2.840.10008.1.2.4.50"
// JPEG Extended (Process 2 & 4)
#define UI1_2_840_10008_1_2_4_51 "1.2.840.10008.1.2.4.51"
// JPEG Extended (Process 3 & 5)
#define UI1_2_840_10008_1_2_4_52 "1.2.840.10008.1.2.4.52"
// JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8)
#define UI1_2_840_10008_1_2_4_53 "1.2.840.10008.1.2.4.53"
// JPEG Full Progression, Non-Hierarchical (Process 10 & 12)
#define UI1_2_840_10008_1_2_4_55 "1.2.840.10008.1.2.4.55"
// JPEG Lossless, Non-Hierarchical (Process 14)
#define UI1_2_840_10008_1_2_4_57 "1.2.840.10008.1.2.4.57"
// JPEG Lossless, Hierarchical, First-Order Prediction (Process 14,
// [Selection Value 1])
#define UI1_2_840_10008_1_2_4_70 "1.2.840.10008.1.2.4.70"
// JPEG 2000 Lossless
#define UI1_2_840_10008_1_2_4_90 "1.2.840.10008.1.2.4.90"
// JPEG 2000
#define UI1_2_840_10008_1_2_4_91 "1.2.840.10008.1.2.4.91"
// RLE Lossless
#define UI1_2_840_10008_1_2_5    "1.2.840.10008.1.2.5"
// UI1_1_2_840_10008_1_2_5
#define str2num(str, typeNum) *((typeNum *)(str))

//-----------------------------------------------------------------------------
// Refer to gdcmDocument::CheckSwap()
const unsigned int gdcmDocument::HEADER_LENGTH_TO_READ = 256;

// Refer to gdcmDocument::SetMaxSizeLoadEntry()
const unsigned int gdcmDocument::MAX_SIZE_LOAD_ELEMENT_VALUE = 4096;

const unsigned int gdcmDocument::MAX_SIZE_PRINT_ELEMENT_VALUE = 64;

//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   constructor  
 * @param   inFilename file to be opened for parsing
 * @param   exception_on_error whether we throw an exception or not
 * @param   enable_sequences = true to allow the header 
 *          to be parsed *inside* the SeQuences,
 *          when they have an actual length 
 * \warning enable_sequences *has to be* true for reading PAPYRUS 3.0 files 
 * @param   ignore_shadow to allow skipping the shadow elements, 
 *          to save memory space.
 * \warning The TRUE value for this param has to be used 
 *          with a FALSE value for the 'enable_sequence' param.
 *          ('public elements' may be embedded in 'shadow Sequences')
 */
gdcmDocument::gdcmDocument(std::string const & inFilename, 
                           bool exception_on_error,
                           bool enable_sequences,
                           bool ignore_shadow) 
              : gdcmElementSet(-1)   {
   enableSequences=enable_sequences;
   IgnoreShadow   =ignore_shadow;
   
   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE); 
   Filename = inFilename;
   Initialise();

   if ( !OpenFile(exception_on_error))
      return;
   
   dbg.Verbose(0, "gdcmDocument::gdcmDocument: starting parsing of file: ",
                  Filename.c_str());
   rewind(fp);
   
   fseek(fp,0L,SEEK_END);
   long lgt = ftell(fp);    
           
   rewind(fp);
   CheckSwap();
   long beg = ftell(fp);
   lgt -= beg;
   
   SQDepthLevel=0;
   
   long l=ParseDES( this, beg, lgt, false); // le Load sera fait a la volee
   (void)l; //is l used anywhere ?
	
	rewind(fp); 
   
   // Load 'non string' values
      
   std::string PhotometricInterpretation = GetEntryByNumber(0x0028,0x0004);   
   if( PhotometricInterpretation == "PALETTE COLOR " ) {
      LoadEntryVoidArea(0x0028,0x1200);  // gray LUT   
      LoadEntryVoidArea(0x0028,0x1201);  // R    LUT
      LoadEntryVoidArea(0x0028,0x1202);  // G    LUT
      LoadEntryVoidArea(0x0028,0x1203);  // B    LUT
      
      LoadEntryVoidArea(0x0028,0x1221);  // Segmented Red   Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1222);  // Segmented Green Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1223);  // Segmented Blue  Palette Color LUT Data
   } 
   //FIXME later : how to use it?
   LoadEntryVoidArea(0x0028,0x3006);  //LUT Data (CTX dependent) 
	   	
   CloseFile(); 
  
   // --------------------------------------------------------------
   // Special Patch to allow gdcm to read ACR-LibIDO formated images
   //
   // if recognition code tells us we deal with a LibIDO image
   // we switch lineNumber and columnNumber
   //
   std::string RecCode;
   RecCode = GetEntryByNumber(0x0008, 0x0010); // recognition code
   if (RecCode == "ACRNEMA_LIBIDO_1.1" ||
       RecCode == "CANRME_AILIBOD1_1." )  // for brain-damaged softwares
                                          // with "little-endian strings"
   {
         Filetype = gdcmACR_LIBIDO; 
         std::string rows    = GetEntryByNumber(0x0028, 0x0010);
         std::string columns = GetEntryByNumber(0x0028, 0x0011);
         SetEntryByNumber(columns, 0x0028, 0x0010);
         SetEntryByNumber(rows   , 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ---------------- 

   printLevel = 1;  // 'Medium' print level by default
}

/**
 * \brief  constructor 
 * @param   exception_on_error
 */
gdcmDocument::gdcmDocument(bool exception_on_error) 
             :gdcmElementSet(-1)    {
   (void)exception_on_error;
   enableSequences=0;

   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE);
   Initialise();

   printLevel = 1;  // 'Medium' print level by default
}

/**
 * \brief   Canonical destructor.
 */
gdcmDocument::~gdcmDocument (void) {
   RefPubDict = NULL;
   RefShaDict = NULL;

   // Recursive clean up of sequences
   for (TagDocEntryHT::iterator it = tagHT.begin(); it != tagHT.end(); ++it )
   { 
         delete it->second;
   }
   tagHT.clear();
}

//-----------------------------------------------------------------------------
// Print

/**
  * \brief   Prints The Dict Entries of THE public Dicom Dictionary
  * @return
  */  
void gdcmDocument::PrintPubDict(std::ostream & os) {
   RefPubDict->Print(os);
}

/**
  * \brief   Prints The Dict Entries of THE shadow Dicom Dictionary
  * @return
  */
void gdcmDocument::PrintShaDict(std::ostream & os) {
   RefShaDict->Print(os);
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Get the public dictionary used
 */
gdcmDict *gdcmDocument::GetPubDict(void) {
   return(RefPubDict);
}

/**
 * \brief   Get the shadow dictionary used
 */
gdcmDict *gdcmDocument::GetShaDict(void) {
   return(RefShaDict);
}

/**
 * \brief   Set the shadow dictionary used
 * \param   dict dictionary to use in shadow
 */
bool gdcmDocument::SetShaDict(gdcmDict *dict){
   RefShaDict=dict;
   return(!RefShaDict);
}

/**
 * \brief   Set the shadow dictionary used
 * \param   dictName name of the dictionary to use in shadow
 */
bool gdcmDocument::SetShaDict(DictKey dictName){
   RefShaDict=gdcmGlobal::GetDicts()->GetDict(dictName);
   return(!RefShaDict);
}

/**
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current gdcmDocument was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when gdcmDocument is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool gdcmDocument::IsReadable(void) { 
   if(Filetype==gdcmUnknown) {
      dbg.Verbose(0, "gdcmDocument::IsReadable: wrong filetype");
      return(false);
   }
   if(!tagHT.empty()<=0) { 
      dbg.Verbose(0, "gdcmDocument::IsReadable: no tags in internal"
                     " hash table.");
      return(false);
   }

   return(true);
}


/**
 * \brief   Internal function that checks whether the Transfer Syntax given
 *          as argument is the one present in the current document.
 * @param   SyntaxToCheck The transfert syntax we need to check against.
 * @return  True when SyntaxToCheck corresponds to the Transfer Syntax of
 *          the current document. False either when the document contains
 *          no Transfer Syntax, or when the Tranfer Syntaxes don't match.
 */
bool gdcmDocument::IsGivenTransferSyntax(const std::string & SyntaxToCheck)
{
   gdcmDocEntry *Entry = GetDocEntryByNumber(0x0002, 0x0010);
   if ( !Entry )
      return false;

   // The entry might be present but not loaded (parsing and loading
   // happen at differente stages): try loading and proceed with check...
   LoadDocEntrySafe(Entry);
   if (gdcmValEntry* ValEntry = dynamic_cast< gdcmValEntry* >(Entry) )
   {
      std::string Transfer = ValEntry->GetValue();
      // The actual transfer (as read from disk) might be padded. We
      // first need to remove the potential padding. We can make the
      // weak assumption that padding was not executed with digits...
      while ( ! isdigit(Transfer[Transfer.length()-1]) )
      {
         Transfer.erase(Transfer.length()-1, 1);
      }
      if ( Transfer == SyntaxToCheck )
         return true;
   }
   return false;
}

/**
 * \brief   Determines if the Transfer Syntax of the present document
 *          corresponds to a Implicit Value Representation of 
 *          Little Endian.
 * \sa      \ref gdcmDocument::IsGivenTransferSyntax.
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsImplicitVRLittleEndianTransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsExplicitVRLittleEndianTransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_1);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsDeflatedExplicitVRLittleEndianTransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_1_99);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmDocument::IsExplicitVRBigEndianTransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_2);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGBaseLineProcess1 one.
 * @return  True when JPEGBaseLineProcess1found. False in all other cases.
 */
bool gdcmDocument::IsJPEGBaseLineProcess1TransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_50);
}
                                                                                
/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGExtendedProcess2-4 one.
 * @return  True when JPEGExtendedProcess2-4 found. False in all other cases.
 */
bool gdcmDocument::IsJPEGExtendedProcess2_4TransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_51);
}
                                                                                
/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGExtendeProcess3-5 one.
 * @return  True when JPEGExtendedProcess3-5 found. False in all other cases.
 */
bool gdcmDocument::IsJPEGExtendedProcess3_5TransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_52);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEGSpectralSelectionProcess6-8 one.
 * @return  True when JPEGSpectralSelectionProcess6-8 found. False in all
 *          other cases.
 */
bool gdcmDocument::IsJPEGSpectralSelectionProcess6_8TransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_53);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a RLE Lossless one.
 * @return  True when RLE Lossless found. False in all
 *          other cases.
 */
bool gdcmDocument::IsRLELossLessTransferSyntax(void)
{
   return IsGivenTransferSyntax(UI1_2_840_10008_1_2_5);
}

/**
 * \brief  Determines if Transfer Syntax was already encountered
 *          and if it corresponds to a JPEG Lossless one.
 * @return  True when RLE Lossless found. False in all
 *          other cases.
 */
 
bool gdcmDocument::IsJPEGLossless(void)
{
   return (   IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_55)
           || IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_57)
           || IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_70) ); // was 90 
}
                                                                                
/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a JPEG2000 one
 * @return  True when JPEG2000 (Lossly or LossLess) found. False in all
 *          other cases.
 */
bool gdcmDocument::IsJPEG2000(void)
{
   return (   IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_90)
           || IsGivenTransferSyntax(UI1_2_840_10008_1_2_4_91) );
}

/**
 * \brief   Predicate for dicom version 3 file.
 * @return  True when the file is a dicom version 3.
 */
bool gdcmDocument::IsDicomV3(void) {
   // Checking if Transfert Syntax exists is enough
   // Anyway, it's to late check if the 'Preamble' was found ...
   // And ... would it be a rich idea to check ?
   // (some 'no Preamble' DICOM images exist !)
   return (GetDocEntryByNumber(0x0002, 0x0010) != NULL);
}

/**
 * \brief  returns the File Type 
 *         (ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown)
 * @return the FileType code
 */
FileType gdcmDocument::GetFileType(void) {
   return Filetype;
}

/**
 * \brief   opens the file
 * @param   exception_on_error
 * @return  
 */
FILE *gdcmDocument::OpenFile(bool exception_on_error)
  throw(gdcmFileError) 
{
  fp=fopen(Filename.c_str(),"rb");

  if(!fp)
  {
     if(exception_on_error) 
        throw gdcmFileError("gdcmDocument::gdcmDocument(const char *, bool)");
     else
     {
        dbg.Verbose(0, "gdcmDocument::OpenFile cannot open file: ",
                    Filename.c_str());
        return NULL;
     }
  }

  if ( fp ) 
  {
     guint16 zero;
     fread(&zero,  (size_t)2, (size_t)1, fp);

    //ACR -- or DICOM with no Preamble --
    if( zero == 0x0008 || zero == 0x0800 || zero == 0x0002 || zero == 0x0200)
       return fp;

    //DICOM
    fseek(fp, 126L, SEEK_CUR);
    char dicm[4];
    fread(dicm,  (size_t)4, (size_t)1, fp);
    if( memcmp(dicm, "DICM", 4) == 0 )
       return fp;

    fclose(fp);
    dbg.Verbose(0, "gdcmDocument::OpenFile not DICOM/ACR", Filename.c_str());
  }
  else {
    dbg.Verbose(0, "gdcmDocument::OpenFile cannot open file", Filename.c_str());
  }
  return NULL;
}

/**
 * \brief closes the file  
 * @return  TRUE if the close was successfull 
 */
bool gdcmDocument::CloseFile(void) {
  int closed = fclose(fp);
  fp = (FILE *)0;
  if (! closed)
     return false;
  return true;
}

/**
 * \brief Writes in a file all the Header Entries (Dicom Elements) 
 * @param fp file pointer on an already open file
 * @param filetype Type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * \return Always true.
 */
bool gdcmDocument::WriteF(FileType filetype) {
/// \todo
/// ==============
///      The stuff is rewritten using the SeQuence based 
///       tree-like stucture (cf : Print )
///      To be checked
/// =============

   /// \todo move the following lines (and a lot of others, to be written)
   /// to a future function CheckAndCorrectHeader
   
   /// WARNING : Si on veut ecrire du DICOM V3 a partir d'un DcmHeader ACR-NEMA
   /// no way (check : FileType est un champ de gdcmDocument ...)
   /// a moins de se livrer a un tres complique ajout des champs manquants.
   /// faire un CheckAndCorrectHeader (?)  

   if (filetype == gdcmImplicitVR) 
   {
      std::string implicitVRTransfertSyntax = UI1_2_840_10008_1_2;
      ReplaceOrCreateByNumber(implicitVRTransfertSyntax,0x0002, 0x0010);
      
      /// \todo Refer to standards on page 21, chapter 6.2
      ///       "Value representation": values with a VR of UI shall be
      ///       padded with a single trailing null
      ///       Dans le cas suivant on doit pader manuellement avec un 0
      
      SetEntryLengthByNumber(18, 0x0002, 0x0010);
   } 

   if (filetype == gdcmExplicitVR)
   {
      std::string explicitVRTransfertSyntax = UI1_2_840_10008_1_2_1;
      ReplaceOrCreateByNumber(explicitVRTransfertSyntax,0x0002, 0x0010);
      
      /// \todo Refer to standards on page 21, chapter 6.2
      ///       "Value representation": values with a VR of UI shall be
      ///       padded with a single trailing null
      ///       Dans le cas suivant on doit pader manuellement avec un 0
      
      SetEntryLengthByNumber(20, 0x0002, 0x0010);
   }

/**
 * \todo rewrite later, if really usefull
 *               ('Group Length' element is optional in DICOM)
 *
 *       --> Warning : un-updated odd groups lengthes can causes pb
 *       -->           (xmedcon breaks)
 *       --> to be re- written with future org.
 *
 * if ( (filetype == ImplicitVR) || (filetype == ExplicitVR) )
 *    UpdateGroupLength(false,filetype);
 * if ( filetype == ACR)
 *    UpdateGroupLength(true,ACR);
 */
 
   Write(fp,filetype);  // the gdcmElementSet one !

   /// WriteEntries(fp,type); // old stuff
   return true;
}

/**
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * @param   Value (string) Value to be set
 * @param   Group   Group number of the Entry 
 * @param   Elem  Element number of the Entry
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */
  
gdcmValEntry * gdcmDocument::ReplaceOrCreateByNumber(
                                         std::string Value, 
                                         guint16 Group, 
                                         guint16 Elem )
{
   gdcmDocEntry* CurrentEntry;
   gdcmValEntry* ValEntry;

   CurrentEntry = GetDocEntryByNumber( Group, Elem);
   if (!CurrentEntry)
   {
      // The entry wasn't present and we simply create the required ValEntry:
      CurrentEntry = NewDocEntryByNumber(Group, Elem);
      if (!CurrentEntry)
      {
         dbg.Verbose(0, "gdcmDocument::ReplaceOrCreateByNumber: call to"
                        " NewDocEntryByNumber failed.");
         return NULL;
      }
      ValEntry = new gdcmValEntry(CurrentEntry);
      if ( !AddEntry(ValEntry))
      {
         dbg.Verbose(0, "gdcmDocument::ReplaceOrCreateByNumber: AddEntry"
                        " failed allthough this is a creation.");
      }
   }
   else
   {
      ValEntry = dynamic_cast< gdcmValEntry* >(CurrentEntry);
      if ( !ValEntry )
      {
         // We need to promote the gdcmDocEntry to a gdcmValEntry:
         ValEntry = new gdcmValEntry(CurrentEntry);
         if (!RemoveEntry(CurrentEntry))
         {
            dbg.Verbose(0, "gdcmDocument::ReplaceOrCreateByNumber: removal"
                           " of previous DocEntry failed.");
            return NULL;
         }
         if ( !AddEntry(ValEntry))
         {
            dbg.Verbose(0, "gdcmDocument::ReplaceOrCreateByNumber: adding"
                           " promoted ValEntry failed.");
            return NULL;
         }
      }
   }

   SetEntryByNumber(Value, Group, Elem);

   return ValEntry;
}   

/*
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * @param   voidArea (binary) value to be set
 * @param   Group   Group number of the Entry 
 * @param   Elem  Element number of the Entry
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */
gdcmBinEntry * gdcmDocument::ReplaceOrCreateByNumber(
                                         void *voidArea,
                                         int lgth, 
                                         guint16 Group, 
                                         guint16 Elem)
{
   gdcmDocEntry* a;
   gdcmBinEntry* b;
   a = GetDocEntryByNumber( Group, Elem);
   if (a == NULL) {
      a =NewBinEntryByNumber(Group, Elem);
      if (a == NULL) 
         return NULL;

      b = new gdcmBinEntry(a);
      AddEntry(b);
   }   
   SetEntryByNumber(voidArea, lgth, Group, Elem);
   b->SetVoidArea(voidArea);

   return b;
}  



/**
 * \brief Set a new value if the invoked element exists
 *        Seems to be useless !!!
 * @param Value new element value
 * @param Group  group number of the Entry 
 * @param Elem element number of the Entry
 * \return  boolean 
 */
bool gdcmDocument::ReplaceIfExistByNumber(char* Value, guint16 Group, guint16 Elem ) 
{
   std::string v = Value;
   SetEntryByNumber(v, Group, Elem);
   return true;
} 

//-----------------------------------------------------------------------------
// Protected

/**
 * \brief   Checks if a given Dicom Element exists within the H table
 * @param   group      Group number of the searched Dicom Element 
 * @param   element  Element number of the searched Dicom Element 
 * @return  number of occurences
 */
int gdcmDocument::CheckIfEntryExistByNumber(guint16 group, guint16 element ) {
   std::string key = gdcmDictEntry::TranslateToKey(group, element );
   return tagHT.count(key);
}

/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value of a given tag.
 * \warning Don't use any longer : use GetPubEntryByName
 * @param   tagName name of the searched element.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   return GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement());
}

/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryVRByName(TagName tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   gdcmDocEntry* elem =  GetDocEntryByNumber(dictEntry->GetGroup(),
                                             dictEntry->GetElement());
   return elem->GetVR();
}


/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 * @param   group Group number of the searched tag.
 * @param   element Element number of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryByNumber(guint16 group, guint16 element){
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return GDCM_UNFOUND;
   return ((gdcmValEntry *)tagHT.find(key)->second)->GetValue();
}

/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag..
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   group     Group number of the searched tag.
 * @param   element Element number of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryVRByNumber(guint16 group, guint16 element) {
   gdcmDocEntry* elem =  GetDocEntryByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the value length of a given tag..
 * @param   group     Group number of the searched tag.
 * @param   element Element number of the searched tag.
 * @return  Corresponding element length; -2 if not found
 */
int gdcmDocument::GetEntryLengthByNumber(guint16 group, guint16 element) {
   gdcmDocEntry* elem =  GetDocEntryByNumber(group, element);
   if ( !elem )
      return -2;
   return elem->GetLength();
}
/**
 * \brief   Sets the value (string) of the Header Entry (Dicom Element)
 * @param   content string value of the Dicom Element
 * @param   tagName name of the searched Dicom Element.
 * @return  true when found
 */
bool gdcmDocument::SetEntryByName(std::string content,std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return false;    

   return SetEntryByNumber(content,dictEntry->GetGroup(),
                                   dictEntry->GetElement());
}

/**
 * \brief   Accesses an existing gdcmDocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (string) to substitute with
 * @param   group     group number of the Dicom Element to modify
 * @param   element element number of the Dicom Element to modify
 */
bool gdcmDocument::SetEntryByNumber(std::string content, 
                                  guint16 group,
                                  guint16 element) 
{
   gdcmValEntry* ValEntry = GetValEntryByNumber(group, element);
   if (!ValEntry)
   {
      dbg.Verbose(0, "gdcmDocument::SetEntryByNumber: no corresponding",
                     " ValEntry (try promotion first).");
      return false;
   }

   // Non even content must be padded with a space (020H).
   if((content.length())%2)
      content = content + '\0';
      
   ValEntry->SetValue(content);
   
   // Integers have a special treatement for their length:
   VRKey vr = ValEntry->GetVR();
   if( (vr == "US") || (vr == "SS") ) 
      ValEntry->SetLength(2);
   else if( (vr == "UL") || (vr == "SL") )
      ValEntry->SetLength(4);
   else
      ValEntry->SetLength(content.length());

   return true;
} 

/**
 * \brief   Accesses an existing gdcmDocEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * @param   content new value (void *) to substitute with
 * @param   group     group number of the Dicom Element to modify
 * @param   element element number of the Dicom Element to modify
 */
bool gdcmDocument::SetEntryByNumber(void *content,
                                  int lgth, 
                                  guint16 group,
                                  guint16 element) 
{
   (void)lgth;  //not used
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;

/* Hope Binray field length is never wrong    
   if(lgth%2) // Non even length are padded with a space (020H).
   {  
      lgth++;
      //content = content + '\0'; // fing a trick to enlarge a binary field?
   }
*/      
   gdcmBinEntry * a;
   a = (gdcmBinEntry *)tagHT[key];           
   a->SetVoidArea(content);  
   //a->SetLength(lgth);  // ???  
   return true;
} 

/**
 * \brief   Accesses an existing gdcmDocEntry (i.e. a Dicom Element)
 *          in the PubDocEntrySet of this instance
 *          through it's (group, element) and modifies it's length with
 *          the given value.
 * \warning Use with extreme caution.
 * @param l new length to substitute with
 * @param group     group number of the Entry to modify
 * @param element element number of the Entry to modify
 * @return  true on success, false otherwise.
 */
bool gdcmDocument::SetEntryLengthByNumber(guint32 l, 
                                        guint16 group, 
                                        guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   if (l%2) l++; // length must be even
   ( ((tagHT.equal_range(key)).first)->second )->SetLength(l); 

   return true ;
}

/**
 * \brief   Gets (from Header) the offset  of a 'non string' element value 
 *          (LoadElementValues has already be executed)
 * @param Group   group number of the Entry 
 * @param Elem  element number of the Entry
 * @return File Offset of the Element Value 
 */
size_t gdcmDocument::GetEntryOffsetByNumber(guint16 Group, guint16 Elem) 
{
   gdcmDocEntry* Entry = GetDocEntryByNumber(Group, Elem);
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmDocument::GetDocEntryByNumber: no entry present.");
      return (size_t)0;
   }
   return Entry->GetOffset();
}

/**
 * \brief   Gets (from Header) a 'non string' element value 
 *          (LoadElementValues has already be executed)  
 * @param Group   group number of the Entry 
 * @param Elem  element number of the Entry
 * @return Pointer to the 'non string' area
 */
void * gdcmDocument::GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem) 
{
   gdcmDocEntry* Entry = GetDocEntryByNumber(Group, Elem);
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmDocument::GetDocEntryByNumber: no entry");
      return (NULL);
   }
   return ((gdcmBinEntry *)Entry)->GetVoidArea();
}

/**
 * \brief         Loads (from disk) the element content 
 *                when a string is not suitable
 * @param Group   group number of the Entry 
 * @param Elem  element number of the Entry
 */
void *gdcmDocument::LoadEntryVoidArea(guint16 Group, guint16 Elem) 
{
   gdcmDocEntry * Element= GetDocEntryByNumber(Group, Elem);
   if ( !Element )
      return NULL;
   size_t o =(size_t)Element->GetOffset();
   fseek(fp, o, SEEK_SET);
   size_t l = Element->GetLength();
   char* a = new char[l];
   if(!a) {
      dbg.Verbose(0, "gdcmDocument::LoadEntryVoidArea cannot allocate a");
      return NULL;
   }
   SetEntryVoidAreaByNumber(a, Group, Elem);
   /// \todo check the result 
   size_t l2 = fread(a, 1, l ,fp);
   if(l != l2) 
   {
      delete[] a;
      return NULL;
   }

   return a;  
}

/**
 * \brief   Sets a 'non string' value to a given Dicom Element
 * @param   area area containing the 'non string' value
 * @param   group     Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
bool gdcmDocument::SetEntryVoidAreaByNumber(void * area,
                                          guint16 group, 
                                          guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
      // This was for multimap ?
    (( gdcmBinEntry *)( ((tagHT.equal_range(key)).first)->second ))->SetVoidArea(area);
      
   return true;
}

/**
 * \brief   Update the entries with the shadow dictionary. 
 *          Only non even entries are analyzed       
 */
void gdcmDocument::UpdateShaEntries(void) {
   //gdcmDictEntry *entry;
   std::string vr;
   
   /// \todo TODO : still any use to explore recursively the whole structure?
/*
   for(ListTag::iterator it=listEntries.begin();
       it!=listEntries.end();
       ++it)
   {
      // Odd group => from public dictionary
      if((*it)->GetGroup()%2==0)
         continue;

      // Peer group => search the corresponding dict entry
      if(RefShaDict)
         entry=RefShaDict->GetDictEntryByNumber((*it)->GetGroup(),(*it)->GetElement());
      else
         entry=NULL;

      if((*it)->IsImplicitVR())
         vr="Implicit";
      else
         vr=(*it)->GetVR();

      (*it)->SetValue(GetDocEntryUnvalue(*it));  // to go on compiling
      if(entry){
         // Set the new entry and the new value
         (*it)->SetDictEntry(entry);
         CheckDocEntryVR(*it,vr);

         (*it)->SetValue(GetDocEntryValue(*it));    // to go on compiling
 
      }
      else
      {
         // Remove precedent value transformation
         (*it)->SetDictEntry(NewVirtualDictEntry((*it)->GetGroup(),(*it)->GetElement(),vr));
      }
   }
*/   
}

/**
 * \brief   Searches within the Header Entries for a Dicom Element of
 *          a given tag.
 * @param   tagName name of the searched Dicom Element.
 * @return  Corresponding Dicom Element when it exists, and NULL
 *          otherwise.
 */
 gdcmDocEntry *gdcmDocument::GetDocEntryByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return NULL;

  return(GetDocEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));
}

/**
 * \brief  retrieves a Dicom Element (the first one) using (group, element)
 * \warning (group, element) IS NOT an identifier inside the Dicom Header
 *           if you think it's NOT UNIQUE, check the count number
 *           and use iterators to retrieve ALL the Dicoms Elements within
 *           a given couple (group, element)
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
gdcmDocEntry* gdcmDocument::GetDocEntryByNumber(guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);   
   if ( ! tagHT.count(key))
      return NULL;
   return tagHT.find(key)->second;
}

/**
 * \brief  Same as \ref gdcmDocument::GetDocEntryByNumber except it only
 *         returns a result when the corresponding entry is of type
 *         ValEntry.
 * @return When present, the corresponding ValEntry. 
 */
gdcmValEntry* gdcmDocument::GetValEntryByNumber(guint16 group, guint16 element)
{
  gdcmDocEntry* CurrentEntry = GetDocEntryByNumber(group, element);
  if (! CurrentEntry)
     return (gdcmValEntry*)0;
  if ( gdcmValEntry* ValEntry = dynamic_cast<gdcmValEntry*>(CurrentEntry) )
  {
     return ValEntry;
  }
  dbg.Verbose(0, "gdcmDocument::GetValEntryByNumber: unfound ValEntry.");
  return (gdcmValEntry*)0;
}

/**
 * \brief         Loads the element while preserving the current
 *                underlying file position indicator as opposed to
 *                to LoadDocEntry that modifies it.
 * @param entry   Header Entry whose value shall be loaded. 
 * @return  
 */
void gdcmDocument::LoadDocEntrySafe(gdcmDocEntry * entry) {
   long PositionOnEntry = ftell(fp);
   LoadDocEntry(entry);
   fseek(fp, PositionOnEntry, SEEK_SET);
}

/**
 * \brief   Swaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 * @return  The properly swaped 32 bits integer.
 */
guint32 gdcmDocument::SwapLong(guint32 a) {
   switch (sw) {
      case    0 :
         break;
      case 4321 :
         a=( ((a<<24) & 0xff000000) | ((a<<8)  & 0x00ff0000) | 
             ((a>>8)  & 0x0000ff00) | ((a>>24) & 0x000000ff) );
         break;
   
      case 3412 :
         a=( ((a<<16) & 0xffff0000) | ((a>>16) & 0x0000ffff) );
         break;
   
      case 2143 :
         a=( ((a<<8) & 0xff00ff00) | ((a>>8) & 0x00ff00ff)  );
         break;
      default :
         std::cout << "swapCode= " << sw << std::endl;
         dbg.Error(" gdcmDocument::SwapLong : unset swap code");
         a=0;
   }
   return(a);
} 

/**
 * \brief   Unswaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 * @return  The properly unswaped 32 bits integer.
 */
guint32 gdcmDocument::UnswapLong(guint32 a) {
   return (SwapLong(a));
}

/**
 * \brief   Swaps the bytes so they agree with the processor order
 * @return  The properly swaped 16 bits integer.
 */
guint16 gdcmDocument::SwapShort(guint16 a) {
   if ( (sw==4321)  || (sw==2143) )
      a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
   return (a);
}

/**
 * \brief   Unswaps the bytes so they agree with the processor order
 * @return  The properly unswaped 16 bits integer.
 */
guint16 gdcmDocument::UnswapShort(guint16 a) {
   return (SwapShort(a));
}

//-----------------------------------------------------------------------------
// Private

/**
 * \brief   Parses a DocEntrySet (Zero-level DocEntries or SQ Item DocEntries)
 * @return  length of the parsed set. 
 */ 

long gdcmDocument::ParseDES(gdcmDocEntrySet *set, long offset, long l_max, bool delim_mode) {

   gdcmDocEntry *NewDocEntry = (gdcmDocEntry *)0;
   gdcmValEntry *NewValEntry = (gdcmValEntry *)0;
   gdcmBinEntry *bn;   
   gdcmSeqEntry *sq;
   VRKey vr;
   unsigned long l;
   int depth; 
   
   depth = set->GetDepthLevel();     
   while (true) { 
   
      if ( !delim_mode && ftell(fp)-offset >= l_max) { 
         break;  
      }
      NewDocEntry = ReadNextDocEntry( );
      if (!NewDocEntry)
         break;

      vr = NewDocEntry->GetVR();
      if (vr!="SQ")
      {
               
         if ( gdcmGlobal::GetVR()->IsVROfGdcmStringRepresentable(vr) )
         {
            /////// ValEntry
            NewValEntry = new gdcmValEntry(NewDocEntry->GetDictEntry());
            NewValEntry->Copy(NewDocEntry);
            NewValEntry->SetDepthLevel(depth);
            set->AddEntry(NewValEntry);
            LoadDocEntry(NewValEntry);
            if (NewValEntry->isItemDelimitor())
               break;
            if ( !delim_mode && ftell(fp)-offset >= l_max)
            {
               break;
            }
         }
         else
         {
            if ( ! gdcmGlobal::GetVR()->IsVROfGdcmBinaryRepresentable(vr) )
            { 
                ////// Neither ValEntry NOR BinEntry: should mean UNKOWN VR
                dbg.Verbose(0, "gdcmDocument::ParseDES: neither Valentry, "
                               "nor BinEntry. Probably unknown VR.");
            }

            ////// BinEntry or UNKOWN VR:
            bn = new gdcmBinEntry(NewDocEntry->GetDictEntry());
            bn->Copy(NewDocEntry);
            set->AddEntry(bn);
            LoadDocEntry(bn);
         }

         if (NewDocEntry->GetGroup()   == 0x7fe0 && 
             NewDocEntry->GetElement() == 0x0010 )
         {
             if (NewDocEntry->GetLength()==0xffffffff)
                // Broken US.3405.1.dcm
                Parse7FE0(); // to skip the pixels 
                             // (multipart JPEG/RLE are trouble makers)
         }
         else
         {
             // to be sure we are at the beginning 
             SkipToNextDocEntry(NewDocEntry);
             l = NewDocEntry->GetFullLength(); 
         }
      }
      else
      {   // VR = "SQ"
      
         l=NewDocEntry->GetReadLength();            
         if (l != 0) // don't mess the delim_mode for zero-length sequence
            if (l == 0xffffffff)
              delim_mode = true;
            else
              delim_mode = false;
         // no other way to create it ...
         sq = new gdcmSeqEntry(NewDocEntry->GetDictEntry(),
                               set->GetDepthLevel());
         sq->Copy(NewDocEntry);
         sq->SetDelimitorMode(delim_mode);
         sq->SetDepthLevel(depth);

         if (l != 0)
         {  // Don't try to parse zero-length sequences
            long lgt = ParseSQ( sq, 
                                NewDocEntry->GetOffset(),
                                l, delim_mode);
            (void)lgt;  //not used...
         }
         // FIXME : on en fait quoi, de lgt ?
         set->AddEntry(sq);
         if ( !delim_mode && ftell(fp)-offset >= l_max)
         {
            break;
         }
      }
      delete NewDocEntry;
   }
   return l; // ?? 
}

/**
 * \brief   Parses a Sequence ( SeqEntry after SeqEntry)
 * @return  parsed length for this level
 */ 
long gdcmDocument::ParseSQ(gdcmSeqEntry *set,
                           long offset, long l_max, bool delim_mode)
{
   int SQItemNumber = 0;

   gdcmDocEntry *NewDocEntry = (gdcmDocEntry *)0;
   gdcmSQItem *itemSQ;
   bool dlm_mod;
   int lgr, lgth;
   unsigned int l;
   int depth = set->GetDepthLevel();
   (void)depth; //not used

   while (true) {
      NewDocEntry = ReadNextDocEntry();   
      if(delim_mode) {   
         if (NewDocEntry->isSequenceDelimitor()) {
            set->SetSequenceDelimitationItem(NewDocEntry);
            break;
          }
      }
      if (!delim_mode && (ftell(fp)-offset) >= l_max) {
          break;
      }

      itemSQ = new gdcmSQItem(set->GetDepthLevel());
      itemSQ->AddEntry(NewDocEntry);
      l= NewDocEntry->GetReadLength();
      
      if (l == 0xffffffff)
         dlm_mod = true;
      else
         dlm_mod=false;
   
      lgr=ParseDES(itemSQ, NewDocEntry->GetOffset(), l, dlm_mod);
      
      set->AddEntry(itemSQ,SQItemNumber); 
      SQItemNumber ++;
      if (!delim_mode && (ftell(fp)-offset) >= l_max) {
         break;
      }
   }
   lgth = ftell(fp) - offset;
   return(lgth);
}

/**
 * \brief         Loads the element content if its length doesn't exceed
 *                the value specified with gdcmDocument::SetMaxSizeLoadEntry()
 * @param         Entry Header Entry (Dicom Element) to be dealt with
 */
void gdcmDocument::LoadDocEntry(gdcmDocEntry *Entry)
{
   size_t item_read;
   guint16 group  = Entry->GetGroup();
   std::string  vr= Entry->GetVR();
   guint32 length = Entry->GetLength();

   fseek(fp, (long)Entry->GetOffset(), SEEK_SET);

   // A SeQuence "contains" a set of Elements.  
   //          (fffe e000) tells us an Element is beginning
   //          (fffe e00d) tells us an Element just ended
   //          (fffe e0dd) tells us the current SeQuence just ended
   if( group == 0xfffe ) {
      // NO more value field for SQ !
      return;
   }

   // When the length is zero things are easy:
   if ( length == 0 ) {
      ((gdcmValEntry *)Entry)->SetValue("");
      return;
   }

   // The elements whose length is bigger than the specified upper bound
   // are not loaded. Instead we leave a short notice of the offset of
   // the element content and it's length.
	
   if (length > MaxSizeLoadEntry) {
      if (gdcmBinEntry* BinEntryPtr = dynamic_cast< gdcmBinEntry* >(Entry) )
      {
         std::ostringstream s;
         s << "gdcm::NotLoaded (BinEntry)";
         s << " Address:" << (long)Entry->GetOffset();
         s << " Length:"  << Entry->GetLength();
         s << " x(" << std::hex << Entry->GetLength() << ")";
         BinEntryPtr->SetValue(s.str());
      }
      // to be sure we are at the end of the value ...
      fseek(fp,(long)Entry->GetOffset()+(long)Entry->GetLength(),SEEK_SET);      
      return;		
       // Be carefull : a BinEntry IS_A ValEntry ...  	
      if (gdcmValEntry* ValEntryPtr = dynamic_cast< gdcmValEntry* >(Entry) )
      {
         std::ostringstream s;
         s << "gdcm::NotLoaded. (ValEntry)";
         s << " Address:" << (long)Entry->GetOffset();
         s << " Length:"  << Entry->GetLength();
         s << " x(" << std::hex << Entry->GetLength() << ")";
         ValEntryPtr->SetValue(s.str());
      }		
      // to be sure we are at the end of the value ...
      fseek(fp,(long)Entry->GetOffset()+(long)Entry->GetLength(),SEEK_SET);      
      return;
   }

   // When we find a BinEntry not very much can be done :
   if (gdcmBinEntry* BinEntryPtr = dynamic_cast< gdcmBinEntry* >(Entry) ) {
      LoadEntryVoidArea (BinEntryPtr->GetGroup(),BinEntryPtr->GetElement());
		return;	
	}
 
    
   // Any compacter code suggested (?)
   if ( IsDocEntryAnInteger(Entry) ) {   
      guint32 NewInt;
      std::ostringstream s;
      int nbInt;
   // When short integer(s) are expected, read and convert the following 
   // n *two characters properly i.e. as short integers as opposed to strings.
   // Elements with Value Multiplicity > 1
   // contain a set of integers (not a single one)       
      if (vr == "US" || vr == "SS") {
         nbInt = length / 2;
         NewInt = ReadInt16();
         s << NewInt;
         if (nbInt > 1){
            for (int i=1; i < nbInt; i++) {
               s << '\\';
               NewInt = ReadInt16();
               s << NewInt;
            }
         }
      }
   // When integer(s) are expected, read and convert the following 
   // n * four characters properly i.e. as integers as opposed to strings.
   // Elements with Value Multiplicity > 1
   // contain a set of integers (not a single one)           
      else if (vr == "UL" || vr == "SL") {
         nbInt = length / 4;
         NewInt = ReadInt32();
         s << NewInt;
         if (nbInt > 1) {
            for (int i=1; i < nbInt; i++) {
               s << '\\';
               NewInt = ReadInt32();
               s << NewInt;
            }
         }
      }
#ifdef GDCM_NO_ANSI_STRING_STREAM
      s << std::ends; // to avoid oddities on Solaris
#endif //GDCM_NO_ANSI_STRING_STREAM

      ((gdcmValEntry *)Entry)->SetValue(s.str());
      return;
   }
   
   // We need an additional byte for storing \0 that is not on disk
   std::string NewValue(length,0);
   item_read = fread(&(NewValue[0]), (size_t)length, (size_t)1, fp);
   if ( item_read != 1 ) {
      dbg.Verbose(1, "gdcmDocument::LoadElementValue","unread element value");
      ((gdcmValEntry *)Entry)->SetValue("gdcm::UnRead");
      return;
   }

   if( (vr == "UI") ) // Because of correspondance with the VR dic
      ((gdcmValEntry *)Entry)->SetValue(NewValue.c_str());
   else
      ((gdcmValEntry *)Entry)->SetValue(NewValue);
}


/**
 * \brief  Find the value Length of the passed Header Entry
 * @param  Entry Header Entry whose length of the value shall be loaded. 
 */
 void gdcmDocument::FindDocEntryLength (gdcmDocEntry *Entry) {
   guint16 element = Entry->GetElement();
   //guint16 group   = Entry->GetGroup(); //FIXME
   std::string  vr = Entry->GetVR();
   guint16 length16;
       
   
   if ( (Filetype == gdcmExplicitVR) && (! Entry->IsImplicitVR()) ) 
   {
      if ( (vr=="OB") || (vr=="OW") || (vr=="SQ") || (vr=="UN") ) 
      {
         // The following reserved two bytes (see PS 3.5-2001, section
         // 7.1.2 Data element structure with explicit vr p27) must be
         // skipped before proceeding on reading the length on 4 bytes.
         fseek(fp, 2L, SEEK_CUR);
         guint32 length32 = ReadInt32();

         if ( (vr == "OB") && (length32 == 0xffffffff) ) 
         {
            Entry->SetLength(FindDocEntryLengthOB());
            return;
         }
         FixDocEntryFoundLength(Entry, length32); 
         return;
      }

      // Length is encoded on 2 bytes.
      length16 = ReadInt16();
      
      // We can tell the current file is encoded in big endian (like
      // Data/US-RGB-8-epicard) when we find the "Transfer Syntax" tag
      // and it's value is the one of the encoding of a big endian file.
      // In order to deal with such big endian encoded files, we have
      // (at least) two strategies:
      // * when we load the "Transfer Syntax" tag with value of big endian
      //   encoding, we raise the proper flags. Then we wait for the end
      //   of the META group (0x0002) among which is "Transfer Syntax",
      //   before switching the swap code to big endian. We have to postpone
      //   the switching of the swap code since the META group is fully encoded
      //   in little endian, and big endian coding only starts at the next
      //   group. The corresponding code can be hard to analyse and adds
      //   many additional unnecessary tests for regular tags.
      // * the second strategy consists in waiting for trouble, that shall
      //   appear when we find the first group with big endian encoding. This
      //   is easy to detect since the length of a "Group Length" tag (the
      //   ones with zero as element number) has to be of 4 (0x0004). When we
      //   encounter 1024 (0x0400) chances are the encoding changed and we
      //   found a group with big endian encoding.
      // We shall use this second strategy. In order to make sure that we
      // can interpret the presence of an apparently big endian encoded
      // length of a "Group Length" without committing a big mistake, we
      // add an additional check: we look in the already parsed elements
      // for the presence of a "Transfer Syntax" whose value has to be "big
      // endian encoding". When this is the case, chances are we have got our
      // hands on a big endian encoded file: we switch the swap code to
      // big endian and proceed...
      if ( (element  == 0x0000) && (length16 == 0x0400) ) 
      {
         if ( ! IsExplicitVRBigEndianTransferSyntax() ) 
         {
            dbg.Verbose(0, "gdcmDocument::FindLength", "not explicit VR");
            errno = 1;
            return;
         }
         length16 = 4;
         SwitchSwapToBigEndian();
         // Restore the unproperly loaded values i.e. the group, the element
         // and the dictionary entry depending on them.
         guint16 CorrectGroup   = SwapShort(Entry->GetGroup());
         guint16 CorrectElem    = SwapShort(Entry->GetElement());
         gdcmDictEntry * NewTag = GetDictEntryByNumber(CorrectGroup,
                                                       CorrectElem);
         if (!NewTag) 
         {
            // This correct tag is not in the dictionary. Create a new one.
            NewTag = NewVirtualDictEntry(CorrectGroup, CorrectElem);
         }
         // FIXME this can create a memory leaks on the old entry that be
         // left unreferenced.
         Entry->SetDictEntry(NewTag);
      }
       
      // Heuristic: well some files are really ill-formed.
      if ( length16 == 0xffff) 
      {
         length16 = 0;
         //dbg.Verbose(0, "gdcmDocument::FindLength",
         //            "Erroneous element length fixed.");
         // Actually, length= 0xffff means that we deal with
         // Unknown Sequence Length 
      }
      FixDocEntryFoundLength(Entry, (guint32)length16);
      return;
   }
   else
   {
      // Either implicit VR or a non DICOM conformal (see note below) explicit
      // VR that ommited the VR of (at least) this element. Farts happen.
      // [Note: according to the part 5, PS 3.5-2001, section 7.1 p25
      // on Data elements "Implicit and Explicit VR Data Elements shall
      // not coexist in a Data Set and Data Sets nested within it".]
      // Length is on 4 bytes.
      
      FixDocEntryFoundLength(Entry, ReadInt32());
      return;
   }
}

/**
 * \brief     Find the Value Representation of the current Dicom Element.
 * @param     Entry
 */
void gdcmDocument::FindDocEntryVR( gdcmDocEntry *Entry) 
{
   if (Filetype != gdcmExplicitVR)
      return;

   char VR[3];

   long PositionOnEntry = ftell(fp);
   // Warning: we believe this is explicit VR (Value Representation) because
   // we used a heuristic that found "UL" in the first tag. Alas this
   // doesn't guarantee that all the tags will be in explicit VR. In some
   // cases (see e-film filtered files) one finds implicit VR tags mixed
   // within an explicit VR file. Hence we make sure the present tag
   // is in explicit VR and try to fix things if it happens not to be
   // the case.
   
   (void)fread (&VR, (size_t)2,(size_t)1, fp);
   VR[2]=0;
   if(!CheckDocEntryVR(Entry,VR))
   {
      fseek(fp, PositionOnEntry, SEEK_SET);
      // When this element is known in the dictionary we shall use, e.g. for
      // the semantics (see the usage of IsAnInteger), the VR proposed by the
      // dictionary entry. Still we have to flag the element as implicit since
      // we know now our assumption on expliciteness is not furfilled.
      // avoid  .
      if ( Entry->IsVRUnknown() )
         Entry->SetVR("Implicit");
      Entry->SetImplicitVR();
   }
}

/**
 * \brief     Check the correspondance between the VR of the header entry
 *            and the taken VR. If they are different, the header entry is 
 *            updated with the new VR.
 * @param     Entry Header Entry to check
 * @param     vr    Dicom Value Representation
 * @return    false if the VR is incorrect of if the VR isn't referenced
 *            otherwise, it returns true
*/
bool gdcmDocument::CheckDocEntryVR(gdcmDocEntry *Entry, VRKey vr)
{
   char msg[100]; // for sprintf
   bool RealExplicit = true;

   // Assume we are reading a falsely explicit VR file i.e. we reached
   // a tag where we expect reading a VR but are in fact we read the
   // first to bytes of the length. Then we will interogate (through find)
   // the dicom_vr dictionary with oddities like "\004\0" which crashes
   // both GCC and VC++ implementations of the STL map. Hence when the
   // expected VR read happens to be non-ascii characters we consider
   // we hit falsely explicit VR tag.

   if ( (!isalpha(vr[0])) && (!isalpha(vr[1])) )
      RealExplicit = false;

   // CLEANME searching the dicom_vr at each occurence is expensive.
   // PostPone this test in an optional integrity check at the end
   // of parsing or only in debug mode.
   if ( RealExplicit && !gdcmGlobal::GetVR()->Count(vr) )
      RealExplicit= false;

   if ( !RealExplicit ) 
   {
      // We thought this was explicit VR, but we end up with an
      // implicit VR tag. Let's backtrack.   
      sprintf(msg,"Falsely explicit vr file (%04x,%04x)\n", 
                   Entry->GetGroup(),Entry->GetElement());
      dbg.Verbose(1, "gdcmDocument::FindVR: ",msg);
      if (Entry->GetGroup()%2 && Entry->GetElement() == 0x0000) { // Group length is UL !
         gdcmDictEntry* NewEntry = NewVirtualDictEntry(
                                   Entry->GetGroup(),Entry->GetElement(),
                                   "UL","FIXME","Group Length");
         Entry->SetDictEntry(NewEntry);     
      }
      return(false);
   }

   if ( Entry->IsVRUnknown() ) 
   {
      // When not a dictionary entry, we can safely overwrite the VR.
      if (Entry->GetElement() == 0x0000) { // Group length is UL !
         Entry->SetVR("UL");
      } else {
         Entry->SetVR(vr);
      }
   }
   else if ( Entry->GetVR() != vr ) 
   {
      // The VR present in the file and the dictionary disagree. We assume
      // the file writer knew best and use the VR of the file. Since it would
      // be unwise to overwrite the VR of a dictionary (since it would
      // compromise it's next user), we need to clone the actual DictEntry
      // and change the VR for the read one.
      gdcmDictEntry* NewEntry = NewVirtualDictEntry(
                                 Entry->GetGroup(),Entry->GetElement(),
                                 vr,"FIXME",Entry->GetName());
      Entry->SetDictEntry(NewEntry);
   }
   return(true); 
}

/**
 * \brief   Get the transformed value of the header entry. The VR value 
 *          is used to define the transformation to operate on the value
 * \warning NOT end user intended method !
 * @param   Entry 
 * @return  Transformed entry value
 */
std::string gdcmDocument::GetDocEntryValue(gdcmDocEntry *Entry)
{
   if ( (IsDocEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string val=((gdcmValEntry *)Entry)->GetValue();
      std::string vr=Entry->GetVR();
      guint32 length = Entry->GetLength();
      std::ostringstream s;
      int nbInt;

   // When short integer(s) are expected, read and convert the following 
   // n * 2 bytes properly i.e. as a multivaluated strings
   // (each single value is separated fromthe next one by '\'
   // as usual for standard multivaluated filels
   // Elements with Value Multiplicity > 1
   // contain a set of short integers (not a single one) 
   
      if (vr == "US" || vr == "SS")
      {
         guint16 NewInt16;

         nbInt = length / 2;
         for (int i=0; i < nbInt; i++) 
         {
            if(i!=0)
               s << '\\';
            NewInt16 = (val[2*i+0]&0xFF)+((val[2*i+1]&0xFF)<<8);
            NewInt16 = SwapShort(NewInt16);
            s << NewInt16;
         }
      }

   // When integer(s) are expected, read and convert the following 
   // n * 4 bytes properly i.e. as a multivaluated strings
   // (each single value is separated fromthe next one by '\'
   // as usual for standard multivaluated filels
   // Elements with Value Multiplicity > 1
   // contain a set of integers (not a single one) 
      else if (vr == "UL" || vr == "SL")
      {
         guint32 NewInt32;

         nbInt = length / 4;
         for (int i=0; i < nbInt; i++) 
         {
            if(i!=0)
               s << '\\';
            NewInt32= (val[4*i+0]&0xFF)+((val[4*i+1]&0xFF)<<8)+
                     ((val[4*i+2]&0xFF)<<16)+((val[4*i+3]&0xFF)<<24);
            NewInt32=SwapLong(NewInt32);
            s << NewInt32;
         }
      }
#ifdef GDCM_NO_ANSI_STRING_STREAM
      s << std::ends; // to avoid oddities on Solaris
#endif //GDCM_NO_ANSI_STRING_STREAM
      return(s.str());
   }

   return(((gdcmValEntry *)Entry)->GetValue());
}

/**
 * \brief   Get the reverse transformed value of the header entry. The VR 
 *          value is used to define the reverse transformation to operate on
 *          the value
 * \warning NOT end user intended method !
 * @param   Entry 
 * @return  Reverse transformed entry value
 */
std::string gdcmDocument::GetDocEntryUnvalue(gdcmDocEntry *Entry)
{
   if ( (IsDocEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string vr=Entry->GetVR();
      std::ostringstream s;
      std::vector<std::string> tokens;

      if (vr == "US" || vr == "SS") 
      {
         guint16 NewInt16;

         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize (((gdcmValEntry *)Entry)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) 
         {
            NewInt16 = atoi(tokens[i].c_str());
            s<<(NewInt16&0xFF)<<((NewInt16>>8)&0xFF);
         }
         tokens.clear();
      }
      if (vr == "UL" || vr == "SL") 
      {
         guint32 NewInt32;

         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize (((gdcmValEntry *)Entry)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) 
         {
            NewInt32 = atoi(tokens[i].c_str());
            s<<(char)(NewInt32&0xFF)<<(char)((NewInt32>>8)&0xFF)
               <<(char)((NewInt32>>16)&0xFF)<<(char)((NewInt32>>24)&0xFF);
         }
         tokens.clear();
      }

#ifdef GDCM_NO_ANSI_STRING_STREAM
      s << std::ends; // to avoid oddities on Solaris
#endif //GDCM_NO_ANSI_STRING_STREAM
      return(s.str());
   }

   return(((gdcmValEntry *)Entry)->GetValue());
}

/**
 * \brief   Skip a given Header Entry 
 * \warning NOT end user intended method !
 * @param   entry 
 */
void gdcmDocument::SkipDocEntry(gdcmDocEntry *entry) 
{
   SkipBytes(entry->GetLength());
}

/**
 * \brief   Skips to the begining of the next Header Entry 
 * \warning NOT end user intended method !
 * @param   entry 
 */
void gdcmDocument::SkipToNextDocEntry(gdcmDocEntry *entry) 
{
   (void)fseek(fp, (long)(entry->GetOffset()),     SEEK_SET);
   (void)fseek(fp, (long)(entry->GetReadLength()), SEEK_CUR);
}

/**
 * \brief   When the length of an element value is obviously wrong (because
 *          the parser went Jabberwocky) one can hope improving things by
 *          applying some heuristics.
 */
void gdcmDocument::FixDocEntryFoundLength(gdcmDocEntry *Entry,
                                          guint32 FoundLength)
{
   Entry->SetReadLength(FoundLength); // will be updated only if a bug is found        
   if ( FoundLength == 0xffffffff) {
      FoundLength = 0;
   }
   
   guint16 gr =Entry->GetGroup();
   guint16 el =Entry->GetElement(); 
     
   if (FoundLength%2) {
      std::ostringstream s;
      s << "Warning : Tag with uneven length "
        << FoundLength 
        <<  " in x(" << std::hex << gr << "," << el <<")" << std::dec;
      dbg.Verbose(0, s.str().c_str());
   }
      
   //////// Fix for some naughty General Electric images.
   // Allthough not recent many such GE corrupted images are still present
   // on Creatis hard disks. Hence this fix shall remain when such images
   // are no longer in user (we are talking a few years, here)...
   // Note: XMedCom probably uses such a trick since it is able to read
   //       those pesky GE images ...
   if (FoundLength == 13) {  // Only happens for this length !
      if (   (Entry->GetGroup() != 0x0008)
          || (   (Entry->GetElement() != 0x0070)
              && (Entry->GetElement() != 0x0080) ) )
      {
         FoundLength = 10;
         Entry->SetReadLength(10); /// \todo a bug is to be fixed !?
      }
   }

   //////// Fix for some brain-dead 'Leonardo' Siemens images.
   // Occurence of such images is quite low (unless one leaves close to a
   // 'Leonardo' source. Hence, one might consider commenting out the
   // following fix on efficiency reasons.
   else
   if (   (Entry->GetGroup() == 0x0009)
       && (   (Entry->GetElement() == 0x1113)
           || (Entry->GetElement() == 0x1114) ) )
   {
      FoundLength = 4;
      Entry->SetReadLength(4); /// \todo a bug is to be fixed !?
   } 
 
   //////// Deal with sequences, but only on users request:
   else
   if ( ( Entry->GetVR() == "SQ") && enableSequences)
   {
         FoundLength = 0;      // ReadLength is unchanged 
   } 
    
   //////// We encountered a 'delimiter' element i.e. a tag of the form 
   // "fffe|xxxx" which is just a marker. Delimiters length should not be
   // taken into account.
   else
   if(Entry->GetGroup() == 0xfffe)
   {    
     // According to the norm, fffe|0000 shouldn't exist. BUT the Philips
     // image gdcmData/gdcm-MR-PHILIPS-16-Multi-Seq.dcm happens to
     // causes extra troubles...
     if( Entry->GetElement() != 0x0000 )
     {
        FoundLength = 0;
     }
   } 
           
   Entry->SetUsableLength(FoundLength);
}

/**
 * \brief   Apply some heuristics to predict whether the considered 
 *          element value contains/represents an integer or not.
 * @param   Entry The element value on which to apply the predicate.
 * @return  The result of the heuristical predicate.
 */
bool gdcmDocument::IsDocEntryAnInteger(gdcmDocEntry *Entry) {
   guint16 element = Entry->GetElement();
   guint16 group   = Entry->GetGroup();
   std::string  vr = Entry->GetVR();
   guint32 length  = Entry->GetLength();

   // When we have some semantics on the element we just read, and if we
   // a priori know we are dealing with an integer, then we shall be
   // able to swap it's element value properly.
   if ( element == 0 )  // This is the group length of the group
   {  
      if (length == 4)
         return true;
      else 
      {
         // Allthough this should never happen, still some images have a
         // corrupted group length [e.g. have a glance at offset x(8336) of
         // gdcmData/gdcm-MR-PHILIPS-16-Multi-Seq.dcm].
         // Since for dicom compliant and well behaved headers, the present
         // test is useless (and might even look a bit paranoid), when we
         // encounter such an ill-formed image, we simply display a warning
         // message and proceed on parsing (while crossing fingers).
         std::ostringstream s;
         int filePosition = ftell(fp);
         s << "Erroneous Group Length element length  on : (" \
           << std::hex << group << " , " << element 
           << ") -before- position x(" << filePosition << ")"
           << "lgt : " << length;
         dbg.Verbose(0, "gdcmDocument::IsDocEntryAnInteger", s.str().c_str() );
      }
   }

   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
      return true;
   
   return false;
}

/**
 * \brief  Find the Length till the next sequence delimiter
 * \warning NOT end user intended method !
 * @return 
 */

 guint32 gdcmDocument::FindDocEntryLengthOB(void)  {
   // See PS 3.5-2001, section A.4 p. 49 on encapsulation of encoded pixel data.
   guint16 g;
   guint16 n; 
   long PositionOnEntry = ftell(fp);
   bool FoundSequenceDelimiter = false;
   guint32 TotalLength = 0;
   guint32 ItemLength;

   while ( ! FoundSequenceDelimiter) 
   {
      g = ReadInt16();
      n = ReadInt16();   
      if (errno == 1)
         return 0;
      TotalLength += 4;  // We even have to decount the group and element 
     
      if ( g != 0xfffe && g!=0xb00c ) //for bogus header  
      {
         char msg[100]; // for sprintf. Sorry
         sprintf(msg,"wrong group (%04x) for an item sequence (%04x,%04x)\n",g, g,n);
         dbg.Verbose(1, "gdcmDocument::FindLengthOB: ",msg); 
         errno = 1;
         return 0;
      }
      if ( n == 0xe0dd || ( g==0xb00c && n==0x0eb6 ) ) // for bogus header 
         FoundSequenceDelimiter = true;
      else if ( n != 0xe000 )
      {
         char msg[100];  // for sprintf. Sorry
         sprintf(msg,"wrong element (%04x) for an item sequence (%04x,%04x)\n",
                      n, g,n);
         dbg.Verbose(1, "gdcmDocument::FindLengthOB: ",msg);
         errno = 1;
         return 0;
      }
      ItemLength = ReadInt32();
      TotalLength += ItemLength + 4;  // We add 4 bytes since we just read
                                      // the ItemLength with ReadInt32                                     
      SkipBytes(ItemLength);
   }
   fseek(fp, PositionOnEntry, SEEK_SET);
   return TotalLength;
}

/**
 * \brief Reads a supposed to be 16 Bits integer
 *       (swaps it depending on processor endianity) 
 * @return read value
 */
guint16 gdcmDocument::ReadInt16() {
   guint16 g;
   size_t item_read;
   item_read = fread (&g, (size_t)2,(size_t)1, fp);
   if ( item_read != 1 ) {
      if(ferror(fp)) 
         dbg.Verbose(0, "gdcmDocument::ReadInt16", " File Error");
      errno = 1;
      return 0;
   }
   errno = 0;
   g = SwapShort(g);   
   return g;
}

/**
 * \brief  Reads a supposed to be 32 Bits integer
 *         (swaps it depending on processor endianity)  
 * @return read value
 */
guint32 gdcmDocument::ReadInt32() {
   guint32 g;
   size_t item_read;
   item_read = fread (&g, (size_t)4,(size_t)1, fp);
   if ( item_read != 1 ) { 
     if(ferror(fp)) 
         dbg.Verbose(0, "gdcmDocument::ReadInt32", " File Error");   
      errno = 1;
      return 0;
   }
   errno = 0;   
   g = SwapLong(g);
   return g;
}

/**
 * \brief skips bytes inside the source file 
 * \warning NOT end user intended method !
 * @return 
 */
void gdcmDocument::SkipBytes(guint32 NBytes) {
   //FIXME don't dump the returned value
   (void)fseek(fp, (long)NBytes, SEEK_CUR);
}

/**
 * \brief Loads all the needed Dictionaries
 * \warning NOT end user intended method !   
 */
void gdcmDocument::Initialise(void) 
{
   RefPubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   RefShaDict = NULL;
}

/**
 * \brief   Discover what the swap code is (among little endian, big endian,
 *          bad little endian, bad big endian).
 *          sw is set
 * @return false when we are absolutely sure 
 *               it's neither ACR-NEMA nor DICOM
 *         true  when we hope ours assuptions are OK
 */
bool gdcmDocument::CheckSwap() {

   // The only guaranted way of finding the swap code is to find a
   // group tag since we know it's length has to be of four bytes i.e.
   // 0x00000004. Finding the swap code in then straigthforward. Trouble
   // occurs when we can't find such group...
   
   guint32  x=4;  // x : for ntohs
   bool net2host; // true when HostByteOrder is the same as NetworkByteOrder
   guint32  s32;
   guint16  s16;
       
   int lgrLue;
   char *entCur;
   char deb[HEADER_LENGTH_TO_READ];
    
   // First, compare HostByteOrder and NetworkByteOrder in order to
   // determine if we shall need to swap bytes (i.e. the Endian type).
   if (x==ntohs(x))
      net2host = true;
   else
      net2host = false; 
         
   // The easiest case is the one of a DICOM header, since it possesses a
   // file preamble where it suffice to look for the string "DICM".
   lgrLue = fread(deb, 1, HEADER_LENGTH_TO_READ, fp);
   
   entCur = deb + 128;
   if(memcmp(entCur, "DICM", (size_t)4) == 0) {
      dbg.Verbose(1, "gdcmDocument::CheckSwap:", "looks like DICOM Version3");
      
      // Next, determine the value representation (VR). Let's skip to the
      // first element (0002, 0000) and check there if we find "UL" 
      // - or "OB" if the 1st one is (0002,0001) -,
      // in which case we (almost) know it is explicit VR.
      // WARNING: if it happens to be implicit VR then what we will read
      // is the length of the group. If this ascii representation of this
      // length happens to be "UL" then we shall believe it is explicit VR.
      // FIXME: in order to fix the above warning, we could read the next
      // element value (or a couple of elements values) in order to make
      // sure we are not commiting a big mistake.
      // We need to skip :
      // * the 128 bytes of File Preamble (often padded with zeroes),
      // * the 4 bytes of "DICM" string,
      // * the 4 bytes of the first tag (0002, 0000),or (0002, 0001)
      // i.e. a total of  136 bytes.
      entCur = deb + 136;
     
      // FIXME : FIXME:
      // Sometimes (see : gdcmData/icone.dcm) group 0x0002 *is* Explicit VR,
      // but elem 0002,0010 (Transfert Syntax) tells us the file is
      // *Implicit* VR.  -and it is !- 
      
      if( (memcmp(entCur, "UL", (size_t)2) == 0) ||
          (memcmp(entCur, "OB", (size_t)2) == 0) ||
          (memcmp(entCur, "UI", (size_t)2) == 0) ||
          (memcmp(entCur, "CS", (size_t)2) == 0) )  // CS, to remove later
                                                    // when Write DCM *adds*
      // FIXME
      // Use gdcmDocument::dicom_vr to test all the possibilities
      // instead of just checking for UL, OB and UI !? group 0000 
      {
         Filetype = gdcmExplicitVR;
         dbg.Verbose(1, "gdcmDocument::CheckSwap:",
                     "explicit Value Representation");
      } 
      else 
      {
         Filetype = gdcmImplicitVR;
         dbg.Verbose(1, "gdcmDocument::CheckSwap:",
                     "not an explicit Value Representation");
      }
      
      if (net2host) 
      {
         sw = 4321;
         dbg.Verbose(1, "gdcmDocument::CheckSwap:",
                        "HostByteOrder != NetworkByteOrder");
      } 
      else 
      {
         sw = 0;
         dbg.Verbose(1, "gdcmDocument::CheckSwap:",
                        "HostByteOrder = NetworkByteOrder");
      }
      
      // Position the file position indicator at first tag (i.e.
      // after the file preamble and the "DICM" string).
      rewind(fp);
      fseek (fp, 132L, SEEK_SET);
      return true;
   } // End of DicomV3

   // Alas, this is not a DicomV3 file and whatever happens there is no file
   // preamble. We can reset the file position indicator to where the data
   // is (i.e. the beginning of the file).
   dbg.Verbose(1, "gdcmDocument::CheckSwap:", "not a DICOM Version3 file");
   rewind(fp);

   // Our next best chance would be to be considering a 'clean' ACR/NEMA file.
   // By clean we mean that the length of the first tag is written down.
   // If this is the case and since the length of the first group HAS to be
   // four (bytes), then determining the proper swap code is straightforward.

   entCur = deb + 4;
   // We assume the array of char we are considering contains the binary
   // representation of a 32 bits integer. Hence the following dirty
   // trick :
   s32 = *((guint32 *)(entCur));
      
   switch (s32) {
      case 0x00040000 :
         sw = 3412;
         Filetype = gdcmACR;
         return true;
      case 0x04000000 :
         sw = 4321;
         Filetype = gdcmACR;
         return true;
      case 0x00000400 :
         sw = 2143;
         Filetype = gdcmACR;
         return true;
      case 0x00000004 :
         sw = 0;
         Filetype = gdcmACR;
         return true;
      default :

      // We are out of luck. It is not a DicomV3 nor a 'clean' ACR/NEMA file.
      // It is time for despaired wild guesses. 
      // So, let's check if this file wouldn't happen to be 'dirty' ACR/NEMA,
      //  i.e. the 'group length' element is not present :     
      
      //  check the supposed to be 'group number'
      //  0x0002 or 0x0004 or 0x0008
      //  to determine ' sw' value .
      //  Only 0 or 4321 will be possible 
      //  (no oportunity to check for the formerly well known
      //  ACR-NEMA 'Bad Big Endian' or 'Bad Little Endian' 
      //  if unsuccessfull (i.e. neither 0x0002 nor 0x0200 etc -4, 8-) 
      //  the file IS NOT ACR-NEMA nor DICOM V3
      //  Find a trick to tell it the caller...
      
      s16 = *((guint16 *)(deb));
      
      switch (s16) {
      case 0x0002 :
      case 0x0004 :
      case 0x0008 :      
         sw = 0;
         Filetype = gdcmACR;
         return true;
      case 0x0200 :
      case 0x0400 :
      case 0x0800 : 
         sw = 4321;
         Filetype = gdcmACR;
         return true;
      default :
         dbg.Verbose(0, "gdcmDocument::CheckSwap:",
                     "ACR/NEMA unfound swap info (Really hopeless !)"); 
         Filetype = gdcmUnknown;     
         return false;
      }
      
      // Then the only info we have is the net2host one.
      //if (! net2host )
         //   sw = 0;
         //else
         //  sw = 4321;
         //return;
   }
}

/**
 * \brief Restore the unproperly loaded values i.e. the group, the element
 *        and the dictionary entry depending on them. 
 */
void gdcmDocument::SwitchSwapToBigEndian(void) 
{
   dbg.Verbose(1, "gdcmDocument::SwitchSwapToBigEndian",
                  "Switching to BigEndian mode.");
   if ( sw == 0    ) 
   {
      sw = 4321;
      return;
   }
   if ( sw == 4321 ) 
   {
      sw = 0;
      return;
   }
   if ( sw == 3412 ) 
   {
      sw = 2143;
      return;
   }
   if ( sw == 2143 )
      sw = 3412;
}

/**
 * \brief  during parsing, Header Elements too long are not loaded in memory 
 * @param NewSize
 */
void gdcmDocument::SetMaxSizeLoadEntry(long NewSize) 
{
   if (NewSize < 0)
      return;
   if ((guint32)NewSize >= (guint32)0xffffffff) 
   {
      MaxSizeLoadEntry = 0xffffffff;
      return;
   }
   MaxSizeLoadEntry = NewSize;
}


/**
 * \brief Header Elements too long will not be printed
 * \todo  See comments of \ref gdcmDocument::MAX_SIZE_PRINT_ELEMENT_VALUE 
 * @param NewSize
 */
void gdcmDocument::SetMaxSizePrintEntry(long NewSize) 
{
   if (NewSize < 0)
      return;
   if ((guint32)NewSize >= (guint32)0xffffffff) 
   {
      MaxSizePrintEntry = 0xffffffff;
      return;
   }
   MaxSizePrintEntry = NewSize;
}



/**
 * \brief   Read the next tag but WITHOUT loading it's value
 *          (read the 'Group Number', the 'Element Number',
 *           gets the Dict Entry
 *          gets the VR, gets the length, gets the offset value)
 * @return  On succes the newly created DocEntry, NULL on failure.      
 */
gdcmDocEntry *gdcmDocument::ReadNextDocEntry(void) {
   guint16 g,n;
   gdcmDocEntry *NewEntry;
   g = ReadInt16();
   n = ReadInt16();
      
   if (errno == 1)
      // We reached the EOF (or an error occured) therefore 
      // header parsing has to be considered as finished.
      return (gdcmDocEntry *)0;

   NewEntry = NewDocEntryByNumber(g, n);
   FindDocEntryVR(NewEntry);
   FindDocEntryLength(NewEntry);

   if (errno == 1) {
      // Call it quits
      delete NewEntry;
      return NULL;
   }
   NewEntry->SetOffset(ftell(fp));  
   return NewEntry;
}


/**
 * \brief   Generate a free TagKey i.e. a TagKey that is not present
 *          in the TagHt dictionary.
 * @param   group The generated tag must belong to this group.  
 * @return  The element of tag with given group which is fee.
 */
guint32 gdcmDocument::GenerateFreeTagKeyInGroup(guint16 group) 
{
   for (guint32 elem = 0; elem < UINT32_MAX; elem++) 
   {
      TagKey key = gdcmDictEntry::TranslateToKey(group, elem);
      if (tagHT.count(key) == 0)
         return elem;
   }
   return UINT32_MAX;
}


/**
 * \brief   Assuming the internal file pointer \ref gdcmDocument::fp 
 *          is placed at the beginning of a tag (TestGroup, TestElement),
 *          read the length associated to the Tag.
 * \warning On success the internal file pointer \ref gdcmDocument::fp
 *          is modified to point after the tag and it's length.
 *          On failure (i.e. when the tag wasn't the expected tag
 *          (TestGroup, TestElement) the internal file pointer
 *          \ref gdcmDocument::fp is restored to it's original position.
 * @param   TestGroup   The expected group of the tag.
 * @param   TestElement The expected Element of the tag.
 * @return  On success returns the length associated to the tag. On failure
 *          returns 0.
 */
guint32 gdcmDocument::ReadTagLength(guint16 TestGroup, guint16 TestElement)
{
   guint16 ItemTagGroup;
   guint16 ItemTagElement; 
   long PositionOnEntry = ftell(fp);
   long CurrentPosition = ftell(fp);          // On debugging purposes

   //// Read the Item Tag group and element, and make
   // sure they are respectively 0xfffe and 0xe000:
   ItemTagGroup   = ReadInt16();
   ItemTagElement = ReadInt16();
   if ( (ItemTagGroup != TestGroup) || (ItemTagElement != TestElement ) )
   {
      std::ostringstream s;
      s << "   We should have found tag (";
      s << std::hex << TestGroup << "," << TestElement << ")" << std::endl;
      s << "   but instead we encountered tag (";
      s << std::hex << ItemTagGroup << "," << ItemTagElement << ")"
        << std::endl;
      s << "  at address: " << (unsigned)CurrentPosition << std::endl;
      dbg.Verbose(0, "gdcmDocument::ReadItemTagLength: wrong Item Tag found:");
      dbg.Verbose(0, s.str().c_str());
      fseek(fp, PositionOnEntry, SEEK_SET);
      return 0;
   }
                                                                                
   //// Then read the associated Item Length
   CurrentPosition=ftell(fp);
   guint32 ItemLength;
   ItemLength = ReadInt32();
   {
      std::ostringstream s;
      s << "Basic Item Length is: "
        << ItemLength << std::endl;
      s << "  at address: " << (unsigned)CurrentPosition << std::endl;
      dbg.Verbose(0, "gdcmDocument::ReadItemTagLength: ", s.str().c_str());
   }
   return ItemLength;
}

/**
 * \brief   Read the length of an exptected Item tag i.e. (0xfffe, 0xe000).
 * \sa      \ref gdcmDocument::ReadTagLength
 * \warning See warning of \ref gdcmDocument::ReadTagLength
 * @return  On success returns the length associated to the item tag.
 *          On failure returns 0.
 */ 
guint32 gdcmDocument::ReadItemTagLength(void)
{
   return ReadTagLength(0xfffe, 0xe000);
}

/**
 * \brief   Read the length of an expected Sequence Delimiter tag i.e.
 *          (0xfffe, 0xe0dd).
 * \sa      \ref gdcmDocument::ReadTagLength
 * \warning See warning of \ref gdcmDocument::ReadTagLength
 * @return  On success returns the length associated to the Sequence
 *          Delimiter tag. On failure returns 0.
 */
guint32 gdcmDocument::ReadSequenceDelimiterTagLength(void)
{
   return ReadTagLength(0xfffe, 0xe0dd);
}


/**
 * \brief   Parse pixel data from disk for multi-fragment Jpeg/Rle files
 *          No other way so 'skip' the Data
 *
 */

void gdcmDocument::Parse7FE0 (void)
{
   gdcmDocEntry* Element = GetDocEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return;
      
   if (   IsImplicitVRLittleEndianTransferSyntax()
       || IsExplicitVRLittleEndianTransferSyntax()
       || IsExplicitVRBigEndianTransferSyntax() /// \todo 1.2.2 ??? A verifier !
       || IsDeflatedExplicitVRLittleEndianTransferSyntax() )
      return;
      
   // ---------------- for Parsing : Position on begining of Jpeg/RLE Pixels 

   //// Read the Basic Offset Table Item Tag length...
   guint32 ItemLength = ReadItemTagLength();

   //// ... and then read length[s] itself[themselves]. We don't use
   // the values read (BTW  what is the purpous of those lengths ?)
   if (ItemLength != 0) {
      // BTW, what is the purpous of those length anyhow !? 
      char * BasicOffsetTableItemValue = new char[ItemLength + 1];
      fread(BasicOffsetTableItemValue, ItemLength, 1, fp); 
      for (unsigned int i=0; i < ItemLength; i += 4){
         guint32 IndividualLength;
         IndividualLength = str2num(&BasicOffsetTableItemValue[i],guint32);
         std::ostringstream s;
         s << "   Read one length: ";
         s << std::hex << IndividualLength << std::endl;
         dbg.Verbose(0, "gdcmDocument::Parse7FE0: ", s.str().c_str());
      }              
   }

   if ( ! IsRLELossLessTransferSyntax() )
   {
      // JPEG Image
      
      //// We then skip (not reading them) all the fragments of images:
      while ( (ItemLength = ReadItemTagLength()) )
      {
         SkipBytes(ItemLength);
      } 

   }
   else
   {
      // RLE Image
      long ftellRes;
      long RleSegmentLength[15], fragmentLength;

      // while 'Sequence Delimiter Item' (fffe,e0dd) not found
      while ( (fragmentLength = ReadSequenceDelimiterTagLength()) )
      { 
         // Parse fragments of the current Fragment (Frame)    
         //------------------ scanning (not reading) fragment pixels
         guint32 nbRleSegments = ReadInt32();
         printf("   Nb of RLE Segments : %d\n",nbRleSegments);
 
         //// Reading RLE Segments Offset Table
         guint32 RleSegmentOffsetTable[15];
         for(int k=1; k<=15; k++) {
            ftellRes=ftell(fp);
            RleSegmentOffsetTable[k] = ReadInt32();
            printf("        at : %x Offset Segment %d : %d (%x)\n",
                    (unsigned)ftellRes,k,RleSegmentOffsetTable[k],
                    RleSegmentOffsetTable[k]);
         }

         // skipping (not reading) RLE Segments
         if (nbRleSegments>1) {
            for(unsigned int k=1; k<=nbRleSegments-1; k++) { 
                RleSegmentLength[k]=   RleSegmentOffsetTable[k+1]
                                     - RleSegmentOffsetTable[k];
                ftellRes=ftell(fp);
                printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                        k,(unsigned)RleSegmentLength[k],
                       (unsigned)RleSegmentLength[k], (unsigned)ftellRes);
                SkipBytes(RleSegmentLength[k]);    
             }
          }

          RleSegmentLength[nbRleSegments]= fragmentLength 
                                         - RleSegmentOffsetTable[nbRleSegments];
          ftellRes=ftell(fp);
          printf ("  Segment %d : Length = %d x(%x) Start at %x\n",
                  nbRleSegments,(unsigned)RleSegmentLength[nbRleSegments],
                  (unsigned)RleSegmentLength[nbRleSegments],(unsigned)ftellRes);
          SkipBytes(RleSegmentLength[nbRleSegments]); 
      } 
   }
}



/**
 * \brief   Compares two documents, according to \ref gdcmDicomDir rules
 * \warning Does NOT work with ACR-NEMA files
 * \todo    Find a trick to solve the pb (use RET fields ?)
 * @param   document
 * @return  true if 'smaller'
 */
bool gdcmDocument::operator<(gdcmDocument &document)
{
   std::string s1,s2;
                                                                                
   // Patient Name
   s1=this->GetEntryByNumber(0x0010,0x0010);
   s2=document.GetEntryByNumber(0x0010,0x0010);
   if(s1 < s2)
      return true;
   else if(s1 > s2)
      return false;
   else
   {
      // Patient ID
      s1=this->GetEntryByNumber(0x0010,0x0020);
      s2=document.GetEntryByNumber(0x0010,0x0020);
      if (s1 < s2)
         return true;
      else if (s1 > s2)
         return true;
      else
      {
         // Study Instance UID
         s1=this->GetEntryByNumber(0x0020,0x000d);
         s2=document.GetEntryByNumber(0x0020,0x000d);
         if (s1 < s2)
            return true;
         else if(s1 > s2)
            return false;
         else
         {
            // Serie Instance UID
            s1=this->GetEntryByNumber(0x0020,0x000e);
            s2=document.GetEntryByNumber(0x0020,0x000e);
            if (s1 < s2)
               return true;
            else if(s1 > s2)
               return false;
         }
      }
   }
   return false;
}


//-----------------------------------------------------------------------------
