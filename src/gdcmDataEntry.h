/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDataEntry.h,v $
  Language:  C++
  Date:      $Date: 2006/03/01 09:29:29 $
  Version:   $Revision: 1.13 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDATAENTRY_H
#define GDCMDATAENTRY_H

#include "gdcmDocEntry.h"

#include <vector> // for GetDSValue
#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief   Any Dicom Document (File or DicomDir, or ...) contains 
 *           a set of DocEntry  - Dicom entries -
 *           (when successfuly parsed against a given Dicom dictionary)
 *          DataEntry is an elementary DocEntry (as opposed to SeqEntry).
 *          Depending on the type of its content,
 */
class GDCM_EXPORT DataEntry  : public DocEntry
{
   gdcmTypeMacro(DataEntry);

public:
/// \brief Contructs a DataEntry with a RefCounter from DictEntry
   static DataEntry *New(DictEntry *e) {return new DataEntry(e);}
/// \brief Contructs a DataEntry with a RefCounter from DocEntry
   static DataEntry *New(DocEntry *d)  {return new DataEntry(d);}

// Print
   void Print(std::ostream &os = std::cout, std::string const &indent = "");

// Write
   virtual void WriteContent(std::ofstream *fp, FileType filetype);
   uint32_t ComputeFullLength();
   
// Set/Get data
   // Sets the value (string) of the current Dicom entry
   //virtual void SetValue(std::string const &val);

   /// \brief Returns the area value of the current Dicom Entry
   ///  when it's not string-translatable (e.g : LUT table, overlay, icon)   
   uint8_t *GetBinArea()  { return BinArea; }
   void SetBinArea( uint8_t *area, bool self = true );
   void CopyBinArea( uint8_t *area, uint32_t length );

   void SetValue(const uint32_t &id,const double &val);
   double GetValue(const uint32_t &id) const;
   uint32_t GetValueCount() const;
   bool IsValueCountValid() const;

   void SetString(std::string const &value);
   std::string const &GetString() const;

   /// \brief Sets SelfArea
   void SetSelfArea(bool area) { SelfArea = area; }
   /// \brief True if Entry owns its BinArea
   bool IsSelfArea() { return SelfArea; }

   ///\brief values for current state of a DataEntry (internal use only)
   enum TValueState
   {
      STATE_LOADED    = 0x00,
      STATE_NOTLOADED = 0x01,
      STATE_UNFOUND   = 0x02,
      STATE_UNREAD    = 0x03
   };
   
   ///\brief values for current pixel status of a DataEntry (internal use only)
   enum TValueFlag
   {
      FLAG_NONE       = 0x00,
      FLAG_PIXELDATA  = 0x01
   };

   // State
   /// \brief Sets the state (Loaded, NotLoaded, UnFound, ...) of the DataEntry
   void SetState(const TValueState &state) { State = state; }
   /// \brief Returns the state (Loaded, NotLoaded, ...) of the DataEntry
      const TValueState &GetState() const { return State; }
   /// \brief true when value Entry not loaded  
   bool IsNotLoaded() { return State == STATE_NOTLOADED; }
   /// \brief true if Entry not found  
   bool IsUnfound()   { return State == STATE_UNFOUND; }
   /// \brief true if Entry not read    
   bool IsUnread()    { return State == STATE_UNREAD; }
   /// \brief true if Entry value properly loaded
   bool IsGoodValue() { return State == STATE_LOADED; }

   // Flags
   /// \brief sets the 'pixel data flag'   
   void SetFlag(const TValueFlag &flag) { Flag = flag; }
   /// \brief returns the 'pixel data flag'    
   const TValueFlag &GetFlag() const { return Flag; }
   /// \brief true id Entry is a Pixel Data entry
   bool IsPixelData() { return (Flag &FLAG_PIXELDATA) != 0; }

   virtual void Copy(DocEntry *doc);

   /// \brief returns the size threshold above which an element value 
   ///        will NOT be *printed* in order no to polute the screen output
   static const uint32_t &GetMaxSizePrintEntry() { return MaxSizePrintEntry; }
   /// \brief Header Elements too long will not be printed
   static void SetMaxSizePrintEntry(const uint32_t &size) 
                                                 { MaxSizePrintEntry = size; }

   bool GetDSValue(std::vector <double> &valueVector);

protected:
   DataEntry(DictEntry *e);
   DataEntry(DocEntry *d); 
   ~DataEntry();

// Methods :
   void NewBinArea( );
   void DeleteBinArea( );

// Members :
   /// \brief memory area to hold 'non std::string' representable values 
   ///       (ie : Lookup Tables, overlays, icons)   
   uint8_t *BinArea;
   /// \brief Whether DataEntry has its own BinArea or not
   bool SelfArea;
   /// \brief  std::string representable value of the Entry. 
   ///        Parts of a multivaluated data are separated by back-slash
   mutable std::string StrArea;

private:
   /// \brief 0 for straight entries, FLAG_PIXELDATA for Pixel Data entries
   TValueFlag Flag;
   /// \brief Entry status:STATE_NOTLOADED,STATE_UNFOUND,STATE_UNREAD,STATE_LOADED
   TValueState State;

   /// \brief Size threshold above which an element is printed
   static uint32_t MaxSizePrintEntry;   
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif

