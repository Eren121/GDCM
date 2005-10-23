/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDataEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/23 15:32:30 $
  Version:   $Revision: 1.10 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDataEntry.h"
#include "gdcmVR.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmDebug.h"

#include <fstream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
#define MAX_SIZE_PRINT_ELEMENT_VALUE 0x7fffffff
uint32_t DataEntry::MaxSizePrintEntry = MAX_SIZE_PRINT_ELEMENT_VALUE;

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor for a given DictEntry
 * @param   e Pointer to existing dictionary entry
 */
DataEntry::DataEntry(DictEntry *e) 
            : DocEntry(e)
{
   State = STATE_LOADED;
   Flag = FLAG_NONE;

   BinArea = 0;
   SelfArea = true;
}

/**
 * \brief   Constructor for a given DocEntry
 * @param   e Pointer to existing Doc entry
 */
DataEntry::DataEntry(DocEntry *e)
            : DocEntry(e->GetDictEntry())
{
   Flag = 0;
   BinArea = 0;
   SelfArea = true;

   Copy(e);
}

/**
 * \brief   Canonical destructor.
 */
DataEntry::~DataEntry ()
{
   DeleteBinArea();
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \brief Sets the value (non string) of the current Dicom Header Entry
 */
void DataEntry::SetBinArea( uint8_t *area, bool self )  
{ 
   DeleteBinArea();

   BinArea = area;
   SelfArea = self;

   State = STATE_LOADED;
}
/**
 * \brief Inserts the value (non string) into the current Dicom Header Entry
 */
void DataEntry::CopyBinArea( uint8_t *area, uint32_t length )
{
   DeleteBinArea();

   uint32_t lgh = length + length%2;
   SetLength(lgh);

   if( area && length > 0 )
   {
      NewBinArea();
      memcpy(BinArea,area,length);
      if( length!=lgh )
         BinArea[length]=0;

      State = STATE_LOADED;
   }
}

void DataEntry::SetValue(const uint32_t &id, const double &val)
{
   if( !BinArea )
      NewBinArea();
   State = STATE_LOADED;

   if( id > GetValueCount() )
   {
      gdcmErrorMacro("Index (" << id << ")is greater than the data size");
      return;
   }

   const VRKey &vr = GetVR();
   if( vr == "US" || vr == "SS" )
   {
      uint16_t *data = (uint16_t *)BinArea;
      data[id] = (uint16_t)val;
   }
   else if( vr == "UL" || vr == "SL" )
   {
      uint32_t *data = (uint32_t *)BinArea;
      data[id] = (uint32_t)val;
   }
   else if( vr == "FL" )
   {
      float *data = (float *)BinArea;
      data[id] = (float)val;
   }
   else if( vr == "FD" )
   {
      double *data = (double *)BinArea;
      data[id] = (double)val;
   }
   else if( Global::GetVR()->IsVROfStringRepresentable(vr) )
   {
      gdcmErrorMacro("SetValue on String representable not implemented yet");
   }
   else
   {
      BinArea[id] = (uint8_t)val;
   }
}
/**
 * \brief returns, as a double (?!?) one of the values 
 //      (when entry is multivaluated), identified by its index.
 //      Returns 0.0 if index is wrong
 //     FIXME : warn the user there was a problem ! 
 */
double DataEntry::GetValue(const uint32_t &id) const
{
   if( !BinArea )
   {
      gdcmErrorMacro("BinArea not set. Can't get the value");
      return 0.0;
   }

   uint32_t count = GetValueCount();
   if( id > count )
   {
      gdcmErrorMacro("Index (" << id << ")is greater than the data size");
      return 0.0;
   }

   // FIX the API : user *knows* that entry contains a US
   //               and he receives a double ?!?
   
   const VRKey &vr = GetVR();
   if( vr == "US" || vr == "SS" )
      return ((uint16_t *)BinArea)[id];
   else if( vr == "UL" || vr == "SL" )
      return ((uint32_t *)BinArea)[id];
   else if( vr == "FL" )
      return ((float *)BinArea)[id];
   else if( vr == "FD" )
      return ((double *)BinArea)[id];
   else if( Global::GetVR()->IsVROfStringRepresentable(vr) )
   {
      if( GetLength() )
      {
         // Don't use std::string to accelerate processing
         double val = 0.0;
         char *tmp = new char[GetLength()+1];
         memcpy(tmp,BinArea,GetLength());
         tmp[GetLength()]=0;

         if( count == 0 )
         {
            val = atof(tmp);
         }
         else
         {
            count = id;
            char *beg = tmp;
            for(uint32_t i=0;i<GetLength();i++)
            {
               if( tmp[i] == '\\' )
               {
                  if( count == 0 )
                  {
                     tmp[i] = 0;
                     break;
                  }
                  else
                  {
                     count--;
                     beg = &(tmp[i+1]);
                  }
               }
            }
            val = atof(beg);
         }

         delete[] tmp;
         return val;
      }
      else 
         return 0.0;
   }
   else
      return BinArea[id];
}

/**
 * \brief Checks if the multiplicity of the value follows Dictionary VM
 */
bool DataEntry::IsValueCountValid() const
{
  bool valid = false;
  uint32_t vm;
  const std::string &strVM = GetVM();
  uint32_t vc = GetValueCount();
  
  // FIXME : what shall we do with VM = "2-n", "3-n", etc
  
  if( strVM == "1-n" )
  {
    // make sure it is at least one ??? FIXME
    valid = vc >= 1 || vc == 0;
  }
  else
  {
    std::istringstream os;
    os.str( strVM );
    os >> vm;
    // Two cases:
    // vm respect the one from the dict
    // vm is 0 (we need to check if this element is allowed to be empty) FIXME

    // note  (JPR)
    // ----    
    // Entries whose type is 1 are mandatory, with a mandatory value.
    // Entries whose type is 1c are mandatory-inside-a-Sequence,
    //                          with a mandatory value.
    // Entries whose type is 2 are mandatory, with an optional value.
    // Entries whose type is 2c are mandatory-inside-a-Sequence,
    //                          with an optional value.
    // Entries whose type is 3 are optional.

    // case vc == 0 is only applicable for 'type 2' entries.
    // Problem : entry type may depend on the modality and/or the Sequence
    //           it's embedded in !
    //          (Get the information in the 'Conformance Statements' ...)  
    valid = vc == vm || vc == 0;
  }
  return valid;
}

/**
 * \brief returns the number of elementary values
 */ 
uint32_t DataEntry::GetValueCount( ) const
{
   const VRKey &vr = GetVR();
   if( vr == "US" || vr == "SS" )
      return GetLength()/sizeof(uint16_t);
   else if( vr == "UL" || vr == "SL" )
      return GetLength()/sizeof(uint32_t);
   else if( vr == "FL" )
      return GetLength()/sizeof(float);
   else if( vr == "FD" )
      return GetLength()/sizeof(double);
   else if( Global::GetVR()->IsVROfStringRepresentable(vr) )
   {
      // Some element in DICOM are allowed to be empty
      if( !GetLength() ) return 0;
      // Don't use std::string to accelerate processing
      uint32_t count = 1;
      for(uint32_t i=0;i<GetLength();i++)
      {
         if( BinArea[i] == '\\')
            count++;
      }
      return count;
   }

   return GetLength();
}
/**
 * \brief Sets the 'value' of an Entry, passed as a std::string
 * @param value string representation of the value to be set
 */ 
void DataEntry::SetString(std::string const &value)
{
   DeleteBinArea();

   const VRKey &vr = GetVR();
   if ( vr == "US" || vr == "SS" )
   {
      std::vector<std::string> tokens;
      Util::Tokenize (value, tokens, "\\");
      SetLength(tokens.size()*sizeof(uint16_t));
      NewBinArea();

      uint16_t *data = (uint16_t *)BinArea;
      for (unsigned int i=0; i<tokens.size();i++)
         data[i] = atoi(tokens[i].c_str());
      tokens.clear();
   }
   else if ( vr == "UL" || vr == "SL" )
   {
      std::vector<std::string> tokens;
      Util::Tokenize (value, tokens, "\\");
      SetLength(tokens.size()*sizeof(uint32_t));
      NewBinArea();

      uint32_t *data = (uint32_t *)BinArea;
      for (unsigned int i=0; i<tokens.size();i++)
         data[i] = atoi(tokens[i].c_str());
      tokens.clear();
   }
   else if ( vr == "FL" )
   {
      std::vector<std::string> tokens;
      Util::Tokenize (value, tokens, "\\");
      SetLength(tokens.size()*sizeof(float));
      NewBinArea();

      float *data = (float *)BinArea;
      for (unsigned int i=0; i<tokens.size();i++)
         data[i] = (float)atof(tokens[i].c_str());
      tokens.clear();
   }
   else if ( vr == "FD" )
   {
      std::vector<std::string> tokens;
      Util::Tokenize (value, tokens, "\\");
      SetLength(tokens.size()*sizeof(double));
      NewBinArea();

      double *data = (double *)BinArea;
      for (unsigned int i=0; i<tokens.size();i++)
         data[i] = atof(tokens[i].c_str());
      tokens.clear();
   }
   else
   {
      if( value.size() > 0 )
      {
         // FIXME : should be quicker if we don't create one more std::string
         //         just to make even the length of a char array ...

         /*
         std::string finalVal = Util::DicomString( value.c_str() );
         SetLength(finalVal.size());
         NewBinArea();

         memcpy(BinArea, &(finalVal[0]), finalVal.size());
         */

         size_t l =  value.size();    
         SetLength(l + l%2);
         NewBinArea();
         memcpy(BinArea, &(value[0]), l);
         if (l%2)
            BinArea[l] = '\0';
      }
   }
   State = STATE_LOADED;
}
/**
 * \brief   returns as a string (when possible) the value of the DataEntry
 */
std::string const &DataEntry::GetString() const
{
   static std::ostringstream s;
   const VRKey &vr = GetVR();

   s.str("");
   StrArea="";

   if( !BinArea )
      return StrArea;
      
   // When short integer(s) are stored, convert the following (n * 2) characters 
   // as a displayable string, the values being separated by a back-slash

   if( vr == "US" || vr == "SS" )
   {
      uint16_t *data=(uint16_t *)BinArea;

      for (unsigned int i=0; i < GetValueCount(); i++)
      {
         if( i!=0 )
            s << '\\';
         s << data[i];
      }
      StrArea=s.str();
   }
   // See above comment on multiple short integers (mutatis mutandis).
   else if( vr == "UL" || vr == "SL" )
   {
      uint32_t *data=(uint32_t *)BinArea;

      for (unsigned int i=0; i < GetValueCount(); i++)
      {
         if( i!=0 )
            s << '\\';
         s << data[i];
      }
      StrArea=s.str();
   }
   else if( vr == "FL" )
   {
      float *data=(float *)BinArea;

      for (unsigned int i=0; i < GetValueCount(); i++)
      {
         if( i!=0 )
            s << '\\';
         s << data[i];
      }
      StrArea=s.str();
   }
   else if( vr == "FD" )
   {
      double *data=(double *)BinArea;

      for (unsigned int i=0; i < GetValueCount(); i++)
      {
         if( i!=0 )
            s << '\\';
         s << data[i];
      }
      StrArea=s.str();
   }
   else
      StrArea.append((const char *)BinArea,GetLength());

   return StrArea;
}
/**
 * \brief   Copies all the attributes from an other DocEntry 
 * @param doc entry to copy from
 */
void DataEntry::Copy(DocEntry *doc)
{
   DocEntry::Copy(doc);

   DataEntry *entry = dynamic_cast<DataEntry *>(doc);
   if ( entry )
   {
      State = entry->State;
      Flag = entry->Flag;
      CopyBinArea(entry->BinArea,entry->GetLength());
   }
}
/**
 * \brief   Writes the value of a DataEntry
 * @param fp already open ofstream pointer
 * @param filetype type of the file (ACR, ImplicitVR, ExplicitVR, ...)
 */
void DataEntry::WriteContent(std::ofstream *fp, FileType filetype)
{ 
   DocEntry::WriteContent(fp, filetype);

   if ( GetGroup() == 0xfffe )
   {
      return; //delimitors have NO value
   }

   uint8_t *binArea8 = BinArea; //safe notation
   size_t lgr = GetLength();
   if (BinArea) // the binArea was *actually* loaded
   {

   //  The same operation should be done if we wanted 
   //  to write image with Big Endian Transfer Syntax, 
   //  while working on Little Endian Processor
   // --> forget Big Endian Transfer Syntax writting!
   //     Next DICOM version will give it up ...

   // --> FIXME 
   //    The stuff looks nice, but it's probably bugged,
   //    since troubles occur on big endian processors (SunSparc, Motorola)
   //    while reading the pixels of a 
   //    gdcm-written Little-Endian 16 bits per pixel image

#if defined(GDCM_WORDS_BIGENDIAN) || defined(GDCM_FORCE_BIGENDIAN_EMULATION)

      /// \todo FIXME : Right now, we only care of Pixels element
      ///       we should deal with *all* the BinEntries
      ///       Well, not really since we are not interpreting values read...

      // 8 Bits Pixels *are* OB, 16 Bits Pixels *are* OW
      // -value forced while Reading process-
      
      // -->  WARNING
      // -->        the following lines *looked* very clever, 
      // -->        but they don't work on big endian processors.
      // -->        since I've no access for the moment to a big endian proc :-(
      // -->        I comment them out, to see the result on the dash board 
      // -->     
      
      // --> Revert to initial code : TestWriteSimple hangs on Darwin :-(     
      if (GetGroup() == 0x7fe0 && GetVR() == "OW")
      {  
         uint16_t *binArea16 = (uint16_t*)binArea8;
         binary_write (*fp, binArea16, lgr );
      }
      else
      { 
         // For any other VR, DataEntry is re-written as-is
         binary_write (*fp, binArea8, lgr );
      }

      // -->  WARNING      
      // -->         remove the following line, an uncomment the previous ones, 
      // -->         if it doesn't work better
      // -->     
      /*binary_write ( *fp, binArea8, lgr ); // Elem value*/
      
#else
      binary_write ( *fp, binArea8, lgr ); // Elem value
#endif //GDCM_WORDS_BIGENDIAN
   }
   else
   {
      // nothing was loaded, but we need to skip space on disc
      
      //  --> WARNING : nothing is written; 
      //  --> the initial data (on the the source image) is lost
      //  --> user is *not* informed !
      
      fp->seekp(lgr, std::ios::cur);
   }
}

//-----------------------------------------------------------------------------
// Protected
void DataEntry::NewBinArea(void)
{
   DeleteBinArea();
   if( GetLength() > 0 )
      BinArea = new uint8_t[GetLength()];
   SelfArea = true;
}

void DataEntry::DeleteBinArea(void)
{
   if (BinArea && SelfArea)
   {
      delete[] BinArea;
      BinArea = NULL;
   }
}

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
/**
 * \brief   Prints a DataEntry (Dicom entry)
 * @param   os ostream we want to print in
 * @param indent Indentation string to be prepended during printing
 */
void DataEntry::Print(std::ostream &os, std::string const & )
{
   os << "D ";
   DocEntry::Print(os);

   uint16_t g = GetGroup();
   if (g == 0xfffe) // delimiters have NO value
   {          
      return; // just to avoid identing all the remaining code 
   }

   std::ostringstream s;
   TSAtr v;

   if( BinArea )
   {
      v = GetString();
      const VRKey &vr = GetVR();

      if( vr == "US" || vr == "SS" )
         s << " [" << GetString() << "]";
      else if( vr == "UL" || vr == "SL" )
         s << " [" << GetString() << "]";
      else if ( vr == "FL" )
         s << " [" << GetString() << "]";
      else if ( vr == "FD" )
         s << " [" << GetString() << "]";
      else
      { 
         if(Global::GetVR()->IsVROfStringRepresentable(vr))
         {
            // replace non printable characters by '.'
            std::string cleanString = Util::CreateCleanString(v);
            if ( cleanString.length() <= GetMaxSizePrintEntry()
              || PrintLevel >= 3
              || IsNotLoaded() )
           // FIXME : when IsNotLoaded(), you create a Clean String ?!?
           // FIXME : PrintLevel<2 *does* print the values 
           //        (3 is only for extra offsets printing)
           // What do you wanted to do ? JPR
            {
               s << " [" << cleanString << "]";
            }
            else
            {
               s << " [gdcm::too long for print (" << cleanString.length() << ") ]";
            }
         }
         else
         {
            // A lot of Private elements (with no VR) contain actually 
            // only printable characters;
            // Let's deal with them as is they were VR std::string representable
    
            if ( Util::IsCleanArea( GetBinArea(), GetLength()  ) )
            {
               // FIXME : since the 'Area' *is* clean, just use
               //         a 'CreateString' method, to save CPU time.
               std::string cleanString = 
                     Util::CreateCleanString( BinArea,GetLength()  );
               s << " [" << cleanString << "]";
            }
            else
            {
               s << " [" << GDCM_BINLOADED << ";"
               << "length = " << GetLength() << "]";
            }
         }
      }
   }
   else
   {
      if( IsNotLoaded() )
         s << " [" << GDCM_NOTLOADED << "]";
      else if( IsUnfound() )
         s << " [" << GDCM_UNFOUND << "]";
      else if( IsUnread() )
         s << " [" << GDCM_UNREAD << "]";
      else if ( GetLength() == 0 )
         s << " []";
   }

   if( IsPixelData() )
      s << " (" << GDCM_PIXELDATA << ")";

   // Display the UID value (instead of displaying only the rough code)
   // First 'clean' trailing character (space or zero) 
   if(BinArea)
   {
      const uint16_t &gr = GetGroup();
      const uint16_t &elt = GetElement();
      TS *ts = Global::GetTS();

      if (gr == 0x0002)
      {
         // Any more to be displayed ?
         if ( elt == 0x0010 || elt == 0x0002 )
         {
            if ( v.length() != 0 )  // for brain damaged headers
            {
               if ( ! isdigit((unsigned char)v[v.length()-1]) )
               {
                  v.erase(v.length()-1, 1);
               }
            }
            s << "  ==>\t[" << ts->GetValue(v) << "]";
         }
      }
      else if (gr == 0x0008)
      {
         if ( elt == 0x0016 || elt == 0x1150 )
         {
            if ( v.length() != 0 )  // for brain damaged headers
            {
               if ( ! isdigit((unsigned char)v[v.length()-1]) )
               {
                  v.erase(v.length()-1, 1);
               }
            }
            s << "  ==>\t[" << ts->GetValue(v) << "]";
         }
      }
      else if (gr == 0x0004)
      {
         if ( elt == 0x1510 || elt == 0x1512  )
         {
            if ( v.length() != 0 )  // for brain damaged headers  
            {
               if ( ! isdigit((unsigned char)v[v.length()-1]) )
               {
                  v.erase(v.length()-1, 1);  
               }
            }
            s << "  ==>\t[" << ts->GetValue(v) << "]";
         }
      }
   }

   os << s.str();
}

//-----------------------------------------------------------------------------
} // end namespace gdcm

