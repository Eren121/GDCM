/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDataEntry.h,v $
  Language:  C++
  Date:      $Date: 2005/10/18 08:35:51 $
  Version:   $Revision: 1.1 $
                                                                                
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

#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief   Any Dicom Document (File or DicomDir) contains 
 *           a set of DocEntry  - Dicom entries -
 *           (when successfuly parsed against a given Dicom dictionary)
 *          DataEntry is an elementary DocEntry (as opposed to SeqEntry).
 *          Depending on the type of its content,
 */
class GDCM_EXPORT DataEntry  : public DocEntry
{
public:
   DataEntry(DictEntry *e);
   DataEntry(DocEntry *d); 
   ~DataEntry();

// Print
   void Print(std::ostream &os = std::cout, std::string const &indent = "");

// Write
   virtual void WriteContent(std::ofstream *fp, FileType filetype);

// Set/Get datas
   /// Sets the value (string) of the current Dicom entry
   //virtual void SetValue(std::string const &val);
   /// \brief Returns the 'Value' (e.g. "Dupond^Marcel") converted 
   /// into a 'string', event if it's physically stored on disk as an integer
   /// (e.g. : 0x000c returned as "12")
   //virtual std::string const &GetValue() const { return Value; }

   /// \brief Returns the area value of the current Dicom Entry
   ///  when it's not string-translatable (e.g : LUT table, overlay, icon)         
   uint8_t *GetBinArea()  { return BinArea; }
   void SetBinArea( uint8_t *area, bool self = true );
   void CopyBinArea( uint8_t *area, uint32_t length );

   void SetValue(const uint32_t &id,const double &val);
   double GetValue(const uint32_t &id) const;
   uint32_t GetValueCount(void) const;

   void SetString(std::string const &value);
   std::string const &GetString() const;

   /// \brief Sets SelfArea
   void SetSelfArea(bool area) { SelfArea = area; }
   /// \brief Returns SelfArea
   bool IsSelfArea() { return SelfArea; }

   // State
   void SetState(const char &state) { State = state; }
   const char &GetState() const { return State; }
   bool IsNotLoaded() { return State == STATE_NOTLOADED; }
   bool IsUnfound()   { return State == STATE_UNFOUND; }
   bool IsUnread()    { return State == STATE_UNREAD; }
   bool IsGoodValue() { return State == 0; }

   // Flags
   void SetFlag(const char &flag) { Flag = flag; }
   const char &GetFlag() const { return Flag; }
   bool IsPixelData() { return (Flag & FLAG_PIXELDATA) != 0; }

   void Copy(DocEntry *doc);

   /// \brief returns the size threshold above which an element value 
   ///        will NOT be *printed* in order no to polute the screen output
   static const uint32_t &GetMaxSizePrintEntry() { return MaxSizePrintEntry; }
   /// \brief Header Elements too long will not be printed
   static void SetMaxSizePrintEntry(const uint32_t &size) { MaxSizePrintEntry = size; }

   typedef enum
   {
      STATE_LOADED    = 0x00,
      STATE_NOTLOADED = 0x01,
      STATE_UNFOUND   = 0x02,
      STATE_UNREAD    = 0x03,
   } TValueState;

   typedef enum
   {
      FLAG_NONE       = 0x00,
      FLAG_PIXELDATA  = 0x01,
   } TValueFlag;

protected:
// Methods :
   void NewBinArea(void);
   void DeleteBinArea(void);

// Members :
   /// \brief memory area to hold 'non std::string' representable values 
   ///       (ie : Lookup Tables, overlays, icons)   
   uint8_t *BinArea;
   /// \brief Whether DataEntry has its own BinArea or not
   bool SelfArea;

   mutable std::string StrArea;

private:
   char Flag;
   char State;

   /// \brief Size threshold above which an element val
   ///        By default, this upper bound is fixed to 64 bytes.
   static uint32_t MaxSizePrintEntry;   
};

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif

