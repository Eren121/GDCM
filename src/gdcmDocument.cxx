// gdcmDocument.cxx
//-----------------------------------------------------------------------------

#include "gdcmDocument.h"
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

#define UI1_2_840_10008_1_2      "1.2.840.10008.1.2"
#define UI1_2_840_10008_1_2_1    "1.2.840.10008.1.2.1"
#define UI1_2_840_10008_1_2_2    "1.2.840.10008.1.2.2"
#define UI1_2_840_10008_1_2_1_99 "1.2.840.10008.1.2.1.99"

typedef struct {
   guint32 totalSQlength;
   guint32 alreadyParsedlength;
} pileElem;

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
gdcmDocument::gdcmDocument(const char *inFilename, 
                       bool exception_on_error,
                       bool enable_sequences,
                       bool ignore_shadow) {
   enableSequences=enable_sequences;
   ignoreShadow   =ignore_shadow;
   
   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE); 
   filename = inFilename;
   Initialise();

   if ( !OpenFile(exception_on_error))
      return;

   rewind(fp); 
   if (!CheckSwap()) 
      return NULL; // Du menage à faire avant de sortir ?
   LoadDocEntries();   
   CloseFile();

   wasUpdated = 0;  // will be set to 1 if user adds an entry
   printLevel = 1;  // 'Medium' print level by default
}

/**
 * \brief  constructor 
 * @param   exception_on_error
 */
gdcmDocument::gdcmDocument(bool exception_on_error) {
	(void)exception_on_error;
   enableSequences=0;

   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE);
   Initialise();

   wasUpdated = 0;  // will be set to 1 if user adds an entry
   printLevel = 1;  // 'Medium' print level by default
}

/**
 * \brief   Canonical destructor.
 */
gdcmDocument::~gdcmDocument (void) {
   RefPubDict = NULL;
   RefShaDict = NULL;
}

//-----------------------------------------------------------------------------
// Print
/**
  * \brief   Prints the Doc Entries (Dicom Elements)
  *          from the chained list
  * @return
  */ 
void gdcmDocument::PrintEntry(std::ostream & os) {

   for (ListTag::iterator i = listEntries.begin();  
        i != listEntries.end();
        ++i)
   {
      (*i)->SetPrintLevel(printLevel);
      (*i)->Print(os);   
   } 
}


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
   if(filetype==Unknown) {
      return(false);
   }
   if(listEntries.size()<=0) {    
      return(false);
   }

   return(true);
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsImplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry *Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == UI1_2_840_10008_1_2 )
      return true;
   return false;
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsExplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == UI1_2_840_10008_1_2_1 )
      return true;
   return false;
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmDocument::IsDeflatedExplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == UI1_2_840_10008_1_2_1_99 )
      return true;
   return false;
}

/**
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmDocument::IsExplicitVRBigEndianTransferSyntax(void) {
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == UI1_2_840_10008_1_2_2 )  //1.2.2 ??? A verifier !
      return true;
   return false;
}

/**
 * \brief  returns the File Type 
 *         (ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown)
 * @return the FileType code
 */
FileType gdcmDocument::GetFileType(void) {
   return(filetype);
}

/**
 * \brief   opens the file
 * @param   exception_on_error
 * @return  
 */
FILE *gdcmDocument::OpenFile(bool exception_on_error)
  throw(gdcmFileError) 
{
  fp=fopen(filename.c_str(),"rb");
  if(exception_on_error) 
  {
    if(!fp)
      throw gdcmFileError("gdcmDocument::gdcmDocument(const char *, bool)");
  }

  if ( fp ) 
  {
     guint16 zero;
     fread(&zero,  (size_t)2, (size_t)1, fp);

    //ACR -- or DICOM with no Preamble --
    if( zero == 0x0008 || zero == 0x0800 || zero == 0x0002 || zero == 0x0200)
       return(fp);

    //DICOM
    fseek(fp, 126L, SEEK_CUR);
    char dicm[4];
    fread(dicm,  (size_t)4, (size_t)1, fp);
    if( memcmp(dicm, "DICM", 4) == 0 )
       return(fp);

    fclose(fp);
    dbg.Verbose(0, "gdcmDocument::OpenFile not DICOM/ACR", filename.c_str());
  }
  else {
    dbg.Verbose(0, "gdcmDocument::OpenFile cannot open file", filename.c_str());
  }
  return(NULL);
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
 * \brief Writes in a file all the Doc Entries (Dicom Elements) 
 *        of the Chained List
 * @param fp file pointer on an already open file
 * @param type Type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * \return Always true.
 */
bool gdcmDocument::Write(FILE *fp, FileType type) {
/// \todo
///  ==============
///      The stuff will have to be rewritten using the SeQuence based 
///       tree-like stucture instead  of the chained list .
///      (so we shall remove the GroupHT from the gdcmDocument)
///      To be checked
/// =============

   /// \todo move the following lines (and a lot of others, to be written)
   /// to a future function CheckAndCorrectHeader
   
   /// \todo
   /// Question :
   /// Comment pourrait-on savoir si le DcmHeader vient d'un fichier
   /// DicomV3 ou non (FileType est un champ de gdcmDocument ...)
   /// WARNING : Si on veut ecrire du DICOM V3 a partir d'un DcmHeader ACR-NEMA
   /// no way 
   /// a moins de se livrer a un tres complique ajout des champs manquants.
   /// faire un CheckAndCorrectHeader (?)  

   if (type == ImplicitVR) 
   {
      std::string implicitVRTransfertSyntax = UI1_2_840_10008_1_2;
      ReplaceOrCreateByNumber(implicitVRTransfertSyntax,0x0002, 0x0010);
      
      /// \todo Refer to standards on page 21, chapter 6.2
      ///       "Value representation": values with a VR of UI shall be
      ///       padded with a single trailing null
      ///       Dans le cas suivant on doit pader manuellement avec un 0
      
      SetEntryLengthByNumber(18, 0x0002, 0x0010);
   } 

   if (type == ExplicitVR) 
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
 *
 *       --> Warning : un-updated odd groups lengths can causes pb 
 *       -->           (xmedcon breaks)
 *       --> to be re- written with future org.
 *
 * if ( (type == ImplicitVR) || (type == ExplicitVR) )
 *    UpdateGroupLength(false,type);
 * if ( type == ACR)
 *    UpdateGroupLength(true,ACR);
 */

   WriteEntries(fp,type);
   return(true);
}

/**
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          when it exists. Create it with the given value when unexistant.
 * \warning Adds the Header Entry to the HTable, NOT to the chained List
 * @param   Value Value to be set
 * @param   Group Group of the Entry 
 * @param   Elem  Element of the Entry
 * \return  pointer to the modified/created Header Entry (NULL when creation
 *          failed).
 */
gdcmHeaderEntry * gdcmDocument::ReplaceOrCreateByNumber(
                                         std::string Value, 
                                         guint16 Group, 
                                         guint16 Elem ){
   gdcmHeaderEntry* a;
   a = GetHeaderEntryByNumber( Group, Elem);
   if (a == NULL) {
      a =NewHeaderEntryByNumber(Group, Elem);
      if (a == NULL) 
         return NULL;
      AddHeaderEntry(a);
   }   
   //CLEANME SetEntryByNumber(Value, Group, Elem);
   a->SetValue(Value);
   return(a);
}   

/**
 * \brief Set a new value if the invoked element exists
 *        Seems to be useless !!!
 * @param Value new element value
 * @param Group   group of the Entry 
 * @param Elem element of the Entry
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
 * \brief   Checks if a given Dicom Element exists
 *          within the H table
 * @param   group Group   number of the searched Dicom Element 
 * @param   element  Element number of the searched Dicom Element 
 * @return  number of occurences
 */
int gdcmDocument::CheckIfEntryExistByNumber(guint16 group, guint16 element ) {
   std::string key = gdcmDictEntry::TranslateToKey(group, element );
   return (tagHT.count(key));
}

/**
 * \brief   Searches within Doc Entries (Dicom Elements) parsed with 
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

   return(GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));  
}

/**
 * \brief   Searches within Doc Entries (Dicom Elements) parsed with 
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
std::string gdcmDocument::GetEntryVRByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(dictEntry->GetGroup(),
                                                   dictEntry->GetElement());
   return elem->GetVR();
}


/**
 * \brief   Searches within Doc Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryByNumber(guint16 group, guint16 element){
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return GDCM_UNFOUND;
   return tagHT.find(key)->second->GetValue();
}

/**
 * \brief   Searches within Doc Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag..
 *
 *          Obtaining the VR (Value Representation) might be needed by caller
 *          to convert the string typed content to caller's native type 
 *          (think of C++ vs Python). The VR is actually of a higher level
 *          of semantics than just the native C++ type.
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmDocument::GetEntryVRByNumber(guint16 group, guint16 element) {
   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \brief   Searches within Doc Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the value length of a given tag..
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element length; -2 if not found
 */
int gdcmDocument::GetEntryLengthByNumber(guint16 group, guint16 element) {
   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(group, element);
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

   return(SetEntryByNumber(content,dictEntry->GetGroup(),
                                   dictEntry->GetElement()));
}

/**
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * \warning Don't use any longer : use SetPubEntryByNumber
 * @param   content new value to substitute with
 * @param   group   group of the Dicom Element to modify
 * @param   element element of the Dicom Element to modify
 */
bool gdcmDocument::SetEntryByNumber(std::string content, 
                                  guint16 group,
                                  guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   int l = content.length();
   if(l%2) // Non even length are padded with a space (020H).
   {  
      l++;
      content = content + '\0';
   }
      
   gdcmHeaderEntry * a;
   IterHT p;
   TagHeaderEntryHT::iterator p2;
   // DO NOT remove the following lines : they explain the stuff   
   //p= tagHT.equal_range(key); // get a pair of iterators first-last synonym
   //p2=p.first;                // iterator on the first synonym 
   //a=p2->second;              // H Table target column (2-nd col)
    
   // or, easier :
   a = ((tagHT.equal_range(key)).first)->second; 
       
   a-> SetValue(content); 
   
   std::string vr = a->GetVR();
   
   guint32 lgr;
   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;
   else
      lgr = l;   

   a->SetLength(lgr);   
   return true;
}  

/**
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          in the PubHeaderEntrySet of this instance
 *          through it's (group, element) and modifies it's length with
 *          the given value.
 * \warning Use with extreme caution.
 * @param l new length to substitute with
 * @param group   group of the Entry to modify
 * @param element element of the Entry to modify
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
 * @param Group   group of the Entry 
 * @param Elem  element of the Entry
 * @return File Offset of the Element Value 
 */
size_t gdcmDocument::GetEntryOffsetByNumber(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmDocument::GetHeaderEntryByNumber",
                      "failed to Locate gdcmHeaderEntry");
      return (size_t)0;
   }
   return Entry->GetOffset();
}

/**
 * \brief   Gets (from Header) a 'non string' element value 
 *          (LoadElementValues has already be executed)  
 * @param Group   group of the Entry 
 * @param Elem element of the Entry
 * @return Pointer to the 'non string' area
 */
void * gdcmDocument::GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmDocument::GetHeaderEntryByNumber",
                  "failed to Locate gdcmHeaderEntry");
      return (NULL);
   }
   return Entry->GetVoidArea();
}

/**
 * \brief         Loads (from disk) the element content 
 *                when a string is not suitable
 * @param Group   group of the Entry 
 * @param Elem element of the Entry
 */
void *gdcmDocument::LoadEntryVoidArea(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry * Element= GetHeaderEntryByNumber(Group, Elem);
   if ( !Element )
      return NULL;
   size_t o =(size_t)Element->GetOffset();
   fseek(fp, o, SEEK_SET);
   size_t l=Element->GetLength();
   char* a = new char[l];
   if(!a) 
      return NULL;

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
 * @param   area
 * @param   group Group number of the searched Dicom Element 
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
   ( ((tagHT.equal_range(key)).first)->second )->SetVoidArea(area); 
   return true;
}

/**
 * \brief   Update the entries with the shadow dictionary. 
 *          Only non even entries are analyzed       
 */
void gdcmDocument::UpdateShaEntries(void) {
   gdcmDictEntry *entry;
   std::string vr;

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

      (*it)->SetValue(GetHeaderEntryUnvalue(*it));
      if(entry){
         // Set the new entry and the new value
         (*it)->SetDictEntry(entry);
         CheckHeaderEntryVR(*it,vr);

         (*it)->SetValue(GetHeaderEntryValue(*it));
      }
      else
      {
         // Remove precedent value transformation
         (*it)->SetDictEntry(NewVirtualDictEntry((*it)->GetGroup(),(*it)->GetElement(),vr));
      }
   }
}

/**
 * \brief   Searches within the Doc Entries for a Dicom Element of
 *          a given tag.
 * @param   tagName name of the searched Dicom Element.
 * @return  Corresponding Dicom Element when it exists, and NULL
 *          otherwise.
 */
 gdcmHeaderEntry *gdcmDocument::GetHeaderEntryByName(std::string tagName) {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return NULL;

  return(GetHeaderEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));
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
gdcmHeaderEntry* gdcmDocument::GetHeaderEntryByNumber(guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);   
   if ( ! tagHT.count(key))
      return NULL;
   return tagHT.find(key)->second;
}

/**
 * \brief   retrieves the Dicom Elements (all of them) using (group, element) 
 * @param   group Group number of the searched Dicom Element.
 * @param   element Element number of the searched Dicom Element.
 * @return  a range (i.e.pair<,>) containing all elements whose key is group|element) 
 */
 
IterHT gdcmDocument::GetHeaderEntrySameNumber(guint16 group, guint16 element){
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   return (tagHT.equal_range(key));
}

/**
 * \brief         Loads the element while preserving the current
 *                underlying file position indicator as opposed to
 *                to LoadHeaderEntry that modifies it.
 * @param entry   Header Entry whose value shall be loaded. 
 * @return  
 */
void gdcmDocument::LoadHeaderEntrySafe(gdcmHeaderEntry * entry) {
   long PositionOnEntry = ftell(fp);
   LoadHeaderEntry(entry);
   fseek(fp, PositionOnEntry, SEEK_SET);
}

/**
 * \brief   Re-computes the length of a ACR-NEMA/Dicom group from a DcmHeader
 * \warning : to be re-written using the chained list instead of the H table.
 * \warning : DO NOT use (doesn't work any longer because of the multimap)
 * \todo : to be re-written using the chained list instead of the H table
 * @param   SkipSequence TRUE if we don't want to write Sequences (ACR-NEMA Files)
 * @param   type Type of the File (ExplicitVR,ImplicitVR, ACR, ...) 
 */
void gdcmDocument::UpdateGroupLength(bool SkipSequence, FileType type) {
   guint16 gr, el;
   std::string vr;
   
   gdcmHeaderEntry *elem;
   char trash[10];
   std::string str_trash;
   
   GroupKey key;
   GroupHT groupHt;  // to hold the length of each group
   TagKey tk;
   // remember :
   // typedef std::map<GroupKey, int> GroupHT;
   
   gdcmHeaderEntry *elemZ;
  
   // for each Tag in the DCM Header
   
   for (TagHeaderEntryHT::iterator tag2 = tagHT.begin(); 
        tag2 != tagHT.end();
        ++tag2)
   {
      elem  = tag2->second;
      gr = elem->GetGroup();
      el = elem->GetElement();
      vr = elem->GetVR(); 
                 
      sprintf(trash, "%04x", gr);
      key = trash;   // generate 'group tag'
      
      // if the caller decided not to take SEQUENCEs into account 
      // e.g : he wants to write an ACR-NEMA File 
                
      if (SkipSequence && vr == "SQ") 
         continue;
      
      // Still unsolved problem :
      // we cannot find the 'Sequence Delimitation Item'
      // since it's at the end of the Hash Table
      // (fffe,e0dd) 
       
      // there is SEQUENCE in ACR-NEMA
      // WARNING : 
      // --> la descente a l'interieur' des SQ 
      // devrait etre faite avec une liste chainee, pas avec une HTable...
            
      if ( groupHt.count(key) == 0) // we just read the first elem of a given group
      { 
         if (el == 0x0000) // the first elem is 0x0000
         {
            groupHt[key] = 0;      // initialize group length 
         } 
         else 
         {
            groupHt[key] = 2 + 2 + 4 + elem->GetLength(); // non 0x0000 first group elem
         } 
      } 
      else // any elem but the first
      {   
         if (type == ExplicitVR) 
         {
            if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) 
            {
               groupHt[key] +=  4; // explicit VR AND OB, OW, SQ : 4 more bytes
            }
         }
         groupHt[key] += 2 + 2 + 4 + elem->GetLength(); 
      } 
   }

   unsigned short int gr_bid;
  
   for (GroupHT::iterator g = groupHt.begin(); // for each group we found
        g != groupHt.end();
        ++g)
   { 
      // FIXME: g++ -Wall -Wstrict-prototypes reports on following line:
      //        warning: unsigned int format, different type arg
      sscanf(g->first.c_str(),"%x",&gr_bid); //FIXME
      tk = g->first + "|0000";   // generate the element full tag
                     
      if ( tagHT.count(tk) == 0) // if element 0x0000 not found
      {
         gdcmDictEntry * tagZ = new gdcmDictEntry(gr_bid, 0x0000, "UL");       
         elemZ = new gdcmHeaderEntry(tagZ);
         elemZ->SetLength(4);
         AddHeaderEntry(elemZ);   // create it
      } 
      else 
      {
         elemZ=GetHeaderEntryByNumber(gr_bid, 0x0000);
      }     
      sprintf(trash ,"%d",g->second);
      str_trash=trash;
      elemZ->SetValue(str_trash);
   }   
}

/**
 * \brief Writes in a file (according to the requested format)
 *        the group, the element, the value representation and the length
 *        of a single gdcmHeaderEntry passed as argument.
 * @param tag  pointer on the gdcmHeaderEntry to be written
 * @param _fp  already open file pointer
 * @param type type of the File to be written 
 */
void gdcmDocument::WriteEntryTagVRLength(gdcmHeaderEntry *tag,
                                       FILE *_fp,
                                       FileType type)
{
   guint16 group  = tag->GetGroup();
   std::string vr = tag->GetVR();
   guint16 el     = tag->GetElement();
   guint32 lgr    = tag->GetReadLength();

   if ( (group == 0xfffe) && (el == 0x0000) ) 
     // Fix in order to make some MR PHILIPS images e-film readable
     // see gdcmData/gdcm-MR-PHILIPS-16-Multi-Seq.dcm:
     // we just *always* ignore spurious fffe|0000 tag !   
      return; 

   fwrite ( &group,(size_t)2 ,(size_t)1 ,_fp);  //group
   fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp);     //element
      
   if ( type == ExplicitVR ) {

      // Special case of delimiters:
      if (group == 0xfffe) {
         // Delimiters have NO Value Representation and have NO length.
         // Hence we skip writing the VR and length and we pad by writing
         // 0xffffffff

         int ff=0xffffffff;
         fwrite (&ff,(size_t)4 ,(size_t)1 ,_fp);
         return;
      }

      guint16 z=0;
      guint16 shortLgr = lgr;
      if (vr == "unkn") {     // Unknown was 'written'
         // deal with Little Endian            
         fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,_fp);
         fwrite ( &z,  (size_t)2 ,(size_t)1 ,_fp);
      } else {
         fwrite (vr.c_str(),(size_t)2 ,(size_t)1 ,_fp);                     
         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") )
         {
            fwrite ( &z,  (size_t)2 ,(size_t)1 ,_fp);
            fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);
         } else {
            fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,_fp);
         }
      }
   } 
   else // IMPLICIT VR 
   { 
      fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);
   }
}
      
/**
 * \brief Writes in a file (according to the requested format)
 *        the value of a single gdcmHeaderEntry passed as argument.
 * @param tag  Pointer on the gdcmHeaderEntry to be written
 * @param _fp  Already open file pointer
 * @param type type of the File to be written
 */
void gdcmDocument::WriteEntryValue(gdcmHeaderEntry *tag, FILE *_fp,FileType type)
{
   (void)type;
   guint16 group  = tag->GetGroup();
   std::string vr = tag->GetVR();
   guint32 lgr    = tag->GetReadLength();

   if (vr == "SQ")
      // SeQuences have no value:
      return;
   if (group == 0xfffe)
      // Delimiters have no associated value:
      return;
      
   void *voidArea;
   voidArea = tag->GetVoidArea();
   if (voidArea != NULL) 
   { // there is a 'non string' LUT, overlay, etc
      fwrite ( voidArea,(size_t)lgr ,(size_t)1 ,_fp); // Elem value
      return;            
   }
      
   if (vr == "US" || vr == "SS") 
   {
      // some 'Short integer' fields may be mulivaluated
      // each single value is separated from the next one by '\'
      // we split the string and write each value as a short int
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Tokenize (tag->GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++) 
      {
         guint16 val_uint16 = atoi(tokens[i].c_str());
         void *ptr = &val_uint16;
         fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);
      }
      tokens.clear();
      return;
   }
      // some 'Integer' fields may be mulivaluated
      // each single value is separated from the next one by '\'
      // we split the string and write each value as an int
   if (vr == "UL" || vr == "SL") 
   {
      std::vector<std::string> tokens;
      tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
      Tokenize (tag->GetValue(), tokens, "\\");
      for (unsigned int i=0; i<tokens.size();i++) 
      {
         guint32 val_uint32 = atoi(tokens[i].c_str());
         void *ptr = &val_uint32;
         fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);
      }
      tokens.clear();
      return;
   }           
   fwrite (tag->GetValue().c_str(), (size_t)lgr ,(size_t)1, _fp); // Elem value
}

/**
 * \brief Writes in a file (according to the requested format)
 *        a single gdcmHeaderEntry passed as argument.
 * \sa    WriteEntryValue, WriteEntryTagVRLength.
 * @param tag  Pointer on the gdcmHeaderEntry to be written
 * @param _fp  Already open file pointer
 * @param type type of the File to be written
 */

bool gdcmDocument::WriteEntry(gdcmHeaderEntry *tag, FILE *_fp,FileType type)
{
   guint32 length = tag->GetLength();

   // The value of a tag MUST (see the DICOM norm) be an odd number of
   // bytes. When this is not the case, pad with an additional byte:
   if(length%2==1)
   { 
      tag->SetValue(tag->GetValue()+"\0");
      tag->SetLength(tag->GetReadLength()+1);
   }

   WriteEntryTagVRLength(tag, _fp, type);
   WriteEntryValue(tag, _fp, type);
   return true;
}

/**
 * \brief   writes on disc according to the requested format
 *          (ACR-NEMA, ExplicitVR, ImplicitVR) the image
 *          using the Chained List
 * \warning does NOT add the missing elements in the header :
 *           it's up to the user doing it !
 *           (function CheckHeaderCoherence to be written)
 * \warning DON'T try, right now, to write a DICOM image
 *           from an ACR Header (meta elements will be missing!)
 * \sa WriteEntriesDeprecated (Special temporary method for Theralys)
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @param   _fp already open file pointer
 */

bool gdcmDocument::WriteEntries(FILE *_fp,FileType type)
{   
   /// \todo (?) check write failures (after *each* fwrite)
     
   for (ListTag::iterator tag2=listEntries.begin();
                          tag2 != listEntries.end();
                          ++tag2)
   {
      if ( type == ACR ){ 
         if ((*tag2)->GetGroup() < 0x0008)
            // Ignore pure DICOM V3 groups
            continue;
         if ((*tag2)->GetElement() %2)
            // Ignore the "shadow" groups
            continue;
         if ((*tag2)->GetVR() == "SQ" ) // ignore Sequences
            continue;
         if ((*tag2)->GetSQDepthLevel() != 0) // Not only ignore the SQ element
            continue;	    
      } 
      if (! WriteEntry(*tag2,_fp,type) )
         return false;
   }
   return true;
}   

/**
 * \brief   writes on disc according to the requested format
 *          (ACR-NEMA, ExplicitVR, ImplicitVR) the image,
 *          using only the last synonym of each mutimap H Table post.
 * \warning Uses the H Table, instead of the Chained List
 *          in order to be compliant with the old way to proceed
 *         (added elements taken in to account)
 *         Only THERALYS, during a transitory phase is supposed
 *         to use this method !!!
 * \warning DON'T try, right now, to write a DICOM image
 *           from an ACR Header (meta elements will be missing!)
 * \sa WriteEntries
 * @param   _fp already open file pointer
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 */
void gdcmDocument::WriteEntriesDeprecated(FILE *_fp,FileType type) {

   // restent a tester les echecs en ecriture (apres chaque fwrite)

   for (TagHeaderEntryHT::iterator tag2=tagHT.begin();
        tag2 != tagHT.end();
        ++tag2){
      if ( type == ACR ){ 
         if ((*tag2->second).GetGroup() < 0x0008)    continue; // ignore pure DICOM V3 groups
         if ((*tag2->second).GetElement() %2)        continue; // ignore shadow groups
         if ((*tag2->second).GetVR() == "SQ" )       continue; // ignore Sequences
         if ((*tag2->second).GetSQDepthLevel() != 0) continue; // Not only ignore the SQ element          
      }
      if ( ! WriteEntry(tag2->second,_fp,type))
         break;
   }
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
 * \brief         Loads the element content if its length doesn't exceed
 *                the value specified with gdcmDocument::SetMaxSizeLoadEntry()
 * @param         Entry Header Entry (Dicom Element) to be dealt with
 */
void gdcmDocument::LoadHeaderEntry(gdcmHeaderEntry *Entry)  {
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
      Entry->SetValue("gdcm::Skipped");
      return;
   }

   // When the length is zero things are easy:
   if ( length == 0 ) {
      Entry->SetValue("");
      return;
   }

   // The elements whose length is bigger than the specified upper bound
   // are not loaded. Instead we leave a short notice of the offset of
   // the element content and it's length.
   if (length > MaxSizeLoadEntry) {
      std::ostringstream s;
      s << "gdcm::NotLoaded.";
      s << " Address:" << (long)Entry->GetOffset();
      s << " Length:"  << Entry->GetLength();
      s << " x(" << std::hex << Entry->GetLength() << ")";
      Entry->SetValue(s.str());
      return;
   }
    
   // Any compacter code suggested (?)
   if ( IsHeaderEntryAnInteger(Entry) ) {   
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

      Entry->SetValue(s.str());
      return;
   }
   
   // We need an additional byte for storing \0 that is not on disk
   std::string NewValue(length,0);
   item_read = fread(&(NewValue[0]), (size_t)length, (size_t)1, fp);
   if ( item_read != 1 ) {
      dbg.Verbose(1, "gdcmDocument::LoadElementValue","unread element value");
      Entry->SetValue("gdcm::UnRead");
      return;
   }

   if( (vr == "UI") ) // Because of correspondance with the VR dic
      Entry->SetValue(NewValue.c_str());
   else
      Entry->SetValue(NewValue);
}

/**
 * \brief   add a new Dicom Element pointer to 
 *          the H Table and at the end of the chained List
 * \warning push_bash in listEntries ONLY during ParseHeader
 * \todo    something to allow further Elements addition,
 *          (at their right place in the chained list)
 *          when position to be taken care of     
 * @param   newHeaderEntry
 */
void gdcmDocument::AddHeaderEntry(gdcmHeaderEntry *newHeaderEntry) {
   tagHT.insert( PairHT( newHeaderEntry->GetKey(),newHeaderEntry) );
   listEntries.push_back(newHeaderEntry); 
   wasUpdated = 1;
}

/**
 * \brief  Find the value Length of the passed Header Entry
 * @param  Entry Header Entry whose length of the value shall be loaded. 
 */
 void gdcmDocument::FindHeaderEntryLength (gdcmHeaderEntry *Entry) {
   guint16 element = Entry->GetElement();
   //guint16 group   = Entry->GetGroup(); //FIXME
   std::string  vr = Entry->GetVR();
   guint16 length16;
       
   
   if ( (filetype == ExplicitVR) && (! Entry->IsImplicitVR()) ) 
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
            Entry->SetLength(FindHeaderEntryLengthOB());
            return;
         }
         FixHeaderEntryFoundLength(Entry, length32); 
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
      FixHeaderEntryFoundLength(Entry, (guint32)length16);
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
      
      FixHeaderEntryFoundLength(Entry, ReadInt32());
      return;
   }
}

/**
 * \brief     Find the Value Representation of the current Dicom Element.
 * @param     Entry
 */
void gdcmDocument::FindHeaderEntryVR( gdcmHeaderEntry *Entry) 
{
   if (filetype != ExplicitVR)
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
   if(!CheckHeaderEntryVR(Entry,VR))
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
bool gdcmDocument::CheckHeaderEntryVR(gdcmHeaderEntry *Entry, VRKey vr)
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
std::string gdcmDocument::GetHeaderEntryValue(gdcmHeaderEntry *Entry)
{
   if ( (IsHeaderEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string val=Entry->GetValue();
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

   return(Entry->GetValue());
}

/**
 * \brief   Get the reverse transformed value of the header entry. The VR 
 *          value is used to define the reverse transformation to operate on
 *          the value
 * \warning NOT end user intended method !
 * @param   Entry 
 * @return  Reverse transformed entry value
 */
std::string gdcmDocument::GetHeaderEntryUnvalue(gdcmHeaderEntry *Entry)
{
   if ( (IsHeaderEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string vr=Entry->GetVR();
      std::ostringstream s;
      std::vector<std::string> tokens;

      if (vr == "US" || vr == "SS") 
      {
         guint16 NewInt16;

         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize (Entry->GetValue(), tokens, "\\");
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
         Tokenize (Entry->GetValue(), tokens, "\\");
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

   return(Entry->GetValue());
}

/**
 * \brief   Skip a given Header Entry 
 * \warning NOT end user intended method !
 * @param   entry 
 */
void gdcmDocument::SkipHeaderEntry(gdcmHeaderEntry *entry) 
{
    SkipBytes(entry->GetLength());
}

/**
 * \brief   When the length of an element value is obviously wrong (because
 *          the parser went Jabberwocky) one can hope improving things by
 *          applying this heuristic.
 */
void gdcmDocument::FixHeaderEntryFoundLength(gdcmHeaderEntry *Entry, guint32 FoundLength) 
{
   Entry->SetReadLength(FoundLength); // will be updated only if a bug is found        
   if ( FoundLength == 0xffffffff) {
      FoundLength = 0;
   }
   
   guint16 gr =Entry->GetGroup();
   guint16 el =Entry->GetElement(); 
     
   if (FoundLength%2) {
      std::ostringstream s;
      s << "Warning : Tag with uneven length " << FoundLength 
         <<  " in x(" << std::hex << gr << "," << el <<")" << std::dec;
      dbg.Verbose(0,s.str().c_str());
   }
      
   // Sorry for the patch!  
   // XMedCom did the trick to read some naughty GE images ...
   if (FoundLength == 13) {
      // The following 'if' will be removed when there is no more
      // images on Creatis HDs with a 13 length for Manufacturer...
      if ( (Entry->GetGroup() != 0x0008) ||  
           ( (Entry->GetElement() != 0x0070) && (Entry->GetElement() != 0x0080) ) ){
      // end of remove area
         FoundLength =10;
         Entry->SetReadLength(10); // a bug is to be fixed
      }
   }

   // to fix some garbage 'Leonardo' Siemens images
   // May be commented out to avoid overhead
   else if ( (Entry->GetGroup() == 0x0009) &&
       ( (Entry->GetElement() == 0x1113) || (Entry->GetElement() == 0x1114) ) ){
      FoundLength =4;
      Entry->SetReadLength(4); // a bug is to be fixed 
   } 
   // end of fix
 
   // to try to 'go inside' SeQuences (with length), and not to skip them        
   else if ( Entry->GetVR() == "SQ") 
   { 
      if (enableSequences)    // only if the user does want to !
         FoundLength =0;      // ReadLength is unchanged 
   } 
    
   // we found a 'delimiter' element                                         
   // fffe|xxxx is just a marker, we don't take its length into account                                                   
   else if(Entry->GetGroup() == 0xfffe)
   {    
                                         // *normally, fffe|0000 doesn't exist ! 
     if( Entry->GetElement() != 0x0000 ) // gdcm-MR-PHILIPS-16-Multi-Seq.dcm
                                         // causes extra troubles :-(                                                              	
        FoundLength =0;
   } 
           
   Entry->SetUsableLength(FoundLength);
}

/**
 * \brief   Apply some heuristics to predict whether the considered 
 *          element value contains/represents an integer or not.
 * @param   Entry The element value on which to apply the predicate.
 * @return  The result of the heuristical predicate.
 */
bool gdcmDocument::IsHeaderEntryAnInteger(gdcmHeaderEntry *Entry) {
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
         std::ostringstream s;
         int filePosition = ftell(fp);
         s << "Erroneous Group Length element length  on : (" \
           << std::hex << group << " , " << element 
           << ") -before- position x(" << filePosition << ")"
           << "lgt : " << length;
         // These 2 lines commented out : a *very dirty* patch
         // to go on PrintHeader'ing gdcm-MR-PHILIPS-16-Multi-Seq.dcm.
         // have a glance at offset  x(8336) ...
         // For *regular* headers, the test is useless..
         // lets's print a warning message and go on, 
         // instead of giving up with an error message

         //std::cout << s.str().c_str() << std::endl;
         // dbg.Error("gdcmDocument::IsHeaderEntryAnInteger",
         //           s.str().c_str());     
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

 guint32 gdcmDocument::FindHeaderEntryLengthOB(void)  {
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
guint16 gdcmDocument::ReadInt16(void) {
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
guint32 gdcmDocument::ReadInt32(void) {
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
   RefShaDict = (gdcmDict*)0;
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
         filetype = ExplicitVR;
         dbg.Verbose(1, "gdcmDocument::CheckSwap:",
                     "explicit Value Representation");
      } 
      else 
      {
         filetype = ImplicitVR;
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
         filetype = ACR;
         return true;
      case 0x04000000 :
         sw = 4321;
         filetype = ACR;
         return true;
      case 0x00000400 :
         sw = 2143;
         filetype = ACR;
         return true;
      case 0x00000004 :
         sw = 0;
         filetype = ACR;
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
         filetype = ACR;
         return true;
      case 0x0200 :
      case 0x0400 :
      case 0x0800 : 
         sw = 4321;
         filetype = ACR;
         return true;
      default :
         dbg.Verbose(0, "gdcmDocument::CheckSwap:",
                     "ACR/NEMA unfound swap info (Really hopeless !)"); 
         filetype = Unknown;     
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
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given name.
 *          The public dictionary has precedence on the shadow one.
 * @param   Name name of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmDocument::GetDictEntryByName(std::string Name) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   if (!RefPubDict && !RefShaDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   if (RefPubDict) 
   {
      found = RefPubDict->GetDictEntryByName(Name);
      if (found)
         return found;
   }
   if (RefShaDict) 
   {
      found = RefShaDict->GetDictEntryByName(Name);
      if (found)
         return found;
   }
   return found;
}

/**
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one.
 * @param   group   group of the searched DictEntry
 * @param   element element of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmDocument::GetDictEntryByNumber(guint16 group,guint16 element) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   if (!RefPubDict && !RefShaDict) 
   {
      dbg.Verbose(0, "gdcmDocument::GetDictEntry",
                     "we SHOULD have a default dictionary");
   }
   if (RefPubDict) 
   {
      found = RefPubDict->GetDictEntryByNumber(group, element);
      if (found)
         return found;
   }
   if (RefShaDict) 
   {
      found = RefShaDict->GetDictEntryByNumber(group, element);
      if (found)
         return found;
   }
   return found;
}

/**
 * \brief   Read the next tag but WITHOUT loading it's value
 * @return  On succes the newly created HeaderEntry, NULL on failure.      
 */
gdcmHeaderEntry *gdcmDocument::ReadNextHeaderEntry(void) {
   guint16 g,n;
   gdcmHeaderEntry *NewEntry;
   g = ReadInt16();
   n = ReadInt16();
      
   if (errno == 1)
      // We reached the EOF (or an error occured) therefore 
      // header parsing has to be considered as finished.
      return (gdcmHeaderEntry *)0;

// Pb : how to propagate the element length (used in SkipHeaderEntry)
//       direct call to SkipBytes ?
   
//   if (ignoreShadow == 1 && g%2 ==1)
      // if user wants to skip shadow groups
      // and current element *is* a shadow element
      // we don't create anything
//      return (gdcmHeaderEntry *)1; // to tell caller it's NOT finished
  
   NewEntry = NewHeaderEntryByNumber(g, n);
   FindHeaderEntryVR(NewEntry);
   FindHeaderEntryLength(NewEntry);

   if (errno == 1) {
      // Call it quits
      return NULL;
   }
   NewEntry->SetOffset(ftell(fp));  
   return NewEntry;
}

/**
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Name    Name of the underlying DictEntry
 */
gdcmHeaderEntry *gdcmDocument::NewHeaderEntryByName(std::string Name) 
{
   gdcmDictEntry *NewTag = GetDictEntryByName(Name);
   if (!NewTag)
      NewTag = NewVirtualDictEntry(0xffff, 0xffff, "LO", "unkn", Name);

   gdcmHeaderEntry* NewEntry = new gdcmHeaderEntry(NewTag);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::ObtainHeaderEntryByName",
                  "failed to allocate gdcmHeaderEntry");
      return (gdcmHeaderEntry *)0;
   }
   return NewEntry;
}  

/**
 * \brief   Request a new virtual dict entry to the dict set
 * @param   group  group   of the underlying DictEntry
 * @param   element  element of the underlying DictEntry
 * @param   vr     VR of the underlying DictEntry
 * @param   fourth owner group
 * @param   name   english name
 */
gdcmDictEntry *gdcmDocument::NewVirtualDictEntry(guint16 group, guint16 element,
                                               std::string vr,
                                               std::string fourth,
                                               std::string name)
{
   return gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,element,vr,fourth,name);
}

/**
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   of the underlying DictEntry
 * @param   Elem  element of the underlying DictEntry
 */
gdcmHeaderEntry *gdcmDocument::NewHeaderEntryByNumber(guint16 Group, guint16 Elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *DictEntry = GetDictEntryByNumber(Group, Elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(Group, Elem);

   gdcmHeaderEntry *NewEntry = new gdcmHeaderEntry(DictEntry);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmDocument::NewHeaderEntryByNumber",
                  "failed to allocate gdcmHeaderEntry");
      return NULL;
   }
   return NewEntry;
}

/// \todo Never used; commented out, waiting for removal.
/**
 * \brief   Small utility function that creates a new manually crafted
 *          (as opposed as read from the file) gdcmHeaderEntry with user
 *          specified name and adds it to the public tag hash table.
 * \note    A fake TagKey is generated so the PubDict can keep it's coherence.
 * @param   NewTagName The name to be given to this new tag.
 * @param   VR The Value Representation to be given to this new tag.
 * @return  The newly hand crafted Element Value.
 */
//gdcmHeaderEntry *gdcmDocument::NewManualHeaderEntryToPubDict(std::string NewTagName, 
//                                                           std::string VR) 
//{
//   gdcmHeaderEntry *NewEntry = NULL;
//   guint32 StuffGroup = 0xffff;   // Group to be stuffed with additional info
//   guint32 FreeElem = 0;
//   gdcmDictEntry *DictEntry = NULL;
//
//   FreeElem = GenerateFreeTagKeyInGroup(StuffGroup);
//   if (FreeElem == UINT32_MAX) 
//   {
//      dbg.Verbose(1, "gdcmHeader::NewManualHeaderEntryToPubDict",
//                     "Group 0xffff in Public Dict is full");
//      return NULL;
//   }
//
//   DictEntry = NewVirtualDictEntry(StuffGroup, FreeElem,
//                                VR, "GDCM", NewTagName);
//   NewEntry = new gdcmHeaderEntry(DictEntry);
//   AddHeaderEntry(NewEntry);
//   return NewEntry;
//}

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

//-----------------------------------------------------------------------------
