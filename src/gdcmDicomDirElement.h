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

typedef std::list<gdcmElement> ListMetaElem;
typedef std::list<gdcmElement> ListPatientElem;
typedef std::list<gdcmElement> ListStudyElem;
typedef std::list<gdcmElement> ListSerieElem;
typedef std::list<gdcmElement> ListImageElem;

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

   ListMetaElem    &GetMetaElements(void)    {return(MetaList);};
   ListPatientElem &GetPatientElements(void) {return(PatientList);};
   ListStudyElem   &GetStudyElements(void)   {return(StudyList);};
   ListSerieElem   &GetSerieElements(void)   {return(SerieList);};
   ListImageElem   &GetImageElements(void)   {return(ImageList);};

private:
   ListMetaElem    MetaList;
   ListPatientElem PatientList;
   ListStudyElem   StudyList;
   ListSerieElem   SerieList;
   ListImageElem   ImageList;
};

//-----------------------------------------------------------------------------
#endif
