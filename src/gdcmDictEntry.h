/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDictEntry.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.14 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICTENTRY_H
#define GDCMDICTENTRY_H

#include "gdcmCommon.h"

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDictEntry
 * \brief
 * the gdcmDictEntry in an element contained by the gdcmDict.
 * It contains :
 *  - the key referenced by the DICOM norm or the constructor (for private keys)
 *  - the corresponding name in english (it's equivalent to a label)
 *  - the owner group
 *  - etc.
 */
class GDCM_EXPORT gdcmDictEntry 
{
public:
   gdcmDictEntry(guint16 group, 
                 guint16 element,
                 std::string vr     = "Unknown",
                 std::string fourth = "Unknown",
                 std::string name   = "Unknown");
	
   static TagKey TranslateToKey(guint16 group, guint16 element);

   void SetVR(std::string);

   /**
    * \ingroup     gdcmDictEntry
    * \brief       tells if the V(alue) R(epresentation) is known (?!)
    *              
    * @return 
    */
   inline bool IsVRUnknown() {return vr == "??"; }

   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Group Number of the current gdcmDictEntry
    * return the Dicom Group Number
    */
   inline guint16 GetGroup(void) { return group; }
  
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Element Number of the current gdcmDictEntry
    * return the Dicom Element Number
    */
   inline guint16 GetElement(void) { return element; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Value Representation of the current gdcmDictEntry
    * return the Dicom Value Representation
    */
   inline std::string GetVR(void) { return vr; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   sets the key of the current gdcmDictEntry
    * @param k New key to be set.
    */
   inline void SetKey(std::string k)  { key = k; }
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Fourth field of the current gdcmDictEntry
    * \warning NOT part of the Dicom Standard
    * \        May be REMOVED an any time
    * \        NEVER use it
    * return the Fourth field
    */
   inline std::string GetFourth(void) { return fourth; } 

   /**
    * \ingroup gdcmDictEntry
    * \brief   returns the Dicom Name of the current gdcmDictEntry
    * \        e.g. "Patient Name" for Dicom Tag (0x0010, 0x0010) 
    * return the Dicom Name
    */
   inline std::string GetName(void) { return name; } 
 
   /**
    * \ingroup gdcmDictEntry
    * \brief   Gets the key of the current gdcmDictEntry
    * @return the key .
    */
   inline std::string GetKey(void) { return key; }

private:
   // FIXME : where are the group and element used except from building up
   //         a TagKey. If the answer is nowhere then there is no need
   //         to store the group and element independently.
   //
   //         --> EVERYWHERE ! The alternate question would be :
   //                          What's TagKey used for ?
   
   /// DicomGroup number
   guint16 group;   // e.g. 0x0010
   /// DicomElement number
   guint16 element; // e.g. 0x0103
   /**
    * \ingroup gdcmDictEntry
    * \brief   Value Representation i.e. some clue about the nature
    *          of the data represented 
    *          e.g. "FD" short for "Floating Point Double"
    */ 
   std::string vr;
	                	                
   /**
    * \brief AVOID using the following fourth field at all costs.
    * 
    *  They are at leat two good reasons for NOT using fourth:
    *  - the main reason is that it is NOT part of the 'official'
    *    Dicom Dictionnary.
    *  - a second reason is that it is not defined for all the groups.
    *  .
    *  Still it provides some semantics as group name abbreviation that
    *  can prove of some help when organizing things in an interface.
    *  For the time being we keep it in gdcm but it migth be removed in
    *  future releases it proves to be source of confusion.
    *  Here is a small dictionary we encountered in "nature":
    *  - CMD      Command        
    *  - META     Meta Information 
    *  - DIR      Directory
    *  - ID       ???
    *  - PAT      Patient
    *  - ACQ      Acquisition
    *  - REL      Related
    *  - IMG      Image
    *  - SDY      Study
    *  - VIS      Visit 
    *  - WAV      Waveform
    *  - PRC      ???
    *  - DEV      Device
    *  - NMI      Nuclear Medicine
    *  - MED      ???
    *  - BFS      Basic Film Session
    *  - BFB      Basic Film Box
    *  - BIB      Basic Image Box
    *  - BAB
    *  - IOB
    *  - PJ
    *  - PRINTER
    *  - RT       Radio Therapy
    *  - DVH   
    *  - SSET
    *  - RES      Results
    *  - CRV      Curve
    *  - OLY      Overlays
    *  - PXL      Pixels
    *  - DL       Delimiters
    *  .
    *
    *  Other usefull abreviations used for Radiographic view associated with
    *  Patient Position (0018,5100):
    *  -  AP = Anterior/Posterior 
    *  -  PA = Posterior/Anterior 
    *  -  LL = Left Lateral 
    *  -  RL = Right Lateral 
    *  - RLD = Right Lateral Decubitus 
    *  - LLD = Left  Lateral Decubitus 
    *  - RLO = Right Lateral Oblique 
    *  - LLO = Left  Lateral Oblique  
    *  .
    */
   std::string fourth; 

   /// e.g. "Patient's Name"                    
   std::string name;      

   /// Redundant with (group, element) but we add it on efficiency purposes. 
   TagKey  key;
                     
	// DCMTK has many fields for handling a DictEntry (see below). What are the
	// relevant ones for gdcmlib ?
	//      struct DBI_SimpleEntry {
	//         Uint16 upperGroup;
	//         Uint16 upperElement;
	//         DcmEVR evr;
	//         const char* tagName;
	//         int vmMin;
	//         int vmMax;
	//         const char* standardVersion;
	//         DcmDictRangeRestriction groupRestriction;
	//         DcmDictRangeRestriction elementRestriction;
	//       };
};

//-----------------------------------------------------------------------------
#endif
