/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.h,v $
  Language:  C++
  Date:      $Date: 2004/06/22 13:47:33 $
  Version:   $Revision: 1.12 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDOCENTRYSET_H
#define GDCMDOCENTRYSET_H

#include "gdcmException.h"
#include "gdcmDocEntry.h"
 
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmDocEntrySet
{

public:

   gdcmDocEntrySet(int depth = 0); 
   virtual ~gdcmDocEntrySet();

   /// \brief adds any type of entry to the entry set (pure vitual)
   virtual bool AddEntry(gdcmDocEntry *Entry) = 0; // pure virtual
	 
   /// \brief prints any type of entry to the entry set (pure vitual)
   virtual void Print (std::ostream & os = std::cout) = 0;// pure virtual

   /// \brief write any type of entry to the entry set
   virtual void Write (FILE *fp, FileType filetype)=0;// pure virtual

   /// \brief Gets the depth level of a Dicom Header Entry embedded in a
   ///        SeQuence
   inline int GetDepthLevel(void) { return SQDepthLevel; }

   /// \brief Sets the depth level of a Dicom Header Entry embedded in a
   /// SeQuence
   inline void SetDepthLevel(int depth) { SQDepthLevel = depth; }

	virtual gdcmDocEntry *GetDocEntryByNumber(guint16 group,guint16 element) = 0;
   gdcmDocEntry *GetDocEntryByName(std::string name);
   virtual std::string GetEntryByNumber(guint16 group,guint16 element) = 0;
   std::string GetEntryByName(TagName name);		         
   gdcmDictEntry *NewVirtualDictEntry(guint16 group, 
                                      guint16 element,
                                      std::string vr     = "unkn",
                                      std::string fourth = "unkn",
                                      std::string name   = "unkn");
													  
 protected:

  // DocEntry  related utilities 
  
	gdcmValEntry *NewValEntryByNumber(guint16 group, 
                                     guint16 element); 												 
   gdcmBinEntry *NewBinEntryByNumber(guint16 group, 
                                     guint16 element); 	
   gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                     guint16 element); 
   gdcmDocEntry *NewDocEntryByName  (std::string Name);
	
  // DictEntry  related utilities
   
   gdcmDictEntry *GetDictEntryByName  (std::string Name);
   gdcmDictEntry *GetDictEntryByNumber(guint16, guint16);
		  	
   /// Gives the depth level of the element set inside SeQuences   
   int SQDepthLevel;

private:
    
};


//-----------------------------------------------------------------------------
#endif

