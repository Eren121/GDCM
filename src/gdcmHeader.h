// gdcmHeader.h
//-----------------------------------------------------------------------------
#ifndef GDCMHEADER_H
#define GDCMHEADER_H

#include "gdcmCommon.h"
#include "gdcmParser.h"

//-----------------------------------------------------------------------------
/*
 * \defgroup gdcmHeader
 * \brief
 * The purpose of an instance of gdcmHeader is to act as a container of
 * all the DICOM elements and their corresponding values (and
 * additionaly the corresponding DICOM dictionary entry) of the header
 * of a DICOM file.
 *
 * The typical usage of instances of class gdcmHeader is to classify a set of
 * dicom files according to header information e.g. to create a file hierarchy
 * reflecting the Patient/Study/Serie informations, or extracting a given
 * SerieId. Accessing the content (image[s] or volume[s]) is beyond the
 * functionality of this class and belongs to gdmcFile.
 * \note  The various entries of the explicit value representation (VR) shall
 *        be managed within a dictionary which is shared by all gdcmHeader
 *        instances.
 * \note  The gdcmHeader::Set*Tag* family members cannot be defined as
 *        protected due to Swig limitations for as Has_a dependency between
 *        gdcmFile and gdcmHeader.
 */
class GDCM_EXPORT gdcmHeader : public gdcmParser
{
public:
   gdcmHeader(bool exception_on_error = false);
   gdcmHeader(const char *filename, 
              bool  exception_on_error = false, 
              bool  enable_sequences   = false,
	      bool  skip_shadow        = false);
	      
   virtual ~gdcmHeader();

// Standard values and informations contained in the header
   virtual bool IsReadable(void);
   bool IsJPEGBaseLineProcess1TransferSyntax(void);
   bool IsJPEGExtendedProcess2_4TransferSyntax(void); 
   bool IsJPEGExtendedProcess3_5TransferSyntax(void);
   bool IsJPEGSpectralSelectionProcess6_8TransferSyntax(void); 
   bool IsRLELossLessTransferSyntax(void); 
   bool IsJPEGLossless(void); 
   bool IsJPEG2000(void); 
   bool IsDicomV3(void); 

   // Some heuristic based accessors, end user intended 
   // (to be move to gdcmHeaderHelper?) 
   int GetXSize(void);
   int GetYSize(void);
   int GetZSize(void);
   int GetBitsStored(void);
   int GetBitsAllocated(void);
   int GetSamplesPerPixel(void);   
   int GetPlanarConfiguration(void);

   int GetPixelSize(void);   
   std::string GetPixelType(void);  
   size_t GetPixelOffset(void);
   size_t GetPixelAreaLength(void);

   bool   HasLUT(void);
   int    GetLUTNbits(void);
   unsigned char * GetLUTRGBA(void);

   std::string GetTransfertSyntaxName(void);

   // When some proprietary shadow groups are disclosed, we can set up
   // an additional specific dictionary to access extra information.
   
   // OK : we still have *ONE* HeaderEntrySet, 
   // with both Public and Shadow Elements
   // parsed against THE Public Dictionary and A (single) Shadow Dictionary
   
   // TODO Swig int SetShaDict(std::string filename);
   // TODO Swig int SetPubDict(std::string filename);
   
// System access
/**
 * \ingroup gdcmHeader
 * \brief   the Header Entry Group Number of the 'Pixel Group' 
 *          is not allways 0x7fe0
 * @return  GrPixel
 */
   guint16 GetGrPixel(void)  {return GrPixel;}
/**
 * \ingroup gdcmHeader
 * \brief   the Header Entry Element Number of the 'Pixel Element' 
 *          is not allways 0x0010
 * @return  NumPixel
 */
    guint16 GetNumPixel(void) {return NumPixel;}   

// Entry

   inline virtual std::string GetEntryByNumber  (guint16 group, guint16 element)
      { return(gdcmParser::GetEntryByNumber(group,element)); }
      
   inline virtual std::string GetEntryVRByNumber(guint16 group, guint16 element)
      { return(gdcmParser::GetEntryVRByNumber(group,element)); }
      
   inline virtual int GetEntryLengthByNumber(guint16 group, guint16 element)
      { return(gdcmParser::GetEntryLengthByNumber(group,element)); }
      
   inline virtual std::string GetEntryByName    (std::string tagName) 
      { return(gdcmParser::GetEntryByName(tagName)); }
      
   inline virtual std::string GetEntryVRByName  (std::string tagName)
      { return(gdcmParser::GetEntryVRByName(tagName)); }
      
   inline virtual bool SetEntryByNumber(std::string content,guint16 group, guint16 element)
      { return(gdcmParser::SetEntryByNumber(content,group,element)); }
      
   inline virtual bool SetEntryByName(std::string content,std::string tagName)
      { return(gdcmParser::SetEntryByName(content,tagName)); }

  inline virtual bool SetEntryLengthByNumber(guint32 l,guint16 group, guint16 element)
      { return(gdcmParser::SetEntryLengthByNumber(l,group,element)); }

   inline virtual void UpdateShaEntries(void)
      { gdcmParser::UpdateShaEntries(); }

// Read (used in gdcmFile)
   void SetImageDataSize(size_t ExpectedSize);

   bool operator<(gdcmHeader &header);

protected:
   int write(std::ostream&);   
   int anonymize(std::ostream&);  // FIXME : anonymize should be a friend ?
   bool anonymizeHeader(void);
private:

};

//-----------------------------------------------------------------------------
#endif
