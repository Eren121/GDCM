/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirElement.h,v $
  Language:  C++
  Date:      $Date: 2005/02/05 00:22:14 $
  Version:   $Revision: 1.25 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDICOMDIRELEMENT_H
#define GDCMDICOMDIRELEMENT_H

#include "gdcmCommon.h"

#include <list>

namespace gdcm 
{

//-----------------------------------------------------------------------------
/**
 * \brief internal structure, not end user intended
 *        shouln't appear here
 */  
typedef struct
{
   /// DicomGroup number
   unsigned short int Group;
   /// DicomElement number
   unsigned short int Elem;
   /// value (coded as a std::string) of the Element
   std::string Value;
} Element;

typedef std::list<Element> ListDicomDirElem;
typedef std::list<Element> ListDicomDirMetaElem;
typedef std::list<Element> ListDicomDirPatientElem;
typedef std::list<Element> ListDicomDirStudyElem;
typedef std::list<Element> ListDicomDirSerieElem;
typedef std::list<Element> ListDicomDirImageElem;

//-----------------------------------------------------------------------------
/**
 * \brief   Represents elements contained in a DicomDir
 *           class for the chained lists from the file 'Dicts/DicomDir.dic'
 */
class GDCM_EXPORT DicomDirElement
{
public:
   DicomDirElement();
   ~DicomDirElement();

  /**
    * \brief   canonical Printer 
    */ 
   void Print(std::ostream &os);

   /**
    * \brief   returns a reference to the chained List 
    *          related to the META Elements of a DICOMDIR.
    */
   ListDicomDirMetaElem const &GetDicomDirMetaElements() const
      { return DicomDirMetaList; };

   /**
    * \brief   returns a reference to the chained List 
    *          related to the PATIENT Elements of a DICOMDIR.
    */      
   ListDicomDirPatientElem const &GetDicomDirPatientElements() const
      { return DicomDirPatientList; };

   /**
    * \brief   returns a reference to the chained List 
    *          related to the STUDY Elements of a DICOMDIR.
    */      
   ListDicomDirStudyElem const &GetDicomDirStudyElements() const
      { return DicomDirStudyList; };

   /**
    * \brief   returns a reference to the chained List 
    *          related to the SERIE Elements of a DICOMDIR.
    */
   ListDicomDirSerieElem const &GetDicomDirSerieElements() const
      { return DicomDirSerieList; };

   /**
    * \brief   returns a reference to the chained List 
    *          related to the IMAGE Elements of a DICOMDIR.
    */
   ListDicomDirImageElem const &GetDicomDirImageElements() const
      { return DicomDirImageList; };

   // Public method to add an element
   bool AddEntry(DicomDirType type, Element const &elem);

private:
   /// Elements chained list, related to the MetaElements of DICOMDIR
   ListDicomDirMetaElem    DicomDirMetaList;
   /// Elements chained list, related to the PatientElements of DICOMDIR
   ListDicomDirPatientElem DicomDirPatientList;
   /// Elements chained list, related to the StudyElements of DICOMDIR
   ListDicomDirStudyElem   DicomDirStudyList;
   /// Elements chained list, related to the SerieElements of DICOMDIR
   ListDicomDirSerieElem   DicomDirSerieList;
   /// Elements chained list, related to the ImageElements of DICOMDIR
   ListDicomDirImageElem   DicomDirImageList;
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
