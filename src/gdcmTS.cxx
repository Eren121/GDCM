// gdcmTS.cxx

#include <stdio.h>

#include "gdcmTS.h"

gdcmTS::gdcmTS(void) {
   ts["1.2.840.10008.1.2"]      = "Implicit VR - Little Endian";  
   ts["1.2.840.10008.1.2.1"]    = "Explicit VR - Little Endian";         
   ts["1.2.840.10008.1.2.1.99"] = "Deflated Explicit VR - Little Endian";          
   ts["1.2.840.10008.1.2.2"]    = "Explicit VR - Big Endian";                 
   ts["1.2.840.10008.1.2.4.50"] = "JPEG Baseline (Process 1)";       
   ts["1.2.840.10008.1.2.4.51"] = "JPEG Extended (Process 2 & 4)";            
   ts["1.2.840.10008.1.2.4.52"] = "JPEG Extended (Process 3 & 5) (Retired)"; 
   ts["1.2.840.10008.1.2.4.53"] = "JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8) (Retired)"; 
   ts["1.2.840.10008.1.2.4.54"] = "JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9) (Retired)";        
   ts["1.2.840.10008.1.2.4.55"] = "JPEG Spectral Selection, Non-Hierarchical (Process 10 & 12) (Retired)";           
   ts["1.2.840.10008.1.2.4.56"] = "JPEG Spectral Selection, Non-Hierarchical (Process 11 & 13) (Retired)";             
   ts["1.2.840.10008.1.2.4.57"] = "JPEG Baseline (Process 14)";     
   ts["1.2.840.10008.1.2.4.58"] = "JPEG Baseline (Process 15) (Retired)";     
   ts["1.2.840.10008.1.2.4.59"] = "JPEG Spectral Selection, Non-Hierarchical (Process 16 & 18) (Retired)";           
   ts["1.2.840.10008.1.2.4.60"] = "JPEG Spectral Selection, Non-Hierarchical (Process 17 & 19) (Retired)";          
   ts["1.2.840.10008.1.2.4.61"] = "JPEG Spectral Selection, Non-Hierarchical (Process 20 & 22) (Retired)";           
   ts["1.2.840.10008.1.2.4.62"] = "JPEG Spectral Selection, Non-Hierarchical (Process 21 & 23) (Retired)";          
   ts["1.2.840.10008.1.2.4.63"] = "JPEG Spectral Selection, Non-Hierarchical (Process 24 & 26) (Retired)";   
   ts["1.2.840.10008.1.2.4.64"] = "JPEG Spectral Selection, Non-Hierarchical (Process 25 & 27) (Retired)";   
   ts["1.2.840.10008.1.2.4.65"] = "JPEG Lossless, Hierarchical (Process 28) (Retired)";   
   ts["1.2.840.10008.1.2.4.66"] = "JPEG Lossless, Hierarchical (Process 29) (Retired))";   
   ts["1.2.840.10008.1.2.4.70"] = "Non-Hierarchical, First-Order Prediction (Process 14 [Selection Value 1])"; 
   ts["1.2.840.10008.1.2.4.80"] = "JPEG Lossless Image Compression";
   ts["1.2.840.10008.1.2.4.81"] = "JPEG Lossy (Near-Lossless) Image Compression";
   ts["1.2.840.10008.1.2.4.90"] = "JPEG 2000 Image Compression (Lossless Only)";   
   ts["1.2.840.10008.1.2.4.91"] = "JPEG 2000 Image Compression";   
   ts["1.2.840.10008.1.2.5"]    = "RLE Lossless"; 
   
   // The following are NOT 'Transfer Syntax UID'
   // but 'SOP Class UID', 'Meta SOP Class UID'.
   // May be usefull later.
   // See PS 3.6-2003 Annex A p77
   
   ts["1.2.840.10008.1.1"]               = "Verification SOP Class";
   ts["1.2.840.10008.1.3.10"]            = "Media Storage Directory Storage"; 
   ts["1.2.840.10008.1.9"]               = "Basic Study Content Notification SOP Class";   
   ts["1.2.840.10008.1.20.1"]            = "Image Commitment Push Model SOP Class";
   ts["1.2.840.10008.1.20.1.1"]          = "Image Commitment Push Model SOP Instance";
   ts["1.2.840.10008.1.20.2"]            = "Image Commitment Push Model SOP Class (Retired)";        
   ts["1.2.840.10008.1.20.2.1"]          = "Image Commitment Push Model SOP Instance (Retired)";        
   ts["1.2.840.10008.2.16.4"]            = "DICOM Controlled Terminology";
   ts["1.2.840.10008.3.1.1.1"]           = "DICOM Application Context Name";   
   ts["1.2.840.10008.3.1.2.1.1"]         = "Detached Patient Management SOP Class";
   ts["1.2.840.10008.3.1.2.1.4"]         = "Detached Patient Management Meta SOP Class"; 
   ts["1.2.840.10008.3.1.2.2.1"]         = "Detached Visit Management SOP Class";      
   ts["1.2.840.10008.3.1.2.3.1"]         = "Detached Study Management SOP Class";    
   ts["1.2.840.10008.3.1.2.3.2"]         = "Study Component Management SOP Class";
   ts["1.2.840.10008.3.1.2.3.3"]         = "Modality Performed Procedure Step SOP Class"; 
   ts["1.2.840.10008.3.1.2.3.4"]         = "Modality Performed Procedure Step Retrieve SOP Class"; 
   ts["1.2.840.10008.3.1.2.3.5"]         = "Modality Performed Procedure Step Notification SOP Class";
   ts["1.2.840.10008.3.1.2.5.1"]         = "Detached Results Management SOP Class";
   ts["1.2.840.10008.3.1.2.5.4"]         = "Detached Results Management Meta SOP Class";
   ts["1.2.840.10008.3.1.2.5.5"]         = "Detached Study Management Meta SOP Class";
   ts["1.2.840.10008.3.1.2.6.1"]         = "Detached Interpretation Management SOP Class";                
   ts["1.2.840.10008.5.1.1.1"]           = "Basic Film Session SOP Class"; 
   ts["1.2.840.10008.5.1.1.2"]           = "Basic Film Box SOP Class";     
   ts["1.2.840.10008.5.1.1.4"]           = "Basic Gray Scale Image Box SOP Class"; 
   ts["1.2.840.10008.5.1.1.4.1"]         = "Basic Color Image Box SOP Class"; 
   ts["1.2.840.10008.5.1.1.4.2"]         = "Referenced Image Box SOP Class (Retired)";       
   ts["1.2.840.10008.5.1.1.9"]           = "Basic Gray Scale Print Management Meta SOP Class";       
   ts["1.2.840.10008.5.1.1.9.1"]         = "Referenced Gray Scale Print Management Meta SOP Class(Retired)";       
   ts["1.2.840.10008.5.1.1.14"]          = "Print Job SOP Class";
   ts["1.2.840.10008.5.1.1.15"]          = "Basic Annotation Box SOP Class";
   ts["1.2.840.10008.5.1.1.16"]          = "Printer SOP Class";
   ts["1.2.840.10008.5.1.1.16.376"]      = "Printer Configuration Retrieval SOP Class";
   ts["1.2.840.10008.5.1.1.17"]          = "Printer SOP Instance";
   ts["1.2.840.10008.5.1.1.17.376"]      = "Printer Configuration Retrieval SOP Instance";
   ts["1.2.840.10008.5.1.1.18"]          = "Basic Color Print Management Meta SOP Class";
   ts["1.2.840.10008.5.1.1.18.1"]        = "Referenced Color Print Management Meta SOP Class (Retired)";
   ts["1.2.840.10008.5.1.1.22"]          = "VOI LUT Box SOP Class";
   ts["1.2.840.10008.5.1.1.23"]          = "Presentation LUT Box SOP Class";
   ts["1.2.840.10008.5.1.1.24"]          = "Image Overlay Box SOP Class (Retired)";
   ts["1.2.840.10008.5.1.1.24.1"]        = "Basic Print Image Overlay Box SOP Class";
   ts["1.2.840.10008.5.1.1.25"]          = "Print Queue SOP Instance";
   ts["1.2.840.10008.5.1.1.26"]          = "Print Queue Management SOP Class";         
   ts["1.2.840.10008.5.1.1.27"]          = "Stored Print Storage SOP Class";  
   ts["1.2.840.10008.5.1.1.29"]          = "Hardcopy Grayscale Image Storage SOP Class";  
   ts["1.2.840.10008.5.1.1.30"]          = "Hardcopy Color Image Storage SOP Class";  
   ts["1.2.840.10008.5.1.1.31"]          = "Pull Print Request SOP Class";  
   ts["1.2.840.10008.5.1.1.32"]          = "Pull Stored Print Management Meta SOP Class";
     
   ts["1.2.840.10008.5.1.4.1.1.1"]       = "Computed Radiography Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.1.1"]     = "Digital X-Ray Image Storage - For Presentation";
   ts["1.2.840.10008.5.1.4.1.1.1.1.1"]   = "Digital X-Ray Image Storage - For Processing";   
   ts["1.2.840.10008.5.1.4.1.1.1.2"]     = "Digital Mammography Image Storage - For Presentation";
   ts["1.2.840.10008.5.1.4.1.1.1.2.1"]   = "Digital Mammography Image Storage - For Processing";
   ts["1.2.840.10008.5.1.4.1.1.1.3"]     = "Digital Intra-oral X-ray Image Storage - For Presentation";
   ts["1.2.840.10008.5.1.4.1.1.1.3.1"]   = "Digital Intra-oral X-Ray Image Storage - For Processing";               
   ts["1.2.840.10008.5.1.4.1.1.2"]       = "CT Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.3"]       = "Ultrasound Multi-Frame Image Storage (Retired)";
   ts["1.2.840.10008.5.1.4.1.1.3.1"]     = "Ultrasound Multi-Frame Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.4"]       = "MR Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.4.1"]     = "Enhanced MR Image Storage";  
   ts["1.2.840.10008.5.1.4.1.1.4.2"]     = "MR Spectroscopy Storage";  
   ts["1.2.840.10008.5.1.4.1.1.5"]       = "Nuclear Medicine Image Storage (Retired)";
   ts["1.2.840.10008.5.1.4.1.1.6"]       = "Ultrasound Image Storage (Retired)";
   ts["1.2.840.10008.5.1.4.1.1.6.1"]     = "Ultrasound Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.7"]       = "Secondary Capture Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.7.1"]     = "Multi-frame Single Bit Secondary Capture Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.7.2"]     = "Multi-frame Grayscale Byte Secondary Capture Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.7.3"]     = "Multi-frame Grayscale Word Secondary Capture Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.7.4"]     = "Multi-frame True Color Secondary Capture Image Storage";  
   ts["1.2.840.10008.5.1.4.1.1.8"]       = "Stand-alone Overlay Storage";
   ts["1.2.840.10008.5.1.4.1.1.9"]       = "Stand-alone Curve Storage";
   ts["1.2.840.10008.5.1.4.1.1.9.1.1"]   = "Twelve Lead ECG Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.9.1.2"]   = "General ECG Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.9.1.3"]   = "Ambulatory ECG Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.9.2.1"]   = "Hemodynamic Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.9.3.1"]   = "Cardiac Electrophysiology Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.9.4.1"]   = "Basic Voice Audio Waveform Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.10"]      = "Stand-alone Modality LUT Storage"; 
   ts["1.2.840.10008.5.1.4.1.1.11"]      = "Stand-alone VOI LUT Storage";      
   ts["1.2.840.10008.5.1.4.1.1.11.1"]    = "Grayscale Softcopy Presentation State Storage SOP Class";
   ts["1.2.840.10008.5.1.4.1.1.12.1"]    = "X-Ray Angiographic Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.12.2"]    = "X-Ray Radiofluoroscoping Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.12.3"]    = "X-Ray Angiographic Bi-Plane Image Storage (Retired)";  
   ts["1.2.840.10008.5.1.4.1.1.20"]      = "Nuclear Medicine Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.66"]      = "Raw Data Storage";
   ts["1.2.840.10008.5.1.4.1.1.77.1"]    = "VL Image Storage (Retired)"; 
   ts["1.2.840.10008.5.1.4.1.1.77.2"]    = "VL MultiFrame Image Storage (Retired)";   
   ts["1.2.840.10008.5.1.4.1.1.77.1.1"]  = "VL Endoscoping Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.77.1.2"]  = "VL Microscoping Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.77.1.3"]  = "VL Slide-Coordinates Microscoping Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.77.1.4"]  = "VL Photographic Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.88.1"]    = "SR Text Storage";
   ts["1.2.840.10008.5.1.4.1.1.88.2"]    = "SR Audio Storage";
   ts["1.2.840.10008.5.1.4.1.1.88.3"]    = "SR Detail Storage";
   ts["1.2.840.10008.5.1.4.1.1.88.4"]    = "SR Comprehensive Storage";     
   ts["1.2.840.10008.5.1.4.1.1.88.11"]   = "Basic Text SR";
   ts["1.2.840.10008.5.1.4.1.1.88.22"]   = "Enhanced SR";
   ts["1.2.840.10008.5.1.4.1.1.88.33"]   = "Comprehensive SR";
   ts["1.2.840.10008.5.1.4.1.1.88.50"]   = "Mammography CAD SR";
   ts["1.2.840.10008.5.1.4.1.1.88.59"]   = "Key Object Selection Document";
   ts["1.2.840.10008.5.1.4.1.1.88.65"]   = "Check CAD SR";   
   ts["1.2.840.10008.5.1.4.1.1.128"]     = "Positron Emission Tomography Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.129"]     = "Standalone PET Curve Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.1"]   = "RT Image Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.2"]   = "RT Dose Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.3"]   = "RT Structure Set Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.4"]   = "RT Beams Treatment Record Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.5"]   = "RT Plan Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.6"]   = "RT Brachy Treatment Record Storage";
   ts["1.2.840.10008.5.1.4.1.1.481.7"]   = "RT Treatment Summary Record Storage";
   
   ts["1.2.840.10008.5.1.4.1.2.1.1"]     = "Patient Root Query/Retrieve Information Model - FIND";
   ts["1.2.840.10008.5.1.4.1.2.1.2"]     = "Patient Root Query/Retrieve Information Model - MOVE";
   ts["1.2.840.10008.5.1.4.1.2.1.3"]     = "Patient Root Query/Retrieve Information Model - GET";
   ts["1.2.840.10008.5.1.4.1.2.2.1"]     = "Study Root Query/Retrieve Information Model - FIND";
   ts["1.2.840.10008.5.1.4.1.2.2.2"]     = "Study Root Query/Retrieve Information Model - MOVE";
   ts["1.2.840.10008.5.1.4.1.2.2.3"]     = "Study Root Query/Retrieve Information Model - GET";
   ts["1.2.840.10008.5.1.4.1.2.3.1"]     = "Patient/Study Only Query/Retrieve Information Model - FIND";
   ts["1.2.840.10008.5.1.4.1.2.3.2"]     = "Patient/Study Only Query/Retrieve Information Model - GET";
   ts["1.2.840.10008.5.1.4.1.2.3.3"]     = "Patient/Study Only Query/Retrieve Information Model - MOVE";
   ts["1.2.840.10008.5.1.4.31"]          = "Modality Worklist Information Model - FIND";
   ts["1.2.840.10008.5.1.4.32.1"]        = "General Purpose Worklist Information Model - FIND";
   ts["1.2.840.10008.5.1.4.32.2"]        = "General Purpose Scheduled Procedure Step SOP Class";
   ts["1.2.840.10008.5.1.4.32.3"]        = "General Purpose Performed Procedure Step SOP Class";
   ts["1.2.840.10008.5.1.4.32"]          = "General Purpose Worklist Management Meta SOP Class";
}

gdcmTS::~gdcmTS() {
   ts.clear();
}

int gdcmTS::Count(TSKey key) {
   return ts.count(key);
}

std::string gdcmTS::GetValue(TSKey key) {
   if (ts.count(key) == 0) 
      return (GDCM_UNFOUND);
   return ts[key];
}
