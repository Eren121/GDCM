// gdcmDicomDirElement.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIRELEMENT_H
#define GDCMDICOMDIRELEMENT_H

#include "gdcmCommon.h"

#include <list>

//-----------------------------------------------------------------------------
typedef struct
{
   /// DicomGroup number
   	unsigned short int group;
   /// DicomElement number
	unsigned short int elem;
   /// value (coded as a std::string) of the Element	
	std::string value;
} gdcmElement;

typedef std::list<gdcmElement> ListDicomDirMetaElem;
typedef std::list<gdcmElement> ListDicomDirPatientElem;
typedef std::list<gdcmElement> ListDicomDirStudyElem;
typedef std::list<gdcmElement> ListDicomDirSerieElem;
typedef std::list<gdcmElement> ListDicomDirImageElem;

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmDicomDirElement
 * \brief    gdcmDicomDirElement represents elements contained in a dicom dir
 *
 */
class GDCM_EXPORT gdcmDicomDirElement
{
public:
   gdcmDicomDirElement(void);
   ~gdcmDicomDirElement(void);

  /**
    * \ingroup gdcmParser
    * \brief   canonical Printer 
    * \sa    SetPrintLevel
  */ 
   void Print(std::ostream &os);
/**
 * \ingroup gdcmDicomDirElement
 * \brief   returns a reference to the chained List 
 *          related to the META Elements of a DICOMDIR.
 */
   ListDicomDirMetaElem    &GetDicomDirMetaElements(void)    
      {return(DicomDirMetaList);};
/**
 * \ingroup gdcmDicomDirElement
 * \brief   returns a reference to the chained List 
 *          related to the PATIENT Elements of a DICOMDIR.
 */      
   ListDicomDirPatientElem &GetDicomDirPatientElements(void) 
      {return(DicomDirPatientList);};
/**
 * \ingroup gdcmDicomDirElement
 * \brief   returns a reference to the chained List 
 *          related to the STUDY Elements of a DICOMDIR.
 */      
   ListDicomDirStudyElem   &GetDicomDirStudyElements(void)   
      {return(DicomDirStudyList);};
/**
 * \ingroup gdcmDicomDirElement
 * \brief   returns a reference to the chained List 
 *          related to the SERIE Elements of a DICOMDIR.
 */
   ListDicomDirSerieElem   &GetDicomDirSerieElements(void)   
      {return(DicomDirSerieList);};
/**
 * \ingroup gdcmDicomDirElement
 * \brief   returns a reference to the chained List 
 *          related to the IMAGE Elements of a DICOMDIR.
 */
   ListDicomDirImageElem   &GetDicomDirImageElements(void)   
      {return(DicomDirImageList);};

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
