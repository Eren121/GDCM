// gdcmHeader.cxx
//-----------------------------------------------------------------------------
#include "gdcmParser.h"
#include "gdcmUtil.h"
#include <errno.h>

// For nthos:
#ifdef _MSC_VER
   #include <winsock.h>
#else
   #include <netinet/in.h>
#endif

#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif
#  include <iomanip>

//-----------------------------------------------------------------------------
// Refer to gdcmParser::CheckSwap()
const unsigned int gdcmParser::HEADER_LENGTH_TO_READ = 256;

// Refer to gdcmParser::SetMaxSizeLoadElementValue()
const unsigned int gdcmParser::MAX_SIZE_LOAD_ELEMENT_VALUE = 4096;

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmParser
 * \brief   
 * @param   InFilename
 * @param   exception_on_error
 * @param   enable_sequences = true to allow the header 
 *          to be parsed *inside* the SeQuences, 
 *          when they have an actual length 
 *\TODO    may be we need one more bool, 
 *         to allow skipping the private elements while parsing the header
 *         in order to save space	  
 */
gdcmParser::gdcmParser(const char *InFilename, 
                       bool exception_on_error,
                       bool enable_sequences ) 
{
   enableSequences=enable_sequences;
   
   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE);
   filename = InFilename;
   Initialise();

   if ( !OpenFile(exception_on_error))
      return;

   Parse();
   LoadHeaderEntries();

   CloseFile();

   wasUpdated = 0;  // will be set to 1 if user adds an entry
   printLevel = 1;  // 'Medium' header print by default
}

/**
 * \ingroup gdcmParser
 * \brief   
 * @param   exception_on_error
 */
gdcmParser::gdcmParser(bool exception_on_error) 
{
   //enableSequences=0;

   SetMaxSizeLoadEntry(MAX_SIZE_LOAD_ELEMENT_VALUE);
   Initialise();

   wasUpdated = 0;  // will be set to 1 if user adds an entry
   printLevel = 1;  // 'Heavy' header print by default
}

/**
 * \ingroup gdcmParser
 * \brief   Canonical destructor.
 */
gdcmParser::~gdcmParser (void) 
{
   RefPubDict = NULL;
   RefShaDict = NULL;
}

//-----------------------------------------------------------------------------
// Print
/**
  * \ingroup gdcmParser
  * \brief   Prints the Header Entries (Dicom Elements)
  *          both from the H Table and the chained list
  * @return
  */ 
void gdcmParser::PrintEntry(std::ostream & os) 
{
   std::ostringstream s;   
	   
   s << "------------ gdcmParser::Print, using listEntries ----------------" << std::endl; 
   for (ListTag::iterator i = listEntries.begin();  
	   i != listEntries.end();
	   ++i)
   {
	   (*i)->SetPrintLevel(printLevel);
	   (*i)->Print(os);   
   } 
   os<<s.str();
}

/**
  * \ingroup gdcmParser
  * \brief   Prints The Dict Entries of THE public Dicom Dictionnry
  * @return
  */  
void gdcmParser::PrintPubDict(std::ostream & os) 
{
   RefPubDict->Print(os);
}

/**
  * \ingroup gdcmParser
  * \brief   Prints The Dict Entries of the current shadow Dicom Dictionnry
  * @return
  */
void gdcmParser::PrintShaDict(std::ostream & os) 
{
   RefShaDict->Print(os);
}

//-----------------------------------------------------------------------------
// Public
/**
 * \ingroup gdcmParser
 * \brief   Get THE public dictionary used
 */
gdcmDict *gdcmParser::GetPubDict(void)
{
   return(RefPubDict);
}

/**
 * \ingroup gdcmParser
 * \brief   Get the current shadow dictionary 
 */
gdcmDict *gdcmParser::GetShaDict(void)
{
   return(RefShaDict);
}

/**
 * \ingroup gdcmParser
 * \brief   Set the shadow dictionary used
 * \param   dict dictionary to use in shadow
 */
bool gdcmParser::SetShaDict(gdcmDict *dict)
{
   RefShaDict=dict;
   return(!RefShaDict);
}

/**
 * \ingroup gdcmParser
 * \brief   Set the shadow dictionary used
 * \param   dictName name of the dictionary to use in shadow
 */
bool gdcmParser::SetShaDict(DictKey dictName) {
   RefShaDict=gdcmGlobal::GetDicts()->GetDict(dictName);
   return(!RefShaDict);
}

/**
 * \ingroup gdcmParser
 * \brief  This predicate, based on hopefully reasonable heuristics,
 *         decides whether or not the current gdcmParser was properly parsed
 *         and contains the mandatory information for being considered as
 *         a well formed and usable Dicom/Acr File.
 * @return true when gdcmParser is the one of a reasonable Dicom/Acr file,
 *         false otherwise. 
 */
bool gdcmParser::IsReadable(void) 
{
   std::string res = GetEntryByNumber(0x0028, 0x0005);
   if ( res != GDCM_UNFOUND && atoi(res.c_str()) > 4 ) {
      //std::cout << "error on : 28 5" << std::endl;
      return false; // Image Dimensions
   }

   if ( !GetHeaderEntryByNumber(0x0028, 0x0100) ) {
      //std::cout << "error on : 28 100" << std::endl;
      return false; // "Bits Allocated"
   }
   if ( !GetHeaderEntryByNumber(0x0028, 0x0101) ){ 
        // std::cout << "error on : 28 101" << std::endl;
      return false; // "Bits Stored"
   }
   if ( !GetHeaderEntryByNumber(0x0028, 0x0102) ) {
         //std::cout << "error on : 28 102" << std::endl;
      return false; // "High Bit"
   }
   if ( !GetHeaderEntryByNumber(0x0028, 0x0103) ) {
         //std::cout << "error on : 28 103" << std::endl;
      return false; // "Pixel Representation"
   }
   return true;
}

/**
 * \ingroup gdcmParser
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ImplicitVRLittleEndian one.
 *
 * @return  True when ImplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmParser::IsImplicitVRLittleEndianTransferSyntax(void) {
   gdcmHeaderEntry *Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2" )
      return true;
   return false;
}

/**
 * \ingroup gdcmParser
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a ExplicitVRLittleEndian one.
 *
 * @return  True when ExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmParser::IsExplicitVRLittleEndianTransferSyntax(void) 
{
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1" )
      return true;
   return false;
}

/**
 * \ingroup gdcmParser
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a DeflatedExplicitVRLittleEndian one.
 *
 * @return  True when DeflatedExplicitVRLittleEndian found. False in all other cases.
 */
bool gdcmParser::IsDeflatedExplicitVRLittleEndianTransferSyntax(void) 
{
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.1.99" )
      return true;
   return false;
}

/**
 * \ingroup gdcmParser
 * \brief   Determines if the Transfer Syntax was already encountered
 *          and if it corresponds to a Explicit VR Big Endian one.
 *
 * @return  True when big endian found. False in all other cases.
 */
bool gdcmParser::IsExplicitVRBigEndianTransferSyntax(void) 
{
   gdcmHeaderEntry* Element = GetHeaderEntryByNumber(0x0002, 0x0010);
   if ( !Element )
      return false;
   LoadHeaderEntrySafe(Element);

   std::string Transfer = Element->GetValue();
   if ( Transfer == "1.2.840.10008.1.2.2" )  //1.2.2 ??? A verifier !
      return true;
   return false;
}

/**
 * \ingroup gdcmParser
 * \brief  returns the File Type 
 *         (ACR, ACR_LIBIDO, ExplicitVR, ImplicitVR, Unknown)
 * @return 
 */
FileType gdcmParser::GetFileType(void) {
   return(filetype);
}

/**
 * \ingroup gdcmParser
 * \brief   opens the file
 * @param   exception_on_error
 * @return  
 */
FILE *gdcmParser::OpenFile(bool exception_on_error)
  throw(gdcmFileError) 
{
  fp=fopen(filename.c_str(),"rb");
  if(exception_on_error) 
  {
    if(!fp)
      throw gdcmFileError("gdcmParser::gdcmParser(const char *, bool)");
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
    dbg.Verbose(0, "gdcmParser::gdcmParser not DICOM/ACR", filename.c_str());
  }
  else {
    dbg.Verbose(0, "gdcmParser::gdcmParser cannot open file", filename.c_str());
  }
  return(NULL);
}

/**
 * \ingroup gdcmParser
 * \brief closes the file  
 * @return  TRUE if the close was successfull 
 */
bool gdcmParser::CloseFile(void) 
{
  int closed = fclose(fp);
  fp = (FILE *)0;
  if (! closed)
     return false;
  return true;
}

/**
 * \ingroup gdcmParser
 * \brief 
 * @param fp file pointer on an already open file
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @return  always "True" ?!
 */
bool gdcmParser::Write(FILE *fp, FileType type) 
{
// ==============
// TODO The stuff has been rewritten using the chained list instead 
//      of the H table
//      so we could remove the GroupHT from the gdcmParser
// To be checked
// =============

   // TODO : move the following lines (and a lot of others, to be written)
   // to a future function CheckAndCorrectHeader
   
   	// Question :
	// Comment pourrait-on savoir si le DcmHeader vient d'un fichier DicomV3 ou non
	// (FileType est un champ de gdcmParser ...)
	// WARNING : Si on veut ecrire du DICOM V3 a partir d'un DcmHeader ACR-NEMA
	// no way 
        // a moins de se livrer a un tres complique ajout des champs manquants.
        // faire un CheckAndCorrectHeader (?)  
	 

   if (type == ImplicitVR) 
   {
      std::string implicitVRTransfertSyntax = "1.2.840.10008.1.2";
      ReplaceOrCreateByNumber(implicitVRTransfertSyntax,0x0002, 0x0010);
      
      //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
      //      values with a VR of UI shall be padded with a single trailing null
      //      Dans le cas suivant on doit pader manuellement avec un 0
      
      SetEntryLengthByNumber(18, 0x0002, 0x0010);
   } 

   if (type == ExplicitVR) 
   {
      std::string explicitVRTransfertSyntax = "1.2.840.10008.1.2.1";
      ReplaceOrCreateByNumber(explicitVRTransfertSyntax,0x0002, 0x0010);
      
      //FIXME Refer to standards on page 21, chapter 6.2 "Value representation":
      //      values with a VR of UI shall be padded with a single trailing null
      //      Dans le cas suivant on doit pader manuellement avec un 0
      
      SetEntryLengthByNumber(20, 0x0002, 0x0010);
   }


   if ( (type == ImplicitVR) || (type == ExplicitVR) )
      UpdateGroupLength(false,type);
   if ( type == ACR)
      UpdateGroupLength(true,ACR);

   WriteEntries(type, fp);
   return(true);
 }

/**
 * \ingroup gdcmParser
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          if it exists; Creates it with the given value if it doesn't
 * @param   Value passed as a std::string
 * @param   Group
 * @param   Elem
 * \return  boolean
 */
bool gdcmParser::ReplaceOrCreateByNumber(std::string Value, 
                                        guint16 Group, guint16 Elem ) 
{
   if (CheckIfEntryExistByNumber(Group, Elem) == 0) {
      gdcmHeaderEntry *a =NewHeaderEntryByNumber(Group, Elem);
      if (a == NULL) 
         return false;
      AddHeaderEntry(a);
   }   
   SetEntryByNumber(Value, Group, Elem);
   return(true);
}   

/**
 * \ingroup gdcmParser
 * \brief   Modifies the value of a given Header Entry (Dicom Element)
 *          if it exists; Creates it with the given value if it doesn't
 * @param   Value passed as a char*
 * @param   Group
 * @param   Elem
 * \return  boolean 
 * 
 */
bool gdcmParser::ReplaceOrCreateByNumber(char* Value, guint16 Group, guint16 Elem ) 
{
   gdcmHeaderEntry* nvHeaderEntry=NewHeaderEntryByNumber(Group, Elem);

   if(!nvHeaderEntry)
      return(false);

   AddHeaderEntry(nvHeaderEntry);

   std::string v = Value;	
   SetEntryByNumber(v, Group, Elem);
   return(true);
}  

/**
 * \ingroup gdcmParser
 * \brief   Set a new value if the invoked element exists
 *          Seems to be useless !!!
 * @param   Value
 * @param   Group
 * @param   Elem
 * \return  boolean 
 */
bool gdcmParser::ReplaceIfExistByNumber(char* Value, guint16 Group, guint16 Elem ) 
{
   std::string v = Value;	
   SetEntryByNumber(v, Group, Elem);
   return true;
} 

//-----------------------------------------------------------------------------
// Protected
/**
 * \ingroup gdcmParser
 * \brief   Checks if a given Dicom Element exists
 * \        within the H table
 * @param   group Group   number of the searched Dicom Element 
 * @param   element  Element number of the searched Dicom Element 
 * @return  number of occurences
 */
int gdcmParser::CheckIfEntryExistByNumber(guint16 group, guint16 element ) 
{
	std::string key = gdcmDictEntry::TranslateToKey(group, element );
	return (tagHT.count(key));
}

/**
 * \ingroup gdcmParser
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value of a given tag.
 * @param   tagName name of the searched element.
 * @return  Corresponding element value when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmParser::GetEntryByName(std::string tagName) 
{
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   return(GetEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));  
}

/**
 * \ingroup gdcmParser
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
std::string gdcmParser::GetEntryVRByName(std::string tagName) 
{
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return GDCM_UNFOUND;

   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(dictEntry->GetGroup(),
                                                   dictEntry->GetElement());					
   return elem->GetVR();
}

/**
 * \ingroup gdcmParser
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
 *          the public and private dictionaries 
 *          for the element value representation of a given tag.
 * @param   group Group of the searched tag.
 * @param   element Element of the searched tag.
 * @return  Corresponding element value representation when it exists,
 *          and the string GDCM_UNFOUND ("gdcm::Unfound") otherwise.
 */
std::string gdcmParser::GetEntryByNumber(guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return GDCM_UNFOUND;
   return tagHT.find(key)->second->GetValue();
}

/**
 * \ingroup gdcmParser
 * \brief   Searches within Header Entries (Dicom Elements) parsed with 
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
std::string gdcmParser::GetEntryVRByNumber(guint16 group, guint16 element) 
{
   gdcmHeaderEntry* elem =  GetHeaderEntryByNumber(group, element);
   if ( !elem )
      return GDCM_UNFOUND;
   return elem->GetVR();
}

/**
 * \ingroup gdcmParser
 * \brief   Sets the value (string) of the Header Entry (Dicom Element)
 * @param   content string value of the Dicom Element
 * @param   tagName name of the searched Dicom Element.
 * @return  true when found
 */
bool gdcmParser::SetEntryByName(std::string content,std::string tagName) 
{
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return false;				    

   return(SetEntryByNumber(content,dictEntry->GetGroup(),
                                   dictEntry->GetElement()));
}

/**
 * \ingroup gdcmParser
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          through it's (group, element) and modifies it's content with
 *          the given value.
 * \warning Don't use any longer : use SetPubEntryByNumber
 * @param   content new value to substitute with
 * @param   group   group of the Dicom Element to modify
 * @param   element element of the Dicom Element to modify
 */
bool gdcmParser::SetEntryByNumber(std::string content, 
                                  guint16 group,
                                  guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   int l = content.length();
   if(l%2) // Odd length are padded with a space (020H).
   {  
      l++;
      content = content + '\0';
   }
      
   //tagHT[key]->SetValue(content);   
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
   
   //std::string vr = tagHT[key]->GetVR();
   std::string vr = a->GetVR();
   
   guint32 lgr;
   if( (vr == "US") || (vr == "SS") ) 
      lgr = 2;
   else if( (vr == "UL") || (vr == "SL") )
      lgr = 4;
   else
      lgr = l;	   

   //tagHT[key]->SetLength(lgr);
   a->SetLength(lgr);   
   return true;
}					  

/**
 * \ingroup gdcmParser
 * \brief   Accesses an existing gdcmHeaderEntry (i.e. a Dicom Element)
 *          in the PubHeaderEntrySet of this instance
 *          through it's (group, element) and modifies it's length with
 *          the given value.
 * \warning Use with extreme caution.
 * @param   length new length to substitute with
 * @param   group   group of the entry to modify
 * @param   element element of the Entry to modify
 * @return  1 on success, 0 otherwise.
 */

bool gdcmParser::SetEntryLengthByNumber(guint32 length, 
                                        guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   if (length%2) length++; // length must be even
   //tagHT[key]->SetLength(length);
   ( ((tagHT.equal_range(key)).first)->second )->SetLength(length);	 
	 
   return true ;		
}

/**
 * \ingroup gdcmParser
 * \brief   Gets (from Header) the offset  of a 'non string' element value 
 * \        (LoadElementValues has already be executed)
 * @param   Group
 * @param   Elem
 * @return File Offset of the Element Value 
 */
size_t gdcmParser::GetEntryOffsetByNumber(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);	 
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmParser::GetHeaderEntryByNumber",
                      "failed to Locate gdcmHeaderEntry");
      return (size_t)0;
   }
   return Entry->GetOffset();
}

/**
 * \ingroup gdcmParser
 * \brief   Gets (from Header) a 'non string' element value 
 * \        (LoadElementValues has already be executed)  
 * @param   Group
 * @param   Elem
 * @return Pointer to the 'non string' area
 */
void * gdcmParser::GetEntryVoidAreaByNumber(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry* Entry = GetHeaderEntryByNumber(Group, Elem);	 
   if (!Entry) 
   {
      dbg.Verbose(1, "gdcmParser::GetHeaderEntryByNumber",
                  "failed to Locate gdcmHeaderEntry");
      return (NULL);
   }
   return Entry->GetVoidArea();
}

/**
 * \ingroup       gdcmParser
 * \brief         Loads (from disk) the element content 
 *                when a string is not suitable
 */
void *gdcmParser::LoadEntryVoidArea(guint16 Group, guint16 Elem) 
{
   gdcmHeaderEntry * Element= GetHeaderEntryByNumber(Group, Elem);
   if ( !Element )
      return NULL;
   size_t o =(size_t)Element->GetOffset();
   fseek(fp, o, SEEK_SET);
   int l=Element->GetLength();
   void * a = malloc(l);
   if(!a) 
   	return NULL;

   SetEntryVoidAreaByNumber(a, Group, Elem);
   // TODO check the result 
   size_t l2 = fread(a, 1, l ,fp);
   if(l != l2) 
   {
   	free(a);
   	return NULL;
   }
   return a;  
}

/**
 * \ingroup gdcmParser
 * \brief   Sets a 'non string' value to a given Dicom Element
 * @param   area
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
bool gdcmParser::SetEntryVoidAreaByNumber(void * area,guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return false;
   //tagHT[key]->SetVoidArea(area);
   ( ((tagHT.equal_range(key)).first)->second )->SetVoidArea(area);	 
   return true;
}

/**
 * \ingroup gdcmParser
 * \brief   Update the entries with the shadow dictionary. Only odd entries are
 *          analized
 */
void gdcmParser::UpdateShaEntries(void)
{
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
      if(entry)
      {
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
 * \ingroup gdcmParser
 * \brief   Searches within the Header Entries for a Dicom Element of
 *          a given tag.
 * @param   tagName name of the searched Dicom Element.
 * @return  Corresponding Dicom Element when it exists, and NULL
 *          otherwise.
 */
 gdcmHeaderEntry *gdcmParser::GetHeaderEntryByName(std::string tagName) 
 {
   gdcmDictEntry *dictEntry = RefPubDict->GetDictEntryByName(tagName); 
   if( dictEntry == NULL)
      return NULL;

  return(GetHeaderEntryByNumber(dictEntry->GetGroup(),dictEntry->GetElement()));
}

/**
 * \ingroup gdcmParser
 * \brief  retrieves a Dicom Element (the first one) using (group, element)
 * \ warning (group, element) IS NOT an identifier inside the Dicom Header
 *           if you think it's NOT UNIQUE, check the count number
 *           and use iterators to retrieve ALL the Dicoms Elements within
 *           a given couple (group, element)
 * @param   group Group number of the searched Dicom Element 
 * @param   element Element number of the searched Dicom Element 
 * @return  
 */
gdcmHeaderEntry* gdcmParser::GetHeaderEntryByNumber(guint16 group, guint16 element) 
{
   TagKey key = gdcmDictEntry::TranslateToKey(group, element);
   if ( ! tagHT.count(key))
      return NULL;
   return tagHT.find(key)->second;
}

/**
 * \ingroup       gdcmParser
 * \brief         Loads the element while preserving the current
 *                underlying file position indicator as opposed to
 *                to LoadHeaderEntry that modifies it.
 * @param entry   Header Entry whose value shall be loaded. 
 * @return  
 */
void gdcmParser::LoadHeaderEntrySafe(gdcmHeaderEntry * entry) 
{
   long PositionOnEntry = ftell(fp);
   LoadHeaderEntry(entry);
   fseek(fp, PositionOnEntry, SEEK_SET);
}

/**
 * \ingroup gdcmParser
 * \brief   Re-computes the length of a ACR-NEMA/Dicom group from a DcmHeader
 * \warning : to be re-written using the chained list instead of the H table.
 * \warning : DO NOT use (doesn't work any longer because of the multimap)
 * \todo : to be re-written using the chained list instead of the H table
 * @param   SkipSequence TRUE if we don't want to write Sequences (ACR-NEMA Files)
 * @param   type Type of the File (ExplicitVR,ImplicitVR, ACR, ...) 
 */
void gdcmParser::UpdateGroupLength(bool SkipSequence, FileType type) 
{
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
      key = trash;		// generate 'group tag'
      
      // if the caller decided not to take SEQUENCEs into account 
      // e.g : he wants to write an ACR-NEMA File 
                
      if (SkipSequence && vr == "SQ") 
         continue;
      
      // Still unsolved problem :
      // we cannot find the 'Sequence Delimitation Item'
      // since it's at the end of the Hash Table
      // (fffe,e0dd) 
       
      // pas SEQUENCE en ACR-NEMA
      // WARNING : 
      // --> la descente a l'interieur' des SQ 
      // devrait etre faite avec une liste chainee, pas avec une HTable...
            
      if ( groupHt.count(key) == 0) // we just read the first elem of a given group
      { 
         if (el == 0x0000) // the first elem is 0x0000
         {	      
            groupHt[key] = 0;	      // initialize group length 
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
      sscanf(g->first.c_str(),"%x",&gr_bid);
      tk = g->first + "|0000";			// generate the element full tag
                     
      if ( tagHT.count(tk) == 0) // if element 0x0000 not found
      { 		
         gdcmDictEntry * tagZ = new gdcmDictEntry(gr_bid, 0x0000, "UL");       
         elemZ = new gdcmHeaderEntry(tagZ);
         elemZ->SetLength(4);
         AddHeaderEntry(elemZ);				// create it
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
 * \ingroup gdcmParser
 * \brief   writes on disc according to the requested format
 * \        (ACR-NEMA, ExplicitVR, ImplicitVR) the image
 * \ warning does NOT add the missing elements in the header :
 * \         it's up to the user doing it !
 * \         (function CheckHeaderCoherence to be written)
 * @param   type type of the File to be written 
 *          (ACR-NEMA, ExplicitVR, ImplicitVR)
 * @param   _fp already open file pointer
 */
void gdcmParser::WriteEntries(FileType type, FILE * _fp) 
{
   guint16 gr, el;
   guint32 lgr;
   const char * val;
   std::string vr;
   guint32 val_uint32;
   guint16 val_uint16;
   
   std::vector<std::string> tokens;
   
   //  uses now listEntries to iterate, not TagHt!
   //
   //        pb : gdcmParser.Add does NOT update listEntries
   //       TODO : find a trick (in STL?) to do it, at low cost !

   void *ptr;

   // TODO (?) tester les echecs en ecriture (apres chaque fwrite)

   for (ListTag::iterator tag2=listEntries.begin();
        tag2 != listEntries.end();
        ++tag2)
   {
      gr =  (*tag2)->GetGroup();
      el =  (*tag2)->GetElement();
      lgr = (*tag2)->GetLength();
      val = (*tag2)->GetValue().c_str();
      vr =  (*tag2)->GetVR();
      
      if ( type == ACR ) 
      { 
         if (gr < 0x0008)   continue; // ignore pure DICOM V3 groups
         if (gr %2)         continue; // ignore shadow groups
         if (vr == "SQ" )   continue; // ignore Sequences
	           // TODO : find a trick to *skip* the SeQuences !
		   // Not only ignore the SQ element
         if (gr == 0xfffe ) continue; // ignore delimiters
      } 

      fwrite ( &gr,(size_t)2 ,(size_t)1 ,_fp);  //group
      fwrite ( &el,(size_t)2 ,(size_t)1 ,_fp);  //element

      if ( (type == ExplicitVR) && (gr <= 0x0002) ) 
      {
         // EXPLICIT VR
         guint16 z=0, shortLgr;
         fwrite (vr.c_str(),(size_t)2 ,(size_t)1 ,_fp);

         if ( (vr == "OB") || (vr == "OW") || (vr == "SQ") ) 
         {
            fwrite ( &z,  (size_t)2 ,(size_t)1 ,_fp);
            fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);

         } 
         else 
         {
            shortLgr=lgr;
            fwrite ( &shortLgr,(size_t)2 ,(size_t)1 ,_fp);
         }
      } 
      else // IMPLICIT VR
      { 
         fwrite ( &lgr,(size_t)4 ,(size_t)1 ,_fp);
      }

      if (vr == "US" || vr == "SS") 
      {
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize ((*tag2)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) 
         {
            val_uint16 = atoi(tokens[i].c_str());
            ptr = &val_uint16;
            fwrite ( ptr,(size_t)2 ,(size_t)1 ,_fp);
         }
         tokens.clear();
         continue;
      }
      if (vr == "UL" || vr == "SL") 
      {
         tokens.erase(tokens.begin(),tokens.end()); // clean any previous value
         Tokenize ((*tag2)->GetValue(), tokens, "\\");
         for (unsigned int i=0; i<tokens.size();i++) 
         {
            val_uint32 = atoi(tokens[i].c_str());
            ptr = &val_uint32;
            fwrite ( ptr,(size_t)4 ,(size_t)1 ,_fp);
         }
         tokens.clear();
         continue;
      }     
      // Pixels are never loaded in the element !
      if ((gr == 0x7fe0) && (el == 0x0010) ) 
         break;

      fwrite ( val,(size_t)lgr ,(size_t)1 ,_fp); // Elem value
   }
}

/**
 * \ingroup gdcmParser
 * \brief   Swaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 * @return  The properly swaped 32 bits integer.
 */
guint32 gdcmParser::SwapLong(guint32 a) 
{
   switch (sw) 
   {
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
         dbg.Error(" gdcmParser::SwapLong : unset swap code");
         a=0;
   }
   return(a);
}

/**
 * \ingroup gdcmParser
 * \brief   Unswaps back the bytes of 4-byte long integer accordingly to
 *          processor order.
 * @return  The properly unswaped 32 bits integer.
 */
guint32 gdcmParser::UnswapLong(guint32 a) 
{
   return (SwapLong(a));
}

/**
 * \ingroup gdcmParser
 * \brief   Swaps the bytes so they agree with the processor order
 * @return  The properly swaped 16 bits integer.
 */
guint16 gdcmParser::SwapShort(guint16 a) 
{
   if ( (sw==4321)  || (sw==2143) )
      a =(((a<<8) & 0x0ff00) | ((a>>8)&0x00ff));
   return (a);
}

/**
 * \ingroup gdcmParser
 * \brief   Unswaps the bytes so they agree with the processor order
 * @return  The properly unswaped 16 bits integer.
 */
guint16 gdcmParser::UnswapShort(guint16 a) 
{
   return (SwapShort(a));
}

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmParser
 * \brief   Parses the header of the file but WITHOUT loading element values.
 */
void gdcmParser::Parse(bool exception_on_error) throw(gdcmFormatError) 
{
   gdcmHeaderEntry *newHeaderEntry = (gdcmHeaderEntry *)0;
   
   rewind(fp);
   CheckSwap();
   while ( (newHeaderEntry = ReadNextHeaderEntry()) ) 
   {
      SkipHeaderEntry(newHeaderEntry);
      AddHeaderEntry(newHeaderEntry);
   }
}

/**
 * \ingroup gdcmParser
 * \brief   Loads the element values of all the Header Entries pointed in the
 *          public Chained List.
 */
void gdcmParser::LoadHeaderEntries(void) 
{
   rewind(fp);
   for (ListTag::iterator i = GetListEntry().begin();
      i != GetListEntry().end();
      ++i)
   {
      LoadHeaderEntry(*i);
   }
            
   rewind(fp);

   // Load 'non string' values   
   std::string PhotometricInterpretation = GetEntryByNumber(0x0028,0x0004);   
   if( PhotometricInterpretation == "PALETTE COLOR " )
   {
      LoadEntryVoidArea(0x0028,0x1200);  // gray LUT   
      LoadEntryVoidArea(0x0028,0x1201);  // R    LUT
      LoadEntryVoidArea(0x0028,0x1202);  // G    LUT
      LoadEntryVoidArea(0x0028,0x1203);  // B    LUT
      
      LoadEntryVoidArea(0x0028,0x1221);  // Segmented Red   Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1222);  // Segmented Green Palette Color LUT Data
      LoadEntryVoidArea(0x0028,0x1223);  // Segmented Blue  Palette Color LUT Data
   }

   // --------------------------------------------------------------
   // Special Patch to allow gdcm to read ACR-LibIDO formated images
   //
   // if recognition code tells us we deal with a LibIDO image
   // we switch lineNumber and columnNumber
   //
   std::string RecCode;	
   RecCode = GetEntryByNumber(0x0008, 0x0010); // recognition code
   if (RecCode == "ACRNEMA_LIBIDO_1.1" ||
       RecCode == "CANRME_AILIBOD1_1." ) 
   {
         filetype = ACR_LIBIDO; 
         std::string rows    = GetEntryByNumber(0x0028, 0x0010);
         std::string columns = GetEntryByNumber(0x0028, 0x0011);
         SetEntryByNumber(columns, 0x0028, 0x0010);
         SetEntryByNumber(rows   , 0x0028, 0x0011);
   }
   // ----------------- End of Special Patch ----------------
}

/**
 * \ingroup       gdcmParser
 * \brief         Loads the element content if it's length is not bigger
 *                than the value specified with
 *                gdcmParser::SetMaxSizeLoadEntry()
 * @param         Entry Header Entry (Dicom Element) to be dealt with
 */
void gdcmParser::LoadHeaderEntry(gdcmHeaderEntry *Entry) 
{
   size_t item_read;
   guint16 group  = Entry->GetGroup();
   std::string  vr= Entry->GetVR();
   guint32 length = Entry->GetLength();
   bool SkipLoad  = false;

   fseek(fp, (long)Entry->GetOffset(), SEEK_SET);
   
   // the test was commented out to 'go inside' the SeQuences
   // we don't any longer skip them !
    
   // if( vr == "SQ" )  //  (DO NOT remove this comment)
   //    SkipLoad = true;

   // A SeQuence "contains" a set of Elements.  
   //          (fffe e000) tells us an Element is beginning
   //          (fffe e00d) tells us an Element just ended
   //          (fffe e0dd) tells us the current SeQuence just ended
   if( group == 0xfffe )
      SkipLoad = true;

   if ( SkipLoad ) 
   {
      Entry->SetLength(0);
      Entry->SetValue("gdcm::Skipped");
      return;
   }

   // When the length is zero things are easy:
   if ( length == 0 ) 
   {
      Entry->SetValue("");
      return;
   }

   // The elements whose length is bigger than the specified upper bound
   // are not loaded. Instead we leave a short notice of the offset of
   // the element content and it's length.
   if (length > MaxSizeLoadEntry) 
   {
      std::ostringstream s;
      s << "gdcm::NotLoaded.";
      s << " Address:" << (long)Entry->GetOffset();
      s << " Length:"  << Entry->GetLength();
      s << " x(" << std::hex << Entry->GetLength() << ")";
      Entry->SetValue(s.str());
      return;
   }
   
   // When integer(s) are expected, read and convert the following 
   // n *(two or four bytes)
   // properly i.e. as integers as opposed to a strings.	
   // Elements with Value Multiplicity > 1
   // contain a set of integers (not a single one) 
    	
   // Any compacter code suggested (?)
   if ( IsHeaderEntryAnInteger(Entry) ) 
   {
      guint32 NewInt;
      std::ostringstream s;
      int nbInt;
      if (vr == "US" || vr == "SS")
      {
         nbInt = length / 2;
         NewInt = ReadInt16();
         s << NewInt;
         if (nbInt > 1) 
         {
            for (int i=1; i < nbInt; i++) 
            {
               s << '\\';
               NewInt = ReadInt16();
               s << NewInt;
            }
         }
			
      }
      else if (vr == "UL" || vr == "SL") 
      {
         nbInt = length / 4;
         NewInt = ReadInt32();
         s << NewInt;
         if (nbInt > 1) 
         {
            for (int i=1; i < nbInt; i++) 
            {
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
   if ( item_read != 1 ) 
   {
      dbg.Verbose(1, "gdcmParser::LoadElementValue","unread element value");
      Entry->SetValue("gdcm::UnRead");
      return;
   }
   Entry->SetValue(NewValue);
}

/**
 * \ingroup gdcmParser
 * \brief   add a new Dicom Element pointer to 
 *          the H Table and to the chained List
 * \warning push_bash in listEntries ONLY during ParseHeader
 * \todo    something to allow further Elements addition,
 * \        when position to be taken care of     
 * @param   newHeaderEntry
 */
void gdcmParser::AddHeaderEntry(gdcmHeaderEntry *newHeaderEntry) 
{
   tagHT.insert( PairHT( newHeaderEntry->GetKey(),newHeaderEntry) );
   listEntries.push_back(newHeaderEntry); 
   wasUpdated = 1;
}

/**
 * \ingroup gdcmParser
 * \brief   
 * @param   Entry Header Entry whose length of the value shall be loaded. 

 * @return 
 */
 void gdcmParser::FindHeaderEntryLength (gdcmHeaderEntry *Entry) 
 {
   guint16 element = Entry->GetElement();
   guint16 group   = Entry->GetGroup();
   std::string  vr = Entry->GetVR();
   guint16 length16;
   if( (element == 0x0010) && (group == 0x7fe0) ) 
   {
      dbg.SetDebug(-1);
      dbg.Verbose(2, "gdcmParser::FindLength: ",
                     "we reached 7fe0 0010");
   }   
   
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
            dbg.Verbose(0, "gdcmParser::FindLength", "not explicit VR");
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
         //dbg.Verbose(0, "gdcmParser::FindLength",
         //            "Erroneous element length fixed.");
         // Actually, length= 0xffff means that we deal with
         // Unknown Sequence Length 
      }

      FixHeaderEntryFoundLength(Entry, (guint32)length16);
      return;
   }
   else
   {
      // Either implicit VR or a non DICOM conformal (see not below) explicit
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
 * \ingroup   gdcmParser
 * \brief     Find the Value Representation of the current Dicom Element.
 * @param     Entry
 */
void gdcmParser::FindHeaderEntryVR( gdcmHeaderEntry *Entry) 
{
   if (filetype != ExplicitVR)
      return;

   char VR[3];
   int lgrLue;

   long PositionOnEntry = ftell(fp);
   // Warning: we believe this is explicit VR (Value Representation) because
   // we used a heuristic that found "UL" in the first tag. Alas this
   // doesn't guarantee that all the tags will be in explicit VR. In some
   // cases (see e-film filtered files) one finds implicit VR tags mixed
   // within an explicit VR file. Hence we make sure the present tag
   // is in explicit VR and try to fix things if it happens not to be
   // the case.
   
   lgrLue=fread (&VR, (size_t)2,(size_t)1, fp);
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
 * \ingroup   gdcmParser
 * \brief     Check the correspondance between the VR of the header entry
 *            and the taken VR. If they are different, the header entry is 
 *            updated with the new VR.
 * @param     Entry
 * @param     VR
 * @return    false if the VR is incorrect of if the VR isn't referenced
 *            otherwise, it returns true
*/
bool gdcmParser::CheckHeaderEntryVR(gdcmHeaderEntry *Entry, VRKey vr)
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
      dbg.Verbose(1, "gdcmParser::FindVR: ",msg);

      return(false);
   }

   if ( Entry->IsVRUnknown() ) 
   {
      // When not a dictionary entry, we can safely overwrite the VR.
      Entry->SetVR(vr);
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
 * \ingroup gdcmParser
 * \brief   Get the transformed value of the header entry. The VR value 
 *          is used to define the transformation to operate on the value
 * \warning NOT end user intended method !
 * @param   Entry 
 * @return  Transformed entry value
 */
std::string gdcmParser::GetHeaderEntryValue(gdcmHeaderEntry *Entry)
{
   if ( (IsHeaderEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string val=Entry->GetValue();
      std::string vr=Entry->GetVR();
      guint32 length = Entry->GetLength();
      std::ostringstream s;
      int nbInt;

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

      else if (vr == "UL" || vr == "SL")
      {
         guint32 NewInt32;

         nbInt = length / 4;
         for (int i=0; i < nbInt; i++) 
         {
            if(i!=0)
               s << '\\';
            NewInt32=(val[4*i+0]&0xFF)+((val[4*i+1]&0xFF)<<8)+((val[4*i+2]&0xFF)<<16)+((val[4*i+3]&0xFF)<<24);
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
 * \ingroup gdcmParser
 * \brief   Get the reverse transformed value of the header entry. The VR 
 *          value is used to define the reverse transformation to operate on
 *          the value
 * \warning NOT end user intended method !
 * @param   Entry 
 * @return  Reverse transformed entry value
 */
std::string gdcmParser::GetHeaderEntryUnvalue(gdcmHeaderEntry *Entry)
{
   if ( (IsHeaderEntryAnInteger(Entry)) && (Entry->IsImplicitVR()) )
   {
      std::string vr=Entry->GetVR();
      std::ostringstream s;
      std::vector<std::string> tokens;
      unsigned char *ptr;

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
 * \ingroup gdcmParser
 * \brief   Skip a given Header Entry 
 * \warning NOT end user intended method !
 * @param   entry 
 */
void gdcmParser::SkipHeaderEntry(gdcmHeaderEntry *entry) 
{
    SkipBytes(entry->GetLength());
}

/**
 * \ingroup gdcmParser
 * \brief   When the length of an element value is obviously wrong (because
 *          the parser went Jabberwocky) one can hope improving things by
 *          applying this heuristic.
 */
void gdcmParser::FixHeaderEntryFoundLength(gdcmHeaderEntry *Entry, guint32 FoundLength) 
{
   Entry->SetReadLength(FoundLength); // will be updated only if a bug is found
		     
   if ( FoundLength == 0xffffffff) 
   {
      FoundLength = 0;
   }
      
   // Sorry for the patch!  
   // XMedCom did the trick to read some nasty GE images ...
   else if (FoundLength == 13) 
   {
      // The following 'if' will be removed when there is no more
      // images on Creatis HDs with a 13 length for Manufacturer...
      if ( (Entry->GetGroup() != 0x0008) ||  
           ( (Entry->GetElement() != 0x0070) && (Entry->GetElement() != 0x0080) ) ) {
      // end of remove area
         FoundLength =10;
         Entry->SetReadLength(10); // a bug is to be fixed
      }
   }

   // to fix some garbage 'Leonardo' Siemens images
   // May be commented out to avoid overhead
   else if ( (Entry->GetGroup() == 0x0009) &&
       ( (Entry->GetElement() == 0x1113) || (Entry->GetElement() == 0x1114) ) )
   {
      FoundLength =4;
      Entry->SetReadLength(4); // a bug is to be fixed 
   } 
   // end of fix
	 
   // to try to 'go inside' SeQuences (with length), and not to skip them        
   else if ( Entry->GetVR() == "SQ") 
   { 
      if (enableSequences)    // only if the user does want to !
         FoundLength =0; 	 
   } 
    
   // a SeQuence Element is beginning                                          
   // Let's forget it's length                                                 
   // (we want to 'go inside')  

   // Pb : *normaly*  fffe|e000 is just a marker, its length *should be* zero
   // in gdcm-MR-PHILIPS-16-Multi-Seq.dcm we find lengthes as big as 28800
   // if we set the length to zero IsHeaderEntryAnInteger() breaks...
   // if we don't, we lost 28800 characters from the Header :-(
                                                 
   else if(Entry->GetGroup() == 0xfffe)
   { 
                       // sometimes, length seems to be wrong                                      
      FoundLength =0;  // some more clever checking to be done !
                       // I give up!
		       // only  gdcm-MR-PHILIPS-16-Multi-Seq.dcm
		       // causes troubles :-(                                                     
   }     
    
   Entry->SetUsableLength(FoundLength);
}

/**
 * \ingroup gdcmParser
 * \brief   Apply some heuristics to predict wether the considered 
 *          element value contains/represents an integer or not.
 * @param   Entry The element value on which to apply the predicate.
 * @return  The result of the heuristical predicate.
 */
bool gdcmParser::IsHeaderEntryAnInteger(gdcmHeaderEntry *Entry) 
{
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
         s << "Erroneous Group Length element length  on :" \
           << std::hex << group << " , " << element;
         dbg.Error("gdcmParser::IsAnInteger",
            s.str().c_str());     
      }
   }
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") )
      return true;
   
   return false;
}

/**
 * \ingroup gdcmParser
 * \brief   
 *
 * @return 
 */
 guint32 gdcmParser::FindHeaderEntryLengthOB(void) 
 {
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
     
      if ( g != 0xfffe && g!=0xb00c ) /*for bogus header */ 
      {
         char msg[100]; // for sprintf. Sorry
         sprintf(msg,"wrong group (%04x) for an item sequence (%04x,%04x)\n",g, g,n);
         dbg.Verbose(1, "gdcmParser::FindLengthOB: ",msg); 
         errno = 1;
         return 0;
      }
      if ( n == 0xe0dd || ( g==0xb00c && n==0x0eb6 ) ) /* for bogus header  */ 
         FoundSequenceDelimiter = true;
      else if ( n != 0xe000 )
      {
         char msg[100];  // for sprintf. Sorry
         sprintf(msg,"wrong element (%04x) for an item sequence (%04x,%04x)\n",
                      n, g,n);
         dbg.Verbose(1, "gdcmParser::FindLengthOB: ",msg);
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
 * \ingroup gdcmParser
 * \brief Reads a supposed to be 16 Bits integer
 * \     (swaps it depending on processor endianity) 
 *
 * @return integer acts as a boolean
 */
guint16 gdcmParser::ReadInt16(void) 
{
   guint16 g;
   size_t item_read;
   item_read = fread (&g, (size_t)2,(size_t)1, fp);
   if ( item_read != 1 ) 
   {
      if(ferror(fp)) 
         dbg.Verbose(0, "gdcmParser::ReadInt16", " File Error");
      errno = 1;
      return 0;
   }
   errno = 0;
   g = SwapShort(g);
   return g;
}

/**
 * \ingroup gdcmParser
 * \brief  Reads a supposed to be 32 Bits integer
 * \       (swaps it depending on processor endianity)  
 *
 * @return 
 */
guint32 gdcmParser::ReadInt32(void) 
{
   guint32 g;
   size_t item_read;
   item_read = fread (&g, (size_t)4,(size_t)1, fp);
   if ( item_read != 1 ) 
   { 
     if(ferror(fp)) 
         dbg.Verbose(0, "gdcmParser::ReadInt32", " File Error");   
      errno = 1;
      return 0;
   }
   errno = 0;   
   g = SwapLong(g);
   return g;
}

/**
 * \ingroup gdcmParser
 * \brief   
 *
 * @return 
 */
void gdcmParser::SkipBytes(guint32 NBytes) 
{
   //FIXME don't dump the returned value
   (void)fseek(fp, (long)NBytes, SEEK_CUR);
}

/**
 * \ingroup gdcmParser
 * \brief   
 */
void gdcmParser::Initialise(void) 
{
   RefPubDict = gdcmGlobal::GetDicts()->GetDefaultPubDict();
   RefShaDict = (gdcmDict*)0;
}

/**
 * \ingroup gdcmParser
 * \brief   Discover what the swap code is (among little endian, big endian,
 *          bad little endian, bad big endian).
 *
 */
void gdcmParser::CheckSwap()
{
   // Fourth semantics:
   //
   // ---> Warning : This fourth field is NOT part 
   //                of the 'official' Dicom Dictionnary
   //                and should NOT be used.
   //                (Not defined for all the groups
   //                 may be removed in a future release)
   //
   // CMD      Command        
   // META     Meta Information 
   // DIR      Directory
   // ID
   // PAT      Patient
   // ACQ      Acquisition
   // REL      Related
   // IMG      Image
   // SDY      Study
   // VIS      Visit 
   // WAV      Waveform
   // PRC
   // DEV      Device
   // NMI      Nuclear Medicine
   // MED
   // BFS      Basic Film Session
   // BFB      Basic Film Box
   // BIB      Basic Image Box
   // BAB
   // IOB
   // PJ
   // PRINTER
   // RT       Radio Therapy
   // DVH   
   // SSET
   // RES      Results
   // CRV      Curve
   // OLY      Overlays
   // PXL      Pixels
   // DL       Delimiters
   //

   // The only guaranted way of finding the swap code is to find a
   // group tag since we know it's length has to be of four bytes i.e.
   // 0x00000004. Finding the swap code in then straigthforward. Trouble
   // occurs when we can't find such group...
   guint32  s;
   guint32  x=4;  // x : for ntohs
   bool net2host; // true when HostByteOrder is the same as NetworkByteOrder
    
   int lgrLue;
   char *entCur;
   char deb[HEADER_LENGTH_TO_READ];
    
   // First, compare HostByteOrder and NetworkByteOrder in order to
   // determine if we shall need to swap bytes (i.e. the Endian type).
   if (x==ntohs(x))
      net2host = true;
   else
      net2host = false; 
    //cout << net2host << endl;
         
   // The easiest case is the one of a DICOM header, since it possesses a
   // file preamble where it suffice to look for the string "DICM".
   lgrLue = fread(deb, 1, HEADER_LENGTH_TO_READ, fp);
   
   entCur = deb + 128;
   if(memcmp(entCur, "DICM", (size_t)4) == 0) 
   {
      dbg.Verbose(1, "gdcmParser::CheckSwap:", "looks like DICOM Version3");
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
      // FIXME
      // Use gdcmParser::dicom_vr to test all the possibilities
      // instead of just checking for UL, OB and UI !?
      if(  (memcmp(entCur, "UL", (size_t)2) == 0) ||
      	  (memcmp(entCur, "OB", (size_t)2) == 0) ||
      	  (memcmp(entCur, "UI", (size_t)2) == 0) )   
      {
         filetype = ExplicitVR;
         dbg.Verbose(1, "gdcmParser::CheckSwap:",
                     "explicit Value Representation");
      } 
      else 
      {
         filetype = ImplicitVR;
         dbg.Verbose(1, "gdcmParser::CheckSwap:",
                     "not an explicit Value Representation");
      }
      if (net2host) 
      {
         sw = 4321;
         dbg.Verbose(1, "gdcmParser::CheckSwap:",
                        "HostByteOrder != NetworkByteOrder");
      } 
      else 
      {
         sw = 0;
         dbg.Verbose(1, "gdcmParser::CheckSwap:",
                        "HostByteOrder = NetworkByteOrder");
      }
      
      // Position the file position indicator at first tag (i.e.
      // after the file preamble and the "DICM" string).
      rewind(fp);
      fseek (fp, 132L, SEEK_SET);
      return;
   } // End of DicomV3

   // Alas, this is not a DicomV3 file and whatever happens there is no file
   // preamble. We can reset the file position indicator to where the data
   // is (i.e. the beginning of the file).
   dbg.Verbose(1, "gdcmParser::CheckSwap:", "not a DICOM Version3 file");
   rewind(fp);

   // Our next best chance would be to be considering a 'clean' ACR/NEMA file.
   // By clean we mean that the length of the first tag is written down.
   // If this is the case and since the length of the first group HAS to be
   // four (bytes), then determining the proper swap code is straightforward.

   entCur = deb + 4;
   // We assume the array of char we are considering contains the binary
   // representation of a 32 bits integer. Hence the following dirty
   // trick :
   s = *((guint32 *)(entCur));
   
   switch (s)
   {
      case 0x00040000 :
         sw = 3412;
         filetype = ACR;
         return;
      case 0x04000000 :
         sw = 4321;
         filetype = ACR;
         return;
      case 0x00000400 :
         sw = 2143;
         filetype = ACR;
         return;
      case 0x00000004 :
         sw = 0;
         filetype = ACR;
         return;
      default :
         dbg.Verbose(0, "gdcmParser::CheckSwap:",
                        "ACR/NEMA unfound swap info (time to raise bets)");
   }

   // We are out of luck. It is not a DicomV3 nor a 'clean' ACR/NEMA file.
   // It is time for despaired wild guesses. So, let's assume this file
   // happens to be 'dirty' ACR/NEMA, i.e. the length of the group is
   // not present. Then the only info we have is the net2host one.
   filetype = Unknown;
   if (! net2host )
      sw = 0;
   else
      sw = 4321;
   return;
}

/**
 * \ingroup gdcmParser
 * \brief   
 */
void gdcmParser::SwitchSwapToBigEndian(void) 
{
   dbg.Verbose(1, "gdcmParser::SwitchSwapToBigEndian",
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
 * \ingroup gdcmParser
 * \brief   
 * @param NewSize
 * @return 
 */
void gdcmParser::SetMaxSizeLoadEntry(long NewSize) 
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
 * \ingroup gdcmParser
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given name.
 *          The public dictionary has precedence on the shadow one.
 * @param   Name name of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmParser::GetDictEntryByName(std::string Name) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   if (!RefPubDict && !RefShaDict) 
   {
      dbg.Verbose(0, "gdcmParser::GetDictEntry",
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
 * \ingroup gdcmParser
 * \brief   Searches both the public and the shadow dictionary (when they
 *          exist) for the presence of the DictEntry with given
 *          group and element. The public dictionary has precedence on the
 *          shadow one.
 * @param   group   group of the searched DictEntry
 * @param   element element of the searched DictEntry
 * @return  Corresponding DictEntry when it exists, NULL otherwise.
 */
gdcmDictEntry *gdcmParser::GetDictEntryByNumber(guint16 group,guint16 element) 
{
   gdcmDictEntry *found = (gdcmDictEntry *)0;
   if (!RefPubDict && !RefShaDict) 
   {
      dbg.Verbose(0, "gdcmParser::GetDictEntry",
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
 * \ingroup gdcmParser
 * \brief   Read the next tag but WITHOUT loading it's value
 * @return  On succes the newly created HeaderEntry, NULL on failure.      
 */
gdcmHeaderEntry *gdcmParser::ReadNextHeaderEntry(void) 
{
   guint16 g,n;
   gdcmHeaderEntry *NewEntry;
   
   g = ReadInt16();
   n = ReadInt16();
      
   if (errno == 1)
      // We reached the EOF (or an error occured) and header parsing
      // has to be considered as finished.
      return (gdcmHeaderEntry *)0;
   
   NewEntry = NewHeaderEntryByNumber(g, n);
   FindHeaderEntryVR(NewEntry);
   FindHeaderEntryLength(NewEntry);
	
   if (errno == 1) 
   {
      // Call it quits
      return NULL;
   }
   NewEntry->SetOffset(ftell(fp));  
   //if ( (g==0x7fe0) && (n==0x0010) ) 
   return NewEntry;
}

/**
 * \ingroup gdcmParser
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Name    Name of the underlying DictEntry
 */
gdcmHeaderEntry *gdcmParser::NewHeaderEntryByName(std::string Name) 
{
   gdcmDictEntry *NewTag = GetDictEntryByName(Name);
   if (!NewTag)
      NewTag = NewVirtualDictEntry(0xffff, 0xffff, "LO", "Unknown", Name);

   gdcmHeaderEntry* NewEntry = new gdcmHeaderEntry(NewTag);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmParser::ObtainHeaderEntryByName",
                  "failed to allocate gdcmHeaderEntry");
      return (gdcmHeaderEntry *)0;
   }
   return NewEntry;
}  

/**
 * \ingroup gdcmParser
 * \brief   Request a new virtual dict entry to the dict set
 * @param   group   group   of the underlying DictEntry
 * @param   element element of the underlying DictEntry
 * @param   vr      VR of the underlying DictEntry
 * @param   fourth  owner group
 * @param   name    english name
 */
gdcmDictEntry *gdcmParser::NewVirtualDictEntry(guint16 group, guint16 element,
                                               std::string vr,
                                               std::string fourth,
                                               std::string name)
{
   return gdcmGlobal::GetDicts()->NewVirtualDictEntry(group,element,vr,fourth,name);
}

/**
 * \ingroup gdcmParser
 * \brief   Build a new Element Value from all the low level arguments. 
 *          Check for existence of dictionary entry, and build
 *          a default one when absent.
 * @param   Group group   of the underlying DictEntry
 * @param   Elem  element of the underlying DictEntry
 */
gdcmHeaderEntry *gdcmParser::NewHeaderEntryByNumber(guint16 Group, guint16 Elem) 
{
   // Find out if the tag we encountered is in the dictionaries:
   gdcmDictEntry *DictEntry = GetDictEntryByNumber(Group, Elem);
   if (!DictEntry)
      DictEntry = NewVirtualDictEntry(Group, Elem);

   gdcmHeaderEntry *NewEntry = new gdcmHeaderEntry(DictEntry);
   if (!NewEntry) 
   {
      dbg.Verbose(1, "gdcmParser::NewHeaderEntryByNumber",
                  "failed to allocate gdcmHeaderEntry");
      return NULL;
   }
   return NewEntry;
}

/**
 * \ingroup gdcmParser
 * \brief   Small utility function that creates a new manually crafted
 *          (as opposed as read from the file) gdcmHeaderEntry with user
 *          specified name and adds it to the public tag hash table.
 * \note    A fake TagKey is generated so the PubDict can keep it's coherence.
 * @param   NewTagName The name to be given to this new tag.
 * @param   VR The Value Representation to be given to this new tag.
 * @return  The newly hand crafted Element Value.
 */
gdcmHeaderEntry *gdcmParser::NewManualHeaderEntryToPubDict(std::string NewTagName, 
                                                           std::string VR) 
{
   gdcmHeaderEntry *NewEntry = NULL;
   guint32 StuffGroup = 0xffff;   // Group to be stuffed with additional info
   guint32 FreeElem = 0;
   gdcmDictEntry *DictEntry = NULL;

   FreeElem = GenerateFreeTagKeyInGroup(StuffGroup);
   if (FreeElem == UINT32_MAX) 
   {
      dbg.Verbose(1, "gdcmHeader::NewManualHeaderEntryToPubDict",
                     "Group 0xffff in Public Dict is full");
      return NULL;
   }

   DictEntry = NewVirtualDictEntry(StuffGroup, FreeElem,
                                VR, "GDCM", NewTagName);
   NewEntry = new gdcmHeaderEntry(DictEntry);
   AddHeaderEntry(NewEntry);
   return NewEntry;
}

/**
 * \ingroup gdcmParser
 * \brief   Generate a free TagKey i.e. a TagKey that is not present
 *          in the TagHt dictionary.
 * @param   group The generated tag must belong to this group.  
 * @return  The element of tag with given group which is fee.
 */
guint32 gdcmParser::GenerateFreeTagKeyInGroup(guint16 group) 
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
