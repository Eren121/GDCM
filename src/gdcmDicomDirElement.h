/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDicomDirElement.h,v $
  Language:  C++
  Date:      $Date: 2004/10/09 03:21:55 $
  Version:   $Revision: 1.11 $
                                                                                
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

//-----------------------------------------------------------------------------
typedef struct
{
   /// DicomGroup number
   unsigned short int Group;
   /// DicomElement number
   unsigned short int Elem;
   /// value (coded as a std::string) of the Element
   std::string Value;
} gdcmElement;

typedef std::list<gdcmElement> ListDicomDirMetaElem;
typedef std::list<gdcmElement> ListDicomDirPatientElem;
typedef std::list<gdcmElement> ListDicomDirStudyElem;
typedef std::list<gdcmElement> ListDicomDirSerieElem;
typedef std::list<gdcmElement> ListDicomDirImageElem;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmDicomDirElement
 * \brief    gdcmDicomDirElement represents elements contained in a dicom dir
 *           Class for the chained lists from the file 'Dicts/DicomDir.dic'
 */
class GDCM_EXPORT gdcmDicomDirElement
{
public:
   gdcmDicomDirElement();
   ~gdcmDicomDirElement();

  /**
    * \brief   canonical Printer 
    * \sa    SetPrintLevel
    */ 
   void Print(std::ostream &os);

   /**
    * \ingroup gdcmDicomDirElement
    * \brief   returns a reference to the chained List 
    *          related to the META Elements of a DICOMDIR.
    */
   ListDicomDirMetaElem    &GetDicomDirMetaElements()
      { return DicomDirMetaList; };

   /**
    * \ingroup gdcmDicomDirElement
    * \brief   returns a reference to the chained List 
    *          related to the PATIENT Elements of a DICOMDIR.
    */      
   ListDicomDirPatientElem &GetDicomDirPatientElements()
      { return DicomDirPatientList; };

   /**
    * \ingroup gdcmDicomDirElement
    * \brief   returns a reference to the chained List 
    *          related to the STUDY Elements of a DICOMDIR.
    */      
   ListDicomDirStudyElem   &GetDicomDirStudyElements()
      { return DicomDirStudyList; };

   /**
    * \ingroup gdcmDicomDirElement
    * \brief   returns a reference to the chained List 
    *          related to the SERIE Elements of a DICOMDIR.
    */
   ListDicomDirSerieElem   &GetDicomDirSerieElements()
      { return DicomDirSerieList; };

   /**
    * \ingroup gdcmDicomDirElement
    * \brief   returns a reference to the chained List 
    *          related to the IMAGE Elements of a DICOMDIR.
    */
   ListDicomDirImageElem   &GetDicomDirImageElements()
      { return DicomDirImageList; };

private:
   /// gdcmElements chained list, related to the MetaElements of DICOMDIR
   ListDicomDirMetaElem    DicomDirMetaList;
   /// gdcmElements chained list, related to the PatientElements of DICOMDIR
   ListDicomDirPatientElem DicomDirPatientList;
   /// gdcmElements chained list, related to the StudyElements of DICOMDIR
   ListDicomDirStudyElem   DicomDirStudyList;
   /// gdcmElements chained list, related to the SerieElements of DICOMDIR
   ListDicomDirSerieElem   DicomDirSerieList;
   /// gdcmElements chained list, related to the ImageElements of DICOMDIR
   
   ListDicomDirImageElem   DicomDirImageList;
};

//-----------------------------------------------------------------------------
#endif
