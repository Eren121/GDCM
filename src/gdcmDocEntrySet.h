/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDocEntrySet.h,v $
  Language:  C++
  Date:      $Date: 2004/06/21 04:43:02 $
  Version:   $Revision: 1.11 $
                                                                                
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

   /// \brief Gets the depth level of a Dicom Header Entry embedded in a
   ///        SeQuence
   inline int GetDepthLevel(void) { return SQDepthLevel; }

   /// \brief Sets the depth level of a Dicom Header Entry embedded in a
   /// SeQuence
   inline void SetDepthLevel(int depth) { SQDepthLevel = depth; }
         
protected:
   /// \brief   Build a new Element Value from all the low level arguments. 
   ///         Check for existence of dictionary entry, and build
   ///          a default one when absent (pure virtual)
   virtual gdcmDocEntry *NewDocEntryByNumber(guint16 group, 
                                             guint16 element)=0; // pure virtual

   /// \brief   Build a new Element Value from all the low level arguments. 
   ///         Check for existence of dictionary entry, and build
   ///          a default one when absent (pure virtual)
   virtual gdcmDocEntry *NewDocEntryByName  (std::string Name)=0;// pure virtual  
   /// Gives the depth level of the element set inside SeQuences   
   int SQDepthLevel;

private:
    
};


//-----------------------------------------------------------------------------
#endif

