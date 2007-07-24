# include stuff
INCLUDE_DIRECTORIES(  
  ${GDCM_SOURCE_DIR}/src  
  ${GDCM_BINARY_DIR}  
  ${GDCM_BINARY_DIR}/src)
SET(EXAMPLE_SOURCES
  #names starting with 'ex' are examples  
  #Txt2Mat  
  #exDicomRTStructSetFile  
  #exExtractCSA
  Dense2007ToDicom  
  exReadPapyrus  
  exReadWriteFile  
  exColorToRGB  
  exGrey2RGB  
  exGC
  exImageLighten  
  #exInLine  
  exOverlaysACR  
  exOverlaysDCM  
  exCurveData  
  exExtractTag  
  exSerieHelper
  exXCoherentFileSet
  exExtractDicomTags  
  exMoveImagesToSingleSerieUID
  #the following are utilities  
  PrintDicomDir  
  PrintFile  
  MakeDicomDir  
  AnonymizeDicomDir     
  # without loading it as a gdcm::DicomDir  
  Anonymize             
  # for full gdcm readable files  
  AnonymizeNoLoad       
  # without loading the Pixel Data  
  AnonymizeMultiPatient 
  # without loading the Pixel Data  
  PatchHeader  
  ToInTag  
  ReWrite  
  #ReWriteExtended  
  RawToDicom  
  TestValidate  
  ToMRIregister
  DenseMultiFramesToDicom
    
  #BatchUncompress  
  ExtractOverlays   
  #the following will be transformed into 'examples', or 'utilities'
  #              or will be removed
  # Better you don't use them (not fully checked ...)   
  FindTags  
  FlatHashTablePrint  
  Volume2Dicom  
  WriteDicomSimple  

  WriteRead 
  ## Troubles on Windows ! 
  #WriteDicomAsJPEG2000
  #WriteDicomAsJPEG

  exCTPET  
  #Slice
)

FOREACH(name ${EXAMPLE_SOURCES})  
  ADD_EXECUTABLE(${name} ${name}.cxx )  
  TARGET_LINK_LIBRARIES(${name} gdcm)  
  INSTALL_TARGETS(/bin/ ${name})
ENDFOREACH(name)
