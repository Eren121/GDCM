// gdcmDicomDirElement.h
//-----------------------------------------------------------------------------
#ifndef GDCMDICOMDIRELEMENT_H
#define GDCMDICOMDIRELEMENT_H

#include "gdcmCommon.h"

#include <list>

//-----------------------------------------------------------------------------
typedef struct
{
	unsigned short int group;
	unsigned short int elem;
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

   void Print(std::ostream &os);

   ListDicomDirMetaElem    &GetDicomDirMetaElements(void)    
      {return(DicomDirMetaList);};
   ListDicomDirPatientElem &GetDicomDirPatientElements(void) 
      {return(DicomDirPatientList);};
   ListDicomDirStudyElem   &GetDicomDirStudyElements(void)   
      {return(DicomDirStudyList);};
   ListDicomDirSerieElem   &GetDicomDirSerieElements(void)   
      {return(DicomDirSerieList);};
   ListDicomDirImageElem   &GetDicomDirImageElements(void)   
      {return(DicomDirImageList);};

private:
   ListDicomDirMetaElem    DicomDirMetaList;
   ListDicomDirPatientElem DicomDirPatientList;
   ListDicomDirStudyElem   DicomDirStudyList;
   ListDicomDirSerieElem   DicomDirSerieList;
   ListDicomDirImageElem   DicomDirImageList;
};

//-----------------------------------------------------------------------------
#endif
